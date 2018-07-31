
static double luminosity(35870.);
static std::map<std::string, double> cross_sections {
  {"DY", 5765.4},
  {"DY1", 5765.4},
  {"DY1_missingfiles", 5765.4},
  {"DY2", 5765.4},
  {"DY1_noFakeTauCorr", 5765.4},
  {"DY2_missingfiles", 5765.4},
  {"DY2_noFakeTauCorr", 5765.4},
  {"DY3", 5765.4},
  {"DY3_noFakeTauCorr", 5765.4},
  {"DY4", 5765.4},
  {"DY4_missingfiles", 5765.4},
  {"DY4_noFakeTauCorr", 5765.4},
  {"DY_missingfiles", 5765.4},
  {"DY_noFakeTauCorr", 5765.4},
  {"DYlow", 5765.4},
  {"EWKminus", 20.25},
  {"EWKplus", 25.62},
  {"EWKZLL", 3.987},
  {"EWKZNuNu", 10.01},
  {"HWW_gg125", 48.58*0.2137*0.3258},
  {"HWW_vbf125", 3.782*0.2137*0.3258},
  {"SMH_VBF110", 4.434*0.0791},
  {"SMH_VBF120", 3.935*0.0698},
  {"SMH_VBF125", 3.782*0.0627},
  {"SMH_VBF130", 3.637*0.0541},
  {"SMH_VBF130_missingfiles", 3.637*0.0541},
  {"SMH_VBF140", 3.492*0.0360},
  {"SMH_gg110", 57.90*0.0791},
  {"SMH_ggH120", 47.38*0.0698},
  {"SMH_ggH125", 44.14*0.0627},
  {"SMH_ggH130", 41.23*0.0541},
  {"SMH_gg140", 36.0*0.0360},
  {"ST_tW_antitop", 35.6},
  {"ST_tW_top", 35.6},
  {"ST_t_antitop", 26.23},
  {"ST_t_top", 44.07},
  {"TT", 831.76},
  {"TT_missingfiles", 831.76},
  {"VV2L2Nu", 11.95},
  {"W", 61526.7},
  {"W1", 61526.7},
  {"W2", 61526.7},
  {"W3", 61526.7},
  {"W4", 61526.7},
  {"WGToLNuG", 489.0},
  {"WGstarLNuEE", 3.526},
  {"WGstarLNuMuMu", 2.793},
  {"WW1L1Nu2Q", 49.997},
  {"WZ1L1Nu2Q", 10.71},
  {"WZ1L3Nu", 3.05},
  {"WZ2L2Q", 5.595},
  {"WZLLLNu", 4.708},
  {"WminusH110", 0.8587*0.0791},
  {"WminusH120", 0.6092*0.0698},
  {"WminusH125", 0.5328*0.0627},
  {"WminusH130", 0.4676*0.0541},
  {"WminusH140", 0.394*0.0360},
  {"WplusH110", 1.335*0.0791},
  {"WplusH120", 0.9558*0.0698},
  {"WplusH125", 0.840*0.0627},
  {"WplusH130", 0.7414*0.0541},
  {"WplusH140", 0.6308*0.0360},
  {"ZZ2L2Q", 3.22},
  {"ZZ4L", 1.212},
  {"data", 1.0}
};

// do the mt calculation
static float calculate_mt(electron* const el, float met_x, float met_y, float met_pt) {
  return sqrt(pow(el->getPt() + met_pt, 2) - pow(el->getPx() + met_x, 2) - pow(el->getPy() + met_y, 2));
}

static float deltaR(float eta1, float phi1, float eta2, float phi2) {
  return sqrt(pow(eta1-eta2, 2) + pow(phi1-phi2, 2));
}

void initHistos_1D(std::unordered_map<std::string, TH1D*>* histos) {
  histos->insert({"n70", new TH1D("n70", "n70", 6,0,6)});
  histos->insert({"cutflow", new TH1D("cutflow", "Cutflow", 12, -0.5, 11.5)});

  histos->insert({"htau_pt", new TH1D("tau_pt", "Tau p_{T};p_{T} [GeV];;", 40, 0., 200)});
  histos->insert({"htau_pt_QCD", new TH1D("tau_pt_QCD", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.)});
  histos->insert({"htau_pt_SS", new TH1D("tau_pt_SS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.)});
  histos->insert({"htau_pt_WOS", new TH1D("tau_pt_WOS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.)});
  histos->insert({"htau_pt_WSS", new TH1D("tau_pt_WSS", "Tau p_{T}; p_{T} [GeV]", 40, 0., 200.)});
  histos->insert({"htau_eta", new TH1D("tau_eta", "Tau #eta;#eta [GeV];;", 80, -4., 4.)});
  histos->insert({"htau_phi", new TH1D("tau_phi", "Tau #phi;#phi [GeV];;", 15, -3.14, 3.14)});
  histos->insert({"htau_phi_QCD", new TH1D("tau_phi_QCD", "Tau p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});
  histos->insert({"htau_phi_SS", new TH1D("tau_phi_SS", "Tau p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});
  histos->insert({"htau_phi_WOS", new TH1D("tau_phi_WOS", "Tau p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});
  histos->insert({"htau_phi_WSS", new TH1D("tau_phi_WSS", "Tau p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});

  histos->insert({"hel_pt", new TH1D("el_pt", "Electron p_{T};p_{T} [GeV];;", 20, 0., 100)});
  histos->insert({"hel_pt_QCD", new TH1D("el_pt_QCD", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.)});
  histos->insert({"hel_pt_SS", new TH1D("el_pt_SS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.)});
  histos->insert({"hel_pt_WOS", new TH1D("el_pt_WOS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.)});
  histos->insert({"hel_pt_WSS", new TH1D("el_pt_WSS", "Electron p_{T}; p_{T} [GeV]", 20, 0., 100.)});
  histos->insert({"hel_eta", new TH1D("el_eta", "Electron #eta;#eta [GeV];;", 80, -4., 4.)});
  histos->insert({"hel_phi", new TH1D("el_phi", "Electron #phi;#phi [GeV];;", 15, -3.14, 3.14)});
  histos->insert({"hel_phi_QCD", new TH1D("el_phi_QCD", "el p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});
  histos->insert({"hel_phi_SS", new TH1D("el_phi_SS", "el p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});
  histos->insert({"hel_phi_WOS", new TH1D("el_phi_WOS", "el p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});
  histos->insert({"hel_phi_WSS", new TH1D("el_phi_WSS", "el p_{T}; p_{T} [GeV]", 15, -3.14, 3.14)});

  histos->insert({"hmsv", new TH1D("msv", "SV Fit Mass; Mass [GeV];;", 100, 0, 300)});
  histos->insert({"hmsv_QCD", new TH1D("msv_QCD", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.)});
  histos->insert({"hmsv_SS", new TH1D("msv_SS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.)});
  histos->insert({"hmsv_WOS", new TH1D("msv_WOS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.)});
  histos->insert({"hmsv_WSS", new TH1D("msv_WSS", "SV Fit Mass; Mass [GeV];;", 100, 0., 300.)});

  histos->insert({"hmet", new TH1D("met", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});
  histos->insert({"hmet_QCD", new TH1D("met_QCD", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});
  histos->insert({"hmet_SS", new TH1D("met_SS", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});
  histos->insert({"hmet_WOS", new TH1D("met_WOS", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});
  histos->insert({"hmet_WSS", new TH1D("met_WSS", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});

  histos->insert({"hmt", new TH1D("mt", "MT", 50, 0, 100)});
  histos->insert({"hmt_QCD", new TH1D("mt_QCD", "MT", 50, 0, 100)});
  histos->insert({"hmt_SS", new TH1D("mt_SS", "MT", 50, 0, 100)});
  histos->insert({"hmt_WOS", new TH1D("mt_WOS", "MT", 50, 0, 100)});
  histos->insert({"hmt_WSS", new TH1D("mt_WSS", "MT", 50, 0, 100)});

  histos->insert({"hmjj", new TH1D("mjj", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmjj_QCD", new TH1D("mjj_QCD", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmjj_SS", new TH1D("mjj_SS", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmjj_WOS", new TH1D("mjj_WOS", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmjj_WSS", new TH1D("mjj_WSS", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});

  histos->insert({"hmvis", new TH1D("mvis", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmvis_QCD", new TH1D("mvis_QCD", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmvis_SS", new TH1D("mvis_SS", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmvis_WOS", new TH1D("mvis_WOS", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});
  histos->insert({"hmvis_WSS", new TH1D("mvis_WSS", "Dijet Mass; Mass [GeV];;", 100, 0, 200)});  

  histos->insert({"hmetphi", new TH1D("metphi", "Missing E_{T} #phi;Missing E_{T} [GeV];;", 60, -3.14, 3.14)});
  histos->insert({"hmet_x", new TH1D("met_x", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});
  histos->insert({"hmet_y", new TH1D("met_y", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});
  histos->insert({"hmet_pt", new TH1D("met_pt", "Missing E_{T};Missing E_{T} [GeV];;", 100, 0., 500)});

  histos->insert({"hnjets", new TH1D("njets", "N(jets)", 10, -0.5, 9.5)});
  histos->insert({"hNGenJets", new TH1D("NGenJets", "Number of Gen Jets", 12, -0.5, 11.5)});
}

void initHistos_2D(std::unordered_map<std::string, TH2F*>* histos, TFile* fout, std::string name) {

  float bins0[] = {0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400};
  float bins1[] = {0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300};
  float bins2[] = {0, 95, 115, 135, 155, 400};

  float bins_pth[] = {0, 100, 150, 200, 250, 300, 5000};
  float bins_mjj[] = {300, 700, 1100, 1500, 10000};
  float bins_taupt[] = {0, 1, 10, 11};

  int  binnum1 = sizeof(bins1)/sizeof(Float_t) - 1;
  int  binnum2 = sizeof(bins2)/sizeof(Float_t) - 1;
  int  binnum0 = sizeof(bins0)/sizeof(Float_t) - 1;
  int  binnum_pth = sizeof(bins_pth)/sizeof(Float_t) - 1;
  int  binnum_taupt = sizeof(bins_taupt)/sizeof(Float_t) - 1;
  int  binnum_mjj = sizeof(bins_mjj)/sizeof(Float_t) - 1;

  fout->mkdir("et_0jet");
  fout->mkdir("et_boosted");
  fout->mkdir("et_ZH");
  fout->mkdir("et_vbf");
  fout->cd("et_0jet");
  histos->insert({"h0_OS", new TH2F(name.c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_boosted");
  histos->insert({"h1_OS", new TH2F(name.c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_vbf");
  histos->insert({"h2_OS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_ZH");
  histos->insert({"h3_OS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_antiiso_0jet_cr");
  fout->mkdir("et_antiiso_boosted_cr");
  fout->mkdir("et_antiiso_vbf_cr");
  fout->mkdir("et_antiiso_ZH_cr");
  fout->cd("et_antiiso_0jet_cr");
  histos->insert({"h0_QCD", new TH2F(name.c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_antiiso_boosted_cr");
  histos->insert({"h1_QCD", new TH2F(name.c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_antiiso_ZH_cr");
  histos->insert({"h2_QCD", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_antiiso_vbf_cr");
  histos->insert({"hvbf_QCD", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_wjets_0jet_cr");
  fout->mkdir("et_wjets_boosted_cr");
  fout->mkdir("et_wjets_vbf_cr");
  fout->mkdir("et_wjets_ZH_cr");
  fout->cd("et_wjets_0jet_cr");
  histos->insert({"h0_WOS", new TH2F(name.c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_wjets_boosted_cr");
  histos->insert({"h1_WOS", new TH2F(name.c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_wjets_ZH_cr");
  histos->insert({"h2_WOS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_wjets_vbf_cr");
  histos->insert({"hvbf_WOS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_antiiso_0jet_crSS");
  fout->mkdir("et_antiiso_boosted_crSS");
  fout->mkdir("et_antiiso_vbf_crSS");
  fout->mkdir("et_antiiso_ZH_crSS");
  fout->cd("et_antiiso_0jet_crSS");
  histos->insert({"h0_SS", new TH2F(name.c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_antiiso_boosted_crSS");
  histos->insert({"h1_SS", new TH2F(name.c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_antiiso_ZH_crSS");
  histos->insert({"h2_SS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_antiiso_vbf_crSS");
  histos->insert({"hvbf_SS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_wjets_0jet_crSS");
  fout->mkdir("et_wjets_boosted_crSS");
  fout->mkdir("et_wjets_vbf_crSS");
  fout->mkdir("et_wjets_ZH_crSS");
  fout->cd("et_wjets_0jet_crSS");
  histos->insert({"h0_WSS", new TH2F(name.c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_wjets_boosted_crSS");
  histos->insert({"h1_WSS", new TH2F(name.c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_wjets_ZH_crSS");
  histos->insert({"h2_WSS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_wjets_vbf_crSS");
  histos->insert({"hvbf_WSS", new TH2F(name.c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
}
