import json
import ROOT
import time
import numpy
import pandas
import uproot
import datetime
from glob import glob
from array import array
from pprint import pprint


def build_filelist(input_dir):
    files = [ifile for ifile in glob('{}/*/*.root'.format(input_dir))]

    filelist = {'nominal': []}
    for fname in files:
        ifile = uproot.open(fname)

        if 'nominal' in fname:
            filelist['nominal'].append(fname)
        else:
            keyname = fname.split('/')[-2]
            filelist.setdefault(keyname, [])
            filelist[keyname].append(fname)
    return filelist


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


def fill_hists(data, hists, xvar_name, yvar_name, zvar_name=None, edges=None, ac_weight=None, fake_weight=None):
    evtwt = data['evtwt'].values if ac_weight == None else (data['evtwt'] * data[ac_weight]).values
    xvar = data[xvar_name].values
    yvar = data[yvar_name].values
    zvar = data[zvar_name].values if zvar_name != None else None

    if fake_weight != None:
        evtwt *= data[fake_weight].values

    for i in xrange(len(data.index)):
        if zvar_name != None:
            for j, edge in enumerate(edges[1:]):  # remove lowest left edge
                if zvar[i] < edge:
                    hists[j].Fill(xvar[i], yvar[i], evtwt[i])
                    break
        else:
            hists.Fill(xvar[i], yvar[i], evtwt[i])

    return hists


def get_syst_name(syst, syst_name_map):
    if syst == 'nominal':
        return ''
    elif syst in syst_name_map.keys():
        return syst_name_map[syst]
    else:
        print '[INFO]  {} is unknown. Skipping...'.format(syst)
        return 'unknown'

def main(args):
    start = time.time()
    config = {}
    boilerplate = {}
    with open('configs/boilerplate.json', 'r') as config_file:
        boilerplate = json.load(config_file)
    syst_name_map = boilerplate['syst_name_map']

    with open('configs/binning.json', 'r') as config_file:
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
    filelist = build_filelist(args.input_dir)
    assert len(filelist['nominal']) > 0, 'could\'nt locate any nominal files'

    # get things for output file name
    ztt_name = 'emb' if args.embed else 'ztt'
    syst_name = 'Sys' if args.syst else 'noSys'

    date = datetime.datetime.now().strftime("%B%d")
    output_file = ROOT.TFile('Output/templates/2D_htt_{}_{}_{}_fa3_{}_{}{}.root'.format(channel_prefix,
                                                                                        ztt_name, syst_name, args.year, date, args.suffix), 'RECREATE')

    # create structure within output file
    for cat in boilerplate['categories']:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
    output_file.cd()

    # use this once uproot supports sub-directories inside root files
    # output_file = uproot.recreate('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
    #                                                                               ztt_name, syst_name, args.date, '_'+args.suffix))

    for syst, files in filelist.iteritems():
        postfix = get_syst_name(syst, syst_name_map)
        if postfix == 'unknown': # skip unknown systematics
            continue

        postfix = postfix.replace('YEAR', args.year) # add correct year

        for ifile in files:
            # handle ZTT vs embedded
            if args.embed and 'ZTT' in ifile:
                continue
            elif not args.embed and 'embed' in ifile:
                continue

            name = ifile.replace('.root', '').split('/')[-1]
            print name + postfix
            input_file = uproot.open(ifile)
            trees = [args.tree_name]

            # get data naming correct
            if 'Data' in name:
                name = 'data_obs'

            # handle MC vs embedded name
            if 'embed' in ifile:
                name = name.replace('embedded', 'ZTT')
                name = name.replace('embed', 'ZTT')

            variables = set([
                'is_signal', 'is_antiTauIso', 'OS', 'nbjets', 'njets', 'mjj', 'evtwt', 'wt_*',
                'mu_iso', 'el_iso', 't1_decayMode', 'vis_mass', 't1_pt', 'higgs_pT', 'm_sv',
                'D0_VBF', 'D0_ggH', 'DCP_VBF', 'DCP_ggH', 'j1_phi', 'j2_phi', 'mt', 'mu_pt', 'el_pt',
                vbf_cat_x_var, vbf_cat_y_var, vbf_cat_edge_var
            ])

            # get fake factor weights if needed
            if 'jetFakes' in ifile:
                variables.add('fake_weight')
                if args.syst:
                    variables.add('ff_*')

            name = name + postfix # add systematic postfix to file name

            events = input_file[args.tree_name].arrays(list(variables), outputtype=pandas.DataFrame)

            general_selection = events[
                (events['mt'] < 50) & (events['nbjets'] == 0)
            ]

            if not 'jetFakes' in name:
                # do signal categorization
                signal_events = general_selection[general_selection['is_signal'] > 0]

                zero_jet_events = signal_events[signal_events['njets'] == 0]
                boosted_events = signal_events[
                    (signal_events['njets'] == 1) |
                    ((signal_events['njets'] > 1) & (signal_events['mjj'] < 300))
                ]
                vbf_events = signal_events[(signal_events['njets'] > 1) & (signal_events['mjj'] > 300)]

                # start with 0-jet category
                output_file.cd('{}_0jet'.format(channel_prefix))
                zero_jet_hist = build_histogram(name, decay_mode_bins, vis_mass_bins)
                fill_hists(zero_jet_events, zero_jet_hist, 't1_decayMode', 'vis_mass')

                # now boosted category
                output_file.cd('{}_boosted'.format(channel_prefix))
                boost_hist = build_histogram(name, higgs_pT_bins_boost, m_sv_bins_boost)
                fill_hists(boosted_events, boost_hist, 'higgs_pT', 'm_sv')

                # vbf category is last
                output_file.cd('{}_vbf'.format(channel_prefix))
                vbf_hist = build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins)
                fill_hists(vbf_events, vbf_hist, vbf_cat_x_var, vbf_cat_y_var)

                # vbf sub-categories event after normal vbf categories
                vbf_cat_hists = []
                for cat in boilerplate['vbf_sub_cats']:
                    output_file.cd('{}_{}'.format(channel_prefix, cat))
                    vbf_cat_hists.append(build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins))
                fill_hists(vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var,
                        zvar_name=vbf_cat_edge_var, edges=vbf_cat_edges)

                # write then reset histograms
                output_file.Write()

                if '_JHU' in name:
                    for weight in get_ac_weights(name, boilerplate['jhu_ac_reweighting_map']):
                        print 'Reweighting sample {} to {}'.format(name, weight[1]+postfix)
                        # start with 0-jet category
                        output_file.cd('{}_0jet'.format(channel_prefix))
                        zero_jet_hist = build_histogram(weight[1]+postfix, decay_mode_bins, vis_mass_bins)
                        fill_hists(zero_jet_events, zero_jet_hist, 't1_decayMode', 'vis_mass', ac_weight=weight[0])

                        # now boosted category
                        output_file.cd('{}_boosted'.format(channel_prefix))
                        boost_hist = build_histogram(weight[1]+postfix, higgs_pT_bins_boost, m_sv_bins_boost,)
                        fill_hists(boosted_events, boost_hist, 'higgs_pT', 'm_sv', ac_weight=weight[0])

                        # vbf category is last
                        output_file.cd('{}_vbf'.format(channel_prefix))
                        vbf_hist = build_histogram(weight[1]+postfix, vbf_cat_x_bins, vbf_cat_y_bins)
                        fill_hists(vbf_events, vbf_hist, vbf_cat_x_var, vbf_cat_y_var, ac_weight=weight[0])

                        # vbf sub-categories event after normal vbf categories
                        vbf_cat_hists = []
                        for cat in boilerplate['vbf_sub_cats']:
                            output_file.cd('{}_{}'.format(channel_prefix, cat))
                            vbf_cat_hists.append(build_histogram(weight[1]+postfix, vbf_cat_x_bins, vbf_cat_y_bins))
                        fill_hists(vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var,
                                zvar_name=vbf_cat_edge_var, edges=vbf_cat_edges, ac_weight=weight[0])
                        output_file.Write()
                elif '_madgraph' in name:
                    for weight in get_ac_weights(name, boilerplate['mg_ac_reweighting_map']):
                        print 'Reweighting sample {} to {}'.format(name, weight[1]+postfix)
                        # start with 0-jet category
                        output_file.cd('{}_0jet'.format(channel_prefix))
                        zero_jet_hist = build_histogram(weight[1]+postfix+postfix, decay_mode_bins, vis_mass_bins)
                        fill_hists(zero_jet_events, zero_jet_hist, 't1_decayMode', 'vis_mass', ac_weight=weight[0])

                        # now boosted category
                        output_file.cd('{}_boosted'.format(channel_prefix))
                        boost_hist = build_histogram(weight[1]+postfix, higgs_pT_bins_boost, m_sv_bins_boost,)
                        fill_hists(boosted_events, boost_hist, 'higgs_pT', 'm_sv', ac_weight=weight[0])

                        # vbf category is last
                        output_file.cd('{}_vbf'.format(channel_prefix))
                        vbf_hist = build_histogram(weight[1]+postfix, vbf_cat_x_bins, vbf_cat_y_bins)
                        fill_hists(vbf_events, vbf_hist, vbf_cat_x_var, vbf_cat_y_var, ac_weight=weight[0])

                        # vbf sub-categories event after normal vbf categories
                        vbf_cat_hists = []
                        for cat in boilerplate['vbf_sub_cats']:
                            output_file.cd('{}_{}'.format(channel_prefix, cat))
                            vbf_cat_hists.append(build_histogram(weight[1]+postfix, vbf_cat_x_bins, vbf_cat_y_bins))
                        fill_hists(vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var,
                                zvar_name=vbf_cat_edge_var, edges=vbf_cat_edges, ac_weight=weight[0])
                        output_file.Write()

            # do anti-iso categorization for fake-factor using data
            else:
                print 'making fake factor hists'
                antiIso_events = general_selection[general_selection['is_antiTauIso'] > 0]
                fake_zero_jet_events = antiIso_events[antiIso_events['njets'] == 0]
                fake_boosted_events = antiIso_events[
                    (antiIso_events['njets'] == 1) |
                    ((antiIso_events['njets'] > 1) & (antiIso_events['mjj'] < 300))
                ]
                fake_vbf_events = antiIso_events[(antiIso_events['njets'] > 1) & (antiIso_events['mjj'] > 300)]

                output_file.cd('{}_0jet'.format(channel_prefix))
                zero_jet_hist = build_histogram('jetFakes', decay_mode_bins, vis_mass_bins)
                zero_jet_hist = fill_hists(fake_zero_jet_events, zero_jet_hist,
                                        't1_decayMode', 'vis_mass', fake_weight='fake_weight')

                output_file.cd('{}_boosted'.format(channel_prefix))
                boost_hist = build_histogram('jetFakes', higgs_pT_bins_boost, m_sv_bins_boost)
                boost_hist = fill_hists(fake_boosted_events, boost_hist, 'higgs_pT', 'm_sv', fake_weight='fake_weight')

                output_file.cd('{}_vbf'.format(channel_prefix))
                vbf_hist = build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins)
                vbf_hist = fill_hists(fake_vbf_events, vbf_hist, vbf_cat_x_var,
                                    vbf_cat_y_var, fake_weight='fake_weight')

                # vbf sub-categories event after normal vbf categories
                vbf_cat_hists = []
                for cat in boilerplate['vbf_sub_cats']:
                    output_file.cd('{}_{}'.format(channel_prefix, cat))
                    vbf_cat_hists.append(build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins))
                fill_hists(fake_vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var, zvar_name=vbf_cat_edge_var,
                        edges=vbf_cat_edges, fake_weight='fake_weight')

                output_file.Write()

                if args.syst:
                    for syst in boilerplate['fake_factor_systematics']:
                        output_file.cd('{}_0jet'.format(channel_prefix))
                        zero_jet_hist = build_histogram(
                            'jetFakes_CMS_htt_{}'.format(syst), decay_mode_bins, vis_mass_bins)
                        zero_jet_hist = fill_hists(fake_zero_jet_events, zero_jet_hist,
                                                't1_decayMode', 'vis_mass', fake_weight=syst)

                        output_file.cd('{}_boosted'.format(channel_prefix))
                        boost_hist = build_histogram('jetFakes_CMS_htt_{}'.format(syst),
                                                    higgs_pT_bins_boost, m_sv_bins_boost)
                        boost_hist = fill_hists(fake_boosted_events, boost_hist, 'higgs_pT', 'm_sv', fake_weight=syst)

                        output_file.cd('{}_vbf'.format(channel_prefix))
                        vbf_hist = build_histogram('jetFakes_CMS_htt_{}'.format(syst), vbf_cat_x_bins, vbf_cat_y_bins)
                        vbf_hist = fill_hists(fake_vbf_events, vbf_hist, vbf_cat_x_var, vbf_cat_y_var, fake_weight=syst)

                        # vbf sub-categories event after normal vbf categories
                        vbf_cat_hists = []
                        for cat in boilerplate['vbf_sub_cats']:
                            output_file.cd('{}_{}'.format(channel_prefix, cat))
                            vbf_cat_hists.append(build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins))
                        fill_hists(fake_vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var, zvar_name=vbf_cat_edge_var,
                                edges=vbf_cat_edges, fake_weight=syst)
                        output_file.Write()

    output_file.Close()
    print 'Finished in {} seconds'.format(time.time() - start)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--no-syst', '-s', dest='syst', action='store_false', help='run without systematics')
    parser.add_argument('--embed', '-e', action='store_true', help='use embedded instead of MC')
    parser.add_argument('--local', '-l', action='store_true', help='running locally')
    parser.add_argument('--year', '-y', required=True, action='store', help='year being processed')
    parser.add_argument('--input-dir', '-i', required=True, action='store', dest='input_dir', help='path to files')
    parser.add_argument('--tree-name', '-t', required=True, action='store', dest='tree_name', help='name of input tree')
    parser.add_argument('--suffix', action='store', default='', help='suffix for filename')
    parser.add_argument('--config', '-c', action='store', default=None, required=True, help='config for binning, etc.')
    main(parser.parse_args())
