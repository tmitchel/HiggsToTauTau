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

  hists->histoLoop(files, dir, tree_name);
  hists->getJetFakes(files, dir, tree_name, doSyst);
  if (doAC) {
    for (auto weight : acNameMap) {
      hist->histoLoop(files, dir, tree_name, weight->first);
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

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
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
      tree->SetBranchAddress(acWeight.c_str(), acWeightVal);
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
          hists.at(channel_prefix + "_0jet").back()->Fill(t1_decayMode, vis_mass, weight);
        }
        if (cat1) {
          hists.at(channel_prefix + "_boosted").back()->Fill(higgs_pT, m_sv, weight);
        }
        if (cat2) {
          hists.at(channel_prefix + "_vbf").back()->Fill(observable, m_sv, weight);
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
        }
      }
    }
  }

  for (int i = 0; i < data.size(); i++) {
    frac_qcd.at(i) = reinterpret_cast<TH2F *>(data.at(i)->Clone());
    frac_qcd.at(i)->Add(frac_w.at(i), -1);
    frac_qcd.at(i)->Add(frac_tt.at(i), -1);
    frac_qcd.at(i)->Add(frac_real.at(i), -1);

    std::cout << frac_w.at(i)->GetName() << " " << frac_w.at(i)->Integral()/data.at(i)->Integral() << std::endl;
    std::cout << frac_tt.at(i)->GetName() << " " << frac_tt.at(i)->Integral()/data.at(i)->Integral() << std::endl;
    std::cout << frac_qcd.at(i)->GetName() << " " << frac_qcd.at(i)->Integral()/data.at(i)->Integral() << std::endl;
    std::cout << frac_real.at(i)->GetName() << " " << frac_real.at(i)->Integral()/data.at(i)->Integral() << std::endl;

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
          auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(vis_mass);
          auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(njets);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                              frac_w.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(zeroJet)->GetBinContent(bin_x, bin_y)});
          convertDataToFake(fake_0jet, name, t1_decayMode, vis_mass, weight * fakeweight);
        } else if (cat1) {
          auto bin_x = data.at(boosted)->GetXaxis()->FindBin(vis_mass);
          auto bin_y = data.at(boosted)->GetYaxis()->FindBin(njets);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                              frac_w.at(boosted)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(boosted)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(boosted)->GetBinContent(bin_x, bin_y)});
          convertDataToFake(fake_boosted, name, higgs_pT, m_sv, weight * fakeweight);
        } else if (cat2) {
          auto bin_x = data.at(vbf)->GetXaxis()->FindBin(vis_mass);
          auto bin_y = data.at(vbf)->GetYaxis()->FindBin(njets);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso,
                                              frac_w.at(vbf)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(vbf)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(vbf)->GetBinContent(bin_x, bin_y)});
          convertDataToFake(fake_vbf, name, observable, m_sv, weight * fakeweight);
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
