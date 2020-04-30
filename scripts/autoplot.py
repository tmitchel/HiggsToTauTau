import plotter

plots = [
    ('m_sv', 'm_{SVFit}', 2.2),
    ('D0_VBF', 'D_{0-}^{VBF}', 2.2),
    ('D_a2_VBF', 'D_{a2}^{VBF}', 2.2),
    ('D_l1_VBF', 'D_{l1}^{VBF}', 2.2),
    ('D_l1zg_VBF', 'D_{l1zg}^{VBF}', 2.2),

    ('D0_ggH', 'D_{0-}^{ggH}', 2.2),
    ('DCP_VBF', 'D_{CP}^{VBF}', 2.2),
    ('DCP_ggH', 'D_{CP}^{ggH}', 2.2),
    ('MELA_D2j', 'D_{2jet}^{VBF}', 2.2),
    ('NN_disc', 'D_{NN}^{VBF}', 1.6),
    ('higgs_pT', 'Higgs p_{T}', 1.4),
    ('mjj', 'm_{JJ}', 1.4),
    ('met', 'Missing E_{T}', 1.4),
    ('njets', 'N(jets)', 1.4),
    ('t1_decayMode', 'Tau Decay Mode', 2.2),
    ('vis_mass', 'm_{Visible}', 2.2),
    ('t1_pt', 'Tau p_{T}', 1.4),
    ('t1_eta', 'Tau #eta', 2.2),
    ('mt', 'm_{T}', 2.2),
    ('Phi', '#Phi', 2.2),
    ('Phi1', '#Phi_{1}', 2.2),
    ('Q2V1', 'Q2V1', 1.8),
    ('Q2V2', 'Q2V2', 1.8),
    ('costheta1', 'cos(#theta_{1})', 1.4),
    ('costheta2', 'cos(#theta_{2})', 1.4),
    ('costhetastar', 'cos(#theta*)', 2.2),

    ('el_pt', 'Electron p_{T}', 1.4),
    ('el_eta', 'Electron #eta', 2.2),
    ('mu_pt', 'Muon p_{T}', 1.4),
    ('mu_eta', 'Muon #eta', 2.2),
]

categories = [ '_0jet', '_boosted', '_vbf', '_inclusive']

class PlotArgs():
    """Wrap all inputs as if they are comming from ArgumentParser"""
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
            if ('D0' in plot[0] or 'DCP' in plot[0] or 'NN' in plot[0] or 'MELA' in plot[0] or 'mjj' in plot[0] or 'costheta' in plot[0] or 'Phi' in plot[0] or 'Q2V' in plot[0]) and not 'vbf' in category:
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
