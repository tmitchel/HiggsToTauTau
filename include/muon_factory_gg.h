// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_MUON_FACTORY_GG_H_
#define INCLUDE_MUON_FACTORY_GG_H_

#include <cmath>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"
#include "models/muon.h"

class muon_factory_gg {
 private:
    std::string syst;
    Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1, mediumID, mGenPt, mGenEta, mGenPhi,
        mGenEnergy;
    Int_t gen_match_1;

 public:
    muon_factory_gg(TTree*, int, std::string);
    virtual ~muon_factory_gg() {}
    muon run_factory();
};

// read data from tree into member variabl
muon_factory_gg::muon_factory_gg(TTree* input, int era, std::string _syst) : syst(_syst) {
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
muon muon_factory_gg::run_factory() {
    muon mu(pt_1, eta_1, phi_1, m_1, q_1);
    mu.setGenMatch(gen_match_1);
    mu.setIso(iso_1);
    mu.setMediumID(mediumID);
    mu.setGenPt(mGenPt);
    mu.setGenEta(mGenEta);
    mu.setGenPhi(mGenPhi);
    mu.setGenEnergy(mGenEnergy);

    return mu;
}

#endif  // INCLUDE_MUON_FACTORY_GG_H_
