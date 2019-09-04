import plotter

plots = [
    ('D0_VBF', 'D_{0-}^{VBF}', 2.2),
    ('D0_ggH', 'D_{CP}^{ggH}', 2.2),
    ('DCP_VBF', 'D_{0-}^{VBF}', 2.2),
    ('DCP_ggH', 'D_{CP}^{ggH}', 2.2),
    ('MELA_D2j', 'D_{2jet}^{VBF', 2.2),
    ('NN_disc', 'D_{NN}^{VBF}', 1.6),
    ('m_sv', 'm_{SVFit}', 2.2),
    ('higgs_pT', 'Higgs p_{T}', 1.4),
    ('mjj', 'm_{JJ}', 1.4),
    ('met', 'Missing E_{T}', 1.4),
    ('t1_decayMode', 'Tau Decay Mode', 2.2),
    # ('vis_mass', 'm_{Visible}', 2.2),
    ('t1_pt', 'Tau p_{T}', 1.4),
    ('t1_eta', 'Tau #eta', 2.2),

    ('el_pt', 'Electron p_{T}', 1.4),
    ('el_eta', 'Electron #eta', 2.2),
    ('mu_pt', 'Muon p_{T}', 1.4),
    ('mu_eta', 'Muon #eta', 2.2),
]

categories = [ '_0jet', '_boosted', '_vbf']

class PlotArgs():
    def __init__(self, input, year, category, variable, label, prefix, scale):
        self.input = input
        self.year = year
        self.category = category
        self.variable = variable
        self.label = label
        self.prefix = prefix
        self.scale = scale

def main(args):
    for plot in plots:
        for category in categories:
            if ('D0' in plot[0] or 'DCP' in plot[0] or 'NN' in plot[0] or 'MELA' in plot[0] or 'mjj' in plot[0]) and not 'vbf' in category:
                continue
            if 'el' in plot[0] and args.channel == 'mt':
                continue
            elif 'mu' in plot[0] and args.channel == 'et':
                continue
            inputs = PlotArgs(
                args.input,
                args.year,
                args.channel + category,
                plot[0],
                plot[1],
                args.prefix,
                plot[2]
            )
            plotter.BuildPlot(inputs)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--prefix', '-p', required=True, help='prefix for files')
    parser.add_argument('--year', '-y', required=True, help='year to process')
    parser.add_argument('--channel', '-c', required=True, help='channel to plot')
    main(parser.parse_args())