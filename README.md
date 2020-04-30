# Higgs to Tau-Tau Analysis Code 

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. The repository includes analyzers, corresponding to the final states being studied: electron-tau (et) and muon-tau (mt).

##### Table of Contents
[Organization](#organization) <br/>
[File Locations](#files) <br/>
[Processed Files](#ofiles) <br/>
[Quick Start](#quickstart) <br/>
[Objects](#objects) <br/>
[Helpers](#helpers) <br/>
[Plugins](#plugins) <br/>
[Compiling Plugins](#compiling) <br/>
[Scripts](#scripts) <br/>

<a name="organization"/>

## Organization

- Directories
  - `bin`: Contains binaries primarily used for initial processing.
  - `include`: This is where the majority of the library is contained. Header files for each object are stored here along with headers for corrections and other necessary components. 
  - `data`: All input ROOT files for corrections are contained here. This includes a file containing legacy scale factors for each year, pileup corrections, and Higgs pT reweighting files for MadGraph samples.
  - `plugins`: This directory contains all C++ plugins using the library. This is currently limited to analyzers, but other plugins may be added.
  - `scripts`: Scripts used to process the outputs from analyzers are contained here. This includes plotters, datacard builders, and fake weighters.
  - `Output`: All outputs are stored in the directory. Outputs include plots, fake fractions, TTrees, and datacards for Higgs Combine. These are all stored in subdirectories within the `Output` directory.
- Other Files
  - `build`: A simple bash script for compiling a plugin with the correct libraries (ROOT). The script takes two ordered arguments: the plugin to compile and the name of the output binary. The binary should be copied to your $HOME/bin directory
  - `automate_analysis.py`: Used for analyzing an entire directory. Explained more later

<a name="files"/>

## File Locations

Here are the locations of all currently used files on the Wisconsin cluster. Directory names should be obvious
- /hdfs/store/user/tmitchel/legacy-v5

<a name="ofiles"/>

## Processed Locations

Location of all files already processed by one of the analyzers. They are ready to be fed to the NN or used directly for making datacards/plots.
- /hdfs/store/user/tmitchel/legacy-v5/analyzed

<a name="quickstart"/>

## Quick Start

This section is intended to help someone quickly get started producing plots and datacards. Most details about how to use the repository will be excluded, so read the rest of the instructions for more detailed information.

1. Setup a new CMSSW release (must be 104X or greater for python scripts)
```
cmsrel CMSSW_CMSSW_10_4_0 && cd CMSSW_10_4_0/src && cmsenv
```
2. Clone and build all necessary repositories, including this one.
    - clone this repo
        ```
        git clone -b stabalize-workflow git@github.com:tmitchel/LTau_Analyzers.git
        ```
    - Compile all repos (none are currently needed)
        ```
        cd ${CMSSW_BASE}/src
        scram b -j 8
        ```
    - Setup a python virtual environment
        ```
        cd ${CMSSW_BASE}/src/LTau_Analyzers
        virtualenv .pyenv
        source .pyenv/bin/activate  # this must be done every time you log in
        source setup/setup-python.sh
        ```
3. Compile the appropriate plugin. For example, to compile the 2018 analyzer for the muon+tau channel, use the following command
    ```
    ./build plugins/mt_analyzer2018.cc bin/analyze2018_mt
    ```
    This will produce a binary named `analyze2018_mt` in the `bin` directory that can be used to analyze 2018 mutau events.
4. Use the python automation script to processes all files in a directory and produce output trees with selection branches and weight branches
    ```
    python automate_analysis.py -e bin/analyze2018_mt -p /hdfs/store/user/tmitchel/legacy-v5/mela/mt2018_v1p3 --output-dir mt2018_v5p3 --parallel --syst
    ```
    This command requires some explanation because it has many options. The purpose of this script is to run a provided binary on all *.root files in a given directory. The binary is supplied with the `-e` option and the input directory is supplied with the `-p` option. Because the analyzers use the name of the input root file to lookup the correct cross-section, any prefixes added to the filenames (looking at you SVFit and MELA) need to be stripped before providing the sample name to the binary. All processed files are stored in `Output/trees/`. The `--output-dir` option can be used to create a new directory in `Output/trees/` and store the processed files there. The --parallel option is used to enable multiprocessing. The maximum number of processes at any one time is the minimum(10, ncores / 2). Each input file will recieve it's own process.  The final option `--syst` is provided when you want to produce additional output files containing systematic shifts. Within the output directory, nominal files are stored in the `NOMINAL` sub-directory and sub-directories will be created for each systematic with the prefix SYST_. Logs will also be saved in the `logs` subdirectory.

5. hadd the appropriate files together
    ```
    python scripts/hadder.py -p Output/trees/mt2018_v5p3
    ```
    This will hadd all files in the directory `Output/trees/mt2018_v5p3` into the appropriate files. This will descend into each subdirectory (exluding logs) to hadd files for NOMINAL and all systematics. The hadded files will be stored in the new sub-directory within the current sub-directory named `merged`.
7. Fill the fake fractions to be used in plotting and datacard making and store them in the `fake_factors` directory.
    ```
    python scripts/fill_fake_fractions.py -i Output/trees/mt2018_v5p3/NOMINAL/merged -y 2018 -t mt_tree -s v5p3 -c
    ```
    The `-s` option is used to add a descriptor to the output file containing the fake fractions. When the `-c` flag is provided, the newly calculated fractions will be used to create a new ROOT file named 'jetFakes.root' within the input directory. This file contains all necessary data and MC events used for calculating the jetFakes background. A new branch is added containing the fake weight used to appropriately scale events.
8. (Optional) Create histograms for plotting.
    - Plots and binning are defined in the JSON file `scripts/plotting.json`. Multiple plotting scenarios can be defined and then chosen at runtime via a command line option. The `variables` key defines the variable name along with the binning. Binning is assumed to be [nbins, lowest, highest]. The `zvar` key is used to specify a variable and binning to use for the VBF sub-categorization. The binning for `zvar` is assumed to be [lowest, edge1, edge2, ...].
    - Produce the plots
        ```
        python scripts/produce_histograms.py -e -y 2018 -t mt_tree -i Output/trees/mt2018_v5p3/NOMINAL/merged -d v5p3 -c baseline
        ```
        This will produce an output root file in the `Output/histograms` directory. The file contains a TDirectory for each category. Inside each category will be a TDirectory for each variable containing histograms for each sample. `-d` is used to add a descriptive string to the name of the output file. `-c` is used to specify the plotting scenario to be read from `plotting.json`. The `-e` flag tells the script to use the embedded background instead of ZTT MC.
9. (Optional) Create stack plots from previous histograms.
    ```
    python scripts/autoplot.py --input Output/histograms/htt_mt_emb_noSys_fa3_2018_v5p3.root -c mt -p v5p3 -y 2018
    ```
    This will create stacked histograms for each variable listed at the beginning of the script. 
10. Create a 2D datacards for Combine
    ```
    python scripts/produce_datacards.py -e -y 2018 -t mt_tree -i Output/trees/mt2018_v5p3 --suffix v5p3 -c baseline
    ```
    This script will produce all the 2D datacards needed for Higgs Combine. VBF category variables and binning are configured using the `scripts/binning.json` file where multiple scenarios may be defined. The previous command will use the baseline scenario. Use `python -h` to see other options including running with systematics.

<a name="objects"/>

## Objects

All input variables are accessed through factories. These factories group information into physics-motivated classes. Some factories provide access to variables directly (i.e. event_info provides direct access to the event's run number), while others provide access to friend classes that then provide even more physics context to the variables (i.e. electron_factory provides access to electron objects containing information about a single electron). This reduces the mental burden of remembering the meaning of each variable name in your input files. These factories are all contained within the `include` directory.

<a name="helpers"/>

## Helpers

The `include` directory also contains headers providing many useful functions. 
- ACWeighter.h provides methods for accessing AC reweighting coefficients for JHU samples. These can then be stored in output TTrees.
- CLParser.h provides the basic command-line parsing capabilities used by plugins
- LumiReweightingStandAlone.h provides helper functions for reading pileup corrections
- slim_tree.h contains the output TTree and defines how it will be filled
- swiss_army_class.h contains useful information with no other home. This includes: luminosities, cross-sections, embedded tracking scale factors, and more.

<a name="plugins"/>

## Plugins

The `plugins` directory contains the code for analyzing input skims. These plugins are all compiled using the `build` script discussed in the "Compiling Plugins" section. A list of currently maintained plugins is shown below:

- `et_analyzer2016.cc`: Used to analyze the 2016 etau channel and produce slimmed trees.
- `et_analyzer2017.cc`: Used to analyze the 2017 etau channel and produce slimmed trees.
- `et_analyzer2018.cc`: Used to analyze the 2018 etau channel and produce slimmed trees.
- `mt_analyzer2016.cc`: Used to analyze the 2016 mutau channel and produce slimmed trees.
- `mt_analyzer2017.cc`: Used to analyze the 2017 mutau channel and produce slimmed trees.
- `mt_analyzer2018.cc`: Used to analyze the 2018 mutau channel and produce slimmed trees.

<a name="compiling"/>

## Compiling Plugins

The `build` script is provided to make compilation easier/less verbose. The script takes two input parameters and outputs a compiled binary. The first parameter must be the name of the analyzer to be compiled; the second parameter is the desired name of the output binary. An example is shown below:
```
./build plugins/et_analyzer.cc bin/Analyze_et
```
This example compiles the electron-tau channel analyzer to make an executable named Analyze_et. All analyzers are compiled with O3 level optimization as well as linking ROOT and RooFit.

<a name="scripts"/>

## Scripts

Most scripts cannot be run with the tools provided by the basic CMSSW environment. It is recommended to create a virtual environment for installing all necessary packages.
```
virtualenv .pyenv
```
This will create a python2 virtual environment in the directory .pyenv. To activiate this environment, use
```
source .pyenv/bin/activate  # deactivate to return to normal environment
```
With the environment activated, install all necessary packages (/cvmfs does weird things so it's hard to get a requirements.txt)
```
source setup/setup-python.sh
```