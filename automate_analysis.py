#!/usr/bin/env python

############################################################
## Script to automate running an analyzer on all files in ##
## a directory.                                           ##
############################################################

from os import popen, makedirs, path
from pprint import pprint
from subprocess import call
from optparse import OptionParser
import time
import multiprocessing
from glob import glob

parser = OptionParser()
parser.add_option('--exe', '-e', action='store',
                  default='Analyze', dest='exe',
                  help='name of executable'
                  )
parser.add_option('--syst', action='store',
                  default=None, dest='syst',
                  help='run systematics as well'
                  )
parser.add_option('--path', '-p', action='store',
                  default='root_files/', dest='path',
                  help='path to input file directory'
                  )
parser.add_option('--parallel', action='store_true',
                  help='run in parallel'
                  )
parser.add_option('--output-dir', action='store',
                  default='', dest='output_dir',
                  help='name of output directory after Output/trees'
                  )
(options, args) = parser.parse_args()
suffix = '.root'

try:
    makedirs('Output/trees/{}/logs'.format(options.output_dir))
except:
    pass

start = time.time()
fileList = [ifile for ifile in glob(options.path+'/*') if '.root' in ifile]


def getSyst2016(name, exe):
    systs = ['']

    if name == 'TTT' or name == 'VTT' or name == 'embed' or name == 'ZTT':
        systs += ['tau_id_Up', 'tau_id_Down'] # names will probably be updated
        systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']

    if name == 'ZL' or name == 'W':
        if '_et' in args.exe:
            systs += ['efaket_Up', 'efaket_Down']
        elif '_mt' in args.exe:
            systs += ['mfaket_Up', 'mfaket_Down']
            
        systs += ['ltau_FES_DM0_Up', 'ltau_FES_DM0_Down', 'ltau_FES_DM1_Up', 'ltau_FES_DM1_Down']

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

    if name != 'data_obs' and '_et' in args.exe:
        systs += ['EEScale_Up', 'EEScale_Down', 'EESigma_Up', 'EESigma_Down', 'el_combo_Up', 'el_combo_Down']
    elif name != 'data_obs' and '_mt' in args.exe:
        systs += ['MES_Up', 'MES_Down', 'mu_combo_Up', 'mu_combo_Down']

    return systs


def getSyst2017(name):
    systs = ['']

    if name != 'embed' and name != 'data_obs':
        systs += [
            'UncMet_Up', 'UncMet_Down', 'ClusteredMet_Up', 'ClusteredMet_Down',
            'JetTotalUp', 'JetTotalDown'
        ]

    if name == 'TTT' or name == 'TTJ':
        systs += ['ttbarShape_Up', 'ttbarShape_Down']

    if name == 'TTT' or name == 'VTT' or name == 'embed' or name == 'ZTT':
        #    systs += ['Up', 'Down', 'DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']
        systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']

    if name == 'TTJ' or name == 'ZJ' or name == 'VVJ' or name == 'W':
        systs += ['jetToTauFake_Up', 'jetToTauFake_Down']

    return systs


def run_process(proc):
    print proc
    return call(proc, shell=True)


get_systs = None
if options.syst == '2016':
    get_systs = getSyst2016
elif options.syst == '2017':
    get_systs = getSyst2017

processes = []

for ifile in fileList:
    sample = ifile.split('/')[-1].split(suffix)[0]
    tosample = ifile.replace(sample+suffix, '')

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

    callstring = './{} -p {} -s {} -d {} --stype {} '.format(options.exe,
                                                             tosample, sample, options.output_dir, signal_type)

    if get_systs != None and not 'Data' in sample.lower():
        for name in names:
            for isyst in get_systs(name, args.exe):
                if isyst == "" and not path.exists('Output/trees/{}/NOMINAL'.format(options.output_dir)):
                    makedirs('Output/trees/{}/NOMINAL'.format(options.output_dir))
                if isyst != "" and not path.exists('Output/trees/{}/SYST_{}'.format(options.output_dir, isyst)):
                    makedirs('Output/trees/{}/SYST_{}'.format(options.output_dir, isyst))

                tocall = callstring + ' -n %s -u %s' % (name, isyst)
                processes.append(tocall)
    else:
        for name in names:
            if not path.exists('Output/trees/{}/NOMINAL'.format(options.output_dir)):
                makedirs('Output/trees/{}/NOMINAL'.format(options.output_dir))
            tocall = callstring + ' -n %s ' % name
            processes.append(tocall)

pprint(processes)
if options.parallel:
    # Use 8 cores if the machine has more than 16 total cores.
    # Otherwise, use half the available cores.
    n_processes = min(8, multiprocessing.cpu_count() / 2)

    pool = multiprocessing.Pool(processes=n_processes)
    r = pool.map_async(run_process, processes)
    r.wait()
else:
    [run_process(proc) for proc in processes]

end = time.time()
print 'Processing completed in', end-start, 'seconds.'
