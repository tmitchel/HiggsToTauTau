// Copyright [2020] Tyler Mitchell

#ifndef INCLUDE_MODELS_GEN_PARTICLE_H_
#define INCLUDE_MODELS_GEN_PARTICLE_H_

#include <string>
#include "TLorentzVector.h"

class gen {
 private:
    Int_t pid;
    UShort_t status;
    Float_t pt, eta, phi, mass;
    TLorentzVector p4;

 public:
    gen(Float_t, Float_t, Float_t, Float_t);
    virtual ~gen() {}

    // getters
    Int_t getPID() { return pid; }
    Bool_t getStatus(int shift) { return (status >> shift & 1); }
    Float_t getPt() { return pt; }
    Float_t getEta() { return eta; }
    Float_t getPhi() { return phi; }
    Float_t getMass() { return mass; }
    TLorentzVector getP4() { return p4; }

    // setters
    void setPID(Int_t _pid) { pid = _pid; }
    void setStatus(UShort_t _status) { status = _status; }
};

// initialize member data and set TLorentzVector
gen::gen(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _mass)
    : pt(_pt), eta(_eta), phi(_phi), mass(_mass) {
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INCLUDE_MODELS_GEN_PARTICLE_H_
