#!/usr/bin/env python
import ROOT
import re
from array import array
from optparse import OptionParser
import sys

inputfile = sys.argv[1]
channel = sys.argv[2]
outputfile= sys.argv[3]

parser = OptionParser()
parser.add_option('--svn', '-s', action='store_true',
                  default=False, dest='is_SVN',
                  help='input is SVN datacard'
                  )
parser.add_option('--ztt', '-z', action='store_true',
                  default=False, dest='is_zttMC',
                  help='run on embedded or MC ZTT'
                  )
parser.add_option('--shapeSyst', '-y', action='store_true',
                  default=False, dest='do_shapeSyst',
                  help='use shapeSyst or not'
                  )
(options, args) = parser.parse_args()

islog=1
'''
def add_lumi():
    lowX=0.7
    lowY=0.835
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.30, lowY+0.16, "NDC")
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.SetTextSize(0.08)
    lumi.SetTextFont (   42 )
    lumi.AddText("2016, 20.1 fb^{-1} (13 TeV)")
    return lumi

def add_CMS():
    lowX=0.11
    lowY=0.835
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.15, lowY+0.16, "NDC")
    lumi.SetTextFont(61)
    lumi.SetTextSize(0.1)
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.AddText("CMS")
    return lumi

def add_Preliminary():
    lowX=0.18
    lowY=0.835
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.15, lowY+0.16, "NDC")
    lumi.SetTextFont(52)
    lumi.SetTextSize(0.08)
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.AddText("Preliminary")
    return lumi

def make_legend():
	if islog:
	   output = ROOT.TLegend(0.12, 0.05, 0.92, 0.25, "", "brNDC")
           output.SetNColumns(5)
	else:
           output = ROOT.TLegend(0.55, 0.3, 0.92, 0.75, "", "brNDC")
	   output.SetNColumns(2)
        output.SetLineWidth(0)
        output.SetLineStyle(0)
        #output.SetFillStyle(0)
        output.SetFillColor(0)
        output.SetBorderSize(0)
        output.SetTextFont(62)
        return output

ROOT.gStyle.SetFrameLineWidth(3)
ROOT.gStyle.SetLineWidth(3)
ROOT.gStyle.SetOptStat(0)

c=ROOT.TCanvas("canvas","",0,0,1800,600)
c.cd()
'''
file=ROOT.TFile(inputfile,"r")
file1D=ROOT.TFile("htt_"+channel+".inputs-sm-13TeV-2D"+outputfile+".root","recreate")
categories_list= file.GetListOfKeys()
categories=[]
for k2 in categories_list:
	categories.append(k2.GetName())

#categories=[channel+"_0jet",channel+"_boosted",channel+"_vbf",channel+"_vbf_D0_0p0to0p2",channel+"_vbf_D0_0p2to0p4",channel+"_vbf_D0_0p4to0p8",channel+"_vbf_D0_0p8to1p0",channel+"_vbf_D0_0p0to0p2_DCPm",channel+"_vbf_D0_0p2to0p4_DCPm",channel+"_vbf_D0_0p4to0p8_DCPm",channel+"_vbf_D0_0p8to1p0_DCPm",channel+"_vbf_D0_0p0to0p2_DCPp",channel+"_vbf_D0_0p2to0p4_DCPp",channel+"_vbf_D0_0p4to0p8_DCPp",channel+"_vbf_D0_0p8to1p0_DCPp",channel+"_vbf_ggHMELA_bin1",channel+"_vbf_ggHMELA_bin2",channel+"_vbf_ggHMELA_bin3",channel+"_vbf_ggHMELA_bin4"] # input dir names

#processes=["data_obs","embed","VVT","W","ZL","ZJ","TTT","TTJ","VVJ","ggH125","VBF125","WH125","ZH125","jetFakes","EWKZ"] # input histos
#processes=["data_obs","embedded","ZTT","VVT","W","ZL","ZJ","TTT","TTJ","VVJ","ggH125","VBF125","WH125","ZH125","jetFakes","EWKZ", "reweighted_qqH_htt_0L1125","reweighted_qqH_htt_0L1Zg125","reweighted_qqH_htt_0M125","reweighted_qqH_htt_0PM125","reweighted_qqH_htt_0PH125","reweighted_qqH_htt_0L1f05ph0125","reweighted_qqH_htt_0L1Zgf05ph0125","reweighted_qqH_htt_0Mf05ph0125","reweighted_qqH_htt_0PHf05ph0125","reweighted_WH_htt_0L1125","reweighted_WH_htt_0M125","reweighted_WH_htt_0PM125","reweighted_WH_htt_0PH125","reweighted_WH_htt_0L1f05ph0125","reweighted_WH_htt_0L1Zgf05ph0125","reweighted_WH_htt_0Mf05ph0125","reweighted_WH_htt_0PHf05ph0125","reweighted_ZH_htt_0L1125","reweighted_ZH_htt_0L1Zg125","reweighted_ZH_htt_0M125","reweighted_ZH_htt_0PM125","reweighted_ZH_htt_0PH125","reweighted_ZH_htt_0L1f05ph0125","reweighted_ZH_htt_0L1Zgf05ph0125","reweighted_ZH_htt_0Mf05ph0125","reweighted_ZH_htt_0PHf05ph0125","GGH2Jets_sm_M125","GGH2Jets_pseudoscalar_M125", "GGH2Jets_pseudoscalar_Mf05ph0125"] # input histos
processes=["data_obs","embedded","VVT","W","ZL","TTT","ggH125","VBF125","WH125","ZH125","jetFakes","EWKZ", "reweighted_qqH_htt_0L1125","reweighted_qqH_htt_0L1Zg125","reweighted_qqH_htt_0M125","reweighted_qqH_htt_0PM125","reweighted_qqH_htt_0PH125","reweighted_qqH_htt_0L1f05ph0125","reweighted_qqH_htt_0L1Zgf05ph0125","reweighted_qqH_htt_0Mf05ph0125","reweighted_qqH_htt_0PHf05ph0125","reweighted_WH_htt_0L1125","reweighted_WH_htt_0M125","reweighted_WH_htt_0PM125","reweighted_WH_htt_0PH125","reweighted_WH_htt_0L1f05ph0125","reweighted_WH_htt_0Mf05ph0125","reweighted_WH_htt_0PHf05ph0125","reweighted_ZH_htt_0L1125","reweighted_ZH_htt_0M125","reweighted_ZH_htt_0PM125","reweighted_ZH_htt_0PH125","reweighted_ZH_htt_0L1f05ph0125","reweighted_ZH_htt_0Mf05ph0125","reweighted_ZH_htt_0PHf05ph0125","GGH2Jets_sm_M125","GGH2Jets_pseudoscalar_M125", "GGH2Jets_pseudoscalar_Mf05ph0125"] # input histos

processes_plot_bkg=["embedded","W","QCD","ZL","ZJ","TTT","TTJ","VV","EWKZ"] # bkg processes for plot
if options.is_SVN:
    del categories[:]
    del processes[:]
    categories=["htt_"+channel+"_1_13TeV","htt_"+channel+"_2_13TeV","htt_"+channel+"_3_13TeV"]
    processes=["data_obs","ZTT","W","QCD","ZL","ZJ","TTT","TTJ","VV","EWKZ","ggH_htt125","qqH_htt125","WH_htt125","ZH_htt125"] # input histos    

if options.is_zttMC:
    del processes[:]
    del processes_plot_bkg[:]
    processes=["data_obs","ZTT","W","QCD","ZL","ZJ","TTT","TTJ","VV","EWKZ","ggH125","VBF125","WH125","ZH125"] # input histos    
    processes_plot_bkg=["ZTT","W","QCD","ZL","ZJ","TTT","TTJ","VV","EWKZ"]

if channel == 'tt':
    print "this is tt channel"    
    #processes.remove("VV")
    #processes.insert(8,"VVT")
    #processes.insert(9,"VVJ")
    #processes_plot_bkg.remove("VV")
    #processes_plot_bkg.insert(8,"VVT")
    #processes_plot_bkg.insert(9,"VVJ")

if channel == 'et':
    print "this is et channel"
    #processes.remove("embedded")
    #processes.insert(2,"ZTT")

#cat=[channel+"_0jet",channel+"_boosted",channel+"_vbf_D0_0p0to0p2_DCPm",channel+"_vbf_D0_0p2to0p4_DCPm",channel+"_vbf_D0_0p4to0p8_DCPm",channel+"_vbf_D0_0p8to1p0_DCPm",channel+"_vbf_D0_0p0to0p2_DCPp",channel+"_vbf_D0_0p2to0p4_DCPp",channel+"_vbf_D0_0p4to0p8_DCPp",channel+"_vbf_D0_0p8to1p0_DCPp"] # outout dir names

#cat=[channel+"_0jet",channel+"_boosted",channel+"_vbf",channel+"_vbf_D0_0p0to0p2",channel+"_vbf_D0_0p2to0p4",channel+"_vbf_D0_0p4to0p8",channel+"_vbf_D0_0p8to1p0",channel+"_vbf_D0_0p0to0p2_DCPm",channel+"_vbf_D0_0p2to0p4_DCPm",channel+"_vbf_D0_0p4to0p8_DCPm",channel+"_vbf_D0_0p8to1p0_DCPm",channel+"_vbf_D0_0p0to0p2_DCPp",channel+"_vbf_D0_0p2to0p4_DCPp",channel+"_vbf_D0_0p4to0p8_DCPp",channel+"_vbf_D0_0p8to1p0_DCPp",channel+"_vbf_ggHMELA_bin1",channel+"_vbf_ggHMELA_bin2",channel+"_vbf_ggHMELA_bin3",channel+"_vbf_ggHMELA_bin4"] # input dir names
cat=categories

systematics=[] # systematics
if options.do_shapeSyst:
        systematics=[ # systematics
                "_CMS_htt_dyShape_13TeV",
                "_CMS_htt_jetToTauFake_13TeV",
                "_CMS_htt_ttbarShape_13TeV",
                "_CMS_scale_t_13TeV",
                "_CMS_scale_t_1prong_13TeV",
                "_CMS_scale_t_1prong1pizero_13TeV",
                "_CMS_scale_t_3prong_13TeV",
                "_CMS_scale_met_unclustered_13TeV",
                "_CMS_scale_met_clustered_13TeV",
                "_CMS_scale_j_13TeV",
                "_CMS_htt_zmumuShape_VBF_13TeV",
		"_CMS_htt_ff_qcd_dm0_njet0_stat_",
                "_CMS_htt_ff_qcd_dm0_njet1_stat_",
                "_CMS_htt_ff_qcd_dm1_njet0_stat_",
                "_CMS_htt_ff_qcd_dm1_njet1_stat_",
                "_CMS_htt_ff_qcd_syst_",
                "_CMS_htt_ff_tt_frac_syst_",
                "_CMS_htt_ff_tt_syst_",
                "_CMS_htt_ff_w_frac_syst_",
                "_CMS_htt_ff_w_syst_"

        ]


processes_plot_signal=["ggH125","VBF125",] # signal processes for plot
#ncat=3
ncat=len(cat)
print "ncat=",ncat
adapt=ROOT.gROOT.GetColor(12)
new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
trans=ROOT.TColor(new_idx, adapt.GetRed(), adapt.GetGreen(),adapt.GetBlue(), "",0.5)
for i in range (0,ncat): # loop over categories
    if channel == 'tt':
        cat[i]=cat[i].replace('0p0','0')
        cat[i]=cat[i].replace('1p0','1')
        categories[i]=categories[i].replace('0p0','0')
        categories[i]=categories[i].replace('1p0','1')
        '''
        if channel == 'et':
        cat[i]=cat[i].replace('ggHMELA_bin1','D0ggH_0p0to0p2')
        cat[i]=cat[i].replace('ggHMELA_bin2','D0ggH_0p2to0p4')
        cat[i]=cat[i].replace('ggHMELA_bin3','D0ggH_0p4to0p7')
        cat[i]=cat[i].replace('ggHMELA_bin4','D0ggH_0p7to1p0')
        categories[i]=categories[i].replace('ggHMELA_bin1','D0ggH_0p0to0p2')
        categories[i]=categories[i].replace('ggHMELA_bin2','D0ggH_0p2to0p4')
        categories[i]=categories[i].replace('ggHMELA_bin3','D0ggH_0p4to0p7')
        categories[i]=categories[i].replace('ggHMELA_bin4','D0ggH_0p7to1p0')
        '''
    mydir=file1D.mkdir(cat[i])

    print "=================>>>  category: ", categories[i]
    N_histo=0
    binsX_N=0
    binsY_N=0
    binsX_low=0
    binsY_low=0
    binsX_high=0
    binsY_high=0

    binsX_N_first=0
    binsY_N_first=0
    binsX_low_first=0
    binsY_low_first=0
    binsX_high_first=0
    binsY_high_first=0

    for i_histo in processes: # loop over input histos (processes)
        print " histo: ", i_histo
        N_histo=N_histo+1
            
        histo2D=file.Get(categories[i]).Get(i_histo)

        if N_histo==1:
            binsX_N_first=histo2D.GetNbinsX()
            binsY_N_first=histo2D.GetNbinsY()
            binsX_low_first=histo2D.GetXaxis().GetBinLowEdge(1)
            binsX_high_first=histo2D.GetXaxis().GetBinLowEdge(binsX_N_first+1)
            binsY_low_first=histo2D.GetYaxis().GetBinLowEdge(1)
            binsY_high_first=histo2D.GetYaxis().GetBinLowEdge(binsX_N_first+1)

        binsX_N=histo2D.GetNbinsX()
        binsY_N=histo2D.GetNbinsY()
        binsX_low=histo2D.GetXaxis().GetBinLowEdge(1)
        binsX_high=histo2D.GetXaxis().GetBinLowEdge(binsX_N+1)
        binsY_low=histo2D.GetYaxis().GetBinLowEdge(1)
        binsY_high=histo2D.GetYaxis().GetBinLowEdge(binsX_N+1)

        if binsX_N_first!=binsX_N or binsY_N_first!=binsY_N or binsX_low_first!=binsX_low or binsX_high_first!=binsX_high or binsY_low_first!=binsY_low or binsY_high_first!=binsY_high:
            print " ###########################   WARNING! different binning: X bins %s %s, Y bins %s %s, X low %s %s, Y low %s %s   "%(binsX_N_first, binsX_N, binsY_N_first, binsY_N, binsX_low_first, binsX_low,binsY_low_first, binsY_low )

        nx=histo2D.GetXaxis().GetNbins()
        ny=histo2D.GetYaxis().GetNbins()
        histo=ROOT.TH1F("histo",histo2D.GetName(),nx*ny,0,nx*ny)
        histo.SetName(histo2D.GetName())    
        if options.is_SVN:
            if histo2D.GetName()=="ggH_htt125":
                histo.SetName("ggH125")
            if histo2D.GetName()=="qqH_htt125":
                histo.SetName("VBF125")
            if histo2D.GetName()=="WH_htt125":
                histo.SetName("WH125")
            if histo2D.GetName()=="ZH_htt125":
                histo.SetName("ZH125")
            if histo2D.GetName()=="ZTT":
                histo.SetName("embedded")

        #print " in histo: ",histo2D.GetName(), 
        l=0
        for j in range(1,nx+1):
            for k in range(1,ny+1):
	        l=l+1
                n = histo2D.GetBin(j,k);
                histo.SetBinContent(l,histo2D.GetBinContent(n))
                histo.SetBinError(l,histo2D.GetBinError(n))
        mydir.cd()
        histo.Write()

        for systematic in systematics : # loop over available systematics
            if file.Get(categories[i]).Get(i_histo+systematic+"Down") != None:
                print i_histo+systematic+"Down/Up"
                histo2D_d=file.Get(categories[i]).Get(i_histo+systematic+"Down") #
                histo2D_u=file.Get(categories[i]).Get(i_histo+systematic+"Up") #
                histo_d=ROOT.TH1F("histo_d",histo2D_d.GetName(),nx*ny,0,nx*ny) #
                histo_u=ROOT.TH1F("histo_u",histo2D_u.GetName(),nx*ny,0,nx*ny) #
                print "  input name: %s, %s"%(histo2D_d.GetName(),histo2D_u.GetName())
                histo_d_name=histo2D_d.GetName()
                histo_u_name=histo2D_u.GetName()
                histo_d_name=histo_d_name.replace('dm0','1prong')
                histo_u_name=histo_u_name.replace('dm0','1prong')
                histo_d_name=histo_d_name.replace('dm1','3prong')
                histo_u_name=histo_u_name.replace('dm1','3prong')

                histo_d_name=histo_d_name.replace('_stat_Down','_stat_13TeVDown')
                histo_u_name=histo_u_name.replace('_stat_Up','_stat_13TeVUp')
                histo_d_name=histo_d_name.replace('_syst_Down','_syst_13TeVDown')
                histo_u_name=histo_u_name.replace('_syst_Up','_syst_13TeVUp')

                print "    output name: %s, %s"%(histo_d.GetName(),histo_u.GetName())

                l=0
                for j in range(1,nx+1):
                    for k in range(1,ny+1):
                        l=l+1
                        n = histo2D.GetBin(j,k);
                        histo_u.SetBinContent(l,histo2D_u.GetBinContent(n)) #
                        histo_u.SetBinError(l,histo2D_u.GetBinError(n)) #
                        histo_d.SetBinContent(l,histo2D_d.GetBinContent(n)) #
                        histo_d.SetBinError(l,histo2D_d.GetBinError(n)) #
                histo_u.Write(histo_u_name) #
                histo_d.Write(histo_d_name) #           
            #else :
            #    print i_histo+systematic+"Down/Up fail"

# now make nice unrolled plots:
