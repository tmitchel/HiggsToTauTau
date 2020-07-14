// Copyright [2020] Tyler Mitchell

#ifndef INCLUDE_MODELS_TAU_H_
#define INCLUDE_MODELS_TAU_H_

#include <string>
#include <vector>

#include "./defaults.h"
#include "TLorentzVector.h"

class tau {
   private:
    std::string name = "tau";
    Int_t gen_match;
    Float_t pt, eta, phi, mass, charge, decay_mode, dmf, dmf_new, iso, deepiso, gen_pt, gen_eta, gen_phi;
    Float_t VLooseIsoMVA, LooseIsoMVA, MediumIsoMVA, TightIsoMVA, VTightIsoMVA, VVTightIsoMVA;
    Float_t VVVLooseIsoDeep, VLooseIsoDeep, LooseIsoDeep, MediumIsoDeep, TightIsoDeep, VTightIsoDeep, VVTightIsoDeep;
    Float_t AgainstTightElectronMVA, AgainstVLooseElectronMVA, AgainstTightMuonMVA, AgainstLooseMuonMVA;
    Float_t AgainstTightElectronDeep, AgainstVVLooseElectronDeep, AgainstVVVLooseElectronDeep, AgainstTightMuonDeep, AgainstVLooseMuonDeep;
    std::vector<Float_t> mva_wps, deep_wps, mva_againstel_wps, mva_againstmu_wps, deep_againstel_wps, deep_againstmu_wps;

    TLorentzVector p4;

   public:
    tau(Float_t, Float_t, Float_t, Float_t, Float_t);
    ~tau() {}

    // getters
    std::string getName() { return name; }
    Int_t getGenMatch() { return gen_match; }
    Int_t getCharge() { return charge; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIso() { return iso; }
    Float_t getIsoWP(wps wp) { return mva_wps.at(wp); }
    Float_t getAgainstMuonMVAWP(wps wp) { return mva_againstmu_wps.at(wp); }
    Float_t getAgainstElectronMVAWP(wps wp) { return mva_againstel_wps.at(wp); }
    Float_t getDeepIso() { return deepiso; }
    Float_t getDeepIsoWP(wps wp) { return deep_wps.at(wp); }
    Float_t getAgainstMuonDeepWP(wps wp) { return deep_againstmu_wps.at(wp); }
    Float_t getAgainstElectronDeepWP(wps wp) { return deep_againstel_wps.at(wp); }
    Float_t getDecayMode() { return decay_mode; }
    Float_t getDecayModeFinding() { return dmf; }
    Float_t getDecayModeFindingNew() { return dmf_new; }
    Float_t getGenPt() { return gen_pt; }
    Float_t getGenEta() { return gen_eta; }
    Float_t getGenPhi() { return gen_phi; }

    TLorentzVector getP4() { return p4; }

    // setters
    void setGenMatch(Int_t _gen_match) { gen_match = _gen_match; }
    void setRawMVAIso(Float_t _iso) { iso = _iso; }
    void setRawDeepIso(Float_t _iso) { deepiso = _iso; }
    void setDecayMode(Float_t _decay_mode) { decay_mode = _decay_mode; }
    void setDecayModeFinding(Float_t _dmf) { dmf = _dmf; }
    void setDecayModeFindingNew(Float_t _dmf_new) { dmf_new = _dmf_new; }
    void setGenPt(Float_t _gen_pt) { gen_pt = _gen_pt; }
    void setGenEta(Float_t _gen_eta) { gen_eta = _gen_eta; }
    void setGenPhi(Float_t _gen_phi) { gen_phi = _gen_phi; }
    void setP4(TLorentzVector _p4) { p4 = _p4; }

    void setMVAIsoWPs(std::vector<Float_t>);
    void setDeepIsoWPs(std::vector<Float_t>);
    void setMVAAgainstElectron(std::vector<Float_t>);
    void setMVAAgainstMuon(std::vector<Float_t>);
    void setDeepAgainstElectron(std::vector<Float_t>);
    void setDeepAgainstMuon(std::vector<Float_t>);
};

void tau::setMVAIsoWPs(std::vector<Float_t> wps) {
    if (wps.size() < 5) {
        std::cerr << "MVA tau iso must be provided 5 working points" << std::endl;
    }
    for (unsigned i = 0; i < 5; i++) {
        mva_wps.at(i) = wps.at(i);
    }
}

void tau::setDeepIsoWPs(std::vector<Float_t> wps) {
    if (wps.size() < 9) {
        std::cerr << "Deep tau iso must be provided 9 working points" << std::endl;
    }
    for (unsigned i = 0; i < 9; i++) {
        deep_wps.at(i) = wps.at(i);
    }
}

void tau::setMVAAgainstElectron(std::vector<Float_t> wps) {
    if (wps.size() < 5) {
        std::cerr << "MVA tau iso must be provided 5 working points" << std::endl;
    }
    for (unsigned i = 0; i < 5; i++) {
        mva_againstel_wps.at(i) = wps.at(i);
    }
}

void tau::setMVAAgainstMuon(std::vector<Float_t> wps) {
    if (wps.size() < 5) {
        std::cerr << "MVA tau iso must be provided 5 working points" << std::endl;
    }
    for (unsigned i = 0; i < 5; i++) {
        mva_againstmu_wps.at(i) = wps.at(i);
    }
}

void tau::setDeepAgainstElectron(std::vector<Float_t> wps) {
    if (wps.size() < 9) {
        std::cerr << "Deep tau iso must be provided 9 working points" << std::endl;
    }
    for (unsigned i = 0; i < 9; i++) {
        deep_againstel_wps.at(i) = wps.at(i);
    }
}

void tau::setDeepAgainstMuon(std::vector<Float_t> wps) {
    if (wps.size() < 9) {
        std::cerr << "Deep tau iso must be provided 9 working points" << std::endl;
    }
    for (unsigned i = 0; i < 9; i++) {
        deep_againstmu_wps.at(i) = wps.at(i);
    }
}

// initialize member data and set TLorentzVector
tau::tau(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _mass, Float_t _charge)
    : pt(_pt),
      eta(_eta),
      phi(_phi),
      mass(_mass),
      charge(_charge),
      deep_wps{0, 0, 0, 0, 0, 0, 0, 0, 0},
      deep_againstel_wps{0, 0, 0, 0, 0, 0, 0, 0, 0},
      deep_againstmu_wps{0, 0, 0, 0, 0, 0, 0, 0, 0},
      mva_wps{0, 0, 0, 0, 0},
      mva_againstel_wps{0, 0, 0, 0, 0},
      mva_againstmu_wps{0, 0, 0, 0, 0} {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INCLUDE_MODELS_TAU_H_
