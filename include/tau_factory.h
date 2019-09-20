// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_TAU_FACTORY_H_
#define INCLUDE_TAU_FACTORY_H_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"

class tau_factory;
Float_t get_TES_shift(int, float);

////////////////////////////////////////////////////
// Purpose: To hold a single composite tau lepton //
////////////////////////////////////////////////////
class tau {
    friend tau_factory;

 private:
    std::string name = "tau";
    Int_t gen_match;
    Float_t pt, eta, phi, mass, charge, decayMode, dmf, dmf_new, iso, gen_pt, gen_eta, gen_phi;
    Bool_t AgainstTightElectron, AgainstVLooseElectron, AgainstTightMuon, AgainstLooseMuon;
    Float_t VLooseIsoMVA, LooseIsoMVA, MediumIsoMVA, TightIsoMVA, VTightIsoMVA, VVTightIsoMVA;
    TLorentzVector p4;

 public:
    tau(Float_t, Float_t, Float_t, Float_t, Float_t);
    ~tau() {}

    // scale tau pT for embedded samples
    void scalePt(Float_t newPt) {
        pt *= newPt;
        p4.SetPtEtaPhiM(pt, eta, phi, mass);
    }

    void scaleTES(Float_t val) {
        p4 *= val;
    }

    // getters
    std::string getName() { return name; }
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIso() { return iso; }
    Float_t getVLooseIsoMVA() { return VLooseIsoMVA; }
    Float_t getLooseIsoMVA() { return LooseIsoMVA; }
    Float_t getMediumIsoMVA() { return MediumIsoMVA; }
    Float_t getTightIsoMVA() { return TightIsoMVA; }
    Float_t getVTightIsoMVA() { return VTightIsoMVA; }
    Float_t getVVTightIsoMVA() { return VVTightIsoMVA; }
    Float_t getDecayMode() { return decayMode; }
    Float_t getDecayModeFinding() { return dmf; }
    Float_t getDecayModeFindingNew() { return dmf_new; }
    Bool_t getAgainstTightElectron() { return AgainstTightElectron; }
    Bool_t getAgainstVLooseElectron() { return AgainstVLooseElectron; }
    Bool_t getAgainstTightMuon() { return AgainstTightMuon; }
    Bool_t getAgainstLooseMuon() { return AgainstLooseMuon; }
    Int_t getGenMatch() { return gen_match; }
    Float_t getGenPt() { return gen_pt; }
    Float_t getGenEta() { return gen_eta; }
    Float_t getGenPhi() { return gen_phi; }
    Int_t getCharge() { return charge; }
};

// initialize member data and set TLorentzVector
tau::tau(Float_t Pt, Float_t Eta, Float_t Phi, Float_t M, Float_t Charge) : pt(Pt), eta(Eta), phi(Phi), mass(M), charge(Charge) {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

/////////////////////////////////////////////////
// Purpose: To build a collection of taus      //
// from the ntuple                             //
/////////////////////////////////////////////////
class tau_factory {
 private:
    std::string syst;
    Int_t gen_match_2, era;
    Float_t px_2, py_2, pz_2, pt_2, eta_2, phi_2, m_2, e_2, iso_2, q_2, mt_2, tZTTGenPt, tZTTGenEta, tZTTGenPhi;
    Float_t againstElectronTightMVA6_2, againstElectronVLooseMVA6_2, againstMuonTight3_2, againstMuonLoose3_2, decayMode, dmf, dmf_new;
    Float_t byVLooseIsolationMVArun2v1DBoldDMwLT_2, byLooseIsolationMVArun2v1DBoldDMwLT_2, byMediumIsolationMVArun2v1DBoldDMwLT_2,
        byTightIsolationMVArun2v1DBoldDMwLT_2, byVTightIsolationMVArun2v1DBoldDMwLT_2, byVVTightIsolationMVArun2v1DBoldDMwLT_2;

 public:
    tau_factory(TTree*, int, std::string);
    virtual ~tau_factory() {}
    tau run_factory();
};

// read data from tree Int_to member variables
tau_factory::tau_factory(TTree* input, int _era, std::string _syst) : era(_era), syst(_syst) {
    input->SetBranchAddress("pt_2", &pt_2);
    input->SetBranchAddress("eta_2", &eta_2);
    input->SetBranchAddress("phi_2", &phi_2);
    input->SetBranchAddress("m_2", &m_2);
    input->SetBranchAddress("e_2", &e_2);
    input->SetBranchAddress("q_2", &q_2);
    input->SetBranchAddress("gen_match_2", &gen_match_2);
    input->SetBranchAddress("tZTTGenPt", &tZTTGenPt);
    input->SetBranchAddress("tZTTGenEta", &tZTTGenEta);
    input->SetBranchAddress("tZTTGenPhi", &tZTTGenPhi);
    input->SetBranchAddress("tAgainstElectronTightMVA6", &againstElectronTightMVA6_2);
    input->SetBranchAddress("tAgainstElectronVLooseMVA6", &againstElectronVLooseMVA6_2);
    input->SetBranchAddress("tAgainstMuonTight3", &againstMuonTight3_2);
    input->SetBranchAddress("tAgainstMuonLoose3", &againstMuonLoose3_2);
    input->SetBranchAddress("tDecayMode", &decayMode);
    input->SetBranchAddress("tDecayModeFinding", &dmf);
    input->SetBranchAddress("tDecayModeFindingNewDMs", &dmf_new);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTraw", &iso_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVLoose", &byVLooseIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTLoose", &byLooseIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTMedium", &byMediumIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTTight", &byTightIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVTight", &byVTightIsolationMVArun2v1DBoldDMwLT_2);
    input->SetBranchAddress("tRerunMVArun2v2DBoldDMwLTVVTight", &byVVTightIsolationMVArun2v1DBoldDMwLT_2);
}

// create electron object and set member data
tau tau_factory::run_factory() {
    tau t(pt_2, eta_2, phi_2, m_2, q_2);
    t.iso = iso_2;
    t.gen_match = gen_match_2;
    t.AgainstTightElectron = againstElectronTightMVA6_2;
    t.AgainstVLooseElectron = againstElectronVLooseMVA6_2;
    t.AgainstTightMuon = againstMuonTight3_2;
    t.AgainstLooseMuon = againstMuonLoose3_2;
    t.decayMode = decayMode;
    t.VLooseIsoMVA = byVLooseIsolationMVArun2v1DBoldDMwLT_2;
    t.LooseIsoMVA = byLooseIsolationMVArun2v1DBoldDMwLT_2;
    t.MediumIsoMVA = byMediumIsolationMVArun2v1DBoldDMwLT_2;
    t.TightIsoMVA = byTightIsolationMVArun2v1DBoldDMwLT_2;
    t.VTightIsoMVA = byVTightIsolationMVArun2v1DBoldDMwLT_2;
    t.VVTightIsoMVA = byVVTightIsolationMVArun2v1DBoldDMwLT_2;
    t.dmf = dmf;
    t.dmf_new = dmf_new;
    t.gen_pt = tZTTGenPt;
    t.gen_eta = tZTTGenEta;
    t.gen_phi = tZTTGenPhi;

    float shift = get_TES_shift(era, dmf);
    if (syst.find("ltau_FES") != std::string::npos) {
        syst.find("_Up") != std::string::npos ? t.scaleTES(1 + 0.02) : t.scaleTES(1 - 0.02);
    } else if (syst.find("DM0") != std::string::npos && dmf == 0) {
        syst.find("_Up") != std::string::npos ? t.scaleTES(1 + shift) : t.scaleTES(1 - shift);
    } else if (syst.find("DM1_") != std::string::npos && dmf == 1) {
        syst.find("_Up") != std::string::npos ? t.scaleTES(1 + shift) : t.scaleTES(1 - shift);
    } else if (syst.find("DM10_") != std::string::npos && dmf == 10) {
        syst.find("_Up") != std::string::npos ? t.scaleTES(1 + shift) : t.scaleTES(1 - shift);
    }

    return t;
}

// handle shifting TES for systematics
Float_t get_TES_shift(int year, float decayMode) {
  if (year == 2016) {
    if (decayMode == 0) {
        return 0.010;
    } else if (decayMode == 1) {
        return 0.009;
    } else if (decayMode == 10) {
        return 0.011;
    }
  } else if (year == 2017) {
    if (decayMode == 0) {
        return 0.008;
    } else if (decayMode == 1) {
        return 0.008;
    } else if (decayMode == 10) {
        return 0.009;
    }
  } else if (year == 2018) {
    if (decayMode == 0) {
        return 0.011;
    } else if (decayMode == 1) {
        return 0.008;
    } else if (decayMode == 10) {
        return 0.009;
    }
  }
  return -999;
}

#endif  // INCLUDE_TAU_FACTORY_H_
