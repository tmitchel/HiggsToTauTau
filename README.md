# 2016 Higgs to Tau-Tau Analysis Code 

[![pipeline status](https://gitlab.cern.ch/KState-HEP-HTT/ltau_analyzers/badges/master/pipeline.svg)](https://gitlab.cern.ch/KState-HEP-HTT/ltau_analyzers/commits/master)

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. The repository includes four analyzers, corresponding to the four final states being studied: electron-tau (et), muon-tau (mt), tau-tau (tt), and electron-muon (emu).

##### Table of Contents
[Organization](#organization) <br/>
[Quick Start](#quickstart) <br/>
<!-- [Quick Start](#quickstart) <br/> -->

<a name="organization"/>

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

<a name="quickstart"/>

## Quick Start

This section is intended to help someone quickly get started producing plots and templates. Most details about how to use the repository will be excluded, so read the rest of the instructions for more detailed information.

To start, there are many files containing scale factors, corrections, etc. that need to be gathered. For those that can't be easily grabbed, I put a tarball in the /afs area that can be copied.

1. Setup a new CMSSW release (needed for fake factor and 2017 tau trigger SFs)
    - `cmsrel CMSSW_9_4_0 && cd CMSSW_9_4_0/src && cmsenv`
2. Clone and build all necessary repositories, including this one.
    - clone this repo
        ```
        git clone ssh://git@gitlab.cern.ch:7999/KState-HEP-HTT/ltau_analyzers.git
        ```
    - get lepton SF files
        ```
        git clone https://github.com/CMS-HTT/LeptonEfficiencies
        ```
    - get tau trigger SF repo
        ```
        git clone -b tauTriggers2017_reMiniaod_test git@github.com:truggles/TauTriggerSFs2017.git TauTriggerSFs2017
        ```
    -  <a href="https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauJet2TauFakes#Install">Follow Jet2TauFakes installation instructions</a>
        - change `git clone -b 2017` to `git clone -b 2016` to get the fake factors for 2016
    - Compile all repos
        ```
        scram b -j 8
        ```
    - Get the tarball full of missing files
        ```
        cp /afs/hep.wisc.edu/home/tmitchel/public/ltau_analyzer_data.tar.gz .
        tar xzvf ltau_analyzer_data.tar.gz
        ```
3. Compile the appropriate plugin. For example, to compile the 2016 analyzer for the electron+tau channel, use the following command
        ```
        ./build plugins/et_analyzer2016.cc analyze2016_et 
        ```
    This will produce a binary named `analyze2016_et` that can be used to analyze 2016 etau events.
4. Later, you will also need to a plugin for creating 1D templates for plotting and a plugin for creating 2D templates for Combine. Might as well compile these right now.
    - Compile the binary for creating 1D templates with fake-factor estimation (using recommended 2D method). These are used for plotting later
        ```
        ./build plugins/fakeFactor2D.cc plotFF
        ```
    - Compile the binary for creating the 2D template that is given to Combine for limit setting. This also uses embedded samples and 2D fake factor
        ```
        ./build plugins/produceTemplatesFFv2.cc finalFF
        ```
5. Use the python automation script to processes all MELA'd files and produce output trees with selection branches and weight branches
        ```
        python automate_analysis.py -a -e analyze2016_et -p /hdfs/store/user/tmitchel/etau2016_AC_v1_mela/etau2016_AC_v1_hadd/ -P ETau_13_etau2016_AC_v1_hadd- --output-dir etau2016_AC_test1
        ```
    This command requires some explanation because it has many options. The purpose of this script is to run a provided binary on all *.root files in a given directory. The binary is supplied with the `-e` option and the input directory is supplied with the `-p` option. Because the analyzers use the name of the input root file to lookup the correct cross-section, any prefixes added to the filenames (looking at you SVFit and MELA) need to be stripped before providing the sample name to the binary. This is done using the `-P` option. Note that this is an uppercase P, while the path is provided with lowercase p. All processed files are stored in `Output/trees/`. The `--output-dir` option can be used to create a new directory in `Output/trees/` and store the processed files there. Lastly, the `-a` option enable the AC reweighter. The reweighter is used for AC samples to read weights from `data/AC_weights` files and store them in the output tree.

    NOTE: The AC sample directory shown in the previous command is the current, up-to-date directory. The binaries will complain about missing branches (mainly trigger branches), but they aren't used for 2016 and the warnings can be ignored.
6. hadd the appropriate files together
    ```
    bash hadder.sh El Output/trees/etau2016_AC_test1
    ```
    This will hadd all files in the directory `Output/trees/etau2016_AC_test1` into the appropriate files. The hadded files will be stored in the same directory and a sub-directory named `originals` will be made to store all pre-hadded files. The `El` option needs to be passed so that embedEl*.root is hadded. `Mu` is used for the muon embedded samples.
7. (Optional) Convert the slim trees into templates for plotting using the binary we compiled earlier.
    ```
    ./plotFF -y 2016 -t etau_tree -d Output/trees/etau2016_AC_test1 -v t1_pt -b 20 30 150 
    ```
    This will create a root file named `Output/templates/template_et_t1_pt_ff2016.root` containing 4 directories full of histograms. These directories correspond to the inclusive, 0jet, boosted, and vbf categories of the analysis. Each directory will have a histogram showing `t1_pt` in 20 bins from [30,150] for each sample in the input diretory (provided with `-d`). The -t option tells the binary the name of the TTree to process and `-y` tells the binary which year this data corresponds to (important for getting correct fake factors).
8. (Optional) Plot histograms from the previously created template
    ```
    cd scripts
    python stackPlot.py -p your_prefix -c et_inclusive --channel et -v t1_pt -y 2016 -s 1.6
    ```
    This will create a stacked histogram showing t1_pt in the etau inclusive region with the previously chosen binning. The `-y` option tells the script which year to use in labels. The `-s` option tells the script by what factor to multiply the the maximum bin yield when setting the top of the histogram. Increase the value of `-s` if histograms are going off the top of the pad. The plot will be stored in `../Output/plots/your_prefix_t1_pt_et_inclusive_2016.pdf`. NOTE: The most likely error here occurs when you attempt to plot a variable for which I haven't given an appropriate title. To correct this, open the script and find the `titles` dictionary. Simply add the variable provided to `-v` and give a name to be printed on the X-axis of histograms.

9. Create a 2D template for Combine
    ```cd $CMSSW_BASE/src/ltau_analyzers
    /finalFF -y 2016 -t etau_tree -d Output/trees/etau2016_AC_test1 -N -O
    ```
    The finalFF binary is used to create the 2D templates that will unrolled and given to Combine for limit setting. The binning and sensitive observables are all hard-coded, so changing them (with one exception) requires recompiling. The above command will create a template using the input directory given to `-d` and store the root file containing templates as `Output/templates/template_et2016_finalFFv2.root`. `-y` tells the binary to use things specific to 2016 and `-t` tells the binary to read a TTree named "etau_tree". By default, the binary uses the new definition of the vbf category and mjj vs m_sv in that category. The `-N` option tells the binary to use "NN_disc" instead of mjj and adjusts the binning appropriately. The `-O` option tells the binary to use the old vbf category definition and also adjusts the boosted category definition appropriately. The output root file should be ready for unrolling.

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

