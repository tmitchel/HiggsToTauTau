// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_SAMPLE_H_
#define INCLUDE_SAMPLE_H_

#include <map>
#include <string>
#include <vector>
#include "./TemplateTool.h"

class Sample : public TemplateTool {
 public:
  Sample(std::string, std::string, std::string, std::string, std::shared_ptr<TFile>);
  void set_branches(TTree *, std::string);
  void load_fake_fractions(std::string);
  void init_systematics(std::string name);
  void fill_histograms(std::string, std::string, bool, std::string);
  std::string get_category(double, double);
  void convert_data_to_fake(std::string, double, double, int);
  void write_histograms(bool, std::string);

 private:
  std::string sample_name;
  std::map<std::string, TH2F *> fakes_2d, hists_2d;
  std::map<std::string, TH2F *> fake_fractions;
  std::map<std::string, std::vector<TH2F *>> FF_systs;
  std::vector<Float_t> bins_l2, bins_hpt, bins_vbf_var1, bins_lpt, bins_msv1, bins_vbf_var2, bins_hpt2;

  // get variables from file
  Int_t is_signal, is_antiTauIso, OS;                                                           // flags
  Float_t nbjets, njets, mjj;                                                                   // selection
  Float_t weight, acWeightVal;                                                                  // weights
  Float_t lep_iso, mt, t1_decayMode, vis_mass, t1_pt;                                           // for fake factor
  Float_t higgs_pT, m_sv, NN_disc, D0_VBF, D0_ggH, DCP_VBF, DCP_ggH, VBF_MELA, j1_phi, j2_phi;  // observables
};

Sample::Sample(std::string channel_prefix, std::string year, std::string in_sample_name, std::string suffix, std::shared_ptr<TFile> output_file)
    : TemplateTool(channel_prefix, year, suffix, output_file),
      sample_name(in_sample_name),
      acWeightVal(1.),
      // x-axis
      bins_l2{0, 1, 10, 11},
      bins_hpt{0, 100, 150, 200, 250, 300, 5000},
      // bins_vbf_var1{300, 500, 10000},  // real mjj
      bins_vbf_var1{0, 0.25, 0.5, 0.75, 1.},  // actually VBF MELA

      // y-axis
      bins_lpt{0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400},
      bins_msv1{0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300},
      // bins_vbf_var2{0, 80, 100, 115, 130, 150, 1000},
      // bins_vbf_var2{-5, -1.25, -0.75, 0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2., 3.},  // Fisher Disc
      // bins_vbf_var2{0, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.5,  0.6, 0.65, 0.7, 0.8},  // Perceptron
      // bins_vbf_var2{0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.7, 0.8, 0.9, 1.},  // NN including m_sv et2016/mt2017
      // bins_vbf_var2{0, 0.05, 0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.7, 0.8, 0.9, 0.95, 1.}, // mt2016
      bins_vbf_var2{0, 0.25, 0.5, 0.75, 1.} {
  for (auto cat : categories) {
    // make directory in TFile
    fout->cd();
    fout->mkdir(cat.c_str());
    fout->cd(cat.c_str());

    // convert data name to be appropriate and do ff things
    if (sample_name.find("Data") != std::string::npos) {
      sample_name = "data_obs";

      // also push the fake factor histograms only for Data
      if (cat.find("0jet") != std::string::npos) {
        fakes_2d[cat] = new TH2F("fake_0jet", "fake_SS", bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]);
      } else if (cat.find("boosted") != std::string::npos) {
        fakes_2d[cat] = new TH2F("fake_boosted", "fake_SS", bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]);
      } else {
        fakes_2d[cat] = new TH2F(("fake_" + cat).c_str(), "fake_SS", bins_vbf_var1.size() - 1, &bins_vbf_var1[0], bins_vbf_var2.size() - 1, &bins_vbf_var2[0]);
      }

      // histograms for fake-factor are always 2d
      FF_systs[cat.c_str()] = std::vector<TH2F *>();
    }

    // push empty histograms into map to be filled
    if (cat == channel_prefix + "_0jet") {
      hists_2d[cat] = new TH2F(sample_name.c_str(), sample_name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]);
    } else if (cat == channel_prefix + "_boosted") {
      hists_2d[cat] = new TH2F(sample_name.c_str(), sample_name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]);
    } else if (cat.find("_vbf") != std::string::npos) {
      hists_2d[cat] = new TH2F(sample_name.c_str(), sample_name.c_str(), bins_vbf_var1.size() - 1, &bins_vbf_var1[0], bins_vbf_var2.size() - 1, &bins_vbf_var2[0]);
    }
  }

  fout->cd();
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void Sample::init_systematics(std::string name) {
  // systematics still only need the data histograms
  if (name == "Data") {
    for (auto key : FF_systs) {
      fout->cd(key.first.c_str());
      std::string name = "jetFakes_";
      for (auto syst : systematics) {
        if (key.first == channel_prefix + "_0jet") {
          FF_systs.at(key.first.c_str()).push_back(new TH2F((name + syst).c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
        } else if (key.first == channel_prefix + "_boosted") {
          FF_systs.at(key.first.c_str()).push_back(new TH2F((name + syst).c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
        } else if (key.first.find("_vbf") != std::string::npos) {
          FF_systs.at(key.first.c_str()).push_back(new TH2F((name + syst).c_str(), name.c_str(), bins_vbf_var1.size() - 1, &bins_vbf_var1[0], bins_vbf_var2.size() - 1, &bins_vbf_var2[0]));
        }
      }
    }
    std::cout << "initialized systematics" << std::endl;
  }
}

void Sample::set_branches(TTree *tree, std::string acWeight) {
  std::string iso;
  std::string tree_name = tree->GetName();
  if (tree_name.find("etau_tree") != std::string::npos) {
    iso = "el_iso";
  } else if (tree_name.find("mutau_tree") != std::string::npos) {
    iso = "mu_iso";
  }

  tree->SetBranchAddress("evtwt", &weight);
  tree->SetBranchAddress("higgs_pT", &higgs_pT);
  tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
  tree->SetBranchAddress("vis_mass", &vis_mass);
  tree->SetBranchAddress("mjj", &mjj);
  tree->SetBranchAddress("mt", &mt);
  tree->SetBranchAddress(iso.c_str(), &lep_iso);
  tree->SetBranchAddress("m_sv", &m_sv);
  tree->SetBranchAddress("njets", &njets);
  tree->SetBranchAddress("nbjets", &nbjets);
  tree->SetBranchAddress("D0_VBF", &D0_VBF);
  tree->SetBranchAddress("D0_ggH", &D0_ggH);
  tree->SetBranchAddress("DCP_VBF", &DCP_VBF);
  tree->SetBranchAddress("DCP_ggH", &DCP_ggH);
  tree->SetBranchAddress("is_signal", &is_signal);
  tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
  tree->SetBranchAddress("OS", &OS);
  tree->SetBranchAddress("t1_pt", &t1_pt);
  tree->SetBranchAddress("VBF_MELA", &VBF_MELA);
  tree->SetBranchAddress("j1_phi", &j1_phi);
  tree->SetBranchAddress("j2_phi", &j2_phi);
  tree->SetBranchAddress("NN_disc", &NN_disc);

  if (acWeight != "None") {
    tree->SetBranchAddress(acWeight.c_str(), &acWeightVal);
  }
}

void Sample::fill_histograms(std::string ifile, std::string tree_name, bool doSyst = false, std::string acWeight = "None") {
  auto fin = new TFile(ifile.c_str(), "read");
  auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));
  set_branches(tree, acWeight);

  bool cat0(false), cat1(false), cat2(false);
  float vbf_var1(0.), vbf_var2(0.), vbf_var3(0.);

  // start the event loop
  for (auto i = 0; i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    weight *= acWeightVal;  // acWeightVal = 1 for SM

    // only look at opposite-sign events
    if (OS == 0) {
      continue;
    }

    // choose observables
    vbf_var1 = VBF_MELA;
    vbf_var2 = NN_disc;
    vbf_var3 = D0_ggH;
    // vbf_var1 = mjj;
    // vbf_var3 = dPhijj;
    // vbf_var3 = TMath::ACos(TMath::Cos(j1_phi - j2_phi));

    // event selection
    if (nbjets > 0) {
      continue;
    }
    cat0 = (njets == 0);
    cat1 = (njets == 1 || (njets > 1 && (mjj < 300)));
    cat2 = (njets > 1 && mjj > 300);

    // find the correct MELA ggH/Higgs pT bin for this event
    auto ACcat = get_category(vbf_var3);

    // fill histograms
    if (is_signal) {
      if (cat0) {
        hists_2d.at(channel_prefix + "_0jet")->Fill(t1_decayMode, vis_mass, weight);
      } else if (cat1) {
        hists_2d.at(channel_prefix + "_boosted")->Fill(higgs_pT, m_sv, weight);
      } else if (cat2) {
        hists_2d.at(channel_prefix + "_vbf")->Fill(vbf_var1, vbf_var2, weight);
        hists_2d.at(ACcat)->Fill(vbf_var1, vbf_var2, weight);
      }
    } else if (is_antiTauIso && sample_name == "Data") {
      if (cat0) {
        // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
        convert_data_to_fake(channel_prefix + "_0jet", t1_decayMode, vis_mass);  // 2d template
      } else if (cat1) {
        convert_data_to_fake(channel_prefix + "_boosted", higgs_pT, m_sv);
      } else if (cat2) {
        convert_data_to_fake(channel_prefix + "_vbf", vbf_var1, vbf_var2);
        convert_data_to_fake(ACcat, vbf_var1, vbf_var2);
      }

      // loop through all systematic names and get the corresponding weight to fill a histogram
      if (doSyst) {
        for (int i = 0; i < systematics.size(); i++) {
          if (cat0) {
            // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
            convert_data_to_fake(channel_prefix + "_0jet", t1_decayMode, vis_mass, i);  // 2d template
          } else if (cat1) {
            convert_data_to_fake(channel_prefix + "_boosted", higgs_pT, m_sv, i);
          } else if (cat2) {
            convert_data_to_fake(channel_prefix + "_vbf", vbf_var1, vbf_var2, i);
            convert_data_to_fake(ACcat, vbf_var1, vbf_var2, i);
          }
        }
      }
    }
  }
}

void Sample::convert_data_to_fake(std::string cat, double var1, double var2, int syst = -1) {
  auto bin_x = fake_fractions.at(cat + "_data")->GetXaxis()->FindBin(vis_mass);
  auto bin_y = fake_fractions.at(cat + "_data")->GetYaxis()->FindBin(njets);
  double fakeweight;
  if (syst == -1) {
    fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                   fake_fractions.at(cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                                   fake_fractions.at(cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                                   fake_fractions.at(cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)});
    fakes_2d.at(cat)->Fill(var1, var2, weight * fakeweight);
  } else {
    fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                   fake_fractions.at(cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                                   fake_fractions.at(cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                                   fake_fractions.at(cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)},
                                  systematics.at(syst));
    FF_systs.at(cat).at(syst)->Fill(var1, var2, weight * fakeweight);
  }
}

void Sample::load_fake_fractions(std::string file_name) {
  auto ifile = new TFile(file_name.c_str(), "READ");
  for (auto cat : categories) {
    fake_fractions[cat + "_data"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "data_" + cat).c_str()));
    fake_fractions[cat + "_frac_w"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_w_" + cat).c_str()));
    fake_fractions[cat + "_frac_tt"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_tt_" + cat).c_str()));
    fake_fractions[cat + "_frac_qcd"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_qcd_" + cat).c_str()));
  }
}

// basically a map from 2 inputs -> 1 Category
std::string Sample::get_category(double vbf_var3, double vbf_var4 = -1) {
  double edge = 1. / 6.;
  if (vbf_var3 > 0 && vbf_var3 <= 1. * edge) {
    return "vbf_ggHMELA_bin1_NN_bin1";
  } else if (vbf_var3 <= 2. * edge) {
    return "vbf_ggHMELA_bin2_NN_bin1";
  } else if (vbf_var3 <= 3. * edge) {
    return "vbf_ggHMELA_bin3_NN_bin1";
  } else if (vbf_var3 <= 4. * edge) {
    return "vbf_ggHMELA_bin4_NN_bin1";
  } else if (vbf_var3 <= 5. * edge) {
    return "vbf_ggHMELA_bin5_NN_bin1";
  } else if (vbf_var3 <= 6. * edge) {
    return "vbf_ggHMELA_bin6_NN_bin1";
  }

  //  else if (D0_ggH <= 7.*edge) {
  //    return vbf_ggHMELA_bin7_NN_bin1;
  //  } else if (D0_ggH <= 8.*edge) {
  //    return vbf_ggHMELA_bin8_NN_bin1;
  //  } else if (D0_ggH <= 9.*edge) {
  //    return vbf_ggHMELA_bin9_NN_bin1;
  //  } else if (D0_ggH <= 10.*edge) {
  //    return vbf_ggHMELA_bin10_NN_bin1;
  //  } else if (D0_ggH <= 11.*edge) {
  //    return vbf_ggHMELA_bin11_NN_bin1;
  //  } else if (D0_ggH <= 12.*edge) {
  //    return vbf_ggHMELA_bin12_NN_bin1;
  //  }
}

void Sample::write_histograms(bool doSyst = false, std::string newName = "") {
  for (auto cat : categories) {
    fout->cd(cat.c_str());

    // potentially change the name (for JHU)
    if (newName != "") {
      auto hist = hists_2d.at(cat);
      hist->SetName(newName.c_str());
    } else {
      hists_2d.at(cat)->Write();
    }

    if (sample_name == "Data") {
      fakes_2d.at(cat)->Write();
    }
  }
}

#endif  // INCLUDE_SAMPLE_H_
