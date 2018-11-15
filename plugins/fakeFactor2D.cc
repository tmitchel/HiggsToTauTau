// system includes
#include <dirent.h>
#include <map>
#include <string>
#include <sys/types.h>
#include <iostream>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
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

enum categories {inclusive, zeroJet, boosted, vbf};

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
  histHolder(std::vector<int>, std::string, std::string);
  ~histHolder() { delete ff_weight; };
  void writeHistos();
  void initVectors(std::string);
  void fillFraction(int, std::string, double, double, double);
  void convertDataToFake(TH1F*, std::string, double, double);
  void histoLoop(std::vector<std::string>, std::string, std::string, std::string);
  void getJetFakes(std::vector<std::string>, std::string, std::string, std::string);

  TFile *fout;
  std::vector<int> bins;
  std::vector<float> mvis_bins, njets_bins;
  float *mvis_bins_real, *njets_bins_real;
  FakeFactor* ff_weight;
  std::string channel_prefix;
  std::map<std::string, std::vector<TH1F *>> hists;
  TH1F *fake_0jet, *fake_boosted, *fake_vbf, *fake_inclusive;
  std::vector<TH2F*> data, frac_w, frac_tt, frac_real, frac_qcd;
};

int main(int argc, char *argv[]) {
  // get CLI arguments
  CLParser parser(argc, argv);
  std::string dir = parser.Option("-d");
  std::string var_name = parser.Option("-v");
  std::string tree_name = parser.Option("-t");
  std::vector<std::string> sbins = parser.MultiOption("-b", 3);

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
  auto hists = new histHolder(bins, var_name, channel_prefix);

  // read all files from input directory
  std::vector<std::string> files;
  read_directory(dir, files);

  hists->histoLoop(files, dir, tree_name, var_name);
  hists->getJetFakes(files, dir, tree_name, var_name);
  hists->writeHistos();

  delete hists->ff_weight;
}

// histHolder contructor to create the output file, the qcd histograms with the correct binning
// and the map from categories to vectors of TH1F*'s. Each TH1F* in the vector corresponds to 
// one file that is being put into that categories directory in the output tempalte
histHolder::histHolder(std::vector<int> Bins, std::string var_name, std::string channel_prefix) :
  hists {
    {(channel_prefix+"_inclusive").c_str(), std::vector<TH1F *>()},
    {(channel_prefix+"_0jet").c_str(), std::vector<TH1F *>()},
    {(channel_prefix+"_boosted").c_str(), std::vector<TH1F *>()},
    {(channel_prefix+"_vbf").c_str(), std::vector<TH1F *>()},
  }, 
  fout( new TFile(("Output/templates/template_"+channel_prefix+"_"+var_name+".root").c_str(), "recreate") ),
  bins( Bins ), 
  mvis_bins( {0,50,80,100,110,120,130,150,170,200,250,1000} ),
  njets_bins( {-0.5,0.5,1.5,15} ),
  channel_prefix( channel_prefix )
{
  for (auto it = hists.begin(); it != hists.end(); it++) {
    fout->cd();
    fout->mkdir((it->first).c_str());
    fout->cd();
  }

  mvis_bins_real = &mvis_bins[0];
  njets_bins_real = &njets_bins[0];

  // final jetFakes distributions
  fake_inclusive = new TH1F("fake_inclusive", "fake_inclusive", bins.at(0), bins.at(1), bins.at(2));
  fake_0jet      = new TH1F("fake_0jet"     , "fake_0jet"     , bins.at(0), bins.at(1), bins.at(2));
  fake_boosted   = new TH1F("fake_boosted"  , "fake_boosted"  , bins.at(0), bins.at(1), bins.at(2));
  fake_vbf       = new TH1F("fake_vbf"      , "fake_vbf"      , bins.at(0), bins.at(1), bins.at(2));
  
  // histograms for getting the fake fractions
  data = {
      new TH2F("data_inclusive", "data_inclusive", mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("data_0jet"     , "data_0jet"     , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("data_boosted"  , "data_boosted"  , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("data_vbf"      , "data_vbf"      , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
  };
  frac_w = {
      new TH2F("frac_w_inclusive", "frac_w_inclusive", mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_w_0jet"     , "frac_w_0jet"     , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_w_boosted"  , "frac_w_boosted"  , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_w_vbf"      , "frac_w_vbf"      , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
  };
  frac_tt = {
      new TH2F("frac_tt_inclusive", "frac_tt_inclusive", mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_tt_0jet"     , "frac_tt_0jet"     , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_tt_boosted"  , "frac_tt_boosted"  , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_tt_vbf"      , "frac_tt_vbf"      , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
  };
  frac_real = {
      new TH2F("frac_real_inclusive", "frac_real_inclusive", mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_real_0jet"     , "frac_real_0jet"     , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_real_boosted"  , "frac_real_boosted"  , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_real_vbf"      , "frac_real_vbf"      , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
  };
  frac_qcd = {
      new TH2F("frac_qcd_inclusive", "frac_qcd_inclusive", mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_qcd_0jet"     , "frac_qcd_0jet"     , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_qcd_boosted"  , "frac_qcd_boosted"  , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
      new TH2F("frac_qcd_vbf"      , "frac_qcd_vbf"      , mvis_bins.size()-1, mvis_bins_real, njets_bins.size()-1, njets_bins_real),
  };

  // get FakeFactor workspace
  TFile ff_file("${CMSSW_BASE}/src/HTTutilities/Jet2TauFakes/data/SM2016_ML/tight/et/fakeFactors_20180831_tight.root");
  ff_weight = (FakeFactor *)ff_file.Get("ff_comb");
  ff_file.Close();
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void histHolder::initVectors(std::string name) {
  for (auto key : hists) {
    fout->cd(key.first.c_str());
    hists.at(key.first.c_str()).push_back(new TH1F(name.c_str(), name.c_str(), bins.at(0), bins.at(1), bins.at(2)));
  }
}

// fill histograms to get the 4 fractions
void histHolder::fillFraction(int cat, std::string name, double vis_mass, double njets, double weight) {
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
  hist->Fill(vis_mass, njets, weight);
}

// only apply the fake factor to data to get jetFakes
void histHolder::convertDataToFake(TH1F *hist, std::string name, double var, double weight) {
  if (name.find("Data") != std::string::npos) {
    hist->Fill(var, weight);
  }
}

void histHolder::histoLoop(std::vector<std::string> files, std::string dir, std::string tree_name, std::string var_name) {

  for (auto ifile : files) {
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = (TTree *)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    initVectors(name);
    fout->cd();

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_antiTauIso, is_signal, OS;
    Float_t var, weight, t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso;

    if (var_name.find("t1_pt") != std::string::npos) {
      tree->SetBranchAddress("t1_pt", &var);
    } else {
      tree->SetBranchAddress("t1_pt", &t1_pt);
      tree->SetBranchAddress(var_name.c_str(), &var);
    }

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress("el_iso", &el_iso);

    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("is_signal", &is_signal);
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
        hists.at(channel_prefix + "_inclusive").back()->Fill(var, weight);
        if (cat_0jet > 0) {
          hists.at(channel_prefix + "_0jet").back()->Fill(var, weight);
        }
        if (cat_boosted > 0) {
          hists.at(channel_prefix + "_boosted").back()->Fill(var, weight);
        }
        if (cat_vbf > 0) {
          hists.at(channel_prefix + "_vbf").back()->Fill(var, weight);
        }
      } else if (is_antiTauIso) {

        if (!(name == "W"   || name == "ZJ"  || name == "VVJ" ||
              name == "TTJ" ||
              name == "ZTT" || name == "TTT" || name == "VVT" ||
              name == "Data")) {
          continue;
        }
        
        fillFraction(inclusive, name, vis_mass, njets, weight);
        if (cat_0jet > 0) {
          fillFraction(zeroJet, name, vis_mass, njets, weight);
        } else if (cat_boosted > 0) {
          fillFraction(boosted, name, vis_mass, njets, weight);
        } else if (cat_vbf > 0) {
          fillFraction(vbf, name, vis_mass, njets, weight);
        }
      }
    }
  }

  // take the fractions
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

void histHolder::getJetFakes(std::vector<std::string> files, std::string dir, std::string tree_name, std::string var_name) {

  for (auto ifile : files) {
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = (TTree *)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    if (name != "Data") {
      continue;
    }

    // get variables from file
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_antiTauIso, is_signal, OS;
    Float_t var, weight, t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso;

    if (var_name.find("t1_pt") != std::string::npos) {
      tree->SetBranchAddress("t1_pt", &var);
    } else {
      tree->SetBranchAddress("t1_pt", &t1_pt);
      tree->SetBranchAddress(var_name.c_str(), &var);
    }

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress("el_iso", &el_iso);

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
        auto bin_x = data.at(inclusive)->GetXaxis()->FindBin(vis_mass);
        auto bin_y = data.at(inclusive)->GetYaxis()->FindBin(njets);
        
        auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                            frac_w.at(inclusive)->GetBinContent(bin_x, bin_y),
                                            frac_tt.at(inclusive)->GetBinContent(bin_x, bin_y),
                                            frac_qcd.at(inclusive)->GetBinContent(bin_x, bin_y)});
        convertDataToFake(fake_inclusive, name, var, weight * fakeweight);

        if (cat_0jet) {
          auto bin = data.at(zeroJet)->FindBin(var);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                              frac_w.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(zeroJet)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(zeroJet)->GetBinContent(bin_x, bin_y)});
          convertDataToFake(fake_0jet, name, var, weight * fakeweight);
        } else if (cat_boosted) {
          auto bin = data.at(boosted)->FindBin(var);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                              frac_w.at(boosted)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(boosted)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(boosted)->GetBinContent(bin_x, bin_y)});
          convertDataToFake(fake_boosted, name, var, weight * fakeweight);
        } else if (cat_vbf) {
          auto bin = data.at(vbf)->FindBin(var);
          auto fakeweight = ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, el_iso,
                                              frac_w.at(vbf)->GetBinContent(bin_x, bin_y),
                                              frac_tt.at(vbf)->GetBinContent(bin_x, bin_y),
                                              frac_qcd.at(vbf)->GetBinContent(bin_x, bin_y)});
          convertDataToFake(fake_vbf, name, var, weight * fakeweight);
        }
      }
    }
  }
}

// write output histograms including the QCD histograms after scaling by OS/SS ratio
void histHolder::writeHistos() {
  for (auto cat : hists) {
    fout->cd(cat.first.c_str());
    TH1F *allBkg = new TH1F("allBkg", "allBkg", bins.at(0), bins.at(1), bins.at(2));
    for (auto hist : cat.second) {
      hist->Write();
    }
  }

  fout->cd((channel_prefix + "_inclusive").c_str());
  fake_inclusive->SetName("jetFakes");
  for (auto i = 0; i < fake_inclusive->GetNbinsX(); i++) {
    if (fake_inclusive->GetBinContent(i) < 0) {
      fake_inclusive->SetBinContent(i, 0);
    }
  }
  fake_inclusive->Write();

  fout->cd((channel_prefix + "_0jet").c_str());
  fake_0jet->SetName("jetFakes");
  for (auto i = 0; i < fake_0jet->GetNbinsX(); i++) {
    if (fake_0jet->GetBinContent(i) < 0) {
      fake_0jet->SetBinContent(i, 0);
    }
  }
  fake_0jet->Write();

  fout->cd((channel_prefix + "_boosted").c_str());
  fake_boosted->SetName("jetFakes");
  for (auto i = 0; i < fake_boosted->GetNbinsX(); i++) {
    if (fake_boosted->GetBinContent(i) < 0) {
      fake_boosted->SetBinContent(i, 0);
    }
  }
  fake_boosted->Write();

  fout->cd((channel_prefix + "_vbf").c_str());
  fake_vbf->SetName("jetFakes");
  for (auto i = 0; i < fake_vbf->GetNbinsX(); i++) {
    if (fake_vbf->GetBinContent(i) < 0) {
      fake_vbf->SetBinContent(i, 0);
    }
  }
  fake_vbf->Write();

  fout->Close();
}