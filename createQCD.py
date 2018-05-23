#!/usr/bin/env python

from ROOT import *

fout = TFile('QCD_output.root', 'recreate')
fdata = TFile('output/data.root', 'read')
fout.cd()

bkg_files = ['ZL', 'ZTT', 'W_unscaled', 'VV', 'ttbar']
hists = [
        ('tau_pt' , fdata.Get('tau_pt_SS') .Clone(), fdata.Get('tau_pt_QCD') .Clone()),
        ('el_pt'  , fdata.Get('el_pt_SS')  .Clone(), fdata.Get('el_pt_QCD')  .Clone()),
        ('tau_phi', fdata.Get('tau_phi_SS').Clone(), fdata.Get('tau_phi_QCD').Clone()),
        ('el_phi' , fdata.Get('el_phi_SS') .Clone(), fdata.Get('el_phi_QCD') .Clone()),
        ('msv'    , fdata.Get('msv_SS')    .Clone(), fdata.Get('msv_QCD')    .Clone()),
        ('met'    , fdata.Get('met_SS')    .Clone(), fdata.Get('met_QCD')    .Clone()),
        ('mjj'    , fdata.Get('mjj_SS')    .Clone(), fdata.Get('mjj_QCD')    .Clone()),
        ('mt'     , fdata.Get('mt_SS')     .Clone(), fdata.Get('mt_QCD')     .Clone()),
        ('h0_OS'  , fdata.Get('h0_SS')     .Clone(), fdata.Get('h0_QCD')     .Clone()),
        ('h1_OS'  , fdata.Get('h1_SS')     .Clone(), fdata.Get('h1_QCD')     .Clone()),
        ('h2_OS'  , fdata.Get('h2_SS')     .Clone(), fdata.Get('h2_QCD')     .Clone()),
        ('hvbf_OS', fdata.Get('hvbf_SS')   .Clone(), fdata.Get('hvbf_QCD')   .Clone()),
        ]

for ifile in bkg_files:
    fbkg = TFile('output/'+ifile+'.root', 'READ')
    for ihist in hists:
        fout.cd()
        hname = ihist[0]
        if '_OS' in hname:
            hname = hname.strip('_OS')
        bkg_SS = fbkg.Get(hname+'_SS').Clone()
        bkg_QCD = fbkg.Get(hname+'_QCD').Clone()

        ihist[1].Add(bkg_SS, -1)
        ihist[2].Add(bkg_QCD, -1)

for ihist in hists:
    ihist[2].Scale(ihist[1].Integral()/ihist[2].Integral())
    ihist[2].SetName(ihist[0])
    ihist[2].Write()

fout.Close()
