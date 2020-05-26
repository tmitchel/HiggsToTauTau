import ROOT
from collections import namedtuple

# short-hand
GetColor = ROOT.TColor.GetColor
black = ROOT.kBlack
no_color = 0

style_map_tuple = namedtuple('style_map_tuple', [
    'fill_color', 'line_color', 'line_style', 'line_width', 'marker_style'
])
ac_style_map = {
    "data_obs": style_map_tuple(no_color, black, 1, 1, 8),
    "backgrounds": {
        "embedded": style_map_tuple(GetColor("#f9cd66"), black, 1, 1, 1),
        "ZTT": style_map_tuple(GetColor("#f9cd66"), black, 1, 1, 1),
        "jetFakes": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        "ZL": style_map_tuple(GetColor("#de5a6a"), black, 1, 1, 1),
        "TTT": style_map_tuple(GetColor("#cfe87f"), black, 1, 1, 1),
        "TTL": style_map_tuple(GetColor("#cfe87f"), black, 1, 1, 1),
        "VVT": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
        "VVL": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
        "STT": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
        "STL": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
    },
    "signals": {
        "ggh125_powheg": style_map_tuple(no_color, no_color, 0, 0, 1),  # don't show powheg
        "reweighted_ggH_htt_0PM125": style_map_tuple(no_color, GetColor("#0000FF"), 1, 3, 1),
        "reweighted_ggH_htt_0M125": style_map_tuple(no_color, GetColor("#00AAFF"), 1, 3, 1),

        "vbf125_powheg": style_map_tuple(no_color, no_color, 0, 0, 1),  # don't show powheg
        "reweighted_qqH_htt_0PM125": style_map_tuple(no_color, GetColor("#FF0000"), 1, 3, 1),
        "reweighted_qqH_htt_0M125": style_map_tuple(no_color, GetColor("#ff5e00"), 1, 3, 1),
    }
}
boost_style_map = {
    "data_obs": style_map_tuple(no_color, black, 1, 1, 8),
    "backgrounds": {
        "embedded": style_map_tuple(GetColor("#f9cd66"), black, 1, 1, 1),
        "ZTT": style_map_tuple(GetColor("#f9cd66"), black, 1, 1, 1),
        "jetFakes": style_map_tuple(GetColor("#ffccff"), black, 1, 1, 1),
        "ZL": style_map_tuple(GetColor("#de5a6a"), black, 1, 1, 1),
        "TTT": style_map_tuple(GetColor("#cfe87f"), black, 1, 1, 1),
        "TTL": style_map_tuple(GetColor("#cfe87f"), black, 1, 1, 1),
        "VVT": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
        "VVL": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
        "STT": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
        "STL": style_map_tuple(GetColor("#9feff2"), black, 1, 1, 1),
    },
    "signals": {
        "ggh125_powheg": style_map_tuple(no_color, GetColor("#0000FF"), 1, 3, 1),  # don't show powheg
        "reweighted_ggH_htt_0PM125": style_map_tuple(no_color, GetColor("#0000FF"), 1, 3, 1),
        "reweighted_ggH_htt_0M125": style_map_tuple(no_color, GetColor("#00AAFF"), 1, 3, 1),

        "vbf125_powheg": style_map_tuple(no_color, GetColor("#FF0000"), 1, 3, 1),  # don't show powheg
    }
}


def createCanvas():
    """Build the TCanvas and split into pads."""
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


def ApplyStyle(ihist, styles):
    """Apply styling to histogram."""
    ihist.SetFillColor(styles.fill_color)
    ihist.SetLineColor(styles.line_color)
    ihist.SetLineStyle(styles.line_style)
    ihist.SetLineWidth(styles.line_width)
    ihist.SetMarkerStyle(styles.marker_style)
    return ihist


def formatStat(stat):
    """Format the statistical uncertainty histogram"""
    stat.SetMarkerStyle(0)
    stat.SetLineWidth(2)
    stat.SetLineColor(0)
    stat.SetFillStyle(3004)
    stat.SetFillColor(ROOT.kBlack)
    return stat


def formatStack(stack):
    """Format the stacked background histograms."""
    stack.GetXaxis().SetLabelSize(0)
    stack.GetYaxis().SetTitle('Events / Bin')
    stack.GetYaxis().SetTitleFont(42)
    stack.GetYaxis().SetTitleSize(.05)
    stack.GetYaxis().SetTitleOffset(1.2)
    stack.SetTitle('')
    stack.GetXaxis().SetNdivisions(505)


def formatPull(pull, title):
    """Format the pull (or ratio) histogram in the lower pad."""
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


def sigmaLines(data, high_line, low_line):
    """Draw lines on pull (or ratio) plot."""
    low = data.GetBinLowEdge(1)
    high = data.GetBinLowEdge(data.GetNbinsX()) + \
        data.GetBinWidth(data.GetNbinsX())

    # high line
    line1 = ROOT.TLine(low, high_line, high, high_line)
    line1.SetLineWidth(1)
    line1.SetLineStyle(3)
    line1.SetLineColor(ROOT.kBlack)

    # low line
    line2 = ROOT.TLine(low, low_line, high, low_line)
    line2.SetLineWidth(1)
    line2.SetLineStyle(3)
    line2.SetLineColor(ROOT.kBlack)

    return line1, line2


def blindData(data, signal, background):
    """Apply blinding procedure to data."""
    for ibin in range(data.GetNbinsX()+1):
        sig = signal.GetBinContent(ibin)
        bkg = background.GetBinContent(ibin)
        if bkg > 0 and sig / ROOT.TMath.Sqrt(bkg + pow(0.09*bkg, 2)) >= .3:
            err = data.GetBinError(ibin)
            data.SetBinContent(ibin, -1)
            data.SetBinError(ibin, err)

    return data
