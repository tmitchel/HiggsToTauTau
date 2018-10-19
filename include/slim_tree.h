#include <iostream>
#include "TTree.h"
#include "ditau_factory.h"
#include "tau_factory.h"

class slim_tree {
public:
    slim_tree(std::string);
    ~slim_tree() {}; // default destructor

    // member functions
    // fill the tree for this event
    void fillTree(std::vector<std::string>, electron*  , tau*, jet_factory*, met_factory*, event_info*, Float_t, Float_t);
    void fillTree(std::vector<std::string>, muon*      , tau*, jet_factory*, met_factory*, event_info*, Float_t, Float_t);
    void fillTree(std::vector<std::string>, ditau*     , ditau*, jet_factory*, met_factory*, event_info*, Float_t, Float_t);
    void generalFill(std::vector<std::string>, jet_factory*, met_factory*, event_info*, Float_t, TLorentzVector, Float_t);

    // member data
    TTree* otree;
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_inclusive, cat_antiiso, cat_antiiso_0jet, cat_antiiso_boosted, cat_antiiso_vbf,
        cat_qcd, cat_qcd_0jet, cat_qcd_boosted, cat_qcd_vbf;
    Float_t evtwt,
        el_pt, el_eta, el_phi, el_mass, el_charge,
        mu_pt, mu_eta, mu_phi, mu_mass, mu_charge,
        t1_pt, t1_eta, t1_phi, t1_mass, t1_charge, // t1 is used for et and mt, as well
        t2_pt, t2_eta, t2_phi, t2_mass, t2_charge,
        njets, nbjets,
        j1_pt, j1_eta, j1_phi,
        j2_pt, j2_eta, j2_phi,
        b1_pt, b1_eta, b1_phi,
        b2_pt, b2_eta, b2_phi,
        met, metphi, mjj,  numGenJets, mt,
        pt_sv, m_sv, Dbkg_VBF, Dbkg_ggH,
        Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2,
        ME_sm_VBF, ME_sm_ggH, ME_sm_WH, ME_sm_ZH, ME_bkg, ME_bkg1, ME_bkg2,
        higgs_pT, higgs_m, hjj_pT, hjj_m, dEtajj, dPhijj, l2decay, vis_mass;
};

slim_tree::slim_tree(std::string tree_name) : otree( new TTree(tree_name.c_str(), tree_name.c_str()) ) {
    otree->Branch("evtwt",               &evtwt,               "evtwt/F"              );

    otree->Branch("el_pt",               &el_pt,               "el_pt/F"              );
    otree->Branch("el_eta",              &el_eta,              "el_eta/F"             );
    otree->Branch("el_phi",              &el_phi,              "el_phi/F"             );
    otree->Branch("el_mass",             &el_mass,             "el_mass/F"            );
    otree->Branch("el_charge",           &el_charge,           "el_charge/F"          );
    otree->Branch("mu_pt",               &mu_pt,               "mu_pt/F"              );
    otree->Branch("mu_eta",              &mu_eta,              "mu_eta/F"             );
    otree->Branch("mu_phi",              &mu_phi,              "mu_phi/F"             );
    otree->Branch("mu_mass",             &mu_mass,             "mu_mass/F"            );
    otree->Branch("mu_charge",           &mu_charge,           "mu_charge/F"          );
    otree->Branch("t1_pt",               &t1_pt,               "t1_pt/F"              );
    otree->Branch("t1_eta",              &t1_eta,              "t1_eta/F"             );
    otree->Branch("t1_phi",              &t1_phi,              "t1_phi/F"             );
    otree->Branch("t1_mass",             &t1_mass,             "t1_mass/F"            );
    otree->Branch("t1_charge",           &t1_charge,           "t1_charge/F"          );
    otree->Branch("t2_pt",               &t2_pt,               "t2_pt/F"              );
    otree->Branch("t2_eta",              &t2_eta,              "t2_eta/F"             );
    otree->Branch("t2_phi",              &t2_phi,              "t2_phi/F"             );
    otree->Branch("t2_mass",             &t2_mass,             "t2_mass/F"            );
    otree->Branch("t2_charge",           &t2_charge,           "t2_charge/F"          );

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
    otree->Branch("ME_sm_VBF"   ,        &ME_sm_VBF    ,        "ME_sm_/F"            );
    otree->Branch("ME_sm_ggH"   ,        &ME_sm_ggH    ,        "ME_sm_/F"            );
    otree->Branch("ME_sm_WH"    ,        &ME_sm_WH     ,        "ME_sm_WH/F"          );
    otree->Branch("ME_sm_ZH"    ,        &ME_sm_ZH     ,        "ME_sm_ZH/F"          );
    otree->Branch("ME_bkg"      ,        &ME_bkg       ,        "MEbkg_/F"            );
    otree->Branch("ME_bkg1"     ,        &ME_bkg1      ,        "MEbkg1_/F"           );
    otree->Branch("ME_bkg2"     ,        &ME_bkg2      ,        "MEbkg2_/F"           );

    otree->Branch("higgs_pT",            &higgs_pT,            "higgs_pT/F"           );
    otree->Branch("higgs_m",             &higgs_m,             "higgs_m/F"            );
    otree->Branch("hjj_pT",              &hjj_pT,              "hjj_pT/F"             );
    otree->Branch("hjj_m",               &hjj_m,               "hjj_m/F"              );
    otree->Branch("l2decay",             &l2decay,             "l2decay/F"            );
    otree->Branch("vis_mass",              &vis_mass,              "vis_mass/F"             );
    otree->Branch("dEtajj",              &dEtajj,              "dEtajj/F"             );
    otree->Branch("dPhijj",              &dPhijj,              "dPhijj/F"             );
    otree->Branch("cat_0jet",            &cat_0jet,            "cat_0jet/I"           );
    otree->Branch("cat_boosted",         &cat_boosted,         "cat_boosted/I"        );
    otree->Branch("cat_vbf",             &cat_vbf,             "cat_vbf/I"            );
    otree->Branch("cat_inclusive",       &cat_inclusive,       "cat_inclusive/I"      );
    otree->Branch("cat_antiiso",         &cat_antiiso,         "cat_antiiso/I"        );
    otree->Branch("cat_antiiso_0jet",    &cat_antiiso_0jet,    "cat_antiiso_0jet/I"   );
    otree->Branch("cat_antiiso_boosted", &cat_antiiso_boosted, "cat_antiiso_boosted/I");
    otree->Branch("cat_antiiso_vbf",     &cat_antiiso_vbf,     "cat_antiiso_vbf/I"    );
    otree->Branch("cat_qcd",             &cat_qcd,             "cat_qcd/I"            );
    otree->Branch("cat_qcd_0jet",        &cat_qcd_0jet,        "cat_qcd_0jet/I"       );
    otree->Branch("cat_qcd_boosted",     &cat_qcd_boosted,     "cat_qcd_boosted/I"    );
    otree->Branch("cat_qcd_vbf",         &cat_qcd_vbf,         "cat_qcd_vbf/I"        );
}

void slim_tree::generalFill(std::vector<std::string> cats, jet_factory* fjets, met_factory* fmet, event_info* evt, Float_t weight, TLorentzVector higgs, Float_t Mt) {
    // create things needed for later
    auto jets(fjets->getJets());

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

    // reset the categories
    cat_0jet = 0;
    cat_boosted = 0;
    cat_vbf = 0;
    cat_inclusive = 0;
    cat_antiiso = 0;
    cat_antiiso_0jet = 0;
    cat_antiiso_boosted = 0;
    cat_antiiso_vbf = 0;
    cat_qcd = 0;
    cat_qcd_0jet = 0;
    cat_qcd_boosted = 0;
    cat_qcd_vbf = 0;

    // decide on which selections have been passed
    for (auto cat : cats) {
        if (cat == "0jet") {
            cat_inclusive = 1;
            cat_0jet = 1;
        } else if (cat == "boosted") {
            cat_inclusive = 1;
            cat_boosted = 1;
        } else if (cat == "vbf") {
            cat_inclusive = 1;
            cat_vbf = 1;
        } else if (cat == "inclusive") {
            cat_inclusive = 1;
        } else if (cat == "antiiso_0jet") {
            cat_antiiso = 1;
            cat_antiiso_0jet = 1;
        } else if (cat == "antiiso_boosted") {
            cat_antiiso = 1;
            cat_antiiso_boosted = 1;
        } else if (cat == "antiiso_vbf") {
            cat_antiiso = 1;
            cat_antiiso_vbf = 1;
        } else if (cat == "antiiso") {
            cat_antiiso = 1;
        } else if (cat == "looseIso_0jet") {
            cat_qcd = 1;
            cat_qcd_0jet = 1;
        } else if (cat == "looseIso_boosted") {
            cat_qcd = 1;
            cat_qcd_boosted = 1;
        } else if (cat == "looseIso_vbf") {
            cat_qcd = 1;
            cat_qcd_vbf = 1;
        } else if (cat == "looseIso") {
            cat_qcd = 1;
        }
    }
}

void slim_tree::fillTree(std::vector<std::string> cat, electron* el, tau* t, jet_factory* fjets, met_factory* fmet, event_info* evt, Float_t mt, Float_t weight) {

    TLorentzVector higgs(el->getP4() + t->getP4() + fmet->getP4());
    generalFill(cat, fjets, fmet, evt, weight, higgs, mt);

    el_pt = el->getPt();
    el_eta = el->getEta();
    el_phi = el->getPhi();
    el_mass = el->getMass();
    el_charge = el->getCharge();
    t1_pt = t->getPt();
    t1_eta = t->getEta();
    t1_phi = t->getPhi();
    t1_mass = t->getMass();
    t1_charge = t->getCharge();
    vis_mass = (el->getP4() + t->getP4()).M();
    l2decay = t->getL2DecayMode();

    otree->Fill();
}

void slim_tree::fillTree(std::vector<std::string> cat, ditau *t1, ditau *t2, jet_factory *fjets, met_factory *fmet, event_info *evt, Float_t mt, Float_t weight) {

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

    otree->Fill();
}

void slim_tree::fillTree(std::vector<std::string> cat, muon *mu, tau *t1, jet_factory *fjets, met_factory *fmet, event_info *evt, Float_t mt, Float_t weight) {

    TLorentzVector higgs(mu->getP4() + t1->getP4() + fmet->getP4());
    generalFill(cat, fjets, fmet, evt, weight, higgs, mt);

    mu_pt = mu->getPt();
    mu_eta = mu->getEta();
    mu_phi = mu->getPhi();
    mu_mass = mu->getMass();
    mu_charge = mu->getCharge();
    t1_pt = t1->getPt();
    t1_eta = t1->getEta();
    t1_phi = t1->getPhi();
    t1_mass = t1->getMass();
    t1_charge = t1->getCharge();
    vis_mass = (mu->getP4() + t1->getP4()).M();
    l2decay = t1->getL2DecayMode();

    otree->Fill();
}
