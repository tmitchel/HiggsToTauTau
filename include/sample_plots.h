// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_SAMPLE_H_
#define INCLUDE_SAMPLE_H_

#include <map>
#include <string>
#include <vector>
#include "./TemplateTool.h"

// Sample_Plots inherits from the TemplateTool, which contains basic information like
// ff systematic names, the acWeight map, and the names of the categories for
// the analysis. This is mainly used just to keep all of the long lists out of
// this file so it is easier to read.
class Sample_Plots : public TemplateTool {
 public:
  Sample_Plots(std::string, std::string, std::string, std::string, std::shared_ptr<TFile>, std::vector<Float_t>);
  void set_branches(std::shared_ptr<TTree>, std::string);
  void load_fake_fractions(std::string);
  void fill_histograms(std::shared_ptr<TTree>, std::string);
  std::string get_category(double);
  void convert_data_to_fake(std::string, double);
  void write_histograms();
  void set_variable(std::shared_ptr<TTree>, std::string);

 private:
  std::string sample_name;
  std::map<std::string, TH1F *> fakes, hists;
  std::map<std::string, TH2F *> fake_fractions;
  std::vector<Float_t> var_bins;

  // get variables from file
  Int_t is_signal, is_antiTauIso, OS;                  // flags
  Float_t nbjets, njets, mjj;                          // selection
  Float_t weight, acWeightVal;                         // weights
  Float_t lep_iso, mt, t1_decayMode, vis_mass, t1_pt;  // for fake factor
  Float_t D0_ggH;                                      // for 3D separation
  Float_t observable;                                  // variable to plot
};

// Sample_Plots constructor. Loads all of the basic information from the parent TemplateTool class.
// Empty histograms for fake jets and for normal samples are created and stored in the maps:
// fakes and hists. The category is used as the key for the maps. To get the
// "mt_vbf_ggHMELA_bin1_NN_bin1" histogram use
//
//       hists.at("mt_vbf_ggHMELA_bin1_NN_bin1");
//
// this will give a pointer to the histogram. The fakes is accessed the exact same way, but
// is only available for data.
Sample_Plots::Sample_Plots(std::string channel_prefix, std::string year, std::string in_sample_name, std::string suffix, std::shared_ptr<TFile> output_file, std::vector<Float_t> var_bins)
    : TemplateTool(channel_prefix, year, suffix, output_file),
      sample_name(in_sample_name),
      acWeightVal(1.),
      var_bins(var_bins) {
  for (auto cat : categories) {
    fout->cd(cat.c_str());

    // convert data name to be appropriate and do ff things
    if (sample_name.find("Data") != std::string::npos || sample_name == "data_obs") {
      sample_name = "data_obs";
      fakes[cat] = new TH1F(("fake_" + cat).c_str(), "fake_SS", var_bins.at(0), var_bins.at(1), var_bins.at(2));
    }

    // create output histograms
    hists[cat] = new TH1F(sample_name.c_str(), sample_name.c_str(), var_bins.at(0), var_bins.at(1), var_bins.at(2));
  }
  fout->cd();
}

// set_branches takes an input TTree and sets all of the needed branch
// addresses to the appropriate member variables. Provided an acWeight,
// it will also load the acWeight stored in the root file for reweighting
// JHU samples.
void Sample_Plots::set_branches(std::shared_ptr<TTree> tree, std::string acWeight) {
  std::string iso;
  std::string tree_name = tree->GetName();
  if (tree_name.find("etau_tree") != std::string::npos) {
    iso = "el_iso";
  } else if (tree_name.find("mutau_tree") != std::string::npos) {
    iso = "mu_iso";
  }

  tree->SetBranchAddress("evtwt", &weight);
  tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
  tree->SetBranchAddress("vis_mass", &vis_mass);
  tree->SetBranchAddress("mjj", &mjj);
  tree->SetBranchAddress("mt", &mt);
  tree->SetBranchAddress(iso.c_str(), &lep_iso);
  tree->SetBranchAddress("njets", &njets);
  tree->SetBranchAddress("nbjets", &nbjets);
  tree->SetBranchAddress("D0_ggH", &D0_ggH);
  tree->SetBranchAddress("is_signal", &is_signal);
  tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
  tree->SetBranchAddress("OS", &OS);
  tree->SetBranchAddress("t1_pt", &t1_pt);

  if (acWeight != "None") {
    tree->SetBranchAddress(acWeight.c_str(), &acWeightVal);
  }
}

void Sample_Plots::set_variable(std::shared_ptr<TTree> tree, std::string var_name) {
  if (var_name == "t1_decayMode") {
    observable = t1_decayMode;
  } else if (var_name == "vis_mass") {
    observable = vis_mass;
  } else if (var_name == "mjj") {
    observable = mjj;
  } else if (var_name == "mt") {
    observable = mt;
  } else if (var_name == "mu_iso" || var_name == "el_iso") {
    observable = lep_iso;
  } else if (var_name == "njets") {
    observable = njets;
  } else if (var_name == "D0_ggH") {
    observable = D0_ggH;
  } else if (var_name == "t1_pt") {
    observable = t1_pt;
  } else {
    tree->SetBranchAddress(var_name.c_str(), &observable);
  }
}

// fill_histograms does the main work. It will open the given file and loop
// through all events making selections and filling histograms. Additionally,
// if the Sample_Plots is for data, it will fill the fake jets histogram. If "doSyst"
// is true, fill_histograms will also do all the jet fakes systematics shifts,
// provided the Sample_Plots is for data. Passing "acWeight" allows you to reweight
// JHU samples to different coupling scenarios.
void Sample_Plots::fill_histograms(std::shared_ptr<TTree> tree, std::string acWeight = "None") {
  set_branches(tree, acWeight);
  fout->cd();

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

    // event selection
    if (nbjets > 0) {
      continue;
    }
    cat0 = (njets == 0);
    cat1 = (njets == 1 || (njets > 1 && (mjj < 300)));
    cat2 = (njets > 1 && mjj > 300);

    // find the correct MELA ggH/Higgs pT bin for this event
    auto ACcat = get_category(D0_ggH);

    // fill histograms
    if (is_signal) {
      if (cat0) {
        hists.at(channel_prefix + "_0jet")->Fill(observable, weight);
      } else if (cat1) {
        hists.at(channel_prefix + "_boosted")->Fill(observable, weight);
      } else if (cat2) {
        hists.at(channel_prefix + "_vbf")->Fill(observable, weight);
        if (ACcat != "skip") {
          hists.at(ACcat)->Fill(observable, weight);
        }
      }
    } else if (is_antiTauIso && sample_name == "data_obs") {
      if (cat0) {
        // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
        convert_data_to_fake(channel_prefix + "_0jet", observable);  // 2d template
      } else if (cat1) {
        convert_data_to_fake(channel_prefix + "_boosted", observable);
      } else if (cat2) {
        convert_data_to_fake(channel_prefix + "_vbf", observable);
        if (ACcat != "skip") {
          convert_data_to_fake(ACcat, observable);
        }
      }
    }
  }
}

// convert_data_to_fake puts an event in the fake factor histogram. It takes the category for
// this event as well as the 2 input variables for the histogram. It then reads the weights
// from the fake fraction histograms that were loaded in load_fake_fractions. These weights
// are used with the input variables to fill the fake histogram with the correct weight. If
// the "syst" parameter is passed, read the weight for the provided systematic shift.
void Sample_Plots::convert_data_to_fake(std::string cat, double var1) {
  fout->cd();
  auto bin_x = fake_fractions.at(cat + "_data")->GetXaxis()->FindBin(vis_mass);
  auto bin_y = fake_fractions.at(cat + "_data")->GetYaxis()->FindBin(njets);
  double fakeweight;
  fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                 fake_fractions.at(cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                                 fake_fractions.at(cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                                 fake_fractions.at(cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)});
  fakes.at(cat)->Fill(var1, weight * fakeweight);
}

// load_fake_fractions opens the given file and reads the
// pre-computed fake fraction histograms for each category.
// The histograms are stored in a map with the key being
// the category name + the fraction name. For example, to get
// the W+jets fraction for category "et_vbf" use
//
//       fake_fractions.at("et_vbf_frac_w");
//
// this will return a pointer to the histogram.
void Sample_Plots::load_fake_fractions(std::string file_name) {
  auto ifile = new TFile(file_name.c_str(), "READ");
  fout->cd();
  for (auto cat : categories) {
    fake_fractions[cat + "_data"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "data_" + cat).c_str())->Clone());
    fake_fractions[cat + "_frac_w"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_w_" + cat).c_str())->Clone());
    fake_fractions[cat + "_frac_tt"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_tt_" + cat).c_str())->Clone());
    fake_fractions[cat + "_frac_qcd"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_qcd_" + cat).c_str())->Clone());
  }
  ifile->Close();
}

// get_category will take a variable and return the category
// in which this variable belongs. This is to give us
// "3D histograms" for combine although we can really only
// make a 2D histogram. If for some reason the given parameter
// doesn't fit in any bins, return "skip" to be handled by the
// caller.
std::string Sample_Plots::get_category(double vbf_var3) {
  double edge = 1. / 6.;
  if (vbf_var3 >= 0 && vbf_var3 <= 1. * edge) {
    return channel_prefix + "_vbf_ggHMELA_bin1_NN_bin1";
  } else if (vbf_var3 <= 2. * edge) {
    return channel_prefix + "_vbf_ggHMELA_bin2_NN_bin1";
  } else if (vbf_var3 <= 3. * edge) {
    return channel_prefix + "_vbf_ggHMELA_bin3_NN_bin1";
  } else if (vbf_var3 <= 4. * edge) {
    return channel_prefix + "_vbf_ggHMELA_bin4_NN_bin1";
  } else if (vbf_var3 <= 5. * edge) {
    return channel_prefix + "_vbf_ggHMELA_bin5_NN_bin1";
  } else if (vbf_var3 <= 6. * edge) {
    return channel_prefix + "_vbf_ggHMELA_bin6_NN_bin1";
  } else {
    return "skip";
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

// write_histograms is used to write the histograms to the output root file.
// First, change into the correct directory for the category then write the
// histogram. Additionally, if this Sample_Plots is for data, write the fake factor
// histograms in the same directory.
void Sample_Plots::write_histograms() {
  for (auto cat : categories) {
    fout->cd(cat.c_str());

    // potentially change the name (for JHU)
    hists.at(cat)->Write();
    if (sample_name == "data_obs") {
      auto hist = fakes.at(cat);
      hist->SetName("jetFakes");
      hist->Write();
    }
  }
}

#endif  // INCLUDE_SAMPLE_H_
