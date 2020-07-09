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
    Bool_t is_data;
    Int_t nJet;
    Float_t mjj;
    std::vector<Bool_t> *loose_id;
    std::vector<Float_t> *pt, *eta, *phi, *energy, *csv_b, *csv_bb, *flavor, *id, *mva_csv;
    std::vector<jet> plain_jets, btag_jets, all_jets, cleaned_jets;

   public:
    jet_factory(TTree *, Int_t, Bool_t, std::string);
    virtual ~jet_factory() {}
    void run_factory();

    // getters
    Float_t getNbtag() { return btag_jets.size(); }
    Float_t getNjets() { return all_jets.size(); }
    Float_t getDijetMass() { return mjj; }
    Float_t getHT(Float_t, TLorentzVector, TLorentzVector);
    Float_t getST(Float_t);
    // Float_t getTopPt1() { return topQuarkPt1; }
    // Float_t getTopPt2() { return topQuarkPt2; }
    // Float_t getBWeight() { return bweight; }
    std::vector<jet> getJets() { return all_jets; }
    std::vector<jet> getBtagJets() { return btag_jets; }
    std::vector<jet> getCleanedJets() { return cleaned_jets; }
    std::vector<jet> clean_jets(TLorentzVector, std::vector<jet>);
    void set_cleaned_jets(std::vector<jet> cleaned) { cleaned_jets = cleaned; }
};

// read data from tree into member variables
jet_factory::jet_factory(TTree *input, Int_t era, Bool_t _is_data, std::string syst)
    : is_data(_is_data),
      pt(nullptr),
      eta(nullptr),
      phi(nullptr),
      energy(nullptr),
      flavor(nullptr),
      csv_b(nullptr),
      csv_bb(nullptr),
      mva_csv(nullptr),
      id(nullptr),
      loose_id(nullptr) {
    input->SetBranchAddress("nJet", &nJet);
    input->SetBranchAddress("jetPt", &pt);
    input->SetBranchAddress("jetEta", &eta);
    input->SetBranchAddress("jetPhi", &phi);
    input->SetBranchAddress("jetEn", &energy);
    input->SetBranchAddress("jetDeepCSVTags_b", &csv_b);
    input->SetBranchAddress("jetDeepCSVTags_bb", &csv_bb);
    input->SetBranchAddress("jetCSV2BJetTags", &mva_csv);
    input->SetBranchAddress("jetID", &id);
    input->SetBranchAddress("jetPFLooseId", &loose_id);
    if (!is_data) {
        input->SetBranchAddress("jetHadFlvr", &flavor);
    }
}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
    all_jets.clear();
    plain_jets.clear();
    btag_jets.clear();
    Float_t jet_flavor(-1);

    for (auto i = 0; i < nJet; i++) {
        jet_flavor = is_data ? -1 : flavor->at(i);
        auto j = jet(pt->at(i), eta->at(i), phi->at(i), csv_b->at(i), jet_flavor);
        j.setID(id->at(i));
        j.setLooseID(loose_id->at(i));

        all_jets.push_back(j);
        // if (csv_b->at(i) + csv_bb->at(i) > 0.6321) {  // cut taken from FSA
        if (loose_id->at(i) > 0.5 && pt->at(i) > 20 && fabs(eta->at(i)) < 2.4 && mva_csv->at(i) > 0.8838) {
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
    cleaned_jets = all_jets;
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

std::vector<jet> jet_factory::clean_jets(TLorentzVector lep, std::vector<jet> collection) {
    std::vector<jet> cleaned;
    for (auto &jet : collection) {
        if (jet.getP4().DeltaR(lep) < 0.5) {  // possible other selection as well
            continue;
        }
        cleaned.push_back(jet);
    }
    return cleaned;
}

#endif  // INCLUDE_GGNTUPLE_JET_FACTORY_H_
