// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_PRODUCE_TEMPLATES_H_
#define INCLUDE_PRODUCE_TEMPLATES_H_

#include <map>
#include <string>
#include <vector>
#include "./TemplateTool.h"

// Sample_Template inherits from the TemplateTool, which contains basic information like
// ff systematic names, the acWeight map, and the names of the categories for
// the analysis. This is mainly used just to keep all of the long lists out of
// this file so it is easier to read.
class Sample_Template : public TemplateTool {
 public:
    Sample_Template(std::string, std::string, std::string, std::string, std::shared_ptr<TFile>, std::string);
    void set_branches(std::shared_ptr<TTree>, std::string);
    void load_fake_fractions(std::string);
    void init_systematics();
    void fill_histograms(std::shared_ptr<TTree>, bool, std::string);
    std::string get_category(double);
    void convert_data_to_fake(std::string, double, double, int);
    void write_histograms(bool, std::string);

 private:
    std::string sample_name;
    std::map<std::string, TH2F *> fakes_2d, hists_2d;
    std::map<std::string, TH2F *> fake_fractions;
    std::map<std::string, std::vector<TH2F *>> FF_systs;
    std::vector<Float_t> bins_l2, bins_hpt, bins_vbf_var1, bins_lpt, bins_msv1, bins_vbf_var2, bins_hpt2;

    // get variables from file
    Int_t is_signal, is_antiTauIso, OS;                                                           // flags
    Float_t nbjets, njets, mjj;                                                                   // selection
    Float_t weight, acWeightVal;                                                                  // weights
    Float_t lep_iso, mt, t1_decayMode, vis_mass, t1_pt;                                           // for fake factor
    Float_t higgs_pT, m_sv, NN_disc, D0_VBF, D0_ggH, DCP_VBF, DCP_ggH, VBF_MELA, j1_phi, j2_phi;  // observables
};

// Sample_Template constructor. Loads all of the basic information from the parent TemplateTool class.
// Empty histograms for fake jets and for normal samples are created and stored in the maps:
// fakes_2d and hists_2d. The category is used as the key for the maps. To get the
// "mt_vbf_ggHMELA_bin1_NN_bin1" histogram use
//
//       hists_2d.at("mt_vbf_ggHMELA_bin1_NN_bin1");
//
// this will give a pointer to the histogram. The fakes_2d is accessed the exact same way, but
// is only available for data. The FF_syst map is also created for the fake fraction systematics,
// but you must call init_systematics to actually use it.
Sample_Template::Sample_Template(std::string channel_prefix, std::string year, std::string in_sample_name, std::string suffix,
                                 std::shared_ptr<TFile> output_file, std::string ext = "")
    : TemplateTool(channel_prefix, year, suffix, output_file),
      sample_name(in_sample_name),
      acWeightVal(1.),
      // x-axis
      bins_l2{0, 1, 10, 11},
      bins_hpt{0, 100, 150, 200, 250, 300, 5000},
      bins_vbf_var1{300, 500, 10000},  // real mjj
      // bins_vbf_var1{0, 0.25, 0.5, 0.75, 1.},  // actually VBF MELA

      // y-axis
      bins_lpt{0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400},
      bins_msv1{0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300},
      bins_vbf_var2{0, 80, 100, 115, 130, 150, 1000} {
    // bins_vbf_var2{-5, -1.25, -0.75, 0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2., 3.},  // Fisher Disc
    // bins_vbf_var2{0, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.5,  0.6, 0.65, 0.7, 0.8},  // Perceptron
    // bins_vbf_var2{0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.7, 0.8, 0.9, 1.},  // NN including m_sv et2016/mt2017
    // bins_vbf_var2{0, 0.05, 0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.7, 0.8, 0.9, 0.95, 1.}, // mt2016
    // bins_vbf_var2{0, 0.25, 0.5, 0.75, 1.} {
    for (auto cat : categories) {
        fout->cd(cat.c_str());

        // convert data name to be appropriate and do ff things
        if (sample_name.find("Data") != std::string::npos || sample_name == "data_obs") {
            sample_name = "data_obs";
            // also push the fake factor histograms only for Data
            if (cat == channel_prefix + "_0jet") {
                fakes_2d[cat] = new TH2F("fake_0jet", "fake_SS", bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1, &bins_lpt[0]);
            } else if (cat == channel_prefix + "_boosted") {
                fakes_2d[cat] = new TH2F("fake_boosted", "fake_SS", bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1, &bins_msv1[0]);
            } else {
                fakes_2d[cat] = new TH2F(("fake_" + cat).c_str(), "fake_SS", bins_vbf_var1.size() - 1, &bins_vbf_var1[0], bins_vbf_var2.size() - 1,
                                         &bins_vbf_var2[0]);
            }

            // histograms for fake-factor are always 2d
            FF_systs[cat.c_str()] = std::vector<TH2F *>();
        }

        // push empty histograms into map to be filled
        if (cat == channel_prefix + "_0jet") {
            hists_2d[cat] = new TH2F((sample_name + ext).c_str(), (sample_name + ext).c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1,
                                     &bins_lpt[0]);
        } else if (cat == channel_prefix + "_boosted") {
            hists_2d[cat] = new TH2F((sample_name + ext).c_str(), (sample_name + ext).c_str(), bins_hpt.size() - 1, &bins_hpt[0],
                                     bins_msv1.size() - 1, &bins_msv1[0]);
        } else if (cat.find("_vbf") != std::string::npos) {
            hists_2d[cat] = new TH2F((sample_name + ext).c_str(), (sample_name + ext).c_str(), bins_vbf_var1.size() - 1, &bins_vbf_var1[0],
                                     bins_vbf_var2.size() - 1, &bins_vbf_var2[0]);
        }
    }

    fout->cd();
}

// init_systematics creates empty histograms for all fake factor systematics shifts.
// This is only done when the Sample_Template is data. The shifts are stored in a map where
// the category is the key. For each entry in the map, there is a vector containing
// all of the shifts in the order from the systematics member variable. For example,
// to get the "ff_qcd_syst_down" shift for the "et_boosted" category use
//
//       FF_systs.at("et_boosted").at(1);
//
// Use the index 1 beacuse "ff_qcd_syst_down" is the second item in the systematics
// vector.
void Sample_Template::init_systematics() {
    // systematics still only need the data histograms
    if (sample_name == "Data" || sample_name == "data_obs") {
        for (auto key : FF_systs) {
            fout->cd(key.first.c_str());
            std::string name = "jetFakes_";
            for (auto syst : systematics) {
                if (key.first == channel_prefix + "_0jet") {
                    FF_systs.at(key.first.c_str())
                        .push_back(new TH2F((name + "CMS_htt_" + syst).c_str(), name.c_str(), bins_l2.size() - 1, &bins_l2[0], bins_lpt.size() - 1,
                                            &bins_lpt[0]));
                } else if (key.first == channel_prefix + "_boosted") {
                    FF_systs.at(key.first.c_str())
                        .push_back(new TH2F((name + "CMS_htt_" + syst).c_str(), name.c_str(), bins_hpt.size() - 1, &bins_hpt[0], bins_msv1.size() - 1,
                                            &bins_msv1[0]));
                } else if (key.first.find("_vbf") != std::string::npos) {
                    FF_systs.at(key.first.c_str())
                        .push_back(new TH2F((name + "CMS_htt_" + syst).c_str(), name.c_str(), bins_vbf_var1.size() - 1, &bins_vbf_var1[0],
                                            bins_vbf_var2.size() - 1, &bins_vbf_var2[0]));
                }
            }
        }
        std::cout << "initialized systematics" << std::endl;
    }
}

// set_branches takes an input TTree and sets all of the needed branch
// addresses to the appropriate member variables. Provided an acWeight,
// it will also load the acWeight stored in the root file for reweighting
// JHU samples.
void Sample_Template::set_branches(std::shared_ptr<TTree> tree, std::string acWeight) {
    std::string iso;
    std::string tree_name = tree->GetName();
    if (tree_name.find("etau_tree") != std::string::npos) {
        iso = "el_iso";
    } else if (tree_name.find("mutau_tree") != std::string::npos) {
        iso = "mu_iso";
    }

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("higgs_pT", &higgs_pT);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress(iso.c_str(), &lep_iso);
    tree->SetBranchAddress("m_sv", &m_sv);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("D0_VBF", &D0_VBF);
    tree->SetBranchAddress("D0_ggH", &D0_ggH);
    tree->SetBranchAddress("DCP_VBF", &DCP_VBF);
    tree->SetBranchAddress("DCP_ggH", &DCP_ggH);
    tree->SetBranchAddress("is_signal", &is_signal);
    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("OS", &OS);
    tree->SetBranchAddress("t1_pt", &t1_pt);
    tree->SetBranchAddress("VBF_MELA", &VBF_MELA);
    // tree->SetBranchAddress("MELA_D2j", &VBF_MELA);
    tree->SetBranchAddress("j1_phi", &j1_phi);
    tree->SetBranchAddress("j2_phi", &j2_phi);
    tree->SetBranchAddress("NN_disc", &NN_disc);

    if (acWeight != "None") {
        tree->SetBranchAddress(acWeight.c_str(), &acWeightVal);
    } else {
        acWeightVal = 1.;
    }
}

// fill_histograms does the main work. It will open the given file and loop
// through all events making selections and filling histograms. Additionally,
// if the Sample_Template is for data, it will fill the fake jets histogram. If "doSyst"
// is true, fill_histograms will also do all the jet fakes systematics shifts,
// provided the Sample_Template is for data. Passing "acWeight" allows you to reweight
// JHU samples to different coupling scenarios.
void Sample_Template::fill_histograms(std::shared_ptr<TTree> tree, bool doSyst = false, std::string acWeight = "None") {
    set_branches(tree, acWeight);
    init_systematics();
    fout->cd();

    bool cat0(false), cat1(false), cat2(false);
    float vbf_var1(0.), vbf_var2(0.), vbf_var3(0.);

    // start the event loop
    for (auto i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        weight *= acWeightVal;  // acWeightVal = 1 for SM

        // only look at opposite-sign events
        if (OS == 0 || mt > 50 || nbjets > 0) {
            continue;
        }

        // choose observables
        vbf_var1 = VBF_MELA;
        vbf_var2 = NN_disc;
        vbf_var3 = D0_ggH;
        // vbf_var1 = mjj;
        // vbf_var3 = dPhijj;
        // vbf_var3 = TMath::ACos(TMath::Cos(j1_phi - j2_phi))
        vbf_var1 = mjj;
        vbf_var2 = m_sv;

        cat0 = (njets == 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300)));
        cat2 = (njets > 1 && mjj > 300);

        // find the correct MELA ggH/Higgs pT bin for this event
        auto ACcat = get_category(vbf_var3);

        // fill histograms
        if (is_signal) {
            if (cat0) {
                hists_2d.at(channel_prefix + "_0jet")->Fill(t1_decayMode, vis_mass, weight);
            } else if (cat1) {
                hists_2d.at(channel_prefix + "_boosted")->Fill(higgs_pT, m_sv, weight);
            } else if (cat2) {
                hists_2d.at(channel_prefix + "_vbf")->Fill(vbf_var1, vbf_var2, weight);
                if (ACcat != "skip") {
                    hists_2d.at(ACcat)->Fill(vbf_var1, vbf_var2, weight);
                }
            }
        } else if (is_antiTauIso && sample_name == "data_obs") {
            if (cat0) {
                // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
                convert_data_to_fake(channel_prefix + "_0jet", t1_decayMode, vis_mass, -1);  // 2d template
            } else if (cat1) {
                convert_data_to_fake(channel_prefix + "_boosted", higgs_pT, m_sv, -1);
            } else if (cat2) {
                if (vbf_var2 > -1) convert_data_to_fake(channel_prefix + "_vbf", vbf_var1, vbf_var2, -1);
                if (ACcat != "skip") {
                    convert_data_to_fake(ACcat, vbf_var1, vbf_var2, -1);
                }
            }

            // loop through all systematic names and get the corresponding weight to fill a histogram
            if (doSyst) {
                for (int i = 0; i < systematics.size(); i++) {
                    if (cat0) {
                        // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
                        convert_data_to_fake(channel_prefix + "_0jet", t1_decayMode, vis_mass, i);  // 2d template
                    } else if (cat1) {
                        convert_data_to_fake(channel_prefix + "_boosted", higgs_pT, m_sv, i);
                    } else if (cat2) {
                        convert_data_to_fake(channel_prefix + "_vbf", vbf_var1, vbf_var2, i);
                        if (ACcat != "skip") {
                            convert_data_to_fake(ACcat, vbf_var1, vbf_var2, i);
                        }
                    }
                }
            }
        }
    }
}

// convert_data_to_fake puts an event in the fake factor histogram. It takes the category for
// this event as well as the 2 input variables for the histogram. It then reads the weights
// from the fake fraction histograms that were loaded in load_fake_fractions. These weights
// are used with the input variables to fill the fake histogram with the correct weight. If
// the "syst" parameter is passed, read the weight for the provided systematic shift.
void Sample_Template::convert_data_to_fake(std::string cat, double var1, double var2, int syst = -1) {
    // fake fractions only for 0jet, boosted, VBF
    std::string get_cat = cat;
    if (cat.find("vbf_ggHMELA") != std::string::npos) {
        get_cat = channel_prefix + "_vbf";
    }

    fout->cd();
    auto bin_x = fake_fractions.at(get_cat + "_data")->GetXaxis()->FindBin(vis_mass);
    auto bin_y = fake_fractions.at(get_cat + "_data")->GetYaxis()->FindBin(njets);
    double fakeweight;
    if (syst == -1) {
        fakeweight =
            ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso, fake_fractions.at(get_cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                              fake_fractions.at(get_cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                              fake_fractions.at(get_cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)});
        fakes_2d.at(cat)->Fill(var1, var2, weight * fakeweight);
    } else {
        fakeweight =
            ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso, fake_fractions.at(get_cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                              fake_fractions.at(get_cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                              fake_fractions.at(get_cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)},
                             systematics.at(syst));
        FF_systs.at(cat).at(syst)->Fill(var1, var2, weight * fakeweight);
    }
}

// load_fake_fractions opens the given file and reads the
// pre-computed fake fraction histograms for each category.
// The histograms are stored in a map with the key being
// the category name + the fraction name. For example, to get
// the W+jets fraction for category "et_vbf" use
//
//       fake_fractions.at("et_vbf_frac_w");
//
// this will return a pointer to the histogram.
void Sample_Template::load_fake_fractions(std::string file_name) {
    auto ifile = new TFile(file_name.c_str(), "READ");
    fout->cd();
    for (auto cat : categories) {
        // fake fractions only for 0jet, boosted, VBF
        if (cat.find("vbf_ggHMELA") != std::string::npos) {
            continue;
        }

        fake_fractions[cat + "_data"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "data_" + cat).c_str())->Clone());
        fake_fractions[cat + "_frac_w"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_w_" + cat).c_str())->Clone());
        fake_fractions[cat + "_frac_tt"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_tt_" + cat).c_str())->Clone());
        fake_fractions[cat + "_frac_qcd"] = reinterpret_cast<TH2F *>(ifile->Get((cat + "/" + "frac_qcd_" + cat).c_str())->Clone());
    }
    ifile->Close();
}

// get_category will take a variable and return the category
// in which this variable belongs. This is to give us
// "3D histograms" for combine although we can really only
// make a 2D histogram. If for some reason the given parameter
// doesn't fit in any bins, return "skip" to be handled by the
// caller.
std::string Sample_Template::get_category(double vbf_var3) {
    double edge = 1. / 6.;
    if (vbf_var3 >= 0 && vbf_var3 <= 1. * edge) {
        return channel_prefix + "_vbf_ggHMELA_bin1";
    } else if (vbf_var3 <= 2. * edge) {
        return channel_prefix + "_vbf_ggHMELA_bin2";
    } else if (vbf_var3 <= 3. * edge) {
        return channel_prefix + "_vbf_ggHMELA_bin3";
    } else if (vbf_var3 <= 4. * edge) {
        return channel_prefix + "_vbf_ggHMELA_bin4";
    } else if (vbf_var3 <= 5. * edge) {
        return channel_prefix + "_vbf_ggHMELA_bin5";
    } else if (vbf_var3 <= 6. * edge) {
        return channel_prefix + "_vbf_ggHMELA_bin6";
    } else {
        return "skip";
    }
}

// write_histograms is used to write the histograms to the output root file.
// First, change into the correct directory for the category then write the
// histogram. Additionally, if this Sample_Template is for data, write the fake factor
// histograms in the same directory.
void Sample_Template::write_histograms(bool doSyst = false, std::string newName = "") {
    for (auto cat : categories) {
        if (cat.find("inclusive") != std::string::npos) {
            continue;
        }
        fout->cd(cat.c_str());

        // potentially change the name (for JHU)
        hists_2d.at(cat)->Write();
        if (sample_name == "data_obs") {
            auto hist = fakes_2d.at(cat);
            hist->SetName("jetFakes");
            hist->Write();
            if (doSyst) {
                for (auto ihist : FF_systs.at(cat)) {
                    ihist->Write();
                }
            }
        }
    }
}

#endif  // INCLUDE_PRODUCE_TEMPLATES_H_
