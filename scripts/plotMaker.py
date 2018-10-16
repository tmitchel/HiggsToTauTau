from argparse import ArgumentParser

parser = ArgumentParser(description='script to produce stacked plots')
parser.add_argument('--var', '-v', action='store',
    dest='var', default='el_pt', 
    help='name of variable to plot'
    )
parser.add_argument('--cat', '-c', action='store',
    dest='cat', default='vbf', 
    help='name of category to pull from'
    )
parser.add_argument('--bins', '-b', action='store', 
    dest='bins', default=[50, 0, 100], nargs='+', type=int,
    help='[N bins, low, high]'
    )
parser.add_argument('--template', '-t', action='store_true',
    dest='template', default=False,
    help='create template'
    )
parser.add_argument('--plotOff', '-p', action='store_true',
    dest='plotOff', default=False,
    help='turn off plotting'
)
args = parser.parse_args()

from ROOT import TFile, TLegend, TH1F, TCanvas, THStack, kBlack, TColor, TLatex, kTRUE, TMath, TLine, gStyle
from glob import glob
gStyle.SetOptStat(0)

def applyStyle(name, hist, leg):
    overlay = 0
    if name == 'embed':
        hist.SetFillColor(TColor.GetColor("#ffcc66"))
        leg.AddEntry(hist, 'ZTT', 'f')
    elif name == 'ZL':
        hist.SetFillColor(TColor.GetColor("#4496c8"))
        leg.AddEntry(hist, 'ZL', 'f')
    elif name == 'ZJ':
        hist.SetFillColor(TColor.GetColor("#33ff11"))
        leg.AddEntry(hist, 'TTJ', 'f')
    elif name == 'TTT':
        hist.SetFillColor(TColor.GetColor("#9999cc"))
        leg.AddEntry(hist, 'TTT', 'f')
    elif name == 'TTJ':
        hist.SetFillColor(TColor.GetColor("#9999cc"))
        leg.AddEntry(hist, 'TTJ', 'f')
    elif name == 'VV':
        hist.SetFillColor(TColor.GetColor("#12cadd"))
        leg.AddEntry(hist, 'Diboson', 'f')
    elif name == 'W':
        hist.SetFillColor(TColor.GetColor("#de5a6a"))
        leg.AddEntry(hist, 'W+jets', 'f')
    elif name == 'QCD':
        hist.SetFillColor(TColor.GetColor("#ffccff"))
        leg.AddEntry(hist, 'QCD', 'f')
    elif name == 'Data':
        hist.SetLineColor(kBlack)
        overlay = 1
        leg.AddEntry(hist, 'Data', 'lep')
    elif name == 'VBF125':
        hist.SetFillColor(0)
        hist.SetLineWidth(3)
        hist.SetLineColor(TColor.GetColor('#000000'))
        hist.SetLineStyle(7)
        overlay = 2
        leg.AddEntry(hist, 'VBF M=125GeV', 'l')
    # elif name == 'ggH125':
    #     hist.SetFillColor(0)
    #     hist.SetLineWidth(3)
    #     hist.SetLineColor(TColor.GetColor('#000000'))
    #     hist.SetLineStyle(7)
    #     overlay = 2
    #     leg.AddEntry(hist, 'ggH M=125GeV', 'l')
    else:
        return None, -1 
    return hist, overlay

def fillQCD(hist, name, var, weight):
    if name == 'Data':
        hist.Fill(var, weight)
    elif name == 'embed' or name == 'ZL' or name == 'ZJ' or name == 'TTT' or name == 'TTJ' or name == 'W' or name == 'VV':
        hist.Fill(var, -1*weight)
    return hist

def fillHist(ifile, incat, leg, qcd_OS, qcd_SS, qcd_nom):
    nomCatName = 'cat_' + incat
    antiisoCatName = 'cat_antiiso_' + incat
    qcdCatName = 'cat_qcd_' + incat
    name = ifile.split('/')[-1].split('.root')[0]
    hist = TH1F(name, name, args.bins[0], args.bins[1], args.bins[2])
    tfile = TFile(ifile, 'read')
    tree = tfile.Get('etau_tree')
    from array import array
    var, weights, hpt, tq, eq = array('f', [0]), array('f', [0]), array('f', [0]), array('f', [0]), array('f', [0])
    nom_cat, antiiso_cat, qcd_cat = array('i', [0]), array('i', [0]), array('i', [0])

    tree.SetBranchAddress(args.var, var)
    tree.SetBranchAddress(nomCatName, nom_cat)
    tree.SetBranchAddress(antiisoCatName, antiiso_cat)
    tree.SetBranchAddress(qcdCatName, qcd_cat)
    tree.SetBranchAddress('evtwt', weights)
    tree.SetBranchAddress('higgs_pT', hpt)
    tree.SetBranchAddress('el_charge', eq)
    tree.SetBranchAddress('t1_charge', tq)

    for i in range(tree.GetEntries()):
        tree.GetEntry(i)

        ## get OS/SS ratio
        if antiiso_cat[0] > 0:
            if eq[0] + tq[0] == 0:
                qcd_OS = fillQCD(qcd_OS, name, var[0], weights[0])
            else:
                qcd_SS = fillQCD(qcd_SS, name, var[0], weights[0])


        if eq[0] + tq[0] == 0:
            if nomCatName == 'cat_vbf' and hpt[0] < 50:
                continue
            if nom_cat[0] > 0:
                hist.Fill(var[0], weights[0])
            if qcd_cat[0] > 0:
                qcd_nom = fillQCD(qcd_nom, name, var[0], weights[0])


    hist, overlay = applyStyle(name, hist, leg)
    if overlay == 0 and hist != None:
        hist.SetName(name)

    return hist, leg, overlay, qcd_OS, qcd_SS, qcd_nom

def createCanvas():
    can = TCanvas()
    can.Divide(2, 1)
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
    't1_pt': 'Tau p_{T} [GeV]',
    'met': 'Missing E_{T} [GeV]',
    'pt_sv': 'SVFit p_{T} [GeV]',
    'm_sv': 'SVFit Mass [GeV]',
    'mjj': 'Dijet Mass [GeV]',
    'Dbkg_VBF': 'MELA VBF Disc',
    'Dbkg_ggH': 'MELA ggH Disc',
    'NN_disc': 'NN Disc.',
    'Q2V1': '',
    'Q2V2': '',
    'Phi': '',
    'Phi1': '',
    'costheta1': '',
    'costheta2': '',
    'costhetastar': '',
}

def formatStack(stack):
    stack.GetXaxis().SetLabelSize(0)
    stack.GetYaxis().SetTitle('Events / Bin')
    stack.GetYaxis().SetTitleFont(42)
    stack.GetYaxis().SetTitleSize(.05)
    stack.GetYaxis().SetTitleOffset(.92)
    stack.SetTitle('')

def createLegend():
    leg = TLegend(0.65, 0.45, 0.85, 0.85)
    leg.SetLineColor(0)
    leg.SetFillColor(0)
    leg.SetTextFont(61)
    leg.SetTextFont(42)
    leg.SetTextSize(0.045)
    return leg

def formatPull(pull):
    pull.SetTitle('')
    pull.SetMaximum(2.8)
    pull.SetMinimum(-2.8)
    pull.SetFillColor(TColor.GetColor('#bbbbbb'))
    pull.SetLineColor(TColor.GetColor('#bbbbbb'))
    pull.GetXaxis().SetTitle(titles[args.var])
    pull.GetXaxis().SetTitleSize(0.18)
    pull.GetXaxis().SetTitleOffset(0.8)
    pull.GetXaxis().SetTitleFont(42)
    pull.GetXaxis().SetLabelFont(42)
    pull.GetXaxis().SetLabelSize(.111)
    pull.GetXaxis().SetNdivisions(505)

    pull.GetYaxis().SetTitle('#frac{Data - Bkg}{Uncertainty}')
    pull.GetYaxis().SetTitleSize(0.16)
    pull.GetYaxis().SetTitleFont(42)
    pull.GetYaxis().SetTitleOffset(.251)
    pull.GetYaxis().SetLabelSize(.12)
    pull.GetYaxis().SetNdivisions(505)
    return pull

def sigmaLines(data):
    low = data.GetBinLowEdge(1)
    high = data.GetBinLowEdge(data.GetNbinsX()) + data.GetBinWidth(data.GetNbinsX())

    ## high line
    line1 = TLine(low, 2., high, 2.)
    line1.SetLineWidth(1)
    line1.SetLineStyle(7)
    line1.SetLineColor(kBlack)

    ## low line
    line2 = TLine(low, -2., high, -2.)
    line2.SetLineWidth(1)
    line2.SetLineStyle(7)
    line2.SetLineColor(kBlack)

    return line1, line2

def createQCD(qcd, OS, SS):
    qcd.Scale(1.06 * SS.Integral()/OS.Integral())
    for ibin in range(qcd.GetNbinsX()):
        if qcd.GetBinContent(ibin) < 0:
            qcd.SetBinContent(ibin, 0)
    qcd.SetFillColor(TColor.GetColor("#ffccff"))
    return qcd

if __name__ == "__main__":
    files = [ifile for ifile in glob('output/*') if '.root' in ifile]
    files.insert(0, files.pop(files.index('output/Data.root')))
    files.insert(-1, files.pop(files.index('output/VBF125.root')))
    data = TH1F('data', 'data', args.bins[0], args.bins[1], args.bins[2])
    sig = TH1F('signal', 'signal', args.bins[0], args.bins[1], args.bins[2])
    stat = TH1F('stat', 'stat', args.bins[0], args.bins[1], args.bins[2])
    qcd_OS = TH1F('qcd_OS', 'qcd_OS', args.bins[0], args.bins[1], args.bins[2])
    qcd_SS = TH1F('qcd_SS', 'qcd_SS', args.bins[0], args.bins[1], args.bins[2])
    qcd_nom = TH1F('qcd_nom', 'qcd_nom', args.bins[0], args.bins[1], args.bins[2])

    leg = createLegend()
    stack = THStack()
    hists = []
    for ifile in files:
        hist, leg, overlay, qcd_OS, qcd_SS, qcd_nom = fillHist(ifile, args.cat, leg, qcd_OS, qcd_SS, qcd_nom)
        if overlay == 0:
            stack.Add(hist)
            stat.Add(hist)
        elif overlay == 1:
            data = hist
        elif overlay == 2:
            sig = hist
        
        if hist != None:
            hists.append(hist.Clone())

    qcd = createQCD(qcd_nom, qcd_OS, qcd_SS)
    print qcd.Integral()

    if args.template:
        fout = TFile('templates/temp_'+args.var+'_'+args.cat+'.root', 'recreate')
        for ihist in hists:
            ihist.SetName(ihist.GetName()+'_'+args.var)
            ihist.Write()
        qcd.SetName('QCD_'+args.var)
        qcd.Write()
        fout.Close()
    
    if not args.plotOff:
        can = createCanvas()
        stat = formatStat(stat)
        leg.AddEntry(qcd, 'QCD', 'f')
        stack.Add(qcd)
        stat.Add(qcd)
        high = max(data.GetMaximum(), stat.GetMaximum()) * 1.2

        stack.SetMaximum(high)
        stack.Draw('hist')
        formatStack(stack)
        data.Draw('same lep')
        stat.Draw('same e2')
        # sig.Scale(50)
        sig.Draw('same hist')
        leg.Draw()

        ll = TLatex()
        ll.SetNDC(kTRUE)
        ll.SetTextSize(0.06)
        ll.SetTextFont(42)
        ll.DrawLatex(0.69, 0.92, "35.9 fb^{-1} (13 TeV)")

        cms = TLatex()
        cms.SetNDC(kTRUE)
        cms.SetTextFont(61)
        cms.SetTextSize(0.09)
        cms.DrawLatex(0.14, 0.8, "CMS")

        prel = TLatex()
        prel.SetNDC(kTRUE)
        prel.SetTextFont(52)
        prel.SetTextSize(0.09)
        prel.DrawLatex(0.23, 0.8, "Preliminary")

        can.cd(2)
        ###########################
        ## create pull histogram ##
        ###########################
        pull = data.Clone()
        pull.Add(stat, -1)
        for ibin in range(pull.GetNbinsX()+1):
            pullContent = pull.GetBinContent(ibin)
            uncertainty = TMath.Sqrt(pow(stat.GetBinErrorUp(ibin), 2)+pow(data.GetBinErrorUp(ibin), 2))
            if uncertainty > 0:
                pull.SetBinContent(ibin, pullContent / uncertainty)
            else:
                pull.SetBinContent(ibin, 0)

        pull = formatPull(pull)
        pull.Draw('hist')

        line1, line2, = sigmaLines(data)
        line1.Draw()
        line2.Draw()

        can.SaveAs(args.var+'_'+args.cat+'.pdf')
