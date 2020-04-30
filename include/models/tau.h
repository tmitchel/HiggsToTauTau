// Copyright [2020] Tyler Mitchell

#ifndef INCLUDE_MODELS_TAU_H_
#define INCLUDE_MODELS_TAU_H_

#include <string>
#include "TLorentzVector.h"
#include "../tau_factory.h"

class tau {
    friend tau_factory;

 private:
    std::string name = "tau";
    Int_t gen_match;
    Float_t pt, eta, phi, mass, charge, decayMode, dmf, dmf_new, iso, deepiso, gen_pt, gen_eta, gen_phi;
    Float_t VLooseIsoMVA, LooseIsoMVA, MediumIsoMVA, TightIsoMVA, VTightIsoMVA, VVTightIsoMVA;
    Float_t VVVLooseIsoDeep, VLooseIsoDeep, LooseIsoDeep, MediumIsoDeep, TightIsoDeep, VTightIsoDeep, VVTightIsoDeep;
    Bool_t AgainstTightElectronMVA, AgainstVLooseElectronMVA, AgainstTightMuonMVA, AgainstLooseMuonMVA;
    Bool_t AgainstTightElectronDeep, AgainstVVLooseElectronDeep, AgainstVVVLooseElectronDeep, AgainstTightMuonDeep, AgainstVLooseMuonDeep;

    TLorentzVector p4;

 public:
    tau(Float_t, Float_t, Float_t, Float_t, Float_t);
    ~tau() {}

    std::string getName() { return name; }
    Int_t getGenMatch() { return gen_match; }
    Int_t getCharge() { return charge; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIso() { return iso; }
    Float_t getDeepIso() { return deepiso; }
    Float_t getVLooseIsoMVA() { return VLooseIsoMVA; }
    Float_t getLooseIsoMVA() { return LooseIsoMVA; }
    Float_t getMediumIsoMVA() { return MediumIsoMVA; }
    Float_t getTightIsoMVA() { return TightIsoMVA; }
    Float_t getVTightIsoMVA() { return VTightIsoMVA; }
    Float_t getVVTightIsoMVA() { return VVTightIsoMVA; }
    Float_t getVVVLooseIsoDeep() { return VVVLooseIsoDeep; }
    Float_t getVLooseIsoDeep() { return VLooseIsoDeep; }
    Float_t getLooseIsoDeep() { return LooseIsoDeep; }
    Float_t getMediumIsoDeep() { return MediumIsoDeep; }
    Float_t getTightIsoDeep() { return TightIsoDeep; }
    Float_t getVTightIsoDeep() { return VTightIsoDeep; }
    Float_t getVVTightIsoDeep() { return VVTightIsoDeep; }
    Float_t getDecayMode() { return decayMode; }
    Float_t getDecayModeFinding() { return dmf; }
    Float_t getDecayModeFindingNew() { return dmf_new; }
    Float_t getGenPt() { return gen_pt; }
    Float_t getGenEta() { return gen_eta; }
    Float_t getGenPhi() { return gen_phi; }
    Bool_t getAgainstTightElectronMVA() { return AgainstTightElectronMVA; }
    Bool_t getAgainstVLooseElectronMVA() { return AgainstVLooseElectronMVA; }
    Bool_t getAgainstTightMuonMVA() { return AgainstTightMuonMVA; }
    Bool_t getAgainstLooseMuonMVA() { return AgainstLooseMuonMVA; }
    Bool_t getAgainstTightElectronDeep() { return AgainstTightElectronDeep; }
    Bool_t getAgainstVVLooseElectronDeep() { return AgainstVVLooseElectronDeep; }
    Bool_t getAgainstVVVLooseElectronDeep() { return AgainstVVVLooseElectronDeep; }
    Bool_t getAgainstTightMuonDeep() { return AgainstTightMuonDeep; }
    Bool_t getAgainstVLooseMuonDeep() { return AgainstVLooseMuonDeep; }
    TLorentzVector getP4() { return p4; }
};

// initialize member data and set TLorentzVector
tau::tau(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _mass, Float_t _charge)
    : pt(_pt), eta(_eta), phi(_phi), mass(_mass), charge(_charge) {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INCLUDE_MODELS_TAU_H_
