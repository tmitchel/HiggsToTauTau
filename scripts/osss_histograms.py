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
        self.ac_weights = None
        self.hists = None

    def __deepcopy__(self, memo):
      cp = Config(deepcopy(self.name, memo), deepcopy(self.data, memo), deepcopy(self.xvar_name, memo), deepcopy(self.bins, memo))
      cp.fake_weight = deepcopy(self.fake_weight, memo)
      cp.ac_weights = deepcopy(self.ac_weights, memo)
      cp.queue = None
      cp.hists = deepcopy(self.hists, memo)
      return cp

    def submit(self):
        self.queue = Queue()
        return {'config': self}

def get_ac_weights(name, ac_reweighting_map):
    if 'ggh' in name.lower():
        return ac_reweighting_map['ggh']
    elif 'vbf' in name.lower():
        return ac_reweighting_map['vbf']
    elif 'wh' in name.lower():
        return ac_reweighting_map['wh']
    elif 'zh' in name.lower():
        return ac_reweighting_map['zh']


def build_histogram(name, bins, output_file, directory):
    output_file.cd(directory)
    return ROOT.TH1F(name, name, bins[0], bins[1], bins[2])


def fill_histograms(config):
    # get common variables
    evtwt = config.data['evtwt'].values if config.ac_weights == None else (
        config.data['evtwt'] * config.data[config.ac_weights]).values
    xvar = config.data[config.xvar_name].values

    # loop over all events
    for i in xrange(len(config.data.index)):
        config.hists.Fill(xvar[i], evtwt[i])

    config.queue.put(config.hists)


def fill_process_list(data, name, variable, bins, boilerplate, output_file, directory, year, doSyst=False):
    all_hists = {}
    config = Config(name, data, variable, bins)
    config.hists = build_histogram(name, bins, output_file, directory)

    all_hists['nominal'] = config.submit()
    if '_JHU' in name or '_madgraph' in name:
        if '_JHU' in name:
            ac_map = 'jhu_ac_reweighting_map'
        else:
            if year == '2018':
                ac_map = 'new_mg_ac_reweighting_map'
            else:
                ac_map = 'mg_ac_reweighting_map'

        for weight in get_ac_weights(name, boilerplate[ac_map]):
            ac_config = copy.deepcopy(config)
            ac_config.hists = build_histogram(weight[1], bins, output_file, directory)
            ac_config.ac_weights = weight[0]
            all_hists[weight[1]] = ac_config.submit()

    return [
        Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in all_hists.iteritems()
    ], all_hists


def fill_all_categories(events, hists, variable, isData):
    # get variables
    xvar = events[variable].values
    evtwt = events['evtwt'].values
    if not isData:
        evtwt *= -1

    # inclusive histograms
    hist = hists['inclusive'].Clone()
    for i in xrange(len(events.index)):
        hist.Fill(xvar[i], evtwt[i])
    hists['inclusive'] = hist

    # categorize
    zero_events = events[(events['njets'] == 0)]
    hist = hists['0jet'].Clone()
    for i in xrange(len(zero_events.index)):
        hist.Fill(xvar[i], evtwt[i])
    hists['0jet'] = hist

    boost_events = events[(events['njets'] == 1) | ((events['njets'] > 1) & (events['mjj'] < 300))]
    hist = hists['boosted'].Clone()
    for i in xrange(len(boost_events.index)):
        hist.Fill(xvar[i], evtwt[i])
    hists['boosted'] = hist

    vbf_events = events[(events['njets'] > 1) & (events['mjj'] > 300)]
    hist = hists['vbf'].Clone()
    for i in xrange(len(vbf_events.index)):
        hist.Fill(xvar[i], evtwt[i])
    hists['vbf'] = hist

    return hists


def osss_filler(config_variables, input_dir, tree_name, categories, output_file, boilerplate, channel_prefix):
    osss_bkg_files = ['embed', 'ZL', 'ZJ', 'TTJ', 'TTT', 'VVJ', 'VVT', 'W']
    osss_data_file = ['data_obs']
    base_variables = ['evtwt', 'OS', 'is_signal', 'is_antiLepIso', 'njets', 'mjj']

    for variable, bins in config_variables.iteritems():
        anti_os_hist = {}
        anti_ss_hist = {}
        loose_hist = {}
        signal_hist = {}
        for c in categories:
            output_file.cd('')
            anti_os_hist[c] = build_histogram('anti_os_qcd' + c, bins, output_file, "")
            anti_ss_hist[c] = build_histogram('anti_ss_qcd' + c, bins, output_file, "")
            loose_hist[c] = build_histogram('loose_qcd' + c, bins, output_file, "")
            signal_hist[c] = build_histogram('signal_qcd' + c, bins, output_file, "")

        for ifile in osss_bkg_files + osss_data_file:
            input_file = uproot.open('{}/{}.root'.format(input_dir, ifile))
            events = input_file[tree_name].arrays(base_variables + [variable], outputtype=pandas.DataFrame)

            output_file.cd("")

            antiiso_os_events = events[(events['is_signal'] == 0) & (events['is_antiLepIso'] > 0) & (events['OS'] > 0)]
            anti_os_hist = fill_all_categories(antiiso_os_events, anti_os_hist, variable, (ifile in osss_data_file))

            antiiso_ss_events = events[(events['is_signal'] == 0) & (events['is_antiLepIso'] > 0) & (events['OS'] == 0)]
            anti_ss_hist = fill_all_categories(antiiso_ss_events, anti_ss_hist, variable, (ifile in osss_data_file))

            loose_ss_events = events[(events['is_antiLepIso'] > 0) & (events['OS'] == 0)]
            loose_hist = fill_all_categories(loose_ss_events, loose_hist, variable, (ifile in osss_data_file))

            signal_ss_events = events[(events['is_signal'] > 0) & (events['OS'] == 0)]
            signal_hist = fill_all_categories(signal_ss_events, signal_hist, variable, (ifile in osss_data_file))

        for c in categories:
            osss_ratio = anti_os_hist[c].Integral(0, anti_os_hist[c].GetNbinsX() + 1) / anti_ss_hist[c].Integral(0, anti_ss_hist[c].GetNbinsX() + 1)
            scale = osss_ratio * (signal_hist[c].Integral(0, signal_hist[c].GetNbinsX() + 1) / loose_hist[c].Integral(0, loose_hist[c].GetNbinsX() + 1))
            output_file.cd('{}_{}/{}'.format(channel_prefix, c, variable))
            final_hist = loose_hist[c].Clone()
            final_hist.Scale(scale)
            for ibin in range(final_hist.GetNbinsX() + 1):
                if final_hist.GetBinContent(ibin) < 0:
                    final_hist.SetBinContent(ibin, 0)
            final_hist.SetName('QCD')
            final_hist.Write()

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

    osss_filler(config_variables, args.input_dir, args.tree_name, boilerplate['categories'], output_file, boilerplate, channel_prefix)

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
                'is_signal', 'is_antiLepIso', 'contamination', 'njets', 'mjj', 'evtwt', 'wt_*',
            ] + config_variables.keys() + [zvars[0]])

            events = input_file[itree].arrays(list(variables), outputtype=pandas.DataFrame)

            general_selection = events[(events['is_signal'] > 0)]

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
