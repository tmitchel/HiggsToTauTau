#include "TTree.h"

class met_factory {
private:
  Float_t met, metphi, met_py, met_px;
  Float_t metSig, metcov00, metcov10, metcov11, metcov01;
  TLorentzVector p4;

public:
  met_factory (TTree*, std::string);
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
};

// initialize member data and set TLorentzVector
met_factory::met_factory(TTree* input, std::string syst) {
  auto met_name("met"), metphi_name("metphi");
  if (syst.find(met_name) != std::string::npos) {
    met_name = syst.c_str();
  } else if (syst.find(metphi_name) != std::string::npos) {
    metphi_name = syst.c_str();
  }
  input -> SetBranchAddress( met_name,      &met          );
  input -> SetBranchAddress( metphi_name,   &metphi       );
  input -> SetBranchAddress( "metSig",      &metSig       );
  input -> SetBranchAddress( "metcov00",    &metcov00     );
  input -> SetBranchAddress( "metcov10",    &metcov10     );
  input -> SetBranchAddress( "metcov11",    &metcov11     );
  input -> SetBranchAddress( "metcov01",    &metcov01     );
  input -> SetBranchAddress( "met_px",      &met_px       );
  input -> SetBranchAddress( "met_py",      &met_py       );
}

TLorentzVector met_factory::getP4() {
  p4.SetPtEtaPhiM(met, 0, metphi, 0);
  return p4;
}
