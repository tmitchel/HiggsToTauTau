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

    log_dir = '{}/logs'.format(head_dir)
    os.system('mkdir -p {}'.format(log_dir))

    config_name = '{}/config.jdl'.format(config_dir)
    condorConfig = '''universe = vanilla
Executable = {2}/overlord.sh
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Requirements = (MY.RequiresSharedFS=!=true || TARGET.HasAFS_OSG) && (TARGET.OSG_major =!= undefined || TARGET.IS_GLIDEIN=?=true) && (TARGET.HasParrotCVMFS=?=true || (TARGET.CMS_CVMFS_Exists && TARGET.CMS_CVMFS_Revision >= 89991 )) && TARGET.HAS_CMS_HDFS
request_memory       = 9000
request_disk         = 2048000
request_cpus         = 1
Transfer_Input_Files = {2},{3}
Output = {1}/out_$(Cluster)_$(Process).out
Error = {1}/run_$(Cluster)_$(Process).err
Log = {1}/running_$(Cluster).log
x509userproxy = /tmp/x509up_u23269
Arguments=$(process)
Queue {0}
    '''.format(len(job_configs), log_dir, exe_dir, config_dir)
    with open(config_name, 'w') as file:
        file.write(condorConfig)

    print 'Condor config has been written: {}'.format(config_name)

    overlord_name = '{}/overlord.sh'.format(exe_dir)
    overloardScript = '''#!/bin/bash
let "sample=${{1}}"
array=($(ls executables/))
echo ./executables/"${{array[${{sample}}]}}"
bash ./executables/"${{array[${{sample}}]}}"
    '''.format(exe_dir)
    with open(overlord_name, 'w') as file:
        file.write(overloardScript)

    print 'Condor overlord has been written: {}'.format(overlord_name)

    # create tarball with user code
    os.system('tar --exclude="Output" --exclude="logs" --exclude="neural-network" --exclude="tmp" -czf ana_code.tar.gz *')
    os.system('mv -v ana_code.tar.gz {}'.format(config_dir))

    bashScriptSetup = '''#!/bin/bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
source /afs/hep.wisc.edu/cms/setup/bashrc
export SCRAM_ARCH=slc6_amd64_gcc700
scramv1 project CMSSW CMSSW_10_4_0
cd CMSSW_10_4_0/src
eval `scram runtime -sh`
cp -rv ../../configs/ana_code.tar.gz .
tar xvzf ana_code.tar.gz \n'''.format(config_dir)

    i = 0
    for config in job_configs:
        # create the bash config script
        bash_name = '{}/submit_{}_{}_{}.sh'.format(exe_dir, config['sample'], config['name'], config['syst'])
        bashScript = bashScriptSetup + config['command'] + '\n'
        bashScript += 'mkdir -p /hdfs/store/user/{}/{}/{} \n'.format(pwd.getpwuid(os.getuid())[0], jobName, config['syst'])
        bashScript += 'cp -v *_output.root /hdfs/store/user/{}/{}/{}/ \n'.format(
            pwd.getpwuid(os.getuid())[0], jobName, config['syst'])
        with open(bash_name, 'w') as file:
            file.write(bashScript)
        os.system('chmod +x {}'.format(bash_name))
        i += 1

    print 'All executables have been written.'
    if not dryrun:
        print 'Now submitting to condor...'
        # os.system('condor_submit {}'.format(config_name))

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
