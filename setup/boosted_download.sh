pushd $CMSSW_BASE/src/HiggsToTauTau/data

wget https://raw.githubusercontent.com/abdollah110/BoostedHTT/boost/interface/MuSF/EfficienciesAndSF_RunBtoF_Nov17Nov2017.root
wget https://raw.githubusercontent.com/abdollah110/BoostedHTT/boost/interface/pileup-hists/RunBCDEF_SF_ISO.root
wget https://raw.githubusercontent.com/abdollah110/BoostedHTT/boost/interface/pileup-hists/RunBCDEF_SF_ID.root
wget https://raw.githubusercontent.com/abdollah110/BoostedHTT/boost/interface/pileup-hists/Data_nPU_new.root
wget https://raw.githubusercontent.com/abdollah110/BoostedHTT/boost/interface/pileup-hists/Trigger_EfficienciesAndSF_RunBtoF.root

popd