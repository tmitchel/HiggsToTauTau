from math import ceil
from glob import glob
import subprocess
import pandas
import numpy
import pprint
import uproot

def find_tree(keys, valid_trees):
    for key in keys:
        clean_key = key.split(';')[0] # remove ;1 or any other cycle number
        if clean_key in valid_trees:
            return clean_key
    return None


def main(args):
    valid_trees = ['etau_tree', 'mutau_tree', 'tt_tree', 'em_tree']
    
    files = [
        ifile for ifile in glob('{}/*.root'.format(args.input))
    ]
    for ifile in files:
        open_file = uproot.open(ifile)
        tree_name = find_tree(open_file.keys(), valid_trees)
        if tree_name == None:
            raise Exception('No acceptable tree was found.')

        input_tree = open_file[tree_name]
        if input_tree.numentries < args.max_entries:
            continue

        print 'Going to split {}'.format(ifile)

        nevents = open_file['nevents']
        steps = ceil(input_tree.numentries / args.max_entries)

        print 'Splitting into {} files'.format(steps)
        base_name = ifile.split('/')[-1]
        for step in range(int(steps) + 1):
            file_name = base_name.replace('.root', '_split{}.root'.format(i))
            call('rooteventselector -f {} -l {} {}:{} {}'.format(
                args.max_entries * step, args.max_entries * (step + 1), ifile, tree_name, file_name))
            with uproot.update(file_name) as f:
                f['nevents'] = nevents


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', required=True)
    parser.add_argument('--max-entries', type=int, required=True)
    main(parser.parse_args())
