# 2016 Higgs to Tau-Tau Analysis Code 

[![Build Status](https://travis-ci.com/KState-HEP-HTT/SMHTT_Analyzers.svg?branch=master)](https://travis-ci.com/KState-HEP-HTT/SMHTT_Analyzers)

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. The repository includes four analyzers, corresponding to the four final states being studied: electron-tau (et), muon-tau (mt), tau-tau (tt), and electron-muon (emu).

## Organization

- Directories
  - `include`: This is where the majority of the library is contained. Header files for each object are stored here along with headers for   corrections and other necessary components. 
  - `inputs`: Some scale factors require input files to do some calculation. All necessary input files are stored in this directory.
  - `plugins`: This directory contains all C++ plugins using the library. This is primarily the analyzers and template makers, but any code   drawing on the library can be placed here.
  - `scripts`: All short scripts are stored here. Plotting scripts and other small scripts can be stored in this directory.
  - `Output`: All outputs are stored in the directory. Outputs include plots, TTrees, and templates for Higgs Combine. These are all stored in subdirectories within the `Output` directory.
  - `.travis`: Contains the script used for testing with Travis CI. Probably just don't touch this one unless you know what you're doing (I barely do..)
- Spare Files
  - `build`: A simple bash script for compiling a plugin with the correct libraries (ROOT). The script takes two ordered arguments: the plugin to compile and the name of the output binary. The binary should be copied to your $HOME/bin directory
  - `automate_analysis.py`: Used for analyzing an entire directory. Explained more later
  - `hadder.sh`: Quick script to hadd analyzed ROOT files into the correctly named files

## Compiling the analysis code

The `build` script is provided to make compilation easier/less verbose. The script takes two input parameters and outputs a compiled binary. The first parameter must be the name of the analyzer to be compiled; the second parameter is the desired name of the output binary. An example is shown below:
```
./build plugins/et_analyzer.cc Analyze_et
```
This example compiles the electron-tau channel analyzer to make an executable named Analyze_et. All analyzers are compiled with O3 level optimization as well as linking ROOT and RooFit.

## Running the analysis code

All analyzers will take a ROOT file containing a skimmed TTree as input and output a new ROOT file containing directories full of histograms. The analyzers must be run with a specific set of command-line flags provided. These include things like the input file name, whether to run nominal or a systematic shift, etc. Generally, it is easier to use the provided python automation scripts to help in providing flags, but the analyzers can be run manually as well. The output file will be stored in the `output` directory.

### Automatic Mode

The script `automate_analysis.py` is used to automate the process of running an analyzer on all input files in a given directory. Provided a set of flags, the script will run a given analyzer with the correct flags on all ROOT files in the provided directory. An example is shown below, assuming the existence of the binary Analyze_et compiled from the electron-tau analyzer. 
```
python automate_analysis.py --exe Analyze_et --data --syst --suffix _aSuffix.root --prefix aPrefix --path root_files/
```

This example will run the Analyze_et binary on all files in the directory `root_files/`. The analyzer will be told it is running on data to prevent MC corrections from being applied. The analyzer will be run once for each file/systematic permutation. The `--suffix` option tells the script to remove the provided suffix from all input files so that the analyzer can read them correctly. Similarly, `--prefix` will strip the given prefix off the input names. An output file for each input will be stored in the `output` directory with the same name as the stripped input file plus the suffix `_output.root`. For more information about options, use

```
python automate_analysis.py --help
```

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

## Producing Templates For Plots
Because the analyzers store output as TTrees, the output must be converted into histograms before it can be plotted. The plugin `templateMaker.cc` is designed specifically to do this. Once compiled, the binary will take multiple flags. The usage is best shown in the example below

```
Maker -d Output/trees/ltau_trainAll -v NN_disc -b 25 0 1 -t etau_tree
```

This example will read analyzed all ROOT files in the directory `Output/trees/ltau_trainAll` and load the TTrees named `etau_tree`. The `NN_disc` branch will be read from the trees and put into a histogram with 25 bins in the range [0,1]. The output will be stored in the directory `Output/templates`.

## Producing 2D templates for Higgs Combine
In order to run limits, the TTrees need to be converted into 2D histograms to be unrolled and processed by Higgs Combine. The plugin `finalMaker.cc` is used for this processing. The variable used to fill the non-m_sv dimension of the histogram in the VBF category. Currently, there is no easy way to choose any variables for any other histogram in any other region. Also, the binning can't be easily modified without recompiling the plugin, yet. An example is shown below

```
finalMaker -d Output/trees/ltau_trainAll/mutau -t mutau_tree
```

This command will read the TTrees named `mutau_tree` from the directory `Output/trees/ltau_trainAll/mutau` and produce the necessary output histograms in the correct directories.

## Stack Plotting
The script `stackPlot.py` is used to create pretty stacked plots of any variable, provided a histogram file containing the requested histograms exists in the `Output/templates` directory. The plotting is done as follows

```
python stackPlot.py -c et_vbf -l et -p ltau_trainAll_14node -v NN_disc 
```

This will store a stacked histogram in the `Output/plots` directory. The histogram will be for the variable `NN_disc` with the binning chosen from the `templateMaker.cc` script. The histogram will be for the `et_vbf` category read from files in the directory `Output/templates/ltau_trainAll_14node`.

## ROC curves
ROC curves can easily be produced by changing the ROOT macro `plotRocCurves.c` and then running with CINT. 

