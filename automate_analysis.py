#!/usr/bin/env python

############################################################
## Script to automate running an analyzer on all files in ##
## a directory.                                           ##
############################################################

from os import popen, makedirs, path
from subprocess import call
from optparse import OptionParser
import time
from glob import glob

parser = OptionParser()
parser.add_option('--data', '-d', action='store_true',
                  default=False, dest='isData',
                  help='run on data or MC'
                  )
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
parser.add_option('--prefix', '-P', action='store',
                  default=None, dest='prefix',
                  help='prefix to strip'
                  )
parser.add_option('--output-dir', action='store',
                  default='', dest='output_dir', 
                  help='name of output directory after Output/trees'
                  )
(options, args) = parser.parse_args()
prefix = options.prefix
suffix = '.root'

try:
    makedirs('Output/trees/{}'.format(options.output_dir))
except:
    pass

start = time.time()
if options.isData:
    fileList = [ifile for ifile in glob(options.path+'/*') if '.root' in ifile and 'data' in ifile.lower()]
else:
    fileList = [ifile for ifile in glob(options.path+'/*') if '.root' in ifile]

def getSyst2016(name):
  systs = ['']
  
  if name != 'embed' and name != 'data_obs':
    systs += [
      'UncMet_Up', 'UncMet_Down', 'ClusteredMet_Up', 'ClusteredMet_Down',
      'JetTotalUp', 'JetTotalDown'
    ]

  if name == 'TTT' or name == 'TTJ':
    systs += ['ttbarShape_Up', 'ttbarShape_Down']

  if name == 'TTT' or name == 'VTT' or name == 'embed':
#    systs += ['Up', 'Down', 'DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']
    systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']

  if name == 'TTJ' or name == 'ZJ' or name == 'VVJ' or name == 'W':
    systs += ['jetToTauFake_Up', 'jetToTauFake_Down']

  if name == 'ZJ' or name == 'ZL':
    systs += ['dyShape_Up', 'dyShape_Down', 'zmumuShape_Up', 'zmumuShape_Down']

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

  if name == 'TTT' or name == 'VTT' or name == 'embed':
#    systs += ['Up', 'Down', 'DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']
    systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']

  if name == 'TTJ' or name == 'ZJ' or name == 'VVJ' or name == 'W':
    systs += ['jetToTauFake_Up', 'jetToTauFake_Down']

  return systs

get_systs = None
if options.syst == '2016':
  get_systs = getSyst2016
elif options.syst == '2017':
  get_systs = getSyst2017

for ifile in fileList:
    sample = ifile.split('/')[-1].split(suffix)[0]
    if prefix:
      sample = sample.replace(prefix, '')
    tosample = ifile.replace(sample+suffix,'')

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

    callstring = './{} -p {} -s {} -d {} --stype {} '.format(options.exe, tosample, sample, options.output_dir, signal_type)
    
    if get_systs != None and not 'Data' in sample.lower():
        for name in names:
            for isyst in get_systs(name):
                if isyst == "" and not path.exists('Output/trees/{}/NOMINAL'.format(options.output_dir)):
                    makedirs('Output/trees/{}/NOMINAL'.format(options.output_dir))
                if isyst != "" and not path.exists('Output/trees/{}/SYST_{}'.format(options.output_dir, isyst)):
                    makedirs('Output/trees/{}/SYST_{}'.format(options.output_dir, isyst))

                tocall = callstring + ' -n %s -u %s' % (name, isyst)
                print tocall
                call(tocall, shell=True)
    else:
        for name in names:
            tocall = callstring + ' -n %s ' % name
            print tocall
            call(tocall, shell=True)



end = time.time()
print 'Processing completed in', end-start, 'seconds.'
