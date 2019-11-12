############################################################
## Script to automate running an analyzer on all files in ##
## a directory.                                           ##
############################################################

from os import popen, makedirs, path
from pprint import pprint
from subprocess import call
import time
import multiprocessing
from glob import glob


def getNames(sample):
    if 'DYJets' in sample:
        names = ['ZL', 'ZJ', 'ZTT']
    elif 'TT' in sample:
        names = ['TTT', 'TTJ']
    elif 'WJets' in sample:
        names = ['W']
    elif 'EWKW' in sample:
        names = ['EWK_W']
    elif 'data' in sample:
        names = ['data_obs']
    elif 'ggh125' in sample.lower():
        names = ['ggH125']
    elif 'vbf125' in sample.lower():
        names = ['VBF125']
    elif 'wplus' in sample.lower() or 'wminus' in sample.lower():
        names = ['WHsigned125']
    elif 'wh125' in sample.lower():
        names = ['WH125']
    elif 'zh125' in sample.lower():
        names = ['ZH125']
    elif 'ttH' in sample or 'tth' in sample:
        names = ['ttH125']
    elif 'embed' in sample:
        names = ['embed']
    else:
        names = ['VVJ', 'VVT']

    signal_type = 'None'
    if 'JHU' in sample:
        signal_type = 'JHU'
    elif 'madgraph' in sample:
        signal_type = 'madgraph'
    elif 'minlo' in sample:
        signal_type = 'minlo'
    elif 'powheg' in sample:
        signal_type = 'powheg'

    return names, signal_type


def getSyst(name, signal_type, exe):
    systs = ['']

    if name == 'TTT' or name == 'VVT' or name == 'embed' or name == 'ZTT' or signal_type != 'None':
        systs += ['tau_id_Up', 'tau_id_Down']  # names will probably be updated
        systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']

    if name == 'ZL' or name == 'W':
        systs += ['LES_DM0_Up', 'LES_DM0_Down', 'LES_DM1_Up', 'LES_DM1_Down']

    if name != 'embed' and name != 'data_obs':
        systs += ['UncMet_Up', 'UncMet_Down', 'ClusteredMet_Up', 'ClusteredMet_Down']
        systs += [
            'JetRelativeSample_Up', 'JetRelativeSample_Down',
            'JetRelativeBal_Up', 'JetRelativeBal_Down',
            'JetEta0to3_Up', 'JetEta0to3_Down', 'JetEta0to5_Up', 'JetEta0to5_Down',
            'JetEta3to5_Up', 'JetEta3to5_Down', 'JetEC2_Up', 'JetEC2_Down'
        ]

    if name == 'TTT' or name == 'TTJ':
        systs += ['ttbarShape_Up', 'ttbarShape_Down']

    if name == 'TTJ' or name == 'ZJ' or name == 'VVJ' or name == 'W':
        systs += ['jetToTauFake_Up', 'jetToTauFake_Down']

    if name == 'ZJ' or name == 'ZL' or name == 'ZTT':
        systs += ['dyShape_Up', 'dyShape_Down', 'zmumuShape_Up', 'zmumuShape_Down']

    if name != 'data_obs' and '_et' in exe:
        systs += ['EEScale_Up', 'EEScale_Down', 'EESigma_Up', 'EESigma_Down']
    elif name != 'data_obs' and '_mt' in exe:
        systs += [
            'MESbin1_Up', 'MESbin1_Down', 'MESbin2_Up', 'MESbin2_Down',
            'MESbin3_Up', 'MESbin3_Down', 'MESbin4_Up', 'MESbin4_Down',
            'MESbin5_Up', 'MESbin5_Down',
        ]

    if name == 'ggH125' and signal_type == 'powheg':
        systs += [
            'Rivet0_Up', 'Rivet0_Down', 'Rivet1_Up', 'Rivet1_Down', 'Rivet2_Up', 'Rivet2_Down',
            'Rivet3_Up', 'Rivet3_Down', 'Rivet4_Up', 'Rivet4_Down', 'Rivet5_Up', 'Rivet5_Down',
            'Rivet6_Up', 'Rivet6_Down', 'Rivet7_Up', 'Rivet7_Down', 'Rivet8_Up', 'Rivet8_Down',
        ]

    if name == 'ZJ' or name == 'ZL' or name == 'ZTT' or name == 'ggH125' or name == 'VBF125' or name == 'W':
        systs += [
            'RecoilReso_0jet_Up', 'RecoilReso_0jet_Down', 'RecoilResp_0jet_Up', 'RecoilResp_0jet_Down',
            'RecoilReso_1jet_Up', 'RecoilReso_1jet_Down', 'RecoilResp_1jet_Up', 'RecoilResp_1jet_Down',
            'RecoilReso_2jet_Up', 'RecoilReso_2jet_Down', 'RecoilResp_2jet_Up', 'RecoilResp_2jet_Down',
        ]

    return systs


def run_command(cmd, q, parallel=False):
    code = call(cmd, shell=True)
    message = ''
    if code != 0:
        message = '\033[91m[ERROR] returned non-zero exit code while running {}\033[0m'.format(cmd)
    else:
        message = '\033[92m[SUCCESS] {} completed successfully \033[0m'.format(cmd)

    # print message
    print message

    # write to log or queue depending on if multiprocessing
    file_message = message[5:-5]  # strip colors off message for file
    q.put(file_message) if parallel else q.write(file_message + '\n')
    return None


def listener(q, fname):
    '''listens for messages on the q, writes to file. '''

    with open(fname, 'w') as f:
        while 1:
            m = q.get()
            if m == 'kill':
                f.write('killed')
                break
            f.write(str(m) + '\n')
            f.flush()


def main(args):
    suffix = '.root'

    try:
        makedirs('Output/trees/{}/logs'.format(args.output_dir))
    except:
        pass

    start = time.time()
    fileList = [ifile for ifile in glob(args.path+'/*') if '.root' in ifile]

    get_systs = None
    if args.syst == '2016':
        get_systs = getSyst
    elif args.syst == '2017':
        get_systs = getSyst
    elif args.syst == '2018':
        get_systs = getSyst

    processes = []

    for ifile in fileList:
        sample = ifile.split('/')[-1].split(suffix)[0]
        tosample = ifile.replace(sample+suffix, '')

        names, signal_type = getNames(sample)
        callstring = './{} -p {} -s {} -d {} --stype {} '.format(args.exe,
                                                                 tosample, sample, args.output_dir, signal_type)

        if get_systs != None and not 'Data' in sample.lower():
            for name in names:
                for isyst in get_systs(name, signal_type, args.exe):
                    if isyst == "" and not path.exists('Output/trees/{}/NOMINAL'.format(args.output_dir)):
                        makedirs('Output/trees/{}/NOMINAL'.format(args.output_dir))
                    if isyst != "" and not path.exists('Output/trees/{}/SYST_{}'.format(args.output_dir, isyst)):
                        makedirs('Output/trees/{}/SYST_{}'.format(args.output_dir, isyst))

                    tocall = callstring + ' -n %s -u %s' % (name, isyst)
                    processes.append(tocall)
        else:
            for name in names:
                if not path.exists('Output/trees/{}/NOMINAL'.format(args.output_dir)):
                    makedirs('Output/trees/{}/NOMINAL'.format(args.output_dir))
                tocall = callstring + ' -n %s ' % name
                processes.append(tocall)

    pprint(processes)
    if args.parallel:
        manager = multiprocessing.Manager()
        q = manager.Queue()

        # Use 10 cores if the machine has more than 20 total cores.
        # Otherwise, use half the available cores.
        n_processes = min(10, multiprocessing.cpu_count() / 2)
        pool = multiprocessing.Pool(processes=n_processes)
        watcher = pool.apply_async(listener, (q, 'Output/trees/{}/logs/runninglog.txt'.format(args.output_dir)))

        jobs = []
        for command in processes:
            job = pool.apply_async(run_command, (command, q, True))
            jobs.append(job)

        for job in jobs:
            job.get()

        q.put('kill')
        pool.close()
        pool.join()
    else:
        with open('Output/trees/{}/logs/runninglog.txt'.format(args.output_dir), 'w') as ifile:
            [run_command(command, ifile, False) for command in processes]

    end = time.time()
    print 'Processing completed in', end-start, 'seconds.'


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--exe', '-e', required=True, help='name of executable')
    parser.add_argument('--syst', default=None, help='run systematics as well')
    parser.add_argument('--path', '-p', required=True, help='path to input file directory')
    parser.add_argument('--parallel', action='store_true', help='run in parallel')
    parser.add_argument('--output-dir', required=True, dest='output_dir',
                        help='name of output directory after Output/trees')
    main(parser.parse_args())
