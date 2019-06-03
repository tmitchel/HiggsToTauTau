#ifndef EVENT_INFO_H
#define EVENT_INFO_H

#include <string>
#include "./swiss_army_class.h"

/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
   private:
    Float_t genpX, genpY, genM, genpT, numGenJets, genweight, genDR;  // gen
    Float_t npv, npu, rho;                                            // event
    Float_t matchEle25, filterEle25, passEle25;                       // 2016 etau trigger
    Float_t matchEle27, filterEle27, passEle27;                       // 2017 single el 27 trigger
    Float_t matchEle32, filterEle32, passEle32;                       // 2017 single el 32 trigger
    Float_t matchEle35, filterEle35, passEle35;                       // 2017 single el 35 trigger
    Float_t matchEle24Tau30, filterEle24Tau30, passEle24Tau30;        // 2017 etau cross trigger
    Float_t passDoubleTauCmbIso35, matchDoubleTauCmbIso35_1, filterDoubleTauCmbIso35_1, matchDoubleTauCmbIso35_2,
        filterDoubleTauCmbIso35_2;  // tt trigger
    Float_t passDoubleTau35, matchDoubleTau35_1, filterDoubleTau35_1, matchDoubleTau35_2,
        filterDoubleTau35_2;  // tt trigger
    Float_t matchIsoMu19Tau20_1, matchIsoMu19Tau20_2, filterIsoMu19Tau20_1, filterIsoMu19Tau20_2,
        passIsoMu19Tau20;                                                          // cross trigger
    Float_t matchIsoMu22_1, filterIsoMu22_1, passIsoMu22;                          // single muon trigger
    Float_t matchIsoTkMu22_1, filterIsoTkMu22_1, passIsoTkMu22;                    // single muon trigger
    Float_t matchIsoMu22eta2p1_1, filterIsoMu22eta2p1_1, passIsoMu22eta2p1;        // single muon trigger
    Float_t matchIsoTkMu22eta2p1_1, filterIsoTkMu22eta2p1_1, passIsoTkMu22eta2p1;  // single muon trigger
    Float_t mMatchesIsoMu20Tau27Path, mMatchesIsoMu20Tau27Filter, tMatchesIsoMu20Tau27Path, tMatchesIsoMu20Tau27Filter,
        Mu20Tau27Pass;  // 2017 single muon trigger
    Float_t mMatchesIsoMu24Path, mMatchesIsoMu24Filter, IsoMu24Pass, mMatchesIsoMu27Path, mMatchesIsoMu27Filter,
        IsoMu27Pass;  // 2017 single muon trigger
    Bool_t PassEle25, PassEle27, PassEle32, PassEle35, PassEle24Tau30, PassDoubleTauCmbIso35, PassDoubleTau35,
        PassIsoMu19Tau20, PassIsoMu22, PassIsoTkMu22, PassIsoMu22eta2p1, PassIsoTkMu22eta2p1;
    Bool_t PassMu24, PassMu27, PassMu20Tau27;
    Float_t m_sv, pt_sv;                                                                                      // SVFit
    Float_t Dbkg_VBF, Dbkg_ggH, Dbkg_ZH, Dbkg_WH, Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2;  // MELA
    Float_t ME_sm_VBF, ME_sm_ggH, ME_sm_WH, ME_sm_ZH, ME_bkg, ME_bkg1, ME_bkg2, D0_VBF, DCP_VBF, D0_ggH, DCP_ggH,
        ME_ps_VBF, ME_ps_ggH, ME_sm_ggH_qqInit, ME_ps_ggH_qqInit;
    Float_t Rivet_nJets30, Rivet_higgsPt;
    Float_t Flag_BadChargedCandidateFilter, Flag_BadPFMuonFilter, Flag_EcalDeadCellTriggerPrimitiveFilter,
        Flag_HBHENoiseFilter, Flag_HBHENoiseIsoFilter, Flag_badMuons, Flag_duplicateMuons, Flag_ecalBadCalibFilter,
        Flag_eeBadScFilter, Flag_globalSuperTightHalo2016Filter, Flag_globalTightHalo2016Filter, Flag_goodVertices;

    UInt_t run, lumi;
    ULong64_t evt;
    UInt_t convert_evt;

    bool isEmbed;

   public:
    event_info(TTree*, lepton, int, std::string);
    virtual ~event_info() {}
    void setEmbed() { isEmbed = true; }
    void setRivets(TTree*);

    // tautau Trigger Info
    Bool_t getPassEle25();
    Bool_t getPassEle27();
    Bool_t getPassEle32();
    Bool_t getPassEle35();
    Bool_t getPassEle24Tau30();
    Bool_t getPassMu19Tau20();
    Bool_t getPassIsoMu22();
    Bool_t getPassIsoTkMu22();
    Bool_t getPassIsoMu22eta2p1();
    Bool_t getPassIsoTkMu22eta2p1();
    Bool_t getPassMu20Tau27();
    Bool_t getPassMu24();
    Bool_t getPassMu27();
    Bool_t getPassDoubleTauCmbIso35();
    Bool_t getPassDoubleTau35();
    Bool_t getPassFlags(Bool_t);

    // Event Info
    Float_t getNPV() { return npv; }
    Float_t getNPU() { return npu; }
    Float_t getRho() { return rho; }
    UInt_t getRun() { return run; }
    UInt_t getLumi() { return lumi; }
    ULong64_t getEvt() { return evt; }

    // Generator Info
    Float_t getGenPx() { return genpX; }
    Float_t getGenPy() { return genpY; }
    Float_t getGenM() { return genM; }
    Float_t getGenPt() { return genpT; }
    Float_t getNumGenJets() { return numGenJets; }
    Float_t getGenWeight() { return genweight; }
    Float_t getTauGenDR() { return genDR; }

    // SVFit Info
    Float_t getMSV() { return m_sv; }
    Float_t getPtSV() { return pt_sv; }

    // MELA Info
    Float_t getDbkg_VBF() { return Dbkg_VBF; }
    Float_t getDbkg_ggH() { return Dbkg_ggH; }
    Float_t getDbkg_ZH() { return Dbkg_ZH; }
    Float_t getDbkg_WH() { return Dbkg_WH; }
    Float_t getD0_VBF() { return D0_VBF; }
    Float_t getDCP_VBF() { return DCP_VBF; }
    Float_t getD0_ggH() { return D0_ggH; }
    Float_t getDCP_ggH() { return DCP_ggH; }
    Float_t getPhi() { return Phi; }
    Float_t getPhi1() { return Phi1; }
    Float_t getCosTheta1() { return costheta1; }
    Float_t getCosTheta2() { return costheta2; }
    Float_t getCosThetaStar() { return costhetastar; }
    Float_t getQ2V1() { return Q2V1; }
    Float_t getQ2V2() { return Q2V2; }
    Float_t getME_sm_ggH_qqInit() { return ME_sm_ggH_qqInit; }
    Float_t getME_ps_ggH_qqInit() { return ME_ps_ggH_qqInit; }
    Float_t getME_sm_VBF() { return ME_sm_VBF; }
    Float_t getME_sm_ggH() { return ME_sm_ggH; }
    Float_t getME_ps_VBF() { return ME_ps_VBF; }
    Float_t getME_ps_ggH() { return ME_ps_ggH; }
    Float_t getME_sm_WH() { return ME_sm_WH; }
    Float_t getME_sm_ZH() { return ME_sm_ZH; }
    Float_t getME_bkg() { return ME_bkg; }
    Float_t getME_bkg1() { return ME_bkg1; }
    Float_t getME_bkg2() { return ME_bkg2; }

    // ggH NNLOPS Info
    Float_t getNjetsRivet() { return Rivet_nJets30; }
    Float_t getHiggsPtRivet() { return Rivet_higgsPt; }
};

// read data from trees into member variables
event_info::event_info(TTree* input, lepton lep, int era, std::string syst) : isEmbed(false) {
    std::string m_sv_name("m_sv"), pt_sv_name("pt_sv"), Dbkg_VBF_name("Dbkg_VBF"), Dbkg_ggH_name("Dbkg_ggH"),
        Dbkg_ZH_name("Dbkg_ZH"), Dbkg_WH_name("Dbkg_WH"), D_PS_VBF_name("D_PS_VBF"), D_CP_VBF_name("D_CP_VBF"),
        D_PS_ggH_name("D_PS_ggH"), D_CP_ggH_name("D_CP_ggH"), Phi_name("Phi"), Phi1_name("Phi1"),
        costheta1_name("costheta1"), costheta2_name("costheta2"), costhetastar_name("costhetastar"), Q2V1_name("Q2V1"),
        Q2V2_name("Q2V2"), ME_sm_VBF_name("ME_sm_VBF"), ME_sm_ggH_name("ME_sm_ggH"), ME_sm_WH_name("ME_sm_WH"),
        ME_sm_ZH_name("ME_sm_ZH"), ME_ps_VBF_name("ME_ps_VBF"), ME_ps_ggH_name("ME_ps_ggH"), ME_bkg1_name("ME_bkg1"),
        ME_bkg2_name("ME_bkg2"), ME_bkg_name("ME_bkg"), ME_sm_ggH_qqInit_name("ME_sm_ggH_qqInit"),
        ME_ps_ggH_qqInit_name("ME_ps_ggH_qqInit");
    if (syst.find("UncMet") != std::string::npos || syst.find("ClusteredMet") != std::string::npos ||
        syst.find("DM") != std::string::npos || syst == "Up" || syst == "Down") {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        // need to update MELA code to get these branches
        // Dbkg_VBF_name += "_" + syst;
        // Dbkg_ggH_name += "_" + syst;
        // Dbkg_ZH_name += "_" + syst;
        // Dbkg_WH_name += "_" + syst;
        // D_PS_VBF_name += "_" + syst;
        // D_CP_VBF_name += "_" + syst;
        // D_PS_ggH_name += "_" + syst;
        // D_CP_ggH_name += "_" + syst;
        // Phi_name += "_" + syst;
        // Phi1_name += "_" + syst;
        // costheta1_name += "_" + syst;
        // costheta2_name += "_" + syst;
        // costhetastar_name += "_" + syst;
        // Q2V1_name += "_" + syst;
        // Q2V2_name += "_" + syst;
        // ME_sm_VBF_name += "_" + syst;
        // ME_sm_ggH_name += "_" + syst;
        // ME_sm_WH_name += "_" + syst;
        // ME_sm_ZH_name += "_" + syst;
        // ME_ps_VBF_name += "_" + syst;
        // ME_ps_ggH_name += "_" + syst;
        // ME_bkg1_name += "_" + syst;
        // ME_bkg2_name += "_" + syst;
        // ME_bkg_name += "_" + syst;
    }

    input->SetBranchAddress(m_sv_name.c_str(), &m_sv);
    input->SetBranchAddress(pt_sv_name.c_str(), &pt_sv);
    input->SetBranchAddress(Dbkg_VBF_name.c_str(), &Dbkg_VBF);
    input->SetBranchAddress(Dbkg_ggH_name.c_str(), &Dbkg_ggH);
    input->SetBranchAddress(Dbkg_ZH_name.c_str(), &Dbkg_ZH);
    input->SetBranchAddress(Dbkg_WH_name.c_str(), &Dbkg_WH);
    input->SetBranchAddress(D_PS_VBF_name.c_str(), &D0_VBF);
    input->SetBranchAddress(D_CP_VBF_name.c_str(), &DCP_VBF);
    input->SetBranchAddress(D_PS_ggH_name.c_str(), &D0_ggH);
    input->SetBranchAddress(D_CP_ggH_name.c_str(), &DCP_ggH);
    input->SetBranchAddress(Phi_name.c_str(), &Phi);
    input->SetBranchAddress(Phi1_name.c_str(), &Phi1);
    input->SetBranchAddress(costheta1_name.c_str(), &costheta1);
    input->SetBranchAddress(costheta2_name.c_str(), &costheta2);
    input->SetBranchAddress(costhetastar_name.c_str(), &costhetastar);
    input->SetBranchAddress(Q2V1_name.c_str(), &Q2V1);
    input->SetBranchAddress(Q2V2_name.c_str(), &Q2V2);
    input->SetBranchAddress(ME_sm_ggH_qqInit_name.c_str(), &ME_sm_ggH_qqInit);
    input->SetBranchAddress(ME_ps_ggH_qqInit_name.c_str(), &ME_ps_ggH_qqInit);
    input->SetBranchAddress(ME_sm_VBF_name.c_str(), &ME_sm_VBF);
    input->SetBranchAddress(ME_sm_ggH_name.c_str(), &ME_sm_ggH);
    input->SetBranchAddress(ME_ps_VBF_name.c_str(), &ME_ps_VBF);
    input->SetBranchAddress(ME_ps_ggH_name.c_str(), &ME_ps_ggH);
    input->SetBranchAddress(ME_sm_WH_name.c_str(), &ME_sm_WH);
    input->SetBranchAddress(ME_sm_ZH_name.c_str(), &ME_sm_ZH);
    input->SetBranchAddress(ME_bkg_name.c_str(), &ME_bkg);
    input->SetBranchAddress(ME_bkg1_name.c_str(), &ME_bkg1);
    input->SetBranchAddress(ME_bkg2_name.c_str(), &ME_bkg2);
    input->SetBranchAddress("run", &run);
    input->SetBranchAddress("lumi", &lumi);
    input->SetBranchAddress("nvtx", &npv);
    input->SetBranchAddress("nTruePU", &npu);
    input->SetBranchAddress("genpX", &genpX);
    input->SetBranchAddress("genpY", &genpY);
    input->SetBranchAddress("genM", &genM);
    input->SetBranchAddress("genpT", &genpT);
    input->SetBranchAddress("numGenJets", &numGenJets);
    input->SetBranchAddress("GenWeight", &genweight);
    input->SetBranchAddress("Flag_BadChargedCandidateFilter", &Flag_BadChargedCandidateFilter);
    input->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_BadPFMuonFilter);
    input->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
    input->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter);
    input->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter);
    input->SetBranchAddress("Flag_badMuons", &Flag_badMuons);
    input->SetBranchAddress("Flag_duplicateMuons", &Flag_duplicateMuons);
    input->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter);
    input->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
    input->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter);
    input->SetBranchAddress("Flag_globalTightHalo2016Filter", &Flag_globalTightHalo2016Filter);
    input->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);

    if (lep == lepton::ELECTRON) {
        input->SetBranchAddress("evt", &evt);
        input->SetBranchAddress("eMatchesSingleE25Tight", &matchEle25);
        input->SetBranchAddress("eMatchesEle25TightFilter", &filterEle25);
        input->SetBranchAddress("singleE25eta2p1TightPass", &passEle25);
        input->SetBranchAddress("eMatchesEle27Path", &matchEle27);
        input->SetBranchAddress("eMatchesEle27Filter", &filterEle27);
        input->SetBranchAddress("Ele27WPTightPass", &passEle27);
        input->SetBranchAddress("eMatchesEle32Path", &matchEle32);
        input->SetBranchAddress("eMatchesEle32Filter", &filterEle32);
        input->SetBranchAddress("Ele32WPTightPass", &passEle32);
        input->SetBranchAddress("eMatchesEle35Path", &matchEle35);
        input->SetBranchAddress("eMatchesEle35Filter", &filterEle35);
        input->SetBranchAddress("Ele35WPTightPass", &passEle35);
        input->SetBranchAddress("eMatchesEle24Tau30Path", &matchEle24Tau30);
        input->SetBranchAddress("eMatchesEle24Tau30Filter", &filterEle24Tau30);
        input->SetBranchAddress("Ele24Tau30Pass", &passEle24Tau30);
    } else if (lep == lepton::MUON) {
        input->SetBranchAddress("evt", &evt);
        input->SetBranchAddress("tZTTGenDR", &genDR);
        input->SetBranchAddress("mMatchesIsoMu19Tau20Path", &matchIsoMu19Tau20_1);
        input->SetBranchAddress("tMatchesIsoMu19Tau20Path", &matchIsoMu19Tau20_2);
        input->SetBranchAddress("mMatchesIsoMu19Tau20Filter", &filterIsoMu19Tau20_1);
        input->SetBranchAddress("tMatchesIsoMu19Tau20Filter", &filterIsoMu19Tau20_2);
        input->SetBranchAddress("passIsoMu19Tau20", &passIsoMu19Tau20);
        input->SetBranchAddress("matchIsoMu22_1", &matchIsoMu22_1);
        input->SetBranchAddress("filterIsoMu22_1", &filterIsoMu22_1);
        input->SetBranchAddress("passIsoMu22", &passIsoMu22);
        input->SetBranchAddress("matchIsoTkMu22_1", &matchIsoTkMu22_1);
        input->SetBranchAddress("filterIsoTkMu22_1", &filterIsoTkMu22_1);
        input->SetBranchAddress("passIsoTkMu22", &passIsoTkMu22);
        input->SetBranchAddress("matchIsoMu22eta2p1_1", &matchIsoMu22eta2p1_1);
        input->SetBranchAddress("filterIsoMu22eta2p1_1", &filterIsoMu22eta2p1_1);
        input->SetBranchAddress("passIsoMu22eta2p1", &passIsoMu22eta2p1);
        input->SetBranchAddress("matchIsoTkMu22eta2p1_1", &matchIsoTkMu22eta2p1_1);
        input->SetBranchAddress("filterIsoTkMu22eta2p1_1", &filterIsoTkMu22eta2p1_1);
        input->SetBranchAddress("passIsoTkMu22eta2p1", &passIsoTkMu22eta2p1);
        input->SetBranchAddress("mMatchesIsoMu20Tau27Path", &mMatchesIsoMu20Tau27Path);
        input->SetBranchAddress("mMatchesIsoMu20Tau27Filter", &mMatchesIsoMu20Tau27Filter);
        input->SetBranchAddress("tMatchesIsoMu20Tau27Path", &tMatchesIsoMu20Tau27Path);
        input->SetBranchAddress("tMatchesIsoMu20Tau27Filter", &tMatchesIsoMu20Tau27Filter);
        input->SetBranchAddress("Mu20Tau27Pass", &Mu20Tau27Pass);
        input->SetBranchAddress("mMatchesIsoMu24Path", &mMatchesIsoMu24Path);
        input->SetBranchAddress("mMatchesIsoMu24Filter", &mMatchesIsoMu24Filter);
        input->SetBranchAddress("IsoMu24Pass", &IsoMu24Pass);
        input->SetBranchAddress("mMatchesIsoMu27Path", &mMatchesIsoMu27Path);
        input->SetBranchAddress("mMatchesIsoMu27Filter", &mMatchesIsoMu27Filter);
        input->SetBranchAddress("IsoMu27Pass", &IsoMu27Pass);

    } else if (lep == lepton::DITAU) {
        input->SetBranchAddress("evt", &convert_evt);
        input->SetBranchAddress("passDoubleTauCmbIso35", &passDoubleTauCmbIso35);
        input->SetBranchAddress("matchDoubleTauCmbIso35_1", &matchDoubleTauCmbIso35_1);
        input->SetBranchAddress("filterDoubleTauCmbIso35_1", &filterDoubleTauCmbIso35_1);
        input->SetBranchAddress("matchDoubleTauCmbIso35_2", &matchDoubleTauCmbIso35_2);
        input->SetBranchAddress("filterDoubleTauCmbIso35_2", &filterDoubleTauCmbIso35_2);
        input->SetBranchAddress("passDoubleTau35", &passDoubleTau35);
        input->SetBranchAddress("matchDoubleTau35_1", &matchDoubleTau35_1);
        input->SetBranchAddress("filterDoubleTau35_1", &filterDoubleTau35_1);
        input->SetBranchAddress("matchDoubleTau35_2", &matchDoubleTau35_2);
        input->SetBranchAddress("filterDoubleTau35_2", &filterDoubleTau35_2);
    } else if (lep == lepton::EMU) {
        // no analyzer yet
    } else {
        std::cerr << "HEY! THAT'S NOT AN ANALYZER. WAT U DOIN." << std::endl;
    }
}

void event_info::setRivets(TTree* input) {
    input->SetBranchAddress("Rivet_nJets30", &Rivet_nJets30);
    input->SetBranchAddress("Rivet_higgsPt", &Rivet_higgsPt);
}

Bool_t event_info::getPassFlags(bool is_data) {
    if (Flag_goodVertices || Flag_globalTightHalo2016Filter || Flag_HBHENoiseFilter || Flag_HBHENoiseIsoFilter ||
        Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter || Flag_BadChargedCandidateFilter) {
        return false;
    } else if (is_data && (Flag_eeBadScFilter || Flag_ecalBadCalibFilter)) {
        return false;
    } else {
        return true;
    }
}

Bool_t event_info::getPassEle25() { return PassEle25; }

Bool_t event_info::getPassEle27() {
    PassEle27 = passEle27 && filterEle27 && matchEle27;
    return PassEle27;
}

Bool_t event_info::getPassEle32() {
    PassEle32 = passEle32 && filterEle32 && matchEle32;
    return PassEle32;
}

Bool_t event_info::getPassEle35() {
    PassEle35 = passEle35 && filterEle35 && matchEle35;
    return PassEle35;
}

Bool_t event_info::getPassEle24Tau30() {
    PassEle24Tau30 = passEle24Tau30 && filterEle24Tau30 && matchEle24Tau30;
    return PassEle24Tau30;
}

Bool_t event_info::getPassMu19Tau20() {
    return PassIsoMu19Tau20 = isEmbed ? passIsoMu19Tau20
                                      : matchIsoMu19Tau20_1 && filterIsoMu19Tau20_1 && passIsoMu19Tau20 &&
                                            matchIsoMu19Tau20_2 && filterIsoMu19Tau20_2;
}

Bool_t event_info::getPassIsoMu22() {
    PassIsoMu22 = isEmbed ? passIsoMu22 : matchIsoMu22_1 && filterIsoMu22_1 && passIsoMu22;
    return PassIsoMu22;
}

Bool_t event_info::getPassIsoTkMu22() {
    PassIsoTkMu22 = isEmbed ? passIsoTkMu22 : matchIsoTkMu22_1 && filterIsoTkMu22_1 && passIsoTkMu22;
    return PassIsoTkMu22;
}

Bool_t event_info::getPassIsoMu22eta2p1() {
    PassIsoMu22eta2p1 =
        isEmbed ? passIsoMu22eta2p1 : matchIsoMu22eta2p1_1 && filterIsoMu22eta2p1_1 && passIsoMu22eta2p1;
    return PassIsoMu22eta2p1;
}

Bool_t event_info::getPassIsoTkMu22eta2p1() {
    PassIsoTkMu22eta2p1 =
        isEmbed ? passIsoTkMu22eta2p1 : matchIsoTkMu22eta2p1_1 && filterIsoTkMu22eta2p1_1 && passIsoTkMu22eta2p1;
    return PassIsoTkMu22eta2p1;
}

Bool_t event_info::getPassMu20Tau27() {
    // PassMu20Tau27 = mMatchesIsoMu20Tau27Path && mMatchesIsoMu20Tau27Filter && tMatchesIsoMu20Tau27Path &&
    // tMatchesIsoMu20Tau27Filter && Mu20Tau27Pass;
    PassMu20Tau27 = mMatchesIsoMu20Tau27Path && tMatchesIsoMu20Tau27Path && tMatchesIsoMu20Tau27Filter && Mu20Tau27Pass;
    return PassMu20Tau27;
}

Bool_t event_info::getPassMu24() {
    PassMu24 = mMatchesIsoMu24Path && mMatchesIsoMu24Filter && IsoMu24Pass;
    return PassMu24;
}

Bool_t event_info::getPassMu27() {
    PassMu27 = mMatchesIsoMu27Path && mMatchesIsoMu27Filter && IsoMu27Pass;
    return PassMu27;
}

Bool_t event_info::getPassDoubleTauCmbIso35() { return PassDoubleTauCmbIso35; }

Bool_t event_info::getPassDoubleTau35() { return PassDoubleTau35; }

#endif
