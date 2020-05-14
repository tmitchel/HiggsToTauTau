// Copyright 2020 Tyler Mitchell

#ifndef INCLUDE_MODELS_DEFAULTS_H_
#define INCLUDE_MODELS_DEFAULTS_H_

// trigger working points
enum trigger {
    Ele24Tau30_2017 = 100,
    Ele24Tau30_2018 = 101,
    Ele35_WPTight_Gsf = 3,
    IsoMu27 = 19,
    Mu50 = 21,
    Ele115_CaloIdVT_GsfTrkIdT = 38
    };

// isolation working points
enum wps {
    mva_vloose = 0,
    mva_loose = 1,
    mva_medium = 2,
    mva_tight = 3,
    mva_vtight = 4,
    deep_vvvloose = 0,
    deep_vvloose = 1,
    deep_vloose = 2,
    deep_loose = 3,
    deep_medium = 4,
    deep_tight = 5,
    deep_vtight = 6,
    deep_vvtight = 7,
    deep_vvvtight = 8,
    btag_any = 9,
    btag_loose = 10,
    btag_medium = 11
};

// possible channels
enum lepton { ELECTRON, MUON, DITAU, EMU };

#endif  // INCLUDE_MODELS_DEFAULTS_H_
