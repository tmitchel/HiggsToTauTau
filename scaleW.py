#!/usr/bin/env python

#############################################################
## Script to scale the WJets contributions. We have filled ##
## the histogram n70 with events from a high mt region.    ##
## WJets normalization is found by subtracting the number  ##
## of background events (excluding WJets) from the number  ##
## of data events in the high mt region. The ratio of this ##
## number to the number of WJets events in the high mt     ##
## region gives the normaliztion factor. The WJets         ##
## distributions are scaled by this factor.                ##
#############################################################

from ROOT import TFile

def getRatio(hname, fdata, fW_unscaled, bkg_list):
    data_file = TFile(fdata, 'read')
    numerator = data_file.Get(hname).Integral()
    data_file.Close()
    for bkg in bkg_list:
        ifile = TFile('output/'+bkg+'.root', 'read')
        numerator -= ifile.Get(hname).Integral()
        ifile.Close()

    return numerator / fW_unscaled.Get(hname).Integral()

def scaleAll(W_file, outname, ratio):
    directories = ['et_0jet', 'et_boosted', 'et_vbf']
    fout = TFile(outname, 'recreate')
    for directory in directories:
        fout.mkdir(directory)
        idir = W_file.Get(directory)
        for ikey in idir.GetListOfKeys():
            fout.cd(directory)
            ihist = idir.Get(ikey.GetName()).Clone()
            ihist.Scale(ratio)
            ihist.Write()
    fout.Close()

if __name__ == "__main__":

    W_file = TFile('output/W_unscaled.root', 'read')
    bkg_files = ['ZL', 'ZTT', 'ZJ', 'EWKZ', 'VV', 'TTT', 'TTJ']

    ratio = getRatio(hname='n70', fdata='output/Data.root',
                    fW_unscaled=W_file, bkg_list=bkg_files)

    scaleAll(W_file, 'output/Wscaled.root', ratio)





