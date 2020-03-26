import os
import sys
import pwd


def submit_command(jobName, job_configs, dryrun=False):
    print "Begin submitting skims..."

    head_dir = '/nfs_scratch/{}/{}'.format(
        pwd.getpwuid(os.getuid())[0], jobName)
    if os.path.exists(head_dir):
        raise Exception('Submission directory exists for {}.'.format(jobName))

    exe_dir = '{}/executables'.format(head_dir)
    os.system('mkdir -p {}'.format(exe_dir))

    config_dir = '{}/configs'.format(head_dir)
    os.system('mkdir -p {}'.format(config_dir))

    config_name = '{}/config.jdl'.format(config_dir)
    condorConfig = '''universe = vanilla
Executable = overlord.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Output = sleep_\$(Cluster)_\$(Process).stdout
Error = sleep_\$(Cluster)_\$(Process).stderr
Log = sleep_\$(Cluster)_\$(Process).log
x509userproxy = $ENV(X509_USER_PROXY)
Queue {}
    '''.format(len(job_configs))
    with open(config_name, 'w') as file:
        file.write(condorConfig)

    print 'Condor config has been written: {}'.format(config_name)

    overlord_name = '{}/overlord.sh'.format(exe_dir)
    overloardScript = '''#!/bin/bash
let "sample=${{1}}"
bash exe_${{sample}}.sh
    '''
    with open(overlord_name, 'w') as file:
        file.write(overloardScript)

    print 'Condor overlord has been written: {}'.format(overlord_name)

    # create tarball with user code
    os.system('tar --exclude="Output" --exclude="logs" -czf ana_code.tar.gz *')
    os.system('mv -v ana_code.tar.gz {}'.format(config_dir))

    bashScriptSetup = '''#!/bin/bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc700
eval `scramv1 project CMSSW CMSSW_10_4_0`
cd CMSSW_10_4_0/src
eval `scramv1 runtime -sh`
cp -rv {}/ana_code.tar.gz .
tar xvzf ana_code.tar.gz
eval `scram b` \n
    '''.format(config_dir)

    i = 0
    for config in job_configs:
        # create the bash config script
        bash_name = '{}/{}_{}_{}.sh'.format(exe_dir, config['sample'], config['name'], config['syst'])
        bashScript = bashScriptSetup + config['command'] + '\n'
        bashScript += 'mkdir -p /hdfs/store/{}/{}/{} \n'.format(pwd.getpwuid(os.getuid())[0], jobName, config['syst'])
        bashScript += 'cp -v *_output.root /hdfs/store/{}/{}/{} \n'.format(
            pwd.getpwuid(os.getuid())[0], jobName, config['syst'])
        with open(bash_name, 'w') as file:
            file.write(bashScript)
        os.system('chmod +x {}'.format(bash_name))
        i += 1

    print 'All executables have been written.'
    if not dryrun:
        print 'Now submitting to condor...'
        os.system('condor_submit {}'.format(config_name))

    return


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(
        description="Run the desired analyzer on FSA n-tuples")
    parser.add_argument('-dr', '--dryrun', action='store_true',
                        help='Create jobs but dont submit')
    parser.add_argument('-j', '--job', action='store', help='job type')
    parser.add_argument('-l', '--lepton', action='store', help='which lepton')
    parser.add_argument('-y', '--year', action='store', help='which year')
    parser.add_argument('-r', '--recoil', action='store', help='recoil type')
    parser.add_argument('-jn', '--jobName', nargs='?', type=str,
                        const='', help='Job Name for condor submission')
    parser.add_argument('-sn', '--samplename', nargs='?',
                        type=str, const='', help='Name of samples')
    parser.add_argument('-sd', '--sampledir', nargs='?',
                        type=str, const='', help='The Sample Input directory')
    args = parser.parse_args()
    main(args)
