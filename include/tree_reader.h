#ifndef INCLUDE_TREE_READER_H_
#define INCLUDE_TREE_READER_H_

#include "TTree.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class tree_reader {
 public:
  tree_reader() {}
  explicit tree_reader(std::map<std::string, std::vector<float>>);
  void setBranches(TTree*, std::string);
  Float_t getVar(std::string);

  Int_t cat_0jet, cat_boosted, cat_vbf, cat_VH, is_signal, is_antiLepIso, is_antiTauIso, is_qcd, is_looseIso, OS, SS;
  Float_t evtwt,
      el_pt, el_eta, el_phi, el_mass, el_charge, el_iso,
      mu_pt, mu_eta, mu_phi, mu_mass, mu_charge, mu_iso,
      t1_pt, t1_eta, t1_phi, t1_mass, t1_charge, t1_iso, t1_iso_VL, t1_iso_L,
      t1_iso_M, t1_iso_T, t1_iso_VT, t1_iso_VVT, t1_decayMode, t1_genMatch,  // t1 is used for et and mt, as well
      t2_pt, t2_eta, t2_phi, t2_mass, t2_charge, t2_iso, t2_iso_VL, t2_iso_L,
      t2_iso_M, t2_iso_T, t2_iso_VT, t2_iso_VVT, t2_decayMode, t2_genMatch,
      njets, nbjets,
      j1_pt, j1_eta, j1_phi,
      j2_pt, j2_eta, j2_phi,
      b1_pt, b1_eta, b1_phi,
      b2_pt, b2_eta, b2_phi,
      met, metphi, mjj, numGenJets, mt, dmf, dmf_new,
      pt_sv, m_sv, Dbkg_VBF, Dbkg_ggH, VBF_MELA,
      Phi, Phi1, costheta1, costheta2, costhetastar, Q2V1, Q2V2,
      ME_sm_VBF, ME_sm_ggH, ME_sm_WH, ME_sm_ZH, ME_bkg, ME_bkg1, ME_bkg2, D0_VBF, DCP_VBF, D0_ggH, DCP_ggH,
      higgs_pT, higgs_m, hjj_pT, hjj_m, dEtajj, dPhijj, vis_mass, NN_disc;

  // Anomolous coupling SetBranchAddresses
  Float_t wt_a1, wt_a2, wt_a3, wt_L1, wt_L1Zg, wt_a2int, wt_a3int, wt_L1int, wt_L1Zgint, wt_ggH_a1, wt_ggH_a3,
      wt_ggH_a3int, wt_wh_a1, wt_wh_a2, wt_wh_a3, wt_wh_L1, wt_wh_L1Zg, wt_wh_a2int, wt_wh_a3int, wt_wh_L1int,
      wt_wh_L1Zgint, wt_zh_a1, wt_zh_a2, wt_zh_a3, wt_zh_L1, wt_zh_L1Zg, wt_zh_a2int, wt_zh_a3int, wt_zh_L1int,
      wt_zh_L1Zgint, acWeightVal;

  std::map<std::string, std::vector<float>> variables;
};

#endif  // INCLUDE_TREE_READER_H_
