// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_TEMPLATETOOL_H_
#define INCLUDE_TEMPLATETOOL_H_

// system includes
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

// FF
#include "HTTutilities/Jet2TauFakes/interface/FakeFactor.h"
#include "HTTutilities/Jet2TauFakes/interface/IFunctionWrapper.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTFormula.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTGraph.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTH2F.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTH3D.h"

enum Categories { inclusive,
                  zeroJet,
                  boosted,
                  vbf,
                  vbf_ggHMELA_bin1,
                  vbf_ggHMELA_bin2,
                  vbf_ggHMELA_bin3,
                  vbf_ggHMELA_bin4,
                  vbf_ggHMELA_bin5,
                  vbf_ggHMELA_bin6,
                  vbf_ggHMELA_bin7,
                  vbf_ggHMELA_bin8,
                  vbf_ggHMELA_bin9,
                  vbf_ggHMELA_bin10,
                  vbf_ggHMELA_bin11,
                  vbf_ggHMELA_bin12 };

// read all *.root files in the given directory and put them in the provided vector
void read_directory(const std::string &name, std::vector<std::string> *v) {
  DIR *dirp = opendir(name.c_str());
  struct dirent *dp;
  while ((dp = readdir(dirp)) != 0) {
    if (static_cast<std::string>(dp->d_name).find("root") != std::string::npos) {
      v->push_back(dp->d_name);
    }
  }
  closedir(dirp);
}

// TemplateTool is used to hold all of the long, hard-coded lists
// that are needed for making templates. It holds the category names,
// acWeight name and weight mappings, the map from systematic input
// names to names ready for combine, and the list of fake factor
// systematics. This class doesn't do anything besides hold this
// information and return it when asked.
class TemplateTool {
 public:
  std::vector<std::pair<std::string, std::string>> get_AC_weights(std::string);
  std::vector<std::string> get_categories() { return categories; }
  explicit TemplateTool(std::string);
  void Close() { delete ff_weight; }  // make sure to explicitly delete this. (NEEDED)
  void make_extension_map(std::string channel_prefix);
  std::string get_extension(std::string);

 protected:
  TemplateTool(std::string, std::string, std::string, std::shared_ptr<TFile>);

  std::shared_ptr<TFile> fout;
  FakeFactor *ff_weight;
  std::vector<std::string> categories, systematics;
  std::string channel_prefix, var;

 private:
  std::map<std::string, std::vector<std::pair<std::string, std::string>>> acNameMap;
  std::map<std::string, std::string> extension_map;
};

// This constructor is what is used by the Sample class. It only initializes that data
// that will be needed by the sample. This is the category list, potentially the fake
// factors systematic list, and loads the fake factor files. A pointer to the already
// open output file is kept for writing.
TemplateTool::TemplateTool(std::string channel_prefix, std::string year, std::string suffix, std::shared_ptr<TFile> output_file)
    : fout(output_file),
      channel_prefix(channel_prefix),
      categories{
          channel_prefix + "_inclusive",
          channel_prefix + "_0jet",
          channel_prefix + "_boosted",
          channel_prefix + "_vbf",
          channel_prefix + "_vbf_ggHMELA_bin1",
          channel_prefix + "_vbf_ggHMELA_bin2",
          channel_prefix + "_vbf_ggHMELA_bin3",
          channel_prefix + "_vbf_ggHMELA_bin4",
          channel_prefix + "_vbf_ggHMELA_bin5",
          channel_prefix + "_vbf_ggHMELA_bin6",
          channel_prefix + "_vbf_ggHMELA_bin7",
          channel_prefix + "_vbf_ggHMELA_bin8",
          channel_prefix + "_vbf_ggHMELA_bin9",
          channel_prefix + "_vbf_ggHMELA_bin10",
          channel_prefix + "_vbf_ggHMELA_bin11",
          channel_prefix + "_vbf_ggHMELA_bin12"},
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
  // get FakeFactor workspace
  TFile *ff_file;
  if (year == "2017") {
    ff_file = new TFile(("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data2017/SM2017/tight/vloose/" + channel_prefix + "/fakeFactors.root").c_str(), "READ");
  } else if (year == "2016") {
    ff_file = new TFile(("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data2016/SM2016_ML/tight/" + channel_prefix + "/fakeFactors_tight.root").c_str(), "READ");
  } else {
    std::cerr << "Bad year" << std::endl;
  }
  ff_weight = reinterpret_cast<FakeFactor *>(ff_file->Get("ff_comb"));
  ff_file->Close();
}

// get_AC_weight makes sure the provided name corresponds to
// a JHU sample for reweighting, then returns the vector of
// pairs that has the weight name and the new sample name.
std::vector<std::pair<std::string, std::string>> TemplateTool::get_AC_weights(std::string name) {
  if (name.find("ggh_inc") != std::string::npos) {
    return acNameMap.at("ggh");
  } else if (name.find("vbf_inc") != std::string::npos) {
    return acNameMap.at("vbf");
  } else if (name.find("wh_inc") != std::string::npos) {
    return acNameMap.at("wh");
  } else if (name.find("zh_inc") != std::string::npos) {
    return acNameMap.at("zh");
  }
}

// make_extension_map constructs the extension map only when requested.
// The map is used to translate from a tree name to the name that should
// be used in the histograms provided to combine.
void TemplateTool::make_extension_map(std::string channel_prefix) {
  extension_map = {
      {channel_prefix + "au_tree_UncMet_Up", "_CMS_scale_met_unclustered_13TeVUp"},
      {channel_prefix + "au_tree_UncMet_Down", "_CMS_scale_met_unclustered_13TeVDown"},
      {channel_prefix + "au_tree_ClusteredMet_Up", "_CMS_scale_met_clustered_13TeVUp"},
      {channel_prefix + "au_tree_ClusteredMet_Down", "_CMS_scale_met_clustered_13TeVDown"},
      {channel_prefix + "au_tree_vbfMass_JetTotalUp", "_CMS_scale_jm_13TeVUp"},
      {channel_prefix + "au_tree_jetVeto30_JetTotalUp", "_CMS_scale_jn_13TeVUp"},
      {channel_prefix + "au_tree_vbfMass_JetTotalDown", "_CMS_scale_jm_13TeVDown"},
      {channel_prefix + "au_tree_jetVeto30_JetTotalDown", "_CMS_scale_jn_13TeVDown"},
      {channel_prefix + "au_tree_ttbarShape_Up", "_CMS_htt_ttbarShape_13TeVUp"},
      {channel_prefix + "au_tree_ttbarShape_Down", "_CMS_htt_ttbarShape_13TeVDown"},
      {channel_prefix + "au_tree_Up", "_CMS_scale_t_allprong_13TeVUp"},
      {channel_prefix + "au_tree_Down", "_CMS_scale_t_allprong_13TeVDown"},
      {channel_prefix + "au_tree_DM0_Up", "_CMS_scale_t_1prong_13TeVUp"},
      {channel_prefix + "au_tree_DM0_Down", "_CMS_scale_t_1prong_13TeVDown"},
      {channel_prefix + "au_tree_DM1_Up", "_CMS_scale_t_1prong1pizero_13TeVUp"},
      {channel_prefix + "au_tree_DM1_Down", "_CMS_scale_t_1prong1pizero_13TeVDown"},
      {channel_prefix + "au_tree_DM10_Up", "_CMS_scale_t_3prong_13TeVUp"},
      {channel_prefix + "au_tree_DM10_Down", "_CMS_scale_t_3prong_13TeVDown"},
      {channel_prefix + "au_tree_jetToTauFake_Up", "_CMS_htt_jetToTauFake_13TeVUp"},
      {channel_prefix + "au_tree_jetToTauFake_Down", "_CMS_htt_jetToTauFake_13TeVDown"},
      {channel_prefix + "au_tree_dyShape_Up", "_CMS_htt_dyShape_13TeVUp"},
      {channel_prefix + "au_tree_dyShape_Down", "_CMS_htt_dyShape_13TeVDown"},
      {channel_prefix + "au_tree_zmumuShape_Up", "_CMS_htt_zmumuShape_VBF_13TeVUp"},
      {channel_prefix + "au_tree_zmumuShape_Down", "_CMS_htt_zmumuShape_VBF_13TeVDown"}};
}

// get_extension returns the histogram name needed for combine
// after being provided the input name from the tree.
std::string TemplateTool::get_extension(std::string name) {
  return extension_map.at(name);
}

// This constructor is used just to get some information and is NOT
// used by Sample. It just constructs the acNameMap and the categories.
TemplateTool::TemplateTool(std::string channel_prefix)
    : channel_prefix(channel_prefix),
      acNameMap{
          {"ggh",
           {std::make_pair("wt_ggH_a1", "JHU_GGH2Jets_sm_M125"),
            std::make_pair("wt_ggH_a3", "JHU_GGH2Jets_pseudoscalar_M125"),
            std::make_pair("wt_ggH_a3int", "JHU_GGH2Jets_pseudoscalar_Mf05ph0125")}},
          {"wh",
           {std::make_pair("wt_wh_a1", "reweighted_WH_htt_0PM125"),
            std::make_pair("wt_wh_a2", "reweighted_WH_htt_0PH125"),
            std::make_pair("wt_wh_a2int", "reweighted_WH_htt_0PHf05ph0125"),
            std::make_pair("wt_wh_a3", "reweighted_WH_htt_0M125"),
            std::make_pair("wt_wh_a3int", "reweighted_WH_htt_0Mf05ph0125"),
            std::make_pair("wt_wh_L1", "reweighted_WH_htt_0L1125"),
            std::make_pair("wt_wh_L1int", "reweighted_WH_htt_0L1f05ph0125"),
            std::make_pair("wt_wh_L1Zg", "reweighted_WH_htt_0L1Zg125"),
            std::make_pair("wt_wh_L1Zgint", "reweighted_WH_htt_0L1Zgf05ph0125")}},
          {"zh",
           {std::make_pair("wt_zh_a1", "reweighted_ZH_htt_0PM125"),
            std::make_pair("wt_zh_a2", "reweighted_ZH_htt_0PH125"),
            std::make_pair("wt_zh_a2int", "reweighted_ZH_htt_0PHf05ph0125"),
            std::make_pair("wt_zh_a3", "reweighted_ZH_htt_0M125"),
            std::make_pair("wt_zh_a3int", "reweighted_ZH_htt_0Mf05ph0125"),
            std::make_pair("wt_zh_L1", "reweighted_ZH_htt_0L1125"),
            std::make_pair("wt_zh_L1int", "reweighted_ZH_htt_0L1f05ph0125"),
            std::make_pair("wt_zh_L1Zg", "reweighted_ZH_htt_0L1Zg125"),
            std::make_pair("wt_zh_L1Zgint", "reweighted_ZH_htt_0L1Zgf05ph0125")}},
          {"vbf",
           {
               std::make_pair("wt_a1", "reweighted_qqH_htt_0PM125"),
               std::make_pair("wt_a2", "reweighted_qqH_htt_0PH125"),
               std::make_pair("wt_a2int", "reweighted_qqH_htt_0PHf05ph0125"),
               std::make_pair("wt_a3", "reweighted_qqH_htt_0M125"),
               std::make_pair("wt_a3int", "reweighted_qqH_htt_0Mf05ph0125"),
               std::make_pair("wt_L1", "reweighted_qqH_htt_0L1125"),
               std::make_pair("wt_L1int", "reweighted_qqH_htt_0L1f05ph0125"),
               std::make_pair("wt_L1Zg", "reweighted_qqH_htt_0L1Zg125"),
               std::make_pair("wt_L1Zgint", "reweighted_qqH_htt_0L1Zgf05ph0125"),
           }}},
      categories{
          channel_prefix + "_inclusive",
          channel_prefix + "_0jet",
          channel_prefix + "_boosted",
          channel_prefix + "_vbf",
          channel_prefix + "_vbf_ggHMELA_bin1",
          channel_prefix + "_vbf_ggHMELA_bin2",
          channel_prefix + "_vbf_ggHMELA_bin3",
          channel_prefix + "_vbf_ggHMELA_bin4",
          channel_prefix + "_vbf_ggHMELA_bin5",
          channel_prefix + "_vbf_ggHMELA_bin6",
          channel_prefix + "_vbf_ggHMELA_bin7",
          channel_prefix + "_vbf_ggHMELA_bin8",
          channel_prefix + "_vbf_ggHMELA_bin9",
          channel_prefix + "_vbf_ggHMELA_bin10",
          channel_prefix + "_vbf_ggHMELA_bin11",
          channel_prefix + "_vbf_ggHMELA_bin12"} {}

#endif  // INCLUDE_TEMPLATETOOL_H_
