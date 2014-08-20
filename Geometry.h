#ifndef _geometry_
#define _geometry_

#include <vector>
#include "USolids/include/VUSolid.hh"

class Geometry {
public:
  void AddLayer(const VUSolid* s) { solids_.push_back(s); }
  unsigned int CountLayers() const { return solids_.size(); }
  const VUSolid* Layer(unsigned int l) const { return solids_.at(l); }
  VUSolid::EnumInside Inside (const UVector3 &aPoint) const;
  const VUSolid* InsideWhat(const UVector3 &aPoint) const;
  double  SafetyFromInside ( const UVector3 &aPoint, bool aAccurate=false) const;
  double  SafetyFromOutside( const UVector3 &aPoint, bool aAccurate=false) const;
private:
  std::vector<const VUSolid*> solids_;
};
#endif
