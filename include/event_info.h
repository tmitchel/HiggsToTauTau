
/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
private:
  Float_t genpX, genpY, genM, genpT, numGenJets, genweight, amcatNLO_weight; // gen
  Float_t npv, npu, rho, extramuon_veto, extraelec_veto;  // event
  Float_t matchEle25, filterEle25, passEle25; // etau trigger
  Float_t passDoubleTauCmbIso35, matchDoubleTauCmbIso35_1, filterDoubleTauCmbIso35_1, matchDoubleTauCmbIso35_2, filterDoubleTauCmbIso35_2; // tt trigger
  Float_t passDoubleTau35, matchDoubleTau35_1, filterDoubleTau35_1, matchDoubleTau35_2, filterDoubleTau35_2;                               // tt trigger
  Float_t matchIsoMu19Tau20_1, matchIsoMu19Tau20_2, filterIsoMu19Tau20_1, filterIsoMu19Tau20_2, passIsoMu19Tau20; // cross trigger
  Float_t matchIsoMu22_1, filterIsoMu22_1, passIsoMu22;                         // single muon trigger
  Float_t matchIsoTkMu22_1, filterIsoTkMu22_1, passIsoTkMu22;                   // single muon trigger
  Float_t matchIsoMu22eta2p1_1, filterIsoMu22eta2p1_1, passIsoMu22eta2p1;       // single muon trigger
  Float_t matchIsoTkMu22eta2p1_1, filterIsoTkMu22eta2p1_1, passIsoTkMu22eta2p1; // single muon trigger
  Bool_t PassEle25, PassDoubleTauCmbIso35, PassDoubleTau35, PassIsoMu19Tau20, PassIsoMu22, PassIsoTkMu22, PassIsoMu22eta2p1, PassIsoTkMu22eta2p1;
  Float_t m_sv, pt_sv; // SVFit
  Float_t Dbkg_VBF, Dbkg_ggH, Dbkg_ZH, Dbkg_WH, Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2;  // MELA
  Float_t ME_sm_VBF, ME_bkg;                                                                                // MELA
  UInt_t run, lumi;
  ULong64_t evt;

public:
  event_info (TTree*, std::string, std::string);
  virtual ~event_info () {};
  void setEmbed();

  // tautau Trigger Info
  Bool_t getPassEle25();
  Bool_t getPassDoubleTauCmbIso35();
  Bool_t getPassDoubleTau35();
  Bool_t getPassCrossTrigger();
  Bool_t getPassIsoMu22();
  Bool_t getPassIsoTkMu22();
  Bool_t getPassIsoMu22eta2p1();
  Bool_t getPassIsoTkMu22eta2p1();

  // Event Info
  Float_t getNPV()          { return npv;             };
  Float_t getNPU()          { return npu;             };
  Float_t getRho()          { return rho;             };
  Float_t getMuonVeto()     { return extramuon_veto;  };
  Float_t getElectronVeto() { return extraelec_veto;  };
  UInt_t getRun()            { return run;             };
  UInt_t getLumi()           { return lumi;            };
  ULong64_t getEvt()        { return evt;             };

  // Generator Info
  Float_t getGenPx()        { return genpX;           };
  Float_t getGenPy()        { return genpY;           };
  Float_t getGenM()         { return genM;            };
  Float_t getGenPt()        { return genpT;           };
  Float_t getNumGenJets()   { return numGenJets;      };
  Float_t getGenWeight()    { return genweight;       };
  Float_t getNLOWeight()    { return amcatNLO_weight; };

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
event_info::event_info(TTree* input, std::string syst, std::string analyzer) {
  auto m_sv_name("m_sv"), pt_sv_name("pt_sv");
  if (syst.find(m_sv_name) != std::string::npos) {
    m_sv_name = syst.c_str();
  } else if (syst.find(pt_sv_name) != std::string::npos) {
    pt_sv_name = syst.c_str();
  }

  input -> SetBranchAddress( m_sv_name        , &m_sv            );
  input -> SetBranchAddress( pt_sv_name       , &pt_sv           );
  input -> SetBranchAddress( "run"            , &run             );
  input -> SetBranchAddress( "lumi"           , &lumi            );
  input -> SetBranchAddress( "evt"            , &evt             );
  input -> SetBranchAddress( "npv"            , &npv             );
  input -> SetBranchAddress( "npu"            , &npu             );
  input -> SetBranchAddress( "extramuon_veto" , &extramuon_veto  );
  input -> SetBranchAddress( "extraelec_veto" , &extraelec_veto  );
  input -> SetBranchAddress( "genpX"          , &genpX           );
  input -> SetBranchAddress( "genpY"          , &genpY           );
  input -> SetBranchAddress( "genM"           , &genM            );
  input -> SetBranchAddress( "genpT"          , &genpT           );
  input -> SetBranchAddress( "numGenJets"     , &numGenJets      );
  input -> SetBranchAddress( "genweight"      , &genweight       );
  input -> SetBranchAddress( "Dbkg_VBF"       , &Dbkg_VBF        );
  input -> SetBranchAddress( "Dbkg_ggH"       , &Dbkg_ggH        );
  input -> SetBranchAddress( "Dbkg_ZH"        , &Dbkg_ZH         );
  input -> SetBranchAddress( "Dbkg_WH"        , &Dbkg_WH         );
  input -> SetBranchAddress( "Phi"            , &Phi             );
  input -> SetBranchAddress( "Phi1"           , &Phi1            );
  input -> SetBranchAddress( "costheta1"      , &costheta1       );
  input -> SetBranchAddress( "costheta2"      , &costheta2       );
  input -> SetBranchAddress( "costhetastar"   , &costhetastar    );
  input -> SetBranchAddress( "Q2V1"           , &Q2V1            );
  input -> SetBranchAddress( "Q2V2"           , &Q2V2            );
  input -> SetBranchAddress( "ME_sm_VBF"      , &ME_sm_VBF       );
  input -> SetBranchAddress( "ME_bkg"         , &ME_bkg          );
  input -> SetBranchAddress( "amcatNLO_weight", &amcatNLO_weight );

  if (analyzer == "et") {
    input -> SetBranchAddress( "matchEle25" , &matchEle25  );
    input -> SetBranchAddress( "filterEle25", &filterEle25 );
    input -> SetBranchAddress( "passEle25"  , &passEle25   );
  } else if (analyzer == "mt") {
    input -> SetBranchAddress( "matchIsoMu19Tau20_1"     , &matchIsoMu19Tau20_1     );
    input -> SetBranchAddress( "matchIsoMu19Tau20_2"     , &matchIsoMu19Tau20_2     );
    input -> SetBranchAddress( "filterIsoMu19Tau20_1"    , &filterIsoMu19Tau20_1    );
    input -> SetBranchAddress( "filterIsoMu19Tau20_2"    , &filterIsoMu19Tau20_2    );
    input -> SetBranchAddress( "passIsoMu19Tau20"        , &passIsoMu19Tau20        );
    input -> SetBranchAddress( "matchIsoMu22_1"          , &matchIsoMu22_1          );
    input -> SetBranchAddress( "filterIsoMu22_1"         , &filterIsoMu22_1         );
    input -> SetBranchAddress( "passIsoMu22"             , &passIsoMu22             );
    input -> SetBranchAddress( "matchIsoTkMu22_1"        , &matchIsoTkMu22_1        );
    input -> SetBranchAddress( "filterIsoTkMu22_1"       , &filterIsoTkMu22_1       );
    input -> SetBranchAddress( "passIsoTkMu22"           , &passIsoTkMu22           );
    input -> SetBranchAddress( "matchIsoMu22eta2p1_1"    , &matchIsoMu22eta2p1_1    );
    input -> SetBranchAddress( "filterIsoMu22eta2p1_1"   , &filterIsoMu22eta2p1_1   );
    input -> SetBranchAddress( "passIsoMu22eta2p1"       , &passIsoMu22eta2p1       );
    input -> SetBranchAddress( "matchIsoTkMu22eta2p1_1"  , &matchIsoTkMu22eta2p1_1  );
    input -> SetBranchAddress( "filterIsoTkMu22eta2p1_1" , &filterIsoTkMu22eta2p1_1 );
    input -> SetBranchAddress( "passIsoTkMu22eta2p1"     , &passIsoTkMu22eta2p1     );
  } else if (analyzer == "tt") {
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
  } else if (analyzer == "emu") {

  } else {
    std::cerr << "HEY! THAT'S NOT AN ANALYZER. WAT U DOIN." << std::endl;
  }

  PassEle25 = passEle25 && filterEle25 && matchEle25;
  PassDoubleTauCmbIso35 = passDoubleTauCmbIso35 && (matchDoubleTauCmbIso35_1 || matchDoubleTauCmbIso35_2) && (filterDoubleTauCmbIso35_1 || filterDoubleTauCmbIso35_2);
  PassDoubleTau35 = passDoubleTau35 && (matchDoubleTau35_1 || matchDoubleTau35_2) && (filterDoubleTau35_1 || filterDoubleTau35_2);
  PassIsoMu19Tau20 = passIsoMu19Tau20 && (matchIsoMu19Tau20_1 || matchIsoMu19Tau20_2) && (filterIsoMu19Tau20_1 || filterIsoMu19Tau20_2);
  PassIsoMu22 = passIsoMu22 && matchIsoMu22_1 && filterIsoMu22_1;
  PassIsoTkMu22 = passIsoTkMu22 && matchIsoTkMu22_1 && filterIsoTkMu22_1;
  PassIsoMu22eta2p1 = passIsoMu22eta2p1 && matchIsoMu22eta2p1_1 && filterIsoMu22eta2p1_1;
  PassIsoTkMu22eta2p1 = passIsoTkMu22eta2p1 && matchIsoTkMu22eta2p1_1 && filterIsoTkMu22eta2p1_1;
}

void event_info::setEmbed() {
  PassEle25 = passEle25;
  PassDoubleTauCmbIso35 = passDoubleTauCmbIso35;
  PassDoubleTau35 = passDoubleTau35;
  PassIsoMu19Tau20 = passIsoMu19Tau20;
  PassIsoMu22 = passIsoMu22;
  PassIsoTkMu22 = passIsoTkMu22;
  PassIsoMu22eta2p1 = passIsoMu22eta2p1;
  PassIsoTkMu22eta2p1 = passIsoTkMu22eta2p1;
}

Bool_t event_info::getPassEle25(){
  return PassEle25;
}

Bool_t event_info::getPassDoubleTauCmbIso35() {
  return PassDoubleTauCmbIso35;
}

Bool_t event_info::getPassDoubleTau35() {
  return PassDoubleTau35;
}

Bool_t event_info::getPassCrossTrigger() {
  return PassIsoMu19Tau20;
}

Bool_t event_info::getPassIsoMu22() {
  return PassIsoMu22;
}

Bool_t event_info::getPassIsoTkMu22() {
  return PassIsoTkMu22;
}

Bool_t event_info::getPassIsoMu22eta2p1() {
  return PassIsoMu22eta2p1;
}

Bool_t event_info::getPassIsoTkMu22eta2p1() {
  return PassIsoTkMu22eta2p1;
}