#include <vector>
#include <string>
#include <cmath>
#include "TTree.h"
#include "TLorentzVector.h"

//////////////////////////////////////////////////
// Purpose: To hold a single composite electron //
//////////////////////////////////////////////////
class electron {
private:
public:
  std::string name = "electron";
  float pt, eta, phi, mass, charge, px, py, pz, iso;
  int gen_match;

  TLorentzVector p4;

  electron(float, float, float, float, float);
  ~electron() {};

  // getters
  std::string getName()  { return name; };
  TLorentzVector getP4() { return p4; };

  float getPt()       { return pt; };
  float getEta()      { return eta; };
  float getPhi()      { return phi; };
  float getMass()     { return mass; };
  float getPx()       { return px; };
  float getPy()       { return py; };
  float getPz()       { return pz; };
  float getIso()      { return iso; };
  int getGenMatch()   { return gen_match; };
  int getCharge()     { return charge; };

  // setters
  void setPt(float Pt)             { pt = Pt; };
  void setEta(float Eta)           { eta = Eta; };
  void setPhi(float Phi)           { phi = Phi; };
  void setMass(float M)            { mass = M; };
  void setCharge(float Charge)     { charge = Charge; };
  void setPx(float Px)             { px = Px; };
  void setPy(float Py)             { py = Py; };
  void setPz(float Pz)             { pz = Pz; };
  void setIso(float Iso)           { iso = Iso; };
  void setGenMatch(int GenMatch)   { gen_match = GenMatch; };
};

electron::electron(float Pt, float Eta, float Phi, float M, float Charge) :
  pt(Pt),
  eta(Eta),
  phi(Phi),
  mass(M),
  charge(Charge)
{
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

/////////////////////////////////////////////////
// Purpose: To build a collection of electrons //
// from the ntuple                             //
/////////////////////////////////////////////////
class electron_factory {
private:
  Float_t px_1, py_1, pz_1, pt_1, eta_1, phi_1, m_1, e_1, q_1, mt_1, iso_1;
  Int_t gen_match_1;

public:
  electron_factory (TTree*);
  virtual ~electron_factory () {};

  electron run_factory();
};

electron_factory::electron_factory(TTree* input) {
  input -> SetBranchAddress( "px_1",        &px_1        );
  input -> SetBranchAddress( "py_1",        &py_1        );
  input -> SetBranchAddress( "pz_1",        &pz_1        );
  input -> SetBranchAddress( "pt_1",        &pt_1        );
  input -> SetBranchAddress( "eta_1",       &eta_1       );
  input -> SetBranchAddress( "phi_1",       &phi_1       );
  input -> SetBranchAddress( "m_1",         &m_1         );
  input -> SetBranchAddress( "q_1",         &q_1         );
  input -> SetBranchAddress( "iso_1",       &iso_1       );
  input -> SetBranchAddress( "gen_match_1", &gen_match_1 );
}

electron electron_factory::run_factory() {
  electron el(pt_1, eta_1, phi_1, m_1, q_1);
  el.setPx(px_1);
  el.setPy(py_1);
  el.setPz(pz_1);
  el.setIso(iso_1);
  el.setGenMatch(gen_match_1);

  return el;
}
