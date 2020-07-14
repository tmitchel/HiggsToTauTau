// Copyright [2020] Tyler Mitchell

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
#include "../../include/fsa/ditau_factory.h"
#include "../../include/slim_tree.h"
#include "../../include/swiss_army_class.h"

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
    
    // Added to make my life easier :)
    bool skip = sample.find("_ac_") != std::string::npos || sample.find("JHU") != std::string::npos;
    if (skip) {
      std::cout << "Skipping" << std::endl;
      return 0;
    }
      
    // get systematic shift name
    // We change the tau energy (for example) and see how the analyis changes
    // to get an estimate for errors
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
    auto ntuple = reinterpret_cast<TTree *>(fin->Get("tt_tree"));

    // get number of generated events
    auto counts = reinterpret_cast<TH1D *>(fin->Get("nevents"));
    auto gen_number = counts->GetBinContent(2);

    auto fout = new TFile(filename.c_str(), "RECREATE");
    counts->Write();
    fout->mkdir("grabbag");
    fout->cd("grabbag");

    // initialize Helper class
    Helper *helper = new Helper(fout, name, syst);

    // cd to root of output file and create tree
    fout->cd();
    // this is important...
    slim_tree *st = new slim_tree("tt_tree", doAC);

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
    

    // reweighter for anomolous coupling samples
    // This causes issues with JHU
    if (signal_type != "JHU") {
      ACWeighter ac_weights = ACWeighter(original, sample, signal_type, "2018");
      ac_weights.fillWeightMap();
    }

    // get normalization (lumi & xs are in util.h)
    double norm(1.);
    if (!isData && !isEmbed) {
        norm = helper->getLuminosity2018() * helper->getCrossSection(sample) / gen_number;
    }

    ///////////////////////////////////////////////
    // Scale Factors:                            //
    // Read weights, hists, graphs, etc. for SFs //
    ///////////////////////////////////////////////
    
    /*
    reweight::LumiReWeighting *lumi_weights;
    // read inputs for lumi reweighting
    if (!isData && !isEmbed && !doAC && !isMG) {
      TNamed *dbsName = reinterpret_cast<TNamed *>(fin->Get("MiniAOD_name"));
      std::string datasetName = dbsName->GetTitle();
      if (datasetName.find("Not Found") != std::string::npos && !isEmbed && !isData) {
	fin->Close();
	fout->Close();
	return 2;
      }
      std::cout << "here1" << std::endl;
      std::replace(datasetName.begin(), datasetName.end(), '/', '#');
      lumi_weights = new reweight::LumiReWeighting("root://cmsxrootd.fnal.gov//store/user/tmitchel/HTT_ScaleFactors/pu_distributions_mc_2017.root",
						   "root://cmsxrootd.fnal.gov//store/user/tmitchel/HTT_ScaleFactors/pu_distributions_data_2017.root",
						   ("pua/#" + datasetName).c_str(), "pileup");
      running_log << "using PU dataset name: " << datasetName << std::endl;
    }

    */
    
    // legacy sf's
    auto htt_sf_file = TFile::Open("root://cmsxrootd.fnal.gov//store/user/tmitchel/HTT_ScaleFactors/htt_scalefactors_legacy_2018.root");
    RooWorkspace *htt_sf = reinterpret_cast<RooWorkspace *>(htt_sf_file->Get("w"));
    htt_sf_file->Close();
    
    //////////////////////////////////////
    // Final setup:                     //
    // Declare histograms and factories //
    //////////////////////////////////////
    // construct factories
    event_factory event(ntuple, isData, lepton::MUON, 2018, isMG, syst);
    ditau_factory taus(ntuple);
    jet_factory jets(ntuple, 2018, syst);
    met_factory met(ntuple, 2018, syst);

    if (sample == "ggh125" && signal_type == "powheg") {
        event.setRivets(ntuple);
    }

    // begin the event loop
    Int_t nevts = ntuple->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (Int_t i = 0; i < nevts; i++) {
      ntuple->GetEntry(i);
      if (i == progress * fraction) {
	running_log << "LOG: Processing: " << progress * 10 << "% complete." << std::endl;
	progress++;
      }

      if (i%1000 == 0)
	std::cout << "Event Number " << i << std::endl;

      // find the event weight (not lumi*xs if looking at W or Drell-Yan)
      Float_t evtwt(norm), corrections(1.), sf_trig(1.), sf_id(1.), sf_iso(1.), sf_reco(1.);
      if (name == "W") {
	if (event.getNumGenJets() == 1) {
	  evtwt = 9.679;
	} else if (event.getNumGenJets() == 2) {
	  evtwt = 4.808;
	} else if (event.getNumGenJets() == 3) {
	  evtwt = 3.290;
	} else if (event.getNumGenJets() == 4) {
	  evtwt = 3.435;
	} else {
	  evtwt = 55.160;
	}
      }

      if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
	if (event.getNumGenJets() == 1) {
	  evtwt = 0.671;
	} else if (event.getNumGenJets() == 2) {
	  evtwt = 0.589;
	} else if (event.getNumGenJets() == 3) {
	  evtwt = 0.640;
	} else if (event.getNumGenJets() == 4) {
	  evtwt = 0.885;
	} else {
	  evtwt = 3.867;
	}
      }
      helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 1, 1.);

      // run factories
      taus.run_factory();
      jets.run_factory();
      event.setNjets(jets.getNjets());

      auto ltau = taus.tau_at(0);
      auto stau = taus.tau_at(1);

      // First tau ID selection
      if (ltau.getAgainstMuonDeepWP(wps::deep_vvvloose) > 0.5) { 
       	helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 2, 1.);
      } else {
       	continue;
      }

      if (ltau.getAgainstElectronDeepWP(wps::deep_vloose) > 0.5) {
	helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 3, 1.);
      } else {
       	continue;
      }

      // Second tau ID selection
      if (stau.getAgainstMuonDeepWP(wps::deep_vvvloose) > 0.5) { 
       	helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 4, 1.);
      } else {
       	continue;
      }

      if (stau.getAgainstElectronDeepWP(wps::deep_vloose) > 0.5) {
	helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 5, 1.);
      } else {
       	continue;
      }

      // only opposite-sign
      int evt_charge = ltau.getCharge() + stau.getCharge();
      if (evt_charge == 0) {
	helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 6, 1.);
      } else {
	continue;
      }

      // build Higgs
      TLorentzVector Higgs = ltau.getP4() + stau.getP4() + met.getP4();

      // calculate mt
      // I don't know how to do this for ditau, need help, this was copied from mt
      // double met_x = met.getMet() * cos(met.getMetPhi());
      // double met_y = met.getMet() * sin(met.getMetPhi());
      // double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
      // double mt = sqrt(pow(muon.getPt() + met_pt, 2) - pow(muon.getP4().Px() + met_x, 2) - pow(muon.getP4().Py() + met_y, 2));
      double mt = 1;
      // mt selection would go here

      // create regions
      bool signalRegion = (ltau.getDeepIsoWP(wps::deep_medium) && stau.getDeepIsoWP(wps::deep_medium));
      bool antiTauIsoRegion = (ltau.getDeepIsoWP(wps::deep_medium) == 0 && stau.getDeepIsoWP(wps::deep_medium) && 
			       ltau.getDeepIsoWP(wps::deep_vvvloose) > 0 && stau.getDeepIsoWP(wps::deep_vvvloose) > 0);
      if (signal_type != "None") {
	antiTauIsoRegion = false;  // don't need anti-tau iso region in signal
      }
      
      // only keep the regions we need
      if (signalRegion || antiTauIsoRegion) {
	helper->create_and_fill("cutflow", {8, 0.5, 8.5}, 7, 1.);
      } else {
	continue;
      }

      if (!isData && !isEmbed) {
            // lead tau id
            htt_sf->var("t_dm")->setVal(ltau.getDecayMode());
            if (ltau.getDecayMode() == 5) {
                evtwt *= htt_sf->function("t_deeptauid_dm_medium")->getVal();
            }

            // sublead tau id
            htt_sf->var("t_dm")->setVal(stau.getDecayMode());
            if (stau.getDecayMode() == 5) {
                evtwt *= htt_sf->function("t_deeptauid_dm_medium")->getVal();
            }

            // trigger scale factors
            htt_sf->var("t_pt")->setVal(ltau.getPt());
            htt_sf->var("t_eta")->setVal(ltau.getEta());
            htt_sf->var("t_phi")->setVal(ltau.getPhi());
            htt_sf->var("t_dm")->setVal(ltau.getDecayMode());
            // trigger sf applied here ...

            // top-pT Reweighting
            if (name == "TTT" || name == "TTJ" || name == "TTL" || name == "STT" || name == "STJ" || name == "STL") {
                float pt_top1 = std::min(static_cast<float>(400.), jets.getTopPt1());
                float pt_top2 = std::min(static_cast<float>(400.), jets.getTopPt2());
                if (syst == "ttbarShape_Up") {
                    evtwt *= (2 * sqrt(exp(0.0615 - 0.0005 * pt_top1) * exp(0.0615 - 0.0005 * pt_top2)) - 1);  // 2*√[e^(..)*e^(..)] - 1
                } else if (syst == "ttbarShape_Up") {
                    // no weight for shift down
                } else {
                    evtwt *= sqrt(exp(0.0615 - 0.0005 * pt_top1) * exp(0.0615 - 0.0005 * pt_top2));  // √[e^(..)*e^(..)]
                }
            }
      } else if (!isData && isEmbed) {
            // embedded generator weights
            auto genweight(event.getGenWeight());
            if (genweight > 1 || genweight < 0) {
                genweight = 0;
            }
            evtwt *= genweight;

            // embedded tracker correction
            evtwt *= helper->embed_tracking(ltau.getDecayMode());
            evtwt *= helper->embed_tracking(stau.getDecayMode());

            // lead tau id
            htt_sf->var("t_dm")->setVal(ltau.getDecayMode());
            if (ltau.getDecayMode() == 5) {
                evtwt *= htt_sf->function("t_deeptauid_dm_embed_medium")->getVal();
            }

            // sublead tau id
            htt_sf->var("t_dm")->setVal(stau.getDecayMode());
            if (stau.getDecayMode() == 5) {
                evtwt *= htt_sf->function("t_deeptauid_dm_embed_medium")->getVal();
            }

            // trigger scale factors
            htt_sf->var("t_pt")->setVal(ltau.getPt());
            htt_sf->var("t_eta")->setVal(ltau.getEta());
            htt_sf->var("t_phi")->setVal(ltau.getPhi());
            htt_sf->var("t_dm")->setVal(ltau.getDecayMode());
            // trigger sf applied here ...

            // embedded scale factors
            htt_sf->var("gt1_pt")->setVal(ltau.getGenPt());
            htt_sf->var("gt1_eta")->setVal(ltau.getGenEta());
            htt_sf->var("gt2_pt")->setVal(stau.getGenPt());
            htt_sf->var("gt2_eta")->setVal(stau.getGenEta());

            // double muon trigger eff in selection
            evtwt *= htt_sf->function("m_sel_trg_ratio")->getVal();

            // muon ID eff in selection (leg 1)
            htt_sf->var("gt_pt")->setVal(ltau.getGenPt());
            htt_sf->var("gt_eta")->setVal(ltau.getGenEta());
            evtwt *= htt_sf->function("m_sel_id_ic_ratio")->getVal();

            // muon ID eff in selection (leg 2)
            htt_sf->var("gt_pt")->setVal(stau.getGenPt());
            htt_sf->var("gt_eta")->setVal(stau.getGenEta());
            evtwt *= htt_sf->function("m_sel_id_ic_ratio")->getVal();
      }
      
      std::vector<std::string> tree_cat;
      // regions
      if (signalRegion) {
	tree_cat.push_back("signal");
      } else if (antiTauIsoRegion) {
	tree_cat.push_back("antiTauIso");
      }
      
      // event charge
      if (evt_charge == 0) {
	tree_cat.push_back("OS");
      }

      // The slim_tree does a nullptr check on the weights, so I think I can skip all of this?
      std::shared_ptr<std::vector<double>> weights(nullptr);
      /*
      Long64_t currentEventID = event.getLumi();
      currentEventID = currentEventID * 1000000 + event.getEvt();
      if (doAC) {
	// Problem here, because JHU skips the weights part :( so ac_weights will be NULL
	weights = std::make_shared<std::vector<double>>(ac_weights.getWeights(currentEventID));
      }
      */

      // Fill Trees
      // slim_tree class is used here (st is object)
      // st->generalFill(tree_cat, &jets, &met, &event, evtwt, Higgs, mt, weights);
      st->generalFill(tree_cat, &jets, &met, &event, evtwt, Higgs, mt, weights);
      st->fillTree(&ltau, &stau, &event, name);
    }  // close event loop

    fin->Close();
    fout->cd();
    fout->Write();
    fout->Close();
    running_log << "Finished processing " << sample << std::endl;
    if (!condor) {
      logfile.close();
    }
    return 0;
}
