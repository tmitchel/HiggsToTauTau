def main(args):
    import ROOT
    from math import floor

    fin = ROOT.TFile(args.input, 'READ')
    fout = ROOT.TFile(args.input.replace('.root', '_unrolled.root'), 'RECREATE')
    categories = [cat.GetName() for cat in fin.GetListOfKeys()]
    pref = 'unrolled'

    for cat in categories:
        fout.cd()
        fout.mkdir('{}/{}'.format(cat, pref))
        tdir = fin.Get(cat)
        histograms = [hist.GetName() for hist in tdir.GetListOfKeys()]
        for hist in histograms:
            ihist = tdir.Get(hist).Clone()
            xbins, ybins = ihist.GetNbinsX(), ihist.GetNbinsY()
            nbins = xbins * ybins
            ohist = ROOT.TH1F(hist + '_unrolled', hist, nbins, 0, nbins)
            ibin = 1
            for xbin in range(1, xbins + 1):
                for ybin in range(1, ybins + 1):
                    ohist.SetBinContent(ibin, ihist.GetBinContent(xbin, ybin))
                    ohist.SetBinError(ibin, ihist.GetBinError(xbin, ybin))
                    ibin += 1
            fout.cd('{}/{}'.format(cat, pref))
            ohist.SetName(hist)
            ohist.Write()


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', required=True, help='name of input file')
    main(parser.parse_args())
