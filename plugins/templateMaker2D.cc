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
#include "../include/CLParser.h"

// class to hold the histograms until I'm ready to write them
class histHolder {
public:
  histHolder(std::vector<int>, std::string, std::string);
  ~histHolder();
  void writeHistos();
  void initVectors(std::string);

  TH2F *qcd_0jet_OS, *qcd_boosted_OS, *qcd_vbf_OS, 
       *qcd_0jet_SS, *qcd_boosted_SS, *qcd_vbf_SS, 
       *qcd_0jet   , *qcd_boosted   , *qcd_vbf   ;
  TFile *fout;
  std::vector<int> bins;
  std::map<std::string, std::vector<TH2F *>> hists;
};

void read_directory(const std::string &name, std::vector<std::string> &v);
void fillQCD(TH2F*, std::string, double, double, double);

int main(int argc, char *argv[]) {
  // get CLI arguments
  CLParser* parser = new CLParser(argc, argv);
  std::string dir = parser->Option("-d");
  std::string tvar1 = parser->Option("-v1");
  std::string tvar2 = parser->Option("-v2");
  std::vector<std::string> sbins1 = parser->MultiOption("-b", 6);

  // get the provided histogram binning
  std::vector<int> bins;
  for (auto sbin : sbins1) {
    bins.push_back(std::stoi(sbin));
  }

  // get input file directory
  if (dir.empty()) {
    std::cerr << "You must give an input directory" << std::endl;
    return -1;
  }

  // initialize histogram holder 
  auto hists = new histHolder(bins, tvar1, tvar2);

  // read all files from input directory
  std::vector<std::string> files;
  read_directory(dir, files);

  for (auto ifile : files) {
    auto fin = new TFile((dir+"/"+ifile).c_str(), "read");
    auto tree = (TTree*)fin->Get("etau_tree");
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    hists->initVectors(name);

    // I hate doing it like this, but when I move the SetBranchAddres I see unexpected behavior
    Int_t cat_inclusive, cat_0jet, cat_boosted, cat_vbf, cat_antiiso, cat_antiiso_0jet, cat_antiiso_boosted, cat_antiiso_vbf, cat_qcd, cat_qcd_0jet, cat_qcd_boosted, cat_qcd_vbf;
    Float_t eq, tq, hpt, var1, var2, weight;
    tree->SetBranchAddress("el_charge", &eq);
    tree->SetBranchAddress("t1_charge", &tq);
    tree->SetBranchAddress("higgs_pT", &hpt);
    tree->SetBranchAddress(tvar1.c_str(), &var1);
    tree->SetBranchAddress(tvar2.c_str(), &var2);
    tree->SetBranchAddress("evtwt", &weight);
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

      if (eq + tq == 0) {
        // output histograms for the template
        if (cat_0jet > 0) {
          hists->hists.at("et_0jet").back()->Fill(var1, var2, weight);
        }
        if (cat_boosted > 0) {
          hists->hists.at("et_boosted").back()->Fill(var1, var2, weight);
        }
        if (cat_vbf > 0 && hpt > 50) {
          hists->hists.at("et_vbf").back()->Fill(var1, var2, weight);
        }
      } else {
        // get QCD shape from SS loose iso region
        if (cat_qcd_0jet > 0) {
          fillQCD(hists->qcd_0jet, name, var1, var2, weight);
        }
        if (cat_qcd_boosted > 0) {
          fillQCD(hists->qcd_boosted, name, var1, var2, weight);
        }
        if (cat_qcd_vbf > 0 && hpt > 50) {
          fillQCD(hists->qcd_vbf, name, var1, var2, weight);
        }

        // get SS in signal region for loose region normalization
        if (cat_0jet > 0) {
          fillQCD(hists->qcd_0jet_SS, name, var1, var2, weight);
        }
        if (cat_boosted > 0) {
          fillQCD(hists->qcd_boosted_SS, name, var1, var2, weight);
        }
        if (cat_vbf > 0 && hpt > 50) {
          fillQCD(hists->qcd_vbf_SS, name, var1, var2, weight);
        }
      }
    }
  }
  // write the output histograms
  hists->writeHistos();
  delete hists;
  delete parser;
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
void fillQCD(TH2F* hist, std::string name, double var1, double var2, double weight) {
  if (name.find("Data") != std::string::npos) {
    hist->Fill(var1, var2, weight);
  } else if (name == "embed" || name == "ZL" || name == "ZJ" || name == "TTT" || name == "TTJ" || name == "W" || name == "VV") {
    hist->Fill(var1, var2, -1*weight);
  }
}

// histHolder contructor to create the output file, the qcd histograms with the correct binning
// and the map from categories to vectors of TH1F*'s. Each TH1F* in the vector corresponds to 
// one file that is being put into that categories directory in the output tempalte
histHolder::histHolder(std::vector<int> Bins, std::string tvar1, std::string tvar2) :
  hists {
    {"et_0jet", std::vector<TH2F *>()},
    {"et_boosted", std::vector<TH2F *>()},
    {"et_vbf", std::vector<TH2F *>()},
  }, 
  fout( new TFile(("Output/templates/template_"+tvar1+"_"+tvar2+".root").c_str(), "recreate") ),
  bins( Bins )
{
  for (auto it = hists.begin(); it != hists.end(); it++) {
    fout->cd();
    fout->mkdir((it->first).c_str());
    fout->cd();
  }

  qcd_0jet_OS    = new TH2F("qcd_0jet_OS"   , "qcd_0jet_OS"   , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_boosted_OS = new TH2F("qcd_boosted_OS", "qcd_boosted_OS", bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_vbf_OS     = new TH2F("qcd_vbf_OS"    , "qcd_vbf_OS"    , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_0jet_SS    = new TH2F("qcd_0jet_SS"   , "qcd_0jet_SS"   , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_boosted_SS = new TH2F("qcd_boosted_SS", "qcd_boosted_SS", bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_vbf_SS     = new TH2F("qcd_vbf_SS"    , "qcd_vbf_SS"    , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_0jet       = new TH2F("qcd_0jet"      , "qcd_0jet"      , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_boosted    = new TH2F("qcd_boosted"   , "qcd_boosted"   , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  qcd_vbf        = new TH2F("qcd_vbf"       , "qcd_vbf"       , bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
}

histHolder::~histHolder() {}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void histHolder::initVectors(std::string name) {
  for (auto key : hists) {
    fout->cd(key.first.c_str());
    if (name.find("Data") != std::string::npos) {
      name = "data_obs";
    }
    hists.at(key.first.c_str()).push_back(new TH2F(name.c_str(), name.c_str(), bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5)));
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

  fout->cd("et_0jet");
  qcd_0jet->SetName("QCD");
  qcd_0jet->Scale(1.0 * qcd_0jet_SS->Integral() / qcd_0jet->Integral());
  for (auto i = 0; i < qcd_0jet->GetNbinsX(); i++) {
    if (qcd_0jet->GetBinContent(i) < 0) {
      qcd_0jet->SetBinContent(i, 0);
    }
  }
  qcd_0jet->Write();

  fout->cd("et_boosted");
  qcd_boosted->SetName("QCD");
  qcd_boosted->Scale(1.28 * qcd_boosted_SS->Integral() / qcd_boosted->Integral());
  for (auto i = 0; i < qcd_boosted->GetNbinsX(); i++) {
    if (qcd_boosted->GetBinContent(i) < 0) {
      qcd_boosted->SetBinContent(i, 0);
    }
  }
  qcd_boosted->Write();

  fout->cd("et_vbf");
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
