// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_MUON_FACTORY_H_
#define INCLUDE_MUON_FACTORY_H_

#include <cmath>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"

class muon_factory;  // forward declare so it can befriend muons

//////////////////////////////////////////////
// Purpose: To hold a single composite muon //
//////////////////////////////////////////////
class muon {
    friend muon_factory;

 private:
    std::string name = "muon";
    Float_t pt, eta, phi, mass, charge, px, py, pz, iso, gen_match, mediumID, gen_pt, gen_eta, gen_phi, gen_energy;
    TLorentzVector p4;

 public:
    muon(Float_t, Float_t, Float_t, Float_t, Float_t);
    ~muon() {}

    // getters
    std::string getName() { return name; }
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIso() { return iso; }
    Float_t getGenMatch() { return gen_match; }
    Float_t getGenPt() { return gen_pt; }
    Float_t getGenEta() { return gen_eta; }
    Float_t getGenPhi() { return gen_phi; }
    Float_t getGenE() { return gen_energy; }
    Float_t getMediumID() { return mediumID; }
    Int_t getCharge() { return charge; }
};

// initialize member data and set TLorentzVector
muon::muon(Float_t Pt, Float_t Eta, Float_t Phi, Float_t M, Float_t Charge)
    : pt(Pt), eta(Eta), phi(Phi), mass(M), charge(Charge) {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

/////////////////////////////////////////////
// Purpose: To build a collection of muons //
// from the ntuple                         //
/////////////////////////////////////////////
class muon_factory {
 private:
    std::string syst;
    Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1, mediumID, mGenPt, mGenEta, mGenPhi,
        mGenEnergy;
    Int_t gen_match_1;

 public:
    muon_factory(TTree*, int, std::string);
    virtual ~muon_factory() {}
    muon run_factory();
};

// read data from tree into member variabl
muon_factory::muon_factory(TTree* input, int era, std::string _syst) : syst(_syst) {
    input->SetBranchAddress("px_1", &px_1);
    input->SetBranchAddress("py_1", &py_1);
    input->SetBranchAddress("pz_1", &pz_1);
    input->SetBranchAddress("pt_1", &pt_1);
    input->SetBranchAddress("eta_1", &eta_1);
    input->SetBranchAddress("phi_1", &phi_1);
    input->SetBranchAddress("m_1", &m_1);
    input->SetBranchAddress("q_1", &q_1);
    input->SetBranchAddress("mRelPFIsoDBDefault", &iso_1);
    input->SetBranchAddress("gen_match_1", &gen_match_1);
    input->SetBranchAddress("mPFIDMedium", &mediumID);
    input->SetBranchAddress("mGenPt", &mGenPt);
    input->SetBranchAddress("mGenEta", &mGenEta);
    input->SetBranchAddress("mGenPhi", &mGenPhi);
    input->SetBranchAddress("mGenEnergy", &mGenEnergy);
}

// create muon object and set member data
muon muon_factory::run_factory() {
    muon mu(pt_1, eta_1, phi_1, m_1, q_1);
    mu.px = px_1;
    mu.py = py_1;
    mu.pz = pz_1;
    mu.iso = iso_1;
    mu.gen_match = gen_match_1;
    mu.mediumID = mediumID;
    mu.gen_pt = mGenPt;
    mu.gen_eta = mGenEta;
    mu.gen_phi = mGenPhi;
    mu.gen_energy = mGenEnergy;

    // muon energy scale systematics
    if (syst.find("MES") != std::string::npos) {
        int dir(1);
        if (syst == "MES_Down") {
            dir = -1;
        }
        if (mu.getEta() < -2.1) {
            mu.p4 *= dir * 1.027;
        } else if (mu.getEta() < -1.2) {
            mu.p4 *= dir * 1.009;
        } else if (mu.getEta() < 1.2) {
            mu.p4 *= dir * 1.004;
        } else if (mu.getEta() < 2.1) {
            mu.p4 *= dir * 1.009;
        } else {
            mu.p4 *= dir * 1.017;
        }
    }

    return mu;
}

#endif  // INCLUDE_MUON_FACTORY_H_
