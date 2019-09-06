# Scale Factors, Corrections, Etc.

### Tau Energy Scale

Applied directly to tau four-momentum at skim level

| Decay Mode | Gen Matching | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------:|:------------:|:-----------------:|:-----------------:|:-----------------:|
| 0          | 5            |0.994              |1.007              |0.987              |
| 1          | 5            |0.995              |0.998              |0.995              |
| 10         | 5            |1.000              |1.001              |0.988              |

### Electron to Tau Energy Scale

Applied directly to tau four-momentum at skim level

| Decay Mode | Gen Matching | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------:|:------------:|:-----------------:|:-----------------:|:-----------------:|
| 0          | 1 or 3       |0.995              |0.982              |0.968              |
| 1          | 1 or 3       |1.060              |1.018              |1.026              |

### Muon to Tau Energy Scale

Applied directly to tau four-momentum at skim level

| Decay Mode | Gen Matching | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------:|:------------:|:-----------------:|:-----------------:|:-----------------:|
| 0          | 2 or 4       |1.000              |0.998              |0.998              |
| 1          | 2 or 4       |0.995              |0.992              |0.990              |

### MET Recoil Corrections

Applied to MET at skim level to DY+Jets, W+Jets, and Higgs Production
https://github.com/CMS-HTT/RecoilCorrections

### Electron Energy Scale

At skim level, apply the correction to the electron p4.
```
TLorentzVector electron;
electron.SetPtEtaPhiM(ePt, eEta, ePhi, eMass);
electron *= eCorrectedEt / electron.Energy();
```

### Tau ID Efficiency Scale Factor

Tau ID Scale Factors are stored here [1] based on the recommendation here [2]

[1] https://github.com/cms-tau-pog/TauIDSFs/tree/master/data

[2] https://twiki.cern.ch/twiki/bin/viewauth/CMS/TauIDRecommendation13TeV#Tau_ID_MVA2017v2_efficiency_scal

Example Usage:
```
#include "TauPOG/TauIDSFs/interface/TauIDSFTool.h"
...
  // tau ID efficiency
  TauIDSFTool *tau_id_eff_sf = new TauIDSFTool(2016);
...
    if (tau.getGenMatch() == 5) {
        evtwt *= tau_id_eff_sf->getSFvsPT(tau.getPt());
    }
...
```

| 2016                                | 2017                                 | 2018                                 |
|:-----------------------------------:|:------------------------------------:|:------------------------------------:|
|TauID_SF_pt_MVAoldDM2017v2_2016.root | TauID_SF_pt_MVAoldDM2017v2_2017.root | TauID_SF_pt_MVAoldDM2017v2_2018.root |


### VLoose Electron to Tau Mis-ID Rate

|                  | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------------:|:-----------------:|:-----------------:|:-----------------:|
| abs(eta) < 1.460 |1.21               |1.09               |None yet           |
| abs(eta) > 1.559 |1.38               |1.19               |None yet           |

### Tight Electron to Tau Mis-ID Rate

|                  | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------------:|:-----------------:|:-----------------:|:-----------------:|
| abs(eta) < 1.460 |1.40               |1.80               |None yet           |
| abs(eta) > 1.559 |1.90               |1.53               |None yet           |

### Loose Muon to Tau Mis-ID Rate

|                  | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------------:|:-----------------:|:-----------------:|:-----------------:|
| abs(eta) < 0.4   |1.22               |1.06               |None yet           |
| abs(eta) < 0.8   |1.12               |1.02               |None yet           |
| abs(eta) < 1.2   |1.26               |1.10               |None yet           |
| abs(eta) < 1.7   |1.22               |1.03               |None yet           |
| abs(eta) < 2.3   |2.39               |1.94               |None yet           |

### Tight Muon to Tau Mis-ID Rate

|                  | Scale Factor 2016 | Scale Factor 2017 | Scale Factor 2018 |
|:----------------:|:-----------------:|:-----------------:|:-----------------:|
| abs(eta) < 0.4   |1.47               |1.17               |None yet           |
| abs(eta) < 0.8   |1.55               |1.29               |None yet           |
| abs(eta) < 1.2   |1.33               |1.14               |None yet           |
| abs(eta) < 1.7   |1.72               |0.93               |None yet           |
| abs(eta) < 2.3   |2.50               |1.61               |None yet           |


### Muon And Electron ID/Iso Efficiency

Contained here:
https://github.com/CMS-HTT/LeptonEfficiencies

| Lepton   | 2016 File                         | 2017 File                    | 2018 File                  |
|:--------:|:---------------------------------:|:----------------------------:|:--------------------------:|
| Electron |Electron_Run2016_legacy_IdIso.root | Electron_Run2017_IdIso.root  | Electron_Run2018_IdIso.root|
| Muon     |Muon_Run2016_legacy_IdIso.root     | Muon_IdIso_IsoLt0.2_eff.root | Muon_Run2018_IdIso.root    |

### Muon And Electron Trigger Efficiency

Single Lepton and Electron/Muon Leg of Cross Triggers here: <br/>
https://github.com/CMS-HTT/LeptonEfficiencies

Tau Leg of Cross Triggers: <br/>
2016 - https://github.com/rmanzoni/triggerSF/tree/moriond17 <br/>
2017 - https://github.com/cms-tau-pog/TauTriggerSFs/tree/final_2017_MCv2/data <br/>
2018 - https://github.com/cms-tau-pog/TauTriggerSFs/tree/final_2017_MCv2/data <br/>

| Single Trigger  | 2016 File                         | 2017 File                  | 2018 File                         |
|:---------------:|:---------------------------------:|:--------------------------:|:---------------------------------:|
| Single Electron |Electron_Run2016_legacy_Ele25.root | Electron_Ele32orEle35.root |Electron_Run2018_Ele32orEle35.root |
| Single Muon     |Muon_Run2016_legacy_IsoMu22.root   | Muon_IsoMu24orIsoMu27.root |Muon_Run2018_IsoMu24orIsoMu27.root |

| Cross Trigger      | 2016 File                                              | 2017 File                      | 2018 File                      |
|:------------------:|:------------------------------------------------------:|:------------------------------:|:------------------------------:|
| Electron - El Leg  |No Cross Trigger                                        |Electron_EleTau_Ele24.root      |Electron_Run2018_Ele24.root     |
| Electron - Tau Leg |No Cross Trigger                                        |tauTriggerEfficiencies2017.root |tauTriggerEfficiencies2017.root |
| Muon - Mu Leg      |Run2016BtoH/Muon_Mu19leg_2016BtoH_eff.root - temporary  |Muon_MuTau_IsoMu20.root         |Muon_Run2018_IsoMu20.root       |
| Muon - Tau Leg     |trigger_sf_mt.root - temporary                          |tauTriggerEfficiencies2017.root |tauTriggerEfficiencies2017.root |


### Electron Tracking Corrections

Tracking correction stored in `e_trk_ratio` dependent on `e_pt` and `e_eta`. 2017 and 2018 have additional factor `evtwt *= 0.991` for Z-Vtx HLT correction.

| 2016                        | 2017                          | 2018                          |
|:---------------------------:|:-----------------------------:|:-----------------------------:|
| htt_scalefactors_v16_3.root | htt_scalefactors_2017_v2.root | htt_scalefactors_2017_v2.root |

### b-Tag Efficiency

To be done...

### Pileup Reweighting

| Year/Type | File                                                                              |
|:---------:|:---------------------------------------------------------------------------------:|
| 2016 Data | /afs/hep.wisc.edu/home/tmitchel/public/Data_Pileup_2016_271036-284044_80bins.root |
| 2016 MC   | /afs/hep.wisc.edu/home/tmitchel/public/MC_Moriond17_PU25ns_V1.root                |
| 2017 Data | /afs/hep.wisc.edu/home/tmitchel/public/pu_distributions_data_2017.root            |
| 2017 MC   | /afs/hep.wisc.edu/home/tmitchel/public/pu_distributions_mc_2017.root              |
| 2018 Data | /afs/hep.wisc.edu/home/tmitchel/public/pu_distributions_data_2018.root            |
| 2018 MC   | /afs/hep.wisc.edu/home/tmitchel/public/pu_distributions_mc_2018.root              |

### Higgs pT Reweighting

Need to understand better, but based on number of jets from Rivet and only applied to ggH.

### Z pT/mass Reweighting

| Year/Type     | File                                                              |
|:-------------:|:-----------------------------------------------------------------:|
| 2016          | /afs/hep.wisc.edu/home/tmitchel/public/zpt_weights_2016_BtoH.root |
| 2017 and 2018 | htt_scalefactors_2017_v2.root                                     |

### Top quark pT Reweighting

Analytic function applied to ttbar background <br/>
```
sqrt(exp(0.0615 - 0.0005 * pt_top1) * exp(0.0615 - 0.0005 * pt_top2))
```

### Generator Weights

Generator weights are stored in FSA ntuples and applied per event.

---

# Triggers

### E-Tau Channel - 2016

| Trigger                       | pT      | filter    |
|:-----------------------------:|:-------:|:---------:|
|HLT_Ele25_eta2p1_WPTight_Gsf_v | pT > 26 | e matches |

### E-Tau Channel - 2017

| Trigger (OR of all)                                                  | Lepton pT    | Tau pT     | abs(tau eta) | filter     |
|:--------------------------------------------------------------------:|:------------:|:----------:|:------------:|:----------:|
|HLT_Ele32_WPTight_Gsf_v                                               | pT > 33      | None       | None         | e matches  |
|HLT_Ele35_WPTight_Gsf_v                                               | pT > 36      | None       | None         | e matches  |
|HLT_Ele24_eta2p1_WPTight_Gsf_Loose_ChargedIsoPFTau30_eta2p1_CrossL1_v | 25 < pT < 33 | pT > 32    | 2.1          | both match |

### E-Tau Channel - 2018

| Trigger (OR of all)                                                               | Lepton pT    | Tau pT     | abs(tau eta) | filter     |
|:---------------------------------------------------------------------------------:|:------------:|:----------:|:------------:|:----------:|
|HLT_Ele32_WPTight_Gsf_v                                                            | pT > 33      | None       | None         | e matches  |
|HLT_Ele35_WPTight_Gsf_v                                                            | pT > 36      | None       | None         | e matches  |
|HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTauHPS30_eta2p1_CrossL1_v**          | 21 < pT < 25 | pT > 31    | 2.1          | both match |
|HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTauHPS30_eta2p1_TightID_CrossL1_v*** | 21 < pT < 25 | pT > 31    | 2.1          | both match |

** Only data run < 317509

*** MC and data run > 317509

### Mu-Tau Channel - 2016

| Trigger (OR of all)                          | abs(eta) | pT           | filter     |
|:--------------------------------------------:|:--------:|:------------:|:----------:|
|HLT_IsoMu22_v                                 | 2.1      | pT > 23      | mu matches |
|HLT_IsoMu22_eta2p1_v                          | 2.1      | pT > 23      | mu matches |
|HLT_IsoTkMu22_v                               | 2.1      | pT > 23      | mu matches |
|HLT_IsoTkMu22_eta2p1                          | 2.1      | pT > 23      | mu matches |
|HLT_IsoMu19_eta2p1_LooseIsoPFTau20_v          | none     | 21 < pT < 23 | both match |
|HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1_v | none     | 21 < pT < 23 | both match |

### Mu-Tau Channel - 2017

| Trigger (OR of all)                                        | abs(lep eta) | Lepton pT    | Tau pT     | abs(tau eta) | filter     |
|:----------------------------------------------------------:|:------------:|:------------:|:----------:|:------------:|:----------:|
|HLT_IsoMu24_v                                               | 2.1          | pT > 25      | None       | None         | mu matches |
|HLT_IsoMu27_v                                               | 2.1          | pT > 28      | None       | None         | mu matches |
|HLT_IsoMu20_eta2p1_LooseChargedIso_PFTau27_eta2p1_CrossL1_v | none         | 21 < pT < 25 | pT > 31    | 2.1          | both match |


### Mu-Tau Channel - 2018

| Trigger (OR of all)                                                      | abs(lep eta) | Lepton pT    | Tau pT     | abs(tau eta) | filter     |
|:------------------------------------------------------------------------:|:------------:|:------------:|:----------:|:------------:|:----------:|
|HLT_IsoMu24_v                                                             | 2.1          | pT > 25      | None       | None         | mu matches |
|HLT_IsoMu27_v                                                             | 2.1          | pT > 28      | None       | None         | mu matches |
|HLT_IsoMu20_eta2p1_LooseChargedIsoPFTauHPS27_eta2p1_CrossL1_v1**          | none         | 21 < pT < 25 | pT > 31    | 2.1          | both match |
|HLT_IsoMu20_eta2p1_LooseChargedIsoPFTauHPS27_eta2p1_TightID_CrossL1_v1*** | none         | 21 < pT < 25 | pT > 31    | 2.1          | both match |

** Only data run < 317509

*** MC and data run > 317509

---

# Event Selection

### Common Selection

- Each trigger needs to be matched with deltaR < 0.5 of the trigger objects and must match the trigger path.
- MET Filters
    - primary vertex filter
    - beam halo filter
    - HBHE noise filter
    - HBHEiso noise filter
    - eebadSC noise filter (data only)
    - ECAL TP filter
    - badMuon filter
    - badCharged hadron filter
    - ECAL bad calibration filter (only 2017)
- b-Jet Veto
    - Discard events with at least one b-jet (pT > 20 GeV && |eta| < 2.1 && medium DeepCSV)
- Extra Lepton Veto
    - Veto events with a third lepton (first two are muon/electron and tau)
- deltaR(tau, lep) > 0.5
- MT(lep, MET) < 50
- Pass trigger with associated criteria

### E-Tau Selection

Electron Selection
- trigger associated criteria
- pT > 25
- |eta| < 2.5 (although from scale factors it appears to be < 2.1)
- d<sub>xy</sub> < 0.045
- d<sub>z</sub> < 0.2
- MVA ID without Isolation at WP80
- Relative Isolation < 0.10

Tau Selection
- pT > 30
- |eta| < 2.3
- Tight MVA ID (byTightIsolationMVArun2017v2DBoldDMwLT2017)
- old decay mode finding (decay != 5 && decay != 6)
- d<sub>z</sub> < 0.2
- Interger charge (+/-1)
- Tight MVA anti-electron discriminator (againstElectronTightMVA6)
- Loose MVA anti-muon discriminator (againstMuonLooseMVA3)

### Mu-Tau Selection

Muon Selection
- trigger associated criteria
- pT > 20
- |eta| < 2.4
- d<sub>xy</sub> < 0.045
- d<sub>z</sub> < 0.2
- Medium ID
- Relative Isolation < 0.15

Tau Selection
- pT > 30
- |eta| < 2.3
- Tight MVA ID (byTightIsolationMVArun2017v2DBoldDMwLT2017)
- old decay mode finding (decay != 5 && decay != 6)
- d<sub>z</sub> < 0.2
- Interger charge (+/-1)
- VLoose MVA anti-electron discriminator (againstElectronVLooseMVA6)
- Tight MVA anti-muon discriminator (againstMuonTightMVA3)

# Deprecated


### Skim Level Selection
 - Mu24 || Mu27 || Mu20Tau27
 - mu pT > 21
 - |mu eta| < 2.1
 - |mu dZ| < 0.2
 - |mu dXY| < 0.045
 - medium muon ID (not applied to embedded)
   - Segment Compatibility > (goodglob ? 0.303 : 0.451)
   - goodglob:
     - global muon
     - Normalized chi2 < 3
     - Chi2 Local Position < 12
     - Track Kink < 20
    - PF ID Loose
    - Valid Fraction > 0.49
 - tau pT > 23
 - |tau eta| < 2.3
 - |tau dZ| < 0.2
 - Rerun MVA 2v2 Loose
 - decay mode finding > 0
 - tau charge < 2
 - tight anti-muon discriminator
 - vloose anti-electron discriminator
 - mu veto < 2
 - ele veto == 0
 - dimuon veto == 0

#### Skim Level Weights


##### Tau Energy Scale in Embedded

| Decay Mode | Gen Matching | Scale Factor |
|:----------:|:------------:|:------------:|
| 0          | 5            |0.975         |
| 1          | 5            |0.975 * 1.051 |
| 10         | 5            |0.975^3       |

TES corrections are propogated to MET as well

##### Recoil Corrections
Based on:
- MET pX
- MET pY
- gen pX
- gen pY
- visible pX
- visible pY
- njets (+1 for W+Jets)
- corrected MET pX
- corrected MET pY

#### Analysis Level Selection

- Tau Decay Mode == 5 || 6
- Triggers with matching/filtering
  - Mu27 with mu pT > 28
  - Mu24 with mu pT > 25
  - Mu20Tau27 with 21 < mu pT < 25
- deltaR(mu, tau) > 0.5
- Transverse Mass < 50
- Opposite Sign
- Veto b-jets
- Gen-Matching for different processes

- Signal Region
  - Tight tau isolation
  - muon isolation < 0.15
- Tau Anti-Isolation Region
  - Tau fails tight isolation
  - muon isolation < 0.15

#### Analysis Level Weights

- Cross Section * Lumi Weighting or Stitching Weights
- MC Only
  - tau ID: 89%
  - Anti-lepton Discriminator
    - | eta < 0.4 | eta < 0.8 | eta < 1.2 | eta < 1.7 | eta < 2.1 |
      |:---------:|:---------:|:---------:|:---------:|:---------:|
      | 1.17      | 1.29      | 1.14      | 0.93      | 1.61      |
  - Pileup Reweighting with sample-specific weights based on NPV
  - Generator Weights
  - Workspace Variables and Weights:
    - muon pT, muon eta, gen Z mass, gen Z pT
    - m_id_kit_ratio
    - m_iso_kit_ratio
    - Trigger SF = single * (firedTrigger) + mu_leg * tau_leg * (firedCross)
      - m_trg24_27_kit_data / m_trg24_27_kit_mc
      - m_trg20_data / m_trg20_mc
      - getMuTauScaleFactor(tau.getPt(), tau.getEta(), tau.getPhi(), TauTriggerSFs2017::kCentral)
    - zptmass_weight_nom for DY processes
  - sqrt(exp(0.0615 - 0.0005 * pt_top1) * exp(0.0615 - 0.0005 * pt_top2));
  - premote-demote for b-tag SF
- Embedded Only
  - tau ID: 97%
  - Workspace Variables and Weights:
    - tau pT, muon pT, muon eta, muon isolation, gen mu pT, gen mu eta, gen tau pT, gen tau eta
    - m_sel_trg_ratio
    - m_sel_idEmb_ratio
    - m_id_embed_kit_ratio
    - m_iso_binned_embed_kit_ratio
    - Trigger SF = single * (firedTrigger) + mu_leg * tau_leg * (firedCross)
      - m_trg24_27_embed_kit_ratio
      - m_trg_MuTau_Mu20Leg_kit_ratio_embed
      - mt_emb_LooseChargedIsoPFTau27_kit_ratio
    - Generator Weight
    - 
