// Copyright [2018] Tyler Mitchell

// system includes
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>

// ROOT includes
#include "RooFunctor.h"
#include "RooMsgService.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

// user includes
#include "../include/ACWeighter.h"
#include "../include/CLParser.h"
#include "../include/EmbedWeight.h"
#include "../include/LumiReweightingStandAlone.h"
#include "../include/SF_factory.h"
#include "../include/electron_factory.h"
#include "../include/event_info.h"
#include "../include/jet_factory.h"
#include "../include/met_factory.h"
#include "../include/muon_factory.h"
#include "../include/slim_tree.h"
#include "../include/swiss_army_class.h"
#include "../include/tau_factory.h"
#include "HTT-utilities/LepEffInterface/interface/ScaleFactor.h"

typedef std::vector<double> NumV;

int main(int argc, char *argv[]) {
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
    auto ntuple = reinterpret_cast<TTree *>(fin->Get("etau_tree"));

    // get number of generated events
    auto counts = reinterpret_cast<TH1D *>(fin->Get("nevents"));
    auto gen_number = counts->GetBinContent(2);

    // create output file
    auto fout = new TFile(filename.c_str(), "RECREATE");
    counts->Write();
    fout->mkdir("grabbag");
    fout->cd("grabbag");

    // initialize Helper class
    Helper *helper = new Helper(fout, name, syst);

    // cd to root of output file and create tree
    fout->cd();
    slim_tree *st = new slim_tree("et_tree", doAC);

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
    ACWeighter ac_weights = ACWeighter(original, sample, signal_type, "2016");
    ac_weights.fillWeightMap();

    // get normalization (lumi & xs are in util.h)
    double norm(1.);
    if (!isData && !isEmbed) {
        norm = helper->getLuminosity2016() * helper->getCrossSection(sample) / gen_number;
    }

    ///////////////////////////////////////////////
    // Scale Factors:                            //
    // Read weights, hists, graphs, etc. for SFs //
    ///////////////////////////////////////////////

    // read inputs for lumi reweighting
    auto lumi_weights =
        new reweight::LumiReWeighting("data/MC_Moriond17_PU25ns_V1.root", "data/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

    // Z-pT reweighting
    TFile *zpt_file = new TFile("data/zpt_weights_2016_BtoH.root");
    auto zpt_hist = reinterpret_cast<TH2F *>(zpt_file->Get("zptmass_histo"));

    // embedded sample weights
    TFile embed_file("data/htt_scalefactors_v16_9_embedded.root", "READ");
    RooWorkspace *wEmbed = reinterpret_cast<RooWorkspace *>(embed_file.Get("w"));
    embed_file.Close();

    // trigger and ID scale factors
    auto ele25_trg_sf = new ScaleFactor();
    ele25_trg_sf->init_ScaleFactor(
        "${CMSSW_BASE}/src/HTT-utilities/LepEffInterface/data/Electron/Run2016_legacy/Electron_Run2016_legacy_Ele25.root");

    auto ele_id_sf = new ScaleFactor();
    ele_id_sf->init_ScaleFactor(
        "${CMSSW_BASE}/src/HTT-utilities/LepEffInterface/data/Electron/Run2016_legacy/Electron_Run2016_legacy_IdIso.root");

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
    event_info event(ntuple, lepton::ELECTRON, 2016, syst);
    electron_factory electrons(ntuple, 2016);
    tau_factory taus(ntuple, 2016);
    jet_factory jets(ntuple, 2016, syst);
    met_factory met(ntuple, 2016, syst);

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
                evtwt = 7.236;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 4.029;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 1.079;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 2.121;
            } else {
                evtwt = 28.84;
            }
        }

        if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
            if (event.getNumGenJets() == 1) {
                evtwt = 0.486;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 0.533;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 0.492;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 0.408;
            } else {
                evtwt = 1.470;
            }
        }
        histos->at("cutflow")->Fill(1., 1.);

        auto electron = electrons.run_factory();
        auto tau = taus.run_factory();
        jets.run_factory();

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
            histos->at("cutflow")->Fill(2., 1.);
        }

        // only opposite-sign
        int evt_charge = tau.getCharge() + electron.getCharge();
        if (evt_charge == 0) {
            histos->at("cutflow")->Fill(3., 1.);
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
            histos->at("cutflow")->Fill(4., 1.);
        } else {
            continue;
        }

        // apply all scale factors/corrections/etc.
        if (!isData && !isEmbed) {
            // tau ID efficiency SF
            if (tau.getGenMatch() == 5) {
                evtwt *= 0.87;
            }

            // anti-lepton discriminator SFs
            if (tau.getGenMatch() == 1 || tau.getGenMatch() == 3) {
                if (fabs(tau.getEta()) < 1.460) {
                    evtwt *= 1.40;
                } else if (fabs(tau.getEta()) > 1.558) {
                    evtwt *= 1.90;
                }
            } else if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
                if (fabs(tau.getEta()) < 0.4) {
                    evtwt *= 1.22;
                } else if (fabs(tau.getEta()) < 0.8) {
                    evtwt *= 1.12;
                } else if (fabs(tau.getEta()) < 1.2) {
                    evtwt *= 1.26;
                } else if (fabs(tau.getEta()) < 1.7) {
                    evtwt *= 1.22;
                } else {
                    evtwt *= 2.39;
                }
            }

            // electron ID SF
            evtwt *= ele_id_sf->get_ScaleFactor(electron.getPt(), electron.getEta());

            // Trigger SF
            evtwt *= ele25_trg_sf->get_ScaleFactor(electron.getPt(), electron.getEta());

            // b-tagging scale factor goes here
            // evtwt *= btagsf;

            // Pileup Reweighting
            evtwt *= lumi_weights->weight(event.getNPU());

            // NNLOPS ggH reweighting
            if (sample.find("ggH125") != std::string::npos) {
                if (event.getNjetsRivet() == 0) evtwt *= g_NNLOPS_0jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(125.0)));
                if (event.getNjetsRivet() == 1) evtwt *= g_NNLOPS_1jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(625.0)));
                if (event.getNjetsRivet() == 2) evtwt *= g_NNLOPS_2jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(800.0)));
                if (event.getNjetsRivet() >= 3) evtwt *= g_NNLOPS_3jet->Eval(std::min(event.getHiggsPtRivet(), static_cast<float>(925.0)));
            }
            // NumV WG1unc = qcd_ggF_uncert_2017(Rivet_nJets30, Rivet_higgsPt, Rivet_stage1_cat_pTjet30GeV);

            // Z-pT and Zmm Reweighting
            if (name == "EWKZLL" || name == "EWKZNuNu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
                // Z-pT Reweighting
                auto nom_zpt_weight =
                    zpt_hist->GetBinContent(zpt_hist->GetXaxis()->FindBin(event.getGenM()), zpt_hist->GetYaxis()->FindBin(event.getGenPt()));
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

            // Apply generator weights
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
            double Stitching_Weight(1.);
            // get the stitching weight
            if (event.getRun() >= 272007 && event.getRun() < 275657) {
                Stitching_Weight = (1.0 / 0.902);
            } else if (event.getRun() < 276315) {
                Stitching_Weight = (1.0 / 0.910);
            } else if (event.getRun() < 276831) {
                Stitching_Weight = (1.0 / 0.945);
            } else if (event.getRun() < 277772) {
                Stitching_Weight = (1.0 / 0.945);
            } else if (event.getRun() < 278820) {
                Stitching_Weight = (1.0 / 0.915);
            } else if (event.getRun() < 280919) {
                Stitching_Weight = (1.0 / 0.903);
            } else if (event.getRun() < 284045) {
                Stitching_Weight = (1.0 / 0.933);
            }

            // get correction factor
            std::vector<double> corrFactor = EmdWeight_Electron(wEmbed, electron.getPt(), electron.getEta(), electron.getIso());
            double totEmbedWeight(corrFactor[2] * corrFactor[5] * corrFactor[6]);  // id SF, iso SF, trg eff. SF

            // data to mc trigger ratio
            double trg_ratio(m_sel_trg_ratio(wEmbed, electron.getPt(), electron.getEta(), tau.getPt(), tau.getEta()));

            auto genweight(event.getGenWeight());
            if (genweight > 1 || genweight < 0) {
                genweight = 0;
            }
            evtwt *= (Stitching_Weight * totEmbedWeight * trg_ratio * genweight);

            // scale-up tau pT
            if (tau.getGenMatch() == 5) {
                tau.scalePt(1.02);
            }
        }

        fout->cd();

        // // b-jet veto
        if (jets.getNbtag() == 0) {
            histos->at("cutflow")->Fill(5., 1.);
        } else {
            continue;
        }

        // create regions
        bool signalRegion = (tau.getTightIsoMVA() && electron.getIso() < 0.10);
        bool looseIsoRegion = (tau.getMediumIsoMVA() && electron.getIso() < 0.30);
        bool antiIsoRegion = (tau.getTightIsoMVA() && electron.getIso() > 0.10 && electron.getIso() < 0.30);
        bool antiTauIsoRegion = (tau.getTightIsoMVA() == 0 && electron.getIso() < 0.10);

        // create categories
        bool zeroJet = (jets.getNjets() == 0);
        bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300));
        bool vbfCat = (jets.getNjets() > 1 && jets.getDijetMass() > 300);
        bool VHCat = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

        // only keep the regions we need
        if (signalRegion || antiTauIsoRegion)  {
            histos->at("cutflow")->Fill(6., 1.);
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
