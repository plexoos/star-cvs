/// \File StiKalmanTrackFinder.cxx
/// \Author Claude A Pruneau, 2001-2003
/// \Copyright(c) 2001, STAR  Experiment at BNL, All rights reserved.
/// \Note
/// Permission to use, copy, modify and distribute this software and its
/// documentation strictly for non-commercial purposes is hereby granted
/// without fee, provided that the above copyright notice appears in all
/// copies and that both the copyright notice and this permission notice
/// appear in the supporting documentation. The authors make no claims
/// about the suitability of this software for any purpose. It is
/// provided "as is" without express or implied warranty.
#include "Stiostream.h"
#include <stdexcept>
#include <math.h>
#include "TMath.h"
#include "TError.h"
#include "TStopwatch.h"
#include "StEnumerations.h"
using namespace std;
#include "Sti/Base/Parameter.h"
#include "Sti/Base/EditableParameter.h"
#include "Sti/Base/EditableFilter.h"
#include "StiToolkit.h"
#include "StiHit.h"
#include "StiHitContainer.h"
#include "StiDetector.h"
#include "StiDetectorContainer.h"
#include "StiTrackContainer.h"
#include "StiTrack.h"
#include "StiTrackFinder.h"
//#include "StiTrackSeedFinder.h"
#include "StiTrack.h"
#include "StiKalmanTrackFinder.h"
#include "StiTrackContainer.h"
#include "StiKalmanTrack.h"
#include "StiKalmanTrackNode.h"
#include "StiDefaultTrackFilter.h"
#include "StiTrackFinderFilter.h"
#include "StiUtilities/StiDebug.h"
#include "Sti/StiKalmanTrackFinderParameters.h"
#include "StMessMgr.h"

#define TIME_StiKalmanTrackFinder
#ifdef TIME_StiKalmanTrackFinder
#include "Sti/StiTimer.h"
#endif
//#define ASSIGNVP
#ifdef ASSIGNVP
#include "TAssign.h"
static int vpAssign = 0;
static TAssign *gTAssign = 0;
static TAssMtx *gTAssMtx = 0;
static double vpLayer=0;
static const StiDetector *vpDetector=0;
static StiHit *vpHit = 0;

#endif //ASSIGNVP

enum {kSeedTimg,kTrakTimg,kPrimTimg};

static const double kRMinTpc =55;
int StiKalmanTrackFinder::_debug = 0;
ostream& operator<<(ostream&, const StiTrack&);
int gLevelOfFind = 0;
//______________________________________________________________________________
void StiKalmanTrackFinder::initialize()
{
  cout << "StiKalmanTrackFinder::initialize() -I- Started"<<endl;
  _toolkit = StiToolkit::instance();
  _trackNodeFactory  = _toolkit->getTrackNodeFactory();
  _detectorContainer = _toolkit->getDetectorContainer();
  _detectorContainer->reset();
  _trackSeedFinder   = _toolkit->getTrackSeedFinder();
  _hitContainer      = _toolkit->getHitContainer();
  _trackContainer    = _toolkit->getTrackContainer();
  /*
  StiDefaultTrackFilter * trackFilter = new StiDefaultTrackFilter("FinderTrackFilter","Reconstructed Track Filter");
  trackFilter->add( new EditableParameter("nPtsUsed","Use nPts", 1., 1., 0., 1., 1.,
                                          Parameter::Boolean, StiTrack::kPointCount) );
  trackFilter->add( new EditableParameter("nPtsMin", "Minimum nPts", 10., 10., 0., 100.,1.,
                                          Parameter::Integer,StiTrack::kPointCount) );
  trackFilter->add( new EditableParameter("nPtsMax", "Maximum nPts", 60., 60., 0., 100.,1.,
                                          Parameter::Integer,StiTrack::kPointCount) );
  trackFilter->add(new EditableParameter("lengthUsed","Use Length", 1., 1., 0.,1.,1.,Parameter::Boolean, StiTrack::kTrackLength));
  trackFilter->add(new EditableParameter("lengthMin", "Min Length", 0., 0., -300.,   300.,2,Parameter::Double, StiTrack::kTrackLength));
  trackFilter->add(new EditableParameter("lengthMax", "Max Length", 300.,  300., -300.,   300.,2,Parameter::Double, StiTrack::kTrackLength));
  _trackFilter = trackFilter;
  */
  _trackFilter = new StiTrackFinderFilter();
  //_toolkit->setFinderTrackFilter(_trackFilter);
  cout << "StiKalmanTrackFinder::initialize() -I- Done"<<endl;
}

StiKalmanTrackFinder::StiKalmanTrackFinder(StiToolkit*toolkit)
:
_toolkit(toolkit),
_trackFilter(0),
_trackSeedFinder(0),
_trackNodeFactory(0),
_detectorContainer(0),
_hitContainer(0),
_trackContainer(0)
{
  cout << "StiKalmanTrackFinder::StiKalmanTrackFinder() - Started"<<endl;
memset(mTimg,0,sizeof(mTimg));
  if (!_toolkit)
    throw runtime_error("StiKalmanTrackFinder::StiKalmanTrackFinder(...) - FATAL - toolkit==0");
  cout << "StiKalmanTrackFinder::StiKalmanTrackFinder() - Done"<<endl;
}

//______________________________________________________________________________
StiKalmanTrackFinder::~StiKalmanTrackFinder()
{ }

//______________________________________________________________________________
/*!
Reset the state of the finder  to "event not tracked"
 <p>
 The track factory, the track container are reset. This
 method is distinct from the "clear" method which reset
 the state to "event not loaded".
 */
//______________________________________________________________________________
void StiKalmanTrackFinder::reset()
{
  //cout << "StiKalmanTrackFinder::reset() -I- Starting" <<endl;
  _detectorContainer->reset();
  _trackContainer->clear();
  _trackNodeFactory->reset();
  _hitContainer->reset();
  _trackSeedFinder->reset();
  //cout << "StiKalmanTrackFinder::reset() -I- Done" <<endl;
}

//______________________________________________________________________________
/*!
Reset the state of the finder  to "no event loaded"
 <p>
 A reset or clear command is used to all components this tracker
 depends on. This include the hitContainer, the detector container,
 the hit, track, track node, mc track factories, the track containers,
 and the seed finder.
 */
//______________________________________________________________________________
void StiKalmanTrackFinder::clear()
{
  //cout << "StiKalmanTrackFinder::clear() -I- Starting" <<endl;
  _hitContainer->clear();
  reset();
  //cout << "StiKalmanTrackFinder::clear() -I- Done" <<endl;
}

//______________________________________________________________________________
/*! Find all tracks associated with the current event.
<p>
Algorithm: In a <b>while loop</b>, obtain track seeds from
current track seed finder and proceed to extend it through the
detector.
<p>Found tracks are added to the track container if no track
filter is set or if they satisfy the track filter requirements.
*/
//______________________________________________________________________________
void StiKalmanTrackFinder::findTracks()
{
  assert(_trackContainer );
  assert(_trackSeedFinder);
  _trackSeedFinder->reset();
  _trackContainer->clear();
  if (_trackFilter) _trackFilter->reset();

#ifdef ASSIGNVP
  vpAssign = 1;
  if (!gTAssign) { gTAssign = new TAssign(); gTAssMtx = new TAssMtxObj();}
  gTAssMtx->Clear();
  extendSeeds (0.); 
  gTAssign->SetMatrix(gTAssMtx);
  double s = gTAssign->Assign();
  int nKeys = gTAssMtx->GetNKeys();

  LOG_DEBUG << Form("findTracks:: nTracks=%d aver Chi2=%g",nKeys,s/nKeys)
        << endm;
  vpAssign = 2;
  long token = 0,lKey=0,lVal=0;
  while ( gTAssign->Iter(token,lKey,lVal)) {
     if(!lVal) continue;
     StiHit *stiHit = (StiHit*)lVal;
     stiHit->setTimesUsed(1);
  }

  extendTracks( 0.);
  return;
#endif// ASSIGNVP
  
  
//  extendSeeds (0.);


  extendSeeds (0.);
//  _trackContainer->sort();
//  extendTracks( 0.);
}


//______________________________________________________________________________
void StiKalmanTrackFinder::extendSeeds(double rMin)
{
static int nCall=0;nCall++;
  StiKalmanTrack *track;
  int nTTot=0,nTSeed=0,nTAdd=0,nTFail=0,nTFilt=0,extend=0,status;
  int nTpcHits=0,nSvtHits=0,nSsdHits=0,nIstHits=0,nPxlHits=0;

  while (true ){
// 		obtain track seed from seed finder
    
    if (mTimg[kSeedTimg]) mTimg[kSeedTimg]->Start(0);

    track = (StiKalmanTrack*)_trackSeedFinder->findTrack(rMin);

    if (mTimg[kSeedTimg]) mTimg[kSeedTimg]->Stop();
    if (!track) break; // no more seeds
      nTTot++;
      int iBreak=1;
      if (mTimg[kTrakTimg]) mTimg[kTrakTimg]->Start(0);
      do { //technical do
        track->setFlag(-1);
        status = track->approx(0); 
        if (status ) 	{nTSeed++; break;}
        status = track->fit(kOutsideIn);
        if (status ) 	{nTSeed++; break;}
        extend = extendTrack(track,rMin);
        if (extend<=0)	{nTFail++; break;}
        if (_trackFilter && (!_trackFilter->filter(track))) {nTFilt++;break;}
	//cout << "  ++++++++++++++++++++++++++++++ Adding Track"<<endl;
//		Add DCA node
        StiHit dcaHit; dcaHit.makeDca();
        StiTrackNode *extenDca = track->extendToVertex(&dcaHit);
        if (extenDca) track->add(extenDca,kOutsideIn);
//		End DCA node
        track->reduce();
        nTAdd++;
        track->setFlag(1);
        _trackContainer->push_back(track);
        track->setId(_trackContainer->size());
        track->reserveHits();
        nTpcHits+=track->getFitPointCount(kTpcId);
        nSvtHits+=track->getFitPointCount(kSvtId);
        nSsdHits+=track->getFitPointCount(kSsdId);
        nIstHits+=track->getFitPointCount(kIstId);
        nPxlHits+=track->getFitPointCount(kPxlId);
	//cout << "  ++++++++++++++++++++++++++++++ Added Track"<<endl;
      }while((iBreak=0));
      if (mTimg[kTrakTimg]) mTimg[kTrakTimg]->Stop();
      if (!iBreak) continue;
      BFactory::Free(track);
    }
   LOG_DEBUG << Form("***extendSeeds***:nTTot=%d nbSeed=%d nTFail=%d nTFilt=%d nTAdd=%d", 
     nTTot,nTSeed,nTFail,nTFilt,nTAdd) << endm;
   LOG_DEBUG << Form("***extendSeeds***:nTpcHits=%d nSvtHits=%d  nSsdHits=%d nPxlHits=%d nIstHits=%d",
	nTpcHits,nSvtHits,nSsdHits,nPxlHits,nIstHits)
   << endm;
}
//______________________________________________________________________________
void StiKalmanTrackFinder::extendTracks(double rMin)
{
static int nCall=0;nCall++;

  int nTKeep=0;
  int ntr = _trackContainer->size();
  int nTpcHits=0,nSvtHits=0,nSsdHits=0,nPxlHits=0,nIstHits=0, extended=0;
  
  for ( int itr=0;itr < ntr;itr++) {	//Track loop
    StiKalmanTrack *track = (StiKalmanTrack*)(*_trackContainer)[itr];
    if (track->getFlag()<=0) 	continue;
#ifdef ASSIGNVP
    if (vpAssign==2) {
      vpHit = (StiHit*)gTAssign->GetVal((long)track);
      vpDetector=0;vpLayer=0;
      if (vpHit) {
        vpDetector = vpHit->detector();
        vpLayer = vpDetector->getPlacement()->getLayerRadius();
    }}
#endif// ASSIGNVP

    extended = extendTrack(track,rMin);
    track->reduce();
    if (extended<0 || track->getFlag()<=0) {
      track->reduce(); continue;
    } else {
      StiHit dcaHit; dcaHit.makeDca();
      StiTrackNode *extenDca = track->extendToVertex(&dcaHit);
      if (extenDca) track->add(extenDca,kOutsideIn);
      track->reduce();
    }
    nTKeep++;
    nTpcHits+=track->getFitPointCount(kTpcId);
    nSvtHits+=track->getFitPointCount(kSvtId);
    nSsdHits+=track->getFitPointCount(kSsdId);
    nPxlHits+=track->getFitPointCount(kPxlId);
    nIstHits+=track->getFitPointCount(kIstId);
    track->reserveHits();
  }// end track loop
   LOG_DEBUG << Form("***extendTracks***: nTKeep=%d", nTKeep) << endm;
   LOG_DEBUG << Form("***extendTracks***: nTpcHits=%d nSvtHits=%d nSsdHits=%d nPxlHits=%d nIstHits=%d",
	nTpcHits,nSvtHits,nSsdHits,nPxlHits,nIstHits) << endm;
}
//______________________________________________________________________________
int StiKalmanTrackFinder::extendTrack(StiKalmanTrack *track,double rMin)
{
static int nCall=0; nCall++;
StiDebug::Break(nCall);
  int trackExtended   =0;  
  int trackExtendedOut=0;
  int status = 0;
  // invoke tracker to find or extend this track
  //cout <<"StiKalmanTrack::find(int) -I- Outside-in"<<endl;
    {
      if (debug()) cout << "StiKalmanTrack::find seed " << *((StiTrack *) track);
      trackExtended = find(track,kOutsideIn,rMin);
      if (trackExtended) {
          status = 0;
#ifdef ASSIGNVP
          if (vpAssign!=1) status = track->refit();
#else 
//        if (!rMin) status = track->refit();
#endif// ASSIGNVP
	  if(status) return -1;
      }	

    }
  // decide if an outward pass is needed.
  const StiKalmanTrackNode * outerMostNode = track->getOuterMostNode(2);
  if (!outerMostNode)
    {
      track->setFlag(-1);
      return 0;
    }
  if (outerMostNode->getX()<185. )
    {
      trackExtendedOut= find(track,kInsideOut);
      if (debug()) cout << "StiKalmanTrackFinder::extendTrack (track,kInsideOut)" << *((StiTrack *) track);
    }
  trackExtended |=trackExtendedOut;
  if (trackExtended) {
    status = track->approx(1);
//    if (status) return -1;
    status = track->refit();
    if (status) return -1;
  }
  //cout << " find track done" << endl;
  return trackExtended;
}
//______________________________________________________________________________
/*
 Extend all known tracks to primary vertex
 <p>
 Attempt an extension of all known tracks to the given primary vertex. If the extension is successfull,
 the vertex is added to the track as a node. Node that in this implementation, it is assumed the
 track has been pruned and thus consists of a single node sequence (as opposed to a tree).
 <p>
 <ol>
 <li>Loop on all tracks currently stored in track container.</li>
 <li>It is assumed that the track does not already have a main vertex associated with it.</li>
 <li>Attempt extension to the given vertex by a call to "extendToMainVertex".
 <li>If extension is successfull, the given vertex is added as node to the track.
 </ol>
 <p>
 <h3>Note</h3>
 Any exception thrown by "getInnerMostNode()" or "extendTrackToVertex()" are
 caught here and reported with "cout".
 */
//______________________________________________________________________________
void StiKalmanTrackFinder::extendTracksToVertex(StiHit* vertex)
{
  //cout << "SKTF::extendTracksToVertex() - vertex position " << vertex->x_g() << ", " << vertex->y_g() << ", " << vertex->z_g() << endl;

  int rawCount = 0;
  int goodCount= 0;
  int plus=0;
  int minus=0;
  int ntr = _trackContainer->size();
  for (int itr=0;itr<ntr;itr++) {
      StiKalmanTrack* track = (StiKalmanTrack*)(*_trackContainer)[itr];
      rawCount++;
      StiTrackNode *extended = track->extendToVertex(vertex);
      if (extended) {
        track->add(extended,kOutsideIn);
static int myRefit=0;
        if (myRefit && track->refit()) 			extended=0;
        if (extended && !extended->isValid()) 		extended=0;
        if (extended && extended->getChi2()>1000) 	extended=0;
      }
      track->reduce();
      // simple diagnostics
      if (extended) goodCount++;
      if (track->getCharge()>0) plus++;else minus++;
   }
  cout << "SKTF::extendTracksToVertex(StiHit* vertex) -I- rawCount:"<<rawCount<<endl
       << "                                          extendedCount:"<<goodCount<<endl
       << "                                                   plus:"<<plus<<endl
       << "                                                  minus:"<<minus<<endl;
}
//______________________________________________________________________________
void StiKalmanTrackFinder::extendTracksToVertices(const std::vector<StiHit*> &vertices)
{
  static const double RMAX2d=StiKalmanTrackFinderParameters::instance()->maxDca2dZeroXY();
  static const double DMAX3d=StiKalmanTrackFinderParameters::instance()->maxDca3dVertex();

  StiKalmanTrackNode *extended=0;
  int goodCount= 0, plus=0, minus=0;
  int nTracks = _trackContainer->size();
  int nVertex =         vertices.size();  
  if (!nVertex || !nTracks) return;

  for (int iTrack=0;iTrack<nTracks;iTrack++)		{
    StiKalmanTrack * track = (StiKalmanTrack*)(*_trackContainer)[iTrack];  
StiDebug::tally("Tracks");

    StiKalmanTrackNode *bestNode=0;  
    StThreeVectorD nearBeam;
    track->getNearBeam(&nearBeam);
    if (nearBeam.perp2()>RMAX2d*RMAX2d) 		continue;
    for (int iVertex=0;iVertex<nVertex;iVertex++) {
      StiHit *vertex = vertices[iVertex];
      if (fabs(track->getDca(vertex)) > DMAX3d)    	continue;
StiDebug::tally("PrimCandidates");
      if (mTimg[kPrimTimg]) mTimg[kPrimTimg]->Start(0);

      extended = (StiKalmanTrackNode*)track->extendToVertex(vertex);
      if (mTimg[kPrimTimg]) mTimg[kPrimTimg]->Stop();

      if (!extended) 					continue;
StiDebug::tally("PrimExtended");
      if (!bestNode) {bestNode=extended;		continue;}
      if (bestNode->getChi2()+log(bestNode->getDeterm())
         <extended->getChi2()+log(extended->getDeterm()))continue;
      BFactory::Free(bestNode);
      bestNode = extended;
    }//End vertex loop
    
    if(!bestNode) 			continue;
    track->add(bestNode,kOutsideIn);
StiDebug::tally("PrimAdded");
    int         ifail = 0;
static int REFIT=2005;
    bestNode->setUntouched();
if (REFIT) {
    ifail = track->refit();
    ifail |= (track->getInnerMostHitNode(3)!=bestNode);
}
    track->reduce();
// something is wrong. It is not a primary
    if (ifail) { track->removeLastNode();continue;}
    goodCount++;
StiDebug::tally("PrimRefited");
    if (track->getCharge()>0) plus++; else minus++;

  }//End track loop 
  if (debug()) {
    cout << "SKTF::extendTracksToVertices(...) -I- rawCount:"<<nTracks<<endl
	 << "                                 extendedCount:"<<goodCount<<endl
	 << "                                          plus:"<<plus<<endl
	 << "                                         minus:"<<minus<<endl;
  }
}

/// Find extension (track) to the given track seed in the given direction
/// Return Ok      if operation was successful
//______________________________________________________________________________
class StiKalmanTrackFinder::QAFind {
public: 
  double rmin;  //minimal radius allowed for search
  double sum; 	//summ of chi2
  int    hits;  //total number of hits
  int    nits;  //total number of no hits
  int    wits;  //total weight of precision hits
  int    qa;	// quality flag for current level
		//   qa =  1 == new hit accepted
		//   qa =  0 == no hits was expected. dead material or edge
		//   qa = -1 == hit expected but not found
		//   qa = -2 == close to beam, stop processing of it
		//   qa = -3 == fake track, stop processing of it
		//   qa = -4 == track can not be continued, stop processing of it

  	QAFind()		{reset();                  }
void 	reset()			{rmin=0; sum=0; hits =0; nits=0; qa=0;wits=0;}
};

//______________________________________________________________________________
bool StiKalmanTrackFinder::find(StiTrack * t, int direction,double rmin) // throws runtime_error, logic_error
{
static int nCall=0; nCall++;
  gLevelOfFind = 0;
StiKalmanTrackNode::Break(nCall);
  int nnBef,nnAft;
  double lnBef,lnAft;
  
  if(direction) rmin=0; //no limitation to outside
  StiKalmanTrack *track = dynamic_cast<StiKalmanTrack *> (t);
  nnBef = track->getNNodes(3);
  lnBef = track->getTrackLength();

  StiKalmanTrackNode *leadNode = track->getInnOutMostNode(direction,2);
  if (!leadNode) return 0;
  leadNode->cutTail(direction);
  assert(leadNode->isValid());
  QAFind qa; qa.rmin = rmin;
  find(track,direction,leadNode,qa);
  track->setFirstLastNode(leadNode);
  nnAft = track->getNNodes(3);
  lnAft = track->getTrackLength();
  return (nnAft>nnBef || lnAft>(lnBef+0.5));
}
//______________________________________________________________________________
void StiKalmanTrackFinder::find(StiKalmanTrack * track, int direction
                              ,StiKalmanTrackNode *leadNode,QAFind &qa) 
{
static int nCall=0; nCall++;
StiKalmanTrackNode::Break(nCall);

static const double degToRad = 3.1415927/180.;
static const double radToDeg = 180./3.1415927;
static const double ref1  = 50.*degToRad;
//static  const double ref2  = 2.*3.1415927-ref1;
static  const double ref1a  = 110.*degToRad;
  //  const double ref2a  = 2.*3.1415927-ref1a;
  gLevelOfFind++;
  StiDetector *tDet=0;
  int status;
  StiKalmanTrackNode testNode;
  int position;
  StiHit * stiHit;
  double  leadAngle,leadRadius;

  assert(leadNode->isValid());
  const StiDetector *leadDet = leadNode->getDetector();
  if (!leadDet)  throw runtime_error("SKTF::find() -E- leadDet==0");
  leadRadius = leadDet->getPlacement()->getNormalRadius();
  assert(leadRadius>0 && leadRadius<1000);
  if (leadRadius < qa.rmin) {gLevelOfFind--;return;}
  leadAngle  = leadDet->getPlacement()->getNormalRefAngle();


////  if ((!direction) && !nRefit && leadRadius <100 && track->getNNodes(3)>10) {
////     nRefit++; track->refit(); if (!leadNode->isValid()) return 0;
////  }
  
  
  double xg = leadNode->x_g();
  double yg = leadNode->y_g();
  double projAngle = atan2(yg,xg);
  if(debug() > 2)cout << "Projection Angle:"<<projAngle*180/3.1415<<endl;
    
  vector<StiDetectorNode*>::const_iterator layer;
  vector<StiDetectorNode*>::const_reverse_iterator rlayer;

  if ((!direction)) {
    if (debug() > 2) cout <<endl<< "out-in"<<endl;
    rlayer=_detectorContainer->rbeginRadial(leadDet); rlayer++;
  } else {
    if (debug() > 2) cout <<endl<< "in-out"<<endl;
    layer=_detectorContainer->beginRadial(leadDet);    layer++;
  }

  if (debug() > 2) cout <<endl<< "lead node:" << *leadNode<<endl<<"lead det:"<<*leadDet<<endl;

#ifdef ASSIGNVP
  int vpSelected=0;
#endif// ASSIGNVP
  
  while (((!direction)? rlayer!=_detectorContainer->rendRadial() : layer!=_detectorContainer->endRadial()))
  {do{//technical do
    vector<StiDetectorNode*>::const_iterator sector;
    vector<StiDetector*> detectors;
    if (debug() > 2) cout << endl<<"lead node:" << *leadNode<<endl<<" lead det:"<<*leadDet;

//#define PEREV

      //find all relevant detectors to visit.
    sector = (!direction)? _detectorContainer->beginPhi(rlayer):_detectorContainer->beginPhi(layer);
    for ( ; (!direction)? sector!=_detectorContainer->endPhi(rlayer):sector!=_detectorContainer->endPhi(layer); ++sector)
    {
       StiDetector * detector = (*sector)->getData();
       double angle  = detector->getPlacement()->getNormalRefAngle();
       double radius = detector->getPlacement()->getNormalRadius();
       assert(radius>0 && radius<1000);
#ifdef ASSIGNVP
       double ladius = detector->getPlacement()->getLayerRadius();
       if (!direction && vpAssign==2 && vpHit && fabs(ladius-vpLayer)<1e-5) {
         if (detector!=vpDetector) continue;
         detectors.clear();
         detectors.push_back(detector);
         vpSelected = 2005;
         break;
       }
#endif// ASSIGNVP
       if (radius < qa.rmin) {gLevelOfFind--;return;}
       double diff = radius-leadRadius;if (!direction) diff = -diff;
       if (diff<-1e-6 && debug()>3) {
          LOG_DEBUG << Form("TrackFinder: Wrong order: (%s).(%g) and (%s).(%g)"
	  ,leadDet->getName().c_str(),leadRadius 
	  ,detector->getName().c_str(),radius) << endm;
       }
       
       
       Int_t shapeCode = detector->getShape()->getShapeCode();
       Double_t OpenAngle = ref1;
       if (shapeCode >= kCylindrical) {
	 OpenAngle = ((StiCylindricalShape *) detector->getShape())->getOpeningAngle();
       } else {
	 if (radius <= 50 && radius>4.2)  OpenAngle = ref1a;
       }
       diff = projAngle-angle;
       if (diff >  M_PI) diff -= 2*M_PI;
       if (diff < -M_PI) diff += 2*M_PI;
       if (fabs(diff) > OpenAngle)	continue;
       detectors.push_back(detector);
    }

    int nDets = detectors.size(); 
    if (debug() > 2 && nDets==0) cout << "no detector of interest on this layer"<<endl;
    if (!nDets) continue;
    if (nDets>1) sort(detectors.begin(),detectors.end(),CloserAngle(projAngle) );
    for (vector<StiDetector*>::const_iterator d=detectors.begin();d!=detectors.end();++d)
    {
      tDet = *d;
      if (debug() > 2) {
	cout << endl<< "target det:"<< *tDet;
	cout << endl<< "lead angle:" << projAngle*radToDeg 
	     <<" this angle:" << radToDeg*(*d)->getPlacement()->getNormalRefAngle()<<endl;
      }
      //begin tracking here...
      testNode.reduce();testNode.reset();
      testNode.setChi2(1e55);
      position = testNode.propagate(leadNode,tDet,direction);
      if (position == kEnded) { gLevelOfFind--; return;}
      if (debug() > 2)  cout << "propagate returned:"<<position<<endl<< "testNode:"<<testNode;
      if (debug() >= 1) StiKalmanTrackNode::PrintStep();
      if (position<0 || position>kEdgeZplus) { 
	// not reaching this detector layer - stop track
	if (debug() > 2) cout << "TRACK DOES NOT REACH CURRENT volume"<<endl;
	continue; // will try the next available volume on this layer
      }
      if (debug() > 2) cout << "position " << position << "<=kEdgeZplus";
      assert(testNode.isValid());
      testNode.setDetector(tDet);
      int active = tDet->isActive(testNode.getY(),testNode.getZ());

      if (debug() > 2) cout << " vol active:" << active<<endl;
      double maxChi2 = tDet->getTrackingParameters()->getMaxChi2ForSelection();

      StiHitContino hitCont;

      if (active) {

	if (debug() > 2)cout<<" search hits";
	// active detector may have a hit
	vector<StiHit*> & candidateHits = _hitContainer->getHits(testNode);//,true);
	vector<StiHit*>::iterator hitIter;
	if (debug() > 2) cout << " candidates:"<< candidateHits.size();
        
	for (hitIter=candidateHits.begin();hitIter!=candidateHits.end();++hitIter)
	{
	  stiHit = *hitIter;
          if (stiHit->detector() && stiHit->detector()!=tDet) continue;
          status = testNode.nudge(stiHit);
          testNode.setReady();
          if (status)		continue;
	  chi2 = testNode.evaluateChi2(stiHit);
	  if (debug() > 2)   cout<< " got chi2:"<< chi2 << " for hit:"<<*stiHit<<endl;
	  if (chi2>maxChi2) 	continue;
	  hitCont.add(stiHit,chi2,testNode.getDeterm());
	  if (debug() > 2) cout << " hit selected"<<endl;
	}// for (hitIter)
      }//if(active)

      int nHits = hitCont.getNHits();
      assert(nHits<100);
      testNode.setHitCand(nHits);
      if (direction) {
        nHits=1;
      } else {
        int flg = (testNode.getX()< kRMinTpc)? useComb()&3:useComb()>>2;
        if ((flg&2) || !nHits) 	nHits++;
        if ((flg&1)==0) 	nHits=1;
        
      }

#ifdef ASSIGNVP
      do {
        if (!active) 		break;
        if (direction) 		break;
	if (testNode.getX()>16) break;
	if (testNode.getX()<14) break;
	
        switch (vpAssign) {
      
          case 1:
          for (int jHit=0;jHit<nHits; jHit++){
            stiHit = hitCont.getHit(jHit);
            if (!stiHit) continue;
            chi2 = hitCont.getChi2(jHit);
            double detr = hitCont.getDetr(jHit);
	    gTAssMtx->SetChi2((long)track,(long)stiHit,chi2+log(detr));
	  } 
	  return;  

          case 2:
          if (!vpSelected) break;
	  hitCont.reset();
	  stiHit= vpHit;
          nHits=1;
          if (stiHit) { 
            chi2 = testNode.evaluateChi2(stiHit);
            hitCont.add(stiHit,chi2);
            assert(tDet==stiHit->detector());
	  }
	}
      }while(0);
#endif //ASSIGNVP
      QAFind qaBest,qaTry;
      for (int jHit=0;jHit<nHits; jHit++)
      {//Loop over Hits
        stiHit = hitCont.getHit(jHit);
	StiKalmanTrackNode * node = _trackNodeFactory->getInstance();
	*node = testNode;
        status = 0;
        do {//fake do
          if (!stiHit) break;
          node->setIHitCand(jHit);
          assert(node->getHitCand());
          node->setHit(stiHit);
          status = node->updateNode();
          if (status)  break;
          node->setChi2(hitCont.getChi2(jHit));
          if (!direction && node->getX()< kRMinTpc) node->saveInfo(); //Save info for pulls 
	  if (debug() & 8) {cout << Form("%5d ",status); StiKalmanTrackNode::PrintStep();}
        }while(0);
        if (status)  {_trackNodeFactory->free(node); continue;}

        qaTry = qa;
        nodeQA(node,position,active,qaTry);
	leadNode->add(node,direction);
        if (qaTry.qa>-2) find(track,direction,node,qaTry);
        
        if (jHit==0) { qaBest=qaTry; continue;}
        int igor = compQA(qaBest,qaTry,maxChi2);
        if (igor<0)  { leadNode->remove(0);}
        else         { leadNode->remove(1);qaBest=qaTry;}
      }
      qa = qaBest; gLevelOfFind--; return;
    }//End Detectors
  }while(0);
  if(!direction){++rlayer;}else{++layer;}}
//end layers
  gLevelOfFind--;
  return;
}
//______________________________________________________________________________
void StiKalmanTrackFinder::nodeQA(StiKalmanTrackNode *node, int position
                                 ,int active,QAFind &qa)
{
  int maxNullCount           = StiKalmanTrackFinderParameters::instance()->maxNullCount()+3;
  int maxContiguousNullCount = StiKalmanTrackFinderParameters::instance()->maxContiguousNullCount()+3;
//		Check and count node
  StiHit *hit = node->getHit();
  if (hit) {
    if (debug() > 2)cout << " got Hit! "<<endl ;
//  const StiDetector *detector = hit->detector();
    qa.sum += node->getChi2() + log(node->getDeterm());
    qa.hits++; qa.qa=1;
    if (node->getRxy() < kRMinTpc) {
      qa.wits+=StiKalmanTrackFinderParameters::instance()->hitWeight((int)node->getRxy());
    }
    node->getHitCount()++;
    node->getContigHitCount()++;
    if (node->getContigHitCount()>StiKalmanTrackFinderParameters::instance()->minContiguousHitCountForNullReset())
       node->getContigNullCount() = 0;

  } else if (position>0 || !active) {// detectors edge - don't really expect a hit here
    qa.qa=0;

  } else {// there should have been a hit but we found none
      if (debug() > 2) cout << " no hit but expected one"<<endl;
      node->getNullCount()++; 
      node->getContigNullCount()++;
      node->getContigHitCount() = 0;
      qa.nits++; qa.qa=-1;
      if (node->getNullCount()>maxNullCount) 			qa.qa= -3;
      if (node->getContigNullCount()>maxContiguousNullCount)	qa.qa= -3;
  }//node->getHit()

  double xg = node->x_g();
  double yg = node->y_g();
  if ((xg*xg + yg*yg) < 4.2*4.2) qa.qa= -2;

}
//______________________________________________________________________________
int StiKalmanTrackFinder::compQA(QAFind &qaBest,QAFind &qaTry,double maxChi2)
{
   int ians;
   ians = qaTry.wits-qaBest.wits;
//	One SVT hit is worse than zero
   if (!qaBest.wits &&  qaTry.wits &&  qaTry.wits < StiKalmanTrackFinderParameters::instance()->sumWeight()) return -1;
   if ( !qaTry.wits && qaBest.wits && qaBest.wits < StiKalmanTrackFinderParameters::instance()->sumWeight()) return  1;
   				        if (ians)	return ians;
   ians =  qaTry.hits-qaBest.hits;	if (ians)	return ians;
   ians = qaBest.nits- qaTry.nits;	if (ians)	return ians;
   if (qaBest.sum  <= qaTry.sum ) 			return -1;
   							return  1;
}

//______________________________________________________________________________
StiTrack * StiKalmanTrackFinder::findTrack(double rMin)
{
assert(0);
  StiTrack * track = 0;
  try
    {
      if (!_trackSeedFinder) throw runtime_error("StiKalmanTrackFinder::findTrack() -E- No Track seed finder instance available");
      track = _trackSeedFinder->findTrack(rMin);
      if (track)
        {
        track->find();
        if (!_trackFilter || _trackFilter->filter(track) ) _trackContainer->push_back(track);
        }
    }
  catch (runtime_error & rte)
    {
    cout << "StiKalmanTrackFinder::findTrack() - Run Time Error :\n" << rte.what() << endl;
    }
  return track;
}

/*
 void StiKalmanTrackFinder::fitNextTrack()
 {
   StiTrack * track = 0;
   try
     {
       track = _trackSeedFinder->findTrack();
       if (track)
         {
         track->fit(kOutsideIn);
         if (!_trackFilter || _trackFilter->filter(track) ) _trackContainer->push_back(track);
         }
     }
   catch (runtime_error & rte)
     {
     cout << "StiKalmanTrackFinder::fitNextTrack() - Run Time Error :" << rte.what() << endl;
     }
 }
 */

//______________________________________________________________________________
void StiKalmanTrackFinder::setTiming()
{
  for (int it=0;it<(int)(sizeof(mTimg)/sizeof(mTimg[0]));it++){
    mTimg[it]= new TStopwatch(); mTimg[it]->Stop();    } 
}
//______________________________________________________________________________
void StiKalmanTrackFinder::finish() const
{
static const char *timg[] = {"SeedFnd","TrakFnd","PrimFnd",0};
  if (mTimg[0]) {
    for (int i=0;timg[i];i++) {
      Info("TrackFinder::Timing","%s(%d) \tCpuTime = %6.2f seconds,\tPerTrak = %g seconds"
      ,timg[i],mTimg[i]->Counter(),mTimg[i]->CpuTime()
      ,mTimg[i]->CpuTime()/mTimg[i]->Counter());    
  } }
}
//______________________________________________________________________________
CloserAngle::CloserAngle(double refAngle)
  : _refAngle(refAngle)
{ }

//______________________________________________________________________________
bool CloserAngle::operator()(const StiDetector*lhs, const StiDetector* rhs)
{
  double lhsa = lhs->getPlacement()->getNormalRefAngle();
  double rhsa = rhs->getPlacement()->getNormalRefAngle();
  double lhsda = fabs(lhsa-_refAngle); if (lhsda>3.1415) lhsda-=3.1415;
  double rhsda = fabs(rhsa-_refAngle); if (rhsda>3.1415) rhsda-=3.1415;
  return lhsda<rhsda;
}

