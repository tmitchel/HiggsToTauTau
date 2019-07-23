// Copyright [2018] Tyle Mitchell

#ifndef INCLUDE_TAUSF_H_
#define INCLUDE_TAUSF_H_

#include <map>
#include <string>
#include "TMath.h"

class tauSF {
 private:
    std::map<std::string, std::map<std::string, double>> data;

 public:
    tauSF();
    ~tauSF() {}
    double compute_SF(double, std::string);
    double crystalballEfficiency(double, double, double, double, double, double);
    double tauID_SF(int, double);
    double boosted_ZmmSF(double, std::string);
    double VBF_ZmmSF(double, std::string);
};

tauSF::tauSF() {
    std::map<std::string, double> mc_genuine_TightIso_dm10{{"alpha", 1.686527964047553},
                                                           {"m_{0}", 39.82716689014602},
                                                           {"sigma", 5.046934778494286},
                                                           {"norm", 0.9709884820866538},
                                                           {"n", 119.86933951486884}};
    std::map<std::string, double> mc_genuine_TightIso_dm1{{"alpha", 4.661955903316635},
                                                          {"m_{0}", 36.33798366748843},
                                                          {"sigma", 4.5521495089687765},
                                                          {"norm", 0.9999999970681116},
                                                          {"n", 1.6079296195874044}};
    std::map<std::string, double> mc_genuine_TightIso_dm0{{"alpha", 4.862445750387961},
                                                          {"m_{0}", 38.083905839292164},
                                                          {"sigma", 6.202388502948717},
                                                          {"norm", 0.9321292524169671},
                                                          {"n", 3.397804647368993}};
    std::map<std::string, double> data_genuine_TightIso_dm10{{"alpha", 2.153157818960823},
                                                             {"m_{0}", 40.57070442578432},
                                                             {"sigma", 5.245672570732005},
                                                             {"norm", 0.9999999969400835},
                                                             {"n", 12.26252955476852}};
    std::map<std::string, double> data_genuine_TightIso_dm1{{"alpha", 5.631705386364713},
                                                            {"m_{0}", 36.70311525314159},
                                                            {"sigma", 4.703474934239983},
                                                            {"norm", 0.999999997352303},
                                                            {"n", 1.6444215737140877}};
    std::map<std::string, double> data_genuine_TightIso_dm0{{"alpha", 7.283491259365051},
                                                            {"m_{0}", 38.822860380950296},
                                                            {"sigma", 7.168040439424631},
                                                            {"norm", 0.9907280391995551},
                                                            {"n", 2.3082298644327626}};

    this->data = std::map<std::string, std::map<std::string, double>>{
        {"mc_genuine_TightIso_dm10", mc_genuine_TightIso_dm10},   {"mc_genuine_TightIso_dm1", mc_genuine_TightIso_dm1},
        {"mc_genuine_TightIso_dm0", mc_genuine_TightIso_dm0},     {"data_genuine_TightIso_dm10", data_genuine_TightIso_dm10},
        {"data_genuine_TightIso_dm1", data_genuine_TightIso_dm1}, {"data_genuine_TightIso_dm0", data_genuine_TightIso_dm0},
    };
}

double tauSF::compute_SF(double x, std::string name) {
    if (x == -1.0) {
        return -1.0;
    }

    auto dt = this->data["data_genuine_TightIso_dm" + name];
    auto mc = this->data["mc_genuine_TightIso_dm" + name];

    double eff_data = crystalballEfficiency(x, dt["alpha"], dt["m_{0}"], dt["sigma"], dt["norm"], dt["n"]);
    double eff_mc = crystalballEfficiency(x, mc["alpha"], mc["m_{0}"], mc["sigma"], mc["norm"], mc["n"]);
    return eff_data / eff_mc;
}

double tauSF::crystalballEfficiency(double m, double alpha, double m0, double sigma, double norm, double n) {
    auto sqrtPiOver2 = TMath::Sqrt(TMath::PiOver2());
    auto sqrt2 = TMath::Sqrt(2.);
    auto sig = TMath::Abs(sigma);
    auto t = (m - m0) / sig * alpha / TMath::Abs(alpha);
    auto absAlpha = TMath::Abs(alpha / sig);
    auto a = TMath::Power(n / absAlpha, n) * TMath::Exp(-0.5 * absAlpha * absAlpha);
    auto b = absAlpha - n / absAlpha;
    auto arg = absAlpha / sqrt2;

    auto ApproxErf(1.);
    if (arg > 5.) {
        ApproxErf = 1.;
    } else if (arg < -5.) {
        ApproxErf = -1.;
    } else {
        ApproxErf = TMath::Erf(arg);
    }

    auto leftArea = (1. + ApproxErf) * sqrtPiOver2;
    auto rightArea = (a * 1. / TMath::Power(absAlpha - b, n - 1)) / (n - 1);
    auto area = leftArea + rightArea;

    if (t <= absAlpha) {
        arg = t / sqrt2;
        if (arg > 5.) {
            ApproxErf = 1.;
        } else if (arg < -5.) {
            ApproxErf = -1.;
        } else {
            ApproxErf = TMath::Erf(arg);
        }
        return norm * (1 + ApproxErf) * sqrtPiOver2 / area;
    }
    return norm * (leftArea + a * (1 / TMath::Power(t - b, n - 1) - 1 / TMath::Power(absAlpha - b, n - 1)) / (1 - n)) / area;
}

double tauSF::tauID_SF(int match, double eta) {
    double sf(1.);
    eta = std::abs(eta);
    if (match == 1 || match == 3) {
        if (eta < 1.460) {
            sf = 1.213;
        } else if (eta > 1.558) {
            sf = 1.375;
        }
    } else if (match == 2 || match == 4) {  // mu->tau fakes Loose
        if (eta < 0.4) {
            sf = 1.010;
        } else if (eta < 0.8) {
            sf = 1.007;
        } else if (eta < 1.2) {
            sf = 0.870;
        } else if (eta < 1.7) {
            sf = 1.154;
        } else {
            sf = 2.281;
        }
    }
    return sf;
}

double tauSF::boosted_ZmmSF(double pt_sv, std::string syst) {
    float sf(1.0);
    if (pt_sv <= 100) {
        sf = 0.973;
    } else if (pt_sv <= 170) {
        sf = 0.959;
    } else if (pt_sv <= 300) {
        sf = 0.934;
    } else {
        sf = 0.993;
    }

    if (syst == "ZmmSF_Up") {
        return ((1.0 + 2 * sf) / (1.0 + sf));
    } else if (syst == "ZmmSF_Down") {
        return (1.0 / (1.0 + sf));
    }
    return sf;
}

double tauSF::VBF_ZmmSF(double mjj, std::string syst) {
    float sf(1.0);
    if (mjj <= 300) {
        sf = (0.010 / 2.0) + 1.0;
    } else if (mjj <= 500) {
        sf = (0.064 / 2.0) + 1.0;
    } else if (mjj <= 800) {
        sf = (0.088 / 2.0) + 1.0;
    } else {
        sf = (0.003 / 2.0) + 1.0;
    }

    if (syst == "ZmmSF_Up") {
        return ((1.0 + 2 * sf) / (1.0 + sf));
    } else if (syst == "ZmmSF_Down") {
        return (1.0 / (1.0 + sf));
    }
    return sf;
}
#endif  // INCLUDE_TAUSF_H_
