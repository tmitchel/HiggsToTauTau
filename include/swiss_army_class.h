// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_SWISS_ARMY_CLASS_H_
#define INCLUDE_SWISS_ARMY_CLASS_H_

// system include
#include <map>
#include <string>
#include <unordered_map>

// ROOT include
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

enum lepton { ELECTRON, MUON, DITAU, EMU };

class Helper {
   private:
    double luminosity2016, luminosity2017, luminosity2018;
    std::map<std::string, double> cross_sections;
    std::unordered_map<std::string, TH1F *> histos_1d;
    std::unordered_map<std::string, TH2F *> histos_2d;
    std::map<std::string, std::string> systematics;

   public:
    Helper(TFile *, std::string, std::string);
    ~Helper() {}
    double getCrossSection(std::string sample) { return cross_sections[sample]; }
    double getLuminosity2016() { return luminosity2016; }
    double getLuminosity2017() { return luminosity2017; }
    double getLuminosity2018() { return luminosity2018; }
    std::unordered_map<std::string, TH1F *> *getHistos1D() { return &histos_1d; }
    std::unordered_map<std::string, TH2F *> *getHistos2D() { return &histos_2d; }

    Float_t deltaR(Float_t eta1, Float_t phi1, Float_t eta2, Float_t phi2) {
        return sqrt(pow(eta1 - eta2, 2) + pow(phi1 - phi2, 2));
    }
};

Helper::Helper(TFile *fout, std::string name, std::string syst)
    : luminosity2016(35900.),
      luminosity2017(41500.),
      luminosity2018(63670.),
      systematics{{"met_UESDown", "_CMS_scale_met_unclustered_13TeVDown"},
                  {"met_UESUp", "_CMS_scale_met_unclustered_13TeVUp"},
                  {"met_JESDown", "_CMS_scale_met_clustered_13TeVDown"},
                  {"met_JESUp", "_CMS_scale_met_clustered_13TeVUp"},
                  {"metphi_UESDown", "_CMS_scale_metphi_unclustered_13TeVDown"},
                  {"metphi_UESUp", "_CMS_scale_metphi_unclustered_13TeVUp"},
                  {"metphi_JESDown", "_CMS_scale_metphi_clustered_13TeVDown"},
                  {"metphi_JESUp", "_CMS_scale_metphi_clustered_13TeVUp"}},
      cross_sections{{"DYJets1_new_pmx_v1", 6225.42},
                     {"DYJets1_old_pmx_v2", 6225.42},
                     {"DYJets1_v1", 6225.42},
                     {"DYJets1_v3", 6225.42},
                     {"DYJets2_ext_v1", 6225.42},
                     {"DYJets_ext2_v2", 6225.42},
                     {"DYJets2_lowM_v2", 6225.42},
                     {"DYJets2_v1", 6225.42},
                     {"DYJets2_new_pmx_v2", 6225.42},
                     {"DYJets3_new_pmx_v2", 6225.42},
                     {"DYJets3_ext_v1", 6225.42},
                     {"DYJets3_v1", 6225.42},
                     {"DYJets3_v2", 6225.42},
                     {"DYJets4_v1", 6225.42},
                     {"DYJets4_v2", 6225.42},
                     {"DYJets_inc", 6225.42},
                     {"DYJets_ext_v1", 6225.42},
                     {"DYJets_ext1_v2", 6225.42},
                     {"DYJets_lowM_v2", 6225.42},
                     {"DYJets_v1", 6225.42},
                     {"EWKWMinus", 20.25},
                     {"EWKMinus_ext2_v2", 20.25},
                     {"EWKWMinus_ext1_v2", 20.25},
                     {"EWKWPlus", 25.62},
                     {"EWKWPlus_ext1_v2", 25.62},
                     {"EWKPlus_ext2_v2", 25.62},
                     {"EWKZ2l_new_pmx_v2", 3.987},
                     {"EWKZ2l_v1", 3.987},
                     {"EWKZ2l_ext1_v2", 3.987},
                     {"EWKZ2l_ext2_v2", 3.987},
                     {"EWKZ2nu_v1", 10.01},
                     {"EWKZ2nu_ext1_v2", 10.01},
                     {"EWKZ2nu_ext2_v2", 10.01},
                     {"EWKZ2nu_new_pmx_v2", 10.01},
                     {"Tbar-tW_v2", 35.6},
                     {"Tbar-tW", 35.6},
                     {"T-tW_v1", 35.6},
                     {"T-tW", 35.6},
                     {"T-tW_v2", 35.6},
                     {"Tbar-tchan", 26.23},
                     {"Tbar-tchan_v1", 26.23},
                     {"Tbar-tchan_v2", 26.23},
                     {"T-tchan_new_pmx_v1", 44.07},
                     {"T-tchan_v1", 44.07},
                     {"T-tchan", 44.07},
                     {"TT_v1", 831.76},
                     {"TTHad_new_pmx_v2", 377.96},
                     {"TTHad_v1", 377.96},
                     {"TTLep_new_pmx_v1", 88.29},
                     {"TTLep_v1", 88.29},
                     {"TTLep_v2", 88.29},
                     {"TTSemi_new_pmx_v1", 365.35},
                     {"TTSemi_v2", 365.35},
                     {"VV_v2", 11.95},
                     {"VV_inc", 11.95},
                     {"VV_ext_v2", 11.95},
                     {"WJets_v2", 61526.7},
                     {"WJets_inc", 61526.7},
                     {"WJets_v3", 61526.7},
                     {"WJets_ext_v2", 61526.7},
                     {"WJets1_v1", 61526.7},
                     {"WJets1_v3", 61526.7},
                     {"WJets1_v4", 61526.7},
                     {"WJets2_v4", 61526.7},
                     {"WJets2_v5", 61526.7},
                     {"WJets2_ext_v2", 61526.7},
                     {"WJets3_v1", 61526.7},
                     {"WJets3_ext_v2", 61526.7},
                     {"WJets4", 61526.7},
                     {"WJets4_new_pmx_v2", 61526.7},
                     {"WJets4_v1", 61526.7},
                     {"WJets4_ext_v1", 61526.7},
                     {"WJets4_ext_v2", 61526.7},
                     {"WW1l1nu2q", 49.997},
                     {"WZ1l1nu2q", 10.71},
                     {"WZ1l1nu2q_v2", 10.71},
                     {"WZ1l3nu", 3.05},
                     {"WZ2l2Q", 5.595},
                     {"WZ3l1nu", 4.708},
                     {"WW_v1", 75.88},
                     {"WW_v2", 75.88},
                     {"WZ_v1", 27.57},
                     {"ZZ_v1", 12.14},
                     {"ZZ_new_pmx_v2", 12.14},
                     {"ZZ2l2q", 3.22},
                     {"ZZ4l", 1.212},
                     {"ZZ4l_ext1", 1.212},
                     {"ZZ4l_v2", 1.212},
                     {"data", 1.0},
                     {"Data", 1.0},
                     {"vbf125", 3.782 * 0.0627},
                     {"ggh125", 48.58 * 0.0627},
                     {"tth125", 0.5071 * 0.0627},
                     {"wminus125", 0.5328 * 0.0627},
                     {"wplus125", 0.840 * 0.0627},
                     {"zh125", 0.8839 * 0.062}},
      histos_1d{{"cutflow", new TH1F("cutflow", "Cutflow", 12, -0.5, 11.5)},
                {"el_pt", new TH1F(name.c_str(), "el_pt", 12, 0, 300)},
                {"tau_pt", new TH1F("tau_pt", "tau_pt", 12, 0, 300)},
                {"triggers", new TH1F("triggers", "triggers", 4, -0.5, 3.5)}} {
    std::string suffix = systematics[syst];
}

#endif  // INCLUDE_SWISS_ARMY_CLASS_H_
