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


def fill_histograms(queue, data, hists, xvar_name, zvar_name=None, edges=None, ac_weights=None, fake_weight=None):
    # get common variables
    evtwt = data['evtwt'].values if ac_weights == None else (data['evtwt'] * data[ac_weights]).values
    xvar = data[xvar_name].values
    zvar = data[zvar_name].values if zvar_name != None else None

    if fake_weight != None:
        evtwt *= data[fake_weight].values

    # loop over all events
    for i in xrange(len(data.index)):
        # do vbf sub-categorization if needed
        if zvar_name != None:
            for j, edge in enumerate(edges[1:]):  # remove lowest left edge
                if zvar[i] < edge:
                    hists[j].Fill(xvar[i], evtwt[i])
                    break
        else:
            hists.Fill(xvar[i], evtwt[i])

    queue.put(hists)


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
                'is_signal', 'is_antiTauIso', 'OS', 'nbjets', 'njets', 'mjj', 'evtwt', 'wt_*',
                'mu_iso', 'el_iso', 't1_decayMode', 'vis_mass', 't1_pt', 'higgs_pT', 'm_sv', 'mt'
            ] + config_variables.keys() + [zvars[0]])

            # get fake factor weights if needed
            if 'jetFakes' in ifile:
                variables.add('fake_weight')
                if args.syst:
                    variables.add('ff_*')

            events = input_file[itree].arrays(list(variables), outputtype=pandas.DataFrame)

            general_selection = events[
                (events['mt'] < 50) & (events['nbjets'] == 0)
            ]

            # do signal categorization
            signal_events = general_selection[(general_selection['is_signal'] > 0)]
            fake_events = general_selection[(general_selection['is_antiTauIso'] > 0)]

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

                # handle just inclusive category first so we aren't moving around a ton of TDirectory's
                # build the histograms
                output_file.cd('{}_inclusive/{}'.format(channel_prefix, variable))
                inclusive_hists = {}
                if 'jetFakes' in ifile:
                    print 'making 0jet fake factor hists'
                    fake_events = fake_events[
                        (general_selection['njets'] == 0)
                    ]

                    inclusive_hists['jetFakes'] = {
                        'data': fake_events,
                        'hists': build_histogram('jetFakes', bins),
                        'xvar_name': variable,
                        'fake_weight': 'fake_weight',
                        'queue': Queue()
                    }
                    if args.syst:
                        for syst in boilerplate['fake_factor_systematics']:
                            inclusive_hists['jetFakes_CMS_htt_{}'.format(syst)] = {
                                'data': fake_events,
                                'hists': build_histogram('jetFakes_CMS_htt_{}'.format(syst), bins),
                                'xvar_name': variable,
                                'fake_weight': syst,
                                'queue': Queue()
                            }
                else:
                    inclusive_hists['nominal'] = {
                        'data': signal_events,
                        'hists': build_histogram(name, bins),
                        'xvar_name': variable,
                        'queue': Queue()
                    }
                    if '_JHU' in name:
                        for weight in get_ac_weights(name, boilerplate['jhu_ac_reweighting_map']):
                            output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                            inclusive_hists[weight[1]] = {
                                'data': signal_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }
                    elif '_madgraph' in name and not 'vbf' in name:
                        for weight in get_ac_weights(name, boilerplate['mg_ac_reweighting_map']):
                            output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                            inclusive_hists[weight[1]] = {
                                'data': signal_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }

                inclusive_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in inclusive_hists.iteritems()
                ]

                # build the histograms
                output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                zero_jet_hists = {}
                if 'jetFakes' in ifile:
                    print 'making 0jet fake factor hists'
                    fake_zero_jet_events = fake_events[
                        (general_selection['njets'] == 0)
                    ]

                    zero_jet_hists['jetFakes'] = {
                        'data': fake_zero_jet_events,
                        'hists': build_histogram('jetFakes', bins),
                        'xvar_name': variable,
                        'fake_weight': 'fake_weight',
                        'queue': Queue()
                    }
                    if args.syst:
                        for syst in boilerplate['fake_factor_systematics']:
                            zero_jet_hists['jetFakes_CMS_htt_{}'.format(syst)] = {
                                'data': fake_zero_jet_events,
                                'hists': build_histogram('jetFakes_CMS_htt_{}'.format(syst), bins),
                                'xvar_name': variable,
                                'fake_weight': syst,
                                'queue': Queue()
                            }
                else:
                    zero_jet_hists['nominal'] = {
                        'data': zero_jet_events,
                        'hists': build_histogram(name, bins),
                        'xvar_name': variable,
                        'queue': Queue()
                    }
                    if '_JHU' in name:
                        for weight in get_ac_weights(name, boilerplate['jhu_ac_reweighting_map']):
                            output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                            zero_jet_hists[weight[1]] = {
                                'data': zero_jet_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }
                    elif '_madgraph' in name and not 'vbf' in name:
                        for weight in get_ac_weights(name, boilerplate['mg_ac_reweighting_map']):
                            output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                            zero_jet_hists[weight[1]] = {
                                'data': zero_jet_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }

                zero_jet_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in zero_jet_hists.iteritems()
                ]

                # build the histograms
                output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                boosted_hists = {}
                if 'jetFakes' in ifile:
                    print 'making boosted fake factor hists'
                    fake_boosted_events = general_selection[
                        ((general_selection['njets'] == 1) |
                         ((general_selection['njets'] > 1) & (general_selection['mjj'] < 300)))
                    ]

                    boosted_hists['jetFakes'] = {
                        'data': fake_boosted_events,
                        'hists': build_histogram('jetFakes', bins),
                        'xvar_name': variable,
                        'fake_weight': 'fake_weight',
                        'queue': Queue()
                    }
                    if args.syst:
                        for syst in boilerplate['fake_factor_systematics']:
                            boosted_hists['jetFakes_CMS_htt_{}'.format(syst)] = {
                                'data': fake_boosted_events,
                                'hists': build_histogram('jetFakes_CMS_htt_{}'.format(syst), bins),
                                'xvar_name': variable,
                                'fake_weights': syst,
                                'queue': Queue()
                            }
                else:
                    boosted_hists['nominal'] = {
                        'data': boosted_events,
                        'hists': build_histogram(name, bins),
                        'xvar_name': variable,
                        'queue': Queue()
                    }
                    if '_JHU' in name:
                        for weight in get_ac_weights(name, boilerplate['jhu_ac_reweighting_map']):
                            output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                            boosted_hists[weight[1]] = {
                                'data': boosted_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }
                    elif '_madgraph' in name and not 'vbf' in name:
                        for weight in get_ac_weights(name, boilerplate['mg_ac_reweighting_map']):
                            output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                            boosted_hists[weight[1]] = {
                                'data': boosted_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }

                boosted_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in boosted_hists.iteritems()
                ]

                # build the histograms
                output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                vbf_hists = {}
                if 'jetFakes' in ifile:
                    print 'making vbf fake factor hists'
                    fake_vbf_events = general_selection[
                        (general_selection['njets'] > 1) &
                        (general_selection['mjj'] > 300)
                    ]

                    vbf_hists['jetFakes'] = {
                        'data': fake_vbf_events,
                        'hists': build_histogram('jetFakes', bins),
                        'xvar_name': variable,
                        'fake_weight': 'fake_weight',
                        'queue': Queue()
                    }
                    if args.syst:
                        for syst in boilerplate['fake_factor_systematics']:
                            vbf_hists['jetFakes_CMS_htt_{}'.format(syst)] = {
                                'data': fake_vbf_events,
                                'hists': build_histogram('jetFakes_CMS_htt_{}'.format(syst), bins),
                                'xvar_name': variable,
                                'fake_weight': syst,
                                'queue': Queue()
                            }
                else:
                    vbf_hists['nominal'] = {
                        'data': vbf_events,
                        'hists': build_histogram(name, bins),
                        'xvar_name': variable,
                        'queue': Queue()
                    }
                    if '_JHU' in name:
                        for weight in get_ac_weights(name, boilerplate['jhu_ac_reweighting_map']):
                            output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                            vbf_hists[weight[1]] = {
                                'data': vbf_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }
                    elif '_madgraph' in name and not 'vbf' in name:
                        for weight in get_ac_weights(name, boilerplate['mg_ac_reweighting_map']):
                            output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                            vbf_hists[weight[1]] = {
                                'data': vbf_events,
                                'hists': build_histogram(weight[1], bins),
                                'xvar_name': variable,
                                'ac_weights': weight[0],
                                'queue': Queue()
                            }

                vbf_processes = [
                    Process(target=fill_histograms, kwargs=proc_args, name=proc_name) for proc_name, proc_args in vbf_hists.iteritems()
                ]

                for proc in inclusive_processes + zero_jet_processes + boosted_processes + vbf_processes:
                    proc.start()
                for proc in inclusive_processes + zero_jet_processes + boosted_processes + vbf_processes:
                    proc.join()

                output_file.cd('{}_inclusive/{}'.format(channel_prefix, variable))
                for obj in inclusive_hists.itervalues():
                    obj['queue'].get().Write()

                output_file.cd('{}_0jet/{}'.format(channel_prefix, variable))
                for obj in zero_jet_hists.itervalues():
                    obj['queue'].get().Write()

                output_file.cd('{}_boosted/{}'.format(channel_prefix, variable))
                for obj in boosted_hists.itervalues():
                    obj['queue'].get().Write()

                output_file.cd('{}_vbf/{}'.format(channel_prefix, variable))
                for obj in vbf_hists.itervalues():
                    obj['queue'].get().Write()

    output_file.Close()
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
