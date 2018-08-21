#!/usr/bin/env python

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
parser.add_option('--local', '-l', action='store_true',
                  default=False, dest='local',
                  help='running locally or not'
                  )
parser.add_option('--svfit', '-s', action='store_true',
                  default=False, dest='svfit',
                  help='are these svfitted files?'
                  )
(options, args) = parser.parse_args()

if options.isData:
    path = "/store/user/tmitchel/smhet_22feb_SV/"
else:
    path = "/store/user/tmitchel/smhet_20march/"

start = time.time()
if options.local:
    if options.isData:
    	fileList = [ifile for ifile in glob('root_files/data_svFitted/*') if '.root' in ifile and 'Data' in ifile]
    else:
	    fileList = [ifile for ifile in glob('root_files/svFitted/*') if '.root' in ifile and not 'Data' in ifile]
    suffix = ' -l'
else:
    fileList = [ifile for ifile in filter(None, popen('xrdfs root://cmseos.fnal.gov/ ls '+path).read().split('\n'))]
    suffix = ''

for ifile in fileList:
    if not 'root' in ifile:
        continue
        
    postfix = '.root'
    if options.svfit:
        postfix = '_svFit.root'
    sample = ifile.split('/')[-1].split(postfix)[0]
    tosample = ifile.rstrip(sample+postfix)

    if 'DYJets' in ifile:
        names = ['ZTT', 'ZL', 'ZJ']
    elif 'TT' in ifile:
        names = ['TTT', 'TTJ']
    elif 'Wjets' in ifile or 'EWKW' in ifile:
        names = ['W']
    elif 'EWKZ' in ifile:
        names = ['EWKZ']
    elif 'Data' in ifile:
        names = ['Data']
    elif 'ggHtoTauTau' in ifile:
        mass = sample.split('ggHtoTauTau')[-1]
        names = ['ggH'+mass]
    elif 'VBFHtoTauTau' in ifile:
        mass = sample.split('VBFHtoTauTau')[-1]
        names = ['VBF'+mass]
    else: 
        names = ['VV']

    callstring = './%s -p %s -s %s' % (options.exe, tosample, sample)
    if options.svfit:
        callstring += ' -S'

    for name in names:
        tocall = callstring + ' -n %s' % name 
        call(tocall, shell=True)

end = time.time()
print 'Processing completed in', end-start, 'seconds.'
