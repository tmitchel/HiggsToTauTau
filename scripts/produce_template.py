import ROOT
import time
import numpy
import pandas
import uproot
from glob import glob
from array import array

treename_conversion = {
    "_tree_UncMet_Up": "_CMS_scale_met_unclustered_13TeVUp",
    "_tree_UncMet_Down": "_CMS_scale_met_unclustered_13TeVDown",
    "_tree_ClusteredMet_Up": "_CMS_scale_met_clustered_13TeVUp",
    "_tree_ClusteredMet_Down": "_CMS_scale_met_clustered_13TeVDown",
    "_tree_vbfMass_JetTotalUp": "_CMS_scale_jm_13TeVUp",
    "_tree_jetVeto30_JetTotalUp": "_CMS_scale_jn_13TeVUp",
    "_tree_vbfMass_JetTotalDown": "_CMS_scale_jm_13TeVDown",
    "_tree_jetVeto30_JetTotalDown": "_CMS_scale_jn_13TeVDown",
    "_tree_ttbarShape_Up": "_CMS_htt_ttbarShape_13TeVUp",
    "_tree_ttbarShape_Down": "_CMS_htt_ttbarShape_13TeVDown",
    "_tree_Up": "_CMS_scale_t_allprong_13TeVUp",
    "_tree_Down": "_CMS_scale_t_allprong_13TeVDown",
    "_tree_DM0_Up": "_CMS_scale_t_1prong_13TeVUp",
    "_tree_DM0_Down": "_CMS_scale_t_1prong_13TeVDown",
    "_tree_DM1_Up": "_CMS_scale_t_1prong1pizero_13TeVUp",
    "_tree_DM1_Down": "_CMS_scale_t_1prong1pizero_13TeVDown",
    "_tree_DM10_Up": "_CMS_scale_t_3prong_13TeVUp",
    "_tree_DM10_Down": "_CMS_scale_t_3prong_13TeVDown",
    "_tree_jetToTauFake_Up": "_CMS_htt_jetToTauFake_13TeVUp",
    "_tree_jetToTauFake_Down": "_CMS_htt_jetToTauFake_13TeVDown",
    "_tree_dyShape_Up": "_CMS_htt_dyShape_13TeVUp",
    "_tree_dyShape_Down": "_CMS_htt_dyShape_13TeVDown",
    "_tree_zmumuShape_Up": "_CMS_htt_zmumuShape_VBF_13TeVUp",
    "_tree_zmumuShape_Down": "_CMS_htt_zmumuShape_VBF_13TeVDown"
}

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

decay_mode_bins = [0, 1, 10, 11]
higgs_pT_bins_boost = [0, 100, 150, 200, 250, 300, 5000]
mjj_bins = [300, 500, 10000]
vis_mass_bins = [0, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 400]
m_sv_bins_boost = [0, 80, 90, 100, 110, 120, 130, 140, 150, 160, 300]
m_sv_bins_vbf = [0, 80, 100, 115, 130, 150, 1000]


def build_histogram(name, x_bins, y_bins):
    return ROOT.TH2F(name, name, len(x_bins) - 1, array('d', x_bins), len(y_bins) - 1, array('d', y_bins))


def fill_fake_background(events, fake_file):
    ff_file = ROOT.TFile(fake_file, 'READ')
    ff = ff_file.Get('ff_comb')
    zero_jet_events = events[events['njets'] == 0]
    boosted_events = events[
        (events['njets'] == 1) |
        ((events['njets'] > 1) & events['mjj'] < 300)
    ]
    vbf_events = events[(events['njets'] > 1) & (events['mjj'] > 300)]


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

    # output_file = uproot.recreate('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
    #                                                                               ztt_name, syst_name, args.date, '_'+args.suffix))

    for ifile in files:
        name=ifile.replace('.root', '')
        print name
        events=uproot.open(ifile)[args.tree_name].arrays([
            'is_signal', 'is_antiTauIso', 'OS', 'nbjets', 'njets', 'mjj', 'evtwt', 'wt_*',
            'mu_iso', 'el_iso', 't1_decayMode', 'vis_mass', 't1_pt', 'higgs_pT', 'm_sv',
            'D0_VBF', 'D0_ggH', 'DCP_VBF', 'DCP_ggH', 'j1_phi', 'j2_phi', 'mt'
        ], outputtype=pandas.DataFrame)

        general_selection=events[
            (events['mt'] < 50) & (events['nbjets'] == 0)
        ]

        # do signal categorization
        signal_events=general_selection[general_selection['is_signal'] > 0]

        zero_jet_events=signal_events[signal_events['njets'] == 0]
        boosted_events=signal_events[
            (signal_events['njets'] == 1) |
            ((signal_events['njets'] > 1) & signal_events['mjj'] < 300)
        ]
        vbf_events=signal_events[(signal_events['njets'] > 1) & (signal_events['mjj'] > 300)]

        # do anti-iso categorization for fake-factor for data

        antiIso_events=general_selection[general_selection['is_antiTauIso'] > 0]
        # fill_fake_background(antiIso_events, args.fake_file)

        # uproot can't handle subdirectories yet, so I'll use plain old pyroot

        # start with 0-jet category
        output_file.cd('{}_0jet'.format(channel_prefix))
        zero_jet_hist=build_histogram(name.split('/')[-1], decay_mode_bins, vis_mass_bins)
        for i in xrange(len(zero_jet_events.index)):
            zero_jet_hist.Fill(zero_jet_events['t1_decayMode'].values[i],
                               zero_jet_events['vis_mass'].values[i], zero_jet_events['evtwt'].values[i])

        # now boosted category
        output_file.cd('{}_boosted'.format(channel_prefix))
        boost_hist=build_histogram(name.split('/')[-1], higgs_pT_bins_boost, m_sv_bins_boost)
        for i in xrange(len(boosted_events.index)):
            boost_hist.Fill(boosted_events['higgs_pT'].values[i],
                            boosted_events['m_sv'].values[i], boosted_events['evtwt'].values[i])

        # vbf category is last
        output_file.cd('{}_vbf'.format(channel_prefix))
        vbf_hist=build_histogram(name.split('/')[-1], mjj_bins, m_sv_bins_vbf)
        for i in xrange(len(vbf_events.index)):
            vbf_hist.Fill(vbf_events['mjj'].values[i], vbf_events['m_sv'].values[i], vbf_events['evtwt'].values[i])

        output_file.Write()

    output_file.Close()
    print 'Finished in {} seconds'.format(time.time() - start)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser=ArgumentParser()
    parser.add_argument('--syst', '-s', action='store_true', help='run with systematics')
    parser.add_argument('--embed', '-e', action='store_true', help='use embedded instead of MC')
    parser.add_argument('--year', '-y', required=True, action='store', help='year being processed')
    parser.add_argument('--input-dir', '-i', required=True, action='store', dest='input_dir', help='path to files')
    parser.add_argument('--tree-name', '-t', required=True, action='store', dest='tree_name', help='name of input tree')
    parser.add_argument('--fake-file', '-f', required=True, action='store', dest='fake_file',
                        help='name of file containing fake fractions')
    parser.add_argument('--date', '-d', required=True, action='store', help='today\'s date for output name')
    parser.add_argument('--suffix', action='store', default='', help='suffix for filename')
    main(parser.parse_args())
