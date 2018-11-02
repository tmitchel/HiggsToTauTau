#ifndef UTIL_H
#define UTIL_H

#include <map>

class Helper {
  private:
  double luminosity;
  std::map<std::string, double> cross_sections;
  std::unordered_map<std::string, TH1F *> histos_1d;
  std::map<std::string, std::string> systematics;

public:
  Helper(TFile*,std::string,std::string);
  ~Helper(){};
  double getCrossSection(std::string sample) { return cross_sections[sample]; };
  double getLuminosity() { return luminosity; };
  std::unordered_map<std::string, TH1F *> *getHistos1D() { return &histos_1d; };

  Float_t deltaR(Float_t eta1, Float_t phi1, Float_t eta2, Float_t phi2) {
    return sqrt(pow(eta1 - eta2, 2) + pow(phi1 - phi2, 2));
  }

};

Helper::Helper(TFile *fout, std::string name, std::string syst) : 
luminosity(35870.), 
  systematics {
    {"met_UESDown", "_CMS_scale_met_unclustered_13TeVDown"},
    {"met_UESUp", "_CMS_scale_met_unclustered_13TeVUp"},
    {"met_JESDown", "_CMS_scale_met_clustered_13TeVDown"},
    {"met_JESUp", "_CMS_scale_met_clustered_13TeVUp"},
    {"metphi_UESDown", "_CMS_scale_metphi_unclustered_13TeVDown"},
    {"metphi_UESUp", "_CMS_scale_metphi_unclustered_13TeVUp"},
    {"metphi_JESDown", "_CMS_scale_metphi_clustered_13TeVDown"},
    {"metphi_JESUp", "_CMS_scale_metphi_clustered_13TeVUp"}
  },
  cross_sections {
    {"DYJets", 5765.4},
    {"DYJets1", 5765.4},
    {"DYJets2", 5765.4},
    {"DYJets3", 5765.4},
    {"DYJets4", 5765.4},
    {"EWKMinus", 20.25},
    {"EWKPlus", 25.62},
    {"EWKZ2l", 3.987},
    {"EWKZ2nu", 10.01},
    {"HWW_gg125", 48.58 * 0.2137 * 0.3258},
    {"HWW_vbf125", 3.782 * 0.2137 * 0.3258},
    {"SMH_VBF110", 4.434 * 0.0791},
    {"SMH_VBF120", 3.935 * 0.0698},
    {"SMH_VBF125", 3.782 * 0.0627},
    {"VBFHtoTauTau125", 3.782 * 0.0627},
    {"VBF125", 3.782 * 0.0627},
    {"SMH_VBF130", 3.637 * 0.0541},
    {"SMH_VBF140", 3.492 * 0.0360},
    {"SMH_gg110", 57.90 * 0.0791},
    {"SMH_ggH120", 47.38 * 0.0698},
    {"SMH_ggH125", 44.14 * 0.0627},
    {"SMH_ggH130", 41.23 * 0.0541},
    {"ggHtoTauTau125_v1", 48.58* 0.0627},
    {"ggHtoTauTau125_v2", 48.58* 0.0627},
    {"SMH_gg140", 36.0 * 0.0360},
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
    {"WMinusHTauTau125", 0.5328 * 0.0627},
    {"WPlusHTauTau125", 0.840 * 0.0627},
    {"ZZ2l2q", 3.22},
    {"ZZ4l", 1.212},
    {"ZHTauTau125", 0.8839 * 0.062},
    {"data", 1.0},
    {"Data", 1.0}
  }, 
  histos_1d {
    {"n70", new TH1F("n70", "n70", 6, 0, 6)},
    {"cutflow", new TH1F("cutflow", "Cutflow", 12, -0.5, 11.5)},
  }
{
      std::string suffix = systematics[syst];  
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


#endif
