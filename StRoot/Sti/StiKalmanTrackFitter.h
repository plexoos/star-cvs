#ifndef StiKalmanTrackFitter_H
#define StiKalmanTrackFitter_H
#include "StiTrackFitter.h"
#include "StiKalmanTrackFitterParameters.h"
class StiTrack;
class EditableParameters;

///Class implements a kalman track fitter 
///Based on the abstract interface StiTrackFitter
///Uses the fitting parameters carried by StiKalmanTrackFitterParameters
class StiKalmanTrackFitter : public StiTrackFitter, public Named, public Described
{
 public:
  
  StiKalmanTrackFitter() {}
  virtual ~StiKalmanTrackFitter() {}
  virtual Int_t fit(StiTrack * track, Int_t direction);
  static  void setDebug(Int_t m = 0) {_debug = m;}
  static  Int_t  debug() {return _debug;}

 protected:
  static  Int_t _debug;
};

#endif
