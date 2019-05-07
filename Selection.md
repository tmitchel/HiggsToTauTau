# Event Selections for each channel

## Mu-Tau Channel - 2016



## Mu-Tau Channel - 2017

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

### Skim Level Weights

#### Tau Energy Scale in MC

| Decay Mode | Gen Matching | Scale Factor |
|:----------:|:------------:|:------------:|
| 0          | 5            |1.007         |
| 1          | 5            |0.998         |
| 10         | 5            |1.001         |
| 0          | 1 or 3       |1.003         |
| 1          | 1 or 3       |1.036         |
| 10         | 1 or 3       |1.000         |

#### Tau Energy Scale in Embedded

| Decay Mode | Gen Matching | Scale Factor |
|:----------:|:------------:|:------------:|
| 0          | 5            |0.975         |
| 1          | 5            |0.975 * 1.051 |
| 10         | 5            |0.975^3       |

TES corrections are propogated to MET as well

#### Recoil Corrections
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


### Analysis Level Selection