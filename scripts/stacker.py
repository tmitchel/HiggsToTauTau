#!/usr/bin/env python
from argparse import ArgumentParser

parser = ArgumentParser(description='script to produce stacked plots')
parser.add_argument('--var', '-v', action='store',
                    dest='var', default='el_pt',
                    help='name of variable to plot'
                    )
parser.add_argument('--cat', '-c', action='store',
                    dest='cat', default='et_vbf',
                    help='name of category to pull from'
                    )
parser.add_argument('--year', '-y', action='store',
                    dest='year', default='2016',
                    help='year to plot'   
                    )
parser.add_argument('--input', '-i', action='store',
                    dest='input', default='',
                    help='path to input file'
                    )
parser.add_argument('--prefix', '-p', action='store',
                    dest='prefix', default='test',
                    help='prefix to add to plot name'
                    )
parser.add_argument('--scale', '-s', action='store', type=float,
                    dest='scale', default=1.,
                    help='scale top of hist by x'
                    )
args = parser.parse_args()

from ROOT import TFile, TLegend, TH1F, TCanvas, THStack, kBlack, TColor, TLatex, kTRUE, TMath, TLine, gStyle
from glob import glob
gStyle.SetOptStat(0)

from ROOT import gROOT, kTRUE
gROOT.SetBatch(kTRUE)

def applyStyle(name, hist, leg):
    overlay = 0
    print name, hist.Integral()
    if name == 'embedded' or name == 'ZTT':
        hist.SetFillColor(TColor.GetColor("#f9cd66"))
        hist.SetName("embedded")
        hist.SetFillColor(TColor.GetColor("#f9cd66"))
        hist.SetName("embedded")
    elif name == 'TTT':
        hist.SetFillColor(TColor.GetColor("#cfe87f"))
        hist.SetName('TTT')
    elif name == 'VVT' or name == 'EWKZ':
        hist.SetFillColor(TColor.GetColor("#9feff2"))
        overlay = 4
    elif name == 'ZL' or name == 'VVL' or name == 'TTL':
        hist.SetFillColor(TColor.GetColor("#de5a6a"))
        hist.SetName('ZL')
    elif name == 'QCD':
        hist.SetFillColor(TColor.GetColor("#ffccff"))
        hist.SetName('QCD')
    elif name == 'jetFakes':
        hist.SetFillColor(TColor.GetColor("#ffccff"))
        hist.SetName('jet fakes')
    elif name == 'Data' or name == 'data_obs':
        hist.SetLineColor(kBlack)
        hist.SetMarkerStyle(8)
        overlay = 1
    elif name == 'GGH2Jets_pseudoscalar_Mf05ph0125':
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#F0F000'))
        overlay = 8
    elif name == 'reweighted_qqH_htt_0PM125':
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#FF0000'))
        overlay = 2
    elif name == 'GGH2Jets_pseudoscalar_M125' or name == 'ggh_madgraph_PS_twojet':
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#00AAFF'))
        overlay = 9
    elif name.lower() == 'vbf125':
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#FF0000'))
        overlay = -2
    elif (name == 'GGH2Jets_sm_M125' or name == 'ggh_madgraph_twojet' or name == 'ggh125_madgraph_twojet_nominal_ggH125_output' or name == 'ggh125_madgraph_twojet_nominal_v1_ggH125_output'):
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#0000FF'))
        overlay = 3
    elif name == 'ggH125' or name == 'ggh125':
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#0000FF'))
        overlay = -3
    else:
        return None, -1, leg
    return hist, overlay, leg


def createCanvas():
    can = TCanvas('can', 'can', 432, 451)
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
    stat.SetFillColor(kBlack)
    return stat


titles = {
    'el_pt': 'Electron p_{T} [GeV]',
    'mu_pt': 'Muon p_{T} [GeV]',
    't1_pt': 'Tau p_{T} [GeV]',
    'el_eta': 'Electron Eta [GeV]',
    'mu_eta': 'Muon Eta [GeV]',
    't1_eta': 'Tau Eta [GeV]',
    'el_phi': 'Electron Phi [GeV]',
    'mu_phi': 'Muon Phi [GeV]',
    't1_phi': 'Tau Phi [GeV]',
    't1_iso': 'Tau Isolation',
    'mt' : 'M_{T} [GeV]',
    'vis_mass': 'M_{vis} [GeV]',
    'met': 'Missing E_{T} [GeV]',
    'metphi' : 'Missing E_{T} #Phi [GeV]',
    'pt_sv': 'SVFit p_{T} [GeV]',
    'm_sv': 'SVFit Mass [GeV]',
    'mjj': 'Dijet Mass [GeV]',
    'Dbkg_VBF': 'MELA VBF Disc',
    'Dbkg_ggH': 'MELA ggH Disc',
    'NN_disc': 'D_{NN}^{VBF}',
    'NN_disc_vbf': 'D_{NN}^{VBF}',
    'NN_disc_boost': 'D_{NN}^{VBF}',
    'Q2V1': 'Q^{2} V1 [GeV]',
    'Q2V2': 'Q^{2} V2 [GeV]',
    'Phi': '#phi',
    'Phi1': '#phi_{1}',
    'costheta1': 'Cos(#theta_{1})',
    'costheta2': 'Cos(#theta_{2})',
    'costhetastar': 'Cos(#theta*)',
    'nbjets': 'N(b-jets)',
    'nn_vbf_full': 'NN Disc.',
    'dPhi': '#Delta#phi',
    'njets': 'N(jets)',
    'j1_eta': 'Lead Jet Eta',
    'j2_eta': 'Sub-Lead Jet Eta',
    'j1_pt': 'Lead Jet p_{T} [GeV]',
    'j2_pt': 'Sub-Lead Jet p_{T} [GeV]',
    'hjj_pT': 'pT(Higgs,j1,j2) [GeV]',
    'higgs_pT': 'pT(Higgs) [GeV]',
    'VBF_MELA': 'D_{2jet}^{ggH}',
    'super': 'super',
    'dPhijj': 'dPhijj',
    'lt_dphi': 'lt_dphi',
    "MT_lepMET": 'MT_lepMET',
    "MT_HiggsMET": "MT_HiggsMET",
    "hj_dphi": "hj_dphi",
    "jmet_dphi": "jmet_dphi",
    "MT_t2MET": "MT_t2MET",
    "hj_deta": "hj_deta",
    "hmet_dphi": "hmet_dphi",
    "hj_dr": "hj_dr",
    "D0_ggH": "D_{0-}^{ggH}",
    "D0_VBF": "D_{0-}^{VBF}",
    "t1_decayMode": "t1_decayMode",
    "trigger": "trigger"

}

def formatStack(stack):
    stack.GetXaxis().SetLabelSize(0)
    stack.GetYaxis().SetTitle('Events / Bin')
    stack.GetYaxis().SetTitleFont(42)
    stack.GetYaxis().SetTitleSize(.05)
    stack.GetYaxis().SetTitleOffset(1.2)
    stack.SetTitle('')

def formatOther(other, holder):
    other.SetFillColor(TColor.GetColor("#9feff2"))
    other.SetName('Other')
    holder.append(other.Clone())
    return holder


def fillStackAndLegend(data, vbf, ggh, ggh_ps, ggh_int, holder, leg):
    stack = THStack()
    leg.AddEntry(data, 'Data', 'lep')
    leg.AddEntry(vbf, 'VBF Higgs(125)x100', 'l')
    leg.AddEntry(ggh, 'ggH Higgs(125)x100', 'l')
    # leg.AddEntry(ggh_ps, 'ggH PS Higgs(125)x100', 'l')
    # leg.AddEntry(ggh_int, 'ggH INT Higgs(125)x50', 'l')
    leg.AddEntry(filter(lambda x: x.GetName() == 'embedded', holder)[0], 'ZTT', 'f')
    # leg.AddEntry(filter(lambda x: x.GetName() == 'ZTT', holder)[0], 'ZTT', 'f')
    leg.AddEntry(filter(lambda x: x.GetName() == 'ZL', holder)[0], 'ZL', 'f')
    leg.AddEntry(filter(lambda x: x.GetName() == 'jet fakes', holder)[0], 'jetFakes', 'f')
    leg.AddEntry(filter(lambda x: x.GetName() == 'TTT', holder)[0], 'TTT', 'f')
    leg.AddEntry(filter(lambda x: x.GetName() == 'Other', holder)[0], 'Others', 'f')

    holder = sorted(holder, key=lambda hist: hist.Integral())
    for hist in holder:
        stack.Add(hist)
    return stack, leg

def createLegend():
    leg = TLegend(0.5, 0.45, 0.85, 0.85)
    leg.SetLineColor(0)
    leg.SetFillColor(0)
    leg.SetTextFont(61)
    leg.SetTextFont(42)
    leg.SetTextSize(0.045)
    return leg


def formatPull(pull):
    pull.SetTitle('')
    pull.SetMaximum(1.5)
    pull.SetMinimum(0.5)
    pull.GetXaxis().SetTitle(titles[args.var])
    pull.SetMarkerStyle(21)
    pull.GetXaxis().SetTitleSize(0.18)
    pull.GetXaxis().SetTitleOffset(0.8)
    pull.GetXaxis().SetTitleFont(42)
    pull.GetXaxis().SetLabelFont(42)
    pull.GetXaxis().SetLabelSize(.111)
    pull.GetXaxis().SetNdivisions(505)
    # pull.GetXaxis().SetLabelSize(0)
    # pull.GetXaxis().SetTitleSize(0)

    pull.GetYaxis().SetTitle('Data / MC')
    pull.GetYaxis().SetTitleSize(0.12)
    pull.GetYaxis().SetTitleFont(42)
    pull.GetYaxis().SetTitleOffset(.475)
    pull.GetYaxis().SetLabelSize(.12)
    pull.GetYaxis().SetNdivisions(204)
    return pull

# def formatPull(pull):
#     pull.SetTitle('')
#     pull.SetMaximum(2.8)
#     pull.SetMinimum(-2.8)
#     pull.SetFillColor(TColor.GetColor('#bbbbbb'))
#     pull.SetLineColor(TColor.GetColor('#bbbbbb'))
#     pull.GetXaxis().SetTitle(titles[args.var])
#     pull.GetXaxis().SetTitleSize(0.18)
#     pull.GetXaxis().SetTitleOffset(0.8)
#     pull.GetXaxis().SetTitleFont(42)
#     pull.GetXaxis().SetLabelFont(42)
#     pull.GetXaxis().SetLabelSize(.111)
#     pull.GetXaxis().SetNdivisions(505)

#     pull.GetYaxis().SetTitle('#frac{Data - Bkg}{Uncertainty}')
#     pull.GetYaxis().SetTitleSize(0.16)
#     pull.GetYaxis().SetTitleFont(42)
#     pull.GetYaxis().SetTitleOffset(.251)
#     pull.GetYaxis().SetLabelSize(.12)
#     pull.GetYaxis().SetNdivisions(505)
#     return pull


def sigmaLines(data):
    low = data.GetBinLowEdge(1)
    high = data.GetBinLowEdge(data.GetNbinsX()) + \
        data.GetBinWidth(data.GetNbinsX())

    ## high line
    line1 = TLine(low, 1.2, high, 1.2)
    line1.SetLineWidth(1)
    line1.SetLineStyle(3)
    line1.SetLineColor(kBlack)

    ## low line
    line2 = TLine(low, 0.8, high, 0.8)
    line2.SetLineWidth(1)
    line2.SetLineStyle(3)
    line2.SetLineColor(kBlack)

    return line1, line2


def blindData(data, signal, background):
    # for ibin in range(data.GetNbinsX()+1):
    #     sig = signal.GetBinContent(ibin)
    #     bkg = background.GetBinContent(ibin)
    #     if bkg > 0 and sig / TMath.Sqrt(bkg + pow(0.09*bkg, 2)) > 0.5:
    #         data.SetBinContent(ibin, 0)

    # if args.var == 'NN_disc':
    #     middleBin = data.FindBin(0.5)
    #     for ibin in range(middleBin, data.GetNbinsX()+1):
    #         data.SetBinContent(ibin, 0)

    return data

def main():
    fin = TFile(args.input, 'read')
    
    idir = fin.Get('plots/{}/{}'.format(args.var, args.cat))
    # idir = fin.Get('grabbag/triggers')
    leg = createLegend()
    # data = idir.Get('Data').Clone()
    data = idir.Get('data_obs').Clone()
    vbf = data.Clone()
    vbf.Reset()
    ggh = vbf.Clone()
    ggh_ps = vbf.Clone()
    ggh_int = vbf.Clone()
    pw_vbf = vbf.Clone()
    ph_ggh = vbf.Clone()
    allSig = vbf.Clone()
    stat = vbf.Clone()
    other = stat.Clone()
    inStack = []
    hists = [idir.Get(key.GetName()).Clone() for key in idir.GetListOfKeys()]
    for ihist in hists:
        hist, overlay, leg = applyStyle(ihist.GetName(), ihist, leg)
        if overlay == 0:
            inStack.append(hist)
            stat.Add(hist)
        elif overlay == 1:
            data = hist
        elif overlay == 2:
            vbf = hist
            allSig.Add(hist)
        elif overlay == -2:
            pw_vbf = hist
        elif overlay == 3:
            ggh = hist
            allSig.Add(hist)
        elif overlay == -3:
            pw_ggh = hist
        elif overlay == 4:
            other.Add(hist)
            stat.Add(hist)
        elif overlay == 5:
            allSig.Add(hist)
        elif overlay == 8:
            ggh_int = hist
        elif overlay == 9:
            ggh_ps = hist
    can = createCanvas()
    inStack = formatOther(other, inStack)

    # vbf.Scale(pw_vbf.Integral()/vbf.Integral())
    ggh.Scale(pw_ggh.Integral()/ggh.Integral())
    if ggh_ps.Integral() > 0:
      ggh_ps.Scale(pw_ggh.Integral()/ggh_ps.Integral())
    # if ggh_int.Integral() > 0:
    #   ggh_int.Scale(pw_ggh.Integral()/ggh_int.Integral())

    stack, leg = fillStackAndLegend(data, vbf, ggh, ggh_ps, ggh_int, inStack, leg)
    stat = formatStat(stat)
    leg.AddEntry(stat, 'Uncertainty', 'f')

    data = blindData(data, allSig, stat)

    high = max(data.GetMaximum(), stat.GetMaximum()) * args.scale
    stack.SetMaximum(high)
    stack.Draw('hist')
    formatStack(stack)
    data.Draw('same lep')
    stat.Draw('same e2')
    # vbf.Scale(100)
    # vbf.Draw('same hist e')
    pw_vbf.Scale(100)
    pw_vbf.Draw('same hist e')
    ggh.Scale(100)
    ggh.Draw('same hist e')
    # ggh_int.Scale(50)
    # ggh_int.Draw('same hist e')
    ggh_ps.Scale(100)
    ggh_ps.Draw('same hist e')
    leg.Draw()

    ll = TLatex()
    ll.SetNDC(kTRUE)
    ll.SetTextSize(0.06)
    ll.SetTextFont(42)
    if 'et_' in args.cat:
        lepLabel = "e#tau_{e}"
    elif 'mt_' in args.cat:
        lepLabel = "#mu#tau_{#mu}"
    if args.year == '2016':
        lumi = "35.9 fb^{-1}"
    elif args.year == '2017':
        lumi = "41.5 fb^{-1}"
    
    ll.DrawLatex(0.42, 0.92, "{} {}, {} (13 TeV)".format(lepLabel, args.year, lumi))

    cms = TLatex()
    cms.SetNDC(kTRUE)
    cms.SetTextFont(61)
    cms.SetTextSize(0.09)
    cms.DrawLatex(0.16, 0.8, "CMS")

    prel = TLatex()
    prel.SetNDC(kTRUE)
    prel.SetTextFont(52)
    prel.SetTextSize(0.06)
    prel.DrawLatex(0.16, 0.74, "Preliminary")

    if args.cat == 'et_inclusive' or args.cat == 'mt_inclusive':
        catName = 'Inclusive'
    elif args.cat == 'et_0jet' or args.cat == 'mt_0jet':
        catName = '0-Jet'
    elif args.cat == 'et_boosted' or args.cat == 'mt_boosted':
        catName = 'Boosted'
    elif args.cat == 'et_vbf' or args.cat == 'mt_vbf':
        catName = 'VBF enriched'
    else:
      catName = ''

    lcat = TLatex()
    lcat.SetNDC(kTRUE)
    lcat.SetTextFont(42)
    lcat.SetTextSize(0.06)
    lcat.DrawLatex(0.16, 0.68, catName)

    can.cd(2)
    ###########################
    ## create pull histogram ##
    ###########################
    # pull = data.Clone()
    # pull.Add(stat, -1)
    # for ibin in range(pull.GetNbinsX()+1):
    #     pullContent = pull.GetBinContent(ibin)
    #     uncertainty = TMath.Sqrt(pow(stat.GetBinErrorUp(ibin), 2)+pow(data.GetBinErrorUp(ibin), 2))
    #     if uncertainty > 0:
    #         pull.SetBinContent(ibin, pullContent / uncertainty)
    #     else:
    #         pull.SetBinContent(ibin, 0)
    ratio = data.Clone()
    ratio.Divide(stat)
    ratio = formatPull(ratio)
    rat_unc = ratio.Clone()
    for ibin in range(1, rat_unc.GetNbinsX()+1):
        rat_unc.SetBinContent(ibin, 1)
        rat_unc.SetBinError(ibin, ratio.GetBinError(ibin))
    rat_unc.SetMarkerSize(0)
    rat_unc.SetMarkerStyle(8)
    from ROOT import kGray
    rat_unc.SetFillColor(kGray)
    rat_unc.Draw('same e2')
    ratio.Draw('same lep')

    line1, line2 = sigmaLines(data)
    line1.Draw()
    line2.Draw()

#    can.SaveAs('Output/plots/{}_{}_{}_{}_LOG.pdf'.format(args.prefix, args.var, args.cat, args.year))
    can.SaveAs('Output/plots/{}_{}_{}_{}.pdf'.format(args.prefix, args.var, args.cat, args.year))


if __name__ == "__main__":
    main()
