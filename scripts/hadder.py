
import os
import json
import multiprocessing
from glob import glob
from pprint import pprint


def clean(hadd_list):
    for idir, isample in hadd_list.items():
        for sample, files in isample.items():
            if len(files) == 0:
                del hadd_list[idir][sample]
    return hadd_list


def do_hadd(hadd_list, path):
    for idir, isamples in hadd_list.items():
        if not os.path.exists(path + '/' + idir + '/merged'):
            os.mkdir(path + '/' + idir + '/merged')
            
        commands = ['hadd {}/{}.root {}'.format(path + '/' + idir + '/merged', sample, ' '.join(files))
                    for sample, files in isamples.items()]

        n_processes = min(12, multiprocessing.cpu_count() / 2)
        pool = multiprocessing.Pool(processes=n_processes)
        r = pool.map_async(os.system, commands)
        r.wait()
        # for sample, files in isamples.items():
        #     os.system('hadd {}/{}.root {}'.format(path + '/' + idir + '/merged', sample, ' '.join(files)))


def combine_wh(hadd_list, path):
    for idir in hadd_list.keys():
        wh_files = []
        for ifile in glob('{}/*.root'.format(path + '/' + idir)):
            if 'wplus125' in ifile:
                wh_files.append(ifile)
            elif 'wminus125' in ifile:
                wh_files.append(ifile)
        if len(wh_files) == 2:
            hadd_list[idir]['wh125_powheg'] = wh_files
    return hadd_list


def rename_wh_zh(hadd_list, path):
    for idir, samples in hadd_list.iteritems():
      if not 'wh125_JHU' in samples and not 'zh125_JHU' in samples:
          print '\033[93m[WARNING] No wh or zh samples in {}\033[0m'.format(idir)
          continue

      if 'wh125_JHU' in samples:
        for ifile in samples['wh125_JHU']:
            new_name = ifile.split('/')[-1].split('-prod_nom-decay')[0] + '.root'  
            os.system('cp {} {}/{}/merged/{}'.format(ifile, path, idir, new_name))

      if 'zh125_JHU' in samples:
        for ifile in samples['zh125_JHU']:
            new_name = ifile.split('/')[-1].split('-prod_nom-decay')[0] + '.root'  
            os.system('cp {} {}/{}/merged/{}'.format(ifile, path, idir, new_name))
    

def good_bkg(ifile):
    if not 'EWK_W' in ifile and not 'EWKZ' in ifile:
        return True
    return False


def good_sig(ifile):
    if 'decay' in ifile and not 'nom-decay' in ifile:
        return False
    elif 'madgraph' in ifile and 'inc' in ifile:
        return False
    return True


def main(args):
    bkgs = [
        'ZJ', 'ZL', 'ZTT', 'embed', 'data_obs', 'VVJ', 'VVT', 'TTT', 'TTJ', 'W',
    ]
    signals = [
        'ggh125_JHU', 'vbf125_JHU', 'wh125_JHU', 'zh125_JHU',

        'ggh125_madgraph', 'ggh125_madgraph', 'ggh125_madgraph',
        # 'vbf125_madgraph',
        'ggh125_powheg', 'vbf125_powheg', 'zh125_powheg'
    ]
    bkg_hadd_list = {
        idir: {
            sample: [
                ifile for ifile in glob('{}/*_{}_*.root'.format(args.path + '/' + idir, sample)) if good_bkg(ifile)
            ] for sample in bkgs
        } for idir in os.listdir(args.path) if os.path.isdir(args.path + '/' + idir) and not 'logs' in idir
    }
    sig_hadd_list = {
        idir: {
            sample: [
                ifile for ifile in glob('{}/{}*.root'.format(args.path + '/' + idir, sample)) if good_sig(ifile)
            ] for sample in signals
        } for idir in os.listdir(args.path) if os.path.isdir(args.path + '/' + idir) and not 'logs' in idir
    }

    bkg_hadd_list = clean(bkg_hadd_list)
    sig_hadd_list = clean(sig_hadd_list)
    sig_hadd_list = combine_wh(sig_hadd_list, args.path)

    with file('haddlog.txt', 'a') as outfile:
        json.dump({
            'background': bkg_hadd_list,
            'signal': sig_hadd_list
        }, outfile, sort_keys=True, indent=4, separators=(',', ': '))

    do_hadd(bkg_hadd_list, args.path)
    do_hadd(sig_hadd_list, args.path)
    rename_wh_zh(sig_hadd_list, args.path)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--path', '-p', required=True, help='path to files')
    main(parser.parse_args())
