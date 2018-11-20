#ifndef MUON_FACTORY_H
#define MUON_FACTORY_H

#include <vector>
#include <string>
#include <cmath>
#include "TTree.h"
#include "TLorentzVector.h"

class muon_factory; // forward declare so it can befriend muons

//////////////////////////////////////////////
// Purpose: To hold a single composite muon //
//////////////////////////////////////////////
class muon {
  friend muon_factory;
 private:
  std::string name = "muon";
  Float_t pt, eta, phi, mass, charge, px, py, pz, iso, gen_match, mediumID_2016, mediumID;
  TLorentzVector p4;
 public:

  muon(Float_t, Float_t, Float_t, Float_t, Float_t);   
  ~muon() {};

  // getters
  std::string getName()     { return name;          };
  TLorentzVector getP4()    { return p4;            };
  Float_t getPt()           { return pt;            };
  Float_t getEta()          { return eta;           };
  Float_t getPhi()          { return phi;           };
  Float_t getMass()         { return mass;          };
  Float_t getPx()           { return px;            };
  Float_t getPy()           { return py;            };
  Float_t getPz()           { return pz;            };
  Float_t getIso()          { return iso;           };
  Float_t getGenMatch()     { return gen_match;     };
  Float_t getMediumID2016() { return mediumID_2016; };
  Float_t getMediumID()     { return mediumID;      };
  Int_t getCharge()         { return charge;        };
};

// initialize member data and set TLorentzVector
muon::muon(Float_t Pt, Float_t Eta, Float_t Phi, Float_t M, Float_t Charge) :
  pt(Pt),
  eta(Eta),
  phi(Phi),
  mass(M),
  charge(Charge)
{
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

/////////////////////////////////////////////
// Purpose: To build a collection of muons //
// from the ntuple                         //
/////////////////////////////////////////////
class muon_factory {
private:
  Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1, mediumID, mediumID_2016;
  Int_t gen_match_1;

public:
  muon_factory (TTree*);
  virtual ~muon_factory () {};
  muon run_factory();
};

// read data from tree into member variabl
muon_factory::muon_factory(TTree* input) {
  input -> SetBranchAddress( "px_1",                        &px_1                       );
  input -> SetBranchAddress( "py_1",                        &py_1                       );
  input -> SetBranchAddress( "pz_1",                        &pz_1                       );
  input -> SetBranchAddress( "pt_1",                        &pt_1                       );
  input -> SetBranchAddress( "eta_1",                       &eta_1                      );
  input -> SetBranchAddress( "phi_1",                       &phi_1                      );
  input -> SetBranchAddress( "m_1",                         &m_1                        );
  input -> SetBranchAddress( "q_1",                         &q_1                        );
  input -> SetBranchAddress( "iso_1",                       &iso_1                      );
  input -> SetBranchAddress( "id_m_medium_1",               &mediumID                   );
  input -> SetBranchAddress( "id_m_medium2016_1",           &mediumID_2016              );
}

// create muon object and set member data
muon muon_factory::run_factory() {
  muon mu(pt_1, eta_1, phi_1, m_1, q_1);
  mu.px = px_1;
  mu.py = py_1;
  mu.pz = pz_1;
  mu.iso = iso_1;
  mu.gen_match = gen_match_1;
  mu.mediumID = mediumID;
  mu.mediumID_2016 = mediumID_2016;

  return mu;
}


#endif
