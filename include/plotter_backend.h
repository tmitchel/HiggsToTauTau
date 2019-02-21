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

#endif  // INCLUDE_PLOTTER_BACKEND_H_
