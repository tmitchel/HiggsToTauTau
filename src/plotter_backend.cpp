// Copyright 2019 Tyler Mitchell

#include "../include/plotter_backend.h"

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
    ff_file = std::make_shared<TFile>((("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data2017/SM2017/tight/vloose/" + channel_prefix + "/fakeFactors.root").c_str()));
  } else if (year == "2016") {
    ff_file = std::make_shared<TFile>((("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data2016/SM2016_ML/tight/" + channel_prefix + "/fakeFactors_tight.root").c_str()));
  } else {
    std::cerr << "Bad year" << std::endl;
  }
  ff_weight = reinterpret_cast<FakeFactor *>(ff_file->Get("ff_comb"));
  ff_file->Close();
}

// create empty histograms to hold jetFakes and create
// directory structure in TFile. (1D)
void HistTool::includeTemplates() {
  fout->cd();

  for (auto cat : categories) {
    // make a 2d template
    hists_2d[cat.c_str()] = vector<shared_ptr<TH2F>>();

    if (cat.find("0jet") != string::npos) {
      fakes_2d.push_back(std::make_shared<TH2F>("fake_0jet", "fake_SS", bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
    } else if (cat.find("boosted") != string::npos) {
      fakes_2d.push_back(std::make_shared<TH2F>("fake_boosted", "fake_SS", bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
    } else {
      fakes_2d.push_back(std::make_shared<TH2F>(("fake_" + cat).c_str(), "fake_SS", bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
    }

    // make all of the directories for templates
    // for (auto it = hists_2d.begin(); it != hists_2d.end(); it++) {
      fout->cd();
      fout->mkdir(cat.c_str());
      fout->cd();
    // }
  }
}

// create empty histograms to hold jetFakes and create
// directory structure in TFile. (2D)
void HistTool::includePlots() {
  fout->cd();
  fout->mkdir("plots");
  fout->cd();
  for (auto it = t.variables.begin(); it != t.variables.end(); it++) {
    auto var = it->first;
    auto bins_1d = it->second;
    fout->cd();
    fout->mkdir(("plots/"+var).c_str());
    fout->cd();

    for (auto cat : categories) {
      // add histograms for holding plots
      hists_1d[(var+"/"+cat).c_str()] = vector<shared_ptr<TH1F>>();
      if (cat.find("0jet") != string::npos) {
        fakes_1d.push_back(std::make_shared<TH1F>((var + "fake_0jet").c_str(), "fake_SS", bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
      } else if (cat.find("boosted") != string::npos) {
        fakes_1d.push_back(std::make_shared<TH1F>((var + "fake_boosted").c_str(), "fake_SS", bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
      } else {
        fakes_1d.push_back(std::make_shared<TH1F>((var + "fake_" + cat).c_str(), "fake_SS", bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
      }

      // make a plots directory to store the plots
      fout->cd();
      fout->mkdir(("plots/" + var + "/" + cat).c_str());
      fout->cd();
    }
  }
}

// create empty histograms for the input sample. This is done
// before looping over events so that this histogram already
// exists to be filled in the loop. (1D)
void HistTool::initVectors1d(string name) {
  for (auto it = t.variables.begin(); it != t.variables.end(); it++) {
    auto var = it->first;
    auto bins_1d = it->second;

    fout->cd();
    for (auto key : hists_1d) {
      fout->cd(("plots/" + key.first).c_str());

      // convert name for data
      if (name.find("Data") != string::npos) {
        name = "data_obs";
      }

      if (key.first.find(var) == std::string::npos) {
        continue;
      }

      // create empty histogram for every category
      hists_1d.at((key.first).c_str())
          .push_back(std::make_shared<TH1F>((var + "_" + name).c_str(), name.c_str(), bins_1d.at(0), bins_1d.at(1), bins_1d.at(2)));
    }
  }
}

// create empty histograms for the input sample. This is done
// before looping over events so that this histogram already
// exists to be filled in the loop. (2D)
void HistTool::initVectors2d(string name) {
  for (auto key : hists_2d) {
    fout->cd(key.first.c_str());
    if (name.find("Data") != string::npos) {
      name = "data_obs";
    }
    if (key.first == channel_prefix + "_0jet") {
      hists_2d.at(key.first.c_str())
          .push_back(std::make_shared<TH2F>(name.c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
    } else if (key.first == channel_prefix + "_boosted") {
      hists_2d.at(key.first.c_str())
          .push_back(std::make_shared<TH2F>(name.c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
    } else if (key.first.find("_vbf") != string::npos) {
      hists_2d.at(key.first.c_str())
          .push_back(std::make_shared<TH2F>(name.c_str(), name.c_str(), bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
    }
  }
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void HistTool::initSystematics(string name) {
  for (auto key : FF_systs) {
    fout->cd(key.first.c_str());
    string name = "jetFakes_";
    for (auto syst : systematics) {
      if (key.first == channel_prefix + "_0jet") {
        FF_systs.at(key.first.c_str())
            .push_back(std::make_shared<TH2F>((name + syst).c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
      } else if (key.first == channel_prefix + "_boosted") {
        FF_systs.at(key.first.c_str())
            .push_back(std::make_shared<TH2F>((name + syst).c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
      } else if (key.first.find("_vbf") != string::npos) {
        FF_systs.at(key.first.c_str())
            .push_back(std::make_shared<TH2F>((name + syst).c_str(), name.c_str(), bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
      }
    }
  }
  std::cout << "initialized systematics" << std::endl;
}

// fill fake fractions to be used in calculating jet fakes
void HistTool::fillFraction(int cat, string name, double var1, double var2, double weight) {
  shared_ptr<TH2F> hist;
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

// convert get weights to convert data events into
// jetFakes (2D)
void HistTool::convertDataToFake(Categories cat, string name, double var1, double var2,
                                 double vis_mass, double njets, double t1_pt, double t1_decayMode,
                                 double mt, double lep_iso, double weight) {
  auto bin_x = data.at(cat)->GetXaxis()->FindBin(vis_mass);
  auto bin_y = data.at(cat)->GetYaxis()->FindBin(njets);
  auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                      frac_w.at(cat)->GetBinContent(bin_x, bin_y),
                                      frac_tt.at(cat)->GetBinContent(bin_x, bin_y),
                                      frac_qcd.at(cat)->GetBinContent(bin_x, bin_y)});
  if (name.find("Data") != string::npos) {
    fakes_2d.at(cat)->Fill(var1, var2, weight * fakeweight);
  }
}

// convert get weights to convert data events into
// jetFakes (1D)
void HistTool::convertDataToFake(Categories cat, int nvar, string name, double var1, double vis_mass,
                                 double njets, double t1_pt, double t1_decayMode, double mt,
                                 double lep_iso, double weight) {
  auto bin_x = data.at(cat)->GetXaxis()->FindBin(vis_mass);
  auto bin_y = data.at(cat)->GetYaxis()->FindBin(njets);
  auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                      frac_w.at(cat)->GetBinContent(bin_x, bin_y),
                                      frac_tt.at(cat)->GetBinContent(bin_x, bin_y),
                                      frac_qcd.at(cat)->GetBinContent(bin_x, bin_y)});
  if (name.find("Data") != string::npos) {
    fakes_1d.at(cat*nvar)->Fill(var1, weight * fakeweight);
  }
}

// Write the output templates
void HistTool::writeTemplates() {
  // write normal templates
  for (auto cat : hists_2d) {
    fout->cd(cat.first.c_str());
    for (auto hist : cat.second) {
      hist->Write();
    }
  }

  // write jetFakes templates after setting the correct name
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

    // do the same loop, but for fake factor systematics
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

// Write the output histograms
void HistTool::writeHistos() {
  // write normal histograms
  for (auto cat : hists_1d) {
    fout->cd(("plots/" + cat.first).c_str());
    for (auto hist : cat.second) {
      hist->Write();
    }
  }

  int nvar = 1;
  for (auto it = t.variables.begin(); it != t.variables.end(); it++) {
    auto var = it->first;
    // write jetFakes histograms after setting the correct name
    for (auto cat = 0; cat < categories.size(); cat++) {
      fout->cd(("plots/" + var + "/" + categories.at(cat)).c_str());
      auto fake_hist = fakes_1d.at(cat * nvar);
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

      // do the same loop, but for fake factor systematics
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
    nvar++;
  }
  //fout->Close();
  //std::cout << "CLOSE" << std::endl;
}

// find the correct "bin" based on input variables
Categories HistTool::getCategory(double D0_ggH, double nn) {
  if (nn < 0.5) {
    if (D0_ggH > 0 && D0_ggH <= 1. / 6) {
      return vbf_ggHMELA_bin1_NN_bin1;
    } else if (D0_ggH <= 2. / 6) {
      return vbf_ggHMELA_bin2_NN_bin1;
    } else if (D0_ggH <= 3. / 6) {
      return vbf_ggHMELA_bin3_NN_bin1;
    } else if (D0_ggH <= 4. / 6) {
      return vbf_ggHMELA_bin4_NN_bin1;
    } else if (D0_ggH <= 5. / 6) {
      return vbf_ggHMELA_bin5_NN_bin1;
    } else if (D0_ggH <= 6. / 6) {
      return vbf_ggHMELA_bin6_NN_bin1;
    }
  } else {
    if (D0_ggH > 0 && D0_ggH <= 1. / 6) {
      return vbf_ggHMELA_bin1_NN_bin2;
    } else if (D0_ggH <= 2. / 6) {
      return vbf_ggHMELA_bin2_NN_bin2;
    } else if (D0_ggH <= 3. / 6) {
      return vbf_ggHMELA_bin3_NN_bin2;
    } else if (D0_ggH <= 4. / 6) {
      return vbf_ggHMELA_bin4_NN_bin2;
    } else if (D0_ggH <= 5. / 6) {
      return vbf_ggHMELA_bin5_NN_bin2;
    } else if (D0_ggH <= 6. / 6) {
      return vbf_ggHMELA_bin6_NN_bin2;
    }
  }
}
