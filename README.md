# 2016 Higgs to Tau-Tau Analysis Code 

[![Build Status](https://travis-ci.com/tmitchel/HTT2016_analyzer.svg?branch=master)](https://travis-ci.com/tmitchel/HTT2016_analyzer)

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. The repository includes four analyzers, corresponding to the four final states being studied: electron-tau (et), muon-tau (mt), tau-tau (tt), and electron-muon (emu).

## Compiling the analysis code

The `build` script is provided to make compilation easier/less verbose. The script takes two input parameters and outputs a compiled binary. The first parameter must be the name of the analyzer to be compiled; the second parameter is the desired name of the output binary. An example is shown below:
```
./build et_analyzer.cc Analyze_et
```
This example compiles the electron-tau channel analyzer to make an executable named Analyze_et. All analyzers are compiled with O3 level optimization as well as linking ROOT and RooFit.

## Running the analysis code

All analyzers will take a ROOT file containing a skimmed TTree as input and output a new ROOT file containing directories full of histograms. The analyzers must be run with a specific set of command-line flags provided. These include things like the input file name, whether to run nominal or a systematic shift, etc. Generally, it is easier to use the provided python automation scripts to help in providing flags, but the analyzers can be run manually as well. The output file will be stored in the `output` directory.

### Automatic Mode

The script `automate_analysis.py` is used to automate the process of running an analyzer on all input files in a given directory. Provided a set of flags, the script will run a given analyzer with the correct flags on all ROOT files in the provided directory. An example is shown below, assuming the existence of the binary Analyze_et compiled from the electron-tau analyzer. 
```
python automate_analysis.py --exe Analyze_et --data --local --syst --suffix _asuffix.root
```

This example will run the Analyze_et binary on all files in the local data directory. The analyzer will be told it is running on data to prevent MC corrections from being applied. The analyzer will be run once for each file/systematic permutation. The `--suffix` option tells the script to remove the provided suffix from all input files so that the analyzer can read them correctly. An output file for each input will be stored in the `output` directory with the same name as the stripped input file plus the suffix `_output.root`. For more information about options, use

```
python automate_analysis.py --help
```

Input directories may be changed by changing lines like:
```python
fileList = [ifile for ifile in glob('root_files/mela_svfit_full/*') if '.root' in ifile and 'Data' in ifile]
```

This needs to be updated to make it more user-friendly.

### Manual Mode

The analyzer can also be run by calling the binary explicitly from the command-line. This is useful for running on single files and testing, but not for processing large sets of inputs. In order to run in manual mode, you must provide the following the set of flags:
 - -s \<Name of the file excluding the postfix\>
 - -n \<Name of the process i.e. "ZJ"\>
 - -p \<Path to the input file\>

Additionally, options may be provided to use a certain systematic variation or to strip a suffix from the filename. An example usage is shown below:
```
./Analyze_et -s DYJets1 -n ZTT -p root_files/mela_svfit_full -P _svFit_mela.root -u met_JESUp
```

This command will run the Analyze_et binary on the file `root_files/mela_svfit_full/DYjets1_svFit_mela.root` telling the analyzer to use met_JESUp instead of met and classify the process as Z->TT.

## To-Do List
 - Check the naming of all branches for all channels
 - Modify helper scripts to work for more channels than just etau
 - Merge the tt code into the master branch once it is ready
   - Add histograms for control regions
   - Add systematics that can't just be read from branches
 - Prepare/Merge the mutau code from Doyeong
 - Check yields of mutau/tt vs old analyzers
 - Write the emu code from scratch

