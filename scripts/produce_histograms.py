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
    def __init__(self, name, data, variable, bins):
        self.name = name
        self.data = data
        self.xvar_name = variable
        self.bins = bins
        self.queue = None
        self.fake_weight = None
        self.hists = None

    def __deepcopy__(self, memo):
      cp = Config(deepcopy(self.name, memo), deepcopy(self.data, memo), deepcopy(self.xvar_name, memo), deepcopy(self.bins, memo))
      cp.fake_weight = deepcopy(self.fake_weight, memo)
      cp.queue = None
      cp.hists = deepcopy(self.hists, memo)
      return cp

    def submit(self):
        self.queue = Queue()
        return {'config': self}


def build_histogram(name, bins, output_file, directory):
    output_file.cd(directory)
    return ROOT.TH1F(name, name, bins[0], bins[1], bins[2])


def fill_histograms(config):
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
    all_hists = {}
    config = Config(name, data, variable, bins)

    if 'jetFakes' in name:
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

    # channel_prefix = args.tree_name.replace('_tree', '')  # need prefix for things later
    channel_prefix = args.tree_name.replace('mt_tree', 'mt')
    channel_prefix = channel_prefix.replace('et_tree', 'et')
    assert channel_prefix == 'mt' or channel_prefix == 'et', 'must provide a valid tree name'
    files = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]  # get files to process

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
                 if 'tree' in ikey] if args.syst else [args.tree_name]
        for itree in trees:
            if itree != args.tree_name:
                name = ifile.replace('.root', '').split(
                    '/')[-1] + boilerplate['syst_name_map'][itree.replace(args.tree_name, '')]
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
            if 'jetFakes' in ifile:
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

            zero_jet_events = general_selection[general_selection['njets'] == 0]
            boosted_events = general_selection[
                (general_selection['njets'] == 1) |
                ((general_selection['njets'] > 1) & (general_selection['mjj'] < 300))
            ]
            vbf_events = general_selection[(general_selection['njets'] > 1) & (general_selection['mjj'] > 300)]

            # I'm thinking the following part can be made parallel. So filling the 3+ histograms can happen in
            # in parallel. I think that would speed things up tremendously. Especially, if I apply the same thing
            # to reweighting to different scenarios in parallel and doing fake factor systematics in parallel. In
            # general, I like this approach much more than the C++ one I had. AND! It's faster than the C++ version.

            for variable, bins in config_variables.iteritems():

                inclusive_processes, inclusive_hists = fill_process_list(general_selection, name, variable, bins, boilerplate, output_file,
                                                                         '{}_inclusive/{}'.format(channel_prefix, variable), args.syst)

                zero_jet_processes, zero_jet_hists = fill_process_list(zero_jet_events, name, variable, bins, boilerplate, output_file,
                                                                     '{}_0jet/{}'.format(channel_prefix, variable), args.syst)

                boosted_processes, boosted_hists = fill_process_list(boosted_events, name, variable, bins, boilerplate, output_file,
                                                                     '{}_boosted/{}'.format(channel_prefix, variable), args.syst)

                vbf_processes, vbf_hists = fill_process_list(vbf_events, name, variable, bins, boilerplate, output_file,
                                                             '{}_vbf/{}'.format(channel_prefix, variable), args.syst)

                for proc in inclusive_processes + zero_jet_processes + boosted_processes + vbf_processes:
                    proc.start()
                for proc in inclusive_processes + zero_jet_processes + boosted_processes + vbf_processes:
                    proc.join()

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
    parser.add_argument('--tree-name', '-t', required=True, action='store', dest='tree_name', help='name of input tree')
    parser.add_argument('--date', '-d', required=True, action='store', help='today\'s date for output name')
    parser.add_argument('--suffix', action='store', default='', help='suffix for filename')
    parser.add_argument('--config', '-c', action='store', default=None, required=True, help='config for binning, etc.')
    main(parser.parse_args())
