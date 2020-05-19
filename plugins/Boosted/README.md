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
    python scripts/hadder.py -p path/to/output
    ```

Not all options are shown in these instructions. A list of possible options for any python script can be seen using
```
python {script_name}.py --help
```
Plots can be made using `scripts/produce_histograms.py` with `scripts/autoplot.py`.

## File Locations

Here are the locations of all currently used files on the LPC. Directory names should be obvious
- Boosted analysis: /store/user/abdollah/SVFit/*/v2_Hadd
