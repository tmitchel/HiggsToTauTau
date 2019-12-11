// Copyright [2018] Tyler Mitchell

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
#include "../include/ComputeWG1Unc.h"
#include "../include/ACWeighter.h"
#include "../include/CLParser.h"
#include "../include/LumiReweightingStandAlone.h"
#include "../include/electron_factory.h"
#include "../include/event_info.h"
#include "../include/jet_factory.h"
#include "../include/met_factory.h"
#include "../include/muon_factory.h"
#include "../include/slim_tree.h"
#include "../include/swiss_army_class.h"
#include "../include/tau_factory.h"
#include "TauAnalysisTools/TauTriggerSFs/interface/TauTriggerSFs2017.h"
#include "HTT-utilities/LepEffInterface/interface/ScaleFactor.h"
#include "TauPOG/TauIDSFs/interface/TauIDSFTool.h"

typedef std::vector<double> NumV;

int main(int argc, char* argv[]) {
    ////////////////////////////////////////////////
    // Initial setup:                             //
    // Get file names, normalization, paths, etc. //
    ////////////////////////////////////////////////

    CLParser parser(argc, argv);
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

    std::string systname = "NOMINAL";
    if (!syst.empty()) {
        systname = "SYST_" + syst;
    }

    // create output path
    auto suffix = "_output.root";
    auto prefix = "Output/trees/" + output_dir + "/" + systname + "/";
    std::string filename, logname;
    if (name == sample) {
        filename = prefix + name + systname + suffix;
        logname = "Output/trees/" + output_dir + "/logs/" + name + systname + ".txt";
    } else {
        filename = prefix + sample + std::string("_") + name + "_" + systname + suffix;
        logname = "Output/trees/" + output_dir + "/logs/" + sample + std::string("_") + name + "_" + systname + ".txt";
    }

    // create the log file
    std::ofstream logfile;
    logfile.open(logname, std::ios::out | std::ios::trunc);

    // open log file and log some things
    logfile << "Opening file... " << sample << std::endl;
    logfile << "With name...... " << name << std::endl;
    logfile << "And running systematic " << systname << std::endl;
    logfile << "Using options: " << std::endl;
    logfile << "\t name: " << name << std::endl;
    logfile << "\t path: " << path << std::endl;
    logfile << "\t syst: " << syst << std::endl;
    logfile << "\t sample: " << sample << std::endl;
    logfile << "\t output_dir: " << output_dir << std::endl;
    logfile << "\t signal_type: " << signal_type << std::endl;
    logfile << "\t isData: " << isData << " isEmbed: " << isEmbed << " doAC: " << doAC << std::endl;

    auto fin = TFile::Open(fname.c_str());
    auto ntuple = reinterpret_cast<TTree*>(fin->Get("etau_tree"));

    // get number of generated events
    auto counts = reinterpret_cast<TH1D*>(fin->Get("nevents"));
    auto gen_number = counts->GetBinContent(2);

    // create output file
    auto fout = new TFile(filename.c_str(), "RECREATE");
    counts->Write();
    fout->mkdir("grabbag");
    fout->cd("grabbag");

    // initialize Helper class
    Helper* helper = new Helper(fout, name, syst);

    // cd to root of output file and create tree
    fout->cd();
    slim_tree* st = new slim_tree("et_tree", doAC);

    std::string original = sample;
    if (name == "VBF125") {
        sample = "vbf125";
    } else if (name == "ggH125") {
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
    ACWeighter ac_weights = ACWeighter(original, sample, signal_type, "2017");
    ac_weights.fillWeightMap();

    // get normalization (lumi & xs are in util.h)
    double norm(1.);
    if (!isData && !isEmbed) {
        norm = helper->getLuminosity2017() * helper->getCrossSection(sample) / gen_number;
    }

    ///////////////////////////////////////////////
    // Scale Factors:                            //
    // Read weights, hists, graphs, etc. for SFs //
    ///////////////////////////////////////////////

    reweight::LumiReWeighting* lumi_weights;
    // read inputs for lumi reweighting
    if (!isData && !isEmbed && !doAC && !isMG) {
        TNamed* dbsName = reinterpret_cast<TNamed*>(fin->Get("MiniAOD_name"));
        std::string datasetName = dbsName->GetTitle();
        if (datasetName.find("Not Found") != std::string::npos && !isEmbed && !isData) {
            fin->Close();
            fout->Close();
            return 2;
        }
        std::replace(datasetName.begin(), datasetName.end(), '/', '#');
        lumi_weights = new reweight::LumiReWeighting("data/pu_distributions_mc_2017.root", "data/pu_distributions_data_2017.root", ("pua/#" +
        datasetName).c_str(), "pileup");
        logfile << "using PU dataset name: " << datasetName << std::endl;
    }

    // legacy sf's
    TFile htt_sf_file("data/htt_scalefactors_legacy_2017.root");
    RooWorkspace *htt_sf = reinterpret_cast<RooWorkspace*>(htt_sf_file.Get("w"));
    htt_sf_file.Close();

    // tau ID efficiency
    TauIDSFTool *tau_id_eff_sf = new TauIDSFTool("2017ReReco");
    TauIDSFTool *antiEl_eff_sf = new TauIDSFTool("2017ReReco", "antiEleMVA6", "Tight");
    TauIDSFTool *antiMu_eff_sf = new TauIDSFTool("2017ReReco", "antiMu3", "Loose");

    TauTriggerSFs2017 *tau_leg_cross_trg_sf =
        new TauTriggerSFs2017("$CMSSW_BASE/src/TauAnalysisTools/TauTriggerSFs/data/tauTriggerEfficiencies2017.root", "etau", "2017", "tight", "MVAv2");

    TFile *f_NNLOPS = new TFile("data/NNLOPS_reweight.root");
    TGraph *g_NNLOPS_0jet = reinterpret_cast<TGraph *>(f_NNLOPS->Get("gr_NNLOPSratio_pt_powheg_0jet"));
    TGraph *g_NNLOPS_1jet = reinterpret_cast<TGraph *>(f_NNLOPS->Get("gr_NNLOPSratio_pt_powheg_1jet"));
    TGraph *g_NNLOPS_2jet = reinterpret_cast<TGraph *>(f_NNLOPS->Get("gr_NNLOPSratio_pt_powheg_2jet"));
    TGraph *g_NNLOPS_3jet = reinterpret_cast<TGraph *>(f_NNLOPS->Get("gr_NNLOPSratio_pt_powheg_3jet"));

    //////////////////////////////////////
    // Final setup:                     //
    // Declare histograms and factories //
    //////////////////////////////////////

    // declare histograms (histogram initializer functions in util.h)
    fout->cd("grabbag");
    auto histos = helper->getHistos1D();

    // construct factories
    event_info event(ntuple, lepton::ELECTRON, 2017, syst);
    electron_factory electrons(ntuple, 2017, syst);
    tau_factory taus(ntuple, 2017, syst);
    jet_factory jets(ntuple, 2017, syst);
    met_factory met(ntuple, 2017, syst);

    if (sample == "ggh125" && signal_type == "powheg") {
        event.setRivets(ntuple);
    }

    // begin the event loop
    Int_t nevts = ntuple->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (Int_t i = 0; i < nevts; i++) {
        ntuple->GetEntry(i);
        if (i == progress * fraction) {
            logfile << "LOG: Processing: " << progress * 10 << "% complete." << std::endl;
            progress++;
        }

        // find the event weight (not lumi*xs if looking at W or Drell-Yan)
        Float_t evtwt(norm), corrections(1.), sf_trig(1.), sf_id(1.), sf_iso(1.), sf_reco(1.);
        if (name == "W") {
            if (event.getNumGenJets() == 1) {
                evtwt = 3.656;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 3.383;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 2.145;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 1.954;
            } else {
                evtwt = 25.609;
            }
        }

        if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
            if (event.getNumGenJets() == 1) {
                evtwt = 0.710;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 0.921;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 1.651;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 0.220;
            } else {
                evtwt = 2.581;
            }
        }

        histos->at("cutflow")->Fill(1., 1.);

        auto electron = electrons.run_factory();
        auto tau = taus.run_factory();
        jets.run_factory();

        if (event.getPassFlags()) {
            histos->at("cutflow")->Fill(2., 1.);
        } else {
            continue;
        }

        if (fabs(electron.getEta()) > 2.1) {
          continue;
        }

        // Separate Drell-Yan
        if (name == "ZL" && tau.getGenMatch() > 4) {
            continue;
        } else if ((name == "ZTT" || name == "TTT" || name == "VVT") && tau.getGenMatch() != 5) {
            continue;
        } else if ((name == "ZLL" || name == "TTJ" || name == "VVJ") && tau.getGenMatch() == 5) {
            continue;
        } else if (name == "ZJ" && tau.getGenMatch() != 6) {
            continue;
        } else {
            histos->at("cutflow")->Fill(3., 1.);
        }

        // only opposite-sign
        int evt_charge = tau.getCharge() + electron.getCharge();
        if (evt_charge == 0) {
            histos->at("cutflow")->Fill(4., 1.);
        } else {
            continue;
        }

        // build Higgs
        TLorentzVector Higgs = electron.getP4() + tau.getP4() + met.getP4();

        // calculate mt
        double met_x = met.getMet() * cos(met.getMetPhi());
        double met_y = met.getMet() * sin(met.getMetPhi());
        double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
        double mt = sqrt(pow(electron.getPt() + met_pt, 2) - pow(electron.getP4().Px() + met_x, 2) - pow(electron.getP4().Py() + met_y, 2));

        // now do mt selection
        if (mt < 50) {
            histos->at("cutflow")->Fill(5., 1.);
        } else {
            continue;
        }

        // apply all scale factors/corrections/etc.
        if (!isData && !isEmbed) {
            // tau ID efficiency SF and systematics
            std::string shift = "";  // nominal
            if (syst.find("tau_id_") != std::string::npos) {
                shift = syst.find("Up")  != std::string::npos ? "Up" : "Down";
            }
            evtwt *= tau_id_eff_sf->getSFvsPT(tau.getPt(), tau.getGenMatch(), shift);

            // anti-lepton discriminator sf's
            evtwt *= antiEl_eff_sf->getSFvsEta(fabs(tau.getEta()), tau.getGenMatch());
            evtwt *= antiMu_eff_sf->getSFvsEta(fabs(tau.getEta()), tau.getGenMatch());

            // Z-Vtx HLT Correction
            evtwt *= 0.991;

            // pileup reweighting
            if (!doAC && !isMG) {
                evtwt *= lumi_weights->weight(event.getNPV());
            }

            // generator weights
            if (signal_type == "madgraph") {
                evtwt *= event.getGenWeight();
            }

            // b-tagging scale factor goes here
            evtwt *= jets.getBWeight();

            // set workspace variables
            htt_sf->var("e_pt")->setVal(electron.getPt());
            htt_sf->var("e_eta")->setVal(electron.getEta());
            htt_sf->var("t_pt")->setVal(tau.getPt());
            htt_sf->var("t_eta")->setVal(tau.getEta());
            htt_sf->var("t_phi")->setVal(tau.getPhi());
            htt_sf->var("t_dm")->setVal(tau.getDecayMode());
            htt_sf->var("z_gen_mass")->setVal(event.getGenM());
            htt_sf->var("z_gen_pt")->setVal(event.getGenPt());

            // start applying weights from workspace
            evtwt *= htt_sf->function("e_trk_ratio")->getVal();
            evtwt *= htt_sf->function("e_idiso_ic_ratio")->getVal();
            // evtwt *= htt_sf->function("t_deeptauid_pt_tight")->getVal(); (for DeepTau ID)

            // trigger scale factors
            if (electron.getPt() < 33) {
                evtwt *= htt_sf->function("e_trg_24_ic_ratio")->getVal();
                evtwt *= tau_leg_cross_trg_sf->getTriggerScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), tau.getDecayMode());
                // evtwt *= htt_sf->function("t_trg_mediumDeepTau_mutau_ratio")->getVal(); (for DeepTau ID)
            } else {
                evtwt *= htt_sf->function("e_trg_ic_ratio")->getVal();
            }

            // Z-pT Reweighting
            if (name == "EWKZ2l" || name == "EWKZ2nu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
                auto nom_zpt_weight = htt_sf->function("zptmass_weight_nom")->getVal();
                if (syst == "dyShape_Up") {
                    nom_zpt_weight = 1.1 * nom_zpt_weight - 0.1;
                } else if (syst == "dyShape_Down") {
                    nom_zpt_weight = 0.9 * nom_zpt_weight + 0.1;
                }
                evtwt *= nom_zpt_weight;
            }

            // top-pT Reweighting
            if (name == "TTT" || name == "TT" || name == "TTJ") {
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

            // NNLOPS ggH reweighting
            if (sample == "ggh125" && signal_type == "powheg") {
                if (event.getNjetsRivet() == 0) evtwt *= g_NNLOPS_0jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(125.0)));
                if (event.getNjetsRivet() == 1) evtwt *= g_NNLOPS_1jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(625.0)));
                if (event.getNjetsRivet() == 2) evtwt *= g_NNLOPS_2jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(800.0)));
                if (event.getNjetsRivet() >= 3) evtwt *= g_NNLOPS_3jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(925.0)));
                NumV WG1unc = qcd_ggF_uncert_2017(event.getNjetsRivet(), event.getHiggsPtRivet(), event.getJetPtRivet());
                if (syst.find("Rivet") != std::string::npos) {
                  evtwt *= (1 + event.getRivetUnc(WG1unc, syst));
                }
            }

            // begin systematics

            // jet to tau fake rate systematic
            if (tau.getGenMatch() == 6 && name == "TTJ" || name == "ZJ" || name == "W" || name == "VVJ") {
                auto temp_tau_pt = std::min(200., static_cast<double>(tau.getPt()));
                if (syst == "jetToTauFake_Up") {
                    evtwt *= (1 - (0.2 * temp_tau_pt / 100));
                } else if (syst == "jetToTauFake_Down") {
                    evtwt *= (1 + (0.2 * temp_tau_pt / 100));
                }
            }
        } else if (!isData && isEmbed) {
          event.setEmbed();
          if (electron.getPt() < 33 && !event.getPassEle24Tau30()) {
            continue;
          }

            // tau ID eff SF
            if (tau.getGenMatch() == 5) {
                evtwt *= 0.97;
            }

            auto genweight(event.getGenWeight());
            if (genweight > 1 || genweight < 0) {
                genweight = 0;
            }
            evtwt *= genweight;

            // set workspace variables
            htt_sf->var("e_pt")->setVal(electron.getPt());
            htt_sf->var("e_eta")->setVal(electron.getEta());
            htt_sf->var("t_pt")->setVal(tau.getPt());
            htt_sf->var("t_eta")->setVal(tau.getEta());
            htt_sf->var("t_phi")->setVal(tau.getPhi());
            htt_sf->var("t_dm")->setVal(tau.getDecayMode());
            htt_sf->var("gt1_pt")->setVal(electron.getGenPt());
            htt_sf->var("gt1_eta")->setVal(electron.getGenEta());
            htt_sf->var("gt2_pt")->setVal(tau.getGenPt());
            htt_sf->var("gt2_eta")->setVal(tau.getGenEta());

            evtwt *= htt_sf->function("e_trk_embed_ratio")->getVal();
            evtwt *= htt_sf->function("e_idiso_ic_embed_ratio")->getVal();

            if (electron.getPt() < 33) {
                if (fabs(electron.getEta()) < 1.479) {
                  evtwt *= htt_sf->function("e_trg_24_ic_embed_ratio")->getVal();
                }
                // evtwt *= tau_leg_cross_trg_sf->getTriggerScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), tau.getDecayMode());
                // evtwt *= htt_sf->function("t_trg_mediumDeepTau_etau_embed_ratio")->getVal(); (for DeepTau ID)
            } else {
                evtwt *= htt_sf->function("e_trg_ic_embed_ratio")->getVal();
            }

            // double muon trigger eff in selection
            evtwt *= htt_sf->function("m_sel_trg_ic_ratio")->getVal();

            // muon ID eff in selection (leg 1)
            htt_sf->var("gt_pt")->setVal(electron.getGenPt());
            htt_sf->var("gt_eta")->setVal(electron.getGenEta());
            evtwt *= htt_sf->function("m_sel_id_ic_ratio")->getVal();

            // muon ID eff in selection (leg 1)
            htt_sf->var("gt_pt")->setVal(tau.getGenPt());
            htt_sf->var("gt_eta")->setVal(tau.getGenEta());
            evtwt *= htt_sf->function("m_sel_id_ic_ratio")->getVal();
        }

        fout->cd();

        // b-jet veto
        if (jets.getNbtagLoose() < 2 && jets.getNbtagMedium() < 1) {
            histos->at("cutflow")->Fill(6., 1.);
        } else {
            continue;
        }

        // create regions
        bool signalRegion = (tau.getTightIsoMVA() && electron.getIso() < 0.15);
        bool antiTauIsoRegion = (tau.getTightIsoMVA() == 0 && electron.getIso() < 0.15);

        // only keep the regions we need
        if (signalRegion || antiTauIsoRegion)  {
            histos->at("cutflow")->Fill(7., 1.);
        } else {
            continue;
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

        std::shared_ptr<std::vector<double>> weights(nullptr);
        Long64_t currentEventID = event.getLumi();
        currentEventID = currentEventID * 1000000 + event.getEvt();
        if (doAC) {
            weights = std::make_shared<std::vector<double>>(ac_weights.getWeights(currentEventID));
        }

        // fill the tree
        st->fillTree(tree_cat, &electron, &tau, &jets, &met, &event, mt, evtwt, weights);
    }  // close event loop

    fin->Close();
    fout->cd();
    fout->Write();
    fout->Close();
    logfile << "Finished processing " << sample << std::endl;
    logfile.close();
    return 0;
}
