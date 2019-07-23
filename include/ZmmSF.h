// Copyright [2018] Tyler Mitchell

#ifndef INCLUDE_ZMMSF_H_
#define INCLUDE_ZMMSF_H_

double GetZmmSF(float jets, float mj, float pthi, float taupt, float syst) {
    double aweight = 1.0;
    if (syst == 0) {
        if (jets >= 2 && mj > 300 && taupt > 40 && pthi > 50) {  // VBF
            if (mj >= 300 && mj < 700) aweight = 1.070;
            if (mj >= 700 && mj < 1100) aweight = 1.090;
            if (mj >= 1100 && mj < 1500) aweight = 1.055;
            if (mj >= 1500) aweight = 1.015;
        }
    }
    if (syst == 1) {
        if (jets >= 2 && mj > 300 && taupt > 40 && pthi > 50) {  // VBF
            if (mj >= 300 && mj < 700) aweight = 1.14;
            if (mj >= 700 && mj < 1100) aweight = 1.18;
            if (mj >= 1100 && mj < 1500) aweight = 1.11;
            if (mj >= 1500) aweight = 1.030;
        }
    }
    return aweight;
}

#endif  // INCLUDE_ZMMSF_H_
