// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_FSA_EVENT_FACTORY_H_
#define INCLUDE_FSA_EVENT_FACTORY_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/defaults.h"
#include "../qq2Hqq_uncert_scheme.h"
#include "../swiss_army_class.h"
#include "TTree.h"

/////////////////////////////////////////
// Purpose: To hold general event data //
/////////////////////////////////////////
class event_factory {
   private:
    ULong64_t evt;
    UInt_t run, lumi, convert_evt;
    Float_t genM, genpT, numGenJets, genweight;                   // gen
    Float_t npu, Rivet_nJets30, Rivet_higgsPt, Rivet_stage1_cat_pTjet30GeV;  // event
    Float_t prefiring_weight, prefiring_weight_up, prefiring_weight_down;              // prefiring weights
    Float_t sm_weight_nlo, mm_weight_nlo, ps_weight_nlo;                               // madgraph reweighting

    // triggers (passing, matching, filtering)
    Float_t Flag_BadChargedCandidateFilter, Flag_BadPFMuonFilter, Flag_EcalDeadCellTriggerPrimitiveFilter, Flag_HBHENoiseFilter,
        Flag_HBHENoiseIsoFilter, Flag_badMuons, Flag_duplicateMuons, Flag_ecalBadCalibFilter, Flag_eeBadScFilter, Flag_globalSuperTightHalo2016Filter,
        Flag_globalTightHalo2016Filter, Flag_goodVertices;
    Float_t Ele24LooseTau30Pass, eMatchesEle24Tau30Filter, eMatchesEle24Tau30Path, tMatchesEle24Tau30Filter, tMatchesEle24Tau30Path,
        Ele24LooseHPSTau30Pass, PassEle24Tau30_2018, eMatchesEle24HPSTau30Filter, eMatchesEle24HPSTau30Path, tMatchesEle24HPSTau30Filter,
        tMatchesEle24HPSTau30Path;
    Float_t m_sv, pt_sv, m_sv_shift, m_sv_noshift;                      // SVFit
    Float_t Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2;  // MELA
    Float_t DCP_VBF, DCP_ggH;
    Float_t ME_sm_VBF, ME_sm_ggH, ME_sm_ggH_qqInit, ME_sm_WH, ME_sm_ZH, ME_ps_VBF, ME_ps_ggH, ME_ps_ggH_qqInit, ME_a2_VBF, ME_L1_VBF, ME_L1Zg_VBF,
        ME_bkg, ME_bkg1, ME_bkg2;
    Float_t njets;
    std::string syst;
    bool shifting, valid_shift, always_shift;

    bool isEmbed, isData;
    int era;
    lepton lep;
    std::unordered_map<std::string, int> unc_map;
    std::unordered_map<std::string, std::string> syst_name_map;

    Bool_t getPassEle24Tau30();
    Bool_t getPassEle24Tau30_2018();

   public:
    event_factory(TTree*, Bool_t, lepton, int, bool, std::string);
    virtual ~event_factory() {}
    void setEmbed() { isEmbed = true; }
    void setNjets(Float_t _njets) { njets = _njets; }  // must be set in event loop
    void setRivets(TTree*);
    std::string fix_syst_string(std::string);
    void do_shift(bool _shift) { valid_shift = (_shift || always_shift); }

    // general interface
    ULong64_t getEvt() { return evt; }
    UInt_t getRun() { return run; }
    UInt_t getLumi() { return lumi; }
    Float_t getGenWeight() { return genweight; }
    Float_t getMSV() { return shifting && valid_shift ? m_sv_shift : m_sv_noshift; }
    Float_t getPtSV() { return pt_sv; }
    Bool_t fire_trigger(trigger t);
    Bool_t getPassFlags(Bool_t);
    Float_t getPrefiringWeight();
    Float_t getNPU() { return npu; }

    // FSA - AC specific
    Bool_t getPassCrossTrigger(Float_t);

    // Generator Info
    Float_t getGenM() { return genM; }
    Float_t getGenPt() { return genpT; }
    Float_t getNumGenJets() { return numGenJets; }

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
    Float_t getME_ps_VBF() { return ME_ps_VBF; }
    Float_t getME_ps_ggH() { return ME_ps_ggH; }
    Float_t getME_ps_ggH_qqInit() { return ME_ps_ggH_qqInit; }
    Float_t getME_a2_VBF() { return ME_a2_VBF; }
    Float_t getME_L1_VBF() { return ME_L1_VBF; }
    Float_t getME_L1Zg_VBF() { return ME_L1Zg_VBF; }

    // ggH NNLOPS Info
    Float_t getNjetsRivet() { return Rivet_nJets30; }
    Float_t getHiggsPtRivet() { return Rivet_higgsPt; }
    Float_t getJetPtRivet() { return Rivet_stage1_cat_pTjet30GeV; }
    Float_t getRivetUnc(std::vector<double>, std::string);
    Float_t getVBFTheoryUnc(std::string);

    // Madgraph Reweighting
    Float_t getMadgraphSM() { return sm_weight_nlo; }
    Float_t getMadgraphMM() { return mm_weight_nlo; }
    Float_t getMadgraphPS() { return ps_weight_nlo; }
};

// read data from trees into member variables
event_factory::event_factory(TTree* input, Bool_t _is_data, lepton _lep, int _era, bool isMadgraph, std::string _syst)
    : sm_weight_nlo(1.),
      mm_weight_nlo(1.),
      ps_weight_nlo(1.),
      isEmbed(false),
      era(_era),
      syst(_syst),
      lep(_lep),
      shifting(false),
      valid_shift(false),
      always_shift(false),
      isData(_is_data),
      unc_map{{"Rivet0_Up", 0}, {"Rivet0_Down", 0}, {"Rivet1_Up", 1}, {"Rivet1_Down", 1}, {"Rivet2_Up", 2}, {"Rivet2_Down", 2},
              {"Rivet3_Up", 3}, {"Rivet3_Down", 3}, {"Rivet4_Up", 4}, {"Rivet4_Down", 4}, {"Rivet5_Up", 5}, {"Rivet5_Down", 5},
              {"Rivet6_Up", 6}, {"Rivet6_Down", 6}, {"Rivet7_Up", 7}, {"Rivet7_Down", 7}, {"Rivet8_Up", 8}, {"Rivet8_Down", 8}} {
    auto end = std::string::npos;
    std::string m_sv_name("m_sv"), pt_sv_name("pt_sv");
    if (syst.find("efaket_es") != end || syst.find("mfaket_et") != end) {  // lepton faking tau ES
        m_sv_name += "_" + fix_syst_string(syst);
        pt_sv_name += "_" + fix_syst_string(syst);
        if (syst.find("mfaket_et") != end) {
            always_shift = true;  // shift is always valid for mutau
        }
    } else if ((syst.find("DM0") != end || syst.find("DM1") != end)) {  // genuine tau ES
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        always_shift = true;
    } else if (syst.find("Jet") != end) {  // JEC and JER
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        always_shift = true;               // shift is always valid
    } else if (syst.find("EES") != end) {  // electron ES
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
        always_shift = true;  // shift is always valid
    } else if (syst.find("MES") != end) {  // muon ES
        m_sv_name += "_" + fix_syst_string(syst);
        pt_sv_name += "_" + fix_syst_string(syst);
    } else if (syst.find("RecoilReso") != end || syst.find("RecoilResp") != end) {  // recoil corrections
        m_sv_name += "_" + syst;
        pt_sv_name += "_" + syst;
    }

    // is SVFit mass different than nominal?
    shifting = (m_sv_name == "m_sv");

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
    input->SetBranchAddress("genM", &genM);
    input->SetBranchAddress("genpT", &genpT);
    input->SetBranchAddress("numGenJets", &numGenJets);
    input->SetBranchAddress("prefiring_weight", &prefiring_weight);
    if (lep == lepton::DITAU) {
      input->SetBranchAddress("nTruePU", &npu);
      input->SetBranchAddress("GenWeight", &genweight);
      input->SetBranchAddress("prefiring_weight_up", &prefiring_weight_up);
      input->SetBranchAddress("prefiring_weight_down", &prefiring_weight_down);
    } else if (lep == lepton::ELECTRON || lep == lepton::MUON || lep == lepton::EMU) {
      input->SetBranchAddress("npu", &npu);
      input->SetBranchAddress("genweight", &genweight);
      input->SetBranchAddress("prefiring_weight_Up", &prefiring_weight_up);
      input->SetBranchAddress("prefiring_weight_Down", &prefiring_weight_down);
    }
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

    if (isMadgraph) {
        input->SetBranchAddress("sm_weight_nlo", &sm_weight_nlo);
        input->SetBranchAddress("mm_weight_nlo", &mm_weight_nlo);
        input->SetBranchAddress("ps_weight_nlo", &ps_weight_nlo);
    }
}

std::string event_factory::fix_syst_string(std::string syst) {
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

Float_t event_factory::getPrefiringWeight() {
    if (syst == "prefiring_up") {
        return prefiring_weight_up;
    } else if (syst == "prefiring_down") {
        return prefiring_weight_down;
    }
    return prefiring_weight;
}

void event_factory::setRivets(TTree* input) {
    input->SetBranchAddress("Rivet_nJets30", &Rivet_nJets30);
    input->SetBranchAddress("Rivet_higgsPt", &Rivet_higgsPt);
    input->SetBranchAddress("Rivet_stage1_cat_pTjet30GeV", &Rivet_stage1_cat_pTjet30GeV);
}

Float_t event_factory::getRivetUnc(std::vector<double> uncs, std::string syst) {
    if (syst.find("Rivet") != std::string::npos) {
        int index = unc_map[syst];
        if (syst.find("Up") != std::string::npos) {
            return uncs.at(index);
        } else {
            return -1 * uncs.at(index);
        }
    }
    return 1.;
}

Float_t event_factory::getVBFTheoryUnc(std::string syst) {
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

Bool_t event_factory::getPassFlags(Bool_t isData) {
    if (era == 2016) {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter || Flag_HBHENoiseIsoFilter ||
                 (Flag_eeBadScFilter && isData) || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter);
    } else if (era == 2017) {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter || Flag_HBHENoiseIsoFilter ||
                 (Flag_eeBadScFilter && isData) || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter || Flag_ecalBadCalibFilter);
    } else {
        return !(Flag_goodVertices || Flag_globalSuperTightHalo2016Filter || Flag_HBHENoiseFilter || Flag_HBHENoiseIsoFilter ||
                 (Flag_eeBadScFilter && isData) || Flag_EcalDeadCellTriggerPrimitiveFilter || Flag_BadPFMuonFilter || Flag_ecalBadCalibFilter);
    }
}

// PRIVATE - fire_trigger(trigger::Ele24Tau30_2017)
Bool_t event_factory::getPassEle24Tau30() {
    return Ele24LooseTau30Pass && eMatchesEle24Tau30Filter && eMatchesEle24Tau30Path && tMatchesEle24Tau30Filter && tMatchesEle24Tau30Path;
}

// PRIVATE - fire_trigger(trigger::Ele24Tau30_2018)
Bool_t event_factory::getPassEle24Tau30_2018() {
    PassEle24Tau30_2018 = eMatchesEle24HPSTau30Filter && eMatchesEle24HPSTau30Path && tMatchesEle24HPSTau30Filter && tMatchesEle24HPSTau30Path;
    if (isData && run < 317509) {
        return eMatchesEle24Tau30Filter && eMatchesEle24Tau30Path && tMatchesEle24Tau30Filter && tMatchesEle24Tau30Path && Ele24LooseTau30Pass;
    } else if ((isData && run >= 317509) || !isData) {
        return eMatchesEle24HPSTau30Filter && eMatchesEle24HPSTau30Path && tMatchesEle24HPSTau30Filter && tMatchesEle24HPSTau30Path &&
               Ele24LooseHPSTau30Pass;
    }
    return false;
}

Bool_t event_factory::getPassCrossTrigger(Float_t pt) {
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
    return false;
}

Bool_t event_factory::fire_trigger(trigger t) {
    if (t == trigger::Ele24Tau30_2017) {
        return getPassEle24Tau30();
    } else if (t == trigger::Ele24Tau30_2018) {
        return getPassEle24Tau30_2018();
    }
    return true;
}

#endif  // INCLUDE_FSA_EVENT_FACTORY_H_
