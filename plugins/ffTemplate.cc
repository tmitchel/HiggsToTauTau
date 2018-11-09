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
#include "../include/CLParser.h"

// class to hold the histograms until I'm ready to write them
class histHolder {
public:
  histHolder(std::vector<int>, std::string, std::string);
  void writeHistos();
  void initVectors(std::string);

  TH1F *fake_0jet, *fake_boosted, *fake_vbf, *fake_inclusive;
  TFile *fout;
  std::vector<int> bins;
  std::string channel_prefix;
  std::map<std::string, std::vector<TH1F *>> hists;
};

void read_directory(const std::string &name, std::vector<std::string> &v);
void fillFake(TH1F*, std::string, double, double);

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
    lep_charge = "el_charge";
  } else if (tree_name.find("mutau_tree") != std::string::npos) {
    channel_prefix = "mt";
    lep_charge = "mu_charge";
  } else if (tree_name.find("tt_tree") != std::string::npos) {
    channel_prefix = "tt";
    lep_charge = "t2_charge";
  } else {
    std::cerr << "Um. I don't know that tree. Sorry...";
    return -1;
  }

  // initialize histogram holder 
  auto hists = new histHolder(bins, var_name, channel_prefix);

  // read all files from input directory
  std::vector<std::string> files;
  read_directory(dir, files);

  for (auto ifile : files) {
    auto fin = new TFile((dir+"/"+ifile).c_str(), "read");
    auto tree = (TTree*)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    hists->initVectors(name);

    // I hate doing it like this, but when I move the SetBranchAddress I see unexpected behavior
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_qcd, is_antiLepIso, is_antiTauIso, is_looseIso, OS;
    Float_t hpt, mt, mjj, var, weight, FF_weight;
    Float_t njets, nbjets;

    tree->SetBranchAddress("higgs_pT", &hpt);
    tree->SetBranchAddress("mt", &mt);

    if (var_name.find("mjj") != std::string::npos) {
      tree->SetBranchAddress("mjj", &var);
    } else {
      tree->SetBranchAddress("mjj", &mjj);
      tree->SetBranchAddress(var_name.c_str(), &var);
    }

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("FF_weight", &FF_weight);

    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("is_signal", &is_signal);
    tree->SetBranchAddress("is_qcd", &is_qcd);
    tree->SetBranchAddress("is_antiLepIso", &is_antiLepIso);
    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("is_looseIso", &is_looseIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);
    tree->SetBranchAddress("OS", &OS);

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

      if (OS == 0) {
        continue;
      }

      // output histograms for the template
      if (is_signal > 0) {
        hists->hists.at(channel_prefix+"_inclusive").back()->Fill(var, weight);
        if (cat_0jet > 0) {
          hists->hists.at(channel_prefix+"_0jet").back()->Fill(var, weight);
        } else if (cat_boosted > 0) {
          hists->hists.at(channel_prefix+"_boosted").back()->Fill(var, weight);
        }else if (cat_vbf > 0) {
          hists->hists.at(channel_prefix+"_vbf").back()->Fill(var, weight);
        }
      } else if (is_antiTauIso) {
        fillFake(hists->fake_inclusive, name, var, weight * FF_weight);
        if (cat_0jet > 0) {
          fillFake(hists->fake_0jet, name, var, weight * FF_weight);
        } else if (cat_boosted > 0) {
          fillFake(hists->fake_boosted, name, var, weight * FF_weight);
        } else if (cat_vbf > 0) {
          fillFake(hists->fake_vbf, name, var, weight * FF_weight);
        }
      }
    }
  }
  // write the output histograms
  hists->writeHistos();
}

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

// Fill histogram with positive weight for Data and negative weight for BKG. Equivalent to 
// doing data-bkg
void fillFake(TH1F* hist, std::string name, double var, double weight) {
  if (name.find("Data") != std::string::npos) {
    hist->Fill(var, weight);
  } else if (name == "embed" || name == "ZL" || name == "ZJ" || name == "TTT"  || 
             name == "TTJ"   || name == "W"  || name == "VV" || name == "EWKZ" || name == "ZTT") 
  {
    hist->Fill(var, -1*weight);
  }
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
  channel_prefix( channel_prefix )
{
  for (auto it = hists.begin(); it != hists.end(); it++) {
    fout->cd();
    fout->mkdir((it->first).c_str());
    fout->cd();
  }

  fake_inclusive = new TH1F("fake_inclusive", "fake_inclusive", bins.at(0), bins.at(1), bins.at(2));
  fake_0jet = new TH1F("fake_0jet", "fake_SS", bins.at(0), bins.at(1), bins.at(2));
  fake_boosted = new TH1F("fake_boosted", "fake_SS", bins.at(0), bins.at(1), bins.at(2));
  fake_vbf = new TH1F("fake_vbf", "fake_SS", bins.at(0), bins.at(1), bins.at(2));
}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void histHolder::initVectors(std::string name) {
  for (auto key : hists) {
    fout->cd(key.first.c_str());
    hists.at(key.first.c_str()).push_back(new TH1F(name.c_str(), name.c_str(), bins.at(0), bins.at(1), bins.at(2)));
  }
}

// write output histograms including the QCD histograms after scaling by OS/SS ratio
void histHolder::writeHistos() {
  for (auto cat : hists) {
    fout->cd(cat.first.c_str());
    TH1F* allBkg = new TH1F("allBkg", "allBkg", bins.at(0), bins.at(1), bins.at(2));
    for (auto hist : cat.second) {
      hist->Write();
      std::string name = hist->GetName();
      if (name == "embed" || name == "ZL" || name == "ZJ" || name == "TTT"  || 
          name == "TTJ"   || name == "W"  || name == "VV" || name == "EWKZ" || name == "ZTT") 
      {
        allBkg->Add(hist);
      }
    }
  }

  fout->cd((channel_prefix+"_inclusive").c_str());
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
      fake_boosted -> SetBinContent(i, 0);
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