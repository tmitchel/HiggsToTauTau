#!/usr/bin/env python

from ROOT import TFile

fout = TFile('output/QCD_output.root', 'recreate')
fdata = TFile('output/mela_svfit_full/Data.root', 'read')
fout.cd()

nom = 'grabbag/'
qcd = 'et_antiiso_'

bkg_files = ['ZL', 'ZTT', 'ZJ', 'EWKZ', 'W_unscaled', 'VV', 'TTT', 'TTJ']
systs = ['', '_CMS_scale_metphi_unclustered_13TeVUp', '_CMS_scale_metphi_unclustered_13TeVDown', 
        '_CMS_scale_metphi_clustered_13TeVUp', '_CMS_scale_metphi_clustered_13TeVDown', '_CMS_scale_met_unclustered_13TeVUp', 
        '_CMS_scale_met_unclustered_13TeVDown', '_CMS_scale_met_clustered_13TeVUp', '_CMS_scale_met_clustered_13TeVDown'
        ]

def createQCD(suffix):
  hists_2d = [
        ('0jet'   , fdata.Get(qcd+'0jet_crSS/data_obs'+suffix)   .Clone(), fdata.Get(qcd+'0jet_cr/data_obs'+suffix)   .Clone()),
        ('boosted', fdata.Get(qcd+'boosted_crSS/data_obs'+suffix).Clone(), fdata.Get(qcd+'boosted_cr/data_obs'+suffix).Clone()),
        ('ZH'     , fdata.Get(qcd+'ZH_crSS/data_obs'+suffix)     .Clone(), fdata.Get(qcd+'ZH_cr/data_obs'+suffix)     .Clone()),
        ('vbf'    , fdata.Get(qcd+'vbf_crSS/data_obs'+suffix)    .Clone(), fdata.Get(qcd+'vbf_cr/data_obs'+suffix)    .Clone()),
        ]

  for ihist in hists_2d:
    for ifile in bkg_files:
      fbkg = TFile('output/mela_svfit_full/'+ifile+'.root', 'READ')
      fout.cd()

      name = qcd+ihist[0]
      dir_SS = fbkg.Get(name+'_crSS')
      dir_QCD = fbkg.Get(name+'_cr')
      ihist[1].Add(dir_SS.Get(ifile), -1)
      ihist[2].Add(dir_QCD.Get(ifile), -1)

    idir = fout.mkdir('et_'+ihist[0])
    idir.cd()
    if ihist[2].Integral() > 0:
      ihist[2].Scale(ihist[1].Integral()/ihist[2].Integral())
    else:
      ihist[2].Scale(0)
    ihist[2].SetName('QCD')
    ihist[2].Write()

for syst in systs:
  createQCD(suffix=syst)

fout.Close()
