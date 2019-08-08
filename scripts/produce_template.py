import ROOT
import uproot
from glob import glob

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


def main(args):
    channel_prefix = args.tree_name.replace('_tree', '')  # need prefix for things later
    assert channel_prefix == 'mt' or channel_prefix == 'et', 'must provide a valid tree name'
    files = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]  # get files to process

    # get things for output file name
    ztt_name = '_emb' if args.embed else '_ztt'
    syst_name = '_Sys' if args.syst else '_noSys'

    output_file = ROOT.TFile('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
                                                                                  ztt_name, syst_name, args.date, '_'+args.suffix))
    
    # create structure within output file
    for cat in categories:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
    output_file.cd()

    for ifile in files:
        name = ifile.replace('.root', '')
        events = uproot.open(ifile)[args.tree_name]
        print events.array('m_sv')[:10]


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
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
