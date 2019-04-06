// Copyright 2019 Tyler Mitchell

#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TH2F.h"
#include "TTree.h"

enum Categories { zeroJet, boosted, vbf };

class FakeFractions {
 public:
  FakeFractions(std::string, std::string, std::string);
  void fillFraction(int, std::string, double, double, double);
  void writeTemplates();
  void fillQCD();

 private:
  std::vector<TH2F *> data, frac_w, frac_tt, frac_real, frac_qcd;
  std::vector<Float_t> mvis_bins, njets_bins;
  std::vector<std::string> categories;
  TFile *fout;
};

FakeFractions::FakeFractions(std::string channel_prefix, std::string year, std::string suffix)
    : fout(new TFile(("Output/fake_fractions/" + channel_prefix + year + "_" + suffix + ".root").c_str(), "recreate")),
      mvis_bins({0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000}),
      njets_bins({-0.5, 0.5, 1.5, 15}),
      categories{
          channel_prefix + "_0jet",
          channel_prefix + "_boosted",
          channel_prefix + "_vbf"} {
  for (auto cat : categories) {
    data.push_back(new TH2F(("data_" + cat).c_str(), ("data_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_w.push_back(new TH2F(("frac_w_" + cat).c_str(), ("frac_w_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_tt.push_back(new TH2F(("frac_tt_" + cat).c_str(), ("frac_tt_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_real.push_back(new TH2F(("frac_real_" + cat).c_str(), ("frac_real_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
    frac_qcd.push_back(new TH2F(("frac_qcd_" + cat).c_str(), ("frac_qcd_" + cat).c_str(), mvis_bins.size() - 1, &mvis_bins[0], njets_bins.size() - 1, &njets_bins[0]));
  }

  // make all of the directories for templates
  for (auto cat : categories) {
    fout->cd();
    fout->mkdir(cat.c_str());
    fout->cd();
  }
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

void FakeFractions::fillQCD() {
  // calculate fake-fractions
  for (int i = 0; i < data.size(); i++) {
    frac_qcd.at(i) = reinterpret_cast<TH2F *>(data.at(i)->Clone());
    frac_qcd.at(i)->SetName(("frac_qcd_"+categories.at(i)).c_str());
    frac_qcd.at(i)->Add(frac_w.at(i), -1);
    frac_qcd.at(i)->Add(frac_tt.at(i), -1);
    frac_qcd.at(i)->Add(frac_real.at(i), -1);

    frac_w.at(i)->Divide(data.at(i));
    frac_tt.at(i)->Divide(data.at(i));
    frac_real.at(i)->Divide(data.at(i));
    frac_qcd.at(i)->Divide(data.at(i));
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
