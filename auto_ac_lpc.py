############################################################
## Script to automate running an analyzer on all files in ##
## a directory.                                           ##
############################################################

from os import popen, makedirs, path
from pprint import pprint
from subprocess import call, check_output
import time
import multiprocessing
from glob import glob
from collections import defaultdict
from raw_condor_submit import submit_command


def getNames(sample):
    """Return the sample names and signal type."""
    if 'DYJets' in sample:
        names = ['ZL', 'ZJ', 'ZTT']
    elif 'TT' in sample:
        names = ['TTT', 'TTJ', 'TTL']
    elif '-tW' in sample or '-tchan' in sample:
        names = ['STT', 'STL', 'STJ']
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
        names = ['VVJ', 'VVT', 'VVL']

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


def valid_sample(ifile):
    """Remove samples that aren't used any longer"""
    invalid_samples = ['EWKZ', 'EWKW', 'WW.root', 'WZ.root', 'ZZ.root', 'ggh125_madgraph_inc', 'minlo']
    for sample in invalid_samples:
        if sample in ifile:
            return False
    return True


def getSyst(name, signal_type, exe, doSyst):
    """Return the list of systematics to be processed for this sample.

    The list of systematics is built based on the process, signal type, and channel.
    All applicable systematics will be added to the list for processing.
    Arguments:
    name        -- name of the process
    signal_type -- signal type or None
    exe         -- name of the executable to determine the channel
    doSyst      -- if False, returns a list with just the nominal case
    Returns:
    systs       -- list of systematics to processes
    """
    systs = ['']

    # handle cases with no systematics
    if not doSyst or signal_type == 'minlo' or name == 'data_obs' or (
            name == 'TTJ' or name == 'VVJ' or name == 'ZJ' or name == 'STJ' or name == 'W'):
        return systs

    if name == 'TTT' or name == 'VVT' or name == 'embed' or name == 'ZTT' or name == 'STT' or signal_type != 'None':
        # tau id vsJet systematics
        systs += [
            'tau_id_pt_30to35_Up', 'tau_id_pt_30to35_Down',
            'tau_id_pt_35to40_Up', 'tau_id_pt_35to40_Down',
            'tau_id_pt_ptgt40_Up', 'tau_id_pt_ptgt40_Down',
        ]
        # this is for once the embedded energy scale is updated
        # if name == 'embed':
        #     systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down', 'DM11_Up', 'DM11_Down']
        # else:
        #     # tau energy scale systematics (will be split by pT as well soon)
        #     systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down', 'DM11_Up', 'DM11_Down']
        # tau energy scale systematics (will be split by pT as well soon)
        systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down', 'DM11_Up', 'DM11_Down']

    if name == 'ZL' or name == 'TTL' or name == 'VVL' or name == 'STL' or name == 'embed':
        if '_et' in exe:
            # tau id vsEl systematics
            systs += [
                'tau_id_el_disc_barrel_Up', 'tau_id_el_disc_barrel_Down',
                'tau_id_el_disc_endcap_Up', 'tau_id_el_disc_endcap_Down',
            ]
            if name != 'embed':
                # electron faking tau energy scale systematics (will be included eventually)
                systs += [
                    'efaket_es_barrel_DM0_Up', 'efaket_es_barrel_DM0_Down',
                    'efaket_es_endcap_DM0_Up', 'efaket_es_endcap_DM0_Down',
                    'efaket_es_barrel_DM1_Up', 'efaket_es_barrel_DM1_Down',
                    'efaket_es_endcap_DM1_Up', 'efaket_es_endcap_DM1_Down',
                    'efaket_norm_pt30to40_Up', 'efaket_norm_pt30to40_Down',
                    'efaket_norm_pt40to50_Up', 'efaket_norm_pt40to50_Down',
                    'efaket_norm_ptgt50_Up', 'efaket_norm_ptgt50_Down',
                ]
        elif '_mt' in exe:
            # tau id vsMu systematics
            systs += [
                'tau_id_mu_disc_eta_lt0p4_Up', 'tau_id_mu_disc_eta_lt0p4_Down',
                'tau_id_mu_disc_eta_0p4to0p8_Up', 'tau_id_mu_disc_eta_0p4to0p8_Down',
                'tau_id_mu_disc_eta_0p8to1p2_Up', 'tau_id_mu_disc_eta_0p8to1p2_Down',
                'tau_id_mu_disc_eta_1p2to1p7_Up', 'tau_id_mu_disc_eta_1p2to1p7_Down',
                'tau_id_mu_disc_eta_gt1p7_Up', 'tau_id_mu_disc_eta_gt1p7_Down',
            ]
            if name != 'embed':
                # muon faking tau energy scale systematics (currently identical to nominal,
                # but will be 1% uncorrelated across DM bins)
                systs += [
                    'mfaket_es_DM0_Up', 'mfaket_es_DM0_Down',
                    'mfaket_es_DM1_Up', 'mfaket_es_DM1_Down',
                ]

    if name != 'embed':
        # jet energy scale, unclustered MET, and JER
        systs += [
            'UncMet_Up', 'UncMet_Down',
            'JetJER_Up', 'JetJER_Down',
            'JetAbsolute_Up', 'JetAbsolute_Down',
            'JetAbsoluteyear_Up', 'JetAbsoluteyear_Down',
            'JetBBEC1_Up', 'JetBBEC1_Down',
            'JetBBEC1year_Up', 'JetBBEC1year_Down',
            'JetEC2_Up', 'JetEC2_Down',
            'JetEC2year_Up', 'JetEC2year_Down',
            'JetFlavorQCD_Up', 'JetFlavorQCD_Down',
            'JetHF_Up', 'JetHF_Down',
            'JetHFyear_Up', 'JetHFyear_Down',
            'JetRelBal_Up', 'JetRelBal_Down',
            'JetRelSam_Up', 'JetRelSam_Down',
        ]
        if '2016' in exe or '2017' in exe:
            systs += ['prefiring_up', 'prefiring_down']
    else:
        systs += ['tracking_up', 'tracking_down']

    systs += [
        'single_trigger_up', 'single_trigger_down',
        'cross_trigger_up', 'cross_trigger_down',
    ]

    if name == 'TTT' or name == 'TTL':
        systs += ['ttbarShape_Up', 'ttbarShape_Down']

    if name == 'ZL' or name == 'ZTT':
        systs += ['dyShape_Up', 'dyShape_Down']

    # lepton energy scales
    if '_et' in exe:
        systs += ['EEScale_Up', 'EEScale_Down']
    elif '_mt' in exe:
        systs += [
            'MES_gt2p1_Up', 'MES_gt2p1_Down',
            'MES_1p2to2p1_Up', 'MES_1p2to2p1_Down',
            'MES_lt1p2_Up', 'MES_lt1p2_Down',
        ]

    if name == 'ZJ' or name == 'ZL' or name == 'ZTT' or name == 'ggH125' or name == 'VBF125' or name == 'W':
        systs += [
            'RecoilReso_0jet_Up', 'RecoilReso_0jet_Down', 'RecoilResp_0jet_Up', 'RecoilResp_0jet_Down',
            'RecoilReso_1jet_Up', 'RecoilReso_1jet_Down', 'RecoilResp_1jet_Up', 'RecoilResp_1jet_Down',
            'RecoilReso_2jet_Up', 'RecoilReso_2jet_Down', 'RecoilResp_2jet_Up', 'RecoilResp_2jet_Down',
            ]

    if name == 'ggH125' and signal_type == 'powheg':
        systs += [
            'ggH_Rivet0_Up', 'ggH_Rivet0_Down', 'ggH_Rivet1_Up', 'ggH_Rivet1_Down', 'ggH_Rivet2_Up', 'ggH_Rivet2_Down',
            'ggH_Rivet3_Up', 'ggH_Rivet3_Down', 'ggH_Rivet4_Up', 'ggH_Rivet4_Down', 'ggH_Rivet5_Up', 'ggH_Rivet5_Down',
            'ggH_Rivet6_Up', 'ggH_Rivet6_Down', 'ggH_Rivet7_Up', 'ggH_Rivet7_Down', 'ggH_Rivet8_Up', 'ggH_Rivet8_Down',
        ]

    if name == 'VBF125' and signal_type == 'powheg':
        systs += [
            'VBF_Rivet0_Up', 'VBF_Rivet0_Down', 'VBF_Rivet1_Up', 'VBF_Rivet1_Down', 'VBF_Rivet2_Up', 'VBF_Rivet2_Down',
            'VBF_Rivet3_Up', 'VBF_Rivet3_Down', 'VBF_Rivet4_Up', 'VBF_Rivet4_Down', 'VBF_Rivet5_Up', 'VBF_Rivet5_Down',
            'VBF_Rivet6_Up', 'VBF_Rivet6_Down', 'VBF_Rivet7_Up', 'VBF_Rivet7_Down', 'VBF_Rivet8_Up', 'VBF_Rivet8_Down',
            'VBF_Rivet9_Up', 'VBF_Rivet9_Down',
        ]

    return systs


def run_command(cmd, q, parallel=False):
    """Run the provided command and write the output

    Arguments:
    cmd      -- the command to be run in the shell
    q        -- the writable object (must be a Queue if parallel is True)
    save_fcn -- are you running with multiprocessing? 
    """
    code = call(cmd, shell=True)
    message = ''
    if code != 0:
        message = '\033[91m[ERROR] returned non-zero exit code while running {}\033[0m'.format(cmd)
    else:
        message = '\033[92m[SUCCESS] {} completed successfully \033[0m'.format(cmd)

    # print message
    print message

    file_message = message[5:-5]  # strip colors off message for file
    q.put(message) if parallel else q.write(message + "\n")
    return None


def listener(q, fname):
    """Listen for messages on q then writes to file."""
    with open(fname, 'w') as f:
        while 1:
            m = q.get()
            if m == 'kill':
                f.write('killed')
                break
            f.write(str(m) + '\n')
            f.flush()


def build_processes(processes, callstring, names, signal_type, exe, output_dir, doSyst):
    """Create output directories and callstrings then add them to the list of processes."""
    for name in names:
        for isyst in getSyst(name, signal_type, exe, doSyst):
            if isyst == "" and not path.exists('Output/trees/{}/NOMINAL'.format(output_dir)):
                makedirs('Output/trees/{}/NOMINAL'.format(output_dir))
            if isyst != "" and not path.exists('Output/trees/{}/SYST_{}'.format(output_dir, isyst)):
                makedirs('Output/trees/{}/SYST_{}'.format(output_dir, isyst))

            tocall = callstring + ' -n {}'.format(name)
            if isyst != "":
                tocall += ' -u {}'.format(isyst)

            processes.append(tocall)
    return processes


def run_parallel(output_dir, processes):
    """Run analyzer using multiprocessing."""
    manager = multiprocessing.Manager()
    q = manager.Queue()

    # Use 12 cores if the machine has more than 24 total cores.
    # Otherwise, use half the available cores.
    n_processes = min(12, multiprocessing.cpu_count() / 2)
    print 'Process with {} cores'.format(n_processes)
    pool = multiprocessing.Pool(processes=n_processes)
    watcher = pool.apply_async(listener, (q, 'Output/trees/{}/logs/runninglog.txt'.format(output_dir)))

    jobs = []
    for command in processes:
        job = pool.apply_async(run_command, (command, q, True))
        jobs.append(job)

    for job in jobs:
        job.get()

    q.put('kill')
    pool.close()
    pool.join()


def run_series(output_dir, processes):
    """Run analyzer on processes in series."""
    with open('Output/trees/{}/logs/runninglog.txt'.format(output_dir), 'w') as ifile:
        [run_command(command, ifile, False) for command in processes]


def main(args):
    """Build all processes and run them."""
    start = time.time()
    suffix = '.root'
    fileList = [ifile for ifile in check_output(['xrdfs', 'root://cmseos.fnal.gov/', 'ls', args.path]).split('\n') if '.root' in ifile and valid_sample(ifile)]

    if args.condor:
        job_map = {}
        for ifile in fileList:
            sample = ifile.split('/')[-1].split(suffix)[0]
            tosample = ifile.replace(sample+suffix, '')
            names, signal_type = getNames(sample)
            file_map = defaultdict(list)
            for name in names:
                systs = getSyst(name, signal_type, args.exe, args.syst)
                for syst in systs:
                    if syst == '':
                      syst = 'NOMINAL'
                    else:
                      syst = 'SYST_' + syst

                    command = '{} -p {} -s {} -d ./ --stype {} -n {} -u {} --condor'.format(
                            args.exe, tosample, sample, signal_type,
                            name, syst.replace('SYST_', ''))

                    file_map[syst].append({
                        'path': tosample,
                        'sample': sample,
                        'name': name,
                        'command': command,
                        'signal_type': signal_type,
                        'syst': syst,
                    })

            job_map[sample] = file_map

        to_submit = []
        for sample, systs in job_map.iteritems():
            for syst, configs in systs.iteritems():
                for config in configs:
                    to_submit.append(config)
        submit_command(args.output_dir, to_submit, False)
    else:
        try:
            makedirs('Output/trees/{}/logs'.format(args.output_dir))
        except:
            pass

        processes = []
        for ifile in fileList:
            sample = ifile.split('/')[-1].split(suffix)[0]
            tosample = 'root://cmsxrootd.fnal.gov/' + ifile.replace(sample+suffix, '')

            names, signal_type = getNames(sample)
            # if signal_type != "None": continue
            callstring = '{} -p {} -s {} -d {} --stype {} '.format(args.exe,
                                                                     tosample, sample, args.output_dir, signal_type)

            doSyst = True if args.syst and not 'data' in sample.lower() else False
            processes = build_processes(processes, callstring, names, signal_type, args.exe, args.output_dir, doSyst)
        pprint(processes, width=150)

        if args.dont_process:
            return

        if args.parallel:
            run_parallel(args.output_dir, processes)
        else:
            run_series(args.output_dir, processes)

        end = time.time()
        print 'Processing completed in', end-start, 'seconds.'


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--exe', '-e', required=True, help='name of executable')
    parser.add_argument('--syst', action='store_true', help='run systematics as well')
    parser.add_argument('--path', '-p', required=True, help='path to input file directory')
    parser.add_argument('--parallel', action='store_true', help='run in parallel')
    parser.add_argument('--output-dir', required=True, dest='output_dir',
                        help='name of output directory after Output/trees')
    parser.add_argument('--dont-process', action='store_true', help='print commands without executing')
    parser.add_argument('--condor', action='store_true', help='submit jobs to condor')
    main(parser.parse_args())
