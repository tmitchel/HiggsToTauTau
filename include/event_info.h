
/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
private:
  Float_t genpX, genpY, genM, genpT, numGenJets, genweight, gen_Higgs_pt, gen_Higgs_mass; // gen
  Float_t npv, npu, rho, extramuon_veto, extraelec_veto;  // event
  Float_t matchEle25, filterEle25, passEle25; // etau trigger
  Float_t passDoubleTauCmbIso35, matchDoubleTauCmbIso35_1, filterDoubleTauCmbIso35_1, matchDoubleTauCmbIso35_2, filterDoubleTauCmbIso35_2;
  Float_t passDoubleTau35, matchDoubleTau35_1, filterDoubleTau35_1, matchDoubleTau35_2, filterDoubleTau35_2;
  Float_t m_sv, pt_sv; // SVFit
  Float_t Dbkg_VBF, Dbkg_ggH, Dbkg_ZH, Dbkg_WH, Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2; // MELA
  Float_t ME_sm_VBF, ME_bkg; // More MELA
  Int_t run, lumi;
  ULong64_t evt;

public:
  event_info (TTree*, std::string);
  virtual ~event_info () {};

  // tautau Trigger Info
  Bool_t getPassDoubleTauCmbIso35();
  Bool_t getPassDoubleTau35();

  // Event Info
  Float_t getNPV()          { return npv;             };
  Float_t getNPU()          { return npu;             };
  Float_t getRho()          { return rho;             };
  Float_t getMuonVeto()     { return extramuon_veto;  };
  Float_t getElectronVeto() { return extraelec_veto;  };
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

  // etau Trigger Info
  Float_t getMatchEle25()   { return matchEle25;      };
  Float_t getFilterEle25()  { return filterEle25;     };
  Float_t getPassEle25()    { return passEle25;       };

  // SVFit Info
  Float_t getMSV()          { return m_sv;            };
  Float_t getPtSV()         { return pt_sv;           };

  // MELA Info
  Float_t getDbkg_VBF()     { return Dbkg_VBF;        };
  Float_t getDbkg_ggH()     { return Dbkg_ggH;        };
  Float_t getDbkg_ZH()      { return Dbkg_ZH;         };
  Float_t getDbkg_WH()      { return Dbkg_WH;         };
  Float_t getPhi()          { return Phi;             };
  Float_t getPhi1()         { return Phi1;            };
  Float_t getCosTheta1()    { return costheta1;       };
  Float_t getCosTheta2()    { return costheta2;       };
  Float_t getCosThetaStar() { return costhetastar;    };
  Float_t getQ2V1()         { return Q2V1;            };
  Float_t getQ2V2()         { return Q2V2;            };
  Float_t getMELA_vbf()     { return ME_sm_VBF;       };
  Float_t getMELA_bkg()     { return ME_bkg;          };
};

// read data from trees into member variables
event_info::event_info(TTree* input, std::string syst) {
  auto m_sv_name("m_sv"), pt_sv_name("pt_sv");
  if (syst.find(m_sv_name) != std::string::npos) {
    m_sv_name = syst.c_str();
  } else if (syst.find(pt_sv_name) != std::string::npos) {
    pt_sv_name = syst.c_str();
  }

  input -> SetBranchAddress( m_sv_name       , &m_sv           );
  input -> SetBranchAddress( pt_sv_name      , &pt_sv          );
  input -> SetBranchAddress( "run"           , &run            );
  input -> SetBranchAddress( "lumi"          , &lumi           );
  input -> SetBranchAddress( "evt"           , &evt            );
  input -> SetBranchAddress( "npv"           , &npv            );
  input -> SetBranchAddress( "npu"           , &npu            );
  input -> SetBranchAddress( "extramuon_veto", &extramuon_veto );
  input -> SetBranchAddress( "extraelec_veto", &extraelec_veto );
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
  input -> SetBranchAddress( "Dbkg_VBF"      , &Dbkg_VBF       );
  input -> SetBranchAddress( "Dbkg_ggH"      , &Dbkg_ggH       );
  input -> SetBranchAddress( "Dbkg_ZH"       , &Dbkg_ZH        );
  input -> SetBranchAddress( "Dbkg_WH"       , &Dbkg_WH        );
  input -> SetBranchAddress( "Phi"           , &Phi            );
  input -> SetBranchAddress( "Phi1"          , &Phi1           );
  input -> SetBranchAddress( "costheta1"     , &costheta1      );
  input -> SetBranchAddress( "costheta2"     , &costheta2      );
  input -> SetBranchAddress( "costhetastar"  , &costhetastar   );
  input -> SetBranchAddress( "Q2V1"          , &Q2V1           );
  input -> SetBranchAddress( "Q2V2"          , &Q2V2           );
  input -> SetBranchAddress( "ME_sm_VBF"     , &ME_sm_VBF      );
  input -> SetBranchAddress( "ME_bkg"        , &ME_bkg         );

  input -> SetBranchAddress( "passDoubleTauCmbIso35"    , &passDoubleTauCmbIso35      );
  input -> SetBranchAddress( "matchDoubleTauCmbIso35_1" , &matchDoubleTauCmbIso35_1   );
  input -> SetBranchAddress( "filterDoubleTauCmbIso35_1", &filterDoubleTauCmbIso35_1  );
  input -> SetBranchAddress( "matchDoubleTauCmbIso35_2" , &matchDoubleTauCmbIso35_2   );
  input -> SetBranchAddress( "filterDoubleTauCmbIso35_2", &filterDoubleTauCmbIso35_2  );
  input -> SetBranchAddress( "passDoubleTau35"          , &passDoubleTau35            );
  input -> SetBranchAddress( "matchDoubleTau35_1"       , &matchDoubleTau35_1         );
  input -> SetBranchAddress( "filterDoubleTau35_1"      , &filterDoubleTau35_1        );
  input -> SetBranchAddress( "matchDoubleTau35_2"       , &matchDoubleTau35_2         );
  input -> SetBranchAddress( "filterDoubleTau35_2"      , &filterDoubleTau35_2        );
}

Bool_t event_info::getPassDoubleTauCmbIso35() {
  if (passDoubleTauCmbIso35 && (matchDoubleTauCmbIso35_1 || matchDoubleTauCmbIso35_2) && (filterDoubleTauCmbIso35_1 || filterDoubleTauCmbIso35_2)) {
    return true;
  }
  return false;
}

Bool_t event_info::getPassDoubleTau35() {
  if (passDoubleTau35 && (matchDoubleTau35_1 || matchDoubleTau35_2) && (filterDoubleTau35_1 || filterDoubleTau35_2)) {
    return true;
  }
  return false;
}

