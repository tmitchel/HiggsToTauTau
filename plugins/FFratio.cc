// system includes
#include <dirent.h>
#include <iostream>
#include <map>
#include <string>
#include <sys/types.h>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"

// user includes
#include "../include/CLParser.h"

// class to hold the histograms until I'm ready to write them
class histHolder {
public:
  histHolder(std::string);
  ~histHolder();
  void writeHistos();
  void initVectors(std::string);

  TH1F *qcd_0jet_OS, *qcd_boosted_OS, *qcd_vbf_OS,
       *qcd_0jet_SS, *qcd_boosted_SS, *qcd_vbf_SS,
       *qcd_0jet   , *qcd_boosted   , *qcd_vbf;
  TFile *fout;
  std::string ch;
  std::map<std::string, std::vector<TH1F *>> hists;

  // binning
  std::vector<Float_t> bins_l2, bins_hpt, bins_mjj, bins_lpt, bins_msv1, bins_msv2;
};

void read_directory(const std::string &name, std::vector<std::string> &v);
void fillQCD(TH1F *, std::string, double, double);

int main(int argc, char *argv[]) {

  // get CLI arguments
  CLParser *parser = new CLParser(argc, argv);
  std::string dir = parser->Option("-d");
  std::string tree_name = parser->Option("-t");

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
  histHolder *hists = new histHolder(channel_prefix);

  // read all files from input directory
  std::vector<std::string> files;
  read_directory(dir, files);

  for (auto ifile : files) {
    auto fin = new TFile((dir + "/" + ifile).c_str(), "read");
    auto tree = (TTree *)fin->Get(tree_name.c_str());
    std::string name = ifile.substr(0, ifile.find(".")).c_str();

    hists->initVectors(name);

    // I hate doing it like this, but when I move the SetBranchAddres I see unexpected behavior
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_qcd, is_antiiso, is_looseIso;
    Float_t eq, tq, hpt, l2decay, vis_mass, mjj, m_sv, weight, ME_sm_VBF, ME_bkg, NN_disc, nbjets, mt, njets, el_pt;
    tree->SetBranchAddress("el_pt", &el_pt);
    tree->SetBranchAddress(lep_charge.c_str(), &eq);
    tree->SetBranchAddress("t1_charge", &tq);
    tree->SetBranchAddress("higgs_pT", &hpt);
    tree->SetBranchAddress("t1_decayMode", &l2decay);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("m_sv", &m_sv);
    tree->SetBranchAddress("ME_sm_VBF", &ME_sm_VBF), 
    tree->SetBranchAddress("ME_bkg", &ME_bkg), 
    tree->SetBranchAddress("NN_disc", &NN_disc), 
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("is_signal", &is_signal);
    tree->SetBranchAddress("is_qcd", &is_qcd);
    tree->SetBranchAddress("is_antiiso", &is_antiiso);
    tree->SetBranchAddress("is_looseIso", &is_looseIso);
    tree->SetBranchAddress("cat_0jet", &cat_0jet);
    tree->SetBranchAddress("cat_boosted", &cat_boosted);
    tree->SetBranchAddress("cat_vbf", &cat_vbf);
    tree->SetBranchAddress("cat_VH", &cat_VH);


    for (auto i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);

      if (eq + tq == 0) {
        // output histograms for the template
        if (is_antiiso > 0) {
          hists->hists.at(channel_prefix+"_inclusive").back()->Fill(el_pt, weight);
          if (cat_0jet > 0) {
            hists->hists.at(channel_prefix+"_0jet").back()->Fill(el_pt, weight);
          }
          if (cat_boosted > 0) {
            hists->hists.at(channel_prefix+"_boosted").back()->Fill(el_pt, weight);
          }
          if (cat_vbf > 0) {
            hists->hists.at(channel_prefix+"_vbf").back()->Fill(el_pt, weight);
          }
        }
      } else {
        // get QCD shape from SS loose iso region
        if (is_qcd > 0 || is_looseIso > 0) {
          if (cat_0jet) {
            fillQCD(hists->qcd_0jet, name, el_pt, weight);
          }
          if (cat_boosted) {
            fillQCD(hists->qcd_boosted, name, el_pt, weight);
          }
          if (cat_vbf > 0 && mt < 50 && nbjets == 0) {
            fillQCD(hists->qcd_vbf, name, el_pt, weight);
          }
        }

        // get SS in signal region for loose region normalization
        if (is_signal > 0) {
          if (cat_0jet > 0) {
            fillQCD(hists->qcd_0jet_SS, name, el_pt, weight);
          }
          if (cat_boosted > 0) {
            fillQCD(hists->qcd_boosted_SS, name, el_pt, weight);
          }
          if (cat_vbf > 0 && mt < 50 && nbjets == 0) {
            fillQCD(hists->qcd_vbf_SS, name, el_pt, weight);
          }
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
void fillQCD(TH1F *hist, std::string name, double var1, double weight) {
  if (name.find("Data") != std::string::npos) {
    hist->Fill(var1, weight);
  } else if (name == "embed" || name == "ZL" || name == "ZJ" || name == "TTT"  ||
             name == "TTJ"   || name == "W"  || name == "VV" || name == "EWKZ" || name == "ZTT") 
  {
    hist->Fill(var1, -1 * weight);
  }
}

// histHolder contructor to create the output file, the qcd histograms with the correct binning
// and the map from categories to vectors of TH1F*'s. Each TH1F* in the vector corresponds to
// one file that is being put into that categories directory in the output tempalte
histHolder::histHolder(std::string channel) : 
    hists{
        {(channel+"_inclusive").c_str(), std::vector<TH1F *>()},
        {(channel+"_0jet").c_str(), std::vector<TH1F *>()},
        {(channel+"_boosted").c_str(), std::vector<TH1F *>()},
        {(channel+"_vbf").c_str(), std::vector<TH1F *>()},
    },
    ch ( channel ),
    fout(new TFile(("Output/templates/FFratio_"+channel+"final.root").c_str(), "recreate")),
    // x-axis
    bins_l2 {0, 1, 10, 11},
    bins_hpt {0, 100, 150, 200, 250, 300, 5000},
    //bins_mjj {300, 700, 1100, 1500, 10000},
    bins_mjj {0., 0.1, 0.5, 0.9, 1.},

    // y-axis
    bins_lpt {0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400},
    bins_msv1 {0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300},
    bins_msv2 {0, 95, 115, 135, 155, 400}
{
    for (auto it = hists.begin(); it != hists.end(); it++) {
        fout->cd();
        fout->mkdir((it->first).c_str());
        fout->cd();
    }
    qcd_0jet       = new TH1F("qcd_0jet"      , "qcd_0jet"      , 1, -1, 10000);
    qcd_0jet_OS    = new TH1F("qcd_0jet_OS"   , "qcd_0jet_OS"   , 1, -1, 10000);
    qcd_0jet_SS    = new TH1F("qcd_0jet_SS"   , "qcd_0jet_SS"   , 1, -1, 10000);
    qcd_boosted    = new TH1F("qcd_boosted"   , "qcd_boosted"   , 1, -1, 10000);
    qcd_boosted_OS = new TH1F("qcd_boosted_OS", "qcd_boosted_OS", 1, -1, 10000);
    qcd_boosted_SS = new TH1F("qcd_boosted_SS", "qcd_boosted_SS", 1, -1, 10000);
    qcd_vbf        = new TH1F("qcd_vbf"       , "qcd_vbf"       , 1, -1, 10000);
    qcd_vbf_SS     = new TH1F("qcd_vbf_SS"    , "qcd_vbf_SS"    , 1, -1, 10000);
    qcd_vbf_OS     = new TH1F("qcd_vbf_OS"    , "qcd_vbf_OS"    , 1, -1, 10000);
}

histHolder::~histHolder() {}

// change to the correct output directory then create a new TH1F that will be filled for the current input file
void histHolder::initVectors(std::string name) {
    for (auto key : hists) {
        fout->cd(key.first.c_str());
        if (name.find("Data") != std::string::npos) {
            name = "data_obs";
        }
        if (key.first == ch+"_0jet") {
            hists.at(key.first.c_str()).push_back(new TH1F(name.c_str(), name.c_str(), 1, -1, 10000));
        } else if (key.first == ch+"_boosted") {                                                                          
            hists.at(key.first.c_str()).push_back(new TH1F(name.c_str(), name.c_str(), 1, -1, 10000));
        } else if (key.first == ch+"_vbf") {                                                                              
            hists.at(key.first.c_str()).push_back(new TH1F(name.c_str(), name.c_str(), 1, -1, 10000));
        } else if (key.first == ch+"_inclusive") {                                                                              
            hists.at(key.first.c_str()).push_back(new TH1F(name.c_str(), name.c_str(), 1, -1, 10000));
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

  fout->cd((ch+"_0jet").c_str());
  qcd_0jet->SetName("QCD");
  qcd_0jet->Scale(1.0 * qcd_0jet_SS->Integral() / qcd_0jet->Integral());
  for (auto i = 0; i < qcd_0jet->GetNbinsX(); i++) {
    if (qcd_0jet->GetBinContent(i) < 0) {
      qcd_0jet->SetBinContent(i, 0);
    }
  }
  qcd_0jet->Write();

  fout->cd((ch+"_boosted").c_str());
  qcd_boosted->SetName("QCD");
  qcd_boosted->Scale(1.28 * qcd_boosted_SS->Integral() / qcd_boosted->Integral());
  for (auto i = 0; i < qcd_boosted->GetNbinsX(); i++) {
    if (qcd_boosted->GetBinContent(i) < 0) {
      qcd_boosted->SetBinContent(i, 0);
    }
  }
  qcd_boosted->Write();

  fout->cd((ch+"_vbf").c_str());
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