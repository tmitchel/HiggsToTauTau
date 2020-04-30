import os
import sys
import pwd


def write_bash_script(commands, output_sample_name, dag_dir):
    bash_name = '{}/{}.sh'.format(dag_dir+'inputs', output_sample_name)
    bashScript = '#!/bin/bash\n'
    for command in commands:
        bashScript += command
        bashScript += '\n'
    with open(bash_name, 'w') as file:
        file.write(bashScript)
    os.system('chmod +x {}'.format(bash_name))
    return bash_name



def default_farmout(jobName, input_name, output_dir, bash_name, submit_dir, dag_dir, filesperjob):
    farmoutString = 'farmoutAnalysisJobs --infer-cmssw-path --fwklite --input-file-list=%s' % (
        input_name)
    farmoutString += ' --submit-dir=%s --output-dag-file=%s --output-dir=%s' % (
        submit_dir, dag_dir, output_dir)
    farmoutString += ' --input-files-per-job=%i %s %s ' % (
        filesperjob, jobName, bash_name)
    farmoutString += '--use-hdfs --job-generates-output-name  --memory-requirements=3000 --vsize-limit=8000 --max-usercode-size=200'
    return farmoutString


def submit_command(jobName, input_files, commands, output_sample_name, syst, dryrun=False):
    print "output_sample_name:", output_sample_name
    nfs_sample_dir = '/nfs_scratch/{}/{}/{}'.format(
        pwd.getpwuid(os.getuid())[0], jobName, output_sample_name)

    # create submit dir
    submit_dir = '%s/submit' % (nfs_sample_dir)
    if os.path.exists(submit_dir):
        print('Submission directory exists for {} {}.'.format(
            jobName, output_sample_name))

    # create dag dir
    dag_dir = '{}/dags/dag'.format(nfs_sample_dir)
    os.system('mkdir -p {}'.format(os.path.dirname(dag_dir)))
    os.system('mkdir -p {}'.format(dag_dir+'inputs'))

    # output dir
    output_dir = '/hdfs/store/user/{}/{}/{}/'.format(
        pwd.getpwuid(os.getuid())[0], jobName, syst)

    # create file list
    filesperjob = 1
    input_name = '{}/{}.txt'.format(dag_dir+'inputs', output_sample_name)
    with open(input_name, 'w') as file:
        for f in input_files:
            file.write('%s\n' % f.replace('/hdfs', '', 1))

    # create bash script
    bash_name = write_bash_script(commands, output_sample_name, dag_dir)

    # create farmout command
    farmoutString = default_farmout(
        jobName, input_name, output_dir, bash_name, submit_dir, dag_dir, filesperjob)

    if not dryrun:
        print('Submitting {}'.format(output_sample_name))
        os.system(farmoutString)
    else:
        print farmoutString

    return


if __name__ == "__main__":
    print 'This script cannot be used by itself. Please import it into your own script where you can provide an input command'
