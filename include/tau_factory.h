#include <vector>
#include <string>
#include <cmath>
#include "TTree.h"
#include "TLorentzVector.h"

////////////////////////////////////////////////////
// Purpose: To hold a single composite tau lepton //
////////////////////////////////////////////////////
class tau {
private:
public:
  std::string name = "tau";
  float pt, eta, phi, mass, charge, px, py, pz, dmf;
  bool tightIsoMVA, AgainstTightElectron, AgainstLooseMuon, MediumIsoMVA;
  int gen_match;
  TLorentzVector p4;

  tau(float, float, float, float, float);
  ~tau() {};

  // getters
  std::string getName()             { return name;                  };
  TLorentzVector getP4()            { return p4;                    };
  float getPt()                     { return pt;                    };
  float getEta()                    { return eta;                   };
  float getPhi()                    { return phi;                   };
  float getMass()                   { return mass;                  };
  float getPx()                     { return px;                    };
  float getPy()                     { return py;                    };
  float getPz()                     { return pz;                    };
  float getDecayModeFinding()       { return dmf;                   };
  bool getTightIsoMVA()             { return tightIsoMVA;           };
  bool getAgainstTightElectron ()   { return AgainstTightElectron;  };
  bool getAgainstLooseMuon ()       { return AgainstLooseMuon;      };
  bool getMediumIsoMVA()            { return MediumIsoMVA;          };
  int getGenMatch()                 { return gen_match;             };
  int getCharge()                   { return charge;                };

  // setters
  void setPt(float Pt)                            { pt = Pt;                            };
  void setEta(float Eta)                          { eta = Eta;                          };
  void setPhi(float Phi)                          { phi = Phi;                          };
  void setMass(float M)                           { mass = M;                           };
  void setCharge(float Charge)                    { charge = Charge;                    };
  void setPx(float Px)                            { px = Px;                            };
  void setPy(float Py)                            { py = Py;                            };
  void setPz(float Pz)                            { pz = Pz;                            };
  void setDecayModeFinding(float DMF)             { dmf = DMF;                          };
  void setTightIsoMVA(bool newIsoTight)           { tightIsoMVA = newIsoTight;          };
  void setAgainstTightElectron(bool newAElTight)  { AgainstTightElectron = newAElTight; };
  void setAgainstLooseMuon(bool newAMuLoose)      { AgainstLooseMuon = newAMuLoose;     };
  void setMediumIsoMVA(bool newIsoMed)            { MediumIsoMVA = newIsoMed;           };
  void setGenMatch(int GenMatch)                  { gen_match = GenMatch;               };
};

// initialize member data and set TLorentzVector
tau::tau(float Pt, float Eta, float Phi, float M, float Charge) :
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
  Float_t byMediumIsolationMVArun2v1DBoldDMwLT_2;
  Int_t gen_match_2;

public:
  tau_factory (TTree*);
  virtual ~tau_factory () {};

  tau run_factory();
};

// read data from tree into member variables
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

}

// create electron object and set member data
tau tau_factory::run_factory() {
  tau t(pt_2, eta_2, phi_2, m_2, q_2);
  t.setPx(px_2);
  t.setPy(py_2);
  t.setPz(pz_2);
  t.setGenMatch(gen_match_2);
  t.setTightIsoMVA(byTightIsolationMVArun2v1DBoldDMwLT_2);
  t.setAgainstTightElectron(againstElectronTightMVA6_2);
  t.setAgainstLooseMuon(againstMuonLoose3_2);
  t.setMediumIsoMVA(byMediumIsolationMVArun2v1DBoldDMwLT_2);
  t.setDecayModeFinding(decayModeFinding_2);

  return t;
}
