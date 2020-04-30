// Copyright [2020] Tyler Mitchell

#ifndef INCLUDE_MODELS_ELECTRON_H_
#define INCLUDE_MODELS_ELECTRON_H_

#include <string>
#include "TLorentzVector.h"

class electron {
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
    Int_t getCharge() { return charge; }
    Int_t getGenMatch() { return gen_match; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIso() { return iso; }
    Float_t getGenPt() { return gen_pt; }
    Float_t getGenEta() { return gen_eta; }
    Float_t getGenPhi() { return gen_phi; }
    Float_t getGenE() { return gen_energy; }
    TLorentzVector getP4() { return p4; }

    // setters
    void setGenMatch(Int_t _gen_match) { gen_match = _gen_match; }
    void setIso(Float_t _iso) { iso = _iso; }
    void setGenPt(Float_t _gen_pt) { gen_pt = _gen_pt; }
    void setGenEta(Float_t _gen_eta) { gen_eta = _gen_eta; }
    void setGenPhi(Float_t _gen_phi) { gen_phi = _gen_phi; }
    void setGenEnergy(Float_t _gen_energy) { gen_energy = _gen_energy; }
    void scaleP4(Float_t sf) { p4 *= sf; }
};

// initialize member data and set TLorentzVector
electron::electron(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _mass, Float_t _charge)
    : pt(_pt), eta(_eta), phi(_phi), mass(_mass), charge(_charge) {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INCLUDE_MODELS_ELECTRON_H_
