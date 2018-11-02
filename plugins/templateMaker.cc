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

  TH1F *qcd_0jet_OS, *qcd_boosted_OS, *qcd_vbf_OS, *qcd_inclusive_OS, 
       *qcd_0jet_SS, *qcd_boosted_SS, *qcd_vbf_SS, *qcd_inclusive_SS, 
       *qcd_0jet   , *qcd_boosted   , *qcd_vbf   , *qcd_inclusive;
  TFile *fout;
  std::vector<int> bins;
  std::string channel_prefix;
  std::map<std::string, std::vector<TH1F *>> hists;
};

void read_directory(const std::string &name, std::vector<std::string> &v);
void fillQCD(TH1F*, std::string, double, double);

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

    // I hate doing it like this, but when I move the SetBranchAddres I see unexpected behavior
    Int_t cat_inclusive, cat_0jet, cat_boosted, cat_vbf, cat_antiiso, cat_antiiso_0jet, cat_antiiso_boosted, 
          cat_antiiso_vbf, cat_qcd, cat_qcd_0jet, cat_qcd_boosted, cat_qcd_vbf;
    Int_t is_qcd;
    Float_t eq, tq, hpt, mt, mjj, var, weight;
    Float_t njets, nbjets;

    tree->SetBranchAddress(lep_charge.c_str(), &eq);
    tree->SetBranchAddress("t1_charge", &tq);
    tree->SetBranchAddress("higgs_pT", &hpt);
    tree->SetBranchAddress("mt", &mt);

    if (var_name.find("mjj") != std::string::npos) {
      tree->SetBranchAddress("mjj", &var);
    } else {
      tree->SetBranchAddress("mjj", &mjj);
      tree->SetBranchAddress(var_name.c_str(), &var);
    }

    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("is_qcd", &is_qcd);
    tree->SetBranchAddress("cat_inclusive", &cat_inclusive);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_antiiso", &cat_antiiso);
    tree->SetBranchAddress("cat_antiiso_0jet", &cat_antiiso_0jet);
    tree->SetBranchAddress("cat_antiiso_boosted", &cat_antiiso_boosted);
    tree->SetBranchAddress("cat_antiiso_vbf", &cat_antiiso_vbf);
    tree->SetBranchAddress("cat_qcd",             &cat_qcd        );
    tree->SetBranchAddress("cat_qcd_0jet",        &cat_qcd_0jet   );
    tree->SetBranchAddress("cat_qcd_boosted",     &cat_qcd_boosted);
    tree->SetBranchAddress("cat_qcd_vbf",         &cat_qcd_vbf    );

    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);
      
      // until I remake these trees, correct boosted defintion
      cat_boosted = cat_inclusive && njets == 1;
      cat_qcd_boosted = cat_qcd && njets == 1;

      if (eq + tq == 0) {
        // output histograms for the template
        if (cat_inclusive > 0) {
          hists->hists.at(channel_prefix+"_inclusive").back()->Fill(var, weight);
        }
        if (cat_0jet > 0) {
          hists->hists.at(channel_prefix+"_0jet").back()->Fill(var, weight);
        }
        if (cat_boosted > 0) {
          hists->hists.at(channel_prefix+"_boosted").back()->Fill(var, weight);
        }
        if (cat_vbf > 0 && mt < 50 && nbjets == 0) {
          hists->hists.at(channel_prefix+"_vbf").back()->Fill(var, weight);
        }
      } else {
        // get QCD shape from SS loose iso region
        if (cat_qcd > 0 || (is_qcd > 0 && cat_inclusive)) {
          fillQCD(hists->qcd_inclusive, name, var, weight);
        }
        if (cat_qcd_0jet > 0 || (is_qcd > 0 && cat_0jet > 0)) {
          fillQCD(hists->qcd_0jet, name, var, weight);
        }
        if (cat_qcd_boosted > 0 || (is_qcd > 0 && cat_boosted > 0)) {
          fillQCD(hists->qcd_boosted, name, var, weight);
        }
        if (cat_qcd_vbf > 0 && mt < 50 && nbjets == 0 && (cat_qcd_vbf > 0 || (is_qcd > 0 && cat_vbf > 0))) {
          fillQCD(hists->qcd_vbf, name, var, weight);
        }

        // get SS in signal region for loose region normalization
        if (cat_inclusive > 0) {
          fillQCD(hists->qcd_inclusive_SS, name, var, weight);
        }
        if (cat_0jet > 0) {
          fillQCD(hists->qcd_0jet_SS, name, var, weight);
        }
        if (cat_boosted > 0) {
          fillQCD(hists->qcd_boosted_SS, name, var, weight);
        }
        if (cat_vbf > 0 && mt < 50 && nbjets == 0) {
          fillQCD(hists->qcd_vbf_SS, name, var, weight);
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
void fillQCD(TH1F* hist, std::string name, double var, double weight) {
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

  qcd_inclusive_OS = new TH1F("qcd_inclusive_OS", "qcd_inclusive_OS", bins.at(0), bins.at(1), bins.at(2));
  qcd_0jet_OS = new TH1F("qcd_0jet_OS", "qcd_0jet_OS", bins.at(0), bins.at(1), bins.at(2));
  qcd_boosted_OS = new TH1F("qcd_boosted_OS", "qcd_boosted_OS", bins.at(0), bins.at(1), bins.at(2));
  qcd_vbf_OS = new TH1F("qcd_vbf_OS", "qcd_vbf_OS", bins.at(0), bins.at(1), bins.at(2));
  qcd_inclusive_SS = new TH1F("qcd_inclusive_SS", "qcd_inclusive_SS", bins.at(0), bins.at(1), bins.at(2));
  qcd_0jet_SS = new TH1F("qcd_0jet_SS", "qcd_0jet_SS", bins.at(0), bins.at(1), bins.at(2));
  qcd_boosted_SS = new TH1F("qcd_boosted_SS", "qcd_boosted_SS", bins.at(0), bins.at(1), bins.at(2));
  qcd_vbf_SS = new TH1F("qcd_vbf_SS", "qcd_vbf_SS", bins.at(0), bins.at(1), bins.at(2));
  qcd_inclusive = new TH1F("qcd_inclusive", "qcd_inclusive", bins.at(0), bins.at(1), bins.at(2));
  qcd_0jet = new TH1F("qcd_0jet", "qcd_SS", bins.at(0), bins.at(1), bins.at(2));
  qcd_boosted = new TH1F("qcd_boosted", "qcd_SS", bins.at(0), bins.at(1), bins.at(2));
  qcd_vbf = new TH1F("qcd_vbf", "qcd_SS", bins.at(0), bins.at(1), bins.at(2));
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
      allBkg->Write();
    }
  }

  fout->cd((channel_prefix+"_inclusive").c_str());
  qcd_inclusive->SetName("QCD");
  qcd_inclusive->Scale(1.0 * qcd_inclusive_SS->Integral() / qcd_inclusive->Integral());
  for (auto i = 0; i < qcd_inclusive->GetNbinsX(); i++) {
    if (qcd_inclusive->GetBinContent(i) < 0) {
      qcd_inclusive->SetBinContent(i, 0);
    }
  }
  qcd_inclusive->Write();

  fout->cd((channel_prefix+"_0jet").c_str());
  qcd_0jet->SetName("QCD");
  qcd_0jet->Scale(1.0 * qcd_0jet_SS->Integral() / qcd_0jet->Integral());
  for (auto i = 0; i < qcd_0jet->GetNbinsX(); i++) {
    if (qcd_0jet->GetBinContent(i) < 0) {
      qcd_0jet->SetBinContent(i, 0);
    }
  }
  qcd_0jet->Write();

  fout->cd((channel_prefix+"_boosted").c_str());
  qcd_boosted->SetName("QCD");
  qcd_boosted->Scale(1.28 * qcd_boosted_SS->Integral() / qcd_boosted->Integral());
  for (auto i = 0; i < qcd_boosted->GetNbinsX(); i++) {
    if (qcd_boosted->GetBinContent(i) < 0) {
      qcd_boosted->SetBinContent(i, 0);
    }
  }
  qcd_boosted->Write();

  fout->cd((channel_prefix+"_vbf").c_str());
  qcd_vbf->SetName("QCD");
  qcd_vbf->Scale(1.0 * qcd_vbf_SS->Integral() / qcd_vbf->Integral());
  for (auto i = 0; i < qcd_vbf->GetNbinsX(); i++) {
    if (qcd_vbf->GetBinContent(i) < 0) {
      qcd_vbf->SetBinContent(i, 0);
    }
  }
  qcd_vbf->Write();
  fout->Close();
}
