#ifndef StSpectraCut_hh
#define StSpectraCut_hh

//#ifdef SOLARIS 
// #ifndef false
//  typedef int bool;
//  #define false 0
//  #define true 1
// #endif
//#endif

#include "StEvent.h"

class StSpectraCut {

 private:

 protected:

  double mLowLimit;
  double mHighLimit;

  StSpectraCut(double lowLimit, double highLimit);

 public:

  virtual  ~StSpectraCut() {};
 
  virtual bool satisfiesCut(StTrack* track, StEvent* event);
  virtual bool satisfiesCut(StV0Vertex* v0, StEvent* event);

};

#endif
