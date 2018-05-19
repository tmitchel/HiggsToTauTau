// system includes
#include <iostream>
#include <cmath>

// ROOT includes
#include "TH1D.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"

// user includes
#include "include/trigger.h"
#include "include/event_info.h"
#include "include/electron_factory.h"
#include "include/tau_factory.h"
#include "include/jet_factory.h"
#include "include/met_factory.h"
#include "include/gen_factory.h"
#include "include/util.h"
#include "include/LumiReweightingStandAlone.h"
#include "include/SF_factory.h"

int main(int argc, char* argv[]) {

  // parse user options
  std::string sample = "ZZ4L";
  if (argc > 1)
    sample = argv[1];

  std::string name = sample;
  if (argc > 2)
    name = argv[2];

  bool local = false;
  if (argc > 3)
    local = argv[3];

  // path to find input root files
  std::string fname;
  bool isData = sample.find("Data") != std::string::npos;
  if (local) {
    if (isData)
      fname = "root_files/smhet_22feb_SV/"+sample+".root";
    else
      fname = "root_files/smhet_20march/"+sample+".root";
  }
  else {
    if (isData)
      fname = "root://cmsxrootd.fnal.gov//store/user/tmitchel/smhet_22feb_SV/"+sample+".root";
    else
      fname = "root://cmsxrootd.fnal.gov//store/user/tmitchel/smhet_20march/"+sample+".root";
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
  if (name == sample)
    filename = prefix + name + suffix;
  else
    filename = prefix + sample + std::string("_") + name + suffix;
  auto fout = new TFile(filename.c_str(), "RECREATE");

  // get normalization
  double norm;
  if (isData)
    norm = 1.0;
  else
    norm = luminosity * cross_sections[sample] / gen_number;

  // read inputs for lumi reweighting
  auto lumi_weights = new reweight::LumiReWeighting("inputs/MC_Moriond17_PU25ns_V1.root", "inputs/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  // tracking corrections
  TFile *f_Trk=new TFile("inputs/ratios.root");
  TGraph *h_Trk=(TGraph*) f_Trk->Get("ratio_eta");

  // trigger and ID scale factors
  auto trig_SF = new SF_factory("inputs/Electron_Ele25eta2p1WPTight_eff.root");
  auto id_SF = new SF_factory("inputs/Electron_IdIso0p10_eff.root");
  fout->cd();

  // declare histograms (maybe find a nicer place to put these)
  TH1D *n70=new TH1D ("n70", "n70", 6,0,6);
  TH1D* cutflow = new TH1D("cutflow", "Cutflow", 11, -0.5, 10.5);
  TH1D* weightflow = new TH1D("weightflow", "Cutflow", 11, -0.5, 10.5);

  TH1D* hel_pt = new TH1D("el_pt", "Electron p_{T};p_{T} [GeV];;", 20, 0., 100);
  TH1D* hel_eta = new TH1D("el_eta", "Electron #eta;#eta [GeV];;", 80, -4., 4.);
  TH1D* hel_phi = new TH1D("el_phi", "Electron #phi;#phi [GeV];;", 15, -3.14, 3.14);

  TH1D* htau_pt = new TH1D("tau_pt", "Tau p_{T};p_{T} [GeV];;", 40, 0., 200);
  TH1D* htau_eta = new TH1D("tau_eta", "Tau #eta;#eta [GeV];;", 80, -4., 4.);
  TH1D* htau_phi = new TH1D("tau_phi", "Tau #phi;#phi [GeV];;", 15, -3.14, 3.14);

  TH1D* htau_pt_QCD = new TH1D("tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.);
  TH1D* htau_pt_SS = new TH1D("tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.);
  TH1D* htau_pt_OS = new TH1D("tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.);
  TH1D* htau_pt_WOS = new TH1D("tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.);
  TH1D* htau_pt_WSS = new TH1D("tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.);

  TH1D* hel_pt_QCD = new TH1D("el_pt_QCD", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.);
  TH1D* hel_pt_SS = new TH1D("el_pt_SS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.);
  TH1D* hel_pt_OS = new TH1D("el_pt_OS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.);
  TH1D* hel_pt_WOS = new TH1D("el_pt_WOS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.);
  TH1D* hel_pt_WSS = new TH1D("el_pt_WSS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.);

  TH1D* hmsv_QCD = new TH1D("msv_QCD", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  TH1D* hmsv_SS = new TH1D("msv_SS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  TH1D* hmsv_OS = new TH1D("msv_OS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  TH1D* hmsv_WOS = new TH1D("msv_WOS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  TH1D* hmsv_WSS = new TH1D("msv_WSS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);

  TH1D* hmet = new TH1D("met", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);
  TH1D* hmetphi = new TH1D("metphi", "Missing E_{T} #phi;Missing E_{T} [GeV];;", 60, -3.14, 3.14);
  TH1D* hmet_x = new TH1D("met_x", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);
  TH1D* hmet_y = new TH1D("met_y", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);
  TH1D* hmet_pt = new TH1D("met_pt", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);

  TH1D* hmt = new TH1D("mt", "MT", 50, 0, 100);
  TH1D* hnjets = new TH1D("njets", "N(jets)", 10, -0.5, 9.5);
  TH1D* hmjj = new TH1D("mjj", "Dijet Mass; Mass [GeV];;", 100, 0, 200);
  TH1D* hmsv = new TH1D("msv", "SV Fit Mass; Mass [GeV];;", 100, 0, 300);

  TH1D* hNGenJets = new TH1D("NGenJets", "Number of Gen Jets", 12, -0.5, 11.5);

  // construct factories
  trigger          trigs(ntuple);
  event_info       events(ntuple);
  electron_factory electrons(ntuple);
  tau_factory      taus(ntuple);
  jet_factory      jets(ntuple);
  met_factory      met(ntuple);
  gen_factory      gen(ntuple);

  // begin the event loop
  Int_t nevts = ntuple->GetEntries();
  for (Int_t i = 0; i < nevts; i++) {
    ntuple->GetEntry(i);
    if (i % 100000 == 0)
      std::cout << "Processing event: " << i << " out of " << nevts << std::endl;

    double evtwt(norm), corrections(1.), sf_trg(1.), sf_id(1.);
    if (name == "W") {
      if (gen.getNumGenJets() == 1)
        evtwt = 6.8176;
      else if (gen.getNumGenJets() == 2)
        evtwt = 2.1038;
      else if (gen.getNumGenJets() == 3)
        evtwt = 0.6889;
      else if (gen.getNumGenJets() == 4)
        evtwt = 0.6900;
      else
        evtwt = 25.446;
    }

    if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
      if (gen.getNumGenJets() == 1)
        evtwt = 0.45729;
      else if (gen.getNumGenJets() == 2)
        evtwt = 0.4668;
      else if (gen.getNumGenJets() == 3)
        evtwt = 0.47995;
      else if (gen.getNumGenJets() == 4)
        evtwt = 0.39349;
      else
        evtwt = 1.4184;
    }

    cutflow->Fill(0., evtwt);

    // begin event selection
    // electron pT > 27 GeV
    auto electron = electrons.run_factory();
    if (electron.getPt() > 26 && fabs(electron.getEta()) < 2.1)  cutflow->Fill(1., evtwt);
    else continue;

    // electron passes Ele25eta2p1Tight
    if (!isData || trigs.getPassEle25eta2p1Tight()) cutflow->Fill(2., evtwt);
    else continue;

    // tau passes decay mode finding
    auto tau = taus.run_factory();
    if (tau.getDecayModeFinding()) cutflow->Fill(3., evtwt);
    else continue;

    // tau |eta| < 2.3
    if (fabs(tau.getEta()) < 2.3) cutflow->Fill(4., evtwt);
    else continue;
    // end event selection

    // Separate Drell-Yan
    if (name == "ZL" && tau.getGenMatch() > 4)
      continue;
    else if ((name == "ZTT" || name == "TTT") && tau.getGenMatch() != 5)
      continue;
    else if ((name == "ZLL" || name == "TTJ") && tau.getGenMatch() == 5)
      continue;
    else if (name == "ZJ" && tau.getGenMatch() != 6)
      continue;

    cutflow->Fill(5., evtwt);

    // apply trigger and ID scale factors
    if (!isData) {
      sf_trg = trig_SF->getDataEfficiency(electron.getPt(), electron.getEta());
      sf_id = id_SF->getSF(electron.getPt(), electron.getEta());

      evtwt *= (sf_trg * sf_id * h_Trk->Eval(electron.getEta()) * lumi_weights->weight(events.getNPU()));
      weightflow->Fill(1., evtwt);
      // corrections based on decay mode
      if (tau.getGenMatch() == 5)
        evtwt *= 0.95;

      weightflow->Fill(2., evtwt);

      if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
        if (fabs(tau.getEta())<0.4) evtwt *= 1.263;
        else if (fabs(tau.getEta())<0.8) evtwt *= 1.364;
        else if (fabs(tau.getEta())<1.2) evtwt *= 0.854;
        else if (fabs(tau.getEta())<1.7) evtwt *= 1.712;
        else if (fabs(tau.getEta())<2.3) evtwt *= 2.324;
        if (name == "ZL" && events.getDecayMode() == 0) evtwt *= 0.74; //ZL corrections Laura
        else if (name == "ZL" && events.getDecayMode() == 1) evtwt *= 1.0;
      }
      weightflow->Fill(3., evtwt);

      if (tau.getGenMatch() == 1 or tau.getGenMatch() == 3){//Yiwen
         if (fabs(tau.getEta())<1.460) evtwt *= 1.213;
         else if (fabs(tau.getEta())>1.558) evtwt *= 1.375;
       }
       weightflow->Fill(4., evtwt);
    }
    fout->cd();

    // Tau energy scale corrections
    // ...

    // calculate mt
    double met_x = met.getMet() * cos(met.getMetPhi());
    double met_y = met.getMet() * sin(met.getMetPhi());
    // more tau energy scale corrections to met_x and met_y
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    double mt = calculate_mt(&electron, met_x, met_y, met_pt);
    int evt_charge = tau.getCharge() + electron.getCharge();

    if (mt < 50 && tau.getPt() > 30) {

      cutflow->Fill(6., evtwt);
      // inclusive selection
      if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
        cutflow->Fill(7., evtwt);

        if (evt_charge == 0) {
          // fill histograms
          cutflow->Fill(8., evtwt);
          if (deltaR(electron.getEta(), electron.getPhi(), tau.getEta(), tau.getPhi()) > 0.5) {
            cutflow->Fill(9., evtwt);
            hel_pt->Fill(electron.getPt(), evtwt);
            hel_eta->Fill(electron.getEta(), evtwt);
            hel_phi->Fill(electron.getPhi(), evtwt);
            htau_pt->Fill(tau.getPt(), evtwt);
            htau_eta->Fill(tau.getEta(), evtwt);
            htau_phi->Fill(tau.getPhi(), evtwt);
            hmet->Fill(met.getMet(), evtwt);
            hmet_x->Fill(met_x, evtwt);
            hmet_y->Fill(met_y, evtwt);
            hmet_pt->Fill(met_pt, evtwt);
            hmt->Fill(mt, evtwt);
            hnjets->Fill(jets.getNjets(), evtwt);
            hmjj->Fill(jets.getDijetMass(), evtwt);
            hmsv->Fill(events.getMSV(), evtwt);
            htau_pt_OS->Fill(tau.getPt(), evtwt);
            hel_pt_OS->Fill(electron.getPt(), evtwt);
            hmsv_OS->Fill(events.getMSV(), evtwt);
            hNGenJets->Fill(gen.getNumGenJets(), evtwt);
          }
        }
        else {
          htau_pt_SS->Fill(tau.getPt(), evtwt);
          hel_pt_SS->Fill(electron.getPt(), evtwt);
          hmsv_SS->Fill(events.getMSV(), evtwt);
        }
      } // close signal
      else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
        htau_pt_QCD->Fill(tau.getPt(), evtwt);
        hel_pt_QCD->Fill(electron.getPt(), evtwt);
        hmsv_QCD->Fill(events.getMSV(), evtwt);
      } // close qcd
      else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()){
        if (evt_charge == 0) {
          htau_pt_WOS->Fill(tau.getPt(), evtwt);
          hel_pt_WOS->Fill(electron.getPt(), evtwt);
          hmsv_WOS->Fill(events.getMSV(), evtwt);
        }
        else {
          htau_pt_WSS->Fill(tau.getPt(), evtwt);
          hel_pt_WSS->Fill(electron.getPt(), evtwt);
          hmsv_WSS->Fill(events.getMSV(), evtwt);
        } // close Wjets
      } // close general

      // if (jets.getNjets() == 0) {
      //
      //   if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      //     if (evt_charge == 0) {
      //       h0_OS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h0_tau_pt_OS->Fill(tau.getPt(), evtwt);
      //       h0_msv_OS->Fill(events.getMSV(), evtwt);
      //     }
      //     else {
      //       h0_SS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h0_tau_pt_SS->Fill(tau.getPt(), evtwt);
      //       h0_msv_SS->Fill(events.getMSV(), evtwt);
      //     }
      //   } // close if signal block
      //   else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
      //     h0_QCD->Fill(tau.getPt(), events.getMSV(), evtwt);
      //     h0_tau_pt_QCD->Fill(tau.getPt(), evtwt);
      //     h0_msv_QCD->Fill(events.getMSV(), evtwt);
      //   } // close if qcd block
      //   else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      //     if (evt_charge == 0) {
      //       h0_WOS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h0_tau_pt_WOS->Fill(tau.getPt(), evtwt);
      //       h0_msv_WOS->Fill(events.getMSV(), evtwt);
      //     }
      //     else {
      //       h0_WSS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h0_tau_pt_WSS->Fill(tau.getPt(), evtwt);
      //       h0_msv_WSS->Fill(events.getMSV(), evtwt);
      //     }
      //   } // close if W block
      //
      // } // close njets == 0
      // else if (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300)) {
      //
      //   if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      //     if (evt_charge == 0) {
      //       h1_OS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h1_tau_pt_OS->Fill(tau.getPt(), evtwt);
      //       h1_msv_OS->Fill(events.getMSV(), evtwt);
      //     }
      //     else {
      //       h1_SS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h1_tau_pt_SS->Fill(tau.getPt(), evtwt);
      //       h1_msv_SS->Fill(events.getMSV(), evtwt);
      //     }
      //   } // close if signal block
      //   else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
      //     h1_QCD->Fill(tau.getPt(), events.getMSV(), evtwt);
      //     h1_tau_pt_QCD->Fill(tau.getPt(), evtwt);
      //     h1_msv_QCD->Fill(events.getMSV(), evtwt);
      //   } // close if qcd block
      //   else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      //     if (evt_charge == 0) {
      //       h1_WOS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h1_tau_pt_WOS->Fill(tau.getPt(), evtwt);
      //       h1_msv_WOS->Fill(events.getMSV(), evtwt);
      //     }
      //     else {
      //       h1_WSS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h1_tau_pt_WSS->Fill(tau.getPt(), evtwt);
      //       h1_msv_WSS->Fill(events.getMSV(), evtwt);
      //     }
      //   } // close if W block
      //
      // } // close njets == 1 (or low dijet mass njets == 2)
      // else if (jets.getNjets() == 2 && jets.getDijetMass() > 300) {
      //
      //   if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      //     if (evt_charge == 0) {
      //       h2_OS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h2_tau_pt_OS->Fill(tau.getPt(), evtwt);
      //       h2_msv_OS->Fill(events.getMSV(), evtwt);
      //     }
      //     else {
      //       h2_SS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h2_tau_pt_SS->Fill(tau.getPt(), evtwt);
      //       h2_msv_SS->Fill(events.getMSV(), evtwt);
      //     }
      //   } // close if signal block
      //   else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
      //     h2_QCD->Fill(tau.getPt(), events.getMSV(), evtwt);
      //     h2_tau_pt_QCD->Fill(tau.getPt(), evtwt);
      //     h2_msv_QCD->Fill(events.getMSV(), evtwt);
      //   } // close if qcd block
      //   else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      //     if (evt_charge == 0) {
      //       h2_WOS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h2_tau_pt_WOS->Fill(tau.getPt(), evtwt);
      //       h2_msv_WOS->Fill(events.getMSV(), evtwt);
      //     }
      //     else {
      //       h2_WSS->Fill(tau.getPt(), events.getMSV(), evtwt);
      //       h2_tau_pt_WSS->Fill(tau.getPt(), evtwt);
      //       h2_msv_WSS->Fill(events.getMSV(), evtwt);
      //     }
      //   } // close if W block
      //
      // } // close njets == 2

    } // close mt, tau selection

  } // close event loop

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
