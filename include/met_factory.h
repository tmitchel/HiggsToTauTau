#include "TTree.h"

class met_factory {
private:
  float met, metSig, metcov00, metcov10, metcov11, metcov01;
  float metphi, met_py, met_px;
  TLorentzVector p4;

public:
  met_factory (TTree*);
  virtual ~met_factory () {};

  // getters
  float getMet()          { return met;         };
  float getMetSig()       { return metSig;      };
  float getMetCov00()     { return metcov00;    };
  float getMetCov10()     { return metcov10;    };
  float getMetCov11()     { return metcov11;    };
  float getMetCov01()     { return metcov01;    };
  float getMetPhi()       { return metphi;      };
  float getMetPx()        { return met_px;      };
  float getMetPy()        { return met_py;      };
  TLorentzVector getP4();

  // setters
  void setMet(float Met)                  { met = Met;            };
  void setMetSig(float MetSig)            { metSig = MetSig;      };
  void setMetCov00(float MetCov00)        { metcov00 = MetCov00;  };
  void setMetCov10(float MetCov10)        { metcov10 = MetCov10;  };
  void setMetCov11(float MetCov11)        { metcov11 = MetCov11;  };
  void setMetCov01(float MetCov01)        { metcov01 = MetCov01;  };
  void setMetPhi(float MetPhi)            { metphi = MetPhi;      };
  void setMetPx(float MetPx)              { met_px = MetPx;       };
  void setMetPy(float MetPy)              { met_py = MetPy;       };
};

// initialize member data and set TLorentzVector
met_factory::met_factory(TTree* input) {
  input -> SetBranchAddress( "met",         &met          );
  input -> SetBranchAddress( "metSig",      &metSig       );
  input -> SetBranchAddress( "metcov00",    &metcov00     );
  input -> SetBranchAddress( "metcov10",    &metcov10     );
  input -> SetBranchAddress( "metcov11",    &metcov11     );
  input -> SetBranchAddress( "metcov01",    &metcov01     );
  input -> SetBranchAddress( "metphi",      &metphi       );
  input -> SetBranchAddress( "met_px",      &met_px       );
  input -> SetBranchAddress( "met_py",      &met_py       );
}

TLorentzVector met_factory::getP4() {
  p4.SetPtEtaPhiM(met, 0, metphi, 0);
  return p4;
}
