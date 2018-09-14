#!/usr/bin/env python

############################################################
## Script to automate running an analyzer on all files in ##
## a directory.                                           ##
############################################################

from os import popen
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
parser.add_option('--syst', action='store_true',
                  default=False, dest='syst',
                  help='run systematics as well'
                  )
parser.add_option('--suffix', '-s', action='store',
                  default='.root', dest='suffix',
                  help='suffix to strip off root files'
                  )
parser.add_option('--path', '-p', action='store',
                  default='root_files/', dest='path',
                  help='path to input file directory'
                  )
parser.add_option('--prefix', '-P', action='store',
                  default=None, dest='prefix',
                  help='prefix to strip'
)
(options, args) = parser.parse_args()
suffix = options.suffix
prefix = options.prefix

start = time.time()
if options.isData:
    fileList = [ifile for ifile in glob(options.path+'/*') if '.root' in ifile and 'data' in ifile.lower()]
else:
    fileList = [ifile for ifile in glob(options.path+'/*') if '.root' in ifile and not 'data' in ifile.lower()]

systs = ['', 'met_UESUp', 'met_UESDown', 'met_JESUp', 'met_JESDown', 'metphi_UESUp', 'metphi_UESDown', 'metphi_JESUp', 'metphi_JESDown', 'mjj_JESUp', 'mjj_JESDown']

for ifile in fileList:
    sample = ifile.split('/')[-1].split(suffix)[0]
    if prefix:
      sample = sample.replace(prefix, '')
    tosample = ifile.replace(sample+suffix,'')
    print sample

    if 'DYJets' in ifile:
        names = ['ZTT', 'ZL', 'ZJ']
    elif 'TT' in ifile:
        names = ['TTT', 'TTJ']
    elif 'WJets' in ifile or 'EWKW' in ifile:
        names = ['W']
    elif 'EWKZ' in ifile:
        names = ['EWKZ']
    elif 'data' in ifile.lower():
        names = ['data_obs']
    elif 'ggHtoTauTau' in ifile:
        mass = sample.split('ggHtoTauTau')[-1]
        names = ['ggH'+mass]
    elif 'VBFHtoTauTau' in ifile:
        mass = sample.split('VBFHtoTauTau')[-1]
        names = ['VBF'+mass]
    elif 'WPlusH' in ifile or 'WMinusH' in ifile:
        mass = sample.split('HTauTau')[-1]
        names = ['WH'+mass]
    elif 'ZH' in ifile:
        mass = sample.split('ZHTauTau')[-1]
        names = ['ZH'+mass]
    else: 
        names = ['VV']

    callstring = './%s -p %s -s %s -P %s' % (options.exe, tosample, sample, suffix)

    if options.syst:
        for isyst in systs:
            for name in names:
                tocall = callstring + ' -n %s -u %s' % (name, isyst)
                call(tocall, shell=True)
    else:
        for name in names:
            tocall = callstring + ' -n %s' % name 
            call(tocall, shell=True)

    print tocall

end = time.time()
print 'Processing completed in', end-start, 'seconds.'
