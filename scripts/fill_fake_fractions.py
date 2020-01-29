import ROOT
import pandas
import numpy
import uproot
from multiprocessing import Process, Queue
from array import array
from collections import defaultdict
from ApplyFF import FFApplicationTool

mvis_bins = [0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000]
njets_bins = [-0.5, 0.5, 1.5, 15]
systs = [
    "ff_qcd_syst_up", "ff_qcd_syst_down",
    "ff_qcd_dm0_njet0_stat_up", "ff_qcd_dm0_njet0_stat_down",
    "ff_qcd_dm0_njet1_stat_up", "ff_qcd_dm0_njet1_stat_down",
    "ff_qcd_dm1_njet0_stat_up", "ff_qcd_dm1_njet0_stat_down",
    "ff_qcd_dm1_njet1_stat_up", "ff_qcd_dm1_njet1_stat_down",
    "ff_w_syst_up", "ff_w_syst_down",
    "ff_w_dm0_njet0_stat_up", "ff_w_dm0_njet0_stat_down",
    "ff_w_dm0_njet1_stat_up", "ff_w_dm0_njet1_stat_down",
    "ff_w_dm1_njet0_stat_up", "ff_w_dm1_njet0_stat_down",
    "ff_w_dm1_njet1_stat_up", "ff_w_dm1_njet1_stat_down",
    "ff_tt_syst_up", "ff_tt_syst_down",
    "ff_tt_dm0_njet0_stat_up", "ff_tt_dm0_njet0_stat_down",
    "ff_tt_dm0_njet1_stat_up", "ff_tt_dm0_njet1_stat_down",
    "ff_tt_dm1_njet0_stat_up", "ff_tt_dm1_njet0_stat_down",
    "ff_tt_dm1_njet1_stat_up", "ff_tt_dm1_njet1_stat_down"
]


def get_categories(channel):
    return [channel + pref for pref in ['_inclusive', '_0jet', '_boosted', '_vbf']]


def build_histogram(name):
    return ROOT.TH2F(name, name, len(mvis_bins) - 1, array('d', mvis_bins), len(njets_bins) - 1, array('d', njets_bins))


def categorize(njets, mjj, channel):
    if njets == 0:
        return '{}_0jet'.format(channel)
    elif njets == 1 or (njets > 1 and mjj < 300):
        return '{}_boosted'.format(channel)
    elif njets > 1 and mjj > 300:
        return '{}_vbf'.format(channel)
    else:
        raise Exception('We missed something here...')


def fill_fraction(df, fraction):
    vis_mass = df['vis_mass'].values
    njets = df['njets'].values
    evtwt = df['evtwt'].values
    for i in xrange(len(df.index)):
        fraction.Fill(vis_mass[i], njets[i], evtwt[i])   


def get_weight(df, fake_weights, fractions, channel, syst=None):
    category = categorize(df['njets'], df['mjj'], channel)
    if channel == 'et':
        pt_name = 'el_pt'
    else:
        pt_name = 'mu_pt'

    xbin = fractions['frac_data'][category].GetXaxis().FindBin(df['vis_mass'])
    ybin = fractions['frac_data'][category].GetYaxis().FindBin(df['njets'])

    weights = fake_weights.get_ff(df['t1_pt'], df['mt'], df['vis_mass'], df[pt_name], df['njets'], df['cross_trigger'],
                                  fractions['frac_qcd'][category].GetBinContent(xbin, ybin),
                                  fractions['frac_w'][category].GetBinContent(xbin, ybin),
                                  fractions['frac_tt'][category].GetBinContent(xbin, ybin))

    return weights


def main(args):
    channel_prefix = args.tree_name[:2]
    fout = ROOT.TFile('Output/fake_fractions/{}{}_{}.root'.format(channel_prefix, args.year, args.suffix), 'recreate')
    categories = get_categories(channel_prefix)
    fake_file = '/hdfs/store/user/tmitchel/fake-weights-cecile/ff_files_{}_{}'.format(channel_prefix, args.year)
    ff_weighter = FFApplicationTool(fake_file, channel_prefix)
    for cat in categories:
        fout.cd()
        fout.mkdir(cat)
        fout.cd()

    inputs = {
        'frac_w': ['W', 'ZJ', 'VVJ'],
        'frac_tt': ['TTJ'],
        'frac_data': ['data_obs'],
        'frac_real': ['embed', 'TTT', 'VVT'],
        # 'frac_real': ['ZTT', 'TTT', 'VVT'],
    }

    fractions = {
        'frac_w': {cat: build_histogram('frac_w_{}'.format(cat)) for cat in categories},
        'frac_tt': {cat: build_histogram('frac_tt_{}'.format(cat)) for cat in categories},
        'frac_qcd': {cat: build_histogram('frac_qcd_{}'.format(cat)) for cat in categories},
        'frac_data': {cat: build_histogram('frac_data_{}'.format(cat)) for cat in categories},
        'frac_real': {cat: build_histogram('frac_real_{}'.format(cat)) for cat in categories},
    }

    variables = set([
        'evtwt', 'vis_mass', 'mjj', 'njets',
        'is_antiTauIso', 'mt', 'higgs_pT', 't1_pt', 'contamination'
    ])

    for frac, samples in inputs.iteritems():
        for sample in samples:
            print sample
            events = uproot.open('{}/{}.root'.format(args.input, sample)
                                 )[args.tree_name].arrays(list(variables), outputtype=pandas.DataFrame)

            anti_iso_events = events[
                (events['is_antiTauIso'] > 0) & (events['contamination'] == 0)
            ]

            zero_jet_events = anti_iso_events[anti_iso_events['njets'] == 0]
            boosted_events = anti_iso_events[
                (anti_iso_events['njets'] == 1) |
                ((anti_iso_events['njets'] > 1) & anti_iso_events['mjj'] < 300)
            ]
            vbf_events = anti_iso_events[(anti_iso_events['njets'] > 1) & (anti_iso_events['mjj'] > 300)]

            # inclusive region
            fill_fraction(anti_iso_events, fractions[frac]['{}_inclusive'.format(channel_prefix)])
            fill_fraction(zero_jet_events, fractions[frac]['{}_0jet'.format(channel_prefix)])
            fill_fraction(boosted_events, fractions[frac]['{}_boosted'.format(channel_prefix)])
            fill_fraction(vbf_events, fractions[frac]['{}_vbf'.format(channel_prefix)])

    for cat in categories:
        fractions['frac_qcd'][cat] = fractions['frac_data'][cat].Clone()
        fractions['frac_qcd'][cat].Add(fractions['frac_w'][cat], -1)
        fractions['frac_qcd'][cat].Add(fractions['frac_tt'][cat], -1)
        fractions['frac_qcd'][cat].Add(fractions['frac_real'][cat], -1)

        fractions['frac_w'][cat].Divide(fractions['frac_data'][cat])
        fractions['frac_tt'][cat].Divide(fractions['frac_data'][cat])
        fractions['frac_real'][cat].Divide(fractions['frac_data'][cat])
        fractions['frac_qcd'][cat].Divide(fractions['frac_data'][cat])

    for frac_name, categories in fractions.iteritems():
        for cat_name, ihist in categories.iteritems():
            fout.cd(cat_name)
            ihist.Write(frac_name)

    if args.create_fakes:
        open_file = uproot.open('{}/data_obs.root'.format(args.input))
        oldtree = open_file[args.tree_name].arrays(['*'])
        treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}

        events = open_file[args.tree_name].arrays([
            't1_pt', 't1_decayMode', 'njets', 'vis_mass', 'mt', 'mu_pt', 'el_pt', 'mjj', 'is_antiTauIso', 'cross_trigger'
        ], outputtype=pandas.DataFrame)

        treedict['fake_weight'] = numpy.float64
        print 'getting fake weights...'
        fake_weights = events.apply(lambda x: get_weight(x, ff_weighter, fractions, channel_prefix), axis=1).values

        if args.syst:
            syst_map = {}
            for syst in systs:
                print syst
                treedict[syst] = numpy.float64
                syst_map[syst] = events.apply(lambda x: get_weight(
                    x, ff_weighter, fractions, channel_prefix, syst=syst), axis=1).values

        with uproot.recreate('{}/jetFakes.root'.format(args.input)) as f:
            f[args.tree_name] = uproot.newtree(treedict)
            oldtree['fake_weight'] = fake_weights.reshape(len(fake_weights))
            if args.syst:
                for syst in systs:
                    oldtree[syst] = syst_map[syst]
            f[args.tree_name].extend(oldtree)

    fout.Close()


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--suffix', '-s', required=True, help='string to append to output file name')
    parser.add_argument('--year', '-y', required=True, help='year being processed')
    parser.add_argument('--tree-name', '-t', dest='tree_name', required=True, help='name of TTree to process')
    parser.add_argument('--create-fakes', '-c', action='store_true',
                        help='create root file with data and fake weights applied')
    parser.add_argument('--syst', action='store_true', help='run fake factor systematics as well')
    main(parser.parse_args())
