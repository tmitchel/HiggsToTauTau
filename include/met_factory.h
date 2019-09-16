// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_MET_FACTORY_H_
#define INCLUDE_MET_FACTORY_H_

#include <string>
#include "TTree.h"
#include "TLorentzVector.h"

class met_factory {
 private:
    Float_t met, metphi, met_py, met_px;
    Float_t metSig, metcov00, metcov10, metcov11, metcov01;
    TLorentzVector p4;

 public:
    met_factory(TTree*, int, std::string);
    virtual ~met_factory() {}

    // getters
    Float_t getMet() { return met; }
    Float_t getMetSig() { return metSig; }
    Float_t getMetCov00() { return metcov00; }
    Float_t getMetCov10() { return metcov10; }
    Float_t getMetCov11() { return metcov11; }
    Float_t getMetCov01() { return metcov01; }
    Float_t getMetPhi() { return metphi; }
    Float_t getMetPx() { return met_px; }
    Float_t getMetPy() { return met_py; }
    TLorentzVector getP4();
};

// initialize member data and set TLorentzVector
met_factory::met_factory(TTree* input, int era, std::string syst) {
    std::string met_name("met"), metphi_name("metphi");
    if (syst == "UncMet_Up") {
        met_name += "_UESUp";
        metphi_name += "_UESUp";
    } else if (syst == "UncMet_Down") {
        met_name += "_UESDown";
        metphi_name += "_UESDown";
    } else if (syst == "ClusteredMet_Up") {
        met_name += "_JESUp";
        metphi_name += "_JESUp";
    } else if (syst == "ClusteredMet_Down") {
        met_name += "_JESDown";
        metphi_name += "_JESDown";
    } else if (syst.find("Jet") != std::string::npos && (syst.find("Up") != std::string::npos || syst.find("Down") != std::string::npos)) {
        met_name += "_" + syst;
        metphi_name += "_" + syst;
    }

    input->SetBranchAddress(met_name.c_str(), &met);
    input->SetBranchAddress(metphi_name.c_str(), &metphi);
    input->SetBranchAddress("metSig", &metSig);
    input->SetBranchAddress("metcov00", &metcov00);
    input->SetBranchAddress("metcov10", &metcov10);
    input->SetBranchAddress("metcov11", &metcov11);
    input->SetBranchAddress("metcov01", &metcov01);
    input->SetBranchAddress("met_px", &met_px);
    input->SetBranchAddress("met_py", &met_py);
}

TLorentzVector met_factory::getP4() {
    p4.SetPtEtaPhiM(met, 0, metphi, 0);
    return p4;
}

#endif  // INCLUDE_MET_FACTORY_H_
