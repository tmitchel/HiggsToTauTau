// Copyright [2020] Tyler Mitchell

// system includes
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "RooFunctor.h"
#include "RooMsgService.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "TGraphAsymmErrors.h"

// user includes
#include "../../include/CLParser.h"
#include "../../include/LumiReweightingStandAlone.h"
#include "../../include/boosted_slim_tree.h"
#include "../../include/ggntuple/boosted_tau_factory.h"
#include "../../include/ggntuple/electron_factory.h"
#include "../../include/ggntuple/event_factory.h"
#include "../../include/ggntuple/gen_factory.h"
#include "../../include/ggntuple/jet_factory.h"
#include "../../include/ggntuple/met_factory.h"
#include "../../include/ggntuple/muon_factory.h"
#include "../../include/swiss_army_class.h"

int main(int argc, char *argv[]) {
    CLParser parser(argc, argv);
    bool condor = parser.Flag("--condor");
    std::string name = parser.Option("-n");
    std::string path = parser.Option("-p");
    std::string syst = parser.Option("-u");
    std::string sample = parser.Option("-s");
    std::string output_dir = parser.Option("-d");
    std::string signal_type = parser.Option("--stype");
    std::string fname = path + sample + ".root";
    bool isData = name.find("data") != std::string::npos;
    bool isEmbed = sample.find("embed") != std::string::npos || name.find("embed") != std::string::npos;
    bool isMG = sample.find("madgraph") != std::string::npos;
    bool doAC = signal_type != "None" && signal_type != "powheg";

    std::string systname = "NOMINAL";
    if (!syst.empty()) {
        systname = "SYST_" + syst;
    }

    // create output path
    auto suffix = "_output.root";
    auto prefix = "Output/trees/" + output_dir;
    std::string filename, logname;
    filename = prefix + "/" + systname + "/" + sample + std::string("_") + name + "_" + systname + suffix;
    logname = prefix + "/logs/" + sample + std::string("_") + name + "_" + systname + ".txt";

    if (condor) {
        filename = sample + std::string("_") + name + "_" + systname + suffix;
    }

    // create the log file
    std::ofstream logfile;
    if (!condor) {
        logfile.open(logname, std::ios::out | std::ios::trunc);
    }

    std::ostream &running_log = (condor ? std::cout : logfile);

    // open log file and log some things
    running_log << "Opening file... " << sample << std::endl;
    running_log << "With name...... " << name << std::endl;
    running_log << "And running systematic " << systname << std::endl;
    running_log << "Using options: " << std::endl;
    running_log << "\t name: " << name << std::endl;
    running_log << "\t path: " << path << std::endl;
    running_log << "\t syst: " << syst << std::endl;
    running_log << "\t sample: " << sample << std::endl;
    running_log << "\t output_dir: " << output_dir << std::endl;
    running_log << "\t signal_type: " << signal_type << std::endl;
    running_log << "\t isData: " << isData << " isEmbed: " << isEmbed << " doAC: " << doAC << std::endl;

    auto fin = TFile::Open(fname.c_str());
    auto ntuple = reinterpret_cast<TTree *>(fin->Get("emu_tree"));

    // get number of generated events
    auto counts = reinterpret_cast<TH1F *>(fin->Get("hcount"));
    auto gen_number = counts->GetBinContent(2);

    auto fout = new TFile(filename.c_str(), "RECREATE");
    counts->Write();
    fout->mkdir("grabbag");
    fout->cd("grabbag");

    // initialize Helper class
    Helper *helper = new Helper(fout, name, syst);

    // cd to root of output file and create tree
    fout->cd();
    slim_tree *output_tree = new slim_tree("em_tree", doAC);

    // get normalization (lumi & xs are in swiss_army_class.h)
    if (sample == "ggh125_powheg") {
        sample = "ggh125";
    } else if (sample == "vbf125_powheg") {
        sample = "vbf125";
    }
    double norm(1.);
    if (!isData && !isEmbed) {
        norm = helper->getLuminosity2017() * helper->getCrossSection(sample) / gen_number;
    }

    //std::cout << norm << " " << helper->getLuminosity2017() << " " << helper->getCrossSection(sample) << " " << sample << " " << gen_number;

    // Scale factors
    // auto lumi_weights =
    //     new reweight::LumiReWeighting(fname.c_str(), "data/Data_nPU_new.root", "hPUTrue", "pileup");

    TFile mu_trg_corr_file("data/EfficienciesAndSF_RunBtoF_Nov17Nov2017.root");
    TH2F *mu_trg_corr = reinterpret_cast<TH2F*>(mu_trg_corr_file.Get("IsoMu27_PtEtaBins/pt_abseta_ratio"));
    // mu_trg_corr_file.Close();

    TFile mu_id_corr_file("data/RunBCDEF_SF_ID.root");
    TH2F *mu_id_corr = reinterpret_cast<TH2F*>(mu_id_corr_file.Get("NUM_MediumID_DEN_genTracks_pt_abseta"));
    // mu_id_corr_file.Close();

    TFile mu_iso_corr_file("data/RunBCDEF_SF_ISO.root");
    TH2F *mu_iso_corr = reinterpret_cast<TH2F *>(mu_iso_corr_file.Get("NUM_LooseRelIso_DEN_MediumID_pt_abseta"));
    // mu_iso_corr_file.Close();

    // construct factories
    electron_factory electron(ntuple);
    electron.set_process_all();  // loop through all electrons to build veto
    muon_factory muons(ntuple);
    gen_factory gens(ntuple, isData);
    boosted_tau_factory taus(ntuple);
    event_factory event(ntuple, lepton::MUON, 2017, isMG, syst);
    jet_factory jets(ntuple, 2017, isData, syst);
    met_factory met(ntuple, 2017, syst);

    Int_t nevts = ntuple->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (Int_t i = 0; i < nevts; i++) {
        ntuple->GetEntry(i);
        if (i == progress * fraction) {
            running_log << "LOG: Processing: " << progress * 10 << "% complete." << std::endl;
            progress++;
        }

        Float_t evtwt(norm);
        helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 1., 1.);

        // apply trigger
        if (event.fire_trigger(trigger::Mu50)) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 2., 1.);
        } else {
            continue;
        }

        // apply met filters
        if (true) {  // met filter selection go here
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 3., 1.);
        } else {
            continue;
        }

        // met selection
        if (met.getMet() >= 50) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 4., 1.);
        } else {
            continue;
        }

        // run factory before we access muons
        muons.run_factory();

        // get the good muon
        auto muon = muons.good_muon();

        // muon kinematic selection
        if (muon.getPt() > 52 && fabs(muon.getEta()) < 2.4) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 5., 1.);
        } else {
            continue;
        }

        // muon ID selection
        if (muon.getID()) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 6., 1.);
        } else {
            continue;
        }

        // run factory before we access electrons
        electron.run_factory();

        // get the good electrons
        auto electrons = electron.good_electron();

        // electron kinematic selection
        if (electrons.getPt() > 15 && fabs(electrons.getEta()) < 2.5) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 7., 1.);
        } else {
            continue;
        }

        // electron ID selection
        if (electrons.getID()) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 8., 1.);
        } else {
            continue;
        }

        // event selection
        auto dR_ele_mu = muon.getP4().DeltaR(electrons.getP4());
        if (dR_ele_mu >= 0.1 && dR_ele_mu <= 0.8) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 9., 1.);
        } else {
            continue;
        }

        // calculate mt and do selection
        auto mt = helper->transverse_mass(muon.getP4(), met.getMet(), met.getMetPhi());
        if (mt <= 80) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 10., 1.);
        } else {
            continue;
        }

        // remove low ditau mass
        if (event.getMSV() >= 10) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 11., 1.);
        } else {
            continue;
        }

        // run jet factory before we access jets
        jets.run_factory();

        // b-jet veto
        if (jets.getNbtag() == 0) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 12., 1.);
        } else {
            continue;
        }

        // HT cut
        if (jets.getHT(30., muon.getP4(), electrons.getP4()) > 200) {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 13., 1.);
        } else {
            continue;
        }

        // run electron factory to get veto
        //electron.run_factory();
        //if (electron.num_good_electrons() == 0) {
	//helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 14., 1.);
        //} else {
	//continue;
        //}

        // run gen factory to get matches
        gens.run_factory();

        // separate DY processes
        auto dy_process = gens.DY_process(electrons.getP4(),muon.getP4());
        if (name == "ZL" && dy_process != DY::ZL) {
            continue;
        } else if (name == "ZTT" && dy_process != DY::ZTT) {
            continue;
        } else if (name == "ZJ" && dy_process != DY:: ZJ) {
            continue;
        } else {
            helper->create_and_fill("cutflow", {15, 0.5, 15.5}, 15., 1.);
        }

        auto st = jets.getST(30.);

        auto Higgs = muon.getP4() + electrons.getP4() + met.getP4();
        std::shared_ptr<std::vector<double>> weights(nullptr);
        std::vector<std::string> tree_cat;

        // fill categories
        if (electrons.getIso() < 0.15 && muon.getIso() < 0.15) {
            tree_cat.push_back("signal");
        } else if (electrons.getIso() >= 0.15 && muon.getIso() < 0.15) {
            tree_cat.push_back("antiTauIso");
        } else if (electrons.getIso() < 0.15 && muon.getIso() >= 0.15) {
            tree_cat.push_back("antiLepIso");
        } else if (electrons.getIso() >= 0.15 && muon.getIso() >= 0.15) {
            tree_cat.push_back("antiBothIso");
        }

        // opposite-sign?
        if (electrons.getCharge() * muon.getCharge() < 0) {
            tree_cat.push_back("OS");
        }

        // Apply scale factors once we know the event has passed selection
        if (!isData) {
            // generator weights
            // evtwt *= event.getGenWeight();

            // muon trigger efficiency
            evtwt *= mu_trg_corr->GetBinContent(mu_trg_corr->GetXaxis()->FindBin(std::min(muon.getPt(), static_cast<Float_t>(1000.))), mu_trg_corr->GetYaxis()->FindBin(fabs(muon.getEta())));

            // // muon ID
            evtwt *= mu_id_corr->GetBinContent(mu_id_corr->GetXaxis()->FindBin(std::min(muon.getPt(), static_cast<Float_t>(100.))), mu_id_corr->GetYaxis()->FindBin(fabs(muon.getEta())));

            // // muon isolation
            evtwt *= mu_iso_corr->GetBinContent(mu_iso_corr->GetXaxis()->FindBin(std::min(muon.getPt(), static_cast<Float_t>(100.))), mu_iso_corr->GetYaxis()->FindBin(fabs(muon.getEta())));

            // muon tracking
            evtwt *= helper->muon_tracking(muon.getEta());
        }

        // store outputs in TTree
        output_tree->generalFill(tree_cat, &jets, &met, &event, evtwt, Higgs, mt, weights);
        output_tree->fillTree(&electrons, &muon, &event, name);
    }
    fin->Close();
    fout->cd();
    fout->Write();
    fout->Close();
    running_log << "Finished processing " << sample << std::endl;
    return 0;
}
