#include "TTree.h"

class met_factory {
private:
  Float_t met, metphi, met_py, met_px;
  Float_t metSig, metcov00, metcov10, metcov11, metcov01;
  // Variable for systematics
  Float_t met_UESUp, met_UESDown, metphi_UESUp, metphi_UESDown;
  Float_t met_JESUp, met_JESDown, metphi_JESUp, metphi_JESDown;
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

  // get systematics
  Float_t getMet_UESUp()      { return met_UESUp;      };
  Float_t getMet_UESDown()    { return met_UESDown;    };
  Float_t getMet_JESUp()      { return met_JESUp;      };
  Float_t getMet_JESDown()    { return met_JESDown;    };
  Float_t getMetPhi_UESUp()   { return metphi_UESUp;   };
  Float_t getMetPhi_UESDown() { return metphi_UESDown; };
  Float_t getMetPhi_JESUp()   { return metphi_JESUp;   };
  Float_t getMetPhi_JESDown() { return metphi_JESDown; };
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

  // Systematics related variables
  input ->  SetBranchAddress("met_UESUp",       &met_UESUp      );
  input ->  SetBranchAddress("met_UESDown",     &met_UESDown    );
  input ->  SetBranchAddress("metphi_UESUp",    &metphi_UESUp   );
  input ->  SetBranchAddress("metphi_UESDown",  &metphi_UESDown );
  input ->  SetBranchAddress("met_JESUp",       &met_JESUp      );
  input ->  SetBranchAddress("met_JESDown",     &met_JESDown    );
  input ->  SetBranchAddress("metphi_JESUp",    &metphi_JESUp   );
  input ->  SetBranchAddress("metphi_JESDown",  &metphi_JESDown );
}

TLorentzVector met_factory::getP4() {
  p4.SetPtEtaPhiM(met, 0, metphi, 0);
  return p4;
}
