// Copyright 2018 Tyler Mitchell

// system includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>

// ROOT includes
#include "TH1D.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooFunctor.h"
#include "RooMsgService.h"

// user includes
#include "../include/ACWeighter.h"
#include "../include/CLParser.h"
#include "../include/EmbedWeight.h"
#include "../include/LumiReweightingStandAlone.h"
#include "../include/SF_factory.h"
#include "../include/ZmmSF.h"
#include "../include/muon_factory.h"
#include "../include/event_info.h"
#include "../include/jet_factory.h"
#include "../include/met_factory.h"
#include "../include/slim_tree.h"
#include "../include/swiss_army_class.h"
#include "../include/tau_factory.h"

typedef std::vector<double> NumV;

int main(int argc, char* argv[]) {
  ////////////////////////////////////////////////
  // Initial setup:                             //
  // Get file names, normalization, paths, etc. //
  ////////////////////////////////////////////////

  CLParser parser(argc, argv);
  bool doAC = parser.Flag("-a");
  std::string name = parser.Option("-n");
  std::string path = parser.Option("-p");
  std::string syst = parser.Option("-u");
  std::string sample = parser.Option("-s");
  std::string output_dir = parser.Option("-d");
  std::string fname = path + sample + ".root";
  bool isData = sample.find("data") != std::string::npos;
  bool isEmbed = sample.find("embed") != std::string::npos || name.find("embed") != std::string::npos;

  std::string systname = "";
  if (!syst.empty()) {
    systname = "_" + syst;
  }

  // open input file
  std::cout << "Opening file... " << sample << std::endl;
  auto fin = TFile::Open(fname.c_str());
  std::cout << "Loading Ntuple..." << std::endl;
  auto ntuple = reinterpret_cast<TTree*>(fin->Get("mutau_tree"));

  // get number of generated events
  auto counts = reinterpret_cast<TH1D*>(fin->Get("nevents"));
  auto gen_number = counts->GetBinContent(2);

  // reweighter for anomolous coupling samples
  ACWeighter ac_weights = ACWeighter(sample, "2016");
  ac_weights.fillWeightMap();

  // create output file
  auto suffix = "_output.root";
  auto prefix = "Output/trees/" + output_dir + "/";
  std::string filename;
  if (name == sample) {
    filename = prefix + name + systname + suffix;
  } else {
    filename = prefix + sample + std::string("_") + name + systname + suffix;
  }
  auto fout = new TFile(filename.c_str(), "RECREATE");
  counts->Write();
  fout->mkdir("grabbag");
  fout->cd("grabbag");

  // initialize Helper class
  Helper *helper;
  if (isEmbed) {
    helper = new Helper(fout, "ZTT", syst);
  } else {
    helper = new Helper(fout, name, syst);
  }

  // cd to root of output file and create tree
  fout->cd();
  slim_tree* st = new slim_tree("mutau_tree"+systname, doAC);

  if (sample.find("vbf125") != std::string::npos) {
    sample = "vbf125";
  } else if (sample.find("ggh125") != std::string::npos) {
    sample = "ggh125";
  } else if (sample.find("wminus125") != std::string::npos) {
    sample = "wminus125";
  } else if (sample.find("wplus125") != std::string::npos) {
    sample = "wplus125";
  } else if (sample.find("zh125") != std::string::npos) {
    sample = "zh125";
  }

  // get normalization (lumi & xs are in util.h)
  double norm(1.);
  if (!isData && !isEmbed) {
    norm = helper->getLuminosity2016() * helper->getCrossSection(sample) / gen_number;
  }

  ///////////////////////////////////////////////
  // Scale Factors:                            //
  // Read weights, hists, graphs, etc. for SFs //
  ///////////////////////////////////////////////

  // read inputs for lumi reweighting
  auto lumi_weights = new reweight::LumiReWeighting("data/MC_Moriond17_PU25ns_V1.root", "data/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  // Z-pT reweighting
  TFile *zpt_file = new TFile("data/zpt_weights_2016_BtoH.root");
  auto zpt_hist = reinterpret_cast<TH2F *>(zpt_file->Get("zptmass_histo"));

  // H->tau tau scale factors
  TFile htt_sf_file("data/htt_scalefactors_sm_moriond_v1.root");
  RooWorkspace *htt_sf = reinterpret_cast<RooWorkspace *>(htt_sf_file.Get("w"));
  htt_sf_file.Close();

  // embedded sample weights
  TFile embed_file("data/htt_scalefactors_v16_9_embedded.root", "READ");
  RooWorkspace *wEmbed = reinterpret_cast<RooWorkspace *>(embed_file.Get("w"));
  embed_file.Close();

  // trigger and ID scale factors
  auto myScaleFactor_trgMu19 = new SF_factory("$CMSSW_BASE/src/LeptonEfficiencies/Muon/Run2016BtoH/Muon_Mu19leg_2016BtoH_eff.root");
  auto myScaleFactor_trgMu22 = new SF_factory("$CMSSW_BASE/src/LeptonEfficiencies/Muon/Run2016BtoH/Muon_Mu22OR_eta2p1_eff.root");
  auto myScaleFactor_id = new SF_factory("$CMSSW_BASE/src/LeptonEfficiencies/Muon/Run2016BtoH/Muon_IdIso_IsoLt0p15_2016BtoH_eff.root");

  //////////////////////////////////////
  // Final setup:                     //
  // Declare histograms and factories //
  //////////////////////////////////////

  // declare histograms (histogram initializer functions in util.h)
  fout->cd("grabbag");
  auto histos = helper->getHistos1D();
  auto histos_2d = helper->getHistos2D();

  // construct factories
  event_info       event(ntuple, lepton::MUON, 2016, syst);
  muon_factory     muons(ntuple, 2016);
  tau_factory      taus(ntuple, 2016);
  jet_factory      jets(ntuple, 2016, syst);
  met_factory      met(ntuple, 2016, syst);

  if (sample.find("ggHtoTauTau125") != std::string::npos) {
    event.setRivets(ntuple);
  }

  // begin the event loop
  Int_t nevts = ntuple->GetEntries();
  for (Int_t i = 0; i < nevts; i++) {
    ntuple->GetEntry(i);
    if (i % 100000 == 0)
      std::cout << "Processing event: " << i << " out of " << nevts << std::endl;

    // find the event weight (not lumi*xs if looking at W or Drell-Yan)
    Float_t evtwt(norm), corrections(1.), sf_trig(1.), sf_id(1.), sf_iso(1.), sf_reco(1.);
    if (name == "W") {
      if (event.getNumGenJets() == 1) {
        evtwt = 6.82;
      } else if (event.getNumGenJets() == 2) {
        evtwt = 2.099;
      } else if (event.getNumGenJets() == 3) {
        evtwt = 0.689;
      } else if (event.getNumGenJets() == 4) {
        evtwt = 0.690;
      } else {
        evtwt = 25.44;
      }
    }

    if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
      if (event.getNumGenJets() == 1) {
        evtwt = 0.457;
      } else if (event.getNumGenJets() == 2) {
        evtwt = 0.467;
      } else if (event.getNumGenJets() == 3) {
        evtwt = 0.480;
      } else if (event.getNumGenJets() == 4) {
        evtwt = 0.393;
      } else {
        evtwt = 1.418;
      }
    }

    histos->at("cutflow") -> Fill(1., 1.);

    auto muon = muons.run_factory();
    auto tau = taus.run_factory();
    jets.run_factory();

    // remove 2-prong taus
    if (!tau.getDecayModeFinding() || tau.getL2DecayMode() == 5 || tau.getL2DecayMode() == 6) {
      continue;
    }

    // apply special ID for data
    if (isData && !muon.getMediumID()) {
      continue;
    }

    if (fabs(muon.getEta()) > 2.1) {
      continue;
    }

    if (tau.getPt() < 30) {
      continue;
    }

    if (isEmbed) {
      event.setEmbed();
    }

    // apply correct lepton pT thresholds
    bool fireSingle(false), fireCross(false);
    if (muon.getPt() > 23 && (event.getPassIsoMu22() || event.getPassIsoTkMu22() || event.getPassIsoMu22eta2p1() || event.getPassIsoTkMu22eta2p1() )) {
      fireSingle = true;
    } else if (muon.getPt() > 20 && muon.getPt() < 23 && event.getPassMu19Tau20()) {
      fireCross = true;
    } else if (!isEmbed) {
      // if it's embedded samples, don't apply trigger
      continue;
    }

    // Separate Drell-Yan
    if (name == "ZL" && tau.getGenMatch() > 4) {
      continue;
    } else if ((name == "ZTT" || name == "TTT" || name == "VVT") && tau.getGenMatch() != 5) {
      continue;
    } else if ((name == "ZLL" || name == "TTJ" || name == "VVJ") && tau.getGenMatch() == 5) {
      continue;
    } else if (name == "ZJ" && tau.getGenMatch() != 6) {
      continue;
    }

    histos->at("cutflow") -> Fill(7., 1.);

    // build Higgs
    TLorentzVector Higgs = muon.getP4() + tau.getP4() + met.getP4();

    // calculate mt
    double met_x = met.getMet() * cos(met.getMetPhi());
    double met_y = met.getMet() * sin(met.getMetPhi());
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    double mt = sqrt(pow(muon.getPt() + met_pt, 2) - pow(muon.getPx() + met_x, 2) - pow(muon.getPy() + met_y, 2));
    int evt_charge = tau.getCharge() + muon.getCharge();

    // now do mt selection
    if (mt > 50) {
      continue;
    }

    // only opposite-sign
    if (evt_charge != 0) {
      continue;
    }

    // apply all scale factors/corrections/etc.
    if (!isData && !isEmbed) {
      // Trigger SF
      if (muon.getPt() < 23) {
        htt_sf->var("t_pt")->setVal(tau.getPt());
        htt_sf->var("t_eta")->setVal(tau.getEta());
        htt_sf->var("t_dm")->setVal(tau.getL2DecayMode());
        evtwt *= htt_sf->function("t_genuine_TightIso_mt_ratio")->getVal();
        evtwt *= myScaleFactor_trgMu19->getSF(muon.getPt(), muon.getEta());
      } else {
        evtwt *= myScaleFactor_trgMu22->getSF(muon.getPt(), muon.getEta());
      }

      // muon ID SF
      evtwt *= myScaleFactor_id->getSF(muon.getPt(), muon.getEta());

      // Pileup Reweighting
      evtwt *= lumi_weights->weight(event.getNPU());

      // Apply generator weights
      evtwt *= event.getGenWeight();

      // tau ID efficiency SF
      if (tau.getGenMatch() == 5) {
        evtwt *= 0.95;
      }

      // // anti-lepton discriminator SFs
      if (tau.getGenMatch() == 1 || tau.getGenMatch() == 3) {  // Yiwen
        if (fabs(tau.getEta()) < 1.460)
          evtwt *= 1.213;
        else if (fabs(tau.getEta()) > 1.558)
          evtwt *= 1.375;
      } else if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
        if (fabs(tau.getEta()) < 0.4)
          evtwt *= 1.263;
        else if (fabs(tau.getEta()) < 0.8)
          evtwt *= 1.364;
        else if (fabs(tau.getEta()) < 1.2)
          evtwt *= 0.854;
        else if (fabs(tau.getEta()) < 1.7)
          evtwt *= 1.712;
        else
          evtwt *= 2.324;

        if (name == "ZL" && tau.getL2DecayMode() == 0)
          evtwt *= 0.74;
        else if (sample == "ZL" && tau.getL2DecayMode() == 1)
          evtwt *= 1.0;
      }

      // Z-pT and Zmm Reweighting
      if (name == "EWKZLL" || name == "EWKZNuNu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
        // Z-pT Reweighting
        auto nom_zpt_weight = zpt_hist->GetBinContent(zpt_hist->GetXaxis()->FindBin(event.getGenM()), zpt_hist->GetYaxis()->FindBin(event.getGenPt()));
        if (syst == "dyShape_Up") {
          nom_zpt_weight = 1.1 * nom_zpt_weight - 0.1;
        } else if (syst == "dyShape_Down") {
          nom_zpt_weight = 0.9 * nom_zpt_weight + 0.1;
        }
        evtwt *= nom_zpt_weight;

        // Zmumu SF
        if (syst == "zmumuShape_Up") {
          evtwt *= GetZmmSF(jets.getNjets(), jets.getDijetMass(), Higgs.Pt(), tau.getPt(), 1);
        } else if (syst == "zmumuShape_Down") {
          evtwt *= GetZmmSF(jets.getNjets(), jets.getDijetMass(), Higgs.Pt(), tau.getPt(), -1);
        } else {
          evtwt *= GetZmmSF(jets.getNjets(), jets.getDijetMass(), Higgs.Pt(), tau.getPt(), 0);
        }
      }

      // top-pT Reweighting
      if (name == "TTT" || name == "TT" || name == "TTJ") {
        float pt_top1 = std::min(static_cast<float>(400.), jets.getTopPt1());
        float pt_top2 = std::min(static_cast<float>(400.), jets.getTopPt2());
        if (syst == "ttbarShape_Up") {
          evtwt *= (2 * sqrt(exp(0.0615 - 0.0005 * pt_top1) * exp(0.0615 - 0.0005 * pt_top2)) - 1);  // 2*√[e^(..)*e^(..)] - 1
        } else if (syst == "ttbarShape_Up") {
          // no weight for shift down
        } else {
          evtwt *= sqrt(exp(0.0615 - 0.0005 * pt_top1) * exp(0.0615 - 0.0005 * pt_top2));  // √[e^(..)*e^(..)]
        }
      }

      // b-tagging SF - no systematic and want 0 jets
      float weight_btag(jets.bTagEventWeight());

      // jet to tau fake rate
      if (tau.getGenMatch() == 6 && name == "TTJ" || name == "ZJ" || name == "W" || name == "VVJ") {
        auto temp_tau_pt = std::min(200., static_cast<double>(tau.getPt()));
        if (syst == "jetToTauFake_Up") {
          evtwt *= (1 - (0.2 * temp_tau_pt / 100));
        } else if (syst == "jetToTauFake_Down") {
          evtwt *= (1 + (0.2 * temp_tau_pt / 100));
        }
      }

    } else if (!isData && isEmbed) {
      float Stitching_Weight = 1.0;
      if (event.getRun() >= 272007 && event.getRun() < 275657) {
        Stitching_Weight = (1.0 / 0.899 * 1.02);
      } else if (event.getRun() < 276315) {
        Stitching_Weight = (1.0 / 0.881 * 1.02);
      } else if (event.getRun() < 276831) {
        Stitching_Weight = (1.0 / 0.877 * 1.02);
      } else if (event.getRun() < 277772) {
        Stitching_Weight = (1.0 / 0.939 * 1.02);
      } else if (event.getRun() < 278820) {
        Stitching_Weight = (1.0 / 0.936 * 1.02);
      } else if (event.getRun() < 280919) {
        Stitching_Weight = (1.0 / 0.908 * 1.02);
      } else if (event.getRun() < 284045) {
        Stitching_Weight = (1.0 / 0.962 * 1.02);
      }

      // get correction factor
      std::vector<double> corrFactor = EmdWeight_Muon(wEmbed, muon.getPt(), muon.getEta(), muon.getIso());
      double totEmbedWeight(corrFactor[2] * corrFactor[5] * corrFactor[6]);  // id SF, iso SF, trg eff. SF

      // data to mc trigger ratio
      double trg_ratio(m_sel_trg_ratio(wEmbed, muon.getPt(), muon.getEta(), tau.getPt(), tau.getEta()));

      auto genweight(event.getGenWeight());
      if (genweight > 1 || genweight < 0) {
        genweight = 0;
      }
      evtwt *= (Stitching_Weight * totEmbedWeight * trg_ratio * genweight);

      // scale-up tau pT
      if (tau.getGenMatch() == 5) {
        tau.scalePt(1.02);
      }
    }

    fout->cd();

    // b-jet veto
    if (jets.getNbtag() > 0) {
      continue;
    }

    // create regions
    bool signalRegion   = (tau.getTightIsoMVA()  && muon.getIso() < 0.15);
    bool looseIsoRegion = (tau.getMediumIsoMVA() && muon.getIso() < 0.30);
    bool antiIsoRegion  = (tau.getTightIsoMVA()  && muon.getIso() > 0.15 && muon.getIso() < 0.30);
    bool antiTauIsoRegion = (tau.getTightIsoMVA() == 0 && muon.getIso() < 0.15);

    // create categories
    bool zeroJet = (jets.getNjets() == 0);
    bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300));
    bool vbfCat  = (jets.getNjets() > 1 && jets.getDijetMass() > 300);
    bool VHCat   = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

    // only keep the regions we need
    if (!signalRegion && !antiTauIsoRegion) {
      continue;
    }

    if (signalRegion && evt_charge == 0) {
      histos->at("el_pt") -> Fill(muon.getPt(), evtwt);
      histos->at("tau_pt") -> Fill(tau.getPt(), evtwt);
    }

    std::vector<std::string> tree_cat;

    // regions
    if (signalRegion) {
      tree_cat.push_back("signal");
    } else if (antiIsoRegion) {
      tree_cat.push_back("antiLepIso");
    } else if (antiTauIsoRegion) {
      tree_cat.push_back("antiTauIso");
    }

    if (looseIsoRegion) {
      tree_cat.push_back("looseIso");
    }

    // categorization
    if (zeroJet) {
      tree_cat.push_back("0jet");
    } else if (boosted) {
      tree_cat.push_back("boosted");
    } else if (vbfCat) {
      tree_cat.push_back("vbf");
    } else if (VHCat) {
      tree_cat.push_back("VH");
    }

    // event charge
    if (evt_charge == 0) {
      tree_cat.push_back("OS");
    } else {
      tree_cat.push_back("SS");
    }

    std::shared_ptr<std::vector<double>> weights(nullptr);
    Long64_t currentEventID = event.getLumi();
    currentEventID = currentEventID * 1000000 + event.getEvt();
    if (doAC) {
      weights = std::make_shared<std::vector<double>>(ac_weights.getWeights(currentEventID));
    }

    // fill the tree
    st->fillTree(tree_cat, &muon, &tau, &jets, &met, &event, mt, evtwt, weights);
  }  // close event loop

  fin->Close();
  fout->cd();
  fout->Write(0, TObject::kOverwrite);
  // fout->Write();
  fout->Close();
  return 0;
}
