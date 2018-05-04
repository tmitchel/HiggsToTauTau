#include "TTree.h"

class trigger {
private:
  Float_t matchEle25eta2p1Tight_1, matchEle27eta2p1Loose_1, matchEle45L1TauJet_1, matchEle115_1, matchEle24Tau20sL1_1;
  Float_t matchEle24Tau20_1, matchEle24Tau30_1, filterEle25eta2p1Tight_1, filterEle27eta2p1Loose_1, filterEle45L1TauJet_1;
  Float_t filterEle115_1, filterEle24Tau20sL1_1, filterEle24Tau20_1, filterEle24Tau30_1;
  Float_t passEle25eta2p1Tight, passEle27eta2p1Loose, passEle45L1TauJet, passEle115, passEle24Tau20sL1, passEle24Tau20;
  Float_t passEle24Tau30, matchEle24Tau20sL1_2, matchEle24Tau20_2, matchEle24Tau30_2, filterEle24Tau20sL1_2, filterEle24Tau20_2;
  Float_t filterEle24Tau30_2;

public:
  trigger (TTree*);
  virtual ~trigger () {};

  // electron
  float getMatchEle25eta2p1Tight()  { return matchEle25eta2p1Tight_1; };
  float getMatchEle27eta2p1Loose()  { return matchEle27eta2p1Loose_1; };
  float getMatchEle45L1TauJet()     { return matchEle45L1TauJet_1; };
  float getMatchEle115()            { return matchEle115_1; };
  float getMatchEle24Tau20sL1_1()   { return matchEle24Tau20sL1_1; };
  float getMatchEle24Tau20_1()      { return matchEle24Tau20_1; };
  float getMatchEle24Tau30_1()      { return matchEle24Tau30_1; };

  float getFilterEle25eta2p1Tight() { return filterEle25eta2p1Tight_1; };
  float getFilterEle27eta2p1Loose() { return filterEle27eta2p1Loose_1; };
  float getFilterEle45L1TauJet()    { return filterEle45L1TauJet_1; };
  float getFilterEle115()           { return filterEle115_1; };
  float getFilterEle24Tau20sL1_1()  { return filterEle24Tau20sL1_1; };
  float getFilterEle24Tau20_1()     { return filterEle24Tau20_1; };
  float getFilterEle24Tau30_1()     { return filterEle24Tau30_1; };

  // taus
  float getPassEle25eta2p1Tight()   { return passEle25eta2p1Tight; };
  float getPassEle27eta2p1Loose()   { return passEle27eta2p1Loose; };
  float getPassEle45L1TauJet()      { return passEle45L1TauJet; };
  float getPassEle115()             { return passEle115; };
  float getPassEle24Tau20sL1()      { return passEle24Tau20sL1; };
  float getPassEle24Tau20()         { return passEle24Tau20; };
  float getPassEle24Tau30()         { return passEle24Tau30; };

  float getMatchEle24Tau20sL1_2()   { return matchEle24Tau20sL1_2; };
  float getMatchEle24Tau20_2()      { return matchEle24Tau20_2; };
  float getMatchEle24Tau30_2()      { return matchEle24Tau30_2; };

  float getFilterEle24Tau20sL1_2()  { return filterEle24Tau20sL1_2; };
  float getFilterEle24Tau20_2()     { return filterEle24Tau20_2; };
  float getFilterEle24Tau30_2()     { return filterEle24Tau30_2; };

};


trigger::trigger(TTree* input) {
  input -> SetBranchAddress( "matchEle25eta2p1Tight_1",  &matchEle25eta2p1Tight_1  );
  input -> SetBranchAddress( "matchEle25eta2p1Tight_1",  &matchEle25eta2p1Tight_1  );
  input -> SetBranchAddress( "matchEle45L1TauJet_1",     &matchEle45L1TauJet_1     );
  input -> SetBranchAddress( "matchEle115_1",            &matchEle115_1            );
  input -> SetBranchAddress( "matchEle24Tau20sL1_1",     &matchEle24Tau20sL1_1     );
  input -> SetBranchAddress( "matchEle24Tau20_1",        &matchEle24Tau20_1        );
  input -> SetBranchAddress( "matchEle24Tau30_1",        &matchEle24Tau30_1        );
  input -> SetBranchAddress( "filterEle25eta2p1Tight_1", &filterEle25eta2p1Tight_1 );
  input -> SetBranchAddress( "filterEle27eta2p1Loose_1", &filterEle27eta2p1Loose_1 );
  input -> SetBranchAddress( "filterEle45L1TauJet_1",    &filterEle45L1TauJet_1    );
  input -> SetBranchAddress( "filterEle115_1",           &filterEle115_1           );
  input -> SetBranchAddress( "filterEle24Tau20sL1_1",    &filterEle24Tau20sL1_1    );
  input -> SetBranchAddress( "filterEle24Tau20_1",       &filterEle24Tau20_1       );
  input -> SetBranchAddress( "filterEle24Tau30_1",       &filterEle24Tau30_1       );

  input -> SetBranchAddress( "passEle25eta2p1Tight",     &passEle25eta2p1Tight     );
  input -> SetBranchAddress( "passEle27eta2p1Loose",     &passEle27eta2p1Loose     );
  input -> SetBranchAddress( "passEle45L1TauJet",        &passEle45L1TauJet        );
  input -> SetBranchAddress( "passEle115",               &passEle115               );
  input -> SetBranchAddress( "passEle24Tau20sL1",        &passEle24Tau20sL1        );
  input -> SetBranchAddress( "passEle24Tau20",           &passEle24Tau20           );
  input -> SetBranchAddress( "passEle24Tau30",           &passEle24Tau30           );
  input -> SetBranchAddress( "matchEle24Tau20sL1_2",     &matchEle24Tau20sL1_2     );
  input -> SetBranchAddress( "matchEle24Tau20_2",        &matchEle24Tau20_2        );
  input -> SetBranchAddress( "matchEle24Tau30_2",        &matchEle24Tau30_2        );
  input -> SetBranchAddress( "filterEle24Tau20sL1_2",    &filterEle24Tau20sL1_2    );
  input -> SetBranchAddress( "filterEle24Tau20_2",       &filterEle24Tau20_2       );
  input -> SetBranchAddress( "filterEle24Tau30_2",       &filterEle24Tau30_2       );
}
