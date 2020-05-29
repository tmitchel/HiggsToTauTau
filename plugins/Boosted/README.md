# Boosted Taus

The boosted tau analysis is generally conducted using the LPC so commands should use eos and /uscmst1b_scratch/lpc1/3DayLifetime areas.

## Quick Start

1. Follow installation instructions
2. Compile the appropriate plugin. For example, to compile the 2017 analyzer for the muon+tau channel boosted analysis, use the following command
    ```
    make boost-mt-2017
    ```
    This will produce a binary named `boost_mt2017` in the `bin` directory.
3. Use the python automation script (`auto_boost_lpc.py`) to processes all files in a directory with the provided executable. Output files will appear in `Output/trees` unless condor is used. For a list of options, use
    ```
    python auto_boost_lpc.py --help
    ```
4. Hadd output files (will descend into subdirectories to handle separate directories per systematic)
    ```
    python scripts/hadder.py -p path/to/output -a boosted
    ```
5. Fill jetFakes background using OSSS ratio method
    ```
    python scripts/fill_osss.py -i path/to/output
    ```
Not all options are shown in these instructions. A list of possible options for any python script can be seen using
```
python {script_name}.py --help
```

## Plotting

Plots can be made using `scripts/produce_histograms.py` and `scripts/boost_plotter.py`.

1. Configure the plots you want filled in `configs/plottting.json`. Provide a name for the config, a list of variables with binning, and a zvar (not used currently).
    ```json
    "boost-test": {
        "variables": {
            "mu_pt": [40, 0, 400],
            "t1_pt": [40, 0, 400],
            "higgs_pT": [40, 0, 400],
            "m_sv": [40, 0, 200],
            "vis_mass": [40, 0, 200]
        },
        "zvar": ["D0_VBF", [0, 0.25, 0.5, 0.75, 1.0]]
    }
    ```
2. Produce a file contianing histograms defined in this config. The output will be contained in the `Output/histograms` directory.
    ```
    python scripts/produce_histograms.py -y 2017 -i path/to/files/NOMINAL/merged -d test -c boost-test
    ```
    
3. Produce stack plots from the output file. The plot will appear in `Output/plots`
    ```
    python scripts/boost_plotter.py -i Output/histograms/htt_mt_ztt_noSys_fa3_2017_test.root -y 2017 -c mt_inclusive -v m_sv -l "m_{SV}" -s 2.2
    ```
    


## File Locations

Here are the locations of all currently used files on the LPC. Directory names should be obvious
- Boosted analysis: /store/user/tmitchel/boosted-higgs-v1/
