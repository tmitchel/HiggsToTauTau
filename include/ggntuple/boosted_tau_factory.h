// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_BOOSTED_TAU_FACTORY_H_
#define INCLUDE_GGNTUPLE_BOOSTED_TAU_FACTORY_H_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../models/tau.h"
#include "TLorentzVector.h"
#include "TTree.h"

class boosted_tau_factory {
   private:
    Bool_t process_all;
    Int_t nTau, tauIndex, localIndex;
    std::vector<Int_t> *charge, *decay_mode;
    std::vector<Bool_t> *decay_mode_finding, *decay_mode_finding_new;
    std::vector<Bool_t> *vloose_antiel_mva2v1_old, *loose_antiel_mva2v1_old, *medium_antiel_mva2v1_old, *tight_antiel_mva2v1_old,
        *vtight_antiel_mva2v1_old;
    std::vector<Bool_t> *loose_antimu_mva2v1_old, *tight_antimu_mva2v1_old;
    std::vector<Bool_t> *vloose_iso_mva2v1_old, *loose_iso_mva2v1_old, *medium_iso_mva2v1_old, *tight_iso_mva2v1_old, *vtight_iso_mva2v1_old;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *mass, *raw_iso_mva2v1_old;
    std::vector<tau> taus;

   public:
    explicit boosted_tau_factory(TTree *);
    virtual ~boosted_tau_factory() {}
    void set_process_all() { process_all = true; }
    void run_factory();
    Int_t num_taus() { return taus.size(); }
    tau tau_at(unsigned i) { return taus.at(i); }
    tau good_tau() { return taus.at(localIndex); }
    std::vector<tau> all_taus() { return taus; }
};

// read data from tree Int_to member variables
boosted_tau_factory::boosted_tau_factory(TTree *input)
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
      raw_iso_mva2v1_old(nullptr),
      vloose_iso_mva2v1_old(nullptr),
      loose_iso_mva2v1_old(nullptr),
      medium_iso_mva2v1_old(nullptr),
      tight_iso_mva2v1_old(nullptr),
      vtight_iso_mva2v1_old(nullptr),
      vloose_antiel_mva2v1_old(nullptr),
      loose_antiel_mva2v1_old(nullptr),
      medium_antiel_mva2v1_old(nullptr),
      tight_antiel_mva2v1_old(nullptr),
      vtight_antiel_mva2v1_old(nullptr),
      loose_antimu_mva2v1_old(nullptr),
      tight_antimu_mva2v1_old(nullptr) {
    input->SetBranchAddress("nBoostedTau", &nTau);
    input->SetBranchAddress("tauIndex", &tauIndex);
    input->SetBranchAddress("boostedTauPt", &pt);
    input->SetBranchAddress("boostedTauEta", &eta);
    input->SetBranchAddress("boostedTauPhi", &phi);
    input->SetBranchAddress("boostedTauEnergy", &energy);
    input->SetBranchAddress("boostedTauCharge", &charge);
    input->SetBranchAddress("boostedTauMass", &mass);
    input->SetBranchAddress("boostedTauDecayMode", &decay_mode);
    input->SetBranchAddress("boostedTaupfTausDiscriminationByDecayModeFinding", &decay_mode_finding);
    input->SetBranchAddress("boostedTaupfTausDiscriminationByDecayModeFindingNewDMs", &decay_mode_finding_new);
    input->SetBranchAddress("boostedTauByIsolationMVArun2v1DBoldDMwLTraw", &raw_iso_mva2v1_old);
    input->SetBranchAddress("boostedTauByVLooseIsolationMVArun2v1DBoldDMwLT", &vloose_iso_mva2v1_old);
    input->SetBranchAddress("boostedTauByLooseIsolationMVArun2v1DBoldDMwLT", &loose_iso_mva2v1_old);
    input->SetBranchAddress("boostedTauByMediumIsolationMVArun2v1DBoldDMwLT", &medium_iso_mva2v1_old);
    input->SetBranchAddress("boostedTauByTightIsolationMVArun2v1DBoldDMwLT", &tight_iso_mva2v1_old);
    input->SetBranchAddress("boostedTauByVTightIsolationMVArun2v1DBoldDMwLT", &vtight_iso_mva2v1_old);
    input->SetBranchAddress("boostedTauByMVA6VLooseElectronRejection", &vloose_antiel_mva2v1_old);
    input->SetBranchAddress("boostedTauByMVA6LooseElectronRejection", &loose_antiel_mva2v1_old);
    input->SetBranchAddress("boostedTauByMVA6MediumElectronRejection", &medium_antiel_mva2v1_old);
    input->SetBranchAddress("boostedTauByMVA6TightElectronRejection", &tight_antiel_mva2v1_old);
    input->SetBranchAddress("boostedTauByMVA6VTightElectronRejection", &vtight_antiel_mva2v1_old);
    input->SetBranchAddress("boostedTauByLooseMuonRejection3", &loose_antimu_mva2v1_old);
    input->SetBranchAddress("boostedTauByTightMuonRejection3", &tight_antimu_mva2v1_old);
}

// create electron object and set member data
void boosted_tau_factory::run_factory() {
    Float_t iso(0.), id(0.);
    taus.clear();
    for (unsigned i = 0; i < nTau; i++) {
        if (!process_all && i != tauIndex) {
            continue;
        }

        tau tt(pt->at(i), eta->at(i), phi->at(i), mass->at(i), charge->at(i));
        tt.setRawMVAIso(raw_iso_mva2v1_old->at(i));
        tt.setDecayMode(decay_mode->at(i));
        tt.setDecayModeFinding(decay_mode_finding->at(i));
        tt.setDecayModeFindingNew(decay_mode_finding_new->at(i));
        tt.setMVAIsoWPs(std::vector<Float_t>{
            static_cast<float>(vloose_iso_mva2v1_old->at(i)),
            static_cast<float>(loose_iso_mva2v1_old->at(i)),
            static_cast<float>(medium_iso_mva2v1_old->at(i)),
            static_cast<float>(tight_iso_mva2v1_old->at(i)),
            static_cast<float>(vtight_iso_mva2v1_old->at(i)),
        });
        tt.setMVAAgainstElectron(std::vector<Float_t>{
            static_cast<float>(vloose_antiel_mva2v1_old->at(i)),
            0.,
            0.,
            static_cast<float>(tight_antiel_mva2v1_old->at(i)),
            0.,
        });
        tt.setMVAAgainstMuon(
            std::vector<Float_t>{0, static_cast<float>(loose_antimu_mva2v1_old->at(i)), 0, static_cast<float>(tight_antimu_mva2v1_old->at(i)), 0});
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

#endif  // INCLUDE_GGNTUPLE_BOOSTED_TAU_FACTORY_H_
