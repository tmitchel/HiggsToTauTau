# KSU Binary Classifier
This repository contains all the code necessary to take input TTrees, train a binary classifier, and store the classifier discriminant for any process. The process is broken into 3 scripts corresponding to the 3 distinct steps in the process.

1. Preprocess the input TTrees to massage input data into a suitable format
2. Train a binary classifier and store the model
3. Apply the classifier to all processes and store the discriminator value

## 1.) Preprocessing
The script responsible for preprocessing the data is aptly named `preprocess.py`. This script will read in all provided ROOT files and output a Pandas DataFrame. A basic selection is applied during preprocessing to remove events that are not of interest. The input variables in remaining events are then scaled to have zero mean and unit variance. The output DataFrame is used for training the classifier, but it is also very useful for data exploration in a Jupyter Notebook. Examples are provided in the `notebooks` directory. 

The preprocessing script allows users to combine any permutation of two decay channels (et and mt) through the use of flags. The command below will create a DataFrame named `testData.hdf5` containing events from both decay channels. 

```
python preprocess.py --el-input root_files/etau --mu-input root_files/mutau -o testData -c vbf
```

The `-c` flag is used to choose the selection applied to events [vbf, boosted]. The output file is stored in the `datasets`  Once the output DataFrame is produced, it can be loaded into a Jupyter Notebook to do some exploration. Otherwise, move directly into training a classifier.

## 2.) Training
`train.py` is used to train a binary classifier provided a single 'signal' process and a single 'background' process. 

By default, the classifier will use the 7 MELA input variables, mjj, higgs_pT, and m_sv as input and build a neural network with a single hidden layer containing 20 nodes (2 * n_variables). Each node uses a sigmoid as an activation function and the binary cross-entropy is used as a loss function. 

Data will be split into training, validation, and testing datasets. 10% of the events will immediately be removed for testing. From the remaining events, a 75/25 training to validation split will be used. In order to prevent overtraining, the validation loss will be monitored and training will end if the value converges for 50 epochs. 

Input samples can be provided from the command-line. All possible options can be shown with `python train.py -h`. In the example below, a network trained to separate VBF125 from ZTT is trained using the DataFrame produced in the preprocessing step. The output model is named `outputModel.hdf5` and stored in the `models` directory.

```
python train.py --signal VBF125 --background ZTT --input datasets/testData.hdf5 --model outputModel -c vbf
```

Training the classifier also results in multiple output pdfs in the `plots` directory. These pdfs contain useful information about the training process as well as important checks against overtraining. The `logs` directory is also filled with log files that can be viewed in TensorBoard.

## 3.) Classifying
Once the model is trained, the `classify.py` script is used to apply the classifier and store the results in a copy of the input ROOT files. The output will be stored in a new branch named `NN_disc`. To complete the examples, apply the classifier trained in step 2 to all ROOT files in the etau directory.

```
python classify.py --treename etau_tree --input-vbf datasets/testData.hdf5 --dir root_files/etau --model-vbf models/outputModel.hdf5 -o OutputLocation
```

The output files will be stored in the directory `output_files/OutputLocation`.

## Other Scripts

- condor_classify.py : `classify.py` script slightly modified to work when submitted to condor. Currently broken.
- wisc_condor_submit.py : script to submit classification jobs to condor. Currently broken.
- correlation.py : script to produce a 2D correlation plot for given variables. The code is short and self-explanatory
- formSuper.py : script to produce a "super discriminant" from NN_disc and m_sv

## To Do
- Generalize the training so that any input variables can be used from the command-line
- Generalize the training so that any network architecture can be used the command-line
- Add some testing for TravisCI