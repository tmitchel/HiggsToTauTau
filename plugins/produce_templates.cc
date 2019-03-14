// Copyright 2019 Tyler Mitchell

#include <memory>

// user includes
#include "../include/CLParser.h"
#include "../include/Sample.h"
#include "TMath.h"
#include "TStopwatch.h"

using std::string;
using std::vector;

int main(int argc, char *argv[]) {
  auto watch = TStopwatch();
  // get CLI arguments
  CLParser parser(argc, argv);
  bool doSyst = parser.Flag("-s");
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

  // read all files from input directory
  vector<string> files;
  read_directory(dir, &files);
  auto fout = std::make_shared<TFile>(("Output/templates/" + channel_prefix + year + "_" + suffix + ".root").c_str(), "recreate");
  auto info = std::make_unique<TemplateTool>(channel_prefix);
  for (auto cat : info->get_categories()) {
    // make directory in TFile
    fout->cd();
    fout->mkdir(cat.c_str());
  }
  fout->cd();

  for (auto ifile : files) {
    // initialize histogram holder
    auto name = ifile.substr(0, ifile.find("."));
    std::cout << name << std::endl;
    // open the file
    auto fin = std::unique_ptr<TFile>(TFile::Open((dir + "/" + ifile).c_str()));

    // run for nominal case first
    auto tree = std::shared_ptr<TTree>(reinterpret_cast<TTree *>(fin->Get(tree_name.c_str())));
    auto sample = std::make_unique<Sample>(channel_prefix, year, name, suffix, fout);
    sample->load_fake_fractions(ff_name);
    sample->fill_histograms(tree, doSyst);
    sample->write_histograms(doSyst);
    sample->Close();

    // start by just printing all the keys in the file.
    for (auto key : fin->GetListOfKeys()) {
      std::cout << key->GetName() << std::endl;
    }

    // AC reweighting for JHU samples
    if (ifile.find("_inc.root") != std::string::npos) {
      auto ac_weights = info->get_AC_weights(ifile);
      for (auto ac_weight : ac_weights) {
        auto jhu_sample = std::make_unique<Sample>(channel_prefix, year, ac_weight.second, suffix, fout);
        jhu_sample->load_fake_fractions(ff_name);
        auto fin = std::unique_ptr<TFile>(TFile::Open((dir + "/" + ifile).c_str()));
        auto tree = std::shared_ptr<TTree>(reinterpret_cast<TTree *>(fin->Get(tree_name.c_str())));
        jhu_sample->fill_histograms(tree, doSyst, ac_weight.first);
        jhu_sample->write_histograms(doSyst, ac_weight.second);
        jhu_sample->Close();
      }
    }
  }
}
