#!/usr/bin/env python
import ROOT
from ROOT import *
import re
from array import array

import operator
import sys
filename_1 = sys.argv[1]
filename_out = sys.argv[2]
chn = sys.argv[3]


islog=1
unrollSV=1

file=ROOT.TFile(filename_1,"r")
file1=ROOT.TFile(filename_out,"recreate")

if chn=='tt':
    vbf_dir_names=[chn+"_vbf_D0_0to0p2",chn+"_vbf_D0_0p2to0p4",chn+"_vbf_D0_0p4to0p8",chn+"_vbf_D0_0p8to1"]
else:
    vbf_dir_names=[chn+"_vbf_D0_0p0to0p2",chn+"_vbf_D0_0p2to0p4",chn+"_vbf_D0_0p4to0p8",chn+"_vbf_D0_0p8to1p0"]    


file.cd()
dirList = gDirectory.GetListOfKeys()

name_SM_qqH_powheg="VBF125"
name_SM_qqH_JHU="reweighted_qqH_htt_0PM125"
name_PS_qqH_JHU="reweighted_qqH_htt_0M125"

name_SM_WH_powheg="WH125"
name_SM_WH_JHU="reweighted_WH_htt_0PM125"
name_PS_WH_JHU="reweighted_WH_htt_0M125"

name_SM_ZH_powheg="ZH125"
name_SM_ZH_JHU="reweighted_ZH_htt_0PM125"
name_PS_ZH_JHU="reweighted_ZH_htt_0M125"

name_SM_ggH_powheg="ggH125"
name_SM_ggH_JHU="reweighted_GGH2Jets_0PM125"


yield_qqH_Powheg=0
yield_qqH_JHU=0
yield_WH_Powheg=0
yield_WH_JHU=0
yield_ZH_Powheg=0
yield_ZH_JHU=0
yield_ggH_Powheg=0
yield_ggH_JHU=0

yield_qqH_Powheg_vbf=0
yield_qqH_JHU_vbf=0
yield_WH_Powheg_vbf=0
yield_WH_JHU_vbf=0
yield_ZH_Powheg_vbf=0
yield_ZH_JHU_vbf=0
yield_ggH_Powheg_vbf=0
yield_ggH_JHU_vbf=0

'''
# sum up yields in VBF category:
for k1 in vbf_dir_names:
    print "dir: ",k1
    #print " signal DCP_minus: ", k1.GetName()
    #h1 = k1.ReadObj()
    #nom=k1.GetName()

    #file.cd(vbf_dir_names)
    yield_qqH_Powheg=yield_qqH_Powheg+file.Get(k1).Get(name_SM_qqH_powheg).Integral()
    yield_ggH_Powheg=yield_ggH_Powheg+file.Get(k1).Get(name_SM_ggH_powheg).Integral()
    yield_WH_Powheg=yield_WH_Powheg+file.Get(k1).Get(name_SM_WH_powheg).Integral()
    yield_ZH_Powheg=yield_ZH_Powheg+file.Get(k1).Get(name_SM_ZH_powheg).Integral()
    yield_qqH_JHU=yield_qqH_JHU+file.Get(k1).Get(name_SM_qqH_JHU).Integral()
    yield_ggH_JHU=yield_ggH_JHU+file.Get(k1).Get(name_SM_ggH_JHU).Integral()
    yield_WH_JHU=yield_WH_JHU+file.Get(k1).Get(name_SM_WH_JHU).Integral()
    yield_ZH_JHU=yield_ZH_JHU+file.Get(k1).Get(name_SM_ZH_JHU).Integral()
   
print "summed ggH VBF yield: ",yield_ggH_Powheg
print "summed VBFH VBF yield: ",yield_qqH_Powheg
print " ==> scale ggH: ",yield_ggH_Powheg/yield_ggH_JHU
'''

'''
yield_qqH_Powheg_VBFcat=0
yield_qqH_JHU_VBFcat=0
yield_WH_Powheg_VBFcat=0
yield_WH_JHU_VBFcat=0
yield_ZH_Powheg_VBFcat=0
yield_ZH_JHU_VBFcat=0
yield_ggH_Powheg_VBFcat=0
yield_ggH_JHU_VBFcat=0

'''
# sum up yields in VBF category:
for k1 in vbf_dir_names:
    print "dir: ",k1
    #print " signal DCP_minus: ", k1.GetName()
    #h1 = k1.ReadObj()
    #nom=k1.GetName()

    #file.cd(vbf_dir_names)
    yield_qqH_Powheg_vbf=yield_qqH_Powheg_vbf+file.Get(k1).Get(name_SM_qqH_powheg).Integral()
    yield_ggH_Powheg_vbf=yield_ggH_Powheg_vbf+file.Get(k1).Get(name_SM_ggH_powheg).Integral()
    yield_WH_Powheg_vbf=yield_WH_Powheg_vbf+file.Get(k1).Get(name_SM_WH_powheg).Integral()
    yield_ZH_Powheg_vbf=yield_ZH_Powheg_vbf+file.Get(k1).Get(name_SM_ZH_powheg).Integral()
    yield_qqH_JHU_vbf=yield_qqH_JHU_vbf+file.Get(k1).Get(name_SM_qqH_JHU).Integral()
    yield_ggH_JHU_vbf=yield_ggH_JHU_vbf+file.Get(k1).Get(name_SM_ggH_JHU).Integral()
    yield_WH_JHU_vbf=yield_WH_JHU_vbf+file.Get(k1).Get(name_SM_WH_JHU).Integral()
    yield_ZH_JHU_vbf=yield_ZH_JHU_vbf+file.Get(k1).Get(name_SM_ZH_JHU).Integral()
   
print "summed ggH VBF yield: ",yield_ggH_Powheg_vbf
print "summed VBFH VBF yield: ",yield_qqH_Powheg_vbf
print " ==> scale ggH: ",yield_ggH_Powheg_vbf/yield_ggH_JHU_vbf

print "-> summed VBFH VBF yield: ",yield_qqH_Powheg_vbf

for k1 in dirList:
    print "\n signal DCP_minus: ", k1.GetName()
    h1 = k1.ReadObj()
    nom=k1.GetName()
    print "\t summed VBFH VBF yield: ",yield_qqH_Powheg_vbf


    nom_out=nom
   
    nom_out=nom_out.replace("D0ggH_0p00to0p30","ggHMELA_bin1")
    nom_out=nom_out.replace("D0ggH_0p30to0p45","ggHMELA_bin2")
    nom_out=nom_out.replace("D0ggH_0p45to0p55","ggHMELA_bin3")
    nom_out=nom_out.replace("D0ggH_0p55to1p00","ggHMELA_bin4")
    
    nom_out=nom_out.replace("D0ggH_0p0to0p2","ggHMELA_bin1")
    nom_out=nom_out.replace("D0ggH_0p2to0p4","ggHMELA_bin2")
    nom_out=nom_out.replace("D0ggH_0p4to0p7","ggHMELA_bin3")
    nom_out=nom_out.replace("D0ggH_0p7to1p0","ggHMELA_bin4")
 
    file1.mkdir(nom_out)
    
    h1.cd()
    histoList = gDirectory.GetListOfKeys()
    name_last=""
    
    
    
    h_SM_qqH_powheg_c=h1.Get(name_SM_qqH_powheg)
    h_SM_qqH_JHU_c=h1.Get(name_SM_qqH_JHU)
    h_SM_qqH_powheg=h_SM_qqH_powheg_c.Clone()
    h_SM_qqH_JHU=h_SM_qqH_JHU_c.Clone()

    h_SM_WH_powheg_c=h1.Get(name_SM_WH_powheg)
    h_SM_WH_JHU_c=h1.Get(name_SM_WH_JHU)
    h_SM_WH_powheg=h_SM_WH_powheg_c.Clone()
    h_SM_WH_JHU=h_SM_WH_JHU_c.Clone()
    
    h_SM_ZH_powheg_c=h1.Get(name_SM_ZH_powheg)
    h_SM_ZH_JHU_c=h1.Get(name_SM_ZH_JHU)
    h_SM_ZH_powheg=h_SM_ZH_powheg_c.Clone()
    h_SM_ZH_JHU=h_SM_ZH_JHU_c.Clone()
    
    h_SM_ggH_powheg_c=h1.Get(name_SM_ggH_powheg)
    h_SM_ggH_JHU_c=h1.Get(name_SM_ggH_JHU)
    h_SM_ggH_powheg=h_SM_ggH_powheg_c.Clone()
    h_SM_ggH_JHU=h_SM_ggH_JHU_c.Clone()
    
    
    scale_to_Powheg_qqH=1.
    scale_to_Powheg_WH=1.
    scale_to_Powheg_ZH=1.
    scale_to_Powheg_ggH=1.


    # for 0jet/boosted
    if "vbf" not in nom:

#        print "to not in:  Powheg qqH: %s, ggH %s, WH %s, ZH %s"%(yield_qqH_Powheg,yield_ggH_Powheg,yield_WH_Powheg,yield_ZH_Powheg)
#        print "to not in:  JHU qqH: %s, ggH %s, WH %s, ZH %s"%(yield_qqH_JHU,yield_ggH_JHU,yield_WH_JHU,yield_ZH_JHU)
        print " \t\t\t  ===> this is not VBF category!"
        yield_qqH_Powheg=h_SM_qqH_powheg.Integral()
        yield_qqH_JHU=h_SM_qqH_JHU.Integral()
        yield_WH_Powheg=h_SM_WH_powheg.Integral()
        yield_WH_JHU=h_SM_WH_JHU.Integral()
        yield_ZH_Powheg=h_SM_ZH_powheg.Integral()
        yield_ZH_JHU=h_SM_ZH_JHU.Integral()
        yield_ggH_Powheg=h_SM_ggH_powheg.Integral()
        yield_ggH_JHU=h_SM_ggH_JHU.Integral()
    else:
        print "summed VBFH VBF yield: ",yield_qqH_Powheg_vbf

        yield_qqH_Powheg=yield_qqH_Powheg_vbf
        yield_qqH_JHU=yield_qqH_JHU_vbf
        yield_WH_Powheg=yield_WH_Powheg_vbf
        yield_WH_JHU=yield_WH_JHU_vbf
        yield_ZH_Powheg=yield_ZH_Powheg_vbf
        yield_ZH_JHU=yield_ZH_JHU_vbf
        yield_ggH_Powheg=yield_ggH_Powheg_vbf
        yield_ggH_JHU=yield_ggH_JHU_vbf
        
    
    print " yield_qqH_Powheg= ",yield_qqH_Powheg 
    file.cd(nom)
    if (h_SM_qqH_JHU.Integral()>0):
        scale_to_Powheg_qqH=yield_qqH_Powheg/yield_qqH_JHU
    if (h_SM_WH_JHU.Integral()>0):
        scale_to_Powheg_WH=yield_WH_Powheg/yield_WH_JHU
    if (h_SM_ZH_JHU.Integral()>0):
        scale_to_Powheg_ZH=yield_ZH_Powheg/yield_ZH_JHU
    if (h_SM_ggH_JHU.Integral()>0):
        scale_to_Powheg_ggH=yield_ggH_Powheg/yield_ggH_JHU
        
    print "scale to Powheg ggH= ",scale_to_Powheg_ggH
 
    for k2 in histoList:
        if (k2.GetName()!=name_last):
            h2 = k2.ReadObj()
            h3=h2.Clone()
            h3.SetName(k2.GetName())
            if "WH_htt_0" in h3.GetName() and h3.Integral()>0.:
                h3.Scale(scale_to_Powheg_WH)
#                h3.Scale(h_SM_WH_powheg.Integral()/h3.Integral())
            if "ZH_htt_0" in h3.GetName() and h3.Integral()>0.:
                h3.Scale(scale_to_Powheg_ZH)
#                h3.Scale(h_SM_ZH_powheg.Integral()/h3.Integral())
            if "GGH2Jets_" in h3.GetName() and h3.Integral()>0.:
                h3.Scale(scale_to_Powheg_ggH)
#                h3.Scale(h_SM_ggH_powheg.Integral()/h3.Integral())
            if "qqH_htt_" in h3.GetName() and h3.Integral()>0.:
                h3.Scale(scale_to_Powheg_qqH)
#                 h3.Scale(h_SM_qqH_powheg.Integral()/h3.Integral())
                   
            nom=k1.GetName()
            dir_m_name=nom
            #print "goto dir: ",dir_m_name
            nom_out=nom
            nom_out=nom_out.replace("D0ggH_0p00to0p30","ggHMELA_bin1")
            nom_out=nom_out.replace("D0ggH_0p30to0p45","ggHMELA_bin2")
            nom_out=nom_out.replace("D0ggH_0p45to0p55","ggHMELA_bin3")
            nom_out=nom_out.replace("D0ggH_0p55to1p00","ggHMELA_bin4")

            nom_out=nom_out.replace("D0ggH_0p0to0p2","ggHMELA_bin1")
            nom_out=nom_out.replace("D0ggH_0p2to0p4","ggHMELA_bin2")
            nom_out=nom_out.replace("D0ggH_0p4to0p7","ggHMELA_bin3")
            nom_out=nom_out.replace("D0ggH_0p7to1p0","ggHMELA_bin4")

            file1.cd(nom_out)

            name_histo=h3.GetName()
            #print " dir: ",nom, "  histo_name= ",name_histo    
            # for 0jet/boosted category do NOT use JHU ggH sample, use Powheg and rename to "GGH2Jets*BLA*"

            # for 0jet/boosted
            if "ggH125" in h3.GetName() and "vbf" not in nom:
                #print " dir: ",nom, "  histo_name= ",name_histo
                name_histo=h3.GetName().replace("ggH125","GGH2Jets_sm_M125")
                #name_histo=h3.GetName().replace("ggH125","reweighted_GGH2Jets_0PM")
                #print "     ===> output histo name : ",name_histo
                name_histo_PS=h3.GetName().replace("ggH125","GGH2Jets_pseudoscalar_M125")
                name_histo_maxmix=h3.GetName().replace("ggH125","GGH2Jets_pseudoscalar_Mf05ph0125")
                h3.Write()
                h3.SetName(name_histo)
                h3.Write(name_histo)
                h3.SetName(name_histo_PS)
                h3.Write(name_histo_PS)
                h3.SetName(name_histo_maxmix)
                h3.Write(name_histo_maxmix)

                if "GGH" in h3.GetName() or "GGH" in name_histo_PS:
                  print " \t ==> *** HERE 1: out histo name: ", h3.GetName(), "  and ",name_histo_PS

            # 0jet/boosted
            if "reweighted_GGH2Jets_0PM" in h3.GetName() and "vbf" not in nom:
#            if "GGH2Jets_sm_M" in h3.GetName() and "to" not in nom:
                #print " dir: ",nom, "  histo_name= ",name_histo
#                name_histo=h3.GetName().replace("GGH2Jets_sm_M","GGH2Jets_sm_origiM")
                name_histo=h3.GetName().replace("reweighted_GGH2Jets_0PM","GGH2Jets_sm_origiM")
                #print "     ===> output histo name : ",name_histo
#            if "GGH2Jets_pseudoscalar_M" in h3.GetName() and "to" not in nom:
            if "reweighted_GGH2Jets_0M" in h3.GetName() and "vbf" not in nom:
                #print " dir: ",nom, "  histo_name= ",name_histo
#                name_histo=h3.GetName().replace("GGH2Jets_pseudoscalar_M","GGH2Jets_pseudoscalar_origiM")
                name_histo=h3.GetName().replace("reweighted_GGH2Jets_0M","GGH2Jets_pseudoscalar_origiM")
                #print "     ===> output histo name : ",name_histo, "   yield: ",h3.Integral()
            if ("GGH2Jets" in h3.GetName()) and ("reweighted" not in h3.GetName()) and ("vbf" not in nom):
                print " =====> IT PASSES SEL"
		name_histo=name_histo.replace("GGH2Jets_0M","GGH2Jets_pseudoscalar_origi2M")
                name_histo=name_histo.replace("GGH2Jets_0PM","GGH2Jets_sm_origi2M")
                name_histo=name_histo.replace("GGH2Jets_pseudoscalar_M","GGH2Jets_pseudoscalar_origi3M")
                name_histo=name_histo.replace("GGH2Jets_sm_M","GGH2Jets_sm_origi3M")

            if ("GGH2Jets" in h3.GetName()) and ("reweighted" not in h3.GetName()):
                print " =====> IT PASSES SEL 1"
            if ("GGH2Jets" in h3.GetName()) and ("vbf" not in nom):
                print " =====> IT PASSES SEL 2"


            name_last=h3.GetName()

	    
            #name_histo=name_histo.replace("embedded","embedded")
            name_histo=name_histo.replace("JetFakes","jetFakes")
            h3.SetName(name_histo)
            h3.Write(name_histo)

            if "GGH" in name_histo:
              print " \t ==> *** HERE 2: out histo name: ", name_histo
#            h3.Write(name_histo.replace("M0","M"))

            # now in output file write histos reweighted_GGH2Jets_0PM125 as GGH2Jets_sm_M125 (so that I don't have to change Htt files..)
#            if "reweighted_GGH2Jets_" in name_histo and "to" in nom:

            # for vbf
	    if "reweighted_GGH2Jets_" in name_histo and ("vbf" in nom):
               name_histo=name_histo.replace("reweighted_GGH2Jets_0PM","GGH2Jets_sm_M")
	       name_histo=name_histo.replace("reweighted_GGH2Jets_0M","GGH2Jets_pseudoscalar_M")	
               h3.SetName(name_histo)     
               h3.Write(name_histo)     
               h3.SetName(name_histo.replace("M0","M"))
               h3.Write(name_histo.replace("M0","M"))
               if name_histo=="GGH2Jets_sm_M125":
                print "summed ggH SM yield: ",h3.Integral()

               print " \t ==> *** HERE 3: out histo name: ", name_histo
#               h3.Write(name_histo.replace("M0","M"))

    h1.Close()
