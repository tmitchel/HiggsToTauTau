#ifndef INCLUDE_JET_FACTORY_H_
#define INCLUDE_JET_FACTORY_H_

#include <algorithm>
#include <string>
#include <vector>
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
jet::jet(Float_t Pt, Float_t Eta, Float_t Phi, Float_t Csv, Float_t Flavor = -9999) : pt(Pt),
                                                                                      eta(Eta),
                                                                                      phi(Phi),
                                                                                      csv(Csv),
                                                                                      flavor(Flavor) {
  p4.SetPtEtaPhiM(Pt, Eta, Phi, 0.);
}

class jet_factory {
 private:
  Float_t mjj;
  Float_t jpt_1, jeta_1, jphi_1, jcsv_1;
  Float_t jpt_2, jeta_2, jphi_2, jcsv_2;
  Float_t bpt_1, beta_1, bphi_1, bcsv_1, bflavor_1;
  Float_t bpt_2, beta_2, bphi_2, bcsv_2, bflavor_2;
  Float_t topQuarkPt1, topQuarkPt2;
  Int_t Nbtag, nbtag, njetspt20, njets;
  std::vector<jet> plain_jets, btag_jets;

 public:
  jet_factory(TTree *, std::string);
  virtual ~jet_factory() {}
  void run_factory();
  void promoteDemote(TH2F *, TH2F *, TH2F *, int);
  double bTagEventWeight(int, int);

  // getters
  Int_t getNbtag() { return Nbtag; }
  Int_t getNjets() { return njets; }
  Int_t getNjetPt20() { return njetspt20; }
  Float_t getDijetMass() { return mjj; }
  Float_t getTopPt1() { return topQuarkPt1; }
  Float_t getTopPt2() { return topQuarkPt2; }
  std::vector<jet> getJets() { return plain_jets; }
  std::vector<jet> getBtagJets() { return btag_jets; }
};

// read data from tree into member variables
jet_factory::jet_factory(TTree *input, std::string syst) {
  auto mjj_name("vbfMass"), njets_name("njets");
  // auto mjj_name("vbfMassWoNoisyJets"), njets_name("njets");

  if (syst.find(mjj_name) != std::string::npos) {
    mjj_name = syst.c_str();
  } else if (syst.find("jetVeto30") != std::string::npos) {
    njets_name = syst.c_str();
  }

  input->SetBranchAddress(mjj_name, &mjj);
  input->SetBranchAddress(njets_name, &njets);
  input->SetBranchAddress("nbtag", &nbtag);
  input->SetBranchAddress("njetspt20", &njetspt20);
  input->SetBranchAddress("jpt_1", &jpt_1);
  input->SetBranchAddress("jeta_1", &jeta_1);
  input->SetBranchAddress("jphi_1", &jphi_1);
  input->SetBranchAddress("jcsv_1", &jcsv_1);
  input->SetBranchAddress("jpt_2", &jpt_2);
  input->SetBranchAddress("jeta_2", &jeta_2);
  input->SetBranchAddress("jphi_2", &jphi_2);
  input->SetBranchAddress("jcsv_2", &jcsv_2);
  input->SetBranchAddress("bpt_1", &bpt_1);
  input->SetBranchAddress("beta_1", &beta_1);
  input->SetBranchAddress("bphi_1", &bphi_1);
  input->SetBranchAddress("bcsv_1", &bcsv_1);
  input->SetBranchAddress("bflavor_1", &bflavor_1);
  input->SetBranchAddress("bpt_2", &bpt_2);
  input->SetBranchAddress("beta_2", &beta_2);
  input->SetBranchAddress("bphi_2", &bphi_2);
  input->SetBranchAddress("bcsv_2", &bcsv_2);
  input->SetBranchAddress("bflavor_2", &bflavor_2);
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
  // if (jpt_1 > 0) {
  plain_jets.push_back(j1);
  // }
  // if (jpt_2 > 0) {
  plain_jets.push_back(j2);
  // }
  // if (bpt_1 > 0) {
  btag_jets.push_back(b1);
  // }
  // if (bpt_2 > 0) {
  btag_jets.push_back(b2);
  // }
}

namespace btagSF {
double GetSF(float x, int flavour, int syst) {
  if (fabs(flavour) == 4 or fabs(flavour) == 5) {
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
    if (syst == 1) return (1.0589 + 0.000382569 * x + -2.4252e-07 * x * x + 2.20966e-10 * x * x * x) * (1 + (0.100485 + 3.95509e-05 * x + -4.90326e-08 * x * x));
    if (syst == -1) return (1.0589 + 0.000382569 * x + -2.4252e-07 * x * x + 2.20966e-10 * x * x * x) * (1 - (0.100485 + 3.95509e-05 * x + -4.90326e-08 * x * x));
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
  auto bjetpt_1(bjets.at(0).getPt()), bjetflavour_1(bjets.at(0).getFlavor()),
      bjetpt_2(bjets.at(1).getPt()), bjetflavour_2(bjets.at(1).getFlavor());
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
    auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_b->GetXaxis()->GetBinLowEdge(h_btag_eff_b->GetNbinsX() + 1) - 1));  // get bjet pT or 1 lower than max bin edge
    beff == h_btag_eff_b->GetBinContent(effective_pt, h_btag_eff_b->GetYaxis()->FindBin(fabs(bjet->getEta())));

  } else if (bjet->getFlavor() == 4) {
    // c-jet
    auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_c->GetXaxis()->GetBinLowEdge(h_btag_eff_c->GetNbinsX() + 1) - 1));  // get bjet pT or 1 lower than max bin edge
    beff == h_btag_eff_c->GetBinContent(effective_pt, h_btag_eff_c->GetYaxis()->FindBin(fabs(bjet->getEta())));

  } else {
    // light-jet
    auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_oth->GetXaxis()->GetBinLowEdge(h_btag_eff_oth->GetNbinsX() + 1) - 1));  // get bjet pT or 1 lower than max bin edge
    beff == h_btag_eff_oth->GetBinContent(effective_pt, h_btag_eff_oth->GetYaxis()->FindBin(fabs(bjet->getEta())));
  }

  TRandom3 *rand = new TRandom3();
  rand->SetSeed((int)((bjet->getEta() + 5) * 100000));
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

double jet_factory::bTagEventWeight(int nBTags = 0, int syst = 0) {
  return btagSF::_bTagEventWeight(this, nBTags, syst);
}

#endif  // INCLUDE_JET_FACTORY_H_
