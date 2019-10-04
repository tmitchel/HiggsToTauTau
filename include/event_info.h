// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_EVENT_INFO_H_
#define INCLUDE_EVENT_INFO_H_

#include <unordered_map>
#include <string>
#include <vector>
#include "./swiss_army_class.h"

/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
 private:
    ULong64_t evt;
    UInt_t run, lumi, convert_evt;
    Float_t genpX, genpY, genM, genpT, numGenJets, genweight, genDR;  // gen
    Float_t npv, npu, rho, Rivet_nJets30, Rivet_higgsPt, Rivet_stage1_cat_pTjet30GeV;              // event

    // triggers (passing, matching, filtering)
    Float_t PassEle24HPSTau30, PassEle24Tau30, PassEle25, PassEle27, PassEle32, PassEle35, PassIsoMu19Tau20, PassIsoMu19Tau20SingleL1,
        PassIsoMu20HPSTau27, PassIsoMu20Tau27, PassIsoMu22, PassIsoMu22eta2p1, PassIsoMu24, PassIsoMu27, PassIsoTkMu22, PassIsoTkMu22eta2p1;
    Float_t eMatchesEle24HPSTau30Filter, eMatchesEle24Tau30Filter, eMatchesEle25Filter, eMatchesEle27Filter, eMatchesEle32Filter,
        eMatchesEle35Filter;  // electron matches filter
    Float_t mMatchesIsoMu19Tau20Filter, mMatchesIsoMu19Tau20SingleL1Filter, mMatchesIsoMu20HPSTau27Filter, mMatchesIsoMu20Tau27Filter,
        mMatchesIsoMu22Filter, mMatchesIsoMu22eta2p1Filter, mMatchesIsoMu24Filter, mMatchesIsoMu27Filter, mMatchesIsoTkMu22Filter,
        mMatchesIsoTkMu22eta2p1Filter;  // muon matches filter
    Float_t tMatchesEle24HPSTau30Filter, tMatchesEle24Tau30Filter, tMatchesIsoMu19Tau20Filter, tMatchesIsoMu19Tau20SingleL1Filter,
        tMatchesIsoMu20HPSTau27Filter, tMatchesIsoMu20Tau27Filter;  // tau matches filter
    Float_t eMatchesEle24HPSTau30Path, eMatchesEle24Tau30Path, eMatchesEle25Path, eMatchesEle27Path, eMatchesEle32Path,
        eMatchesEle35Path;  // electron matches path
    Float_t mMatchesIsoMu19Tau20Path, mMatchesIsoMu19Tau20SingleL1Path, mMatchesIsoMu20HPSTau27Path, mMatchesIsoMu20Tau27Path, mMatchesIsoMu22Path,
        mMatchesIsoMu22eta2p1Path, mMatchesIsoMu24Path, mMatchesIsoMu27Path, mMatchesIsoTkMu22Path,
        mMatchesIsoTkMu22eta2p1Path;  // muon matches path
    Float_t tMatchesEle24HPSTau30Path, tMatchesEle24Tau30Path, tMatchesIsoMu19Tau20Path, tMatchesIsoMu19Tau20SingleL1Path,
        tMatchesIsoMu20HPSTau27Path, tMatchesIsoMu20Tau27Path;  // tau matches path
    Float_t Ele24LooseHPSTau30Pass, Ele24LooseHPSTau30TightIDPass, Ele24LooseTau30Pass, Ele24LooseTau30TightIDPass, Ele27WPTightPass,
        Ele32WPTightPass, Ele35WPTightPass, Ele38WPTightPass, Ele40WPTightPass, IsoMu24Pass, IsoMu27Pass, Mu20LooseHPSTau27Pass,
        Mu20LooseHPSTau27TightIDPass, Mu20LooseTau27Pass, Mu20LooseTau27TightIDPass, Mu50Pass, singleE25eta2p1TightPass, singleIsoMu22Pass,
        singleIsoMu22eta2p1Pass, singleIsoTkMu22Pass, singleIsoTkMu22eta2p1Pass, singleMu19eta2p1LooseTau20Pass,
        singleMu19eta2p1LooseTau20singleL1Pass;
    Float_t Flag_BadChargedCandidateFilter, Flag_BadPFMuonFilter, Flag_EcalDeadCellTriggerPrimitiveFilter, Flag_HBHENoiseFilter,
        Flag_HBHENoiseIsoFilter, Flag_badMuons, Flag_duplicateMuons, Flag_ecalBadCalibFilter, Flag_eeBadScFilter, Flag_globalSuperTightHalo2016Filter,
        Flag_globalTightHalo2016Filter, Flag_goodVertices;

    Float_t m_sv, pt_sv;                                                                                      // SVFit
    Float_t Dbkg_VBF, Dbkg_ggH, Dbkg_ZH, Dbkg_WH, Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2;  // MELA
    Float_t ME_sm_VBF, ME_sm_ggH, ME_sm_WH, ME_sm_ZH, ME_bkg, ME_bkg1, ME_bkg2, D0_VBF, DCP_VBF, D0_ggH, DCP_ggH, ME_ps_VBF, ME_ps_ggH,
        ME_sm_ggH_qqInit, ME_ps_ggH_qqInit;

    bool isEmbed;
    int era;
    std::unordered_map<std::string, int> unc_map;

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
    Bool_t getPassFlags();

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
    Float_t getJetPtRivet() { return Rivet_stage1_cat_pTjet30GeV; }
    Float_t getRivetUnc(std::vector<double>, std::string);
};

// read data from trees into member variables
event_info::event_info(TTree* input, lepton lep, int _era, std::string syst) :
    isEmbed(false),
    era(_era),
    unc_map{
        {"Rivet0_Up", 0}, {"Rivet0_Down", 0}, {"Rivet1_Up", 1}, {"Rivet1_Down", 1},
        {"Rivet2_Up", 2}, {"Rivet2_Down", 2}, {"Rivet3_Up", 3}, {"Rivet3_Down", 3},
        {"Rivet4_Up", 4}, {"Rivet4_Down", 4}, {"Rivet5_Up", 5}, {"Rivet5_Down", 5},
        {"Rivet6_Up", 6}, {"Rivet6_Down", 6}, {"Rivet7_Up", 7}, {"Rivet7_Down", 7},
        {"Rivet8_Up", 8}, {"Rivet8_Down", 8}
    }
     {
    std::string m_sv_name("m_sv"), pt_sv_name("pt_sv");
    if ((syst.find("DM0") != std::string::npos || syst.find("DM1") != std::string::npos) && syst.find("FES") == std::string::npos) {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
    } else if (syst.find("UncMet") != std::string::npos || syst.find("ClusteredMet") != std::string::npos) {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
    } else if (syst.find("Jet") != std::string::npos) {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
    } else if (syst.find("EES") != std::string::npos || syst.find("MES") != std::string::npos) {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
    }

    input->SetBranchAddress(m_sv_name.c_str(), &m_sv);
    input->SetBranchAddress(pt_sv_name.c_str(), &pt_sv);
    input->SetBranchAddress("Dbkg_VBF", &Dbkg_VBF);
    input->SetBranchAddress("Dbkg_ggH", &Dbkg_ggH);
    input->SetBranchAddress("Dbkg_ZH", &Dbkg_ZH);
    input->SetBranchAddress("Dbkg_WH", &Dbkg_WH);
    input->SetBranchAddress("D_PS_VBF", &D0_VBF);
    input->SetBranchAddress("D_CP_VBF", &DCP_VBF);
    input->SetBranchAddress("D_PS_ggH", &D0_ggH);
    input->SetBranchAddress("D_CP_ggH", &DCP_ggH);
    input->SetBranchAddress("Phi", &Phi);
    input->SetBranchAddress("Phi1", &Phi1);
    input->SetBranchAddress("costheta1", &costheta1);
    input->SetBranchAddress("costheta2", &costheta2);
    input->SetBranchAddress("costhetastar", &costhetastar);
    input->SetBranchAddress("Q2V1", &Q2V1);
    input->SetBranchAddress("Q2V2", &Q2V2);
    input->SetBranchAddress("ME_sm_ggH_qqInit", &ME_sm_ggH_qqInit);
    input->SetBranchAddress("ME_ps_ggH_qqInit", &ME_ps_ggH_qqInit);
    input->SetBranchAddress("ME_sm_VBF", &ME_sm_VBF);
    input->SetBranchAddress("ME_sm_ggH", &ME_sm_ggH);
    input->SetBranchAddress("ME_ps_VBF", &ME_ps_VBF);
    input->SetBranchAddress("ME_ps_ggH", &ME_ps_ggH);
    input->SetBranchAddress("ME_sm_WH", &ME_sm_WH);
    input->SetBranchAddress("ME_sm_ZH", &ME_sm_ZH);
    input->SetBranchAddress("ME_bkg", &ME_bkg);
    input->SetBranchAddress("ME_bkg1", &ME_bkg1);
    input->SetBranchAddress("ME_bkg2", &ME_bkg2);
    input->SetBranchAddress("evt", &evt);
    input->SetBranchAddress("run", &run);
    input->SetBranchAddress("lumi", &lumi);
    input->SetBranchAddress("nvtx", &npv);
    input->SetBranchAddress("nTruePU", &npu);
    input->SetBranchAddress("genpX", &genpX);
    input->SetBranchAddress("genpY", &genpY);
    input->SetBranchAddress("genM", &genM);
    input->SetBranchAddress("genpT", &genpT);
    input->SetBranchAddress("tZTTGenDR", &genDR);
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
        input->SetBranchAddress("tMatchesEle24HPSTau30Filter", &tMatchesEle24HPSTau30Filter);
        input->SetBranchAddress("tMatchesEle24Tau30Filter", &tMatchesEle24Tau30Filter);
        input->SetBranchAddress("tMatchesIsoMu20Tau27Filter", &tMatchesIsoMu20Tau27Filter);
        input->SetBranchAddress("tMatchesEle24HPSTau30Path", &tMatchesEle24HPSTau30Path);
        input->SetBranchAddress("tMatchesEle24Tau30Path", &tMatchesEle24Tau30Path);
        input->SetBranchAddress("singleE25eta2p1TightPass", &singleE25eta2p1TightPass);
        input->SetBranchAddress("Ele24LooseHPSTau30Pass", &Ele24LooseHPSTau30Pass);
        input->SetBranchAddress("Ele24LooseHPSTau30TightIDPass", &Ele24LooseHPSTau30TightIDPass);
        input->SetBranchAddress("Ele24LooseTau30Pass", &Ele24LooseTau30Pass);
        input->SetBranchAddress("Ele24LooseTau30TightIDPass", &Ele24LooseTau30TightIDPass);
        input->SetBranchAddress("Ele27WPTightPass", &Ele27WPTightPass);
        input->SetBranchAddress("Ele32WPTightPass", &Ele32WPTightPass);
        input->SetBranchAddress("Ele35WPTightPass", &Ele35WPTightPass);
        input->SetBranchAddress("Ele38WPTightPass", &Ele38WPTightPass);
        input->SetBranchAddress("Ele40WPTightPass", &Ele40WPTightPass);
        input->SetBranchAddress("eMatchesEle24HPSTau30Filter", &eMatchesEle24HPSTau30Filter);
        input->SetBranchAddress("eMatchesEle24Tau30Filter", &eMatchesEle24Tau30Filter);
        input->SetBranchAddress("eMatchesEle25Filter", &eMatchesEle25Filter);
        input->SetBranchAddress("eMatchesEle27Filter", &eMatchesEle27Filter);
        input->SetBranchAddress("eMatchesEle32Filter", &eMatchesEle32Filter);
        input->SetBranchAddress("eMatchesEle35Filter", &eMatchesEle35Filter);
        input->SetBranchAddress("eMatchesEle24HPSTau30Path", &eMatchesEle24HPSTau30Path);
        input->SetBranchAddress("eMatchesEle24Tau30Path", &eMatchesEle24Tau30Path);
        input->SetBranchAddress("eMatchesEle25Path", &eMatchesEle25Path);
        input->SetBranchAddress("eMatchesEle27Path", &eMatchesEle27Path);
        input->SetBranchAddress("eMatchesEle32Path", &eMatchesEle32Path);
        input->SetBranchAddress("eMatchesEle35Path", &eMatchesEle35Path);
    } else if (lep == lepton::MUON) {
        input->SetBranchAddress("tMatchesIsoMu19Tau20Filter", &tMatchesIsoMu19Tau20Filter);
        input->SetBranchAddress("tMatchesIsoMu19Tau20SingleL1Filter", &tMatchesIsoMu19Tau20SingleL1Filter);
        input->SetBranchAddress("tMatchesIsoMu20HPSTau27Filter", &tMatchesIsoMu20HPSTau27Filter);
        input->SetBranchAddress("tMatchesIsoMu19Tau20Path", &tMatchesIsoMu19Tau20Path);
        input->SetBranchAddress("tMatchesIsoMu19Tau20SingleL1Path", &tMatchesIsoMu19Tau20SingleL1Path);
        input->SetBranchAddress("tMatchesIsoMu20HPSTau27Path", &tMatchesIsoMu20HPSTau27Path);
        input->SetBranchAddress("tMatchesIsoMu20Tau27Path", &tMatchesIsoMu20Tau27Path);
        input->SetBranchAddress("IsoMu24Pass", &IsoMu24Pass);
        input->SetBranchAddress("IsoMu27Pass", &IsoMu27Pass);
        input->SetBranchAddress("Mu20LooseHPSTau27Pass", &Mu20LooseHPSTau27Pass);
        input->SetBranchAddress("Mu20LooseHPSTau27TightIDPass", &Mu20LooseHPSTau27TightIDPass);
        input->SetBranchAddress("Mu20LooseTau27Pass", &Mu20LooseTau27Pass);
        input->SetBranchAddress("Mu20LooseTau27TightIDPass", &Mu20LooseTau27TightIDPass);
        input->SetBranchAddress("Mu50Pass", &Mu50Pass);
        input->SetBranchAddress("singleIsoMu22Pass", &singleIsoMu22Pass);
        input->SetBranchAddress("singleIsoMu22eta2p1Pass", &singleIsoMu22eta2p1Pass);
        input->SetBranchAddress("singleIsoTkMu22Pass", &singleIsoTkMu22Pass);
        input->SetBranchAddress("singleIsoTkMu22eta2p1Pass", &singleIsoTkMu22eta2p1Pass);
        input->SetBranchAddress("singleMu19eta2p1LooseTau20Pass", &singleMu19eta2p1LooseTau20Pass);
        input->SetBranchAddress("singleMu19eta2p1LooseTau20singleL1Pass", &singleMu19eta2p1LooseTau20singleL1Pass);
        input->SetBranchAddress("mMatchesIsoMu19Tau20Filter", &mMatchesIsoMu19Tau20Filter);
        input->SetBranchAddress("mMatchesIsoMu19Tau20SingleL1Filter", &mMatchesIsoMu19Tau20SingleL1Filter);
        input->SetBranchAddress("mMatchesIsoMu20HPSTau27Filter", &mMatchesIsoMu20HPSTau27Filter);
        input->SetBranchAddress("mMatchesIsoMu20Tau27Filter", &mMatchesIsoMu20Tau27Filter);
        input->SetBranchAddress("mMatchesIsoMu22Filter", &mMatchesIsoMu22Filter);
        input->SetBranchAddress("mMatchesIsoMu22eta2p1Filter", &mMatchesIsoMu22eta2p1Filter);
        input->SetBranchAddress("mMatchesIsoMu24Filter", &mMatchesIsoMu24Filter);
        input->SetBranchAddress("mMatchesIsoMu27Filter", &mMatchesIsoMu27Filter);
        input->SetBranchAddress("mMatchesIsoTkMu22Filter", &mMatchesIsoTkMu22Filter);
        input->SetBranchAddress("mMatchesIsoTkMu22eta2p1Filter", &mMatchesIsoTkMu22eta2p1Filter);
        input->SetBranchAddress("mMatchesIsoMu19Tau20Path", &mMatchesIsoMu19Tau20Path);
        input->SetBranchAddress("mMatchesIsoMu19Tau20SingleL1Path", &mMatchesIsoMu19Tau20SingleL1Path);
        input->SetBranchAddress("mMatchesIsoMu20HPSTau27Path", &mMatchesIsoMu20HPSTau27Path);
        input->SetBranchAddress("mMatchesIsoMu20Tau27Path", &mMatchesIsoMu20Tau27Path);
        input->SetBranchAddress("mMatchesIsoMu22Path", &mMatchesIsoMu22Path);
        input->SetBranchAddress("mMatchesIsoMu22eta2p1Path", &mMatchesIsoMu22eta2p1Path);
        input->SetBranchAddress("mMatchesIsoMu24Path", &mMatchesIsoMu24Path);
        input->SetBranchAddress("mMatchesIsoMu27Path", &mMatchesIsoMu27Path);
        input->SetBranchAddress("mMatchesIsoTkMu22Path", &mMatchesIsoTkMu22Path);
        input->SetBranchAddress("mMatchesIsoTkMu22eta2p1Path", &mMatchesIsoTkMu22eta2p1Path);
    } else if (lep == lepton::EMU) {
        // no analyzer yet
    } else {
        std::cerr << "HEY! THAT'S NOT AN ANALYZER. WAT U DOIN." << std::endl;
    }
}

void event_info::setRivets(TTree* input) {
    input->SetBranchAddress("Rivet_nJets30", &Rivet_nJets30);
    input->SetBranchAddress("Rivet_higgsPt", &Rivet_higgsPt);
    input->SetBranchAddress("Rivet_stage1_cat_pTjet30GeV", &Rivet_stage1_cat_pTjet30GeV);
}

Float_t event_info::getRivetUnc(std::vector<double> uncs, std::string syst) {
    if (syst.find("Rivet") != std::string::npos) {
        int index = unc_map[syst];
        if (syst.find("Up") != std::string::npos) {
            return uncs.at(index);
        } else {
            return -1 * uncs.at(index);
        }
    }
}

Bool_t event_info::getPassFlags() {
    if (era == 2016) {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseIsoFilter
                || Flag_HBHENoiseFilter || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter);

    } else if (era == 2017) {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter
                || Flag_HBHENoiseIsoFilter || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter
                || Flag_BadChargedCandidateFilter || Flag_eeBadScFilter || Flag_ecalBadCalibFilter);
    } else {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter
                || Flag_HBHENoiseIsoFilter || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter
                || Flag_eeBadScFilter || Flag_ecalBadCalibFilter);
    }
}


Bool_t event_info::getPassEle25() {
    PassEle25 = singleE25eta2p1TightPass && eMatchesEle25Filter && eMatchesEle25Path;
    return PassEle25;
}

Bool_t event_info::getPassEle27() {
    PassEle27 = Ele27WPTightPass && eMatchesEle27Filter && eMatchesEle27Path;
    return PassEle27;
}

Bool_t event_info::getPassEle32() {
    PassEle32 = Ele32WPTightPass && eMatchesEle32Filter && eMatchesEle32Path;
    return PassEle32;
}

Bool_t event_info::getPassEle35() {
    PassEle35 = Ele35WPTightPass && eMatchesEle35Filter && eMatchesEle35Path;
    return PassEle35;
}

Bool_t event_info::getPassEle24Tau30() {
    PassEle24Tau30 = Ele24LooseTau30Pass && eMatchesEle24Tau30Filter && eMatchesEle24Tau30Path && tMatchesEle24Tau30Filter && tMatchesEle24Tau30Path;
    return PassEle24Tau30;
}

Bool_t event_info::getPassMu19Tau20() {
    return PassIsoMu19Tau20 =
               singleMu19eta2p1LooseTau20Pass && mMatchesIsoMu19Tau20Filter && mMatchesIsoMu19Tau20Path
               && tMatchesIsoMu19Tau20Filter && tMatchesIsoMu19Tau20Path;
}

Bool_t event_info::getPassIsoMu22() {
    PassIsoMu22 = singleIsoMu22Pass && mMatchesIsoMu22Filter && mMatchesIsoMu22Path;
    return PassIsoMu22;
}

Bool_t event_info::getPassIsoTkMu22() {
    PassIsoTkMu22 = singleIsoTkMu22Pass && mMatchesIsoTkMu22Filter && mMatchesIsoTkMu22Path;
    return PassIsoTkMu22;
}

Bool_t event_info::getPassIsoMu22eta2p1() {
    PassIsoMu22eta2p1 = singleIsoMu22eta2p1Pass && mMatchesIsoMu22eta2p1Filter && mMatchesIsoMu22eta2p1Path;
    return PassIsoMu22eta2p1;
}

Bool_t event_info::getPassIsoTkMu22eta2p1() {
    PassIsoTkMu22eta2p1 = singleIsoTkMu22eta2p1Pass && mMatchesIsoTkMu22eta2p1Filter && mMatchesIsoTkMu22eta2p1Path;
    return PassIsoTkMu22eta2p1;
}

Bool_t event_info::getPassMu20Tau27() {
    PassIsoMu20Tau27 =
            singleMu19eta2p1LooseTau20Pass && mMatchesIsoMu19Tau20Filter && mMatchesIsoMu19Tau20Path
            && tMatchesIsoMu19Tau20Filter && tMatchesIsoMu19Tau20Path;
    return PassIsoMu20Tau27;
}

Bool_t event_info::getPassMu24() {
    PassIsoMu24 = IsoMu24Pass && mMatchesIsoMu24Filter && mMatchesIsoMu24Path;
    return PassIsoMu24;
}

Bool_t event_info::getPassMu27() {
    PassIsoMu27 = IsoMu27Pass && mMatchesIsoMu27Filter && mMatchesIsoMu27Path;
    return PassIsoMu27;
}

#endif  // INCLUDE_EVENT_INFO_H_
