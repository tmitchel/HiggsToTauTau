// Copyright [2019] Tyler Mitchell

// system includes
#include <algorithm>
#include <cmath>
#include <iostream>
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
#include "../include/ZmmSF.h"
#include "../include/event_info.h"
#include "../include/jet_factory.h"
#include "../include/met_factory.h"
#include "../include/muon_factory.h"
#include "../include/slim_tree.h"
#include "../include/swiss_army_class.h"
#include "../include/tau_factory.h"
#include "TauAnalysisTools/TauTriggerSFs/interface/TauTriggerSFs2017.h"

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
    bool isMG = sample.find("madgraph") != std::string::npos;
    bool doAC = signal_type != "None";

    std::string systname = "NOMINAL";
    if (!syst.empty()) {
        systname = "SYST_" + syst;
    }

    // open input file
    std::cout << "Opening file... " << sample << std::endl;
    std::cout << "With name...... " << name << std::endl;
    if (!syst.empty()) {
        std::cout << "And running systematic " << systname << std::endl;
    }
    auto fin = TFile::Open(fname.c_str());
    auto ntuple = reinterpret_cast<TTree *>(fin->Get("mutau_tree"));

    // get number of generated events
    auto counts = reinterpret_cast<TH1D *>(fin->Get("nevents"));
    auto gen_number = counts->GetBinContent(2);

    // create output file
    auto suffix = "_output.root";
    auto prefix = "Output/trees/" + output_dir + "/" + systname + "/";
    std::string filename;
    if (name == sample) {
        filename = prefix + name + systname + suffix;
    } else {
        filename = prefix + sample + std::string("_") + name + "_" + systname + suffix;
    }
    auto fout = new TFile(filename.c_str(), "RECREATE");
    counts->Write();
    fout->mkdir("grabbag");
    fout->cd("grabbag");

    // initialize Helper class
    Helper *helper;
    if (isEmbed) {
        helper = new Helper(fout, "ZTT", syst);
    } else {
        helper = new Helper(fout, name, syst);
    }

    // cd to root of output file and create tree
    fout->cd();
    slim_tree *st = new slim_tree("mt_tree", doAC);

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
    ACWeighter ac_weights = ACWeighter(original, sample, signal_type, "2018");
    ac_weights.fillWeightMap();

    // get normalization (lumi & xs are in util.h)
    double norm(1.);
    if (!isData && !isEmbed) {
        norm = helper->getLuminosity2018() * helper->getCrossSection(sample) / gen_number;
    }

    ///////////////////////////////////////////////
    // Scale Factors:                            //
    // Read weights, hists, graphs, etc. for SFs //
    ///////////////////////////////////////////////

    auto lumi_weights =
        new reweight::LumiReWeighting("data/MC_Moriond17_PU25ns_V1.root", "data/Data_Pileup_2016_271036-284044_80bins.root", "pileup", "pileup");

    // H->tau tau scale factors
    TFile htt_sf_file("data/htt_scalefactors_2017_v2.root");
    RooWorkspace *htt_sf = reinterpret_cast<RooWorkspace *>(htt_sf_file.Get("w"));
    htt_sf_file.Close();

    // embedded sample weights
    TFile embed_file("data/htt_scalefactors_v17_6.root", "READ");
    RooWorkspace *wEmbed = reinterpret_cast<RooWorkspace *>(embed_file.Get("w"));
    embed_file.Close();

    TFile bTag_eff_file("data/tagging_efficiencies_march2018_btageff-all_samp-inc-DeepCSV_medium.root", "READ");
    TH2F *btag_eff_b = reinterpret_cast<TH2F *>(bTag_eff_file.Get("btag_eff_b")->Clone());
    TH2F *btag_eff_c = reinterpret_cast<TH2F *>(bTag_eff_file.Get("btag_eff_c")->Clone());
    TH2F *btag_eff_oth = reinterpret_cast<TH2F *>(bTag_eff_file.Get("btag_eff_oth")->Clone());

    TauTriggerSFs2017 *tau_trigger_sf =
        new TauTriggerSFs2017("$CMSSW_BASE/src/TauAnalysisTools/TauTriggerSFs/data/tauTriggerEfficiencies2017.root", "mutau", "2017", "tight", "MVAv2");

    //////////////////////////////////////
    // Final setup:                     //
    // Declare histograms and factories //
    //////////////////////////////////////

    // declare histograms (histogram initializer functions in util.h)
    fout->cd("grabbag");
    auto histos = helper->getHistos1D();
    auto histos_2d = helper->getHistos2D();

    // construct factories
    event_info event(ntuple, lepton::MUON, 2018, syst);
    muon_factory muons(ntuple, 2018);
    tau_factory taus(ntuple, 2018);
    jet_factory jets(ntuple, 2018, syst);
    met_factory met(ntuple, 2018, syst);

    if (sample.find("ggHtoTauTau125") != std::string::npos) {
        event.setRivets(ntuple);
    }

    // begin the event loop
    Int_t nevts = ntuple->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (Int_t i = 0; i < nevts; i++) {
        ntuple->GetEntry(i);
        if (i == progress * fraction) {
            std::cout << "\tProcessing: " << progress * 10 << "% complete.\r" << std::flush;
            progress++;
        }

        // find the event weight (not lumi*xs if looking at W or Drell-Yan)
        Float_t evtwt(norm), corrections(1.), sf_trig(1.), sf_id(1.), sf_iso(1.), sf_reco(1.);
        if (name == "W") {
            if (event.getNumGenJets() == 1) {
                evtwt = 11.738;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 5.278;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 3.498;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 3.663;
            } else {
                evtwt = 0.;
            }
        }

        if (name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
            if (event.getNumGenJets() == 1) {
                evtwt = 0.688;
            } else if (event.getNumGenJets() == 2) {
                evtwt = 0.603;
            } else if (event.getNumGenJets() == 3) {
                evtwt = 0.655;
            } else if (event.getNumGenJets() == 4) {
                evtwt = 0.917;
            } else {
                evtwt = 3.959;
            }
        }

        histos->at("cutflow")->Fill(1., 1.);

        auto muon = muons.run_factory();
        auto tau = taus.run_factory();
        jets.run_factory();

        // remove 2-prong taus
        if (tau.getDecayModeFinding() && tau.getDecayMode() != 5 && tau.getDecayMode() != 6) {
            histos->at("cutflow")->Fill(2., 1.);
        } else {
            continue;
        }

        bool fireSingle(false), fireCross(false);

        // apply correct lepton pT thresholds
        if (muon.getPt() > 28 && event.getPassMu27()) {
            fireSingle = true;
        } else if (muon.getPt() > 25 && event.getPassMu24()) {
            fireSingle = true;
        } else if (muon.getPt() > 21 && muon.getPt() < 25 && tau.getPt() > 32 && fabs(tau.getEta()) < 2.1 && event.getPassMu20Tau27()) {
            fireCross = true;
        } else {
            continue;
        }
        histos->at("cutflow")->Fill(3., 1.);

        if (muon.getP4().DeltaR(tau.getP4()) > 0.5) {
            histos->at("cutflow")->Fill(4., 1.);
        } else {
            continue;
        }

        if (tau.getPt() > 30) {
            histos->at("cutflow")->Fill(5., 1.);
        } else {
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
            histos->at("cutflow")->Fill(6., 1.);
        }

        // build Higgs
        TLorentzVector Higgs = muon.getP4() + tau.getP4() + met.getP4();

        // calculate mt
        double met_x = met.getMet() * cos(met.getMetPhi());
        double met_y = met.getMet() * sin(met.getMetPhi());
        double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
        double mt = sqrt(pow(muon.getPt() + met_pt, 2) - pow(muon.getPx() + met_x, 2) - pow(muon.getPy() + met_y, 2));
        int evt_charge = tau.getCharge() + muon.getCharge();

        // now do mt selection
        if (mt < 50) {
            histos->at("cutflow")->Fill(7., 1.);
        } else {
            continue;
        }

        // only opposite-sign
        if (evt_charge == 0) {
            histos->at("cutflow")->Fill(8., 1.);
        } else {
            continue;
        }

        // apply all scale factors/corrections/etc.
        if (!isData && !isEmbed) {
            // tau ID efficiency SF
            if (tau.getGenMatch() == 5) {
                evtwt *= 0.89;
            }

            // // anti-lepton discriminator SFs
            if (tau.getGenMatch() == 1 || tau.getGenMatch() == 3) {  // Yiwen
                if (fabs(tau.getEta()) < 1.460)
                    evtwt *= 1.80;
                else if (fabs(tau.getEta()) > 1.558)
                    evtwt *= 1.53;
                // if (name == "ZL" && tau.getL2DecayMode() == 0) evtwt *= 0.98;
                // else if (sample == "ZL" && tau.getL2DecayMode() == 1) evtwt *= 1.20;
            } else if (tau.getGenMatch() == 2 || tau.getGenMatch() == 4) {
                if (fabs(tau.getEta()) < 0.4)
                    evtwt *= 1.17;
                else if (fabs(tau.getEta()) < 0.8)
                    evtwt *= 1.29;
                else if (fabs(tau.getEta()) < 1.2)
                    evtwt *= 1.14;
                else if (fabs(tau.getEta()) < 1.7)
                    evtwt *= 0.93;
                else
                    evtwt *= 1.61;
            }

            // pileup reweighting
            if (!doAC && !isMG) {
                evtwt *= lumi_weights->weight(event.getNPV());
            }

            // generator weights
            evtwt *= event.getGenWeight();

            // give inputs to workspace
            htt_sf->var("m_pt")->setVal(muon.getPt());
            htt_sf->var("m_eta")->setVal(muon.getEta());
            htt_sf->var("z_gen_mass")->setVal(event.getGenM());  // TODO(tmitchel): check if these are the right variables.
            htt_sf->var("z_gen_pt")->setVal(event.getGenPt());   // TODO(tmitchel): check if these are the right variables.

            // muon ID SF
            evtwt *= htt_sf->function("m_id_kit_ratio")->getVal();
            // muon Iso SF
            evtwt *= htt_sf->function("m_iso_kit_ratio")->getVal();

            // apply trigger SF's
            auto single_data_eff = htt_sf->function("m_trg24_27_kit_data")->getVal();
            auto single_mc_eff = htt_sf->function("m_trg24_27_kit_mc")->getVal();
            auto mu_cross_data_eff = htt_sf->function("m_trg20_data")->getVal();
            auto mu_cross_mc_eff = htt_sf->function("m_trg20_mc")->getVal();
            auto single_eff = single_data_eff / single_mc_eff;
            auto mu_cross_eff = mu_cross_data_eff / mu_cross_mc_eff;
            double tau_cross_eff(1.);
            if (fireCross) {
                tau_cross_eff = tau_trigger_sf->getTriggerScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), tau.getDecayMode());
            }

            evtwt *= (single_eff * fireSingle + mu_cross_eff * tau_cross_eff * fireCross);

            // Z-pT Reweighting
            double zpt_sf(1.);
            if (name == "EWKZLL" || name == "EWKZNuNu" || name == "ZTT" || name == "ZLL" || name == "ZL" || name == "ZJ") {
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

            // use promote-demote method to correct nbtag with no systematics
            jets.promoteDemote(btag_eff_oth, btag_eff_oth, btag_eff_oth);

            // jet to tau fake rate
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
            wEmbed->var("m_pt")->setVal(muon.getPt());
            wEmbed->var("m_eta")->setVal(muon.getEta());
            wEmbed->var("m_iso")->setVal(muon.getIso());
            wEmbed->var("gt1_pt")->setVal(muon.getGenPt());
            wEmbed->var("gt1_eta")->setVal(muon.getGenEta());
            wEmbed->var("gt2_pt")->setVal(tau.getGenPt());
            wEmbed->var("gt2_eta")->setVal(tau.getGenEta());

            // double muon trigger eff in selection
            evtwt *= wEmbed->function("m_sel_trg_ratio")->getVal();

            // muon ID eff in selectionm
            evtwt *= wEmbed->function("m_sel_idEmb_ratio")->getVal();

            // muon ID SF
            evtwt *= wEmbed->function("m_id_embed_kit_ratio")->getVal();

            // muon iso SF
            evtwt *= wEmbed->function("m_iso_binned_embed_kit_ratio")->getVal();

            // apply trigger SF's
            auto single_eff = wEmbed->function("m_trg24_27_embed_kit_ratio")->getVal();
            auto mu_cross_eff = wEmbed->function("m_trg_MuTau_Mu20Leg_kit_ratio_embed")->getVal();
            auto tau_cross_eff = wEmbed->function("mt_emb_LooseChargedIsoPFTau27_kit_ratio")->getVal();  // TODO(tmitchel): currently being measured.

            evtwt *= (single_eff * fireSingle + mu_cross_eff * tau_cross_eff * fireCross);

            auto genweight(event.getGenWeight());
            if (genweight > 1 || genweight < 0) {
                genweight = 0;
            }
            evtwt *= genweight;
        }

        fout->cd();

        // b-jet veto
        if (jets.getNbtag() == 0) {
            histos->at("cutflow")->Fill(9., 1.);
        } else {
            continue;
        }

        // create regions
        bool signalRegion = (tau.getTightIsoMVA() && muon.getIso() < 0.15);
        bool looseIsoRegion = (tau.getMediumIsoMVA() && muon.getIso() < 0.30);
        bool antiIsoRegion = (tau.getTightIsoMVA() && muon.getIso() > 0.15 && muon.getIso() < 0.30);
        bool antiTauIsoRegion = (tau.getTightIsoMVA() == 0 && muon.getIso() < 0.15);

        // create categories
        bool zeroJet = (jets.getNjets() == 0);
        bool boosted = (jets.getNjets() == 1 || (jets.getNjets() > 1 && jets.getDijetMass() < 300));
        bool vbfCat = (jets.getNjets() > 1 && jets.getDijetMass() > 300);
        bool VHCat = (jets.getNjets() > 1 && jets.getDijetMass() < 300);

        // only keep the regions we need
        if (signalRegion || antiTauIsoRegion) {
            histos->at("cutflow")->Fill(10., 1.);
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
        st->fillTree(tree_cat, &muon, &tau, &jets, &met, &event, mt, evtwt, weights);
    }  // close event loop

    fin->Close();
    fout->cd();
    fout->Write();
    fout->Close();
    std::cout << std::endl;
    return 0;
}
