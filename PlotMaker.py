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
    dest='bins', default=[50, 0, 100], nargs='+', 
    help='[N bins, low, high]'
    )
args = parser.parse_args()

from ROOT import TFile, TH1F, TCanvas, THStack, kRed, kGreen, kYellow, kBlue, kBlack
from glob import glob

def applyStyle(name, hist):
    overlay = 0
    if name == 'embed':
        hist.SetFillColor(kRed)
    elif name == 'TTT':
        hist.SetFillColor(kGreen)
    elif name == 'TTJ':
        hist.SetFillColor(kGreen+2)
    elif name == 'VV':
        hist.SetFillColor(kYellow)
    elif name == 'W':
        hist.SetFillColor(kBlue)
    elif name == 'Data':
        hist.SetLineColor(kBlack)
        overlay = 1
    elif name == 'VBF125':
        hist.SetLineColor(kBlue+2)
        overlay = 2
    else:
        return None, 0
    return hist, overlay

def fillHist(ifile, hist):
    tfile = TFile(ifile, 'read')
    tree = tfile.Get('etau_tree')
    name = ifile.split('/')[-1].split('.root')[0]
    from array import array
    var = array('f', [0])
    weights = array('f', [0])
    cat = array('i', [0])
    tree.SetBranchAddress(args.var, var)
    tree.SetBranchAddress('evtwt', weights)
    tree.SetBranchAddress('cat_'+args.cat, cat)
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        if cat[0] > 0:
            hist.Fill(var[0], weights[0])
    hist, overlay = applyStyle(name, hist)
    if overlay == 0 and hist != None:
        hist.SetName(name)

    return hist, overlay

files = [ifile for ifile in glob('output/et_newtrees/*')]
data = TH1F('data', 'data', args.bins[0], args.bins[1], args.bins[2])
sig = TH1F('signal', 'signal', args.bins[0], args.bins[1], args.bins[2])

stack = THStack()
for ifile in files:
    # hist.Reset()
    hist = TH1F(args.var, args.var, args.bins[0], args.bins[1], args.bins[2])
    name = ifile.split('/')[-1].split('.root')[0]
    hist, overlay = fillHist(ifile, hist) 
    if overlay == 0:
        stack.Add(hist)
    elif overlay == 1:
        data = hist
    elif overlay == 1:
        sig = hist


can = TCanvas()
stack.Draw('hist')
data.Draw('same lep')
sig.Draw('same hist')
can.SaveAs('hi.pdf')
