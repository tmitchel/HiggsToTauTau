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
#include "include/ditau_factory.h"
#include "include/electron_factory.h"
#include "include/jet_factory.h"
#include "include/met_factory.h"
#include "include/SF_factory.h"
#include "include/tauSF.h"
#include "include/btagSF.h"
#include "include/LumiReweightingStandAlone.h"
#include "include/CLParser.h"

int main(int argc, char* argv[]) {

  ////////////////////////////////////////////////
  // Initial setup:                             //
  // Get file names, normalization, paths, etc. //
  ////////////////////////////////////////////////

  CLParser parser(argc, argv);
  bool local = parser.Flag("-l");
  std::string sample = parser.Option("-s");
  std::string name = parser.Option("-n");
  std::string path = parser.Option("-p");
  std::string syst = parser.Option("-u");
  std::string postfix = parser.Option("-P");
  std::string fname = path + sample + postfix;
  bool isData = sample.find("Data") != std::string::npos;
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

  // initialize Helper class
  Helper helper(fout, name, syst);

  // get normalization (lumi & xs are in util.h)
  double norm;
  if (isData)
    norm = 1.0;
  else
    norm = helper.getLuminosity() * helper.getCrossSection(sample) / gen_number;

  ///////////////////////////////////////////////
  // Scale Factors:                            //
  // Read weights, hists, graphs, etc. for SFs //
  ///////////////////////////////////////////////

  // read inputs for lumi reweighting
  auto lumi_weights = new reweight::LumiReWeighting("inputs/MC_Moriond17_PU25ns_V1.root", "inputs/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  // tracking corrections
  TFile *f_Trk = new TFile("inputs/etracking.root");

  // Z-pT reweighting
  TFile *zpt_file = new TFile("inputs/zpt_weights_2016_BtoH.root");
  auto zpt_hist = (TH2F*)zpt_file->Get("zptmass_histo");

  //H->tau tau scale factors
  // TFile htt_sf_file("inputs/htt_scalefactors_v16_3.root");
  // RooWorkspace *htt_sf = (RooWorkspace*)htt_sf_file.Get("w");
  // htt_sf_file.Close();

  // trigger and ID scale factors
  auto tauSFs = tauSF();

  TFile * f_NNLOPS = new TFile("inputs/NNLOPS_reweight.root");
  TGraph * g_NNLOPS_0jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_0jet");
  TGraph * g_NNLOPS_1jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_1jet");
  TGraph * g_NNLOPS_2jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_2jet");
  TGraph * g_NNLOPS_3jet = (TGraph*) f_NNLOPS-> Get("gr_NNLOPSratio_pt_powheg_3jet");

  //////////////////////////////////////
  // Final setup:                     //
  // Declare histograms and factories //
  //////////////////////////////////////
  std::map<std::string, std::string> hist_suffix = {
    {"met_UESDown","_CMS_scale_met_unclustered_13TeVDown"},
    {"met_UESUp","_CMS_scale_met_unclustered_13TeVUp"},
    {"met_JESDown","_CMS_scale_met_clustered_13TeVDown"},
    {"met_JESUp","_CMS_scale_met_clustered_13TeVUp"},
    {"metphi_UESDown","_CMS_scale_metphi_unclustered_13TeVDown"},
    {"metphi_UESUp","_CMS_scale_metphi_unclustered_13TeVUp"},
    {"metphi_JESDown","_CMS_scale_metphi_clustered_13TeVDown"},
    {"metphi_JESUp","_CMS_scale_metphi_clustered_13TeVUp"}
  };
  // declare histograms (histogram initializer functions in util.h)
  fout->cd("grabbag");
  auto histos = helper.getHistos1D();
  auto histos_2d = helper.getHistos2D();

  // construct factories
  event_info       event(ntuple, syst, "tt");
  ditau_factory    ditaus(ntuple);
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
    double evtwt(norm), corrections(1.), sf_trig1(1.), sf_trig2(1.);
    double sf_trig_RR(1.), sf_trig_RF(1.), sf_trig_FR(1.), sf_trig_FF(1.);

    if (name == "W") {
      if (event.getNumGenJets() == 1)
        evtwt = 6.8176;
      else if (event.getNumGenJets() == 2)
        evtwt = 2.1038;
      else if (event.getNumGenJets() == 3)
        evtwt = 0.6889;
      else if (event.getNumGenJets() == 4)
        evtwt = 0.6900;
      else
        evtwt = 25.446;
    }

    if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
      if (event.getNumGenJets() == 1)
        evtwt = 0.457675455;
      else if (event.getNumGenJets() == 2)
        evtwt = 0.467159142;
      else if (event.getNumGenJets() == 3)
        evtwt = 0.480349711;
      else if (event.getNumGenJets() == 4)
        evtwt = 0.3938184351;
      else
        evtwt = 1.41957039;
    }

    histos->at("cutflow")->Fill(1., 1.);

    //////////////////////////////////////////////////////////
    // Event Selection:                                     //
    //   - Trigger: DoubleTauCmbIso35 && DoubleTau35        //
    //       - pass, match, filter                          //
    //   - Taus: Loose Iso, against mu & el, el & mu vetos  //
    //   - Ditau: dR(t1, t2) < 0.5                          //
    //////////////////////////////////////////////////////////
    auto taus = ditaus.run_factory();
    auto tau1( taus.first ); 
    auto tau2( taus.second );

    // trigger selection
    if (event.getPassDoubleTauCmbIso35() || event.getPassDoubleTau35()) histos->at("cutflow") -> Fill(2, 1.);
    else continue;

    // tau against electron/muon selection
    if (tau1.getAgainstTightElectron() || tau2.getAgainstTightElectron() || tau1.getAgainstLooseMuon() || tau2.getAgainstLooseMuon()) 
      histos->at("cutflow")->Fill(3, 1.);
    else continue;

    // |eta| < 2.1
    if (fabs(tau1.getEta()) < 2.1 && fabs(tau2.getEta()) < 2.1) histos->at("cutflow")->Fill(4, 1.);
    else continue;

    // dR(t1, t2) selection
    if (tau1.getP4().DeltaR(tau2.getP4())) histos->at("cutflow")->Fill(5, 1.);
    else continue;

    // finally, apply vetos
    if (!event.getMuonVeto() && ! event.getElectronVeto()) histos->at("cutflow")->Fill(7, 1.);
    else continue;
    // end event selection

    // get jet data for the event
    jets.run_factory();

    // build Higgs
    TLorentzVector Higgs = tau1.getP4() + tau2.getP4() + met.getP4();

    // Separate Drell-Yan
    if ((name == "ZTT" || name == "TTT" || name == "VVT") && !(tau1.getGenMatch() == 5 && tau2.getGenMatch() == 5)) {
      continue;
    } else if ((name == "ZJ" || name == "TTJ" || name == "VVJ") && !(tau1.getGenMatch() == 6 || tau2.getGenMatch() == 6)) {
      continue;
    } else if (name == "ZL" && (tau1.getGenMatch() < 6 && tau2.getGenMatch() < 6) 
               && !(tau1.getGenMatch() == 5 && tau2.getGenMatch() == 5)) {
      continue;
    }

    histos->at("cutflow") -> Fill(6., 1.);

    // apply all scale factors/corrections/etc.
    if (!isData) {

      // apply trigger and id SF's
      sf_trig1 = tauSFs.compute_SF(tau1.getPt(), std::to_string(int(tau1.getL2DecayMode())));
      sf_trig2 = tauSFs.compute_SF(tau1.getPt(), std::to_string(int(tau2.getL2DecayMode())));
      evtwt *= (sf_trig1 * sf_trig2 * lumi_weights->weight(event.getNPU()) * event.getGenWeight());

      // for trigger SF systematics
      if (tau1.getGenMatch() == 5) {
        sf_trig_RR *= sf_trig1;
        sf_trig_RF *= sf_trig1;
      } else if (tau1.getGenMatch() == 6) {
        sf_trig_FF *= sf_trig1;
        sf_trig_FR *= sf_trig1;
      }
      if (tau2.getGenMatch() == 5) {
        sf_trig_RR *= sf_trig2;
        sf_trig_RF *= sf_trig2;
      } else if (tau2.getGenMatch() == 6) {
        sf_trig_FF *= sf_trig2;
        sf_trig_FR *= sf_trig2;
      }

      // tau ID efficiency SF
      if (tau1.getGenMatch() == 5) {
        evtwt *= 0.95;
      }
      if (tau2.getGenMatch() == 5) {
        evtwt *= 0.95;
      }

      // htt_sf->var("e_pt")->setVal(electron.getPt());
      // htt_sf->var("e_eta")->setVal(electron.getEta());
      // evtwt *= htt_sf->function("e_trk_ratio")->getVal();

      // // anti-lepton discriminator SFs
      evtwt *= tauSFs.tauID_SF(tau1.getGenMatch(), tau1.getEta());
      evtwt *= tauSFs.tauID_SF(tau2.getGenMatch(), tau2.getEta());

      // Z-pT and Zmm Reweighting
      if (name=="EWKZLL" || name=="EWKZNuNu" || name=="ZTT" || name=="ZLL" || name=="ZL" || name=="ZJ") {
        evtwt *= zpt_hist->GetBinContent(zpt_hist->GetXaxis()->FindBin(event.getGenM()),zpt_hist->GetYaxis()->FindBin(event.getGenPt()));
      } 

      // top-pT Reweighting (only for some systematic)
      if (name == "TTT" || name == "TT" || name == "TTJ") {
        float pt_top1 = std::min(float(400.), jets.getTopPt1());
        float pt_top2 = std::min(float(400.), jets.getTopPt2());
        evtwt *= sqrt(exp(0.0615-0.0005*pt_top1)*exp(0.0615-0.0005*pt_top2));
      }

      // b-tagging SF (only used in scaling W, I believe)
      int nbtagged = std::min(static_cast<Float_t>(2), jets.getNbtag());
      auto bjets = jets.getBtagJets();
      float weight_btag( bTagEventWeight(nbtagged, bjets.at(0).getPt() ,bjets.at(0).getFlavor(), bjets.at(1).getPt(), bjets.at(1).getFlavor() ,1,0,0) );
      if (nbtagged>2) weight_btag=0;
    }
    fout->cd();

    histos->at("cutflow") -> Fill(11, 1.);

    int evt_charge = tau1.getCharge() + tau2.getCharge();
    auto jet1 = jets.getJets().at(0);
    auto jet2 = jets.getJets().at(1);

    // create regions
    bool signalRegion  = (tau1.getTightIsoMVA()  &&  tau2.getTightIsoMVA());
    bool antiIsoRegion = (tau1.getMediumIsoMVA() && !tau2.getTightIsoMVA() && tau2.getLooseIsoMVA()) 
                      || (tau2.getMediumIsoMVA() && !tau1.getTightIsoMVA() && tau1.getLooseIsoMVA());
    bool qcdRegion = (tau1.getDecayModeFinding() == 1 && tau2.getDecayModeFinding() == 1 && tau1.getVLooseIsoMVA() && tau2.getVLooseIsoMVA());

    // create categories
    bool zeroJet = (jets.getNjets() == 0);
    bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && 
                   !(Higgs.Pt() < 100 && fabs(jet1.getEta() - jet2.getEta()) > 2.5)));
    bool vbfCat = (jets.getNjets() > 1 && Higgs.Pt() > 100 && fabs(jet1.getEta() - jet2.getEta()) > 2.5);
    bool VHCat   = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

    double normMELA(event.getMELA_vbf()); 
    normMELA /= (event.getMELA_vbf() + (45*event.getMELA_bkg()));

    if (name == "EWKZLL" || name == "EWKZNuNu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
      if (boosted) {
        evtwt *= tauSFs.boosted_ZmmSF(event.getPtSV(), syst);
      } else if (vbfCat) {
        evtwt *= tauSFs.VBF_ZmmSF(jets.getDijetMass(), syst);
      }
    }

    if (tau1.getPt() > 50 && tau2.getPt() > 40) {

      // event categorizaation
      if (zeroJet) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h0_OS")->Fill(event.getMSV(), 1., evtwt);
          } else {
            histos_2d->at("h0_SS")->Fill(event.getMSV(), 1., evtwt);
          }
        } // close if signal block

      } else if (boosted) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h1_OS")->Fill(event.getPtSV(), event.getMSV(), evtwt);
          } else {
            histos_2d->at("h1_SS")->Fill(event.getPtSV(), event.getMSV(), evtwt);
          }
        } // close if signal block

      } else if (vbfCat) {

        if (signalRegion) {
          if (evt_charge == 0) {
            histos_2d->at("h2_OS")->Fill(normMELA, 1., evtwt);
          } else {
            histos_2d->at("h2_SS")->Fill(normMELA, 1., evtwt);
          }
        } // close if signal block

      } // close VBF

    } // close tau selection
    histos->at("cutflow")->Fill(7., 1.);
  } // close event loop

  histos->at("n70")->Fill(1, n70_count);
  histos->at("n70")->Write();

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
