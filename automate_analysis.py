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
(options, args) = parser.parse_args()

if options.isData:
    path = "/store/user/tmitchel/smhet_22feb_SV/"
else:
    path = "/store/user/tmitchel/smhet_20march/"

start = time.time()
if options.local:
    if options.isData:
    	fileList = [ifile for ifile in glob('root_files/newfiles_data/*') if '.root' in ifile and 'Data' in ifile]
    else:
	    fileList = [ifile for ifile in glob('root_files/newfiles/*') if '.root' in ifile and not 'Data' in ifile]
    suffix = ' -l'
else:
    fileList = [ifile for ifile in filter(None, popen('xrdfs root://cmseos.fnal.gov/ ls '+path).read().split('\n'))]
    suffix = ''
print options.local, options.isData
for ifile in fileList:
    if not 'root' in ifile:
        continue
    if 'DYJets' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' ZTT'+suffix, shell=True)
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' ZL'+suffix, shell=True)
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' ZJ'+suffix, shell=True)
    elif 'TT' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' TTT'+suffix, shell=True)
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' TTJ'+suffix, shell=True)
    elif 'WJets' in ifile or 'EWKW' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' W'+suffix, shell=True)
    elif 'EWKZ' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' EWKZ'+suffix, shell=True)
    elif 'HWW_gg' in ifile or 'ggHtoTauTau' in ifile:
        name = ifile.split('/')[-1].split('.root')[0]
        call('./'+options.exe+' '+name+' ggH_hww'+name.split('HWW_gg')[-1]+suffix, shell=True)
    elif 'HWW_vbf' in ifile or 'VBFHtoTauTau' in ifile:
        name = ifile.split('/')[-1].split('.root')[0]
        call('./'+options.exe+' '+name+' qqH_hww'+name.split('VBFHtoTauTau')[-1]+suffix, shell=True)
    elif 'Data' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' Data'+suffix, shell=True)
    else:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' VV'+suffix, shell=True)
end = time.time()
print 'Processing completed in', end-start, 'seconds.'
