#!/usr/bin/env python

from os import popen
from subprocess import call
from optparse import OptionParser
import time

parser = OptionParser()
parser.add_option('--data', '-d', action='store_true',
                  default=False, dest='isData',
                  help='run on data or MC'
                  )
parser.add_option('--exe', '-e', action='store',
                  default='analyzer', dest='exe',
                  help='name of executable'
                  )
(options, args) = parser.parse_args()

if options.isData:
    path = "/store/user/tmitchel/smhet_22feb_SV/"
else:
    path = "/store/user/tmitchel/smhet_20march/"

start = time.time()
fileList = [ifile for ifile in filter(None, popen('xrdfs root://cmseos.fnal.gov/ ls '+path).read().split('\n'))]
for ifile in fileList:
    if not 'root' in ifile:
        continue
    if 'DY' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' ZTT', shell=True)
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' ZL', shell=True)
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' ZJ', shell=True)
    elif 'TT' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' TTT', shell=True)
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' TTJ', shell=True)
    elif 'W_' in ifile or 'W1' in ifile or 'W2' in ifile or 'W3' in ifile or 'W4' in ifile:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0]+' W', shell=True)        
    else:
        call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0], shell=True)
end = time.time()
print 'Processing completed in', end-start, 'seconds.'
