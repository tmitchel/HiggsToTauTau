// system includes
#include <iostream>
#include <cmath>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"

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

int main(int argc, char* argv[]) {

  std::string sample = "ZZ4L";
  if (argc > 1)
    sample = argv[1];

  std::string fname = "root_files/"+sample+".root";

  auto fin = new TFile(fname.c_str(), "READ");
  auto ntuple = (TTree*)fin->Get("etau_tree");
  auto counts = (TH1F*)fin->Get("nevents");
  auto gen_number = counts->GetBinContent(2);
  auto suffix = "_output.root";
  auto fout = new TFile((sample+suffix).c_str(), "RECREATE");

  double evtwt(luminosity * cross_sections[sample] / gen_number);
  // need to get root files for pileup reweighting
  // auto lumi_weights = new reweight::LumiReWeighting("", "", "", "");

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


    // apply lots of SF's that I don't have
    if (sample != "data") {
      // apply trigger SF
      // apply id SF
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

    if (mt < 50 && tau.getPt() > 30) {
      if (jets.getNjets() == 0) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0)
            h0_OS->Fill(tau.getPt(), events.getMSV());
          else
            h0_SS->Fill(tau.getPt(), events.getMSV());
        } // close if signal block

      } // close njets == 0
      else if (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300)) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0)
            h1_OS->Fill(tau.getPt(), events.getMSV());
          else
            h1_SS->Fill(tau.getPt(), events.getMSV());
        } // close if signal block

      } // close njets == 1 (or low dijet mass njets == 2)
      else if (jets.getNjets() == 2 && jets.getDijetMass() > 300) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0)
            h2_OS->Fill(tau.getPt(), events.getMSV());
          else
            h2_SS->Fill(tau.getPt(), events.getMSV());
        } // close if signal block

      } // close njets == 2
    } // close mt, tau selection




  } // close event loop

  fin->Close();
  fout->cd();
  fout->Write();
  fout->Close();
  return 0;
}
