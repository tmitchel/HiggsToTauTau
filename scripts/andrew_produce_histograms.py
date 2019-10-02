import json
import ROOT
import time
import numpy
import pandas
import uproot
from glob import glob
from array import array
from pprint import pprint
from multiprocessing import Process, Queue


import signal
import sys
fake_weights = None


def signal_handler(sig, frame):
    print('Trying to close {}'.format(fake_weights))
    if fake_weights != None:
        fake_weights.Delete()
    sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)


def get_ac_weights(name, ac_reweighting_map):
    if 'ggh' in name.lower():
        return ac_reweighting_map['ggh']
    elif 'vbf' in name.lower():
        return ac_reweighting_map['vbf']
    elif 'wh' in name.lower():
        return ac_reweighting_map['wh']
    elif 'zh' in name.lower():
        return ac_reweighting_map['zh']


def build_histogram(name, bins):
    return ROOT.TH1F(name, name, bins[0], bins[1], bins[2])


def fill_histograms(queue, data, hists, xvar_name, zvar_name=None, edges=None, ac_weights=None, fake_fractions=None, fake_weights=None, syst=None, local=None):
    # get common variables
    evtwt = data['evtwt'].values if ac_weights == None else (data['evtwt'] * data[ac_weights]).values
    xvar = data[xvar_name].values
    zvar = data[zvar_name].values if zvar_name != None else None

    # load fake weights/fractions if needed
    if fake_fractions != None and fake_weights != None:
        columns = data[['t1_pt', 't1_decayMode', 'njets', 'vis_mass', 'mt', 'mu_iso']].values
        t1_pt, t1_decayMode, njets = columns[:, 0], columns[:, 1], columns[:, 2]
        vis_mass, mt, iso = columns[:, 3], columns[:, 4], columns[:, 5]
        # get fake fractions
        frac_data = fake_fractions[0]
        frac_qcd = fake_fractions[1]
        frac_tt = fake_fractions[3]
        frac_w = fake_fractions[4]

    # loop over all events
    for i in xrange(len(data.index)):
        fake_weight = 1.
        if fake_fractions != None and fake_weights != None:
            xbin, ybin = -1, -1
            for x, bin in enumerate(frac_data.bins[0]):
                if vis_mass[i] < bin[1] and vis_mass[i] > bin[0]:
                    xbin = x
                    break
            for y, bin in enumerate(frac_data.bins[1]):
                if njets[i] < bin[1] and njets[i] > bin[0]:
                    ybin = y
                    break

            # I guess local and Wisc have different versions of uproot?
            if not local:
                xbin, ybin = ybin, xbin

            # make fake-weight input
            inputs = [
                t1_pt[i], t1_decayMode[i], njets[i], vis_mass[i], mt[i], iso[i],
                frac_qcd.values[xbin][ybin],
                frac_w.values[xbin][ybin],
                frac_tt.values[xbin][ybin]
            ]

            if local:
                fake_weight = 1.  # for testing the rest
            else:
                fake_weight = fake_weights.value(
                    9, array('d', inputs)) if syst == None else fake_weights.value(9, array('d', inputs), syst)
        # do vbf sub-categorization if needed
        if zvar_name != None:
            for j, edge in enumerate(edges[1:]):  # remove lowest left edge
                if zvar[i] < edge:
                    hists[j].Fill(xvar[i], evtwt[i]*fake_weight)
                    break
        else:
            hists.Fill(xvar[i], evtwt[i]*fake_weight)

    queue.put(hists)


def load_fake_factor_weights(fake_file):
    ff_file = ROOT.TFile(fake_file, 'READ')
    return ff_file.Get('ff_comb')


def load_fake_fractions(input_file):
    ifile = uproot.open(input_file)
    return {cat.replace(';1', ''): [ifile[cat][ihist] for ihist in sorted(ifile[cat].keys())] for cat in ifile.keys()}


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
    channel_prefix = args.tree_name.replace('mt_Selected', 'mt')
    channel_prefix = channel_prefix.replace('et_Selected', 'et')
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

    # Preload the fake fractions and fake factor weights.
    fake_fractions = load_fake_fractions(args.fake_file)

    if args.local:
        fake_weights = None
    else:
        random_ext = '_tight' if channel_prefix == 'et' and args.year == "2016" else ''
        fake_weights = load_fake_factor_weights(
            '../HTTutilities/Jet2TauFakes/data{}/SM{}/tight/vloose/{}/fakeFactors{}.root'.format(args.year, args.year, channel_prefix, random_ext))

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
                 if 'Selected' in ikey] if args.syst else [args.tree_name]
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
                'njets', 'mjj', 'FinalWeighting',
            ] + config_variables.keys() + [zvars[0]])

            events = input_file[itree].arrays(list(variables), outputtype=pandas.DataFrame)

            general_selection = events[
                (events['pt_2'] > 30)
            ]

            # do signal categorization
            signal_events = general_selection

            zero_jet_events = signal_events[signal_events['njets'] == 0]
            boosted_events = signal_events[
                (signal_events['njets'] == 1) |
                ((signal_events['njets'] > 1) & (signal_events['mjj'] < 300))
            ]
            vbf_events = signal_events[(signal_events['njets'] > 1) & (signal_events['mjj'] > 300)]

            # I'm thinking the following part can be made parallel. So filling the 3+ histograms can happen in
            # in parallel. I think that would speed things up tremendously. Especially, if I apply the same thing
            # to reweighting to different scenarios in parallel and doing fake factor systematics in parallel. In
            # general, I like this approach much more than the C++ one I had. AND! It's faster than the C++ version.

            for variable, bins in config_variables.iteritems():

                # handle just 0jet category first so we aren't moving around a ton of TDirectory's

                # build the histograms
                output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                zero_jet_hists = {}
                zero_jet_hists['nominal'] = {
                    'data': zero_jet_events,
                    'hists': build_histogram(name, bins),
                    'xvar_name': variable,
                    'queue': Queue()
                }

                zero_jet_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in zero_jet_hists.iteritems()
                ]

                # build the histograms
                output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                boosted_hists = {}
                boosted_hists['nominal'] = {
                    'data': boosted_events,
                    'hists': build_histogram(name, bins),
                    'xvar_name': variable,
                    'queue': Queue()
                }

                boosted_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in boosted_hists.iteritems()
                ]

                # build the histograms
                output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                vbf_hists = {}
                vbf_hists['nominal'] = {
                    'data': vbf_events,
                    'hists': build_histogram(name, bins),
                    'xvar_name': variable,
                    'queue': Queue()
                }

                vbf_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in vbf_hists.iteritems()
                ]

                for proc in zero_jet_processes + boosted_processes + vbf_processes:
                    proc.start()
                for proc in zero_jet_processes + boosted_processes + vbf_processes:
                    proc.join()
                
                output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                for obj in zero_jet_hists.itervalues():
                    obj['queue'].get().Write()

                output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                for obj in boosted_hists.itervalues():
                    obj['queue'].get().Write()

                output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                for obj in vbf_hists.itervalues():
                    obj['queue'].get().Write()
        # break
    output_file.Close()
    if not args.local:
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
    parser.add_argument('--config', '-c', action='store', default=None, required=True, help='config for binning, etc.')
    parser.add_argument('--do-subcat', action='store_true', dest='do_subcat',
                        help='fill histograms in vbf subcategories')
    main(parser.parse_args())
