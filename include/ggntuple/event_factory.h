// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_GGNTUPLE_EVENT_FACTORY_H_
#define INCLUDE_GGNTUPLE_EVENT_FACTORY_H_

#include <unordered_map>
#include <string>
#include <vector>
#include "TTree.h"
#include "../models/defaults.h"
#include "../swiss_army_class.h"
#include "../qq2Hqq_uncert_scheme.h"

/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_factory {
 private:
    Long64_t evt;
    ULong64_t HLTEleMuX;
    Int_t run, lumi;
    Float_t genWeight;
    Float_t m_sv, pt_sv;
    std::string syst;

    bool isEmbed;
    int era;
    lepton lep;

 public:
    event_factory(TTree*, lepton, int, bool, std::string);
    virtual ~event_factory() {}

    // Event Info
    ULong64_t getEvt() { return evt; }
    UInt_t getRun() { return run; }
    UInt_t getLumi() { return lumi; }
    Float_t getGenWeight() { return genWeight; }
    Float_t getMSV() { return m_sv; }
    Float_t getPtSV() { return pt_sv; }
    Bool_t fire_trigger(trigger t) { return (HLTEleMuX >> t & 1); }
    Bool_t getPassFlags(Bool_t) { return true; }  // TODO(tyler): implement
    Float_t getPrefiringWeight() { return 1.; }  // TODO(tyler): implement
    Float_t getNPU() { return 1.; }  // TODO(tyler): implement

};

// read data from trees into member variables
event_factory::event_factory(TTree* input, lepton _lep, int _era, bool isMadgraph, std::string _syst) :
    isEmbed(false),
    era(_era),
    syst(_syst),
    lep(_lep) {
    input->SetBranchAddress("event", &evt);
    input->SetBranchAddress("run", &run);
    input->SetBranchAddress("lumis", &lumi);
    input->SetBranchAddress("genWeight", &genWeight);
    input->SetBranchAddress("m_sv", &m_sv);
    input->SetBranchAddress("pt_sv", &pt_sv);
    input->SetBranchAddress("HLTEleMuX", &HLTEleMuX);
}

#endif  // INCLUDE_GGNTUPLE_EVENT_FACTORY_H_
