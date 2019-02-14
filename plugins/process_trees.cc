// Copyright 2018 Tyler Mitchell
// user includes
#include "TStopwatch.h"
#include "../include/CLParser.h"
#include "../include/plotter_backend.h"

using std::string;
using std::vector;

int main(int argc, char *argv[]) {
  auto watch = TStopwatch();
  // get CLI arguments
  CLParser parser(argc, argv);
  bool old = parser.Flag("-O");
  bool doAC = parser.Flag("-a");
  bool doNN = parser.Flag("-n");
  bool doSyst = parser.Flag("-s");
  string var = parser.Option("-v");
  string dir = parser.Option("-d");
  string year = parser.Option("-y");
  string suffix = parser.Option("--suf");
  string tree_name = parser.Option("-t");
  vector<string> sbins = parser.MultiOption("-b", 3);

  // get the provided histogram binning
  std::vector<int> bins;
  for (auto sbin : sbins) {
    bins.push_back(std::stoi(sbin));
  }

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
  auto hists = new HistTool(channel_prefix, year, suffix, doNN, old);

  // make a 1d plot
  if (bins.size() > 2 && !var.empty()) {
    hists->includePlots(bins, var);
  }
  hists->histoLoop(files, dir, tree_name, "None");    // fill histograms
  hists->getJetFakes(files, dir, tree_name, doSyst);  // get QCD, etc from fake factor
  if (doAC) {
    for (auto weight : hists->acNameMap) {
      hists->histoLoop(files, dir, tree_name, weight.first);  // fill with different weights
    }
  }
  hists->writeTemplates();  // write histograms to file
  hists->writeHistos();
  hists->fout->Close();

  std::cout << "Template created.\n Timing Info: \n\t CPU Time: " << watch.CpuTime() << "\n\tReal Time: " << watch.RealTime() << std::endl;

  delete hists->ff_weight;
}

void HistTool::histoLoop(vector<string> files, string dir, string tree_name, string acWeight = "None") {
  float observable(0.);
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

    // using madgraph file now so map name correctly
    if (name == "ggh_madgraph_twojet") {
      name = "GGH2Jets_sm_M125";
    } else if (name == "ggh_madgraph_PS_twojet") {
      name = "GGH2Jets_pseudoscalar_M125";
    } else if (name == "ggh_madgraph_Maxmix_twojet") {
      name = "GGH2Jets_pseudoscalar_Mf05ph0125";
    }

    // do some initialization
    initVectors2d(name);
    fout->cd();
    if (bins_1d.size() > 2) {
      initVectors1d(name);
    }
    fout->cd();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiTauIso, OS;
    Float_t higgs_pT, t1_decayMode, vis_mass, mjj, m_sv, njets, nbjets, weight, NN_disc, acWeightVal(1.);
    Float_t D0_VBF, D0_ggH, DCP_VBF, DCP_ggH, var_val, t1_pt, VBF_MELA, dPhijj;

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
    tree->SetBranchAddress("dPhijj", &dPhijj);

    // if (doNN) {
      tree->SetBranchAddress("NN_disc", &NN_disc);
    // }

    if (acWeight != "None") {
      tree->SetBranchAddress(acWeight.c_str(), &acWeightVal);
    }

    if (!(var == "" || var == "higgs_pT" || var == "t1_decayMode" || var == "vis_mass" || var == "mjj" || var == "m_sv" || var == "dPhijj" ||
          var == "njets" || var == "nbjets" || var == "D0_VBF" || var == "D0_ggH" || var == "DCP_VBF" || var == "DCP_ggH" || var == "VBF_MELA") &&
          !(doNN && var == "NN_disc")) {
      tree->SetBranchAddress(var.c_str(), &var_val);
    }

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);
      weight *= acWeightVal;  // acWeightVal = 1 for SM

      // only look at opposite-sign events
      if (OS == 0) {
        continue;
      }

      // choose VBF category variable
      if (doNN) {
        observable = NN_disc;
      } else {
        observable = mjj;
      }
      observable = VBF_MELA;

      // pick either old 2016 selection or optimized one
      if (old_selection) {
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300 || higgs_pT < 50 || t1_pt < 40)));
        cat2 = (njets > 1 && mjj > 300 && higgs_pT > 50 && t1_pt > 40);
      } else {
        if (nbjets > 0) {
          continue;
        }
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300)));
        cat2 = (njets > 1 && mjj > 300);
      }

      if (var == "higgs_pT") {
        var_val = higgs_pT;
      } else if (var == "t1_decayMode") {
        var_val = t1_decayMode;
      } else if (var == "vis_mass") {
        var_val = vis_mass;
      } else if (var == "mjj") {
        var_val = mjj;
      } else if (var == "m_sv") {
        var_val = m_sv;
      } else if (var == "njets") {
        var_val = njets;
      } else if (var == "nbjets") {
        var_val = nbjets;
      } else if (var == "D0_VBF") {
        var_val = D0_VBF;
      } else if (var == "D0_ggH") {
        var_val = D0_ggH;
      } else if (var == "DCP_VBF") {
        var_val = DCP_VBF;
      } else if (var == "DCP_ggH") {
        var_val = DCP_ggH;
      } else if (var == "NN_disc" && doNN) {
        var_val = NN_disc;
      } else if (var == "VBF_MELA") {
        var_val = VBF_MELA;
      } else if (var == "dPhijj") {
        var_val = dPhijj;
      }

      // find the correct MELA ggH/Higgs pT bin for this event
      auto ACcat = getCategory(D0_ggH, NN_disc);

      // fill histograms
      if (is_signal) {
        vector<Categories> passing;
        if (cat0) {
          hists_2d.at(categories.at(zeroJet)).back()->Fill(t1_decayMode, vis_mass, weight);
          passing.push_back(zeroJet);
        }
        if (cat1) {
          hists_2d.at(categories.at(boosted)).back()->Fill(higgs_pT, m_sv, weight);
          passing.push_back(boosted);
        }
        if (cat2) {
          hists_2d.at(categories.at(vbf)).back()->Fill(observable, m_sv, weight);
          passing.push_back(vbf);
          // ggH bins: [0.0, 0.3, 0.7, 1.0]
          hists_2d.at(categories.at(ACcat)).back()->Fill(observable, m_sv, weight);
          if (bins_1d.size() > 2) {
            hists_1d.at(categories.at(ACcat)).back()->Fill(var_val, weight);
          }
        }

        // fill plots
        if (bins_1d.size() > 2) {
          for (auto passer : passing) {
            hists_1d.at(categories.at(passer)).back()->Fill(var_val, weight);
          }
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
  float observable(0.);
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
    Float_t D0_VBF, D0_ggH, DCP_VBF, DCP_ggH, var_val, VBF_MELA, dPhijj;

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
    tree->SetBranchAddress("dPhijj", &dPhijj);

    // if (doNN) {
      tree->SetBranchAddress("NN_disc", &NN_disc);
    // }

    if (!(var == "" || var == "higgs_pT" || var == "t1_decayMode" || var == "vis_mass" || var == "mjj" || var == "m_sv" ||
          var == "njets" || var == "nbjets" || var == "D0_VBF" || var == "D0_ggH" || var == "DCP_VBF" || var == "DCP_ggH" ||
          var == "t1_pt" || var == "mt" || var == iso.c_str() || var == "VBF_MELA" || var == "dPhijj") && !(doNN && var == "NN_disc")) {
      tree->SetBranchAddress(var.c_str(), &var_val);
    }

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

      // only look at opposite-sign events
      if (OS == 0) {
        continue;
      }

      // choose VBF category variable
      if (doNN) {
        observable = NN_disc;
      } else {
        observable = mjj;
      }
      observable = VBF_MELA;

      if (old_selection) {
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300 || higgs_pT < 50 || t1_pt < 40)));
        cat2 = (njets > 1 && mjj > 300 && higgs_pT > 50 && t1_pt > 40);
      } else {
        if (nbjets > 0) {
          continue;
        }
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && mjj < 300));
        cat2 = (njets > 1 && mjj > 300);
      }

      if (var == "higgs_pT") {
        var_val = higgs_pT;
      } else if (var == "t1_decayMode") {
        var_val = t1_decayMode;
      } else if (var == "vis_mass") {
        var_val = vis_mass;
      } else if (var == "mjj") {
        var_val = mjj;
      } else if (var == "m_sv") {
        var_val = m_sv;
      } else if (var == "njets") {
        var_val = njets;
      } else if (var == "nbjets") {
        var_val = nbjets;
      } else if (var == "D0_VBF") {
        var_val = D0_VBF;
      } else if (var == "D0_ggH") {
        var_val = D0_ggH;
      } else if (var == "DCP_VBF") {
        var_val = DCP_VBF;
      } else if (var == "DCP_ggH") {
        var_val = DCP_ggH;
      } else if (var == "t1_pt") {
        var_val = t1_pt;
      } else if (var == "mt") {
        var_val = mt;
      } else if (var == iso.c_str()) {
        var_val = lep_iso;
      } else if (var == "NN_disc" && doNN) {
        var_val = NN_disc;
      } else if (var == "VBF_MELA") {
        var_val = VBF_MELA;
      } else if (var == "dPhijj") {
        var_val = dPhijj;
      }

      auto ACcat = getCategory(D0_ggH, NN_disc);

      if (is_antiTauIso) {
        if (cat0) {
          // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
          convertDataToFake(zeroJet, name, var_val, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);  // 1d plot
          convertDataToFake(zeroJet, name, t1_decayMode, vis_mass, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);  // 2d template
        } else if (cat1) {
          convertDataToFake(boosted, name, var_val, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
          convertDataToFake(boosted, name, higgs_pT, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
        } else if (cat2) {
          convertDataToFake(vbf, name, var_val, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
          convertDataToFake(vbf, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);

          convertDataToFake(ACcat, name, var_val, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
          convertDataToFake(ACcat, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
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
              FF_systs.at("et_vbf").at(i)->Fill(mjj, m_sv, weight * fakeweight);
            }
          }
        }
      }
    }
  }
}
