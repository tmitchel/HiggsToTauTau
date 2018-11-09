hadd Output/trees/Data.root Output/trees/data*
hadd Output/trees/TTT.root Output/trees/*_TTT_*.root
hadd Output/trees/TTJ.root Output/trees/*_TTJ_*.root

hadd Output/trees/ZJ.root Output/trees/*_ZJ_*.root
hadd Output/trees/ZL.root Output/trees/*_ZL_*.root
hadd Output/trees/mc_ZTT.root Output/trees/DY*ZTT*
hadd Output/trees/ZTT.root Output/trees/embed-*

hadd Output/trees/W.root Output/trees/WJet*_W_*.root
hadd Output/trees/W_v2.root Output/trees/*_W_*.root

hadd Output/trees/VV.root Output/trees/*_VV_*.root
hadd Output/trees/VVT.root Output/trees/*_VVT_*.root
hadd Output/trees/VVJ.root Output/trees/*_VVJ_*.root

hadd Output/trees/EWKZ.root Output/trees/*_EWKZ_*.root


hadd Output/trees/WH125.root Output/trees/WPlus* Output/trees/WMinus*

mv Output/trees/VBF125_output.root Output/trees/VBF125.root
mv Output/trees/ZHTauTau125_ZH125_output.root Output/trees/ZH125.root
hadd Output/trees/ggH125.root Output/trees/ggH125*
mv Output/trees/ttH125_output.root Output/trees/ttH125.root

mkdir Output/trees/originals
mv Output/trees/*output*.root Output/trees/originals
