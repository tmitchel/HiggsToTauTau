#!/usr/bin/env python

from ROOT import *
from glob import glob

fout = TFile('QCD_output.root', 'recreate')
filelist = [ifile for ifile in glob('output/*')]

fdata = TFile('output/data.root', 'read')
fout.cd()

hdata_taupt_SS = fdata.Get('h0_tau_pt_SS').Clone()
hdata_taupt_QCD = fdata.Get('h0_tau_pt_QCD').Clone()
hdata_msv_SS = fdata.Get('h0_msv_SS').Clone()
hdata_msv_QCD = fdata.Get('h0_msv_QCD').Clone()

hbkg_taupt_SS = hdata_taupt_SS.Clone()
hbkg_taupt_QCD = hdata_taupt_QCD.Clone()
hbkg_msv_SS = hdata_msv_SS.Clone()
hbkg_msv_QCD = hdata_msv_QCD.Clone()
hbkg_taupt_SS.Reset()
hbkg_taupt_QCD.Reset()
hbkg_msv_SS.Reset()
hbkg_msv_QCD.Reset()

hdata_0jet_QCD = fdata.Get('h0_QCD').Clone()
hdata_1jet_QCD = fdata.Get('h1_QCD').Clone()
hdata_2jet_QCD = fdata.Get('h2_QCD').Clone()
hdata_0jet_SS = fdata.Get('h0_QCD').Clone()
hdata_1jet_SS = fdata.Get('h1_QCD').Clone()
hdata_2jet_SS = fdata.Get('h2_QCD').Clone()

hbkg_0jet_QCD = hdata_0jet_QCD.Clone()
hbkg_1jet_QCD = hdata_1jet_QCD.Clone()
hbkg_2jet_QCD = hdata_2jet_QCD.Clone()
hbkg_0jet_QCD.Reset()
hbkg_1jet_QCD.Reset()
hbkg_2jet_QCD.Reset()

hbkg_0jet_SS = hdata_0jet_SS.Clone()
hbkg_1jet_SS = hdata_1jet_SS.Clone()
hbkg_2jet_SS = hdata_2jet_SS.Clone()
hbkg_0jet_SS.Reset()
hbkg_1jet_SS.Reset()
hbkg_2jet_SS.Reset()

for ifile in filelist:
    if 'data' in ifile or 'SMH' in ifile or 'ZH' in ifile or 'vbf' in ifile or 'gg' in ifile:
        continue
    ftemp = TFile(ifile, 'read')
    print ftemp.GetName()
    fout.cd()

    hbkg_taupt_SS.Add(ftemp.Get('h0_tau_pt_SS'))
    hbkg_taupt_QCD.Add(ftemp.Get('h0_tau_pt_QCD'))
    hbkg_msv_SS.Add(ftemp.Get('h0_msv_SS'))
    hbkg_msv_QCD.Add(ftemp.Get('h0_msv_QCD'))

    htemp_bkg_QCD = ftemp.Get('h0_QCD').Clone()
    hbkg_0jet_QCD.Add(htemp_bkg_QCD)
    htemp_bkg_QCD = ftemp.Get('h1_QCD').Clone()
    hbkg_1jet_QCD.Add(htemp_bkg_QCD)
    htemp_bkg_QCD = ftemp.Get('h2_QCD').Clone()
    hbkg_2jet_QCD.Add(htemp_bkg_QCD)

    htemp_SS = ftemp.Get('h0_SS').Clone()
    hbkg_0jet_SS.Add(htemp_SS)
    htemp_SS = ftemp.Get('h1_SS').Clone()
    hbkg_1jet_SS.Add(htemp_SS)
    htemp_SS = ftemp.Get('h2_SS').Clone()
    hbkg_2jet_SS.Add(htemp_SS)

fout.cd()
hdata_0jet_QCD.Add(hbkg_0jet_QCD, -1)
hdata_1jet_QCD.Add(hbkg_1jet_QCD, -1)
hdata_2jet_QCD.Add(hbkg_2jet_QCD, -1)
hdata_0jet_SS.Add(hbkg_0jet_SS, -1)
hdata_1jet_SS.Add(hbkg_1jet_SS, -1)
hdata_2jet_SS.Add(hbkg_2jet_SS, -1)
hdata_taupt_SS.Add(hbkg_taupt_SS, -1)
hdata_taupt_QCD.Add(hbkg_taupt_QCD, -1)
hdata_msv_SS.Add(hbkg_msv_SS, -1)
hdata_msv_QCD.Add(hbkg_msv_QCD, -1)

hdata_0jet_QCD.Scale(hdata_0jet_SS.Integral()/hdata_0jet_QCD.Integral())
hdata_1jet_QCD.Scale(hdata_1jet_SS.Integral()/hdata_1jet_QCD.Integral())
hdata_2jet_QCD.Scale(hdata_2jet_SS.Integral()/hdata_2jet_QCD.Integral())
hdata_taupt_QCD.Scale(hdata_taupt_SS.Integral()/hdata_taupt_QCD.Integral())
hdata_msv_QCD.Scale(hdata_msv_SS.Integral()/hdata_msv_QCD.Integral())

hdata_taupt_QCD.SetName('htau_pt')
hdata_msv_QCD.SetName('msv')

hdata_0jet_QCD.Write()
hdata_1jet_QCD.Write()
hdata_2jet_QCD.Write()
hdata_taupt_QCD.Write()
hdata_msv_QCD.Write()
