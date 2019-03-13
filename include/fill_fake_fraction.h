// Copyright 2019 Tyler Mitchell

#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <vector>

#include "../include/CLParser.h"
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
                  vbf_ggHMELA_bin12_NN_bin1 };

class FakeFractions {
 public:
  FakeFractions(std::string, std::string, std::string);
  Categories getCategory(double, double);
  void fillFraction(int, std::string, double, double, double);
  void writeTemplates();

 private:
  std::vector<TH2F *> data, frac_w, frac_tt, frac_real, frac_qcd;
  std::vector<Float_t> mvis_bins, njets_bins;
  std::vector<std::string> categories;
  FakeFactor *ff_weight;
  TFile *fout;
};

FakeFractions::FakeFractions(std::string channel_prefix, std::string year, std::string suffix)
    : fout(new TFile(("Output/fake_fractions/" + channel_prefix + year + "_" + suffix + ".root").c_str(), "recreate")),
      mvis_bins({0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000}),
      njets_bins({-0.5, 0.5, 1.5, 15}),
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
          channel_prefix + "_vbf_ggHMELA_bin12_NN_bin2"} {
  for (auto cat : categories) {
    data.push_back(new TH2F(("data_" + cat).c_str(), ("data_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_w.push_back(new TH2F(("frac_w_" + cat).c_str(), ("frac_w_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_tt.push_back(new TH2F(("frac_tt_" + cat).c_str(), ("frac_tt_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_real.push_back(new TH2F(("frac_real_" + cat).c_str(), ("frac_real_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_qcd.push_back(new TH2F(("frac_qcd_" + cat).c_str(), ("frac_qcd_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
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

void FakeFractions::fillFraction(int cat, std::string name, double var1, double var2, double weight) {
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

// basically a map from 2 inputs -> 1 Category
Categories FakeFractions::getCategory(double vbf_var3, double vbf_var4 = -1) {
  double edge = 1. / 6.;
  if (vbf_var3 > 0 && vbf_var3 <= 1. * edge) {
    return vbf_ggHMELA_bin1_NN_bin1;
  } else if (vbf_var3 <= 2. * edge) {
    return vbf_ggHMELA_bin2_NN_bin1;
  } else if (vbf_var3 <= 3. * edge) {
    return vbf_ggHMELA_bin3_NN_bin1;
  } else if (vbf_var3 <= 4. * edge) {
    return vbf_ggHMELA_bin4_NN_bin1;
  } else if (vbf_var3 <= 5. * edge) {
    return vbf_ggHMELA_bin5_NN_bin1;
  } else if (vbf_var3 <= 6. * edge) {
    return vbf_ggHMELA_bin6_NN_bin1;
  }
}

// write output histograms including the QCD histograms after scaling by OS/SS ratio
void FakeFractions::writeTemplates() {
  for (auto i = 0; i < categories.size(); i++) {
    fout->cd(categories.at(i).c_str());
    std::vector<TH2F *> toWrite = {data.at(i), frac_w.at(i), frac_tt.at(i), frac_real.at(i), frac_qcd.at(i)};
    for (auto hist : toWrite) {
      hist->Write();
    }
  }
  fout->Close();
}
