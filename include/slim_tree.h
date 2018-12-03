#include <iostream>
#include "TTree.h"
#include "ditau_factory.h"
#include "tau_factory.h"
#include "muon_factory.h"
#include "electron_factory.h"

class slim_tree {
 public:
    slim_tree(std::string, bool);
    ~slim_tree() {};  // default destructor

    // member functions
    // fill the tree for this event
    void fillTree(std::vector<std::string>, electron*  , tau*  , jet_factory*, met_factory*, event_info*, Float_t, Float_t, std::vector<double>*);
    void fillTree(std::vector<std::string>, muon*      , tau*  , jet_factory*, met_factory*, event_info*, Float_t, Float_t, std::vector<double>*);
    void fillTree(std::vector<std::string>, ditau*     , ditau*, jet_factory*, met_factory*, event_info*, Float_t, std::vector<double>*);
    void generalFill(std::vector<std::string>, jet_factory *, met_factory *, event_info *, Float_t, TLorentzVector, Float_t, std::vector<double> *);

    // member data
    TTree* otree;
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiLepIso, is_antiTauIso, is_qcd, is_looseIso, OS, SS;
    Float_t evtwt,
        el_pt, el_eta, el_phi, el_mass, el_charge, el_iso,
        mu_pt, mu_eta, mu_phi, mu_mass, mu_charge, mu_iso,
        t1_pt, t1_eta, t1_phi, t1_mass, t1_charge, t1_iso, t1_iso_VL, t1_iso_L, t1_iso_M, t1_iso_T, t1_iso_VT, t1_iso_VVT, t1_decayMode, t1_genMatch,  // t1 is used for et and mt, as well
        t2_pt, t2_eta, t2_phi, t2_mass, t2_charge, t2_iso, t2_iso_VL, t2_iso_L, t2_iso_M, t2_iso_T, t2_iso_VT, t2_iso_VVT, t2_decayMode, t2_genMatch,
        njets, nbjets,
        j1_pt, j1_eta, j1_phi,
        j2_pt, j2_eta, j2_phi,
        b1_pt, b1_eta, b1_phi,
        b2_pt, b2_eta, b2_phi,
        met, metphi, mjj, numGenJets, mt, dmf, dmf_new,
        pt_sv, m_sv, Dbkg_VBF, Dbkg_ggH,
        Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2,
        ME_sm_VBF, ME_sm_ggH, ME_sm_WH, ME_sm_ZH, ME_bkg, ME_bkg1, ME_bkg2,
        higgs_pT, higgs_m, hjj_pT, hjj_m, dEtajj, dPhijj, vis_mass;

    // Anomolous coupling branches
    Float_t wt_a1, wt_a2, wt_a3, wt_L1, wt_L1Zg, wt_a2int, wt_a3int, wt_L1int, wt_L1Zgint, wt_ggH_a1, wt_ggH_a3, wt_ggH_a3int, wt_wh_a1, wt_wh_a2, wt_wh_a3, wt_wh_L1, 
            wt_wh_L1Zg, wt_wh_a2int, wt_wh_a3int, wt_wh_L1int, wt_wh_L1Zgint, wt_zh_a1, wt_zh_a2, wt_zh_a3, wt_zh_L1, wt_zh_L1Zg, wt_zh_a2int, wt_zh_a3int, wt_zh_L1int, 
            wt_zh_L1Zgint;
};

slim_tree::slim_tree(std::string tree_name, bool isAC = false) : otree( new TTree(tree_name.c_str(), tree_name.c_str()) ) {
    otree->Branch("evtwt",               &evtwt,               "evtwt/F"              );

    otree->Branch("el_pt",               &el_pt,               "el_pt/F"              );
    otree->Branch("el_eta",              &el_eta,              "el_eta/F"             );
    otree->Branch("el_phi",              &el_phi,              "el_phi/F"             );
    otree->Branch("el_mass",             &el_mass,             "el_mass/F"            );
    otree->Branch("el_charge",           &el_charge,           "el_charge/F"          );
    otree->Branch("el_iso",              &el_iso,              "el_iso/F"             );
    otree->Branch("mu_pt",               &mu_pt,               "mu_pt/F"              );
    otree->Branch("mu_eta",              &mu_eta,              "mu_eta/F"             );
    otree->Branch("mu_phi",              &mu_phi,              "mu_phi/F"             );
    otree->Branch("mu_mass",             &mu_mass,             "mu_mass/F"            );
    otree->Branch("mu_charge",           &mu_charge,           "mu_charge/F"          );
    otree->Branch("mu_iso",              &mu_iso,              "mu_iso/F"             );
    otree->Branch("t1_pt",               &t1_pt,               "t1_pt/F"              );
    otree->Branch("t1_eta",              &t1_eta,              "t1_eta/F"             );
    otree->Branch("t1_phi",              &t1_phi,              "t1_phi/F"             );
    otree->Branch("t1_mass",             &t1_mass,             "t1_mass/F"            );
    otree->Branch("t1_charge",           &t1_charge,           "t1_charge/F"          );
    otree->Branch("t1_iso",              &t1_iso,              "t1_iso/F"             );
    otree->Branch("t1_iso_VL",           &t1_iso_VL,           "t1_iso_VL/F"          );
    otree->Branch("t1_iso_L",            &t1_iso_L,            "t1_iso_L/F"           );
    otree->Branch("t1_iso_M",            &t1_iso_M,            "t1_iso_M/F"           );
    otree->Branch("t1_iso_T",            &t1_iso_T,            "t1_iso_T/F"           );
    otree->Branch("t1_iso_VT",           &t1_iso_VT,           "t1_iso_VT/F"          );
    otree->Branch("t1_iso_VVT",          &t1_iso_VVT,          "t1_iso_VVT/F"         );
    otree->Branch("t1_decayMode",        &t1_decayMode,        "t1_decayMode/F"       );
    otree->Branch("t1_dmf",              &dmf,                 "t1_dmf/F"             );
    otree->Branch("t1_dmf_new",          &dmf_new,             "t1_dmf_new/F"         );
    otree->Branch("t1_genMatch",         &t1_genMatch,         "t1_genMatch/F"        );
    otree->Branch("t2_pt",               &t2_pt,               "t2_pt/F"              );
    otree->Branch("t2_eta",              &t2_eta,              "t2_eta/F"             );
    otree->Branch("t2_phi",              &t2_phi,              "t2_phi/F"             );
    otree->Branch("t2_mass",             &t2_mass,             "t2_mass/F"            );
    otree->Branch("t2_charge",           &t2_charge,           "t2_charge/F"          );
    otree->Branch("t2_iso",              &t2_iso,              "t2_iso/F"             );
    otree->Branch("t2_iso_VL",           &t2_iso_VL,           "t2_iso_VL/F"          );
    otree->Branch("t2_iso_L",            &t2_iso_L,            "t2_iso_L/F"           );
    otree->Branch("t2_iso_M",            &t2_iso_M,            "t2_iso_M/F"           );
    otree->Branch("t2_iso_T",            &t2_iso_T,            "t2_iso_T/F"           );
    otree->Branch("t2_iso_VT",           &t2_iso_VT,           "t2_iso_VT/F"          );
    otree->Branch("t2_iso_VVT",          &t2_iso_VVT,          "t2_iso_VVT/F"         );
    otree->Branch("t2_decayMode",        &t2_decayMode,        "t2_decayMode/F"       );
    otree->Branch("t2_dmf",              &dmf,                 "t2_dmf/F"             );
    otree->Branch("t2_dmf_new",          &dmf_new,             "t2_dmf_new/F"         );
    otree->Branch("t2_genMatch",         &t2_genMatch,         "t2_genMatch/F"        );

    otree->Branch("njets",               &njets,               "njets"                );
    otree->Branch("nbjets",              &nbjets,              "nbjets"               ); 
    otree->Branch("j1_pt",               &j1_pt,               "j1_pt/F"              );
    otree->Branch("j1_eta",              &j1_eta,              "j1_eta/F"             );
    otree->Branch("j1_phi",              &j1_phi,              "j1_phi/F"             );
    otree->Branch("j2_pt",               &j2_pt,               "j2_pt/F"              );
    otree->Branch("j2_eta",              &j2_eta,              "j2_eta/F"             );
    otree->Branch("j2_phi",              &j2_phi,              "j2_phi/F"             );
    otree->Branch("b1_pt",               &b1_pt,               "b1_pt/F"              );
    otree->Branch("b1_eta",              &b1_eta,              "b1_eta/F"             );
    otree->Branch("b1_phi",              &b1_phi,              "b1_phi/F"             );
    otree->Branch("b2_pt",               &b2_pt,               "b2_pt/F"              );
    otree->Branch("b2_eta",              &b2_eta,              "b2_eta/F"             );
    otree->Branch("b2_phi",              &b2_phi,              "b2_phi/F"             );

    otree->Branch("met",                 &met,                 "met/F"                );
    otree->Branch("metphi",              &metphi,              "metphi/F"             );
    otree->Branch("mjj",                 &mjj,                 "mjj/F"                );
    otree->Branch("mt",                  &mt,                  "mt/F"                 );

    otree->Branch("numGenJets",          &numGenJets,          "numGenJets/F"         );

    otree->Branch("pt_sv",               &pt_sv,               "pt_sv/F"              );
    otree->Branch("m_sv",                &m_sv,                "m_sv/F"               );
    otree->Branch("Dbkg_VBF",            &Dbkg_VBF,            "Dbkg_VBF/F"           );
    otree->Branch("Dbkg_ggH",            &Dbkg_ggH,            "Dbkg_ggH/F"           );

    otree->Branch("Phi"         ,        &Phi         ,        "Phi/F"                );
    otree->Branch("Phi1"        ,        &Phi1        ,        "Phi1/F"               );
    otree->Branch("costheta1"   ,        &costheta1   ,        "costheta1/F"          );
    otree->Branch("costheta2"   ,        &costheta2   ,        "costheta2/F"          );
    otree->Branch("costhetastar",        &costhetastar,        "costhetastar/F"       );
    otree->Branch("Q2V1"        ,        &Q2V1        ,        "Q2V1/F"               );
    otree->Branch("Q2V2"        ,        &Q2V2        ,        "Q2V2/F"               );
    otree->Branch("ME_sm_VBF"   ,        &ME_sm_VBF    ,       "ME_sm_/F"             );
    otree->Branch("ME_sm_ggH"   ,        &ME_sm_ggH    ,       "ME_sm_/F"             );
    otree->Branch("ME_sm_WH"    ,        &ME_sm_WH     ,       "ME_sm_WH/F"           );
    otree->Branch("ME_sm_ZH"    ,        &ME_sm_ZH     ,       "ME_sm_ZH/F"           );
    otree->Branch("ME_bkg"      ,        &ME_bkg       ,       "MEbkg_/F"             );
    otree->Branch("ME_bkg1"     ,        &ME_bkg1      ,       "MEbkg1_/F"            );
    otree->Branch("ME_bkg2"     ,        &ME_bkg2      ,       "MEbkg2_/F"            );

    otree->Branch("higgs_pT",            &higgs_pT,            "higgs_pT/F"           );
    otree->Branch("higgs_m",             &higgs_m,             "higgs_m/F"            );
    otree->Branch("hjj_pT",              &hjj_pT,              "hjj_pT/F"             );
    otree->Branch("hjj_m",               &hjj_m,               "hjj_m/F"              );
    otree->Branch("vis_mass",            &vis_mass,            "vis_mass/F"           );
    otree->Branch("dEtajj",              &dEtajj,              "dEtajj/F"             );
    otree->Branch("dPhijj",              &dPhijj,              "dPhijj/F"             );

    otree->Branch("is_signal",           &is_signal,           "is_signal/I"          );
    otree->Branch("is_antiLepIso",       &is_antiLepIso,       "is_antiLepIso/I"      );
    otree->Branch("is_antiTauIso",       &is_antiTauIso,       "is_antiTauIso/I"      );
    otree->Branch("is_qcd",              &is_qcd,              "is_qcd/I"             );
    otree->Branch("is_looseIso",         &is_looseIso,         "is_looseIso/I"        );
    otree->Branch("cat_0jet",            &cat_0jet,            "cat_0jet/I"           );
    otree->Branch("cat_boosted",         &cat_boosted,         "cat_boosted/I"        );
    otree->Branch("cat_vbf",             &cat_vbf,             "cat_vbf/I"            );
    otree->Branch("cat_VH",              &cat_VH,              "cat_VH/I"             );
    otree->Branch("OS",                  &OS,                  "OS/I"                 );
    otree->Branch("SS",                  &SS,                  "SS/I"                 );

    // include weights for anomolous coupling
    if (isAC) {
      otree->Branch("wt_a1", &wt_a1);
      otree->Branch("wt_a2", &wt_a2);
      otree->Branch("wt_a3", &wt_a3);
      otree->Branch("wt_L1", &wt_L1);
      otree->Branch("wt_L1Zg", &wt_L1Zg);
      otree->Branch("wt_a2int", &wt_a2int);
      otree->Branch("wt_a3int", &wt_a3int);
      otree->Branch("wt_L1int", &wt_L1int);
      otree->Branch("wt_L1Zgint", &wt_L1Zgint);

      otree->Branch("wt_ggH_a1", &wt_ggH_a1);
      otree->Branch("wt_ggH_a3", &wt_ggH_a3);
      otree->Branch("wt_ggH_a3int", &wt_ggH_a3int);

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
    }
}

void slim_tree::generalFill(std::vector<std::string> cats, jet_factory* fjets, met_factory* fmet, event_info* evt, Float_t weight, 
                            TLorentzVector higgs, Float_t Mt, std::vector<double>* ac_weights = nullptr) {
    // create things needed for later
    auto jets(fjets->getJets());
    auto btags(fjets->getBtagJets());

    // start filling branches
    evtwt = weight;
    higgs_pT = higgs.Pt();
    higgs_m = higgs.M();

    met = fmet->getMet();
    metphi = fmet->getMetPhi();
    mjj = fjets->getDijetMass();
    pt_sv = evt->getPtSV();
    m_sv = evt->getMSV();
    Dbkg_VBF = evt->getDbkg_VBF();
    Dbkg_ggH = evt->getDbkg_ggH();
    Phi = evt->getPhi();
    Phi1 = evt->getPhi1();
    costheta1 = evt->getCosTheta1();
    costheta2 = evt->getCosTheta2();
    costhetastar = evt->getCosThetaStar();
    Q2V1 = evt->getQ2V1();
    Q2V2 = evt->getQ2V2();
    ME_sm_VBF = evt->getME_sm_VBF();
    ME_sm_ggH = evt->getME_sm_ggH();
    ME_sm_WH = evt->getME_sm_WH();
    ME_sm_ZH = evt->getME_sm_ZH();
    ME_bkg = evt->getME_bkg();
    ME_bkg1 = evt->getME_bkg1();
    ME_bkg2 = evt->getME_bkg2();

    mt = Mt;
    numGenJets = evt->getNumGenJets();
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
        if (njets > 1) {
            j2_pt = jets.at(1).getPt();
            j2_eta = jets.at(1).getEta();
            j2_phi = jets.at(1).getPhi();
            hjj_pT = (higgs + jets.at(0).getP4() + jets.at(1).getP4()).Pt();
            hjj_m = (higgs + jets.at(0).getP4() + jets.at(1).getP4()).M();
            dEtajj = fabs(jets.at(0).getEta() - jets.at(1).getEta());
            dPhijj = fabs(jets.at(0).getPhi() - jets.at(1).getPhi());
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
    is_qcd = 0;
    is_looseIso = 0;
    cat_0jet = 0;
    cat_boosted = 0;
    cat_vbf = 0;
    cat_VH = 0;
    OS = 0;
    SS = 0;

    // decide on which selections have been passed
    for (auto cat : cats) {

        // regions
        if (cat == "signal") {
            is_signal = 1;
        } else if (cat == "antiLepIso") {
            is_antiLepIso = 1;
        } else if (cat == "antiTauIso") {
            is_antiTauIso = 1;
        } else if (cat == "looseIso") {
            is_looseIso = 1;
        }

        // categories
        if (cat == "0jet") {
            cat_0jet = 1;
        } else if (cat == "boosted") {
            cat_boosted = 1;
        } else if (cat == "vbf") {
            cat_vbf = 1;
        } else if (cat == "VH") {
            cat_VH = 1;
        }

        // event charge
        if (cat == "OS") {
            OS = 1;
        } else if (cat == "SS") {
            SS = 1;
        }
    }
    is_qcd = is_looseIso;

    // anomolous coupling files
    if (ac_weights != nullptr) {
      wt_a1 = ac_weights->at(0);
      wt_a2 = ac_weights->at(1);
      wt_a3 = ac_weights->at(2);
      wt_L1Zg = ac_weights->at(3);
      wt_a2int = ac_weights->at(4);
      wt_a3int = ac_weights->at(5);
      wt_L1int = ac_weights->at(6);
      wt_L1Zgint = ac_weights->at(7);
      wt_ggH_a1 = ac_weights->at(8);
      wt_ggH_a3 = ac_weights->at(9);
      wt_ggH_a3 = ac_weights->at(10);
      wt_ggH_a3int = ac_weights->at(11);
      wt_wh_a1 = ac_weights->at(12);
      wt_wh_a2 = ac_weights->at(13);
      wt_wh_a3 = ac_weights->at(14);
      wt_wh_L1 = ac_weights->at(15);
      wt_wh_L1Zg = ac_weights->at(16);
      wt_wh_a2int = ac_weights->at(17);
      wt_wh_a3int = ac_weights->at(18);
      wt_wh_L1int = ac_weights->at(19);
      wt_wh_L1Zgint = ac_weights->at(20);
      wt_zh_a1 = ac_weights->at(21);
      wt_zh_a2 = ac_weights->at(22);
      wt_zh_a3 = ac_weights->at(23);
      wt_zh_L1 = ac_weights->at(24);
      wt_zh_L1Zg = ac_weights->at(25);
      wt_zh_a2int = ac_weights->at(26);
      wt_zh_a3int = ac_weights->at(27);
      wt_zh_L1int = ac_weights->at(28);
      wt_zh_L1Zgint = ac_weights->at(29);
    }
}

void slim_tree::fillTree(std::vector<std::string> cat, electron *el, tau *t, jet_factory *fjets, met_factory *fmet, event_info *evt, 
                         Float_t mt, Float_t weight, std::vector<double> *ac_weights = nullptr) {

  TLorentzVector higgs(el->getP4() + t->getP4() + fmet->getP4());
  generalFill(cat, fjets, fmet, evt, weight, higgs, mt, ac_weights);

  el_pt = el->getPt();
  el_eta = el->getEta();
  el_phi = el->getPhi();
  el_mass = el->getMass();
  el_charge = el->getCharge();
  el_iso = el->getIso();
  t1_pt = t->getPt();
  t1_eta = t->getEta();
  t1_phi = t->getPhi();
  t1_mass = t->getMass();
  t1_charge = t->getCharge();
  t1_decayMode = t->getL2DecayMode();
  t1_iso = t->getIso();
  t1_iso_VL = t->getVLooseIsoMVA();
  t1_iso_L = t->getLooseIsoMVA();
  t1_iso_M = t->getMediumIsoMVA();
  t1_iso_T = t->getTightIsoMVA();
  t1_iso_VT = t->getVTightIsoMVA();
  t1_iso_VVT = t->getVVTightIsoMVA();
  t1_genMatch = t->getGenMatch();
  dmf = t->getDecayModeFinding();
  dmf_new = t->getDecayModeFindingNew();
  vis_mass = (el->getP4() + t->getP4()).M();

  otree->Fill();
}

void slim_tree::fillTree(std::vector<std::string> cat, muon *mu, tau *t, jet_factory *fjets, met_factory *fmet, event_info *evt, 
                         Float_t mt, Float_t weight, std::vector<double>* ac_weights = nullptr) {

    TLorentzVector higgs(mu->getP4() + t->getP4() + fmet->getP4());
    generalFill(cat, fjets, fmet, evt, weight, higgs, mt, ac_weights);

    mu_pt = mu->getPt();
    mu_eta = mu->getEta();
    mu_phi = mu->getPhi();
    mu_mass = mu->getMass();
    mu_charge = mu->getCharge();
    mu_iso = mu->getIso();
    t1_pt = t->getPt();
    t1_eta = t->getEta();
    t1_phi = t->getPhi();
    t1_mass = t->getMass();
    t1_charge = t->getCharge();
    t1_decayMode = t->getL2DecayMode();
    t1_iso = t->getIso();
    t1_iso_VL = t->getVLooseIsoMVA();
    t1_iso_L = t->getLooseIsoMVA();
    t1_iso_M = t->getMediumIsoMVA();
    t1_iso_T = t->getTightIsoMVA();
    t1_iso_VT = t->getVTightIsoMVA();
    t1_iso_VVT = t->getVVTightIsoMVA();
    t1_genMatch = t->getGenMatch();
    dmf = t->getDecayModeFinding();
    dmf_new = t->getDecayModeFindingNew();
    vis_mass = (mu->getP4() + t->getP4()).M();

    otree->Fill();
}

void slim_tree::fillTree(std::vector<std::string> cat, ditau *t1, ditau *t2, jet_factory *fjets, met_factory *fmet, 
                         event_info *evt, Float_t weight, std::vector<double> *ac_weights = nullptr) {

  TLorentzVector higgs(t1->getP4() + t2->getP4() + fmet->getP4());
  generalFill(cat, fjets, fmet, evt, weight, higgs, mt);

  t1_pt = t1->getPt();
  t1_eta = t1->getEta();
  t1_phi = t1->getPhi();
  t1_mass = t1->getMass();
  t1_charge = t1->getCharge();
  t2_pt = t2->getPt();
  t2_eta = t2->getEta();
  t2_phi = t2->getPhi();
  t2_mass = t2->getMass();
  t2_charge = t2->getCharge();
  vis_mass = (t1->getP4() + t2->getP4()).M();
  t1_iso_VL = t1->getVLooseIsoMVA();
  t1_iso_L = t1->getLooseIsoMVA();
  t1_iso_M = t1->getMediumIsoMVA();
  t1_iso_T = t1->getTightIsoMVA();
  t1_iso_VT = t1->getVTightIsoMVA();
  t1_iso_VVT = t1->getVVTightIsoMVA();
  t2_iso_VL = t2->getVLooseIsoMVA();
  t2_iso_L = t2->getLooseIsoMVA();
  t2_iso_M = t2->getMediumIsoMVA();
  t2_iso_T = t2->getTightIsoMVA();
  t2_iso_VT = t2->getVTightIsoMVA();
  t2_iso_VVT = t2->getVVTightIsoMVA();
  t1_genMatch = t1->getGenMatch();
  t2_genMatch = t2->getGenMatch();

  otree->Fill();
}