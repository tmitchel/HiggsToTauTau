# Higgs to Tau-Tau Analysis Code [![<tmitchel>](https://circleci.com/gh/tmitchel/HiggsToTauTau.svg?style=svg)](https://app.circleci.com/pipelines/github/tmitchel/HiggsToTauTau)

This code is used for the study of a Higgs boson decaying to a pair of tau leptons. Backends are provided for analyzing FSA ntuples and ggNtuples. These backends provide methods to access variables in a uniform manner independent of the input file type. This allows analyzers to write plugins for different analyses without worrying about the input file format. Adding new backends can be done in a straightforward manner. 

##### Table of Contents
[Installing](#install) <br/>
[Usage](#usage) <br/>
[Organization](#organization) <br/>
[Objects](#objects) <br/>
[Helpers](#helpers) <br/>

<a name="install"/>

## Installing

1. Setup a new CMSSW release (must be 104X or greater for python scripts)
    ```
    cmsrel CMSSW_CMSSW_10_4_0 && cd CMSSW_10_4_0/src && cmsenv
    ```
2. Clone and build all necessary repositories, including this one.
    - clone this repo
        ```
        git clone git@github.com:tmitchel/HiggsToTauTau.git
        ```
    - nothing else is currently required, but other dependencies can be install here
3. Compile CMSSW-based things
    ```
    cd ${CMSSW_BASE}/src
    scram b -j 8
    ```
4. Setup a python virtual environment
    ```
    cd ${CMSSW_BASE}/src/HiggsToTauTau
    virtualenv .pyenv
    source .pyenv/bin/activate  # this must be done every time you log in
    python -m pip install -U uproot keras pandas
    ```
5. (Optional) Download scale factors
    ```
    source setup/download.sh
    ```

<a name="usage"/>

## Usage

Write analyzers and store them in the `plugins` directory, preferably in a subdirectory corresponding to your analysis. Based on the type of ntuples you are using, include the correct header files: `include/fsa/` for FSA ntuples, `include/ggntuple` for ggNtuples. Add compilation commands to `Makefile` in order to make compiling analyzers simpler. Use the automation scripts to run your compiled analyzer over your set of input files. Lastly, do any post-processing required (i.e. calculate fake contributions, fill histograms, etc.).

Analysis specific directions are included in README's for each plugin subdirectory.

<a name="organization"/>

## Organization

- Directories
  - `bin`: Contains binaries compiled by the build scripts.
  - `include`: This is where the majority of the library is contained. General header files for corrections and other general tasks are stored here. This directory contains a subdirectory for each backend that handles reading the input file and providing the relevant information to the user.
  - `plugins`: This directory contains all C++ plugins using the library. This is currently limited to analyzers, but other plugins may be added.
  - `scripts`: Scripts used to process the outputs from analyzers are contained here. This includes plotters, datacard builders, and fake weighters.
  - `Output`: All outputs are stored in the directory. Outputs include plots, fake fractions, TTrees, and datacards for Higgs Combine. The `neural-network` directory stores files here as well. These are all stored in subdirectories within the `Output` directory.
  - `neural-network`: Contains python scripts for training and applying a neural network.
  - `docs`: Markdown files containing documentation.
  - `configs`: JSON files for configuring different processes.
  - `setup`: Scripts for initial setup of this repository.
- Other Files
  - `auto_boost_lpc.py`: Used to run boosted tau jobs on the LPC (condor not yet supported)
  - `auto_ac_wisc.py`: Used to submit anomalous coupling jobs on the Wisconsin cluster.
  - `raw_condor_submit.py`: Script to submit anomalous coupling jobs to the Wisconsin condor cluster. Does not use farmout scripts.


<a name="objects"/>

## Objects

All input variables are grouped into physics objects and must be accessed through these objects. For example, all electron-related variables are grouped into the `electron` object. To access the transverse momentum of this electron, use `electron->getPt()`. This provides much more context than simply accessing the `pt_1` variable. All objects are stored in `include/models`. These models are used independent of the input ntuple type providing a consistent interface for all ntuple types.

In order to easily construct objects, factories are provided for all ntuple types. These factories process the relevant TTrees and use the information to construct a `std::vector` of objects the user can access. Factories provide methods to access all constructed objects or a single constructed object. A special case is the `good_{object}` method which provides access to a specially chosen object based on a selection implemented within the factory. The factories also provide additional methods to access the number of objects and other collective properties. All these factories require a call to the `run_factory` once per event to fill their lists of objects.

A special factories is the `event_factory`. This factory doesn't contain any objects, but contains individual event-related variables and provides methods to access these variables. The `met_factory` is similar. It simply provides methods to access MET-related variables directly. These factories do not require a call to `run_factory`.

<a name="helpers"/>

## Helpers

The `include` directory also contains headers providing many useful functions. 
- ACWeighter.h provides methods for accessing AC reweighting coefficients for JHU samples. These can then be stored in output TTrees.
- CLParser.h provides the basic command-line parsing capabilities used by plugins
- LumiReweightingStandAlone.h provides helper functions for reading pileup corrections
- slim_tree.h contains the output TTree and defines how it will be filled
- swiss_army_class.h contains useful information with no other home. This includes: luminosities, cross-sections, embedded tracking scale factors, and more.
