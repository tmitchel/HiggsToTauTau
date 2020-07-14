import copy
import json
import ROOT
import time
import numpy
import pandas
import uproot
from glob import glob
from array import array
from pprint import pprint
from copy import deepcopy
from multiprocessing import Process, Queue


class Config:
    """
    Provide a wrapper around all information needed to plot a single sample/variable combination.

    Config wraps all of the information needed to process a sample and create correctly weighted
    histograms for the provided variable and binning. Config contains a Queue so that things can
    be done in parallel.

    Variables:
    name        -- name of the process
    data        -- pandas DataFrame containing event data
    xvar_name   -- variable being plotted
    bins        -- binning for the histogram
    queue       -- multiprocessing.Queue to write output histogram
    fake_weight -- for jetFakes, we need to weight with the fake_weight
    hists       -- output histograms

    Functions:
    submit      -- create a new Queue and return Config as a dict to provide as kwargs
    """

    def __init__(self, name, data, variable, bins):
        """Initialize variables and set the rest to None"""
        self.name = name
        self.data = data
        self.xvar_name = variable
        self.bins = bins
        self.queue = None
        self.fake_weight = None
        self.hists = None

    def __deepcopy__(self, memo):
        """Deep copy everything except the Queue."""
        cp = Config(deepcopy(self.name, memo), deepcopy(self.data, memo), deepcopy(self.xvar_name, memo), deepcopy(self.bins, memo))
        cp.fake_weight = deepcopy(self.fake_weight, memo)
        cp.queue = None
        cp.hists = deepcopy(self.hists, memo)
        return cp

    def submit(self):
        """Create a new Queue and return self in a form for fill_histograms"""
        self.queue = Queue()
        return {'config': self}


def build_histogram(name, bins, output_file, directory):
    """Build a TH1F with provided binning in the correct TDirectory"""
    output_file.cd(directory)
    return ROOT.TH1F(name, name, bins[0], bins[1], bins[2])


def fill_histograms(config):
    """Use config to fill a histogram"""
    # get common variables
    evtwt = config.data['evtwt'].values
    xvar = config.data[config.xvar_name].values

    if config.fake_weight != None:
        evtwt *= config.data[config.fake_weight].values

    # loop over all events
    for i in xrange(len(config.data.index)):
        config.hists.Fill(xvar[i], evtwt[i])

    config.queue.put(config.hists)


def fill_process_list(data, name, variable, bins, boilerplate, output_file, directory, year, doSyst=False):
    """
    Create the list of processes to be run.

    Configure all options needed to fill histograms for this process. A list of multiprocessing.Process-es will be
    returned for processing later.

    Variables:
    data        -- pandas DataFrame containing event information
    name        -- name of the process
    variable    -- variable to plot
    bins        -- binning for the histogram
    boilerplate -- json config containing some naming
    output_file -- TFile where histograms will be written
    directory   -- name of TDirectory for this histogram
    year        -- which year is this? (2016, 2017, 2018)
    doSyst      -- fill histograms after systematic shifts (not implemented) 

    Returns:
    List        -- multiprocessing.Process-es to be run
    all_hists   -- dictionary containing Configs used in List
    """
    all_hists = {}
    config = Config(name, data, variable, bins)

    if 'jetFakes' in name or 'QCD' in name:
        config.fake_weight = 'fake_weight'
        config.hists = build_histogram('jetFakes', bins, output_file, directory)
        all_hists['jetFakes'] = config.submit()
        if doSyst:
            for syst in boilerplate['fake_factor_systematics']:
                syst_config = copy.deepcopy(config)
                syst_name = 'jetFakes_CMS_htt_{}'.format(syst)
                syst_config.name = name
                syst_config.fake_weight = syst
                syst_config.hists = build_histogram(syst_name, bins, output_file, directory)
                all_hists[name] = syst_config.submit()
    else:
        config.hists = build_histogram(name, bins, output_file, directory)
        all_hists['nominal'] = config.submit()

    return [
        Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in all_hists.iteritems()
    ], all_hists

# I've added the tt part to make it compatible with ditau analysis
def parse_tree_name(keys):
    """Take list of keys in the file and search for our TTree"""
    if 'et_tree;1' in keys:
        return 'et_tree'
    elif 'mt_tree;1' in keys:
        return 'mt_tree'
    elif 'em_tree;1' in keys:
        return 'em_tree'
    elif 'tt_tree;1' in keys:
        return 'tt_tree'
    else:
        raise Exception('Can\t find et_tree, mt_tree, or tt_tree in keys: {}'.format(keys))


def main(args):
    start = time.time()
    config = {}
    boilerplate = {}
    with open('configs/boilerplate.json', 'r') as config_file:
        boilerplate = json.load(config_file)

    with open('configs/plotting.json', 'r') as config_file:
        config = json.load(config_file)
        config = config[args.config]
        config_variables = config['variables']
        zvars = config['zvar']

    files = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]  # get files to process

    keys = uproot.open(files[0]).keys()
    tree_name = parse_tree_name(keys)

    channel_prefix = tree_name.replace('mt_tree', 'mt')
    channel_prefix = channel_prefix.replace('et_tree', 'et')
    channel_prefix = channel_prefix.replace('em_tree', 'em')
    channel_prefix = channel_prefix.replace('tt_tree', 'tt')
    assert channel_prefix == 'mt' or channel_prefix == 'et' or channel_prefix == 'tt' or channel_prefix == 'em', 'must provide a valid tree name'

    # get things for output file name
    ztt_name = 'emb' if args.embed else 'ztt'
    syst_name = 'Sys' if args.syst else 'noSys'

    output_file = ROOT.TFile('Output/histograms/htt_{}_{}_{}_fa3_{}_{}{}.root'.format(channel_prefix,
                                                                                      ztt_name, syst_name, args.year, args.date, args.suffix), 'RECREATE')

    # create structure within output file
    for cat in boilerplate['categories']:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
        for variable in config_variables.iterkeys():
            output_file.mkdir('{}_{}/{}'.format(channel_prefix, cat, variable))
    output_file.cd()

    # use this once uproot supports sub-directories inside root files
    # output_file = uproot.recreate('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
    #                                                                               ztt_name, syst_name, args.date, '_'+args.suffix))
    for ifile in files:

        # handle ZTT vs embedded
        if args.embed and 'ZTT' in ifile:
            continue
        elif not args.embed and 'embed' in ifile:
            continue

        name = ifile.replace('.root', '').split('/')[-1]
        print name
        input_file = uproot.open(ifile)
        trees = [ikey.replace(';1', '') for ikey in input_file.keys()
                 if 'tree' in ikey] if args.syst else [tree_name]
        for itree in trees:
            if itree != tree_name:
                name = ifile.replace('.root', '').split(
                    '/')[-1] + boilerplate['syst_name_map'][itree.replace(tree_name, '')]
            else:
                name = ifile.replace('.root', '').split('/')[-1]

            # get data naming correct
            if 'Data' in name:
                name = 'data_obs'

            # handle MC vs embedded name
            if 'embed' in ifile:
                name = name.replace('embed', 'ZTT')

            variables = set([
                'is_signal', 'is_antiTauIso', 'contamination', 'njets', 'mjj', 'evtwt',
                't1_decayMode', 'vis_mass', 'higgs_pT', 'm_sv'
            ] + config_variables.keys() + [zvars[0]])

            # get fake factor weights if needed
            if 'jetFakes' in ifile or 'QCD' in ifile:
                variables.add('fake_weight')
                if args.syst:
                    variables.add('ff_*')

            events = input_file[itree].arrays(list(variables), outputtype=pandas.DataFrame)

            if 'jetFakes' in ifile:
                iso_branch = 'is_antiTauIso'
            else:
                iso_branch = 'is_signal'
            general_selection = events[(events[iso_branch] > 0)]

            # remove ttbar/diboson contamination to embedded sample
            if args.embed:
                general_selection = general_selection[(general_selection['contamination'] == 0)]

            # categorize
            zero_jet_events = general_selection[general_selection['njets'] == 0]
            boosted_events = general_selection[
                (general_selection['njets'] == 1) |
                ((general_selection['njets'] > 1) & (general_selection['mjj'] < 300))
            ]
            vbf_events = general_selection[(general_selection['njets'] > 1) & (general_selection['mjj'] > 300)]

            for variable, bins in config_variables.iteritems():
                # build list of processes
                inclusive_processes, inclusive_hists = fill_process_list(general_selection, name, variable, bins, boilerplate, output_file,
                                                                         '{}_inclusive/{}'.format(channel_prefix, variable), args.syst)

                zero_jet_processes, zero_jet_hists = fill_process_list(zero_jet_events, name, variable, bins, boilerplate, output_file,
                                                                     '{}_0jet/{}'.format(channel_prefix, variable), args.syst)

                boosted_processes, boosted_hists = fill_process_list(boosted_events, name, variable, bins, boilerplate, output_file,
                                                                     '{}_boosted/{}'.format(channel_prefix, variable), args.syst)

                vbf_processes, vbf_hists = fill_process_list(vbf_events, name, variable, bins, boilerplate, output_file,
                                                             '{}_vbf/{}'.format(channel_prefix, variable), args.syst)

                # start all processes and wait for all to finish
                for proc in inclusive_processes + zero_jet_processes + boosted_processes + vbf_processes:
                    proc.start()
                for proc in inclusive_processes + zero_jet_processes + boosted_processes + vbf_processes:
                    proc.join()

                # grab everything from the Queue's and write to the TFile
                output_file.cd('{}_inclusive/{}'.format(channel_prefix, variable))
                for obj in inclusive_hists.itervalues():
                    obj['config'].queue.get().Write()

                output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                for obj in zero_jet_hists.itervalues():
                    obj['config'].queue.get().Write()

                output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                for obj in boosted_hists.itervalues():
                    obj['config'].queue.get().Write()

                output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                for obj in vbf_hists.itervalues():
                    obj['config'].queue.get().Write()

    output_file.Close()
    print 'Finished in {} seconds'.format(time.time() - start)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--syst', '-s', action='store_true', help='run with systematics')
    parser.add_argument('--embed', '-e', action='store_true', help='use embedded instead of MC')
    parser.add_argument('--year', '-y', required=True, action='store', help='year being processed')
    parser.add_argument('--input-dir', '-i', required=True, action='store', dest='input_dir', help='path to files')
    parser.add_argument('--date', '-d', required=True, action='store', help='today\'s date for output name')
    parser.add_argument('--suffix', action='store', default='', help='suffix for filename')
    parser.add_argument('--config', '-c', action='store', default=None, required=True, help='config for binning, etc.')
    main(parser.parse_args())
