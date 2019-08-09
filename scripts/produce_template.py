import ROOT
import time
import numpy
import pandas
import uproot
from glob import glob
from array import array
from pprint import pprint

fake_factor_systematics = [
    "ff_qcd_syst_up", "ff_qcd_syst_down", "ff_qcd_dm0_njet0_stat_up", "ff_qcd_dm0_njet0_stat_down",
    "ff_qcd_dm0_njet1_stat_up", "ff_qcd_dm0_njet1_stat_down", "ff_qcd_dm1_njet0_stat_up", "ff_qcd_dm1_njet0_stat_down",
    "ff_qcd_dm1_njet1_stat_up", "ff_qcd_dm1_njet1_stat_down", "ff_w_syst_up", "ff_w_syst_down",
    "ff_w_dm0_njet0_stat_up", "ff_w_dm0_njet0_stat_down", "ff_w_dm0_njet1_stat_up", "ff_w_dm0_njet1_stat_down",
    "ff_w_dm1_njet0_stat_up", "ff_w_dm1_njet0_stat_down", "ff_w_dm1_njet1_stat_up", "ff_w_dm1_njet1_stat_down",
    "ff_tt_syst_up", "ff_tt_syst_down", "ff_tt_dm0_njet0_stat_up", "ff_tt_dm0_njet0_stat_down",
    "ff_tt_dm0_njet1_stat_up",  "ff_tt_dm0_njet1_stat_down", "ff_tt_dm1_njet0_stat_up",  "ff_tt_dm1_njet0_stat_down",
    "ff_tt_dm1_njet1_stat_up", "ff_tt_dm1_njet1_stat_down"]

categories = [
    "inclusive",         "0jet",
    "boosted",           "vbf",
    "vbf_ggHMELA_bin1",  "vbf_ggHMELA_bin2",
    "vbf_ggHMELA_bin3",  "vbf_ggHMELA_bin4",
    "vbf_ggHMELA_bin5",  "vbf_ggHMELA_bin6",
    "vbf_ggHMELA_bin7",  "vbf_ggHMELA_bin8",
    "vbf_ggHMELA_bin9",  "vbf_ggHMELA_bin10",
    "vbf_ggHMELA_bin11", "vbf_ggHMELA_bin12"
]

vbf_sub_cats = [
    "vbf_ggHMELA_bin1",  "vbf_ggHMELA_bin2",
    "vbf_ggHMELA_bin3",  "vbf_ggHMELA_bin4",
    "vbf_ggHMELA_bin5",  "vbf_ggHMELA_bin6",
]

ac_reweighting_map = {
    "ggh": [
        ("wt_ggh_a1", "JHU_GGH2Jets_sm_M125"), ("wt_ggh_a3", "JHU_GGH2Jets_pseudoscalar_M125"),
        ("wt_ggh_a3int", "JHU_GGH2Jets_pseudoscalar_Mf05ph0125")
    ],
    "wh": [
        ("wt_wh_a1", "reweighted_WH_htt_0PM125"), ("wt_wh_a2", "reweighted_WH_htt_0PH125"),
        ("wt_wh_a2int", "reweighted_WH_htt_0PHf05ph0125"), ("wt_wh_a3", "reweighted_WH_htt_0M125"),
        ("wt_wh_a3int", "reweighted_WH_htt_0Mf05ph0125"), ("wt_wh_L1", "reweighted_WH_htt_0L1125"),
        ("wt_wh_L1int", "reweighted_WH_htt_0L1f05ph0125"), ("wt_wh_L1Zg", "reweighted_WH_htt_0L1Zg125"),
        ("wt_wh_L1Zgint", "reweighted_WH_htt_0L1Zgf05ph0125")
    ],
    "zh": [
        ("wt_zh_a1", "reweighted_ZH_htt_0PM125"), ("wt_zh_a2", "reweighted_ZH_htt_0PH125"),
        ("wt_zh_a2int", "reweighted_ZH_htt_0PHf05ph0125"), ("wt_zh_a3", "reweighted_ZH_htt_0M125"),
        ("wt_zh_a3int", "reweighted_ZH_htt_0Mf05ph0125"), ("wt_zh_L1", "reweighted_ZH_htt_0L1125"),
        ("wt_zh_L1int", "reweighted_ZH_htt_0L1f05ph0125"), ("wt_zh_L1Zg", "reweighted_ZH_htt_0L1Zg125"),
        ("wt_zh_L1Zgint", "reweighted_ZH_htt_0L1Zgf05ph0125")],
    "vbf": [
        ("wt_vbf_a1", "reweighted_qqH_htt_0PM125"), ("wt_vbf_a2", "reweighted_qqH_htt_0PH125"),
        ("wt_vbf_a2int", "reweighted_qqH_htt_0PHf05ph0125"), ("wt_vbf_a3", "reweighted_qqH_htt_0M125"),
        ("wt_vbf_a3int", "reweighted_qqH_htt_0Mf05ph0125"), ("wt_vbf_L1", "reweighted_qqH_htt_0L1125"),
        ("wt_vbf_L1int", "reweighted_qqH_htt_0L1f05ph0125"), ("wt_vbf_L1Zg", "reweighted_qqH_htt_0L1Zg125"),
        ("wt_vbf_L1Zgint", "reweighted_qqH_htt_0L1Zgf05ph0125"),
    ]
}

decay_mode_bins = [0, 1, 10, 11]
higgs_pT_bins_boost = [0, 100, 150, 200, 250, 300, 5000]
mjj_bins = [300, 500, 10000]
vis_mass_bins = [0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400]
m_sv_bins_boost = [0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300]
m_sv_bins_vbf = [0, 80, 100, 115, 130, 150, 1000]


syst_name_map = {
    "_UncMet_Up": "_CMS_scale_met_unclustered_13TeVUp",
    "_UncMet_Down": "_CMS_scale_met_unclustered_13TeVDown",
    "_ClusteredMet_Up": "_CMS_scale_met_clustered_13TeVUp",
    "_ClusteredMet_Down": "_CMS_scale_met_clustered_13TeVDown",
    "_vbfMass_JetTotalUp": "_CMS_scale_jm_13TeVUp",
    "_jetVeto30_JetTotalUp": "_CMS_scale_jn_13TeVUp",
    "_vbfMass_JetTotalDown": "_CMS_scale_jm_13TeVDown",
    "_jetVeto30_JetTotalDown": "_CMS_scale_jn_13TeVDown",
    "_ttbarShape_Up": "_CMS_htt_ttbarShape_13TeVUp",
    "_ttbarShape_Down": "_CMS_htt_ttbarShape_13TeVDown",
    "_Up": "_CMS_scale_t_allprong_13TeVUp",
    "_Down": "_CMS_scale_t_allprong_13TeVDown",
    "_DM0_Up": "_CMS_scale_t_1prong_13TeVUp",
    "_DM0_Down": "_CMS_scale_t_1prong_13TeVDown",
    "_DM1_Up": "_CMS_scale_t_1prong1pizero_13TeVUp",
    "_DM1_Down": "_CMS_scale_t_1prong1pizero_13TeVDown",
    "_DM10_Up": "_CMS_scale_t_3prong_13TeVUp",
    "_DM10_Down": "_CMS_scale_t_3prong_13TeVDown",
    "_jetToTauFake_Up": "_CMS_htt_jetToTauFake_13TeVUp",
    "_jetToTauFake_Down": "_CMS_htt_jetToTauFake_13TeVDown",
    "_dyShape_Up": "_CMS_htt_dyShape_13TeVUp",
    "_dyShape_Down": "_CMS_htt_dyShape_13TeVDown",
    "_zmumuShape_Up": "_CMS_htt_zmumuShape_VBF_13TeVUp",
    "_zmumuShape_Down": "_CMS_htt_zmumuShape_VBF_13TeVDown",
    "_JetTotalDown": "_CMS_htt_tempjetotaldown_VBF_13TeVDown",
    "_JetTotalUp": "_CMS_htt_tempjetotalup_VBF_13TeVDown",
    "": ""
}


def build_histogram(name, x_bins, y_bins):
    return ROOT.TH2F(name, name, len(x_bins) - 1, array('d', x_bins), len(y_bins) - 1, array('d', y_bins))


def fill_hist(data, xvar, yvar, hist):
    evtwt = data['evtwt'].values
    xvar = data[xvar].values
    yvar = data[yvar].values
    for i in xrange(len(data.index)):
        hist.Fill(xvar[i], yvar[i], evtwt[i])
    return hist


def fill_vbf_subcat_hists(data, xvar, yvar, zvar, hists):
    evtwt = data['evtwt'].values
    xvar = data[xvar].values
    yvar = data[yvar].values
    zvar = data[zvar].values
    edges = [i/6. for i in range(1, 7)]
    for i in xrange(len(data.index)):
        for j, edge in enumerate(edges):
            if zvar[i] < edge:
                hists[j-1].Fill(xvar[i], yvar[i], evtwt[i])
                break

    return hists


def fill_fake_hist(data, xvar, yvar, hist, fake_fractions, fake_weights, syst=None, local=False):
    # get event data
    columns = data[['evtwt', xvar, yvar, 't1_pt', 't1_decayMode', 'njets', 'vis_mass', 'mt', 'mu_iso']].values
    evtwt, xvar, yvar = columns[:, 0], columns[:, 1], columns[:, 2]
    t1_pt, t1_decayMode, njets = columns[:, 3], columns[:, 4], columns[:, 5]
    vis_mass, mt, iso = columns[:, 6], columns[:, 7], columns[:, 8]

    # get fake fractions
    frac_data = fake_fractions[0]
    frac_qcd = fake_fractions[1]
    # frac_real = fake_fractions[2]
    frac_tt = fake_fractions[3]
    frac_w = fake_fractions[4]

    for i in xrange(len(data.index)):
        xbin, ybin = -1, -1
        for x, bin in enumerate(frac_data.bins[0]):
            if vis_mass[i] < bin[1] and vis_mass[i] > bin[0]:
                print 'found {} at {} between {} and {}'.format(vis_mass[i], x, bin[0], bin[1])
                xbin = x
                break
        for y, bin in enumerate(frac_data.bins[1]):
            if njets[i] < bin[1] and njets[i] > bin[0]:
                print 'found {} at {} between {} and {}'.format(njets[i], y, bin[0], bin[1])
                ybin = y
                break

        print xbin, ybin
        print frac_qcd.values
        # make fake-weight input
        inputs = [
            t1_pt[i], t1_decayMode[i], njets[i], vis_mass[i], mt[i], iso[i],
            frac_qcd.values[ybin][xbin],
            frac_w.values[ybin][xbin],
            frac_tt.values[ybin][xbin]
        ]

        if local:
            fake_weight = 1.  # for testing the rest
        else:
            fake_weight = fake_weights.value(
                9, array('d', inputs)) if syst == None else fake_weights.value(9, array('d', inputs), syst)
        hist.Fill(xvar[i], yvar[i], evtwt[i] * fake_weight)
    return hist


def load_fake_factor_weights(fake_file):
    ff_file = ROOT.TFile(fake_file, 'READ')
    return ff_file.Get('ff_comb')


def load_fake_fractions(input_file):
    ifile = uproot.open(input_file)
    return {cat.replace(';1', ''): [ifile[cat][ihist] for ihist in sorted(ifile[cat].keys())] for cat in ifile.keys()}


def main(args):
    start = time.time()
    # channel_prefix = args.tree_name.replace('_tree', '')  # need prefix for things later
    channel_prefix = args.tree_name.replace('mutau_tree', 'mt')
    channel_prefix = channel_prefix.replace('etau_tree', 'et')
    assert channel_prefix == 'mt' or channel_prefix == 'et', 'must provide a valid tree name'
    files = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]  # get files to process

    # get things for output file name
    ztt_name = '_emb' if args.embed else '_ztt'
    syst_name = '_Sys' if args.syst else '_noSys'

    output_file = ROOT.TFile('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
                                                                                  ztt_name, syst_name, args.date, '_'+args.suffix), 'RECREATE')

    # create structure within output file
    for cat in categories:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
    output_file.cd()

    # Preload the fake fractions and fake factor weights.
    fake_fractions = load_fake_fractions(args.fake_file)

    if args.local:
        fake_weights = None
    else:
        fake_weights = load_fake_factor_weights(
            '../HTTutilities/Jet2TauFakes/data2017/SM2017/tight/vloose/mt/fakeFactors.root')

    # use this once uproot supports sub-directories inside root files
    # output_file = uproot.recreate('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
    #                                                                               ztt_name, syst_name, args.date, '_'+args.suffix))

    for ifile in files:
        name = ifile.replace('.root', '').split('/')[-1]
        print name
        input_file = uproot.open(ifile)
        trees = [ikey.replace(';1', '') for ikey in input_file.keys()
                 if 'tree' in ikey] if args.syst else [args.tree_name]
        for itree in trees:
            if itree != args.tree_name:
                name = ifile.replace('.root', '') + syst_name_map[itree.replace(args.tree_name, '')]
            else:
                name = ifile.replace('.root', '')

            events = input_file[itree].arrays([
                'is_signal', 'is_antiTauIso', 'OS', 'nbjets', 'njets', 'mjj', 'evtwt', 'wt_*',
                'mu_iso', 'el_iso', 't1_decayMode', 'vis_mass', 't1_pt', 'higgs_pT', 'm_sv',
                'D0_VBF', 'D0_ggH', 'DCP_VBF', 'DCP_ggH', 'j1_phi', 'j2_phi', 'mt', 'mu_pt', 'el_pt'
            ], outputtype=pandas.DataFrame)

            general_selection = events[
                (events['mt'] < 50) & (events['nbjets'] == 0)
            ]

            # do signal categorization
            signal_events = general_selection[general_selection['is_signal'] > 0]

            zero_jet_events = signal_events[signal_events['njets'] == 0]
            boosted_events = signal_events[
                (signal_events['njets'] == 1) |
                ((signal_events['njets'] > 1) & signal_events['mjj'] < 300)
            ]
            vbf_events = signal_events[(signal_events['njets'] > 1) & (signal_events['mjj'] > 300)]

            # start with 0-jet category
            output_file.cd('{}_0jet'.format(channel_prefix))
            zero_jet_hist = build_histogram(name.split('/')[-1], decay_mode_bins, vis_mass_bins)
            fill_hist(zero_jet_events, 't1_decayMode', 'vis_mass', zero_jet_hist)

            # now boosted category
            output_file.cd('{}_boosted'.format(channel_prefix))
            boost_hist = build_histogram(name.split('/')[-1], higgs_pT_bins_boost, m_sv_bins_boost)
            fill_hist(boosted_events, 'higgs_pT', 'm_sv', boost_hist)

            # vbf category is last
            output_file.cd('{}_vbf'.format(channel_prefix))
            vbf_hist = build_histogram(name.split('/')[-1], mjj_bins, m_sv_bins_vbf)
            fill_hist(vbf_events, 'mjj', 'm_sv', vbf_hist)

            # vbf sub-categories event after normal vbf categories
            vbf_cat_hists = []
            for cat in vbf_sub_cats:
                output_file.cd('{}_{}'.format(channel_prefix, cat))
                vbf_cat_hists.append(build_histogram(name.split('/')[-1], mjj_bins, m_sv_bins_vbf))
            fill_vbf_subcat_hists(vbf_events, 'mjj', 'm_sv', 'D0_ggH', vbf_cat_hists)

            # write then reset histograms
            output_file.Write()

            if '_JHU' in ifile:
                print ifile

            # do anti-iso categorization for fake-factor using data
            if 'data' in ifile.lower():
                print 'making fake factor hists'
                antiIso_events = general_selection[general_selection['is_antiTauIso'] > 0]
                fake_zero_jet_events = antiIso_events[antiIso_events['njets'] == 0]
                fake_boosted_events = antiIso_events[
                    (antiIso_events['njets'] == 1) |
                    ((antiIso_events['njets'] > 1) & antiIso_events['mjj'] < 300)
                ]
                fake_vbf_events = antiIso_events[(antiIso_events['njets'] > 1) & (antiIso_events['mjj'] > 300)]

                output_file.cd('{}_0jet'.format(channel_prefix))
                zero_jet_hist = build_histogram('jetFakes', decay_mode_bins, vis_mass_bins)
                zero_jet_hist = fill_fake_hist(fake_zero_jet_events, 't1_decayMode',
                                               'vis_mass', zero_jet_hist, fake_fractions['mt_0jet'], fake_weights, local=args.local)

                output_file.cd('{}_boosted'.format(channel_prefix))
                boost_hist = build_histogram('jetFakes', higgs_pT_bins_boost, m_sv_bins_boost)
                boost_hist = fill_fake_hist(fake_boosted_events, 'higgs_pT',
                                            'm_sv', boost_hist, fake_fractions['mt_boosted'], fake_weights, local=args.local)

                output_file.cd('{}_vbf'.format(channel_prefix))
                vbf_hist = build_histogram('jetFakes', mjj_bins, m_sv_bins_vbf)
                vbf_hist = fill_fake_hist(fake_vbf_events, 'mjj', 'm_sv',
                                          vbf_hist, fake_fractions['mt_vbf'], fake_weights, local=args.local)
                output_file.Write()

                if args.syst:
                    for syst in fake_factor_systematics:
                        output_file.cd('{}_0jet'.format(channel_prefix))
                        zero_jet_hist = build_histogram(
                            'jetFakes_CMS_htt_{}'.format(syst), decay_mode_bins, vis_mass_bins)
                        zero_jet_hist = fill_fake_hist(fake_zero_jet_events, 't1_decayMode',
                                                       'vis_mass', zero_jet_hist, fake_fractions['mt_0jet'], fake_weights, syst, local=rgs.local)

                        output_file.cd('{}_boosted'.format(channel_prefix))
                        boost_hist = build_histogram('jetFakes_CMS_htt_{}'.format(syst),
                                                     higgs_pT_bins_boost, m_sv_bins_boost)
                        boost_hist = fill_fake_hist(fake_boosted_events, 'higgs_pT',
                                                    'm_sv', boost_hist, fake_fractions['mt_boosted'], fake_weights, syst, local=args.local)

                        output_file.cd('{}_vbf'.format(channel_prefix))
                        vbf_hist = build_histogram('jetFakes_CMS_htt_{}'.format(syst), mjj_bins, m_sv_bins_vbf)
                        vbf_hist = fill_fake_hist(fake_vbf_events, 'mjj', 'm_sv',
                                                  vbf_hist, fake_fractions['mt_vbf'], fake_weights, syst, local=args.local)
                        output_file.Write()

    output_file.Close()
    fake_weights.Delete()
    print 'Finished in {} seconds'.format(time.time() - start)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--syst', '-s', action='store_true', help='run with systematics')
    parser.add_argument('--embed', '-e', action='store_true', help='use embedded instead of MC')
    parser.add_argument('--local', '-l', action='store_true', help='running locally')
    parser.add_argument('--year', '-y', required=True, action='store', help='year being processed')
    parser.add_argument('--input-dir', '-i', required=True, action='store', dest='input_dir', help='path to files')
    parser.add_argument('--tree-name', '-t', required=True, action='store', dest='tree_name', help='name of input tree')
    parser.add_argument('--fake-file', '-f', required=True, action='store', dest='fake_file',
                        help='name of file containing fake fractions')
    parser.add_argument('--date', '-d', required=True, action='store', help='today\'s date for output name')
    parser.add_argument('--suffix', action='store', default='', help='suffix for filename')
    main(parser.parse_args())
