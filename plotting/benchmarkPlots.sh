#! /bin/bash

## input
suite=${1:-"forPR"}

source xeon_scripts/common-variables.sh ${suite}
source xeon_scripts/init-env.sh

##### Make benchmark plots for each architecture #####
for archV in "SNB snb" "KNL knl" "SKL-SP skl-sp"
do
    echo ${archV} | while read -r archN archO
    do
	echo "Extract benchmarking results for" ${archN}
	python plotting/makeBenchmarkPlots.py ${archN} ${sample}
	
	echo "Make final plot comparing different build options for" ${archN}
	root -b -q -l plotting/makeBenchmarkPlots.C\(\"${archN}\",\"${sample}\",\"${suite}\"\)
	
	for build in "${meif_builds[@]}"
	do
	    echo ${!build} | while read -r bN bO
	    do
		echo "Extract multiple events in flight benchmark results for" ${bN} "on" ${archN}
		python plotting/makeMEIFBenchmarkPlots.py ${archN} ${sample} ${bN}
		
		echo "Make final plot comparing multiple events in flight for" ${bN} "on" ${archN}
		root -b -q -l plotting/makeMEIFBenchmarkPlots.C\(\"${archN}\",\"${sample}\",\"${bN}\"\)
	    done
	done
    done
done
