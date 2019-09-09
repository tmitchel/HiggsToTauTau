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
        lumi_weights = new reweight::LumiReWeighting("~/public/pu_distributions_mc_2017.root", "~/public/pu_distributions_data_2017.root", ("pua/#" +
        datasetName).c_str(), "pileup");
        logfile << "using PU dataset name: " << datasetName << std::endl;
    }

    // H->tau tau scale factors
    TFile htt_sf_file("data/htt_scalefactors_2017_v2.root");
    RooWorkspace* htt_sf = reinterpret_cast<RooWorkspace*>(htt_sf_file.Get("w"));
    htt_sf_file.Close();

    // tau ID efficiency
    TauIDSFTool *tau_id_eff_sf = new TauIDSFTool(2017);

    // embedded sample weights
    TFile embed_file("data/htt_scalefactors_v17_6.root", "READ");
    RooWorkspace* wEmbed = reinterpret_cast<RooWorkspace*>(embed_file.Get("w"));
    embed_file.Close();

    TFile bTag_eff_file("data/tagging_efficiencies_march2018_btageff-all_samp-inc-DeepCSV_medium.root", "READ");
    TH2F* btag_eff_b = reinterpret_cast<TH2F*>(bTag_eff_file.Get("btag_eff_b")->Clone());
    TH2F* btag_eff_c = reinterpret_cast<TH2F*>(bTag_eff_file.Get("btag_eff_c")->Clone());
    TH2F* btag_eff_oth = reinterpret_cast<TH2F*>(bTag_eff_file.Get("btag_eff_oth")->Clone());

    auto el_id_sf = new ScaleFactor();
    el_id_sf->init_ScaleFactor("${CMSSW_BASE}/src/HTT-utilities/LepEffInterface/data/Electron/Run2017/Electron_Run2017_IdIso.root");

    auto el32_el35_trg_sf = new ScaleFactor();
    el32_el35_trg_sf->init_ScaleFactor(
        "${CMSSW_BASE}/src/HTT-utilities/LepEffInterface/data/Electron/Run2017/Electron_Ele32orEle35.root");

    auto el_leg_cross_trg_sf = new ScaleFactor();
    el_leg_cross_trg_sf->init_ScaleFactor(
        "${CMSSW_BASE}/src/HTT-utilities/LepEffInterface/data/Electron/Run2017/Electron_EleTau_Ele24.root");

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
    electron_factory electrons(ntuple, 2017);
    tau_factory taus(ntuple, 2017);
    int temp = 2017;
    if (doAC) {
      temp = 20172;
    }
    jet_factory jets(ntuple, temp, syst);
    met_factory met(ntuple, 2017, syst);

    if (sample.find("ggHtoTauTau125") != std::string::npos) {
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
                evtwt = 3.417;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 3.315;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 2.415;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 2.358;
            } else {
                evtwt = 27.830;
            }
        }

        if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
            if (event.getNumGenJets() == 1) {
                evtwt = 0.805;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 1.044;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 1.871;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 0.249;
            } else {
                evtwt = 2.924;
            }
        }

        histos->at("cutflow")->Fill(1., 1.);

        auto electron = electrons.run_factory();
        auto tau = taus.run_factory();
        jets.run_factory();

        if (fabs(electron.getEta()) < 2.1) {
            histos->at("cutflow")->Fill(2., 1.);
        } else {
            continue;
        }

        // if (event.getPassFlags(isData)) {
        //     histos->at("cutflow")->Fill(5., 1.);
        // } else {
        //     continue;
        // }

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
        double mt = sqrt(pow(electron.getPt() + met_pt, 2) - pow(electron.getPx() + met_x, 2) - pow(electron.getPy() + met_y, 2));

        // now do mt selection
        if (mt < 50) {
            histos->at("cutflow")->Fill(5., 1.);
        } else {
            continue;
        }

        // apply all scale factors/corrections/etc.
        if (!isData && !isEmbed) {
            // tau ID efficiency SF
            if (tau.getGenMatch() == 5) {
                evtwt *= tau_id_eff_sf->getSFvsPT(tau.getPt());
            }

            // anti-lepton discriminator SFs
            if (tau.getGenMatch() == 1 || tau.getGenMatch() == 3) {
                if (fabs(tau.getEta()) < 1.460)
                    evtwt *= 1.80;
                else if (fabs(tau.getEta()) > 1.558)
                    evtwt *= 1.53;
            } else if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
                if (fabs(tau.getEta()) < 0.4)
                    evtwt *= 1.06;
                else if (fabs(tau.getEta()) < 0.8)
                    evtwt *= 1.02;
                else if (fabs(tau.getEta()) < 1.2)
                    evtwt *= 1.10;
                else if (fabs(tau.getEta()) < 1.7)
                    evtwt *= 1.03;
                else
                    evtwt *= 1.94;
            }

            // electron ID/Iso
            evtwt *= el_id_sf->get_ScaleFactor(electron.getPt(), electron.getEta());

            // trigger scale factors
            if (electron.getPt() < 33) {
                if (fabs(tau.getEta()) > 2.1) continue;
                evtwt *= el_leg_cross_trg_sf->get_ScaleFactor(electron.getPt(), electron.getEta());
                evtwt *= tau_leg_cross_trg_sf->getTriggerScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), tau.getDecayMode());
            } else {
                evtwt *= el32_el35_trg_sf->get_ScaleFactor(electron.getPt(), electron.getEta());
            }

            // Z-Vtx HLT Correction
            evtwt *= 0.991;

            // electron track reco SF
            htt_sf->var("e_pt")->setVal(electron.getPt());
            htt_sf->var("e_eta")->setVal(electron.getEta());
            evtwt *= htt_sf->function("e_trk_ratio")->getVal();

            // use promote-demote method to correct nbtag with no systematics
            jets.promoteDemote(btag_eff_oth, btag_eff_oth, btag_eff_oth);

            // pileup reweighting
            if (!doAC && !isMG) {
                evtwt *= lumi_weights->weight(event.getNPV());
            }

            // Z-pT Reweighting
            if (name == "EWKZLL" || name == "EWKZNuNu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
                // give inputs to workspace
                htt_sf->var("z_gen_mass")->setVal(event.getGenM());  // TODO(tmitchel): check if these are the right variables.
                htt_sf->var("z_gen_pt")->setVal(event.getGenPt());   // TODO(tmitchel): check if these are the right variables.
                evtwt *= htt_sf->function("zptmass_weight_nom")->getVal();
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

            // generator weights
            evtwt *= event.getGenWeight();

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
            // tau ID eff SF
            if (tau.getGenMatch() == 5) {
                evtwt *= 0.97;
            }
            // set workspace variables
            wEmbed->var("t_pt")->setVal(tau.getPt());
            wEmbed->var("e_pt")->setVal(electron.getPt());
            wEmbed->var("e_eta")->setVal(electron.getEta());
            wEmbed->var("e_iso")->setVal(electron.getIso());
            wEmbed->var("gt1_pt")->setVal(electron.getPt());
            wEmbed->var("gt1_eta")->setVal(electron.getEta());
            wEmbed->var("gt2_pt")->setVal(tau.getPt());
            wEmbed->var("gt2_eta")->setVal(tau.getEta());

            // double muon trigger eff in selection
            evtwt *= wEmbed->function("m_sel_trg_ratio")->getVal();

            // muon ID eff in selectionm
            evtwt *= wEmbed->function("m_sel_idEmb_ratio")->getVal();

            // electron ID SF
            evtwt *= wEmbed->function("e_id90_kit_ratio")->getVal();

            // electron iso SF
            evtwt *= wEmbed->function("e_iso_kit_ratio")->getVal();

            // apply trigger SF's
            bool fireSingle = electron.getPt() > 33;
            bool fireCross = electron.getPt() < 33;
            auto single_eff(1.), el_leg_eff(1.), tau_leg_eff(1.);
            if (fabs(electron.getEta()) < 1.479) {
                // apply trigger now
                if (!fireSingle && !fireCross) {
                    continue;
                }
                single_eff = wEmbed->function("e_trg27_trg32_trg35_embed_kit_ratio")->getVal();
                el_leg_eff = wEmbed->function("e_trg_EleTau_Ele24Leg_kit_ratio_embed")->getVal();
                tau_leg_eff = wEmbed->function("et_emb_LooseChargedIsoPFTau30_kit_ratio")->getVal();
                evtwt *= (single_eff * fireSingle + el_leg_eff * tau_leg_eff * fireCross);
            } else {
                // don't actually apply the trigger
                single_eff = wEmbed->function("e_trg27_trg32_trg35_kit_data")->getVal();
                el_leg_eff = wEmbed->function("e_trg_EleTau_Ele24Leg_desy_data")->getVal();
                if (fabs(tau.getEta()) < 2.1) {
                    tau_leg_eff = tau_leg_cross_trg_sf->getTriggerScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), tau.getDecayMode());
                }
                evtwt *= (single_eff * fireSingle + el_leg_eff * tau_leg_eff * fireCross);
            }

            auto genweight(event.getGenWeight());
            if (genweight > 1 || genweight < 0) {
                genweight = 0;
            }
            evtwt *= genweight;
        }

        fout->cd();

        // b-jet veto
        if (jets.getNbtag() == 0) {
            histos->at("cutflow")->Fill(6., 1.);
        } else {
            continue;
        }

        // create regions
        bool signalRegion = (tau.getTightIsoMVA() && electron.getIso() < 0.15);
        bool looseIsoRegion = (tau.getMediumIsoMVA() && electron.getIso() < 0.30);
        bool antiIsoRegion = (tau.getTightIsoMVA() && electron.getIso() > 0.15 && electron.getIso() < 0.30);
        bool antiTauIsoRegion = (tau.getTightIsoMVA() == 0 && electron.getIso() < 0.15);

        // create categories
        bool zeroJet = (jets.getNjets() == 0);
        bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300));
        bool vbfCat = (jets.getNjets() > 1 && jets.getDijetMass() > 300);
        bool VHCat = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

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
        } else if (antiIsoRegion) {
            tree_cat.push_back("antiLepIso");
        } else if (antiTauIsoRegion) {
            tree_cat.push_back("antiTauIso");
        }

        if (looseIsoRegion) {
            tree_cat.push_back("looseIso");
        }

        // categorization
        if (zeroJet) {
            tree_cat.push_back("0jet");
        } else if (boosted) {
            tree_cat.push_back("boosted");
        } else if (vbfCat) {
            tree_cat.push_back("vbf");
        } else if (VHCat) {
            tree_cat.push_back("VH");
        }

        // event charge
        if (evt_charge == 0) {
            tree_cat.push_back("OS");
        } else {
            tree_cat.push_back("SS");
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
