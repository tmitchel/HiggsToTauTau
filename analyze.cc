// system includes
#include <iostream>
#include <cmath>

// ROOT includes
#include "TH1F.h"
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
  auto counts = (TH1F*)fin->Get("nevents");
  auto gen_number = counts->GetBinContent(2);
  auto suffix = "_output.root";
  auto prefix = "output/";
  auto fout = new TFile((prefix+sample+suffix).c_str(), "RECREATE");

  double evtwt;
  if (isData)
    evtwt = 1.0;
  else
    evtwt = luminosity * cross_sections[sample] / gen_number;

  // need to get root files for pileup reweighting
  auto lumi_weights = new reweight::LumiReWeighting("inputs/MC_Moriond17_PU25ns_V1.root", "inputs/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  TFile *f_Trk=new TFile("inputs/ratios.root");
  TGraph *h_Trk=(TGraph*) f_Trk->Get("ratio_eta");

  auto trig_SF = new SF_factory("Electron_Ele25eta2p1WPTight_eff.root");
  auto id_SF = new SF_factory("Electron_IdIso0p10_eff.root");

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

  TH1F *n70=new TH1F ("n70", "n70", 6,0,6);

  TH2F *h0_OS=new TH2F ("h0_OS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_OS->Sumw2();
  TH2F *h1_OS=new TH2F ("h1_OS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_OS->Sumw2();
  TH2F *h2_OS=new TH2F ("h2_OS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_OS->Sumw2();
  TH2F *h0_SS=new TH2F ("h0_SS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_SS->Sumw2();
  TH2F *h1_SS=new TH2F ("h1_SS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_SS->Sumw2();
  TH2F *h2_SS=new TH2F ("h2_SS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_SS->Sumw2();
  TH2F *h0_QCD=new TH2F ("h0_QCD","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_QCD->Sumw2();
  TH2F *h1_QCD=new TH2F ("h1_QCD","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_QCD->Sumw2();
  TH2F *h2_QCD=new TH2F ("h2_QCD","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_QCD->Sumw2();
  TH2F *h0_WOS=new TH2F ("h0_WOS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_WOS->Sumw2();
  TH2F *h1_WOS=new TH2F ("h1_WOS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_WOS->Sumw2();
  TH2F *h2_WOS=new TH2F ("h2_WOS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_WOS->Sumw2();
  TH2F *h0_WSS=new TH2F ("h0_WSS","Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0);h0_WSS->Sumw2();
  TH2F *h1_WSS=new TH2F ("h1_WSS","Invariant mass",binnum_pth,bins_pth,binnum1,bins1);h1_WSS->Sumw2();
  TH2F *h2_WSS=new TH2F ("h2_WSS","Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2);h2_WSS->Sumw2();

  TH1F* hel_pt = new TH1F("hel_pt", "Electron p_{T};p_{T} [GeV];;", 100, 0., 500);
  TH1F* hel_eta = new TH1F("hel_eta", "Electron #eta;#eta [GeV];;", 80, -4., 4.);

  TH1F* htau_pt = new TH1F("htau_pt", "Tau p_{T};p_{T} [GeV];;", 100, 0., 500);
  TH1F* htau_eta = new TH1F("htau_eta", "Tau #eta;#eta [GeV];;", 80, -4., 4.);

  TH1F* h0_tau_pt_QCD = new TH1F("h0_tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h1_tau_pt_QCD = new TH1F("h1_tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h2_tau_pt_QCD = new TH1F("h2_tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h0_tau_pt_SS = new TH1F("h0_tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h1_tau_pt_SS = new TH1F("h1_tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h2_tau_pt_SS = new TH1F("h2_tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h0_tau_pt_OS = new TH1F("h0_tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h1_tau_pt_OS = new TH1F("h1_tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h2_tau_pt_OS = new TH1F("h2_tau_pt_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h0_tau_pt_WSS = new TH1F("h0_tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h1_tau_pt_WSS = new TH1F("h1_tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h2_tau_pt_WSS = new TH1F("h2_tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h0_tau_pt_WOS = new TH1F("h0_tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h1_tau_pt_WOS = new TH1F("h1_tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);
  TH1F* h2_tau_pt_WOS = new TH1F("h2_tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 500.);

  TH1F* h0_msv_QCD = new TH1F("h0_msv_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h1_msv_QCD = new TH1F("h1_msv_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h2_msv_QCD = new TH1F("h2_msv_QCD", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h0_msv_SS = new TH1F("h0_msv_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h1_msv_SS = new TH1F("h1_msv_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h2_msv_SS = new TH1F("h2_msv_SS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h0_msv_OS = new TH1F("h0_msv_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h1_msv_OS = new TH1F("h1_msv_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h2_msv_OS = new TH1F("h2_msv_OS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h0_msv_WSS = new TH1F("h0_msv_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h1_msv_WSS = new TH1F("h1_msv_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h2_msv_WSS = new TH1F("h2_msv_WSS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h0_msv_WOS = new TH1F("h0_msv_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h1_msv_WOS = new TH1F("h1_msv_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);
  TH1F* h2_msv_WOS = new TH1F("h2_msv_WOS", "Tau p_{T}; p_{T} [GeV]", 100, 0., 300.);

  TH1F* hmet = new TH1F("met", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);
  TH1F* hmetphi = new TH1F("metphi", "Missing E_{T} #phi;Missing E_{T} [GeV];;", 60, -3.14, 3.14);
  TH1F* hmet_x = new TH1F("met_x", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);
  TH1F* hmet_y = new TH1F("met_y", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);
  TH1F* hmet_pt = new TH1F("met_pt", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500);

  TH1F* hmt = new TH1F("mt", "MT", 50, 0, 100);
  TH1F* hnjets = new TH1F("njets", "N(jets)", 10, -0.5, 9.5);
  TH1F* hmjj = new TH1F("mjj", "Dijet Mass; Mass [GeV];;", 100, 0, 200);
  TH1F* hmsv = new TH1F("msv", "SV Fit Mass; Mass [GeV];;", 100, 0, 300);

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
    if (i % 10000 == 0)
      std::cout << "Processing event: " << i << " out of " << nevts << std::endl;

    // electron pT > 27 GeV
    auto electron = electrons.run_factory();
    if (electron.getPt() < 27)
      continue;

    // electron passes Ele25eta2p1Tight
    if (!trigs.getPassEle25eta2p1Tight())
      continue;

    // tau passes decay mode finding and |eta| < 2.3
    auto tau = taus.run_factory();
    if (!tau.getDecayModeFinding() || tau.getEta() > 2.3)
      continue;

    double sf_trg(1.), sf_id(1.);

    // apply lots of SF's that I don't have
    if (!isData) {
      sf_trg = trig_SF->getDataEfficiency(electron.getPt(), electron.getEta());
      sf_id = id_SF->GetSF(electron.getPt(), electron.getEta());
      evtwt *= (sf_trg * sf_id * h_Trk->Eval(electron.getEta()) * lumi_weights->weight(events.getNPU()));
    }

    // Tau energy scale corrections
    // ...

    // electron/tau visible mass
    // ...

    if (sample == "W") {
      // weights based on NUP, but that isn't in the new ntuples...
    }

    if (sample == "ZTT" || sample == "ZLL" || sample == "ZL" || sample == "ZJ") {
      // more NUP based weights
    }

    // corrections based on decay mode
    // ...

    double met_x = met.getMet() * cos(met.getMetPhi());
    double met_y = met.getMet() * sin(met.getMetPhi());
    // more tau energy scale corrections to met_x and met_y
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    double mt = calculate_mt(&electron, met_x, met_y, met_pt);
    int evt_charge = tau.getCharge() + electron.getCharge();

    // fill histograms
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

    if (mt < 50 && tau.getPt() > 30) {
      if (jets.getNjets() == 0) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            h0_OS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h0_tau_pt_OS->Fill(tau.getPt(), evtwt);
            h0_msv_OS->Fill(events.getMSV(), evtwt);
          }
          else {
            h0_SS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h0_tau_pt_SS->Fill(tau.getPt(), evtwt);
            h0_msv_SS->Fill(events.getMSV(), evtwt);
          }
        } // close if signal block
        else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          h0_QCD->Fill(tau.getPt(), events.getMSV(), evtwt);
          h0_tau_pt_QCD->Fill(tau.getPt(), evtwt);
          h0_msv_QCD->Fill(events.getMSV(), evtwt);
        } // close if qcd block
        else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            h0_WOS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h0_tau_pt_WOS->Fill(tau.getPt(), evtwt);
            h0_msv_WOS->Fill(events.getMSV(), evtwt);
          }
          else {
            h0_WSS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h0_tau_pt_WSS->Fill(tau.getPt(), evtwt);
            h0_msv_WSS->Fill(events.getMSV(), evtwt);
          }
        } // close if W block

      } // close njets == 0
      else if (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300)) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            h1_OS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h1_tau_pt_OS->Fill(tau.getPt(), evtwt);
            h1_msv_OS->Fill(events.getMSV(), evtwt);
          }
          else {
            h1_SS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h1_tau_pt_SS->Fill(tau.getPt(), evtwt);
            h1_msv_SS->Fill(events.getMSV(), evtwt);
          }
        } // close if signal block
        else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          h1_QCD->Fill(tau.getPt(), events.getMSV(), evtwt);
          h1_tau_pt_QCD->Fill(tau.getPt(), evtwt);
          h1_msv_QCD->Fill(events.getMSV(), evtwt);
        } // close if qcd block
        else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            h1_WOS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h1_tau_pt_WOS->Fill(tau.getPt(), evtwt);
            h1_msv_WOS->Fill(events.getMSV(), evtwt);
          }
          else {
            h1_WSS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h1_tau_pt_WSS->Fill(tau.getPt(), evtwt);
            h1_msv_WSS->Fill(events.getMSV(), evtwt);
          }
        } // close if W block


      } // close njets == 1 (or low dijet mass njets == 2)
      else if (jets.getNjets() == 2 && jets.getDijetMass() > 300) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            h2_OS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h2_tau_pt_OS->Fill(tau.getPt(), evtwt);
            h2_msv_OS->Fill(events.getMSV(), evtwt);
          }
          else {
            h2_SS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h2_tau_pt_SS->Fill(tau.getPt(), evtwt);
            h2_msv_SS->Fill(events.getMSV(), evtwt);
          }
        } // close if signal block
        else if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          h2_QCD->Fill(tau.getPt(), events.getMSV(), evtwt);
          h2_tau_pt_QCD->Fill(tau.getPt(), evtwt);
          h2_msv_QCD->Fill(events.getMSV(), evtwt);
        } // close if qcd block
        else if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            h2_WOS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h2_tau_pt_WOS->Fill(tau.getPt(), evtwt);
            h2_msv_WOS->Fill(events.getMSV(), evtwt);
          }
          else {
            h2_WSS->Fill(tau.getPt(), events.getMSV(), evtwt);
            h2_tau_pt_WSS->Fill(tau.getPt(), evtwt);
            h2_msv_WSS->Fill(events.getMSV(), evtwt);
          }
        } // close if W block

      } // close njets == 2
    } // close mt, tau selection

  } // close event loop

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
