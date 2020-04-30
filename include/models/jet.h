// Copyright [2020] Tyler Mitchell

#ifndef INCLUDE_MODELS_JET_H_
#define INCLUDE_MODELS_JET_H_

#include <string>
#include "TLorentzVector.h"
#include "../jet_factory_fsa.h"
#include "../jet_factory_gg.h"

class jet {
    friend class jet_factory_fsa
    friend class jet_factory_gg;

 private:
    Float_t pt, eta, phi, csv, flavor;
    TLorentzVector p4;

 public:
    jet(Float_t, Float_t, Float_t, Float_t, Float_t);
    virtual ~jet() {}

    Float_t getPt() { return pt; }
    Float_t getEta() { return eta; }
    Float_t getPhi() { return phi; }
    Float_t getCSV() { return csv; }
    Float_t getFlavor() { return flavor; }
    TLorentzVector getP4() { return p4; }
};

// initialize member data and set TLorentzVector
jet::jet(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _csv, Float_t _flavor = -9999)
    : pt(_pt), eta(_eta), phi(_phi), csv(_csv), flavor(_flavor) {
    p4.SetPtEtaPhiM(pt, eta, phi, 0.);
}

#endif  // INCLUDE_MODELS_JET_H_
