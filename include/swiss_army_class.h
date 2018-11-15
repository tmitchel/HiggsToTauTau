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
  double luminosity;
  std::map<std::string, double> cross_sections;
  std::unordered_map<std::string, TH1F *> histos_1d;
  std::unordered_map<std::string, TH2F *> histos_2d;
  std::map<std::string, std::string> systematics;

public:
  Helper(TFile*,std::string,std::string);
  ~Helper(){};
  double getCrossSection(std::string sample) { return cross_sections[sample]; };
  double getLuminosity() { return luminosity; };
  std::unordered_map<std::string, TH1F *> *getHistos1D() { return &histos_1d; };
  std::unordered_map<std::string, TH2F *> *getHistos2D() { return &histos_2d; };

  Float_t deltaR(Float_t eta1, Float_t phi1, Float_t eta2, Float_t phi2) {
    return sqrt(pow(eta1 - eta2, 2) + pow(phi1 - phi2, 2));
  }

};

Helper::Helper(TFile *fout, std::string name, std::string syst) : 
luminosity(41500.), 
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
    {"DYJets1_ext", 6225.42},
    {"DYJets2", 6225.42},
    {"DYJets2_ext", 6225.42},
    {"DYJets3", 6225.42},
    {"DYJets3_ext", 6225.42},
    {"DYJets4", 6225.42},
    {"EWKMinus", 20.25},
    {"EWKPlus", 25.62},
    {"EWKZ2l", 3.987},
    {"EWKZ2nu", 10.01},
    {"Tbar-tW", 35.6},
    {"T-tW", 35.6},
    {"Tbar-tchan", 26.23},
    {"T-tchan", 44.07},
    {"TT", 831.76},
    {"TTHad", 377.96},
    {"TTLep", 88.29},
    {"TTSemi", 365.35},
    {"VV2l2nu", 11.95},
//    {"WJets", 61526.7},
//    {"WJets_ext1", 61526.7},
    {"WJets", 52940.},
    {"WJets_ext1", 52940.},

    {"WJets1", 61526.7},
    {"WJets2", 61526.7},
    {"WJets3", 61526.7},
    {"WJets4", 61526.7},
    {"WW1l1nu2q", 49.997},
    {"WZ1l1nu2q", 10.71},
    {"WZ1l3nu", 3.05},
    {"WZ2l2Q", 5.595},
    {"WZ3l1nu", 4.708},
    {"WW", 75.88},
    {"WZ", 27.57},
    {"ZZ", 12.14},
    {"ZZ2l2q", 3.22},
    {"ZZ4l", 1.212},
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
    {"el_pt", new TH1F("el_pt", "el_pt", 12, 0, 300)},
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

      Float_t bins0[] = {0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400};
      Float_t bins1[] = {0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300};
      Float_t bins2[] = {0, 95, 115, 135, 155, 400};
      Float_t bins_pth[] = {0, 100, 150, 200, 250, 300, 5000};
      Float_t bins_mjj[] = {300, 700, 1100, 1500, 10000};
      Float_t bins_taupt[] = {0, 1, 10, 11};
      Int_t binnum1 = sizeof(bins1) / sizeof(Float_t) - 1;
      Int_t binnum2 = sizeof(bins2) / sizeof(Float_t) - 1;
      Int_t binnum0 = sizeof(bins0) / sizeof(Float_t) - 1;
      Int_t binnum_pth = sizeof(bins_pth) / sizeof(Float_t) - 1;
      Int_t binnum_taupt = sizeof(bins_taupt) / sizeof(Float_t) - 1;
      Int_t binnum_mjj = sizeof(bins_mjj) / sizeof(Float_t) - 1;

      fout->mkdir("et_0jet");
      fout->mkdir("et_boosted");
      fout->mkdir("et_vbf");
      fout->mkdir("et_SS_0jet");
      fout->mkdir("et_SS_boosted");
      fout->mkdir("et_SS_vbf");
      fout->mkdir("et_antiLepIso_0jet_OS");
      fout->mkdir("et_antiLepIso_boosted_OS");
      fout->mkdir("et_antiLepIso_vbf_OS");
      fout->mkdir("et_antiLepIso_0jet_SS");
      fout->mkdir("et_antiLepIso_boosted_SS");
      fout->mkdir("et_antiLepIso_vbf_SS");
      fout->mkdir("et_antiTauIso_0jet_OS");
      fout->mkdir("et_antiTauIso_boosted_OS");
      fout->mkdir("et_antiTauIso_vbf_OS");
      fout->mkdir("et_antiTauIso_0jet_SS");
      fout->mkdir("et_antiTauIso_boosted_SS");
      fout->mkdir("et_antiTauIso_vbf_SS");
      fout->mkdir("et_looseIso_0jet_OS");
      fout->mkdir("et_looseIso_boosted_OS");
      fout->mkdir("et_looseIso_vbf_OS");
      fout->mkdir("et_looseIso_0jet_SS");
      fout->mkdir("et_looseIso_boosted_SS");
      fout->mkdir("et_looseIso_vbf_SS");

      // Signal Region
      fout->cd("et_0jet");
      histos_2d.insert({"h0_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_boosted");
      histos_2d.insert({"h1_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_vbf");
      histos_2d.insert({"h2_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      fout->cd("et_SS_0jet");
      histos_2d.insert({"h0_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_SS_boosted");
      histos_2d.insert({"h1_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_SS_vbf");
      histos_2d.insert({"h2_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      // Anti-Isolated Lepton Region
      fout->cd("et_antiLepIso_0jet_OS");
      histos_2d.insert({"h0_anti_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_antiLepIso_boosted_OS");
      histos_2d.insert({"h1_anti_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_antiLepIso_vbf_OS");
      histos_2d.insert({"h2_anti_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      fout->cd("et_antiLepIso_0jet_SS");
      histos_2d.insert({"h0_anti_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_antiLepIso_boosted_SS");
      histos_2d.insert({"h1_anti_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_antiLepIso_vbf_SS");
      histos_2d.insert({"h2_anti_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      // Anti-Isolated Tau Region
      fout->cd("et_antiTauIso_0jet_OS");
      histos_2d.insert({"h0_Fake_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_antiTauIso_boosted_OS");
      histos_2d.insert({"h1_Fake_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_antiTauIso_vbf_OS");
      histos_2d.insert({"h2_Fake_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      fout->cd("et_antiTauIso_0jet_SS");
      histos_2d.insert({"h0_Fake_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_antiTauIso_boosted_SS");
      histos_2d.insert({"h1_Fake_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_antiTauIso_vbf_SS");
      histos_2d.insert({"h2_Fake_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      // Loose-Isolated Region
      fout->cd("et_looseIso_0jet_OS");
      histos_2d.insert({"h0_loose_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_looseIso_boosted_OS");
      histos_2d.insert({"h1_loose_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_looseIso_vbf_OS");
      histos_2d.insert({"h2_loose_OS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      fout->cd("et_looseIso_0jet_SS");
      histos_2d.insert({"h0_loose_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_taupt, bins_taupt, binnum0, bins0)});
      fout->cd("et_looseIso_boosted_SS");
      histos_2d.insert({"h1_loose_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_pth, bins_pth, binnum1, bins1)});
      fout->cd("et_looseIso_vbf_SS");
      histos_2d.insert({"h2_loose_SS", new TH2F((name + suffix).c_str(), "Invariant mass", binnum_mjj, bins_mjj, binnum2, bins2)});

      
}

#endif