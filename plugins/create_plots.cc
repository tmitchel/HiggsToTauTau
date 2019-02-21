// Copyright 2018 Tyler Mitchell
// user includes
#include "TStopwatch.h"
#include "../include/CLParser.h"
#include "../include/plotter_backend.h"

using std::string;
using std::vector;

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

int main(int argc, char *argv[]) {
  auto watch = TStopwatch();
  // get CLI arguments
  CLParser parser(argc, argv);
  bool doSyst = parser.Flag("-s");
  string dir = parser.Option("-d");
  string year = parser.Option("-y");
  string suffix = parser.Option("--suf");
  string tree_name = parser.Option("-t");

  // get input file directory
  if (dir.empty()) {
    std::cerr << "You must give an input directory" << std::endl;
    return -1;
  }

  // get channel info
  string channel_prefix, lep_charge;
  if (tree_name.find("etau_tree") != string::npos) {
    channel_prefix = "et";
  } else if (tree_name.find("mutau_tree") != string::npos) {
    channel_prefix = "mt";
  } else if (tree_name.find("tautau_tree") != string::npos) {
    channel_prefix = "tt";
  } else {
    std::cerr << "Um. I don't know that tree. Sorry...";
    return -1;
  }

  // read all files from input directory
  vector<string> files;
  read_directory(dir, &files);

  std::map<std::string, std::vector<float>> vars = {
    {"m_sv", {30, 50, 180}},
    {"el_pt", {30, 30, 200}},
    {"mu_pt", {30, 30, 200}},
    {"t1_pt", {30, 30, 200}},
    {"j1_pt", {30, 30, 200}},
    {"j2_pt", {30, 30, 200}},
    {"VBF_MELA", {25, 0, 1}},
    {"D0_ggH", {25, 0, 1}},
    {"dPhijj", {25, 0, 3.14}},
    {"NN_disc", {25, 0, 1}}
  };

  // initialize histogram holder
  auto t = tree_reader(vars);
  auto hists = std::make_shared<HistTool>(channel_prefix, year, suffix, t);

  hists->includePlots();
  hists->histoLoop(files, dir, tree_name, "None");    // fill histograms
  hists->getJetFakes(files, dir, tree_name, doSyst);  // get QCD, etc from fake factor
  for (auto weight : hists->acNameMap) {
    hists->histoLoop(files, dir, tree_name, weight.first);  // fill with different weights
  }
  hists->writeHistos();

  std::cout << "Template created.\n Timing Info: \n\t CPU Time: " << watch.CpuTime() << "\n\tReal Time: " << watch.RealTime() << std::endl;
}

void HistTool::histoLoop(vector<string> files, string dir, string tree_name, string acWeight = "None") {
  bool cat0(false), cat1(false), cat2(false);
  for (auto ifile : files) {
    auto fin = std::make_shared<TFile>((dir + "/" + ifile).c_str());
    auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));
    string name = ifile.substr(0, ifile.find(".")).c_str();

    // see if these are AC files or nah
    if (acWeight.find("ggH") != string::npos && name != "ggh_inc") {
      continue;
    } else if ((acWeight.find("wt_a") != string::npos || acWeight.find("wt_L") != string::npos) && name != "vbf_inc") {
      continue;
    } else if (acWeight.find("wh") != string::npos && name != "wh_inc") {
      continue;
    } else if (acWeight.find("zh") != string::npos && name != "zh_inc") {
      continue;
    } else if (acWeight != "None") {
      name = acNameMap[acWeight];
    }

    // using madgraph file now so map name correctly
    if (name == "ggh_madgraph_twojet") {
      name = "GGH2Jets_sm_M125";
    } else if (name == "ggh_madgraph_PS_twojet") {
      name = "GGH2Jets_pseudoscalar_M125";
    } else if (name == "ggh_madgraph_Maxmix_twojet") {
      name = "GGH2Jets_pseudoscalar_Mf05ph0125";
    }

    // do some initialization
    fout->cd();
    initVectors1d(name);
    fout->cd();

    // set all the branch addresses before the event loop
    t.setBranches(tree, acWeight);

    // event loop
    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);
      Float_t weight = t.evtwt;
      if (acWeight != "None") {
        weight *= t.acWeightVal;
      }

      // only look at opposite-sign events
      if (t.OS == 0) {
        continue;
      }

      // b-jet veto
      if (t.nbjets > 0) {
        continue;
      }

      cat0 = (t.cat_0jet > 0);
      cat1 = (t.njets == 1 || (t.njets > 1 && (t.mjj < 300)));
      cat2 = (t.njets > 1 && t.mjj > 300);

      // find the correct MELA ggH/NN bin for this event
      auto ACcat = getCategory(t.D0_ggH, t.NN_disc);

      // fill histograms
      if (t.is_signal) {
        for (auto var = t.variables.begin(); var != t.variables.end(); var++) {
          auto observable = t.getVar(var->first);
          if (cat0) {
            hists_1d.at(var->first+"/"+categories.at(zeroJet)).back()->Fill(observable, weight);
          } else if (cat1) {
            hists_1d.at(var->first+"/"+categories.at(boosted)).back()->Fill(observable, weight);
          } else if (cat2) {
            hists_1d.at(var->first+"/"+categories.at(vbf)).back()->Fill(observable, weight);
            hists_1d.at(var->first+"/"+categories.at(ACcat)).back()->Fill(observable, weight);
          }
        }
      } else if (t.is_antiTauIso) {
        if (!(name == "W" || name == "ZJ" || name == "VVJ" ||
              name == "TTJ" ||
              name == "embedded" || name == "TTT" || name == "VVT" ||
              name == "Data")) {
          continue;
        }

        if (cat0) {
          fillFraction(zeroJet, name, t.vis_mass, t.njets, weight);
        } else if (cat1) {
          fillFraction(boosted, name, t.vis_mass, t.njets, weight);
        } else if (cat2) {
          fillFraction(vbf, name, t.vis_mass, t.njets, weight);
          fillFraction(ACcat, name, t.vis_mass, t.njets, weight);
        }
      }
    }
  }

  // calculate fake-fractions
  for (int i = 0; i < data.size(); i++) {
    if (acWeight != "None") {
      break;
    }
    frac_qcd.at(i) = std::shared_ptr<TH2F>(reinterpret_cast<TH2F*>(data.at(i)->Clone()));
    frac_qcd.at(i)->Add(frac_w.at(i).get(), -1);
    frac_qcd.at(i)->Add(frac_tt.at(i).get(), -1);
    frac_qcd.at(i)->Add(frac_real.at(i).get(), -1);

    frac_w.at(i)->Divide(data.at(i).get());
    frac_tt.at(i)->Divide(data.at(i).get());
    frac_real.at(i)->Divide(data.at(i).get());
    frac_qcd.at(i)->Divide(data.at(i).get());
  }
}

void HistTool::getJetFakes(vector<string> files, string dir, string tree_name, bool doSyst = false) {
  bool cat0(false), cat1(false), cat2(false);
  for (auto ifile : files) {
    if (ifile.find("Data.root") == std::string::npos) {
      continue;
    }
    auto fin = std::make_shared<TFile>((dir + "/" + ifile).c_str());
    auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));
    string name = ifile.substr(0, ifile.find(".")).c_str();

    if (name != "Data") {
      continue;
    }

    // create systematics histograms
    if (doSyst) {
      initSystematics(name);
      fout->cd();
    }

    // set all the branch addresses before the event loop
    t.setBranches(tree, "None");

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);
      auto weight = t.evtwt;

      float iso;
      if (tree_name.find("etau_tree") != string::npos) {
        iso = t.el_iso;
      } else if (tree_name.find("mutau_tree") != string::npos) {
        iso = t.mu_iso;
      }

      // only look at opposite-sign events
      if (t.OS == 0) {
        continue;
      }

      if (t.nbjets > 0) {
        continue;
      }

      cat0 = (t.cat_0jet > 0);
      cat1 = (t.njets == 1 || (t.njets > 1 && t.mjj < 300));
      cat2 = (t.njets > 1 && t.mjj > 300);

      auto ACcat = getCategory(t.D0_ggH, t.NN_disc);

      if (t.is_antiTauIso) {
        int nvar = 1;
        for (auto var = t.variables.begin(); var != t.variables.end(); var++) {
          auto observable = t.getVar(var->first);
          if (cat0) {
            // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
            convertDataToFake(zeroJet, nvar, name, observable, t.vis_mass, t.njets, t.t1_pt, t.t1_decayMode, t.mt, iso, weight);
          } else if (cat1) {
            convertDataToFake(boosted, nvar, name, observable, t.vis_mass, t.njets, t.t1_pt, t.t1_decayMode, t.mt, iso, weight);
          } else if (cat2) {
            convertDataToFake(vbf, nvar, name, observable, t.vis_mass, t.njets, t.t1_pt, t.t1_decayMode, t.mt, iso, weight);
            convertDataToFake(ACcat, nvar, name, observable, t.vis_mass, t.njets, t.t1_pt, t.t1_decayMode, t.mt, iso, weight);
          }

          // loop through all systematic names and get the corresponding weight to fill a histogram
          if (doSyst) {
            for (int i = 0; i < systematics.size(); i++) {
              if (cat0) {
                auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(t.vis_mass);
                auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(t.njets);
                auto fakeweight = ff_weight->value({t.t1_pt, t.t1_decayMode, t.njets, t.vis_mass, t.mt, iso,
                                                    frac_w.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                                    frac_tt.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                                    frac_qcd.at(zeroJet)->GetBinContent(bin_x, bin_y)},
                                                   systematics.at(i));
                FF_systs.at("et_0jet").at(i)->Fill(t.t1_decayMode, t.vis_mass, weight * fakeweight);
              } else if (cat1) {
                auto bin_x = data.at(boosted)->GetXaxis()->FindBin(t.vis_mass);
                auto bin_y = data.at(boosted)->GetYaxis()->FindBin(t.njets);
                auto fakeweight = ff_weight->value({t.t1_pt, t.t1_decayMode, t.njets, t.vis_mass, t.mt, iso,
                                                    frac_w.at(boosted)->GetBinContent(bin_x, bin_y),
                                                    frac_tt.at(boosted)->GetBinContent(bin_x, bin_y),
                                                    frac_qcd.at(boosted)->GetBinContent(bin_x, bin_y)},
                                                   systematics.at(i));
                FF_systs.at("et_boosted").at(i)->Fill(t.higgs_pT, t.m_sv, weight * fakeweight);
              } else if (cat2) {
                auto bin_x = data.at(vbf)->GetXaxis()->FindBin(t.vis_mass);
                auto bin_y = data.at(vbf)->GetYaxis()->FindBin(t.njets);
                auto fakeweight = ff_weight->value({t.t1_pt, t.t1_decayMode, t.njets, t.vis_mass, t.mt, iso,
                                                    frac_w.at(vbf)->GetBinContent(bin_x, bin_y),
                                                    frac_tt.at(vbf)->GetBinContent(bin_x, bin_y),
                                                    frac_qcd.at(vbf)->GetBinContent(bin_x, bin_y)},
                                                   systematics.at(i));
                FF_systs.at("et_vbf").at(i)->Fill(t.mjj, t.m_sv, weight * fakeweight);
              }
            }
          }
          nvar++;
        }
      }
    }
  }
}
