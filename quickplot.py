from ROOT import *
gStyle.SetOptStat(0)

f1 = TFile('output/et_newtrees/embed.root', 'read')
f2 = TFile('output/et_newtrees/ZTT.root', 'read')

t1 = f1.Get('etau_tree')
t2 = f2.Get('etau_tree')

h1 = TH1F('h1', 'h1', 100, 0., 1000.)
h2 = TH1F('h2', 'h2', 100, 0., 1000.) 

for i in t1:
  h1.Fill(i.higgs_pT, i.evtwt) 

for i in t2:
  h2.Fill(i.higgs_pT, i.evtwt)

print 'Embed', h1.Integral()
print 'MC', h2.Integral()
print 'ratio', (h1.Integral()-h2.Integral())/h1.Integral()

can = TCanvas('can', 'can', 800, 600)
can.Draw()
can.Divide(1, 2)

pad1 = can.cd(1)
pad1.cd()
pad1.SetPad(0, .3, 1, 1)
pad1.SetTopMargin(.1)
pad1.SetBottomMargin(0.02)
# pad1.SetLogy()
pad1.SetTickx(1)
pad1.SetTicky(1)

pad2 = can.cd(2)
pad2.SetPad(0, 0, 1, .3)
pad2.SetTopMargin(0.06)
pad2.SetBottomMargin(0.35)
pad2.SetTickx(1)
pad2.SetTicky(1)

can.cd(1)

h1.Draw('hist e')
h2.SetLineColor(kRed)
h2.Draw('hist e same')

h1.GetXaxis().SetLabelSize(0)
h1.SetTitle('')

h1.GetXaxis().SetLabelSize(0)
h1.GetYaxis().SetTitle('Events / Bin')
h1.GetYaxis().SetTitleFont(42)
h1.GetYaxis().SetTitleSize(.05)
h1.GetYaxis().SetTitleOffset(.92)

leg = TLegend(.75,.75,.92,.92)
leg.AddEntry(h1, 'Embedded B-H', 'l')
leg.AddEntry(h2, 'ZTT MC', 'l')
leg.Draw()

can.cd(2)
pull = h1.Clone()
pull.Divide(h2)

pull.SetTitle('')
pull.SetMaximum(1.8)
pull.SetMinimum(0.2)
pull.SetFillColor(0)
pull.SetLineColor(kBlack)
pull.GetXaxis().SetTitle('Dijet Mass [GeV]')
pull.GetXaxis().SetTitleSize(0.15)
pull.GetXaxis().SetTitleOffset(0.8)
pull.GetXaxis().SetLabelFont(42)
pull.GetXaxis().SetLabelSize(.1)

pull.GetYaxis().SetTitle('#frac{Emb.}{MC}')
pull.GetYaxis().SetTitleSize(0.14)
pull.GetYaxis().SetTitleFont(42)
pull.GetYaxis().SetTitleOffset(.31)
pull.GetYaxis().SetLabelSize(.1)
pull.GetYaxis().SetNdivisions(505)

line_low = pull.GetBinLowEdge(1)
line_high = pull.GetBinLowEdge(
    pull.GetNbinsX())+pull.GetBinWidth(pull.GetNbinsX())

line1 = TLine(line_low, 1, line_high, 1)
line1.SetLineWidth(1)
line1.SetLineStyle(7)
line1.SetLineColor(kBlack)
pull.Draw('lep')
line1.Draw()

can.SaveAs('meh.pdf')
