// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_SAMPLE_H_
#define INCLUDE_SAMPLE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "./TemplateTool.h"

// Sample_Plots inherits from the TemplateTool, which contains basic information like
// ff systematic names, the acWeight map, and the names of the categories for
// the analysis. This is mainly used just to keep all of the long lists out of
// this file so it is easier to read.
class Sample_Plots : public TemplateTool {
 public:
    Sample_Plots(std::string, std::string, std::string, std::string, std::shared_ptr<TFile>, std::map<std::string, std::vector<float>>);
    void set_branches(std::shared_ptr<TTree>, std::string);
    void load_fake_fractions(std::string);
    void fill_histograms(std::shared_ptr<TTree>, std::string);
    std::string get_category(double);
    void convert_data_to_fake(std::string, double, std::string);
    void write_histograms();
    void set_variable(std::shared_ptr<TTree>);
    Float_t get_var(std::string);

 private:
    std::string sample_name, in_var_name;
    std::map<std::string, TH1F *> fakes, hists;
    std::map<std::string, TH2F *> fake_fractions;
    std::vector<std::string> plot_variables;
    std::map<std::string, std::map<std::string, TH1F *>> all_fakes, all_hists;

    // get variables from file
    Int_t is_signal, is_antiTauIso, OS;                                      // flags
    Float_t weight, acWeightVal;                                             // weights
    Float_t lep_iso, mjj, t1_pt, vis_mass, mt, t1_decayMode, njets, nbjets;  // for fake factor
    Float_t D0_ggH;                                                          // 3D separation
    std::map<std::string, Float_t> variables;                                // all variables
};

// Sample_Plots constructor. Loads all of the basic information from the parent TemplateTool class.
// Empty histograms for fake jets and for normal samples are created and stored in the maps:
// fakes and hists. The category is used as the key for the maps. To get the
// "mt_vbf_ggHMELA_bin1_NN_bin1" histogram use
//
//       hists.at("mt_vbf_ggHMELA_bin1_NN_bin1");
//
// this will give a pointer to the histogram. The fakes is accessed the exact same way, but
// is only available for data.
Sample_Plots::Sample_Plots(std::string channel_prefix, std::string year, std::string in_sample_name, std::string suffix,
                           std::shared_ptr<TFile> output_file, std::map<std::string, std::vector<float>> in_variables)
    : TemplateTool(channel_prefix, year, suffix, output_file), sample_name(in_sample_name), acWeightVal(1.) {
    for (auto it = in_variables.begin(); it != in_variables.end(); it++) {
        for (auto cat : categories) {
            fout->cd(("plots/" + it->first + "/" + cat).c_str());

            // convert data name to be appropriate and do ff things
            if (sample_name.find("Data") != std::string::npos || sample_name == "data_obs") {
                sample_name = "data_obs";
                fakes[cat] = new TH1F(("fake_" + cat).c_str(), "fake_SS", it->second.at(0), it->second.at(1), it->second.at(2));
            }

            // create output histograms
            hists[cat] = new TH1F(sample_name.c_str(), sample_name.c_str(), it->second.at(0), it->second.at(1), it->second.at(2));
        }
        fout->cd();
        plot_variables.push_back(it->first);
        all_fakes[it->first] = fakes;
        all_hists[it->first] = hists;
    }
}

Float_t Sample_Plots::get_var(std::string var) {
    if (var == "t1_decayMode") {
        return t1_decayMode;
    } else if (var == "vis_mass") {
        return vis_mass;
    } else if (var == "mjj") {
        return mjj;
    } else if (var == "mt") {
        return mt;
    } else if (var == "mu_iso" || var == "el_iso") {
        return lep_iso;
    } else if (var == "njets") {
        return njets;
    } else if (var == "D0_ggH") {
        return D0_ggH;
    } else if (var == "t1_pt") {
        return t1_pt;
    } else {
        return variables.at(var);
    }
}

// fill_histograms does the main work. It will open the given file and loop
// through all events making selections and filling histograms. Additionally,
// if the Sample_Plots is for data, it will fill the fake jets histogram. If "doSyst"
// is true, fill_histograms will also do all the jet fakes systematics shifts,
// provided the Sample_Plots is for data. Passing "acWeight" allows you to reweight
// JHU samples to different coupling scenarios.
void Sample_Plots::fill_histograms(std::shared_ptr<TTree> tree, std::string acWeight = "None") {
    set_branches(tree, acWeight);
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

        cat0 = (njets == 0);
        cat1 = (njets == 1 || (njets > 1 && (mjj < 300)));
        cat2 = (njets > 1 && mjj > 300);

        // find the correct MELA ggH/Higgs pT bin for this event
        auto ACcat = get_category(D0_ggH);
        for (auto var : plot_variables) {
            // fill histograms
            if (is_signal) {
                all_hists.at(var).at(channel_prefix + "_inclusive")->Fill(get_var(var), weight);
                if (cat0) {
                    all_hists.at(var).at(channel_prefix + "_0jet")->Fill(get_var(var), weight);
                } else if (cat1) {
                    all_hists.at(var).at(channel_prefix + "_boosted")->Fill(get_var(var), weight);
                } else if (cat2) {
                    all_hists.at(var).at(channel_prefix + "_vbf")->Fill(get_var(var), weight);
                    if (ACcat != "skip") {
                        all_hists.at(var).at(ACcat)->Fill(get_var(var), weight);
                    }
                }
            } else if (is_antiTauIso && sample_name == "data_obs") {
                convert_data_to_fake(channel_prefix + "_inclusive", get_var(var), var);
                if (cat0) {
                    // category, name, var1, var2, vis_mass, njets, t1_pt, t1_decayMode, mt, lep_iso, evtwt
                    convert_data_to_fake(channel_prefix + "_0jet", get_var(var), var);  // 2d template
                } else if (cat1) {
                    convert_data_to_fake(channel_prefix + "_boosted", get_var(var), var);
                } else if (cat2) {
                    convert_data_to_fake(channel_prefix + "_vbf", get_var(var), var);
                    if (ACcat != "skip") {
                        convert_data_to_fake(ACcat, get_var(var), var);
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
void Sample_Plots::convert_data_to_fake(std::string cat, double var1, std::string j) {
    // fake fractions only for 0jet, boosted, VBF
    std::string get_cat = cat;
    if (cat.find("vbf_ggHMELA") != std::string::npos) {
        get_cat = channel_prefix + "_vbf";
    }

    fout->cd();
    auto bin_x = fake_fractions.at(get_cat + "_data")->GetXaxis()->FindBin(vis_mass);
    auto bin_y = fake_fractions.at(get_cat + "_data")->GetYaxis()->FindBin(njets);
    double fakeweight;
    fakeweight =
        ff_weight->value({t1_pt, t1_decayMode, njets, vis_mass, mt, lep_iso, fake_fractions.at(get_cat + "_frac_w")->GetBinContent(bin_x, bin_y),
                          fake_fractions.at(get_cat + "_frac_tt")->GetBinContent(bin_x, bin_y),
                          fake_fractions.at(get_cat + "_frac_qcd")->GetBinContent(bin_x, bin_y)});
    all_fakes.at(j).at(cat)->Fill(var1, weight * fakeweight);
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
void Sample_Plots::load_fake_fractions(std::string file_name) {
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
std::string Sample_Plots::get_category(double vbf_var3) {
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
// histogram. Additionally, if this Sample_Plots is for data, write the fake factor
// histograms in the same directory.
void Sample_Plots::write_histograms() {
    auto i = 0;
    for (auto var : plot_variables) {
        for (auto cat : categories) {
            fout->cd(("plots/" + var + "/" + cat).c_str());
            all_hists.at(var).at(cat)->Write();
            if (sample_name == "data_obs") {
                auto hist = all_fakes.at(var).at(cat);
                hist->SetName("jetFakes");
                hist->Write();
            }
        }
        i++;
    }
}

// set_branches takes an input TTree and sets all of the needed branch
// addresses to the appropriate member variables. Provided an acWeight,
// it will also load the acWeight stored in the root file for reweighting
// JHU samples.
void Sample_Plots::set_branches(std::shared_ptr<TTree> tree, std::string acWeight) {
    std::string iso;
    std::string tree_name = tree->GetName();
    if (tree_name.find("etau_tree") != std::string::npos) {
        iso = "el_iso";
    } else if (tree_name.find("mutau_tree") != std::string::npos) {
        iso = "mu_iso";
    }

    // now make the map
    variables = {{"el_pt", 0},     {"el_eta", 0},      {"el_phi", 0},    {"el_mass", 0},   {"mu_pt", 0},        {"mu_eta", 0},     {"mu_phi", 0},
                 {"mu_mass", 0},   {"t1_eta", 0},      {"t1_phi", 0},    {"t1_mass", 0},   {"t1_iso", 0},       {"j1_pt", 0},      {"j1_eta", 0},
                 {"j1_phi", 0},    {"j2_pt", 0},       {"j2_eta", 0},    {"j2_phi", 0},    {"b1_pt", 0},        {"b1_eta", 0},     {"b1_phi", 0},
                 {"b2_pt", 0},     {"b2_eta", 0},      {"b2_phi", 0},    {"met", 0},       {"metphi", 0},       {"numGenJets", 0}, {"pt_sv", 0},
                 {"m_sv", 0},      {"Dbkg_VBF", 0},    {"Dbkg_ggH", 0},  {"D0_VBF", 0},    {"DCP_VBF", 0},      {"D0_ggH", 0},     {"DCP_ggH", 0},
                 {"Phi", 0},       {"Phi1", 0},        {"costheta1", 0}, {"costheta2", 0}, {"costhetastar", 0}, {"Q2V1", 0},       {"Q2V2", 0},
                 {"ME_sm_VBF", 0}, {"ME_sm_ggH", 0},   {"ME_sm_WH", 0},  {"ME_sm_ZH", 0},  {"ME_bkg", 0},       {"ME_bkg1", 0},    {"ME_bkg2", 0},
                 {"VBF_MELA", 0},  {"higgs_pT", 0},    {"higgs_m", 0},   {"hjj_pT", 0},    {"hjj_m", 0},        {"dEtajj", 0},     {"dPhijj", 0},
                 {"MT_lepMET", 0}, {"MT_HiggsMET", 0}, {"hj_dphi", 0},   {"jmet_dphi", 0}, {"MT_t2MET", 0},     {"hj_deta", 0},    {"hmet_dphi", 0},
                 {"hj_dr", 0},     {"lt_dphi", 0},     {"NN_disc", 0},   {"MELA_D2j", 0},  {"trigger", 0}};

    tree->SetBranchAddress("evtwt", &weight);
    tree->SetBranchAddress("t1_decayMode", &t1_decayMode);
    tree->SetBranchAddress("vis_mass", &vis_mass);
    tree->SetBranchAddress("mjj", &mjj);
    tree->SetBranchAddress("mt", &mt);
    tree->SetBranchAddress(iso.c_str(), &lep_iso);
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("nbjets", &nbjets);
    tree->SetBranchAddress("D0_ggH", &D0_ggH);
    tree->SetBranchAddress("is_signal", &is_signal);
    tree->SetBranchAddress("is_antiTauIso", &is_antiTauIso);
    tree->SetBranchAddress("OS", &OS);
    tree->SetBranchAddress("t1_pt", &t1_pt);
    tree->SetBranchAddress("el_pt", &variables.at("el_pt"));
    tree->SetBranchAddress("el_eta", &variables.at("el_eta"));
    tree->SetBranchAddress("el_phi", &variables.at("el_phi"));
    tree->SetBranchAddress("el_mass", &variables.at("el_mass"));
    tree->SetBranchAddress("mu_pt", &variables.at("mu_pt"));
    tree->SetBranchAddress("mu_eta", &variables.at("mu_eta"));
    tree->SetBranchAddress("mu_phi", &variables.at("mu_phi"));
    tree->SetBranchAddress("mu_mass", &variables.at("mu_mass"));
    tree->SetBranchAddress("t1_eta", &variables.at("t1_eta"));
    tree->SetBranchAddress("t1_phi", &variables.at("t1_phi"));
    tree->SetBranchAddress("t1_mass", &variables.at("t1_mass"));
    tree->SetBranchAddress("t1_iso", &variables.at("t1_iso"));
    tree->SetBranchAddress("j1_pt", &variables.at("j1_pt"));
    tree->SetBranchAddress("j1_eta", &variables.at("j1_eta"));
    tree->SetBranchAddress("j1_phi", &variables.at("j1_phi"));
    tree->SetBranchAddress("j2_pt", &variables.at("j2_pt"));
    tree->SetBranchAddress("j2_eta", &variables.at("j2_eta"));
    tree->SetBranchAddress("j2_phi", &variables.at("j2_phi"));
    tree->SetBranchAddress("b1_pt", &variables.at("b1_pt"));
    tree->SetBranchAddress("b1_eta", &variables.at("b1_eta"));
    tree->SetBranchAddress("b1_phi", &variables.at("b1_phi"));
    tree->SetBranchAddress("b2_pt", &variables.at("b2_pt"));
    tree->SetBranchAddress("b2_eta", &variables.at("b2_eta"));
    tree->SetBranchAddress("b2_phi", &variables.at("b2_phi"));
    tree->SetBranchAddress("met", &variables.at("met"));
    tree->SetBranchAddress("metphi", &variables.at("metphi"));
    tree->SetBranchAddress("numGenJets", &variables.at("numGenJets"));
    tree->SetBranchAddress("pt_sv", &variables.at("pt_sv"));
    tree->SetBranchAddress("m_sv", &variables.at("m_sv"));
    tree->SetBranchAddress("Dbkg_VBF", &variables.at("Dbkg_VBF"));
    tree->SetBranchAddress("Dbkg_ggH", &variables.at("Dbkg_ggH"));
    tree->SetBranchAddress("D0_VBF", &variables.at("D0_VBF"));
    tree->SetBranchAddress("MELA_D2j", &variables.at("MELA_D2j"));
    tree->SetBranchAddress("DCP_VBF", &variables.at("DCP_VBF"));
    tree->SetBranchAddress("DCP_ggH", &variables.at("DCP_ggH"));
    tree->SetBranchAddress("Phi", &variables.at("Phi"));
    tree->SetBranchAddress("Phi1", &variables.at("Phi1"));
    tree->SetBranchAddress("costheta1", &variables.at("costheta1"));
    tree->SetBranchAddress("costheta2", &variables.at("costheta2"));
    tree->SetBranchAddress("costhetastar", &variables.at("costhetastar"));
    tree->SetBranchAddress("Q2V1", &variables.at("Q2V1"));
    tree->SetBranchAddress("Q2V2", &variables.at("Q2V2"));
    tree->SetBranchAddress("ME_sm_VBF", &variables.at("ME_sm_VBF"));
    tree->SetBranchAddress("ME_sm_ggH", &variables.at("ME_sm_ggH"));
    tree->SetBranchAddress("ME_sm_WH", &variables.at("ME_sm_WH"));
    tree->SetBranchAddress("ME_sm_ZH", &variables.at("ME_sm_ZH"));
    tree->SetBranchAddress("ME_bkg", &variables.at("ME_bkg"));
    tree->SetBranchAddress("ME_bkg1", &variables.at("ME_bkg1"));
    tree->SetBranchAddress("ME_bkg2", &variables.at("ME_bkg2"));
    tree->SetBranchAddress("VBF_MELA", &variables.at("VBF_MELA"));
    tree->SetBranchAddress("higgs_pT", &variables.at("higgs_pT"));
    tree->SetBranchAddress("higgs_m", &variables.at("higgs_m"));
    tree->SetBranchAddress("hjj_pT", &variables.at("hjj_pT"));
    tree->SetBranchAddress("hjj_m", &variables.at("hjj_m"));
    tree->SetBranchAddress("dEtajj", &variables.at("dEtajj"));
    tree->SetBranchAddress("dPhijj", &variables.at("dPhijj"));
    tree->SetBranchAddress("MT_lepMET", &variables.at("MT_lepMET"));
    tree->SetBranchAddress("MT_HiggsMET", &variables.at("MT_HiggsMET"));
    tree->SetBranchAddress("hj_dphi", &variables.at("hj_dphi"));
    tree->SetBranchAddress("jmet_dphi", &variables.at("jmet_dphi"));
    tree->SetBranchAddress("MT_t2MET", &variables.at("MT_t2MET"));
    tree->SetBranchAddress("hj_deta", &variables.at("hj_deta"));
    tree->SetBranchAddress("hmet_dphi", &variables.at("hmet_dphi"));
    tree->SetBranchAddress("hj_dr", &variables.at("hj_dr"));
    tree->SetBranchAddress("lt_dphi", &variables.at("lt_dphi"));
    tree->SetBranchAddress("NN_disc", &variables.at("NN_disc"));
    tree->SetBranchAddress("trigger", &variables.at("trigger"));
    if (acWeight != "None") {
        tree->SetBranchAddress(acWeight.c_str(), &acWeightVal);
    }
}

#endif  // INCLUDE_PRODUCE_PLOTS_H_
