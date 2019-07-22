find ${2}/*.root -type f -size -15k | xargs rm
rm ${2}/*tth125*
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

hadd ${2}/wh125.root ${2}/wplus125_powheg*.root ${2}/wminus125_powheg*.root
hadd -f ${2}/vbf125.root ${2}/vbf125_powheg*.root
hadd -f ${2}/zh125.root ${2}/zh125_powheg*.root
hadd ${2}/ggh125.root ${2}/ggh125_powheg*

hadd ${2}/ggh_inc.root ${2}/ggh125_JHU*
hadd ${2}/vbf_inc.root ${2}/vbf125_JHU*
hadd ${2}/wh_inc.root ${2}/wh125_JHU*
hadd ${2}/zh_inc.root ${2}/zh125_JHU*

mv ${2}/*output*.root ${2}/originals

cp ${2}/originals/*madgraph* ${2}
cp ${2}/originals/*minlo* ${2}
