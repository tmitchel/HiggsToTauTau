import ROOT
import pandas
import uproot
from array import array
from collections import defaultdict

mvis_bins = [0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000]
njets_bins = [-0.5, 0.5, 1.5, 15]


def get_categories(channel):
    return [channel + pref for pref in ['_inclusive', '_0jet', '_boosted', '_vbf']]


def build_histogram(name):
    return ROOT.TH2F(name, name, len(mvis_bins) - 1, array('d', mvis_bins), len(njets_bins) - 1, array('d', njets_bins))


def main(args):
    channel_prefix = args.tree_name[:2]
    fout = ROOT.TFile('Output/fake_fractions/{}{}_{}.root'.format(channel_prefix, args.year, args.suffix), 'recreate')
    categories = get_categories(channel_prefix)
    for cat in categories:
        fout.cd()
        fout.mkdir(cat)
        fout.cd()

    inputs = {
        'frac_w': ['W', 'ZJ', 'VVJ'],
        'frac_tt': ['TTJ'],
        'frac_data': ['Data'],
        'frac_real': ['ZTT', 'TTT', 'VVT'],
    }

    fractions = {
        'frac_w': {cat: build_histogram('frac_w_{}'.format(cat)) for cat in categories},
        'frac_tt': {cat: build_histogram('frac_tt_{}'.format(cat)) for cat in categories},
        'frac_qcd': {cat: build_histogram('frac_qcd_{}'.format(cat)) for cat in categories},
        'frac_data': {cat: build_histogram('frac_data_{}'.format(cat)) for cat in categories},
        'frac_real': {cat: build_histogram('frac_real_{}'.format(cat)) for cat in categories},
    }

    variables = set([
        'evtwt', 'vis_mass', 'mjj', 'njets', 'nbjets',
        'is_antiTauIso', 'OS', 'mt', 'higgs_pT', 't1_pt'
    ])

    for frac, samples in inputs.iteritems():
        for sample in samples:
            events = uproot.open('{}/{}.root'.format(args.input, sample)
                                 )[args.tree_name].arrays(list(variables), outputtype=pandas.DataFrame)

            anti_iso_events = events[
                (events['mt'] < 50) & (events['nbjets'] == 0) & (events['is_antiTauIso'] > 0)
            ]

            zero_jet_events = anti_iso_events[anti_iso_events['njets'] == 0]
            boosted_events = anti_iso_events[
                (anti_iso_events['njets'] == 1) |
                ((anti_iso_events['njets'] > 1) & anti_iso_events['mjj'] < 300)
            ]
            vbf_events = anti_iso_events[(anti_iso_events['njets'] > 1) & (anti_iso_events['mjj'] > 300)]

            # inclusive region
            vis_mass = anti_iso_events['vis_mass'].values
            njets = anti_iso_events['njets'].values
            evtwt = anti_iso_events['evtwt'].values
            for i in xrange(len(anti_iso_events.index)):
                fractions[frac]['{}_inclusive'.format(channel_prefix)].Fill(vis_mass[i], njets[i], evtwt[i])

            # 0jet region
            vis_mass = zero_jet_events['vis_mass'].values
            njets = zero_jet_events['njets'].values
            evtwt = zero_jet_events['evtwt'].values
            for i in xrange(len(zero_jet_events.index)):
                fractions[frac]['{}_0jet'.format(channel_prefix)].Fill(vis_mass[i], njets[i], evtwt[i])

            # boosted region
            vis_mass = boosted_events['vis_mass'].values
            njets = boosted_events['njets'].values
            evtwt = boosted_events['evtwt'].values
            for i in xrange(len(boosted_events.index)):
                fractions[frac]['{}_boosted'.format(channel_prefix)].Fill(vis_mass[i], njets[i], evtwt[i])

            # vbf region
            vis_mass = vbf_events['vis_mass'].values
            njets = vbf_events['njets'].values
            evtwt = vbf_events['evtwt'].values
            for i in xrange(len(vbf_events.index)):
                fractions[frac]['{}_vbf'.format(channel_prefix)].Fill(vis_mass[i], njets[i], evtwt[i])

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

    fout.Close()


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--suffix', '-s', required=True, help='string to append to output file name')
    parser.add_argument('--year', '-y', required=True, help='year being processed')
    parser.add_argument('--tree-name', '-t', dest='tree_name', required=True, help='name of TTree to process')
    main(parser.parse_args())
