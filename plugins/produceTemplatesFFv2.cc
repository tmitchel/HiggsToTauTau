// Copyright 2018 Tyler Mitchell
// user includes
#include "CLParser.h"
#include "produceTemplatesFFv2.h"

using std::string;

int main(int argc, char *argv[]) {
  // get CLI arguments
  CLParser parser(argc, argv);
  bool old = parser.Flag("-O");
  bool doAC = parser.Flag("-a");
  bool doNN = parser.Flag("-n");
  bool doSyst = parser.Flag("-s");
  string dir = parser.Option("-d");
  string year = parser.Option("-y");
  string tree_name = parser.Option("-t");
  string suffix = parser.Option("--suf");

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

  // initialize histogram holder
  auto hists = new histHolder(channel_prefix, year, suffix, doNN, old);

  // read all files from input directory
  std::vector<string> files;
  read_directory(dir, &files);

  hists->histoLoop(files, dir, tree_name, "None");
  hists->getJetFakes(files, dir, tree_name, doSyst);
  if (doAC) {
    for (auto weight : hists->acNameMap) {
      hists->histoLoop(files, dir, tree_name, weight.first);
    }
  }
  hists->writeHistos();

  delete hists->ff_weight;
}

void histHolder::histoLoop(std::vector<string> files, string dir, string tree_name, string acWeight = "None") {
  float observable(0.);
  bool cat0(false), cat1(false), cat2(false);
  for (auto ifile : files) {
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));
    string name = ifile.substr(0, ifile.find(".")).c_str();


    if (acWeight.find("ggH") != string::npos && name != "ggH_inc") {
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

    initVectors(name);
    fout->cd();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiTauIso, OS;
    Float_t higgs_pT, t1_decayMode, vis_mass, mjj, m_sv, njets, nbjets, weight, NN_disc, acWeightVal(1.);
    Float_t D0_VBF, D0_ggH, DCP_VBF, DCP_ggH;

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

    if (doNN) {
      tree->SetBranchAddress("NN_disc", &NN_disc);
    }

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

      // choose VBF category variable
      if (doNN) {
        observable = NN_disc;
      } else {
        observable = mjj;
      }

      if (old_selection) {
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300 || higgs_pT < 50)));
        cat2 = (njets > 1 && mjj > 300 && higgs_pT > 50);
      } else {
        if (nbjets > 0) {
          continue;
        }
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && mjj < 400));
        cat2 = (njets > 1 && mjj > 400);
      }

      if (is_signal) {
        if (cat0) {
          hists.at(categories.at(zeroJet)).back()->Fill(t1_decayMode, vis_mass, weight);
        }
        if (cat1) {
          hists.at(categories.at(boosted)).back()->Fill(higgs_pT, m_sv, weight);
        }
        if (cat2) {
          hists.at(categories.at(vbf)).back()->Fill(observable, m_sv, weight);

          // Split VBF bins based on MELA VBF variables.
          if (D0_VBF > 0 && D0_VBF <= 0.2) {
            hists.at(categories.at(vbf_D0_0p0to0p2)).back()->Fill(observable, m_sv, weight);
            if (DCP_VBF > 0) {
              hists.at(categories.at(vbf_D0_0p0to0p2_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_VBF < 0) {
              hists.at(categories.at(vbf_D0_0p0to0p2_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          } else if (D0_VBF <= 0.4) {
            hists.at(categories.at(vbf_D0_0p2to0p4)).back()->Fill(observable, m_sv, weight);
            if (DCP_VBF > 0) {
              hists.at(categories.at(vbf_D0_0p2to0p4_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_VBF < 0) {
              hists.at(categories.at(vbf_D0_0p2to0p4_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          } else if (D0_VBF <= 0.8) {
            hists.at(categories.at(vbf_D0_0p4to0p8)).back()->Fill(observable, m_sv, weight);
            if (DCP_VBF > 0) {
              hists.at(categories.at(vbf_D0_0p4to0p8_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_VBF < 0) {
              hists.at(categories.at(vbf_D0_0p4to0p8_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          } else if (D0_VBF <= 1.0) {
            hists.at(categories.at(vbf_D0_0p8to1p0)).back()->Fill(observable, m_sv, weight);
            if (DCP_VBF > 0) {
              hists.at(categories.at(vbf_D0_0p8to1p0_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_VBF < 0) {
              hists.at(categories.at(vbf_D0_0p8to1p0_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          }

          // Split VBF bins based on MELA ggH variables.
          if (D0_ggH > 0 && D0_ggH <= 0.3) {
            hists.at(categories.at(vbf_D0ggH_0p00to0p30)).back()->Fill(observable, m_sv, weight);
            if (DCP_ggH > 0) {
              hists.at(categories.at(vbf_D0ggH_0p00to0p30_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_ggH < 0) {
              hists.at(categories.at(vbf_D0ggH_0p00to0p30_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          } else if (D0_ggH <= 0.45) {
            hists.at(categories.at(vbf_D0ggH_0p30to0p45)).back()->Fill(observable, m_sv, weight);
            if (DCP_ggH > 0) {
              hists.at(categories.at(vbf_D0ggH_0p30to0p45_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_ggH < 0) {
              hists.at(categories.at(vbf_D0ggH_0p30to0p45_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          } else if (D0_ggH <= 0.55) {
            hists.at(categories.at(vbf_D0ggH_0p45to0p55)).back()->Fill(observable, m_sv, weight);
            if (DCP_ggH > 0) {
              hists.at(categories.at(vbf_D0ggH_0p45to0p55_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_ggH < 0) {
              hists.at(categories.at(vbf_D0ggH_0p45to0p55_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          } else if (D0_ggH <= 1.0) {
            hists.at(categories.at(vbf_D0ggH_0p55to1p00)).back()->Fill(observable, m_sv, weight);
            if (DCP_ggH > 0) {
              hists.at(categories.at(vbf_D0ggH_0p55to1p00_DCPp)).back()->Fill(observable, m_sv, weight);
            } else if (DCP_ggH < 0) {
              hists.at(categories.at(vbf_D0ggH_0p55to1p00_DCPm)).back()->Fill(observable, m_sv, weight);
            }
          }
        }
      } else if (is_antiTauIso) {
        if (!(name == "W" || name == "ZJ" || name == "VVJ" ||
              name == "TTJ" ||
              name == "ZTT" || name == "TTT" || name == "VVT" ||
              name == "Data")) {
          continue;
        }

        if (cat0) {
          fillFraction(zeroJet, name, vis_mass, njets, weight);
        } else if (cat1) {
          fillFraction(boosted, name, vis_mass, njets, weight);
        } else if (cat2) {
          fillFraction(vbf, name, vis_mass, njets, weight);

          // Split VBF bins based on MELA VBF variables.
          if (D0_VBF > 0 && D0_VBF <= 0.2) {
            fillFraction(vbf_D0_0p0to0p2, name, vis_mass, njets, weight);
            if (DCP_VBF > 0) {
              fillFraction(vbf_D0_0p0to0p2_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_VBF < 0) {
              fillFraction(vbf_D0_0p0to0p2_DCPm, name, vis_mass, njets, weight);
            }
          } else if (D0_VBF <= 0.4) {
            fillFraction(vbf_D0_0p2to0p4, name, vis_mass, njets, weight);
            if (DCP_VBF > 0) {
              fillFraction(vbf_D0_0p2to0p4_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_VBF < 0) {
              fillFraction(vbf_D0_0p2to0p4_DCPm, name, vis_mass, njets, weight);
            }
          } else if (D0_VBF <= 0.8) {
            fillFraction(vbf_D0_0p4to0p8, name, vis_mass, njets, weight);
            if (DCP_VBF > 0) {
              fillFraction(vbf_D0_0p4to0p8_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_VBF < 0) {
              fillFraction(vbf_D0_0p4to0p8_DCPm, name, vis_mass, njets, weight);
            }
          } else if (D0_VBF <= 1.0) {
            fillFraction(vbf_D0_0p8to1p0, name, vis_mass, njets, weight);
            if (DCP_VBF > 0) {
              fillFraction(vbf_D0_0p8to1p0_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_VBF < 0) {
              fillFraction(vbf_D0_0p8to1p0_DCPm, name, vis_mass, njets, weight);
            }
          }

          // Split VBF bins based on MELA ggH variables.
          if (D0_ggH > 0 && D0_ggH <= 0.3) {
            fillFraction(vbf_D0ggH_0p00to0p30, name, vis_mass, njets, weight);
            if (DCP_ggH > 0) {
              fillFraction(vbf_D0ggH_0p00to0p30_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_ggH < 0) {
              fillFraction(vbf_D0ggH_0p00to0p30_DCPm, name, vis_mass, njets, weight);
            }
          } else if (D0_ggH <= 0.45) {
            fillFraction(vbf_D0ggH_0p30to0p45, name, vis_mass, njets, weight);
            if (DCP_ggH > 0) {
              fillFraction(vbf_D0ggH_0p30to0p45_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_ggH < 0) {
              fillFraction(vbf_D0ggH_0p30to0p45_DCPm, name, vis_mass, njets, weight);
            }
          } else if (D0_ggH <= 0.55) {
            fillFraction(vbf_D0ggH_0p45to0p55, name, vis_mass, njets, weight);
            if (DCP_ggH > 0) {
              fillFraction(vbf_D0ggH_0p45to0p55_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_ggH < 0) {
              fillFraction(vbf_D0ggH_0p45to0p55_DCPm, name, vis_mass, njets, weight);
            }
          } else if (D0_ggH <= 1.0) {
            fillFraction(vbf_D0ggH_0p55to1p00, name, vis_mass, njets, weight);
            if (DCP_ggH > 0) {
              fillFraction(vbf_D0ggH_0p55to1p00_DCPp, name, vis_mass, njets, weight);
            } else if (DCP_ggH < 0) {
              fillFraction(vbf_D0ggH_0p55to1p00_DCPm, name, vis_mass, njets, weight);
            }
          }
        }
      }
    }
  }

  for (int i = 0; i < data.size(); i++) {
    if (acWeight != "None") {
      break;
    }
    frac_qcd.at(i) = reinterpret_cast<TH2F *>(data.at(i)->Clone());
    frac_qcd.at(i)->Add(frac_w.at(i), -1);
    frac_qcd.at(i)->Add(frac_tt.at(i), -1);
    frac_qcd.at(i)->Add(frac_real.at(i), -1);

     // std::cout << frac_w.at(i)->GetName() << " " << frac_w.at(i)->Integral()/data.at(i)->Integral() << std::endl;
     // std::cout << frac_tt.at(i)->GetName() << " " << frac_tt.at(i)->Integral()/data.at(i)->Integral() << std::endl;
     // std::cout << frac_qcd.at(i)->GetName() << " " << frac_qcd.at(i)->Integral()/data.at(i)->Integral() << std::endl;
     // std::cout << frac_real.at(i)->GetName() << " " << frac_real.at(i)->Integral()/data.at(i)->Integral() << std::endl;

    frac_w.at(i)->Divide(data.at(i));
    frac_tt.at(i)->Divide(data.at(i));
    frac_real.at(i)->Divide(data.at(i));
    frac_qcd.at(i)->Divide(data.at(i));
  }
}

void histHolder::getJetFakes(std::vector<string> files, string dir, string tree_name, bool doSyst = false) {
  float observable(0.);
  bool cat0(false), cat1(false), cat2(false);
  for (auto ifile : files) {
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
    Float_t D0_VBF, D0_ggH, DCP_VBF, DCP_ggH;

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

    if (doNN) {
      tree->SetBranchAddress("NN_disc", &NN_disc);
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

      if (old_selection) {
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300 || higgs_pT < 50)));
        cat2 = (njets > 1 && mjj > 300 && higgs_pT > 50);
      } else {
        if (nbjets > 0) {
          continue;
        }
        cat0 = (cat_0jet > 0);
        cat1 = (njets == 1 || (njets > 1 && mjj < 400));
        cat2 = (njets > 1 && mjj > 400);
      }

      if (is_antiTauIso) {
        if (cat0) {
          // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
          convertDataToFake(zeroJet, name, t1_decayMode, vis_mass, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
        } else if (cat1) {
          convertDataToFake(boosted, name, higgs_pT, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
        } else if (cat2) {
          convertDataToFake(vbf, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);

          // Split VBF bins based on MELA VBF variables.
          if (D0_VBF > 0 && D0_VBF <= 0.2) {
            convertDataToFake(vbf_D0_0p0to0p2, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_VBF > 0) {
              convertDataToFake(vbf_D0_0p0to0p2_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_VBF < 0) {
              convertDataToFake(vbf_D0_0p0to0p2_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          } else if (D0_VBF <= 0.4) {
            convertDataToFake(vbf_D0_0p2to0p4, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_VBF > 0) {
              convertDataToFake(vbf_D0_0p2to0p4_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_VBF < 0) {
              convertDataToFake(vbf_D0_0p2to0p4_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          } else if (D0_VBF <= 0.8) {
            convertDataToFake(vbf_D0_0p4to0p8, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_VBF > 0) {
              convertDataToFake(vbf_D0_0p4to0p8_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_VBF < 0) {
              convertDataToFake(vbf_D0_0p4to0p8_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          } else if (D0_VBF <= 1.0) {
            convertDataToFake(vbf_D0_0p8to1p0, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_VBF > 0) {
              convertDataToFake(vbf_D0_0p8to1p0_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_VBF < 0) {
              convertDataToFake(vbf_D0_0p8to1p0_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          }

          // Split VBF bins based on MELA ggH variables.
          if (D0_ggH > 0 && D0_ggH <= 0.3) {
            convertDataToFake(vbf_D0ggH_0p00to0p30, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_ggH > 0) {
              convertDataToFake(vbf_D0ggH_0p00to0p30_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_ggH < 0) {
              convertDataToFake(vbf_D0ggH_0p00to0p30_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          } else if (D0_ggH <= 0.45) {
            convertDataToFake(vbf_D0ggH_0p30to0p45, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_ggH > 0) {
              convertDataToFake(vbf_D0ggH_0p30to0p45_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_ggH < 0) {
              convertDataToFake(vbf_D0ggH_0p30to0p45_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          } else if (D0_ggH <= 0.55) {
            convertDataToFake(vbf_D0ggH_0p45to0p55, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_ggH > 0) {
              convertDataToFake(vbf_D0ggH_0p45to0p55_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_ggH < 0) {
              convertDataToFake(vbf_D0ggH_0p45to0p55_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          } else if (D0_ggH <= 1.0) {
            convertDataToFake(vbf_D0ggH_0p55to1p00, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            if (DCP_ggH > 0) {
              convertDataToFake(vbf_D0ggH_0p55to1p00_DCPp, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            } else if (DCP_ggH < 0) {
              convertDataToFake(vbf_D0ggH_0p55to1p00_DCPm, name, observable, m_sv, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, weight);
            }
          }
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
