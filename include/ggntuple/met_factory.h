// Copyright [2020] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_MET_FACTORY_H_
#define INCLUDE_GGNTUPLE_MET_FACTORY_H_

#include <algorithm>
#include <string>
#include "TLorentzVector.h"
#include "TTree.h"

class met_factory {
 private:
    Float_t met, metphi;
    Float_t metSig, metcov00, metcov10, metcov11, metcov01;
    TLorentzVector p4;

 public:
    met_factory(TTree*, int, std::string);
    virtual ~met_factory() {}

    // getters
    Float_t getMet() { return met; }
    Float_t getMetPhi() { return metphi; }
    Float_t getMetSig() { return metSig; }
    Float_t getMetCov00() { return metcov00; }
    Float_t getMetCov10() { return metcov10; }
    Float_t getMetCov11() { return metcov11; }
    Float_t getMetCov01() { return metcov01; }
    TLorentzVector getP4();
};

// initialize member data and set TLorentzVector
met_factory::met_factory(TTree* input, int era, std::string syst) {
    input->SetBranchAddress("pfMET", &met);
    input->SetBranchAddress("pfMETPhi", &metphi);
    // input->SetBranchAddress("metSig", &metSig);
    input->SetBranchAddress("metcov00", &metcov00);
    input->SetBranchAddress("metcov10", &metcov10);
    input->SetBranchAddress("metcov11", &metcov11);
    input->SetBranchAddress("metcov01", &metcov01);
}

TLorentzVector met_factory::getP4() {
    p4.SetPtEtaPhiM(met, 0, metphi, 0);
    return p4;
}

#endif  // INCLUDE_GGNTUPLE_MET_FACTORY_H_
