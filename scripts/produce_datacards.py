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
    """Gather all files to process (including systematic shifts)"""
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


def powheg_naming(name):
    if 'ggh125_powheg' in name:
        name = name.replace('ggh125_powheg', 'ggH125')
    elif 'vbf125_powheg' in name:
        name = name.replace('vbf125_powheg', 'VBF125')
    elif 'wh125_powheg' in name:
        name = name.replace('wh125_powheg', 'WH125')
    elif 'zh125_powheg' in name:
        name = name.replace('zh125_powheg', 'ZH125')
    return name


def build_histogram(name, x_bins, y_bins, powheg_map):
    """Build TH2F to fill later."""
    if 'powheg' in name:
        name = powheg_naming(name)
    return ROOT.TH2F(name, name, len(x_bins) - 1, array('d', x_bins), len(y_bins) - 1, array('d', y_bins))


def fill_hists(data, hists, xvar_name, yvar_name, zvar_name=None, edges=None, fake_weight=None, DCP_idx=None):
    """
    Fill histograms with all necessary weights, binnings, etc.

    Produce a 2D histogram (or set of 2D histograms) with everything necessary for datacards. The histograms
    will be properly weighted and binned.

    Variables:
    data        -- pandas DataFrame containing event data
    hists       -- histogram (or list of histograms) to fill
    xvar_name   -- name of variable for x-axis
    yvar_name   -- name of variable for y-axis
    zvar_name   -- if provided, fill a histogram for each bin in this variable
    edges       -- binning for zvar_name. Must be provided if zvar_name is provided
    fake_weight -- weights to be applied to jetFakes
    DCP_idx     -- hists index offset (DCP minus bins are offset by this amount)

    Returns:
    hists -- filled histograms
    """
    evtwt = data['evtwt'].to_numpy(copy=True)
    xvar = data[xvar_name].values
    yvar = data[yvar_name].values
    zvar = data[zvar_name].values if zvar_name != None else None
    if zvar_name == 'D0_ggH':
        dcp = data['DCP_ggH'].values
    elif zvar_name == 'D0_VBF':
        dcp = data['DCP_VBF'].values
    elif zvar_name == 'D_a2_VBF' or zvar_name == 'D_l1_VBF' or zvar_name == 'D_l1zg_VBF':
        DCP_idx = None  # DCP binning is only used when measuring fa3
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
    """Map input systematic name to the name needed for Higgs Combine datacards"""
    if syst == 'nominal':
        return ''
    elif syst in syst_name_map.keys():
        return syst_name_map[syst]
    else:
        print '\t \033[91m[INFO]  {} is unknown. Skipping...\033[0m'.format(syst)
        return 'unknown'


def parse_tree_name(keys):
    """Take list of keys in the file and search for our TTree"""
    if 'et_tree;1' in keys:
        return 'et_tree'
    elif 'mt_tree;1' in keys:
        return 'mt_tree'
    else:
        raise Exception('Can\t find et_tree or mt_tree in keys: {}'.format(keys))


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
        tau_pt_bins = config['tau_pt_bins']
        m_sv_bins_0jet = config['m_sv_bins_0jet']
        higgs_pT_bins_boost = config['higgs_pT_bins_boost']
        m_sv_bins_boost = config['m_sv_bins_boost']
        vbf_cat_x_var, vbf_cat_x_bins = config['vbf_cat_x_bins']
        vbf_cat_y_var, vbf_cat_y_bins = config['vbf_cat_y_bins']
        vbf_cat_edge_var, vbf_cat_edges = config['vbf_cat_edges']

    filelist = build_filelist(args.input_dir)
    assert len(filelist['nominal']) > 0, 'could\'nt locate any nominal files'

    keys = uproot.open(filelist['nominal'][0]).keys()
    tree_name = parse_tree_name(keys)

    channel_prefix = tree_name.replace('mt_tree', 'mt')
    channel_prefix = channel_prefix.replace('et_tree', 'et')
    assert channel_prefix == 'mt' or channel_prefix == 'et', 'must provide a valid tree name'

    # get things for output file name
    ztt_name = 'emb' if args.embed else 'ztt'
    syst_name = 'Sys' if args.syst else 'noSys'

    date = datetime.datetime.now().strftime("%B%d")
    output_file = ROOT.TFile('Output/templates/2D_htt_{}_{}_{}_fa3_{}_{}{}.root'.format(channel_prefix,
                                                                                        ztt_name, syst_name, args.year, date, args.suffix), 'RECREATE')

    # create structure within output file
    vbf_categories = []
    if 'D0_' in vbf_cat_edge_var:
        vbf_categories += boilerplate['vbf_sub_cats_plus'] + boilerplate['vbf_sub_cats_minus']
    else:
        vbf_categories += boilerplate['vbf_sub_cats']

    for cat in boilerplate['categories'] + vbf_categories:
        output_file.cd()
        output_file.mkdir('{}_{}'.format(channel_prefix, cat))
    output_file.cd()

    # use this once uproot supports sub-directories inside root files
    # output_file = uproot.recreate('Output/templates/htt_{}_{}_{}_fa3_{}{}.root'.format(channel_prefix,
    #                                                                               ztt_name, syst_name, args.date, '_'+args.suffix))

    logging.basicConfig(filename='logs/2D_htt_{}_{}_{}_fa3_{}_{}{}.log'.format(channel_prefix,
                                                                               ztt_name, syst_name, args.year, date, args.suffix))
    nsysts = len(filelist.keys())
    idx = -1
    for syst, files in filelist.iteritems():
        if not args.syst and syst != 'nominal':
            continue
        idx += 1
        print 'Processing syst {} ({} of {})'.format(syst, idx, nsysts)
        postfix = get_syst_name(channel_prefix, syst, syst_name_map)
        if postfix == 'unknown':  # skip unknown systematics
            continue

        postfix = postfix.replace('YEAR', args.year)  # add correct year
        postfix = postfix.replace('LEP', 'ele') if channel_prefix == 'et' else postfix.replace('LEP', 'mu')
        postfix = postfix.replace('CHAN', 'et') if channel_prefix == 'et' else postfix.replace('CHAN', 'mt')
        stable_postfix = postfix

        for ifile in files:
            # handle ZTT vs embedded
            if args.embed and 'ZTT' in ifile:
                continue
            elif not args.embed and 'embed' in ifile:
                continue

            # handle embed vs mc systematics
            postfix = stable_postfix # reset to original
            if 'embed' in ifile:
                if 'CMS_tauideff' in postfix:
                    postfix = postfix.replace('tauideff', 'eff_t_embedded')
                elif 'CMS_scale_e_' in postfix:
                    postfix = postfix.replace('scale_e_', 'scale_emb_e')
                elif 'CMS_single' in postfix and 'trg' in postfix:
                    postfix = postfix.replace('trg', 'trg_emb')
                elif 'tautrg_' in postfix:
                    postfix = postfix.replace('trg', 'trg_emb')
                # this will be once I update my embedded energy scale
                # elif 'CMS_scale_t_' in postfix:
                #     postfix = postfix.replace('scale_t_', 'scale_emb_t_')

            name = ifile.replace('.root', '').split('/')[-1]
            if 'wh125_JHU_CMS' in name or 'zh125_JHU_CMS' in name or name == 'wh125_JHU' or name == 'zh125_JHU':
                continue

            logging.info('Processing: {}'.format(name + postfix))
            input_file = uproot.open(ifile)

            # get data naming correct
            if 'Data' in name:
                name = 'data_obs'

            variables = set([
                'is_signal', 'is_antiTauIso', 'contamination', 'njets', 'mjj', 'evtwt',
                't1_pt', 'higgs_pT', 'm_sv',
                'DCP_VBF', 'DCP_ggH',
                vbf_cat_x_var, vbf_cat_y_var, vbf_cat_edge_var
            ])

            # get fake factor weights if needed
            if 'jetFakes' in ifile:
                variables.add('fake_weight')
                if args.syst:
                    variables.add('ff_*')
                    variables.add('mtclosure_*')
                    variables.add('lptclosure_*')
                    variables.add('osssclosure_*')

            name = name + postfix  # add systematic postfix to file name

            events = input_file[tree_name].arrays(list(variables), outputtype=pandas.DataFrame)

            if 'jetFakes' in name:
                iso_branch = 'is_antiTauIso'
            else:
                iso_branch = 'is_signal'
            general_selection = events[(events[iso_branch] > 0)]

            # remove ttbar/diboson contamination to embedded sample
            if args.embed:
                general_selection = general_selection[(general_selection['contamination'] == 0)]

            # do signal categorization
            zero_jet_events = general_selection[general_selection['njets'] == 0]
            boosted_events = general_selection[
                (general_selection['njets'] == 1) |
                ((general_selection['njets'] > 1) & (general_selection['mjj'] < 300))
            ]
            vbf_events = general_selection[(general_selection['njets'] > 1) & (general_selection['mjj'] > 300)]

            fweight = None
            if 'jetFakes' in name:
                fweight = 'fake_weight'

            # start with 0-jet category
            output_file.cd('{}_0jet'.format(channel_prefix))
            zero_jet_hist = build_histogram(name, tau_pt_bins, m_sv_bins_0jet, boilerplate["powheg_map"])
            fill_hists(zero_jet_events, zero_jet_hist, 't1_pt', 'm_sv', fake_weight=fweight)

            output_file.cd('{}_boosted'.format(channel_prefix))
            boost_hist = build_histogram(name, higgs_pT_bins_boost, m_sv_bins_boost, boilerplate["powheg_map"])
            boost_hist = fill_hists(boosted_events, boost_hist, 'higgs_pT', 'm_sv', fake_weight=fweight)

            output_file.cd('{}_vbf'.format(channel_prefix))
            vbf_hist = build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins, boilerplate["powheg_map"])
            vbf_hist = fill_hists(vbf_events, vbf_hist, vbf_cat_x_var,
                                  vbf_cat_y_var, fake_weight=fweight)

            # vbf sub-categories event after normal vbf categories
            vbf_cat_hists = []
            for cat in vbf_categories:
                output_file.cd('{}_{}'.format(channel_prefix, cat))
                vbf_cat_hists.append(build_histogram(name, vbf_cat_x_bins, vbf_cat_y_bins, boilerplate["powheg_map"]))
            fill_hists(vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var, zvar_name=vbf_cat_edge_var,
                       edges=vbf_cat_edges, fake_weight=fweight, DCP_idx=len(boilerplate['vbf_sub_cats_plus']))

            output_file.Write()

            if args.syst and 'jetFakes' in name:
                for syst in boilerplate['fake_factor_systematics']:
                    jet_postfix = get_syst_name(channel_prefix, syst, syst_name_map)
                    if jet_postfix == 'unknown':  # skip unknown systematics
                        continue
            
                    jet_postfix = jet_postfix.replace('YEAR', args.year)  # add correct year
                    jet_postfix = jet_postfix.replace('LEP', 'ele') if channel_prefix == 'et' else jet_postfix.replace('LEP', 'mu')
                    jet_postfix = jet_postfix.replace('CHAN', 'et') if channel_prefix == 'et' else jet_postfix.replace('CHAN', 'mt')

                    output_file.cd('{}_0jet'.format(channel_prefix))
                    zero_jet_hist = build_histogram(
                        'jetFakes{}'.format(jet_postfix), tau_pt_bins, m_sv_bins_0jet, boilerplate["powheg_map"])
                    zero_jet_hist = fill_hists(zero_jet_events, zero_jet_hist,
                                               't1_pt', 'm_sv', fake_weight=syst)

                    output_file.cd('{}_boosted'.format(channel_prefix))
                    boost_hist = build_histogram('jetFakes{}'.format(jet_postfix),
                                                 higgs_pT_bins_boost, m_sv_bins_boost, boilerplate["powheg_map"])
                    boost_hist = fill_hists(boosted_events, boost_hist, 'higgs_pT', 'm_sv', fake_weight=syst)

                    output_file.cd('{}_vbf'.format(channel_prefix))
                    vbf_hist = build_histogram('jetFakes{}'.format(
                        jet_postfix), vbf_cat_x_bins, vbf_cat_y_bins, boilerplate["powheg_map"])
                    vbf_hist = fill_hists(vbf_events, vbf_hist, vbf_cat_x_var, vbf_cat_y_var, fake_weight=syst)

                    # vbf sub-categories event after normal vbf categories
                    vbf_cat_hists = []
                    for cat in vbf_categories:
                        output_file.cd('{}_{}'.format(channel_prefix, cat))
                        vbf_cat_hists.append(build_histogram('jetFakes' + jet_postfix, vbf_cat_x_bins,
                                                             vbf_cat_y_bins, boilerplate["powheg_map"]))
                    fill_hists(vbf_events, vbf_cat_hists, vbf_cat_x_var, vbf_cat_y_var, zvar_name=vbf_cat_edge_var,
                               edges=vbf_cat_edges, fake_weight=syst, DCP_idx=len(boilerplate['vbf_sub_cats_plus']))
                    output_file.Write()

    output_file.Close()
    print 'Finished in {} seconds'.format(time.time() - start)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--no-syst', '-s', dest='syst', action='store_false', help='run without systematics')
    parser.add_argument('--embed', '-e', action='store_true', help='use embedded instead of MC')
    parser.add_argument('--year', '-y', required=True, action='store', help='year being processed')
    parser.add_argument('--input-dir', '-i', required=True, action='store', dest='input_dir', help='path to files')
    parser.add_argument('--suffix', action='store', default='', help='suffix for filename')
    parser.add_argument('--config', '-c', action='store', default=None, required=True, help='config for binning, etc.')
    main(parser.parse_args())
