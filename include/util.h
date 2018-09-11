#include <map>

static double luminosity(35870.);
static std::map<std::string, double> cross_sections {
  {"DYJets", 5765.4},
  {"DYJets1", 5765.4},
  {"DYJets2", 5765.4},
  {"DYJets3", 5765.4},
  {"DYJets4", 5765.4},
  {"EWKMinus", 20.25},
  {"EWKPlus", 25.62},
  {"EWKZ2l", 3.987},
  {"EWKZ2nu", 10.01},
  {"HWW_gg125", 48.58*0.2137*0.3258},
  {"HWW_vbf125", 3.782*0.2137*0.3258},
  {"SMH_VBF110", 4.434*0.0791},
  {"SMH_VBF120", 3.935*0.0698},
  {"SMH_VBF125", 3.782*0.0627},
  {"VBFHtoTauTau125", 3.782*0.0627},
  {"SMH_VBF130", 3.637*0.0541},
  {"SMH_VBF140", 3.492*0.0360},
  {"SMH_gg110", 57.90*0.0791},
  {"SMH_ggH120", 47.38*0.0698},
  {"SMH_ggH125", 44.14*0.0627},
  {"ggHtoTauTau125", 44.14*0.0627},
  {"SMH_ggH130", 41.23*0.0541},
  {"SMH_gg140", 36.0*0.0360},
  {"ST_tW_antitop", 35.6},
  {"ST_tW_top", 35.6},
  {"ST_t_antitop", 26.23},
  {"ST_t_top", 44.07},
  {"Tbar-tW", 35.6},
  {"T-tW", 35.6},
  {"Tbar-tchan", 26.23},
  {"T-tchan", 44.07},
  {"TT", 831.76},
  {"VV2l2nu", 11.95},
  {"WJets", 61526.7},
  {"WJets1", 61526.7},
  {"WJets2", 61526.7},
  {"WJets3", 61526.7},
  {"WJets4", 61526.7},
  {"WGLNu", 489.0},
  {"WGstarEE", 3.526},
  {"WGstarMuMu", 2.793},
  {"WW1l1nu2q", 49.997},
  {"WZ1l1nu2q", 10.71},
  {"WZ1l3nu", 3.05},
  {"WZ2l2Q", 5.595},
  {"WZ3l1nu", 4.708},
  {"WMinusHTauTau125", 0.5328*0.0627},
  {"WPlusHTauTau125", 0.840*0.0627},
  {"ZZ2l2q", 3.22},
  {"ZZ4l", 1.212},
  {"ZHTauTau125", 0.8839*0.062},
  {"data", 1.0},
  {"Data", 1.0}
};

// do the mt calculation
Float_t calculate_mt(electron* const el, Float_t met_x, Float_t met_y, Float_t met_pt) {
  return sqrt(pow(el->getPt() + met_pt, 2) - pow(el->getPx() + met_x, 2) - pow(el->getPy() + met_y, 2));
}

Float_t deltaR(Float_t eta1, Float_t phi1, Float_t eta2, Float_t phi2) {
  return sqrt(pow(eta1-eta2, 2) + pow(phi1-phi2, 2));
}

void initHistos_1D(std::unordered_map<std::string, TH1D*>* histos) {
  histos->insert({"n70", new TH1D("n70", "n70", 6,0,6)});
  histos->insert({"cutflow", new TH1D("cutflow", "Cutflow", 12, -0.5, 11.5)});

  histos->insert({"pre_tau_pt", new TH1D("pre_tau_pt", "Tau p_{T};p_{T} [GeV];;", 40, 0., 200)});
  histos->insert({"pre_mt", new TH1D("pre_mt", "mt", 50, 0., 100.)});
  histos->insert({"pre_tau_iso", new TH1D("pre_tau_iso","" ,50, 0, .3)});
  histos->insert({"pre_el_iso", new TH1D("pre_el_iso","", 50, 0, .3)});

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

  histos->insert({"pt_sv", new TH1D("pt_sv", "pt_sv", 50, 0., 500.)});
  histos->insert({"m_sv", new TH1D("m_sv", "m_sv", 50, 30., 180.)});
  histos->insert({"Dbkg_VBF", new TH1D("Dbkg_VBF", "Dbkg_VBF", 50, 0., 1.)});
  histos->insert({"Phi", new TH1D("Phi", "Phi", 50, -3.14, 3.14)});
  histos->insert({"Phi1", new TH1D("Phi1", "Phi1", 50, -3.14, 3.14)});
  histos->insert({"Q2V1", new TH1D("Q2V1", "Q2V1", 1000, 0., 1000000.)});
  histos->insert({"Q2V2", new TH1D("Q2V2", "Q2V2", 1000, 0., 1000000.)});
  histos->insert({"costheta1", new TH1D("costheta1", "costheta1", 50, -1., 1.)});
  histos->insert({"costheta2", new TH1D("costheta2", "costheta2", 50, -1., 1.)});
  histos->insert({"costhetastar", new TH1D("costhetastar", "costhetastar", 50, -1., 1.)});
}

void initHistos_2D(std::unordered_map<std::string, TH2F*>* histos, TFile* fout, std::string name, std::string suffix) {

  Float_t bins0[] = {0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400};
  Float_t bins1[] = {0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300};
  Float_t bins2[] = {0, 95, 115, 135, 155, 400};

  Float_t bins_pth[] = {0, 100, 150, 200, 250, 300, 5000};
  Float_t bins_mjj[] = {300, 700, 1100, 1500, 10000};
  Float_t bins_taupt[] = {0, 1, 10, 11};

  Int_t  binnum1 = sizeof(bins1)/sizeof(Float_t) - 1;
  Int_t  binnum2 = sizeof(bins2)/sizeof(Float_t) - 1;
  Int_t  binnum0 = sizeof(bins0)/sizeof(Float_t) - 1;
  Int_t  binnum_pth = sizeof(bins_pth)/sizeof(Float_t) - 1;
  Int_t  binnum_taupt = sizeof(bins_taupt)/sizeof(Float_t) - 1;
  Int_t  binnum_mjj = sizeof(bins_mjj)/sizeof(Float_t) - 1;

  fout->mkdir("et_0jet");
  fout->mkdir("et_boosted");
  fout->mkdir("et_ZH");
  fout->mkdir("et_vbf");
  fout->cd("et_0jet");
  histos->insert({"h0_OS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_boosted");
  histos->insert({"h1_OS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_vbf");
  histos->insert({"h2_OS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_ZH");
  histos->insert({"h3_OS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_antiiso_0jet_cr");
  fout->mkdir("et_antiiso_boosted_cr");
  fout->mkdir("et_antiiso_vbf_cr");
  fout->mkdir("et_antiiso_ZH_cr");
  fout->cd("et_antiiso_0jet_cr");
  histos->insert({"h0_QCD", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_antiiso_boosted_cr");
  histos->insert({"h1_QCD", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_antiiso_vbf_cr");
  histos->insert({"h2_QCD", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_antiiso_ZH_cr");
  histos->insert({"h3_QCD", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_wjets_0jet_cr");
  fout->mkdir("et_wjets_boosted_cr");
  fout->mkdir("et_wjets_vbf_cr");
  fout->mkdir("et_wjets_ZH_cr");
  fout->cd("et_wjets_0jet_cr");
  histos->insert({"h0_WOS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_wjets_boosted_cr");
  histos->insert({"h1_WOS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_wjets_vbf_cr");
  histos->insert({"h2_WOS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_wjets_ZH_cr");
  histos->insert({"h3_WOS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_antiiso_0jet_crSS");
  fout->mkdir("et_antiiso_boosted_crSS");
  fout->mkdir("et_antiiso_vbf_crSS");
  fout->mkdir("et_antiiso_ZH_crSS");
  fout->cd("et_antiiso_0jet_crSS");
  histos->insert({"h0_SS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_antiiso_boosted_crSS");
  histos->insert({"h1_SS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_antiiso_vbf_crSS");
  histos->insert({"h2_SS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_antiiso_ZH_crSS");
  histos->insert({"h3_SS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});

  fout->mkdir("et_wjets_0jet_crSS");
  fout->mkdir("et_wjets_boosted_crSS");
  fout->mkdir("et_wjets_vbf_crSS");
  fout->mkdir("et_wjets_ZH_crSS");
  fout->cd("et_wjets_0jet_crSS");
  histos->insert({"h0_WSS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_taupt,bins_taupt,binnum0,bins0)});
  fout->cd("et_wjets_boosted_crSS");
  histos->insert({"h1_WSS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_pth,bins_pth,binnum1,bins1)});
  fout->cd("et_wjets_vbf_crSS");
  histos->insert({"h2_WSS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
  fout->cd("et_wjets_ZH_crSS");
  histos->insert({"h3_WSS", new TH2F((name+suffix).c_str(),"Invariant mass",binnum_mjj,bins_mjj,binnum2,bins2)});
}

double GetZmmSF(float jets, float mj, float pthi, float taupt, float syst) {
  double aweight = 1.0;
  if (syst == 0)
  {
    if (jets >= 2 && mj > 300 && taupt > 40 && pthi > 50)
    { //VBF
      if (mj >= 300 && mj < 700)
        aweight = 1.070;
      if (mj >= 700 && mj < 1100)
        aweight = 1.090;
      if (mj >= 1100 && mj < 1500)
        aweight = 1.055;
      if (mj >= 1500)
        aweight = 1.015;
    }
  }
  if (syst == 1)
  {
    if (jets >= 2 && mj > 300 && taupt > 40 && pthi > 50)
    { //VBF
      if (mj >= 300 && mj < 700)
        aweight = 1.14;
      if (mj >= 700 && mj < 1100)
        aweight = 1.18;
      if (mj >= 1100 && mj < 1500)
        aweight = 1.11;
      if (mj >= 1500)
        aweight = 1.030;
    }
  }
  return aweight;
}
