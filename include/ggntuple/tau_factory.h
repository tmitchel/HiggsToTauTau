// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_TAU_FACTORY_H_
#define INCLUDE_GGNTUPLE_TAU_FACTORY_H_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../models/tau.h"
#include "TLorentzVector.h"
#include "TTree.h"

class tau_factory {
   private:
    Bool_t process_all;
    Int_t nTau, tauIndex, localIndex;
    std::vector<Int_t> *charge, *decay_mode;
    std::vector<Bool_t> *decay_mode_finding, *decay_mode_finding_new;
    std::vector<Bool_t> *vloose_antiel_mva2v2_old, *loose_antiel_mva2v2_old, *medium_antiel_mva2v2_old, *tight_antiel_mva2v2_old,
        *vtight_antiel_mva2v2_old;
    std::vector<Bool_t> *loose_antimu_mva2v2_old, *tight_antimu_mva2v2_old;
    std::vector<Bool_t> *vloose_iso_mva2v2_old, *loose_iso_mva2v2_old, *medium_iso_mva2v2_old, *tight_iso_mva2v2_old, *vtight_iso_mva2v2_old;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *mass, *raw_iso_mva2v2_old;
    std::vector<tau> taus;

   public:
    explicit tau_factory(TTree *);
    virtual ~tau_factory() {}
    void set_process_all() { process_all = true; }
    void run_factory();
    Int_t num_taus() { return taus.size(); }
    tau tau_at(unsigned i) { return taus.at(i); }
    tau good_tau() { return taus.at(localIndex); }
    std::vector<tau> all_taus() { return taus; }
};

// read data from tree Int_to member variables
tau_factory::tau_factory(TTree *input)
    : process_all(false),
      pt(nullptr),
      eta(nullptr),
      phi(nullptr),
      energy(nullptr),
      charge(nullptr),
      mass(nullptr),
      decay_mode(nullptr),
      decay_mode_finding(nullptr),
      decay_mode_finding_new(nullptr),
      vloose_iso_mva2v2_old(nullptr),
      raw_iso_mva2v2_old(nullptr),
      loose_iso_mva2v2_old(nullptr),
      medium_iso_mva2v2_old(nullptr),
      tight_iso_mva2v2_old(nullptr),
      vtight_iso_mva2v2_old(nullptr),
      vloose_antiel_mva2v2_old(nullptr),
      loose_antiel_mva2v2_old(nullptr),
      medium_antiel_mva2v2_old(nullptr),
      tight_antiel_mva2v2_old(nullptr),
      vtight_antiel_mva2v2_old(nullptr),
      loose_antimu_mva2v2_old(nullptr),
      tight_antimu_mva2v2_old(nullptr) {
    input->SetBranchAddress("nTau", &nTau);
    input->SetBranchAddress("tauIndex", &tauIndex);
    input->SetBranchAddress("tauPt", &pt);
    input->SetBranchAddress("tauEta", &eta);
    input->SetBranchAddress("tauPhi", &phi);
    input->SetBranchAddress("tauEnergy", &energy);
    input->SetBranchAddress("tauCharge", &charge);
    input->SetBranchAddress("tauMass", &mass);
    input->SetBranchAddress("tauDecayMode", &decay_mode);
    input->SetBranchAddress("taupfTausDiscriminationByDecayModeFinding", &decay_mode_finding);
    input->SetBranchAddress("taupfTausDiscriminationByDecayModeFindingNewDMs", &decay_mode_finding_new);
    input->SetBranchAddress("tauByIsolationMVArun2v2DBoldDMwLTraw", &raw_iso_mva2v2_old);
    input->SetBranchAddress("tauByVLooseIsolationMVArun2v2DBoldDMwLT", &vloose_iso_mva2v2_old);
    input->SetBranchAddress("tauByLooseIsolationMVArun2v2DBoldDMwLT", &loose_iso_mva2v2_old);
    input->SetBranchAddress("tauByMediumIsolationMVArun2v2DBoldDMwLT", &medium_iso_mva2v2_old);
    input->SetBranchAddress("tauByTightIsolationMVArun2v2DBoldDMwLT", &tight_iso_mva2v2_old);
    input->SetBranchAddress("tauByVTightIsolationMVArun2v2DBoldDMwLT", &vtight_iso_mva2v2_old);
    input->SetBranchAddress("tauByMVA6VLooseElectronRejection", &vloose_antiel_mva2v2_old);
    input->SetBranchAddress("tauByMVA6LooseElectronRejection", &loose_antiel_mva2v2_old);
    input->SetBranchAddress("tauByMVA6MediumElectronRejection", &medium_antiel_mva2v2_old);
    input->SetBranchAddress("tauByMVA6TightElectronRejection", &tight_antiel_mva2v2_old);
    input->SetBranchAddress("tauByMVA6VTightElectronRejection", &vtight_antiel_mva2v2_old);
    input->SetBranchAddress("tauByLooseMuonRejection3", &loose_antimu_mva2v2_old);
    input->SetBranchAddress("tauByTightMuonRejection3", &tight_antimu_mva2v2_old);
}

// create electron object and set member data
void tau_factory::run_factory() {
    Float_t iso(0.), id(0.);
    taus.clear();
    for (unsigned i = 0; i < nTau; i++) {
        if (!process_all && i != tauIndex) {
            continue;
        }

        tau tt(pt->at(i), eta->at(i), phi->at(i), mass->at(i), charge->at(i));
        tt.setRawMVAIso(raw_iso_mva2v2_old->at(i));
        tt.setDecayMode(decay_mode->at(i));
        tt.setDecayModeFinding(decay_mode_finding->at(i));
        tt.setDecayModeFindingNew(decay_mode_finding_new->at(i));
        tt.setMVAIsoWPs(std::vector<Float_t>{
            vloose_iso_mva2v2_old->at(i),
            loose_iso_mva2v2_old->at(i),
            medium_iso_mva2v2_old->at(i),
            tight_iso_mva2v2_old->at(i),
            vtight_iso_mva2v2_old->at(i),
        });
        tt.setMVAAgainstElectron(std::vector<Float_t>{
            vloose_antiel_mva2v2_old->at(i),
            loose_antiel_mva2v2_old->at(i),
            medium_antiel_mva2v2_old->at(i),
            tight_antiel_mva2v2_old->at(i),
            vtight_antiel_mva2v2_old->at(i),
        });
        tt.setMVAAgainstMuon(std::vector<Float_t>{0, loose_antimu_mva2v2_old->at(i), 0, tight_antimu_mva2v2_old->at(i), 0});
        // tt.setGenMatch(gen_match_1);
        // tt.setGenPt(eGenPt);
        // tt.setGenEta(eGenEta);
        // tt.setGenPhi(eGenPhi);
        // tt.setGenEnergy(eGenEnergy);
        taus.push_back(tt);
    }

    // change index if we don't process the full vector
    localIndex = process_all ? tauIndex : 0;
}

#endif  // INCLUDE_GGNTUPLE_TAU_FACTORY_H_
