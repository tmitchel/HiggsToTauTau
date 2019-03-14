// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_SAMPLE_H_
#define INCLUDE_SAMPLE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "./TemplateTool.h"

// Sample_Plots inherits from the TemplateTool, which contains basic information like
// ff systematic names, the acWeight map, and the names of the categories for
// the analysis. This is mainly used just to keep all of the long lists out of
// this file so it is easier to read.
class Sample_Plots : public TemplateTool {
 public:
  Sample_Plots(std::string, std::string, std::string, std::string, std::shared_ptr<TFile>, std::map<std::string, std::vector<float>>);
  void set_branches(std::shared_ptr<TTree>, std::string);
  void load_fake_fractions(std::string);
  void fill_histograms(std::shared_ptr<TTree>, std::string);
  std::string get_category(double);
  void convert_data_to_fake(std::string, double, std::string);
  void write_histograms();
  void set_variable(std::shared_ptr<TTree>);

 private:
  std::string sample_name, in_var_name;
  std::map<std::string, TH1F *> fakes, hists;
  std::map<std::string, TH2F *> fake_fractions;
  // std::map<std::string, std::vector<float>> variables;
  std::map<std::string, std::map<std::string, TH1F *>> all_fakes, all_hists;

  // get variables from file
  Int_t is_signal, is_antiTauIso, OS;                  // flags
  Float_t nbjets, njets, mjj;                          // selection
  Float_t weight, acWeightVal;                         // weights
  Float_t lep_iso, mt, t1_decayMode, vis_mass, t1_pt;  // for fake factor
  Float_t D0_ggH;                                      // for 3D separation
  std::map<std::string, std::shared_ptr<Float_t>> observables;                  // variable to plot
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
Sample_Plots::Sample_Plots(std::string channel_prefix, std::string year, std::string in_sample_name, std::string suffix,
                           std::shared_ptr<TFile> output_file, std::map<std::string, std::vector<float>> variables)
    : TemplateTool(channel_prefix, year, suffix, output_file),
      sample_name(in_sample_name),
      acWeightVal(1.) {
      // variables(in_variables) {
  for (auto it = variables.begin(); it != variables.end(); it++) {
    observables[it->first] =  std::make_shared<Float_t>(0.);  // entry for each variable to register later
    for (auto cat : categories) {
      fout->cd(("plots/" + it->first + "/" + cat).c_str());

      // convert data name to be appropriate and do ff things
      if (sample_name.find("Data") != std::string::npos || sample_name == "data_obs") {
        sample_name = "data_obs";
        fakes[cat] = new TH1F(("fake_" + cat).c_str(), "fake_SS", it->second.at(0), it->second.at(1), it->second.at(2));
      }

      // create output histograms
      hists[cat] = new TH1F(sample_name.c_str(), sample_name.c_str(), it->second.at(0), it->second.at(1), it->second.at(2));
    }
    fout->cd();
    all_fakes[it->first] = fakes;
    all_hists[it->first] = hists;
  }
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

void Sample_Plots::set_variable(std::shared_ptr<TTree> tree) {
  int i = 0;
  for (auto it = observables.begin(); it != observables.end(); it++) {
    if (it->first == "t1_decayMode") {
      it->second = std::shared_ptr<float_t>(&t1_decayMode);
    } else if (it->first == "vis_mass") {
      it->second = std::shared_ptr<float_t>(&vis_mass);
    } else if (it->first == "mjj") {
      it->second = std::shared_ptr<float_t>(&mjj);
    } else if (it->first == "mt") {
      it->second = std::shared_ptr<float_t>(&mt);
    } else if (it->first == "mu_iso" || it->first == "el_iso") {
      it->second = std::shared_ptr<float_t>(&lep_iso);
    } else if (it->first == "njets") {
      it->second = std::shared_ptr<float_t>(&njets);
    } else if (it->first == "D0_ggH") {
      it->second = std::shared_ptr<float_t>(&D0_ggH);
    } else if (it->first == "t1_pt") {
      it->second = std::shared_ptr<float_t>(&t1_pt);
    } else {
      tree->SetBranchAddress(it->first.c_str(), &(*(it->second)));
    }
    i++;
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
  set_variable(tree);
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
    for (auto it = observables.begin(); it != observables.end(); it++) {
    // std::cout << observables.size() << std::endl;
    // std::cout << it->first << std::endl;
    // std::cout << (it->second) << std::endl;
      // fill histograms
      if (is_signal) {
        if (cat0) {
          all_hists.at(it->first).at(channel_prefix + "_0jet")->Fill(*(it->second), weight);
        } else if (cat1) {
          all_hists.at(it->first).at(channel_prefix + "_boosted")->Fill(*(it->second), weight);
        } else if (cat2) {
          all_hists.at(it->first).at(channel_prefix + "_vbf")->Fill(*(it->second), weight);
          if (ACcat != "skip") {
            all_hists.at(it->first).at(ACcat)->Fill(*(it->second), weight);
          }
        }
      } else if (is_antiTauIso && sample_name == "data_obs") {
        if (cat0) {
          // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
          convert_data_to_fake(channel_prefix + "_0jet", *(it->second), it->first);  // 2d template
        } else if (cat1) {
          convert_data_to_fake(channel_prefix + "_boosted", *(it->second), it->first);
        } else if (cat2) {
          convert_data_to_fake(channel_prefix + "_vbf", *(it->second), it->first);
          if (ACcat != "skip") {
            convert_data_to_fake(ACcat, *(it->second), it->first);
          }
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
void Sample_Plots::convert_data_to_fake(std::string cat, double var1, std::string j) {
  fout->cd();
  auto bin_x = fake_fractions.at(cat + "_data")->GetXaxis()->FindBin(vis_mass);
  auto bin_y = fake_fractions.at(cat + "_data")->GetYaxis()->FindBin(njets);
  double fakeweight;
  fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                 fake_fractions.at(cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                                 fake_fractions.at(cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                                 fake_fractions.at(cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)});
  all_fakes.at(j).at(cat)->Fill(var1, weight * fakeweight);
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
  auto i = 0;
  for (auto it = observables.begin(); it != observables.end(); it++) {
    for (auto cat : categories) {
      fout->cd(("plots/" + it->first + "/" + cat).c_str());
      all_hists.at(it->first).at(cat)->Write();
      if (sample_name == "data_obs") {
        auto hist = all_fakes.at(it->first).at(cat);
        hist->SetName("jetFakes");
        hist->Write();
      }
    }
    i++;
  }
}

#endif  // INCLUDE_SAMPLE_H_
