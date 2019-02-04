
// system includes
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// ROOT includes
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

enum Categories { zeroJet,
                  boosted,
                  vbf,
                  vbf_ggHMELA_bin1_lowH,
                  vbf_ggHMELA_bin2_lowH,
                  vbf_ggHMELA_bin3_lowH,
                  vbf_ggHMELA_bin4_lowH,
                  vbf_ggHMELA_bin5_lowH,
                  vbf_ggHMELA_bin6_lowH,
                  vbf_ggHMELA_bin7_lowH,
                  vbf_ggHMELA_bin8_lowH,
                  vbf_ggHMELA_bin9_lowH,
                  vbf_ggHMELA_bin10_lowH,
                  vbf_ggHMELA_bin11_lowH,
                  vbf_ggHMELA_bin12_lowH,
                  vbf_ggHMELA_bin1_highH,
                  vbf_ggHMELA_bin2_highH,
                  vbf_ggHMELA_bin3_highH,
                  vbf_ggHMELA_bin4_highH,
                  vbf_ggHMELA_bin5_highH,
                  vbf_ggHMELA_bin6_highH,
                  vbf_ggHMELA_bin7_highH,
                  vbf_ggHMELA_bin8_highH,
                  vbf_ggHMELA_bin9_highH,
                  vbf_ggHMELA_bin10_highH,
                  vbf_ggHMELA_bin11_highH,
                  vbf_ggHMELA_bin12_highH};

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

// class to hold the histograms until I'm ready to write them
class HistTool {
 public:
  HistTool(std::string, std::string, std::string, bool, bool);
  ~HistTool() { delete ff_weight; }
  void writeHistos();
  void writeTemplates();
  void initVectors1d(std::string);
  void initVectors2d(std::string);
  void initSystematics(std::string);
  void includePlots(std::vector<int>, std::string);
  void fillFraction(int, std::string, double, double, double);
  void convertDataToFake(Categories, std::string, double, double, double, double, double, double, double, double);  // 1d
  void convertDataToFake(Categories, std::string, double, double, double, double, double, double, double, double, double);  // 2d
  void histoLoop(std::vector<std::string>, std::string, std::string, std::string);
  void getJetFakes(std::vector<std::string>, std::string, std::string, bool);
  Categories getCategory(double, double);

  bool doNN, old_selection;
  TFile *fout;
  FakeFactor *ff_weight;
  std::string channel_prefix, var;
  std::vector<std::string> categories, systematics;
  std::vector<float> mvis_bins, njets_bins;
  std::map<std::string, std::string> acNameMap;
  std::map<std::string, std::vector<TH1F *>> hists_1d;
  std::map<std::string, std::vector<TH2F *>> hists_2d, FF_systs;
  std::vector<TH1F *> fakes_1d;
  std::vector<TH2F *> data, fakes_2d, frac_w, frac_tt, frac_real, frac_qcd;

  // binning
  std::vector<int> bins_1d;
  std::vector<Float_t> bins_l2, bins_hpt, bins_mjj, bins_lpt, bins_msv1, bins_msv2, bins_hpt2;
};

// HistTool contructor to create the output file, the qcd histograms with the correct binning
// and the map from categories to vectors of TH2F*'s. Each TH2F* in the vector corresponds to
// one file that is being put into that categories directory in the output tempalte
HistTool::HistTool(std::string channel_prefix, std::string year, std::string suffix = "final", bool doNN = false, bool old = false)
    : fout(new TFile(("Output/templates/" + channel_prefix + year + "_" + suffix + ".root").c_str(), "recreate")),
      mvis_bins({0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000}),
      njets_bins({-0.5, 0.5, 1.5, 15}),
      // x-axis
      bins_l2{0, 1, 10, 11},
      bins_hpt{0, 100, 150, 200, 250, 300, 5000},
      bins_mjj{300, 500, 10000},

      // y-axis
      bins_lpt{0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400},
      bins_msv1{0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300},
      bins_msv2{0, 80, 100, 115, 130, 150, 1000},
      bins_hpt2{0, 150, 10000},
      channel_prefix(channel_prefix),
      doNN(doNN),
      old_selection(old),
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
          channel_prefix + "_vbf_ggHMELA_bin1_lowH",
          channel_prefix + "_vbf_ggHMELA_bin2_lowH",
          channel_prefix + "_vbf_ggHMELA_bin3_lowH",
          channel_prefix + "_vbf_ggHMELA_bin4_lowH",
          channel_prefix + "_vbf_ggHMELA_bin5_lowH",
          channel_prefix + "_vbf_ggHMELA_bin6_lowH",
          channel_prefix + "_vbf_ggHMELA_bin7_lowH",
          channel_prefix + "_vbf_ggHMELA_bin8_lowH",
          channel_prefix + "_vbf_ggHMELA_bin9_lowH",
          channel_prefix + "_vbf_ggHMELA_bin10_lowH",
          channel_prefix + "_vbf_ggHMELA_bin11_lowH",
          channel_prefix + "_vbf_ggHMELA_bin12_lowH",
          channel_prefix + "_vbf_ggHMELA_bin1_highH",
          channel_prefix + "_vbf_ggHMELA_bin2_highH",
          channel_prefix + "_vbf_ggHMELA_bin3_highH",
          channel_prefix + "_vbf_ggHMELA_bin4_highH",
          channel_prefix + "_vbf_ggHMELA_bin5_highH",
          channel_prefix + "_vbf_ggHMELA_bin6_highH",
          channel_prefix + "_vbf_ggHMELA_bin7_highH",
          channel_prefix + "_vbf_ggHMELA_bin8_highH",
          channel_prefix + "_vbf_ggHMELA_bin9_highH",
          channel_prefix + "_vbf_ggHMELA_bin10_highH",
          channel_prefix + "_vbf_ggHMELA_bin11_highH",
          channel_prefix + "_vbf_ggHMELA_bin12_highH"},
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
  if (doNN) {
    bins_mjj = {0., 0.3, 1.};
  }

  // Create empty histograms for each category to fill later.
  for (auto cat : categories) {
    // make a 2d template
    hists_2d[cat.c_str()] = std::vector<TH2F *>();

    if (cat.find("0jet") != std::string::npos) {
      fakes_2d.push_back(new TH2F("fake_0jet", "fake_SS", bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
    } else if (cat.find("boosted") != std::string::npos) {
      fakes_2d.push_back(new TH2F("fake_boosted", "fake_SS", bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
    } else {
      fakes_2d.push_back(new TH2F(("fake_" + cat).c_str(), "fake_SS", bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
    }

    // histograms for fake-factor are always 2d
    FF_systs[cat.c_str()] = std::vector<TH2F *>();

    data.push_back(new TH2F(("data_" + cat).c_str(), ("data_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_w.push_back(new TH2F(("frac_w_" + cat).c_str(), ("frac_w_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_tt.push_back(new TH2F(("frac_tt_" + cat).c_str(), ("frac_tt_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_real.push_back(new TH2F(("frac_real_" + cat).c_str(), ("frac_real_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_qcd.push_back(new TH2F(("frac_qcd_" + cat).c_str(), ("frac_qcd_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
  }

  // make all of the directories for templates
  for (auto it = hists_2d.begin(); it != hists_2d.end(); it++) {
    fout->cd();
    fout->mkdir((it->first).c_str());
    fout->cd();
  }

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

void HistTool::includePlots(std::vector<int> bins_1d, std::string var) {
  if (bins_1d.size() < 3) {
    std::cerr << "Must give more than 3 bin arguments for plotting" << std::endl;
  }
  this->var = var;
  this->bins_1d = bins_1d;
  fout->cd();
  fout->mkdir("plots");
  fout->cd();

  for (auto cat : categories) {
    // add histograms for holding plots
    hists_1d[cat.c_str()] = std::vector<TH1F *>();
    if (cat.find("0jet") != std::string::npos) {
      fakes_1d.push_back(new TH1F((var+"fake_0jet").c_str(), "fake_SS", bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    } else if (cat.find("boosted") != std::string::npos) {
      fakes_1d.push_back(new TH1F((var+"fake_boosted").c_str(), "fake_SS", bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    } else {
      fakes_1d.push_back(new TH1F((var+"fake_" + cat).c_str(), "fake_SS", bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    }

    // make a plots directory to store the plots

    if (bins_1d.size() > 0) {
      fout->cd();
      fout->mkdir(("plots/" + var + "_" + cat).c_str());
      std::cout << "Making... " << "plots/" + var + "_" + cat << std::endl;
      fout->cd();
    }
  }
}

// change to the correct output directory then create a new TH2F that will be filled for the current input file
void HistTool::initVectors1d(std::string name) {
  fout->cd();
  for (auto key : hists_1d) {
    fout->cd(("plots/"+var+"_"+key.first).c_str());
    if (name.find("Data") != std::string::npos) {
      name = "data_obs";
    }
    if (key.first == channel_prefix + "_0jet") {
      hists_1d.at((key.first).c_str()).push_back(new TH1F((name).c_str(), name.c_str(), bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    } else if (key.first == channel_prefix + "_boosted") {
      hists_1d.at((key.first).c_str()).push_back(new TH1F((name).c_str(), name.c_str(), bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    } else if (key.first.find("_vbf") != std::string::npos) {
      hists_1d.at((key.first).c_str()).push_back(new TH1F((name).c_str(), name.c_str(), bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    }
  }
}

// change to the correct output directory then create a new TH2F that will be filled for the current input file
void HistTool::initVectors2d(std::string name) {
  for (auto key : hists_2d) {
    fout->cd(key.first.c_str());
    if (name.find("Data") != std::string::npos) {
      name = "data_obs";
    }
    if (key.first == channel_prefix + "_0jet") {
      hists_2d.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
    } else if (key.first == channel_prefix + "_boosted") {
      hists_2d.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
    } else if (key.first.find("_vbf") != std::string::npos) {
      hists_2d.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
    }
  }
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void HistTool::initSystematics(std::string name) {
  for (auto key : FF_systs) {
    fout->cd(key.first.c_str());
    std::string name = "jetFakes_";
    for (auto syst : systematics) {
      if (key.first == channel_prefix + "_0jet") {
        FF_systs.at(key.first.c_str()).push_back(new TH2F((name + syst).c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
      } else if (key.first == channel_prefix + "_boosted") {
        FF_systs.at(key.first.c_str()).push_back(new TH2F((name + syst).c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
      } else if (key.first.find("_vbf") != std::string::npos) {
        FF_systs.at(key.first.c_str()).push_back(new TH2F((name + syst).c_str(), name.c_str(), bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
      }
    }
  }
  std::cout << "initialized systematics" << std::endl;
}

void HistTool::fillFraction(int cat, std::string name, double var1, double var2, double weight) {
  TH2F *hist;
  if (name == "Data") {
    hist = data.at(cat);
  } else if (name == "W" || name == "ZJ" || name == "VVJ") {
    hist = frac_w.at(cat);
  } else if (name == "TTJ") {
    hist = frac_tt.at(cat);
  } else if (name == "embedded" || name == "TTT" || name == "VVT") {
    hist = frac_real.at(cat);
  }
  hist->Fill(var1, var2, weight);
}

void HistTool::convertDataToFake(Categories cat, std::string name, double var1, double var2, double vis_mass, double njets, double t1_pt, double t1_decayMode, double mt, double lep_iso, double weight) {
  auto bin_x = data.at(cat)->GetXaxis()->FindBin(vis_mass);
  auto bin_y = data.at(cat)->GetYaxis()->FindBin(njets);
  auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                      frac_w.at(cat)->GetBinContent(bin_x, bin_y),
                                      frac_tt.at(cat)->GetBinContent(bin_x, bin_y),
                                      frac_qcd.at(cat)->GetBinContent(bin_x, bin_y)});
  if (name.find("Data") != std::string::npos) {
    fakes_2d.at(cat)->Fill(var1, var2, weight * fakeweight);
  }
}

void HistTool::convertDataToFake(Categories cat, std::string name, double var1, double vis_mass, double njets, double t1_pt, double t1_decayMode, double mt, double lep_iso, double weight) {
  if (bins_1d.size() > 2) {
    auto bin_x = data.at(cat)->GetXaxis()->FindBin(vis_mass);
    auto bin_y = data.at(cat)->GetYaxis()->FindBin(njets);
    auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                        frac_w.at(cat)->GetBinContent(bin_x, bin_y),
                                        frac_tt.at(cat)->GetBinContent(bin_x, bin_y),
                                        frac_qcd.at(cat)->GetBinContent(bin_x, bin_y)});
    if (name.find("Data") != std::string::npos) {
      fakes_1d.at(cat)->Fill(var1, weight * fakeweight);
    }
  }
}

// write output histograms including the QCD histograms after scaling by OS/SS ratio
void HistTool::writeTemplates() {
  for (auto cat : hists_2d) {
    fout->cd(cat.first.c_str());
    for (auto hist : cat.second) {
      hist->Write();
    }
  }

  for (auto cat = 0; cat < fakes_2d.size(); cat++) {
    fout->cd(categories.at(cat).c_str());
    auto fake_hist = fakes_2d.at(cat);
    fake_hist->SetName("jetFakes");

    // if fake yield is negative, make it zero
    for (auto i = 0; i < fake_hist->GetNbinsX(); i++) {
      for (auto j = 0; j < fake_hist->GetNbinsY(); j++) {
        if (fake_hist->GetBinContent(i, j) < 0) {
          fake_hist->SetBinContent(i, j, 0);
        }
      }
    }
    fake_hist->Write();

    for (auto &hist : FF_systs.at(categories.at(cat))) {
      for (auto i = 0; i < hist->GetNbinsX(); i++) {
        for (auto j = 0; j < hist->GetNbinsY(); j++) {
          if (hist->GetBinContent(i, j) < 0) {
            hist->SetBinContent(i, j, 0);
          }
        }
      }
      hist->Write();
    }
  }
}

void HistTool::writeHistos() {
  for (auto cat : hists_1d) {
    fout->cd(("plots/"+var+"_"+cat.first).c_str());
    for (auto hist : cat.second) {
      hist->Write();
    }
  }
  for (auto cat = 0; cat < fakes_1d.size(); cat++) {
    fout->cd(("plots/"+var+"_"+categories.at(cat)).c_str());
    auto fake_hist = fakes_1d.at(cat);
    fake_hist->SetName("jetFakes");

    // if fake yield is negative, make it zero
    for (auto i = 0; i < fake_hist->GetNbinsX(); i++) {
      for (auto j = 0; j < fake_hist->GetNbinsY(); j++) {
        if (fake_hist->GetBinContent(i, j) < 0) {
          fake_hist->SetBinContent(i, j, 0);
        }
      }
    }
    fake_hist->Write();

    for (auto &hist : FF_systs.at(categories.at(cat))) {
      for (auto i = 0; i < hist->GetNbinsX(); i++) {
        for (auto j = 0; j < hist->GetNbinsY(); j++) {
          if (hist->GetBinContent(i, j) < 0) {
            hist->SetBinContent(i, j, 0);
          }
        }
      }
      hist->Write();
    }
  }

  fout->Close();
}

// basically a map from 2 inputs -> 1 Category
Categories HistTool::getCategory(double D0_ggH, double hpt) {
  if (hpt < 150.) {
    if (D0_ggH > 0 && D0_ggH <= 1./5) {
      return vbf_ggHMELA_bin1_lowH;
    } else if (D0_ggH <= 3./5) {
      return vbf_ggHMELA_bin2_lowH;
    } else if (D0_ggH <= 4./5) {
      return vbf_ggHMELA_bin3_lowH;
    } else if (D0_ggH <= 4.5/5) {
      return vbf_ggHMELA_bin4_lowH;
    } else if (D0_ggH <= 5./5) {
      return vbf_ggHMELA_bin5_lowH;
    }
    //  else if (D0_ggH <= 6./10) {
    //   return vbf_ggHMELA_bin6_lowH;
    // }
    //  else if (D0_ggH <= 7./10) {
    //   return vbf_ggHMELA_bin7_lowH;
    // } else if (D0_ggH <= 8./10) {
    //   return vbf_ggHMELA_bin8_lowH;
    // } else if (D0_ggH <= 9./10) {
    //   return vbf_ggHMELA_bin9_lowH;
    // } else if (D0_ggH <= 10./10) {
    //   return vbf_ggHMELA_bin10_lowH;
    // } 
    // else if (D0_ggH <= 11./10) {
    //   return vbf_ggHMELA_bin11_lowH;
    // } else if (D0_ggH <= 13./10) {
    //   return vbf_ggHMELA_bin12_lowH;
    // }
  } else {
    if (D0_ggH > 0 && D0_ggH <= 1./10) {
      return vbf_ggHMELA_bin1_highH;
    } else if (D0_ggH <= 2./10) {
      return vbf_ggHMELA_bin2_highH;
    } else if (D0_ggH <= 3./10) {
      return vbf_ggHMELA_bin3_highH;
    } else if (D0_ggH <= 4./10) {
      return vbf_ggHMELA_bin4_highH;
    } else if (D0_ggH <= 5./10) {
      return vbf_ggHMELA_bin5_highH;
    } else if (D0_ggH <= 6./10) {
      return vbf_ggHMELA_bin6_highH;
    } else if (D0_ggH <= 7./10) {
      return vbf_ggHMELA_bin7_highH;
    } else if (D0_ggH <= 8./10) {
      return vbf_ggHMELA_bin8_highH;
    } else if (D0_ggH <= 8.5/10) {
      return vbf_ggHMELA_bin9_highH;
    } else if (D0_ggH <= 10./10) {
      return vbf_ggHMELA_bin10_highH;
    }
    //  else if (D0_ggH <= 11./13) {
    //   return vbf_ggHMELA_bin11_highH;
    // } else if (D0_ggH <= 13./13) {
    //   return vbf_ggHMELA_bin12_highH;
    // }
  }
}
