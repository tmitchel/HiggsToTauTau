// Copyright [2020] Tyler Mitchell

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
    Int_t gen_match_1, gen_match_2;
    Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, iso_1, q_1;
    Float_t px_2, py_2, pz_2, pt_2, eta_2, phi_2, m_2, e_2, iso_2, q_2;
    std::vector<tau> taus;

 public:
    explicit ditau_factory(TTree*);
    virtual ~ditau_factory() {}
    void set_process_all() { /* nothing to do */ }
    void run_factory();
    void handle_systematics(std::string);
    Int_t num_taus() { return taus.size(); }
    tau tau_at(unsigned i) { return taus.at(i); }
    tau good_tau() { return taus.at(0); }
    std::vector<tau> all_taus() { return taus; }

};

// read data from tree Int_to member variables
ditau_factory::ditau_factory(TTree* input) {
    input->SetBranchAddress("pt_1", &pt_1);
    input->SetBranchAddress("eta_1", &eta_1);
    input->SetBranchAddress("phi_1", &phi_1);
    input->SetBranchAddress("m_1", &m_1);
    input->SetBranchAddress("e_1", &e_1);
    input->SetBranchAddress("q_1", &q_1);
    input->SetBranchAddress("gen_match_1", &gen_match_1);

    input->SetBranchAddress("pt_2", &pt_2);
    input->SetBranchAddress("eta_2", &eta_2);
    input->SetBranchAddress("phi_2", &phi_2);
    input->SetBranchAddress("m_2", &m_2);
    input->SetBranchAddress("e_2", &e_2);
    input->SetBranchAddress("q_2", &q_2);
    input->SetBranchAddress("gen_match_2", &gen_match_2);
}

// create electron object and set member data
void ditau_factory::run_factory() {
    // create lead tau
    tau t1(pt_1, eta_1, phi_1, m_1, q_1);
    t1.setGenMatch(gen_match_1);
    t1.setRawMVAIso(iso_1);

    // create sub-lead tau
    tau t2(pt_2, eta_2, phi_2, m_2, q_2);
    t2.setGenMatch(gen_match_2);
    t2.setRawMVAIso(iso_2);
    // t.setRawDeepIso(deepiso_2);
    // t.setDecayMode(decayMode);
    // t.setDecayModeFinding(dmf);
    // t.setDecayModeFindingNew(dmf_new);
    // t.setGenPt(tZTTGenPt);
    // t.setGenEta(tZTTGenEta);
    // t.setGenPhi(tZTTGenPhi);
    // t.setMVAIsoWPs(std::vector<Float_t>{
    //     byVLooseIsolationMVArun2v1DBoldDMwLT_2,
    //     byLooseIsolationMVArun2v1DBoldDMwLT_2,
    //     byMediumIsolationMVArun2v1DBoldDMwLT_2,
    //     byTightIsolationMVArun2v1DBoldDMwLT_2,
    //     byVTightIsolationMVArun2v1DBoldDMwLT_2
    // });
    // t.setDeepIsoWPs(std::vector<Float_t>{
    //     tVVVLooseDeepTau2017v2p1VSjet,
    //     0,
    //     tVLooseDeepTau2017v2p1VSjet,
    //     tLooseDeepTau2017v2p1VSjet,
    //     tMediumDeepTau2017v2p1VSjet,
    //     tTightDeepTau2017v2p1VSjet,
    //     tVTightDeepTau2017v2p1VSjet,
    //     tVVTightDeepTau2017v2p1VSjet,
    //     0
    // });
    // t.setMVAAgainstElectron(std::vector<Float_t>{
    //     againstElectronVLooseMVA6_2,
    //     0,
    //     0,
    //     againstElectronTightMVA6_2,
    //     0
    // });
    // t.setMVAAgainstMuon(std::vector<Float_t>{
    //     0,
    //     againstMuonLoose3_2,
    //     0,
    //     againstMuonTight3_2,
    //     0
    // });
    // t.setDeepAgainstElectron(std::vector<Float_t>{
    //     tVVVLooseDeepTau2017v2p1VSe,
    //     tVVLooseDeepTau2017v2p1VSe,
    //     0,
    //     0,
    //     0,
    //     tTightDeepTau2017v2p1VSe,
    //     0,
    //     0,
    //     0
    // });
    // t.setDeepAgainstMuon(std::vector<Float_t>{
    //     0,
    //     0,
    //     tVLooseDeepTau2017v2p1VSmu,
    //     0,
    //     0,
    //     tTightDeepTau2017v2p1VSmu,
    //     0,
    //     0,
    //     0
    // });

    taus = { t1, t2 };
}

void ditau_factory::handle_systematics(std::string syst) {
    // double scale(1.);
    // TLorentzVector new_tau;
    // auto old_tau = taus.at(0);
    // if (old_tau.getGenMatch() == 5 && (syst.substr(0, 3) == "DM0" || syst.substr(0, 3) == "DM1")) {
    //     scale = syst.find("Up") == std::string::npos ? tes_syst_up : tes_syst_down;
    //     new_tau.SetPtEtaPhiM(old_tau.getPt() * (1 + scale), old_tau.getEta(), old_tau.getPhi(), old_tau.getMass());
    //     taus.at(0).setP4(new_tau);
    // } else if (old_tau.getGenMatch() < 5 && (syst.substr(0, 6) == "efaket" || syst.substr(0, 6) == "mfaket")) {
    //     scale = syst.find("Up") == std::string::npos ? ftes_syst_up : ftes_syst_down;
    //     new_tau.SetPtEtaPhiM(old_tau.getPt() * (1 + scale), old_tau.getEta(), old_tau.getPhi(), old_tau.getMass());
    //     taus.at(0).setP4(new_tau);
    // }
}

#endif  // INCLUDE_FSA_DITAU_FACTORY_H_
