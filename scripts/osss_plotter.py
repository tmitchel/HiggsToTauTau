import ROOT
from pprint import pprint
from collections import namedtuple
ROOT.gROOT.SetBatch(ROOT.kTRUE)
ROOT.gStyle.SetOptStat(0)


# short-hand
GetColor = ROOT.TColor.GetColor
black = ROOT.kBlack
no_color = 0

style_map_tuple = namedtuple('style_map_tuple', [
    'fill_color', 'line_color', 'line_style', 'line_width', 'marker_style'
])
style_map = {
    "data_obs": style_map_tuple(no_color, black, 1, 1, 8),
    "backgrounds": {
        "embedded": style_map_tuple(GetColor("#f9cd66"), black, 1, 1, 1),
        "ZTT": style_map_tuple(GetColor("#f9cd66"), black, 1, 1, 1),
        "QCD": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        "W": style_map_tuple(GetColor("#ff0000"), black, 1, 1, 1),
        "ZJ": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        "TTJ": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        "VVJ": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        #"jetFakes": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        "TTT": style_map_tuple(GetColor("#cfe87f"), black, 1, 1, 1),
        "ZL": style_map_tuple(GetColor("#de5a6a"), black, 1, 1, 1),
        "VVT": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
    },
    "signals": {
        "ggh125_powheg": style_map_tuple(no_color, no_color, 0, 0, 1),  # don't show powheg
        "MG__GGH2Jets_sm_M125": style_map_tuple(no_color, GetColor("#0000FF"), 1, 3, 1),
        "MG__GGH2Jets_pseudoscalar_M125": style_map_tuple(no_color, GetColor("#00AAFF"), 1, 3, 1),
        # use JHU for 2018 because MG isn't available
        # "JHU__GGH2Jets_sm_M125": style_map_tuple(no_color, GetColor("#0000FF"), 1, 3, 1),
        # "JHU__GGH2Jets_pseudoscalar_M125": style_map_tuple(no_color, GetColor("#00AAFF"), 1, 3, 1),

        "vbf125_powheg": style_map_tuple(no_color, no_color, 0, 0, 1),  # don't show powheg
        "JHU__reweighted_qqH_htt_0PM125": style_map_tuple(no_color, GetColor("#FF0000"), 1, 3, 1),
        "JHU__reweighted_qqH_htt_0M125": style_map_tuple(no_color, GetColor("#ff5e00"), 1, 3, 1),
    }
}


def ApplyStyle(ihist, styles):
    ihist.SetFillColor(styles.fill_color)
    ihist.SetLineColor(styles.line_color)
    ihist.SetLineStyle(styles.line_style)
    ihist.SetLineWidth(styles.line_width)
    ihist.SetMarkerStyle(styles.marker_style)
    return ihist


def createCanvas():
    can = ROOT.TCanvas('can', 'can', 432, 451)
    can.Divide(2, 1)

    pad1 = can.cd(1)
    pad1.SetLeftMargin(.12)
    pad1.cd()
    pad1.SetPad(0, .3, 1, 1)
    pad1.SetTopMargin(.1)
    pad1.SetBottomMargin(0.02)
#    pad1.SetLogy()
    pad1.SetTickx(1)
    pad1.SetTicky(1)

    pad2 = can.cd(2)
    pad2.SetLeftMargin(.12)
    pad2.SetPad(0, 0, 1, .3)
    pad2.SetTopMargin(0.06)
    pad2.SetBottomMargin(0.35)
    pad2.SetTickx(1)
    pad2.SetTicky(1)

    can.cd(1)
    return can

def formatStat(stat):
    stat.SetMarkerStyle(0)
    stat.SetLineWidth(2)
    stat.SetLineColor(0)
    stat.SetFillStyle(3004)
    stat.SetFillColor(ROOT.kBlack)
    return stat

def formatStack(stack):
    stack.GetXaxis().SetLabelSize(0)
    stack.GetYaxis().SetTitle('Events / Bin')
    stack.GetYaxis().SetTitleFont(42)
    stack.GetYaxis().SetTitleSize(.05)
    stack.GetYaxis().SetTitleOffset(1.2)
    stack.SetTitle('')
    stack.GetXaxis().SetNdivisions(505)


def fillLegend(data, backgrounds, signals, stat):
    leg = ROOT.TLegend(0.5, 0.45, 0.85, 0.85)
    leg.SetLineColor(0)
    leg.SetFillColor(0)
    leg.SetTextFont(61)
    leg.SetTextFont(42)
    leg.SetTextSize(0.045)

    # data
    leg.AddEntry(data, 'Data', 'lep')

    # signals
    leg.AddEntry(signals['MG__GGH2Jets_sm_M125'], 'ggH SM Higgs(125)x50', 'l')
    leg.AddEntry(signals['MG__GGH2Jets_pseudoscalar_M125'], 'ggH PS Higgs(125)x50', 'l')
    # leg.AddEntry(signals['JHU__GGH2Jets_sm_M125'], 'ggH SM Higgs(125)x50', 'l')
    # leg.AddEntry(signals['JHU__GGH2Jets_pseudoscalar_M125'], 'ggH PS Higgs(125)x50', 'l')

    leg.AddEntry(signals['JHU__reweighted_qqH_htt_0PM125'], 'VBF SM Higgs(125)x50', 'l')
    leg.AddEntry(signals['JHU__reweighted_qqH_htt_0M125'], 'VBF PS Higgs(125)x50', 'l')
    
    # backgrounds
    leg.AddEntry(backgrounds['ZTT'], 'ZTT', 'f')
    leg.AddEntry(backgrounds['ZL'], 'ZL', 'f')
    leg.AddEntry(backgrounds['QCD'], 'QCD', 'f')
    leg.AddEntry(backgrounds['W'], 'W', 'f')
    leg.AddEntry(backgrounds['TTT'], 'TTT', 'f')
    leg.AddEntry(backgrounds['VVT'], 'VVT', 'f')

    # stat. uncertainty
    leg.AddEntry(stat, 'Uncertainty', 'f')
    
    return leg

def formatPull(pull, title):
    pull.SetTitle('')
    pull.SetMaximum(1.3)
    pull.SetMinimum(0.7)
    pull.GetXaxis().SetTitle(title)
    pull.SetMarkerStyle(21)
    pull.GetXaxis().SetTitleSize(0.18)
    pull.GetXaxis().SetTitleOffset(0.8)
    pull.GetXaxis().SetTitleFont(42)
    pull.GetXaxis().SetLabelFont(42)
    pull.GetXaxis().SetLabelSize(.111)
    pull.GetXaxis().SetNdivisions(505)
    # pull.GetXaxis().SetLabelSize(0)
    # pull.GetXaxis().SetTitleSize(0)

    pull.GetYaxis().SetTitle('Obs. / Exp.')
    pull.GetYaxis().SetTitleSize(0.12)
    pull.GetYaxis().SetTitleFont(42)
    pull.GetYaxis().SetTitleOffset(.475)
    pull.GetYaxis().SetLabelSize(.12)
    pull.GetYaxis().SetNdivisions(204)
    return pull

def sigmaLines(data):
    low = data.GetBinLowEdge(1)
    high = data.GetBinLowEdge(data.GetNbinsX()) + \
        data.GetBinWidth(data.GetNbinsX())

    ## high line
    line1 = ROOT.TLine(low, 1.2, high, 1.2)
    line1.SetLineWidth(1)
    line1.SetLineStyle(3)
    line1.SetLineColor(ROOT.kBlack)

    ## low line
    line2 = ROOT.TLine(low, 0.8, high, 0.8)
    line2.SetLineWidth(1)
    line2.SetLineStyle(3)
    line2.SetLineColor(ROOT.kBlack)

    return line1, line2

def blindData(data, signal, background):
    for ibin in range(data.GetNbinsX()+1):
        sig = signal.GetBinContent(ibin)
        bkg = background.GetBinContent(ibin)
        if bkg > 0 and sig / ROOT.TMath.Sqrt(bkg + pow(0.09*bkg, 2)) >= .3:
            err = data.GetBinError(ibin)
            data.SetBinContent(ibin, -1)
            data.SetBinError(ibin, err)

    # if args.var == 'NN_disc':
    #     middleBin = data.FindBin(0.5)
    #     for ibin in range(middleBin, data.GetNbinsX()+1):
    #         data.SetBinContent(ibin, 0)

    return data

def BuildPlot(args):
    ifile = ROOT.TFile(args.input)
    category = ifile.Get(args.category)
    variable = category.Get(args.variable)

    # start getting histograms
    data_hist = variable.Get('data_obs').Clone()
    signals = {}
    backgrounds = {}

    # loop through histograms to read and store to dict
    for hkey in variable.GetListOfKeys():
        hname = hkey.GetName()
        ihist = variable.Get(hname).Clone()
        if hname in style_map['backgrounds']:
            ihist = ApplyStyle(ihist, style_map['backgrounds'][hname])
            backgrounds[hname] = ihist
        elif hname in style_map['signals']:
            ihist = ApplyStyle(ihist, style_map['signals'][hname])
            signals[hname] = ihist
            
    # now get stat and stack filled
    stat = data_hist.Clone() # sum of all backgrounds
    stat.Reset()
    stack = ROOT.THStack() # stack of all backgrounds
    for bkg in sorted(backgrounds.itervalues(), key = lambda hist: hist.Integral()):
        stat.Add(bkg)
        stack.Add(bkg)

    sig_yields = [ihist.GetMaximum() for ihist in signals.itervalues()] + [data_hist.GetMaximum(), stat.GetMaximum()]
    stack.SetMaximum(max(sig_yields) * args.scale)
    
    # format the plots
    can = createCanvas()
    data_hist = ApplyStyle(data_hist, style_map['data_obs'])
    stat = formatStat(stat)
    stack.Draw('hist')
    formatStack(stack)

    # combo_signal = signals['MG__GGH2Jets_pseudoscalar_M125'].Clone()
    combo_signal = signals['MG__GGH2Jets_pseudoscalar_M125'].Clone()
    combo_signal.Reset()
    combo_signal.Add(signals['ggh125_powheg'])
    combo_signal.Add(signals['vbf125_powheg'])
    data_hist = blindData(data_hist, combo_signal, stat)

    # draw the plots
    data_hist.Draw('same lep')
    stat.Draw('same e2')
    # for sig_name, sig_hist in signals.iteritems():
    #     if 'GGH' in sig_name:
    #         sig_hist.Scale(50*signals['ggh125_powheg'].Integral()/sig_hist.Integral())
    #     if 'qqH' in sig_name:
    #         sig_hist.Scale(50*signals['vbf125_powheg'].Integral()/sig_hist.Integral())
    #     sig_hist.Draw('same hist')
    

    legend = fillLegend(data_hist, backgrounds, signals, stat)
    legend.Draw('same')

    # do some printing on the canvas
    ll = ROOT.TLatex()
    ll.SetNDC(ROOT.kTRUE)
    ll.SetTextSize(0.06)
    ll.SetTextFont(42)
    if 'et_' in args.category:
        lepLabel = "e#tau_{e}"
    elif 'mt_' in args.category:
        lepLabel = "#mu#tau_{#mu}"
    if args.year == '2016':
        lumi = "35.9 fb^{-1}"
    elif args.year == '2017':
        lumi = "41.5 fb^{-1}"
    elif args.year == '2018':
        lumi = "59.7 fb^{-1}"

    ll.DrawLatex(0.42, 0.94, "{} {}, {} (13 TeV)".format(lepLabel, args.year, lumi))

    cms = ROOT.TLatex()
    cms.SetNDC(ROOT.kTRUE)
    cms.SetTextFont(61)
    cms.SetTextSize(0.09)
    cms.DrawLatex(0.16, 0.8, "CMS")

    prel = ROOT.TLatex()
    prel.SetNDC(ROOT.kTRUE)
    prel.SetTextFont(52)
    prel.SetTextSize(0.06)
    prel.DrawLatex(0.16, 0.74, "Preliminary")

    if args.category == 'et_inclusive' or args.category == 'mt_inclusive':
        catName = 'Inclusive'
    elif args.category == 'et_0jet' or args.category == 'mt_0jet':
        catName = '0-Jet'
    elif args.category == 'et_boosted' or args.category == 'mt_boosted':
        catName = 'Boosted'
    elif args.category == 'et_vbf' or args.category == 'mt_vbf':
        catName = 'VBF Category'
    else:
        catName = ''

    lcat = ROOT.TLatex()
    lcat.SetNDC(ROOT.kTRUE)
    lcat.SetTextFont(42)
    lcat.SetTextSize(0.06)
    lcat.DrawLatex(0.16, 0.68, catName)

    # now work on ratio plot
    can.cd(2)
    ratio = data_hist.Clone()
    ratio.Divide(stat)
    ratio = formatPull(ratio, args.label)
    rat_unc = ratio.Clone()
    for ibin in range(1, rat_unc.GetNbinsX()+1):
        rat_unc.SetBinContent(ibin, 1)
        rat_unc.SetBinError(ibin, ratio.GetBinError(ibin))
    rat_unc = formatStat(rat_unc)
    # rat_unc.SetMarkerSize(0)
    # rat_unc.SetMarkerStyle(8)

    # rat_unc.SetFillColor(ROOT.kGray)
    rat_unc.Draw('same e2')
    ratio.Draw('same lep')

    line1, line2 = sigmaLines(data_hist)
    line1.Draw()
    line2.Draw()

    # save the pdf
    can.SaveAs('Output/plots/{}_{}_{}_{}.pdf'.format(args.prefix, args.variable, args.category, args.year))



if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, action='store', help='input file')
    parser.add_argument('--year', '-y', required=True, action='store', help='year to plot')
    parser.add_argument('--category', '-c', required=True, action='store', help='category to plot')
    parser.add_argument('--variable', '-v', required=True, action='store', help='variable to plot')
    parser.add_argument('--label', '-l', required=True, action='store', help='label for plot')
    parser.add_argument('--prefix', '-p', action='store', help='prefix for output name')
    parser.add_argument('--scale', '-s', default=1.2, type=float, action='store', help='scale max by x')
    BuildPlot(parser.parse_args())
