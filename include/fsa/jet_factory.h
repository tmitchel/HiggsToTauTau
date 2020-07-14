// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_FSA_JET_FACTORY_H_
#define INCLUDE_FSA_JET_FACTORY_H_

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/defaults.h"
#include "../models/jet.h"
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TTree.h"

class jet_factory {
   private:
    std::string channel;
    Float_t mjj;
    Float_t jpt_1, jeta_1, jphi_1, jcsv_1;
    Float_t jpt_2, jeta_2, jphi_2, jcsv_2;
    Float_t bpt_1, beta_1, bphi_1, bcsv_1, bflavor_1;
    Float_t bpt_2, beta_2, bphi_2, bcsv_2, bflavor_2;
    Float_t topQuarkPt1, topQuarkPt2, temp_njets;
    Float_t Nbtag, njetspt20, njets, nbtag_loose, nbtag_medium;
    Int_t nbtag;
    Float_t bweight;
    std::vector<jet> plain_jets, btag_jets;
    std::unordered_map<std::string, std::string> syst_name_map;

   public:
    jet_factory(TTree *, int, std::string);
    virtual ~jet_factory() {}
    void run_factory();
    std::string fix_syst_string(std::string);

    // getters
    Float_t getNbtag(wps);
    Float_t getNjets() { return njets; }
    Int_t getNjetPt20() { return njetspt20; }
    Float_t getDijetMass() { return mjj; }
    Float_t getTopPt1() { return topQuarkPt1; }
    Float_t getTopPt2() { return topQuarkPt2; }
    Float_t getBWeight() { return bweight; }
    std::vector<jet> getJets() { return plain_jets; }
    std::vector<jet> getBtagJets() { return btag_jets; }
};

// read data from tree into member variables
jet_factory::jet_factory(TTree *input, int era, std::string syst)
    : channel(input->GetName()),
      syst_name_map{
          {"JetRelSam_Up", "JetRelativeSampleUp"},
          {"JetRelSam_Down", "JetRelativeSampleDown"},
          {"JetRelBal_Up", "JetRelativeBalUp"},
          {"JetRelBal_Down", "JetRelativeBalDown"},
          {"JetJER_Up", "JERUp"},
          {"JetJER_Down", "JERDown"},
      } {
    // Check name and change vars
    std::string mjj_name, njets_name;
    if (input->GetName() == "tt_tree") {
        mjj_name = "vbfMass";
        njets_name = "jetVeto30";
    } else {
        mjj_name = "mjj";
        njets_name = "njets";
    }

    if (era == 2017) {
        mjj_name += "WoNoisyJets";
        njets_name += "WoNoisyJets";
    }

    auto end = std::string::npos;
    if (syst.find("Jet") != end) {
        auto syst_name = fix_syst_string(syst);
        mjj_name += "_" + syst_name;
        njets_name += "_" + syst_name;
    }

    std::string btag_string("2016"), bweight_string("bweight_");
    if (era == 2016) {
        btag_string = "2016";
        bweight_string += "2016";
    } else if (era == 2017) {
        btag_string = "2017";
        bweight_string += "2017";
    } else if (era == 2018) {
        btag_string = "2018";
        bweight_string += "2018";
    }

    input->SetBranchAddress(mjj_name.c_str(), &mjj);
    input->SetBranchAddress(njets_name.c_str(), &njets);

    // Only do these for mt || et || em
    if ((channel == "mt_tree") || (channel == "et_tree") || (channel == "em_tree")) {
        input->SetBranchAddress("nbtag", &nbtag);
        input->SetBranchAddress(("bjetDeepCSVVeto20Loose_" + btag_string + "_DR0p5").c_str(), &nbtag_loose);
        input->SetBranchAddress(("bjetDeepCSVVeto20Medium_" + btag_string + "_DR0p5").c_str(), &nbtag_medium);
        input->SetBranchAddress(bweight_string.c_str(), &bweight);

        input->SetBranchAddress("j1pt", &jpt_1);
        input->SetBranchAddress("j1eta", &jeta_1);
        input->SetBranchAddress("j1phi", &jphi_1);
        input->SetBranchAddress("j1csv", &jcsv_1);
        input->SetBranchAddress("j2pt", &jpt_2);
        input->SetBranchAddress("j2eta", &jeta_2);
        input->SetBranchAddress("j2phi", &jphi_2);
        input->SetBranchAddress("j2csv", &jcsv_2);
        input->SetBranchAddress(("jb1pt_" + btag_string).c_str(), &bpt_1);
        input->SetBranchAddress(("jb1eta_" + btag_string).c_str(), &beta_1);
        input->SetBranchAddress(("jb1phi_" + btag_string).c_str(), &bphi_1);
        // input->SetBranchAddress(("jb1csv_" + btag_string).c_str(), &bcsv_1);
        input->SetBranchAddress(("jb1hadronflavor_" + btag_string).c_str(), &bflavor_1);
        input->SetBranchAddress(("jb2pt_" + btag_string).c_str(), &bpt_2);
        input->SetBranchAddress(("jb2eta_" + btag_string).c_str(), &beta_2);
        input->SetBranchAddress(("jb2phi_" + btag_string).c_str(), &bphi_2);
        // input->SetBranchAddress(("jb2csv_" + btag_string).c_str(), &bcsv_2);
        input->SetBranchAddress(("jb2hadronflavor_" + btag_string).c_str(), &bflavor_2);
        input->SetBranchAddress("topQuarkPt1", &topQuarkPt1);
        input->SetBranchAddress("topQuarkPt2", &topQuarkPt2);
    } else if (TString(input->GetName()) == "tt_tree") {
        input->SetBranchAddress("nbtag", &Nbtag);
        input->SetBranchAddress("jetPt_1", &jpt_1);
        input->SetBranchAddress("jeta_1", &jeta_1);
        input->SetBranchAddress("jphi_1", &jphi_1);
        input->SetBranchAddress("jcsv_1", &jcsv_1);
        input->SetBranchAddress("jetPt_2", &jpt_2);
        input->SetBranchAddress("jeta_2", &jeta_2);
        input->SetBranchAddress("jphi_2", &jphi_2);
        input->SetBranchAddress("jcsv_2", &jcsv_2);
        input->SetBranchAddress("bpt_1", &bpt_1);
        input->SetBranchAddress("beta_1", &beta_1);
        input->SetBranchAddress("bphi_1", &bphi_1);
        // input->SetBranchAddress("bcsv_1", &bcsv_1);
        input->SetBranchAddress("bflavor_1", &bflavor_1);
        input->SetBranchAddress("bpt_2", &bpt_2);
        input->SetBranchAddress("beta_2", &beta_2);
        input->SetBranchAddress("bphi_2", &bphi_2);
        // input->SetBranchAddress("bcsv_2", &bcsv_2);
        input->SetBranchAddress("bflavor_2", &bflavor_2);
        input->SetBranchAddress("pt_top1", &topQuarkPt1);
        input->SetBranchAddress("pt_top2", &topQuarkPt2);
    } else {
        std::cout << "NAME ERROR" << std::endl;
    }
}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
    plain_jets.clear();
    btag_jets.clear();

    if (channel == "mt_tree" || channel == "et_tree" || channel == "em_tree") {
        Nbtag = nbtag;
    }

    jet j1(jpt_1, jeta_1, jphi_1, jcsv_1);
    jet j2(jpt_2, jeta_2, jphi_2, jcsv_2);
    jet b1(bpt_1, beta_1, bphi_1, bcsv_1, bflavor_1);
    jet b2(bpt_2, beta_2, bphi_2, bcsv_2, bflavor_2);
    plain_jets.push_back(j1);
    plain_jets.push_back(j2);
    btag_jets.push_back(b1);
    btag_jets.push_back(b2);
}

Float_t jet_factory::getNbtag(wps wp = wps::btag_any) {
    if (wp == wps::btag_any) {
        return Nbtag;
    } else if (wp == wps::btag_loose) {
        return nbtag_loose;
    } else if (wp == wps::btag_medium) {
        return nbtag_medium;
    }
    return 0;
}

std::string jet_factory::fix_syst_string(std::string syst) {
    auto formatted(syst);
    auto end = std::string::npos;
    if (syst.find("JetRel") != end || syst.find("JetJER") != end) {
        return syst_name_map[syst];
    } else {
        auto pos = syst.find("_Up");
        if (pos != end) {
            formatted.replace(pos, 3, "Up");
        }
        pos = syst.find("_Down");
        if (pos != end) {
            formatted.replace(pos, 5, "Down");
        }
    }
    return formatted;
}

#endif  // INCLUDE_FSA_JET_FACTORY_H_
