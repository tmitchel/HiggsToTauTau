#ifndef JET_FACTORY_H
#define JET_FACTORY_H

#include "TTree.h"

class jet {
private:
  Float_t pt, eta, phi, csv, flavor;
  TLorentzVector p4;

public:
  jet (Float_t,Float_t,Float_t,Float_t,Float_t);
  virtual ~jet() {};

  Float_t getPt()        { return pt;      };
  Float_t getEta()       { return eta;     };
  Float_t getPhi()       { return phi;     };
  Float_t getCSV()       { return csv;     };
  Float_t getFlavor()    { return flavor;  };
  TLorentzVector getP4() { return p4; };
};

// initialize member data and set TLorentzVector
jet::jet(Float_t Pt, Float_t Eta, Float_t Phi, Float_t Csv, Float_t Flavor=-9999) :
  pt(Pt),
  eta(Eta),
  phi(Phi),
  csv(Csv),
  flavor(Flavor)
  {
    p4.SetPtEtaPhiM(Pt, Eta, Phi, 0.);
  }

class jet_factory {
private:
  Float_t mjj;
  Float_t jpt_1, jeta_1, jphi_1, jcsv_1;
  Float_t jpt_2, jeta_2, jphi_2, jcsv_2;
  Float_t bpt_1, beta_1, bphi_1, bcsv_1;
  Float_t bpt_2, beta_2, bphi_2, bcsv_2;
  Float_t topQuarkPt1, topQuarkPt2;
  Int_t nbtag, njetspt20, njets;
  std::vector<jet> plain_jets, btag_jets;

public:
  jet_factory (TTree*, std::string);
  virtual ~jet_factory () {};
  void run_factory();

  // getters
  Int_t getNbtag()                { return nbtag;      };
  Int_t getNjets()                { return njets;      };
  Int_t getNjetPt20()             { return njetspt20;  };
  Float_t getDijetMass()          { return mjj;        };
  Float_t getTopPt1()             { return topQuarkPt1;};
  Float_t getTopPt2()             { return topQuarkPt2;};
  std::vector<jet> getJets()      { return plain_jets; };
  std::vector<jet> getBtagJets()  { return btag_jets;  };
};

// read data from tree into member variables
jet_factory::jet_factory(TTree* input, std::string syst) {
  auto mjj_name("mjj"), njets_name("njets");
  if (syst.find(mjj_name) != std::string::npos) {
    mjj_name = syst.c_str();
  } else if (syst.find(njets_name) != std::string::npos) {
    njets_name = syst.c_str();
  }

  input -> SetBranchAddress ( mjj_name,    &mjj       );
  input -> SetBranchAddress ( njets_name,  &njets     );
  input -> SetBranchAddress ( "nbtag",     &nbtag     );
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
  input -> SetBranchAddress ( "bpt_2",     &bpt_2     );
  input -> SetBranchAddress ( "beta_2",    &beta_2    );
  input -> SetBranchAddress ( "bphi_2",    &bphi_2    );
  input -> SetBranchAddress ( "bcsv_2",    &bcsv_2    );
  input -> SetBranchAddress ( "topQuarkPt1", &topQuarkPt1);
  input -> SetBranchAddress ( "topQuarkPt2", &topQuarkPt2);

}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
  plain_jets.clear();
  btag_jets.clear();

  jet j1(jpt_1, jeta_1, jphi_1, jcsv_1);
  jet j2(jpt_2, jeta_2, jphi_2, jcsv_2);
  jet b1(bpt_1, beta_1, bphi_1, bcsv_1);
  jet b2(bpt_2, beta_2, bphi_2, bcsv_2);
  plain_jets.push_back(j1);
  plain_jets.push_back(j2);
  btag_jets.push_back(b1);
  btag_jets.push_back(b2);
}


#endif