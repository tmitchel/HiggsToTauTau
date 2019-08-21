
import os
from glob import glob
from pprint import pprint


def clean(hadd_list):
    for idir, isample in hadd_list.items():
        for sample, files in isample.items():
            if len(files) == 0:
                del hadd_list[idir][sample]
    print 'About to add with the following file setup...'
    pprint(hadd_list)
    return hadd_list

def do_hadd(hadd_list, path):
    for idir, isamples in hadd_list.items():
        if not os.path.exists(path + '/' + idir + '/merged'):
            os.mkdir(path + '/' + idir + '/merged')
        for sample, files in isamples.items():
            os.system('hadd {}/{}.root {}'.format(path + '/' + idir + '/merged', sample, ' '.join(files)))    

def main(args):
    bkgs = [
        'ZJ', 'ZL', 'ZTT', 'embed', 'data_obs', 'VVJ', 'VVT', 'TTT', 'TTJ', 'W',
    ]
    signals = [
        'ggh125_JHU_a1-prod', 'ggh125_JHU_a3-prod', 'ggh125_JHU_a3int-prod',
        'vbf125_JHU_a1-prod', 'vbf125_JHU_a2-prod', 'vbf125_JHU_a2int-prod',
        'vbf125_JHU_a3-prod', 'vbf125_JHU_a3int-prod', 'vbf125_JHU_l1-prod',
        'vbf125_JHU_l1int-prod', 'vbf125_JHU_l1zg-prod', 'vbf125_JHU_l1zgint-prod',
        'wh125_JHU_a1-prod', 'wh125_JHU_a2-prod', 'wh125_JHU_a2int-prod', 'wh125_JHU_a3-prod',
        'wh125_JHU_a3int-prod', 'wh125_JHU_l1-prod', 'wh125_JHU_l1int-prod',
        'wh125_JHU_l1zg-prod', 'wh125_JHU_l1zgint-prod',
        'zh125_JHU_a1-prod', 'zh125_JHU_a2-prod', 'zh125_JHU_a2int-prod', 'zh125_JHU_a3-prod',
        'zh125_JHU_a3int-prod', 'zh125_JHU_l1-prod', 'zh125_JHU_l1int-prod',
        'zh125_JHU_l1zg-prod', 'zh125_JHU_l1zgint-prod',

        'ggh125_madgraph_two_a1-prod', 'ggh125_madgraph_two_a3-prod', 'ggh125_madgraph_two_a3int-prod',
        'vbf125_madgraph',
        'ggh125_powheg', 'vbf125_powheg', 'zh125_powheg'
    ]
    bkg_hadd_list = {
        idir: {
            sample: [
                ifile for ifile in glob('{}/*_{}_*.root'.format(args.path + '/' +idir, sample)) if not 'EWK_W' in ifile
            ] for sample in bkgs
        } for idir in os.listdir(args.path) if os.path.isdir(args.path + '/' + idir) and not 'logs' in idir
    }
    sig_hadd_list= {
        idir: {
            sample: [
                ifile for ifile in glob('{}/{}*.root'.format(args.path + '/' +idir, sample)) if not 'EWK_W' in ifile
            ] for sample in signals
        } for idir in os.listdir(args.path) if os.path.isdir(args.path + '/' + idir) and not 'logs' in idir
    }

    bkg_hadd_list = clean(bkg_hadd_list)
    do_hadd(bkg_hadd_list, args.path)

    sig_hadd_list = clean(sig_hadd_list)
    do_hadd(sig_hadd_list, args.path)



if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--path', '-p', required=True, help='path to files')
    main(parser.parse_args())
