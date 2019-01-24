find ${2}/*.root -type f -size -15k | xargs rm
rm ${2}/*_amc_*.root

hadd ${2}/Data.root ${2}/data*
hadd ${2}/TTT.root ${2}/*_TTT_*.root
hadd ${2}/TTJ.root ${2}/*_TTJ_*.root

hadd ${2}/ZJ.root ${2}/*_ZJ_*.root
hadd ${2}/ZL.root ${2}/*_ZL_*.root
hadd ${2}/mc_ZTT.root ${2}/DY*ZTT*
hadd ${2}/embedded.root ${2}/embed${1}-*

hadd ${2}/W.root ${2}/WJets*_W_*.root

hadd ${2}/VVT.root ${2}/*_VVT_*.root
hadd ${2}/VVJ.root ${2}/*_VVJ_*.root

hadd ${2}/EWKZ.root ${2}/*_EWKZ_*.root

hadd ${2}/WH125.root ${2}/WPlus* ${2}/WMinus*

hadd -f ${2}/VBF125.root ${2}/VBF*.root
hadd -f ${2}/ZH125.root ${2}/ZH*.root
hadd ${2}/ggH125.root ${2}/ggH*

hadd ${2}/ggh_inc.root ${2}/ggh_*
hadd ${2}/vbf_inc.root ${2}/vbf_*
hadd ${2}/wh_inc.root ${2}/wh_*
hadd ${2}/zh_inc.root ${2}/zh_*

mkdir ${2}/originals
mv ${2}/*output*.root ${2}/originals
