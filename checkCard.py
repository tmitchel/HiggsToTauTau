#!/usr/bin/env python

import ROOT
from glob import glob

files = [ifile for ifile in glob('output/*')]
for ifile in files:
    fin = ROOT.TFile(ifile, 'READ')
    ihist = fin.Get('cutflow').Clone()
    print fin.GetName(), ihist.GetBinContent(9)

# fin = ROOT.TFile('htt_et.inputs-sm-13TeV-2D.root', 'READ')
# jet = fin.Get('et_0jet')
# boost = fin.Get('et_boosted')
# vbf = fin.Get('et_vbf')
#
# hists = ['W', 'data_obs', 'ZL', 'ZJ', 'ZTT', 'EWKZ', 'TTT', 'TTJ', 'QCD', 'VV', 'ggH_htt125', 'qqH_htt125']
#
# for ihist in hists:
#     sum = 0
#     sum += jet.Get(ihist).Integral()
#     sum += boost.Get(ihist).Integral()
#     sum += vbf.Get(ihist).Integral()
#
#     print ihist, sum
