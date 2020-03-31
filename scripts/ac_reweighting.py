import json
import pandas
import pprint
import uproot
from glob import glob
from subprocess import call


temp_wh_zh_map = {
    'wh125_JHU_a1': 'JHU__reweighted_WH_htt_0PM125',
    'wh125_JHU_a2': 'JHU__reweighted_WH_htt_0PH125',
    'wh125_JHU_a2int': 'JHU__reweighted_WH_htt_0PHf05ph0125',
    'wh125_JHU_a3': 'JHU__reweighted_WH_htt_0Mf05ph0125',
    'wh125_JHU_a3int': 'JHU__reweighted_WH_htt_0M125',
    'wh125_JHU_l1': 'JHU__reweighted_WH_htt_0L1125',
    'wh125_JHU_l1int': 'JHU__reweighted_WH_htt_0L1f05ph0125',
    'wh125_JHU_l1zg': 'JHU__reweighted_WH_htt_0L1Zg125',
    'wh125_JHU_l1zgint': 'JHU__reweighted_WH_htt_0L1Zgf05ph0125',
    'zh125_JHU_a1': 'JHU__reweighted_ZH_htt_0PM125',
    'zh125_JHU_a2': 'JHU__reweighted_ZH_htt_0PH125',
    'zh125_JHU_a2int': 'JHU__reweighted_ZH_htt_0PHf05ph0125',
    'zh125_JHU_a3': 'JHU__reweighted_ZH_htt_0Mf05ph0125',
    'zh125_JHU_a3int': 'JHU__reweighted_ZH_htt_0M125',
    'zh125_JHU_l1': 'JHU__reweighted_ZH_htt_0L1125',
    'zh125_JHU_l1int': 'JHU__reweighted_ZH_htt_0L1f05ph0125',
    'zh125_JHU_l1zg': 'JHU__reweighted_ZH_htt_0L1Zg125',
    'zh125_JHU_l1zgint': 'JHU__reweighted_ZH_htt_0L1Zgf05ph0125',
}

powheg_map = {
    "ggh125_minlo": "ggH_MINLO125",
    "ggh125_powheg": "ggH125",
    "vbf125_powheg": "VBF125",
    "wh125_powheg": "WH125",
    "zh125_powheg": "ZH125",
}


def to_reweight(ifile):
    """List of signal samples. Only processes these files."""
    for name in [
        'ggh125_madgraph.root',
        'vbf125_JHU.root', 'wh125_JHU.root', 'zh125_JHU.root'
        ]:
        if name in ifile:
            return True
    return False


def recognize_signal(ifile, is2017):
    """Pick the correct keys for this sample."""
    process = ifile.split('/')[-1].split('125')[0]
    key = ''
    if process == 'ggh':
        key = 'new_mg_ac_reweighting_map'
    elif process == 'ggh' and is2017:
        key = 'mg_ac_reweighting_map'
    else:
        key = 'jhu_ac_reweighting_map'
    return key, process


def handle_wh_zh(ifile):
    """Copy the file and fix the name."""
    sample_name = ifile.split('/')[-1].replace('.root', '')
    new_name = temp_wh_zh_map[sample_name]
    new_file_name = ifile.replace(sample_name, new_name)
    call('mv -v {} {}'.format(ifile, new_file_name), shell=True)


def parse_tree_name(keys):
    """Take list of keys in the file and search for our TTree"""
    if 'et_tree;1' in keys:
        return 'et_tree'
    elif 'mt_tree;1' in keys:
        return 'mt_tree'
    else:
        raise Exception('Can\'t find et_tree or mt_tree in keys: {}'.format(keys))


def handle_powheg(ifile):
    """Copy the file and fix the name."""
    filename = ifile.split('/')[-1].replace('.root', '')
    new_name = powheg_map[filename]
    new_file_name = ifile.replace(filename, new_name)
    call('mv -v {} {}'.format(ifile, new_file_name), shell=True)


def main(args):
    input_directories = [idir for idir in glob('{}/*'.format(args.input)) if not 'logs' in idir]
    input_files = {
        idir: [ifile for ifile in glob('{}/merged/*.root'.format(idir)) if to_reweight(ifile)]
        for idir in input_directories
    }
    print 'Directory structure to process'
    pprint.pprint(input_files)

    boilerplate = {}
    with open('configs/boilerplate.json', 'r') as config_file:
        boilerplate = json.load(config_file)

    for idir, files in input_files.iteritems():
        for ifile in files:
            # if 'powheg' in ifile or 'minlo' in ifile:
            #     handle_powheg(ifile)
            #     continue
            # until weights are corrected, don't reweight WH or ZH
            # if 'wh125_JHU' in ifile or 'zh125_JHU' in ifile:
            #     handle_wh_zh(ifile)
            #     continue

            open_file = uproot.open(ifile)
            tree_name = parse_tree_name(open_file.keys())
            oldtree = open_file[tree_name].arrays(['*'])
            treedict = {ikey: oldtree[ikey].dtype for ikey in oldtree.keys()}

            # build DataFrame
            events = pandas.DataFrame(oldtree)
            signal_events = events[(events['is_signal'] > 0)]

            key, process = recognize_signal(ifile, args.is2017)
            weight_names = boilerplate[key][process]
            for weight, name in weight_names:
                print idir, ifile, name
                weighted_signal_events = signal_events.copy(deep=True)
                weighted_signal_events['evtwt'] *= weighted_signal_events[weight]

                output_name = '{}.root'.format(name) if '/hdfs' in args.input else '{}/merged/{}.root'.format(idir, name)
                with uproot.recreate(output_name) as f:
                    f[tree_name] = uproot.newtree(treedict)
                    f[tree_name].extend(weighted_signal_events.to_dict('list'))

                if '/hdfs' in args.input:
                    call('mv -v {}.root {}/merged'.format(name, idir), shell=True)

            call('mv -v {} {}'.format(ifile, ifile.replace('/merged', '')), shell=True)            


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--is2017', action='store_true', help='is this 2017?')
    main(parser.parse_args())
