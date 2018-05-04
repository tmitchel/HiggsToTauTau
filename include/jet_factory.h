#include "TTree.h"

class jet {
private:
  float pt, eta, phi, csv;

public:
  jet (float,float,float,float);
  virtual ~jet() {};

  float getPt()  { return pt; };
  float getEta() { return eta; };
  float getPhi() { return phi; };
  float getCSV() { return csv; };
};

jet::jet(float Pt, float Eta, float Phi, float Csv) :
  pt(Pt),
  eta(Eta),
  phi(Phi),
  csv(Csv)
  {}

class jet_factory {
private:
  float mjj, jpt_1, jeta_1, jphi_1, jcsv_1, jpt_2, jeta_2, jphi_2, jcsv_2;
  int nbtag, njets, njetspt20;
  std::vector<jet> collection;

public:
  jet_factory (TTree*);
  virtual ~jet_factory () {};

  std::vector<jet> run_factory();
  int getNbtag()       { return nbtag; };
  int getNjets()       { return njets; };
  int getNjetPt20()    { return njetspt20; };
  float getDijetMass() { return mjj; };
  
  std::vector<jet> getCollection() { return collection; };
};

jet_factory::jet_factory(TTree* input) {
  input -> SetBranchAddress ( "mjj",       &mjj );
  input -> SetBranchAddress ( "nbtag",     &nbtag );
  input -> SetBranchAddress ( "njets",     &njets );
  input -> SetBranchAddress ( "njetspt20", &njetspt20 );
  input -> SetBranchAddress ( "jpt_1",     &jpt_1 );
  input -> SetBranchAddress ( "jeta_1",    &jeta_1 );
  input -> SetBranchAddress ( "jphi_1",    &jphi_1 );
  input -> SetBranchAddress ( "jcsv_1",    &jcsv_1 );
  input -> SetBranchAddress ( "jpt_2",     &jpt_2 );
  input -> SetBranchAddress ( "jeta_2",    &jeta_2 );
  input -> SetBranchAddress ( "jphi_2",    &jphi_2 );
  input -> SetBranchAddress ( "jcsv_2",    &jcsv_2 );
}

std::vector<jet> jet_factory::run_factory() {
  collection.clear();

  jet j1(jpt_1, jeta_1, jphi_1, jcsv_1);
  jet j2(jpt_2, jeta_2, jphi_2, jcsv_2);
  collection.push_back(j1);
  collection.push_back(j2);

  return collection;
}
