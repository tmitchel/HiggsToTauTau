
/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
private:
  Float_t genpX, genpY, genM, genpT, numGenJets, genweight, gen_Higgs_pt, gen_Higgs_mass; // gen
  Float_t npv, npu, rho, m_sv, pt_sv, m_vis, pt_tt;  // event
  Float_t matchEle25, filterEle25, passEle25; // trigger
  Int_t run, lumi;
  ULong64_t evt;

public:
  event_info (TTree*);
  virtual ~event_info () {};

  // Event Info
  Float_t getNPV()          { return npv;             };
  Float_t getNPU()          { return npu;             };
  Float_t getRho()          { return rho;             };
  Float_t getMSV()          { return m_sv;            };
  Float_t getPtSV()         { return pt_sv;           };
  Float_t getVisM()         { return m_vis;           };
  Float_t getPtDiTau()      { return pt_tt;           };
  Int_t getRun()            { return run;             };
  Int_t getLumi()           { return lumi;            };
  ULong64_t getEvt()        { return evt;             };

  // Generator Info
  Float_t getGenPx()        { return genpX;           };
  Float_t getGenPy()        { return genpY;           };
  Float_t getGenM()         { return genM;            };
  Float_t getGenPt()        { return genpT;           };
  Float_t getNumGenJets()   { return numGenJets;      };
  Float_t getGenWeight()    { return genweight;       };
  Float_t getGenHiggsPt()   { return gen_Higgs_pt;    };
  Float_t getGenHiggsMass() { return gen_Higgs_mass;  };

  // Trigger Info
  Float_t getMatchEle25()   { return matchEle25;      };
  Float_t getFilterEle25()  { return filterEle25;     };
  Float_t getPassEle25()    { return passEle25;       };
};

// read data from trees into member variables
event_info::event_info(TTree* input) {
  input -> SetBranchAddress( "run"           , &run            );
  input -> SetBranchAddress( "lumi"          , &lumi           );
  input -> SetBranchAddress( "evt"           , &evt            );
  input -> SetBranchAddress( "npv"           , &npv            );
  input -> SetBranchAddress( "npu"           , &npu            );
  input -> SetBranchAddress( "m_sv"          , &m_sv           );
  input -> SetBranchAddress( "pt_sv"         , &pt_sv          );
  input -> SetBranchAddress( "m_vis"         , &m_vis          );
  input -> SetBranchAddress( "pt_tt"         , &pt_tt          );
  input -> SetBranchAddress( "genpX"         , &genpX          );
  input -> SetBranchAddress( "genpY"         , &genpY          );
  input -> SetBranchAddress( "genM"          , &genM           );
  input -> SetBranchAddress( "genpT"         , &genpT          );
  input -> SetBranchAddress( "numGenJets"    , &numGenJets     );
  input -> SetBranchAddress( "genweight"     , &genweight      );
  input -> SetBranchAddress( "gen_Higgs_pt"  , &gen_Higgs_pt   );
  input -> SetBranchAddress( "gen_Higgs_mass", &gen_Higgs_mass );
  input -> SetBranchAddress( "matchEle25"    , &matchEle25     );
  input -> SetBranchAddress( "filterEle25"   , &filterEle25    );
  input -> SetBranchAddress( "passEle25"     , &passEle25      );
}
