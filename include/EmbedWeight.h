
// ############################################################

// This file is for obtaining the Correction factors for the embedded samples:
// For further information visit:   https://github.com/CMS-HTT/CorrectionsWorkspace
// These corrections should be applied at the last stage of the analysis: i.e. filling datacard root file and in the FinalSelection2D_relaxedFR.cc file
//accessing the correction factor is as following:




//  for muons
//  vector<double> info=EmdWeight_Muon(wEmbed,mytau1.Pt(),mytau1.Eta(),MUONISOLATION);

//  for electron
//  vector<double> info2=EmdWeight_Electron(wEmbed,mytau2.Pt(),mytau2.Eta(),ELECTRONISOLATION);

//  for tau
//  vector<double> info3=EmdWeight_Tau(wEmbed,mytau2.Pt(),mytau2.Eta(),t1_decayMode);



//Note: It seems that muon isolationa nd electron isolation variables are not filled in the skimmed ntuple!




// ############################################################


#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include "TMultiGraph.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <stdio.h>
#include <TF1.h>
#include <TDirectoryFile.h>
#include <TRandom3.h>
#include "TLorentzVector.h"
#include "TString.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TKey.h"
#include "THashList.h"
#include "THStack.h"
#include "TPaveLabel.h"
#include "TFile.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooFunctor.h"

using namespace std;

vector<double> EmdWeight_Muon(RooWorkspace *wEmbed, float pt, float eta, float MuonIsolation ){
    
    
    
    vector<double>  Muon_Embed_Weights;
    Muon_Embed_Weights.clear();
    
    // In the event loop:
    wEmbed->var("m_pt")->setVal(pt);
    wEmbed->var("m_eta")->setVal(eta);
    wEmbed->var("m_iso")->setVal(MuonIsolation);
    
    //Muon Id weight
    double Weight_Muon_0 = 0;
    Weight_Muon_0 = wEmbed->function("m_id_data")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_0);
    
    double Weight_Muon_1 = 0;
    Weight_Muon_1 = wEmbed->function("m_id_mc")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_1);
    
    double Weight_Muon_2 = 0;
    Weight_Muon_2 = wEmbed->function("m_id_ratio")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_2);
    
    
    //Muon Iso weight
    double Weight_Muon_3 = 0;
    Weight_Muon_3 = wEmbed->function("m_iso_data")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_3);
    
    double Weight_Muon_4 = 0;
    Weight_Muon_4 = wEmbed->function("m_iso_mc")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_4);
    
    double Weight_Muon_5 = 0;
    Weight_Muon_5 = wEmbed->function("m_iso_ratio")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_5);
    
    
    //Muon Trg weight
    double Weight_Muon_6 = 0;
    Weight_Muon_6 = wEmbed->function("m_trg_data")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_6);
    
    double Weight_Muon_7 =0;
    //    Weight_Muon_7 = wEmbed->function("m_trgOR_data")->getVal();  ##Buggy
    Muon_Embed_Weights.push_back(Weight_Muon_7);
    
    
    //Muon Iso weight for various Muon isolation
    double Weight_Muon_8 = 0;
    Weight_Muon_8 = wEmbed->function("m_iso_binned_data")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_8);
    
    double Weight_Muon_9 = 0;
    Weight_Muon_9 = wEmbed->function("m_iso_binned_mc")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_9);
    
    double Weight_Muon_10 = 0;
    Weight_Muon_10 = wEmbed->function("m_iso_binned_ratio")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_10);
    
    
    //Muon Trg weight for various Muon isolation
    double Weight_Muon_11 = 0;
    Weight_Muon_11 = wEmbed->function("m_trg_binned_data")->getVal();
    Muon_Embed_Weights.push_back(Weight_Muon_11);
    
    double Weight_Muon_12 = 0;
//    Weight_Muon_12 = wEmbed->function("m_trgOR_binned_data")->getVal(); ##Buggy
    Muon_Embed_Weights.push_back(Weight_Muon_12);
    
    
    return Muon_Embed_Weights;
    
    
}


vector<double> EmdWeight_Electron(RooWorkspace *wEmbed, float pt, float eta, float ElectronIsolation ){
    
    
    
    vector<double>  Electron_Embed_Weights;
    Electron_Embed_Weights.clear();
    
    // In the event loop:
    wEmbed->var("e_pt")->setVal(pt);
    wEmbed->var("e_eta")->setVal(eta);
    wEmbed->var("e_iso")->setVal(ElectronIsolation);
    
    //Electron Id weight
    double Weight_Electron_0 = 0;
    Weight_Electron_0 = wEmbed->function("e_id_data")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_0);
    
    double Weight_Electron_1 = 0;
    Weight_Electron_1 = wEmbed->function("e_id_mc")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_1);
    
    double Weight_Electron_2 = 0;
    Weight_Electron_2 = wEmbed->function("e_id_ratio")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_2);
    
    
    //Electron Iso weight
    double Weight_Electron_3 = 0;
    Weight_Electron_3 = wEmbed->function("e_iso_data")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_3);
    
    double Weight_Electron_4 = 0;
    Weight_Electron_4 = wEmbed->function("e_iso_mc")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_4);
    
    double Weight_Electron_5 = 0;
    Weight_Electron_5 = wEmbed->function("e_iso_ratio")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_5);
    
    
    //Electron Trg weight
    double Weight_Electron_6 = 0;
    Weight_Electron_6 = wEmbed->function("e_trg_data")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_6);
    
    
    //Electron Iso weight for various Electron isolation
    double Weight_Electron_7 = 0;
    Weight_Electron_7 = wEmbed->function("e_iso_binned_data")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_7);
    
    double Weight_Electron_8 = 0;
    Weight_Electron_8 = wEmbed->function("e_iso_binned_mc")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_8);
    
    double Weight_Electron_9 = 0;
    Weight_Electron_9 = wEmbed->function("e_iso_binned_ratio")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_9);
    
    
    //Electron Trg weight for various Electron isolation
    double Weight_Electron_10 = 0;
    Weight_Electron_10 = wEmbed->function("e_trg_binned_data")->getVal();
    Electron_Embed_Weights.push_back(Weight_Electron_10);
    
    
    
    return Electron_Embed_Weights;
    
    
}




vector<double> EmdWeight_Tau(RooWorkspace *wEmbed, float pt, float eta, int TauDM ){
    
    
    
    vector<double>  Tau_Embed_Weights;
    Tau_Embed_Weights.clear();
    
    // In the event loop:
    wEmbed->var("t_pt")->setVal(pt);
    wEmbed->var("t_eta")->setVal(eta);
    wEmbed->var("t_dm")->setVal(TauDM);
    
    //Tau Id weight
    double Weight_Tau_0 = 0;
//    Weight_Tau_0 = wEmbed->function("t_iso_mva_m_pt30_sf")->getVal();  ##Buggy
    Tau_Embed_Weights.push_back(Weight_Tau_0);
    
    double Weight_Tau_1 = 0;
//    Weight_Tau_1 = wEmbed->function("t_iso_mva_t_pt40_eta2p1_sf")->getVal();  ##Buggy
    Tau_Embed_Weights.push_back(Weight_Tau_1);
    
    return Tau_Embed_Weights;

    
}

float m_sel_trg_ratio(RooWorkspace *wEmbed, float pt1, float eta1, float pt2, float eta2) {

  wEmbed->var("gt1_pt")->setVal(pt1);
  wEmbed->var("gt1_eta")->setVal(eta1);
  wEmbed->var("gt2_pt")->setVal(pt2);
  wEmbed->var("gt2_eta")->setVal(eta2);

  return wEmbed->function("m_sel_trg_ratio")->getVal();
}
