// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_FSA_ELECTRON_FACTORY_H_
#define INCLUDE_FSA_ELECTRON_FACTORY_H_

#include <cmath>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"
#include "../models/electron.h"

class electron_factory {
 private:
    std::string syst;
    Int_t gen_match_1;
    Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1, eGenPt, eGenEta, eGenPhi, eGenEnergy;
    Float_t eCorrectedEt, eEnergyScaleUp, eEnergyScaleDown, eEnergySigmaUp, eEnergySigmaDown;
    std::vector<electron> electrons;

 public:
    electron_factory(TTree*, int, std::string);
    virtual ~electron_factory() {}
    void run_factory();
    Int_t num_electrons() { return electrons.size(); }
    electron electron_at(unsigned i) { return electrons.at(i); }
    electron good_electron() { return electrons.at(0); }
    std::vector<electron> all_electrons() { return electrons; }

};

// read data from tree into member variables
electron_factory::electron_factory(TTree* input, int era, std::string _syst) : syst(_syst) {
    input->SetBranchAddress("px_1", &px_1);
    input->SetBranchAddress("py_1", &py_1);
    input->SetBranchAddress("pz_1", &pz_1);
    input->SetBranchAddress("pt_1", &pt_1);
    input->SetBranchAddress("eta_1", &eta_1);
    input->SetBranchAddress("phi_1", &phi_1);
    input->SetBranchAddress("m_1", &m_1);
    input->SetBranchAddress("q_1", &q_1);
    input->SetBranchAddress("eRelPFIsoRho", &iso_1);
    input->SetBranchAddress("gen_match_1", &gen_match_1);
    input->SetBranchAddress("eGenPt", &eGenPt);
    input->SetBranchAddress("eGenEta", &eGenEta);
    input->SetBranchAddress("eGenPhi", &eGenPhi);
    input->SetBranchAddress("eGenEnergy", &eGenEnergy);
    input->SetBranchAddress("eCorrectedEt", &eCorrectedEt);
    input->SetBranchAddress("eEnergyScaleUp", &eEnergyScaleUp);
    input->SetBranchAddress("eEnergyScaleDown", &eEnergyScaleDown);
    input->SetBranchAddress("eEnergySigmaUp", &eEnergySigmaUp);
    input->SetBranchAddress("eEnergySigmaDown", &eEnergySigmaDown);
}

// create electron object and set member data
void electron_factory::run_factory() {
    electron el(pt_1, eta_1, phi_1, m_1, q_1);
    el.setGenMatch(gen_match_1);
    el.setIso(iso_1);
    el.setGenPt(eGenPt);
    el.setGenEta(eGenEta);
    el.setGenPhi(eGenPhi);
    el.setGenEnergy(eGenEnergy);

    // Electron energy scale systematics (eCorrectedEt is already applied so divide it out)
    if (syst == "EEScale_Up") {
        el.scaleP4(eEnergyScaleUp / eCorrectedEt);
    } else if (syst == "EEScale_Down") {
        el.scaleP4(eEnergyScaleDown / eCorrectedEt);
    } else if (syst == "EESigma_Up") {
        el.scaleP4(eEnergySigmaUp / eCorrectedEt);
    } else if (syst == "EESigma_Down") {
        el.scaleP4(eEnergySigmaDown / eCorrectedEt);
    }

    electrons = { el };
}

#endif  // INCLUDE_FSA_ELECTRON_FACTORY_H_
