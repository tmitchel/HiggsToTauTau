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
    Float_t evtwt, higgs_pT, higgs_m, hjj_pT, hjj_m, dEtajj, dPhijj;
    // TBranch *b_weight, *b_passSelection, *b_higgs_pT, *b_higgs_m, *b_hjj_pT, *b_hjj_m, *b_dEtajj, *b_dPhijj;
};

slim_tree::slim_tree(std::string tree_name) : otree( new TTree("tree_name", "tree_name") ) {
//   b_weight        = otree->Branch("evtwt",         &weight,        "evtwt/F");
//   b_passSelection = otree->Branch("passSelection", &passSelection, "passSelection/F");
//   b_higgs_pT      = otree->Branch("higgs_pT",      &higgs_pT,      "higgs_pT/F");
//   b_higgs_m       = otree->Branch("higgs_m",       &higgs_m,       "higgs_m/F");
//   b_hjj_pT        = otree->Branch("hjj_pT",        &hjj_pT,        "hjj_pT/F");
//   b_hjj_m         = otree->Branch("hjj_m",         &hjj_m,         "hjj_m/F");
//   b_dEtajj        = otree->Branch("dEtajj",        &dEtajj,        "dEtajj/F");
//   b_dPhijj        = otree->Branch("dPhijj",        &dPhijj,        "dPhijj/F");
    otree->Branch("evtwt",         &evtwt,         "evtwt/F"          );
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

void slim_tree::fillTree(std::string cat, electron* el, tau* t, jet_factory* fjets, met_factory* met, event_info* evt, Float_t weight) {
    // create things needed for later
    auto jets( fjets->getJets() );
    TLorentzVector higgs( el->getP4() + t->getP4() + met->getP4() );

    // start filling branches
    evtwt = weight;
    higgs_pT = higgs.Pt();
    higgs_m = higgs.M();
    hjj_pT = (higgs + jets.at(0).getP4() + jets.at(1).getP4()).Pt();
    hjj_m = (higgs + jets.at(0).getP4() + jets.at(1).getP4()).M();
    dEtajj = fabs(jets.at(0).getEta() - jets.at(1).getEta());
    dPhijj = fabs(jets.at(0).getPhi() - jets.at(1).getPhi());

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
