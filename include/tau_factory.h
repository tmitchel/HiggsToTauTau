#include <vector>
#include <string>
#include <cmath>
#include "TTree.h"
#include "TLorentzVector.h"

class tau_factory;

////////////////////////////////////////////////////
// Purpose: To hold a single composite tau lepton //
////////////////////////////////////////////////////
class tau {
  friend tau_factory;
private:
  std::string name = "tau";
  Float_t pt, eta, phi, mass, charge, px, py, pz, dmf, tightIsoMVA, l2_decayMode, gen_match;
  Bool_t AgainstTightElectron, AgainstLooseMuon, MediumIsoMVA;
  TLorentzVector p4;
public:

  tau(Float_t, Float_t, Float_t, Float_t, Float_t);
  ~tau() {};

  // getters
  std::string getName()               { return name;                  };
  TLorentzVector getP4()              { return p4;                    };
  Float_t getPt()                     { return pt;                    };
  Float_t getEta()                    { return eta;                   };
  Float_t getPhi()                    { return phi;                   };
  Float_t getMass()                   { return mass;                  };
  Float_t getPx()                     { return px;                    };
  Float_t getPy()                     { return py;                    };
  Float_t getPz()                     { return pz;                    };
  Float_t getDecayModeFinding()       { return dmf;                   };
  Float_t getTightIsoMVA()            { return tightIsoMVA;           };
  Float_t getL2DecayMode()            { return l2_decayMode;          };
  Float_t getGenMatch()               { return gen_match;             };
  Bool_t getAgainstTightElectron()    { return AgainstTightElectron;  };
  Bool_t getAgainstLooseMuon()        { return AgainstLooseMuon;      };
  Bool_t getMediumIsoMVA()            { return MediumIsoMVA;          };
  Int_t getCharge()                   { return charge;                };
};

// initialize member data and set TLorentzVector
tau::tau(Float_t Pt, Float_t Eta, Float_t Phi, Float_t M, Float_t Charge) :
  pt(Pt),
  eta(Eta),
  phi(Phi),
  mass(M),
  charge(Charge)
{
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

/////////////////////////////////////////////////
// Purpose: To build a collection of taus      //
// from the ntuple                             //
/////////////////////////////////////////////////
class tau_factory {
private:
  Float_t px_2, py_2, pz_2, pt_2, eta_2, phi_2, m_2, e_2, iso_2, q_2, mt_2, decayModeFinding_2;
  Float_t byTightIsolationMVArun2v1DBoldDMwLT_2, againstElectronTightMVA6_2, againstMuonLoose3_2;
  Float_t byMediumIsolationMVArun2v1DBoldDMwLT_2, l2_decayMode, gen_match_2;

public:
  tau_factory (TTree*);
  virtual ~tau_factory () {};

  tau run_factory();
};

// read data from tree Int_to member variables
tau_factory::tau_factory(TTree* input) {
  input -> SetBranchAddress( "px_2",                                    &px_2                                   );
  input -> SetBranchAddress( "py_2",                                    &py_2                                   );
  input -> SetBranchAddress( "pz_2",                                    &pz_2                                   );
  input -> SetBranchAddress( "pt_2",                                    &pt_2                                   );
  input -> SetBranchAddress( "eta_2",                                   &eta_2                                  );
  input -> SetBranchAddress( "phi_2",                                   &phi_2                                  );
  input -> SetBranchAddress( "m_2",                                     &m_2                                    );
  input -> SetBranchAddress( "e_2",                                     &e_2                                    );
  input -> SetBranchAddress( "iso_2",                                   &iso_2                                  );
  input -> SetBranchAddress( "q_2",                                     &q_2                                    );
  input -> SetBranchAddress( "gen_match_2",                             &gen_match_2                            );
  input -> SetBranchAddress( "decayModeFinding_2",                      &decayModeFinding_2                     );
  input -> SetBranchAddress( "byTightIsolationMVArun2v1DBoldDMwLT_2",   &byTightIsolationMVArun2v1DBoldDMwLT_2  );
  input -> SetBranchAddress( "againstElectronTightMVA6_2",              &againstElectronTightMVA6_2             );
  input -> SetBranchAddress( "againstMuonLoose3_2",                     &againstMuonLoose3_2                    );
  input -> SetBranchAddress( "byMediumIsolationMVArun2v1DBoldDMwLT_2",  &byMediumIsolationMVArun2v1DBoldDMwLT_2 );
  input -> SetBranchAddress( "l2_decayMode",                            &l2_decayMode                           );
}

// create electron object and set member data
tau tau_factory::run_factory() {
  tau t(pt_2, eta_2, phi_2, m_2, q_2);
  t.px = px_2;
  t.py = py_2;
  t.pz = pz_2;
  t.gen_match = gen_match_2;
  t.tightIsoMVA = byTightIsolationMVArun2v1DBoldDMwLT_2;
  t.AgainstTightElectron = againstElectronTightMVA6_2;
  t.AgainstLooseMuon = againstMuonLoose3_2;
  t.dmf = decayModeFinding_2;
  t.l2_decayMode = l2_decayMode;

  return t;
}
