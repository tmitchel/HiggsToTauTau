// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_JET_FACTORY_H_
#define INCLUDE_JET_FACTORY_H_

#include <algorithm>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TTree.h"

class jet {
   private:
    Float_t pt, eta, phi, csv, flavor;
    TLorentzVector p4;

   public:
    jet(Float_t, Float_t, Float_t, Float_t, Float_t);
    virtual ~jet() {}

    Float_t getPt() { return pt; }
    Float_t getEta() { return eta; }
    Float_t getPhi() { return phi; }
    Float_t getCSV() { return csv; }
    Float_t getFlavor() { return flavor; }
    TLorentzVector getP4() { return p4; }
};

// initialize member data and set TLorentzVector
jet::jet(Float_t Pt, Float_t Eta, Float_t Phi, Float_t Csv, Float_t Flavor = -9999) : pt(Pt), eta(Eta), phi(Phi), csv(Csv), flavor(Flavor) {
    p4.SetPtEtaPhiM(Pt, Eta, Phi, 0.);
}

class jet_factory {
   private:
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
    void promoteDemote(TH2F *, TH2F *, TH2F *, int);
    double bTagEventWeight(int, int);
    std::string fix_syst_string(std::string);

    // getters
    Float_t getNbtag() { return Nbtag; }
    Float_t getNbtagLoose() { return nbtag_loose; }
    Float_t getNbtagMedium() { return nbtag_medium; }
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
    : syst_name_map{
          {"JetRelSam_Up", "JetRelativeSampleUp"},
          {"JetRelSam_Down", "JetRelativeSampleDown"},
          {"JetRelBal_Up", "JetRelativeBalUp"},
          {"JetRelBal_Down", "JetRelativeBalDown"},
          {"JetJER_Up", "JERUp"},
          {"JetJER_Down", "JERDown"},
      } {
    std::string mjj_name("vbfMass"), njets_name("jetVeto30");
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
}

// initialize member data and set TLorentzVector
void jet_factory::run_factory() {
    plain_jets.clear();
    btag_jets.clear();

    Nbtag = nbtag;

    jet j1(jpt_1, jeta_1, jphi_1, jcsv_1);
    jet j2(jpt_2, jeta_2, jphi_2, jcsv_2);
    jet b1(bpt_1, beta_1, bphi_1, bcsv_1, bflavor_1);
    jet b2(bpt_2, beta_2, bphi_2, bcsv_2, bflavor_2);
    plain_jets.push_back(j1);
    plain_jets.push_back(j2);
    btag_jets.push_back(b1);
    btag_jets.push_back(b2);
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

namespace btagSF {
double GetSF(float x, int flavour, int syst) {
    if (fabs(flavour) == 4 || fabs(flavour) == 5) {
        if (syst == 0) {
            return 0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)));
        } else if (syst == -1) {
            if (x < 30)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.078885495662689209;
            else if (x < 50)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.025339335203170776;
            else if (x < 70)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.022487226873636246;
            else if (x < 100)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.021372253075242043;
            else if (x < 140)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.021989239379763603;
            else if (x < 200)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.023777997121214867;
            else if (x < 300)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.030794138088822365;
            else if (x < 600)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.041836585849523544;
            else
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) - 0.063810773193836212;
        } else if (syst == 1) {
            if (x < 30)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.078885495662689209;
            else if (x < 50)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.025339335203170776;
            else if (x < 70)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.022487226873636246;
            else if (x < 100)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.021372253075242043;
            else if (x < 140)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.021989239379763603;
            else if (x < 200)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.023777997121214867;
            else if (x < 300)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.030794138088822365;
            else if (x < 600)
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.041836585849523544;
            else
                return (0.498094 * ((1. + (0.422991 * x)) / (1. + (0.210944 * x)))) + 0.063810773193836212;
        }
    } else {
        if (syst == 0) return 1.0589 + 0.000382569 * x + -2.4252e-07 * x * x + 2.20966e-10 * x * x * x;
        if (syst == 1)
            return (1.0589 + 0.000382569 * x + -2.4252e-07 * x * x + 2.20966e-10 * x * x * x) *
                   (1 + (0.100485 + 3.95509e-05 * x + -4.90326e-08 * x * x));
        if (syst == -1)
            return (1.0589 + 0.000382569 * x + -2.4252e-07 * x * x + 2.20966e-10 * x * x * x) *
                   (1 - (0.100485 + 3.95509e-05 * x + -4.90326e-08 * x * x));
    }
}

double _bTagEventWeight(jet_factory *jets, int nBTags = 0, int syst = 0) {
    /*
        ##################################################################
        Event weight matrix:
        ------------------------------------------------------------------
        nBTags\b-tagged jets  |    0        1             2
        ------------------------------------------------------------------
          0                   |    1      1-SF      (1-SF1)(1-SF2)
                              |
          1                   |    0       SF    SF1(1-SF2)+(1-SF1)SF2
                              |
          2                   |    0        0           SF1SF2
        ##################################################################
      */

    auto bjets = jets->getBtagJets();
    auto bjetpt_1(bjets.at(0).getPt()), bjetflavour_1(bjets.at(0).getFlavor()), bjetpt_2(bjets.at(1).getPt()), bjetflavour_2(bjets.at(1).getFlavor());
    auto nBtaggedJets = jets->getNbtag();

    double weight(0.);
    if (nBTags == 0 && nBtaggedJets == 0) {
        weight = 1.;
    } else if (nBtaggedJets == 1) {
        double SF = GetSF(bjetpt_1, bjetflavour_1, syst);
        for (unsigned int i = 0; i < 2; i++) {
            if (i == nBTags) {
                weight += pow(SF, i) * pow(1 - SF, 1 - i);
            }
        }
    } else if (nBtaggedJets == 2) {
        double SF1 = GetSF(bjetpt_1, bjetflavour_1, syst);
        double SF2 = GetSF(bjetpt_2, bjetflavour_2, syst);

        for (unsigned int i = 0; i < 2; i++) {
            for (unsigned int j = 0; j < 2; j++) {
                if (i + j == nBTags) {
                    weight += pow(SF1, i) * pow(1 - SF1, 1 - i) * pow(SF2, j) * pow(1 - SF2, 1 - j);
                }
            }
        }
    }
    return weight;
}

int _PromoteDemote(TH2F *h_btag_eff_b, TH2F *h_btag_eff_c, TH2F *h_btag_eff_oth, jet *bjet, int nbtag, int syst) {
    float SF = GetSF(bjet->getPt(), bjet->getFlavor(), syst);
    float beff = 1.0;
    if (bjet->getFlavor() == 5) {
        // b-jet
        auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_b->GetXaxis()->GetBinLowEdge(h_btag_eff_b->GetNbinsX() + 1) -
                                                                         1));  // get bjet pT or 1 lower than max bin edge
        beff == h_btag_eff_b->GetBinContent(effective_pt, h_btag_eff_b->GetYaxis()->FindBin(fabs(bjet->getEta())));

    } else if (bjet->getFlavor() == 4) {
        // c-jet
        auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_c->GetXaxis()->GetBinLowEdge(h_btag_eff_c->GetNbinsX() + 1) -
                                                                         1));  // get bjet pT or 1 lower than max bin edge
        beff == h_btag_eff_c->GetBinContent(effective_pt, h_btag_eff_c->GetYaxis()->FindBin(fabs(bjet->getEta())));

    } else {
        // light-jet
        auto effective_pt = std::min(bjet->getPt(),
                                     static_cast<Float_t>(h_btag_eff_oth->GetXaxis()->GetBinLowEdge(h_btag_eff_oth->GetNbinsX() + 1) -
                                                          1));  // get bjet pT or 1 lower than max bin edge
        beff == h_btag_eff_oth->GetBinContent(effective_pt, h_btag_eff_oth->GetYaxis()->FindBin(fabs(bjet->getEta())));
    }

    TRandom3 *rand = new TRandom3();
    rand->SetSeed(static_cast<int>((bjet->getEta() + 5) * 100000));
    float myrand = rand->Rndm();

    if (SF < 1 && myrand < (1 - SF) && nbtag > 0) {
        nbtag = nbtag - 1;
    }

    if (SF > 1 && myrand < ((1 - SF) / (1 - 1.0 / beff))) {
        nbtag = nbtag + 1;
    }

    return nbtag;
}
}  // namespace btagSF

void jet_factory::promoteDemote(TH2F *h_btag_eff_b, TH2F *h_btag_eff_c, TH2F *h_btag_eff_oth, int syst = 0) {
    Nbtag = btagSF::_PromoteDemote(h_btag_eff_b, h_btag_eff_c, h_btag_eff_oth, &(this->btag_jets.at(0)), this->nbtag, syst);
}

double jet_factory::bTagEventWeight(int nBTags = 0, int syst = 0) { return btagSF::_bTagEventWeight(this, nBTags, syst); }

#endif  // INCLUDE_JET_FACTORY_H_
