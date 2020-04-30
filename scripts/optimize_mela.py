import numpy
import pandas
import uproot


settings = {
    'D0_VBF_a3': {
        'sig_sample': 'JHU__reweighted_qqH_htt_0PM125.root',
        'bkg_sample': 'JHU__reweighted_qqH_htt_0M125.root',
        'sig_variable': 'ME_sm_VBF',
        'bkg_variable': 'ME_ps_VBF',
    },
    'D0_VBF_a2': {
        'sig_sample': 'JHU__reweighted_qqH_htt_0PM125.root',
        'bkg_sample': 'JHU__reweighted_qqH_htt_0PH125.root',
        'sig_variable': 'ME_sm_VBF',
        'bkg_variable': 'ME_a2_VBF',
    },
    'D0_VBF_l1': {
        'sig_sample': 'JHU__reweighted_qqH_htt_0PM125.root',
        'bkg_sample': 'JHU__reweighted_qqH_htt_0L1125.root',
        'sig_variable': 'ME_sm_VBF',
        'bkg_variable': 'ME_L1_VBF',
    },
    'D0_VBF_l1zg': {
        'sig_sample': 'JHU__reweighted_qqH_htt_0PM125.root',
        'bkg_sample': 'JHU__reweighted_qqH_htt_0L1Zg125.root',
        'sig_variable': 'ME_sm_VBF',
        'bkg_variable': 'ME_L1Zg_VBF',
    },
    'D0_ggH': {
        'sig_sample': 'MG__GGH2Jets_sm_M125.root',
        'bkg_sample': 'MG__GGH2Jets_pseudoscalar_M125.root',
        'sig_variable': 'ME_sm_ggH',
        'bkg_variable': 'ME_ps_ggH',
    },
}


def parse_tree_name(keys):
    """Take list of keys in the file and search for our TTree"""
    if 'et_tree;1' in keys:
        return 'et_tree'
    elif 'mt_tree;1' in keys:
        return 'mt_tree'
    else:
        raise Exception('Can\t find et_tree or mt_tree in keys: {}'.format(keys))


def main(args):
    variables = ['evtwt', 'is_signal', 'njets', 'mjj']
    setting = settings[args.variable]
    sig = setting['sig_sample']
    bkg = setting['bkg_sample']
    sig_variable = setting['sig_variable']
    bkg_variable = setting['bkg_variable']
    branches = [sig_variable, bkg_variable]

    # open files
    keys = uproot.open('{}/{}'.format(args.input_dir, sig)).keys()
    tree_name = parse_tree_name(keys)
    signal = uproot.open('{}/{}'.format(args.input_dir, sig)
                         )[tree_name].arrays(variables + branches, outputtype=pandas.DataFrame)
    background = uproot.open('{}/{}'.format(args.input_dir, bkg)
                             )[tree_name].arrays(variables + branches, outputtype=pandas.DataFrame)

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
        if '_L1_' in bkg_variable:
            alpha *= 10000 + 1000000
        elif '_L1Zg_' in bkg_variable:
            alpha *= 10000 + 5000000

        d0_vbf = data[sig_variable] / (data[sig_variable] + alpha * data[bkg_variable])
        numerator = (data[(d0_vbf > 0.5) & (data['signal_sample'] == 0)]['final_weight'].sum() /
                     data[(data['signal_sample'] == 0)]['final_weight'].sum())
        denominator = (data[(d0_vbf <= 0.5) & (data['signal_sample'] == 1)]['final_weight'].sum() /
                       data[(data['signal_sample'] == 1)]['final_weight'].sum())
        results = pandas.concat([results, pandas.DataFrame({'alpha': [alpha], 'ratio': [abs(1 - (numerator / denominator)) if denominator > 0 else 1]})])

    print (results[results['ratio'] == results['ratio'].min()])


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('-v', '--variable', required=True, help='variable to optimize')
    parser.add_argument('-i', '--input-dir', required=True, help='directory containing files')
    main(parser.parse_args())
