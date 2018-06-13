// system includes
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_map>

// ROOT includes
#include "TH1D.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooFunctor.h"
#include "RooMsgService.h"

// user includes
#include "include/trigger.h"
#include "include/event_info.h"
#include "include/tau_factory.h"
#include "include/electron_factory.h"
#include "include/jet_factory.h"
#include "include/met_factory.h"
#include "include/gen_factory.h"
#include "include/SF_factory.h"
#include "include/util.h"
#include "include/btagSF.h"
#include "include/LumiReweightingStandAlone.h"

int main(int argc, char* argv[]) {

  ////////////////////////////////////////////////
  // Initial setup:                             //
  // Get file names, normalization, paths, etc. //
  ////////////////////////////////////////////////

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
  fout->mkdir("grabbag");

  // get normalization (lumi & xs are in util.h)
  double norm;
  if (isData)
    norm = 1.0;
  else
    norm = luminosity * cross_sections[sample] / gen_number;

  ///////////////////////////////////////////////
  // Scale Factors:                            //
  // Read weights, hists, graphs, etc. for SFs //
  ///////////////////////////////////////////////

  // read inputs for lumi reweighting
  auto lumi_weights = new reweight::LumiReWeighting("inputs/MC_Moriond17_PU25ns_V1.root", "inputs/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

  // tracking corrections
  TFile *f_Trk = new TFile("inputs/Tracking_EfficienciesAndSF_BCDEFGH.root");
  TGraph *h_Trk = (TGraph*) f_Trk->Get("ratio_eff_eta3_dr030e030_corr");

  // Z-pT reweighting
  TFile *zpt_file=new TFile("inputs/zpt_weights_2016_BtoH.root");
  auto zpt_hist=(TH2F*) zpt_file->Get("zptmass_histo");

  //H->tau tau scale factors
  TFile htt_sf_file("inputs/htt_scalefactors_sm_moriond_v1.root");
  RooWorkspace *htt_sf = (RooWorkspace*)htt_sf_file.Get("w");
  htt_sf_file.Close();
  fout->cd();

  // trigger and ID scale factors
  auto trig_SF = new SF_factory("inputs/Electron_Ele25eta2p1WPTight_eff.root");
  auto id_SF = new SF_factory("inputs/Electron_IdIso0p10_eff.root");
  fout->cd();

  //////////////////////////////////////
  // Final setup:                     //
  // Declare histograms and factories //
  //////////////////////////////////////

  // declare histograms (histogram initializer functions in util.h)
  auto histos = new std::unordered_map<std::string, TH1D*>;
  auto histos_2d = new std::unordered_map<std::string, TH2F*>;
  fout->cd("grabbag");
  initHistos_1D(histos);
  initHistos_2D(histos_2d, fout, name);

  // construct factories
  trigger          trigs(ntuple);
  event_info       events(ntuple);
  electron_factory electrons(ntuple);
  tau_factory      taus(ntuple);
  jet_factory      jets(ntuple);
  met_factory      met(ntuple);
  gen_factory      gen(ntuple);
  double n70_count;

  // begin the event loop
  Int_t nevts = ntuple->GetEntries();
  for (Int_t i = 0; i < nevts; i++) {
    ntuple->GetEntry(i);
    if (i % 100000 == 0)
      std::cout << "Processing event: " << i << " out of " << nevts << std::endl;

    // find the event weight (not lumi*xs if looking at W or Drell-Yan)
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

    // fout->cd("grabbag");
    histos->at("cutflow")->Fill(0., evtwt);

    //////////////////////////////////////////////////////////
    // Event Selection:                                     //
    //   - Trigger: Ele25eta2p1Tight -> pass, match, filter //
    //   - Electron: pT > 26, |eta| < 2.1                   //
    //   - Tau: decayModeFinding, |eta| < 2.3               //
    //////////////////////////////////////////////////////////

    // electron pT > 26 GeV
    auto electron = electrons.run_factory();
    if (electron.getPt() > 26 && fabs(electron.getEta()) < 2.1)  histos->at("cutflow") -> Fill(1., evtwt);
    else continue;

    // electron passes Ele25eta2p1Tight
    if (!isData || (trigs.getPassEle25eta2p1Tight() && trigs.getMatchEle25eta2p1Tight() && trigs.getFilterEle25eta2p1Tight())) histos->at("cutflow") -> Fill(2., evtwt);
    else continue;

    // tau passes decay mode finding
    auto tau = taus.run_factory();
    if (tau.getDecayModeFinding()) histos->at("cutflow") -> Fill(3., evtwt);
    else continue;

    // tau |eta| < 2.3
    if (fabs(tau.getEta()) < 2.3) histos->at("cutflow") -> Fill(4., evtwt);
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

    histos->at("cutflow") -> Fill(5., evtwt);

    // apply all scale factors/corrections/etc.
    if (!isData) {

      // apply trigger and id SF's
      sf_trg = trig_SF->getDataEfficiency(electron.getPt(), electron.getEta());
      sf_id = id_SF->getSF(electron.getPt(), electron.getEta());
      evtwt *= (sf_trg * sf_id * h_Trk->Eval(electron.getEta()) * lumi_weights->weight(events.getNPU()));
      // evtwt *= (sf_trg * sf_id * h_Trk->GetBinContent(h_Trk->FindBin(electron.getEta())) * lumi_weights->weight(events.getNPU()));

      // tau ID efficiency SF
      if (tau.getGenMatch() == 5)
        evtwt *= 0.95;

      // anti-lepton discriminator SFs
      if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
        if (fabs(tau.getEta())<0.4) evtwt *= 1.263;
        else if (fabs(tau.getEta())<0.8) evtwt *= 1.364;
        else if (fabs(tau.getEta())<1.2) evtwt *= 0.854;
        else if (fabs(tau.getEta())<1.7) evtwt *= 1.712;
        else if (fabs(tau.getEta())<2.3) evtwt *= 2.324;
        if (name == "ZL" && events.getDecayMode() == 0) evtwt *= 0.74; //ZL corrections Laura
        else if (name == "ZL" && events.getDecayMode() == 1) evtwt *= 1.0;
      }

      if (tau.getGenMatch() == 1 or tau.getGenMatch() == 3){//Yiwen
         if (fabs(tau.getEta())<1.460) evtwt *= 1.213;
         else if (fabs(tau.getEta())>1.558) evtwt *= 1.375;
       }

       // Z-pT Reweighting
       if (name=="EWKZLL" || name=="EWKZNuNu" || name=="ZTT" || name=="ZLL" || name=="ZL" || name=="ZJ")
        evtwt *= zpt_hist->GetBinContent(zpt_hist->GetXaxis()->FindBin(gen.getGenM()),zpt_hist->GetYaxis()->FindBin(gen.getGenPt()));

      // top-pT Reweighting
      if (name == "TTT" || name == "TT" || name == "TTJ") {
        float pt_top1 = std::min(float(400.), jets.getTopPt1());
        float pt_top2 = std::min(float(400.), jets.getTopPt2());
        evtwt *= sqrt(exp(0.0615-0.0005*pt_top1)*exp(0.0615-0.0005*pt_top2));
      }

      // b-tagging SF (only used in scaling W, I believe)
      // int nbtagged = std::min(2, jets.getNbtag());
      // auto bjets = jets.getBtagJets();
      // float weight_btag( bTagEventWeight(nbtagged, bjets.at(0).getPt() ,bjets.at(0).getFlavor(), bjets.at(1).getPt(), bjets.at(1).getFlavor() ,1,0,0) );
      // if (nbtagged>2) weight_btag=0;

      // recoil correction
      htt_sf->var("t_pt")->setVal(tau.getPt());
      htt_sf->var("t_eta")->setVal(tau.getEta());
      htt_sf->var("t_dm")->setVal(events.getDecayMode());
      evtwt *= htt_sf->function("t_genuine_TightIso_mt_ratio")->getVal();

    }
    fout->cd();

    // calculate mt (calculate_mt in util.h)
    double met_x = met.getMet() * cos(met.getMetPhi());
    double met_y = met.getMet() * sin(met.getMetPhi());
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    double mt = calculate_mt(&electron, met_x, met_y, met_pt);
    int evt_charge = tau.getCharge() + electron.getCharge();

    TLorentzVector Higgs = electron.getP4() + tau.getP4() + met.getP4();

    if (mt > 80 && mt < 200 && evt_charge == 0 && tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
      histos->at("n70") -> Fill(0.1, evtwt);
      if (jets.getNjets() == 0 && events.getMSV() < 400)
        histos->at("n70") -> Fill(1.1, evtwt);
      else if (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() > 300 && Higgs.Pt() < 100))
        histos->at("n70") -> Fill(2.1, evtwt);
      else if (jets.getNjets() > 1 && jets.getDijetMass() > 300 && Higgs.Pt() > 100)
        histos->at("n70") -> Fill(3.1, evtwt);
    }

    if (mt < 50 && tau.getPt() > 30) {

      histos->at("cutflow") -> Fill(6., evtwt);
      // inclusive selection
      if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
        histos->at("cutflow") -> Fill(7., evtwt);

        if (evt_charge == 0) {
          // fill histograms
          histos->at("cutflow") -> Fill(8., evtwt);
          if (deltaR(electron.getEta(), electron.getPhi(), tau.getEta(), tau.getPhi()) > 0.5) {
            histos->at("cutflow")     -> Fill(9.                  , evtwt);
            histos->at("hel_pt")      -> Fill(electron.getPt()    , evtwt);
            histos->at("hel_eta")     -> Fill(electron.getEta()   , evtwt);
            histos->at("hel_phi")     -> Fill(electron.getPhi()   , evtwt);
            histos->at("htau_pt")     -> Fill(tau.getPt()         , evtwt);
            histos->at("htau_eta")    -> Fill(tau.getEta()        , evtwt);
            histos->at("htau_phi")    -> Fill(tau.getPhi()        , evtwt);
            histos->at("hmet")        -> Fill(met.getMet()        , evtwt);
            histos->at("hmet_x")      -> Fill(met_x               , evtwt);
            histos->at("hmet_y")      -> Fill(met_y               , evtwt);
            histos->at("hmet_pt")     -> Fill(met_pt              , evtwt);
            histos->at("hmt")         -> Fill(mt                  , evtwt);
            histos->at("hnjets")      -> Fill(jets.getNjets()     , evtwt);
            histos->at("hmjj")        -> Fill(jets.getDijetMass() , evtwt);
            histos->at("hmsv")        -> Fill(events.getVisM()    , evtwt);
            histos->at("hNGenJets")   -> Fill(gen.getNumGenJets() , evtwt);
          }
        }
        else {
          histos->at("htau_pt_SS")  -> Fill(tau.getPt()        , evtwt);
          histos->at("hel_pt_SS")   -> Fill(electron.getPt()   , evtwt);
          histos->at("htau_phi_SS") -> Fill(tau.getPhi()       , evtwt);
          histos->at("hel_phi_SS")  -> Fill(electron.getPhi()  , evtwt);
          histos->at("hmsv_SS")     -> Fill(events.getVisM()   , evtwt);
          histos->at("hmet_SS")     -> Fill(met.getMet()       , evtwt);
          histos->at("hmt_SS")      -> Fill(mt                 , evtwt);
          histos->at("hmjj_SS")     -> Fill(jets.getDijetMass(), evtwt);
        }
      } // close signal
      if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
        histos->at("htau_pt_QCD")  -> Fill(tau.getPt()        , evtwt);
        histos->at("hel_pt_QCD")   -> Fill(electron.getPt()   , evtwt);
        histos->at("htau_phi_QCD") -> Fill(tau.getPhi()       , evtwt);
        histos->at("hel_phi_QCD")  -> Fill(electron.getPhi()  , evtwt);
        histos->at("hmsv_QCD")     -> Fill(events.getVisM()   , evtwt);
        histos->at("hmet_QCD")     -> Fill(met.getMet()       , evtwt);
        histos->at("hmt_QCD")      -> Fill(mt                 , evtwt);
        histos->at("hmjj_QCD")     -> Fill(jets.getDijetMass(), evtwt);
      } // close qcd
      if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()){
        if (evt_charge == 0) {
          histos->at("htau_pt_WOS")  -> Fill(tau.getPt()        , evtwt);
          histos->at("hel_pt_WOS")   -> Fill(electron.getPt()   , evtwt);
          histos->at("htau_phi_WOS") -> Fill(tau.getPhi()       , evtwt);
          histos->at("hel_phi_WOS")  -> Fill(electron.getPhi()  , evtwt);
          histos->at("hmsv_WOS")     -> Fill(events.getVisM()   , evtwt);
          histos->at("hmet_WOS")     -> Fill(met.getMet()       , evtwt);
          histos->at("hmt_WOS")      -> Fill(mt                 , evtwt);
          histos->at("hmjj_WOS")     -> Fill(jets.getDijetMass(), evtwt);
        }
        else {
          histos->at("htau_pt_WSS")  -> Fill(tau.getPt()        , evtwt);
          histos->at("hel_pt_WSS")   -> Fill(electron.getPt()   , evtwt);
          histos->at("htau_phi_WSS") -> Fill(tau.getPhi()       , evtwt);
          histos->at("hel_phi_WSS")  -> Fill(electron.getPhi()  , evtwt);
          histos->at("hmsv_WSS")     -> Fill(events.getVisM()   , evtwt);
          histos->at("hmet_WSS")     -> Fill(met.getMet()       , evtwt);
          histos->at("hmt_WSS")      -> Fill(mt                 , evtwt);
          histos->at("hmjj_WSS")     -> Fill(jets.getDijetMass(), evtwt);
        } // close Wjets
      } // close general

      // event categorizaation
      if (jets.getNjets() == 0) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("h0_OS") -> Fill(tau.getPt(), events.getVisM(), evtwt);
          }
          else {
            histos_2d->at("h0_SS") -> Fill(tau.getPt(), events.getVisM(), evtwt);
          }
        } // close if signal block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          histos_2d->at("h0_QCD") -> Fill(tau.getPt(), events.getVisM(), evtwt);
        } // close if qcd block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("h0_WOS") -> Fill(tau.getPt(), events.getVisM(), evtwt);
          }
          else {
            histos_2d->at("h0_WSS") -> Fill(tau.getPt(), events.getVisM(), evtwt);
          }
        } // close if W block

      } // close njets == 0
      else if (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300 && Higgs.Pt() < 50)) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("h1_OS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
          else {
            histos_2d->at("h1_SS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
        } // close if signal block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          histos_2d->at("h1_QCD") -> Fill(tau.getPt(), events.getMSV(), evtwt);
        } // close if qcd block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("h1_WOS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
          else {
            histos_2d->at("h1_WSS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
        } // close if W block

      } // close njets == 1 (or low dijet mass njets == 2)
      else if (jets.getNjets() > 1 && Higgs.Pt() > 50 && jets.getDijetMass() > 300) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("h2_OS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
          else {
            histos_2d->at("h2_SS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
        } // close if signal block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          histos_2d->at("h2_QCD") -> Fill(tau.getPt(), events.getMSV(), evtwt);
        } // close if qcd block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("h2_WOS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
          else {
            histos_2d->at("h2_WSS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
        } // close if W block

      } // close VBF
      else if (jets.getNjets() > 1 && jets.getDijetMass() < 300) {

        if (tau.getTightIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("hvbf_OS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
          else {
            histos_2d->at("hvbf_SS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
        } // close if signal block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.30 && tau.getAgainstTightElectron()) {
          histos_2d->at("hvbf_QCD") -> Fill(tau.getPt(), events.getMSV(), evtwt);
        } // close if qcd block
        if (tau.getMediumIsoMVA() && electron.getIso() < 0.10 && tau.getAgainstTightElectron() && tau.getAgainstLooseMuon()) {
          if (evt_charge == 0) {
            histos_2d->at("hvbf_WOS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
          else {
            histos_2d->at("hvbf_WSS") -> Fill(tau.getPt(), events.getMSV(), evtwt);
          }
        } // close if W block

      } // close VH

    } // close mt, tau selection

  } // close event loop
  histos->at("n70")->Fill(1, n70_count);
  histos->at("n70")->Write();

  fin->Close();
  fout->cd();
  // grabbag->Write();
  fout->Write();
  fout->Close();
  return 0;
}
