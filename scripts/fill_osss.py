import numpy
import pandas
import uproot


backgrounds = ['ZTT', 'ZL', 'ZJ', 'TT', 'VV', 'W']


def parse_tree_name(keys):
    """Take list of keys in the file and search for our TTree"""
    if 'et_tree;1' in keys:
        return 'et_tree'
    elif 'mt_tree;1' in keys:
        return 'mt_tree'
    elif 'em_tree;1' in keys:
        return 'em_tree'
    else:
        raise Exception('Can\t find et_tree or mt_tree in keys: {}'.format(keys))


def categorize(events):
    """Sort events by isolation and charge"""
    passing = events[(events['is_signal'] == 1)]
    failing = events[(events['is_antiBothIso'] == 1)]

    os_passing = passing[(passing['OS'] == 1)]
    ss_passing = passing[(passing['OS'] == 0)]
    os_failing = failing[(failing['OS'] == 1)]
    ss_failing = failing[(failing['OS'] == 0)]

    return os_passing, ss_passing, os_failing, ss_failing


def main(args):
    open_file = uproot.open('{}/data_obs.root'.format(args.input))
    tree_name = parse_tree_name(open_file.keys())
    channel_prefix = tree_name[:2]
    oldtree = open_file[tree_name].arrays(['*'])
    treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}
    treedict['fake_weight'] = numpy.float64

    # categorize data
    events = pandas.DataFrame(oldtree)
    os_passing, ss_passing, os_failing, ss_failing = categorize(events)

    # categorize backgrounds
    for bkg in backgrounds:
        bkg_events = uproot.open('{}/{}.root'.format(args.input, bkg)
                                 )[tree_name].arrays(['*'], outputtype=pandas.DataFrame)
        bkg_events['evtwt'] *= -1  # subtract backgroungds from data
        bkg_os_passing, bkg_ss_passing, bkg_os_failing, bkg_ss_failing = categorize(bkg_events)
        os_passing = pandas.concat([os_passing, bkg_os_passing], ignore_index=True, sort=False)
        ss_passing = pandas.concat([ss_passing, bkg_ss_passing], ignore_index=True, sort=False)
        os_failing = pandas.concat([os_failing, bkg_os_failing], ignore_index=True, sort=False)
        ss_failing = pandas.concat([ss_failing, bkg_ss_failing], ignore_index=True, sort=False)

    os_ss_ratio = os_failing['evtwt'].sum() / ss_failing['evtwt'].sum()  # calculate ratio
    print 'OSSS ratio: {}'.format(os_ss_ratio)
    output_events = ss_passing.copy(deep=True)  # use passing same sign events for shape
    output_events['fake_weight'] = os_ss_ratio  # add weight branch to output

    with uproot.recreate('{}/QCD.root'.format(args.input)) as f:
        f[tree_name] = uproot.newtree(treedict)
        f[tree_name].extend(output_events.to_dict('list'))


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    main(parser.parse_args())
