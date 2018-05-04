#include "TTree.h"

class met_factory {
private:
  float met, metSig, metcov00, metcov10, metcov11, metcov01, metcov00_v2, metcov10_v2;
  float metcov11_v2, metcov01_v2, metphi, met_py, met_px;

public:
  met_factory (TTree*);
  virtual ~met_factory () {};

  // getters
  float getMet()          { return met; };
  float getMetSig()       { return metSig; };
  float getMetCov00()     { return metcov00; };
  float getMetCov10()     { return metcov10; };
  float getMetCov11()     { return metcov11; };
  float getMetCov01()     { return metcov01; };
  float getMetCov00_v2()  { return metcov00_v2; };
  float getMetCov10_v2()  { return metcov10_v2; };
  float getMetCov11_v2()  { return metcov11_v2; };
  float getMetCov01_v2()  { return metcov01_v2; };
  float getMetPhi()       { return metphi; };
  float getMetPx()        { return met_px; };
  float getMetPy()        { return met_py; };

  // setters
  void setMet(float Met)                  { met = Met;                  };
  void setMetSig(float MetSig)            { metSig = MetSig;            };
  void setMetCov00(float MetCov00)        { metcov00 = MetCov00;        };
  void setMetCov10(float MetCov10)        { metcov10 = MetCov10;        };
  void setMetCov11(float MetCov11)        { metcov11 = MetCov11;        };
  void setMetCov01(float MetCov01)        { metcov01 = MetCov01;        };
  void setMetCov00_v2(float MetCov00_v2)  { metcov00_v2 = MetCov00_v2;  };
  void setMetCov10_v2(float MetCov10_v2)  { metcov10_v2 = MetCov10_v2;  };
  void setMetCov11_v2(float MetCov11_v2)  { metcov11_v2 = MetCov11_v2;  };
  void setMetCov01_v2(float MetCov01_v2)  { metcov01_v2 = MetCov01_v2;  };
  void setMetPhi(float MetPhi)            { metphi = MetPhi;            };
  void setMetPx(float MetPx)              { met_px = MetPx;             };
  void setMetPy(float MetPy)              { met_py = MetPy;             };
};

met_factory::met_factory(TTree* input) {
  input -> SetBranchAddress( "met",         &met          );
  input -> SetBranchAddress( "metSig",      &metSig       );
  input -> SetBranchAddress( "metcov00",    &metcov00     );
  input -> SetBranchAddress( "metcov10",    &metcov10     );
  input -> SetBranchAddress( "metcov11",    &metcov11     );
  input -> SetBranchAddress( "metcov01",    &metcov01     );
  input -> SetBranchAddress( "metcov00_v2", &metcov00_v2  );
  input -> SetBranchAddress( "metcov10_v2", &metcov10_v2  );
  input -> SetBranchAddress( "metcov11_v2", &metcov11_v2  );
  input -> SetBranchAddress( "metcov01_v2", &metcov01_v2  );
  input -> SetBranchAddress( "metphi",      &metphi       );
  input -> SetBranchAddress( "met_px",      &met_px       );
  input -> SetBranchAddress( "met_py",      &met_py       );
}