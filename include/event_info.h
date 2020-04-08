// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_EVENT_INFO_H_
#define INCLUDE_EVENT_INFO_H_

#include <unordered_map>
#include <string>
#include <vector>
#include "./swiss_army_class.h"
#include "./qq2Hqq_uncert_scheme.h"

/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_info {
 private:
    ULong64_t evt;
    UInt_t run, lumi, convert_evt;
    Float_t genpX, genpY, genM, genpT, numGenJets, genweight, genDR;  // gen
    Float_t npv, npu, rho, Rivet_nJets30, Rivet_higgsPt, Rivet_stage1_cat_pTjet30GeV;  // event
    Float_t prefiring_weight, prefiring_weight_up, prefiring_weight_down;  // prefiring weights
    Float_t muVetoZTTp001dxyzR0, eVetoZTTp001dxyzR0, dimuonVeto, dielectronVeto;  // lepton vetos
    Float_t sm_weight_nlo, mm_weight_nlo, ps_weight_nlo;  // madgraph reweighting

    // triggers (passing, matching, filtering)
    Float_t Flag_BadChargedCandidateFilter, Flag_BadPFMuonFilter, Flag_EcalDeadCellTriggerPrimitiveFilter, Flag_HBHENoiseFilter,
        Flag_HBHENoiseIsoFilter, Flag_badMuons, Flag_duplicateMuons, Flag_ecalBadCalibFilter, Flag_eeBadScFilter, Flag_globalSuperTightHalo2016Filter,
        Flag_globalTightHalo2016Filter, Flag_goodVertices;
    Float_t eMatchEmbeddedFilterEle24Tau30, tMatchEmbeddedFilterEle24Tau30;
    Float_t mMatchEmbeddedFilterMu20Tau27_2018, mMatchEmbeddedFilterMu20Tau27_2017, tMatchEmbeddedFilterMu20HPSTau27;
    Float_t Ele24LooseTau30Pass, eMatchesEle24Tau30Filter, eMatchesEle24Tau30Path, tMatchesEle24Tau30Filter, tMatchesEle24Tau30Path;
    Float_t eMatchesEle24HPSTau30Filter, eMatchesEle24HPSTau30Path, tMatchesEle24HPSTau30Filter, tMatchesEle24HPSTau30Path;
    Float_t Ele24LooseHPSTau30Pass, Ele24LooseHPSTau30TightIDPass;
    Bool_t PassEle24Tau30_2018;

    Float_t m_sv, pt_sv, m_sv_shift, m_sv_noshift;                                                                                      // SVFit
    Float_t Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2;  // MELA
    Float_t DCP_VBF, DCP_ggH;
    Float_t ME_sm_VBF, ME_sm_ggH, ME_sm_ggH_qqInit, ME_sm_WH, ME_sm_ZH, ME_ps_VBF, ME_ps_ggH, ME_ps_ggH_qqInit,
        ME_a2_VBF, ME_L1_VBF, ME_L1Zg_VBF, ME_bkg, ME_bkg1, ME_bkg2;
    Float_t njets;
    std::string syst;
    bool shifting, valid_shift, always_shift;

    bool isEmbed;
    int era;
    lepton lep;
    std::unordered_map<std::string, int> unc_map;
    std::unordered_map<std::string, std::string> syst_name_map;

 public:
    event_info(TTree*, lepton, int, bool, std::string);
    virtual ~event_info() {}
    void setEmbed() { isEmbed = true; }
    void setNjets(Float_t _njets) { njets = _njets; }  // must be set in event loop
    void setRivets(TTree*);
    std::string fix_syst_string(std::string);
    void do_shift(bool _shift) { valid_shift = (_shift || always_shift); }
    Float_t getMSV() { return shifting && valid_shift ? m_sv_shift : m_sv_noshift; }
    Float_t getPtSV() { return pt_sv; }

    // tautau Trigger Info
    Bool_t getPassElEmbedCross();
    Bool_t getPassMuEmbedCross2017();
    Bool_t getPassMuEmbedCross2018();
    Bool_t getPassEle25();
    Bool_t getPassEle27();
    Bool_t getPassEle32();
    Bool_t getPassEle35();
    Bool_t getPassEle24Tau30();
    Bool_t getPassEle24Tau30_2018(Bool_t);
    Bool_t getPassMu19Tau20();
    Bool_t getPassIsoMu22();
    Bool_t getPassIsoTkMu22();
    Bool_t getPassIsoMu22eta2p1();
    Bool_t getPassIsoTkMu22eta2p1();
    Bool_t getPassMu20Tau27();
    Bool_t getPassMu24();
    Bool_t getPassMu27();
    Bool_t getPassFlags(Bool_t);
    Bool_t getPassCrossTrigger(Float_t);

    // Lepton vetos
    Bool_t hasExtraMuon(lepton lep) { return lep == lepton::MUON ? muVetoZTTp001dxyzR0 > 1 : muVetoZTTp001dxyzR0 > 0; }
    Bool_t hasExtraElectron(lepton lep) { return lep == lepton::ELECTRON ? eVetoZTTp001dxyzR0 > 1 : eVetoZTTp001dxyzR0 > 0; }
    Bool_t hasDimuon() { return dimuonVeto > 0; }
    Bool_t hasDielectron() { return dielectronVeto > 0; }

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

    // MELA Info
    Float_t getDCP_VBF() { return DCP_VBF; }
    Float_t getDCP_ggH() { return DCP_ggH; }
    Float_t getPhi() { return Phi; }
    Float_t getPhi1() { return Phi1; }
    Float_t getCosTheta1() { return costheta1; }
    Float_t getCosTheta2() { return costheta2; }
    Float_t getCosThetaStar() { return costhetastar; }
    Float_t getQ2V1() { return Q2V1; }
    Float_t getQ2V2() { return Q2V2; }
    Float_t getME_sm_VBF() { return ME_sm_VBF; }
    Float_t getME_sm_ggH() { return ME_sm_ggH; }
    Float_t getME_sm_ggH_qqInit() { return ME_sm_ggH_qqInit; }
    Float_t getME_sm_WH() { return ME_sm_WH; }
    Float_t getME_sm_ZH() { return ME_sm_ZH; }
    Float_t getME_ps_VBF() { return ME_ps_VBF; }
    Float_t getME_ps_ggH() { return ME_ps_ggH; }
    Float_t getME_ps_ggH_qqInit() { return ME_ps_ggH_qqInit; }
    Float_t getME_a2_VBF() { return ME_a2_VBF; }
    Float_t getME_L1_VBF() { return ME_L1_VBF; }
    Float_t getME_L1Zg_VBF() { return ME_L1Zg_VBF; }
    Float_t getME_bkg() { return ME_bkg; }
    Float_t getME_bkg1() { return ME_bkg1; }
    Float_t getME_bkg2() { return ME_bkg2; }

    // ggH NNLOPS Info
    Float_t getNjetsRivet() { return Rivet_nJets30; }
    Float_t getHiggsPtRivet() { return Rivet_higgsPt; }
    Float_t getJetPtRivet() { return Rivet_stage1_cat_pTjet30GeV; }
    Float_t getRivetUnc(std::vector<double>, std::string);
    Float_t getVBFTheoryUnc(std::string);

    // Prefiring Weight
    Float_t getPrefiringWeight();

    // Madgraph Reweighting
    Float_t getMadgraphSM() { return sm_weight_nlo; }
    Float_t getMadgraphMM() { return mm_weight_nlo; }
    Float_t getMadgraphPS() { return ps_weight_nlo; }
};

// read data from trees into member variables
event_info::event_info(TTree* input, lepton _lep, int _era, bool isMadgraph, std::string _syst) :
    sm_weight_nlo(1.),
    mm_weight_nlo(1.),
    ps_weight_nlo(1.),
    isEmbed(false),
    era(_era),
    syst(_syst),
    lep(_lep),
    shifting(false),
    valid_shift(false),
    always_shift(false),
    unc_map{
        {"Rivet0_Up", 0}, {"Rivet0_Down", 0}, {"Rivet1_Up", 1}, {"Rivet1_Down", 1},
        {"Rivet2_Up", 2}, {"Rivet2_Down", 2}, {"Rivet3_Up", 3}, {"Rivet3_Down", 3},
        {"Rivet4_Up", 4}, {"Rivet4_Down", 4}, {"Rivet5_Up", 5}, {"Rivet5_Down", 5},
        {"Rivet6_Up", 6}, {"Rivet6_Down", 6}, {"Rivet7_Up", 7}, {"Rivet7_Down", 7},
        {"Rivet8_Up", 8}, {"Rivet8_Down", 8}
    }
     {
    auto end = std::string::npos;
    std::string m_sv_name("m_sv"), pt_sv_name("pt_sv");
    if (syst.find("efaket_es") != end || syst.find("mfaket_et") != end) {  // lepton faking tau ES
        m_sv_name += "_" + fix_syst_string(syst);
        pt_sv_name += "_" + fix_syst_string(syst);
        shifting = true;  // shift will depend on eta cuts
        if (syst.find("mfaket_et") != end) {
            always_shift = true;  // shift is always valid
        }
    } else if ((syst.find("DM0") != end || syst.find("DM1") != end)) {  // genuine tau ES
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        shifting = true;
        always_shift = true;  // shifts will depend on pT in the future
    } else if (syst.find("Jet") != end) {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        shifting = true;
        always_shift = true;  // shift is always valid
    } else if (syst.find("EES") != end) {  // lepton ES
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        shifting = true;
        always_shift = true;  // shift is always valid
    } else if (syst.find("MES") != end) {
        m_sv_name += "_" + fix_syst_string(syst);
        pt_sv_name += "_" + fix_syst_string(syst);
        shifting = true;
    } else if (syst.find("RecoilReso") != end || syst.find("RecoilResp") != end) {
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        shifting = true;
    }

    input->SetBranchAddress("m_sv", &m_sv_noshift);
    input->SetBranchAddress(pt_sv_name.c_str(), &pt_sv);
    if (shifting) {
        input->SetBranchAddress(m_sv_name.c_str(), &m_sv_shift);
    }
    input->SetBranchAddress("D_CP_VBF", &DCP_VBF);
    input->SetBranchAddress("D_CP_ggH", &DCP_ggH);
    input->SetBranchAddress("Phi", &Phi);
    input->SetBranchAddress("Phi1", &Phi1);
    input->SetBranchAddress("costheta1", &costheta1);
    input->SetBranchAddress("costheta2", &costheta2);
    input->SetBranchAddress("costhetastar", &costhetastar);
    input->SetBranchAddress("Q2V1", &Q2V1);
    input->SetBranchAddress("Q2V2", &Q2V2);
    input->SetBranchAddress("ME_sm_VBF", &ME_sm_VBF);
    input->SetBranchAddress("ME_sm_ggH", &ME_sm_ggH);
    input->SetBranchAddress("ME_sm_ggH_qqInit", &ME_sm_ggH_qqInit);
    input->SetBranchAddress("ME_sm_WH", &ME_sm_WH);
    input->SetBranchAddress("ME_sm_ZH", &ME_sm_ZH);
    input->SetBranchAddress("ME_ps_VBF", &ME_ps_VBF);
    input->SetBranchAddress("ME_ps_ggH", &ME_ps_ggH);
    input->SetBranchAddress("ME_ps_ggH_qqInit", &ME_ps_ggH_qqInit);
    input->SetBranchAddress("ME_a2_VBF", &ME_a2_VBF);
    input->SetBranchAddress("ME_L1_VBF", &ME_L1_VBF);
    input->SetBranchAddress("ME_L1Zg_VBF", &ME_L1Zg_VBF);
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
    input->SetBranchAddress("prefiring_weight", &prefiring_weight);
    input->SetBranchAddress("prefiring_weight_up", &prefiring_weight_up);
    input->SetBranchAddress("prefiring_weight_down", &prefiring_weight_down);
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
    input->SetBranchAddress("muVetoZTTp001dxyzR0", &muVetoZTTp001dxyzR0);
    input->SetBranchAddress("eVetoZTTp001dxyzR0", &eVetoZTTp001dxyzR0);
    input->SetBranchAddress("dimuonVeto", &dimuonVeto);
    input->SetBranchAddress("dielectronVeto", &dielectronVeto);

    if (isMadgraph) {
        input->SetBranchAddress("sm_weight_nlo", &sm_weight_nlo);
        input->SetBranchAddress("mm_weight_nlo", &mm_weight_nlo);
        input->SetBranchAddress("ps_weight_nlo", &ps_weight_nlo);
    }

    if (lep == lepton::ELECTRON) {
        if (isEmbed) {
          input->SetBranchAddress("eMatchEmbeddedFilterEle24Tau30", &eMatchEmbeddedFilterEle24Tau30);
          input->SetBranchAddress("tMatchEmbeddedFilterEle24Tau30", &tMatchEmbeddedFilterEle24Tau30);
        }
    } else if (lep == lepton::MUON) {
        if (isEmbed) {
          input->SetBranchAddress("mMatchEmbeddedFilterMu20Tau27_2017", &mMatchEmbeddedFilterMu20Tau27_2017);
          input->SetBranchAddress("mMatchEmbeddedFilterMu20Tau27_2018", &mMatchEmbeddedFilterMu20Tau27_2018);
          input->SetBranchAddress("tMatchEmbeddedFilterMu20HPSTau27", &tMatchEmbeddedFilterMu20HPSTau27);
        }
    } else if (lep == lepton::EMU) {
        // no analyzer yet
    } else {
        std::cerr << "HEY! THAT'S NOT AN ANALYZER. WAT U DOIN." << std::endl;
    }
}

std::string event_info::fix_syst_string(std::string syst) {
    auto end = std::string::npos;
    if (syst.find("DM0_Up") != end) {
        return "LES_DM0_Up";
    } else if (syst.find("DM0_Down") != end) {
        return "LES_DM0_Down";
    } else if (syst.find("DM1_Up") != end) {
        return "LES_DM1_Up";
    } else if (syst.find("DM1_Down") != end) {
        return "LES_DM1_Down";
    } else if (syst.find("MES") != end) {
        return syst.find("Up") != end ? "MES_Up" : "MES_Down";
    }
    return syst;
}

Float_t event_info::getPrefiringWeight() {
    if (syst == "prefiring_up") {
        return prefiring_weight_up;
    } else if (syst == "prefiring_down") {
        return prefiring_weight_down;
    }
    return prefiring_weight;
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

Float_t event_info::getVBFTheoryUnc(std::string syst) {
    if (syst.find("VBF_Rivet") == std::string::npos) {
        return 1.;
    }

    double shift(1.0);
    if (syst.find("_Down") != std::string::npos) {
        shift *= -1.;
    }

    int source;
    if (syst.find("VBF_Rivet0") != std::string::npos) {
        source = 0;
    } else if (syst.find("VBF_Rivet1") != std::string::npos) {
        source = 1;
    } else if (syst.find("VBF_Rivet2") != std::string::npos) {
        source = 2;
    } else if (syst.find("VBF_Rivet3") != std::string::npos) {
        source = 3;
    } else if (syst.find("VBF_Rivet4") != std::string::npos) {
        source = 4;
    } else if (syst.find("VBF_Rivet5") != std::string::npos) {
        source = 5;
    } else if (syst.find("VBF_Rivet6") != std::string::npos) {
        source = 6;
    } else if (syst.find("VBF_Rivet7") != std::string::npos) {
        source = 7;
    } else if (syst.find("VBF_Rivet8") != std::string::npos) {
        source = 8;
    } else if (syst.find("VBF_Rivet9") != std::string::npos) {
        source = 9;
    } else {
        return 1.;
    }

    return vbf_uncert_stage_1_1(source, static_cast<int>(Rivet_stage1_cat_pTjet30GeV), shift);
}

Bool_t event_info::getPassFlags(Bool_t isData) {
    if (era == 2016) {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter
                || Flag_HBHENoiseIsoFilter || (Flag_eeBadScFilter && isData) || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter);
    } else if (era == 2017) {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter
                || Flag_HBHENoiseIsoFilter || (Flag_eeBadScFilter && isData) || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter
                || Flag_ecalBadCalibFilter);
    } else {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter
                || Flag_HBHENoiseIsoFilter || (Flag_eeBadScFilter && isData) || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter
                || Flag_ecalBadCalibFilter);
    }
}

Bool_t event_info::getPassEle24Tau30() {
    return Ele24LooseTau30Pass && eMatchesEle24Tau30Filter && eMatchesEle24Tau30Path && tMatchesEle24Tau30Filter && tMatchesEle24Tau30Path;
}

Bool_t event_info::getPassEle24Tau30_2018(Bool_t isData) {
    PassEle24Tau30_2018 = eMatchesEle24HPSTau30Filter && eMatchesEle24HPSTau30Path && tMatchesEle24HPSTau30Filter && tMatchesEle24HPSTau30Path;
    if (isData && run < 317509) {
      return eMatchesEle24Tau30Filter && eMatchesEle24Tau30Path && tMatchesEle24Tau30Filter && tMatchesEle24Tau30Path && Ele24LooseTau30Pass; 
    } else if ((isData && run >= 317509) || !isData) {
      return eMatchesEle24HPSTau30Filter && eMatchesEle24HPSTau30Path && tMatchesEle24HPSTau30Filter && tMatchesEle24HPSTau30Path && Ele24LooseHPSTau30Pass; 
    }
    return false;
}


Bool_t event_info::getPassElEmbedCross() {
  return eMatchEmbeddedFilterEle24Tau30 && tMatchEmbeddedFilterEle24Tau30;
}

Bool_t event_info::getPassMuEmbedCross2017() {
  return mMatchEmbeddedFilterMu20Tau27_2017 && tMatchEmbeddedFilterMu20HPSTau27;
}

Bool_t event_info::getPassMuEmbedCross2018() {
  return mMatchEmbeddedFilterMu20Tau27_2018 && tMatchEmbeddedFilterMu20HPSTau27;
}

Bool_t event_info::getPassCrossTrigger(Float_t pt) {
    if (lep == lepton::ELECTRON) {
        if (era == 2016) {
            return false;
        } else if (era == 2017) {
            return pt < 28;
        } else if (era == 2018) {
            return pt < 33;
        }
    } else if (lep == lepton::MUON) {
        if (era == 2016) {
            return pt < 23;
        } else if (era == 2017) {
            return pt < 25;
        } else if (era == 2018) {
            return pt < 25;
        }
    } else {
        std::cerr << "Event wasn't ELECTRON or MUON" << std::endl;
    }
}

#endif  // INCLUDE_EVENT_INFO_H_
