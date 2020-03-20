// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_MET_FACTORY_H_
#define INCLUDE_MET_FACTORY_H_

#include <algorithm>
#include <string>
#include "TLorentzVector.h"
#include "TTree.h"

class met_factory {
   private:
    Float_t met, metphi, met_py, met_px;
    Float_t metSig, metcov00, metcov10, metcov11, metcov01;
    TLorentzVector p4;
    std::unordered_map<std::string, std::string> syst_name_map;

   public:
    met_factory(TTree*, int, std::string);
    virtual ~met_factory() {}
    std::string fix_syst_string(std::string);

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
met_factory::met_factory(TTree* input, int era, std::string syst)
    : syst_name_map{
          {"UncMet_Up", "UESUp"},
          {"UncMet_Down", "UESDown"},
          {"JetRelBal_Up", "JetRelativeBalUp"},
          {"JetRelBal_Down", "JetRelativeBalDown"},
          {"JetJER_Up", "JERUp"},
          {"JetJER_Down", "JERDown"},
      } {
    std::string met_name("met"), metphi_name("metphi");
    auto end = std::string::npos;
    if (syst.find("Jet") != end && (syst.find("Up") != end || syst.find("Down") != end)) {
        syst.erase(std::remove(syst.begin(), syst.end(), '_'), syst.end());
        std::string syst_name = syst;
        syst_name.erase(0, 3);
        met_name += "_" + syst_name;
        metphi_name += "_" + syst_name;
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

std::string met_factory::fix_syst_string(std::string syst) {
    auto formatted(syst);
    auto end = std::string::npos;
    if (syst.find("JetRel") != end || syst.find("JetJER") != end || syst.find("UncMet") != end) {
        return "_" + syst_name_map[syst];
    } else {
        auto pos = syst.find("_Up");
        if (pos != end) {
            formatted.replace(pos, 3, "Up");
        }
        pos = syst.find("_Down");
        if (pos != end) {
            formatted.replace(pos, 5, "Down");
        }
    }
    return formatted;
}

TLorentzVector met_factory::getP4() {
    p4.SetPtEtaPhiM(met, 0, metphi, 0);
    return p4;
}

#endif  // INCLUDE_MET_FACTORY_H_
