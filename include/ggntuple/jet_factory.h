// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_JET_FACTORY_H_
#define INCLUDE_GGNTUPLE_JET_FACTORY_H_

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/jet.h"
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TTree.h"

class jet_factory {
   private:
    Int_t nJet;
    Float_t mjj;
    std::vector<Bool_t> *loose_id;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *csv_b, *csv_bb, *flavor, *id;
    std::vector<jet> plain_jets, btag_jets, all_jets;

   public:
    jet_factory(TTree *, int, std::string);
    virtual ~jet_factory() {}
    void run_factory();

    // getters
    Float_t getNbtag() { return btag_jets.size(); }
    Float_t getNjets() { return plain_jets.size(); }
    Float_t getDijetMass() { return mjj; }
    Float_t getHT(Float_t, TLorentzVector, TLorentzVector);
    Float_t getST(Float_t);
    // Float_t getTopPt1() { return topQuarkPt1; }
    // Float_t getTopPt2() { return topQuarkPt2; }
    // Float_t getBWeight() { return bweight; }
    std::vector<jet> getJets() { return plain_jets; }
    std::vector<jet> getBtagJets() { return btag_jets; }
};

// read data from tree into member variables
jet_factory::jet_factory(TTree *input, int era, std::string syst)
    : pt(nullptr), eta(nullptr), phi(nullptr), energy(nullptr), flavor(nullptr), csv_b(nullptr), csv_bb(nullptr), id(nullptr), loose_id(nullptr) {
    input->SetBranchAddress("nJet", &nJet);
    input->SetBranchAddress("jetPt", &pt);
    input->SetBranchAddress("jetEta", &eta);
    input->SetBranchAddress("jetPhi", &phi);
    input->SetBranchAddress("jetEn", &energy);
    input->SetBranchAddress("jetHadFlvr", &flavor);
    input->SetBranchAddress("jetDeepCSVTags_b", &csv_b);
    input->SetBranchAddress("jetDeepCSVTags_bb", &csv_bb);
    input->SetBranchAddress("jetID", &id);
    input->SetBranchAddress("jetPFLooseId", &loose_id);
}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
    all_jets.clear();
    plain_jets.clear();
    btag_jets.clear();

    for (auto i = 0; i < nJet; i++) {
        auto j = jet(pt->at(i), eta->at(i), phi->at(i), csv_b->at(i), flavor->at(i));
        j.setID(id->at(i));
        j.setLooseID(loose_id->at(i));

        all_jets.push_back(j);
        if (csv_b->at(i) + csv_bb->at(i) > 0.6321) {  // cut taken from FSA
            btag_jets.push_back(j);
        } else {
            plain_jets.push_back(j);
        }
    }

    // example of calculating in the class
    // calculated every event and stored in mjj variable to be retrieved
    if (all_jets.size() > 1) {
        mjj = (all_jets.at(0).getP4() + all_jets.at(1).getP4()).M();
    }
}

Float_t jet_factory::getHT(Float_t pt, TLorentzVector lep, TLorentzVector tt) {
    Float_t ht(0.);
    for (auto jet : all_jets) {
        // remove overlap
        if (jet.getP4().DeltaR(lep) < 0.1 || jet.getP4().DeltaR(tt) < 0.1) {
            continue;
        }

        // apply selection and calculate ht
        if (jet.getLooseID() > 0.5 && jet.getPt() > pt && fabs(jet.getEta()) < 3.0) {
            ht += jet.getPt();
        }
    }
    return ht;
}

Float_t jet_factory::getST(Float_t pt) {
    Float_t st(0.);
    for (auto jet : all_jets) {
        // apply selection and calculate st
        if (jet.getLooseID() > 0.5 && jet.getPt() > pt && fabs(jet.getEta()) < 3.0) {
            st += jet.getPt();
        }
    }
    return st;
}

#endif  // INCLUDE_GGNTUPLE_JET_FACTORY_H_
