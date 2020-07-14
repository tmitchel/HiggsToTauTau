import ROOT
from pprint import pprint
from collections import namedtuple
import utils.plot_tools as plot_tools
ROOT.gROOT.SetBatch(ROOT.kTRUE)
ROOT.gStyle.SetOptStat(0)


def clean_samples(input_histos):
    merged = {
        'ZTT': input_histos['ZTT'].Clone(),
        'ZL': input_histos['ZL'].Clone(),
        'jetFakes': input_histos['jetFakes'].Clone(),
        'tt': input_histos['TTT'].Clone(),
        'Others': input_histos['STT'].Clone()
    }

    merged['tt'].Add(input_histos['TTL'])
    for name in ['STL', 'VVT', 'VVL']:
        merged['Others'].Add(input_histos[name])

    return merged


def fillLegend(data, backgrounds, signals, stat):
    """Fill the legend with appropriate processes."""
    leg = ROOT.TLegend(0.5, 0.45, 0.85, 0.85)
    leg.SetLineColor(0)
    leg.SetFillColor(0)
    leg.SetTextFont(61)
    leg.SetTextFont(42)
    leg.SetTextSize(0.045)

    # data
    leg.AddEntry(data, 'Data', 'lep')

    # signals
    leg.AddEntry(signals['ggh125_powheg'], 'ggh SM Higgs(125)x50', 'l')
    leg.AddEntry(signals['vbf125_powheg'], 'VBF SM Higgs(125)x50', 'l')

    # leg.AddEntry(signals['reweighted_ggH_htt_0PM125'], 'ggH SM Higgs(125)x50', 'l')
    # leg.AddEntry(signals['reweighted_ggH_htt_0M125'], 'ggH PS Higgs(125)x50', 'l')
    #
    # leg.AddEntry(signals['reweighted_qqH_htt_0PM125'], 'VBF SM Higgs(125)x50', 'l')
    # leg.AddEntry(signals['reweighted_qqH_htt_0M125'], 'VBF PS Higgs(125)x50', 'l')

    # backgrounds
    leg.AddEntry(backgrounds['ZTT'], 'ZTT', 'f')
    leg.AddEntry(backgrounds['ZL'], 'ZL', 'f')
    leg.AddEntry(backgrounds['jetFakes'], 'Jet Mis-ID', 'f')
    leg.AddEntry(backgrounds['tt'], 'tt', 'f')
    leg.AddEntry(backgrounds['Others'], 'Others', 'f')

    # stat. uncertainty
    leg.AddEntry(stat, 'Uncertainty', 'f')

    return leg


def BuildPlot(args):
    """
    Build the stacked plot with everything included and formatted then save as PDF.

    Variables (inside args):
    input       -- input TFile full of histograms
    category    -- which TDirectory to read
    variable    -- which variable to plot
    scale       -- value to scale the top of the plot (keep histograms from being cutoff)
    year        -- which era is this?
    label       -- LaTeX label for variable on x-axis
    prefix      -- name to attach to output file
    """
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
        if hname in plot_tools.ac_style_map['backgrounds']:
            ihist = plot_tools.ApplyStyle(ihist, plot_tools.ac_style_map['backgrounds'][hname])
            backgrounds[hname] = ihist
        elif hname in plot_tools.ac_style_map['signals']:
            ihist = plot_tools.ApplyStyle(ihist, plot_tools.ac_style_map['signals'][hname])
            signals[hname] = ihist

    # merge backgrounds
    backgrounds = clean_samples(backgrounds)

    # now get stat and stack filled
    stat = data_hist.Clone()  # sum of all backgrounds
    stat.Reset()
    stack = ROOT.THStack()  # stack of all backgrounds
    for bkg in sorted(backgrounds.itervalues(), key=lambda hist: hist.Integral()):
        stat.Add(bkg)
        stack.Add(bkg)

    sig_yields = [ihist.GetMaximum() for ihist in [signals['ggh125_powheg'], signals['vbf125_powheg']]] + \
        [data_hist.GetMaximum(), stat.GetMaximum()]
    stack.SetMaximum(max(sig_yields) * args.scale)

    # format the plots
    can = plot_tools.createCanvas()
    data_hist = plot_tools.ApplyStyle(data_hist, plot_tools.ac_style_map['data_obs'])
    stat = plot_tools.formatStat(stat)
    stack.Draw('hist')
    plot_tools.formatStack(stack)

    # combo_signal = signals['MG__GGH2Jets_pseudoscalar_M125'].Clone()
    combo_signal = signals['ggh125_powheg'].Clone()
    combo_signal.Reset()
    combo_signal.Add(signals['ggh125_powheg'])
    combo_signal.Add(signals['vbf125_powheg'])
    data_hist = plot_tools.blindData(data_hist, combo_signal, stat)

    # draw the plots
    data_hist.Draw('same lep')
    stat.Draw('same e2')
    for sig_name, sig_hist in signals.iteritems():
        if 'ggh' in sig_name:
            sig_hist.Scale(50*signals['ggh125_powheg'].Integral()/sig_hist.Integral())
        if 'vbf' in sig_name:
            sig_hist.Scale(50*signals['vbf125_powheg'].Integral()/sig_hist.Integral())
        sig_hist.Draw('same hist')

    legend = fillLegend(data_hist, backgrounds, signals, stat)
    legend.Draw('same')

    # do some printing on the canvas
    ll = ROOT.TLatex()
    ll.SetNDC(ROOT.kTRUE)
    ll.SetTextSize(0.06)
    ll.SetTextFont(42)
    if 'et_' in args.category:
        lepLabel = "#tau_{e}#tau_{h}"
    elif 'mt_' in args.category:
        lepLabel = "#tau_{#mu}#tau_{h}"
    elif 'tt_' in args.category:
        lepLabel = "#tau_{h}#tau_{h}"
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

    if args.category == 'et_inclusive' or args.category == 'mt_inclusive' or args.category == 'tt_inclusive':
        catName = 'Inclusive'
    elif args.category == 'et_0jet' or args.category == 'mt_0jet' or args.category == 'tt_0jet':
        catName = '0-Jet'
    elif args.category == 'et_boosted' or args.category == 'mt_boosted' or args.category == 'tt_boosted':
        catName = 'Boosted'
    elif args.category == 'et_vbf' or args.category == 'mt_vbf' or args.catagory == 'tt_vbf':
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
    ratio = plot_tools.formatPull(ratio, args.label)
    rat_unc = ratio.Clone()
    for ibin in range(1, rat_unc.GetNbinsX()+1):
        rat_unc.SetBinContent(ibin, 1)
        rat_unc.SetBinError(ibin, ratio.GetBinError(ibin))
    rat_unc = plot_tools.formatStat(rat_unc)
    # rat_unc.SetMarkerSize(0)
    # rat_unc.SetMarkerStyle(8)

    # rat_unc.SetFillColor(ROOT.kGray)
    rat_unc.Draw('same e2')
    ratio.Draw('same lep')

    line1, line2 = plot_tools.sigmaLines(data_hist, 1.2, 0.8)
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
