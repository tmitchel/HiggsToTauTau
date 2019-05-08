# Event Selections for each channel

## Mu-Tau Channel - 2016

---

## Mu-Tau Channel - 2017
---

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

##### Tau Energy Scale in MC

| Decay Mode | Gen Matching | Scale Factor |
|:----------:|:------------:|:------------:|
| 0          | 5            |1.007         |
| 1          | 5            |0.998         |
| 10         | 5            |1.001         |
| 0          | 1 or 3       |1.003         |
| 1          | 1 or 3       |1.036         |
| 10         | 1 or 3       |1.000         |

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
