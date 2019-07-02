find ${2}/*.root -type f -size -15k | xargs rm
rm ${2}/*_amc_*.root
rm ${2}/tth125_*.root
mkdir ${2}/originals

hadd ${2}/Data.root ${2}/data*
hadd ${2}/TTT.root ${2}/*_TTT_*.root
hadd ${2}/TTJ.root ${2}/*_TTJ_*.root

hadd ${2}/ZJ.root ${2}/*_ZJ_*.root
hadd ${2}/ZL.root ${2}/*_ZL_*.root
hadd ${2}/ZTT.root ${2}/*_ZTT_*.root
# hadd ${2}/embedded.root ${2}/embed${1}-*

hadd ${2}/W.root ${2}/*_W_*.root

hadd ${2}/VVT.root ${2}/*_VVT_*.root
hadd ${2}/VVJ.root ${2}/*_VVJ_*.root

hadd ${2}/wh125.root ${2}/wplus125* ${2}/wminus125*

hadd -f ${2}/vbf125.root ${2}/vbf125_powheg*.root
hadd -f ${2}/zh125.root ${2}/zh125_powheg*.root
hadd ${2}/ggh125.root ${2}/ggh125_powheg*

hadd ${2}/ggh_inc.root ${2}/ggh125_JHU*
hadd ${2}/vbf_inc.root ${2}/vbf125_JHU*
hadd ${2}/wh_inc.root ${2}/wh125_JHU*
hadd ${2}/zh_inc.root ${2}/zh125_JHU*

# hadd ${2}/ggh_madgraph_Maxmix_twojet.root ${2}/ggH_Maxmix_TwoJet_madgraph_ggH125*_output.root
# mv ${2}/ggH_Maxmix_TwoJet_madgraph_ggH125*_output.root ${2}/originals
# 
# hadd ${2}/ggh_madgraph_PS_twojet.root ${2}/ggH_PS_TwoJet_madgraph_ggH125*_output.root
# mv ${2}/ggH_PS_TwoJet_madgraph_ggH125*_output.root ${2}/originals
# 
# hadd ${2}/ggh_madgraph_twojet.root ${2}/ggH_TwoJet_madgraph_ggH125*_output.root
# 
mv ${2}/*output*.root ${2}/originals
