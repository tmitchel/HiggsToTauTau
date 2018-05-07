#!/usr/bin/env python

import ROOT as r
from glob import glob
from pprint import pprint
from optparse import OptionParser
import math

parser = OptionParser()
parser.add_option('--dir', '-d', action='store',
                  dest='in_dir', default='output',
                  help='directory for input files'
                  )
parser.add_option('--var', '-v', action='store',
                  dest='var', default='msv',
                  help='variable to plot'
                  )
(options, args) = parser.parse_args()

r.gStyle.SetOptStat(0)
def formatCanvas(can):
    r.gStyle.SetOptStat(0)
    # gStyle.SetNdivisions(405, "x")
    can.Draw()
    can.Divide(1, 2)

    pad1 = can.cd(1)
    pad1.cd()
    pad1.SetPad(0, .3, 1, 1)
    pad1.SetTopMargin(.1)
    pad1.SetBottomMargin(0.02)
    pad1.SetLogy()
    pad1.SetTickx(1)
    pad1.SetTicky(1)

    pad2 = can.cd(2)
    pad2.SetPad(0, 0, 1, .3)
    pad2.SetTopMargin(0.06)
    pad2.SetBottomMargin(0.35)
    pad2.SetTickx(1)
    pad2.SetTicky(1)

    can.cd(1)


infiles = [ifile for ifile in glob(options.in_dir+'/*')]

temp = r.TFile(infiles[0], 'read')
fout = r.TFile('hist_output.root', 'recreate')
htemplate = temp.Get(options.var).Clone()
htemplate.Reset()
dytemp = htemplate.Clone()
dytemp.SetLineColor(90)
dytemp.SetFillColor(90)
tttemp = htemplate.Clone()
tttemp.SetLineColor(8)
tttemp.SetFillColor(8)
sttemp = htemplate.Clone()
sttemp.SetLineColor(r.kBlue)
sttemp.SetFillColor(r.kBlue)
wjtemp = htemplate.Clone()
wjtemp.SetLineColor(r.kMagenta+1)
wjtemp.SetFillColor(r.kMagenta+1)
sitemp = htemplate.Clone()
sitemp.SetLineColor(r.kCyan)
sitemp.SetFillColor(0)
ottemp = htemplate.Clone()
ottemp.SetLineColor(r.kRed)
ottemp.SetFillColor(r.kRed)
qcdtemp = htemplate.Clone()
qcdtemp.SetLineColor(r.kOrange+1)
qcdtemp.SetFillColor(r.kOrange+1)
datahist = htemplate.Clone()
datahist.SetMarkerStyle(20)
datahist.SetLineColor(r.kBlack)
datahist.SetFillColor(0)

samples = {}
stat = htemplate.Clone()
for ifile in infiles:
    fin = r.TFile(ifile, 'read')
    fout.cd()
    ihist = fin.Get(options.var).Clone()
    temp = htemplate.Clone()
    if 'drellYan' in ifile:
        samples.setdefault('drellYan', dytemp)
        samples['drellYan'].Add(ihist)
    elif 'ttbar' in ifile:
        samples.setdefault('ttbar', tttemp)
        samples['ttbar'].Add(ihist)
    elif 'singleTop' in ifile:
        samples.setdefault('singleTop', sttemp)
        samples['singleTop'].Add(ihist)
    elif 'wjets' in ifile:
        samples.setdefault('wjets', wjtemp)
        samples['wjets'].Add(ihist)
    elif 'QCD' in ifile:
        samples.setdefault('QCD', qcdtemp)
        samples['QCD'].Add(ihist)
    elif 'SMH_VBF125' in ifile or 'SMH_ggH125' in ifile:
        sitemp.Add(ihist)
        continue
    elif 'data' in ifile:
        datahist.Add(ihist)
        continue
    else:
        temp.SetLineColor(r.kRed)
        temp.SetFillColor(r.kRed)
        samples.setdefault('other', ottemp)
        samples['other'].Add(ihist)
    stat.Add(ihist)

samples = sorted(samples.iteritems(), key=lambda (n,hist) : hist.Integral())

can = r.TCanvas('can', 'can', 800, 600)
formatCanvas(can)
stack = r.THStack()
for sample in samples:
    if not 'sig' in sample[0]:
        stack.Add(sample[1])

stat.SetMarkerStyle(0)
stat.SetLineWidth(2)
stat.SetLineColor(0)
stat.SetFillStyle(3004)
stat.SetFillColor(r.kBlack)

stack.Draw('hist')
stack.GetXaxis().SetLabelSize(0)
stack.GetYaxis().SetTitle('Events / Bin')
stack.GetYaxis().SetTitleFont(42)
stack.GetYaxis().SetTitleSize(.05)
stack.GetYaxis().SetTitleOffset(.72)

pull = datahist.Clone()
pull.Add(stat, -1)
for ibin in range(pull.GetNbinsX()):
    pullContent = pull.GetBinContent(ibin)
    uncertainty = math.sqrt(pow(pull.GetBinErrorUp(ibin), 2)+pow(datahist.GetBinErrorUp(ibin), 2))
    if uncertainty > 0:
        pull.SetBinContent(ibin, pullContent/uncertainty)
    else:
        pull.SetBinContent(ibin, 1000)

pull.SetTitle('')
pull.SetMaximum(2.8)
pull.SetMinimum(-2.8)
pull.SetFillColor(r.kGray+1)
pull.SetLineColor(r.kGray+1)
pull.GetXaxis().SetTitle('P_{T} [GeV]')
pull.GetXaxis().SetTitleSize(0.15)
pull.GetXaxis().SetTitleOffset(0.8)
pull.GetXaxis().SetLabelFont(42)
pull.GetXaxis().SetLabelSize(.1)

pull.GetYaxis().SetTitle('#frac{Data - Bkg.}{Total unc.}')
pull.GetYaxis().SetTitleSize(0.12)
pull.GetYaxis().SetTitleFont(42)
pull.GetYaxis().SetTitleOffset(.32)
pull.GetYaxis().SetLabelSize(.17)
pull.GetYaxis().SetNdivisions(505)

line1 = r.TLine(0, 2., 500, 2.)
line1.SetLineWidth(1)
line1.SetLineStyle(7)
line1.SetLineColor(r.kBlack)
# line1.Draw()

line2 = r.TLine(0, -2., 500, -2.)
line2.SetLineWidth(1)
line2.SetLineStyle(7)
line2.SetLineColor(r.kBlack)
# line2.Draw()

leg = r.TLegend(0.5,0.42,0.88,0.88)
leg.SetTextSize(0.045)
leg.SetLineColor(0)
leg.SetFillColor(0)
leg.AddEntry(datahist, "35.9 fb^{-1} Data", 'lep')
leg.AddEntry(dytemp, "Drell-Yan", "f")
leg.AddEntry(tttemp, 't#bar{t}', 'f')
leg.AddEntry(wjtemp, 'Wjets', 'f')
leg.AddEntry(sttemp, 'Single Top', 'f')
leg.AddEntry(ottemp, 'Other', 'f')
leg.AddEntry(qcdtemp, 'QCD', 'f')
leg.AddEntry(stat, 'Background Stat. Uncertainty', 'f')
leg.AddEntry(sitemp, 'Signal', 'l')

stack.SetMaximum(stack.GetMaximum()*5000000)
stack.SetMinimum(1)
sitemp.Draw('hist same')
datahist.Draw('same lep')
stat.Draw('same e2')
leg.Draw()

ll = r.TLatex()
ll.SetNDC(r.kTRUE)
ll.SetTextSize(0.06)
ll.SetTextFont(42)
ll.DrawLatex(0.69,0.92, "35.9 fb^{-1} (13 TeV)");

cms = r.TLatex()
cms.SetNDC(r.kTRUE)
cms.SetTextFont(61)
cms.SetTextSize(0.1)
cms.DrawLatex(0.14, 0.75,"CMS")

prel = r.TLatex()
prel.SetNDC(r.kTRUE)
prel.SetTextFont(52)
prel.SetTextSize(0.1)
prel.DrawLatex(0.25,0.75,"Preliminary")

can.cd(2)
pull.Draw('hist')
line1.Draw('same')
line2.Draw('same')

can.SaveAs(options.var+'.pdf')
