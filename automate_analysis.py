#!/usr/bin/env python

from os import popen
from subprocess import call
from optparse import OptionParser

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

fileList = [ifile for ifile in filter(None, popen('xrdfsls root://cmseos.fnal.gov/ ls '+path).read().split('\n'))]
for ifile in fileList:
    print ifile
    call('./'+options.exe+' '+ifile, shell=True)
