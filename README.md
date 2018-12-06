# 2016 Higgs to Tau-Tau Analysis Code 

[![pipeline status](https://gitlab.cern.ch/KState-HEP-HTT/ltau_analyzers/badges/master/pipeline.svg)](https://gitlab.cern.ch/KState-HEP-HTT/ltau_analyzers/commits/master)

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. The repository includes four analyzers, corresponding to the four final states being studied: electron-tau (et), muon-tau (mt), tau-tau (tt), and electron-muon (emu).

##### Table of Contents
[Organization](#organization) <br/>
[File Locations](#files) <br/>
[Quick Start](#quickstart) <br/>
[Modules](#modules) <br/>
[Plugins](#plugins) <br/>
[Compiling Plugins](#compiling) <br/>
[ROC curves](#roc) <br/>

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

<a name="files"/>

## File Locations

Here are the locations of all currently used files. Directory names should be obvious
- /hdfs/store/user/tmitchel/etau2016_AC_v1_mela/etau2016_AC_v1_hadd/
- /hdfs/store/user/tmitchel/etau2016_stable_v2_mela/etau2016_stable_v2_hadd/
- /hdfs/store/user/tmitchel/etau2017_update_v2_mela/etau2017_update_v2_hadd/
- /hdfs/store/user/tmitchel/mutau2016_stable_v2_mela/mutau2016_stable_v2_hadd/
- /hdfs/store/user/tmitchel/mutau2017_update_v2_mela/mutau2017_update_v2_hadd/

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
        mkdir TauTriggerSFs2017
        cd TauTriggerSFs2017
        git clone -b tauTriggers2017_reMiniaod_test git@github.com:truggles/TauTriggerSFs2017.git TauTriggerSFs2017
        cd TauTriggerSFs2017
        scram b -j 8
        cd ../..
        ```
    -  <a href="https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauJet2TauFakes#Install">Follow Jet2TauFakes installation instructions</a>
        - change `git clone -b 2017` to `git clone -b 2016` to get the fake factors for 2016
    - Compile all repos
        ```
        cd ../..
        scram b -j 8
        ```
    - Get the tarball full of missing files
        ```
        cd ltau_analyzers
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
        ./build plugins/fakeFactor2D.cc plotFF -f
        ```
    - Compile the binary for creating the 2D template that is given to Combine for limit setting. This also uses embedded samples and 2D fake factor
        ```
        ./build plugins/produceTemplatesFFv2.cc finalFF -f
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

    NOTE: The `-s` option can be given to the finalFF binary in order to produce fake factor systematics up/down shapes.

<a name="modules"/>

## Modules

Each physics object has a module containing all relevant data. All modules are defined in the `include` directory and can be included in plugins as needed. In addition to physics object modules, the include directory also includes classes for various reweighters, as well as a utility class containing histograms and cross-sections. Additionally, the include directory has a simple class used to provide functionality for command line flags.


<a name="plugins"/>

## Plugins

The `plugins` directory contains many useful C++ plugins for analyzing data, making plotting templates, or making Combine templates. These plugins are all compiled using the `build` script discussed in the "Compiling Plugins" section. A list of currently maintained plugins is shown below:

- `et_analyzer2016.cc`: Used to analyze the 2016 etau channel and produce slimmed trees. Has the capability to do anomolous couplings. Partial systematics included. 
- `et_analyzer2017.cc`: Used to analyze the 2017 etau channel and produce slimmed trees. AC capability is not yet implemented. Partail systematics included.
- `mt_analyzer2016.cc`: Used to analyze the 2016 mutau channel and produce slimmed trees. AC capability is not yet implemented. Partial systematics included. 
- `mt_analyzer2017.cc`: Used to analyze the 2017 mutau channel and produce slimmed trees. AC capability is not yet implemented. Partail systematics included.
- `fakeFactor2D.cc`: Produce templates for plotting with fake factor. No systematics incuded.
- `produceTemplatesFFv2.cc`: Produce Combine templates using fake factor. Fake factor systematics are implemented. 

All other plugins have not been constantly updated and are at various degrees of deprecation.

<a name="compiling"/>

## Compiling Plugins

The `build` script is provided to make compilation easier/less verbose. The script takes two input parameters and outputs a compiled binary. The first parameter must be the name of the analyzer to be compiled; the second parameter is the desired name of the output binary. An example is shown below:
```
./build plugins/et_analyzer.cc Analyze_et
```
This example compiles the electron-tau channel analyzer to make an executable named Analyze_et. All analyzers are compiled with O3 level optimization as well as linking ROOT and RooFit.

<a name="roc"/>

## ROC curves
ROC curves can easily be produced by changing the ROOT macro `plotRocCurves.c` and then running with CINT. 

