import ROOT
import pandas

tnames = ['data_obs', 'ZTT', 'TTT', 'VVT', 'ZL', 'jetFakes', 'vbf125_powheg', 'ggh125_powheg', 'zh125_powheg']
dnames = ['data_obs', 'EmbedZTT', 'TTT', 'VVT', 'ZL', 'jetFakes', 'qqH_htt125', 'ggHsm_htt125', 'ZH_htt125']
directories = ['mt_inclusive', 'mt_0jet', 'mt_boosted', 'mt_vbf']


def draw_plot(danny, tyler, directory, sample):
    canvas = ROOT.TCanvas()
    ROOT.gStyle.SetOptStat(0)
    danny.SetLineColor(ROOT.kRed)
    if tyler.GetMaximum() > danny.GetMaximum():
        tyler.DrawNormalized('hist e')
        danny.DrawNormalized('hist same e')
    else:
        danny.DrawNormalized('hist e')
        tyler.DrawNormalized('hist same e')

    leg = ROOT.TLegend(0.8, 0.8, 0.95, 0.95)
    leg.SetHeader(directory)
    leg.AddEntry(tyler, 'tyler', 'l')
    leg.AddEntry(danny, 'danny', 'l')
    leg.Draw()
    canvas.SaveAs('{}_{}.pdf'.format(directory, sample))


def main(args):
    tfile = ROOT.TFile(args.tyler, 'READ')
    dfile = ROOT.TFile(args.danny, 'READ')

    data = []
    for directory in directories:
        tdir = tfile.Get('{}/vis_mass'.format(directory))
        ddir = dfile.Get(directory)
        for (t, d) in zip(tnames, dnames):
            tyield = tdir.Get(t).Integral()
            dyield = ddir.Get(d).Integral()

            ratio = 0 if dyield == 0 else tyield / dyield

            data.append([directory, t, d, ratio, tyield, dyield])

            if args.plot:
                thist = tdir.Get(t).Clone()
                dhist = ddir.Get(d).Clone()
                draw_plot(thist, dhist, directory, t)

        data.append(['padding', 'padding', 'padding', 0, 0, 0])

    tfile.Close()
    dfile.Close()
    comp = pandas.DataFrame(data=data, columns=['directory', 'tname', 'dname', 'ratio', 'tyield', 'dyield'])
    print comp

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--tyler', required=True)
    parser.add_argument('--danny', required=True)
    parser.add_argument('--plot', action='store_true')
    main(parser.parse_args())