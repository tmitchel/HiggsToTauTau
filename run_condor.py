from os import makedirs
from glob import glob
from pprint import pprint

def valid_sample(ifile):
    """Remove samples that aren't used any longer"""
    invalid_samples = ['EWKZ', 'EWKW', 'WW.root', 'WZ.root', 'ZZ.root', 'ggh125_madgraph_inc']
    for sample in invalid_samples:
        if sample in ifile:
            return False
    return True

def getNames(sample):
    """Return the sample names and signal type."""
    if 'DYJets' in sample:
        names = ['ZL', 'ZJ', 'ZTT']
    elif 'TT' in sample:
        names = ['TTT', 'TTJ', 'TTL']
    elif '-tW' in sample or '-tchan' in sample:
        names = ['STT', 'STL', 'STJ']
    elif 'WJets' in sample:
        names = ['W']
    elif 'EWKW' in sample:
        names = ['EWK_W']
    elif 'data' in sample:
        names = ['data_obs']
    elif 'ggh125' in sample.lower():
        names = ['ggH125']
    elif 'vbf125' in sample.lower():
        names = ['VBF125']
    elif 'wplus' in sample.lower() or 'wminus' in sample.lower():
        names = ['WHsigned125']
    elif 'wh125' in sample.lower():
        names = ['WH125']
    elif 'zh125' in sample.lower():
        names = ['ZH125']
    elif 'ttH' in sample or 'tth' in sample:
        names = ['ttH125']
    elif 'embed' in sample:
        names = ['embed']
    else:
        names = ['VVJ', 'VVT', 'VVL']

    signal_type = 'None'
    if 'JHU' in sample:
        signal_type = 'JHU'
    elif 'madgraph' in sample:
        signal_type = 'madgraph'
    elif 'minlo' in sample:
        signal_type = 'minlo'
    elif 'powheg' in sample:
        signal_type = 'powheg'

    return names, signal_type


def getSyst(name, signal_type, channel, year, doSyst):
    """Return the list of systematics to be processed for this sample.

    The list of systematics is built based on the process, signal type, and channel.
    All applicable systematics will be added to the list for processing.
    Arguments:
    name        -- name of the process
    signal_type -- signal type or None
    channel     -- name of the channel
    year        -- name of the year
    doSyst      -- if False, returns a list with just the nominal case
    Returns:
    systs       -- list of systematics to processes
    """
    systs = ['']
    if not doSyst or signal_type == 'minlo':
        return systs

    if name == 'TTT' or name == 'VVT' or name == 'embed' or name == 'ZTT' or signal_type != 'None':
        systs += ['tau_id_Up', 'tau_id_Down']  # names will probably be updated
        systs += ['DM0_Up', 'DM0_Down', 'DM1_Up', 'DM1_Down', 'DM10_Up', 'DM10_Down']

    if name == 'ZL' or name == 'W' or name == 'TTL' or name == 'VVL':
        systs += ['LES_DM0_Up', 'LES_DM0_Down', 'LES_DM1_Up', 'LES_DM1_Down']

    if name != 'embed' and name != 'data_obs':
        systs += [
            'UncMet_Up', 'UncMet_Down',
            'JetJER_Up', 'JetJER_Down',
            'JetAbsolute_Up', 'JetAbsolute_Down',
            'JetAbsolute_Up', 'JetAbsolute_Down',
            'JetAbsoluteyear_Up', 'JetAbsoluteyear_Down',
            'JetBBEC1_Up', 'JetBBEC1_Down',
            'JetBBEC1year_Up', 'JetBBEC1year_Down',
            'JetEC2_Up', 'JetEC2_Down',
            'JetEC2year_Up', 'JetEC2year_Down',
            'JetFlavorQCD_Up', 'JetFlavorQCD_Down',
            'JetHF_Up', 'JetHF_Down',
            'JetHFyear_Up', 'JetHFyear_Down',
            'JetRelBal_Up', 'JetRelBal_Down',
            'JetRelSam_Up', 'JetRelSam_Down',
            'JetRes_Up', 'JetRes_Down'
        ]
        systs += ['trigger_up', 'trigger_down']
        if '2016' in exe or '2017' in year:
            systs += ['prefiring_up', 'prefiring_down']

    if name == 'TTT' or name == 'TTJ':
        systs += ['ttbarShape_Up', 'ttbarShape_Down']

    if name == 'TTJ' or name == 'ZJ' or name == 'VVJ' or name == 'W':
        systs += ['jetToTauFake_Up', 'jetToTauFake_Down']

    if name == 'ZJ' or name == 'ZL' or name == 'ZTT':
        systs += ['dyShape_Up', 'dyShape_Down', 'zmumuShape_Up', 'zmumuShape_Down']

    if name != 'data_obs' and channel == 'et':
        systs += ['EEScale_Up', 'EEScale_Down', 'EESigma_Up', 'EESigma_Down']
    elif name != 'data_obs' and channel == 'mt':
        systs += ['MES_Up', 'MES_Down']

    # if name == 'ggH125' and signal_type == 'powheg':
    #     systs += [
    #         'Rivet0_Up', 'Rivet0_Down', 'Rivet1_Up', 'Rivet1_Down', 'Rivet2_Up', 'Rivet2_Down',
    #         'Rivet3_Up', 'Rivet3_Down', 'Rivet4_Up', 'Rivet4_Down', 'Rivet5_Up', 'Rivet5_Down',
    #         'Rivet6_Up', 'Rivet6_Down', 'Rivet7_Up', 'Rivet7_Down', 'Rivet8_Up', 'Rivet8_Down',
    #     ]

    if name == 'ZJ' or name == 'ZL' or name == 'ZTT' or name == 'ggH125' or name == 'VBF125' or name == 'W':
        systs += ['RecoilReso_Up', 'RecoilReso_Down', 'RecoilResp_Up', 'RecoilResp_Down']

    return systs


def main(args):
    """Build all processes and run them."""
    suffix = '.root'

    try:
        makedirs('/hdfs/store/user/tmitchel/{}'.format(args.jobname))
    except:
        pass

    fileList = [ifile for ifile in glob(args.path+'/*') if '.root' in ifile and valid_sample(ifile)]
    job_map = {}
    for ifile in fileList:
        sample = ifile.split('/')[-1].split(suffix)[0]
        tosample = ifile.replace(sample+suffix, '')
        names, signal_type = getNames(sample)
        for name in names:
            systs = getSyst(name, signal_type, args.channel, args.year, args.syst)
            for syst in systs:
                job_map[syst] = {
                    'path': ifile,
                    'name': name,
                    'signal_type': signal_type,
                    'syst': syst,
                }
    pprint(job_map)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--jobname', '-j', required=True, help='name for this job')
    parser.add_argument('--path', '-p', required=True, help='path to input files')
    parser.add_argument('--channel', '-c', required=True, help='channel to process')
    parser.add_argument('--year', '-y', required=True, help='year to process')
    parser.add_argument('--syst', action='store_true', help='run all systematics')
    main(parser.parse_args())
