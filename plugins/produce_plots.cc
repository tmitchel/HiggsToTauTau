// Copyright 2019 Tyler Mitchell

#include <memory>

// user includes
#include "../include/CLParser.h"
#include "../include/sample_plots.h"
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
      {"m_sv", {30, 50, 180}},
      // {"mu_pt", {30, 30, 200}},
      // {"t1_pt", {30, 30, 200}},
      // {"met", {30, 0, 500}},
      // {"lt_dphi", {30, 0, 3.14}},
      // {"higgs_pT", {30, 0, 300}},
      // {"MT_lepMET", {30, 0, 55}},
      // {"MT_HiggsMET", {30, 0, 300}},
      // {"hj_dphi", {30, 0, 3.14}},
      // {"jmet_dphi", {30, 0, 3.14}},
      // {"MT_t2MET", {30, 0, 300}},
      // {"hj_deta", {30, 0, 5}},
      // {"hmet_dphi", {30, 0, 3.14}},
      // {"hj_dr", {30, 0, 5}}
      // {"NN_disc_boost", {25, 0, 1}},
      {"Dbkg_VBF", {30, 0, 1}},
      {"Dbkg_ggH", {30, 0, 1}}};

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
    fout->cd("plots");
    fout->mkdir(it->first.c_str());
    for (auto cat : info->get_categories()) {
      fout->cd(("plots/" + it->first).c_str());
      fout->mkdir(cat.c_str());
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
    auto tree = std::shared_ptr<TTree>(reinterpret_cast<TTree *>(fin->Get(tree_name.c_str())));  // open TTree
    auto sample = std::make_unique<Sample_Plots>(channel_prefix, year, name, suffix, fout, vars.at("m_sv"));      // create Sample_Plots
    sample->load_fake_fractions(ff_name);                                                        // load fractions from input file
    sample->set_variable(tree, "m_sv");
    sample->fill_histograms(tree);                                                               // do event loop and fill histos
    sample->write_histograms();                                                                  // write all to the file
    sample->Close();                                                                             // delete the ff_weight pointer

    // AC reweighting for JHU samples only
    if (ifile.find("_inc.root") != std::string::npos) {
      auto ac_weights = info->get_AC_weights(ifile);
      for (auto ac_weight : ac_weights) {
        auto fin = std::unique_ptr<TFile>(TFile::Open((dir + "/" + ifile).c_str()));
        auto tree = std::shared_ptr<TTree>(reinterpret_cast<TTree *>(fin->Get(tree_name.c_str())));
        auto jhu_sample = std::make_unique<Sample_Plots>(channel_prefix, year, ac_weight.second, suffix, fout, vars.at("m_sv"));
        jhu_sample->load_fake_fractions(ff_name);
        jhu_sample->set_variable(tree, "m_sv");
        jhu_sample->fill_histograms(tree, ac_weight.first);
        jhu_sample->write_histograms();
        jhu_sample->Close();
      }
    }
  }
}
