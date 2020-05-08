// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_FSA_TAU_FACTORY_H_
#define INCLUDE_FSA_TAU_FACTORY_H_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"
#include "../models/tau.h"

class tau_factory {
 private:
    Int_t gen_match_2;
    Float_t px_2, py_2, pz_2, pt_2, eta_2, phi_2, m_2, e_2, iso_2, q_2, mt_2, tZTTGenPt, tZTTGenEta, tZTTGenPhi;
    Float_t againstElectronTightMVA6_2, againstElectronVLooseMVA6_2, againstMuonTight3_2, againstMuonLoose3_2, decayMode, dmf, dmf_new;
    Float_t byVLooseIsolationMVArun2v1DBoldDMwLT_2, byLooseIsolationMVArun2v1DBoldDMwLT_2, byMediumIsolationMVArun2v1DBoldDMwLT_2,
        byTightIsolationMVArun2v1DBoldDMwLT_2, byVTightIsolationMVArun2v1DBoldDMwLT_2, byVVTightIsolationMVArun2v1DBoldDMwLT_2;
    Float_t tTightDeepTau2017v2p1VSe, tVVLooseDeepTau2017v2p1VSe, tVVVLooseDeepTau2017v2p1VSe, tTightDeepTau2017v2p1VSmu, tVLooseDeepTau2017v2p1VSmu;
    Float_t tVVVLooseDeepTau2017v2p1VSjet, tVLooseDeepTau2017v2p1VSjet, tLooseDeepTau2017v2p1VSjet, tMediumDeepTau2017v2p1VSjet,
        tTightDeepTau2017v2p1VSjet, tVTightDeepTau2017v2p1VSjet, tVVTightDeepTau2017v2p1VSjet, deepiso_2;
        Float_t tes_syst_up, tes_syst_down, ftes_syst_up, ftes_syst_down;
    std::vector<tau> taus;

 public:
    explicit tau_factory(TTree*);
    virtual ~tau_factory() {}
    void set_process_all() { /* nothing to do */ }
    void run_factory();
    void handle_systematics(std::string);
    Int_t num_taus() { return taus.size(); }
    tau tau_at(unsigned i) { return taus.at(i); }
    tau good_tau() { return taus.at(0); }
    std::vector<tau> all_taus() { return taus; }

};

// read data from tree Int_to member variables
tau_factory::tau_factory(TTree* input) {
    input->SetBranchAddress("pt_2", &pt_2);
    input->SetBranchAddress("eta_2", &eta_2);
    input->SetBranchAddress("phi_2", &phi_2);
    input->SetBranchAddress("m_2", &m_2);
    input->SetBranchAddress("e_2", &e_2);
    input->SetBranchAddress("q_2", &q_2);
    input->SetBranchAddress("gen_match_2", &gen_match_2);
    input->SetBranchAddress("tZTTGenPt", &tZTTGenPt);
    input->SetBranchAddress("tZTTGenEta", &tZTTGenEta);
    input->SetBranchAddress("tZTTGenPhi", &tZTTGenPhi);
    input->SetBranchAddress("tDecayMode", &decayMode);
    input->SetBranchAddress("tDecayModeFinding", &dmf);
    input->SetBranchAddress("tDecayModeFindingNewDMs", &dmf_new);
    input->SetBranchAddress("tAgainstElectronTightMVA6", &againstElectronTightMVA6_2);
    input->SetBranchAddress("tAgainstElectronVLooseMVA6", &againstElectronVLooseMVA6_2);
    input->SetBranchAddress("tAgainstMuonTight3", &againstMuonTight3_2);
    input->SetBranchAddress("tAgainstMuonLoose3", &againstMuonLoose3_2);
    input->SetBranchAddress("tTightDeepTau2017v2p1VSe", &tTightDeepTau2017v2p1VSe);
    input->SetBranchAddress("tVVLooseDeepTau2017v2p1VSe", &tVVLooseDeepTau2017v2p1VSe);
    input->SetBranchAddress("tVVVLooseDeepTau2017v2p1VSe", &tVVVLooseDeepTau2017v2p1VSe);
    input->SetBranchAddress("tTightDeepTau2017v2p1VSmu", &tTightDeepTau2017v2p1VSmu);
    input->SetBranchAddress("tVLooseDeepTau2017v2p1VSmu", &tVLooseDeepTau2017v2p1VSmu);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTraw", &iso_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVLoose", &byVLooseIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTLoose", &byLooseIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTMedium", &byMediumIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTTight", &byTightIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVTight", &byVTightIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVVTight", &byVVTightIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tDeepTau2017v2p1VSjetraw", &deepiso_2);
    input->SetBranchAddress("tVVVLooseDeepTau2017v2p1VSjet", &tVVVLooseDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tVLooseDeepTau2017v2p1VSjet", &tVLooseDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tLooseDeepTau2017v2p1VSjet", &tLooseDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tMediumDeepTau2017v2p1VSjet", &tMediumDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tTightDeepTau2017v2p1VSjet", &tTightDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tVTightDeepTau2017v2p1VSjet", &tVTightDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tVVTightDeepTau2017v2p1VSjet", &tVVTightDeepTau2017v2p1VSjet);
    input->SetBranchAddress("tes_syst_up", &tes_syst_up);
    input->SetBranchAddress("tes_syst_down", &tes_syst_down);
    input->SetBranchAddress("ftes_syst_up", &ftes_syst_up);
    input->SetBranchAddress("ftes_syst_down", &ftes_syst_down);
}

// create electron object and set member data
void tau_factory::run_factory() {
    tau t(pt_2, eta_2, phi_2, m_2, q_2);
    t.setGenMatch(gen_match_2);
    t.setRawMVAIso(iso_2);
    t.setRawDeepIso(deepiso_2);
    t.setDecayMode(decayMode);
    t.setDecayModeFinding(dmf);
    t.setDecayModeFindingNew(dmf_new);
    t.setGenPt(tZTTGenPt);
    t.setGenEta(tZTTGenEta);
    t.setGenPhi(tZTTGenPhi);
    t.setMVAIsoWPs(std::vector<Float_t>{
        byVLooseIsolationMVArun2v1DBoldDMwLT_2,
        byLooseIsolationMVArun2v1DBoldDMwLT_2,
        byMediumIsolationMVArun2v1DBoldDMwLT_2,
        byTightIsolationMVArun2v1DBoldDMwLT_2,
        byVTightIsolationMVArun2v1DBoldDMwLT_2
    });
    t.setDeepIsoWPs(std::vector<Float_t>{
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
    t.setMVAAgainstElectron(std::vector<Float_t>{
        againstElectronVLooseMVA6_2,
        0,
        0,
        againstElectronTightMVA6_2,
        0
    });
    t.setMVAAgainstMuon(std::vector<Float_t>{
        0,
        againstMuonLoose3_2,
        0,
        againstMuonTight3_2,
        0
    });
    t.setDeepAgainstElectron(std::vector<Float_t>{
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
    t.setDeepAgainstMuon(std::vector<Float_t>{
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

    taus = { t };
}

void tau_factory::handle_systematics(std::string syst) {
    if (syst.substr(0, 3) == "DM0" || syst.substr(0, 3) == "DM1") {
        double scale(1.);
        TLorentzVector new_tau;
        auto old_tau = taus.at(0);
        if (old_tau.getGenMatch() == 5) {
            scale = syst.find("Up") == std::string::npos ? tes_syst_up : tes_syst_down;
        } else if (old_tau.getGenMatch() < 5) {
            scale = syst.find("Up") == std::string::npos ? ftes_syst_up : ftes_syst_down;
        }
        new_tau.SetPtEtaPhiM(old_tau.getPt() * (1 + scale), old_tau.getEta(), old_tau.getPhi(), old_tau.getMass());
        taus.at(0).setP4(new_tau);
    }
}

#endif  // INCLUDE_FSA_TAU_FACTORY_H_
