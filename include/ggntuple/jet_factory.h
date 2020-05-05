// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_JET_FACTORY_H_
#define INCLUDE_GGNTUPLE_JET_FACTORY_H_

#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TTree.h"
#include "../models/jet.h"

class jet_factory {
 private:
    Int_t nJet;
    Float_t mjj;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *csv_b, *csv_bb, *flavor, *id;
    std::vector<jet> plain_jets, btag_jets;

 public:
    jet_factory(TTree *, int, std::string);
    virtual ~jet_factory() {}
    void run_factory();

    // getters
    Float_t getNbtag() { return btag_jets.size(); }
    Float_t getNjets() { return plain_jets.size(); }
    Float_t getDijetMass() { return mjj; }
    // Float_t getTopPt1() { return topQuarkPt1; }
    // Float_t getTopPt2() { return topQuarkPt2; }
    // Float_t getBWeight() { return bweight; }
    std::vector<jet> getJets() { return plain_jets; }
    std::vector<jet> getBtagJets() { return btag_jets; }
};

// read data from tree into member variables
jet_factory::jet_factory(TTree *input, int era, std::string syst) {
    input->SetBranchAddress("nJet", &nJet);
    input->SetBranchAddress("jetPt", &pt);
    input->SetBranchAddress("jetEta", &eta);
    input->SetBranchAddress("jetPhi", &phi);
    input->SetBranchAddress("jetEn", &energy);
    input->SetBranchAddress("jetHadFlvr", &flavor);
    input->SetBranchAddress("jetDeepCSVTags_b", &csv_b);
    input->SetBranchAddress("jetDeepCSVTags_bb", &csv_bb);
    input->SetBranchAddress("jetID", &id);
}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
    plain_jets.clear();
    btag_jets.clear();

    for (auto i = 0; i < nJet; i++) {
        auto j = jet(pt->at(i), eta->at(i), phi->at(i), csv_b->at(i), flavor->at(i));
        j.setID(id->at(i));
        if (csv_b->at(i) + csv_bb->at(i) >  0.6321) {  // cut taken from FSA
            btag_jets.push_back(j);
        } else {
            plain_jets.push_back(j);
        }
    }

    // example of calculating in the class
    // calculated every event and stored in mjj variable to be retrieved
    if (plain_jets.size() > 1) {
        mjj = (plain_jets.at(0).getP4() + plain_jets.at(1).getP4()).M();
    }
}

#endif  // INCLUDE_GGNTUPLE_JET_FACTORY_H_
