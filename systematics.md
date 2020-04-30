# Systematic Uncertainties

### DeepTau vsJet efficiency
- The uncertainties in the SFs should be treated uncorrelated across years, as well as across tau pT bins
    - The pT bins should be at the finest [20,25,30,35,40,infty], so five nuisance parameters (__these are included already__)
- Analyses that use looser working point of VSe (looser than VLoose) and/or VSmu (looser than Tight) discriminators, should add additional uncertainty. In this case, additional 3% should be added to MC and 5% to embedding sample for nominal tau pT < 100 GeV. For high pT tau, where tau pT > 100 GeV, additional 15% should be added.
    - mutau channel: VVVLoose VSe (__need to separate into tau pT bins__)
    - etau channel: VLoose VSmu (__need to separate into tau pT bins__)

### Genuine DeepTau Energy Scale (not used yet)
- The uncertainty of TES are binned in pT (not mentioned in twiki, but I assume should be binned in DM as well)
    - pT <= 34
    - 34 < pT < 170
    - pT >= 170

### Genuine MVA Tau Energy Scale (currently used in ltau)
- For taus with a pT between 20 and 400 GeV, the uncertainties in the tau energy scale should be treated uncorrelated across different decay modes (__remember to include 11__), and years.
- For analyses using taus with higher pT %gt; 400 GeV, no correction is necessary (while it can still be applied for consistency). A flat 3% uncertainty that is correlated between all decay modes should be used in this latter case. (__I think we can ignore this because we have so few taus above 400 GeV if any__)

### DeepTau vsEl efficiency
-  The uncertainties in the SFs should be treated uncorrelated across years, as well as across tau eta bins
    - barrel vs endcap

### Electron Misidentified as Tau Energy Scale
- The uncertainties in the electron faking tau energy scale should be treated uncorrelated across different decay modes, and years (__and eta bins, but the twiki isn't up to date yet__)
    - ends up split into year, DM, barrel vs endcap

### DeepTau vsMu efficiency
- The uncertainties in the SFs should be treated uncorrelated across years, as well as across tau eta bins
    - The eta bins should be at the finest [0,0.4,0.8,1.2,1.7,infty]

### Muon Misidentified as Tau Energy Scale
- The uncertainty of muon to tau energy scale is 1% uncorrelated in DM
- __Need to rerun SVFit to include this__

### Trigger
- systematics are split between single trigger and cross-trigger with naming like 
    - singleeletrg
    - eletautrg

### Genuine Electron Energy Scale
- drop EESigma shifts because we don't care about them (done in STXS as well)

### Genuine Muon Energy Scale
- uncorrelated in eta bins [0, 1.2, 2.1, infty]
