// Copyright 2020 Tyler Mitchell

#ifndef INCLUDE_BOOSTED_SLIM_TREE_H_
#define INCLUDE_BOOSTED_SLIM_TREE_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "TMath.h"
#include "TTree.h"
#include "ggntuple/jet_factory.h"
#include "ggntuple/event_factory.h"
#include "ggntuple/met_factory.h"
#include "models/electron.h"
#include "models/muon.h"
#include "models/tau.h"

class slim_tree {
 public:
    slim_tree(std::string, bool);
    ~slim_tree() {}  // default destructor

    // member functions
    void fillTree(electron *, tau *, event_factory *, std::string);
    void fillTree(muon *, tau *, event_factory *, std::string);
    void fillTree(electron *, muon *, event_factory *, std::string);
    void generalFill(std::vector<std::string>, jet_factory *, met_factory *, event_factory *, Float_t, TLorentzVector, Float_t,
                     std::shared_ptr<std::vector<double>>);
    void initial_values();
    void add_ac_branches();

    // member data
    TTree *otree;
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiLepIso, is_antiTauIso, is_antiBothIso, is_qcd, is_looseIso, OS, SS, contamination;
    ULong64_t evtno;
    UInt_t run, lumi;
    Float_t evtwt, el_pt, el_eta, el_phi, el_mass, el_charge, el_iso, el_genMatch, mu_pt, mu_eta, mu_phi, mu_mass, mu_charge, mu_iso, mu_genMatch,
        t1_pt, t1_eta, t1_phi, t1_mass, t1_charge, t1_iso, t1_iso_VL, t1_iso_L, t1_iso_M, t1_iso_T, t1_iso_VT, t1_iso_VVT, t1_decayMode,
        t1_genMatch,  // t1 is used for et and mt, as well
        njets, nbjets, j1_pt, j1_eta, j1_phi, j2_pt, j2_eta, j2_phi, b1_pt, b1_eta, b1_phi, b2_pt, b2_eta, b2_phi, met, metphi, mjj, numGenJets, mt,
        dmf, dmf_new, pt_sv, m_sv, MELA_D2j, Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2, ME_sm_ggH_qqInit, ME_ps_ggH_qqInit, ME_ps_ggH,
        ME_ps_VBF, ME_sm_VBF, ME_sm_ggH, ME_sm_WH, ME_sm_ZH, ME_bkg, ME_bkg1, ME_bkg2, ME_a2_VBF, ME_L1_VBF, ME_L1Zg_VBF, D0_ggH, DCP_ggH, D0_VBF,
        D_a2_VBF, D_l1_VBF, D_l1zg_VBF, DCP_VBF, higgs_pT, higgs_m, hjj_pT, hjj_m, dEtajj, dPhijj, vis_mass, MT_lepMET, MT_t2MET, MT_HiggsMET,
        hj_dphi, hj_deta, jmet_dphi, hmet_dphi, hj_dr, lt_dphi;
    Float_t cross_trigger;
    Float_t lep_dr;

    // Anomolous coupling branches
    Float_t wt_a1, wt_a2, wt_a3, wt_L1, wt_L1Zg, wt_a2int, wt_a3int, wt_L1int, wt_L1Zgint, wt_ggH_a1, wt_ggH_a3, wt_ggH_a3int, wt_wh_a1, wt_wh_a2,
        wt_wh_a3, wt_wh_L1, wt_wh_L1Zg, wt_wh_a2int, wt_wh_a3int, wt_wh_L1int, wt_wh_L1Zgint, wt_zh_a1, wt_zh_a2, wt_zh_a3, wt_zh_L1, wt_zh_L1Zg,
        wt_zh_a2int, wt_zh_a3int, wt_zh_L1int, wt_zh_L1Zgint;
    Float_t sm_weight_nlo, mm_weight_nlo, ps_weight_nlo;
};

slim_tree::slim_tree(std::string tree_name, bool isAC = false) : otree(new TTree(tree_name.c_str(), tree_name.c_str())) {
    otree->Branch("evtwt", &evtwt, "evtwt/F");
    // otree->Branch("evt", &evtno);
    // otree->Branch("run", &run);
    // otree->Branch("lumi", &lumi);

    otree->Branch("el_pt", &el_pt, "el_pt/F");
    otree->Branch("el_eta", &el_eta, "el_eta/F");
    otree->Branch("el_phi", &el_phi, "el_phi/F");
    otree->Branch("el_mass", &el_mass, "el_mass/F");
    // otree->Branch("el_charge", &el_charge, "el_charge/F");
    // otree->Branch("el_iso", &el_iso, "el_iso/F");
    // otree->Branch("el_genMatch", &el_genMatch, "el_genMatch/F");
    otree->Branch("mu_pt", &mu_pt, "mu_pt/F");
    otree->Branch("mu_eta", &mu_eta, "mu_eta/F");
    otree->Branch("mu_phi", &mu_phi, "mu_phi/F");
    otree->Branch("mu_mass", &mu_mass, "mu_mass/F");
    // otree->Branch("mu_charge", &mu_charge, "mu_charge/F");
    // otree->Branch("mu_iso", &mu_iso, "mu_iso/F");
    // otree->Branch("mu_genMatch", &mu_genMatch, "mu_genMatch/F");
    otree->Branch("t1_pt", &t1_pt, "t1_pt/F");
    otree->Branch("t1_eta", &t1_eta, "t1_eta/F");
    otree->Branch("t1_phi", &t1_phi, "t1_phi/F");
    otree->Branch("t1_mass", &t1_mass, "t1_mass/F");
    // otree->Branch("t1_charge", &t1_charge, "t1_charge/F");
    otree->Branch("t1_iso", &t1_iso, "t1_iso/F");
    otree->Branch("t1_decayMode", &t1_decayMode, "t1_decayMode/F");
    otree->Branch("t1_dmf", &dmf, "t1_dmf/F");
    otree->Branch("t1_dmf_new", &dmf_new, "t1_dmf_new/F");
    otree->Branch("t1_genMatch", &t1_genMatch, "t1_genMatch/F");
    otree->Branch("lep_dr", &lep_dr, "lep_dr/F");

    otree->Branch("numGenJets", &numGenJets, "numGenJets/F");
    otree->Branch("njets", &njets, "njets");
    otree->Branch("nbjets", &nbjets, "nbjets");
    otree->Branch("j1_pt", &j1_pt, "j1_pt/F");
    otree->Branch("j1_eta", &j1_eta, "j1_eta/F");
    otree->Branch("j1_phi", &j1_phi, "j1_phi/F");
    otree->Branch("j2_pt", &j2_pt, "j2_pt/F");
    otree->Branch("j2_eta", &j2_eta, "j2_eta/F");
    otree->Branch("j2_phi", &j2_phi, "j2_phi/F");

    otree->Branch("met", &met, "met/F");
    otree->Branch("metphi", &metphi, "metphi/F");
    otree->Branch("mt", &mt, "mt/F");

    otree->Branch("mjj", &mjj, "mjj/F");
    otree->Branch("higgs_pT", &higgs_pT, "higgs_pT/F");
    otree->Branch("vis_mass", &vis_mass, "vis_mass/F");
    otree->Branch("dPhijj", &dPhijj, "dPhijj/F");
    otree->Branch("pt_sv", &pt_sv, "pt_sv/F");
    otree->Branch("m_sv", &m_sv, "m_sv/F");

    otree->Branch("D0_ggH", &D0_ggH, "D0_ggH/F");
    otree->Branch("DCP_ggH", &DCP_ggH, "DCP_ggH/F");
    otree->Branch("D0_VBF", &D0_VBF, "D0_VBF/F");
    otree->Branch("D_a2_VBF", &D_a2_VBF, "D_a2_VBF/F");
    otree->Branch("D_l1_VBF", &D_l1_VBF, "D_l1_VBF/F");
    otree->Branch("D_l1zg_VBF", &D_l1zg_VBF, "D_l1zg_VBF/F");
    otree->Branch("DCP_VBF", &DCP_VBF, "DCP_VBF/F");
    otree->Branch("MELA_D2j", &MELA_D2j, "MELA_D2j/F");

    otree->Branch("Phi", &Phi, "Phi/F");
    otree->Branch("Phi1", &Phi1, "Phi1/F");
    otree->Branch("costheta1", &costheta1, "costheta1/F");
    otree->Branch("costheta2", &costheta2, "costheta2/F");
    otree->Branch("costhetastar", &costhetastar, "costhetastar/F");
    otree->Branch("Q2V1", &Q2V1, "Q2V1/F");
    otree->Branch("Q2V2", &Q2V2, "Q2V2/F");

    otree->Branch("OS", &OS, "OS/I");
    otree->Branch("is_signal", &is_signal, "is_signal/I");
    otree->Branch("is_antiLepIso", &is_antiLepIso, "is_antiLepIso/I");
    otree->Branch("is_antiTauIso", &is_antiTauIso, "is_antiTauIso/I");
    otree->Branch("is_antiBothIso", &is_antiBothIso, "is_antiBothIso/I");
    otree->Branch("contamination", &contamination, "contamination/I");
    otree->Branch("cross_trigger", &cross_trigger, "cross_trigger/F");

    // Set any initial values for variables. For instance, ac weights are
    // initialized to = 1 so that any non-AC sample has the weight saved as
    // 1 for all events.
    initial_values();

    // include weights for anomolous coupling
    if (isAC) {
        add_ac_branches();
    }
}

void slim_tree::generalFill(std::vector<std::string> cats, jet_factory *fjets, met_factory *fmet, event_factory *evt, Float_t weight,
                            TLorentzVector higgs, Float_t Mt, std::shared_ptr<std::vector<double>> ac_weights) {
    // create things needed for later
    auto jets(fjets->getCleanedJets());
    auto btags(fjets->getBtagJets());

    // start filling branches
    evtwt = weight;
    evtno = evt->getEvt();
    run = evt->getRun();
    lumi = evt->getLumi();
    higgs_pT = higgs.Pt();
    higgs_m = higgs.M();

    met = fmet->getMet();
    metphi = fmet->getMetPhi();
    mjj = fjets->getDijetMass();
    pt_sv = evt->getPtSV();
    m_sv = evt->getMSV();

    auto met_x = fmet->getMet() * cos(fmet->getMetPhi());
    auto met_y = fmet->getMet() * sin(fmet->getMetPhi());
    auto met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    MT_HiggsMET = sqrt(pow(higgs.Pt() + met_pt, 2) - pow(higgs.Px() + met_x, 2) - pow(higgs.Py() + met_y, 2));

    mt = Mt;
    njets = fjets->getNjets();
    nbjets = fjets->getNbtag();

    // dijet info is only ok if you have 2 jets, imagine that
    hjj_pT = 0.;
    hjj_m = 0.;
    dEtajj = 0.;
    dPhijj = 0.;
    j1_pt = 0;
    j1_eta = 0;
    j1_phi = 0;
    j2_pt = 0;
    j2_eta = 0;
    j2_phi = 0;
    b1_pt = 0;
    b1_eta = 0;
    b1_phi = 0;
    b2_pt = 0;
    b2_eta = 0;
    b2_phi = 0;

    if (njets > 0) {
        j1_pt = jets.at(0).getPt();
        j1_eta = jets.at(0).getEta();
        j1_phi = jets.at(0).getPhi();
        hj_dphi = TMath::ACos(TMath::Cos(jets.at(0).getPhi() - higgs.Phi()));
        hj_deta = fabs(jets.at(0).getEta() - higgs.Eta());
        jmet_dphi = TMath::ACos(TMath::Cos(fmet->getMetPhi() - jets.at(0).getPhi()));
        hmet_dphi = TMath::ACos(TMath::Cos(fmet->getMetPhi() - higgs.Phi()));
        hj_dr = higgs.DeltaR(jets.at(0).getP4());

        if (njets > 1) {
            j2_pt = jets.at(1).getPt();
            j2_eta = jets.at(1).getEta();
            j2_phi = jets.at(1).getPhi();
            hjj_pT = (higgs + jets.at(0).getP4() + jets.at(1).getP4()).Pt();
            hjj_m = (higgs + jets.at(0).getP4() + jets.at(1).getP4()).M();
            dEtajj = fabs(jets.at(0).getEta() - jets.at(1).getEta());
            // dPhijj = TMath::ACos(TMath::Cos((jets.at(0).getPhi() - jets.at(1).getPhi())));
            if (jets.at(0).getEta() > jets.at(1).getEta()) {
                dPhijj = jets.at(0).getPhi() - jets.at(1).getPhi();
            } else {
                dPhijj = jets.at(1).getPhi() - jets.at(0).getPhi();
            }

            if (dPhijj > TMath::Pi()) {
                dPhijj -= 2 * TMath::Pi();
            } else if (dPhijj < -1 * TMath::Pi()) {
                dPhijj += 2 * TMath::Pi();
            }

            dPhijj = fabs(dPhijj);
        }
    }

    if (nbjets > 0) {
        b1_pt = btags.at(0).getPt();
        b1_eta = btags.at(0).getEta();
        b1_phi = btags.at(0).getPhi();
        if (nbjets > 1) {
            j2_pt = btags.at(1).getPt();
            j2_eta = btags.at(1).getEta();
            j2_phi = btags.at(1).getPhi();
        }
    }

    // reset the categories
    is_signal = 0;
    is_antiLepIso = 0;
    is_antiTauIso = 0;
    is_antiBothIso = 0;
    is_qcd = 0;
    is_looseIso = 0;
    cat_0jet = 0;
    cat_boosted = 0;
    cat_vbf = 0;
    cat_VH = 0;
    OS = 0;
    SS = 0;
    contamination = 0;

    // decide on which selections have been passed
    for (auto cat : cats) {
        // regions
        if (cat == "signal") {
            is_signal = 1;
        } else if (cat == "antiTauIso") {
            is_antiTauIso = 1;
        } else if (cat == "antiLepIso") {
            is_antiLepIso = 1;
        } else if (cat == "antiBothIso") {
            is_antiBothIso = 1;
        }

        // event charge
        if (cat == "OS") {
            OS = 1;
        }
    }
}

void slim_tree::fillTree(electron *el, tau *t, event_factory *evt, std::string name) {
    el_pt = el->getPt();
    el_eta = el->getEta();
    el_phi = el->getPhi();
    el_mass = el->getMass();
    el_charge = el->getCharge();
    el_iso = el->getIso();
    el_genMatch = el->getGenMatch();
    t1_pt = t->getPt();
    t1_eta = t->getEta();
    t1_phi = t->getPhi();
    t1_mass = t->getMass();
    t1_charge = t->getCharge();
    t1_decayMode = t->getDecayMode();
    t1_iso = t->getIso();
    t1_genMatch = t->getGenMatch();
    dmf = t->getDecayModeFinding();
    dmf_new = t->getDecayModeFindingNew();
    vis_mass = (el->getP4() + t->getP4()).M();
    if ((name == "ZTT" || name == "ZL" || name == "TTT" || name == "TTL" || name == "STT" || name == "STL" || name == "VVT" || name == "VVL") &&
        (el->getGenMatch() > 2 && el->getGenMatch() < 6) && (t->getGenMatch() > 2 && t->getGenMatch() < 6)) {
        contamination = 1;  // mc contaminating embedded samples
    }
    lep_dr = el->getP4().DeltaR(t->getP4());

    otree->Fill();
}

void slim_tree::fillTree(muon *mu, tau *t, event_factory *evt, std::string name) {
    mu_pt = mu->getPt();
    mu_eta = mu->getEta();
    mu_phi = mu->getPhi();
    mu_mass = mu->getMass();
    mu_charge = mu->getCharge();
    mu_iso = mu->getIso();
    mu_genMatch = mu->getGenMatch();
    t1_pt = t->getPt();
    t1_eta = t->getEta();
    t1_phi = t->getPhi();
    t1_mass = t->getMass();
    t1_charge = t->getCharge();
    t1_decayMode = t->getDecayMode();
    t1_iso = t->getIso();
    t1_genMatch = t->getGenMatch();
    dmf = t->getDecayModeFinding();
    dmf_new = t->getDecayModeFindingNew();
    vis_mass = (mu->getP4() + t->getP4()).M();
    if ((name == "ZTT" || name == "ZL" || name == "TTT" || name == "TTL" || name == "STT" || name == "STL" || name == "VVT" || name == "VVL") &&
        (mu->getGenMatch() > 2 && mu->getGenMatch() < 6) && (t->getGenMatch() > 2 && t->getGenMatch() < 6)) {
        contamination = 1;  // mc contaminating embedded samples
    }
    lep_dr = mu->getP4().DeltaR(t->getP4());

    otree->Fill();
}

void slim_tree::fillTree(electron *el, muon *mu, event_factory *evt, std::string name) {
  el_pt = el->getPt();
  el_eta = el->getEta();
  el_phi = el->getPhi();
  el_mass = el->getMass();
  el_charge = el->getCharge();
  el_iso = el->getIso();
  el_genMatch = el->getGenMatch();
  mu_pt = mu->getPt();
  mu_eta = mu->getEta();
  mu_phi = mu->getPhi();
  mu_mass = mu->getMass();
  mu_charge = mu->getCharge();
  mu_iso = mu->getIso();
  mu_genMatch = mu->getGenMatch();
  vis_mass = (el->getP4() + mu->getP4()).M();
  if ((name == "ZTT" || name == "ZL" || name == "TTT" || name == "TTL" || name == "STT" || name == "STL" || name == "VVT" || name == "VVL") && (el->getGenMatch() > 2 && el-> getGenMatch() < 6) && (mu->getGenMatch() > 2 && mu->getGenMatch() < 6)) {
    contamination = 1; //mc contaminating embedded samples
  }
  lep_dr = el->getP4().DeltaR(mu->getP4());

  otree->Fill();
}

void slim_tree::initial_values() {
    wt_a1 = 1.;
    wt_a2 = 1.;
    wt_a3 = 1.;
    wt_L1 = 1.;
    wt_L1Zg = 1.;
    wt_a2int = 1.;
    wt_a3int = 1.;
    wt_L1int = 1.;
    wt_L1Zgint = 1.;
    wt_ggH_a1 = 1.;
    wt_ggH_a3 = 1.;
    wt_ggH_a3int = 1.;
    wt_wh_a1 = 1.;
    wt_wh_a2 = 1.;
    wt_wh_a3 = 1.;
    wt_wh_L1 = 1.;
    wt_wh_L1Zg = 1.;
    wt_wh_a2int = 1.;
    wt_wh_a3int = 1.;
    wt_wh_L1int = 1.;
    wt_wh_L1Zgint = 1.;
    wt_zh_a1 = 1.;
    wt_zh_a2 = 1.;
    wt_zh_a3 = 1.;
    wt_zh_L1 = 1.;
    wt_zh_L1Zg = 1.;
    wt_zh_a2int = 1.;
    wt_zh_a3int = 1.;
    wt_zh_L1int = 1.;
    wt_zh_L1Zgint = 1.;
}

void slim_tree::add_ac_branches() {
    otree->Branch("wt_vbf_a1", &wt_a1);
    otree->Branch("wt_vbf_a2", &wt_a2);
    otree->Branch("wt_vbf_a3", &wt_a3);
    otree->Branch("wt_vbf_L1", &wt_L1);
    otree->Branch("wt_vbf_L1Zg", &wt_L1Zg);
    otree->Branch("wt_vbf_a2int", &wt_a2int);
    otree->Branch("wt_vbf_a3int", &wt_a3int);
    otree->Branch("wt_vbf_L1int", &wt_L1int);
    otree->Branch("wt_vbf_L1Zgint", &wt_L1Zgint);

    otree->Branch("wt_ggh_a1", &wt_ggH_a1);
    otree->Branch("wt_ggh_a3", &wt_ggH_a3);
    otree->Branch("wt_ggh_a3int", &wt_ggH_a3int);

    otree->Branch("wt_wh_a1", &wt_wh_a1);
    otree->Branch("wt_wh_a2", &wt_wh_a2);
    otree->Branch("wt_wh_a3", &wt_wh_a3);
    otree->Branch("wt_wh_L1", &wt_wh_L1);
    otree->Branch("wt_wh_L1Zg", &wt_wh_L1Zg);
    otree->Branch("wt_wh_a2int", &wt_wh_a2int);
    otree->Branch("wt_wh_a3int", &wt_wh_a3int);
    otree->Branch("wt_wh_L1int", &wt_wh_L1int);
    otree->Branch("wt_wh_L1Zgint", &wt_wh_L1Zgint);

    otree->Branch("wt_zh_a1", &wt_zh_a1);
    otree->Branch("wt_zh_a2", &wt_zh_a2);
    otree->Branch("wt_zh_a3", &wt_zh_a3);
    otree->Branch("wt_zh_L1", &wt_zh_L1);
    otree->Branch("wt_zh_L1Zg", &wt_zh_L1Zg);
    otree->Branch("wt_zh_a2int", &wt_zh_a2int);
    otree->Branch("wt_zh_a3int", &wt_zh_a3int);
    otree->Branch("wt_zh_L1int", &wt_zh_L1int);
    otree->Branch("wt_zh_L1Zgint", &wt_zh_L1Zgint);

    otree->Branch("sm_weight_nlo", &sm_weight_nlo);
    otree->Branch("mm_weight_nlo", &mm_weight_nlo);
    otree->Branch("ps_weight_nlo", &ps_weight_nlo);
}

#endif  // INCLUDE_BOOSTED_SLIM_TREE_H_
