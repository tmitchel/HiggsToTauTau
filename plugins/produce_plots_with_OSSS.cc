// Copyright 2019 Tyler Mitchell

#include <memory>

// user includes
#include "../include/CLParser.h"
#include "../include/produce_plots_with_OSSS.h"
#include "TMath.h"
#include "TStopwatch.h"

using std::string;
using std::vector;

int main(int argc, char *argv[]) {
  auto watch = TStopwatch();
  // get CLI arguments
  CLParser parser(argc, argv);
  string dir = parser.Option("-d");
  string year = parser.Option("-y");
  string suffix = parser.Option("--suf");
  string tree_name = parser.Option("-t");
  string ff_name = parser.Option("-f");

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

  // variables to plot
  std::map<std::string, std::vector<float>> vars = {
      // {"trigger", {4, -0.5, 3.5}},
      {"t1_decayMode", {11, -0.5, 10.5}},
      {"m_sv", {30, 50, 180}},
      {"t1_pt", {30, 30, 200}},
      {"el_pt", {30, 30, 200}},
      {"mu_pt", {30, 30, 200}},
      {"j1_pt", {30, 30, 200}},
      {"j2_pt", {30, 30, 200}},
      {"njets", {10, -0.5, 9.5}},
      {"met", {30, 0, 500}},
      {"higgs_pT", {30, 0, 300}},
      // {"NN_disc", {25, 0, 1}},
      {"mjj", {25, 300, 1000}}};
      // {"D0_ggH", {25, 0, 1}},
      // {"D0_VBF", {25, 0, 1}},
      // {"MELA_D2j", {25, 0, 1}},
      // {"Q2V1", {30, 0, 20000}},
      // {"Q2V2", {30, 0, 20000}},
      // {"Phi", {30, -3.14, 3.14}},
      // {"Phi1", {30, -3.14, 3.14}},
      // {"costheta1", {30, -1, 1}},
      // {"costheta2", {30, -1, 1}},
      // {"costhetastar", {30, -1, 1}},
      // {"Dbkg_VBF", {30, 0, 1}},
      // {"Dbkg_ggH", {30, 0, 1}}};

  // read all files from input directory
  vector<string> files;
  read_directory(dir, &files);

  // make output file and TemplateTool containing useful information
  auto fout = std::make_shared<TFile>(("Output/histos/" + channel_prefix + year + "_" + suffix + ".root").c_str(), "recreate");
  auto info = std::make_unique<TemplateTool>(channel_prefix);

  // make all of the TDirectoryFiles we need
  fout->cd();
  fout->mkdir("plots");
  for (auto it = vars.begin(); it != vars.end(); it++) {
    fout->mkdir(("plots/" + it->first).c_str());
    for (auto cat : info->get_categories()) {
      fout->mkdir(("plots/" + it->first + "/" + cat).c_str());
      fout->mkdir(("plots/" + it->first + "/SS_iso_" + cat).c_str());
      fout->mkdir(("plots/" + it->first + "/SS_anti_" + cat).c_str());
    }
  }
  fout->cd();

  // loop through all files in the directory
  for (auto ifile : files) {
    // get the sample name
    auto name = ifile.substr(0, ifile.find("."));
    std::cout << name << std::endl;

    // open the file
    auto fin = std::unique_ptr<TFile>(TFile::Open((dir + "/" + ifile).c_str()));

    // run for nominal case first
    auto tree = std::shared_ptr<TTree>(reinterpret_cast<TTree *>(fin->Get(tree_name.c_str())));    // open TTree
    auto sample = std::make_unique<Sample_Plots>(channel_prefix, year, name, suffix, fout, vars);  // create Sample_Plots
    sample->fill_histograms(tree);                                                                 // do event loop and fill histos
    sample->write_histograms();                                                                    // write all to the file
    sample->Close();                                                                               // delete the ff_weight pointer

    // AC reweighting for JHU samples only
    if (ifile.find("_inc.root") != std::string::npos) {
      auto ac_weights = info->get_AC_weights(ifile);
      for (auto ac_weight : ac_weights) {
        auto fin = std::unique_ptr<TFile>(TFile::Open((dir + "/" + ifile).c_str()));
        auto tree = std::shared_ptr<TTree>(reinterpret_cast<TTree *>(fin->Get(tree_name.c_str())));
        auto jhu_sample = std::make_unique<Sample_Plots>(channel_prefix, year, ac_weight.second, suffix, fout, vars);
        jhu_sample->fill_histograms(tree, ac_weight.first);
        jhu_sample->write_histograms();
        jhu_sample->Close();
      }
    }
  }
  std::cout << "Plots created.\n Timing Info: \n\t CPU Time: " << watch.CpuTime() << "\n\tReal Time: " << watch.RealTime() << std::endl;
}
