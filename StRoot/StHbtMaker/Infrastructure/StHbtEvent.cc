/***************************************************************************
 *
 * $Id: StHbtEvent.cc,v 1.9 2000/08/31 22:31:31 laue Exp $
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *   HbtEvent is the "transient microDST"  Objects of this class are
 *   generated from the input data by a Reader, and then presented to
 *   the Cuts of the various active Analyses.
 *
 ***************************************************************************
 *
 * $Log: StHbtEvent.cc,v $
 * Revision 1.9  2000/08/31 22:31:31  laue
 * StHbtAnalysis: output changed (a little bit less)
 * StHbtEvent: new version, members for reference mult added
 * StHbtIOBinary: new IO for new StHbtEvent version
 * StHbtTypes: TTree typedef to StHbtTTree added
 * StHbtVertexAnalysis: overflow and underflow added
 *
 * Revision 1.8  2000/07/16 21:38:22  laue
 * StHbtCoulomb.cxx StHbtSectoredAnalysis.cxx : updated for standalone version
 * StHbtV0.cc StHbtV0.hh : some cast to prevent compiling warnings
 * StHbtParticle.cc StHbtParticle.hh : pointers mTrack,mV0 initialized to 0
 * StHbtIOBinary.cc : some printouts in #ifdef STHBTDEBUG
 * StHbtEvent.cc : B-Field set to 0.25Tesla, we have to think about a better
 *                 solution
 *
 * Revision 1.7  2000/05/25 21:54:16  laue
 * RotateZ implemented. Rotates momentum and helix around the z axis
 *
 * Revision 1.5  2000/02/18 21:32:23  laue
 * franksTrackCut changed. If mCharge is set to '0' there will be no cut
 * on charge. This is important for front-loaded cuts.
 *
 * copy constructor implemented for StHbtEvent, StHbtTrack and StHbtV0.
 *
 * franks1HistoD.cxx franks1HistoD.h franks2HistoD.cxx franks2HistoD.h
 * removed. We can now (CC5 on Solaris) use the versions (no D)
 *
 * Revision 1.4  1999/09/16 18:47:59  lisa
 * replace placeholder HbtV0Track stuff with Helens StHbtV0 classes
 *
 * Revision 1.3  1999/07/27 10:47:04  lisa
 * now works in dev on linux and solaris - mistake in deleting picoEvents fixed
 *
 * Revision 1.2  1999/07/19 14:24:05  hardtke
 * modifications to implement uDST
 *
 * Revision 1.1.1.1  1999/06/29 16:02:57  lisa
 * Installation of StHbtMaker
 *
 **************************************************************************/

#include "StHbtMaker/Infrastructure/StHbtEvent.hh"
#include "StHbtMaker/Base/StHbtTrackCut.h"
#include "StHbtMaker/Base/StHbtV0Cut.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"

//___________________
StHbtEvent::StHbtEvent(){
  mPrimVertPos[0]=-999.0;
  mPrimVertPos[1]=-999.0;
  mPrimVertPos[2]=-999.0;
  mTrackCollection = new StHbtTrackCollection;
  mV0Collection = new StHbtV0Collection;
}
//___________________
StHbtEvent::StHbtEvent(const StHbtEvent& ev, StHbtTrackCut* tCut, StHbtV0Cut* vCut){ // copy constructor with track and v0 cuts
  //cout << "StHbtEvent::StHbtEvent(const StHbtEvent& ev, StHbtTrackCut* tCut, StHbtV0Cut* vCut)" << endl;
  mEventNumber = ev.mEventNumber;
  mCtbMultiplicity = ev.mCtbMultiplicity;
  mZdcAdc[0] = ev.mZdcAdc[0];
  mZdcAdc[1] = ev.mZdcAdc[1];
  mTpcNhits = ev.mTpcNhits;
  mNumberOfTracks = ev.mNumberOfTracks;
  mNumberOfGoodTracks = ev.mNumberOfGoodTracks;
  mUncorrectedNumberOfPositivePrimaries = ev.mUncorrectedNumberOfPositivePrimaries;
  mUncorrectedNumberOfNegativePrimaries = ev.mUncorrectedNumberOfNegativePrimaries;
  mReactionPlane[0] = ev.mReactionPlane[0];
  mReactionPlane[1] = ev.mReactionPlane[1];
  mPrimVertPos = ev.mPrimVertPos;

  // create collections
  mTrackCollection = new StHbtTrackCollection;
  mV0Collection = new StHbtV0Collection;
  // copy track collection  
  for ( StHbtTrackIterator tIter=ev.mTrackCollection->begin(); tIter!=ev.mTrackCollection->end(); tIter++) {
      if ( !tCut || tCut->Pass(*tIter) ) {
	  //cout << " trackCut passed " << endl;
	  StHbtTrack* trackCopy = new StHbtTrack(**tIter);
	  mTrackCollection->push_back(trackCopy);
      }
  }
  // copy v0 collection
  for ( StHbtV0Iterator vIter=ev.mV0Collection->begin(); vIter!=ev.mV0Collection->end(); vIter++) {
      if ( !vCut || vCut->Pass(*vIter) ) {
	  StHbtV0* v0Copy = new StHbtV0(**vIter);
	  mV0Collection->push_back(v0Copy);
      }
  }
}
//___________________
StHbtEvent::~StHbtEvent(){
#ifdef STHBTDEBUG
  cout << " StHbtEvent::~StHbtEvent() " << endl;
#endif
  StHbtTrackIterator iter;
  for (iter=mTrackCollection->begin();iter!=mTrackCollection->end();iter++){
    delete *iter;
  }
  mTrackCollection->clear();
  delete mTrackCollection;
  //must do the same for the V0 collection
  StHbtV0Iterator V0iter;
  for (V0iter=mV0Collection->begin();V0iter!=mV0Collection->end();V0iter++){
    delete *V0iter;
  }
  mV0Collection->clear();
  delete mV0Collection;
}
//___________________
void StHbtEvent::RotateZ(const double angle){

  double field = 0.25;

  StHbtTrackIterator iter;
  StHbtV0Iterator V0iter;

  StPhysicalHelixD helix;
  StHbtThreeVector p;
  StHbtThreeVector o;
  double c;

  mReactionPlane[0] += angle;
  cout << " StHbtEvent::RotateZ(const double angle) - angle=" << angle << " rad    ";
  cout << angle / degree << " deg " << endl; 
  for (iter=mTrackCollection->begin();iter!=mTrackCollection->end();iter++){
      p = (*iter)->P();    p.rotateZ(angle);  (*iter)->SetP(p);
      p= (*iter)->Helix().momentum(field);
      o= (*iter)->Helix().origin();
      p.rotateZ(angle);
      o.rotateZ(angle);
      c= (*iter)->Helix().charge(field);
      helix = StPhysicalHelixD(p,o,field,c);
      (*iter)->SetHelix(helix);
  }
  for (V0iter=mV0Collection->begin();V0iter!=mV0Collection->end();V0iter++){
    p=(*V0iter)->decayVertexV0();  p.rotateX(angle);   (*V0iter)->SetdecayVertexV0(p);
    p=(*V0iter)->momV0();          p.rotateX(angle);   (*V0iter)->SetmomV0(p);    
    p=(*V0iter)->momPos();         p.rotateX(angle);   (*V0iter)->SetmomPos(p);    
    p=(*V0iter)->momNeg();         p.rotateX(angle);   (*V0iter)->SetmomNeg(p);
  }
}
