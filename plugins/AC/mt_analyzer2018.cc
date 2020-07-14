// Copyright [2019] Tyler Mitchell

// system includes
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

// ROOT includes
#include "RooFunctor.h"
#include "RooMsgService.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TTree.h"

// user includes
#include "../../include/ACWeighter.h"
#include "../../include/CLParser.h"
#include "../../include/ComputeWG1Unc.h"
#include "../../include/LumiReweightingStandAlone.h"
#include "../../include/fsa/event_factory.h"
#include "../../include/fsa/jet_factory.h"
#include "../../include/fsa/met_factory.h"
#include "../../include/fsa/muon_factory.h"
#include "../../include/slim_tree.h"
#include "../../include/swiss_army_class.h"
// #include "../../include/fsa/tau_factory.h"
#include "../../include/fsa/ditau_factory.h"

typedef std::vector<double> NumV;

int main(int argc, char *argv[]) {
    ////////////////////////////////////////////////
    // Initial setup:                             //
    // Get file names, normalization, paths, etc. //
    ////////////////////////////////////////////////

    CLParser parser(argc, argv);
    bool condor = parser.Flag("--condor");
    std::string name = parser.Option("-n");
    std::string path = parser.Option("-p");
    std::string syst = parser.Option("-u");
    std::string sample = parser.Option("-s");
    std::string output_dir = parser.Option("-d");
    std::string signal_type = parser.Option("--stype");
    std::string fname = path + sample + ".root";
    bool isData = sample.find("data") != std::string::npos;
    bool isEmbed = sample.find("embed") != std::string::npos || name.find("embed") != std::string::npos;
    bool isMG = sample.find("madgraph") != std::string::npos;
    bool doAC = signal_type != "None";

    // get systematic shift name
    std::string systname = "NOMINAL";
    if (!syst.empty()) {
        systname = "SYST_" + syst;
    }

    // Keep
    auto fin = TFile::Open(fname.c_str());
    auto ntuple = reinterpret_cast<TTree *>(fin->Get("mutau_tree"));

    // get number of generated events
    auto counts = reinterpret_cast<TH1D *>(fin->Get("nevents"));
    auto gen_number = counts->GetBinContent(2);

    std::string original = sample;
    if (name == "VBF125") {
        sample = "vbf125";
    } else if (name == "ggH125" && signal_type != "madgraph") {
        sample = "ggh125";
    } else if (name == "WH125") {
        sample = "wh125";
    } else if (name == "WHsigned125") {
        sample = sample.find("plus") == std::string::npos ? "wplus125" : "wminus125";
    } else if (name == "ZH125") {
        sample = "zh125";
    }

    if (signal_type == "JHU" && (sample == "ggh125" || sample == "vbf125")) {
        gen_number = 1.;
    }

    //////////////////////////////////////
    // Final setup:                     //
    // Declare histograms and factories //
    //////////////////////////////////////

    // construct factories
    event_factory event(ntuple, isData, lepton::MUON, 2018, isMG, syst);
    muon_factory muons(ntuple);
    // tau_factory taus(ntuple);
    jet_factory jets(ntuple, 2018, syst);
    met_factory met(ntuple, 2018, syst);

    // NEW
    ditau_factory ditaus(ntuple);

    // Try just getting one entry
    // begin the event loop
    Int_t nevts = ntuple->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (Int_t i = 0; i < 1; i++) {
        ntuple->GetEntry(i);

        // run factories
        muons.run_factory();
        // taus.run_factory();
	ditaus.run_factory();
        jets.run_factory();
        event.setNjets(jets.getNjets());

        auto muon = muons.good_muon();
	auto ditau = ditaus.good_tau();

        // build Higgs
        TLorentzVector Higgs = ditau.getP4() + met.getP4();

        // calculate mt
        // double met_x = met.getMet() * cos(met.getMetPhi());
        // double met_y = met.getMet() * sin(met.getMetPhi());
        // double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
        // double mt = sqrt(pow(muon.getPt() + met_pt, 2) - pow(muon.getP4().Px() + met_x, 2) - pow(muon.getP4().Py() + met_y, 2));

    }  // close event loop

    fin->Close();
    /*
    fout->cd();
    fout->Write();
    fout->Close();
    */
    return 0;
}
