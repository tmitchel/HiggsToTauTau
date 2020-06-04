// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_FSA_DITAU_FACTORY_H_
#define INCLUDE_FSA_DITAU_FACTORY_H_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"
#include "../models/tau.h"

class ditau_factory {
 private:
  // Tau 1 variables
  Int_t gen_match_1;
  Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, iso_1, q_1, mt_1, tZTTGenPt_1, tZTTGenEta_1, tZTTGenPhi_1;
  Float_t decayMode_1, dmf_1, dmf_new_1;
  // Tau 2 variables
  Int_t gen_match_2;
  Float_t px_2, py_2, pz_2, pt_2, eta_2, phi_2, m_2, e_2, iso_2, q_2, mt_2, tZTTGenPt_2, tZTTGenEta_2, tZTTGenPhi_2;
  Float_t againstElectronTightMVA6_2, againstElectronVLooseMVA6_2, againstMuonTight3_2, againstMuonLoose3_2, decayMode_2, dmf_2, dmf_new_2;
  Float_t byVLooseIsolationMVArun2v1DBoldDMwLT_2, byLooseIsolationMVArun2v1DBoldDMwLT_2, byMediumIsolationMVArun2v1DBoldDMwLT_2,
    byTightIsolationMVArun2v1DBoldDMwLT_2, byVTightIsolationMVArun2v1DBoldDMwLT_2, byVVTightIsolationMVArun2v1DBoldDMwLT_2;
  Float_t tTightDeepTau2017v2p1VSe, tVVLooseDeepTau2017v2p1VSe, tVVVLooseDeepTau2017v2p1VSe, tTightDeepTau2017v2p1VSmu, tVLooseDeepTau2017v2p1VSmu;
  Float_t tVVVLooseDeepTau2017v2p1VSjet, tVLooseDeepTau2017v2p1VSjet, tLooseDeepTau2017v2p1VSjet, tMediumDeepTau2017v2p1VSjet,
    tTightDeepTau2017v2p1VSjet, tVTightDeepTau2017v2p1VSjet, tVVTightDeepTau2017v2p1VSjet, deepiso_2;
  Float_t tes_syst_up, tes_syst_down, ftes_syst_up, ftes_syst_down;

  std::vector<tau> taus;
  
 public:
  explicit ditau_factory(TTree*);
  virtual ~ditau_factory() {}
  void set_process_all() { /* nothing to do */ }
  void run_factory();
  void handle_systematics(std::string);
  Int_t num_taus() { return taus.size(); }
  tau tau_at(unsigned i) { return taus.at(i); }
  // do I need to change this? because now we have 2?
  tau good_tau() { return taus.at(0); }
  std::vector<tau> all_taus() { return taus; }

};

// read data from tree Int_to member variables
// The branch address in quotes comes from the root file
ditau_factory::ditau_factory(TTree* input) {
  // Adding tau 1 variables
  input->SetBranchAddress("pt_1", &pt_1);
  input->SetBranchAddress("eta_1", &eta_1);
  input->SetBranchAddress("phi_1", &phi_1);
  input->SetBranchAddress("m_1", &m_1);
  input->SetBranchAddress("e_1", &e_1);
  input->SetBranchAddress("q_1", &q_1);
  input->SetBranchAddress("gen_match_1", &gen_match_1); 
  input->SetBranchAddress("t1ZTTGenPt", &tZTTGenPt_1);
  input->SetBranchAddress("t1ZTTGenEta", &tZTTGenEta_1);
  input->SetBranchAddress("t1ZTTGenPhi", &tZTTGenPhi_1);
  input->SetBranchAddress("t1_DecayMode", &decayMode_1);
  input->SetBranchAddress("DecayModeFinding_1", &dmf_1); 
  input->SetBranchAddress("DecayModeFindingNewDMs_1", &dmf_new_1); 

  // Original tau 2 variables
  input->SetBranchAddress("pt_2", &pt_2);
  input->SetBranchAddress("eta_2", &eta_2);
  input->SetBranchAddress("phi_2", &phi_2);
  input->SetBranchAddress("m_2", &m_2);
  input->SetBranchAddress("e_2", &e_2);
  input->SetBranchAddress("q_2", &q_2);
  input->SetBranchAddress("gen_match_2", &gen_match_2); 
  input->SetBranchAddress("t2ZTTGenPt", &tZTTGenPt_2);
  input->SetBranchAddress("t2ZTTGenEta", &tZTTGenEta_2);
  input->SetBranchAddress("t2ZTTGenPhi", &tZTTGenPhi_2);
  input->SetBranchAddress("t2_DecayMode", &decayMode_2);
  input->SetBranchAddress("DecayModeFinding_2", &dmf_2);
  input->SetBranchAddress("DecayModeFindingNewDMs_2", &dmf_new_2);

  // No branch in file for the following, are the NN vars?
  // input->SetBranchAddress("AgainstElectronTightMVA6", &againstElectronTightMVA6_2); 
  // input->SetBranchAddress("AgainstElectronVLooseMVA6", &againstElectronVLooseMVA6_2);
  // input->SetBranchAddress("AgainstMuonTight3", &againstMuonTight3_2); 
  // input->SetBranchAddress("AgainstMuonLoose3", &againstMuonLoose3_2); 
  // input->SetBranchAddress("tTightDeepTau2017v2p1VSe", &tTightDeepTau2017v2p1VSe);
  // input->SetBranchAddress("tVVLooseDeepTau2017v2p1VSe", &tVVLooseDeepTau2017v2p1VSe);
  // input->SetBranchAddress("tVVVLooseDeepTau2017v2p1VSe", &tVVVLooseDeepTau2017v2p1VSe);
  // input->SetBranchAddress("tTightDeepTau2017v2p1VSmu", &tTightDeepTau2017v2p1VSmu);
  // input->SetBranchAddress("tVLooseDeepTau2017v2p1VSmu", &tVLooseDeepTau2017v2p1VSmu);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTraw", &iso_2);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVLoose", &byVLooseIsolationMVArun2v1DBoldDMwLT_2);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTLoose", &byLooseIsolationMVArun2v1DBoldDMwLT_2);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTMedium", &byMediumIsolationMVArun2v1DBoldDMwLT_2);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTTight", &byTightIsolationMVArun2v1DBoldDMwLT_2);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVTight", &byVTightIsolationMVArun2v1DBoldDMwLT_2);
  // input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVVTight", &byVVTightIsolationMVArun2v1DBoldDMwLT_2);
  // input->SetBranchAddress("tDeepTau2017v2p1VSjetraw", &deepiso_2);
  // input->SetBranchAddress("tVVVLooseDeepTau2017v2p1VSjet", &tVVVLooseDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tVLooseDeepTau2017v2p1VSjet", &tVLooseDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tLooseDeepTau2017v2p1VSjet", &tLooseDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tMediumDeepTau2017v2p1VSjet", &tMediumDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tTightDeepTau2017v2p1VSjet", &tTightDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tVTightDeepTau2017v2p1VSjet", &tVTightDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tVVTightDeepTau2017v2p1VSjet", &tVVTightDeepTau2017v2p1VSjet);
  // input->SetBranchAddress("tes_syst_up", &tes_syst_up);
  // input->SetBranchAddress("tes_syst_down", &tes_syst_down);
  // input->SetBranchAddress("ftes_syst_up", &ftes_syst_up);
  // input->SetBranchAddress("ftes_syst_down", &ftes_syst_down);
}

// create electron object and set member data
void ditau_factory::run_factory() {
  // Set up first tau
  tau t1(pt_1, eta_1, phi_1, m_1, q_1);
  t1.setGenMatch(gen_match_1);
  // Outdated (i think)
  // t1.setRawMVAIso(iso_1_new);
  // t1.setRawDeepIso(deepiso_2);
  t1.setDecayMode(decayMode_1);
  t1.setDecayModeFinding(dmf_1);
  t1.setDecayModeFindingNew(dmf_new_1);
  t1.setGenPt(tZTTGenPt_1);
  t1.setGenEta(tZTTGenEta_1);
  t1.setGenPhi(tZTTGenPhi_1);
  // MVA is no longer used at all, so all of these need to be changed/eliminated at some point
  /*
  t1.setMVAIsoWPs(std::vector<Float_t>{
      byVLooseIsolationMVArun2v1DBoldDMwLT_2,
        byLooseIsolationMVArun2v1DBoldDMwLT_2,
        byMediumIsolationMVArun2v1DBoldDMwLT_2,
        byTightIsolationMVArun2v1DBoldDMwLT_2,
        byVTightIsolationMVArun2v1DBoldDMwLT_2
	});
  t1.setDeepIsoWPs(std::vector<Float_t>{
      tVVVLooseDeepTau2017v2p1VSjet,
        0,
        tVLooseDeepTau2017v2p1VSjet,
        tLooseDeepTau2017v2p1VSjet,
        tMediumDeepTau2017v2p1VSjet,
        tTightDeepTau2017v2p1VSjet,
        tVTightDeepTau2017v2p1VSjet,
        tVVTightDeepTau2017v2p1VSjet,
        0
	});
  t1.setMVAAgainstElectron(std::vector<Float_t>{
	againstElectronVLooseMVA6_2,
        0,
        0,
        againstElectronTightMVA6_2,
        0
	});
  t1.setMVAAgainstMuon(std::vector<Float_t>{
      0,
        againstMuonLoose3_2,
        0,
        againstMuonTight3_2,
        0
	});
  t1.setDeepAgainstElectron(std::vector<Float_t>{
      tVVVLooseDeepTau2017v2p1VSe,
        tVVLooseDeepTau2017v2p1VSe,
        0,
        0,
        0,
        tTightDeepTau2017v2p1VSe,
        0,
        0,
        0
	});
  t1.setDeepAgainstMuon(std::vector<Float_t>{
      0,
        0,
        tVLooseDeepTau2017v2p1VSmu,
        0,
        0,
        tTightDeepTau2017v2p1VSmu,
        0,
        0,
        0
	});
  */
  // Set up second tau
  tau t2(pt_2, eta_2, phi_2, m_2, q_2);
  t2.setGenMatch(gen_match_2);
  // Outdated (i think)
  // t2.setRawMVAIso();
  // t2.setRawDeepIso(deepiso_2);
  t2.setDecayMode(decayMode_2);
  t2.setDecayModeFinding(dmf_2);
  t2.setDecayModeFindingNew(dmf_new_2);
  t2.setGenPt(tZTTGenPt_2);
  t2.setGenEta(tZTTGenEta_2);
  t2.setGenPhi(tZTTGenPhi_2);
  // Don't have any of these
  /*
  t2.setMVAIsoWPs(std::vector<Float_t>{
      byVLooseIsolationMVArun2v1DBoldDMwLT_2,
        byLooseIsolationMVArun2v1DBoldDMwLT_2,
        byMediumIsolationMVArun2v1DBoldDMwLT_2,
        byTightIsolationMVArun2v1DBoldDMwLT_2,
        byVTightIsolationMVArun2v1DBoldDMwLT_2
	});
  t2.setDeepIsoWPs(std::vector<Float_t>{
      tVVVLooseDeepTau2017v2p1VSjet,
        0,
        tVLooseDeepTau2017v2p1VSjet,
        tLooseDeepTau2017v2p1VSjet,
        tMediumDeepTau2017v2p1VSjet,
        tTightDeepTau2017v2p1VSjet,
        tVTightDeepTau2017v2p1VSjet,
        tVVTightDeepTau2017v2p1VSjet,
        0
	});
  t2.setMVAAgainstElectron(std::vector<Float_t>{
      againstElectronVLooseMVA6_2,
        0,
        0,
        againstElectronTightMVA6_2,
        0
	});
  t2.setMVAAgainstMuon(std::vector<Float_t>{
      0,
        againstMuonLoose3_2,
        0,
        againstMuonTight3_2,
        0
	});
  t2.setDeepAgainstElectron(std::vector<Float_t>{
      tVVVLooseDeepTau2017v2p1VSe,
        tVVLooseDeepTau2017v2p1VSe,
        0,
        0,
        0,
        tTightDeepTau2017v2p1VSe,
        0,
        0,
        0
	});
  t2.setDeepAgainstMuon(std::vector<Float_t>{
      0,
        0,
        tVLooseDeepTau2017v2p1VSmu,
        0,
        0,
        tTightDeepTau2017v2p1VSmu,
        0,
        0,
        0
	});
  */
  // Add them both
  taus = { t1, t2 };
}

// Don't need to worry about this
void ditau_factory::handle_systematics(std::string syst) {
    double scale(1.);
    TLorentzVector new_tau;
    auto old_tau = taus.at(0);
    if (old_tau.getGenMatch() == 5 && (syst.substr(0, 3) == "DM0" || syst.substr(0, 3) == "DM1")) {
        scale = syst.find("Up") == std::string::npos ? tes_syst_up : tes_syst_down;
        new_tau.SetPtEtaPhiM(old_tau.getPt() * (1 + scale), old_tau.getEta(), old_tau.getPhi(), old_tau.getMass());
        taus.at(0).setP4(new_tau);
    } else if (old_tau.getGenMatch() < 5 && (syst.substr(0, 6) == "efaket" || syst.substr(0, 6) == "mfaket")) {
        scale = syst.find("Up") == std::string::npos ? ftes_syst_up : ftes_syst_down;
        new_tau.SetPtEtaPhiM(old_tau.getPt() * (1 + scale), old_tau.getEta(), old_tau.getPhi(), old_tau.getMass());
        taus.at(0).setP4(new_tau);
    }
}

#endif  // INCLUDE_FSA_DITAU_FACTORY_H_
