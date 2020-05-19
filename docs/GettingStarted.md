# Getting Started

## Connecting to the LPC

- Get your Kerberos credentials:
    ```
    kinit <username>@FNAL.GOV
    ```
- Connect to the LPC
    ```
    ssh -XY <username>@cmslpc-sl6.fnal.gov
    ```
- Setup environment
    ```
    source /cvmfs/cms.cern.ch/cmsset_default.sh // *.csh for default csh shell
    ```

## Setting up a CMSSW release

- Create a new directory in your `nobackup` directory
    ```
    mkdir test-dir && cd test-dir
    ```
- Check CMSSW releases available for your \$SCRAM_ARCH (default is $SCRAM_ARCH=slc6_amd64_gcc700, but can change with `export SCRAM_ARCH=<newarch>`)
    ```
    scram list
    ```
- Setup the CMSSW release
    ```
    cmsrel CMSSW_10_4_0
    ```
- Move into the environment and source it
    ```
    cd CMSSW_10_4_0/src && cmsenv
    ```
- Setup any code that you need - pull from git, write from scratch, etc.
- Compile anything CMSSW related
    ```
    cd $CMSSW_BASE/src
    scram b -j 8  // -j tells how many cores to use while compiling
    ```

## Git-related things

#### Clone a repository
```
git clone git@github.com:<user-name></user-name>/<repo-name>.git  // if SSH keys are set up
git clone https://github.com/<user-name></user-name>/<repo-name>.git  // no SSH keys
```

#### Adding and committing
```
git add <edited files...>
git commit -m "<commit message>"  // commits all files that have been "git add"-ed
```

#### Pushing and pulling
```
git pull <remote> <branch>  // pull code from somewhere else
git push <remote> <branch>  // push code to somewhere else
```
Typically, remote is named `origin` and branch is `master`

## Accessing files on the LPC EOS
A more comprehensive set of instructions are [here](https://uscms.org/uscms_at_work/computing/LPC/usingEOSAtLPC.shtml) and additional commands [here](https://uscms.org/uscms_at_work/computing/LPC/additionalEOSatLPC.shtml)

```
eosls /store/user/<username>/<directory>  // ls for the EOS area
root -l root://cmsxrootd.fnal.gov//store/user/<username>/<directory>/<file>  // open ROOT files stored in EOS area
xrdcp root://cmseos.fnal.gov//store/user/<username>/<directory>/<file> .  // copy file from EOS to current directory on LPC
```

## Useful twikis
- [Tau ID](https://twiki.cern.ch/twiki/bin/viewauth/CMS/TauIDRecommendationForRun2)
- [Higgs to Tau Tau group](https://twiki.cern.ch/twiki/bin/view/CMS/Higgs2Tau)
- [STXS twiki](https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingLegacyRun2)
- [2018 Embedding](https://twiki.cern.ch/twiki/bin/viewauth/CMS/TauTauEmbeddingSamples2018)
- [Primary repo](https://github.com/tmitchel/HiggsToTauTau)