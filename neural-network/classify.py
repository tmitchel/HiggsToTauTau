from sklearn.preprocessing import StandardScaler
from collections import defaultdict
from os import environ, path, mkdir, listdir
environ['KERAS_BACKEND'] = 'tensorflow'
from keras.models import load_model
from pprint import pprint
from array import array
from glob import glob
import pandas as pd
import uproot
import numpy
import ROOT
import sys


def build_filelist(input_dir):
    files = [
        ifile for ifile in glob('{}/*/merged/*.root'.format(input_dir))
    ]

    data = {}
    for fname in files:
        if 'jetFakes' in fname:
            continue
        if 'SYST_' in fname:
            keyname = fname.split('SYST_')[-1].split('/')[0]
            data.setdefault(keyname, [])
            data[keyname].append(fname)
        else:
            data.setdefault('nominal', [])
            data['nominal'].append(fname)
    return data


def main(args):
    if 'mutau' in args.input_dir or 'mt20' in args.input_dir:
        tree_prefix = 'mt_tree'
    elif 'etau' in args.input_dir or 'et20' in args.input_dir:
        tree_prefix = 'et_tree'
    else:
        raise Exception(
            'Input files must have MUTAU or ETAU in the provided path. You gave {}, ya goober.'.format(args.input_dir))

    model = load_model('Output/models/{}.hdf5'.format(args.model))
    all_data = pd.HDFStore(args.input_name)

    if not path.isdir('Output/trees/{}'.format(args.output_dir)):
        mkdir('Output/trees/{}'.format(args.output_dir))

    filelist = build_filelist(args.input_dir)
    print 'Files to process...'
    pprint(dict(filelist))
    for syst, ifiles in filelist.iteritems():
        if not path.exists('Output/trees/{}/{}'.format(args.output_dir, syst)):
            mkdir('Output/trees/{}/{}'.format(args.output_dir, syst))

        for ifile in ifiles:
            # if not '125' in ifile:
            #   continue
            fname = ifile.replace('.root', '').split('/')[-1]
            print 'Processing file: {} from {}'.format(fname, ifile.split('merged')[0].split('/')[-2])

            open_file = uproot.open(ifile)
            nevents = open_file['nevents']
            oldtree = open_file[tree_prefix].arrays(['*'])
            treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}
            treedict['NN_disc'] = numpy.float64

            # load the correct tree
            scaler_info = all_data['scaler']
            scaler_info = scaler_info.drop('isSM', axis=0)

            # drop all variables not going into the network
            to_classify = open_file[tree_prefix].arrays(scaler_info.index.values, outputtype=pd.DataFrame)

            scaler = StandardScaler()
            scaler.mean_ = scaler_info['mean'].values.reshape(1, -1)
            scaler.scale_ = scaler_info['scale'].values.reshape(1, -1)

            to_classify.fillna(-100, inplace=True)
            to_classify.replace([numpy.inf, -numpy.inf], -100, inplace=True)

            # scale correctly
            scaled = pd.DataFrame(
                scaler.transform(to_classify.values),
                columns=to_classify.columns.values, dtype='float64')

            # do the classification
            guesses = model.predict(scaled.values, verbose=True)

            # if 'embed' in fname:
            #   with uproot.recreate('Output/trees/{}/{}/{}.root'.format(args.output_dir, syst, fname), compression=None) as f:
            #       f[tree_prefix] = uproot.newtree(treedict)
            #       oldtree['NN_disc'] = guesses.reshape(len(guesses))
            #       f[tree_prefix].extend(oldtree)
            # else:
            with uproot.recreate('Output/trees/{}/{}/{}.root'.format(args.output_dir, syst, fname)) as f:
                f[tree_prefix] = uproot.newtree(treedict)
                oldtree['NN_disc'] = guesses.reshape(len(guesses))
                f[tree_prefix].extend(oldtree)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--model', '-m', action='store', dest='model', default='testModel', help='name of model to use')
    parser.add_argument('--input', '-i', action='store', dest='input_name',
                        default='test', help='name of input dataset')
    parser.add_argument('--dir', '-d', action='store', dest='input_dir',
                        default='input_files/etau_stable_Oct24', help='name of ROOT input directory')
    parser.add_argument('--out', '-o', action='store', dest='output_dir', default='output_files/example')

    main(parser.parse_args())
