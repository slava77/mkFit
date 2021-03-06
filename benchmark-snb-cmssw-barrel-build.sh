#! /bin/bash

sed -i 's/\/\/\#define PRINTOUTS_FOR_PLOTS/\#define PRINTOUTS_FOR_PLOTS/g' Config.h
patch < cmssw-config.patch

make -j 12

dir=/data/nfsmic/cerati/

for nth in 1 2 4 6 8 12 16 20 24
do
    echo "SNB CMSSW" nth=${nth} "BH (Barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr ${nth} >& log_SNB_CMSSW_Barrel_BH_NVU8int_NTH${nth}.txt
    echo "SNB CMSSW" nth=${nth} "STD (Barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-std --num-thr ${nth} >& log_SNB_CMSSW_Barrel_STD_NVU8int_NTH${nth}.txt
    echo "SNB CMSSW" nth=${nth} "CE (Barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-ce  --num-thr ${nth} --cloner-single-thread >& log_SNB_CMSSW_Barrel_CE_NVU8int_NTH${nth}.txt
done

sed -i 's/# USE_INTRINSICS := -DMPT_SIZE=1/USE_INTRINSICS := -DMPT_SIZE=XX/g' Makefile.config
for nvu in 1 2 4 8
do
    sed -i "s/MPT_SIZE=XX/MPT_SIZE=${nvu}/g" Makefile.config
    make clean
    make -j 12

    echo "SNB CMSSW" nvu=${nvu} "BH (Barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-bh  --num-thr 1 >& log_SNB_CMSSW_Barrel_BH_NVU${nvu}_NTH1.txt
    echo "SNB CMSSW" nvu=${nvu} "STD (Barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-std --num-thr 1 >& log_SNB_CMSSW_Barrel_STD_NVU${nvu}_NTH1.txt
    echo "SNB CMSSW" nvu=${nvu} "CE (Barrel)"
    ./mkFit/mkFit --read --file-name ${dir}/cmssw_100xTTbarPU35_polar_split_mock_noFWD.bin --cms-geom --cmssw-seeds --build-ce  --num-thr 1 --cloner-single-thread >& log_SNB_CMSSW_Barrel_CE_NVU${nvu}_NTH1.txt

    sed -i "s/MPT_SIZE=${nvu}/MPT_SIZE=XX/g" Makefile.config
done
sed -i 's/USE_INTRINSICS := -DMPT_SIZE=XX/# USE_INTRINSICS := -DMPT_SIZE=1/g' Makefile.config

patch -R < cmssw-config.patch
sed -i 's/\#define PRINTOUTS_FOR_PLOTS/\/\/\#define PRINTOUTS_FOR_PLOTS/g' Config.h

make clean

