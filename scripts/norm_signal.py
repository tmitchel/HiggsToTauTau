
name_SM_qqH_powheg = 'vbf125_powheg'
name_SM_qqH_JHU = 'reweighted_vbf125_a1_JHU'
name_PS_qqH_JHU = 'reweighted_vbf125_a3_JHU'
name_INT_qqH_JHU = 'reweighted_vbf125_a3int_JHU'
desired_SM_qqH_name = 'reweighted_qqH_htt_0PM125'
desired_PS_qqH_name = 'reweighted_qqH_htt_0M125'
desired_INT_qqH_name = 'reweighted_qqH_htt_0Mf05ph0125'

name_SM_WH_powheg = 'wh125_powheg'
name_SM_WH_JHU = 'reweighted_wh125_a1_JHU'
desired_SM_WH_name = 'reweighted_WH_htt_0PM125'
desired_PS_WH_name = 'reweighted_WH_htt_0M125'
desired_INT_WH_name = 'reweighted_WH_htt_0Mf05ph0125'

name_SM_ZH_powheg = 'zh125_powheg'
name_SM_ZH_JHU = 'reweighted_zh125_a1_JHU'
desired_SM_ZH_name = 'reweighted_ZH_htt_0PM125'
desired_PS_ZH_name = 'reweighted_ZH_htt_0M125'
desired_INT_ZH_name = 'reweighted_ZH_htt_0Mf05ph0125'

name_SM_ggH_powheg = 'ggh125_powheg'
name_SM_ggH_JHU = 'reweighted_ggh125_a1_JHU'
desired_SM_ggH_name = 'GGH2Jets_sm_M125'
desired_PS_ggH_name = 'GGH2Jets_pseudoscalar_M125'
desired_INT_ggH_name = 'GGH2Jets_pseudoscalar_Mf05ph0125'


def desired_names(process):
    return 'reweighted_{}_htt_0PM125'.format(process), 'reweighted_{}_htt_0M125'.format(process), 'reweighted_{}_htt_0Mf05ph0125'.format(process)


powheg_sm_processes = ['ggh125_powheg', 'vbf125_powheg', 'wh125_powheg', 'zh125_powheg']
reweight_sm_processes = [
    'JHU__GGH2Jets_sm_M125', 'JHU__reweighted_qqH_htt_0PM125',
    'JHU__reweighted_WH_htt_0PM125', 'JHU__reweighted_ZH_htt_0PM125'
]


def main(args):
    import ROOT
    import uproot
    from string import join
    fin = ROOT.TFile(args.input, 'read')
    fout = ROOT.TFile(args.output, 'recreate')
    vbf_inclusive_cat = fin.Get('{}_vbf'.format(args.channel))
    signal_yields = {}
    for sig in zip(powheg_sm_processes, reweight_sm_processes):
        key = join(sig[1].replace('JHU__', '').split('_')[:-1]).replace(' ', '_')
        print sig
        print vbf_inclusive_cat.Get(sig[0]).Integral()
        print vbf_inclusive_cat.Get(sig[1]).Integral()
        if vbf_inclusive_cat.Get(sig[1]).Integral() > 0:
            signal_yields[key] = vbf_inclusive_cat.Get(sig[0]).Integral() / vbf_inclusive_cat.Get(sig[1]).Integral()
        else:
            signal_yields[key] = 0
    print signal_yields

    for idir in fin.GetListOfKeys():
        cat = idir.GetName()
        if not args.channel in cat:
            continue

        fout.mkdir(cat)
        fout.cd(cat)
        directory = fin.Get(cat)
        for ihist in directory.GetListOfKeys():
            hname = ihist.GetName()
            hist = directory.Get(hname)
            if ('0jet' in cat or 'boosted' in cat) and 'powheg' in hname:
                # use powheg for all couplings
                if 'ggh125' in hname:
                    sm, ps, inter = desired_SM_ggH_name, desired_PS_ggH_name, desired_INT_ggH_name
                elif 'vbf125' in hname:
                    sm, ps, inter = desired_names('qqH')
                elif 'wh125' in hname:
                    sm, ps, inter = desired_names('WH')
                elif 'zh125' in hname:
                    sm, ps, inter = desired_names('ZH')
                # now write copies with new names
                hist.Write(sm)
                hist.Write(ps)
                hist.Write(inter)
            elif 'vbf' in cat and 'JHU__' in hname:
                scale_key = join(hname.replace('JHU__', '').split('_')[:-1]).replace(' ', '_')
                scale_key = scale_key.replace('pseudoscalar', 'sm')
                hist.Scale(signal_yields[scale_key])
                hist.Write(hname.replace('JHU__', ''))
            elif 'JHU' in hname or 'madgraph' in hname:
                continue
            else:
                hist.Write()
    fout.Close()


if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', action='store', help='name of input file')
    parser.add_argument('--output', '-o', action='store', help='name of output file')
    parser.add_argument('--channel', '-c', action='store', help='lepton channel')
    main(parser.parse_args())
