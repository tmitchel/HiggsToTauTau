#!/usr/bin/env python

################################################################
## This script is used to create the QCD shape. We use a      ##
## loosened signal region selection to create a QCD region.   ##
## This region allows us to get a smoother shape than the low ##
## statistics signal region. We create an opposite-sign       ##
## sub-region and a same-sign sub-region in our QCD region.   ##
## The ratio of SS/OS is used to normalize the OS region. The ##
## shape is formed by subtracting all other backgrounds from  ##
## data, leaving the contribution from QCD.                   ##
################################################################

from ROOT import TFile

fout = TFile('output/QCD_output.root', 'recreate')
fdata = TFile('output/Data.root', 'read')
fout.cd()

nom = 'grabbag/'
qcd = 'et_antiiso_'

bkg_files = ['ZL', 'ZTT', 'ZJ', 'EWKZ', 'W', 'VV', 'TTT', 'TTJ']
systs = [
  '', '_CMS_scale_metphi_unclustered_13TeVUp', '_CMS_scale_metphi_unclustered_13TeVDown', 
  '_CMS_scale_metphi_clustered_13TeVUp', '_CMS_scale_metphi_clustered_13TeVDown', '_CMS_scale_met_unclustered_13TeVUp', 
  '_CMS_scale_met_unclustered_13TeVDown', '_CMS_scale_met_clustered_13TeVUp', '_CMS_scale_met_clustered_13TeVDown'
]

def createQCD(suffix):
  hists_2d = [
    ('0jet'   , fdata.Get(qcd+'0jet_crSS/data_obs'+suffix)   .Clone(), fdata.Get(qcd+'0jet_cr/data_obs'+suffix)   .Clone()),
    ('boosted', fdata.Get(qcd+'boosted_crSS/data_obs'+suffix).Clone(), fdata.Get(qcd+'boosted_cr/data_obs'+suffix).Clone()),
    # ('ZH'     , fdata.Get(qcd+'ZH_crSS/data_obs'+suffix)     .Clone(), fdata.Get(qcd+'ZH_cr/data_obs'+suffix)     .Clone()),
    ('vbf'    , fdata.Get(qcd+'vbf_crSS/data_obs'+suffix)    .Clone(), fdata.Get(qcd+'vbf_cr/data_obs'+suffix)    .Clone()),
  ]

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

  for ihist in hists_2d:
    for ifile in bkg_files:
      fbkg = TFile('output/'+ifile+'.root', 'READ')
      fout.cd()
      
      name = qcd+ihist[0]
      dir_SS = fbkg.Get(name+'_crSS')
      dir_QCD = fbkg.Get(name+'_cr')
      ihist[1].Add(dir_SS.Get(ifile+suffix), -1)
      ihist[2].Add(dir_QCD.Get(ifile+suffix), -1)

    idir = fout.GetDirectory('et_'+ihist[0])
    if (idir == None):
      idir = fout.mkdir('et_'+ihist[0])
    idir.cd()
    if ihist[2].Integral() > 0:
      ihist[2].Scale(ihist[1].Integral()/ihist[2].Integral())
    else:
      ihist[2].Scale(0)
    ihist[2].SetName('QCD'+syst)
    ihist[2].Write()

  for ihist in hists_1d:
    for ifile in bkg_files:
      fbkg = TFile('output/'+ifile+'.root', 'READ')
      fout.cd()
      
      dir = fbkg.Get('grabbag')
      ihist[1].Add(dir.Get(ihist[0]+'_SS'), -1)
      ihist[2].Add(dir.Get(ihist[0]+'_QCD'), -1)
      idir = fout.GetDirectory('grabbag')
    if (idir == None):
      idir = fout.mkdir('grabbag')
    idir.cd()
    if ihist[2].Integral() > 0:
      ihist[2].Scale(ihist[1].Integral()/ihist[2].Integral())
    else:
      ihist[2].Scale(0)
    ihist[2].SetName(ihist[0]+syst)
    ihist[2].Write()

for syst in systs:
  createQCD(suffix=syst)

fout.Close()
