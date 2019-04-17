pushd ${1}
for i in *.root; do
  cp $i ${i#$2};
done
popd
