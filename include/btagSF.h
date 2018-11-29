#include "jet_factory.h"

double GetSF(float x, int flavour, int syst){

    if (fabs(flavour)==4 or fabs(flavour)==5){
      if (syst==0){
        return 0.498094*((1.+(0.422991*x))/(1.+(0.210944*x)));
      } else if (syst==-1){
            if (x < 30) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.078885495662689209;
            else if (x < 50) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.025339335203170776;
            else if (x < 70) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.022487226873636246;
            else if (x < 100) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.021372253075242043;
            else if (x < 140) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.021989239379763603;
            else if (x < 200) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.023777997121214867;
            else if (x < 300) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.030794138088822365;
            else if (x < 600) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.041836585849523544;
            else return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.063810773193836212;
      } else if (syst==1){
            if (x < 30) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.078885495662689209;
            else if (x < 50) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.025339335203170776;
            else if (x < 70) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.022487226873636246;
            else if (x < 100) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.021372253075242043;
            else if (x < 140) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.021989239379763603;
            else if (x < 200) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.023777997121214867;
            else if (x < 300) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.030794138088822365;
            else if (x < 600) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.041836585849523544;
            else return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.063810773193836212;
      }
    } else {
      if (syst==0) return 1.0589+0.000382569*x+-2.4252e-07*x*x+2.20966e-10*x*x*x;
      if (syst==1) return (1.0589+0.000382569*x+-2.4252e-07*x*x+2.20966e-10*x*x*x)*(1+(0.100485+3.95509e-05*x+-4.90326e-08*x*x));
      if (syst==-1) return (1.0589+0.000382569*x+-2.4252e-07*x*x+2.20966e-10*x*x*x)*(1-(0.100485+3.95509e-05*x+-4.90326e-08*x*x));
  }
}


double bTagEventWeight(jet_factory* jets, int nBTags=0, int syst=0)
{
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
  if( nBTags==0 && nBtaggedJets==0) {
    weight = 1.;
  } else if(nBtaggedJets==1){
    double SF = GetSF(bjetpt_1, bjetflavour_1, syst);
    for (unsigned int i = 0; i < 2; i++){
      if( i == nBTags ) {
        weight += pow(SF,i)*pow(1-SF,1-i);
      } 
    }
  }
  else if(nBtaggedJets==2 ){
    double SF1 = GetSF(bjetpt_1, bjetflavour_1, syst);
    double SF2 = GetSF(bjetpt_2, bjetflavour_2, syst);
    
    for (unsigned int i = 0; i < 2; i++ ){
      for (unsigned int j = 0; j < 2; j++ ){
        if(i + j == nBTags) {
          weight += pow(SF1,i)*pow(1-SF1,1-i)*pow(SF2,j)*pow(1-SF2,1-j);
        }
      }
    }
  }
  return weight;
}

int PromoteDemote(TH2F *h_btag_eff_b, TH2F *h_btag_eff_c, TH2F *h_btag_eff_oth, jet* bjet, int nbtag, int syst) {

  float SF = GetSF(bjet->getPt(), bjet->getFlavor(), syst);
  float beff = 1.0;
  if (bjet->getFlavor() == 5) {
    // b-jet
    auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_b->GetXaxis()->GetBinLowEdge(h_btag_eff_b->GetNbinsX() + 1) - 1)); // get bjet pT or 1 lower than max bin edge
    beff == h_btag_eff_b->GetBinContent(effective_pt, h_btag_eff_b->GetYaxis()->FindBin(fabs(bjet->getEta())));

  } else if (bjet->getFlavor() == 4) {
    // c-jet
    auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_c->GetXaxis()->GetBinLowEdge(h_btag_eff_c->GetNbinsX() + 1) - 1)); // get bjet pT or 1 lower than max bin edge
    beff == h_btag_eff_c->GetBinContent(effective_pt, h_btag_eff_c->GetYaxis()->FindBin(fabs(bjet->getEta())));

  } else {
    // light-jet
    auto effective_pt = std::min(bjet->getPt(), static_cast<Float_t>(h_btag_eff_oth->GetXaxis()->GetBinLowEdge(h_btag_eff_oth->GetNbinsX() + 1) - 1)); // get bjet pT or 1 lower than max bin edge
    beff == h_btag_eff_oth->GetBinContent(effective_pt, h_btag_eff_oth->GetYaxis()->FindBin(fabs(bjet->getEta())));

  }

  TRandom3 *rand = new TRandom3();
  rand->SetSeed((int)((bjet->getEta() + 5) * 100000));
  float myrand = rand->Rndm();

  if (SF < 1 && myrand < (1 - SF)) {
    nbtag = nbtag - 1;
  }

  if (SF > 1 && myrand < ((1 - SF) / (1 - 1.0 / beff))) { 
    nbtag = nbtag + 1;
  }

  return nbtag;
}
