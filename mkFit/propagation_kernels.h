#ifndef _PROPAGATION_KERNELS_H_
#define _PROPAGATION_KERNELS_H_

#include "GPlex.h"

__device__ void propagation_fn(
    GPlexHV &msPar,
    GPlexLV &inPar, GPlexQI &inChg,
    GPlexLV &outPar, GPlexLL &errorProp,
    GPlexLS &outErr, int n, int N);

void propagation_wrapper(const cudaStream_t& stream,
    GPlexHV& msPar,
    GPlexLV& inPar, GPlexQI& inChg,
    GPlexLV& outPar, GPlexLL& errorProp,
    GPlexLS& outErr, 
    const int N);

void propagationForBuilding_wrapper(const cudaStream_t& stream,
    const GPlexLS& inErr, const GPlexLV& inPar,
    const GPlexQI& inChg, const float radius,
    GPlexLS& outErr, GPlexLV& outPar, 
    const int N);

__device__ void propagation_fn(
    GPlexHV &msPar,
    GPlexLV &inPar, GPlexQI &inChg,
    GPlexLV &outPar, GPlexLL &errorProp,
    GPlexLS &outErr, int n, int N);

__device__ void propagationForBuilding_fn(
    const GPlexLS &inErr, const GPlexLV &inPar,
    const GPlexQI &inChg, const float radius,
    GPlexLS &outErr, GPlexLV &outPar, 
    const int n, const int N);

#endif  // _PROPAGATION_KERNELS_H_
