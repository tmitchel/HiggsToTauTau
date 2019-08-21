
import os
from glob import glob
from pprint import pprint


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
    hadd_list = {
        idir: {
            sample: [
                ifile for ifile in glob('{}/*_{}_*.root'.format(args.path + '/' +idir, sample)) if not 'EWK_W' in ifile
            ] for sample in bkgs
        } for idir in os.listdir(args.path) if os.path.isdir(args.path + '/' + idir) and not 'logs' in idir
    }
    hadd_list.update({
        idir: {
            sample: [
                ifile for ifile in glob('{}/{}*.root'.format(args.path + '/' +idir, sample)) if not 'EWK_W' in ifile
            ] for sample in signals
        } for idir in os.listdir(args.path) if os.path.isdir(args.path + '/' + idir) and not 'logs' in idir
    })
    for idir, isample in hadd_list.items():
        for sample, files in isample.items():
            if len(files) == 0:
                del hadd_list[idir][sample]
    print 'About to add with the following file setup...'
    pprint(hadd_list)

    for idir, isamples in hadd_list.items():
        if not os.path.exists(args.path + '/' + idir + '/merged'):
            os.mkdir(args.path + '/' + idir + '/merged')
        for sample, files in isamples.items():
            os.system('hadd {}/{}.root {}'.format(idir + '/merged', sample, ' '.join(files)))    

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--path', '-p', required=True, help='path to files')
    main(parser.parse_args())
