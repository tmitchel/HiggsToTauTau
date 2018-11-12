#!/usr/bin/env python
import ROOT
import re
from array import array
import math
import plotRocCurve_def

obs = "M_{#tau#tau}"
obs1= "m_sv"#"abs_Heata.5jjeta"
file=ROOT.TFile("final_nominal.root","r")
cate={"mt_0jet":"0jet","mt_boosted":"Boosted","mt_vbf":"VBF"}
#cate={"mt_vbf":"VBF"}

sig_stackScale = 30
majors=["ZTT","QCD","TTT"]
minors=["ZL","ZJ","TTJ","W","VV"]
signals=["ggH125","VBF125","WH125","ZH125"]
# Colors
mypalette=["#f9cd66","#ffbcfe","#cfe87f","#fcc894","#a0abff","#d1c7be","#9feff2"]
adapt=ROOT.gROOT.GetColor(12)
new_idx=ROOT.gROOT.GetListOfColors().GetSize() + 1
trans=ROOT.TColor(new_idx, adapt.GetRed(), adapt.GetGreen(),adapt.GetBlue(), "",0.5)


def add_lumi():
    lowX=0.50
    lowY=0.835
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.30, lowY+0.16, "NDC")
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.SetTextSize(0.05)
    lumi.SetTextFont (   42 )
    lumi.AddText("#mu#tau_{h}   2016, 35.9 fb^{-1} (13 TeV)")
    return lumi

def add_CMS():
    lowX=0.21
    lowY=0.70
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.15, lowY+0.16, "NDC")
    lumi.SetTextFont(61)
    lumi.SetTextSize(0.08)
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.AddText("CMS")
    return lumi

def add_Preliminary():
    lowX=0.21
    lowY=0.63
    lumi  = ROOT.TPaveText(lowX, lowY+0.06, lowX+0.15, lowY+0.16, "NDC")
    lumi.SetTextFont(52)
    lumi.SetTextSize(0.06)
    lumi.SetBorderSize(   0 )
    lumi.SetFillStyle(    0 )
    lumi.SetTextAlign(   12 )
    lumi.SetTextColor(    1 )
    lumi.AddText("Preliminary")
    return lumi

def make_legend(x1,y1,x2,y2):
    output = ROOT.TLegend(x1,y1,x2,y2, "", "brNDC")
    #output = ROOT.TLegend(0.2, 0.1, 0.47, 0.65, "", "brNDC")
    output.SetLineWidth(0)
    output.SetLineStyle(0)
    output.SetFillStyle(0)
    output.SetBorderSize(0)
    output.SetMargin(0.3)
    output.SetTextFont(62)
    return output

def add_legendEntryMain(smh,ggh,vbf,wh,zh,cat):
    legend = make_legend(0.60, 0.52, 0.95, 0.80)
    legend.AddEntry(Data,"Data","elp")    
    #if smh is 1:
    #    legend.AddEntry(main_SMH,"SM Higgs(125)x30.0","l")
    if ggh is 1:
        legend.AddEntry(main_ggH,"ggH Higgs(125)x"+str(sig_stackScale),"l")
    if vbf is 1:
        legend.AddEntry(main_VBF,"VBF Higgs(125)x"+str(sig_stackScale),"l")
    if wh is 1:
        legend.AddEntry(main_WH,"WH Higgs(125)x"+str(sig_stackScale),"l")
    if zh is 1:
        legend.AddEntry(main_ZH,"ZH Higgs(125)x"+str(sig_stackScale),"l")
    legend.AddEntry(histoAll["histBkg"][cate[cat]][0],"Z#rightarrow#tau#tau","f")
    legend.AddEntry(histoAll["histBkg"][cate[cat]][1],"QCD","f")
    #legend.AddEntry(histoAll["histBkg"][cate[cat]][2],"W+Jets","f")
    legend.AddEntry(histoAll["histBkg"][cate[cat]][2],"TTT","f")
    #legend.AddEntry(histoAll["histBkg"][cate[cat]][4],"TTJ","f")
    legend.AddEntry(histoAll["histBkg"][cate[cat]][-1],"others","f")
    legend.AddEntry(error,"Uncertainty","f")
    return legend

def add_cate(category):
    lowX=0.21 
    lowY=0.63
    categ  = ROOT.TPaveText(lowX, lowY, lowX+0.12, lowY+0.12, "NDC")
    categ.SetBorderSize(   0 )
    categ.SetFillStyle(    0 )
    categ.SetTextAlign(   12 )
    categ.SetTextSize ( 0.045 )
    categ.SetTextColor(    1 )
    categ.SetTextFont (   41 )
    categ.AddText(category)
    return categ

def make_canvas(hight,c_name):
    c=ROOT.TCanvas(c_name,c_name,0,0,600,hight)
    return c

def set_padMargin(pad,left,right,top,bottom):
    pad.SetLeftMargin(left)
    pad.SetRightMargin(right)
    pad.SetTopMargin(top)
    pad.SetBottomMargin(bottom)
    pad.SetTickx(1)
    pad.SetTicky(1)

def make_stackPad(y_low,y_high):
    padStack = ROOT.TPad("padStack","padStack",0,y_low,1,y_high)
    padStack.SetFillColor(0)
    padStack.SetBorderMode(0)
    padStack.SetBorderSize(10)
    padStack.SetTickx(1)
    padStack.SetTicky(1)
    padStack.SetFrameFillStyle(0)
    padStack.SetFrameLineStyle(0)
    padStack.SetFrameLineWidth(1)
    padStack.SetFrameBorderMode(0)
    padStack.SetFrameBorderSize(10)
    return padStack

def unroll(file,cat,hist):
    if file.Get(cat).Get(hist).GetDimension() is 1 : 
        r_hist = file.Get(cat).Get(hist).Clone()
    else :
        r_hist = file.Get(cat).Get(hist).ProjectionX().Clone()
    return r_hist

def call_histos():
    histos = {"histSig":{},"histBkg":{},"histData":{}}
    for cat in cate.keys():
        histlist=[] # all bkg histograms go into here
        histlist_sig=[] # ggH, VBF and SMH(ggH+VBF+VH) go into here
        ''' Save histograms in the list '''
        # signals
        for signal in signals:
            histlist_sig.append(unroll(file,cat,signal))#file.Get(cat).Get(signal))
        # major bkg
        for major in majors:
            histlist.append(unroll(file,cat,major))#file.Get(cat).Get(major))
        # combine minor bkg
        h_minor = unroll(file,cat,minors[0])#file.Get(cat).Get(minors[0])
        for minor in minors:
            if(minor!=minors[0]): 
                h_minor.Add(unroll(file,cat,minor),1)#file.Get(cat).Get(minor),1)
        histlist.append(h_minor)
        # data
        h_data=unroll(file,cat,"data_obs")#file.Get(cat).Get("data_obs")
        
        # add histograms into dictionary histos
        histos["histSig"][cate[cat]]=histlist_sig
        histos["histBkg"][cate[cat]]=histlist
        histos["histData"][cate[cat]]=h_data
    return histos

def set_dataStyle(cat):
    Data = histoAll["histData"][cate[cat]].Clone()
    Data.SetMarkerStyle(20)
    Data.SetLineColor(1)
    Data.SetMarkerSize(1)
    Data.SetMaximum(Data.GetMaximum()*1.60)#,stack.GetMaximum()*1.20))
    Data.SetMinimum(0)
    Data.SetTitle("")
    Data.GetXaxis().SetTitle("")
    Data.GetXaxis().SetLabelSize(0)
    Data.GetXaxis().SetTitleSize(0)
    Data.GetXaxis().SetNdivisions(505)
    Data.GetYaxis().SetTitle("Events/bin")    
    Data.GetYaxis().SetTitleOffset(1.4)
    Data.GetYaxis().SetTitleSize(0.055)
    Data.GetYaxis().SetLabelOffset(0.01)
    Data.GetYaxis().SetLabelSize(0.045)
    Data.GetYaxis().SetLabelFont(42)

    return Data

def make_stack(category):
    stack=ROOT.THStack("","")
    #errorBand=histoAll["histBkg"][cat].Clone() 
    c_index = 0
    for h_bkg in histoAll["histBkg"][category]:
        h_bkg.SetLineWidth(2)
        h_bkg.SetLineColor(1)
        h_bkg.SetFillColor(ROOT.TColor.GetColor(mypalette[c_index]))
        if h_bkg is histoAll["histBkg"][category][-1]:
            h_bkg.SetFillColor(ROOT.TColor.GetColor(mypalette[-1]))
        c_index+=1
        stack.Add(h_bkg)
    #stack.SetMaximum(stack.GetMaximum()*1.60)
    return stack

def make_sig(category,sig,color,style,scale):
    h_sig = unroll(file,category,sig)#file.Get(category).Get(sig).Clone()
    #h_sig.SetLineColor(ROOT.kBlue)    
    h_sig.SetMarkerStyle(0)
    h_sig.SetLineWidth(4)
    h_sig.SetLineStyle(style)
    h_sig.Scale(scale)
    return h_sig.Clone()

def make_errorBand(category):
    errorBand=histoAll["histBkg"][category][0].Clone()
    for h_bkg in histoAll["histBkg"][category]:
        if h_bkg is not histoAll["histBkg"][category][0]:
            errorBand.Add(h_bkg)
    errorBand.SetMarkerSize(0)
    errorBand.SetFillColor(new_idx)
    errorBand.SetFillStyle(3001)
    errorBand.SetLineWidth(0)
    return errorBand

def make_dividedHisto(num,deno,min,max,off,title,end):
    h_ratio = num.Clone()
    h_deno = deno.Clone()
    h_ratio.Divide(h_deno)
    h_ratio.Sumw2()
    if min is not max:
        h_ratio.SetMaximum(max)
        h_ratio.SetMinimum(min)
    else:
        h_ratio.SetMaximum(h_ratio.GetMaximum()*1.4)
        h_ratio.SetMinimum(h_ratio.GetMinimum()*0.1)
    h_ratio.SetMarkerStyle(21)
    h_ratio.SetLineStyle(0)
    h_ratio.SetLineColor(1)
    h_ratio.SetLineWidth(1)
    h_ratio.GetYaxis().SetTitle(title)
    h_ratio.GetYaxis().SetTitleOffset(off)
    h_ratio.GetYaxis().SetTitleFont(42)
    h_ratio.GetYaxis().SetTitleSize(0.16)
    h_ratio.GetYaxis().SetLabelSize(0.16) #
    h_ratio.GetYaxis().SetLabelFont(42)
    if end is 1:
        h_ratio.GetYaxis().SetTitleOffset(off*1.12)
        h_ratio.GetYaxis().SetTitleSize(0.14)
        h_ratio.GetYaxis().SetLabelSize(0.14) #
    h_ratio.GetYaxis().SetTitleFont(42)
    h_ratio.GetYaxis().SetNdivisions(5, True)
    h_ratio.GetXaxis().SetNdivisions(505)
    h_ratio.GetXaxis().SetTitleSize(0.14)
    h_ratio.GetXaxis().SetLabelSize(0.14) #
    h_ratio.GetXaxis().SetTitleFont(42)
    return h_ratio

def make_ratioErr(error):
    h_error = error.Clone()
    h_error1 = error.Clone()
    for i in range (1, h_error1.GetSize()-2):
        h_error1.SetBinError(i,0)
    h_error.Sumw2()
    h_error.Divide(h_error1)
    return h_error

def make_titleTag():
    obsPave = ROOT.TPaveText(0.5, 0.1, 0.95, 0.9, "NDC")
    obsPave.SetBorderSize(   0 )
    obsPave.SetFillStyle(    0 )
    obsPave.SetTextAlign(   31 )
    obsPave.SetTextSize (  0.5 )
    obsPave.SetTextColor(    1 )
    obsPave.SetTextFont (   42 )
    obsPave.AddText(obs)
    return obsPave

def compute_SensitivityDeno(h_all,cat):    
    h_sqrt = h_all.Clone()
    for j in range(0,h_sqrt.GetSize()):
        if h_sqrt.GetBinContent(j)<0:
            h_sqrt.SetBinContent(j,0)
        else:
            h_sqrt.SetBinContent(j,math.sqrt(h_sqrt.GetBinContent(j)))
    return h_sqrt

##########################
##     Build Canvas     ##
##########################
ROOT.gStyle.SetFrameLineWidth(1)
ROOT.gStyle.SetLineWidth(1)
ROOT.gStyle.SetOptStat(0)

histoAll = call_histos()


#c1=ROOT.TCanvas("canvas1","",0,0,600,1000)
for cat in cate.keys():
    print cate[cat]+' is processing...'

    ''' Making main histogram pad '''
    # Open new canvas
    p_histoStack = make_canvas(500,"p_histoStack")
    p_histoStack.cd()
    # Draw data
    Data = set_dataStyle(cat)
    Data.Draw("e")
    # Draw bkg
    stack = make_stack(cate[cat])
    stack.Draw("HIST same")
    # Draw error band
    error = make_errorBand(cate[cat])
    error.Draw("e2same")
    # Setup sig - flexible! : Draw at each plot
    #main_SMH = make_sig(cat,"SMH",0,1,30)
    #main_SMH.SetLineColor(ROOT.kBlue)   
    main_ggH = make_sig(cat,"ggH125",0,1,sig_stackScale)
    main_ggH.SetLineColor(ROOT.kBlue)   
    main_VBF = make_sig(cat,"VBF125",0,1,sig_stackScale)
    main_VBF.SetLineColor(ROOT.kRed)   
    # Draw miscellaneous
    lumi = add_lumi()
    lumi.Draw("same")
    cms = add_CMS()
    cms.Draw("same")
    prelim = add_Preliminary()
    prelim.Draw("same")
    categ = add_cate(cate[cat])
    categ.Draw("same")
    Data.Draw("esame")
    p_histoStack.SetFillColor(0)
    p_histoStack.SetBorderMode(0)
    p_histoStack.SetBorderSize(10)
    p_histoStack.SetTickx(1)
    p_histoStack.SetTicky(1)
    p_histoStack.SetFrameFillStyle(0)
    p_histoStack.SetFrameLineStyle(0)
    p_histoStack.SetFrameLineWidth(1)
    p_histoStack.SetFrameBorderMode(0)
    p_histoStack.SetFrameBorderSize(10)
    set_padMargin(p_histoStack,0.18,0.05,0.122,0.0)
    p_histoStack.RedrawAxis()
    print "Main histogram pad is made."

    ''' Making signal histogram pad '''
    p_signal = make_canvas(300,"p_signal")
    p_signal.cd()
    ggH = make_sig(cat,"ggH125",0,1,30)
    VBF = make_sig(cat,"VBF125",0,1,30)
    WH = make_sig(cat,"WH125",0,1,3)
    ZH = make_sig(cat,"ZH125",0,1,3)
    ggH.SetMaximum(ggH.GetMaximum()*1.30)
    ggH.GetYaxis().SetLabelSize(0.10)
    ggH.GetYaxis().SetTitleSize(0.12)
    ggH.GetYaxis().SetTitle("Events/bin")
    ggH.GetYaxis().SetTitleFont(42)
    ggH.GetYaxis().SetTitleOffset(0.48)
    ggH.SetLineColor(ROOT.kRed+1)  
    VBF.SetLineColor(ROOT.kBlue)  
    WH.SetLineColor(ROOT.kOrange)  
    ZH.SetLineColor(ROOT.kRed+1)  
    ggH.Draw("HIST")
    VBF.Draw("same HIST")
    WH.Draw("same HIST")
    ZH.Draw("same HIST")
    # Draw Legend
    legendS = make_legend(0.60, 0.50, 1.00, 0.83)
    legendS.AddEntry(ggH,"ggH Higgs(125)","elp")
    legendS.AddEntry(VBF,"VBF Higgs(125)","l")
    legendS.AddEntry(WH,"WH Higgs(125)x3","l")
    legendS.AddEntry(ZH,"ZH Higgs(125)x3","l")
    legendS.Draw()
    p_signal.SetFillColor(0)
    p_signal.SetBorderMode(0)
    p_signal.SetBorderSize(10)
    p_signal.SetTickx(1)
    p_signal.SetTicky(1)
    p_signal.SetFrameFillStyle(0)
    p_signal.SetFrameLineStyle(0)
    p_signal.SetFrameLineWidth(1)
    p_signal.SetFrameBorderMode(0)
    p_signal.SetFrameBorderSize(10)
    p_signal.SetBorderSize(10)
    set_padMargin(p_signal,0.18,0.05,0.0,0.0)
    print "Signal histogram pad is made."
    
    # h_all histogram which is used often
    #SMH = make_sig(cat,"SMH",0,7,1)
    #h_all = SMH.Clone()
    #h_all.Add(error,1)



    ''' Making ratio pads '''
    off = 0.43
    # ratio[1] : Data/MC
    p_ratio_DataMC = make_canvas(150,"p_ratio_DataMC")
    p_ratio_DataMC.cd()
    p_ratio_DataMC.SetGridy()
    data = histoAll["histData"][cate[cat]].Clone()
    # make_dividedHisto(h_numerator,h_denominator,min,max,offset,title on Y axis, is bottom ratio?)
    h_ratio_DataMC = make_dividedHisto(data,error,0,2,off,"Data / MC", 0)
    h_ratio_DataMC.Draw("e0p")
    h_ratioErr_DataMC = make_ratioErr(error)
    h_ratioErr_DataMC.Draw("e2same")
    set_padMargin(p_ratio_DataMC,0.18,0.05,0.0,0.0)
    print "ratio[1] : Data/MC pad is made."


    # ratio[2] : QCD/VBF
    p_ratio_QCDVBF = make_canvas(150,"p_ratio_QCDVBF")
    p_ratio_QCDVBF.cd()
    p_ratio_QCDVBF.SetGridy()
    h_QCD = histoAll["histBkg"][cate[cat]][1]
    h_VBF = make_sig(cat,"VBF125",0,1,1)
    h_ratio_QCDVBF = make_dividedHisto(h_QCD,h_VBF,0,0,off,"QCD / VBF", 1)
    h_ratio_QCDVBF.Draw("e0p") 
    set_padMargin(p_ratio_QCDVBF,0.18,0.05,0.0,0.2) 
    print "ratio[2] : QCD/VBF pad is made."

    # ratio[3] : VBF/Bkg
    p_ratio_VBFBKG = make_canvas(150,"p_ratio_VBFBKG")
    p_ratio_VBFBKG.cd()
    p_ratio_VBFBKG.SetGridy()
    #h_BKG = histoAll["histBkg"][cate[cat]][1]
    #h_VBF = make_sig(cat,"VBF125",0,1,1)
    h_ratio_VBFBKG = make_dividedHisto(h_VBF,error,0,0,off,"VBF / Bkg", 1)
    h_ratio_VBFBKG.Draw("e0p") 
    set_padMargin(p_ratio_VBFBKG,0.18,0.05,0.0,0.2) 
    print "ratio[3] : VBF/Bkg pad is made."

    # ratio[4] : VBF/ggH
    p_ratio_VBFGGH = make_canvas(150,"p_ratio_VBFGGH")
    p_ratio_VBFGGH.cd()
    p_ratio_VBFGGH.SetGridy()
    h_GGH = make_sig(cat,"ggH125",0,1,1)
    h_ratio_VBFGGH = make_dividedHisto(h_VBF,h_GGH,0,0,off,"VBF / ggH", 1)
    h_ratio_VBFGGH.Draw("e0p") 
    set_padMargin(p_ratio_VBFGGH,0.18,0.05,0.0,0.2) 
    print "ratio[4] : VBF/GGH pad is made."

    # Make canvas 
    plot1 = make_canvas(750,"plot1")  
    # Stick main histogram pad   
    pad_Main = make_stackPad(0.45,1.0)
    pad_Main.Draw()
    pad_Main.cd()
    p_histoStack.DrawClonePad()
    p_histoStack.SetTitle("")    
    #main_SMH.Draw("esame HIST")
    main_ggH.Draw("esame HIST")
    main_VBF.Draw("esame HIST")
    legend = add_legendEntryMain(0,1,1,0,0,cat)
    legend.Draw()
    # Stick ratio Data/MC
    plot1.cd()
    pad_DataMC = make_stackPad(0.27,0.45)
    pad_DataMC.Draw()
    pad_DataMC.cd()
    p_ratio_DataMC.DrawClonePad()
    # Stick 2nd ratio
    plot1.cd()
    pad_2ndRatio = make_stackPad(0.07,0.27)
    pad_2ndRatio.Draw()
    pad_2ndRatio.cd()
    ###         HERE YOU CAN CHOOSE THE 2ND RATIO PAD         ###
    #p_ratio_QCDVBF.DrawClonePad()
    #p_ratio_VBFBKG.DrawClonePad()
    p_ratio_VBFGGH.DrawClonePad()
    ###         HERE YOU CAN CHOOSE THE 2ND RATIO PAD         ###
    # Stick title of the plot
    plot1.cd()
    pad_obs = make_stackPad(0,0.07)
    pad_obs.Draw()
    pad_obs.cd()
    set_padMargin(pad_obs,0,0,0,0)
    obsPave = make_titleTag()
    obsPave.Draw()

    # Save plot
    plot1.SaveAs("plots/"+obs1+cate[cat]+"_mt.pdf")


    # Make canvas 
    plot2 = make_canvas(650,"plot1")  
    # Stick main histogram pad   
    pad_Main = make_stackPad(0.32,1.0)
    pad_Main.Draw()
    pad_Main.cd()
    set_padMargin(p_ratio_DataMC,0.18,0.05,0.0,0.2)
    p_histoStack.DrawClonePad()
    p_histoStack.SetTitle("")    
    #main_SMH.Draw("esame HIST")
    main_ggH.Draw("esame HIST")
    main_VBF.Draw("esame HIST")
    legend = add_legendEntryMain(0,1,1,0,0,cat)
    legend.Draw()
    # Stick ratio Data/MC
    plot2.cd()
    pad_DataMC = make_stackPad(0.10,0.32)
    pad_DataMC.Draw()
    pad_DataMC.cd()
    p_ratio_DataMC.DrawClonePad()
    # Stick title of the plot
    plot2.cd()
    pad_obs = make_stackPad(0,0.1)
    pad_obs.Draw()
    pad_obs.cd()
    set_padMargin(pad_obs,0,0,0,0)
    obsPave = make_titleTag()
    obsPave.Draw()

    # Save plot
    plot2.SaveAs("plots/basic"+obs1+cate[cat]+"_mt.pdf")



    '''
    # ratio[2] : Sig/Bkg
    p_ratio_SigBkg = make_canvas(150,"p_ratio_SigBkg")
    p_ratio_SigBkg.cd()
    p_ratio_SigBkg.SetGridy()
    h_ratio_SigBkg = make_dividedHisto(SMH,error,0,0,off,"Sig/Bkg",0)
    h_ratio_SigBkg.Draw("e0p")
    set_padMargin(p_ratio_SigBkg,0.18,0.05,0.0,0.0)
    print "ratio[2] : Sig/sqrt(S+B) pad is made."
    # ratio[3] : VBF/ggH
    p_ratio_VBFggH = make_canvas(150,"p_ratio_VBFggH")
    p_ratio_VBFggH.cd()
    p_ratio_VBFggH.SetGridy()
    h_ratio_VBFggH = make_dividedHisto(VBF,ggH,0,0,off,"VBF/ggH",1)
    h_ratio_VBFggH.Draw("e0p")
    set_padMargin(p_ratio_VBFggH,0.18,0.05,0.0,0.2)
    print "ratio[3] : VBF/ggH pad is made."


    off = 0.44
    # ratio[4] : WH/ZH
    p_ratio_WHZH = make_canvas(150,"p_ratio_WHZH")
    p_ratio_WHZH.cd()
    p_ratio_WHZH.SetGridy()
    h_ratio_WHZH = make_dividedHisto(WH,ZH,0,0,off,"WH/ZH",0)
    h_ratio_WHZH.GetYaxis().SetLabelSize(0.175)
    h_ratio_WHZH.GetYaxis().SetTitleSize(0.175)    
    h_ratio_WHZH.Draw("e0p")
    set_padMargin(p_ratio_WHZH,0.18,0.05,0.0,0.0)
    print "ratio[4] : WH/ZH pad is made."
    # ratio[5] : (WH+ZH)/(VBF+ggH) 
    p_ratio_VHsep1 = make_canvas(150,"p_ratio_VHsep1")
    p_ratio_VHsep1.cd()
    p_ratio_VHsep1.SetGridy()
    h_VH = make_sig(cat,"WH125",0,1,1)
    h_ZH = make_sig(cat,"ZH125",0,1,1)
    h_VH.Add(h_ZH,1)
    h_Vg = make_sig(cat,"VBF125",0,1,1)
    h_ggH = make_sig(cat,"ggH125",0,1,1)
    h_Vg.Add(h_ggH,1)    
    h_ratio_VHsep1 = make_dividedHisto(h_VH,h_Vg,0,0,off,"VH/(Sig-VH) ",0)
    h_ratio_VHsep1.GetYaxis().SetLabelSize(0.175)
    h_ratio_VHsep1.GetYaxis().SetTitleSize(0.175)    
    h_ratio_VHsep1.Draw("e0p")
    set_padMargin(p_ratio_VHsep1,0.18,0.05,0.0,0.0)
    print "ratio[5] : (WH+ZH)/(VBF+ggH) pad is made."
    # ratio[6] : (WH+ZH)/(VBF+ggH+bkg) 
    p_ratio_VHsep2 = make_canvas(150,"p_ratio_VHsep2")
    p_ratio_VHsep2.cd()
    p_ratio_VHsep2.SetGridy()
    h_allbutVH = h_Vg.Clone()
    h_allbutVH.Add(error,1)    
    h_ratio_VHsep2 = make_dividedHisto(h_VH,h_allbutVH,0,0,off,"VH/(others) ",1)
    h_ratio_VHsep2.Draw("e0p")
    h_ratio_VHsep2.GetXaxis().SetLabelSize(0.165)  
    h_ratio_VHsep2.GetYaxis().SetLabelSize(0.165)
    h_ratio_VHsep2.GetYaxis().SetTitleOffset(off*1.075)
    h_ratio_VHsep2.GetYaxis().SetTitleSize(0.165)    
    set_padMargin(p_ratio_VHsep2,0.18,0.05,0.0,0.2)
    print "ratio[6] : (WH+ZH)/(VBF+ggH+bkg) pad is made."

    off = 0.43
    # for all pads in plot3
    h_sqrtAll = compute_SensitivityDeno(h_all,cat)
    # ratio[7] : sensitivity - S = SMH
    p_ratio_senSMH = make_canvas(150,"p_ratio_senSMH")
    p_ratio_senSMH.cd()
    p_ratio_senSMH.SetGridy()
    h_ratio_senSMH = make_dividedHisto(make_sig(cat,"SMH",0,1,1),h_sqrtAll,0,0,off,"sen(SMH)",0)
    h_ratio_senSMH.Draw("e0p")
    set_padMargin(p_ratio_senSMH,0.18,0.05,0.0,0.0)
    print "ratio[7] : sensitivity - S = SMH pad is made."
    # ratio[8] : sensitivity - S = VBF
    p_ratio_senVBF = make_canvas(150,"p_ratio_senVBF")
    p_ratio_senVBF.cd()
    p_ratio_senVBF.SetGridy()
    h_ratio_senVBF = make_dividedHisto(make_sig(cat,"VBF125",0,1,1),h_sqrtAll,0,0,off,"sen(VBF)",0)
    h_ratio_senVBF.Draw("e0p")
    set_padMargin(p_ratio_senVBF,0.18,0.05,0.0,0.0)
    print "ratio[8] : sensitivity - S = VBF pad is made."
    # ratio[9] : sensitivity - S = ggH
    p_ratio_senggH = make_canvas(150,"p_ratio_senggH")
    p_ratio_senggH.cd()
    p_ratio_senggH.SetGridy()
    h_ratio_senggH = make_dividedHisto(make_sig(cat,"ggH125",0,1,1),h_sqrtAll,0,0,off,"sen(ggH)",1)
    h_ratio_senggH.Draw("e0p")
    set_padMargin(p_ratio_senggH,0.18,0.05,0.0,0.2)
    print "ratio[9] : sensitivity - S = ggH pad is made."



    ########################################################################
    ##                                                                    ##
    ##  Plot[1]                                                           ##
    ##  1. Main histogram                                                 ##
    ##  2. Data/MC                                                        ##
    ##  3. S/B            : To see discriminating power btw sig and bkg   ##
    ##  4. VBF/ggH        : To see if we can separate VBF and ggH         ##
    ##                                                                    ##
    ########################################################################

    # Make canvas
    plot1 = make_canvas(1000,"plot1")
    # Stick main histogram pad
    plot1.cd()
    pad_Main = make_stackPad(0.5,1.0)
    pad_Main.Draw()
    pad_Main.cd()
    p_histoStack.DrawClonePad()    
    main_SMH.Draw("esame HIST")
    main_ggH.Draw("esame HIST")
    main_VBF.Draw("esame HIST")
    legend = add_legendEntryMain(1,1,1,0,0,cat)
    legend.Draw()
    # Stick ratio Data/MC
    plot1.cd()
    pad_DataMC = make_stackPad(0.36,0.5)
    pad_DataMC.Draw()
    pad_DataMC.cd()
    p_ratio_DataMC.DrawClonePad()
    # Stick ratio S/sqrt(S+B)
    plot1.cd()
    pad_SigBkg = make_stackPad(0.22,0.36)
    pad_SigBkg.Draw()
    pad_SigBkg.cd()
    p_ratio_SigBkg.DrawClonePad() 
    # Stick ratio VBF/ggH
    plot1.cd()
    pad_VBFggH = make_stackPad(0.05,0.22)
    pad_VBFggH.Draw()
    pad_VBFggH.cd()
    p_ratio_VBFggH.DrawClonePad() 
    # Stick title of the plot
    plot1.cd()
    pad_obs = make_stackPad(0,0.06)
    pad_obs.Draw()
    pad_obs.cd()
    set_padMargin(pad_obs,0,0,0,0)
    obsPave = make_titleTag()
    obsPave.Draw()

    # Save plot
    plot1.SaveAs("plots/general_"+cate[cat]+".pdf")
    

    

    ################################################################################
    ##                                                                            ##
    ##  Plot[2]                                                                   ##
    ##  1. Main histogram                                                         ##
    ##  2. Signal histogram                                                       ##
    ##  3. WH/ZH                  : To make sure if WH and ZH are similar         ##
    ##  4. (WH+ZH)/(VBF+ggH)      : To see separation between VH and (VBF+ggH)    ##
    ##  5. (WH+ZH)/(VBF+ggH+Bkg)  : To see separation between VH and others       ##
    ##                                                                            ##
    ################################################################################

    # Make canvas
    plot2 = ROOT.TCanvas("plot2","",0,0,480,800)
    #plot2 = make_canvas(2000,"plot2")
    # Stick main histogram pad
    plot2.cd()
    pad_Main = make_stackPad(0.6,1.0)
    pad_Main.Draw()
    pad_Main.cd()
    p_histoStack.DrawClonePad()    
    main_SMH.Draw("esame HIST")
    legend = add_legendEntryMain(1,0,0,0,0,cat)
    legend.Draw()
    # Stick Signal histogram
    plot2.cd()
    pad_Signal = make_stackPad(0.40,0.6)
    pad_Signal.Draw()
    pad_Signal.cd()
    p_signal.DrawClonePad()
    # Stick ratio WH/ZH
    plot2.cd()
    pad_WHZH = make_stackPad(0.29,0.40)
    pad_WHZH.Draw()
    pad_WHZH.cd()
    p_ratio_WHZH.DrawClonePad()
    # Stick ratio VH/(VBF+ggH)
    plot2.cd()
    pad_VHsep1 = make_stackPad(0.18,0.29)
    pad_VHsep1.Draw()
    pad_VHsep1.cd()
    p_ratio_VHsep1.DrawClonePad() 
    # Stick ratio VH/(VBF+ggH+Bkg)
    plot2.cd()
    pad_VHsep2 = make_stackPad(0.05,0.18)
    pad_VHsep2.Draw()
    pad_VHsep2.cd()
    p_ratio_VHsep2.DrawClonePad() 
    # Stick title of the plot
    plot2.cd()
    pad_obs = make_stackPad(0,0.055)
    pad_obs.Draw()
    pad_obs.cd()
    set_padMargin(pad_obs,0,0,0,0)
    obsPave = make_titleTag()
    obsPave.Draw()

    # Save plot
    plot2.SaveAs("plots/VHsep_"+cate[cat]+".pdf")
    

    

    ###########################################################################
    ##                                                                       ##
    ##  Plot[3]                                                              ##
    ##  - Sensitivity of siganl S/sqrt(S+B) with diff S and B definitions    ##
    ##    Where B ise all but each S                                         ##
    ##  1. Main histogram                                                    ##
    ##  2. S: SMH                                                            ##
    ##  3. S: VBF                                                            ##
    ##  4. S: ggH                                                            ##
    ##                                                                       ##
    ###########################################################################

    # Make canvas
    plot3 = make_canvas(1000,"plot3")
    # Stick main histogram pad
    plot3.cd()
    pad_Main = make_stackPad(0.50,1.0)
    pad_Main.Draw()
    pad_Main.cd()
    p_histoStack.DrawClonePad()    
    main_SMH.Draw("esame HIST")
    main_ggH.Draw("esame HIST")
    main_VBF.Draw("esame HIST")
    legend = add_legendEntryMain(1,1,1,0,0,cat) 
    legend.Draw()
    # Stick sensitivity - S : SMH
    plot3.cd()
    pad_senSMH = make_stackPad(0.36,0.5)
    pad_senSMH.Draw()
    pad_senSMH.cd()
    p_ratio_senSMH.DrawClonePad()
    # Stick sensitivity - S : SMH
    plot3.cd()
    pad_senVBF = make_stackPad(0.22,0.36)
    pad_senVBF.Draw()
    pad_senVBF.cd()
    p_ratio_senVBF.DrawClonePad()
    # Stick sensitivity - S : SMH
    plot3.cd()
    pad_senggH = make_stackPad(0.05,0.22)
    pad_senggH.Draw()
    pad_senggH.cd()
    p_ratio_senggH.DrawClonePad() 
    # Stick title of the plot
    plot3.cd()
    pad_obs = make_stackPad(0,0.06)
    pad_obs.Draw()
    pad_obs.cd()
    set_padMargin(pad_obs,0,0,0,0)
    obsPave = make_titleTag()
    obsPave.Draw()

    # Save plot
    plot3.SaveAs("plots/sensitivity_"+cate[cat]+".pdf")
    

    
    '''
