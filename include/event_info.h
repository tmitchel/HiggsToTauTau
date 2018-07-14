
/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
private:
  float npv, npu, rho, l2_decayMode;
  int run, lumi;
  ULong64_t evt;

public:
  event_info (TTree*);
  virtual ~event_info () {};

  // getters
  float getNPV()     { return npv;          };
  float getNPU()     { return npu;          };
  float getRho()     { return rho;          };
  int getRun()       { return run;          };
  int getLumi()      { return lumi;         };
  int getEvt()       { return evt;          };
  int getDecayMode() { return l2_decayMode; };

};

// read data from trees into member variables
event_info::event_info(TTree* input) {
  input -> SetBranchAddress( "run"   ,        &run          );
  input -> SetBranchAddress( "lumi"  ,        &lumi         );
  input -> SetBranchAddress( "evt"   ,        &evt          );
  input -> SetBranchAddress( "npv"   ,        &npv          );
  input -> SetBranchAddress( "npu"   ,        &npu          );
  input -> SetBranchAddress( "l2_decayMode" , &l2_decayMode );
}
