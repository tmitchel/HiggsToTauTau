// system includes
#include <dirent.h>
#include <map>
#include <string>
#include <sys/types.h>
#include <iostream>

// ROOT includes
#include "TFile.h"
#include "TH2F.h"
#include "TTree.h"

// user includes
#include "CLParser.h"

//FF
#include "HTTutilities/Jet2TauFakes/interface/FakeFactor.h"
#include "HTTutilities/Jet2TauFakes/interface/IFunctionWrapper.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTFormula.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTGraph.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTH2F.h"
#include "HTTutilities/Jet2TauFakes/interface/WrapperTH3D.h"

enum categories {zeroJet, boosted, vbf};

// read all *.root files in the given directory and put them in the provided vector
void read_directory(const std::string &name, std::vector<std::string> &v) {
  DIR *dirp = opendir(name.c_str());
  struct dirent *dp;
  while ((dp = readdir(dirp)) != 0) {
    if (static_cast<std::string>(dp->d_name).find("root") != std::string::npos) {
      v.push_back(dp->d_name);
    }
  }
  closedir(dirp);
}

// class to hold the histograms until I'm ready to write them
class histHolder {
public:
  histHolder(std::string);
  ~histHolder() { delete ff_weight; };
  void writeHistos();
  void initVectors(std::string);
  void initSystematics(std::string);
  void fillFraction(int, std::string, double, double, double);
  void convertDataToFake(TH2F*, std::string, double, double, double);
  void histoLoop(std::vector<std::string>, std::string, std::string);
  void getJetFakes(std::vector<std::string>, std::string, std::string);
  void runSystematics(std::vector<std::string>, std::string, std::string);

  TFile *fout;
  FakeFactor* ff_weight;
  std::string channel_prefix;
  std::vector<std::string> systematics;
  std::map<std::string, std::vector<TH2F *>> hists;
  std::map<std::string, std::vector<TH2F *>> FF_systs;
  TH2F *fake_0jet, *fake_boosted, *fake_vbf;
  std::vector<TH2F*> data, frac_w, frac_tt, frac_real, frac_qcd;

  // binning
  std::vector<Float_t> bins_l2, bins_hpt, bins_mjj, bins_lpt, bins_msv1, bins_msv2;
};

int main(int argc, char *argv[]) {
  // get CLI arguments
  CLParser parser(argc, argv);
  bool doSyst = parser.Flag("-s");
  std::string dir = parser.Option("-d");
  std::string tree_name = parser.Option("-t");

  // get input file directory
  if (dir.empty()) {
    std::cerr << "You must give an input directory" << std::endl;
    return -1;
  }

  // get channel info
  std::string channel_prefix, lep_charge;
  if (tree_name.find("etau_tree") != std::string::npos) {
    channel_prefix = "et";
  } else if (tree_name.find("mutau_tree") != std::string::npos) {
    channel_prefix = "mt";
  } else if (tree_name.find("tautau_tree") != std::string::npos) {
    channel_prefix = "tt";
  } else {
    std::cerr << "Um. I don't know that tree. Sorry...";
    return -1;
  }

  // initialize histogram holder 
  auto hists = new histHolder(channel_prefix);

  // read all files from input directory
  std::vector<std::string> files;
  read_directory(dir, files);

  hists->histoLoop(files, dir, tree_name);
  hists->getJetFakes(files, dir, tree_name);
  if (doSyst) {
    hists->runSystematics(files, dir, tree_name);
  }
  hists->writeHistos();

  delete hists->ff_weight;
}

// histHolder contructor to create the output file, the qcd histograms with the correct binning
// and the map from categories to vectors of TH2F*'s. Each TH2F* in the vector corresponds to 
// one file that is being put into that categories directory in the output tempalte
histHolder::histHolder(std::string channel_prefix) :
  hists {
    {(channel_prefix+"_0jet").c_str(), std::vector<TH2F *>()},
    {(channel_prefix+"_boosted").c_str(), std::vector<TH2F *>()},
    {(channel_prefix+"_vbf").c_str(), std::vector<TH2F *>()},
  }, 
  FF_systs {
    {(channel_prefix+"_0jet").c_str(), std::vector<TH2F *>()},
    {(channel_prefix+"_boosted").c_str(), std::vector<TH2F *>()},
    {(channel_prefix+"_vbf").c_str(), std::vector<TH2F *>()},
  }, 
  fout( new TFile(("Output/templates/template_"+channel_prefix+"_finalFFv1.root").c_str(), "recreate") ),
  // x-axis
  bins_l2 {0, 1, 10, 11},
  bins_hpt {0, 100, 150, 200, 250, 300, 5000},
  bins_mjj {300, 700, 1100, 1500, 10000},
  //bins_mjj {0., 0.1, 0.5, 0.9, 1.},

  // y-axis
  bins_lpt {0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400},
  bins_msv1 {0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300},
  bins_msv2 {0, 95, 115, 135, 155, 400},
  channel_prefix( channel_prefix ), 
  systematics {
    "ff_qcd_syst_up"               ,"ff_qcd_syst_down"           ,"ff_qcd_dm0_njet0_stat_up"   ,
    "ff_qcd_dm0_njet0_stat_down","ff_qcd_dm0_njet1_stat_up"   ,"ff_qcd_dm0_njet1_stat_down" ,
    "ff_qcd_dm1_njet0_stat_up"  ,"ff_qcd_dm1_njet0_stat_down" ,"ff_qcd_dm1_njet1_stat_up"   ,
    "ff_qcd_dm1_njet1_stat_down","ff_w_syst_up"               ,"ff_w_syst_down"             ,"ff_w_dm0_njet0_stat_up",
    "ff_w_dm0_njet0_stat_down"  ,"ff_w_dm0_njet1_stat_up"     ,"ff_w_dm0_njet1_stat_down"   ,
    "ff_w_dm1_njet0_stat_up"    ,"ff_w_dm1_njet0_stat_down"   ,"ff_w_dm1_njet1_stat_up"     ,
    "ff_w_dm1_njet1_stat_down"  ,"ff_tt_syst_up"              ,"ff_tt_syst_down"            ,"ff_tt_dm0_njet0_stat_up",
    "ff_tt_dm0_njet0_stat_down" ,"ff_tt_dm0_njet1_stat_up"    ,"ff_tt_dm0_njet1_stat_down"  ,
    "ff_tt_dm1_njet0_stat_up"   ,"ff_tt_dm1_njet0_stat_down"  ,"ff_tt_dm1_njet1_stat_up"    , "ff_tt_dm1_njet1_stat_down"
  }
{
  for (auto it = hists.begin(); it != hists.end(); it++) {
    fout->cd();
    fout->mkdir((it->first).c_str());
    fout->cd();
  }

  fake_0jet    = new TH2F("fake_0jet"   , "fake_SS", bins_l2.size()  - 1, &bins_l2[0] , bins_lpt.size()  - 1, &bins_lpt[0] );
  fake_boosted = new TH2F("fake_boosted", "fake_SS", bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]);
  fake_vbf     = new TH2F("fake_vbf"    , "fake_SS", bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]);
  data = {
      new TH2F("data_0jet"     , "data_0jet"     , bins_l2.size()  - 1, &bins_l2[0] , bins_lpt.size()  - 1, &bins_lpt[0] ),
      new TH2F("data_boosted"  , "data_boosted"  , bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]),
      new TH2F("data_vbf"      , "data_vbf"      , bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]),
  };
  frac_w = {
      new TH2F("frac_w_0jet"     , "frac_w_0jet"     , bins_l2.size()  - 1, &bins_l2[0] , bins_lpt.size()  - 1, &bins_lpt[0] ),
      new TH2F("frac_w_boosted"  , "frac_w_boosted"  , bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]),
      new TH2F("frac_w_vbf"      , "frac_w_vbf"      , bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]),
  };
  frac_tt = {
      new TH2F("frac_tt_0jet"     , "frac_tt_0jet"     , bins_l2.size()  - 1, &bins_l2[0] , bins_lpt.size()  - 1, &bins_lpt[0] ),
      new TH2F("frac_tt_boosted"  , "frac_tt_boosted"  , bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]),
      new TH2F("frac_tt_vbf"      , "frac_tt_vbf"      , bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]),
  };
  frac_real = {
      new TH2F("frac_real_0jet"     , "frac_real_0jet"     , bins_l2.size()  - 1, &bins_l2[0] , bins_lpt.size()  - 1, &bins_lpt[0] ),
      new TH2F("frac_real_boosted"  , "frac_real_boosted"  , bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]),
      new TH2F("frac_real_vbf"      , "frac_real_vbf"      , bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]),
  };
  frac_qcd = {
      new TH2F("frac_qcd_0jet"     , "frac_qcd_0jet"     , bins_l2.size()  - 1, &bins_l2[0] , bins_lpt.size()  - 1, &bins_lpt[0] ),
      new TH2F("frac_qcd_boosted"  , "frac_qcd_boosted"  , bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]),
      new TH2F("frac_qcd_vbf"      , "frac_qcd_vbf"      , bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]),
  };

  TFile ff_file("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data/SM2016_ML/tight/et/fakeFactors_20180831_tight.root");
  ff_weight = (FakeFactor *)ff_file.Get("ff_comb");
  ff_file.Close();
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void histHolder::initVectors(std::string name) {
  for (auto key : hists) {
    fout->cd(key.first.c_str());
    if (name.find("Data") != std::string::npos) {
      name = "data_obs";
    }
    if (key.first == channel_prefix + "_0jet") {
      hists.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
    } else if (key.first == channel_prefix + "_boosted") {
      hists.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
    } else if (key.first == channel_prefix + "_vbf") {
      hists.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
    }
  }
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void histHolder::initSystematics(std::string name) {
  for (auto key : FF_systs) {
    fout->cd(key.first.c_str());
    std::string name = "jetFakes_";
    for (auto syst : systematics) {
      if (key.first == channel_prefix + "_0jet") {
        FF_systs.at(key.first.c_str()).push_back(new TH2F((name+syst).c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]));
      } else if (key.first == channel_prefix + "_boosted") {
        FF_systs.at(key.first.c_str()).push_back(new TH2F((name+syst).c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]));
      } else if (key.first == channel_prefix + "_vbf") {
        FF_systs.at(key.first.c_str()).push_back(new TH2F((name+syst).c_str(), name.c_str(), bins_mjj.size() - 1, &bins_mjj[0], bins_msv2.size() - 1, &bins_msv2[0]));
      }
    }
  }
  std::cout << "initialized systematics" << std::endl;
}

void histHolder::fillFraction(int cat, std::string name, double var1, double var2, double weight) {
  TH2F *hist;
  if (name == "Data") {
    hist = data.at(cat);
  } else if (name == "W" || name == "ZJ" || name == "VVJ") {
    hist = frac_w.at(cat);
  } else if (name == "TTJ") {
    hist = frac_tt.at(cat);
  } else if (name == "ZTT" || name == "TTT" || name == "VVT") {
    hist = frac_real.at(cat);
  }
  hist->Fill(var1, var2, weight);
}

void histHolder::histoLoop(std::vector<std::string> files, std::string dir, std::string tree_name) {

  for (auto ifile : files) {
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = (TTree *)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    initVectors(name);
    fout->cd();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiTauIso, OS;
    Float_t higgs_pT, t1_decayMode, vis_mass, mjj, m_sv, weight;

    tree->SetBranchAddress("evtwt", &weight);

    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);
    tree->SetBranchAddress("is_signal", &is_signal);
    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);
    tree->SetBranchAddress("OS", &OS);

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

      // only look at opposite-sign events
      if (OS == 0) {
        continue;
      }

      if (is_signal) {
        if (cat_0jet > 0) {
          hists.at(channel_prefix + "_0jet").back()->Fill(t1_decayMode, vis_mass, weight);
        }
        if (cat_boosted > 0) {
          hists.at(channel_prefix + "_boosted").back()->Fill(higgs_pT, m_sv, weight);
        }
        if (cat_vbf > 0) {
          hists.at(channel_prefix + "_vbf").back()->Fill(mjj, m_sv, weight);
        }
      } else if (is_antiTauIso) {

        if (!(name == "W" || name == "ZJ" || name == "VVJ" ||
              name == "TTJ" ||
              name == "ZTT" || name == "TTT" || name == "VVT" ||
              name == "Data")) {
          continue;
        }

        if (cat_0jet > 0) {
          fillFraction(zeroJet, name, t1_decayMode, vis_mass, weight);
        } else if (cat_boosted > 0) {
          fillFraction(boosted, name, higgs_pT, m_sv, weight);
        } else if (cat_vbf > 0) {
          fillFraction(vbf, name, mjj, m_sv, weight);
        }
      }
    }
  }

  for (int i = 0; i < data.size(); i++) {
    frac_qcd.at(i) = (TH2F*)data.at(i)->Clone();
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

void histHolder::getJetFakes(std::vector<std::string> files, std::string dir, std::string tree_name) {

  for (auto ifile : files) {

    // only need to look at data
    if (ifile.find("Data.root") == std::string::npos) {
      continue;
    }

    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = (TTree *)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_antiTauIso, OS;
    Float_t higgs_pT, mjj, m_sv, weight, t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso;

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("t1_pt", &t1_pt);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress("el_iso", &el_iso);

    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);

    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);
    tree->SetBranchAddress("OS", &OS);

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

      // only look at opposite-sign events
      if (OS == 0) {
        continue;
      }

      if (is_antiTauIso) {
        if (cat_0jet) {
          auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(t1_decayMode);
          auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(vis_mass);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                              frac_w.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(zeroJet)->GetBinContent(bin_x, bin_y)});
          fake_0jet->Fill(t1_decayMode, vis_mass, weight * fakeweight);
        } else if (cat_boosted) {
          auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(higgs_pT);
          auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(m_sv);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                              frac_w.at(boosted)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(boosted)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(boosted)->GetBinContent(bin_x, bin_y)});
          fake_boosted->Fill(higgs_pT, m_sv, weight * fakeweight);
        } else if (cat_vbf) {
          auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(mjj);
          auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(m_sv);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                              frac_w.at(vbf)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(vbf)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(vbf)->GetBinContent(bin_x, bin_y)});
          fake_vbf->Fill(mjj, m_sv, weight * fakeweight);
        }
      }
    }
  }
}

void histHolder::runSystematics(std::vector<std::string> files, std::string dir, std::string tree_name) {

  for (auto ifile : files) {

    // only need to look at data
    if (ifile.find("Data") == std::string::npos) {
      continue;
    }
    
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = (TTree *)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    initSystematics(name);
    fout->cd();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_antiTauIso, OS;
    Float_t higgs_pT, mjj, m_sv, weight, t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso;

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("t1_pt", &t1_pt);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress("el_iso", &el_iso);

    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);

    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);
    tree->SetBranchAddress("OS", &OS);

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

      // only look at opposite-sign events
      if (OS == 0) {
        continue;
      }

      if (is_antiTauIso) {
        for (int i = 0; i < systematics.size(); i++) {
          if (cat_0jet) {
            auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(t1_decayMode);
            auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(vis_mass);
            auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                                frac_w.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                                frac_tt.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                                frac_qcd.at(zeroJet)->GetBinContent(bin_x, bin_y)}, systematics.at(i));
          FF_systs.at("et_0jet").at(i)->Fill(t1_decayMode, vis_mass, weight * fakeweight);
          } else if (cat_boosted) {
            auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(higgs_pT);
            auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(m_sv);
            auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                                frac_w.at(boosted)->GetBinContent(bin_x, bin_y),
                                                frac_tt.at(boosted)->GetBinContent(bin_x, bin_y),
                                                frac_qcd.at(boosted)->GetBinContent(bin_x, bin_y)}, systematics.at(i));
            FF_systs.at("et_boosted").at(i)->Fill(higgs_pT, m_sv, weight * fakeweight);
          } else if (cat_vbf) {
            auto bin_x = data.at(zeroJet)->GetXaxis()->FindBin(mjj);
            auto bin_y = data.at(zeroJet)->GetYaxis()->FindBin(m_sv);
            auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                                frac_w.at(vbf)->GetBinContent(bin_x, bin_y),
                                                frac_tt.at(vbf)->GetBinContent(bin_x, bin_y),
                                                frac_qcd.at(vbf)->GetBinContent(bin_x, bin_y)}, systematics.at(i));
            FF_systs.at("et_vbf").at(i)->Fill(mjj, m_sv, weight * fakeweight);
          }
        }
      }
    }
  }
}

// write output histograms including the QCD histograms after scaling by OS/SS ratio
void histHolder::writeHistos() {
  for (auto cat : hists) {
    fout->cd(cat.first.c_str());
    for (auto hist : cat.second) {
      hist->Write();
    }
  }

  fout->cd((channel_prefix + "_0jet").c_str());
  fake_0jet->SetName("jetFakes");
  for (auto i = 0; i < fake_0jet->GetNbinsX(); i++) {
    for (auto j = 0; j < fake_0jet->GetNbinsY(); j++) {
      if (fake_0jet->GetBinContent(i, j) < 0) {
        fake_0jet->SetBinContent(i, j, 0);
      }
    }
  }
  fake_0jet->Write();

  for (auto& hist : FF_systs.at("et_0jet")) {
    for (auto i = 0; i < hist->GetNbinsX(); i++) {
      for (auto j = 0; j < hist->GetNbinsY(); j++) {
        if (hist->GetBinContent(i, j) < 0) {
          hist->SetBinContent(i, j, 0);
        }
      }
    }
    std::cout << hist->GetName() << std::endl;
    hist->Write();
  }

  fout->cd((channel_prefix + "_boosted").c_str());
  fake_boosted->SetName("jetFakes");
  for (auto i = 0; i < fake_boosted->GetNbinsX(); i++) {
    for (auto j = 0; j < fake_boosted->GetNbinsY(); j++) {
      if (fake_boosted->GetBinContent(i, j) < 0) {
        fake_boosted->SetBinContent(i, j, 0);
      }
    }
  }
  fake_boosted->Write();

  for (auto& hist : FF_systs.at("et_boosted")) {
    for (auto i = 0; i < hist->GetNbinsX(); i++) {
      for (auto j = 0; j < hist->GetNbinsY(); j++) {
        if (hist->GetBinContent(i, j) < 0) {
          hist->SetBinContent(i, j, 0);
        }
      }
    }
    std::cout << hist->GetName() << std::endl;
    hist->Write();
  }

  fout->cd((channel_prefix + "_vbf").c_str());
  fake_vbf->SetName("jetFakes");
  for (auto i = 0; i < fake_vbf->GetNbinsX(); i++) {
    for (auto j = 0; j < fake_vbf->GetNbinsY(); j++) {
      if (fake_vbf->GetBinContent(i, j) < 0) {
        fake_vbf->SetBinContent(i, j, 0);
      }
    }
  }
  fake_vbf->Write();

  for (auto& hist : FF_systs.at("et_vbf")) {
    for (auto i = 0; i < hist->GetNbinsX(); i++) {
      for (auto j = 0; j < hist->GetNbinsY(); j++) {
        if (hist->GetBinContent(i, j) < 0) {
          hist->SetBinContent(i, j, 0);
        }
      }
    }
    std::cout << hist->GetName() << std::endl;
    hist->Write();
  }

  fout->Close();
}
