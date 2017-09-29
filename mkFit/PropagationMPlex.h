#ifndef _propagation_mplex_
#define _propagation_mplex_

#include "Matrix.h"

inline void squashPhiMPlex(MPlexLV& par, const int N_proc)
{
  #pragma simd
  for (int n = 0; n < NN; ++n) {
    if (par(n, 4, 0) >= Config::PI) par(n, 4, 0) -= Config::TwoPI;
    if (par(n, 4, 0) < -Config::PI) par(n, 4, 0) += Config::TwoPI;
  }
}

inline void squashPhiMPlexGeneral(MPlexLV& par, const int N_proc)
{
  #pragma simd
  for (int n = 0; n < NN; ++n) {
    par(n, 4, 0) -= std::floor(0.5f*Config::InvPI*(par(n, 4, 0)+Config::PI)) * Config::TwoPI;
  }
}

void propagateLineToRMPlex(const MPlexLS &psErr,  const MPlexLV& psPar,
                           const MPlexHS &msErr,  const MPlexHV& msPar,
                                 MPlexLS &outErr,       MPlexLV& outPar,
                           const int      N_proc);

void propagateHelixToRMPlex(const MPlexLS &inErr,  const MPlexLV& inPar,
                            const MPlexQI &inChg,  const MPlexHV& msPar,
                                  MPlexLS &outErr,       MPlexLV& outPar,
                            const int      N_proc, const bool useParamBfield = false);

void propagateHelixToRMPlex(const MPlexLS& inErr,  const MPlexLV& inPar,
                            const MPlexQI& inChg,  const float    r,
                                  MPlexLS& outErr,       MPlexLV& outPar,
                            const int      N_proc);

void helixAtRFromIterativeCCSFullJac(const MPlexLV& inPar, const MPlexQI& inChg,
                                           MPlexLV& outPar, const MPlexQF &msRad,
                                           MPlexLL& errorProp,
                                     const int      N_proc);

void helixAtRFromIterativeCCS(const MPlexLV& inPar, const MPlexQI& inChg,
                                    MPlexLV& outPar, const MPlexQF &msRad,
                                    MPlexLL& errorProp,
                              const int      N_proc, const bool useParamBfield = false);

void helixAtRFromIterative(const MPlexLV& inPar, const MPlexQI& inChg, 
                                 MPlexLV& outPar, const MPlexQF &msRad, 
                                 MPlexLL& errorProp,
                           const int      N_proc, const bool useParamBfield = false);

void propagateHelixToZMPlex(const MPlexLS &inErr,  const MPlexLV& inPar,
                            const MPlexQI &inChg,  const MPlexHV& msPar,
                                  MPlexLS &outErr,       MPlexLV& outPar,
                            const int      N_proc, const bool useParamBfield = false);

void propagateHelixToZMPlex(const MPlexLS &inErr,  const MPlexLV& inPar,
                            const MPlexQI &inChg,  const float    z,
                                  MPlexLS &outErr,       MPlexLV& outPar,
                            const int      N_proc);

void helixAtZ(const MPlexLV& inPar,  const MPlexQI& inChg,
                    MPlexLV& outPar, const MPlexQF &msZ,
                    MPlexLL& errorProp,
              const int      N_proc, const bool useParamBfield = false);

void applyMaterialEffects(const MPlexQF &hitsRl, const MPlexQF& hitsXi, 
                                MPlexLS &outErr, MPlexLV& outPar,
                          const int      N_proc);

#endif
