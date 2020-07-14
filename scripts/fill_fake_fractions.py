import ROOT
import pandas
import numpy
import uproot
from multiprocessing import Process, Queue
from array import array
from collections import defaultdict
from utils.ApplyFF import FFApplicationTool
from subprocess import call
import multiprocessing

import warnings
warnings.filterwarnings(
    'ignore', category=pandas.io.pytables.PerformanceWarning)

mvis_bins = [0, 50, 80, 100, 110, 120, 130, 150, 170, 200, 250, 1000]
njets_bins = [-0.5, 0.5, 1.5, 15]
systs_names = [
    'ff_qcd_0jet_unc1', 'ff_qcd_0jet_unc2', 'ff_qcd_1jet_unc1', 'ff_qcd_1jet_unc2', 'ff_qcd_2jet_unc1', 'ff_qcd_2jet_unc2',
    'ff_w_0jet_unc1', 'ff_w_0jet_unc2', 'ff_w_1jet_unc1', 'ff_w_1jet_unc2', 'ff_w_2jet_unc1', 'ff_w_2jet_unc2',
    'ff_tt_0jet_unc1', 'ff_tt_0jet_unc2',
    'mtclosure_w_unc1', 'mtclosure_w_unc2',
    'lptclosure_xtrg_qcd', 'lptclosure_xtrg_w', 'lptclosure_xtrg_tt',
    'lptclosure_qcd', 'lptclosure_w', 'lptclosure_tt',
    'osssclosure_qcd'
]
systs = [(name, 'up') for name in systs_names] + [(name, 'down') for name in systs_names]

filling_variables = [
    't1_pt', 't1_decayMode', 'njets', 'vis_mass', 'mt', 'mu_pt',
    'el_pt', 'mjj', 'is_antiTauIso', 'cross_trigger'
]


def get_categories(channel):
    """Return list of categories with the lepton prefix added."""
    return [channel + pref for pref in ['_inclusive', '_0jet', '_boosted', '_vbf']]


def build_histogram(name):
    """Build TH2F to fill with fake fraction."""
    return ROOT.TH2F(name, name, len(mvis_bins) - 1, array('d', mvis_bins), len(njets_bins) - 1, array('d', njets_bins))


def categorize(njets, mjj, channel):
    """Categorize this event based on njets and mjj."""
    if njets == 0:
        return '{}_0jet'.format(channel)
    elif njets == 1 or (njets > 1 and mjj < 300):
        return '{}_boosted'.format(channel)
    elif njets > 1 and mjj > 300:
        return '{}_vbf'.format(channel)
    else:
        raise Exception('We missed something here...')


def fill_fraction(df, fraction):
    """Use visible Higgs mass and njets to fill this histogram."""
    vis_mass = df['vis_mass'].values
    njets = df['njets'].values
    evtwt = df['evtwt'].values
    for i in xrange(len(df.index)):
        fraction.Fill(vis_mass[i], njets[i], evtwt[i])


def get_weight(df, fake_weights, fractions, channel, syst=None):
    """Read input variables and fake fractions to get the correct fake weight."""
    category = categorize(df['njets'], df['mjj'], channel)
    if channel == 'et':
        pt_name = 'el_pt'
    else:
        pt_name = 'mu_pt'

    xbin = fractions['frac_data'][category].GetXaxis().FindBin(df['vis_mass'])
    ybin = fractions['frac_data'][category].GetYaxis().FindBin(df['njets'])

    if syst != None:
        weights = fake_weights.get_ff(df['t1_pt'], df['mt'], df['vis_mass'], df[pt_name], df['njets'], df['cross_trigger'],
                                      fractions['frac_tt'][category].GetBinContent(xbin, ybin),
                                      fractions['frac_qcd'][category].GetBinContent(xbin, ybin),
                                      fractions['frac_w'][category].GetBinContent(xbin, ybin),
                                      syst[0], syst[1])
    else:
        weights = fake_weights.get_ff(df['t1_pt'], df['mt'], df['vis_mass'], df[pt_name], df['njets'], df['cross_trigger'],
                                      fractions['frac_tt'][category].GetBinContent(xbin, ybin),
                                      fractions['frac_qcd'][category].GetBinContent(xbin, ybin),
                                      fractions['frac_w'][category].GetBinContent(xbin, ybin))

    return weights


def parse_tree_name(keys):
    """Take list of keys in the file and search for our TTree"""
    if 'et_tree;1' in keys:
        return 'et_tree'
    elif 'mt_tree;1' in keys:
        return 'mt_tree'
    else:
        raise Exception('Can\t find et_tree or mt_tree in keys: {}'.format(keys))


def create_fakes(input_name, tree_name, channel_prefix, treedict, output_dir, fake_file, fractions, sample, doSysts=False):
    ff_weighter = FFApplicationTool(fake_file, channel_prefix)

    open_file = uproot.open('{}/{}.root'.format(input_name, sample))
    events = open_file[tree_name].arrays(['*'], outputtype=pandas.DataFrame)
    anti_events = events[(events['is_antiTauIso'] > 0)].copy()

    anti_events['fake_weight'] = anti_events[filling_variables].apply(
        lambda x: get_weight(x, ff_weighter, fractions, channel_prefix), axis=1).values
    if sample != 'data_obs':
        anti_events['fake_weight'] *= -1

    if doSysts:
        for syst in systs:
            print 'Processing: {} {}'.format(sample, syst)
            anti_events[syst[0] + "_" + syst[1]] = anti_events[filling_variables].apply(lambda x: get_weight(
                x, ff_weighter, fractions, channel_prefix, syst=syst), axis=1).values
            if sample != 'data_obs':
                anti_events[syst[0] + "_" + syst[1]] *= -1

    with uproot.recreate('{}/jetFakes_{}.root'.format(output_dir, sample)) as f:
        f[tree_name] = uproot.newtree(treedict)
        f[tree_name].extend(anti_events.to_dict('list'))

    print 'Finished writing {}'.format(sample)
    return None


def main(args):
    keys = uproot.open('{}/data_obs.root'.format(args.input)).keys()
    tree_name = parse_tree_name(keys)
    channel_prefix = tree_name[:2]
    fout = ROOT.TFile('Output/fake_fractions/{}{}_{}.root'.format(channel_prefix, args.year, args.suffix), 'recreate')
    categories = get_categories(channel_prefix)
    fake_file = '/hdfs/store/user/tmitchel/deep-tau-fake-factor/ff_files_{}_{}/'.format(channel_prefix, args.year)
    for cat in categories:
        fout.cd()
        fout.mkdir(cat)
        fout.cd()

    inputs = {
        'frac_w': ['W', 'ZJ', 'VVJ', 'STJ'],
        'frac_tt': ['TTJ'],
        'frac_data': ['data_obs'],
        'frac_real': ['STL', 'VVL', 'TTL', 'ZL', 'STT', 'VVT', 'TTT', 'embed'],
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
        'is_antiTauIso', 'mt', 'higgs_pT', 't1_pt', 'contamination', 't1_genMatch'
    ])

    for frac, samples in inputs.iteritems():
        for sample in samples:
            print sample
            events = uproot.open('{}/{}.root'.format(args.input, sample)
                                 )[tree_name].arrays(list(variables), outputtype=pandas.DataFrame)

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

        # handle bins that go negative
        for xbin in range(0, fractions['frac_qcd'][cat].GetNbinsX() + 1):
            for ybin in range(0, fractions['frac_qcd'][cat].GetNbinsY() + 1):
                if fractions['frac_qcd'][cat].GetBinContent(xbin, ybin) < 0:
                    fractions['frac_qcd'][cat].SetBinContent(xbin, ybin, 0.)

        denom = fractions['frac_qcd'][cat].Clone()
        denom.Add(fractions['frac_w'][cat])
        denom.Add(fractions['frac_tt'][cat])

        print 'Category: {}'.format(cat)
        print '\tfrac_w: {}'.format(fractions['frac_w'][cat].Integral() / denom.Integral())
        print '\tfrac_tt: {}'.format(fractions['frac_tt'][cat].Integral() / denom.Integral())
        print '\tfrac_qcd: {}'.format(fractions['frac_qcd'][cat].Integral() / denom.Integral())
        print '\tfrac_real: {}'.format(fractions['frac_real'][cat].Integral() / denom.Integral())

        fractions['frac_w'][cat].Divide(denom)
        fractions['frac_tt'][cat].Divide(denom)
        fractions['frac_qcd'][cat].Divide(denom)

    for frac_name, categories in fractions.iteritems():
        for cat_name, ihist in categories.iteritems():
            fout.cd(cat_name)
            ihist.Write(frac_name)

    open_file = uproot.open('{}/data_obs.root'.format(args.input))
    oldtree = open_file[tree_name].arrays(['*'])
    treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}
    treedict['fake_weight'] = numpy.float64
    if args.syst:
        for syst in systs:
            treedict[syst[0] + "_" + syst[1]] = numpy.float64

    output_dir = 'tmp/fakes_{}'.format(args.suffix)
    call('mkdir {}'.format(output_dir), shell=True)

    samples = [
        'data_obs', 'embed',
        'STL', 'VVL', 'TTL', 'ZL', 'STT', 'VVT', 'TTT'
    ]

    manager = multiprocessing.Manager()
    n_processes = min(9, multiprocessing.cpu_count() / 2)
    pool = multiprocessing.Pool(processes=n_processes)

    jobs = [pool.apply_async(create_fakes, (args.input, tree_name, channel_prefix, treedict,
                                            output_dir, fake_file, fractions, sample, args.syst)) for sample in samples]
    a = [j.get() for j in jobs]

    pool.close()
    pool.join()
    fout.Close()

    call('ahadd.py {0}/jetFakes.root {0}/jetFakes_*.root'.format(output_dir), shell=True)

    if '/hdfs' in args.input:
        call('mv -v {}/jetFakes.root {}'.format(output_dir, args.input), shell=True)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--suffix', '-s', required=True, help='string to append to output file name')
    parser.add_argument('--year', '-y', required=True, help='year being processed')
    parser.add_argument('--syst', action='store_true', help='run fake factor systematics as well')
    main(parser.parse_args())
