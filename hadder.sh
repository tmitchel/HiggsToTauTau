find ${2}/*.root -type f -size -15k | xargs rm
rm ${2}/*tth125*
mkdir ${2}/originals

hadd ${2}/Data.root ${2}/data*
hadd ${2}/TTT.root ${2}/*_TTT_*.root
hadd ${2}/TTJ.root ${2}/*_TTJ_*.root

hadd ${2}/ZJ.root ${2}/*_ZJ_*.root
hadd ${2}/ZL.root ${2}/*_ZL_*.root
hadd ${2}/ZTT.root ${2}/*_ZTT_*.root
hadd ${2}/embedded.root ${2}/embed${1}-*

# hadd ${2}/EWKZ.root ${2}/EWKZ*.root
mv  ${2}/EWKZ2*.root ${2}/originals

# hadd ${2}/W_with_EWK.root ${2}/*_W_*.root
hadd ${2}/W.root ${2}/WJ*_W_*.root

hadd ${2}/VVT.root ${2}/*_VVT_*.root
hadd ${2}/VVJ.root ${2}/*_VVJ_*.root

hadd ${2}/wh125_powheg.root ${2}/wplus125_powheg*.root ${2}/wminus125_powheg*.root
hadd -f ${2}/vbf125_powheg.root ${2}/vbf125_powheg*.root
hadd -f ${2}/zh125_powheg.root ${2}/zh125_powheg*.root
hadd -f ${2}/ggh125_powheg.root ${2}/ggh125_powheg*

hadd ${2}/ggh125_JHU.root ${2}/ggh125_JHU_*nom-decay*
hadd ${2}/vbf125_JHU.root ${2}/vbf125_JHU_*nom-decay*
hadd ${2}/wh125_JHU.root ${2}/wh125_JHU_*nom-decay*
hadd ${2}/zh125_JHU.root ${2}/zh125_JHU_*nom-decay*

hadd ${2}/ggh125_madgraph.root ${2}/ggh125_madgraph_two_*nom-decay*
hadd ${2}/vbf125_madgraph.root ${2}/vbf125_madgraph_*

mv ${2}/*output*.root ${2}/originals

# cp ${2}/originals/*madgraph* ${2}
# cp ${2}/originals/*minlo* ${2}
