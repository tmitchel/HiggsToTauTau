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
    call('./'+options.exe+' '+ifile.split('/')[-1].split('.root')[0], shell=True)
end = time.time()
print 'Processing completed in', end-start, 'seconds.'
