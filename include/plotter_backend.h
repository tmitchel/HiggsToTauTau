// Copyright 2019 Tyler Mitchell

#ifndef INCLUDE_PLOTTER_BACKEND_H_
#define INCLUDE_PLOTTER_BACKEND_H_

// system includes
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

// ROOT includes
#include "./tree_reader.h"
#include "TFile.h"
#include "TH2F.h"
#include "TTree.h"

// FF
#include "HTTutilities/Jet2TauFakes/interface/FakeFactor.h"
#include "HTTutilities/Jet2TauFakes/interface/IFunctionWrapper.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTFormula.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTGraph.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTH2F.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTH3D.h"

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;

enum Categories { zeroJet,
                  boosted,
                  vbf,
                  vbf_ggHMELA_bin1_NN_bin1,
                  vbf_ggHMELA_bin2_NN_bin1,
                  vbf_ggHMELA_bin3_NN_bin1,
                  vbf_ggHMELA_bin4_NN_bin1,
                  vbf_ggHMELA_bin5_NN_bin1,
                  vbf_ggHMELA_bin6_NN_bin1,
                  vbf_ggHMELA_bin7_NN_bin1,
                  vbf_ggHMELA_bin8_NN_bin1,
                  vbf_ggHMELA_bin9_NN_bin1,
                  vbf_ggHMELA_bin10_NN_bin1,
                  vbf_ggHMELA_bin11_NN_bin1,
                  vbf_ggHMELA_bin12_NN_bin1,
                  vbf_ggHMELA_bin1_NN_bin2,
                  vbf_ggHMELA_bin2_NN_bin2,
                  vbf_ggHMELA_bin3_NN_bin2,
                  vbf_ggHMELA_bin4_NN_bin2,
                  vbf_ggHMELA_bin5_NN_bin2,
                  vbf_ggHMELA_bin6_NN_bin2,
                  vbf_ggHMELA_bin7_NN_bin2,
                  vbf_ggHMELA_bin8_NN_bin2,
                  vbf_ggHMELA_bin9_NN_bin2,
                  vbf_ggHMELA_bin10_NN_bin2,
                  vbf_ggHMELA_bin11_NN_bin2,
                  vbf_ggHMELA_bin12_NN_bin2 };

// read all *.root files in the given directory and put them in the provided vector
void read_directory(const string &name, vector<string> *v) {
  DIR *dirp = opendir(name.c_str());
  struct dirent *dp;
  while ((dp = readdir(dirp)) != 0) {
    if (static_cast<string>(dp->d_name).find("root") != string::npos) {
      v->push_back(dp->d_name);
    }
  }
  closedir(dirp);
}

// class to hold the histograms until I'm ready to write them
class HistTool {
 public:
  HistTool(string, string, string, tree_reader);
  ~HistTool() { delete ff_weight; }

  // create directories and fake factor histograms
  void includeTemplates();
  void includePlots();

  void writeHistos();
  void writeTemplates();
  void initVectors1d(string);
  void initVectors2d(string);
  void initSystematics(string);
  void fillFraction(int, string, double, double, double);
  void convertDataToFake(Categories, string, double, double, double, double, double, double, double, double);          // 1d
  void convertDataToFake(Categories, string, double, double, double, double, double, double, double, double, double);  // 2d
  void histoLoop(vector<string>, string, string, string);
  void getJetFakes(vector<string>, string, string, bool);
  Categories getCategory(double, double);

  tree_reader t;
  shared_ptr<TFile> fout;
  FakeFactor *ff_weight;
  string channel_prefix;
  vector<string> categories, systematics;
  vector<float> mvis_bins, njets_bins;
  map<string, string> acNameMap;
  map<string, vector<shared_ptr<TH1F>>> hists_1d;
  map<string, vector<shared_ptr<TH2F>>> hists_2d, FF_systs;
  vector<shared_ptr<TH1F>> fakes_1d;
  vector<shared_ptr<TH2F>> data, fakes_2d, frac_w, frac_tt, frac_real, frac_qcd;

  // binning
  vector<Float_t> bins_l2, bins_hpt, bins_mjj, bins_lpt, bins_msv1, bins_msv2, bins_hpt2;
};

// HistTool contructor to create the output file, the qcd histograms with the correct binning
// and the map from categories to vectors of TH2F*'s. Each TH2F* in the vector corresponds to
// one file that is being put into that categories directory in the output tempalte
HistTool::HistTool(string channel_prefix, string year, string suffix, tree_reader treader)
    : fout(std::make_shared<TFile>(("Output/templates/" + channel_prefix + year + "_" + suffix + ".root").c_str(), "recreate")),
      mvis_bins({0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000}),
      njets_bins({-0.5, 0.5, 1.5, 15}),
      // x-axis
      bins_l2{0, 1, 10, 11},
      bins_hpt{0, 100, 150, 200, 250, 300, 5000},
      // bins_mjj{300, 500, 10000},  // real mjj
      bins_mjj{0, 0.5, 1.},  // actually VBF MELA

      // y-axis
      bins_lpt{0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400},
      bins_msv1{0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300},
      bins_msv2{0, 80, 100, 115, 130, 150, 1000},
      bins_hpt2{0, 150, 10000},
      channel_prefix(channel_prefix),
      t(treader),
      acNameMap{
          {"wt_ggH_a1", "JHU_GGH2Jets_sm_M125"},
          {"wt_ggH_a3", "JHU_GGH2Jets_pseudoscalar_M125"},
          {"wt_ggH_a3int", "JHU_GGH2Jets_pseudoscalar_Mf05ph0125"},
          {"wt_wh_a1", "reweighted_WH_htt_0PM125"},
          {"wt_wh_a2", "reweighted_WH_htt_0PH125"},
          {"wt_wh_a2int", "reweighted_WH_htt_0PHf05ph0125"},
          {"wt_wh_a3", "reweighted_WH_htt_0M125"},
          {"wt_wh_a3int", "reweighted_WH_htt_0Mf05ph0125"},
          {"wt_wh_L1", "reweighted_WH_htt_0L1125"},
          {"wt_wh_L1int", "reweighted_WH_htt_0L1f05ph0125"},
          {"wt_wh_L1Zg", "reweighted_WH_htt_0L1Zg125"},
          {"wt_wh_L1Zgint", "reweighted_WH_htt_0L1Zgf05ph0125"},
          {"wt_zh_a1", "reweighted_ZH_htt_0PM125"},
          {"wt_zh_a2", "reweighted_ZH_htt_0PH125"},
          {"wt_zh_a2int", "reweighted_ZH_htt_0PHf05ph0125"},
          {"wt_zh_a3", "reweighted_ZH_htt_0M125"},
          {"wt_zh_a3int", "reweighted_ZH_htt_0Mf05ph0125"},
          {"wt_zh_L1", "reweighted_ZH_htt_0L1125"},
          {"wt_zh_L1int", "reweighted_ZH_htt_0L1f05ph0125"},
          {"wt_zh_L1Zg", "reweighted_ZH_htt_0L1Zg125"},
          {"wt_zh_L1Zgint", "reweighted_ZH_htt_0L1Zgf05ph0125"},
          {"wt_a1", "reweighted_qqH_htt_0PM125"},
          {"wt_a2", "reweighted_qqH_htt_0PH125"},
          {"wt_a2int", "reweighted_qqH_htt_0PHf05ph0125"},
          {"wt_a3", "reweighted_qqH_htt_0M125"},
          {"wt_a3int", "reweighted_qqH_htt_0Mf05ph0125"},
          {"wt_L1", "reweighted_qqH_htt_0L1125"},
          {"wt_L1int", "reweighted_qqH_htt_0L1f05ph0125"},
          {"wt_L1Zg", "reweighted_qqH_htt_0L1Zg125"},
          {"wt_L1Zgint", "reweighted_qqH_htt_0L1Zgf05ph0125"},
      },
      categories{
          channel_prefix + "_0jet",
          channel_prefix + "_boosted",
          channel_prefix + "_vbf",
          channel_prefix + "_vbf_ggHMELA_bin1_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin2_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin3_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin4_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin5_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin6_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin7_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin8_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin9_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin10_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin11_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin12_NN_bin1",
          channel_prefix + "_vbf_ggHMELA_bin1_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin2_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin3_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin4_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin5_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin6_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin7_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin8_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin9_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin10_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin11_NN_bin2",
          channel_prefix + "_vbf_ggHMELA_bin12_NN_bin2"},
      systematics{
          "ff_qcd_syst_up", "ff_qcd_syst_down", "ff_qcd_dm0_njet0_stat_up",
          "ff_qcd_dm0_njet0_stat_down", "ff_qcd_dm0_njet1_stat_up", "ff_qcd_dm0_njet1_stat_down",
          "ff_qcd_dm1_njet0_stat_up", "ff_qcd_dm1_njet0_stat_down", "ff_qcd_dm1_njet1_stat_up",
          "ff_qcd_dm1_njet1_stat_down", "ff_w_syst_up", "ff_w_syst_down", "ff_w_dm0_njet0_stat_up",
          "ff_w_dm0_njet0_stat_down", "ff_w_dm0_njet1_stat_up", "ff_w_dm0_njet1_stat_down",
          "ff_w_dm1_njet0_stat_up", "ff_w_dm1_njet0_stat_down", "ff_w_dm1_njet1_stat_up",
          "ff_w_dm1_njet1_stat_down", "ff_tt_syst_up", "ff_tt_syst_down", "ff_tt_dm0_njet0_stat_up",
          "ff_tt_dm0_njet0_stat_down", "ff_tt_dm0_njet1_stat_up", "ff_tt_dm0_njet1_stat_down",
          "ff_tt_dm1_njet0_stat_up", "ff_tt_dm1_njet0_stat_down", "ff_tt_dm1_njet1_stat_up", "ff_tt_dm1_njet1_stat_down"} {
  // Create empty histograms for each category to fill later.
  for (auto cat : categories) {
    // histograms for fake-factor are always 2d
    FF_systs[cat.c_str()] = vector<shared_ptr<TH2F>>();

    data.push_back(std::make_shared<TH2F>(
        ("data_" + cat).c_str(), ("data_" + cat).c_str(),
        mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_w.push_back(std::make_shared<TH2F>(
        ("frac_w_" + cat).c_str(), ("frac_w_" + cat).c_str(),
        mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_tt.push_back(std::make_shared<TH2F>(
        ("frac_tt_" + cat).c_str(), ("frac_tt_" + cat).c_str(),
        mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_real.push_back(std::make_shared<TH2F>(
        ("frac_real_" + cat).c_str(), ("frac_real_" + cat).c_str(),
        mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_qcd.push_back(std::make_shared<TH2F>(
        ("frac_qcd_" + cat).c_str(), ("frac_qcd_" + cat).c_str(),
        mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
  }

  // get FakeFactor workspace
  shared_ptr<TFile> ff_file;
  if (year == "2017") {
    ff_file = std::make_shared<TFile>(TFile(("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data2017/SM2017/tight/vloose/" + channel_prefix + "/fakeFactors.root").c_str(), "READ"));
  } else if (year == "2016") {
    ff_file = std::make_shared<TFile>(TFile(("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data2016/SM2016_ML/tight/" + channel_prefix + "/fakeFactors_tight.root").c_str(), "READ"));
  } else {
    std::cerr << "Bad year" << std::endl;
  }
  ff_weight = reinterpret_cast<FakeFactor *>(ff_file->Get("ff_comb"));
  ff_file->Close();
}

#endif  // INCLUDE_PLOTTER_BACKEND_H_
