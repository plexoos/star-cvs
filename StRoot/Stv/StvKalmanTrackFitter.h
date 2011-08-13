/// \File StvKalmanTrackFitter.h
/// \author Victor Perev 9/2010
#ifndef StvKalmanTrackFitter_HH
#define StvKalmanTrackFitter_HH
#include "StvTrackFitter.h"

/// \class StvKalmanTrackFitter
class StvTrack;
class StvNode;

class StvKalmanTrackFitter : public StvTrackFitter
{
public:
  StvKalmanTrackFitter();
  virtual ~StvKalmanTrackFitter(){;}	
  virtual  int Refit(StvTrack *trak,int dir,int mode=1);
  virtual  int Fit(const StvTrack *trak,const StvHit *vtx,StvNode *node);
           int Propagate(StvNode  *node,const StvNode *preNode,int dir);
  virtual  int Helix(StvTrack *trak,int mode);
  virtual void Clear(const char *opt="");
  

protected:
char mBeg[1];
char mEnd[1];

ClassDef(StvKalmanTrackFitter,0);
};


#endif
