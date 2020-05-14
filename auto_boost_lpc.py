from pprint import pprint
from os import popen, makedirs, path
from subprocess import call, check_output
import time
from glob import glob
import multiprocessing

def valid_sample(ifile):
    """valid_sample can be used to filter out unwanted files"""
    return True


def build_processes(processes, callstring, names, signal_type, exe, output_dir, doSyst):
    """Create output directories and callstrings then add them to the list of processes."""
    for name in names:
        if not path.exists('Output/trees/{}/NOMINAL'.format(output_dir)):
            makedirs('Output/trees/{}/NOMINAL'.format(output_dir))

        tocall = callstring + ' -n {}'.format(name)

        processes.append(tocall)
    return processes


def getNames(sample):
    """Return the sample names and signal type."""
    if 'DYJets' in sample:
        names = ['ZL', 'ZJ', 'ZTT']
    elif 'TT' in sample:
        names = ['TTT', 'TTJ', 'TTL']
    elif 'ST_tW' in sample or 't-channel' in sample:
        names = ['STT', 'STL', 'STJ']
    elif 'WJets' in sample:
        names = ['W']
    elif 'EWKW' in sample:
        names = ['EWK_W']
    elif 'data' in sample.lower():
        names = ['data_obs']
    elif 'ggh125' in sample.lower() or 'JJH0' in sample:
        names = ['ggH125']
    elif 'vbf125' in sample.lower() or 'qqh125' in sample.lower():
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


def run_series(output_dir, processes):
    """Run analyzer on processes in series."""
    with open('Output/trees/{}/logs/runninglog.txt'.format(output_dir), 'w') as ifile:
        [run_command(command, ifile, False) for command in processes]


def run_parallel(output_dir, processes):
    """Run analyzer using multiprocessing."""
    manager = multiprocessing.Manager()
    q = manager.Queue()

    # Use 10 cores if the machine has more than 20 total cores.
    # Otherwise, use half the available cores.
    n_processes = min(10, multiprocessing.cpu_count() / 2)
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


def main(args):
    suffix = '.root'
    fileList = [ifile for ifile in check_output(['xrdfs', 'root://cmseos.fnal.gov/', 'ls', args.path]).split('\n') if '.root' in ifile and valid_sample(ifile)]

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

    if args.parallel:
        run_parallel(args.output_dir, processes)
    else:
        run_series(args.output_dir, processes)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--exe', '-e', required=True, help='name of executable')
    parser.add_argument('--syst', action='store_true', help='run systematics as well')
    parser.add_argument('--path', '-p', required=True, help='path to input file directory')
    parser.add_argument('--parallel', action='store_true', help='run in parallel')
    parser.add_argument('--output-dir', required=True, dest='output_dir',
                        help='name of output directory after Output/trees')
    main(parser.parse_args())
