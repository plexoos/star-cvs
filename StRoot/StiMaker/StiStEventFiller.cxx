/***************************************************************************
 *
 * $Id: StiStEventFiller.cxx,v 2.54 2005/02/25 17:43:15 perev Exp $
 *
 * Author: Manuel Calderon de la Barca Sanchez, Mar 2002
 ***************************************************************************
 *
 * $Log: StiStEventFiller.cxx,v $
 * Revision 2.54  2005/02/25 17:43:15  perev
 * StTrack::setKey(...StiTrack::getId()) now
 *
 * Revision 2.53  2005/02/17 23:19:03  perev
 * NormalRefangle + Error reseting
 *
 * Revision 2.52  2005/02/07 18:34:16  fisyak
 * Add VMC dead material
 *
 * Revision 2.51  2005/01/17 03:56:56  pruneau
 * change track container to vector
 *
 * Revision 2.50  2005/01/17 01:32:13  perev
 * parameters protected
 *
 * Revision 2.49  2004/12/21 20:46:00  perev
 * Cleanup. All known bugs fixed
 *
 * Revision 2.48  2004/12/02 22:14:53  calderon
 * Only fill the fitTraits.chi2[1] data member for primaries.
 * It holds node->getChi2() from the innerMostHitNode, which will be the
 * vertex for primaries.
 *
 * Revision 2.47  2004/12/02 04:18:06  pruneau
 * chi2[1] now set to incremental chi2 at inner most hit or vertex
 *
 * Revision 2.46  2004/12/01 15:35:46  pruneau
 * removed throw and replaced with continue
 *
 * Revision 2.45  2004/11/08 15:34:16  pruneau
 * fix of the chi2 calculation
 *
 * Revision 2.44  2004/10/27 03:25:54  perev
 * Version V3V
 *
 * Revision 2.43  2004/10/26 06:45:41  perev
 * version V2V
 *
 * Revision 2.42  2004/10/14 02:21:34  calderon
 * Updated code in StTrackDetectorInfo, now only increment the reference count
 * for globals, not for primaries.  So fillTrackDetectorInfo changed to reflect
 * this.
 *
 * Revision 2.41  2004/10/01 01:13:51  calderon
 * Added bug fix from Marco:
 * flag%100 -> flag/100.
 *
 * Revision 2.40  2004/08/17 20:04:28  perev
 * small leak fixed, delete physicalHelix,originD
 *
 * Revision 2.39  2004/08/17 04:53:05  calderon
 * When filling fit traits for primary tracks, set the new flag
 * mPrimaryVertexUsedInFit.
 *
 * Revision 2.38  2004/08/10 14:21:13  calderon
 * Use the firstHit from the dynamic_cast, to avoid a compiler warning
 * for an unused variable.
 *
 * Revision 2.37  2004/08/06 22:23:29  calderon
 * Modified the code to use the setNumberOfxxxPoints(unsigned char,StDetectorId)
 * methods of StTrack, StTrackDetectorInfo, StTrackFitTraits, and to use
 * the maxPointCount(unsigned int detId) method of StiKalmanTrack.
 *
 * Revision 2.36  2004/08/06 02:29:20  andrewar
 * Modifed call to getMaxPointCount
 *
 * Revision 2.35  2004/08/05 05:25:25  calderon
 * Fix the assignment of the first point for primaries.  Now,
 * the logic for both globals and primaries is that the first
 * point is the first element of the stHits() vector that
 * can actually be casted to an StHit (the vertex will fail this test,
 * all other hits coming from detectors will satisfy it).
 *
 * Revision 2.34  2004/07/30 18:49:18  calderon
 * For running in production, Yuri's dEdx Maker will fill the Pid Traits,
 * so the filling of Pid Traits in the filler is no longer needed:
 * it actually causes confusion because the V0 finders will loop over
 * the PID traits vector and find the first one, so they won't find
 * the trait created by the dEdx Maker.  It is best to just comment
 * out the filling of the Pid Traits here.
 *
 * Revision 2.33  2004/07/07 19:33:48  calderon
 * Added method fillFlags.  Flags tpc, tpc+svt (globals and primaries) and flags -x02 tracks with less than 5 total fit points
 *
 * Revision 2.32  2004/04/21 21:36:24  calderon
 * Correction in the comments about the encoded method.
 *
 * Revision 2.31  2004/03/31 00:27:29  calderon
 * Modifications for setting the fit points based on the chi2<chi2Max algorithm.
 * -Distinguish between points and fit points, so I added a function for each.
 * -Points is done as it was before, just counting the stHits for a given
 *  detector id.
 * -Fit points is done the same with the additional condition that each
 *  StiKalmanTrackNode has to satisfy the chi2 criterion.
 *
 * Revision 2.30  2004/03/29 00:52:20  andrewar
 * Added key value to StTrack fill. Key is simply the size of the
 * StTrackNode container at the time the track is filled.
 *
 * Revision 2.29  2004/03/23 23:12:36  calderon
 * Added an "accept" function to filter unwanted tracks from Sti into StEvent.
 * The current method just looks for tracks with a negative length, since
 * these were causing problems for the vertex finder (length was nan).  The
 * nan's have been trapped (one hopes!) in StiKalmanTrack, and for these
 * cases the return value is negative, so we can filter them out with a
 * simple length>0 condition.
 *
 * Revision 2.28  2004/03/19 19:33:23  andrewar
 * Restored primary filling logic. Now taking parameters at the
 * vertex for Primary tracks.
 *
 * Revision 2.27  2004/01/27 23:40:46  calderon
 * The filling of the impactParameter() for global tracks is done now
 * only after finding the vertex.  The
 * StPhysicalHelix::distance(StThreeVectorD) method is used for both globals
 * and primaries, the only difference is where the helix is obtained:
 * - globals - helix from StTrack::geometry(), which was filled from the
 *             innermost hit node, which should be a hit at the time.
 * - primaries - helix from innermost hit node, which should be the vertex
 *             at the time it is called.
 *
 * Revision 2.26  2003/12/11 03:44:29  calderon
 * set the length right again, it had dissappeared from the code...
 *
 * Revision 2.25  2003/11/26 04:02:53  calderon
 * track->getChi2() returns the sum of chi2 for all sti nodes.  In StEvent,
 * chi2(0) should be chi2/dof, so we need to divide by
 * dof=track->getPointCount()-5;
 *
 * Revision 2.24  2003/09/07 03:49:10  perev
 * gcc 3.2 + WarnOff
 *
 * Revision 2.23  2003/09/02 17:59:59  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.22  2003/08/21 21:21:56  andrewar
 * Added trap for non-finite dEdx. Added logic to fillGeometry so
 * info is for innerMostHitNode on a detector, not vertex (note:
 * Primaries only)
 *
 * Revision 2.21  2003/08/05 18:26:15  andrewar
 * DCA track update logic modified.
 *
 * Revision 2.20  2003/07/01 20:25:28  calderon
 * fillGeometry() - use node->getX(), as it should have been since the
 * beginning
 * impactParameter() - always use the innermos hit node, not just for globals
 * removed extra variables which are no longer used.
 *
 * Revision 2.19  2003/05/15 03:50:26  andrewar
 * Disabled call to filldEdxInfo for the SVT. Checks need to be
 * applied to make sure the detector is active before calculator
 * is called, but for the review filling this info is unnecessary.
 *
 * Revision 2.18  2003/05/14 00:04:35  calderon
 * The array of 15 floats containing the covariance matrix has a different
 * order in Sti than in StEvent.  In Sti the array is counted starting from
 * the first row, column go to next column until you hit the diagonal,
 * jump to next row starting from first column. In StEvent the array is
 * counted starting from the first row, column go to the next row until you
 * hit the end, jump to next column starting from diagonal.
 * The filling of the fitTraits was fixed to reflect this.
 *
 * Revision 2.17  2003/05/12 21:21:39  calderon
 * switch back to getting the chi2 from track->getChi2()
 * Covariance matrix is still obtained from node->get(), and the values
 * are not as expected in StEvent, so this will still need to change.
 *
 * Revision 2.16  2003/05/08 22:23:33  calderon
 * Adding a check for finiteness of node origin and node curvature.  If any
 * of the numbers is not finite, the code will abort().
 *
 * Revision 2.15  2003/04/29 18:48:52  pruneau
 * *** empty log message ***
 *
 * Revision 2.14  2003/04/29 15:28:10  andrewar
 * Removed hacks to get helicity right; switch now done at source
 * (StiKalmanTrackNode).
 *
 * Revision 2.13  2003/04/25 21:42:47  andrewar
 * corrected DCA bug and added temp fix for helicity problem. This will
 * have to be modified when the helicity convention in StiStKalmanTrack
 * is updated.
 *
 * Revision 2.12  2003/04/04 14:48:34  pruneau
 * *** empty log message ***
 *
 * Revision 2.11  2003/03/14 19:02:55  pruneau
 * various updates - DCA is a bitch
 *
 * Revision 2.10  2003/03/13 21:20:10  pruneau
 * bug fix in filler fixed.
 *
 * Revision 2.9  2003/03/13 18:59:44  pruneau
 * various updates
 *
 * Revision 2.8  2003/03/13 16:01:48  pruneau
 * remove various cout
 *
 * Revision 2.7  2003/03/13 15:15:52  pruneau
 * various
 *
 * Revision 2.6  2003/03/12 17:58:05  pruneau
 * fixing stuff
 *
 * Revision 2.5  2003/02/25 16:56:20  pruneau
 * *** empty log message ***
 *
 * Revision 2.4  2003/02/25 14:21:10  pruneau
 * *** empty log message ***
 *
 * Revision 2.3  2003/01/24 06:12:28  pruneau
 * removing centralized io
 *
 * Revision 2.2  2003/01/23 05:26:02  pruneau
 * primaries rec reasonable now
 *
 * Revision 2.1  2003/01/22 21:12:15  calderon
 * Restored encoded method, uses enums but stores the value in constructor
 * as a data member so bit operations are only done once.
 * Fixed warnings.
 *
 * Revision 2.0  2002/12/04 16:50:59  pruneau
 * introducing version 2.0
 *
 * Revision 1.21  2002/09/20 02:19:32  calderon
 * Quick hack for getting code for review:
 * The filler now checks the global Dca for the tracks and only fills
 * primaries when dca<3 cm.
 * Also removed some comments so that the production log files are not swamped
 * with debug info.
 *
 * Revision 1.20  2002/09/12 22:27:15  andrewar
 * Fixed signed curvature -> StHelixModel conversion bug.
 *
 * Revision 1.19  2002/09/05 05:47:36  pruneau
 * Adding Editable Parameters and dynamic StiOptionFrame
 *
 * Revision 1.18  2002/08/29 21:09:22  andrewar
 * Fixed seg violation bug.
 *
 * Revision 1.17  2002/08/22 21:46:00  pruneau
 * Made a fix to StiStEventFiller to remove calls to StHelix and StPhysicalHelix.
 * Currently there is one instance of StHelix used a calculation broker to
 * get helix parameters such as the distance of closest approach to the main
 * vertex.
 *
 * Revision 1.16  2002/08/19 19:33:00  pruneau
 * eliminated cout when unnecessary, made helix member of the EventFiller
 *
 * Revision 1.15  2002/08/12 21:39:56  calderon
 * Introduced fillPidTraits, which uses the values obtained from
 * Andrews brand new dEdxCalculator to create two instances of an
 * StTrackPidTraits object and pass it to the track being filled.
 *
 * Revision 1.14  2002/08/12 15:29:21  andrewar
 * Added dedx calculators
 *
 * Revision 1.13  2002/06/28 23:30:56  calderon
 * Updated with changes debugging for number of primary tracks added.
 * Merged with Claude's latest changes, but restored the tabs, othewise
 * cvs diff will not give useful information: everything will be different.
 *
 * Revision 1.12  2002/06/26 23:05:31  pruneau
 * changed macro
 *
 * Revision 1.11  2002/06/25 15:09:16  pruneau
 * *** empty log message ***
 *
 * Revision 1.10  2002/06/18 18:08:34  pruneau
 * some cout statements removed/added
 *
 * Revision 1.9  2002/06/05 20:31:15  calderon
 * remove some redundant statements, the call to
 * StTrackNode::addTrack()
 * already calls
 * track->SetNode(this), so I don't need to do it again
 *
 * Revision 1.8  2002/05/29 19:14:45  calderon
 * Filling of primaries, in
 * StiStEventFiller::fillEventPrimaries()
 *
 * Revision 1.7  2002/04/16 19:46:44  pruneau
 * must catch exception
 *
 * Revision 1.6  2002/04/16 13:11:30  pruneau
 * *** empty log message ***
 *
 * Revision 1.5  2002/04/09 16:03:13  pruneau
 * Included explicit extension of tracks to the main vertex.
 *
 * Revision 1.4  2002/04/03 16:35:03  calderon
 * Check if primary vertex is available in StiStEventFiller::impactParameter(),
 * if not, return DBL_MAX;
 *
 * Revision 1.3  2002/03/28 04:29:49  calderon
 * First test version of Filler
 * Currently fills only global tracks with the following characteristics
 * -Flag is set to 101, as most current global tracks are.  This is not
 * strictly correct, as this flag is supposed to mean a tpc only track, so
 * really need to check if the track has svt hits and then set it to the
 * appropriate flag (501 or 601).
 * -Encoded method is set with bits 15 and 1 (starting from bit 0).  Bit 1
 * means Kalman fit.
 *  Bit 15 is an as-yet unused track-finding bit, which Thomas said ITTF
 * could grab.
 * -Impact Parameter calculation is done using StHelix and the primary vertex
 * from StEvent
 * -length is set using getTrackLength, which might still need tweaking
 * -possible points is currently set from getMaxPointCount which returns the
 *  total, and it is not
 *  what we need for StEvent, so this needs to be modified
 * -inner geometry (using the innermostHitNode -> Ben's transformer ->
 *  StPhysicalHelix -> StHelixModel)
 * -outer geometry, needs inside-out pass to obtain good parameters at
 *  outermostHitNode
 * -fit traits, still missing the probability of chi2
 * -topology map, filled from StuFixTopoMap once StDetectorInfo is properly set
 *
 * This version prints out lots of messages for debugging, should be more quiet
 * when we make progress.
 *
 **************************************************************************/

//std
#include "Stiostream.h"
#include <algorithm>
#include <stdexcept>
using namespace std;

// SCL
#include "StPhysicalHelix.hh"
#include "StThreeVector.hh"
#include "StThreeVectorF.hh"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StTrackDefinitions.h"
#include "StTrackMethod.h"
#include "StDedxMethod.h"

//StEvent
#include "StPrimaryVertex.h"
#include "StEventTypes.h"
#include "StDetectorId.h"
#include "StHelix.hh"


#include "StEventUtilities/StEventHelper.h"
#include "StEventUtilities/StuFixTopoMap.cxx"
//Sti
#include "Sti/StiTrackContainer.h"
#include "Sti/StiKalmanTrack.h"
#include "Sti/StiKalmanTrackFitterParameters.h"
/////#include "Sti/StiGeometryTransform.h"
#include "Sti/StiDedxCalculator.h"

//StiMaker
#include "StiMaker/StiStEventFiller.h"

FILE *bgFile=0;


StiStEventFiller::StiStEventFiller() : mEvent(0), mTrackStore(0), mTrkNodeMap()
{
   bgFile = fopen("dbFile.txt","w");

  //temp, make sure we're not constructing extra copies...
  //cout <<"StiStEventFiller::StiStEventFiller()"<<endl;
  dEdxTpcCalculator.setFractionUsed(.6);
  dEdxSvtCalculator.setFractionUsed(.6);
  dEdxTpcCalculator.setDetectorFilter(kTpcId);
  dEdxSvtCalculator.setDetectorFilter(kSvtId);
  
  originD = new StThreeVectorD(0,0,0);
  physicalHelix = new StPhysicalHelixD(0.,0.,0.,*originD,-1);
 

  //mResMaker.setLimits(-1.5,1.5,-1.5,1.5,-10,10,-10,10);
  //mResMaker.setDetector(kSvtId);

  // encoded method = 16 bits = 12 finding and 4 fitting, Refer
  // to StTrackMethod.h and StTrackDefinitions.h in pams/global/inc/
  // and StEvent/StEnumerations.h
  // For the IT tracks use:
  // Fitting: kITKalmanFitId     (should be something like 7, but don't hardwire it)
  // Finding: tpcOther           (should be 9th LSB, or shift the "1" 8 places to the left, but also don't hardwire it) 
  // so need this bit pattern:
  // finding 000000010000     
  // fitting             0111 
  //               256  +   7 = 263;
  unsigned short bit = 1 << tpcOther;  // shifting the "1" exactly tpcOther places to the left
  mStiEncoded = kITKalmanFitId + bit; // adding that to the proper fitting Id

}

StiStEventFiller::~StiStEventFiller()
{
 delete physicalHelix; physicalHelix=0;
 delete originD;       originD      =0;
   cout <<"StiStEventFiller::~StiStEventFiller()"<<endl;
}

//Helper functor, gotta live some place else, just a temp. test of StiKalmanTrack::stHits() method
struct StreamStHit
{
  void operator()(const StHit* h) 
  {
    //cout << "DetectorId: " << (unsigned long) h->detector();
    if (const StTpcHit* hit = dynamic_cast<const StTpcHit*>(h)) 
      {
	cout <<hit->position() << " Sector: " << hit->sector() << " Padrow: " << hit->padrow() << endl;
      }
    else if (const StSvtHit* hit = dynamic_cast<const StSvtHit*>(h)) 
      {
	cout << hit->position() << " layer: " << hit->layer() << " ladder: " << hit->ladder()
	     << " wafer: " << hit->wafer() << " barrel: " << hit->barrel() << endl;
      }
    else 
      {	
	cout << hit->position() << endl;
      }
  }
};

/*! 
  Algorithm:
  Loop over all tracks in the StiTrackContainer, doing for each track:
  - Create a new global track and associated information (see below)
    and set its data members according to the StiKalmanTrack,
    can be done in a StGlobalTrack constructor
  - Hang the new track to the StTrackNode container in StEvent, this creates a new entry
    in the container, the global track is now owned by it.
    <p>
  In addition to the StGlobalTrack, we need to create the following objects (owned by it):
  StTrackTopologyMap
  StTrackFitTraits
  StTrackGeometry (2 are needed, one at first point, one at last point)
  (note: StHelixModel is implementation of the StTrackGeometry abstract class)
  
  The track also owns a container of PidTraits, this algorithm will not fill this container.
  
  And set up links to:
  StTrackDetectorInfo (owned by StEvent, StSPtrVecTrackDetectorInfo)
  StTrackNode         (owned by StEvent, StSPtrVecTrackNode)
  These links are
  track  -> detector info
  track <-> track node

  Skeleton of the algorithm:
  <code> \n
  StSPtrVecTrackNode& trNodeVec = mEvent->trackNodes(); \n
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo(); \n
  for (trackIterator trackIt = mTrackStore->begin(); trackIt != mTrackStore->end(); ++trackIt) { \n
     StiKalmanTrack* kTrack = (*trackIt).second; // the container is a <map>, need second entry of <pair> \n
\n
     StTrackDetectorInfo* detInfo = new StTrackDetectorInfo();\n
     fillDetectorInfo(detInfo,kTrack);\n
     detInfoVec.push_back(detInfo);\n
     \n
     StTrackNode* trackNode = new StTrackNode;\n
     trNodeVec.push_back(trackNode);\n
     \n
     StGlobalTrack* gTrack = new StGlobalTrack();\n
     fillGlobalTrack(gTrack,kTrack);\n
     \n
     // set up relationships between objects\n
     gTrack->setDetectorInfo(detInfo);\n
     gTrack->setNode(trackNode);\n
     trackNode->AddTrack(gTrack);\n
  }\n
  </code>
  The creation of the various objects needed by StGlobalTrack are taken care of in the methods:
  fillTopologyMap(), fillGeometry(), fillFitTraits(), which are called within fillGlobalTrack().
  
*/
StEvent* StiStEventFiller::fillEvent(StEvent* e, StiTrackContainer* t)
{
  //cout << "StiStEventFiller::fillEvent() -I- Started"<<endl;
  if (e==0 || t==0) 
    {
      cout <<"StiStEventFiller::fillEvent(). ERROR:\t"
	   <<"Null StEvent ("<<e<<") || StiTrackContainer ("<<t<<").  Exit"<<endl;
      return 0;
    }
  mEvent = e;
  mTrackStore = t;
  mTrkNodeMap.clear();  // need to reset for this event
  StSPtrVecTrackNode& trNodeVec = mEvent->trackNodes(); 
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo(); 
  int errorCount=0; 

  int fillTrackCount1=0;
  int fillTrackCount2=0;
  int fillTrackCountG=0;
  StErrorHelper errh;
  for (vector<StiTrack*>::iterator trackIt = mTrackStore->begin(); trackIt!=mTrackStore->end();++trackIt) 
    {
      StiKalmanTrack* kTrack = static_cast<StiKalmanTrack*>(*trackIt);
      if (!accept(kTrack)) continue; // get rid of riff-raff
      StTrackDetectorInfo* detInfo = new StTrackDetectorInfo;
      fillDetectorInfo(detInfo,kTrack,true); //3d argument used to increase/not increase the refCount. MCBS oct 04.
      // track node where the new StTrack will reside
      StTrackNode* trackNode = new StTrackNode;
      // actual filling of StTrack from StiKalmanTrack
      StGlobalTrack* gTrack = new StGlobalTrack;
      try 
	{
	  fillTrackCount1++;
	  fillTrack(gTrack,kTrack);
	  // filling successful, set up relationships between objects
	  detInfoVec.push_back(detInfo);
	  gTrack->setDetectorInfo(detInfo);
	  //cout <<"Setting key: "<<(unsigned short)(trNodeVec.size())<<endl;
	  gTrack->setKey((unsigned short)kTrack->getId());
	  trackNode->addTrack(gTrack);
	  trNodeVec.push_back(trackNode);
	  // reuse the utility to fill the topology map
	  // this has to be done at the end as it relies on
	  // having the proper track->detectorInfo() relationship
	  // and a valid StDetectorInfo object.
	  StuFixTopoMap(gTrack);
	  //cout<<"Tester: Event Track Node Entries: "<<trackNode->entries()<<endl;
	  mTrkNodeMap.insert(map<StiKalmanTrack*,StTrackNode*>::value_type (kTrack,trNodeVec.back()) );
	  if (trackNode->entries(global)<1)
	    cout << "StiStEventFiller::fillEvent() -E- Track Node has no entries!! -------------------------" << endl;
          int ibad = gTrack->bad();
	  errh.Add(ibad);
          if (ibad) {
//VP	    printf("GTrack error: %s\n",errh.Say(ibad).Data());
//VP	    throw runtime_error("StiStEventFiller::fillEvent() StTrack::bad() non zero");
          }
	  fillTrackCount2++;
          if (gTrack->numberOfPossiblePoints()<10) continue;
          if (gTrack->geometry()->momentum().mag()<0.1) continue;
	  fillTrackCountG++;
          
	}
      catch (runtime_error & rte ) 
	{
	  cout << "StiStEventFiller::fillEvent() -W- runtime-e filling track"<<rte.what() << endl;
	  delete trackNode;
	  delete detInfo;
	  delete gTrack;
	}
      catch (...) 
	{
	  cout << "StiStEventFiller::fillEvent() -W- Unknown exception filling track."<<endl;
	  delete trackNode;
	  delete detInfo;
	  delete gTrack;
	}
    }
  if (errorCount>4)
    cout << "There were "<<errorCount<<"runtime_error while filling StEvent"<<endl;

  cout <<"StiStEventFiller::fillEvent() -I- Number of filled as global(1):"<< fillTrackCount1<<endl;
  cout <<"StiStEventFiller::fillEvent() -I- Number of filled as global(2):"<< fillTrackCount2<<endl;
  cout <<"StiStEventFiller::fillEvent() -I- Number of filled GOOD globals:"<< fillTrackCountG<<endl;
  errh.Print();

  return mEvent;
}

StEvent* StiStEventFiller::fillEventPrimaries(StEvent* e, StiTrackContainer* t) 
{
  //cout <<"StiStEventFiller::fillEventPrimaries() -I- Started"<<endl;
  if (!mTrkNodeMap.size()) 
    {
      cout <<"StiStEventFiller::fillEventPrimaries(). ERROR:\t"
	   << "Mapping between the StTrackNodes and the StiKalmanTracks is empty.  Exit." << endl;
      return 0;
    }
  if (e==0 || t==0) 
    {
      cout <<"StiStEventFiller::fillEventPrimaries(). ERROR:\t"
	   <<"Null StEvent ("<<e<<") || StiTrackContainer ("<<t<<").  Exit"<<endl;
      return 0;
    }
  mEvent = e;
  mTrackStore = t;
  //Added residual maker...aar
  StPrimaryVertex* vertex = mEvent->primaryVertex(0);
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo();
  if(!vertex)
    {
      cout <<"Failed to find a primary vertex."<<endl;
      return (StEvent*)NULL;
    }
  int skippedCount=0;
  // loop over StiKalmanTracks
  cout << "StiStEventFiller::fillEventPrimaries() -I- Tracks in container:" << mTrackStore->size() << endl;
  int mTrackN=0;
  int noPipe=0;
  int ifcOK=0;
  StiKalmanTrack* kTrack;

  int fillTrackCount1=0;
  int fillTrackCount2=0;
  int fillTrackCountG=0;
  bool testing= false;
  StErrorHelper errh;
  for (vector<StiTrack*>::iterator trackIt = mTrackStore->begin(); trackIt!=mTrackStore->end();++trackIt,++mTrackN) 
    {
      kTrack = static_cast<StiKalmanTrack*>(*trackIt);
      if (kTrack==0) 
	throw runtime_error("StiStEventFiller::fillEventPrimaries() -F- static_cast<StiKalmanTrack*>(*trackIt)==0");
      if (!accept(kTrack)) continue;
      map<StiKalmanTrack*, StTrackNode*>::iterator itKtrack = mTrkNodeMap.find(kTrack);
      if (itKtrack == mTrkNodeMap.end())  continue;
      //throw runtime_error("StiStEventFiller::fillEventPrimaries() -F- itKtrack == mTrkNodeMap.end()");
      StTrackNode* currentTrackNode = (*itKtrack).second;
      //double globalDca = currentTrackNode->track(global)->impactParameter();
      //Even though this is filling of primary tracks, there are certain
      // quantities that need to be filled for global tracks that are only known
      // after the vertex is found, such as dca.  Here we can fill them.
      // 
      StGlobalTrack* currentGlobalTrack = static_cast<StGlobalTrack*>(currentTrackNode->track(global));
      assert(currentGlobalTrack->key()==kTrack->getId());
      float globalDca = impactParameter(currentGlobalTrack);
      currentGlobalTrack->setImpactParameter(globalDca);
      kTrack->setGlobalDca(globalDca);
      
      if (kTrack->isPrimary())
	{
	  fillTrackCount1++;
	  if (currentTrackNode->entries()>10)   continue;//throw runtime_error("StiStEventFiller::fillEventPrimaries() -F- currentTrackNode->entries()>10");
	  if (currentTrackNode->entries(global)<1) continue;//throw runtime_error("StiStEventFiller::fillEventPrimaries() -F- currentTrackNode->entries(global)<1");
	  // detector info
	  StTrackDetectorInfo* detInfo = new StTrackDetectorInfo;
	  fillDetectorInfo(detInfo,kTrack,false); //3d argument used to increase/not increase the refCount. MCBS oct 04.
	  StPrimaryTrack* pTrack = new StPrimaryTrack;
	  pTrack->setKey( currentGlobalTrack->key());
	  try
	    {  
	      if (testing)
		{
		  // test begin ==============
		  StiKalmanTrackNode* leaf = kTrack->getLastNode();
		  StiKTNForwardIterator it(leaf);
		  StiKTNForwardIterator end = it.end();
		  bool weird = false;
		  int bad = 0;
		  bool piped = false;
		  bool ifced = false;
		  //bool out = false; //unused, comment out to remove compiler warning
		  while (it!=end) 
		    {
		      const StiKalmanTrackNode& node = *it;
		      if (node.getRefPosition()>2. && node.getRefPosition()<4.5)
			{
			  piped = true;
			}
		      if (node.getRefPosition()>40. && node.getX()<50.)
			{
			  ifced = true;
			}
		      double x_g = node.x_g();
		      double y_g = node.y_g();
		      double z_g = node.z_g();
		      double rt_g = sqrt(x_g*x_g+y_g*y_g);
		      StiHit * theHit = node.getHit();
		      if (theHit)
			{
			  double dx = fabs(theHit->x() - node.getX());
			  double dy = fabs(theHit->y() - node.getY());
			  double dz = fabs(theHit->z() - node.getZ());
			  if (dx>3 || dy>5 || dz>5)
			    bad++;
			}//theHit
		      if (rt_g>200. || fabs(z_g)>200.)	weird = true;
		      ++it;
		    }//it!=end
		  if (!piped) noPipe++;
		  if (ifced)  ifcOK++;
		  if (weird || !piped || bad>4 || !ifced)
		    {
		      //cout << "  ==";
		      if (weird) cout << "WEIRD/";
		      if (!piped)cout << "NO-PIPE/";
		      if (bad>4) cout << "BAD>4 ("<<bad<<")/";
		      if (!ifced) cout << "no-ifc/";
		      //cout << "=="<<endl;
		      
		      StiKTNForwardIterator it2(leaf);
		      StiKTNForwardIterator end2 = it2.end();
		      while (it2!=end2) 
			{
			  const StiKalmanTrackNode& node2 = *it2;
			  double x_g = node2.x_g();
			  double y_g = node2.y_g();
			  //double z_g = node2.z_g(); //unused, remove to remove compiler warning
			  double rt_g2 = sqrt(x_g*x_g+y_g*y_g);
			  cout << "rt:" << rt_g2 << " --" << node2 << endl;
			  ++it2;
			}
		    }
		}//testing
	      // test end ===============
		  
	      fillTrack(pTrack,kTrack);
	      // set up relationships between objects
	      detInfoVec.push_back(detInfo);
	      pTrack->setDetectorInfo(detInfo);
	     
	      currentTrackNode->addTrack(pTrack);  // StTrackNode::addTrack() calls track->setNode(this);
	      vertex->addDaughter(pTrack);
	      StuFixTopoMap(pTrack);
	      fillTrackCount2++;
              int ibad = pTrack->bad();
	      errh.Add(ibad);
              if (ibad) {
//VP	        printf("PTrack error: %s\n",errh.Say(ibad).Data());
//VP	        throw runtime_error("StiStEventFiller::fillEventPrimaries() StTrack::bad() non zero");
              }
              if (pTrack->numberOfPossiblePoints()<10) 		continue;
              if (pTrack->geometry()->momentum().mag()<0.1) 	continue;
	      fillTrackCountG++;
	    }
	  catch (runtime_error & rte )
	    {
	      cout << "StiStEventFiller::fillEventPrimaries() - runtime exception, filling track: "
		   << rte.what() << endl;
	      delete detInfo;
	      delete pTrack;
	    }
	  catch (...)
	    {
	      cout << "StiStEventFiller::fillEventPrimaries() - Unknown exception, filling track."<<endl;
	      delete detInfo;
	      delete pTrack;
	    }
	}//end if primary
    } // kalman track loop
  if (skippedCount>0) cout << "StiStEventFiller::fillEventPrimaries() -I- A total of "<<skippedCount<<" StiKalmanTracks were skipped"<<endl;
  mTrkNodeMap.clear();  // need to reset for the next event
  cout <<"StiStEventFiller::fillEventPrimaries() -I- Primaries (1):"<< fillTrackCount1<< " (2):"<< fillTrackCount2<< " no pipe node:"<<noPipe<<" with IFC:"<< ifcOK<<endl;
  cout <<"StiStEventFiller::fillEventPrimaries() -I- GOOD:"<< fillTrackCountG <<endl;
  errh.Print();
  return mEvent;
}

/// use the vector of StHits to fill the detector info
/// change: currently point and fit points are the same for StiKalmanTracks,
/// if this gets modified later in ITTF, this must be changed here
/// but maybe use track->getPointCount() later?
void StiStEventFiller::fillDetectorInfo(StTrackDetectorInfo* detInfo, StiKalmanTrack* track, bool refCountIncr) 
{
  //cout << "StiStEventFiller::fillDetectorInfo() -I- Started"<<endl;
  int dets[100]; memset(dets,0,sizeof(dets));
  vector<StMeasuredPoint*> hitVec = track->stHits();
  int numb = hitVec.size();
  detInfo->setFirstPoint(hitVec[     0]->position());
  detInfo->setLastPoint (hitVec[numb-1]->position());

  for (int i=0;i<numb;i++) 
    {
      StHit * hh = (StHit*)hitVec[i];
      detInfo->addHit(hh,refCountIncr);
      hh->setFitFlag(1);
      StDetectorId id = hh->detector();
      dets[id]++;
    }
  for (int i=0;i<int(sizeof(dets)/sizeof(int));i++) {
    if (!dets[i]) continue;
    detInfo->setNumberOfPoints(dets[i],static_cast<StDetectorId>(i));
  }

  //cout << "StiStEventFiller::fillDetectorInfo() -I- Done"<<endl;
}

void StiStEventFiller::fillGeometry(StTrack* gTrack, StiKalmanTrack* track, bool outer)
{
  //cout << "StiStEventFiller::fillGeometry() -I- Started"<<endl;
  if (!gTrack)
    throw runtime_error("StiStEventFiller::fillGeometry() -F- gTrack==0");
  if (!track) 
    throw runtime_error("StiStEventFiller::fillGeometry() -F- track==0");


  StiKalmanTrackNode* node = track->getInnOutMostNode(outer,2);
  StiHit *ihit = node->getHit();
  StThreeVectorF origin(node->x_g(),node->y_g(),node->z_g());
  StThreeVectorF hitpos(ihit->x_g(),ihit->y_g(),ihit->z_g());

  double dif = (hitpos-origin).mag();
  if (gTrack->type()!=primary)
    fprintf(bgFile,"@DZNodHit%d DZ=%g ZH=%g\n",outer,node->z_g()-ihit->z_g(),ihit->z_g());

  if (dif>3.) {
    dif = node->z_g()-ihit->z_g();
    double nowChi2 = node->evaluateChi2(ihit);
    printf("****** DIFF TOO BIG %g chi2 = %g %g\n",dif,node->getChi2(),nowChi2);
    printf("H=%g %g %g N =%g %g %g\n",ihit->x()   ,ihit->y()   ,ihit->z()
		                     ,node->getX(),node->getY(),node->getZ());
    const StMeasuredPoint *mp = ihit->stHit();
    printf("H=%g %g %g N =%g %g %g\n",mp->position().x(),mp->position().y(),mp->position().z()
		                     ,origin.x(),origin.y(),origin.z());
     
    assert(fabs(dif)<50.);
  }

    // making some checks.  Seems the curvature is infinity sometimes and
  // the origin is sometimes filled with nan's...
  
  if (origin.bad()) {
      cout << "StiStEventFiller::fillGeometry() Encountered non-finite numbers!!!! Bail out completely!!! " << endl;
      cout << "Last node had:" << endl;
      cout << "Ref Position  " << node->getRefPosition() << endl;
      cout << "node->getY()  " << node->getY() << endl;
      cout << "node->getZ()  " << node->getZ() << endl;
      cout << "Ref Angle     " << node->getAlpha() << endl;
      cout << "origin        " << origin << endl;
      cout << "curvature     " << node->getCurvature() << endl;
      abort();
  }
  StTrackGeometry* geometry =new StHelixModel(short(track->getCharge()),
					      node->getPsi(),
					      fabs(node->getCurvature()),
					      node->getDipAngle(),
					      origin, 
					      node->getGlobalMomentumF(), 
					      node->getHelicity());

#if 0
  if (outer) cout << "Outer";
  else       cout << "Inner";
  cout << "\tPoints\t" << track->getPointCount();
  if (node->getDetector()) cout << "\tDetector: " << node->getDetector()->getGroupId();
  cout << "\tHelix: " <<geometry->helix() << endl; 
  cout << "\t xCenter = " << geometry->helix().xcenter() << "\t yCenter = " << geometry->helix().ycenter()<<endl;
  cout << "\t getMomentum = " << node->getMomentum() << "\t" << node->getP() << endl;
  cout << "\t getGlobalMomentum = " << node->getGlobalMomentum() << "\t" << node->getP() << endl;
#endif
  if (outer)
    gTrack->setOuterGeometry(geometry);
  else
    gTrack->setGeometry(geometry);

  if (!outer && gTrack->type()!=primary) {
  StPhysicalHelixD helix = gTrack->geometry()->helix();
  double s = helix.pathLength(0.,0.);
  fprintf(bgFile,"@ZPrim X=%g Y=%g Z=%g L=%g\n"
         ,helix.x(s),helix.y(s),helix.z(s),s);
  }

  return;
}

// void StiStEventFiller::fillTopologyMap(StTrack* gTrack, StiKalmanTrack* track){
// 	cout << "StiStEventFiller::fillTopologyMap()" << endl;
//     int map1,map2;
//     map1 = map2 = 0;
//     // change: add code to set the bits appropriately here

//     StTrackTopologyMap topomap(map1,map2);
//     gTrack->setTopologyMap(topomap);
//     return;
// }

void StiStEventFiller::fillFitTraits(StTrack* gTrack, StiKalmanTrack* track){
  // mass
  // this makes no sense right now... double massHyp = track->getMass();  // change: perhaps this mass is not set right?
  unsigned short geantIdPidHyp = 9999;
  //if (.13< massHyp<.14) 
  geantIdPidHyp = 9;
  // chi square and covariance matrix, plus other stuff from the
  // innermost track node
  StiKalmanTrackNode* node = track->getInnerMostHitNode(2);
  double alpha,xRef,x[5],covM[15],chi2node;
  node->get(alpha,xRef,x,covM,chi2node);
  float chi2[2];
  //get chi2/dof
  chi2[0] = track->getChi2();  
  chi2[1] = -999; // change: here goes an actual probability, need to calculate?
  // December 04: The second element of the array will now hold the incremental chi2 of adding
  // the vertex for primary tracks
  if (gTrack->type()==primary) chi2[1]=node->getChi2();

  // @#$%^&
  // need to transform the covariant matrix from double's (Sti) to floats (StEvent)!
  // Actually, the order of the array in Sti is different than in StEvent,
  // so we can't use the same indices, otherwise the matrix is constructed wrong
  // in StTrackFitTraits.
  float covMFloat[15];
  covMFloat[0]  = static_cast<float>(covM[0]);
  covMFloat[1]  = static_cast<float>(covM[1]);
  covMFloat[5]  = static_cast<float>(covM[2]);
  covMFloat[2]  = static_cast<float>(covM[3]);
  covMFloat[6]  = static_cast<float>(covM[4]);
  covMFloat[9]  = static_cast<float>(covM[5]);
  covMFloat[3]  = static_cast<float>(covM[6]);
  covMFloat[7]  = static_cast<float>(covM[7]);
  covMFloat[10] = static_cast<float>(covM[8]);
  covMFloat[12] = static_cast<float>(covM[9]);
  covMFloat[4]  = static_cast<float>(covM[10]);
  covMFloat[8]  = static_cast<float>(covM[11]);
  covMFloat[11] = static_cast<float>(covM[12]);
  covMFloat[13] = static_cast<float>(covM[13]);
  covMFloat[14] = static_cast<float>(covM[14]);
    
  // setFitTraits uses assignment operator of StTrackFitTraits, which is the default one,
  // which does a memberwise copy.  Therefore, constructing a local instance of 
  // StTrackFitTraits is fine, as it will get properly copied.
  StTrackFitTraits fitTraits(geantIdPidHyp,0,chi2,covMFloat);
  // Now we have to use the new setters that take a detector ID to fix
  // a bug.  There is no encoding anymore.

  int nFitPoints[100]; memset(nFitPoints,0,sizeof(nFitPoints));
  stEventFitPoints(track,nFitPoints);

  for (int i=0;i<int(sizeof(nFitPoints)/sizeof(int));i++) {
    if (!nFitPoints[i]) continue;
    fitTraits.setNumberOfFitPoints((unsigned char)nFitPoints[i],(StDetectorId)i);
  }
  if (gTrack->type()==primary) {
     fitTraits.setPrimaryVertexUsedInFit(true);
  }
  gTrack->setFitTraits(fitTraits);
  return;
}

void StiStEventFiller::filldEdxInfo(StiDedxCalculator& dEdxCalculator, StTrack* gTrack, StiKalmanTrack* track){
  double dEdx, errordEdx, nPoints;
  dEdx = errordEdx = nPoints = 9999;
  if (track) {
    dEdxCalculator.getDedx(track, dEdx, errordEdx, nPoints);
  }

  if(!finite(dEdx) || dEdx>9999)
    {
      dEdx = 9999;
      errordEdx= dEdx;
      nPoints=0;
      cout <<"StiStEventFiller::Error: dEdx non-finite."<<endl;
    }
  else if(!finite(errordEdx))
    {
      dEdx = 9999;
      errordEdx= dEdx;
      nPoints=0;
      cout <<"StiStEventFiller::Error: errordEdx non-finite."<<endl;
    }

  StTrackPidTraits* pidTrait = new StDedxPidTraits(dEdxCalculator.whichDetId(),
						   static_cast<short>(kTruncatedMeanId),
						   static_cast<unsigned short>(nPoints),
						   static_cast<float>(1.5*dEdx),
						   static_cast<float>(errordEdx));
  gTrack->addPidTraits(pidTrait);
  return;
}
void StiStEventFiller::fillPidTraits(StTrack* gTrack, StiKalmanTrack* track){

  // TPC
  filldEdxInfo(dEdxTpcCalculator,gTrack,track);

  // SVT
  //filldEdxInfo(dEdxSvtCalculator,gTrack,track);

  return;
}

/// data members from StTrack
/// flags http://www.star.bnl.gov/html/all_l/html/dst_track_flags.html
/// x=1 -> TPC only
/// x=2 -> SVT only
/// x=3 -> TPC + primary vertex
/// x=4 -> SVT + primary vertex
/// x=5 -> SVT+TPC
/// x=6 -> SVT+TPC+primary vertex
/// x=7 -> FTPC only
/// x=8 -> FTPC+primary
/// The last two digits indicate the status of the refit:
/// = +x01 -> good track
/// = -x01 -> Bad fit, outlier removal eliminated too many points
/// = -x02 -> Bad fit, not enough points to fit
/// = -x03 -> Bad fit, too many fit iterations
/// = -x04 -> Bad Fit, too many outlier removal iterations
/// = -x06 -> Bad fit, outlier could not be identified
/// = -x10 -> Bad fit, not enough points to start

void StiStEventFiller::fillFlags(StTrack* gTrack) {
  if (gTrack->type()==global) {
    gTrack->setFlag(101); //change: make sure flag is ok
  }
  else if (gTrack->type()==primary) {
    gTrack->setFlag(301);
  }
  StTrackFitTraits& fitTrait = gTrack->fitTraits();
  //int tpcFitPoints = fitTrait.numberOfFitPoints(kTpcId);
  int svtFitPoints = fitTrait.numberOfFitPoints(kSvtId);
  int totFitPoints = fitTrait.numberOfFitPoints();
  /// In the flagging scheme, I will put in the cases for
  /// TPC only, and TPC+SVT (plus their respective cases with vertex)
  /// Ftpc case has their own code and SSD doesn't have a flag...

  // first case is default above, tpc only = 101 and tpc+vertex = 301
  // next case is:
  // if the track has svt points, it will be an svt+tpc track
  // (we assume that the ittf tracks start from tpc, so we don't
  // use the "svt only" case.)
  if (svtFitPoints>0) {
      if (gTrack->type()==global) {
	  gTrack->setFlag(501); //svt+tpc
      }
      else if (gTrack->type()==primary) {
	  gTrack->setFlag(601); //svt+tpc+primary
      }
  }
  if (totFitPoints<5) {
      int flag = gTrack->flag();
      //keep most sig. digit, set last digit to 2, and flip sign
      gTrack->setFlag(-(((flag/100)*100)+2)); // -x02 
  }

}
void StiStEventFiller::fillTrack(StTrack* gTrack, StiKalmanTrack* track)
{

  //cout << "StiStEventFiller::fillTrack()" << endl;
  // encoded method = 16 bits = 12 fitting and 4 finding, for the moment use:
  // kKalmanFitId
  // bit 15 for finding, (needs to be changed in StEvent).
  // change: make sure bits are ok, are the bits set up one in each position and nothing else?
  // this would mean that the encoded method is wasting space!
  // the problem is that in principle there might be combinations of finders for each tracking detector
  // but the integrated tracker will use only one for all detectors maybe
  // so need this bit pattern:
  // finding 100000000000     
  // fitting             0010 
  //            32768    +    2 = 32770;
  //
  // above is no longer used, instead use kITKalmanfitId as fitter and tpcOther as finding method

  gTrack->setEncodedMethod(mStiEncoded);
  double tlen = track->getTrackLength();
  assert(tlen >0.0 && tlen<1000.);
  gTrack->setLength(tlen);// someone removed this, grrrr!!!!
 
  if (gTrack->type()==primary) {
      float impactParam = impactParameter(track);
      gTrack->setImpactParameter(impactParam );
      track->setDca(impactParam);
  }

  // Follow the StDetectorId.h enumerations...
  // can't include them from here in order not to
  // create a package dependence...
  int maxPointsTpc = track->getMaxPointCount(1);//1 for TPC
  int maxPointsSvt = track->getMaxPointCount(2);//2 for Svt
  int maxPointsSsd = track->getMaxPointCount(8);//8 for Ssd

  
  gTrack->setNumberOfPossiblePoints(static_cast<unsigned char>(maxPointsTpc),kTpcId);
  gTrack->setNumberOfPossiblePoints(static_cast<unsigned char>(maxPointsSvt),kSvtId);
  gTrack->setNumberOfPossiblePoints(static_cast<unsigned char>(maxPointsSsd),kSsdId);

  fillGeometry(gTrack, track, false); // inner geometry
  fillGeometry(gTrack, track, true ); // outer geometry
  fillFitTraits(gTrack, track);
  fillFlags(gTrack);
  return;
}
bool StiStEventFiller::accept(StiKalmanTrack* track) {
    return (track->getTrackLength()>0); // insert other filters for riff-raff we don't want in StEvent here.
}
void StiStEventFiller::stEventFitPoints(StiKalmanTrack* track, int *nFitPoints) 
{
    // need to write the fit points in StEvent,
    // where now StEvent has a set method that knows about the detector id.
    // No encoding is necessary anymore.

    StiKTNBidirectionalIterator it;
    double maxChi2 = track->fitPars()->getMaxChi2();
    // loop here to get the hits using iterator.
    for (it=track->begin();it!=track->end();it++) {
	StiKalmanTrackNode& ktn = (*it);
	if (ktn.getChi2() > maxChi2)	continue;
        StiHit *stih = ktn.getHit();
        if (!stih)			continue;
	const StHit* hit = dynamic_cast<const StHit*>(stih->stHit());
        if (!hit) 			continue;
          // use StDetectorId's and accumulate
	StDetectorId detId = hit->detector();
	nFitPoints[detId]++;
    } // KTNode loop    
}

float StiStEventFiller::impactParameter(StiKalmanTrack* track) 
{
  if (!mEvent->primaryVertex()) 
    {
      return DBL_MAX;
    }
  StiKalmanTrackNode*	node;

  node = track->getInnerMostNode(); // ...

  const StThreeVectorF& vxF = mEvent->primaryVertex()->position();

  originD->setX(node->x_g());
  originD->setY(node->y_g());
  originD->setZ(node->z_g());

  StThreeVectorD vxDD(vxF.x(),vxF.y(),vxF.z());

  physicalHelix->setParameters(fabs(node->getCurvature()),
			       node->getDipAngle(),
			       node->getPhase(),
			       *originD,
			       node->getHelicity());
  

  //cout <<"PHelix: "<<*physicalHelix<<endl;
  float dca = static_cast<float>(physicalHelix->distance(vxDD));
  return dca;
}
float StiStEventFiller::impactParameter(StTrack* track) 
{
  if (!mEvent->primaryVertex()) 
    {
      return DBL_MAX;
    }
  StPhysicalHelixD helix = track->geometry()->helix();

  const StThreeVectorF& vxF = mEvent->primaryVertex()->position();

  StThreeVectorD vxDD(vxF.x(),vxF.y(),vxF.z());
  //cout <<"PHelix: "<<helix<<endl;
  float dca = static_cast<float>(helix.distance(vxDD));
  return dca;
}
