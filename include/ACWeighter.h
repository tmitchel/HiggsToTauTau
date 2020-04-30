// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_ACWEIGHTER_H_
#define INCLUDE_ACWEIGHTER_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TFile.h"
#include "TTree.h"

using std::string;

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
    explicit ACWeighter(string, string, string, string);
    ~ACWeighter();

    void fillWeightMap();
    std::vector<double> getWeights(Long64_t);

 private:
    bool notSignal;
    Long64_t eventID;
    TTree *weightTree;
    TFile *weightTreeFile;
    string ac_prefix;
    string fileName = "/hdfs/store/user/tmitchel/HTT_AC_weights/";
    string signal_type;
    unsigned int numWeightFiles;
    int foundEvents, crapEvents;  // hehe
    bool isVBFAC, isggHAC, isWHAC, isZHAC;
    std::vector<string> weightNames;
    std::map<Long64_t, std::vector<double>> acWeights;

    // variables with weights (for ggH only a1 (SM), a3 (CP-odd), and maxmix (int) is used
    Double_t wt_a1, wt_a2, wt_a3, wt_L1, wt_L1Zg, wt_a2int, wt_a3int, wt_L1int, wt_L1Zgint;
};

ACWeighter::ACWeighter(string original, string sample, string _signal_type, string year)
    : notSignal(false),
      isVBFAC(false),
      isggHAC(false),
      isWHAC(false),
      isZHAC(false),
      foundEvents(0),
      crapEvents(0),
      numWeightFiles(7),
      weightNames{"a1", "a3", "a3int", "a2", "a2int", "l1", "l1int"},
      signal_type(_signal_type) {
    isVBFAC = sample == "vbf125";
    isggHAC = sample == "ggh125";
    isWHAC = (sample == "wplus125" || sample == "wminus125" || sample == "wh125");
    isZHAC = sample == "zh125";

    string stype_dir = "";
    if (signal_type == "madgraph" && year == "2016") {
        notSignal = true;
    } else if (signal_type == "madgraph" && year == "2017") {
        stype_dir = "/MG2017_X10_v2/";
    } else if (signal_type == "madgraph" && year == "2018") {
        notSignal = true;
    } else if (signal_type == "JHU" && year == "2016") {
        stype_dir = "/JHU2016/";
    } else if (signal_type == "JHU" && year == "2017") {
        stype_dir = "/JHU2017/";
    } else if (signal_type == "JHU" && year == "2018") {
        stype_dir = "/JHU2018/";
    } else {
        notSignal = true;
    }

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

    // ggH AC only has 3 files
    if (isggHAC) {
        weightNames = {"a1", "a3", "a3int"};
    }

    // 2017 has extra weights
    if ((year == "2018" || year == "2017") && isVBFAC) {
        weightNames.push_back("l1zg");
        weightNames.push_back("l1zgint");
    } else if (year == "2018" && isZHAC) {
        weightNames.push_back("l1zg");
        weightNames.push_back("l1zgint");
    }

    std::transform(sample.begin(), sample.end(), sample.begin(), ::tolower);

    if (isggHAC || isWHAC || isZHAC || isVBFAC) {
        if (original.find("a1-prod") != string::npos || original.find("nominal") != string::npos) {
            fileName += stype_dir + ac_prefix + "a1.root";
        } else if (original.find("a3-prod") != string::npos || original.find("ps_decay") != string::npos) {
            fileName += stype_dir + ac_prefix + "a3.root";
        } else if (original.find("a3int-prod") != string::npos || original.find("maxmix_prod") != string::npos) {
            fileName += stype_dir + ac_prefix + "a3int.root";
        } else if (original.find("a2-prod") != string::npos && !isggHAC) {
            fileName += stype_dir + ac_prefix + "a2.root";
        } else if (original.find("a2int-prod") != string::npos) {
            fileName += stype_dir + ac_prefix + "a2int.root";
        } else if (original.find("l1-prod") != string::npos && !isggHAC) {
            fileName += stype_dir + ac_prefix + "L1.root";
        } else if (original.find("l1int-prod") != string::npos && !isggHAC) {
            fileName += stype_dir + ac_prefix + "L1int.root";
        } else if (original.find("l1zg-prod") != string::npos && !isggHAC) {
            fileName += stype_dir + ac_prefix + "L1Zg.root";
        } else if (original.find("l1zgint-prod") != string::npos && !isggHAC) {
            fileName += stype_dir + ac_prefix + "L1Zgint.root";
        } else {
            notSignal = true;
        }
    }

    // set the branches
    if (!notSignal) {
        weightTreeFile = TFile::Open(fileName.c_str());
        weightTree = reinterpret_cast<TTree *>(weightTreeFile->Get("weights"));

        weightTree->SetBranchAddress("eventID", &eventID);
        weightTree->SetBranchAddress("wt_a1", &wt_a1);
        weightTree->SetBranchAddress("wt_a3", &wt_a3);
        weightTree->SetBranchAddress("wt_a3int", &wt_a3int);
        if (!isggHAC) {
            weightTree->SetBranchAddress("wt_L1", &wt_L1);
            weightTree->SetBranchAddress("wt_L1Zg", &wt_L1Zg);
            weightTree->SetBranchAddress("wt_a2", &wt_a2);
            weightTree->SetBranchAddress("wt_a2int", &wt_a2int);
            weightTree->SetBranchAddress("wt_L1int", &wt_L1int);
            weightTree->SetBranchAddress("wt_L1Zgint", &wt_L1Zgint);
        }
    }
}

void ACWeighter::fillWeightMap() {
    if ((isVBFAC || isggHAC || isWHAC || isZHAC) && !notSignal) {
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
    } else if (notSignal) {
      return weights;
    } else {
      std::cerr << "Unable to find event " << currentEventID << std::endl;
      throw;
    }
    return weights;
}

ACWeighter::~ACWeighter() {}

#endif  // INCLUDE_ACWEIGHTER_H_
