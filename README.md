# Higgs to Tau-Tau Analysis Code 

[![pipeline status](https://gitlab.cern.ch/KState-HEP-HTT/ltau_analyzers/badges/master/pipeline.svg)](https://gitlab.cern.ch/KState-HEP-HTT/ltau_analyzers/commits/master)

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. The repository includes analyzers, corresponding to the final states being studied: electron-tau (et), muon-tau (mt), and electron-muon (emu).

##### Table of Contents
[Organization](#organization) <br/>
[File Locations](#files) <br/>
[Processed Files](#ofiles) <br/>
[Quick Start](#quickstart) <br/>
[Modules](#modules) <br/>
[Plugins](#plugins) <br/>
[Compiling Plugins](#compiling) <br/>
[ROC curves](#roc) <br/>

<a name="organization"/>

## Organization

- Directories
  - `bin`: Contains binaries to be run later.
  - `include`: This is where the majority of the library is contained. Header files for each object are stored here along with headers for   corrections and other necessary components. 
  - `inputs`: Some scale factors require input files to do some calculation. All necessary input files are stored in this directory.
  - `plugins`: This directory contains all C++ plugins using the library. This is primarily the analyzers and template makers, but any code   drawing on the library can be placed here.
  - `scripts`: All short scripts are stored here. Plotting scripts and other small scripts can be stored in this directory.
  - `Output`: All outputs are stored in the directory. Outputs include plots, TTrees, and templates for Higgs Combine. These are all stored in subdirectories within the `Output` directory.
- Spare Files
  - `build`: A simple bash script for compiling a plugin with the correct libraries (ROOT). The script takes two ordered arguments: the plugin to compile and the name of the output binary. The binary should be copied to your $HOME/bin directory
  - `automate_analysis.py`: Used for analyzing an entire directory. Explained more later
  - `hadder.sh`: Quick script to hadd analyzed ROOT files into the correctly named files

<a name="files"/>

## File Locations

Here are the locations of all currently used files. Directory names should be obvious
- /hdfs/store/user/tmitchel/legacy-v1/mela/

<a name="ofiles"/>

## Processed Locations

Location of all files already processed by one of the analyzers. They are ready to be fed to the NN or used directly for making templates/plots.
- /hdfs/store/user/tmitchel/legacy-v1/analyzed/

<a name="quickstart"/>

## Quick Start

This section is intended to help someone quickly get started producing plots and templates. Most details about how to use the repository will be excluded, so read the rest of the instructions for more detailed information.

To start, there are many files containing scale factors, corrections, etc. that need to be gathered. For those that can't be easily grabbed, I put a tarball in the /afs area that can be copied.

1. Setup a new CMSSW release (must be 104X or greater for python scripts)
```
cmsrel CMSSW_CMSSW_10_4_0 && cd CMSSW_10_4_0/src && cmsenv
```
2. Clone and build all necessary repositories, including this one.
    - clone this repo
        ```
        git clone ssh://git@gitlab.cern.ch:7999/KState-HEP-HTT/ltau_analyzers.git
        ```
    - get lepton SF files and the lepton efficiency interface
        ```
        git clone https://github.com/CMS-HTT/LeptonEff-interface.git HTT-utilities
        cd HTT-utilities/LepEffInterface/
        rm -r data
        git clone https://github.com/CMS-HTT/LeptonEfficiencies data
        cd ${CMSSW_BASE/src
        ```
    - get tau trigger SF repo
        ```
        cd $CMSSW_BASE/src
        mkdir TauAnalysisTools
        git clone -b final_2017_MCv2 https://github.com/cms-tau-pog/TauTriggerSFs $CMSSW_BASE/src/TauAnalysisTools/TauTriggerSFs
        ```
    - get fake factor files
        ```
        cd $CMSSW_BASE/src
        git clone https://github.com/CMS-HTT/Jet2TauFakes.git HTTutilities/Jet2TauFakes
        cd HTTutilities/Jet2TauFakes
        git checkout v0.2.2
        git clone -b 2017 ssh://git@gitlab.cern.ch:7999/cms-htt/Jet2TauFakesFiles.git data2017 // 2016 and 2018 as well
        ```
    - setup tau ID efficiency helper
        ```
        cd $CMSSW_BASE/src
        git clone https://github.com/cms-tau-pog/TauIDSFs TauPOG/TauIDSFs
        ```
    - Compile all repos
        ```
        cd $CMSSW_BASE/src
        scram b -j 8
        ```
    - Get the tarball full of missing files
        ```
        cd ${CMSSW_BASE}/src/ltau_analyzers
        cp /afs/hep.wisc.edu/home/tmitchel/public/ltau_analyzer_data.tar.gz .
        tar xzvf ltau_analyzer_data.tar.gz
        ```
3. Compile the appropriate plugin. For example, to compile the 2016 analyzer for the electron+tau channel, use the following command
    ```
    ./build plugins/et_analyzer2016.cc bin/analyze2016_et
    ```
    This will produce a binary named `analyze2016_et` in the `bin` directory that can be used to analyze 2016 etau events.
4. Use the python automation script to processes all MELA'd files and produce output trees with selection branches and weight branches
    ```
    python automate_analysis.py 2017 -e bin/analyze2017_et -p /hdfs/store/user/tmitchel/etau2017_official_v3-mela/etau2017_official_v3-hadd/ --output-dir etau2017_official_v3-analyzed --syst 2017    
    ```
    This command requires some explanation because it has many options. The purpose of this script is to run a provided binary on all *.root files in a given directory. The binary is supplied with the `-e` option and the input directory is supplied with the `-p` option. Because the analyzers use the name of the input root file to lookup the correct cross-section, any prefixes added to the filenames (looking at you SVFit and MELA) need to be stripped before providing the sample name to the binary. The script `scripts/strip.sh` can be used to make a copy of the files with the prefix removed. All processed files are stored in `Output/trees/`. The `--output-dir` option can be used to create a new directory in `Output/trees/` and store the processed files there. The `-a` option enable the AC reweighter. The reweighter is used for AC samples to read weights from `data/AC_weights` files and store them in the output tree. The final option `--syst` is provided when you want to produce additional trees containing systematic variations. Either '2016' or '2017' must be passed in order to choose the correct branch names.

5. hadd the appropriate files together `problem with naming for ggH JHUGEN and MadGraph samples`
    ```
    bash hadder.sh El Output/trees/etau2017_official_v3-analyzed
    ```
    This will hadd all files in the directory `Output/trees/etau2017_official_v3-analyzed` into the appropriate files. The hadded files will be stored in the same directory and a sub-directory named `originals` will be made to store all pre-hadded files. The `El` option needs to be passed so that embedEl*.root is hadded. `Mu` is used for the muon embedded samples.
7. Fill the fake fractions to be used in plotting and template making.
    ```
    python scripts/fill_fake_fractions.py -i /hdfs/store/user/tmitchel/etau2017_official_v1p9-analyzed -y 2017 -t mt_tree -s suffix
    ```
8. (Optional) Convert the slim trees into templates for plotting.
    - Plots and binning are defined in the JSON file `scripts/plotting.json`. Multiple plotting scenarios can be defined and then chosen at runtime via a command line option. The `variables` key defines the variable name along with the binning. Binning is assumed to be [nbins, lowest, highest]. The `zvar` key is used to specify a variable and binning to use for the VBF sub-categorization. The binning for `zvar` is assumed to be [lowest, edge1, edge2, ...].
    - Produce the plots
        ```
        python scripts/produce_histograms.py -y 2017 -t et_tree -i /hdfs/store/user/tmitchel/etau2017_official_v1p9-analyzed -f Output/fake_fractions/et2017_legacy_v0p1.root -d Aug9-plot -c baseline
        ```
        This will produce an output root file in the `Output/plots` directory. The file contains a TDirectory for each category. Inside each category will be a TDirectory for each variable containing histograms for each sample. `-f` is used to specify the file containing fake fractions which must be loaded to compute the jetFakes background. `-d` is used to add a descriptive string to the name of the output file. `-c` is used to specify the plotting scenario to be read from `plotting.json`.
9. (Optional) Plot histograms from the previously created template
    ```
    python scripts/plotter.py -i Output/histograms/htt_et_ztt_noSys_fa3_Aug9-plot.root -p v1p1p1 -v D0_VBF -y 2017 -l "D_{0-}^{VBF}" -s 1.5 -c et_vbf
    ```
    This will create a stacked histogram showing D0_VBF in the etau vbf region with the previously chosen binning. The `-y` option tells the script which year to use in labels. The `-s` option tells the script by what factor to multiply the the maximum bin yield when setting the top of the histogram. Increase the value of `-s` if histograms are going off the top of the pad. The plot will be stored in `../Output/plots/your_prefix_D0_VBF_et_vbf_2016.pdf`. `-l` is used to give a fancy latex title to the plot.

10. Create a 2D template for Combine
    ```
    cd $CMSSW_BASE/src/ltau_analyzers
    python scripts/produce_template.py -y 2017 -t mutau_tree -i Output/trees/mutau2017_legacy_v1-test-stablize -f mt2017_legacy_v0p1.root -d Aug9-ref -c baseline
    ```
    This script will produce all the 2D templates needed for Higgs Combine. VBF category variables and binning are configured using the `scripts/binning.json` file where multiple scenarios may be defined. The previous command will use the baseline scenario. The `-f` option is used to point to the file containing fake-fractions that must be loaded to get an estimate of the jetFakes background. Use `python -h` to see other options including running with systematics.

<a name="modules"/>

## Modules

Each physics object has a module containing all relevant data. All modules are defined in the `include` directory and can be included in plugins as needed. In addition to physics object modules, the include directory also includes classes for various reweighters, as well as a utility class containing histograms and cross-sections. Additionally, the include directory has a simple class used to provide functionality for command line flags.


<a name="plugins"/>

## Plugins

The `plugins` directory contains many useful C++ plugins for analyzing data, making plotting templates, or making Combine templates. These plugins are all compiled using the `build` script discussed in the "Compiling Plugins" section. A list of currently maintained plugins is shown below:

- `et_analyzer2016.cc`: Used to analyze the 2016 etau channel and produce slimmed trees. Partial systematics included. 
- `et_analyzer2017.cc`: Used to analyze the 2017 etau channel and produce slimmed trees. Partail systematics included.
- `et_analyzer2018.cc`: Used to analyze the 2018 etau channel and produce slimmed trees. Partail systematics included.
- `mt_analyzer2016.cc`: Used to analyze the 2016 mutau channel and produce slimmed trees. Partial systematics included. 
- `mt_analyzer2017.cc`: Used to analyze the 2017 mutau channel and produce slimmed trees. Partail systematics included.
- `mt_analyzer2018.cc`: Used to analyze the 2018 mutau channel and produce slimmed trees. Partail systematics included.
- `plugins/produce_plots.cc`: Produce templates for plotting with fake factor. No systematics incuded.
- `plugins/produce_templates.cc`: Produce Combine templates using fake factor. Fake factor systematics are implemented. 

All other plugins have not been constantly updated and are at various degrees of deprecation.

<a name="compiling"/>

## Compiling Plugins

The `build` script is provided to make compilation easier/less verbose. The script takes two input parameters and outputs a compiled binary. The first parameter must be the name of the analyzer to be compiled; the second parameter is the desired name of the output binary. An example is shown below:
```
./build plugins/et_analyzer.cc bin/Analyze_et
```
This example compiles the electron-tau channel analyzer to make an executable named Analyze_et. All analyzers are compiled with O3 level optimization as well as linking ROOT and RooFit.

<a name="roc"/>

## ROC curves
ROC curves can easily be produced by changing the ROOT macro `plotRocCurves.C` and then running with CINT. 

