// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_SWISS_ARMY_CLASS_H_
#define INCLUDE_SWISS_ARMY_CLASS_H_

// system include
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// ROOT include
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLorentzVector.h"

enum lepton { ELECTRON, MUON, DITAU, EMU };

class Helper {
 private:
    TFile* output_file;
    double luminosity2016, luminosity2017, luminosity2018;
    std::map<std::string, double> cross_sections;
    std::unordered_map<std::string, TH1F *> histos_1d;
    std::unordered_map<std::string, TH2F *> histos_2d;

 public:
    Helper(TFile *, std::string, std::string);
    ~Helper() {}
    double getCrossSection(std::string sample) { return cross_sections[sample]; }
    double getLuminosity2016() { return luminosity2016; }
    double getLuminosity2017() { return luminosity2017; }
    double getLuminosity2018() { return luminosity2018; }
    std::unordered_map<std::string, TH1F *> *getHistos1D() { return &histos_1d; }
    std::unordered_map<std::string, TH2F *> *getHistos2D() { return &histos_2d; }

    Float_t transverse_mass(TLorentzVector, Float_t, Float_t);
    Float_t deltaR(Float_t eta1, Float_t phi1, Float_t eta2, Float_t phi2) { return sqrt(pow(eta1 - eta2, 2) + pow(phi1 - phi2, 2)); }
    Float_t embed_tracking(Float_t, Int_t);
    void create_and_fill(std::string, std::vector<Float_t>, Float_t, Float_t);
    void create_and_fill(std::string, std::vector<Float_t>, Float_t, Float_t, Float_t);
};

Helper::Helper(TFile *fout, std::string name, std::string syst)
    : output_file(fout),
      luminosity2016(35900.),
      luminosity2017(41500.),
      luminosity2018(59740.),
      cross_sections{{"DYJets1", 6225.42},
                     {"DYJets2", 6225.42},
                     {"DYJets2_lowM", 6225.42},
                     {"DYJets3", 6225.42},
                     {"DYJets4", 6225.42},
                     {"DYJets", 6225.42},
                     {"DYJets_lowM", 6225.42},
                     {"EWKWMinus", 20.25},
                     {"EWKWPlus", 25.62},
                     {"EWKZ2l", 3.987},
                     {"EWKZ2nu", 10.01},
                     {"Tbar-tW", 35.6},
                     {"T-tW", 35.6},
                     {"Tbar-tchan", 26.23},
                     {"T-tchan", 44.07},
                     {"TT", 831.76},
                     {"TTHad", 377.96},
                     {"TTLep", 88.29},
                     {"TTSemi", 365.35},
                     {"VV", 11.95},
                     {"WJets", 61526.7},
                     {"WJets1", 61526.7},
                     {"WJets2", 61526.7},
                     {"WJets3", 61526.7},
                     {"WJets4", 61526.7},
                     {"WW1l1nu2q", 49.997},
                     {"WZ1l1nu2q", 10.71},
                     {"WZ1l3nu", 3.05},
                     {"WZ2l2Q", 5.595},
                     {"WZ3l1nu", 4.708},
                     {"WW", 75.88},
                     {"WZ", 27.57},
                     {"ZZ", 12.14},
                     {"ZZ2l2q", 3.22},
                     {"ZZ4l", 1.212},
                     {"data", 1.0},
                     {"Data", 1.0},
                     {"vbf125", 3.782 * 0.0627},
                     {"ggh125", 48.58 * 0.0627},
                     {"ggh125_madgraph_zero_a1_filtered", 0.3989964912},
                     {"ggh125_madgraph_zero_a3_filtered", 0.394402286},
                     {"ggh125_madgraph_zero_a3int_filtered", 0.3909346216},
                     {"ggh125_madgraph_one_a1_filtered", 0.2270577971},
                     {"ggh125_madgraph_one_a3_filtered", 0.2279645653},
                     {"ggh125_madgraph_one_a3int_filtered", 0.2278590524},
                     {"ggh125_madgraph_two_a1_filtered", 0.1383997884},
                     {"ggh125_madgraph_two_a3_filtered", 0.1336590511},
                     {"ggh125_madgraph_two_a3int_filtered", 0.1375149881},
                     {"tth125", 0.5071 * 0.0627},
                     {"wh125", 0.6864 * 0.0627},  // took the average of W+ and W-. Not important because for JHU it's reweighted to Powheg anyways
                     {"wminus125", 0.5328 * 0.0627},
                     {"wplus125", 0.840 * 0.0627},
                     {"zh125", 0.8839 * 0.062}} {};

Float_t Helper::embed_tracking(Float_t decay_mode, Int_t syst = 0) {
  Float_t sf(.99), prong(0.975), pizero(1.051);
  Float_t dm0_syst(0.008), dm1_syst(0.016124515), dm10_syst(0.013856406), dm11_syst(0.019697716);
  if (decay_mode == 0) {
    sf *= prong + (syst * dm0_syst);
  } else if (decay_mode == 1) {
    sf *= prong * pizero + (syst * dm1_syst);
  } else if (decay_mode == 10) {
    sf *= prong * prong * prong + (syst * dm10_syst);
  } else if (decay_mode == 11) {
    sf *= prong * prong * prong * pizero + (syst * dm11_syst);
  } else {
    std::cerr << "Invalid decay mode " << decay_mode << std::endl;
    return 1;
  }
}

Float_t Helper::transverse_mass(TLorentzVector lep, Float_t met, Float_t metphi) {
  double met_x = met * cos(metphi);
  double met_y = met * sin(metphi);
  double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
  return sqrt(pow(lep.Pt() + met_pt, 2) - pow(lep.Px() + met_x, 2) - pow(lep.Py() + met_y, 2));
}

void Helper::create_and_fill(std::string name, std::vector<Float_t> bins, Float_t value, Float_t weight) {
  if (histos_1d.find(name) == histos_1d.end()) {
    if (bins.size() < 3) {
      std::cerr << "Not enough bins provided" << std::endl;
      return;
    }
    output_file->cd("grabbag");
    histos_1d[name] = new TH1F(name.c_str(), name.c_str(), bins.at(0), bins.at(1), bins.at(2));
  }
  histos_1d.at(name)->Fill(value, weight);
}

void Helper::create_and_fill(std::string name, std::vector<Float_t> bins, Float_t value_x, Float_t value_y, Float_t weight) {
  if (histos_2d.find(name) == histos_2d.end()) {
    if (bins.size() < 6) {
      std::cerr << "Not enough bins provided" << std::endl;
      return;
    }
    output_file->cd("grabbag");
    histos_2d[name] = new TH2F(name.c_str(), name.c_str(), bins.at(0), bins.at(1), bins.at(2), bins.at(3), bins.at(4), bins.at(5));
  }
  histos_2d.at(name)->Fill(value_x, value_y, weight);
}

#endif  // INCLUDE_SWISS_ARMY_CLASS_H_
