
static double luminosity(35900.);
static std::map<std::string, double> cross_sections {
  {"DY", 5765.4},
  {"DY1", 5765.4},
  {"DY1_missingfiles", 5765.4},
  {"DY2", 5765.4},
  {"DY1_noFakeTauCorr", 5765.4},
  {"DY2_missingfiles", 5765.4},
  {"DY2_noFakeTauCorr", 5765.4},
  {"DY3", 5765.4},
  {"DY3_noFakeTauCorr", 5765.4},
  {"DY4", 5765.4},
  {"DY4_missingfiles", 5765.4},
  {"DY4_noFakeTauCorr", 5765.4},
  {"DY_missingfiles", 5765.4},
  {"DY_noFakeTauCorr", 5765.4},
  {"DYlow", 5765.4},
  // {"DY1", 1178.3},
  // {"DY1_missingfiles", 1178.3},
  // {"DY2", 387.3},
  // {"DY1_noFakeTauCorr", 1178.3},
  // {"DY2_missingfiles", 387.3},
  // {"DY2_noFakeTauCorr", 387.3},
  // {"DY3", 118.4},
  // {"DY3_noFakeTauCorr", 118.4},
  // {"DY4", 63.7},
  // {"DY4_missingfiles", 63.7},
  // {"DY4_noFakeTauCorr", 63.7},
  // {"DY_missingfiles", 5765.4},
  // {"DY_noFakeTauCorr", 5765.4},
  // {"DYlow", 5765.4},
  {"EWKminus", 20.25},
  {"EWKplus", 25.62},
  {"EWKZLL", 3.987},
  {"EWKZNuNu", 10.01},
  {"HWW_gg125", 48.58*0.2137*0.3258},
  {"HWW_vbf125", 3.782*0.2137*0.3258},
  {"SMH_VBF110", 4.434*0.0791},
  {"SMH_VBF120", 3.935*0.0698},
  {"SMH_VBF125", 3.782*0.0627},
  {"SMH_VBF130", 3.637*0.0541},
  {"SMH_VBF130_missingfiles", 3.637*0.0541},
  {"SMH_VBF140", 3.492*0.0360},
  {"SMH_gg110", 57.90*0.0791},
  {"SMH_ggH120", 47.38*0.0698},
  {"SMH_ggH125", 44.14*0.0627},
  {"SMH_ggH130", 41.23*0.0541},
  {"SMH_gg140", 36.0*0.0360},
  {"ST_tW_antitop", 35.6},
  {"ST_tW_top", 35.6},
  {"ST_t_antitop", 80.95},
  {"ST_t_top", 44.07},
  {"TT", 831.76},
  {"TT_missingfiles", 831.76},
  {"VV2L2Nu", 11.95},
  {"W", 61526.7},
  {"W1", 61526.7},
  {"W2", 61526.7},
  {"W3", 61526.7},
  {"W4", 61526.7},
  {"WGToLNuG", 489.0},
  {"WGstarLNuEE", 3.526},
  {"WGstarLNuMuMu", 2.793},
  {"WW1L1Nu2Q", 49.997},
  {"WZ1L1Nu2Q", 10.71},
  {"WZ1L3Nu", 3.05},
  {"WZ2L2Q", 5.595},
  {"WZLLLNu", 4.708},
  {"WminusH110", 0.8587*0.0791},
  {"WminusH120", 0.6092*0.0698},
  {"WminusH125", 0.5328*0.0627},
  {"WminusH130", 0.4676*0.0541},
  {"WminusH140", 0.394*0.0360},
  {"WplusH110", 1.335*0.0791},
  {"WplusH120", 0.9558*0.0698},
  {"WplusH125", 0.840*0.0627},
  {"WplusH130", 0.7414*0.0541},
  {"WplusH140", 0.6308*0.0360},
  {"ZZ2L2Q", 3.22},
  {"ZZ4L", 1.212},
  {"data", 1.0}



  // {"ZL", 5765.4},
  // {"ZTT", 5765.4},
  // {"ZJ", 5765.4},
  // {"QCD", 720648000*0.00042},
  // {"WZJets", 5.26},
  // {"ggh", 44.14*0.0627},
  // {"VBF", 3.782*0.0627},
  // {"ZZ", 16.523},
  // {"WZ", 47.13},
  // {"WW", 118.7}
};

// do the mt calculation
static float calculate_mt(electron* const el, float met_x, float met_y, float met_pt) {
  return sqrt(pow(el->getPt() + met_pt, 2) - pow(el->getPx() + met_x, 2) - pow(el->getPy() + met_y, 2));
}
