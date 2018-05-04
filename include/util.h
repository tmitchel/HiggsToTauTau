
static double luminosity(35900.);
static std::map<std::string, double> cross_sections {
  // DY
  // DY1
  // DY1_missingfiles
  // DY1_noFakeTauCorr
  // DY2
  // DY2_missingfiles
  // DY2_noFakeTauCorr
  // DY3
  // DY3_noFakeTauCorr
  // DY4
  // DY4_missingfiles
  // DY4_noFakeTauCorr
  // DY_missingfiles
  // DY_noFakeTauCorr
  // DYlow
  // EWKminus
  // EWKplus
  {"EWKZZLL", 5765.4},
  // EWKZNuNu
  // HWW_gg125
  // HWW_vbf125
  // SMH_VBF110
  {"SMH_VBF120", 3.935*0.0698},
  {"SMH_VBF125", 3.782*0.0627},
  {"SMH_VBF130", 3.637*0.0541},
  {"SMH_VBF130_missingfiles", 3.637*0.0541},
  // SMH_VBF140
  // SMH_gg110
  {"SMH_ggH120", 47.38*0.0698},
  {"SMH_ggH125", 44.14*0.0627},
  {"SMH_ggH130", 41.23*0.0541},
  // SMH_gg140
  {"ST_tW_antitop", 35.6},
  {"ST_tW_top", 35.6},
  {"ST_t_antitop", 80.95},
  {"ST_t_top", 136.02},
  {"TT", 831.76},
  {"TT_missingfiles", 831.76},
  {"VV2L2Nu", 11.95},
  {"W", 61526.7},
  {"W1", 61526.7},
  {"W2", 61526.7},
  {"W3", 61526.7},
  {"W4", 61526.7},
  // WGToLNuG
  // WGstarLNuEE
  // WGstarLNuMuMu
  {"WW1L1Nu2Q", 49.997},
  {"WZ1L1Nu2Q", 10.71},
  {"WZ1L3Nu", 3.05},
  {"WZ2L2Q", 5.595},
  // WZLLLNu
  // WminusH110
  // WminusH120
  // WminusH125
  // WminusH130
  // WminusH140
  // WplusH110
  // WplusH120
  // WplusH125
  // WplusH130
  // WplusH140
  {"ZZ2L2Q", 3.22},
  {"ZZ4L", 1.212},

  {"data", 1.0},




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

static float calculate_mt(electron* const el, float met_x, float met_y, float met_pt) {
  return sqrt(pow(el->getPt() + met_pt, 2) - pow(el->getPx() + met_x, 2) - pow(el->getPy() + met_y, 2));
}
