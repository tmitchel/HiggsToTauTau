// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_MUON_FACTORY_H_
#define INCLUDE_GGNTUPLE_MUON_FACTORY_H_

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "../models/muon.h"
#include "TLorentzVector.h"
#include "TTree.h"

class muon_factory {
   private:
    Bool_t process_all;
    Int_t nMu, lepIndex, localIndex, n_good_muons;
    std::vector<Int_t> *charge, *idBit;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *ch_iso, *pho_iso, *neu_iso, *pu_iso, *muD0, *muDz;
    std::vector<ULong64_t> *single_trig, *double_trig, *l1_trig;
    std::vector<muon> muons;

   public:
    explicit muon_factory(TTree *);
    virtual ~muon_factory() {}
    void set_process_all() { process_all = true; }
    void run_factory();
    Int_t num_muons() { return muons.size(); }
    Int_t num_good_muons() { return n_good_muons; }
    muon muon_at(unsigned i) { return muons.at(i); }
    muon good_muon() { return muons.at(localIndex); }
    std::vector<muon> all_muons() { return muons; }
};

// read data from tree into member variabl
muon_factory::muon_factory(TTree *input) : process_all(false) {
    input->SetBranchAddress("nMu", &nMu);
    input->SetBranchAddress("lepIndex", &lepIndex);
    input->SetBranchAddress("muCharge", &charge);
    input->SetBranchAddress("muPt", &pt);
    input->SetBranchAddress("muEta", &eta);
    input->SetBranchAddress("muPhi", &phi);
    input->SetBranchAddress("muEn", &energy);
    input->SetBranchAddress("muPFChIso", &ch_iso);
    input->SetBranchAddress("muPFPhoIso", &pho_iso);
    input->SetBranchAddress("muPFNeuIso", &neu_iso);
    input->SetBranchAddress("muPFPUIso", &pu_iso);
    input->SetBranchAddress("muIDbit", &idBit);
    input->SetBranchAddress("muD0", &muD0);
    input->SetBranchAddress("muDz", &muDz);
}

// create muon object and set member data
void muon_factory::run_factory() {
    Float_t iso(0.), id(0.);
    muons.clear();
    for (unsigned i = 0; i < nMu; i++) {
        if (!process_all && i != lepIndex) {
            continue;
        }

        muon mu(pt->at(i), eta->at(i), phi->at(i), 0.10565837, charge->at(i));
        iso = (ch_iso->at(i) / pt->at(i)) + std::max(0., (neu_iso->at(i) + pho_iso->at(i) - 0.5 * pu_iso->at(i)) / pt->at(i));
        mu.setIso(iso);
        id = (idBit->at(i) >> 2 & 1) && muD0->at(i) < 0.045 && muDz->at(i) < 0.2;
        mu.setID(id);
        // mu.setGenMatch(gen_match_1);
        // mu.setGenPt(eGenPt);
        // mu.setGenEta(eGenEta);
        // mu.setGenPhi(eGenPhi);
        // mu.setGenEnergy(eGenEnergy);
        muons.push_back(mu);

        // check if good muon for veto
        if (pt->at(i) > 15 && fabs(eta->at(i)) < 2.4 && (idBit->at(i) >> 2 & 1) && muD0->at(i) < 0.045 && muDz->at(i) < 0.2 && iso < 0.15) {
            n_good_muons++;
        }
    }

    // change index if we don't process the full vector
    localIndex = process_all ? lepIndex : 0;
}

#endif  // INCLUDE_GGNTUPLE_MUON_FACTORY_H_
