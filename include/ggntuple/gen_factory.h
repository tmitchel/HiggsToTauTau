// Copyright 2020 Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_GEN_FACTORY_H_
#define INCLUDE_GGNTUPLE_GEN_FACTORY_H_

#include <vector>

#include "../models/gen_particle.h"
#include "TLorentzVector.h"
#include "TTree.h"

enum DY { ZTT, ZL, ZJ };

class gen_factory {
 private:
    Int_t nMC, numGenTau;
    Int_t top_idx, topbar_idx;
    std::vector<Int_t> *pid, *status;
    std::vector<UShort_t> *status_flag;
    std::vector<Float_t> *pt, *eta, *phi, *mass;
    std::vector<Float_t> *taudaugPt, *taudaugEta, *taudaugPhi, *taudaugMass;
    std::vector<gen> gen_collection, tau_daughters, leptons;

 public:
    explicit gen_factory(TTree *);
    ~gen_factory() {}
    void run_factory();

    Int_t num_gen_particles() { return gen_collection.size(); }
    DY DY_process(TLorentzVector);
    TLorentzVector getTop() { return top_idx > -1 ? gen_collection.at(top_idx).getP4() : TLorentzVector(); }
    TLorentzVector getAntiTop() { return topbar_idx > -1 ? gen_collection.at(topbar_idx).getP4() : TLorentzVector(); }
};

gen_factory::gen_factory(TTree *input) {
    input->SetBranchAddress("nMC", &nMC);
    input->SetBranchAddress("mcPt", &pt);
    input->SetBranchAddress("mcEta", &eta);
    input->SetBranchAddress("mcPhi", &phi);
    input->SetBranchAddress("mcMass", &mass);
    input->SetBranchAddress("mcPID", &pid);
    input->SetBranchAddress("mcStatus", &status);
    input->SetBranchAddress("mcStatusFlag", &status_flag);
    input->SetBranchAddress("numGenTau", &numGenTau);
    input->SetBranchAddress("taudaugPt", &taudaugPt);
    input->SetBranchAddress("taudaugEta", &taudaugEta);
    input->SetBranchAddress("taudaugPhi", &taudaugPhi);
    input->SetBranchAddress("taudaugMass", &taudaugMass);
}

void gen_factory::run_factory() {
    gen_collection.clear();
    tau_daughters.clear();
    leptons.clear();
    top_idx = -1;
    topbar_idx = -1;

    for (auto i = 0; i < nMC; i++) {
        auto g = gen(pt->at(i), eta->at(i), phi->at(i), mass->at(i));
        g.setPID(pid->at(i));
        g.setStatus(status_flag->at(i));
        gen_collection.push_back(g);

        // check if a top or anti-top
        if (status->at(i) == 62) {
            if (pid->at(i) == 6) {
                top_idx = i;
            } else if (pid->at(i) == -6) {
                topbar_idx = i;
            }
        }

        // check if a lepton
        if (pid->at(i) > 8 && pid->at(i) < 17) {
            leptons.push_back(g);
        }
    }

    for (auto i = 0; i < numGenTau; i++) {
        auto g = gen(taudaugPt->at(i), taudaugEta->at(i), taudaugPhi->at(i), taudaugMass->at(i));
        tau_daughters.push_back(g);
    }
}

DY gen_factory::DY_process(TLorentzVector reco_tau) {
    // check if ZL event
    for (auto g : leptons) {
        // need gen matched to reco tau
        if (reco_tau.DeltaR(g.getP4()) > 0.2) {
            continue;
        }

        // prompt or non-prompt electron/muon
        if (g.getPt() > 8 && (g.getPID() == 11 || g.getPID() == 13) && (g.getStatus(9) || g.getStatus(10))) {
            return DY::ZL;
        }
    }

    // check if ZTT event
    for (auto g : tau_daughters) {
        if (reco_tau.DeltaR(g.getP4()) < 0.2 && g.getPt() > 15) {
            return DY::ZTT;
        }
    }

    // must be ZJ
    return DY::ZJ;
}


#endif  // INCLUDE_GGNTUPLE_GEN_FACTORY_H_
