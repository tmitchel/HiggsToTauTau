import ROOT
from array import array

def clean(pair):
    if pair[0] <= 0:
        pair[0] = 0
    if pair[1] <= 0:
        pair[1] = 0
    return pair

def reorder(sig, bkg):
    if sig.GetNbinsX() != bkg.GetNbinsX():
        raise Exception('Histograms must have the same binning')

    bins = [[sig.GetBinContent(ibin), bkg.GetBinContent(ibin)]
            for ibin in range(sig.GetNbinsX() + 2)]
    clean_bins = map(clean, bins)
    with_r = [(s/b, s, b) if b > 0 else (1e10, s, b) for (s, b) in clean_bins]
    sorted_bins = sorted(with_r, key=lambda r: r[0])

    newSig = ROOT.TH1F('signal', '', sig.GetNbinsX(), 0, 1)
    newBkg = ROOT.TH1F('background', '', sig.GetNbinsX(), 0, 1)

    for ibin in xrange(0, newSig.GetNbinsX() + 2):
        newSig.SetBinContent(ibin, sorted_bins[ibin][1])
        newBkg.SetBinContent(ibin, sorted_bins[ibin][2])

    return newSig, newBkg

def main(args):
    temp_file = ROOT.TFile(args.inputs[0], 'READ')
    template = temp_file.Get('{}/{}/{}'.format(args.channel, args.variable, args.sig[0])).Clone()
    template.Reset()
    signal = template.Clone()
    background = template.Clone()

    can = ROOT.TCanvas()

    frame = ROOT.TH2F("frame","Mu-Tau Channel",1000,0.,1,1000,0,1);
    frame.GetXaxis().SetTitle(args.x_label);
    frame.GetYaxis().SetTitle(args.y_label);
    ROOT.gStyle.SetOptStat(0);
    can.cd()
    frame.Draw(); 
    leg = ROOT.TLegend(0.21, 0.41, 0.41, 0.89);
    leg.SetBorderSize(0);

    graphs = []
    i = 1
    for ifile in args.inputs:
        reading = ROOT.TFile(ifile, 'READ')
        for sig in args.sig:
            hist = reading.Get('{}/{}/{}'.format(args.channel, args.variable, sig)).Clone()
            signal.Add(hist)
        for bkg in args.bkg:
            hist = reading.Get('{}/{}/{}'.format(args.channel, args.variable, bkg)).Clone()
            background.Add(hist)

        siggy, backy = reorder(signal, background)
        backy.SetLineColor(ROOT.kBlue)
        siggy.SetLineColor(ROOT.kRed)

        bkg_yield = backy.Integral(-1, backy.GetNbinsX() + 1)
        sig_yield = siggy.Integral(-1, backy.GetNbinsX() + 1)

        fr = [backy.Integral(ibin, backy.GetNbinsX() + 1) / bkg_yield for ibin in range(0, backy.GetNbinsX()+1)]
        eff = [siggy.Integral(ibin, siggy.GetNbinsX() + 1) / sig_yield for ibin in range(0, siggy.GetNbinsX()+1)]

        fr_array = array('f', fr)
        eff_array = array('f', eff)

        graph = ROOT.TGraph(siggy.GetNbinsX() + 1, eff_array, fr_array)
        graph.SetMarkerColor(i)
        graph.SetLineColor(ROOT.kBlack)
        graph.SetMarkerSize(0.5)
        graph.SetMarkerStyle(21)
        graphs.append(graph)
        leg.AddEntry(graph, 'Era: {}'.format(2015+i), 'pl')
        i += 1

    for graph in graphs:
        graph.Draw('P same')
        can.Update()
    leg.Draw()
    can.Update()
    can.SaveAs('ROC_mt_{}.pdf'.format(args.variable))

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--inputs', required=True, nargs='+', help='path to input files')
    parser.add_argument('--bkg', required=True, nargs='+', help='backgrounds')
    parser.add_argument('--sig', required=True, nargs='+', help='signals')
    parser.add_argument('--channel', required=True, help='channel to process')
    parser.add_argument('--variable', required=True, help='variable to process')
    parser.add_argument('--x-label', required=True)
    parser.add_argument('--y-label', required=True)
    
    main(parser.parse_args())