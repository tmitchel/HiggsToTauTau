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
#include "models/defaults.h"

class Helper {
   private:
    TFile *output_file;
    double luminosity2016, luminosity2017, luminosity2018, w_xs_corr, dy_xs_corr;
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
    Float_t muon_tracking(Float_t);
    void create_and_fill(std::string, std::vector<Float_t>, Float_t, Float_t);
    void create_and_fill(std::string, std::vector<Float_t>, Float_t, Float_t, Float_t);
};

Helper::Helper(TFile *fout, std::string name, std::string syst)
    : output_file(fout),
      luminosity2016(35900.),
      luminosity2017(41500.),
      luminosity2018(59740.),
      w_xs_corr(1.21),
      dy_xs_corr(1.23),
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
                     {"ST_tW_antitop", 35.6},
                     {"ST_tW_top", 35.6},
                     {"ST_t-channel_antitop", 26.23},
                     {"ST_t-channel_topn", 44.07},
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
                     {"zh125", 0.8839 * 0.062},
                     {"WJetsToLNu_HT-100To200", 1345. * w_xs_corr},
                     {"WJetsToLNu_HT-200To400", 359.7 * w_xs_corr},
                     {"WJetsToLNu_HT-400To600", 48.91 * w_xs_corr},
                     {"WJetsToLNu_HT-600To800", 12.05 * w_xs_corr},
                     {"WJetsToLNu_HT-800To1200", 5.501 * w_xs_corr},
                     {"WJetsToLNu_HT-1200To2500", 1.32 * w_xs_corr},
                     {"WJetsToLNu_HT-2500ToInf", 0.03216 * w_xs_corr},
		     {"DYJetsToLL_Pt-50To100", 354.3 * dy_xs_corr},
		     {"DYJetsToLL_Pt-100To250", 83.12 * dy_xs_corr},
		     {"DYJetsToLL_Pt-250To400", 3.047 * dy_xs_corr},
		     {"DYJetsToLL_Pt-400To650", 0.3921 * dy_xs_corr},
		     {"DYJetsToLL_Pt-650ToInf", 0.03636 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-100to200", 147.40 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-200to400", 40.99 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-400to600", 5.678 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-600to800", 1.367 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-800to1200", 0.6304 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-1200to2500", 0.1514 * dy_xs_corr},
                     {"DYJetsToLL_M-50_HT-2500toInf", 0.003565 * dy_xs_corr}} {};

Float_t Helper::embed_tracking(Float_t decay_mode, Int_t syst = 0) {
    Float_t sf(.99), prong(0.975), pizero(1.051);
    Float_t dm0_syst(0.008), dm1_syst(0.016124515), dm10_syst(0.013856406), dm11_syst(0.019697716);
    if (decay_mode == 0) {
        return sf * prong + (syst * dm0_syst);
    } else if (decay_mode == 1) {
        return sf * prong * pizero + (syst * dm1_syst);
    } else if (decay_mode == 10) {
        return sf * prong * prong * prong + (syst * dm10_syst);
    } else if (decay_mode == 11) {
        return sf * prong * prong * prong * pizero + (syst * dm11_syst);
    } else {
        std::cerr << "Invalid decay mode " << decay_mode << std::endl;
        return 1;
    }
}

Float_t Helper::muon_tracking(Float_t eta) {
    if (eta >= -2.4 && eta < -2.1) {
        return 0.9879;
    } else if (eta >= -2.1 && eta < -1.6) {
        return 0.9939;
    } else if (eta >= -1.6 && eta < -1.1) {
        return 0.9970;
    } else if (eta >= -1.1 && eta < -0.6) {
        return 0.9954;
    } else if (eta >= -0.6 && eta < 0) {
        return 0.9937;
    } else if (eta >= 0 && eta < 0.6) {
        return 0.9959;
    } else if (eta >= 0.6 && eta < 1.1) {
        return 0.9976;
    } else if (eta >= 1.1 && eta < 1.6) {
        return 0.9961;
    } else if (eta >= 1.6 && eta < 2.1) {
        return 0.9930;
    } else if (eta >= 2.1 && eta < 2.4) {
        return 0.9819;
    }
    return 1;
}

Float_t Helper::transverse_mass(TLorentzVector lep, Float_t met, Float_t metphi) {
    double met_x = met * cos(metphi);
    double met_y = met * sin(metphi);
    double met_pt = sqrt(pow(met_x, 2) + pow(met_y, 2));
    return sqrt(pow(lep.Pt() + met, 2) - pow(lep.Px() + met_x, 2) - pow(lep.Py() + met_y, 2));
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
