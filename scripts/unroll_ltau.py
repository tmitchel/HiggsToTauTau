def main(args):
    import ROOT
    import uproot
    from string import join
    fin = ROOT.TFile(args.input, 'read')
    fout = ROOT.TFile(args.output, 'recreate')

    for idir in fin.GetListOfKeys():
        cat = idir.GetName()

        fout.mkdir(cat)
        fout.cd(cat)
        directory = fin.Get(cat)
        for ihist in directory.GetListOfKeys():
            hname = ihist.GetName()
            hist = directory.Get(hname)
            nbinsx, nbinsy = hist.GetNbinsX(), hist.GetNbinsY()
            print cat, hname
            fout.cd(cat)
            unrolled = ROOT.TH1F(hname, hname, nbinsx * nbinsy, 0, nbinsx *nbinsy)
            for binx in xrange(nbinsx + 1):
                for biny in xrange(nbinsy + 1):
                    content = hist.GetBinContent(binx, biny)
                    error = hist.GetBinError(binx, biny)
                    unrolled.SetBinContent(binx*biny, content)
                    unrolled.SetBinError(binx*biny, error)
            # # temporary thing until i fix WH
            # if 'WH' in hname:
            #     continue
            # elif 'ZH' in hname:
            #     fout.cd(cat)
            #     unrolled.Write()
            #     unrolled.SetName(unrolled.GetName().replace('ZH', 'WH'))
            #     unrolled.Write()
            #     continue
            fout.cd(cat)
            unrolled.Write()
    fout.Close()
    fin.Close()

if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', action='store', help='name of input file')
    parser.add_argument('--output', '-o', action='store', help='name of output file')
    parser.add_argument('--channel', '-c', action='store', help='lepton channel')
    main(parser.parse_args())