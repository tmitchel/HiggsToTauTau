// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_ELECTRON_FACTORY_H_
#define INCLUDE_GGNTUPLE_ELECTRON_FACTORY_H_

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "../models/electron.h"
#include "TLorentzVector.h"
#include "TTree.h"

class electron_factory {
 private:
    Int_t nEle, lepIndex, localIndex, n_good_electrons;
    Bool_t process_all;
    std::vector<Int_t> *charge;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *ch_iso, *pho_iso, *neu_iso, *pu_iso, *sc_eta, *id_mva_iso, *id_mva_noiso;
    std::vector<ULong64_t> *single_trig, *double_trig, *l1_trig;
    std::vector<UShort_t> *id;
    std::vector<electron> electrons;

 public:
    explicit electron_factory(TTree *);
    virtual ~electron_factory() {}
    void set_process_all() { process_all = true; }
    void run_factory();
    void handle_systematics(std::string);
    Int_t num_electrons() { return electrons.size(); }
    Int_t num_good_electrons() { return n_good_electrons; }
    electron electron_at(unsigned i) { return electrons.at(i); }
    electron good_electron() { return electrons.at(localIndex); }
    std::vector<electron> all_electrons() { return electrons; }
};

// read data from tree into member variables
electron_factory::electron_factory(TTree *input) : process_all(false) {
    input->SetBranchAddress("nEle", &nEle);
    input->SetBranchAddress("lepIndex", &lepIndex);
    input->SetBranchAddress("eleCharge", &charge);
    input->SetBranchAddress("elePt", &pt);
    input->SetBranchAddress("eleEta", &eta);
    input->SetBranchAddress("elePhi", &phi);
    input->SetBranchAddress("eleEn", &energy);
    input->SetBranchAddress("elePFChIso", &ch_iso);
    input->SetBranchAddress("elePFPhoIso", &pho_iso);
    input->SetBranchAddress("elePFNeuIso", &neu_iso);
    input->SetBranchAddress("elePFPUIso", &pu_iso);
    input->SetBranchAddress("eleFiredSingleTrgs", &single_trig);
    input->SetBranchAddress("eleFiredDoubleTrgs", &double_trig);
    input->SetBranchAddress("eleFiredL1Trgs", &l1_trig);
    input->SetBranchAddress("eleIDbit", &id);
    input->SetBranchAddress("eleSCEta", &sc_eta);
    input->SetBranchAddress("eleIDMVAIso", &id_mva_iso);
    input->SetBranchAddress("eleIDMVANoIso", &id_mva_noiso);
}

// create electron object and set member data
void electron_factory::run_factory() {
    Float_t iso(0.), id(0.);
    electrons.clear();
    for (unsigned i = 0; i < nEle; i++) {
        if (!process_all && i != lepIndex) {
            continue;
        }

        electron el(pt->at(i), eta->at(i), phi->at(i), 0.000511, charge->at(i));
        iso = (ch_iso->at(i) / pt->at(i)) + std::max(0., (neu_iso->at(i) + pho_iso->at(i) - 0.5 * pu_iso->at(i)) / pt->at(i));
        el.setIso(iso);
        id = (fabs(sc_eta->at(i)) <= 0.8 && id_mva_noiso->at(i) > 0.837 ||
              fabs(sc_eta->at(i)) > 0.8 && fabs(sc_eta->at(i)) <= 1.5 && id_mva_noiso->at(i) > 0.715 ||
              fabs(sc_eta->at(i)) >= 1.5 && id_mva_noiso->at(i) > 0.357);
        el.setID(id);
        // el.setGenMatch(gen_match_1);
        // el.setGenPt(eGenPt);
        // el.setGenEta(eGenEta);
        // el.setGenPhi(eGenPhi);
        // el.setGenEnergy(eGenEnergy);
        electrons.push_back(el);

        // check if extra electron for veto
        if (pt->at(i) >= 15 && fabs(eta->at(i)) < 2.5 &&
            (fabs(sc_eta->at(i)) <= 0.8 && id_mva_iso->at(i) > -0.83 ||
             fabs(sc_eta->at(i)) > 0.8 && fabs(sc_eta->at(i)) <= 1.5 && id_mva_iso->at(i) > -0.77 ||
             fabs(sc_eta->at(i)) >= 1.5 && id_mva_iso->at(i) > -0.69)) {
            n_good_electrons++;
        }
    }

    // change index if we don't process the full vector
    localIndex = process_all ? lepIndex : 0;
}

void electron_factory::handle_systematics(std::string syst) {
    // not implemented yet
}

#endif  // INCLUDE_GGNTUPLE_ELECTRON_FACTORY_H_
