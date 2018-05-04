
static double luminosity(35900.);
static std::map<std::string, double> cross_sections {
  {"ZL", 5765.4},
  {"ZTT", 5765.4},
  {"ZJ", 5765.4},
  {"ZLL", 5765.4},
  {"TT", 831.76},
  {"W", 61526.7},
  {"ZZ4L", 1.212},
  {"QCD", 720648000*0.00042},
  {"data", 1.0},
  {"WZ1L1Nu2Q", 10.71},
  {"WZ1L3Nu", 3.05},
  {"WZJets", 5.26},
  {"WZ2L2Q", 5.595},
  {"WW1L1Nu2Q", 49.997},
  {"ZZ2L2Q", 3.22},
  {"VV2L2Nu", 11.95},
  {"ST_tW_antitop", 35.6},
  {"ST_tW_top", 35.6},
  {"ST_t_antitop", 80.95},
  {"ST_t_top", 136.02},
  {"ggh", 44.14*0.0627},
  {"VBF", 3.782*0.0627},
  {"ggH125", 44.14*0.0627},
  {"VBF125", 3.782*0.0627},
  {"ggH120", 47.38*0.0698},
  {"VBF120", 3.935*0.0698},
  {"ggH130", 41.23*0.0541},
  {"VBF130", 3.637*0.0541},
  {"ZZ", 16.523},
  {"WZ", 47.13},
  {"WW", 118.7}
};

static float calculate_mt(electron* const el, float met_x, float met_y, float met_pt) {
  return sqrt(pow(el->getPt() + met_pt, 2) - pow(el->getPx() + met_x, 2) - pow(el->getPy() + met_y, 2));
}
