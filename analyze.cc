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

  std::string sample = "ZZ4L";
  if (argc > 1)
    sample = argv[1];

  std::string name = sample;
  if (argc > 2)
    name = argv[2];

  std::string fname;
  bool isData = sample.find("Data") != std::string::npos;
  if (isData)
    fname = "root://cmsxrootd.fnal.gov//store/user/tmitchel/smhet_22feb_SV/"+sample+".root";
  else
    fname = "root://cmsxrootd.fnal.gov//store/user/tmitchel/smhet_20march/"+sample+".root";

  std::cout << "Opening file... " << sample << std::endl;
  auto fin = TFile::Open(fname.c_str());
  std::cout << "Loading Ntuple..." << std::endl;
  auto ntuple = (TTree*)fin->Get("etau_tree");
  auto counts = (TH1D*)fin->Get("nevents");
  auto gen_number = counts->GetBinContent(2);
  auto suffix = "_output.root";
  auto prefix = "output/";
  std::string filename;
  if (name == sample)
    filename = prefix + name + suffix;
  else
    filename = prefix + sample + std::string("_") + name + suffix;
  auto fout = new TFile(filename.c_str(), "RECREATE");

  double norm;
  if (isData)
    norm = 1.0;
  else
    norm = luminosity * cross_sections[sample] / gen_number;

  // need to get root files for pileup reweighting
  auto lumi_weights = new reweight::LumiReWeighting("inputs/MC_Moriond17_PU25ns_V1.root", "inputs/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  TFile *f_Trk=new TFile("inputs/ratios.root");
  TGraph *h_Trk=(TGraph*) f_Trk->Get("ratio_eta");

  auto trig_SF = new SF_factory("inputs/Electron_Ele25eta2p1WPTight_eff.root");
  auto id_SF = new SF_factory("inputs/Electron_IdIso0p10_eff.root");
  fout->cd();

  // stolen directly from Cecile's code
  // will probably find somewhere to hide all of this for readability
  float bins0[] = {0,60,65,70,75,80,85,90,95,100,105,110,115,400};
  float bins1[] = {0,70,80,90,100,110,120,130,140,150,180,300};
  float bins2[] = {0,90,105,120,135,150,400};

  float bins_pth[] = {0,60,100,140,180,220,500};
  float bins_mjj[] = {300,600,900,1200,1700,3000};
  float bins_taupt[] = {30,35,40,45,50,55,300};

  int  binnum1 = sizeof(bins1)/sizeof(Float_t) - 1;
  int  binnum2 = sizeof(bins2)/sizeof(Float_t) - 1;
  int  binnum0 = sizeof(bins0)/sizeof(Float_t) - 1;
  int  binnum_pth = sizeof(bins_pth)/sizeof(Float_t) - 1;
  int  binnum_taupt = sizeof(bins_taupt)/sizeof(Float_t) - 1;
  int  binnum_mjj = sizeof(bins_mjj)/sizeof(Float_t) - 1;

  // TH2F *h0_OS=new TH2F ("h0_OS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_OS->Sumw2();
  // TH2F *h1_OS=new TH2F ("h1_OS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_OS->Sumw2();
  // TH2F *h2_OS=new TH2F ("h2_OS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_OS->Sumw2();
  // TH2F *h0_SS=new TH2F ("h0_SS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_SS->Sumw2();
  // TH2F *h1_SS=new TH2F ("h1_SS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_SS->Sumw2();
  // TH2F *h2_SS=new TH2F ("h2_SS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_SS->Sumw2();
  // TH2F *h0_QCD=new TH2F ("h0_QCD","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_QCD->Sumw2();
  // TH2F *h1_QCD=new TH2F ("h1_QCD","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_QCD->Sumw2();
  // TH2F *h2_QCD=new TH2F ("h2_QCD","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_QCD->Sumw2();
  // TH2F *h0_WOS=new TH2F ("h0_WOS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_WOS->Sumw2();
  // TH2F *h1_WOS=new TH2F ("h1_WOS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_WOS->Sumw2();
  // TH2F *h2_WOS=new TH2F ("h2_WOS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_WOS->Sumw2();
  // TH2F *h0_WSS=new TH2F ("h0_WSS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_WSS->Sumw2();
  // TH2F *h1_WSS=new TH2F ("h1_WSS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_WSS->Sumw2();
  // TH2F *h2_WSS=new TH2F ("h2_WSS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_WSS->Sumw2();

  // TH1D* h0_tau_pt_QCD = new TH1D("h0_tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h1_tau_pt_QCD = new TH1D("h1_tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h2_tau_pt_QCD = new TH1D("h2_tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h0_tau_pt_SS = new TH1D("h0_tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h1_tau_pt_SS = new TH1D("h1_tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h2_tau_pt_SS = new TH1D("h2_tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h0_tau_pt_OS = new TH1D("h0_tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h1_tau_pt_OS = new TH1D("h1_tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h2_tau_pt_OS = new TH1D("h2_tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h0_tau_pt_WSS = new TH1D("h0_tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h1_tau_pt_WSS = new TH1D("h1_tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h2_tau_pt_WSS = new TH1D("h2_tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h0_tau_pt_WOS = new TH1D("h0_tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h1_tau_pt_WOS = new TH1D("h1_tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  // TH1D* h2_tau_pt_WOS = new TH1D("h2_tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  //
  // TH1D* h0_msv_QCD = new TH1D("h0_msv_QCD", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h1_msv_QCD = new TH1D("h1_msv_QCD", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h2_msv_QCD = new TH1D("h2_msv_QCD", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h0_msv_SS = new TH1D("h0_msv_SS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h1_msv_SS = new TH1D("h1_msv_SS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h2_msv_SS = new TH1D("h2_msv_SS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h0_msv_OS = new TH1D("h0_msv_OS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h1_msv_OS = new TH1D("h1_msv_OS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h2_msv_OS = new TH1D("h2_msv_OS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h0_msv_WSS = new TH1D("h0_msv_WSS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h1_msv_WSS = new TH1D("h1_msv_WSS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h2_msv_WSS = new TH1D("h2_msv_WSS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h0_msv_WOS = new TH1D("h0_msv_WOS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h1_msv_WOS = new TH1D("h1_msv_WOS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);
  // TH1D* h2_msv_WOS = new TH1D("h2_msv_WOS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.);

  TH1D *n70=new TH1D ("n70", "n70", 6,0,6);
  TH1D* cutflow = new TH1D("cutflow", "Cutflow", 11, -0.5, 10.5);

  TH1D* hel_pt = new TH1D("el_pt", "Electron p_{T};p_{T} [GeV];;", 20, 0., 100);
  TH1D* hel_eta = new TH1D("el_eta", "Electron #eta;#eta [GeV];;", 80, -4., 4.);

  TH1D* htau_pt = new TH1D("tau_pt", "Tau p_{T};p_{T} [GeV];;", 40, 0., 200);
  TH1D* htau_eta = new TH1D("tau_eta", "Tau #eta;#eta [GeV];;", 80, -4., 4.);

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

  // construct factories
  trigger          trigs(ntuple);
  event_info       events(ntuple);
  electron_factory electrons(ntuple);
  tau_factory      taus(ntuple);
  jet_factory      jets(ntuple);
  met_factory      met(ntuple);
  gen_factory      gen(ntuple);

  Int_t nevts = ntuple->GetEntries();
  for (Int_t i = 0; i < nevts; i++) {
    ntuple->GetEntry(i);
    if (i % 100000 == 0)
      std::cout << "Processing event: " << i << " out of " << nevts << std::endl;

    double evtwt(norm), sf_trg(1.), sf_id(1.);
    cutflow->Fill(0., evtwt);

    // electron pT > 27 GeV
    auto electron = electrons.run_factory();
    if (electron.getPt() > 27)  cutflow->Fill(1., evtwt);
    else continue;

    // electron passes Ele25eta2p1Tight
    if (trigs.getPassEle25eta2p1Tight()) cutflow->Fill(2., evtwt);
    else continue;

    // tau passes decay mode finding and |eta| < 2.3
    auto tau = taus.run_factory();
    if (tau.getDecayModeFinding()) cutflow->Fill(3., evtwt);
    else continue;
    if (abs(tau.getEta()) < 2.3) cutflow->Fill(4., evtwt);
    else continue;

    // Separate Drell-Yan
    if (name == "ZL" && tau.getGenMatch() > 4)
      continue;
    if (name == "ZTT" && tau.getGenMatch() != 5)
      continue;
    if (name == "ZLL" && tau.getGenMatch() == 5)
      continue;
    if (name == "ZJ" && tau.getGenMatch() != 6)
      continue;
    if (name == "ZL" || (name == "ZLL" && tau.getGenMatch() < 5)) {
        if (tau.getEta() < 1.460)
          evtwt *= 1.80;
        else if (tau.getEta() > 1.558)
          evtwt *= 1.30;
    }
    cutflow->Fill(5., evtwt);

    // apply lots of SF's that I don't have
    if (!isData) {
      sf_trg = trig_SF->getDataEfficiency(electron.getPt(), electron.getEta());
      sf_id = id_SF->getSF(electron.getPt(), electron.getEta());
      evtwt *= (sf_trg * sf_id * h_Trk->Eval(electron.getEta()) * lumi_weights->weight(events.getNPU()));
    }
    fout->cd();

    // Tau energy scale corrections
    // ...

    // electron/tau visible mass
    if (!isData) {
      if (name == "W") {
        if (gen.getNumGenJets() == 5)
          evtwt *= 12.43;
        if (gen.getNumGenJets() == 6)
          evtwt *= 4.019;
        if (gen.getNumGenJets() == 7)
          evtwt *= 2.222;
        if (gen.getNumGenJets() == 8)
          evtwt *= 1.077;
        if (gen.getNumGenJets() == 9)
          evtwt *= 1.176;
      }

      if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
        if (gen.getNumGenJets() == 5)
          evtwt *= 1.281;
        if (gen.getNumGenJets() == 6)
          evtwt *= 0.2825;
        if (gen.getNumGenJets() == 7)
          evtwt *= 0.3021;
        if (gen.getNumGenJets() == 8)
          evtwt *= 0.3127;
        if (gen.getNumGenJets() == 9)
          evtwt *= 0.2511;
      }

      // corrections based on decay mode
      if (tau.getGenMatch() == 5 && events.getDecayMode() == 0)
        evtwt *= 0.97;
      else if (tau.getGenMatch() == 5 && events.getDecayMode() == 1)
        evtwt *= 0.92;
      else if (tau.getGenMatch() == 5 && events.getDecayMode() < 2)
        evtwt *= 0.94;
      else if (tau.getGenMatch() == 5 && events.getDecayMode() == 10)
        evtwt *= 0.80;
    }

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
          hel_pt->Fill(electron.getPt(), evtwt);
          hel_eta->Fill(electron.getEta(), evtwt);
          htau_pt->Fill(tau.getPt(), evtwt);
          htau_eta->Fill(tau.getEta(), evtwt);
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
        }
        else {
          htau_pt_SS->Fill(tau.getPt(), evtwt);
          hel_pt_SS->Fill(electron.getPt(), evtwt);
          hmsv_SS->Fill(events.getMSV(), evtwt);
        }
      }
      else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
        htau_pt_QCD->Fill(tau.getPt(), evtwt);
        hel_pt_QCD->Fill(electron.getPt(), evtwt);
        hmsv_QCD->Fill(events.getMSV(), evtwt);
      }
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
        }
      }

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
