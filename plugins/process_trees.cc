// Copyright 2018 Tyler Mitchell
// user includes
#include "TStopwatch.h"
#include "TMath.h"
#include "../include/CLParser.h"
#include "../include/process_trees.h"

using std::string;
using std::vector;

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

  // initialize histogram holder
  auto hists = new HistTool(channel_prefix, year, suffix);

  hists->histoLoop(files, dir, tree_name, "None");    // fill histograms
  hists->getJetFakes(files, dir, tree_name, doSyst);  // get QCD, etc from fake factor
  for (auto weight : hists->acNameMap) {
    hists->histoLoop(files, dir, tree_name, weight.first);  // fill with different weights
  }
  hists->writeTemplates();  // write histograms to file
  hists->fout->Close();

  std::cout << "Template created.\n Timing Info: \n\t CPU Time: " << watch.CpuTime() << "\n\tReal Time: " << watch.RealTime() << std::endl;

  delete hists->ff_weight;
}

void HistTool::histoLoop(vector<string> files, string dir, string tree_name, string acWeight = "None") {
  float vbf_var1(0.), vbf_var2(0.), vbf_var3(0.);
  bool cat0(false), cat1(false), cat2(false);
  for (auto ifile : files) {
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
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

    // do some initialization
    initVectors2d(name);
    fout->cd();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiTauIso, OS;
    Float_t higgs_pT, t1_decayMode, vis_mass, mjj, m_sv, njets, nbjets, weight, NN_disc, acWeightVal(1.);
    Float_t D0_VBF, D0_ggH, DCP_VBF, DCP_ggH, var_val, t1_pt, VBF_MELA, j1_phi, j2_phi;

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("D0_VBF", &D0_VBF);
    tree->SetBranchAddress("D0_ggH", &D0_ggH);
    tree->SetBranchAddress("DCP_VBF", &DCP_VBF);
    tree->SetBranchAddress("DCP_ggH", &DCP_ggH);
    tree->SetBranchAddress("is_signal", &is_signal);
    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);
    tree->SetBranchAddress("OS", &OS);
    tree->SetBranchAddress("t1_pt", &t1_pt);
    tree->SetBranchAddress("VBF_MELA", &VBF_MELA);
    tree->SetBranchAddress("j1_phi", &j1_phi);
    tree->SetBranchAddress("j2_phi", &j2_phi);
    tree->SetBranchAddress("NN_disc", &NN_disc);

    if (acWeight != "None") {
      tree->SetBranchAddress(acWeight.c_str(), &acWeightVal);
    }

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
      cat0 = (cat_0jet > 0);
      cat1 = (njets == 1 || (njets > 1 && (mjj < 300)));
      cat2 = (njets > 1 && mjj > 300);

      // find the correct MELA ggH/Higgs pT bin for this event
      auto ACcat = getCategory(vbf_var3);

      // fill histograms
      if (is_signal) {
        if (cat0) {
          hists_2d.at(categories.at(zeroJet)).back()->Fill(t1_decayMode, vis_mass, weight);
        } else if (cat1) {
          hists_2d.at(categories.at(boosted)).back()->Fill(higgs_pT, m_sv, weight);
        } else if (cat2) {
          hists_2d.at(categories.at(vbf)).back()->Fill(vbf_var1, vbf_var2, weight);
          hists_2d.at(categories.at(ACcat)).back()->Fill(vbf_var1, vbf_var2, weight);
        }
      } else if (is_antiTauIso) {
        if (!(name == "W" || name == "ZJ" || name == "VVJ" ||
              name == "TTJ" ||
              name == "embedded" || name == "TTT" || name == "VVT" ||
              name == "Data")) {
          continue;
        }

        if (cat0) {
          fillFraction(zeroJet, name, vis_mass, njets, weight);
        } else if (cat1) {
          fillFraction(boosted, name, vis_mass, njets, weight);
        } else if (cat2) {
          fillFraction(vbf, name, vis_mass, njets, weight);
          fillFraction(ACcat, name, vis_mass, njets, weight);
        }
      }
    }
  }

  // calculate fake-fractions
  for (int i = 0; i < data.size(); i++) {
    if (acWeight != "None") {
      break;
    }
    frac_qcd.at(i) = reinterpret_cast<TH2F *>(data.at(i)->Clone());
    frac_qcd.at(i)->Add(frac_w.at(i), -1);
    frac_qcd.at(i)->Add(frac_tt.at(i), -1);
    frac_qcd.at(i)->Add(frac_real.at(i), -1);

    frac_w.at(i)->Divide(data.at(i));
    frac_tt.at(i)->Divide(data.at(i));
    frac_real.at(i)->Divide(data.at(i));
    frac_qcd.at(i)->Divide(data.at(i));
  }
}

void HistTool::getJetFakes(vector<string> files, string dir, string tree_name, bool doSyst = false) {
  float vbf_var1(0.), vbf_var2(0.), vbf_var3(0.);
  bool cat0(false), cat1(false), cat2(false);
  for (auto ifile : files) {
    if (ifile.find("Data.root") == std::string::npos) {
      continue;
    }
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
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

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_antiTauIso, OS;
    Float_t higgs_pT, mjj, m_sv, weight, t1_pt, t1_decayMode, njets, nbjets, vis_mass, mt, lep_iso, NN_disc;
    Float_t D0_VBF, D0_ggH, DCP_VBF, DCP_ggH, var_val, VBF_MELA, j1_phi, j2_phi;

    string iso;
    if (tree_name.find("etau_tree") != string::npos) {
      iso = "el_iso";
    } else if (tree_name.find("mutau_tree") != string::npos) {
      iso = "mu_iso";
    }

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("t1_pt", &t1_pt);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress(iso.c_str(), &lep_iso);
    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);
    tree->SetBranchAddress("D0_VBF", &D0_VBF);
    tree->SetBranchAddress("D0_ggH", &D0_ggH);
    tree->SetBranchAddress("DCP_VBF", &DCP_VBF);
    tree->SetBranchAddress("DCP_ggH", &DCP_ggH);
    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);
    tree->SetBranchAddress("OS", &OS);
    tree->SetBranchAddress("VBF_MELA", &VBF_MELA);
    tree->SetBranchAddress("j1_phi", &j1_phi);
    tree->SetBranchAddress("j2_phi", &j2_phi);
    tree->SetBranchAddress("NN_disc", &NN_disc);

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

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
      cat0 = (cat_0jet > 0);
      cat1 = (njets == 1 || (njets > 1 && mjj < 300));
      cat2 = (njets > 1 && mjj > 300);

      auto ACcat = getCategory(vbf_var3);

      if (is_antiTauIso) {
        if (cat0) {
          // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
          convertDataToFake(zeroJet, name, t1_decayMode, vis_mass, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);  // 2d template
        } else if (cat1) {
          convertDataToFake(boosted, name, higgs_pT, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
        } else if (cat2) {
          convertDataToFake(vbf, name, vbf_var1, vbf_var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
          convertDataToFake(ACcat, name, vbf_var1, vbf_var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
        }

        // loop through all systematic names and get the corresponding weight to fill a histogram
        if (doSyst) {
          for (int i = 0; i < systematics.size(); i++) {
            if (cat0) {
              auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(vis_mass);
              auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(njets);
              auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                                  frac_w.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                                  frac_tt.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                                  frac_qcd.at(zeroJet)->GetBinContent(bin_x, bin_y)},
                                                 systematics.at(i));
              FF_systs.at("et_0jet").at(i)->Fill(t1_decayMode, vis_mass, weight * fakeweight);
            } else if (cat1) {
              auto bin_x = data.at(boosted)->GetXaxis()->FindBin(vis_mass);
              auto bin_y = data.at(boosted)->GetYaxis()->FindBin(njets);
              auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                                  frac_w.at(boosted)->GetBinContent(bin_x, bin_y),
                                                  frac_tt.at(boosted)->GetBinContent(bin_x, bin_y),
                                                  frac_qcd.at(boosted)->GetBinContent(bin_x, bin_y)},
                                                 systematics.at(i));
              FF_systs.at("et_boosted").at(i)->Fill(higgs_pT, m_sv, weight * fakeweight);
            } else if (cat2) {
              auto bin_x = data.at(vbf)->GetXaxis()->FindBin(vis_mass);
              auto bin_y = data.at(vbf)->GetYaxis()->FindBin(njets);
              auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                                  frac_w.at(vbf)->GetBinContent(bin_x, bin_y),
                                                  frac_tt.at(vbf)->GetBinContent(bin_x, bin_y),
                                                  frac_qcd.at(vbf)->GetBinContent(bin_x, bin_y)},
                                                 systematics.at(i));
              FF_systs.at("et_vbf").at(i)->Fill(mjj, vbf_var2, weight * fakeweight);
            }
          }
        }
      }
    }
  }
}
