#include "TTree.h"

class jet {
private:
  float pt, eta, phi, csv, flavor;

public:
  jet (float,float,float,float,float);
  virtual ~jet() {};

  float getPt()     { return pt;      };
  float getEta()    { return eta;     };
  float getPhi()    { return phi;     };
  float getCSV()    { return csv;     };
  float getFlavor() { return flavor;  };
};

// initialize member data and set TLorentzVector
jet::jet(float Pt, float Eta, float Phi, float Csv, float Flavor=-9999) :
  pt(Pt),
  eta(Eta),
  phi(Phi),
  csv(Csv),
  flavor(Flavor)
  {}

class jet_factory {
private:
  float mjj;
  float jpt_1, jeta_1, jphi_1, jcsv_1;
  float jpt_2, jeta_2, jphi_2, jcsv_2;
  float bpt_1, beta_1, bphi_1, bcsv_1, bflavor_1;
  float bpt_2, beta_2, bphi_2, bcsv_2, bflavor_2;
  float pt_top1, pt_top2;
  int nbtag, njets, njetspt20;
  std::vector<jet> plain_jets;
  std::vector<jet> btag_jets;

public:
  jet_factory (TTree*);
  virtual ~jet_factory () {};
  void run_factory();

  // getters
  int getNbtag()                  { return nbtag;      };
  int getNjets()                  { return njets;      };
  int getNjetPt20()               { return njetspt20;  };
  float getDijetMass()            { return mjj;        };
  float getTopPt1()               { return pt_top1;    };
  float getTopPt2()               { return pt_top2;    };
  std::vector<jet> getJets()      { return plain_jets; };
  std::vector<jet> getBtagJets()  { return btag_jets;  };
};

// read data from tree into member variables
jet_factory::jet_factory(TTree* input) {
  input -> SetBranchAddress ( "mjj",       &mjj       );
  input -> SetBranchAddress ( "nbtag",     &nbtag     );
  input -> SetBranchAddress ( "njets",     &njets     );
  input -> SetBranchAddress ( "njetspt20", &njetspt20 );
  input -> SetBranchAddress ( "jpt_1",     &jpt_1     );
  input -> SetBranchAddress ( "jeta_1",    &jeta_1    );
  input -> SetBranchAddress ( "jphi_1",    &jphi_1    );
  input -> SetBranchAddress ( "jcsv_1",    &jcsv_1    );
  input -> SetBranchAddress ( "jpt_2",     &jpt_2     );
  input -> SetBranchAddress ( "jeta_2",    &jeta_2    );
  input -> SetBranchAddress ( "jphi_2",    &jphi_2    );
  input -> SetBranchAddress ( "jcsv_2",    &jcsv_2    );
  input -> SetBranchAddress ( "bpt_1",     &bpt_1     );
  input -> SetBranchAddress ( "beta_1",    &beta_1    );
  input -> SetBranchAddress ( "bphi_1",    &bphi_1    );
  input -> SetBranchAddress ( "bcsv_1",    &bcsv_1    );
  input -> SetBranchAddress ( "bflavor_1", &bflavor_1 );
  input -> SetBranchAddress ( "bpt_2",     &bpt_2     );
  input -> SetBranchAddress ( "beta_2",    &beta_2    );
  input -> SetBranchAddress ( "bphi_2",    &bphi_2    );
  input -> SetBranchAddress ( "bcsv_2",    &bcsv_2    );
  input -> SetBranchAddress ( "bflavor_2", &bflavor_2 );
  input -> SetBranchAddress ( "pt_top1",   &pt_top1   );
  input -> SetBranchAddress ( "pt_top2",   &pt_top2   );
}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
  plain_jets.clear();
  btag_jets.clear();

  jet j1(jpt_1, jeta_1, jphi_1, jcsv_1);
  jet j2(jpt_2, jeta_2, jphi_2, jcsv_2);
  jet b1(bpt_1, beta_1, bphi_1, bcsv_1, bflavor_1);
  jet b2(bpt_2, beta_2, bphi_2, bcsv_2, bflavor_2);
  plain_jets.push_back(j1);
  plain_jets.push_back(j2);
  btag_jets.push_back(b1);
  btag_jets.push_back(b2);

}
