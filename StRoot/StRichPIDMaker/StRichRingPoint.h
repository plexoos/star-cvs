/**********************************************************
 * $Id: StRichRingPoint.h,v 2.3 2002/02/01 16:16:37 lasiuk Exp $
 *
 * Description:
 *  
 *
 *  $Log: StRichRingPoint.h,v $
 *  Revision 2.3  2002/02/01 16:16:37  lasiuk
 *  include float.h to allow for use of FLT_MAX (gcc7.2)
 *
 *  Revision 2.2  2001/02/07 16:01:06  lasiuk
 *  no units with mass of particle in c'tor
 *  momentum loss (uses local copy) taken into account
 *  inline functions where possible
 *
 *  Revision 2.1  2000/09/29 01:35:38  horsley
 *  Many changes, added StRichRingHits, StRichMcSwitch, TpcHitvecUtilities
 *  Modified the StRichCalculator, StRichTracks, StRichMCTrack, StRichRingPoint
 *
 *  Revision 1.2  2000/05/19 19:06:11  horsley
 *  many revisions here, updated area calculation ring calc, ring, tracks , etc...
 *
 *  Revision 1.1  2000/04/03 19:36:08  horsley
 *  initial revision
 **********************************************************/

#ifndef StRichRingPoint_h
#define StRichRingPoint_h
#include <float.h>

#include "StRichTrack.h"
#include "StParticleDefinition.hh"
#include "StRichRingDefinition.h" 
#include "StRichMaterialsDb.h"
#include "StRrsMaker/StRichGeometryDb.h"
#include "StThreeVectorF.hh"

class  StRichRingPoint {
public:
  
    StRichRingPoint(StRichTrack* track, StRichRingDefinition type);
    ~StRichRingPoint();

  double rotatedFunction(double psi);
  bool   getPoint(double psi, StThreeVectorF& point);
  void   setPoint(StThreeVectorF& sPoint);
  void   setParticleType(StParticleDefinition* particle);  
  StParticleDefinition* getParticleType();  
  StRichTrack* getTrack(); 
  double getMeanPathInRadiator();
  double getMeanPathInQuartz();
  
private:

  // ring parameters
  StParticleDefinition* mParticle;
  StRichRingDefinition mRingType;

  double mInnerWavelength;
  double mOuterWavelength;
  double mMeanWavelength;
  
  double mMeanPathInRadiator;  
  double mMeanPathInQuartz;

  // track parameters
  double mTrackTheta;
  double mTrackPhi;
  double mPsi;

  double mMomentum;
  double mMass;
  double mBeta;
  double mCher;

  double mTrackCosTheta;
  double mTrackSinTheta;
  double mTrackCosPhi;

  double mTrackSinPhi;
  double mTrackTanTheta;
  double mTanCher;

  StThreeVectorF mImpactPoint;
  bool mFastEnough;
  StRichTrack* mTrack;

  // detector parameters
  double mDepthRad;
  double mMeanDepthRad;
  double mDepthQuar;

  double mDepthProx;
  double mIndexRad[3];
  double mIndexQuartz[3];
  double mIndexMeth[3];

  // detector parameters
  StRichMaterialsDb* richMaterialsDb;
  StRichGeometryDb* richGeometryDb;
  // ray propagation
  double mRadiatorAngle;
  double mQuartzAngle;
  double mMethaneAngle;
  double mPsiPrime;
  StThreeVectorF  mRefractedAway;
  bool status;

  // rotated function
  StThreeVectorF tempPoint;
  StThreeVectorF minPoint;

};

inline StRichTrack* StRichRingPoint::getTrack() { return mTrack; }
inline void StRichRingPoint::setPoint(StThreeVectorF& sPoint) { minPoint = sPoint; }
inline StParticleDefinition* StRichRingPoint::getParticleType() { return mParticle;}
inline double StRichRingPoint::getMeanPathInRadiator() { return mMeanPathInRadiator;}
inline double StRichRingPoint::getMeanPathInQuartz() {  return mMeanPathInQuartz;}

#endif





