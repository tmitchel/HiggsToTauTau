import pprint
import uproot
from glob import glob


def to_reweight(ifile):
    for name in ['ggh125_madgraph.root', 'vbf125_JHU.root', 'wh125_JHU.root', 'zh125_JHU.root']:
        if name in ifile:
            return True
    return False


def recognize_signal(ifile, is2018):
    process = ifile.split('/')[-1].split('125')[0]
    key = ''
    if process == 'ggh' and is2018:
        key = 'new_mg_ac_reweighting_map'
    elif process == 'ggh':
        key = 'mg_ac_reweighting_map'
    else:
        key = 'jhu_ac_reweighting_map'
    return key, process


def main(args):
    input_directories = [idir for idir in glob('{}/*'.format(args.input)) if not 'logs' in idir]
    input_files = {
        idir: [ifile for ifile in glob('{}/merged/*.root'.format(idir) if to_reweight(ifile))]
        for idir in input_directories
    }
    print 'Directory structure to process'
    pprint.pprint(inut_files)

    boilerplate = {}
    with open('configs/boilerplate.json', 'r') as config_file:
        boilerplate = json.load(config_file)

    for idir, files in input_files.iteritems():
        for ifile in files:
            open_file = uproot.open(ifile)
            oldtree = open_file[args.tree_name].arrays(['*'])
            treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}

            # build DataFrame
            events = pandas.DataFrame(oldtree)
            signal_events = events[(events['is_signal'] > 0)]

            key, process = recognize_signal(ifile, args.is2018)
            weight_names = boilerplate[key][process]
            for weight, name in weight_names:
                weighted_signal_events = signal_events.copy(deep=True)
                weighted_signal_events['evtwt'] *= weighted_signal_events[weight]

                with uproot.recreate('{}/{}.root'.format(idir, name), compression=None) as f:
                    f[args.tree_name] = uproot.newtree(treedict)
                    f[args.tree_name].extend(weighted_signal_events.to_dict('list'))


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--tree-name', '-t', required=True, help='name of TTree')
    parser.add_argument('--is2018', action='store_tree', help='is this 2018?')
    main(parser.parse_args())
