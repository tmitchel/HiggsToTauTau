
/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
private:
  float npv, npu, rho, m_sv, pt_sv, m_vis, pt_tt, l2_decayMode;
  int run, lumi;
  ULong64_t evt;

public:
  event_info (TTree*);
  virtual ~event_info () {};

  // getters
  float getNPV()     { return npv;          };
  float getNPU()     { return npu;          };
  float getRho()     { return rho;          };
  float getMSV()     { return m_sv;         };
  float getPtSV()    { return pt_sv;        };
  float getVisM()    { return m_vis;        };
  float getPtDiTau() { return pt_tt;        };
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
  input -> SetBranchAddress( "m_sv"  ,        &m_sv         );
  input -> SetBranchAddress( "pt_sv" ,        &pt_sv        );
  input -> SetBranchAddress( "m_vis" ,        &m_vis        );
  input -> SetBranchAddress( "pt_tt" ,        &pt_tt        );
  input -> SetBranchAddress( "l2_decayMode" , &l2_decayMode );
}
