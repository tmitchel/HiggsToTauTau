import os
import ROOT
import pandas
import numpy
import uproot
from multiprocessing import Process, Queue
from array import array
from collections import defaultdict
from utils.ApplyFF_ditau import FFApplicationTool
from subprocess import call
import multiprocessing

import warnings
warnings.filterwarnings(
    'ignore', category=pandas.io.pytables.PerformanceWarning)

systs_names = [
    'raw_0jet', 'raw_1jet', 'raw_2jet', 'tau2_0jet', 'tau2_1jet', 'tau2_2jet', 'vis_mass'
]
systs = [(name, 'up') for name in systs_names] + [(name, 'down') for name in systs_names]

filling_variables = [
    't1_pt', 't2_pt', 'njets', 'vis_mass', 'mt', 'is_antiTauIso'
]


def get_weight(df, fake_weights, syst=None):
    """Read input variables and fake fractions to get the correct fake weight."""

    if syst != None:
        weights = fake_weights.get_ff(df['t1_pt'], df['t2_pt'], df['vis_mass'], df['njets'], syst[0], syst[1])
    else:
        weights = fake_weights.get_ff(df['t1_pt'], df['t2_pt'], df['vis_mass'], df['njets'])

    return weights


def create_fakes(input_name, tree_name, channel_prefix, treedict, output_dir, fake_file, sample, doSysts=False):
    ff_weighter = FFApplicationTool(fake_file, channel_prefix)

    open_file = uproot.open('{}/{}.root'.format(input_name, sample))
    events = open_file[tree_name].arrays(['*'], outputtype=pandas.DataFrame)
    anti_events = events[(events['is_antiTauIso'] > 0)].copy()

    anti_events['fake_weight'] = anti_events[filling_variables].apply(
        lambda x: get_weight(x, ff_weighter), axis=1).values
    if sample != 'data_obs':
        anti_events['fake_weight'] *= -1

    if doSysts:
        for syst in systs:
            print 'Processing: {} {}'.format(sample, syst)
            anti_events[syst[0] + "_" + syst[1]] = anti_events[filling_variables].apply(lambda x: get_weight(
                x, ff_weighter, syst=syst), axis=1).values
            if sample != 'data_obs':
                anti_events[syst[0] + "_" + syst[1]] *= -1

    with uproot.recreate('{}/jetFakes_{}.root'.format(output_dir, sample)) as f:
        f[tree_name] = uproot.newtree(treedict)
        f[tree_name].extend(anti_events.to_dict('list'))

    print 'Finished writing {}'.format(sample)
    return None


def main(args):
    open_file = uproot.open('{}/data_obs.root'.format(args.input))
    fake_file = '/hdfs/store/user/tmitchel/deep-tau-fake-factor/ff_files_tt_{}/'.format(args.year)
    if 'uscms' in os.getcwd():
        fake_file = 'root://cmsxrootd.fnal.gov//store/user/tmitchel/HTT_ScaleFactors/weightROOTs/FF_v3/ff_files_tt_{}'.format(args.year)

    oldtree = open_file['tt_tree'].arrays(['*'])
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
                                            output_dir, fake_file, sample, args.syst)) for sample in samples]
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
