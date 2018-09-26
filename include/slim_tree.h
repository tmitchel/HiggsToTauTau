#include <iostream>

#include "TTree.h"
// #include "electron_factory.h"
// #include "muon_factory.h"
// #include "tau_factory.h"
// #include "jet_factory.h"
// #include "met_factory.h"
// #include "event_info.h"

class slim_tree {
public:
    slim_tree(std::string);
    ~slim_tree() {}; // default destructor

    // member functions
    // fill the tree for this event
    void fillTree(std::string, electron*, tau*, jet_factory*, met_factory*, event_info*, Float_t);
    void fillTree(std::string, muon*    , tau*, jet_factory*, met_factory*, event_info*, Float_t);
    void fillTree(std::string, tau*     , tau*, jet_factory*, met_factory*, event_info*, Float_t);

    // member data
    TTree* otree;
    Int_t cat_0jet, cat_boosted, cat_vbf, cat_inclusive;
    Float_t evtwt,
        el_pt, el_eta, el_phi, el_mass,
        mu_pt, mu_eta, mu_phi, mu_mass,
        t1_pt, t1_eta, t1_phi, t1_mass, // t1 is used for et and mt, as well
        t2_pt, t2_eta, t2_phi, t2_mass,
        j1_pt, j1_eta, j1_phi,
        j2_pt, j2_eta, j2_phi,
        b1_pt, b1_eta, b1_phi,
        b2_pt, b2_eta, b2_phi,
        met, metphi,
        higgs_pT, higgs_m, hjj_pT, hjj_m, dEtajj, dPhijj;
};

slim_tree::slim_tree(std::string tree_name) : otree( new TTree(tree_name.c_str(), tree_name.c_str()) ) {
    otree->Branch("evtwt",         &evtwt,         "evtwt/F"          );

    otree->Branch("el_pt",         &el_pt,         "el_pt/F"          );
    otree->Branch("el_eta",        &el_eta,        "el_eta/F"         );
    otree->Branch("el_phi",        &el_phi,        "el_phi/F"         );
    otree->Branch("el_mass",       &el_mass,       "el_mass/F"        );
    otree->Branch("mu_pt",         &mu_pt,         "mu_pt/F"          );
    otree->Branch("mu_eta",        &mu_eta,        "mu_eta/F"         );
    otree->Branch("mu_phi",        &mu_phi,        "mu_phi/F"         );
    otree->Branch("mu_mass",       &mu_mass,       "mu_mass/F"        );
    otree->Branch("t1_pt",         &t1_pt,         "t1_pt/F"          );
    otree->Branch("t1_eta",        &t1_eta,        "t1_eta/F"         );
    otree->Branch("t1_phi",        &t1_phi,        "t1_phi/F"         );
    otree->Branch("t1_mass",       &t1_mass,       "t1_mass/F"        );
    otree->Branch("t2_pt",         &t2_pt,         "t2_pt/F"          );
    otree->Branch("t2_eta",        &t2_eta,        "t2_eta/F"         );
    otree->Branch("t2_phi",        &t2_phi,        "t2_phi/F"         );
    otree->Branch("t2_mass",       &t2_mass,       "t2_mass/F"        );

    otree->Branch("j1_pt",         &j1_pt,         "j1_pt/F"          );
    otree->Branch("j1_eta",        &j1_eta,        "j1_eta/F"         );
    otree->Branch("j1_phi",        &j1_phi,        "j1_phi/F"         );
    otree->Branch("j2_pt",         &j2_pt,         "j2_pt/F"          );
    otree->Branch("j2_eta",        &j2_eta,        "j2_eta/F"         );
    otree->Branch("j2_phi",        &j2_phi,        "j2_phi/F"         );
    otree->Branch("b1_pt",         &b1_pt,         "b1_pt/F"          );
    otree->Branch("b1_eta",        &b1_eta,        "b1_eta/F"         );
    otree->Branch("b1_phi",        &b1_phi,        "b1_phi/F"         );
    otree->Branch("b2_pt",         &b2_pt,         "b2_pt/F"          );
    otree->Branch("b2_eta",        &b2_eta,        "b2_eta/F"         );
    otree->Branch("b2_phi",        &b2_phi,        "b2_phi/F"         );

    otree->Branch("met",           &met,           "met/F"            );
    otree->Branch("metphi",        &metphi,        "metphi/F"         );

    otree->Branch("higgs_pT",      &higgs_pT,      "higgs_pT/F"       );
    otree->Branch("higgs_m",       &higgs_m,       "higgs_m/F"        );
    otree->Branch("hjj_pT",        &hjj_pT,        "hjj_pT/F"         );
    otree->Branch("hjj_m",         &hjj_m,         "hjj_m/F"          );
    otree->Branch("dEtajj",        &dEtajj,        "dEtajj/F"         );
    otree->Branch("dPhijj",        &dPhijj,        "dPhijj/F"         );
    otree->Branch("cat_0jet",      &cat_0jet,      "cat_0jet/I"       );
    otree->Branch("cat_boosted",   &cat_boosted,   "cat_boosted/I"    );
    otree->Branch("cat_vbf",       &cat_vbf,       "cat_vbf/I"        );
    otree->Branch("cat_inclusive", &cat_inclusive, "cat_inclusive/I"  );
}

void slim_tree::fillTree(std::string cat, electron* el, tau* t, jet_factory* fjets, met_factory* fmet, event_info* evt, Float_t weight) {
    // create things needed for later
    auto jets( fjets->getJets() );
    TLorentzVector higgs( el->getP4() + t->getP4() + fmet->getP4() );

    // start filling branches
    evtwt = weight;
    higgs_pT = higgs.Pt();
    higgs_m = higgs.M();
    el_pt = el->getPt();
    el_eta = el->getEta();
    el_phi = el->getPhi();
    el_mass = el->getMass();
    t1_pt = t->getPt();
    t1_eta = t->getEta();
    t1_phi = t->getPhi();
    t1_mass = t->getMass();
    met = fmet->getMet();
    metphi = fmet->getMetPhi();

    // dijet info is only ok if you have 2 jets, imagine that
    hjj_pT = 0.;
    hjj_m = 0.;
    dEtajj = 0.;
    dPhijj = 0.;
    j1_pt = 0; j1_eta = 0; j1_phi = 0;
    j2_pt = 0; j2_eta = 0; j2_phi = 0;
    b1_pt = 0; b1_eta = 0; b1_phi = 0;
    b2_pt = 0; b2_eta = 0; b2_phi = 0;

    if (fjets->getNjets() > 0) {
        j1_pt = jets.at(0).getPt();
        j1_eta = jets.at(0).getEta();
        j1_phi = jets.at(0).getPhi();
        if (fjets->getNjets() > 1) {
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
    cat_0jet      = 0;
    cat_boosted   = 0;
    cat_vbf       = 0;
    cat_inclusive = 0;

    // decide on which selections have been passed
    if (cat == "et_0jet") {
        cat_0jet = 1;
    } else if (cat == "et_boosted") {
        cat_boosted = 1;
    } else if (cat == "et_vbf") {
        cat_vbf = 1;
    } else if (cat == "et_inclusive") {
        cat_inclusive = 1;
    }

    otree->Fill();
}
