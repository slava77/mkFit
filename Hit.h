#ifndef _hit_
#define _hit_

#include "Matrix.h"
#include <atomic>

namespace Config {
  static constexpr const unsigned int nPhiPart = 63;
  static constexpr const unsigned int nPhiFactor = 10;  // nPhiPart/2pi
  static constexpr const unsigned int nEtaPart = 10;
};

inline unsigned int getPhiPartition(float phi){
  //assume phi is between -PI and PI
  //  if (!(fabs(phi)<TMath::Pi())) std::cout << "anomalous phi=" << phi << std::endl;
  const float phiPlusPi  = phi+TMath::Pi();
  const unsigned int bin = phiPlusPi*Config::nPhiFactor;
  return bin;
}

inline unsigned int getEtaPartition(float eta, float etaDet){
  const float etaPlusEtaDet  = eta + etaDet;
  const float twiceEtaDet    = 2.0*etaDet;
  const unsigned int bin     = (etaPlusEtaDet * Config::nEtaPart) / twiceEtaDet;  // ten bins for now ... update if changed in Event.cc
  return bin;
}

inline float getRad2(float x, float y){
  return x*x + y*y;
}

inline float getPhi(float x, float y){
  return std::atan2(y,x); 
}

inline float getEta(float r, float z) {
  const float theta = atan2(r,z);
  return -1. * log( tan(theta/2.) );
}

inline float getRadErr2(float x, float y, float exx, float eyy, float exy){
  return (x*x*exx + y*y*eyy + 2*x*y*exy) / getRad2(x,y);
}  

inline float getPhiErr2(float x, float y, float exx, float eyy, float exy){
  const float rad2   = getRad2(x,y);
  //  const float dphidx = -y/rad2;
  //  const float dphidy =  x/rad2;
  //  return dphidx*dphidx*exx + dphidy*dphidy*eyy + 2*dphidx*dphidy*exy;
  return (y*y*exx + x*x*eyy - 2*x*y*exy)/(rad2*rad2);
}

inline float getEtaErr2(float x, float y, float z, float exx, float eyy, float ezz, float exy, float exz, float eyz){
  const float rad2   = getRad2(x,y);
  const float detadx = -x/(rad2*std::sqrt(1+rad2/(z*z)));
  const float detady = -y/(rad2*std::sqrt(1+rad2/(z*z)));
  const float detadz = 1.0/(z*std::sqrt(1+rad2/(z*z)));
  return detadx*detadx*exx + detady*detady*eyy + detadz*detadz*ezz + 2*detadx*detady*exy + 2*detadx*detadz*exz + 2*detady*detadz*eyz;
}

struct MCHitInfo {
  MCHitInfo() : mcHitID_(mcHitIDCounter_++) {}
  MCHitInfo(unsigned int track, unsigned int layer, unsigned int ithlayerhit)
    : mcTrackID_(track), layer_(layer), ithLayerHit_(ithlayerhit), mcHitID_(mcHitIDCounter_++) {}

  unsigned int mcTrackID_;
  unsigned int layer_;
  unsigned int ithLayerHit_;
  unsigned int mcHitID_;

  static std::atomic<unsigned int> mcHitIDCounter_;
};

struct MeasurementState
{
public:
  SVector3 parameters;
  SMatrixSym33 errors;
};

class Hit
{
public:
  Hit(){}

  Hit(const MeasurementState& state) : state_(state) {}

  Hit(const SVector3& position, const SMatrixSym33& error)
  {
    state_.parameters=position;
    state_.errors=error;
  }

  Hit(const SVector3& position, const SMatrixSym33& error, unsigned int itrack, unsigned int ilayer, unsigned int ithLayerHit){
    mcHitInfo_.mcTrackID_ = itrack;
    mcHitInfo_.layer_ = ilayer;
    mcHitInfo_.ithLayerHit_ = ithLayerHit;
    state_.parameters=position;
    state_.errors=error;
  }

  Hit(const SVector3& position, const SMatrixSym33& error, const MCHitInfo& mcHitInfo)
    : mcHitInfo_(mcHitInfo)
  {
    state_.parameters=position;
    state_.errors=error;
  }

  ~Hit(){}

  const SVector3&  position()  const {return state_.parameters;}
  const SMatrixSym33& error()  const {return state_.errors;}
  const SVector3& parameters() const {return state_.parameters;}
  float r() const {return std::sqrt(getRad2(state_.parameters.At(0),state_.parameters.At(1)));}
  float z() const {return state_.parameters.At(2);}
  float phi() const {return getPhi(state_.parameters.At(0),state_.parameters.At(1));}
  float eta() const {return getEta(r(),z());}

  const MeasurementState& measurementState() const {
    return state_;
  }

  const MCHitInfo& mcHitInfo() const {return mcHitInfo_;}
  unsigned int mcTrackID() const {return mcHitInfo_.mcTrackID_;}
  unsigned int layer() const {return mcHitInfo_.layer_;}
  unsigned int ithLayerHit() const {return mcHitInfo_.ithLayerHit_;}
  unsigned int hitID() const {return mcHitInfo_.mcHitID_;}

private:
  MeasurementState state_;
  MCHitInfo mcHitInfo_;
};

typedef std::vector<Hit> HitVec;

#endif
