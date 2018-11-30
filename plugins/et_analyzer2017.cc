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
#include "../include/CLParser.h"
#include "../include/EmbedWeight.h"
#include "../include/LumiReweightingStandAlone.h"
#include "../include/SF_factory.h"
#include "../include/ZmmSF.h"
#include "../include/electron_factory.h"
#include "../include/event_info.h"
#include "../include/jet_factory.h"
#include "../include/met_factory.h"
#include "../include/muon_factory.h"
#include "../include/slim_tree.h"
#include "../include/swiss_army_class.h"
#include "../include/tau_factory.h"
#include "TauTriggerSFs2017/TauTriggerSFs2017/interface/TauTriggerSFs2017.h"

typedef std::vector<double> NumV;

int main(int argc, char* argv[]) {
  ////////////////////////////////////////////////
  // Initial setup:                             //
  // Get file names, normalization, paths, etc. //
  ////////////////////////////////////////////////

  CLParser parser(argc, argv);
  std::string sample = parser.Option("-s");
  std::string name = parser.Option("-n");
  std::string path = parser.Option("-p");
  std::string syst = parser.Option("-u");
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
  auto ntuple = reinterpret_cast<TTree *>(fin->Get("etau_tree"));

  // get number of generated events
  auto counts = reinterpret_cast<TH1D*>(fin->Get("nevents"));
  auto gen_number = counts->GetBinContent(2);

  // create output file
  auto suffix = "_output.root";
  auto prefix = "Output/trees/";
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
  slim_tree* st = new slim_tree("etau_tree");

  // get normalization (lumi & xs are in util.h)
  double norm;
  if (isData) {
    norm = 1.0;
  } else if (isEmbed) {
    norm = 1.0;
  } else {
    norm = helper->getLuminosity2017() * helper->getCrossSection(sample) / gen_number;
  }

  ///////////////////////////////////////////////
  // Scale Factors:                            //
  // Read weights, hists, graphs, etc. for SFs //
  ///////////////////////////////////////////////

  reweight::LumiReWeighting* lumi_weights;
  // read inputs for lumi reweighting
  if (!isData && !isEmbed) {
    TNamed* dbsName = reinterpret_cast<TNamed*>(fin->Get("MiniAOD_name"));
    std::string datasetName = dbsName->GetTitle();
    if (datasetName.find("Not Found") != std::string::npos && !isEmbed && !isData) {
      fin->Close();
      fout->Close();
      return 2;
    }
    std::replace(datasetName.begin(), datasetName.end(), '/', '#');
    lumi_weights = new reweight::LumiReWeighting("data/pudistributions_mc_2017.root", "data/pudistributions_data_2017.root", datasetName.c_str(), "pileup");
  }

  // H->tau tau scale factors
  TFile htt_sf_file("data/htt_scalefactors_2017_v2.root");
  RooWorkspace *htt_sf = reinterpret_cast<RooWorkspace*>(htt_sf_file.Get("w"));
  htt_sf_file.Close();

  // embedded sample weights
  TFile embed_file("data/htt_scalefactors_v17_5.root", "READ");
  RooWorkspace *wEmbed = reinterpret_cast<RooWorkspace *>(embed_file.Get("w"));
  embed_file.Close();

  TFile bTag_eff_file("data/tagging_efficiencies_march2018_btageff-all_samp-inc-DeepCSV_medium.root", "READ");
  TH2F* btag_eff_b = reinterpret_cast<TH2F*>(bTag_eff_file.Get("btag_eff_b")->Clone());
  TH2F* btag_eff_c = reinterpret_cast<TH2F*>(bTag_eff_file.Get("btag_eff_c")->Clone());
  TH2F* btag_eff_oth = reinterpret_cast<TH2F*>(bTag_eff_file.Get("btag_eff_oth")->Clone());

  TauTriggerSFs2017* eh = new TauTriggerSFs2017("data/tauTriggerEfficiencies2017_New.root", "data/tauTriggerEfficiencies2017.root", "tight", "MVA");

  //////////////////////////////////////
  // Final setup:                     //
  // Declare histograms and factories //
  //////////////////////////////////////

  // declare histograms (histogram initializer functions in util.h)
  fout->cd("grabbag");
  auto histos = helper->getHistos1D();
  auto histos_2d = helper->getHistos2D();

  // construct factories
  event_info       event(ntuple, syst, "et");
  electron_factory electrons(ntuple);
  tau_factory      taus(ntuple);
  jet_factory      jets(ntuple, syst);
  met_factory      met(ntuple, syst);

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
        evtwt = 6.963;
      } else if (event.getNumGenJets() == 2) {
        evtwt = 16.376;
      } else if (event.getNumGenJets() == 3) {
        evtwt = 2.533;
      } else if (event.getNumGenJets() == 4) {
        evtwt = 2.419;
      } else {
        evtwt = 61.983;
      }
    }

    if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
      if (event.getNumGenJets() == 1) {
        evtwt = 0.502938039;
      } else if (event.getNumGenJets() == 2) {
        evtwt = 1.042256272;
      } else if (event.getNumGenJets() == 3) {
        evtwt = 0.656337234;
      } else if (event.getNumGenJets() == 4) {
        evtwt = 0.458531131;
      } else {
        evtwt = 2.873324952;
      }
    }

    histos->at("cutflow") -> Fill(1., 1.);

    auto electron = electrons.run_factory();
    auto tau = taus.run_factory();
    jets.run_factory();

    //////////////////////////////////////////////////////////
    // Event Selection in skimmer:                          //
    //   - Trigger: Ele25eta2p1Tight -> pass, match, filter //
    //   - Electron: pT > 26, |eta| < 2.1                   //
    //   - Tau: pt > 27 || 29.5, |eta| < 2.3, lepton vetos, //
    //          VLoose Isolation, against leptons           //
    //   - Event: dR(tau,el) > 0.5                          //
    //////////////////////////////////////////////////////////

    bool fireSingle(false), fireCross(false);

    // apply correct lepton pT thresholds
    if (electron.getPt() > 36 && event.getPassEle35()) {
      fireSingle = true;
    } else if (electron.getPt() > 33 && event.getPassEle32()) {
      fireSingle = true;
    } else if (electron.getPt() > 28 && event.getPassEle27()) {
      fireSingle = true;
    } else if (electron.getPt() > 25 && electron.getPt() < 28 && tau.getPt() > 35 && fabs(tau.getEta()) < 2.1 && event.getPassEle24Tau30()) {
      fireCross = true;
    } else {
      continue;
    }

    if (electron.getP4().DeltaR(tau.getP4()) < 0.5) {
      continue;
    }

    if (electron.getPt() < 36) {
      continue;
    }

    if (!event.getPassFlags()) {
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
    TLorentzVector Higgs = electron.getP4() + tau.getP4() + met.getP4();

    // apply all scale factors/corrections/etc.
    if (!isData && !isEmbed) {
      // tau ID efficiency SF
      if (tau.getGenMatch() == 5) {
        evtwt *= 0.89;
      }

      // Z-Vtx HLT Correction
      evtwt *= 0.991;

      // anti-lepton discriminator SFs
      if (tau.getGenMatch() == 1 || tau.getGenMatch() == 3) {  // Yiwen
        if (fabs(tau.getEta()) < 1.460)
          evtwt *= 1.80;
        else if (fabs(tau.getEta()) > 1.558)
          evtwt *= 1.53;
        //  if (name == "ZL" && tau.getL2DecayMode() == 0) evtwt *= 0.98;
        //  else if (sample == "ZL" && tau.getL2DecayMode() == 1) evtwt *= 1.20;
      } else if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
        if (fabs(tau.getEta()) < 0.4)
          evtwt *= 1.06;
        else if (fabs(tau.getEta()) < 0.8)
          evtwt *= 1.02;
        else if (fabs(tau.getEta()) < 1.2)
          evtwt *= 1.10;
        else if (fabs(tau.getEta()) < 1.7)
          evtwt *= 1.03;
        else
          evtwt *= 1.94;
      }

      // pileup reweighting
      evtwt *= lumi_weights->weight(event.getNPV());

      // generator weights
      evtwt *= event.getGenWeight();

      // give inputs to workspace
      htt_sf->var("e_pt")->setVal(electron.getPt());
      htt_sf->var("e_eta")->setVal(electron.getEta());
      htt_sf->var("z_gen_mass")->setVal(event.getGenM()); // TODO: check if these are the right variables
      htt_sf->var("z_gen_pt")->setVal(event.getGenPt());  // TODO: check if these are the right variables

      // electron ID SF
      evtwt *= htt_sf->function("e_id90_kit_ratio")->getVal();

      // electron Iso SF
      evtwt *= htt_sf->function("e_iso_kit_ratio")->getVal();

      // electron track reco SF
      evtwt *= htt_sf->function("e_trk_ratio")->getVal();

      // apply trigger SF's
      auto single_data_eff = htt_sf->function("e_trg27_trg32_trg35_kit_data")->getVal();
      auto single_mc_eff = htt_sf->function("e_trg27_trg32_trg35_kit_mc")->getVal();
      auto el_cross_data_eff = htt_sf->function("e_trg_EleTau_Ele24Leg_desy_data")->getVal();
      auto el_cross_mc_eff = htt_sf->function("e_trg_EleTau_Ele24Leg_desy_mc")->getVal();
      auto single_eff = single_data_eff / single_mc_eff;
      auto el_cross_eff = el_cross_data_eff / el_cross_mc_eff;
      double tau_cross_eff(1.);
      if (fireCross) {
        tau_cross_eff = eh->getETauScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), TauTriggerSFs2017::kCentral);
      }

      evtwt *= (single_eff*fireSingle + el_cross_eff*tau_cross_eff*fireCross);

      // Z-pT Reweighting
      double zpt_sf(1.);
      if (name == "EWKZLL" || name == "EWKZNuNu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
        evtwt *= htt_sf->function("zptmass_weight_nom")->getVal();
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

      // use promote-demote method to correct nbtag with no systematics
      jets.promoteDemote(btag_eff_oth, btag_eff_oth, btag_eff_oth);

    } else if (!isData && isEmbed) {

      // tau ID eff SF
      if (tau.getGenMatch() == 5) {
        evtwt *= 0.97;
      }

      // set workspace variables
      wEmbed->var("e_pt")->setVal(electron.getPt());
      wEmbed->var("e_eta")->setVal(electron.getEta());

      // electron ID SF
      evtwt *= wEmbed->function("e_id90_embed_kit_ratio")->getVal();

      // electron iso SF
      evtwt *= wEmbed->function("e_iso_binned_embed_kit_ratio")->getVal();

      // unfolding dimuon selection TODO: store gen info in skimmer
      

      // apply trigger SF's
      auto single_eff = wEmbed->function("e_trg27_trg32_trg35_embed_kit_ratio")->getVal();
      auto el_cross_eff(1.);  // TODO: currently being measured
      auto tau_cross_eff(1.); // TODO: currently being measured

      //evtwt *= (single_eff * fireSingle + el_cross_eff * tau_cross_eff * fireCross);
      evtwt *= fireSingle * single_eff;
      
      auto genweight(event.getGenWeight());
      if (genweight > 1 || genweight < 0) {
          genweight = 0;
      }
      evtwt *= genweight;
    }

    fout->cd();

    // calculate mt
    double met_x = met.getMet() * cos(met.getMetPhi());
    double met_y = met.getMet() * sin(met.getMetPhi());
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    double mt = sqrt(pow(electron.getPt() + met_pt, 2) - pow(electron.getPx() + met_x, 2) - pow(electron.getPy() + met_y, 2));
    int evt_charge = tau.getCharge() + electron.getCharge();

    // create regions
    bool signalRegion   = (tau.getTightIsoMVA()  && electron.getIso() < 0.15);
    bool looseIsoRegion = (tau.getMediumIsoMVA() && electron.getIso() < 0.30);
    bool antiIsoRegion  = (tau.getTightIsoMVA()  && electron.getIso() > 0.15 && electron.getIso() < 0.30);
    bool antiTauIsoRegion = (tau.getTightIsoMVA() == 0 && electron.getIso() < 0.15);

    // create categories
    bool zeroJet = (jets.getNjets() == 0);
    bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && (jets.getDijetMass() < 300 || Higgs.Pt() < 50)));
    bool vbfCat  = (jets.getNjets() > 1 && jets.getDijetMass() > 300 && Higgs.Pt() > 50);
    bool VHCat   = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

    // now do mt selection
    if (tau.getPt() < 30 || mt > 50) {
      continue;
    }

    if (signalRegion && evt_charge == 0) {
      histos->at("el_pt") -> Fill(electron.getPt(), evtwt);
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

    // fill the tree
    st->fillTree(tree_cat, &electron, &tau, &jets, &met, &event, mt, evtwt);
  } // close event loop

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
