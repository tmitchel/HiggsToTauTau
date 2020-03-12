from math import ceil
from glob import glob
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
        branches = input_tree.arrays(['*'])
        treedict = {ikey: branches[ikey].dtype for ikey in branches.keys()}
        pprint.pprint(treedict)
        all_events = pandas.DataFrame(branches)

        print numpy.unique(treedict.values())

        steps = ceil(input_tree.numentries / args.max_entries)
        print 'Splitting into {} files'.format(steps)
        split_events = [all_events.iloc[(args.max_entries * step):(args.max_entries * (step + 1))] for step in range(int(steps) + 1)]

        base_name = ifile.split('/')[-1]
        i = 0
        for split in split_events:
            file_name = base_name.replace('.root', '_split{}.root'.format(i))
            with uproot.recreate(file_name) as f:
                f[tree_name] = uproot.newtree(treedict)
                f[tree_name].extend(split.to_dict('list'))
                f['nevents'] = nevents
            print 'File: {} written'.format(file_name)
            i += 1


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', required=True)
    parser.add_argument('--max-entries', type=int, required=True)
    main(parser.parse_args())
