
import os
import json
import multiprocessing
from glob import glob
from pprint import pprint


def clean(hadd_list):
    """Remove any entry with 0 files."""
    for idir, isample in hadd_list.items():
        for sample, files in isample.items():
            if len(files) == 0:
                del hadd_list[idir][sample]
    return hadd_list


def do_hadd(hadd_list, path):
    """Start hadding files."""
    ndir = len(hadd_list.keys())
    i = 0
    for idir, isamples in hadd_list.items():
        i += 1
        print '\n\033[92m Begin directory {} of {} \033[0m'.format(i, ndir)
        if not os.path.exists(path + '/' + idir + '/merged'):
            os.mkdir(path + '/' + idir + '/merged')

        commands = ['ahadd.py {}/{}.root {}'.format(path + '/' + idir + '/merged', sample, ' '.join(files))
                    for sample, files in isamples.items()]

#         commands = ['hadd {}/{}.root {}'.format(path + '/' + idir + '/merged', sample, ' '.join(files))
#                     for sample, files in isamples.items() if not 'wh125_JHU' in sample and not 'zh125_JHU' in sample]

        n_processes = min(12, multiprocessing.cpu_count() / 2)
        pool = multiprocessing.Pool(processes=n_processes)
        r = pool.map_async(os.system, commands)
        r.wait()
        # for sample, files in isamples.items():
        #     os.system('hadd {}/{}.root {}'.format(path + '/' + idir + '/merged', sample, ' '.join(files)))


def combine_wh(hadd_list, path):
    """Take care of combining signed wh powheg samples."""
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
    """Strip suffix off WH/ZH files and copy them into merged directory without hadding."""
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
    """Remove background files we don't want."""
    if not 'EWK_W' in ifile and not 'EWKZ' in ifile and not 'WW_VV' in ifile and not 'WZ_VV' in ifile and not 'ZZ_VV' in ifile:
        return True
    return False


def good_sig(ifile):
    """Remove signal files we don't want."""
    if 'decay' in ifile and not 'nom-decay' in ifile:
        return False
    elif 'madgraph' in ifile and 'inc' in ifile:
        return False
    return True


def main(args):
    """Build list of files and hadd them together."""
    bkgs = [
        'data_obs', 'embed', 
        'ZJ', 'ZTT', 'ZL',
        'VVJ', 'VVT', 'VVL', 
        'TTJ', 'TTT', 'TTL', 
        'STJ', 'STT', 'STL', 
        'W',
    ]
    signals = [
        'ggh125_JHU', 'vbf125_JHU', 'wh125_JHU', 'zh125_JHU',
        'ggh125_madgraph',
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

    # keep list of what is being hadded together
    with file('haddlog.txt', 'a') as outfile:
        json.dump({
            'background': bkg_hadd_list,
            'signal': sig_hadd_list
        }, outfile, sort_keys=True, indent=4, separators=(',', ': '))

    full_hadd_list = bkg_hadd_list
    for isyst, samples in sig_hadd_list.iteritems():
        for sample, files in samples.iteritems():
            full_hadd_list[isyst][sample] = files

    do_hadd(full_hadd_list, args.path)
    # do_hadd(bkg_hadd_list, args.path)
    # do_hadd(sig_hadd_list, args.path)
    # rename_wh_zh(sig_hadd_list, args.path)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--path', '-p', required=True, help='path to files')
    main(parser.parse_args())
