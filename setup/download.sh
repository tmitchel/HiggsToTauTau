#!/bin/bash

pushd ${CMSSW_BASE}/src/ltau_analyzers/data

# scale factors
wget https://github.com/danielwinterbottom/LegacyCorrectionsWorkspace/raw/legacy_newrepo/output/htt_scalefactors_legacy_2016.root
wget https://github.com/danielwinterbottom/LegacyCorrectionsWorkspace/raw/legacy_newrepo/output/htt_scalefactors_legacy_2017.root
wget https://github.com/danielwinterbottom/LegacyCorrectionsWorkspace/raw/legacy_newrepo/output/htt_scalefactors_legacy_2018.root

# Higgs pT reweighting for MadGraph
wget https://github.com/albertdow/CorrectionsWorkspace/raw/MG-ggh-dev/htt_scalefactors_2016_MGggh.root
wget https://github.com/albertdow/CorrectionsWorkspace/raw/MG-ggh-dev/htt_scalefactors_2017_MGggh.root

# Pileup files
cp /afs/hep.wisc.edu/home/tmitchel/public/pileup/*.root .

popd
