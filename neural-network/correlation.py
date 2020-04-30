import pandas as pd
import seaborn as sbr
import matplotlib.pyplot as plt


def main(args):
    data = pd.HDFStore(args.input)['df']
    # samples = data[(data['sample_names'] == 'TTT') | (data['sample_names'] == 'TTJ') | (data['sample_names'] == 'VVT') | (data['sample_names'] == 'VVJ') | (data['sample_names'] == 'embedded') | (data['sample_names'] == 'ZJ') | (data['sample_names'] == 'ZL') | (data['sample_names'] == 'W') ]
    samples = data[(data['sample_names'] == 'ggh_madgraph_twojet')]
    sample = samples[(samples['mjj'] < 800)]
    correlations = sample[['Q2V1', 'Q2V2', 'Phi', 'Phi1', 'costheta1', 'costheta2',
                           'costhetastar', 'mjj', 'higgs_pT', 'm_sv']].corr(method='pearson') * 100
    ax = sbr.heatmap(correlations, linewidth=0.5, annot=True)
    plt.yticks(rotation=45)
    plt.xticks(rotation=45)
    plt.savefig('corr-sig-mjjl800-vbf.png')


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', action='store',
                        dest='input', default='test.h5', help='input dataframe')
    main(parser.parse_args())
