#!/usr/bin/env python

from ROOT import *

fout = TFile('output/QCD_output.root', 'recreate')
fdata = TFile('output/data.root', 'read')
fout.cd()

nom = 'grabbag/'
qcd = 'et_antiiso_'

bkg_files = ['ZL', 'ZTT', 'W_unscaled', 'VV', 'ttbar']
hists_1d = [
        ('tau_pt' , fdata.Get(nom+'tau_pt_SS') .Clone(), fdata.Get(nom+'tau_pt_QCD') .Clone()),
        ('el_pt'  , fdata.Get(nom+'el_pt_SS')  .Clone(), fdata.Get(nom+'el_pt_QCD')  .Clone()),
        ('tau_phi', fdata.Get(nom+'tau_phi_SS').Clone(), fdata.Get(nom+'tau_phi_QCD').Clone()),
        ('el_phi' , fdata.Get(nom+'el_phi_SS') .Clone(), fdata.Get(nom+'el_phi_QCD') .Clone()),
        ('msv'    , fdata.Get(nom+'msv_SS')    .Clone(), fdata.Get(nom+'msv_QCD')    .Clone()),
        ('met'    , fdata.Get(nom+'met_SS')    .Clone(), fdata.Get(nom+'met_QCD')    .Clone()),
        ('mjj'    , fdata.Get(nom+'mjj_SS')    .Clone(), fdata.Get(nom+'mjj_QCD')    .Clone()),
        ('mt'     , fdata.Get(nom+'mt_SS')     .Clone(), fdata.Get(nom+'mt_QCD')     .Clone()),
        ]
hists_2d = [
        ('0jet'   , fdata.Get(qcd+'0jet_crSS/Data')   .Clone(), fdata.Get(qcd+'0jet_cr/Data')   .Clone()),
        ('boosted', fdata.Get(qcd+'boosted_crSS/Data').Clone(), fdata.Get(qcd+'boosted_cr/Data').Clone()),
        ('ZH'     , fdata.Get(qcd+'ZH_crSS/Data')     .Clone(), fdata.Get(qcd+'ZH_cr/Data')     .Clone()),
        ('vbf'    , fdata.Get(qcd+'vbf_crSS/Data')    .Clone(), fdata.Get(qcd+'vbf_cr/Data')    .Clone()),
        ]

for ifile in bkg_files:
    fbkg = TFile('output/'+ifile+'.root', 'READ')
    fout.cd()
    for ihist in hists_1d:
        hname = nom+ihist[0]
        ihist[1].Add(fbkg.Get(hname+'_SS'), -1)
        ihist[2].Add(fbkg.Get(hname+'_QCD'), -1)

    for ihist in hists_2d:
      name = qcd+ihist[0]

      dir_SS = fbkg.Get(name+'_crSS')
      for hist in dir_SS.GetListOfKeys():
        ihist[1].Add(hist.ReadObj(), -1)

      dir_QCD = fbkg.Get(name+'_cr')
      for hist in dir_QCD.GetListOfKeys():
        ihist[2].Add(hist.ReadObj(), -1)

fout.mkdir('grabbag')
fout.cd('grabbag')
for ihist in hists_1d:
  ihist[2].Scale(ihist[1].Integral()/ihist[2].Integral())
  ihist[2].SetName(ihist[0])
  ihist[2].Write()

for ihist in hists_2d:
    idir = fout.mkdir('et_'+ihist[0])
    idir.cd()
    ihist[2].Scale(ihist[1].Integral()/ihist[2].Integral())
    ihist[2].SetName('qcd')
    ihist[2].Write()

fout.Close()
