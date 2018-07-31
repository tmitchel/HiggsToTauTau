#include "TTree.h"

class met_factory {
private:
  Float_t met, metSig, metcov00, metcov10, metcov11, metcov01;
  Float_t metphi, met_py, met_px;
  TLorentzVector p4;

public:
  met_factory (TTree*);
  virtual ~met_factory () {};

  // getters
  Float_t getMet()          { return met;         };
  Float_t getMetSig()       { return metSig;      };
  Float_t getMetCov00()     { return metcov00;    };
  Float_t getMetCov10()     { return metcov10;    };
  Float_t getMetCov11()     { return metcov11;    };
  Float_t getMetCov01()     { return metcov01;    };
  Float_t getMetPhi()       { return metphi;      };
  Float_t getMetPx()        { return met_px;      };
  Float_t getMetPy()        { return met_py;      };
  TLorentzVector getP4();

  // setters
  void setMet(Float_t Met)                  { met = Met;            };
  void setMetSig(Float_t MetSig)            { metSig = MetSig;      };
  void setMetCov00(Float_t MetCov00)        { metcov00 = MetCov00;  };
  void setMetCov10(Float_t MetCov10)        { metcov10 = MetCov10;  };
  void setMetCov11(Float_t MetCov11)        { metcov11 = MetCov11;  };
  void setMetCov01(Float_t MetCov01)        { metcov01 = MetCov01;  };
  void setMetPhi(Float_t MetPhi)            { metphi = MetPhi;      };
  void setMetPx(Float_t MetPx)              { met_px = MetPx;       };
  void setMetPy(Float_t MetPy)              { met_py = MetPy;       };
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
