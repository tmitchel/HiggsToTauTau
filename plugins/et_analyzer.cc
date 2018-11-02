// system includes
#include <iostream>
#include <cmath>
#include <algorithm>
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
#include "../include/util.h"
#include "../include/event_info.h"
#include "../include/tau_factory.h"
#include "../include/electron_factory.h"
#include "../include/muon_factory.h"
#include "../include/jet_factory.h"
#include "../include/met_factory.h"
#include "../include/SF_factory.h"
#include "../include/btagSF.h"
#include "../include/LumiReweightingStandAlone.h"
#include "../include/CLParser.h"
#include "../include/EmbedWeight.h"
#include "../include/slim_tree.h"

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
  std::string postfix = parser.Option("-P");
  std::string fname = path + sample + postfix;
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
  auto ntuple = (TTree*)fin->Get("etau_tree");

  // get number of generated events
  auto counts = (TH1D*)fin->Get("nevents");
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
    if (sample.find("embed-H") != std::string::npos) {
      norm = 1 / .99;
    } else {
      norm = 1 / .99;
    }
  } else {
    norm = helper->getLuminosity() * helper->getCrossSection(sample) / gen_number;
  }

  ///////////////////////////////////////////////
  // Scale Factors:                            //
  // Read weights, hists, graphs, etc. for SFs //
  ///////////////////////////////////////////////

  // read inputs for lumi reweighting
  auto lumi_weights = new reweight::LumiReWeighting("inputs/MC_Moriond17_PU25ns_V1.root", "inputs/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  // tracking corrections
  TFile *f_Trk = new TFile("inputs/etracking.root");
  TH2F *h_Trk = (TH2F*)f_Trk->Get("EGamma_SF2D");

  // Z-pT reweighting
  TFile *zpt_file = new TFile("inputs/zpt_weights_2016_BtoH.root");
  auto zpt_hist = (TH2F*)zpt_file->Get("zptmass_histo");

  //H->tau tau scale factors
  TFile htt_sf_file("inputs/htt_scalefactors_v16_3.root");
  RooWorkspace *htt_sf = (RooWorkspace*)htt_sf_file.Get("w");
  htt_sf_file.Close();

  // embedded sample weights
  TFile embed_file("inputs/htt_scalefactors_v16_9_embedded.root", "READ");
  RooWorkspace *wEmbed = (RooWorkspace *)embed_file.Get("w");
  embed_file.Close();

  // not sure what these are exactly, yetı
  TFile *fEleRec = new TFile("inputs/EGammaRec.root");
  TH2F *histEleRec = (TH2F*)fEleRec->Get("EGamma_SF2D");

  TFile *fEleWP80 = new TFile("inputs/EGammaWP80.root");
  TH2F *histEleWP80 = (TH2F*)fEleWP80->Get("EGamma_SF2D");

  TFile *fEleWP90 = new TFile("inputs/EGammaWP90.root");
  TH2F *histEleWP90 = (TH2F*)fEleWP90->Get("EGamma_SF2D");

  // trigger and ID scale factors
  auto myScaleFactor_trgEle25 = new SF_factory("LeptonEfficiencies/Electron/Run2016BtoH/Electron_Ele25WPTight_eff.root");
  auto myScaleFactor_id = new SF_factory("LeptonEfficiencies/Electron/Run2016BtoH/Electron_IdIso_IsoLt0p1_eff.root");
  auto myScaleFactor_trgEle25Anti = new SF_factory("LeptonEfficiencies/Electron/Run2016BtoH/Electron_Ele25WPTight_antiisolated_Iso0p1to0p3_eff_rb.root");
  auto myScaleFactor_idAnti = new SF_factory("LeptonEfficiencies/Electron/Run2016BtoH/Electron_IdIso_antiisolated_Iso0p1to0p3_eff.root");

  TFile * f_NNLOPS = new TFile("inputs/NNLOPS_reweight.root");
  TGraph * g_NNLOPS_0jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_0jet");
  TGraph * g_NNLOPS_1jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_1jet");
  TGraph * g_NNLOPS_2jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_2jet");
  TGraph * g_NNLOPS_3jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_3jet");

  //////////////////////////////////////
  // Final setup:                     //
  // Declare histograms and factories //
  //////////////////////////////////////

  // declare histograms (histogram initializer functions in util.h)
  fout->cd("grabbag");
  auto histos = helper->getHistos1D();

  // construct factories
  event_info       event(ntuple, syst, "et");
  electron_factory electrons(ntuple);
  tau_factory      taus(ntuple);
  jet_factory      jets(ntuple, syst);
  met_factory      met(ntuple, syst);
  double n70_count;

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
    Float_t evtwt(norm), corrections(1.), sf_trig(1.), sf_trig_anti(1.), sf_id(1.), sf_id_anti(1.);
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

    histos->at("cutflow")->Fill(1., 1.);

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

    // Separate Drell-Yan
    if (name == "ZL" && tau.getGenMatch() > 4) {
      continue;
    } else if ((name == "ZTT" || name == "TTT") && tau.getGenMatch() != 5) {
      continue;
    } else if ((name == "ZLL" || name == "TTJ") && tau.getGenMatch() == 5) {
      continue;
    } else if (name == "ZJ" && tau.getGenMatch() != 6) {
      continue;
    }

    histos->at("cutflow") -> Fill(7., 1.);

    // build Higgs
    TLorentzVector Higgs = electron.getP4() + tau.getP4() + met.getP4();

    // apply all scale factors/corrections/etc.
    if (!isData && !isEmbed) {

      // apply trigger and id SF's
      sf_trig      = myScaleFactor_trgEle25->getSF(electron.getPt(), electron.getEta());
      sf_trig_anti = myScaleFactor_trgEle25Anti->getSF(electron.getPt(), electron.getEta());
      sf_id        = myScaleFactor_id->getSF(electron.getPt(), electron.getEta());
      sf_id_anti   = myScaleFactor_idAnti->getSF(electron.getPt(), electron.getEta());
      
      evtwt *= (sf_trig * sf_id * lumi_weights->weight(event.getNPU()) * event.getGenWeight());

      // tau ID efficiency SF
      if (tau.getGenMatch() == 5) {
        evtwt *= 0.95;
      }

      htt_sf->var("e_pt")->setVal(electron.getPt());
      htt_sf->var("e_eta")->setVal(electron.getEta());
      evtwt *= htt_sf->function("e_trk_ratio")->getVal();

      // // anti-lepton discriminator SFs
      if (tau.getGenMatch() == 1 or tau.getGenMatch() == 3){//Yiwen
         if (fabs(tau.getEta())<1.460) evtwt *= 1.402;
         else if (fabs(tau.getEta())>1.558) evtwt *= 1.900;
         if (name == "ZL" && tau.getL2DecayMode() == 0) evtwt *= 0.98;
         else if (sample == "ZL" && tau.getL2DecayMode() == 1) evtwt *= 1.20;
       }
        else if (tau.getGenMatch() == 2 or tau.getGenMatch() == 4){
            if (fabs(tau.getEta())<0.4) evtwt *= 1.012;
            else if (fabs(tau.getEta())<0.8) evtwt *= 1.007;
            else if (fabs(tau.getEta())<1.2) evtwt *= 0.870;
            else if (fabs(tau.getEta())<1.7) evtwt *= 1.154;
            else evtwt *= 2.281;
        }

      // Z-pT and Zmm Reweighting
      if (name=="EWKZLL" || name=="EWKZNuNu" || name=="ZTT" || name=="ZLL" || name=="ZL" || name=="ZJ") {
        evtwt *= zpt_hist->GetBinContent(zpt_hist->GetXaxis()->FindBin(event.getGenM()),zpt_hist->GetYaxis()->FindBin(event.getGenPt()));
        evtwt *= GetZmmSF(jets.getNjets(), jets.getDijetMass(), Higgs.Pt(), tau.getPt(), 0);
      } 

      // // top-pT Reweighting (only for some systematic)
      // if (name == "TTT" || name == "TT" || name == "TTJ") {
      //   float pt_top1 = std::min(float(400.), jets.getTopPt1());
      //   float pt_top2 = std::min(float(400.), jets.getTopPt2());
      //   evtwt *= sqrt(exp(0.0615-0.0005*pt_top1)*exp(0.0615-0.0005*pt_top2));
      // }

      // b-tagging SF (only used in scaling W, I believe)
      int nbtagged = std::min(2, jets.getNbtag());
      auto bjets = jets.getBtagJets();
      float weight_btag( bTagEventWeight(nbtagged, bjets.at(0).getPt(), bjets.at(0).getFlavor(), bjets.at(1).getPt(), bjets.at(1).getFlavor() ,1,0,0) );
      if (nbtagged>2) weight_btag=0;

      // NNLOPS ggH reweighting
      if (sample.find("ggHtoTauTau125") != std::string::npos) {
        if (event.getNjetsRivet() == 0) evtwt *= g_NNLOPS_0jet->Eval(min(event.getHiggsPtRivet(), float(125.0)));
        if (event.getNjetsRivet() == 1) evtwt *= g_NNLOPS_1jet->Eval(min(event.getHiggsPtRivet(), float(625.0)));
        if (event.getNjetsRivet() == 2) evtwt *= g_NNLOPS_2jet->Eval(min(event.getHiggsPtRivet(), float(800.0)));
        if (event.getNjetsRivet() >= 3) evtwt *= g_NNLOPS_3jet->Eval(min(event.getHiggsPtRivet(), float(925.0)));
      }

      //NumV WG1unc = qcd_ggF_uncert_2017(Rivet_nJets30, Rivet_higgsPt, Rivet_stage1_cat_pTjet30GeV);
    } else if (!isData && isEmbed) {
      double Stitching_Weight(1.);
      // get the stitching weight
      if (event.getRun() >= 272007 && event.getRun() < 275657) {
        Stitching_Weight = (1.0 / 0.902);
      } else if (event.getRun() < 276315) {
        Stitching_Weight = (1.0 / 0.910);
      } else if (event.getRun() < 276831) {
        Stitching_Weight = (1.0 / 0.945);
      } else if (event.getRun() < 277772) {
        Stitching_Weight = (1.0 / 0.945);
      } else if (event.getRun() < 278820) {
        Stitching_Weight = (1.0 / 0.915);
      } else if (event.getRun() < 280919) {
        Stitching_Weight = (1.0 / 0.903);
      } else if (event.getRun() < 284045) {
        Stitching_Weight = (1.0 / 0.933);
      }

      // get correction factor
      std::vector<double> corrFactor = EmdWeight_Electron(wEmbed, electron.getPt(), electron.getEta(), electron.getIso());
      double totEmbedWeight(corrFactor[2] * corrFactor[5] * corrFactor[6]); // id SF, iso SF, trg eff. SF

      // data to mc trigger ratio
      double trg_ratio(m_sel_trg_ratio(wEmbed, electron.getPt(), electron.getEta(), tau.getPt(), tau.getEta()));

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

    // calculate mt
    double met_x = met.getMet() * cos(met.getMetPhi());
    double met_y = met.getMet() * sin(met.getMetPhi());
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    double mt = sqrt(pow(electron.getPt() + met_pt, 2) - pow(electron.getPx() + met_x, 2) - pow(electron.getPy() + met_y, 2));
    int evt_charge = tau.getCharge() + electron.getCharge();

    if (mt > 80 && mt < 200 && evt_charge == 0 && tau.getTightIsoMVA() && electron.getIso() < 0.10) {
      histos->at("n70") -> Fill(0.1, evtwt);
      if (jets.getNjets() == 0 && event.getMSV() < 400)
        histos->at("n70") -> Fill(1.1, evtwt);
      else if (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() > 300 && Higgs.Pt() < 100))
        histos->at("n70") -> Fill(2.1, evtwt);
      else if (jets.getNjets() > 1 && jets.getDijetMass() > 300 && Higgs.Pt() > 100)
        histos->at("n70") -> Fill(3.1, evtwt);
    }

    // create regions
    bool signalRegion   = (tau.getTightIsoMVA()  && electron.getIso() < 0.10);
    bool looseIsoRegion = (tau.getMediumIsoMVA() && electron.getIso() < 0.30);
    bool antiIsoRegion  = (tau.getTightIsoMVA()  && electron.getIso() > 0.10 && electron.getIso() < 0.30);

    // create categories
    bool zeroJet = (jets.getNjets() == 0);
    bool boosted = (jets.getNjets() == 1);
    bool vbfCat  = (jets.getNjets() > 1 && jets.getDijetMass() > 300);
    bool VHCat   = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

    // now do mt selection
    if (tau.getPt() < 30) {
      continue;
    }

    std::vector<std::string> tree_cat;
    if (signalRegion) {
      tree_cat.push_back("inclusive");
      if (zeroJet) {
        tree_cat.push_back("0jet");
      } 
      if (boosted) {
        tree_cat.push_back("boosted");
      } 
      if (vbfCat) {
        tree_cat.push_back("vbf");
      }
    } 
    if (antiIsoRegion) {
      tree_cat.push_back("antiiso");
      if (zeroJet) {
        tree_cat.push_back("antiiso_0jet");
      }
      if (boosted) {
        tree_cat.push_back("antiiso_boosted");
      }
      if (vbfCat) {
        tree_cat.push_back("antiiso_vbf");
      }
    } 
    if (looseIsoRegion) {
      tree_cat.push_back("looseIso");
      if (zeroJet) {
        tree_cat.push_back("looseIso_0jet");
      }
      if (boosted) {
        tree_cat.push_back("looseIso_boosted");
      }
      if (vbfCat) {
        tree_cat.push_back("looseIso_vbf");
      }
    }
    st->fillTree(tree_cat, &electron, &tau, &jets, &met, &event, mt, evtwt);

  } // close event loop
  histos->at("n70")->Fill(1, n70_count);
  histos->at("n70")->Write();

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
