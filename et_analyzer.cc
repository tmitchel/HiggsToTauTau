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
#include "include/util.h"
#include "include/event_info.h"
#include "include/tau_factory.h"
#include "include/electron_factory.h"
#include "include/muon_factory.h"
#include "include/jet_factory.h"
#include "include/met_factory.h"
#include "include/SF_factory.h"
#include "include/btagSF.h"
#include "include/LumiReweightingStandAlone.h"
#include "include/CLParser.h"
#include "include/EmbedWeight.h"
#include "include/slim_tree.h"

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
  bool isEmbed = sample.find("embed") != std::string::npos;
  // bool isData = parser.Flag("-d");

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
  auto prefix = "output/";
  std::string filename;
  if (name == sample) {
    filename = prefix + name + systname + suffix;
  } else {
    filename = prefix + sample + std::string("_") + name + systname + suffix;
  }
  auto fout = new TFile(filename.c_str(), "RECREATE");
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
  slim_tree* st = new slim_tree("test_tree");

  // get normalization (lumi & xs are in util.h)
  double norm;
  if (isData) {
    norm = 1.0;
  } else if (isEmbed) {
    if (sample.find("embed-H") != std::string::npos) {
      norm = 1 / .59;
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
  auto histos_2d = helper->getHistos2D();

  // construct factories
  event_info       event(ntuple, syst, "et");
  electron_factory electrons(ntuple);
  tau_factory      taus(ntuple);
  jet_factory      jets(ntuple, syst);
  met_factory      met(ntuple, syst);
  double n70_count;

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

    //////////////////////////////////////////////////////////
    // Event Selection in skimmer:                          //
    //   - Trigger: Ele25eta2p1Tight -> pass, match, filter //
    //   - Electron: pT > 26, |eta| < 2.1                   //
    //   - Tau: pt > 27 || 29.5, |eta| < 2.3, lepton vetos, //
    //          VLoose Isolation, against leptons           //
    //   - Event: dR(tau,el) > 0.5                          //
    //////////////////////////////////////////////////////////

    auto electron = electrons.run_factory();
    auto tau = taus.run_factory();
    jets.run_factory();

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

    histos->at("cutflow") -> Fill(2., 1.);

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
      float weight_btag( bTagEventWeight(nbtagged, bjets.at(0).getPt() ,bjets.at(0).getFlavor(), bjets.at(1).getPt(), bjets.at(1).getFlavor() ,1,0,0) );
      if (nbtagged>2) weight_btag=0;
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
      histos->at("Stitching")->Fill(Stitching_Weight, 1.);
      // get correction factor
      std::vector<double> corrFactor = EmdWeight_Electron(wEmbed, electron.getPt(), electron.getEta(), electron.getIso());
      double totEmbedWeight(corrFactor[2] * corrFactor[5] * corrFactor[6]); // id SF, iso SF, trg eff. SF
      histos->at("totEmbed")->Fill(totEmbedWeight, 1.);
      histos->at("corr2")->Fill(corrFactor[2], 1.);
      histos->at("corr5")->Fill(corrFactor[5], 1.);
      histos->at("corr6")->Fill(corrFactor[6], 1.);
      // data to mc trigger ratio
      double trg_ratio(m_sel_trg_ratio(wEmbed, electron.getPt(), electron.getEta(), tau.getPt(), tau.getEta()));
      histos->at("trg")->Fill(trg_ratio, 1.);

      auto genweight(event.getGenWeight());
      if (genweight > 1 || genweight < 0) {
        genweight = 0;
      }
      evtwt *= (Stitching_Weight * totEmbedWeight * trg_ratio * genweight);
      histos->at("gen")->Fill(genweight, 1.);
      // temporary SF for failed embed jobs
      if (fname.find("embed-H") != std::string::npos) {
        evtwt *= (1. / .59);
      } else if (fname.find("embed") != std::string::npos) {
        evtwt *= (1. / .99);
      }
      // scale-up tau pT
      tau.scalePt(1.02);
      histos->at("evtwt")->Fill(evtwt, 1.);
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
    bool signalRegion = (tau.getTightIsoMVA()  && electron.getIso() < 0.10);
    bool qcdRegion    = (tau.getMediumIsoMVA() && electron.getIso() < 0.30);
    bool wRegion      = (tau.getMediumIsoMVA() && electron.getIso() < 0.30);
    bool qcdCR        = (tau.getTightIsoMVA()  && electron.getIso() > 0.10 && electron.getIso() < 0.30);

    // create categories
    bool zeroJet = (jets.getNjets() == 0);
    bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && 
                   (jets.getDijetMass() <= 300 || Higgs.Pt() <= 50 || tau.getPt() == 30)));
    bool vbfCat  = (jets.getNjets() > 1 && Higgs.Pt() > 50 && jets.getDijetMass() > 300);
    bool VHCat   = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

    histos->at("pre_mt") -> Fill(mt, 1.);
    histos->at("pre_tau_pt") -> Fill(tau.getPt(), 1.);
    histos->at("pre_tau_iso") -> Fill(tau.getTightIsoMVA(), 1.);
    histos->at("pre_el_iso") -> Fill(electron.getIso(), 1.);

    if (mt < 50 && tau.getPt() > 30) {
      st->fillTree(std::string("et_inclusive"), &electron, &tau, &jets, &met, &event, evtwt);

      // event categorizaation
      if (zeroJet) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h0_OS") -> Fill(tau.getL2DecayMode(), (electron.getP4()+tau.getP4()).M(), evtwt);
          } else {
            histos_2d->at("h0_SS") -> Fill(tau.getL2DecayMode(), (electron.getP4() + tau.getP4()).M(), evtwt);
          }
        } // close if signal block

        if (qcdRegion) {
          histos_2d->at("h0_QCD") -> Fill(tau.getL2DecayMode(), (electron.getP4() + tau.getP4()).M(), evtwt);
        } // close if qcd block

        if (wRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h0_WOS") -> Fill(tau.getL2DecayMode(), (electron.getP4() + tau.getP4()).M(), evtwt);
          } else {
            histos_2d->at("h0_WSS") -> Fill(tau.getL2DecayMode(), (electron.getP4() + tau.getP4()).M(), evtwt);
          }
        } // close if W block

      } else if (boosted) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h1_OS") -> Fill(Higgs.Pt(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h1_SS") -> Fill(Higgs.Pt(), event.getMSV(), evtwt);
          }
        } // close if signal block

        if (qcdRegion) {
          histos_2d->at("h1_QCD") -> Fill(Higgs.Pt(), event.getMSV(), evtwt);
        } // close if qcd block

        if (wRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h1_WOS") -> Fill(Higgs.Pt(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h1_WSS") -> Fill(Higgs.Pt(), event.getMSV(), evtwt);
          }
        } // close if W block

      } else if (vbfCat) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h2_OS") -> Fill(jets.getDijetMass(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h2_SS") -> Fill(jets.getDijetMass(), event.getMSV(), evtwt);
          }
        } // close if signal block

        if (qcdRegion) {
          histos_2d->at("h2_QCD") -> Fill(jets.getDijetMass(), event.getMSV(), evtwt);
        } // close if qcd block

        if (wRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h2_WOS") -> Fill(jets.getDijetMass(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h2_WSS") -> Fill(jets.getDijetMass(), event.getMSV(), evtwt);
          }
        } // close if W block

      } else if (VHCat) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h3_OS") -> Fill(tau.getPt(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h3_SS") -> Fill(tau.getPt(), event.getMSV(), evtwt);
          }
        } // close if signal block

        if (qcdRegion) {
          histos_2d->at("h3_QCD") -> Fill(tau.getPt(), event.getMSV(), evtwt);
        } // close if qcd block

        if (wRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h3_WOS") -> Fill(tau.getPt(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h3_WSS") -> Fill(tau.getPt(), event.getMSV(), evtwt);
          }
        } // close if W block

      } // close VH

      histos->at("cutflow")->Fill(3., 1.);
      // // inclusive selection
      // if (signalRegion) {
      //   histos->at("cutflow")->Fill(8., 1.);

      //   if (evt_charge == 0) {
      //     // fill histograms
      //     histos->at("cutflow")->Fill(9., 1.);
      //     if (helper.deltaR(electron.getEta(), electron.getPhi(), tau.getEta(), tau.getPhi()) > 0.5) {
      //       histos->at("cutflow")->Fill(10., 1.);
      //       histos->at("hel_pt")->Fill(electron.getPt(), evtwt);
      //       histos->at("hel_eta")->Fill(electron.getEta(), evtwt);
      //       histos->at("hel_phi")->Fill(electron.getPhi(), evtwt);
      //       histos->at("htau_pt")->Fill(tau.getPt(), evtwt);
      //       histos->at("htau_eta")->Fill(tau.getEta(), evtwt);
      //       histos->at("htau_phi")->Fill(tau.getPhi(), evtwt);
      //       histos->at("hmet")->Fill(met.getMet(), evtwt);
      //       histos->at("hmet_x")->Fill(met_x, evtwt);
      //       histos->at("hmet_y")->Fill(met_y, evtwt);
      //       histos->at("hmet_pt")->Fill(met_pt, evtwt);
      //       histos->at("hmt")->Fill(mt, evtwt);
      //       histos->at("hnjets")->Fill(jets.getNjets(), evtwt);
      //       histos->at("hmjj")->Fill(jets.getDijetMass(), evtwt);
      //       histos->at("hNGenJets")->Fill(event.getNumGenJets(), evtwt);
      //       histos->at("pt_sv")->Fill(event.getPtSV() ,evtwt);
      //       histos->at("m_sv")->Fill(event.getMSV(), evtwt);
      //       histos->at("Dbkg_VBF")->Fill(event.getDbkg_VBF(), evtwt);
      //       histos->at("Phi")->Fill(event.getPhi(), evtwt);
      //       histos->at("Phi1")->Fill(event.getPhi1(), evtwt);
      //       histos->at("Q2V1")->Fill(event.getQ2V1(), evtwt);
      //       histos->at("Q2V2")->Fill(event.getQ2V2(), evtwt);
      //       histos->at("costheta1")->Fill(event.getCosTheta1(), evtwt);
      //       histos->at("costheta2")->Fill(event.getCosTheta2(), evtwt);
      //       histos->at("costhetastar")->Fill(event.getCosThetaStar(), evtwt);
      //     }
      //   } else {
      //     histos->at("htau_pt_SS")->Fill(tau.getPt(), evtwt);
      //     histos->at("hel_pt_SS")->Fill(electron.getPt(), evtwt);
      //     histos->at("htau_phi_SS")->Fill(tau.getPhi(), evtwt);
      //     histos->at("hel_phi_SS")->Fill(electron.getPhi(), evtwt);
      //     histos->at("hmet_SS")->Fill(met.getMet(), evtwt);
      //     histos->at("hmt_SS")->Fill(mt, evtwt);
      //     histos->at("hmjj_SS")->Fill(jets.getDijetMass(), evtwt);
      //   }
      // } // close signal
      // if (qcdRegion) {
      //   histos->at("htau_pt_QCD")->Fill(tau.getPt(), evtwt);
      //   histos->at("hel_pt_QCD")->Fill(electron.getPt(), evtwt);
      //   histos->at("htau_phi_QCD")->Fill(tau.getPhi(), evtwt);
      //   histos->at("hel_phi_QCD")->Fill(electron.getPhi(), evtwt);
      //   histos->at("hmet_QCD")->Fill(met.getMet(), evtwt);
      //   histos->at("hmt_QCD")->Fill(mt, evtwt);
      //   histos->at("hmjj_QCD")->Fill(jets.getDijetMass(), evtwt);
      // } // close qcd
      // if (wRegion) {
      //   if (evt_charge == 0) {
      //     histos->at("htau_pt_WOS")->Fill(tau.getPt(), evtwt);
      //     histos->at("hel_pt_WOS")->Fill(electron.getPt(), evtwt);
      //     histos->at("htau_phi_WOS")->Fill(tau.getPhi(), evtwt);
      //     histos->at("hel_phi_WOS")->Fill(electron.getPhi(), evtwt);
      //     histos->at("hmet_WOS")->Fill(met.getMet(), evtwt);
      //     histos->at("hmt_WOS")->Fill(mt, evtwt);
      //     histos->at("hmjj_WOS")->Fill(jets.getDijetMass(), evtwt);
      //   } else {
      //     histos->at("htau_pt_WSS")->Fill(tau.getPt(), evtwt);
      //     histos->at("hel_pt_WSS")->Fill(electron.getPt(), evtwt);
      //     histos->at("htau_phi_WSS")->Fill(tau.getPhi(), evtwt);
      //     histos->at("hel_phi_WSS")->Fill(electron.getPhi(), evtwt);
      //     histos->at("hmet_WSS")->Fill(met.getMet(), evtwt);
      //     histos->at("hmt_WSS")->Fill(mt, evtwt);
      //     histos->at("hmjj_WSS")->Fill(jets.getDijetMass(), evtwt);
      //   } // close Wjets
      // }   // close general

    } // close mt, tau selection

  } // close event loop
  histos->at("n70")->Fill(1, n70_count);
  histos->at("n70")->Write();

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
