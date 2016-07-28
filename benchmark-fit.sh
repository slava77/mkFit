#! /bin/bash

sed -i 's/int nTracks = 20000/int nTracks = 1000000/g' Config.cc

make -j 8

dir=/data/nfsmic/${USER}/tmp

mkdir -p ${dir}
./mkFit/mkFit --write --file-name simtracks_10x1M.bin
mv simtracks_10x1M.bin ${dir}/

for nth in 1 3 7 21
do
echo "host" nth=${nth} "FIT"
./mkFit/mkFit --read --file-name ${dir}/simtracks_10x1M.bin --fit-std-only --num-thr ${nth} >& log_host_10x1M_FIT_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8
do
sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
make clean
make -j 8
echo "host" nvu=${nvu} "FIT"
./mkFit/mkFit --read --file-name ${dir}/simtracks_10x1M.bin --fit-std-only --num-thr 1 >& log_host_10x1M_FIT_NVU${nvu}_NTH1.txt
sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

sed -i 's/int nTracks = 1000000/int nTracks = 20000/g' Config.cc
make clean
make -j 8
