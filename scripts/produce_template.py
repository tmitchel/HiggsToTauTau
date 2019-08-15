import json
import ROOT
import time
import numpy
import pandas
import uproot
from glob import glob
from array import array
from pprint import pprint

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


def build_histogram(name, x_bins, y_bins):
    return ROOT.TH2F(name, name, len(x_bins) - 1, array('d', x_bins), len(y_bins) - 1, array('d', y_bins))


def fill_hist(data, xvar, yvar, hist, ac_weight=None):
    evtwt = data['evtwt'].values if ac_weight == None else (data['evtwt'] * data[ac_weight]).values
    xvar = data[xvar].values
    yvar = data[yvar].values
    for i in xrange(len(data.index)):
        hist.Fill(xvar[i], yvar[i], evtwt[i])
    return hist


def fill_vbf_subcat_hists(data, xvar, yvar, zvar, hists, edges, ac_weight=None):
    evtwt = data['evtwt'].values if ac_weight == None else (data['evtwt'] * data[ac_weight]).values
    xvar = data[xvar].values
    yvar = data[yvar].values
    zvar = data[zvar].values
    for i in xrange(len(data.index)):
        for j, edge in enumerate(edges[1:]):  # remove lowest left edge
            if zvar[i] < edge:
                hists[j].Fill(xvar[i], yvar[i], evtwt[i])
                break

    return hists

# dirty fix for now. Refactor later
def fill_fake_hist_with_vbf_cat(data, xvar, yvar, zvar, hist, edges, fake_fractions, fake_weights, syst=None, local=False):
    # get event data
    columns = data[['evtwt', xvar, yvar, 't1_pt', 't1_decayMode', 'njets', 'vis_mass', 'mt', 'mu_iso', zvar]].values
    evtwt, xvar, yvar = columns[:, 0], columns[:, 1], columns[:, 2]
    t1_pt, t1_decayMode, njets = columns[:, 3], columns[:, 4], columns[:, 5]
    vis_mass, mt, iso = columns[:, 6], columns[:, 7], columns[:, 8]
    zvar = columns[:, 9]

    # get fake fractions
    frac_data = fake_fractions[0]
    frac_qcd = fake_fractions[1]
    # frac_real = fake_fractions[2]
    frac_tt = fake_fractions[3]
    frac_w = fake_fractions[4]

    for i in xrange(len(data.index)):
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
        
        for j, edge in enumerate(edges[1:]):  # remove lowest left edge
            if zvar[i] < edge:
                hist[j].Fill(xvar[i], yvar[i], evtwt[i] * fake_weight)
                break
        # hist.Fill(xvar[i], yvar[i], evtwt[i] * fake_weight)
    return hist

def fill_fake_hist(data, xvar, yvar, hist, fake_fractions, fake_weights, syst=None, local=False):
    # get event data
    columns = data[['evtwt', xvar, yvar, 't1_pt', 't1_decayMode', 'njets', 'vis_mass', 'mt', 'mu_iso']].values
    evtwt, xvar, yvar = columns[:, 0], columns[:, 1], columns[:, 2]
    t1_pt, t1_decayMode, njets = columns[:, 3], columns[:, 4], columns[:, 5]
    vis_mass, mt, iso = columns[:, 6], columns[:, 7], columns[:, 8]

    # get fake fractions
    frac_data = fake_fractions[0]
    frac_qcd = fake_fractions[1]
    # frac_real = fake_fractions[2]
    frac_tt = fake_fractions[3]
    frac_w = fake_fractions[4]

    for i in xrange(len(data.index)):
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
        hist.Fill(xvar[i], yvar[i], evtwt[i] * fake_weight)
    return hist


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
    with open('scripts/boilerplate.json', 'r') as config_file:
        boilerplate = json.load(config_file)

    with open('scripts/binning.json', 'r') as config_file:
        config = json.load(config_file)
        config = config[args.config]
        decay_mode_bins = config['decay_mode_bins']
        vis_mass_bins = config['vis_mass_bins']
        higgs_pT_bins_boost = config['higgs_pT_bins_boost']
        m_sv_bins_boost = config['m_sv_bins_boost']
        vbf_cat_x_var, vbf_cat_x_bins = config['vbf_cat_x_bins']
        vbf_cat_y_var, vbf_cat_y_bins = config['vbf_cat_y_bins']
        vbf_cat_edge_var, vbf_cat_edges = config['vbf_cat_edges']

    # channel_prefix = args.tree_name.replace('_tree', '')  # need prefix for things later
    channel_prefix = args.tree_name.replace('mt_tree', 'mt')
    channel_prefix = channel_prefix.replace('et_tree', 'et')
    assert channel_prefix == 'mt' or channel_prefix == 'et', 'must provide a valid tree name'
    files = [ifile for ifile in glob('{}/*.root'.format(args.input_dir))]  # get files to process

    # get things for output file name
    ztt_name = 'emb' if args.embed else 'ztt'
    syst_name = 'Sys' if args.syst else 'noSys'

    output_file = ROOT.TFile('Output/templates/2D_htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
                                                                                  ztt_name, syst_name, args.date, args.suffix), 'RECREATE')

    # create structure within output file
    for cat in boilerplate['categories']:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
    output_file.cd()

    # Preload the fake fractions and fake factor weights.
    fake_fractions = load_fake_fractions(args.fake_file)

    if args.local:
        fake_weights = None
    else:
        fake_weights = load_fake_factor_weights(
            '../HTTutilities/Jet2TauFakes/data{}/SM{}/tight/vloose/{}/fakeFactors.root'.format(args.year, args.year, channel_prefix))

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
        stable_name = name
        print name
        input_file = uproot.open(ifile)
        trees = [ikey.replace(';1', '') for ikey in input_file.keys()
                 if 'tree' in ikey] if args.syst else [args.tree_name]
        for itree in trees:
            if itree != args.tree_name:
                name = stable_name + boilerplate['syst_name_map'][itree.replace(args.tree_name, '')]
            else:
                name = stable_name

            # adjust madgraph naming for later
            if '_madgraph' in name:
                name = name.replace(stable_name, boilerplate['noweighting_name_map'])

            # get data naming correct
            if 'Data' in name:
                name = 'data_obs'

            # handle MC vs embedded name
            if 'embed' in ifile:
                name = name.replace('embedded', 'ZTT')

            variables = set([
                'is_signal', 'is_antiTauIso', 'OS', 'nbjets', 'njets', 'mjj', 'evtwt', 'wt_*',
                'mu_iso', 'el_iso', 't1_decayMode', 'vis_mass', 't1_pt', 'higgs_pT', 'm_sv',
                'D0_VBF', 'D0_ggH', 'DCP_VBF', 'DCP_ggH', 'j1_phi', 'j2_phi', 'mt', 'mu_pt', 'el_pt',
                vbf_cat_x_var, vbf_cat_y_var, vbf_cat_edge_var
            ])

            events = input_file[itree].arrays(list(variables), outputtype=pandas.DataFrame)

            general_selection = events[
                (events['mt'] < 50) & (events['nbjets'] == 0)
            ]

            # do signal categorization
            signal_events = general_selection[general_selection['is_signal'] > 0]

            zero_jet_events = signal_events[signal_events['njets'] == 0]
            boosted_events = signal_events[
                (signal_events['njets'] == 1) |
                ((signal_events['njets'] > 1) & signal_events['mjj'] < 300)
            ]
            vbf_events = signal_events[(signal_events['njets'] > 1) & (signal_events['mjj'] > 300)]

            # I'm thinking the following part can be made parallel. So filling the 3+ histograms can happen in
            # in parallel. I think that would speed things up tremendously. Especially, if I apply the same thing
            # to reweighting to different scenarios in parallel and doing fake factor systematics in parallel. In
            # general, I like this approach much more than the C++ one I had. AND! It's faster than the C++ version.

            # start with 0-jet category
            output_file.cd('{}_0jet'.format(channel_prefix))
            zero_jet_hist = build_histogram(name, decay_mode_bins, vis_mass_bins)
            fill_hist(zero_jet_events, 't1_decayMode', 'vis_mass', zero_jet_hist)

            # now boosted category
            output_file.cd('{}_boosted'.format(channel_prefix))
            boost_hist = build_histogram(name, higgs_pT_bins_boost, m_sv_bins_boost)
            fill_hist(boosted_events, 'higgs_pT', 'm_sv', boost_hist)

            # vbf category is last
            output_file.cd('{}_vbf'.format(channel_prefix))
            vbf_hist = build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins)
            fill_hist(vbf_events, vbf_cat_x_var, vbf_cat_y_var, vbf_hist)

            # vbf sub-categories event after normal vbf categories
            vbf_cat_hists = []
            for cat in boilerplate['vbf_sub_cats']:
                output_file.cd('{}_{}'.format(channel_prefix, cat))
                vbf_cat_hists.append(build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins))
            fill_vbf_subcat_hists(vbf_events, vbf_cat_x_var, vbf_cat_y_var,
                                  vbf_cat_edge_var, vbf_cat_hists, vbf_cat_edges)

            # write then reset histograms
            output_file.Write()

            if '_JHU' in name:
                for weight in get_ac_weights(name, boilerplate['ac_reweighting_map']):
                    print 'Reweighting sample {} to {}'.format(name, weight[1])
                    # start with 0-jet category
                    output_file.cd('{}_0jet'.format(channel_prefix))
                    zero_jet_hist = build_histogram(weight[1], decay_mode_bins, vis_mass_bins)
                    fill_hist(zero_jet_events, 't1_decayMode', 'vis_mass', zero_jet_hist, weight[0])

                    # now boosted category
                    output_file.cd('{}_boosted'.format(channel_prefix))
                    boost_hist = build_histogram(weight[1], higgs_pT_bins_boost, m_sv_bins_boost,)
                    fill_hist(boosted_events, 'higgs_pT', 'm_sv', boost_hist, weight[0])

                    # vbf category is last
                    output_file.cd('{}_vbf'.format(channel_prefix))
                    vbf_hist = build_histogram(weight[1], vbf_cat_x_bins, vbf_cat_y_bins)
                    fill_hist(vbf_events, vbf_cat_x_var, vbf_cat_y_var, vbf_hist, weight[0])

                    # vbf sub-categories event after normal vbf categories
                    vbf_cat_hists = []
                    for cat in boilerplate['vbf_sub_cats']:
                        output_file.cd('{}_{}'.format(channel_prefix, cat))
                        vbf_cat_hists.append(build_histogram(weight[1], vbf_cat_x_bins, vbf_cat_y_bins))
                    fill_vbf_subcat_hists(vbf_events, vbf_cat_x_var, vbf_cat_y_var,
                                          vbf_cat_edge_var, vbf_cat_hists, vbf_cat_edges, weight[0])
                    output_file.Write()

            # do anti-iso categorization for fake-factor using data
            if 'data' in ifile.lower():
                print 'making fake factor hists'
                antiIso_events = general_selection[general_selection['is_antiTauIso'] > 0]
                fake_zero_jet_events = antiIso_events[antiIso_events['njets'] == 0]
                fake_boosted_events = antiIso_events[
                    (antiIso_events['njets'] == 1) |
                    ((antiIso_events['njets'] > 1) & antiIso_events['mjj'] < 300)
                ]
                fake_vbf_events = antiIso_events[(antiIso_events['njets'] > 1) & (antiIso_events['mjj'] > 300)]

                output_file.cd('{}_0jet'.format(channel_prefix))
                zero_jet_hist = build_histogram('jetFakes', decay_mode_bins, vis_mass_bins)
                zero_jet_hist = fill_fake_hist(fake_zero_jet_events, 't1_decayMode',
                                               'vis_mass', zero_jet_hist, fake_fractions['{}_0jet'.format(channel_prefix)], fake_weights, local=args.local)

                output_file.cd('{}_boosted'.format(channel_prefix))
                boost_hist = build_histogram('jetFakes', higgs_pT_bins_boost, m_sv_bins_boost)
                boost_hist = fill_fake_hist(fake_boosted_events, 'higgs_pT',
                                            'm_sv', boost_hist, fake_fractions['{}_boosted'.format(channel_prefix)], fake_weights, local=args.local)

                output_file.cd('{}_vbf'.format(channel_prefix))
                vbf_hist = build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins)
                vbf_hist = fill_fake_hist(fake_vbf_events, vbf_cat_x_var, vbf_cat_y_var,
                                          vbf_hist, fake_fractions['{}_vbf'.format(channel_prefix)], fake_weights, local=args.local)
                
                # vbf sub-categories event after normal vbf categories
                vbf_cat_hists = []
                for cat in boilerplate['vbf_sub_cats']:
                    output_file.cd('{}_{}'.format(channel_prefix, cat))
                    vbf_cat_hists.append(build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins))
                fill_fake_hist_with_vbf_cat(fake_vbf_events, vbf_cat_x_var, vbf_cat_y_var, vbf_cat_edge_var, vbf_cat_hists,
                                    vbf_cat_edges, fake_fractions['{}_vbf'.format(channel_prefix)], fake_weights, local=args.local)
                
                output_file.Write()

                if args.syst:
                    for syst in boilerplate['fake_factor_systematics']:
                        output_file.cd('{}_0jet'.format(channel_prefix))
                        zero_jet_hist = build_histogram(
                            'jetFakes_CMS_htt_{}'.format(syst), decay_mode_bins, vis_mass_bins)
                        zero_jet_hist = fill_fake_hist(fake_zero_jet_events, 't1_decayMode',
                                                       'vis_mass', zero_jet_hist, fake_fractions['{}_0jet'.format(channel_prefix)], fake_weights, syst, local=args.local)

                        output_file.cd('{}_boosted'.format(channel_prefix))
                        boost_hist = build_histogram('jetFakes_CMS_htt_{}'.format(syst),
                                                     higgs_pT_bins_boost, m_sv_bins_boost)
                        boost_hist = fill_fake_hist(fake_boosted_events, 'higgs_pT',
                                                    'm_sv', boost_hist, fake_fractions['{}_boosted'.format(channel_prefix)], fake_weights, syst, local=args.local)

                        output_file.cd('{}_vbf'.format(channel_prefix))
                        vbf_hist = build_histogram('jetFakes_CMS_htt_{}'.format(syst), vbf_cat_x_bins, vbf_cat_y_bins)
                        vbf_hist = fill_fake_hist(fake_vbf_events, vbf_cat_x_var, vbf_cat_y_var,
                                                  vbf_hist, fake_fractions['{}_vbf'.format(channel_prefix)], fake_weights, syst, local=args.local)

                        # vbf sub-categories event after normal vbf categories
                        vbf_cat_hists = []
                        for cat in boilerplate['vbf_sub_cats']:
                            output_file.cd('{}_{}'.format(channel_prefix, cat))
                            vbf_cat_hists.append(build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins))
                        fill_fake_hist_with_vbf_cat(fake_vbf_events, vbf_cat_x_var, vbf_cat_y_var, vbf_cat_edge_var, vbf_cat_hists,
                                            vbf_cat_edges, fake_fractions['{}_vbf'.format(channel_prefix)], fake_weights, syst=syst, local=args.local)
                        output_file.Write()

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
    main(parser.parse_args())
