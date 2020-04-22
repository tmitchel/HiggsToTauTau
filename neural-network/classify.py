from os import environ, path, mkdir, listdir, system
environ['KERAS_BACKEND'] = 'tensorflow'
from collections import defaultdict
from sklearn.preprocessing import StandardScaler
from keras.models import load_model
import sys
import ROOT
import numpy
import uproot
import pandas as pd
from glob import glob
from array import array
from pprint import pprint
import multiprocessing
import subprocess


def build_filelist(input_dir):
    """Build list of files to be processed."""
    files = [
        ifile for ifile in glob('{}/*/merged/*.root'.format(input_dir))
        if not 'VBF_Rivet' in ifile
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


def classify(ifile, tree_prefix, scaler, scaler_info, model_name, output_dir):
    fname = ifile.replace('.root', '').split('/')[-1]
    # print 'Processing file: {} from {}'.format(fname, ifile.split('merged')[0].split('/')[-2])

    # load the model
    model = load_model('Output/models/{}.hdf5'.format(model_name))

    # read input and get things ready for output TTree
    open_file = uproot.open(ifile)
    oldtree = open_file[tree_prefix].arrays(['*'])
    treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}
    treedict['NN_disc'] = numpy.float64

    # drop all variables not going into the network
    to_classify = open_file[tree_prefix].arrays(scaler_info, outputtype=pd.DataFrame)

    # clean inputs
    to_classify.fillna(-100, inplace=True)
    to_classify.replace([numpy.inf, -numpy.inf], -100, inplace=True)

    # scale correctly
    scaled = pd.DataFrame(
        scaler.transform(to_classify.values),
        columns=to_classify.columns.values, dtype='float64')

    # There's room here to try and optimize by only classifying VBF events and storing a
    # default value for others. Just need to figure out how to keep everything in order
    # so that it can be slotted back into the correct place in the TTree.

    # do the classification
    guesses = model.predict(scaled.values, verbose=False)

    with uproot.recreate('Output/trees/{}/{}.root'.format(output_dir, fname)) as f:
        f[tree_prefix] = uproot.newtree(treedict)
        oldtree['NN_disc'] = guesses.reshape(len(guesses))
        f[tree_prefix].extend(oldtree)

    return None

def main(args):
    # this will be removed soon
    if 'mutau' in args.input_dir or 'mt20' in args.input_dir:
        tree_prefix = 'mt_tree'
    elif 'etau' in args.input_dir or 'et20' in args.input_dir:
        tree_prefix = 'et_tree'
    else:
        raise Exception(
            'Input files must have MUTAU or ETAU in the provided path. You gave {}, ya goober.'.format(args.input_dir))

    # get scaler setup
    scaler = StandardScaler()
    scaler_info = pd.HDFStore(args.input_name)['scaler']
    scaler_info = scaler_info.drop('isSM', axis=0)
    scaler.mean_ = scaler_info['mean'].values.reshape(1, -1)
    scaler.scale_ = scaler_info['scale'].values.reshape(1, -1)
    scaler_columns = scaler_info.index.values

    # create output directory
    if not path.isdir('Output/trees/{}'.format(args.output_dir)):
        mkdir('Output/trees/{}'.format(args.output_dir))

    filelist = build_filelist(args.input_dir)
    print 'Files to process...'
    pprint(dict(filelist))
    nsyst = len(filelist.keys())
    i = 0
    for syst, ifiles in filelist.iteritems():
        # create output sub-directory (needed for systematics/nominal)
        out_path = 'Output/trees/{}/{}'.format(args.output_dir, syst)
        if not path.exists(out_path):
            mkdir(out_path)

        n_processes = min(12, multiprocessing.cpu_count() / 2)
        pool = multiprocessing.Pool(processes=n_processes)
        jobs = [pool.apply_async(classify, (ifile, tree_prefix, scaler, scaler_columns, args.model,
                                       '{}/{}'.format(args.output_dir, syst))) for ifile in ifiles]

        [j.get() for j in jobs]
        pool.close()
        pool.join()
        i += 1
        print 'All files written for {} ({} of {})'.format(syst, i, nsyst)
        if args.move != None:
            system('mkdir -p {}'.format(args.move))
            subprocess.Popen('nohup', 'mv', out_path, '{}/'.format(args.move))
            print 'Moved files from {} to {}'.format(out_path, args.move)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--model', '-m', action='store', dest='model', default='testModel', help='name of model to use')
    parser.add_argument('--input', '-i', action='store', dest='input_name',
                        default='test', help='name of input dataset')
    parser.add_argument('--dir', '-d', action='store', dest='input_dir',
                        default='input_files/etau_stable_Oct24', help='name of ROOT input directory')
    parser.add_argument('--out', '-o', action='store', dest='output_dir', default='output_files/example')
    parser.add_argument('--move', action='store', default=None, help='Move files to this location while running')

    main(parser.parse_args())
