#ifndef INCLUDE_ACWEIGHTER_H_
#define INCLUDE_ACWEIGHTER_H_

#include <map>
#include <string>
#include <vector>
#include "TFile.h"
#include "TTree.h"

///////////////////////////////////////////////
// YM AC reweighing                          //
///////////////////////////////////////////////
// a1 - SM                                   //
// a3 - pseudoscalar (CP-odd)                //
// a2 - SM like heavy mass anomalous Higgs   //
// L1, L1Zg - something like a2, not sure    //
// int - interference term (f05)             //
///////////////////////////////////////////////
class ACWeighter {
 public:
    explicit ACWeighter(std::string);
    ~ACWeighter();

    void fillWeightMap();
    std::vector<double> getWeights(Long64_t);

 private:
    Long64_t eventID;
    TTree *weightTree;
    TFile *weightTreeFile;
    std::string ac_prefix;
    std::string fileName = "";
    unsigned int numWeightFiles;
    int foundEvents, crapEvents;  // hehe
    bool isVBFAC, isggHAC, isWHAC, isZHAC;
    std::vector<std::string> weightNames;
    std::map<Long64_t, std::vector<double>> acWeights;

    // variables with weights (for ggH only a1 (SM), a3 (CP-odd), and maxmix (int) is used
    Double_t wt_a1, wt_a2, wt_a3, wt_L1, wt_L1Zg, wt_a2int, wt_a3int, wt_L1int, wt_L1Zgint;
};

ACWeighter::ACWeighter(std::string sample) :
  foundEvents(0),
  crapEvents(0),
  numWeightFiles(7),
  weightNames {
    "a1", "a3", "a3int",
    "a2", "a2int",
    "L1", "L1int"
    // "L1Zg", "L1Zgint" <- for 2017, there will be two additional samples (L1Zg and L1Zgint)
  } {
  isVBFAC = sample.find("vbf_ac") != std::string::npos;
  isggHAC = sample.find("ggh_ac") != std::string::npos;
  isWHAC = sample.find("wh_ac") != std::string::npos;
  isZHAC = sample.find("zh_ac") != std::string::npos;

  // get the correct prefix
  if (isVBFAC) {
    ac_prefix = "vbf_ac_";
  } else if (isggHAC) {
    ac_prefix = "ggh_ac_";
  } else if (isWHAC) {
    ac_prefix = "wh_ac_";
  } else if (isZHAC) {
    ac_prefix = "zh_ac_";
  }

  // tack prefix on to the beginning of the name
  for (auto &name : weightNames) {
    name = ac_prefix + name;
  }

  // ggH AC only has 3 files
  if (isggHAC) {
    numWeightFiles = 3;
  }

  if (isggHAC || isWHAC || isZHAC || isVBFAC) {
    for (unsigned int ifile = 0; ifile != numWeightFiles; ++ifile) {
      // is it interference sample? if yes, skip the weightsNames that do not have int in them
      if (sample.find("int") != std::string::npos &&
          weightNames.at(ifile).find("int") == std::string::npos) {
        continue;
      }

      // if it is not an interference sample, skip weightsNames that have int in them
      if (sample.find("int") == std::string::npos &&
          weightNames.at(ifile).find("int") != std::string::npos) {
        continue;
      }

      if (sample.find(weightNames.at(ifile)) != std::string::npos) {
        fileName = "data/AC_weights/" + weightNames[ifile] + ".root";
        break;
      }
    }
  }

  // set the branches
  if (fileName != "") {
    weightTreeFile = TFile::Open(fileName.c_str());
    weightTree = reinterpret_cast<TTree *>(weightTreeFile->Get("weights"));
    weightTree->SetBranchAddress("eventID", &eventID);
    weightTree->SetBranchAddress("wt_a1", &wt_a1);
    weightTree->SetBranchAddress("wt_a3", &wt_a3);
    weightTree->SetBranchAddress("wt_a3int", &wt_a3int);
    if (isVBFAC || isWHAC || isZHAC) {
      weightTree->SetBranchAddress("wt_a2", &wt_a2);
      weightTree->SetBranchAddress("wt_a2int", &wt_a2int);
      weightTree->SetBranchAddress("wt_L1", &wt_L1);
      weightTree->SetBranchAddress("wt_L1int", &wt_L1int);
      weightTree->SetBranchAddress("wt_L1Zg", &wt_L1Zg);
      weightTree->SetBranchAddress("wt_L1Zgint", &wt_L1Zgint);
    }
  }
}

void ACWeighter::fillWeightMap() {
  if (isVBFAC || isggHAC || isWHAC || isZHAC) {
    for (auto i = 0; i < weightTree->GetEntries(); ++i) {
      weightTree->GetEntry(i);
      std::vector<double> w;
      if (isVBFAC) {
        w.push_back(wt_a1);
        w.push_back(wt_a2);
        w.push_back(wt_a3);
        w.push_back(wt_L1);
        w.push_back(wt_L1Zg);
        w.push_back(wt_a2int);
        w.push_back(wt_a3int);
        w.push_back(wt_L1int);
        w.push_back(wt_L1Zgint);
      } else {
        for (auto j = 0; j != 9; ++j) {
          w.push_back(0);
        }
      }

      if (isggHAC) {
        w.push_back(wt_a1);
        w.push_back(wt_a3);
        w.push_back(wt_a3int);
      } else {
        for (auto j = 0; j != 3; ++j) {
          w.push_back(0);
        }
      }

      if (isWHAC) {
        w.push_back(wt_a1);
        w.push_back(wt_a2);
        w.push_back(wt_a3);
        w.push_back(wt_L1);
        w.push_back(wt_L1Zg);
        w.push_back(wt_a2int);
        w.push_back(wt_a3int);
        w.push_back(wt_L1int);
        w.push_back(wt_L1Zgint);
      } else {
        for (auto j = 0; j != 9; ++j) {
          w.push_back(0);
        }
      }

      if (isZHAC) {
        w.push_back(wt_a1);
        w.push_back(wt_a2);
        w.push_back(wt_a3);
        w.push_back(wt_L1);
        w.push_back(wt_L1Zg);
        w.push_back(wt_a2int);
        w.push_back(wt_a3int);
        w.push_back(wt_L1int);
        w.push_back(wt_L1Zgint);
      } else {
        for (auto j = 0; j != 9; ++j) {
          w.push_back(0);
        }
      }

      // total size of weights is 9 + 3 + 9 + 9 = 30 weights!
      acWeights[eventID] = w;
    }  // map of weights is complete
  }
}

std::vector<double> ACWeighter::getWeights(Long64_t currentEventID) {
  std::vector<double> weights(30, 0);
  auto it = acWeights.find(currentEventID);
  if (it != acWeights.end()) {
    weights = it->second;
  }
}


ACWeighter::~ACWeighter() {
}

#endif  // INCLUDE_ACWEIGHTER_H_
