/**********************************************************
 * $Id: StRichTrack.cxx,v 2.6 2000/11/07 14:13:24 lasiuk Exp $
 *
 * Description:
 *  
 *
 *  $Log: StRichTrack.cxx,v $
 *  Revision 2.6  2000/11/07 14:13:24  lasiuk
 *  add possibility of .4*px/pz correction to the track extrapolation
 *
 *  Revision 2.10  2000/11/28 19:18:54  lasiuk
 *  Include protection/error warning if no MIP
 *
 *  Revision 2.9  2000/11/25 12:27:42  lasiuk
 *  implement algorithm for finding MIP
 *
 *  Revision 2.8  2000/11/21 19:46:09  lasiuk
 *  px/pz correction uncommented
 *
 *  Revision 2.7  2000/11/14 22:31:51  lasiuk
 *  associated MIP (commented)
 *  return copy instead of reference
 *
 *  Revision 2.6  2000/11/07 14:13:24  lasiuk
 *  add possibility of .4*px/pz correction to the track extrapolation
 *
 *  Revision 2.5  2000/11/01 17:43:10  lasiuk
 *  default arguments initialization in c'tor.  Addition of init() member
 *  function to handle generic DB initialization and removal of virtual keyword
 *
 *  Revision 2.4  2000/10/19 01:13:23  horsley
 *  added member functions to StRichPIDMaker to make cuts on hits, tracks, events.
 *  added normal distance sigma cut on hits, quartz and radiator pathlengths
 *  for individual photons, modified minimization routine to correct boundary
 *  problems
 *
 *  Revision 2.3  2000/10/03 19:26:01  horsley
 *  fixed error in StRichTrack correct member function, now returns bool.
 *
 *  Revision 2.2  2000/09/29 17:55:51  horsley
 *  fixed bug in Minimization routine, included StMagF stuff (commented out)
 *  changed StRichRingPoint  HUGE_VALUE   ---> MAXFLOAT for default value
 *
 *  Revision 2.1  2000/09/29 01:35:38  horsley
 *  Many changes, added StRichRingHits, StRichMcSwitch, TpcHitvecUtilities
 *  Modified the StRichCalculator, StRichTracks, StRichMCTrack, StRichRingPoint
 *
 *  Revision 1.3  2000/06/16 02:37:12  horsley
 *  many additions, added features to pad plane display (MIPS, rings, etc)
 *  along with Geant info. Added StMcRichTrack. Modified access to hit collection.
 *
 *  Revision 1.2  2000/05/19 19:06:11  horsley
 *  many revisions here, updated area calculation ring calc, ring, tracks , etc...
 *
 *  Revision 1.1  2000/04/03 19:36:09  horsley
 *  initial revision
 **********************************************************/

#include <iostream.h>
#include <math.h>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

#include <utility> //bool is defined here in SUN
#ifndef ST_NO_NAMESPACES
using std::pair;
using std::adjacent_difference;
using std::max_element;
using std::sort;
#endif
#endif

#include "StParticleTypes.hh"
#include "StParticleDefinition.hh"

#include "StRichTrack.h"
#include "StEventTypes.h"
#include "StRichMaterialsDb.h"
#include "SystemOfUnits.h"
#include "StRichRingPoint.h"

#include "StRichPidTraits.h"

#include "StRrsMaker/StRichGeometryDb.h"


#ifndef ST_NO_NAMESPACES
using namespace units;
// For sorting the possible MIP candiates
//
typedef pair<StRichHit*, double> candidate;
bool operator<(const candidate &a, const candidate &b) {return (a.second<b.second);};


StRichTrack::StRichTrack()
    : mStTrack(0),  mPidTrait(0),  mAssociatedMIP(0), mMagneticField(0)
{/* nopt */}

StRichTrack::StRichTrack(StThreeVectorF mom, StThreeVectorF imp)
    : mPidTrait(0),  mAssociatedMIP(0)
{
    //
#ifdef RICH_WITH_L3_TRACKS
  mL3Track  = 0; 
#endif  

  this->init();
  
  // define system parameters
  this->setMomentum(mom);
  this->setImpactPoint(imp);
}

#ifdef RICH_WITH_L3_TRACKS
StRichTrack::StRichTrack(globalTrack *track, double magField)
    : mStTrack(0),              mPidTrait(0),    mAssociatedMIP(0),
      mMagneticField(magField), mL3Track(track),
{

    //
       
    // define system parameters
    this->init();
    
    // need to include mwpc gap correction, 
    // origin is located in center of gap, not on padplane --> 2 mm
    double anodeDistanceToPadPlane = myGeometryDb->anodeToPadSpacing();
    StRichLocalCoordinate edgeOfRichRadiator_Local(0.0,
						   0.0,
						   myGeometryDb->proximityGap() +
						   myGeometryDb->quartzDimension().z() +
						   myGeometryDb->radiatorDimension().z());
    
    // MIP is measured at anode position
    StRichLocalCoordinate richAnodeWirePlane_Local(0.,
						   0.,
						   0.+anodeDistanceToPadPlane);	
    StGlobalCoordinate edgeOfRichRadiator_Global;
    (*coordinateTransformation)(edgeOfRichRadiator_Local,
				edgeOfRichRadiator_Global);
    
    StGlobalCoordinate richAnodeWirePlane_Global;
    (*coordinateTransformation)(richAnodeWirePlane_Local,
				richAnodeWirePlane_Global);
    

    // L3 Stuff
    double x0=track->r0*cos(track->phi0);
    double y0=track->r0*sin(track->phi0);
    double z0=track->z0;

    //momentum
    double px=track->pt * cos(track->psi);
    double py=track->pt * sin(track->psi);
    double pz=track->pt * track->tanl;

    StPhysicalHelixD mHelix(StThreeVectorD(px,py,pz),
			    StThreeVectorD(x0,y0,z0),
			    magField*kilogauss,
			    track->q);    
    
    StThreeVectorD rr(edgeOfRichRadiator_Global.position().x(),
		      edgeOfRichRadiator_Global.position().y(),
		      edgeOfRichRadiator_Global.position().z());
    
    StThreeVectorD rp(richAnodeWirePlane_Global.position().x(),
		      richAnodeWirePlane_Global.position().y(),
		      richAnodeWirePlane_Global.position().z());
    
    double mPathLengthAtRadiator  = mHelix.pathLength(rr,richNormal);
    double mPathLengthAtPadPlane  = mHelix.pathLength(rp,richNormal);
    
    StThreeVectorD tpcMom;
    if (mPathLengthAtRadiator<10e10 && mPathLengthAtRadiator>0) {
	tpcMom = mHelix.momentumAt(mPathLengthAtRadiator,magField*kilogauss);	
    }

    StThreeVector<double> tpcMomentum(tpcMom.x(),
				      tpcMom.y(),
				      tpcMom.z());    
    
    // do the momentum vector rotation here
    StThreeVector<double> richLocalMomentum;
    momentumTransformation->localMomentum(tpcMomentum,richLocalMomentum);
    
    // impact point on radiator
    StGlobalCoordinate globalImpactPoint(mHelix.x(mPathLengthAtRadiator),
					 mHelix.y(mPathLengthAtRadiator),
					 mHelix.z(mPathLengthAtRadiator));
    
    StRichLocalCoordinate richTransformedImpactPoint(-999,-999,-999);
    (*coordinateTransformation)(globalImpactPoint,richTransformedImpactPoint);
//     double xCorrection = 0.4*(mMomentumAtPadPlane.x()/mMomentumAtPadPlane.z());
    double xCorrection = 0.;
    // correction to the x position of the impact point
    // If this correction is really due to the effect of
    // the offline software cluster finder artificially
    // shifting the x position (z Global) of the track,
    // this may not be appropriate in the L3 unless we
    // use the offline hits as well
    //
    double xCorrection = 0.4*(mMomentumAtPadPlane.x()/mMomentumAtPadPlane.z());
    //double xCorrection = 0.;
    richTransformedImpactPoint.position().setX(richTransformedImpactPoint.position().x()-xCorrection);

    StThreeVectorD ppMomentum;
    
    // pad plane intersection (projected MIP)
    StGlobalCoordinate    globalImpactPointAtAnodeWirePlane(-999,-999,-999);
    StRichLocalCoordinate localImpactPointAtAnodeWirePlane(-999,-999,-999);  

    if (mPathLengthAtPadPlane < 10e10  && mPathLengthAtRadiator>0) {

	globalImpactPointAtAnodeWirePlane.position().setX(mHelix.x(mPathLengthAtPadPlane));
	globalImpactPointAtAnodeWirePlane.position().setY(mHelix.y(mPathLengthAtPadPlane));
	globalImpactPointAtAnodeWirePlane.position().setZ(mHelix.z(mPathLengthAtPadPlane));	

	(*coordinateTransformation)(globalImpactPointAtAnodeWirePlane,
				    localImpactPointAtAnodeWirePlane);

	localImpactPointAtAnodeWirePlane.position()
	    .setX(localImpactPointAtAnodeWirePlane.position().x()-xCorrection);

	ppMomentum =  mHelix.momentumAt(mPathLengthAtPadPlane,magField*kilogauss);
    }

    StThreeVector<double> padPlaneMomentum(ppMomentum.x(),ppMomentum.y(),ppMomentum.z());
    momentumTransformation->localMomentum(padPlaneMomentum,mMomentumAtPadPlane);

    this->setMomentum(richLocalMomentum);
    this->setUnCorrectedMomentum(richLocalMomentum);
    this->setImpactPoint(richTransformedImpactPoint.position());
    this->setProjectedMIP(localImpactPointAtAnodeWirePlane.position());
    this->setPathLength(mPathLengthAtRadiator);
}
#endif

StRichTrack::StRichTrack(StTrack* tpcTrack, double magField)
    : mStTrack(tpcTrack),       mPidTrait(0),  mAssociatedMIP(0),
      mMagneticField(magField)
{
    this->init();

    //
    // need to include mwpc gap correction, 
    // origin is located in center of gap, not on padplane --> 2 mm
    // make correction for gap distance of 4 mm
    //
    StRichLocalCoordinate
	edgeOfRichRadiator_Local(0., 0., myGeometryDb->proximityGap() +
				 myGeometryDb->quartzDimension().z() +
				 myGeometryDb->radiatorDimension().z());

    //
    // MIP is measured at anode position
    //
    StRichLocalCoordinate
	richAnodeWirePlane_Local(0., 0., myGeometryDb->anodeToPadSpacing());

    //
    // CTB in local coord.
    //
    StRichLocalCoordinate CTB_Local(0,0,240.0-214.0);

    StGlobalCoordinate edgeOfRichRadiator_Global;
    (*coordinateTransformation)(edgeOfRichRadiator_Local,edgeOfRichRadiator_Global);
    
    StGlobalCoordinate richAnodeWirePlane_Global;
    (*coordinateTransformation)(richAnodeWirePlane_Local,
				richAnodeWirePlane_Global);
    
    StGlobalCoordinate CTB_Global;
    (*coordinateTransformation)(CTB_Local,CTB_Global);

    
    StPhysicalHelixD  mHelix = tpcTrack->geometry()->helix();    

    mRadiatorGlobal.setX(edgeOfRichRadiator_Global.position().x());
    mRadiatorGlobal.setY(edgeOfRichRadiator_Global.position().y());
    mRadiatorGlobal.setZ(edgeOfRichRadiator_Global.position().z());
		     
 
    mAnodeGlobal.setX(richAnodeWirePlane_Global.position().x());
    mAnodeGlobal.setY(richAnodeWirePlane_Global.position().y());
    mAnodeGlobal.setZ(richAnodeWirePlane_Global.position().z());
		
    mCTBGlobal.setX(CTB_Global.position().x());
    mCTBGlobal.setY(CTB_Global.position().y());
    mCTBGlobal.setZ(CTB_Global.position().z());
    
    float mPathLengthAtRadiator  = mHelix.pathLength(mRadiatorGlobal,mRichNormal);
    float mPathLengthAtPadPlane  = mHelix.pathLength(mAnodeGlobal,mRichNormal);
    float mPathLengthAtCTB       = mHelix.pathLength(mCTBGlobal,mRichNormal);
    
    StThreeVectorF tpcMom;
    if (mPathLengthAtRadiator<10e10  && mPathLengthAtRadiator>0) {
      tpcMom = mHelix.momentumAt(mPathLengthAtRadiator,mMagneticField*kilogauss); 
    }
    
    StThreeVector<double> tpcMomentum(tpcMom.x(), tpcMom.y(), tpcMom.z());
    
    // do the momentum vector rotation here
    StThreeVector<double> tempRichLocalMomentum(0,0,0);
    momentumTransformation->localMomentum(tpcMomentum,tempRichLocalMomentum);
    
    StThreeVectorF richLocalMomentum(tempRichLocalMomentum.x(),
				     tempRichLocalMomentum.y(),
				     tempRichLocalMomentum.z());

    // impact point in CTB
    StThreeVectorF localCTBImpactPoint(-999,-999,-999);
    if (mPathLengthAtCTB>0 && mPathLengthAtCTB<10e10) {
      StGlobalCoordinate CTBGlobal(mHelix.x(mPathLengthAtCTB),
				   mHelix.y(mPathLengthAtCTB),
				   mHelix.z(mPathLengthAtCTB));

      StRichLocalCoordinate richTransformedImpactPoint(-999,-999,-999);
      (*coordinateTransformation)(CTBGlobal,richTransformedImpactPoint);

      localCTBImpactPoint.setX(richTransformedImpactPoint.position().x());
      localCTBImpactPoint.setY(richTransformedImpactPoint.position().y());
      localCTBImpactPoint.setZ(richTransformedImpactPoint.position().z());
    }

    // impact point on radiator
    StGlobalCoordinate globalImpactPoint(mHelix.x(mPathLengthAtRadiator),
					 mHelix.y(mPathLengthAtRadiator),
					 mHelix.z(mPathLengthAtRadiator));

    StRichLocalCoordinate richTransformedImpactPoint(-999,-999,-999);
    (*coordinateTransformation)(globalImpactPoint,richTransformedImpactPoint);
        
    StThreeVectorF ppMomentum;

    //
    // pad plane intersection (projected MIP)
    //
    StGlobalCoordinate    globalImpactPointAtAnodeWirePlane(-999,-999,-999);
    StRichLocalCoordinate localImpactPointAtAnodeWirePlane(-999,-999,-999);  
    
    if (mPathLengthAtPadPlane < 10e10  && mPathLengthAtPadPlane>0) {
      globalImpactPointAtAnodeWirePlane.position().setX(mHelix.x(mPathLengthAtPadPlane));
      globalImpactPointAtAnodeWirePlane.position().setY(mHelix.y(mPathLengthAtPadPlane));
      globalImpactPointAtAnodeWirePlane.position().setZ(mHelix.z(mPathLengthAtPadPlane));
      
      (*coordinateTransformation)(globalImpactPointAtAnodeWirePlane,
				  localImpactPointAtAnodeWirePlane);
      
      ppMomentum =  mHelix.momentumAt(mPathLengthAtPadPlane,mMagneticField*kilogauss);
    }
    
    StThreeVector<double> padPlaneMomentum(ppMomentum.x(),ppMomentum.y(),ppMomentum.z());
    StThreeVector<double> tempMomentum;
    momentumTransformation->localMomentum(padPlaneMomentum,tempMomentum);
    mMomentumAtPadPlane.setX(tempMomentum.x());
    mMomentumAtPadPlane.setY(tempMomentum.y());
    mMomentumAtPadPlane.setZ(tempMomentum.z());
    
    float lastHitDCA = mHelix.distance(tpcTrack->detectorInfo()->lastPoint());

    //
    // Topology Map (functionality should probably be put in Topology Map)
    //
    const StTrackTopologyMap& tMap = mStTrack->topologyMap();
    vector<int> rows;
    vector<int> emptyRows(1,0);
    for (int i=1; i<46; ++i) {
	if (tMap.hasHitInRow(kTpcId,i)) { rows.push_back(i);} 
	else { emptyRows.push_back(i);} 
    }
    emptyRows.push_back(46);
    
    this->setMaxGap(maxSeq(rows)); // max number of continuous empty rows
    this->setMaxChain(maxSeq(emptyRows)); // max number of continuous used rows
    this->setFirstRow(rows.front());
    this->setLastRow(rows.back());
//     double xCorrection = 0.4*(mMomentumAtPadPlane.x()/mMomentumAtPadPlane.z());
    double xCorrection = 0.;
    this->setUnCorrectedMomentum(richLocalMomentum);

    //
    // This is the radiator impact position
    // --> the x component must be shifted by:
    //  .4*px/pz
    //
    double xCorrection = 0.4*(mMomentumAtPadPlane.x()/mMomentumAtPadPlane.z());
    //double xCorrection = 0.;
    
    StThreeVectorF tempHit(richTransformedImpactPoint.position().x() - xCorrection,
			   richTransformedImpactPoint.position().y(),
			   richTransformedImpactPoint.position().z());
    
    this->setImpactPoint(tempHit);

    //
    // Likewise the MIP position must be corrected
    // for the same effect
    //
    StThreeVectorF tempMip(localImpactPointAtAnodeWirePlane.position().x() -xCorrection,
			   localImpactPointAtAnodeWirePlane.position().y(),
			   localImpactPointAtAnodeWirePlane.position().z());

    this->setProjectedMIP(tempMip);
    this->setProjectedCTB(localCTBImpactPoint);
    this->setPathLength(mPathLengthAtRadiator);
    this->setLastHitDCA(lastHitDCA);
    this->setLastHit(tpcTrack->detectorInfo()->lastPoint());
}
vector<StRichRingHit*>&
void StRichTrack::init()
{
    myGeometryDb  = StRichGeometryDb::getDb();  
    myMaterialsDb = StRichMaterialsDb::getDb();  
    coordinateTransformation = StRichCoordinateTransform::getTransform(myGeometryDb);
    momentumTransformation   = StRichMomentumTransform::getTransform(myGeometryDb);  

    mRichNormal.setX(myGeometryDb->normalVectorToPadPlane().x());
    mRichNormal.setY(myGeometryDb->normalVectorToPadPlane().y());
    mRichNormal.setZ(myGeometryDb->normalVectorToPadPlane().z());
}

vector<StRichRingHit*>
StRichTrack::getRingHits( StParticleDefinition* part) {

    if ( (part ==  StPionMinus::instance()) || (part == StPionPlus::instance())) {
	return mPionList; 
    }
  
    else if ( (part ==  StKaonMinus::instance()) || (part == StKaonPlus::instance())) {
	return mKaonList; 
    }
    
    else if ( (part ==  StProton::instance()) || (part == StAntiProton::instance())) {
	return mProtonList; 
    }
  
    vector<StRichRingHit*> tempvec(0);
    tempvec[0]=0;
    return tempvec;
}


void StRichTrack::clearHits() {

    for (size_t l=0;l<mPionList.size();l++) {
	delete mPionList[l];
	mPionList[l] = 0;
    }
    mPionList.clear();
    mPionList.resize(0);
  
    for (size_t l=0;l<mKaonList.size();l++) {
	delete mKaonList[l];
	mKaonList[l] =0;
    }
    mKaonList.clear();
    mKaonList.resize(0);
  
    for (size_t l=0;l<mProtonList.size();l++) {
	delete mProtonList[l];
	mProtonList[l] = 0;
    }
    mProtonList.clear();
    mProtonList.resize(0); 
}


void  StRichTrack::addHit(StRichHit* hit, double dist, double sigma, 
			  double angle, double radPath, 
			  double quaPath, StParticleDefinition* part) {
  
  if ( (part ==  StPionMinus::instance()) || (part == StPionPlus::instance())) {
    mPionList.push_back(new StRichRingHit(hit,angle,dist,sigma,radPath,quaPath)); 
  }
  
  else if ( (part ==  StKaonMinus::instance()) || (part == StKaonPlus::instance())) {
    mKaonList.push_back(new StRichRingHit(hit,angle,dist,sigma,radPath,quaPath)); 
  }
  
  else if ( (part ==  StProton::instance()) || (part == StAntiProton::instance())) {
    mProtonList.push_back(new StRichRingHit(hit,angle,dist,sigma,radPath,quaPath));  
  }
}


StRichTrack::~StRichTrack() {
  clearHits();
  
}




bool StRichTrack::correct() {

  if (!mStTrack) return false;
  
  StThreeVectorF tempMip(-999,-999,-999);
  if (mAssociatedMIP) { tempMip = mAssociatedMIP->local();}
  else return false;

  StThreeVectorF residual = tempMip-mProjectedMIP;
  if (residual.perp()>3.0*centimeter) { return false;}
  
  

  // store original values
  setUnCorrectedProjectedMIP(getProjectedMIP());
  setUnCorrectedImpactPoint(getImpactPoint());
  setUnCorrectedMomentum(getMomentum());


  // get momentum, x at assumed interaction point in CTB
  StPhysicalHelixD mHelix     = mStTrack->geometry()->helix();
  float mPathLengthToCTB      = mHelix.pathLength(mCTBGlobal,mRichNormal);
  if (mPathLengthToCTB>10000  || mPathLengthToCTB<0) return false;
  
  StThreeVectorF CTBPoint(mHelix.x(mPathLengthToCTB),mHelix.y(mPathLengthToCTB),mHelix.z(mPathLengthToCTB));
  StThreeVectorF globalMomentum = mHelix.momentumAt(mPathLengthToCTB, mMagneticField*kilogauss);

  float initialTheta = globalMomentum.theta();
  float initialPhi   = globalMomentum.phi();

  float thetaRange = 5.0*degree;
  float phiRange = 10.0*degree;
  
  float thetaTrials=100.0;
  float phiTrials=100.0;
  
  float thetaStart=initialTheta-thetaRange;
  float thetaFinish=initialTheta+thetaRange;

  float phiStart=initialPhi-phiRange;
  float phiFinish=initialPhi+phiRange;

  float deltaTheta=thetaRange/thetaTrials;
  float deltaPhi=phiRange/phiTrials;

  StThreeVectorF correctedMomentum(0,0,0);
  StThreeVectorF correctedProjectedMIP(0,0,0);
  StThreeVectorF correctedResidual = residual;
  StThreeVectorF tempMomentum(0,0,0);
  
  for (float theta=thetaStart;theta<thetaFinish;theta=theta+deltaTheta) {
    for (float phi=phiStart;phi<phiFinish;phi=phi+deltaPhi) {
      float p = globalMomentum.mag();
      tempMomentum.setX(p*sin(theta)*cos(phi));
      tempMomentum.setY(p*sin(theta)*sin(phi));
      tempMomentum.setZ(p*cos(theta));
    
      StPhysicalHelixD tempHelix(tempMomentum,CTBPoint,mMagneticField*kilogauss,mStTrack->geometry()->charge());
      float path = tempHelix.pathLength(mAnodeGlobal,mRichNormal);
      
      if (path>0 && path<10000) { 
	StGlobalCoordinate globalPadPlane(tempHelix.x(path),tempHelix.y(path),tempHelix.z(path));
	
	StRichLocalCoordinate localPadPlane;
	(*coordinateTransformation)(globalPadPlane,localPadPlane);
	StThreeVectorF tempLocal(localPadPlane.position().x(),
				 localPadPlane.position().y(),
				 localPadPlane.position().z());
	
	StThreeVectorF tempResidual = tempLocal-tempMip;
	
	if (tempResidual.perp()<correctedResidual.perp() && 
	    fabs(tempResidual.x()) < 1.1*fabs(tempResidual.y()) &&
	    fabs(tempResidual.x()) > 0.9*fabs(tempResidual.y())) {
	  correctedMomentum     = tempMomentum;
	  correctedResidual     = tempResidual;
	  correctedProjectedMIP = tempLocal;
	}
      }
    }
  }
  

  if (correctedMomentum.mag()>0) {
    // impact point on radiator
    StPhysicalHelixD tempHelix(correctedMomentum,CTBPoint,mMagneticField*kilogauss,mStTrack->geometry()->charge());
    float path = tempHelix.pathLength(mRadiatorGlobal,mRichNormal);
  
    if (path>0 && path<10000) {
      StGlobalCoordinate globalImpactPoint(tempHelix.x(path),tempHelix.y(path),tempHelix.z(path));
      StRichLocalCoordinate richTransformedImpactPoint(-999,-999,-999);
      (*coordinateTransformation)(globalImpactPoint,richTransformedImpactPoint);
      
      StThreeVectorF impact(richTransformedImpactPoint.position().x(),
			    richTransformedImpactPoint.position().y(),
			    richTransformedImpactPoint.position().z());
      
      // do the momentum vector rotation here
      StThreeVector<double> tempRichLocalMom(0,0,0);
      StThreeVector<double> tempRichGlobalMom(correctedMomentum.x(),correctedMomentum.y(),correctedMomentum.z());
      
      momentumTransformation->localMomentum(tempRichGlobalMom,tempRichLocalMom);
      StThreeVectorF tempRichLocalMomentum(tempRichLocalMom.x(),tempRichLocalMom.y(),tempRichLocalMom.z());  
      StThreeVectorF normalVector(0,0,-1);
      



      setProjectedMIP(correctedProjectedMIP);
      setImpactPoint(impact);

  }
  
    double adcCut=200;
}
    if (hits) {

	StSPtrVecRichHitConstIterator hitIndex;
	for (hitIndex = hits->begin(); hitIndex != hits->end(); hitIndex++) { 
	    testThisResidual = ((*hitIndex)->local() - mProjectedMIP).perp();      

	    //
	    // should look at ePhotoElectron or eMip flag (StRichHitFlag)
	    //
	    if (testThisResidual<smallestResidual && (*hitIndex)->charge()>adcCut) {
		smallestResidual = testThisResidual;   
		mAssociatedMIP   = *hitIndex;
	    }
    // RICH pad plane MIP 
    
    for (hitIndex = hits->begin(); hitIndex != hits->end(); hitIndex++) { 
// 	break;

// 	}
//     }


	cout << highestAmplitude << endl;
	    //cout << "StRichTrack::assignMIP()\n";
	    //cout << "\tTake smallest Residual" << endl;
	    highestAmplitude = candidateHits[0].first->charge();
	}
	    
    }
    //
    
    // In the best case the eMip and eAssociatedMip flag should be set
    //
    else {
	cout << "StRichTrack::associateMIP()\n";
	cout << "\tWARNING no assocatied MIP" << endl;
    }

}


StRichHit*       StRichTrack::getAssociatedMIP() { return mAssociatedMIP;}
StTrack*         StRichTrack::getStTrack()       { return mStTrack;}


double StRichTrack::getUnCorrectedTheta() { return mUnCorrectedTheta;}
double StRichTrack::getUnCorrectedPhi()   { return mUnCorrectedPhi;}
double StRichTrack::getLastHitDCA()       { return mLastHitDCA;}
double StRichTrack::getPathLength()       { return mPath;}
double StRichTrack::getTheta()            { return mTheta;}
double StRichTrack::getPhi()              { return mPhi;}
double StRichTrack::getZVertex(){
#ifdef RICH_WITH_L3_TRACKS
    globalTrack* track = this->getL3Track();
    if(track)
	return track->z0-track->tanl*track->r0*cos(track->psi-track->phi0);
#endif
    return -999;
}


int StRichTrack::getMaxGap()   { return mMaxGap;}
int StRichTrack::getMaxChain() { return mMaxChain;}
int StRichTrack::getFirstRow() { return mFirstRow;}
int StRichTrack::getLastRow()  { return mLastRow;}
int StRichTrack::maxSeq(vector<int>& seqrows) {
    vector<int> diffs(seqrows.size());
    adjacent_difference(seqrows.begin(), seqrows.end(), diffs.begin());
    vector<int>::iterator max = max_element(diffs.begin()+1, diffs.end());
    return max!=diffs.end() ? *max : -999;
}

int StRichTrack::fastEnough(StParticleDefinition* particle) {
    double p = mMomentum.mag()*GeV;
    double m = particle->mass()*GeV;
    
    double indexOfRefraction1 = myMaterialsDb->indexOfRefractionOfC6F14At(219.999*nanometer);
    double indexOfRefraction2 = myMaterialsDb->indexOfRefractionOfC6F14At(169.0*nanometer);
        
    if ( p/sqrt(p*p + m*m) > (1./indexOfRefraction1) && 
	 p/sqrt(p*p + m*m) > (1./indexOfRefraction2)) { 
	return 1;
    }
    
    return 0;
}

StThreeVectorF& StRichTrack::getUnCorrectedMomentum() { return mUnCorrectedMomentum;}
StThreeVectorF& StRichTrack::getUnCorrectedProjectedMIP() { return mUnCorrectedProjectedMIP;}
StThreeVectorF& StRichTrack::getUnCorrectedImpactPoint() { return mUnCorrectedImpactPoint;}
StThreeVectorF& StRichTrack::getLastHit() { return mLastHit;}
StThreeVectorF& StRichTrack::getProjectedCTBPoint() { return mProjectedCTB;} 
StThreeVectorF& StRichTrack::getProjectedMIP() { return mProjectedMIP;}
StThreeVectorF& StRichTrack::getImpactPoint() { return mImpactPoint;}
StThreeVectorF& StRichTrack::getMomentum() { return mMomentum;}
StThreeVectorF& StRichTrack::getMomentumAtPadPlane() { return mMomentumAtPadPlane;}

StRichPidTraits* StRichTrack::getPidTrait() { return mPidTrait; }
void StRichTrack::addPidTrait(StRichPidTraits* trait) { mPidTrait = trait;}

void StRichTrack::setPathLength(double p) { mPath = p;}
void StRichTrack::setLastHitDCA(double lastdca)  {mLastHitDCA = lastdca;}
void StRichTrack::setMaxGap(int n)   { mMaxGap=n;}
void StRichTrack::setMaxChain(int n) { mMaxChain=n;}
void StRichTrack::setFirstRow(int n) { mFirstRow=n;}
void StRichTrack::setLastRow(int n)  { mLastRow=n;}
void StRichTrack::setProjectedCTB(StThreeVectorF& ctb) {mProjectedCTB = ctb;} 
void StRichTrack::setProjectedMIP(StThreeVectorF& mip) {mProjectedMIP = mip;}
void StRichTrack::setTheta(double the) {mTheta = the;}
void StRichTrack::setPhi(double phi) { mPhi = phi;}
void StRichTrack::setImpactPoint(StThreeVectorF& impact) { mImpactPoint = impact;}
void StRichTrack::setUnCorrectedTheta(double the) {mUnCorrectedTheta=the;}
void StRichTrack::setUnCorrectedPhi(double phi) {mUnCorrectedPhi=phi;}
void StRichTrack::setUnCorrectedImpactPoint(StThreeVectorF& point) {mUnCorrectedImpactPoint=point;}
void StRichTrack::setUnCorrectedProjectedMIP(StThreeVectorF& point) {mUnCorrectedProjectedMIP=point;}
void StRichTrack::setUnCorrectedMomentum(StThreeVectorF& point) {mUnCorrectedMomentum=point;}
void StRichTrack::setLastHit(StThreeVectorF hit) { mLastHit = hit;}

void StRichTrack::useUnCorrected() {
  setMomentum(mUnCorrectedMomentum);
  setImpactPoint(mUnCorrectedImpactPoint);
  setProjectedMIP(mUnCorrectedProjectedMIP);
}

void StRichTrack::setMomentum(StThreeVectorF& momentum) {
  mMomentum = momentum;
  StThreeVectorF normalVector(0,0,-1);
  if (mMomentum.mag()) {
    setTheta(acos(normalVector.dot(momentum)/momentum.mag()));}
  if (momentum.y() == 0 && momentum.x() == 0) {setPhi(0.0);}
  else setPhi(momentum.phi());
}




bool StRichTrack::isGood(StParticleDefinition* particle) {   
    StRichRingPoint OuterRing(this,eOuterRing); // Get Ring
    OuterRing.setParticleType(particle);
    StThreeVectorF otemp;
    
    // See If valid point at back of ring
    // cut:   changed from pi          ---->                 pi/2 
    //            very back                                 at least half ring is ok

    bool goodTrack = OuterRing.getPoint(M_PI/2.0,otemp);
    return goodTrack;
}

double StRichTrack::getExpectedNPhots(StParticleDefinition* particle) {
  
  double pathlengthInc6f14 = myGeometryDb->radiatorDimension().z()*cos(getTheta());
  double pathlengthInquartz = myGeometryDb->quartzDimension().z()*cos(getTheta());
  double pathlengthInmeth = myGeometryDb->proximityGap()*cos(getTheta());

  double particleBeta = mMomentum.mag()
    /sqrt(particle->mass()*particle->mass() + mMomentum.mag()*mMomentum.mag());
  
  
  double startLamda = 170.0;
  double endLamda   = 200.0;
  double nSamples   = 200;
  double deltaLamda = (endLamda-startLamda)/nSamples;
 
  double nphots=0;
  double rawNumber=0;
  double c6f14AbsCorrect=0;
  double quartzAbsCorrect=0;
  double methAbsCorrect=0;
  double csiQE = 0;
  
  for (double i=startLamda;i<endLamda;i=i+deltaLamda) {
    
    rawNumber = (1.0/(i*i*i))*(1.0 - 1.0/(particleBeta*particleBeta*myMaterialsDb->indexOfRefractionOfC6F14At(i*nanometer)*myMaterialsDb->indexOfRefractionOfC6F14At(i*nanometer)));
    
    c6f14AbsCorrect  = (exp(-(pathlengthInc6f14/myMaterialsDb->absorptionCoefficientOfC6F14At(i*nanometer))));
    quartzAbsCorrect = (exp(-(pathlengthInquartz/myMaterialsDb->absorptionCoefficientOfQuartzAt(i*nanometer))));
    methAbsCorrect   = (exp(-(pathlengthInmeth/myMaterialsDb->absorptionCoefficientOfMethaneAt(i*nanometer))));
    csiQE  = myMaterialsDb->quantumEfficiencyOfCsIAt(i*nanometer);
    nphots = nphots + rawNumber*c6f14AbsCorrect*quartzAbsCorrect*methAbsCorrect*csiQE;
  }
  
  return nphots/6.05e-9;
}
