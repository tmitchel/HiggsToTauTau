#ifndef DITAU_FACTORY_H
#define DITAU_FACTORY_H
#include <vector>
#include <utility>
#include <string>
#include <cmath>
#include "TTree.h"
#include "TLorentzVector.h"

class ditau_factory;

////////////////////////////////////////////////////
// Purpose: To hold a single composite tau lepton //
////////////////////////////////////////////////////
class ditau {
  friend ditau_factory;
private:
  std::string name = "tau";
  Int_t gen_match;
  Float_t pt, eta, phi, mass, charge, px, py, pz, decayMode;
  Bool_t AgainstTightElectron, AgainstVLooseElectron, AgainstLooseMuon;
  Float_t VLooseIsoMVA, LooseIsoMVA, MediumIsoMVA, TightIsoMVA, VTightIsoMVA, VVTightIsoMVA;
  TLorentzVector p4;
public:

  ditau(Float_t, Float_t, Float_t, Float_t, Float_t);
  ~ditau() {};

  void scalePt(Float_t scalePt) {
    pt *= scalePt;
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
  }

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
  Float_t getDecayMode()              { return decayMode;             };
  Float_t getVLooseIsoMVA()           { return VLooseIsoMVA;          };
  Float_t getLooseIsoMVA()            { return LooseIsoMVA;           };
  Float_t getMediumIsoMVA()           { return MediumIsoMVA;          };
  Float_t getTightIsoMVA()            { return TightIsoMVA;           };
  Float_t getVTightIsoMVA()           { return VTightIsoMVA;          };
  Float_t getVVTightIsoMVA()          { return VVTightIsoMVA;         };
  Bool_t getAgainstTightElectron()    { return AgainstTightElectron;  };
  Bool_t getAgainstVLooseElectron()   { return AgainstVLooseElectron; };
  Bool_t getAgainstLooseMuon()        { return AgainstLooseMuon;      };
  Int_t getGenMatch()                 { return gen_match;             };
  Int_t getCharge()                   { return charge;                };
};

// initialize member data and set TLorentzVector
ditau::ditau(Float_t Pt, Float_t Eta, Float_t Phi, Float_t M, Float_t Charge) :
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
class ditau_factory {
private:
  Int_t gen_match_1, gen_match_2;
  Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, iso_1, q_1, mt_1;
  Float_t againstElectronTightMVA6_1, againstElectronVLooseMVA6_1, againstMuonLoose3_1, t1_decayMode;
  Float_t px_2, py_2, pz_2, pt_2, eta_2, phi_2, m_2, e_2, iso_2, q_2, mt_2;
  Float_t againstElectronTightMVA6_2, againstElectronVLooseMVA6_2, againstMuonLoose3_2, t2_decayMode;
  Float_t byVLooseIsolationMVArun2v1DBoldDMwLT_1, byLooseIsolationMVArun2v1DBoldDMwLT_1, byMediumIsolationMVArun2v1DBoldDMwLT_1,
        byTightIsolationMVArun2v1DBoldDMwLT_1, byVTightIsolationMVArun2v1DBoldDMwLT_1, byVVTightIsolationMVArun2v1DBoldDMwLT_1;
  Float_t byVLooseIsolationMVArun2v1DBoldDMwLT_2, byLooseIsolationMVArun2v1DBoldDMwLT_2, byMediumIsolationMVArun2v1DBoldDMwLT_2,
          byTightIsolationMVArun2v1DBoldDMwLT_2, byVTightIsolationMVArun2v1DBoldDMwLT_2, byVVTightIsolationMVArun2v1DBoldDMwLT_2;

public:
  ditau_factory (TTree*);
  virtual ~ditau_factory () {};

  std::pair<ditau, ditau> run_factory();
};

// read data from tree Int_to member variables
ditau_factory::ditau_factory(TTree* input) {
  input -> SetBranchAddress( "px_1",                                    &px_1                                   );
  input -> SetBranchAddress( "py_1",                                    &py_1                                   );
  input -> SetBranchAddress( "pz_1",                                    &pz_1                                   );
  input -> SetBranchAddress( "pt_1",                                    &pt_1                                   );
  input -> SetBranchAddress( "eta_1",                                   &eta_1                                  );
  input -> SetBranchAddress( "phi_1",                                   &phi_1                                  );
  input -> SetBranchAddress( "m_1",                                     &m_1                                    );
  input -> SetBranchAddress( "e_1",                                     &e_1                                    );
  input -> SetBranchAddress( "iso_1",                                   &iso_1                                  );
  input -> SetBranchAddress( "q_1",                                     &q_1                                    );
  input -> SetBranchAddress( "gen_match_1",                             &gen_match_1                            );
  input -> SetBranchAddress( "byVLooseIsolationMVArun2v1DBoldDMwLT_1",  &byVLooseIsolationMVArun2v1DBoldDMwLT_1 );
  input -> SetBranchAddress( "byLooseIsolationMVArun2v1DBoldDMwLT_1",   &byLooseIsolationMVArun2v1DBoldDMwLT_1  );
  input -> SetBranchAddress( "byMediumIsolationMVArun2v1DBoldDMwLT_1",  &byMediumIsolationMVArun2v1DBoldDMwLT_1 );
  input -> SetBranchAddress( "byTightIsolationMVArun2v1DBoldDMwLT_1",   &byTightIsolationMVArun2v1DBoldDMwLT_1  );
  input -> SetBranchAddress( "byVTightIsolationMVArun2v1DBoldDMwLT_1",  &byVTightIsolationMVArun2v1DBoldDMwLT_1 );
  input -> SetBranchAddress( "byVVTightIsolationMVArun2v1DBoldDMwLT_1", &byVVTightIsolationMVArun2v1DBoldDMwLT_1);
  input -> SetBranchAddress( "againstElectronTightMVA6_1",              &againstElectronTightMVA6_1             );
  input -> SetBranchAddress( "againstElectronVLooseMVA6_1",             &againstElectronVLooseMVA6_1            );
  input -> SetBranchAddress( "againstMuonLoose3_1",                     &againstMuonLoose3_1                    );
  input -> SetBranchAddress( "t1_decayMode",                            &t1_decayMode                           );
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
  input -> SetBranchAddress( "byVLooseIsolationMVArun2v1DBoldDMwLT_2",  &byVLooseIsolationMVArun2v1DBoldDMwLT_2 );
  input -> SetBranchAddress( "byLooseIsolationMVArun2v1DBoldDMwLT_2",   &byLooseIsolationMVArun2v1DBoldDMwLT_2  );
  input -> SetBranchAddress( "byMediumIsolationMVArun2v1DBoldDMwLT_2",  &byMediumIsolationMVArun2v1DBoldDMwLT_2 );
  input -> SetBranchAddress( "byTightIsolationMVArun2v1DBoldDMwLT_2",   &byTightIsolationMVArun2v1DBoldDMwLT_2  );
  input -> SetBranchAddress( "byVTightIsolationMVArun2v1DBoldDMwLT_2",  &byVTightIsolationMVArun2v1DBoldDMwLT_2 );
  input -> SetBranchAddress( "byVVTightIsolationMVArun2v1DBoldDMwLT_2", &byVVTightIsolationMVArun2v1DBoldDMwLT_2);
  input -> SetBranchAddress( "againstElectronTightMVA6_2",              &againstElectronTightMVA6_2             );
  input -> SetBranchAddress( "againstElectronVLooseMVA6_1",             &againstElectronVLooseMVA6_1            );
  input -> SetBranchAddress( "againstMuonLoose3_2",                     &againstMuonLoose3_2                    );
  input -> SetBranchAddress( "t2_decayMode",                            &t2_decayMode                           );
}

// create electron object and set member data
std::pair<ditau, ditau> ditau_factory::run_factory() {
  ditau t1(pt_1, eta_1, phi_1, m_1, q_1);
  t1.px = px_1;
  t1.py = py_1;
  t1.pz = pz_1;
  t1.gen_match = gen_match_1;
  t1.AgainstTightElectron = againstElectronTightMVA6_1;
  t1.AgainstVLooseElectron = againstElectronVLooseMVA6_1;
  t1.AgainstLooseMuon = againstMuonLoose3_1;
  t1.decayMode = t1_decayMode;
  t1.VLooseIsoMVA = byVLooseIsolationMVArun2v1DBoldDMwLT_1;
  t1.LooseIsoMVA = byLooseIsolationMVArun2v1DBoldDMwLT_1;
  t1.MediumIsoMVA = byMediumIsolationMVArun2v1DBoldDMwLT_1;
  t1.TightIsoMVA = byTightIsolationMVArun2v1DBoldDMwLT_1;
  t1.VTightIsoMVA = byVTightIsolationMVArun2v1DBoldDMwLT_1;
  t1.VVTightIsoMVA = byVVTightIsolationMVArun2v1DBoldDMwLT_1;

  ditau t2(pt_2, eta_2, phi_2, m_2, q_2);
  t2.px = px_2;
  t2.py = py_2;
  t2.pz = pz_2;
  t2.gen_match = gen_match_2;
  t2.AgainstTightElectron = againstElectronTightMVA6_2;
  t2.AgainstVLooseElectron = againstElectronVLooseMVA6_1;
  t2.AgainstLooseMuon = againstMuonLoose3_2;
  t2.decayMode = t2_decayMode;
  t2.VLooseIsoMVA = byVLooseIsolationMVArun2v1DBoldDMwLT_2;
  t2.LooseIsoMVA = byLooseIsolationMVArun2v1DBoldDMwLT_2;
  t2.MediumIsoMVA = byMediumIsolationMVArun2v1DBoldDMwLT_2;
  t2.TightIsoMVA = byTightIsolationMVArun2v1DBoldDMwLT_2;
  t2.VTightIsoMVA = byVTightIsolationMVArun2v1DBoldDMwLT_2;
  t2.VVTightIsoMVA = byVVTightIsolationMVArun2v1DBoldDMwLT_2;

  // sort by pT
  if (pt_1 > pt_2) {
    return std::make_pair(t1, t2);
  } 
  return std::make_pair(t2, t1);
}

#endif