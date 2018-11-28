#ifndef SWISS_ARMY_CLASS_H
#define SWISS_ARMY_CLASS_H

// system include
#include <map>
#include <unordered_map>

// ROOT include
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

class Helper {
  private:
  double luminosity2016, luminosity2017;
  std::map<std::string, double> cross_sections;
  std::unordered_map<std::string, TH1F *> histos_1d;
  std::unordered_map<std::string, TH2F *> histos_2d;
  std::map<std::string, std::string> systematics;

public:
  Helper(TFile*,std::string,std::string);
  ~Helper(){};
  double getCrossSection(std::string sample) { return cross_sections[sample]; };
  double getLuminosity2016() { return luminosity2016; };
  double getLuminosity2017() { return luminosity2017; };
  std::unordered_map<std::string, TH1F *> *getHistos1D() { return &histos_1d; };
  std::unordered_map<std::string, TH2F *> *getHistos2D() { return &histos_2d; };

  Float_t deltaR(Float_t eta1, Float_t phi1, Float_t eta2, Float_t phi2) {
    return sqrt(pow(eta1 - eta2, 2) + pow(phi1 - phi2, 2));
  }

};

Helper::Helper(TFile *fout, std::string name, std::string syst) : 
luminosity2016(35900.),
luminosity2017(41500.), 
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
    {"DYJets", 6225.42},
    {"DYJets_ext1", 6225.42},
    {"DYJets1", 6225.42},
    {"DYJets1_ext1", 6225.42},
    {"DYJets1_real", 6225.42},
    {"DYJets1_v2", 6225.42},
    {"DYJets2", 6225.42},
    {"DYJets2_ext", 6225.42},
    {"DYJets3", 6225.42},
    {"DYJets3_ext", 6225.42},
    {"DYJets4", 6225.42},
    {"DYJets4_real", 6225.42},
    {"EWKMinus", 20.25},
    {"EWKPlus", 25.62},
    {"EWKZ2l", 3.987},
    {"EWKZ2nu", 10.01},
    {"Tbar-tW", 35.6},
    {"T-tW", 35.6},
    {"T-tW_v2", 35.6},
    {"Tbar-tchan", 26.23},
    {"Tbar-tchan_v2", 26.23},
    {"T-tchan", 44.07},
    {"TT", 831.76},
    {"TTHad", 377.96},
    {"TTHad_v2", 377.96},
    {"TTLep", 88.29},
    {"TTSemi", 365.35},
    {"TTSemi_v2", 365.35},
    {"VV2l2nu", 11.95},
    {"WJets", 61526.7},
    {"WJets_ext1", 61526.7},
    {"WJets1", 61526.7},
    {"WJets2", 61526.7},
    {"WJets3", 61526.7},
    {"WJets4", 61526.7},
    {"WW1l1nu2q", 49.997},
    {"WZ1l1nu2q", 10.71},
    {"WZ1l1nu2q_v2", 10.71},
    {"WZ1l3nu", 3.05},
    {"WZ2l2Q", 5.595},
    {"WZ3l1nu", 4.708},
    {"WW", 75.88},
    {"WZ", 27.57},
    {"ZZ", 12.14},
    {"ZZ2l2q", 3.22},
    {"ZZ4l", 1.212},
    {"ZZ4l_ext1", 1.212},
    {"ZZ4l_v2", 1.212},
    {"data", 1.0},
    {"Data", 1.0},
    {"VBFHtoTauTau125", 3.782 * 0.0627},
    {"VBF125", 3.782 * 0.0627},
    {"ggHtoTauTau125_v1", 48.58* 0.0627},
    {"ggHtoTauTau125_v2", 48.58* 0.0627},
    {"ggH125_v1", 48.58* 0.0627},
    {"ggH125_ext", 48.58* 0.0627},
    {"ttH125", 0.5071* 0.0627},
    {"WMinusHTauTau125", 0.5328 * 0.0627},
    {"WPlusHTauTau125", 0.840 * 0.0627},
    {"WMinus125", 0.5328 * 0.0627},
    {"WPlus125", 0.840 * 0.0627},
    {"ZHTauTau125", 0.8839 * 0.062}

  }, 
  histos_1d {
    {"n70", new TH1F("n70", "n70", 6, 0, 6)},
    {"cutflow", new TH1F("cutflow", "Cutflow", 12, -0.5, 11.5)},
    {"weightflow", new TH1F("weightflow", "weightflow", 10, 0.5, 10.5)},
    {"el_pt", new TH1F(name.c_str(), "el_pt", 12, 0, 300)},
    {"tau_pt", new TH1F("tau_pt", "tau_pt", 12, 0, 300)},
  }
{
      auto hweights = new TH2F("weights","weights", 10, 0.5, 10.5, 800, -2, 2);
      hweights -> GetXaxis() -> SetBinLabel(1, "xs*lumi/gen");
      hweights -> GetXaxis() -> SetBinLabel(2, "stitch");
      hweights -> GetXaxis() -> SetBinLabel(3, "trig SF");
      hweights -> GetXaxis() -> SetBinLabel(4, "ID SF");
      hweights -> GetXaxis() -> SetBinLabel(5, "pileup corr");
      hweights -> GetXaxis() -> SetBinLabel(6, "gen weight");
      hweights -> GetXaxis() -> SetBinLabel(7, "htt sf");
      hweights -> GetXaxis() -> SetBinLabel(8, "anti-lepton");
      hweights -> GetXaxis() -> SetBinLabel(9, "z pt");
      hweights -> GetXaxis() -> SetBinLabel(10, "zmm");
      histos_2d["weights"] = hweights;

      auto hweightflow = histos_1d.at("weightflow");
      hweightflow -> GetXaxis() -> SetBinLabel(1, "xs*lumi/gen");
      hweightflow -> GetXaxis() -> SetBinLabel(2, "stitch");
      hweightflow -> GetXaxis() -> SetBinLabel(3, "trig SF");
      hweightflow -> GetXaxis() -> SetBinLabel(4, "ID SF");
      hweightflow -> GetXaxis() -> SetBinLabel(5, "pileup corr");
      hweightflow -> GetXaxis() -> SetBinLabel(6, "gen weight");
      hweightflow -> GetXaxis() -> SetBinLabel(7, "htt sf");
      hweightflow -> GetXaxis() -> SetBinLabel(8, "anti-lepton");
      hweightflow -> GetXaxis() -> SetBinLabel(9, "z pt");
      hweightflow -> GetXaxis() -> SetBinLabel(10, "zmm");
      histos_1d["weightflow"] = hweightflow;

      std::string suffix = systematics[syst];



      
}

#endif
