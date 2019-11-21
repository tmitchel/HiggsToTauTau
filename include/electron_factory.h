// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_ELECTRON_FACTORY_H_
#define INCLUDE_ELECTRON_FACTORY_H_

#include <cmath>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"

class electron_factory;  // forward declare so it can befriend electrons

//////////////////////////////////////////////////
// Purpose: To hold a single composite electron //
//////////////////////////////////////////////////
class electron {
    friend electron_factory;

 private:
    std::string name = "electron";
    Int_t gen_match;
    Float_t pt, eta, phi, mass, charge, px, py, pz, iso, gen_pt, gen_eta, gen_phi, gen_energy;
    TLorentzVector p4;

 public:
    electron(Float_t, Float_t, Float_t, Float_t, Float_t);
    ~electron() {}

    // getters
    std::string getName() { return name; }
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIso() { return iso; }
    Int_t getGenMatch() { return gen_match; }
    Float_t getGenPt() { return gen_pt; }
    Float_t getGenEta() { return gen_eta; }
    Float_t getGenPhi() { return gen_phi; }
    Float_t getGenE() { return gen_energy; }
    Int_t getCharge() { return charge; }
};

// initialize member data and set TLorentzVector
electron::electron(Float_t Pt, Float_t Eta, Float_t Phi, Float_t M, Float_t Charge)
    : pt(Pt), eta(Eta), phi(Phi), mass(M), charge(Charge) {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

/////////////////////////////////////////////////
// Purpose: To build a collection of electrons //
// from the ntuple                             //
/////////////////////////////////////////////////
class electron_factory {
 private:
    std::string syst;
    Int_t gen_match_1;
    Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1, eGenPt, eGenEta, eGenPhi, eGenEnergy;
    Float_t eCorrectedEt, eEnergyScaleUp, eEnergyScaleDown, eEnergySigmaUp, eEnergySigmaDown;

 public:
    electron_factory(TTree*, int, std::string);
    virtual ~electron_factory() {}
    electron run_factory();
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
electron electron_factory::run_factory() {
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

#endif  // INCLUDE_ELECTRON_FACTORY_H_
