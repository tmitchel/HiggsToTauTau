// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_ELECTRON_FACTORY_GG_H_
#define INCLUDE_ELECTRON_FACTORY_GG_H_

#include <cmath>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"
#include "models/electron.h"

class electron_factory_gg {
 private:
    std::string syst;
    Int_t gen_match_1;
    Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1, eGenPt, eGenEta, eGenPhi, eGenEnergy;
    Float_t eCorrectedEt, eEnergyScaleUp, eEnergyScaleDown, eEnergySigmaUp, eEnergySigmaDown;

 public:
    electron_factory_gg(TTree*, int, std::string);
    virtual ~electron_factory_gg() {}
    electron run_factory();
};

// read data from tree into member variables
electron_factory_gg::electron_factory_gg(TTree* input, int era, std::string _syst) : syst(_syst) {
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
electron electron_factory_gg::run_factory() {
    electron el(pt_1, eta_1, phi_1, m_1, q_1);
    el.px = px_1;
    el.py = py_1;
    el.pz = pz_1;
    el.iso = iso_1;
    el.gen_match = gen_match_1;
    el.gen_pt = eGenPt;
    el.gen_eta = eGenEta;
    el.gen_phi = eGenPhi;
    el.gen_energy = eGenEnergy;

    // Electron energy scale systematics (eCorrectedEt is already applied so divide it out)
    if (syst == "EEScale_Up") {
        el.p4 *= eEnergyScaleUp / eCorrectedEt;
    } else if (syst == "EEScale_Down") {
        el.p4 *= eEnergyScaleDown / eCorrectedEt;
    } else if (syst == "EESigma_Up") {
        el.p4 *= eEnergySigmaUp / eCorrectedEt;
    } else if (syst == "EESigma_Down") {
        el.p4 *= eEnergySigmaDown / eCorrectedEt;
    }

    return el;
}

#endif  // INCLUDE_ELECTRON_FACTORY_GG_H_
