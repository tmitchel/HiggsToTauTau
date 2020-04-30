#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "TFile.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"

using namespace std;

TH1F* reorder(TH1F* s, TH1F* b, bool signal = true) {
  Int_t nBins = s->GetNbinsX();
  if ( b->GetNbinsX() != nBins) return nullptr;

  vector<pair<float, pair<float, float> > > event;
  for(Int_t iBin = 0; iBin != nBins + 2; ++iBin) {
    float y1 = s->GetBinContent(iBin);
    float y2 = b->GetBinContent(iBin);
    if (y1 < 0) y1 = 0;
    if (y2 < 0) y2 = 0;
    float r = 0;
    if ( y2 == 0 ) r = 1e10;
    else           r = y1/y2;
    pair<float, float> y1y2 = make_pair(y1, y2);
    event.push_back(make_pair(r, y1y2));
  }
  sort(event.begin(), event.end());

  TH1F* newSig = new TH1F(s->GetName(), "", nBins, 0, 1);
  TH1F* newBkg = new TH1F(b->GetName(), "", nBins, 0, 1);
  TH1F* h_r    = new TH1F("r", "", nBins, 0, 1);
  
  for(int i = 0; i != nBins+2; ++i) {
    float y1 = (event[i].second).first;
    float y2 = (event[i].second).second;
    newSig->SetBinContent(i, y1);
    newBkg->SetBinContent(i, y2);
    h_r->SetBinContent(i, event[i].first);
  }

  newSig->SetLineColor(kRed);
  newBkg->SetLineColor(kBlue);

  if ( signal ) return newSig;
  else          return newBkg;
}
  

void plotROCCurves3() {

  TFile *file;
  TH1F* s;
  TH1F* b;
  TString names[12] = {
    "NN_disc",
    "VBF_MELA",
    "mjj",
    "m_sv"
  };

  TString labels[12] = {
    "NN Disc.",
    "D_{bkg}^{VBF}",
    "M_{jj}",
  };
  
  
  TH2F* frame = new TH2F("frame","Di-Tau Channel",1000,0.,1,1000,0,1);
  frame->GetXaxis()->SetTitle("eff VBF H#rightarrow#tau_{h}#tau_{h}");
  frame->GetYaxis()->SetTitle("eff QCD");
  gStyle->SetOptStat(0);
  frame->Draw();

  TGraph* gr[9];

  TLegend* leg = new TLegend(0.21, 0.41, 0.41, 0.89);
  leg->SetBorderSize(0);
  
  for(int iSample = 0; iSample != 1; ++iSample) {
    std::cout << "plots/Quick/templates/"+names[iSample]+".root" << std::endl;
    file = TFile::Open("plots/Quick/templates/"+names[iSample]+".root");
    std::cout << file->GetName() << std::endl;
    s = (TH1F*)file->Get("mt_vbf/VBF125");
    b = (TH1F*)file->Get("mt_vbf/QCD");
    std::cout << s->Integral() << std::endl;
    s->SetLineColor(kRed);
    b->SetLineColor(kBlue);
    
    TH1F* h_s = reorder(s, b, true);
    TH1F* h_b = reorder(s, b, false);

    float normSig = h_s->Integral(-1,h_s->GetNbinsX()+1);
    float normBkg = h_b->Integral(-1,h_b->GetNbinsX()+1);

    float gr_eff[h_s->GetNbinsX()+1];
    float gr_bkg[h_b->GetNbinsX()+1];
    float eff, fr;

    for(int i = 0; i < h_s->GetNbinsX()+1; ++i) {
      eff = h_s->Integral(i, h_s->GetNbinsX()+1)/normSig;
      fr  = h_b->Integral(i, h_s->GetNbinsX()+1)/normBkg;
      cout << i << "\t" << eff << "\t" << fr << endl;
      gr_eff[i] = eff;
      gr_bkg[i] = fr;
    }

    gr[iSample] = new TGraph(h_s->GetNbinsX()+1, gr_eff, gr_bkg);
    gr[iSample]->SetMarkerColor(iSample+1);
    gr[iSample]->SetLineColor(iSample+1);
    gr[iSample]->SetMarkerSize(0.5);
    gr[iSample]->SetMarkerStyle(21);

    if ( iSample > 8 ) {
      gr[iSample]->SetMarkerColor(iSample-7);
      gr[iSample]->SetLineColor(iSample-7);
      gr[iSample]->SetMarkerStyle(22);
    }
    gr[iSample]->Draw("P");

    leg->AddEntry(gr[iSample], labels[iSample], "pl");
    cout << "Done " << names[iSample] << endl;

    delete s, b, h_s, h_b;
  }
  leg->Draw();

}

