# Anomalous Couplings

The anomalous couplings tau analysis is generally conducted using the Wisconsin cluster so commands should use the /hdfs and /nfs_scratch areas.


## Quick Start

1. Follow installation instructions
2. Compile the appropriate plugin. For example, to compile the 2018 analyzer for the muon+tau channel AC analysis, use the following command
    ```
    make ac-mt-2018
    ```
    This will produce a binary named `analyze2018_mt` in the `bin` directory.
3. Use the python automation script (`auto_ac_wisc.py`) to processes all files in a directory with the provided executable. Output files will appear in `Output/trees` unless condor is used. For a list of options, use
    ```
    python auto_ac_wisc.py --help
    ```
4. Hadd output files (will descend into subdirectories to handle separate directories per systematic)
    ```
    python scripts/hadder.py -p path/to/output -a ac
    ```
5. Reweighting AC samples
    ```
    python scripts/ac_reweighting.py --input path/to/output
    ```
6. Run neural network preprocessing, training, and classifying

    a. Preprocess nominal files to fit distributions for scaling (output in `Output/datasets`)
    
    ```
    python neural-network/preprocess.py -m path/to/mutau -e path/to/etau -o {output name}
    ```
    b. Train the network if one is not available (output in `Output/models`)
    
    ```
    python neural-network/train.py -m {model name} -i path/to/dataset -s reweighted_qqH_htt_0PM125 -b embed
    ```
    c. Run classifier on all input (nominal and systematics)
    
    ```
    python neural-network/classify.py -m {model name} -i path/to/data -o {local output name} -d path/to/inputs --move {path to hdfs area if copying outputs there}
    ```
5. Fill fake fractions and produce `jetFakes.root` file containing the fake tau estimate
    ```
    python scripts/fill_fake_fractions.py -i path/to/nn-output/nominal -y {year} -t {year name} -s {suffix} --syst
    ```
6. Fill embedded contamination systematic, if needed
    ```
    python scripts/embedded_contamination.py --input path/to/input
    ```
7. Fill datacards for combine (output in `Output/templates`)
    ```
    python scripts/produce_datacards.py -e -y {year} -i path/to/input --suffix {chosen suffix} -c {config scenario}
    ```

Not all options are shown in these instructions. A list of possible options for any python script can be seen using
```
python {script_name}.py --help
```
Plots can be made using `scripts/produce_histograms.py` with `scripts/autoplot.py`.

## File Locations

Here are the locations of all currently used files on the Wisconsin cluster. Directory names should be obvious
- AC analysis: /hdfs/store/user/tmitchel/legacy-v6

Copy of files on the LPC:
- AC analysis: /store/user/tmitchel/anomalous-couplings_legacy-v6
