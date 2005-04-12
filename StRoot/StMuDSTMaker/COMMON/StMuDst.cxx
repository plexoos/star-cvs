/***************************************************************************
 *
 * $Id: StMuDst.cxx,v 1.34 2005/04/12 21:56:29 mvl Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

#include "StMuDst.h"

#include "StContainers.h"
#include "StEvent/StEventTypes.h"

#include "StarClassLibrary/StTimer.hh"
#include "StMuDstMaker.h"
#include "StMuTrack.h"
#include "StMuEvent.h"
#include "StMuDebug.h"
#include "StMuEmcUtil.h"
#include "StMuPmdUtil.h"
#include "TClonesArray.h"
#include "TTree.h"

#include "StStrangeMuDstMaker/StV0MuDst.hh"
#include "StStrangeMuDstMaker/StXiMuDst.hh"
#include "StStrangeMuDstMaker/StKinkMuDst.hh"

TClonesArray** StMuDst::arrays       = 0;
TClonesArray** StMuDst::strangeArrays= 0;
TClonesArray** StMuDst::emcArrays    = 0;
TClonesArray** StMuDst::pmdArrays    = 0;
TClonesArray** StMuDst::tofArrays    = 0;
TClonesArray *StMuDst::mMuEmcCollectionArray = 0;
StMuEmcCollection *StMuDst::mMuEmcCollection = 0;
TClonesArray *StMuDst::mMuPmdCollectionArray = 0;
StMuPmdCollection *StMuDst::mMuPmdCollection = 0;
StEmcCollection *StMuDst::mEmcCollection = 0;
TClonesArray** StMuDst::eztArrays    = 0;

StMuDst::StMuDst() {
  DEBUGMESSAGE("");
  /* no-op */
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::unset() {
    arrays        = 0;
    strangeArrays = 0;
    emcArrays     = 0;
    pmdArrays     = 0;
    tofArrays     = 0;
    mMuEmcCollectionArray = 0;
    mMuEmcCollection = 0;
    mMuPmdCollectionArray = 0;
    mMuPmdCollection = 0;
    mEmcCollection = 0;
    eztArrays      = 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::set(StMuDstMaker* maker) {
  DEBUGMESSAGE2("");
  if (!maker) { DEBUGVALUE(maker); return;}
  arrays        = maker->mArrays;
  strangeArrays = maker->mStrangeArrays;
  emcArrays     = maker->mEmcArrays;
  pmdArrays     = maker->mPmdArrays;
  tofArrays     = maker->mTofArrays;
  mMuEmcCollectionArray = maker->mEmcCollectionArray;
  mMuEmcCollection      = maker->mEmcCollection;
  mMuPmdCollectionArray = maker->mPmdCollectionArray;
  mMuPmdCollection = maker->mPmdCollection;
  eztArrays     = maker->mEztArrays;

  StStrangeEvMuDst* ev = strangeEvent();
  int nV0s = v0s()->GetEntries(); for (int i=0;i<nV0s; i++) v0s(i)->SetEvent(ev); // set the pointer to the StStrangeEvMuDst which is not read from disk
  int nXis = xis()->GetEntries(); for (int i=0;i<nXis; i++) xis(i)->SetEvent(ev); // set the pointer to the StStrangeEvMuDst which is not read from disk
  //  int nKinks = kinks()->GetEntries(); for (int i=0;i<nKinks; i++) kinks(i)->SetEvent(ev);

}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::set(TClonesArray** theArrays, 
		  TClonesArray** theStrangeArrays, 
		  TClonesArray** theEmcArrays,
		  TClonesArray** thePmdArrays,
                  TClonesArray** theTofArrays,
                  TClonesArray* emc_arr,
		  StMuEmcCollection *emc,
                  TClonesArray* pmd_arr,
		  StMuPmdCollection *pmd,
		  TClonesArray** theEztArrays) 
{
  // I don't understand why this method is still needed,
  // but cannot comile dictionary  when it is removed
  DEBUGMESSAGE2("");
  arrays        = theArrays;
  strangeArrays = theStrangeArrays;
  emcArrays     = theEmcArrays;
  pmdArrays     = thePmdArrays;
  tofArrays     = theTofArrays;
  mMuEmcCollectionArray = emc_arr;  
  mMuEmcCollection = emc;  
  mMuPmdCollectionArray = pmd_arr;
  mMuPmdCollection = pmd;
  eztArrays     = theEztArrays;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixTrackIndices() {
  /// global and primary tracks share the same id, so we can fix the 
  /// index2Global up in case they got out of order (e.g. by removing 
  /// a track from the TClonesArrays
    fixTrackIndices( arrays[muPrimary], arrays[muGlobal] );  
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuDst::fixTrackIndices(TClonesArray* primary, TClonesArray* global) {
  /// global and primary tracks share the same id, so we can fix the 
  /// index2Global up in case they got out of order (e.g. by removing 
  /// a track from the TClonesArrays

  if ( !(global&&primary) ) return;
  DEBUGMESSAGE1("");
  StTimer timer;
  timer.start();

  int nGlobals = global->GetEntries();
  int nPrimaries = primary->GetEntries();
  // fill an array with the indices to the global tracks as function of trackId
  TArrayI globalIndex(2*nGlobals);

  int nIndex=globalIndex.GetSize();
  for (int i=0; i<nIndex; i++) globalIndex[i]=-1;

  for (int i=0; i<nGlobals; i++) {
    if (globalTracks(i)->id()<nIndex) {
      globalIndex[ globalTracks(i)->id() ] = i;
      globalTracks(i)->setIndex2Global(i);
    }
    else {
      int newSize=(int) (1.2*nIndex);
      globalIndex.Set(newSize);
      for (Int_t j=nIndex; j<newSize; j++)
        globalIndex[j]=-1;
      nIndex=newSize;
    }
  }
  // set the indices for the primary tracks
  DEBUGVALUE2(primary->GetEntries());
  for (int i=0; i<nPrimaries; i++) {
    if (primaryTracks(i)->id()<nIndex) 
      primaryTracks(i)->setIndex2Global( globalIndex[ primaryTracks(i)->id() ] );
    else
      primaryTracks(i)->setIndex2Global(-1);
  }
  DEBUGVALUE2(timer.elapsedTime());
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StEvent* StMuDst::createStEvent() {
  static TObjArray nodes;

  DEBUGMESSAGE1("");
  StTimer timer;
  timer.start();

  StMuEvent* mu = event(); 
  if(!mu) return NULL;
  StEvent* ev = new StEvent();

  ev->setInfo( new StEventInfo(mu->eventInfo()) );
  ev->setRunInfo( new StRunInfo(mu->runInfo()) );
  ev->setSummary( new StEventSummary(mu->eventSummary()) );
  //   ev->setSoftwareMonitor(SoftwareMonitor*);
  //   ev->setTpcHitCollection(StTpcHitCollection*);
  //   ev->setFtpcHitCollection(StFtpcHitCollection*);
  //   ev->setSvtHitCollection(StSvtHitCollection*);
  //   ev->setSsdHitCollection(StSsdHitCollection*);
  //   ev->setEmcCollection(StEmcCollection*);
  //   ev->setRichCollection(StRichCollection*);
  //   ev->setTofCollection(StTofCollection*);
  //ev->setTofCollection( new StTofCollection() );
  StTriggerDetectorCollection *trg_coll=new StTriggerDetectorCollection();
  trg_coll->bbc()=mu->bbcTriggerDetector();
  trg_coll->ctb()=mu->ctbTriggerDetector();
  trg_coll->emc()=mu->emcTriggerDetector();
  trg_coll->fpd()=mu->fpdTriggerDetector();
  trg_coll->zdc()=mu->zdcTriggerDetector();
  ev->setTriggerDetectorCollection(trg_coll);

  ev->setFpdCollection( new StFpdCollection(mu->fpdCollection()) );
  // ev->setTriggerDetectorCollection(muStTriggerDetectorCollection*); <<< WE DON'T WANT THAT
  ev->setL0Trigger ( new StL0Trigger(mu->l0Trigger()) );
  //   ev->setL1Trigger ( new StL0Trigger(mu->l0Trigger()) );
  ev->setL3Trigger ( new StL3Trigger() );
  
  StPrimaryVertex* vp  = new StPrimaryVertex();  
  ev->addPrimaryVertex(vp);
  vp->setPosition( mu->eventSummary().primaryVertexPosition() );

  int nGlobals = arrays[muGlobal]->GetEntries();
  nodes.Clear(); // tracknodes are owned by StEvent

  // add global tracks to tracknodes
  for (int i=0; i<nGlobals; i++) if(globalTracks(i)) {
    int id = globalTracks(i)->id();
    StMuTrack *mt = globalTracks(i);
    if (!mt) 		continue;
    if (mt->bad())	continue;
    StTrack *st = createStTrack(mt);
    if (!st)		continue;
    nodes.AddAtAndExpand(new StTrackNode(),id);
    ((StTrackNode*)nodes[id])->addTrack(st);
  }

  /// add primary tracks to tracknodes and primary vertex
  int nPrimaries = arrays[muPrimary]->GetEntries();
  for (int i=0; i<nPrimaries; i++) if(primaryTracks(i)) {
    int id = primaryTracks(i)->id();
    if (nodes[id]==0) nodes.AddAtAndExpand(new StTrackNode(),id);
    StTrack* t = createStTrack(primaryTracks(i));
    ((StTrackNode*)nodes[id])->addTrack( t );
    vp->addDaughter( t );
  }

  /// add all tracknodes to the event
  Int_t nNodes=nodes.GetEntriesFast();
  for (int i=0; i<nNodes; i++) {
    if (nodes[i]) ev->trackNodes().push_back((StTrackNode*)nodes[i]);
  } 

  /// do the same excercise for the l3 tracks
  /// we do this later
  /// we do this later
  /// we do this later
  
  // add detector states
  int nStates = arrays[muState]->GetEntries();
  for (int i=0; i<nStates; i++) {
      StDetectorState* det = new StDetectorState(*detectorStates(i));
      ev->addDetectorState(det);
  }
  

  // now get the EMC stuff and put it in the StEvent
  static StMuEmcUtil* mEmcUtil = new StMuEmcUtil();
  StMuEmcCollection *emc = muEmcCollection();
  if(emc) { // transform to StEvent format and fill it
    StEmcCollection *EMC = mEmcUtil->getEmc(emc);
    if(EMC) ev->setEmcCollection(EMC);
  }
  // now get the PMD stuff and put it in the StEvent
  static StMuPmdUtil* mPmdUtil = new StMuPmdUtil();
  StMuPmdCollection *pmd = pmdCollection();
  if(pmd) { // transform to StEvent format and fill it
    StPhmdCollection *PMD = mPmdUtil->getPmd(pmd);
    if(PMD) ev->setPhmdCollection(PMD);
  }

// now get tof (after fix from Xin)
  StTofCollection *tofcoll = new StTofCollection();
  ev->setTofCollection(tofcoll);
  int nTofData = tofArrays[muTofData]->GetEntries();
  for(int i=0;i<nTofData;i++) {
    StTofData *aData;
    if(tofData(i)) {
      unsigned short id = tofData(i)->dataIndex();
      unsigned short adc = tofData(i)->adc();
      unsigned short tdc = tofData(i)->tdc();
      short tc = tofData(i)->tc();
      unsigned short sc = tofData(i)->sc();
      // run 5 - dongx
      aData = new StTofData(id, adc, tdc, tc, sc, 0, 0);
    } else {
      aData = new StTofData(0, 0, 0, 0, 0, 0, 0);
    }
    tofcoll->addData(aData);
  }
  // run 5 - dongx
  int nTofRawData = tofArrays[muTofRawData]->GetEntries();
  for(int i=0;i<nTofRawData;i++) {
    StTofRawData *aRawData;
    if(tofRawData(i)) {
      unsigned short leteFlag = tofRawData(i)->leteFlag();
      unsigned short channel = tofRawData(i)->channel();
      unsigned int tdc = tofRawData(i)->tdc();
      unsigned short quality = tofRawData(i)->quality();
      aRawData = new StTofRawData(leteFlag,channel,tdc,quality);
    } else {
      aRawData = new StTofRawData(0, 0, 0, 0);
    }
    tofcoll->addRawData(aRawData);
  }

  // now create, fill and add new StTriggerIdCollection to the StEvent
  StTriggerIdCollection* triggerIdCollection = new StTriggerIdCollection();
  StTriggerId triggerId;
  triggerId = mu->triggerIdCollection().l1();
  if ( !StMuTriggerIdCollection::isEmpty( triggerId ) ) triggerIdCollection->setL1( new StTriggerId( triggerId ) );
  triggerId = mu->triggerIdCollection().l2();
  if ( !StMuTriggerIdCollection::isEmpty( triggerId ) ) triggerIdCollection->setL2( new StTriggerId( triggerId ) );
  triggerId = mu->triggerIdCollection().l3();
  if ( !StMuTriggerIdCollection::isEmpty( triggerId ) ) triggerIdCollection->setL3( new StTriggerId( triggerId ) );
  triggerId = mu->triggerIdCollection().nominal();
  if ( !StMuTriggerIdCollection::isEmpty( triggerId ) ) triggerIdCollection->setNominal( new StTriggerId( triggerId ) );
  ev->setTriggerIdCollection( triggerIdCollection );
    
  
  DEBUGVALUE2(timer.elapsedTime());
  return ev;
}

#include "StarClassLibrary/SystemOfUnits.h"
#include "StarClassLibrary/PhysicalConstants.h"
StTrackGeometry* StMuDst::trackGeometry(int q, StPhysicalHelixD* h) {
  static StPhysicalHelixD nullHelix;
  StHelixModel* model=0; 
  if (nullHelix==*h) 			return 0;
  if (fabs(h->curvature()) > 100)	return 0;
  if (fabs(h->origin().x())>1000)	return 0;
  if (fabs(h->origin().y())>1000)	return 0;
  if (fabs(h->origin().z())>1000)	return 0;

  model = new StHelixModel(q, h->phase()+h->h()*pi/2, h->curvature(), h->dipAngle(), h->origin(), 
			     h->momentumAt(0,event()->runInfo().magneticField()*kilogauss), h->h());
  return model;
}

StTrack* StMuDst::createStTrack(StMuTrack* track) {
  StTrack* t=0;
  StTrackGeometry *tg;
  if (track->bad()) return 0;

  if (track->type() == primary) t = new StPrimaryTrack();
  if (track->type() == global)  t = new StGlobalTrack();
  assert(t);
  t->setFlag( track->flag() );
  t->setKey( track->id() );
  
  StPhysicalHelixD helix;
  helix = track->helix(); 
  tg = trackGeometry( track->charge(), &helix );
  if (tg) t->setGeometry( tg );
  helix = track->outerHelix();
  tg = trackGeometry( track->charge(), &helix );
  if (tg) t->setOuterGeometry( tg );

  t->setLength(track->length());
  t->setImpactParameter((track->dca()).mag());
  t->addPidTraits(new StDedxPidTraits(kTpcId, kTruncatedMeanId, track->nHitsDedx(), track->dEdx(),0));
  Float_t a[2],b[15];
  a[0]=track->chi2();
  a[1]=0;
  memset(b,0,15*sizeof(Float_t));

  StTrackFitTraits traits(0,0,a,b);
  traits.setNumberOfFitPoints(track->nHitsFit(kTpcId),kTpcId);
  traits.setNumberOfFitPoints(track->nHitsFit(kFtpcEastId),kFtpcEastId);
  traits.setNumberOfFitPoints(track->nHitsFit(kFtpcWestId),kFtpcWestId);
  traits.setNumberOfFitPoints(track->nHitsFit(kSvtId),kSvtId);
  traits.setNumberOfFitPoints(track->nHitsFit(kSsdId),kSsdId);
  // Set flag for primary tracks, but not if data is old-style
  // Old style data has +1 for vertex built-in for Ftpc (not sure about Tpc)
  if (track->type() == primary && track->mNHitsFitTpc != 255)
    traits.setPrimaryVertexUsedInFit(kTRUE);
  t->setFitTraits(traits);

  t->setNumberOfPossiblePoints(track->nHitsPoss(kTpcId),kTpcId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kFtpcEastId),kFtpcEastId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kFtpcWestId),kFtpcWestId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kSvtId),kSvtId);
  t->setNumberOfPossiblePoints(track->nHitsPoss(kSsdId),kSsdId);

  // set the topology map
  t->setTopologyMap( track->topologyMap() );
  
  return t;
}


ClassImp(StMuDst)

/***************************************************************************
 *
 * $Log: StMuDst.cxx,v $
 * Revision 1.34  2005/04/12 21:56:29  mvl
 * Changes by Xin Dong for year-5 TOF data format: extra TClonesArray and routines to fill it from StEvent (StTofRawData).
 *
 * Revision 1.33  2004/10/28 00:11:33  mvl
 * Added stuff to support ezTree mode of MuDstMaker.
 * This is a special mode for fast-online processing of fast-detector data.
 *
 * Revision 1.32  2004/10/22 23:44:07  mvl
 * Fixed StMuDst::fixTrackIndices()
 *
 * Revision 1.31  2004/10/21 02:56:35  mvl
 * Added pointer to StEmcColleciton for Emc clustering etc.
 * Also made some technical changes for backward compatibility mode with
 * StMuIOMaker (pointers to TClonesArray for StMuEmcCollection)
 *
 * Revision 1.30  2004/10/19 01:45:26  mvl
 * Changes to split Emc and Pmd collections. Minor change to track copying logic
 *
 * Revision 1.29  2004/08/14 00:48:41  mvl
 * Bug fix in createStTrack & mods for vertex flag in fitTraits
 *
 * Revision 1.28  2004/08/07 02:44:06  mvl
 * Added support for fitted and possible points in different detectors, for ITTF
 *
 * Revision 1.27  2004/04/20 18:41:20  perev
 * Change arrays to pointer to StMuDstMaker::arrays StMuDst.h
 *
 * Revision 1.26  2004/04/14 17:15:56  subhasis
 * Xin's TOF reinclusion
 *
 * Revision 1.25  2004/04/09 22:04:55  subhasis
 * after tof createevent fix by Xin
 *
 * Revision 1.24  2004/04/09 03:36:14  jeromel
 * Removed TOF support entirely for now as we need a working version ... Will
 * revisit later.
 *
 * Revision 1.23  2004/04/08 23:58:07  jeromel
 * Small protection on tofcoll
 *
 * Revision 1.22  2004/04/06 00:25:35  jeromel
 * Missing TofCollection addition
 *
 * Revision 1.21  2004/04/02 03:24:53  jeromel
 * Changes implements PMD and TOF.  TOF is clearly incomplete.
 *
 * Revision 1.20  2003/10/31 19:12:56  laue
 * added filling of track id to createStTrack() function
 *
 * Revision 1.19  2003/10/28 18:57:56  perev
 * BadData protection added
 *
 * Revision 1.18  2003/10/23 04:08:29  perev
 * use SetBranchStatus fixed
 *
 * Revision 1.17  2003/10/20 22:55:39  laue
 * added filling of the topology map in the createStTrack function
 *
 * Revision 1.16  2003/10/14 14:35:53  laue
 * Alex Suaide EMC updates
 *
 * Revision 1.15  2003/08/04 14:38:10  laue
 * Alex Suaide's updated for the EMC. Now EEMC is included.
 *
 * Revision 1.14  2003/04/15 18:48:34  laue
 * Minor changes to be able to filter MuDst.root files and an example
 * how to do this. The StMuDstFilterMaker is just an example, it has to be
 * customized (spoilers, chrome weels, etc.) by the user.
 *
 * Revision 1.13  2003/03/19 18:58:04  laue
 * StMuChainMaker: updates for moved file catalog
 * StTriggerIdCollection added to the createStEvent function in StMuDst.cxx
 *
 * Revision 1.12  2003/02/18 20:38:30  laue
 * updates from Alex Suaide for filling StTrack from StMuTrack
 *
 * Revision 1.11  2003/01/09 18:59:45  laue
 * initial check in of new EMC classes and the changes required
 *
 * Revision 1.10  2002/09/21 00:26:09  laue
 * Bug fix in createStEvent() function. Now you can delete the StEvent
 *
 * Revision 1.9  2002/08/27 19:05:56  laue
 * Minor updates to make the muDst from simulation work
 *
 * Revision 1.8  2002/08/20 19:55:48  laue
 * Doxygen comments added
 *
 * Revision 1.7  2002/06/18 19:21:00  laue
 * cout statement taken out
 *
 * Revision 1.6  2002/05/20 18:57:18  laue
 * update for Christof
 *
 * Revision 1.5  2002/03/27 00:50:11  laue
 * bux fix from earlier check in
 *
 * Revision 1.4  2002/03/26 19:33:14  laue
 * minor updates
 *
 * Revision 1.3  2002/03/20 16:04:11  laue
 * minor changes, mostly added access functions
 *
 * Revision 1.2  2002/03/14 04:12:55  laue
 * bug fix: StMuL3EventSummary.cxx
 * update: StMuDst.h StMuDst.cxx
 *
 * Revision 1.1  2002/03/08 17:04:17  laue
 * initial revision
 *
 *
 **************************************************************************/
