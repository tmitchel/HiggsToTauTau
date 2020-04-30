import os
import sys
import copy
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

default_object = {
    'unscaled': pd.DataFrame(),
    'selection': pd.DataFrame(),
    'names': np.array([]),
    'isSignal': np.array([]),
    'weights': np.array([]),
    'lepton': np.array([])
}


def build_filelist(el_input_dir, mu_input_dir):
    """Build list of files to be processed and included in the DataFrame."""
    files = [
        ('et', ifile) for ifile in glob('{}/*/merged/*.root'.format(el_input_dir))
    ]
    files += [
        ('mt', ifile) for ifile in glob('{}/*/merged/*.root'.format(mu_input_dir))
    ]

    nominal = {'nominal': []}
    systematics = {}
    for channel, fname in files:
        if 'jetFakes' in fname:
            continue
        if 'SYST_' in fname:
            continue
            keyname = fname.split('SYST_')[-1].split('/')[0]
            systematics.setdefault(keyname, [])
            systematics[keyname].append((channel, fname))
        else:
            nominal['nominal'].append((channel, fname))
    pprint(nominal, width=150)
    pprint(systematics, width=150)
    return nominal, systematics


def get_columns(fname):
    """Return columns to keep and columns to drop."""
    columns = scaled_vars + selection_vars
    todrop = ['evtwt']
    return columns, todrop


def split_dataframe(fname, slim_df, todrop):
    """Split the DataFrame to get weights separate. Also, weights by AC if neeeded."""
    weights = slim_df['evtwt']
    slim_df = slim_df.drop(selection_vars+todrop, axis=1)
    slim_df = slim_df.astype('float64')
    return slim_df, weights


def get_labels(nevents, name):
    """Label events as sig vs bkg and SM vs non-SM."""
    # get training label
    isSignal = np.zeros(nevents)
    if 'reweighted' in name or 'powheg' in name:
        isSignal = np.ones(nevents)

    # get scaling label
    isSM = np.ones(nevents)
    if 'reweighted' in name or 'powheg' in name:
        isSM = np.zeros(nevents)
    elif 'data' in name:
        isSM = np.zeros(nevents)
    elif 'embedmu' in name or 'embedel' in name:
        print 'already saw this {} in embed.root'.format(name)
        isSM = np.zeros(nevents)

    return isSignal, isSM


def apply_selection(df):
    """Apply basic preselection and clean some variables."""
    # preselection
    slim_df = df[(df['njets'] > 1) & (df['mjj'] > 300)]

    # make sure our DataFrame is actually reasonable
    slim_df = slim_df.dropna(axis=0, how='any')  # drop events with a NaN
    slim_df = slim_df.drop_duplicates()
    slim_df = slim_df[(slim_df['Q2V1'] > -1e10) & (slim_df['Q2V1'] < 1e10)]
    slim_df = slim_df[(slim_df['Q2V2'] > -1e10) & (slim_df['Q2V2'] < 1e10)]
    slim_df = slim_df[(slim_df['Phi'] > -2.1 * math.pi) & (slim_df['Phi']
                                                           < 2.1 * math.pi)]  # gave this a little wiggle room
    slim_df = slim_df[(slim_df['Phi1'] > -2.1 * math.pi) & (slim_df['Phi1'] < 2.1 * math.pi)]
    slim_df = slim_df[(slim_df['costheta1'] > -1) & (slim_df['costheta1'] < 1)]
    slim_df = slim_df[(slim_df['costheta2'] > -1) & (slim_df['costheta2'] < 1)]
    slim_df = slim_df[(slim_df['costhetastar'] > -1) & (slim_df['costhetastar'] < 1)]

    return slim_df


def handle_file(all_data, channel, ifile, syst):
    """Process this file and add to the DataFrame."""
    filename = ifile.replace('.root', '')
    syst = syst.replace(';1', '')
    print 'Loading input file...', filename, 'with syst...', syst

    open_file = uproot.open(ifile)
    columns, todrop = get_columns(filename)

    input_df = open_file['{}_tree'.format(channel)].pandas.df(columns)
    slim_df = apply_selection(input_df)

    # get variables needed for selection (so they aren't normalized)
    selection_df = slim_df[selection_vars]

    # get just the weights (they are scaled differently)
    slim_df, weights = split_dataframe(filename, slim_df, todrop)

    # add the event label
    isSignal, isSM = get_labels(len(slim_df), ifile.lower())
    slim_df['isSM'] = isSM

    # scale event weights between 0 - 1
    if len(weights.values) > 0:
        weights = MinMaxScaler(feature_range=(1., 2.)).fit_transform(
            weights.values.reshape(-1, 1))

    all_data.setdefault(syst, copy.deepcopy(default_object))

    # add data to the full set
    all_data[syst]['unscaled'] = pd.concat([all_data[syst]['unscaled'], slim_df])
    # add selection variables to full set
    all_data[syst]['selection'] = pd.concat([all_data[syst]['selection'], selection_df])
    # insert the name of the current sample
    all_data[syst]['names'] = np.append(all_data[syst]['names'], np.full(len(slim_df), filename.split('/')[-1]))
    # insert the name of the channel
    all_data[syst]['lepton'] = np.append(all_data[syst]['lepton'], np.full(len(slim_df), channel))
    # labels for signal/background
    all_data[syst]['isSignal'] = np.append(all_data[syst]['isSignal'], isSignal)
    # weights scaled from 0 - 1
    all_data[syst]['weights'] = np.append(all_data[syst]['weights'], weights)

    return all_data


def build_scaler(sm_only):
    """Fit to SM only then build DataFrame with all info to store in output file."""
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
    """Take the DataFrame and format it for storage."""
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
    return formatted_data


def main(args):
    start = time.time()

    # create the store
    store = pd.HDFStore('Output/datasets/{}.h5'.format(args.output),
                        complevel=9, complib='bzip2')

    # build dictionaries of directory/files
    nominal, systematics = build_filelist(args.el_input_dir, args.mu_input_dir)

    # handle the nominal case first
    for idir, files in nominal.iteritems():
        all_data = {}
        for info in files:
            all_data = handle_file(all_data, info[0], info[1], idir)

    # build the scaler fit only to nominal SM backgrounds
    sm_only = all_data['nominal']['unscaled'][all_data['nominal']['unscaled']['isSM'] == 1]
    scaler, store['scaler'] = build_scaler(sm_only)

    # scale and save the nominal case to the output file
    store['nominal'] = format_for_store(all_data, 'nominal', scaler)

    # # now handle the systematics
    # for idir, files in systematics.iteritems():
    #     all_data.clear()
    #     for info in files:
    #         all_data = handle_file(all_data, info[0], info[1], idir)

    #     # scale and save to the output file
    #     store[idir] = format_for_store(all_data, idir, scaler)

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
