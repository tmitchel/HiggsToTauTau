#!/usr/bin/env python

############################################################
## Script to automate running an analyzer on all files in ##
## a directory.                                           ##
############################################################

from os import popen, makedirs
from subprocess import call
from optparse import OptionParser
import time
from glob import glob

parser = OptionParser()
parser.add_option('--data', '-d', action='store_true',
                  default=False, dest='isData',
                  help='run on data or MC'
                  )
parser.add_option('--acSample', '-a', action='store_true',
                  default=False, dest='ACsample',
                  help='is an anomolous coupling sample'
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
        names = ['ZL', 'ZJ']
    elif 'TT' in sample:
        names = ['TTT', 'TTJ', 'TTL']
    elif 'WJets' in sample or 'EWKW' in sample:
        names = ['W']
    elif 'data' in sample.lower():
        names = ['data_obs']
    elif 'ggHtoTauTau' in sample:
        mass = sample.split('ggHtoTauTau')[-1]
        names = ['ggH'+mass]
    elif 'ggH125' in sample:
        mass = sample.split('ggH')[-1]
        names = ['ggH'+mass]
    elif 'VBFHtoTauTau' in sample:
        mass = sample.split('VBFHtoTauTau')[-1]
        names = ['VBF'+mass]
    elif 'VBF' in sample:
        mass = sample.split('VBF')[-1]
        names = ['VBF'+mass]
    elif 'WPlus' in sample or 'WMinus' in sample:
        mass = sample.split('HTauTau')[-1]
        names = ['WH'+mass]
    elif 'ZHTauTau' in sample:
        mass = sample.split('ZHTauTau')[-1]
        names = ['ZH'+mass]
    elif 'ZH' in sample:
        mass = sample.split('ZH')[-1]
        names = ['ZH'+mass]
    elif 'ttH' in sample:
        mass = sample.split('ttH')[-1]
        names = ['ttH'+mass]
    elif 'embed' in sample:
        names = ['embed']
    elif 'vbf' in sample:
        names = ['VBF125']
        options.ACsample = True
    elif 'ggH' in sample or 'ggh' in sample:
        names = ['ggH125']
        if not 'madgraph' in sample:
            options.ACsample = True
    elif 'wh' in sample:
        names = ['WH125']
        options.ACsample = True
    elif 'zh' in sample:
        names = ['ZH125']
        options.ACsample = True
    else: 
        names = ['VVJ', 'VVT', 'VVL']

    callstring = './%s -p %s -s %s -d %s ' % (options.exe, tosample, sample, options.output_dir)
    if options.ACsample:
      callstring += ' -a '

    if get_systs != None and not 'Data' in sample.lower():
        for name in names:
            for isyst in get_systs(name):
                tocall = callstring + ' -n %s -u %s' % (name, isyst)
                call(tocall, shell=True)
    else:
        for name in names:
            tocall = callstring + ' -n %s ' % name 
            call(tocall, shell=True)


    print tocall

end = time.time()
print 'Processing completed in', end-start, 'seconds.'
