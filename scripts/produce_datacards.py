import json
import ROOT
import time
import numpy
import pandas
import uproot
import logging
import datetime
from glob import glob
from array import array
from pprint import pprint


def build_filelist(input_dir):
    files = [ifile for ifile in glob('{}/*/*.root'.format(input_dir))]

    filelist = {'nominal': []}
    for fname in files:
        if 'nominal' in fname:
            filelist['nominal'].append(fname)
        else:
            keyname = fname.split('/')[-2]
            filelist.setdefault(keyname, [])
            filelist[keyname].append(fname)
    return filelist



def build_histogram(name, x_bins, y_bins):
    return ROOT.TH2F(name, name, len(x_bins) - 1, array('d', x_bins), len(y_bins) - 1, array('d', y_bins))


def fill_hists(data, hists, xvar_name, yvar_name, zvar_name=None, edges=None, fake_weight=None, DCP_idx=None):
    evtwt = data['evtwt'].to_numpy(copy=True)
    xvar = data[xvar_name].values
    yvar = data[yvar_name].values
    zvar = data[zvar_name].values if zvar_name != None else None
    if zvar_name == 'D0_ggH':
        dcp = data['DCP_ggH'].values
    elif zvar_name == 'D0_VBF':
        dcp = data['DCP_VBF'].values
    elif zvar_name != None:
        raise Exception('Don\'t know how to handle DCP for provided zvar_name {}'.format(zvar_name))

    if fake_weight != None:
        evtwt *= data[fake_weight].values

    for i in xrange(len(data.index)):
        if zvar_name != None:
            for j, edge in enumerate(edges[1:]):  # remove lowest left edge
                if zvar[i] < edge:
                    if DCP_idx == None:
                        hists[j].Fill(xvar[i], yvar[i], evtwt[i])
                    else:
                        if dcp[i] > 0:
                            hists[j].Fill(xvar[i], yvar[i], evtwt[i])
                        else:
                            # DCP minus bins are offset by DCP_idx
                            hists[j+DCP_idx].Fill(xvar[i], yvar[i], evtwt[i])
                    break
        else:
            hists.Fill(xvar[i], yvar[i], evtwt[i])

    return hists


def get_syst_name(channel, syst, syst_name_map):
    if syst == 'nominal':
        return ''
    elif syst in syst_name_map.keys():
        return syst_name_map[syst]
    elif 'LES_DM' in syst:
        temp = syst.replace('LES_DM', 'efaket') if channel == 'et' else syst.replace('LES_DM', 'mfaket')
        return syst_name_map[temp]
    else:
        print '\t \033[91m[INFO]  {} is unknown. Skipping...\033[0m'.format(syst)
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
    for cat in boilerplate['categories'] + boilerplate['vbf_sub_cats_plus'] + boilerplate['vbf_sub_cats_minus']:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
    output_file.cd()

    # use this once uproot supports sub-directories inside root files
    # output_file = uproot.recreate('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
    #                                                                               ztt_name, syst_name, args.date, '_'+args.suffix))

    logging.basicConfig(filename='logs/2D_htt_{}_{}_{}_fa3_{}_{}{}.log'.format(channel_prefix, ztt_name, syst_name, args.year, date, args.suffix))
    nsysts = len(filelist.keys())
    idx = -1
    for syst, files in filelist.iteritems():
        if not args.syst and syst != 'nominal':
          continue
        idx += 1
        print 'Processing syst {} ({} of {})'.format(syst, idx, nsysts)
        postfix = get_syst_name(channel_prefix, syst, syst_name_map)
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
            if 'wh125_JHU_CMS' in name or 'zh125_JHU_CMS' in name or name == 'wh125_JHU' or name == 'zh125_JHU':
              continue

            logging.info('Processing: {}'.format(name + postfix))
            input_file = uproot.open(ifile)

            # get data naming correct
            if 'Data' in name:
                name = 'data_obs'

            # handle MC vs embedded name
            if 'embed' in ifile:
                name = name.replace('embedded', 'ZTT')
                name = name.replace('embed', 'ZTT')

            variables = set([
                'is_signal', 'is_antiTauIso', 'contamination', 'njets', 'mjj', 'evtwt',
                't1_decayMode', 'vis_mass', 'higgs_pT', 'm_sv',
                'D0_VBF', 'D0_ggH', 'DCP_VBF', 'DCP_ggH',
                vbf_cat_x_var, vbf_cat_y_var, vbf_cat_edge_var
            ])

            # get fake factor weights if needed
            if 'jetFakes' in ifile:
                variables.add('fake_weight')
                if args.syst:
                    variables.add('ff_*')

            name = name + postfix # add systematic postfix to file name

            events = input_file[args.tree_name].arrays(list(variables), outputtype=pandas.DataFrame)

            if 'jetFakes' in name:
                iso_branch = 'is_antiTauIso'
            else:
                iso_branch = 'is_signal'
            general_selection = events[(events[iso_branch] > 0)]

            # remove ttbar/diboson contamination to embedded sample
            if args.embed:
                general_selection = general_selection[(general_selection['contamination'] == 0)]

            if not 'jetFakes' in name:
                # do signal categorization
                zero_jet_events = general_selection[general_selection['njets'] == 0]
                boosted_events = general_selection[
                    (general_selection['njets'] == 1) |
                    ((general_selection['njets'] > 1) & (general_selection['mjj'] < 300))
                ]
                vbf_events = general_selection[(general_selection['njets'] > 1) & (general_selection['mjj'] > 300)]

                if 'wh125_JHU_' in name or 'zh125_JHU_' in name:
                    if 'nominal' in ifile:
                      name = boilerplate['wh_zh_name_map'][name]
                    else:
                      name, syst_suf = name.split('_CMS_')[0], name.split('_CMS_')[1]
                      name = boilerplate['wh_zh_name_map'][name]
                      name = name + '_CMS_' + syst_suf

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
                for cat in boilerplate['vbf_sub_cats_plus'] + boilerplate['vbf_sub_cats_minus']:
                    output_file.cd('{}_{}'.format(channel_prefix, cat))
                    vbf_cat_hists.append(build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins))
                fill_hists(vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var,
                        zvar_name=vbf_cat_edge_var, edges=vbf_cat_edges, DCP_idx=len(boilerplate['vbf_sub_cats_plus']))

                # write then reset histograms
                output_file.Write()

            # do anti-iso categorization for fake-factor using data
            else:
                logging.info('making fake factor hists')
                fake_zero_jet_events = general_selection[general_selection['njets'] == 0]
                fake_boosted_events = general_selection[
                    (general_selection['njets'] == 1) |
                    ((general_selection['njets'] > 1) & (general_selection['mjj'] < 300))
                ]
                fake_vbf_events = general_selection[(general_selection['njets'] > 1) & (general_selection['mjj'] > 300)]

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
                for cat in boilerplate['vbf_sub_cats_plus'] + boilerplate['vbf_sub_cats_minus']:
                    output_file.cd('{}_{}'.format(channel_prefix, cat))
                    vbf_cat_hists.append(build_histogram('jetFakes', vbf_cat_x_bins, vbf_cat_y_bins))
                fill_hists(fake_vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var, zvar_name=vbf_cat_edge_var,
                        edges=vbf_cat_edges, fake_weight='fake_weight', DCP_idx=len(boilerplate['vbf_sub_cats_plus']))

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
                        for cat in boilerplate['vbf_sub_cats_plus'] + boilerplate['vbf_sub_cats_minus']:
                            output_file.cd('{}_{}'.format(channel_prefix, cat))
                            vbf_cat_hists.append(build_histogram('jetFakes_' + syst, vbf_cat_x_bins, vbf_cat_y_bins))
                        fill_hists(fake_vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var, zvar_name=vbf_cat_edge_var,
                                edges=vbf_cat_edges, fake_weight=syst, DCP_idx=len(boilerplate['vbf_sub_cats_plus']))
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
