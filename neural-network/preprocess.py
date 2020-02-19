import os
import sys
import math
import time
import uproot
import numpy as np
import pandas as pd
from glob import glob
from pprint import pprint
from sklearn.preprocessing import StandardScaler, MinMaxScaler

import warnings
warnings.filterwarnings(
    'ignore', category=pd.io.pytables.PerformanceWarning)

# Variables used for selection. These shouldn't be normalized
selection_vars = ['njets', 'is_signal']

# Variables that could be used as NN input. These should be normalized
scaled_vars = [
    'evtwt', 'Q2V1', 'Q2V2', 'Phi', 'Phi1', 'costheta1',
    'costheta2', 'costhetastar', 'mjj', 'higgs_pT', 'm_sv',
]


def build_filelist(el_input_dir, mu_input_dir):
    files = [
        ifile for ifile in glob('{}/*/merged/*.root'.format(el_input_dir))
    ]
    files += [
        ifile for ifile in glob('{}/*/merged/*.root'.format(mu_input_dir))
    ]

    nominal = {'nominal': []}
    systematics = {}
    for fname in files:

      if 'SYST_' in fname:
          keyname = fname.split('SYST_')[-1].split('/')[0]
          systematics.setdefault(keyname, [])
          systematics[keyname].append(fname)
      else:
          nominal['nominal'].append(fname)
    pprint(nominal)
    pprint(systematics)
    return nominal, systematics


def get_columns(fname):
    columns = scaled_vars + selection_vars
    todrop = ['evtwt', 'idx']
    if 'vbf125_JHU' in fname:
        columns = columns + ['wt_vbf_a1']
        todrop = todrop + ['wt_vbf_a1']
    return columns, todrop


def split_dataframe(fname, slim_df, todrop):
    weights = slim_df['evtwt']
    index = slim_df['idx']
    if 'vbf125_JHU' in fname:
        weights = weights * slim_df['wt_vbf_a1']
    slim_df = slim_df.drop(selection_vars+todrop, axis=1)
    slim_df = slim_df.astype('float64')
    return slim_df, weights, index


def get_labels(nevents, name):
    # get training label
    isSignal = np.zeros(nevents)
    if 'vbf125' in name or 'ggh125' in name or 'wh125' in name or 'zh125' in name:
        isSignal = np.ones(nevents)

    # temp
    if 'vbf' in name:
        isSignal = np.ones(nevents)

    # get scaling label
    isSM = np.ones(nevents)
    if 'JHU' in name or 'madgraph' in name:
        isSM = np.zeros(nevents)
    elif 'data' in name:
        isSM = np.zeros(nevents)
    elif 'embedmu' in name or 'embedel' in name:
        print 'already saw this {} in embed.root'.format(name)
        isSM = np.zeros(nevents)

    return isSignal, isSM


def loadFile(ifile, open_file, syst):
    print 'Loading input file...', ifile, 'with syst...', syst
    if 'mutau' in ifile or 'mtau' in ifile:
        channel = 'mt'
    elif 'etau' in ifile or 'etau' in ifile:
        channel = 'et'
    else:
        raise Exception(
            'Input files must have MUTAU or ETAU in the provided path. You gave {}, ya goober.'.format(ifile))

    columns, todrop = get_columns(ifile)

    # read from TTrees into DataFrame
    if syst == 'nominal':
        input_df = open_file['{}_tree'.format(channel)].pandas.df(columns)
    else:
        input_df = open_file['{}_tree'.format(channel)].pandas.df(columns)
    input_df['idx'] = np.array([i for i in xrange(0, len(input_df.index))])

    slim_df = input_df[(input_df['njets'] > 1) & (input_df['mjj'] > 300)]  # preselection

    # make sure our DataFrame is actually reasonable
    slim_df = slim_df.dropna(axis=0, how='any')  # drop events with a NaN
    slim_df = slim_df.drop_duplicates()
    slim_df = slim_df[(slim_df['Q2V1'] > -1e10) & (slim_df['Q2V1'] < 1e10)]
    slim_df = slim_df[(slim_df['Q2V2'] > -1e10) & (slim_df['Q2V2'] < 1e10)]
    slim_df = slim_df[(slim_df['Phi'] > -2.1 * math.pi) & (slim_df['Phi'] < 2.1 * math.pi)] # gave this a little wiggle room
    slim_df = slim_df[(slim_df['Phi1'] > -2.1 * math.pi) & (slim_df['Phi1'] < 2.1 * math.pi)]
    slim_df = slim_df[(slim_df['costheta1'] > -1) & (slim_df['costheta1'] < 1)]
    slim_df = slim_df[(slim_df['costheta2'] > -1) & (slim_df['costheta2'] < 1)]
    slim_df = slim_df[(slim_df['costhetastar'] > -1) & (slim_df['costhetastar'] < 1)]

    # get variables needed for selection (so they aren't normalized)
    selection_df = slim_df[selection_vars]

    # get just the weights (they are scaled differently)
    slim_df, weights, index = split_dataframe(ifile, slim_df, todrop)

    # add the event label
    isSignal, isSM = get_labels(len(slim_df), ifile.lower())
    slim_df['isSM'] = isSM

    # scale event weights between 0 - 1
    weights = MinMaxScaler(feature_range=(1., 2.)).fit_transform(
        weights.values.reshape(-1, 1))

    return {
        'slim_df': slim_df,
        'selection_df': selection_df,
        'isSignal': isSignal,
        'weights': weights,
        'index': index,
        'somenames': np.full(len(slim_df), ifile.split('/')[-1]),
        'lepton': np.full(len(slim_df), channel),
    }, syst.replace(';1', '')


def handle_tree(all_data, ifile, syst):
    default_object = {
        'unscaled': pd.DataFrame(),
        'selection': pd.DataFrame(),
        'names': np.array([]),
        'isSignal': np.array([]),
        'weights': np.array([]),
        'index': np.array([]),
        'lepton': np.array([])
    }

    open_file = uproot.open(ifile)
    filename = ifile.replace('.root', '')

    proc_file, syst = loadFile(filename, open_file, syst)
    all_data.setdefault(syst, default_object.copy())

    # add data to the full set
    all_data[syst]['unscaled'] = pd.concat([all_data[syst]['unscaled'], proc_file['slim_df']])
    # add selection variables to full set
    all_data[syst]['selection'] = pd.concat([all_data[syst]['selection'], proc_file['selection_df']])
    # insert the name of the current sample
    all_data[syst]['names'] = np.append(all_data[syst]['names'], proc_file['somenames'])
    # insert the name of the channel
    all_data[syst]['lepton'] = np.append(all_data[syst]['lepton'], proc_file['lepton'])
    # labels for signal/background
    all_data[syst]['isSignal'] = np.append(all_data[syst]['isSignal'], proc_file['isSignal'])
    # weights scaled from 0 - 1
    all_data[syst]['weights'] = np.append(all_data[syst]['weights'], proc_file['weights'])
    all_data[syst]['index'] = np.append(all_data[syst]['index'], proc_file['index'])

    return all_data


def build_scaler(sm_only):
    scaler = StandardScaler()
    # only fit the nominal backgrounds
    scaler.fit(sm_only.values)
    scaler_info = pd.DataFrame.from_dict({
        'mean': scaler.mean_,
        'scale': scaler.scale_,
        'variance': scaler.var_,
        'nsamples': scaler.n_samples_seen_
    })
    scaler_info.set_index(sm_only.columns.values, inplace=True)
    return scaler, scaler_info


def format_for_store(all_data, idir, scaler):
    formatted_data = pd.DataFrame(
        scaler.transform(all_data[idir]['unscaled'].values),
        columns=all_data[idir]['unscaled'].columns.values, dtype='float64')

    # add selection variables
    for column in all_data[idir]['selection'].columns:
        formatted_data[column] = all_data[idir]['selection'][column].values

    # add other useful data
    formatted_data['sample_names'] = pd.Series(all_data[idir]['names'])
    formatted_data['lepton'] = pd.Series(all_data[idir]['lepton'])
    formatted_data['isSignal'] = pd.Series(all_data[idir]['isSignal'])
    formatted_data['evtwt'] = pd.Series(all_data[idir]['weights'])
    formatted_data['idx'] = pd.Series(all_data[idir]['index'])
    return formatted_data


def main(args):
    start = time.time()

    # create the store
    store = pd.HDFStore('datasets/{}.h5'.format(args.output),
                        complevel=9, complib='bzip2')

    # build dictionaries of directory/files
    nominal, systematics = build_filelist(args.el_input_dir, args.mu_input_dir)

    # handle the nominal case first
    for idir, files in nominal.iteritems():
        all_data = {}
        for ifile in files:
            all_data = handle_tree(all_data, ifile, idir)

    # build the scaler fit only to nominal SM backgrounds
    sm_only = all_data['nominal']['unscaled'][all_data['nominal']['unscaled']['isSM'] == 1]
    scaler, store['scaler'] = build_scaler(sm_only)

    # scale and save the nominal case to the output file
    store['nominal'] = format_for_store(all_data, 'nominal', scaler)

    # now handle the systematics
    for idir, files in systematics.iteritems():
        if 'jetVeto' in idir: # temporary
            continue
        all_data.clear()
        for ifile in files:
            all_data = handle_tree(all_data, ifile, idir)

        # scale and save to the output file
        store[idir] = format_for_store(all_data, idir, scaler)

    print 'Complete! Preprocessing completed in {} seconds'.format(time.time() - start)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--el-input', '-e', action='store',
                        dest='el_input_dir', default=None, help='path to etau input files')
    parser.add_argument('--mu-input', '-m', action='store',
                        dest='mu_input_dir', default=None, help='path to mutau input files')
    parser.add_argument('--output', '-o', action='store', dest='output',
                        default='store.h5', help='name of output file')

    main(parser.parse_args())
