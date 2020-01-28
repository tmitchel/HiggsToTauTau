import numpy
import pandas
import uproot


settings = {
    'D0_VBF': {
        'sig_weight': 'wt_vbf_a1',
        'bkg_weight': 'wt_vbf_a3',
        'sig_variable': 'ME_sm_VBF',
        'bkg_variable': 'ME_ps_VBF',
    },
    'D0_ggH': {
        'sig_weight': 'wt_ggh_a1',
        'bkg_weight': 'wt_ggh_a3',
        'sig_variable': 'ME_sm_ggH',
        'bkg_variable': 'ME_ps_ggH',
    },
}

def main(args):
    variables = ['evtwt', 'is_signal', 'njets', 'mjj']
    setting = settings[args.variable]
    sig_weight = setting['sig_weight']
    sig_variable = setting['sig_variable']
    bkg_weight = setting['bkg_weight']
    bkg_variable = setting['bkg_variable']
    branches = [sig_weight, bkg_weight, sig_variable, bkg_variable]

    # open files
    signal = uproot.open(args.signal)[args.tree_name].arrays(variables + branches, outputtype=pandas.DataFrame)
    background = uproot.open(args.background)[args.tree_name].arrays(variables + branches, outputtype=pandas.DataFrame)

    # store branch with signal vs background
    signal['signal_sample'] = numpy.ones(len(signal))
    background['signal_sample'] = numpy.zeros(len(background))

    # add JHU weights
    signal['final_weight'] = signal['evtwt']
    background['final_weight'] = background['evtwt']

    original_data = pandas.concat([signal, background])

    # make vbf region selections
    data = original_data[(original_data['is_signal'] > 0) & (original_data['njets'] > 1) & (original_data['mjj'] > 300)]

    # calculate new variables
    results = pandas.DataFrame({'alpha': [], 'ratio': []})
    for i in range(0, 500):
        alpha = (i * 0.01) + 0.01
        d0_vbf = data[sig_variable] / (data[sig_variable] + alpha * data[bkg_variable])
        numerator = (data[(d0_vbf > 0.5) & (data['signal_sample'] == 1)]['final_weight'].sum() / data[(data['signal_sample'] == 1)]['final_weight'].sum()) 
        denominator = (data[(d0_vbf <= 0.5) & (data['signal_sample'] == 0)]['final_weight'].sum() / data[(data['signal_sample'] == 0)]['final_weight'].sum())
        results = pandas.concat([results, pandas.DataFrame({'alpha': [alpha], 'ratio': [abs(1 - (numerator / denominator))]})])

    print (results[results['ratio'] == results['ratio'].min()])

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('-v', '--variable', required=True, help='variable to optimize')
    parser.add_argument('-s', '--signal', required=True, help='signal sample')
    parser.add_argument('-b', '--background', required=True, help='background sample')
    parser.add_argument('-t', '--tree-name', required=True, help='name of TTree')
    main(parser.parse_args())
