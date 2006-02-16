///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  StEventQAMaker class for QA Histograms using StEvent                 //
//     adapted from St_QA_Maker                                          //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include "PhysicalConstants.h"
#include <math.h>
#include "TROOT.h"
#include "TMath.h"
#include "SystemOfUnits.h"
#include "StQABookHist.h"
#include "TH1.h"
#include "TH2.h"
//#include "TSpectrum.h"
#include "StEventQAMaker.h"
#include "StEventTypes.h"
#include "StMcEventTypes.hh"
#include "StTpcDedxPidAlgorithm.h"
#include "StDbUtilities/StCoordinates.hh"
// include this because it's not in StCoordinates.hh yet
#include "StDbUtilities/StSvtCoordinateTransform.hh"
#include "HitHistograms.h"
#include "StTpcDb/StTpcDb.h"
#include "StarClassLibrary/StTimer.hh"
#include "StMessMgr.h"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/others/StEmcMath.h"
#include "StarClassLibrary/BetheBloch.h"

#include "StPmdUtil/StPmdCollection.h"
#include "StPmdUtil/StPmdDetector.h"
#include "StPmdUtil/StPmdMapUtil.h"
#include "StPhmdCollection.h"
#include "StPhmdDetector.h"
#include "StPhmdModule.h"


static StEmcGeom* emcGeom[4];

// These are the mean z positions of the FTPC padrows (1-20).
// The width of each padrow in z is 2 cm.
static float ftpcPadrowZPos[20] = {162.75,171.25,184.05,192.55,205.35,
  213.85,226.65,235.15,247.95,256.45,
  -162.75,-171.25,-184.05,-192.55,-205.35,
  -213.85,-226.65,-235.15,-247.95,-256.45};

ClassImp(StEventQAMaker)

Bool_t isTriggerInSubRange(const StTriggerId* tr, UInt_t lo, UInt_t hi) {
  // Only the last three digits matter...
  // ...according to Jeff Landgraf's triggerID scheme
  // (only for triggerIDs >= 10000)
  vector<unsigned int> idVec = tr->triggerIds();
  for (UInt_t i=lo; i<=hi; i++) {
    UInt_t j = i - ((i/1000)*1000);
    for (UInt_t k=0; k<idVec.size(); k++)
      if (idVec[k] >= 10000 &&
          j == (idVec[k] - ((idVec[k]/1000)*1000))) return kTRUE;
  }
  return kFALSE;
}
Bool_t isTriggerInRange(const StTriggerId* tr, UInt_t lo, UInt_t hi) {
  for (UInt_t i=lo; i<=hi; i++)
    if (tr->isTrigger(i)) return kTRUE;
  return kFALSE;
}
Bool_t isTriggerAmong(const StTriggerId* tr, UInt_t n, ... ) {
  va_list ap;
  va_start(ap,n);
  Bool_t is=kFALSE;
  for (UInt_t i=0; (!is) && (i<n); i++)
    if (tr->isTrigger(va_arg(ap, unsigned int))) is=kTRUE;
  va_end(ap);
  return is;
}

//_____________________________________________________________________________
StEventQAMaker::StEventQAMaker(const char *name, const char *title) :
StQAMakerBase(name,title,"StE"), event(0), mHitHist(0), mPmdGeom(0), maputil(0) {
  allTrigs = kFALSE;
  mRunNumber = -1;
}


//_____________________________________________________________________________
Int_t StEventQAMaker::Finish() {
  
  return StMaker::Finish();
}


//_____________________________________________________________________________
Int_t StEventQAMaker::Init() {

  return StQAMakerBase::Init();
}


//_____________________________________________________________________________
/// StEventQAMaker - InitRun; Book histograms, set defaults for member functions
Int_t StEventQAMaker::InitRun(int runnumber) {
  
  if(! mHitHist){
    mHitHist = new HitHistograms("QaDedxAllSectors","dE/dx for all TPC sectors",
                                 100,0.,1.e-5,2,this);
  }
  
  if ((gROOT->GetClass("StEmcMath")) && (gROOT->GetClass("StEmcGeom"))) {
    for(Int_t i=0; i<4; i++) {emcGeom[i] = StEmcGeom::getEmcGeom(i+1);} // 3-oct-2001 by PAI
  }
  return kStOK;
}

//_____________________________________________________________________________
/// StEventQAMaker - Make; fill histograms
Int_t StEventQAMaker::Make() {
  
  n_prim_good = 0;
  n_glob_good = 0;
  
  event = (StEvent*) GetInputDS("StEvent");
  if (!event) {
    gMessMgr->Error("StEventQAMaker::Make(): no event found!");
    return kStErr;
  }
  Bool_t realData = (event->info()->type() == "NONE");
  if (eventCount==0) {
    if (histsSet == StQA_Undef) {
      if (realData) {
	histsSet = StQA_AuAuOld;
      } else {
	// process Monte Carlo events
	histsSet = StQA_MC;
      }
    }
    BookHistTrigger();
  }
  UInt_t tword = 0;
  Bool_t doEvent = kTRUE;
  Int_t evClasses[32];
  Int_t nEvClasses = 1;
  Int_t run_num = event->runId();
  Int_t run_year = run_num/1000000;         // Determine run year from run #
  if (allTrigs) {
    
    histsSet = StQA_pp;
    
  } else if (run_year <= 3) {
    
    // Old way of determining the trigger (from L0 trigger word)
    StTrigger* l0Trig = event->l0Trigger();
    if ((l0Trig) && (run_year != 1)) {      // Don't use year 1 trigger word
      if (realData) doEvent = kFALSE;
      tword = l0Trig->triggerWord();
      if (tword) {
        if ((tword >= 0x1000) && (tword < 0x1100)) {
          mTrigWord->Fill(1.); // "MinBias"
	  doEvent = kTRUE;
        } else if ((tword >= 0x1100) && (tword < 0x1200)) {
          mTrigWord->Fill(2.); // "Central"
	  doEvent = kTRUE;
        } else if ((tword >= 0x1200) && (tword < 0x2000)) {
          mTrigWord->Fill(3.); // "Other Physics"
	  doEvent = kTRUE;
        } else if ((tword >= 0x2000) && (tword < 0x3000)) {
          mTrigWord->Fill(4.); // "pp Physics"
	  doEvent = kTRUE;
          if ((eventCount==0) && (histsSet==StQA_AuAuOld)) histsSet = StQA_pp;
        } else if (tword == 0xF200) {
          mTrigWord->Fill(7.); // "Laser"
        } else {
          mTrigWord->Fill(8.); // "Other"
        }
      } else {  // tword
        if (realData)
          gMessMgr->Warning("StEventQAMaker::Make(): trigger word=0 !!!!!");
      }  // tword
    } else { // No trigger info or year 1 data!
      gMessMgr->Warning("StEventQAMaker::Make(): No trigger info...processing anyhow");
    }
    
  } else {  // run_year >= 4
    
    
    StTriggerIdCollection* trigIdColl = event->triggerIdCollection();
    const StTriggerId* trigId = ((trigIdColl) ? trigIdColl->nominal() : 0);
    if (trigId) {
      histsSet = StQA_Undef;
      if (run_num < 4363000) histsSet = StQA_dAu;
      else if (run_num < 5120000) histsSet = StQA_AuAu;
      else if (run_num < 5999999) histsSet = StQA_pp;
      if (realData) doEvent = kFALSE;
      nEvClasses=0;
      tword = trigId->mask();
      
      // dAu
      
      if (isTriggerAmong(trigId,4,2001,2002,2003,2004)) {
	mTrigWord->Fill(1.); // "MinBias"
	doEvent = kTRUE;
	evClasses[nEvClasses] = 1;
	nEvClasses++;
	histsSet = StQA_dAu;
      }
      if (isTriggerAmong(trigId,2,2201,2202)) {
	mTrigWord->Fill(3.); // "High pt"
	doEvent = kTRUE;
	evClasses[nEvClasses] = 2;
	nEvClasses++;
	histsSet = StQA_dAu;
      }
      if ((nEvClasses==0) && (isTriggerInRange(trigId,2000,2999))) {
	mTrigWord->Fill(8.); // "Other"
	evClasses[nEvClasses] = 3;
	nEvClasses++;
	histsSet = StQA_dAu;
      }
      
      // pp
      
      if ((nEvClasses==0) && (isTriggerInRange(trigId,1000,1999))) {
	mTrigWord->Fill(4.); // "pp Physics"
	doEvent = kTRUE;
	evClasses[nEvClasses] = 1;
	nEvClasses++;
	histsSet = StQA_pp;
      }
      
      // AuAu
      
      if (isTriggerInSubRange(trigId,0,99)) {
	//if (isTriggerInRange(trigId,15007,15099))
	mTrigWord->Fill(1.); // "MinBias"
	doEvent = kTRUE;
	evClasses[nEvClasses] = 1;
	nEvClasses++;
	histsSet = StQA_AuAu;
      }
      if (isTriggerInSubRange(trigId,100,199)) {
	//if (isTriggerInRange(trigId,15105,15199))
	mTrigWord->Fill(2.); // "Central"
	doEvent = kTRUE;
	evClasses[nEvClasses] = 2;
	nEvClasses++;
	histsSet = StQA_AuAu;
      }
      if (isTriggerInSubRange(trigId,200,299)) {
	//if (isTriggerInRange(trigId,15202,15299))
	mTrigWord->Fill(5.); // "High Tower"
	doEvent = kTRUE;
	evClasses[nEvClasses] = 3;
	nEvClasses++;
	histsSet = StQA_AuAu;
      }
      if ((nEvClasses==0) && (isTriggerInRange(trigId,15300,15999))) {
	mTrigWord->Fill(3.); // "Other Physics"
	evClasses[nEvClasses] = 4;
	nEvClasses++;
	histsSet = StQA_AuAu;
      }
      
    } else {  // No trigger info!
      gMessMgr->Warning("StEventQAMaker::Make(): No trigger info");
    }
  }  // allTrigs
  
  for (int bitn=0; bitn<32; bitn++) {
    if (tword>>(bitn) & 1U)
      mTrigBits->Fill((Float_t) bitn);
  }
  
  if (!doEvent) {
    gMessMgr->Message() << "StEventQAMaker::Make(): "
    << "skipping because trigger word=" << tword << endm;
    return kStOk;
  }
  
  if (!mNullPrimVtx) BookHist();
  
  multiplicity = event->trackNodes().size();
  switch (histsSet) {
    case (StQA_AuAuOld): {
      if (multiplicity < 50) eventClass = 0;
      else if (multiplicity < 500) eventClass = 1;
      else if (multiplicity < 2500) eventClass = 2;
      else eventClass = 3;
      break; }
    case (StQA_AuAu):
    case (StQA_dAu) : {
      eventClass = evClasses[0];
      break; }
    default: {
      eventClass = 1;
    }
  }
  
  int makeStat = kStOk;
  float vertExists;
  
  // only process if a primary vertex exists !!!
  if (event->primaryVertex()) {
    vertExists = 1.;
    fillHists = kTRUE;
  } else {
    vertExists = -1.;
    fillHists = kFALSE;
    gMessMgr->Warning("StEventQAMaker::Make(): no primary vertex found!");
  }
  mNullPrimVtx->Fill(vertExists);
  
  if (nEvClasses == 1) evClasses[0] = eventClass;
  
  for (int i=0; i<nEvClasses; i++) {
    eventClass = evClasses[i];
    makeStat = StQAMakerBase::Make();
    if ((evClasses[i]) && (histsSet != StQA_MC) && (hists))
      hists->mNullPrimVtxClass->Fill(vertExists);
    if (makeStat != kStOk) break;
  }
  return makeStat;
}


//_____________________________________________________________________________
/// Fill histograms for event summary
void StEventQAMaker::MakeHistEvSum() {
  
  //PrintInfo();
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling software monitor histograms ");
  
  if (event->softwareMonitor()) {
    StTpcSoftwareMonitor *tpcMon = event->softwareMonitor()->tpc();
    StFtpcSoftwareMonitor *ftpcMon = event->softwareMonitor()->ftpc();
    if (tpcMon) {
      Float_t tpcChgWest=0;
      Float_t tpcChgEast=0;
      for (UInt_t i=0; i<24; i++) {
        if (i<12)
	  tpcChgWest += tpcMon->chrg_tpc_in[i]+tpcMon->chrg_tpc_out[i];
        else
	  tpcChgEast += tpcMon->chrg_tpc_in[i]+tpcMon->chrg_tpc_out[i];
      }
      m_glb_trk_chg->Fill(tpcChgEast/(tpcChgWest+1.e-10),(float) eventClass);
    }
    if (ftpcMon) {
      m_glb_trk_chgF->Fill(ftpcMon->chrg_ftpc_tot[1]/(ftpcMon->chrg_ftpc_tot[0]+1.e-10),
			   (float) eventClass);
    }
  }
}


//-----------------------------------------------------------------
void StEventQAMaker::MakeHistGlob() {
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling global track histograms ");
  
  StSPtrVecTrackNode &theNodes = event->trackNodes();
  StThreeVectorF pvert;
  if (event->primaryVertex()) pvert = event->primaryVertex()->position();
  Int_t cnttrk=0;
  Int_t cnttrkT=0;
  Int_t cnttrkTS=0;
  Int_t cnttrkg=0;
  Int_t cnttrkgT=0;
  Int_t cnttrkgTS=0;
  Int_t cnttrkgTTS=0;
  Int_t cnttrkgFE=0;
  Int_t cnttrkgFW=0;
  
/*
  // Determine if Sti was run:
  if ((!ITTF) &&
      ((GetChain()->GetMaker("StiMaker")) ||
       (GetChain()->FindByName("StiRunco"))) ) {
    ITTF = kTRUE;
    EST = 0;
  }

  // Determine if EST was run:
  // If ITTF, definitely no EST
  // If estGlobal tracks, definitely EST
  // Otherwise we are never really sure...?
  //   (could be global tracks with no estGlobal tracks)
  if ((!ITTF) && (EST <= 0)) {
    int estTrackCount = 0;
    int gloTrackCount = 0;
    for (UInt_t i=0; i<theNodes.size(); i++) {
      StTrack *globtrk = theNodes[i]->track(estGlobal);
      if (!globtrk) continue;
      estTrackCount += theNodes[i]->entries(estGlobal);
    }
    for (UInt_t i=0; i<theNodes.size(); i++) {
      StTrack *globtrk = theNodes[i]->track(global);
      if (!globtrk) continue;
      gloTrackCount += theNodes[i]->entries(global);
    }
    if (estTrackCount > 0) EST = 1; // EST was run
    else if (gloTrackCount > 0) EST = 0; // EST appears not to have been run
    // else whatever state we were in before
  }
  StTrackType estOrGlobal = global;
  if (EST > 0) estOrGlobal = estGlobal;
*/

  for (UInt_t i=0; i<theNodes.size(); i++) {
    StTrackType estOrGlobal = estGlobal;
    StTrack *globtrk = theNodes[i]->track(estOrGlobal);
    if (!globtrk) {
      estOrGlobal = global;
      globtrk = theNodes[i]->track(estOrGlobal);
      if (!globtrk) continue;
    }
    cnttrk += theNodes[i]->entries(estOrGlobal);
    hists->m_globtrk_iflag->Fill(globtrk->flag());
    const StTrackTopologyMap& map=globtrk->topologyMap();
    if (map.trackTpcOnly()) cnttrkT++;
    if (map.trackTpcSvt()) cnttrkTS++;
    if (globtrk->flag()>0) {
      StTrackGeometry* geom = globtrk->geometry();
      StTrackFitTraits& fTraits = globtrk->fitTraits();
      StTrackDetectorInfo* detInfo = globtrk->detectorInfo();
      
      n_glob_good++;
      cnttrkg++;
      Float_t pT = -999.;
      pT = geom->momentum().perp();
      if (pT <= 1.e-10) continue;
      Float_t lmevpt = TMath::Log10(1e-30+pT*1000.0);
      Float_t theta = TMath::ASin(1.) - geom->dipAngle();
      Float_t thetad = theta/degree;
      Float_t eta = geom->momentum().pseudoRapidity();
      Float_t gmom = abs(geom->momentum());
      Float_t lmevmom = TMath::Log10(1e-30+gmom*1000.0);
      Float_t chisq0 = fTraits.chi2(0);
      Float_t chisq1 = fTraits.chi2(1);
      Float_t nfitntot = (Float_t(fTraits.numberOfFitPoints())) /
	(Float_t(detInfo->numberOfPoints())+1.e-10);
      Float_t nfitnmax = (Float_t(fTraits.numberOfFitPoints())) /
	(Float_t(globtrk->numberOfPossiblePoints())+1.e-10);
      const StThreeVectorF& firstPoint = detInfo->firstPoint();
      const StThreeVectorF& lastPoint = detInfo->lastPoint();
      const StThreeVectorF& origin = geom->origin();
      Float_t psi = geom->psi()/degree;
      if (psi<0) psi+=360;
      Float_t orphi = origin.phi()/degree;
      if (orphi<0) orphi+=360;
      StPhysicalHelixD hx = geom->helix();
      // get the helix position closest to the first point on track
      double sFirst = hx.pathLength(firstPoint);
      // get the helix position closest to the last point on track
      double sLast = hx.pathLength(lastPoint);
      
      StThreeVectorF dif = firstPoint - hx.at(sFirst);
      StThreeVectorF difl = lastPoint - hx.at(sLast);
      Float_t xcenter = hx.xcenter();
      Float_t ycenter = hx.ycenter();
      Float_t rcircle = 1./hx.curvature();
      Float_t centerOfCircleToFP = ::sqrt(::pow(xcenter-firstPoint.x(),2) +
					  ::pow(ycenter-firstPoint.y(),2));
      Float_t centerOfCircleToLP = ::sqrt(::pow(xcenter-lastPoint.x(),2) +
					  ::pow(ycenter-lastPoint.y(),2));
      Float_t azimdif = dif.perp();
      if (rcircle<centerOfCircleToFP) azimdif *= -1.;
      Float_t azimdifl = difl.perp();
      if (rcircle<centerOfCircleToLP) azimdifl *= -1.;
      Float_t radf = firstPoint.perp();
      
      Float_t logImpact = TMath::Log10(1e-30+globtrk->impactParameter());
      Float_t sImpact = hx.geometricSignedDistance(pvert.x(),pvert.y());
      Float_t logCurvature = TMath::Log10(1e-30+geom->curvature());
      
      // pathLength(double x,double y) should return path length at
      // DCA in the xy-plane to a given point
      double S = hx.pathLength(0,0);
      StThreeVectorD dcaToBeam = hx.at(S);
      
      // from Lanny on 2 Jul 1999 9:56:03
      //1. x0,y0,z0 are coordinates on the helix at the starting point, which
      //   should be close to the first TPC hit position assigned to the track.
      //   The latter, different quantity is in x_first[3].
      
      // from Helen on 14 Jul 1999 - she now fills chisq0,1 with chisq/dof
      // so it doesn't need to be calculated here 
      
      // check if the track has hits in a detector -CPL
      if (map.hasHitInDetector(kUnknownId)) hists->m_det_id->Fill(kUnknownId);
      if (map.hasHitInDetector(kTpcId)) hists->m_det_id->Fill(kTpcId);
      if (map.hasHitInDetector(kSvtId)) hists->m_det_id->Fill(kSvtId);
      if (map.hasHitInDetector(kRichId)) hists->m_det_id->Fill(kRichId);
      if (map.hasHitInDetector(kFtpcWestId)) hists->m_det_id->Fill(kFtpcWestId);
      if (map.hasHitInDetector(kFtpcEastId)) hists->m_det_id->Fill(kFtpcEastId);
      if (map.hasHitInDetector(kTofId)) hists->m_det_id->Fill(kTofId);
      if (map.hasHitInDetector(kCtbId)) hists->m_det_id->Fill(kCtbId);
      if (map.hasHitInDetector(kSsdId)) hists->m_det_id->Fill(kSsdId);
      if (map.hasHitInDetector(kBarrelEmcTowerId)) hists->m_det_id->Fill(kBarrelEmcTowerId);
      if (map.hasHitInDetector(kBarrelEmcPreShowerId)) hists->m_det_id->Fill(kBarrelEmcPreShowerId);
      if (map.hasHitInDetector(kBarrelSmdEtaStripId)) hists->m_det_id->Fill(kBarrelSmdEtaStripId);
      if (map.hasHitInDetector(kBarrelSmdPhiStripId)) hists->m_det_id->Fill(kBarrelSmdPhiStripId);
      if (map.hasHitInDetector(kEndcapEmcTowerId)) hists->m_det_id->Fill(kEndcapEmcTowerId);
      if (map.hasHitInDetector(kEndcapEmcPreShowerId)) hists->m_det_id->Fill(kEndcapEmcPreShowerId);
      if (map.hasHitInDetector(kEndcapSmdUStripId)) hists->m_det_id->Fill(kEndcapSmdUStripId);
      if (map.hasHitInDetector(kEndcapSmdVStripId)) hists->m_det_id->Fill(kEndcapSmdVStripId);
      if (map.hasHitInDetector(kZdcWestId)) hists->m_det_id->Fill(kZdcWestId);
      if (map.hasHitInDetector(kZdcEastId)) hists->m_det_id->Fill(kZdcEastId);
      if (map.hasHitInDetector(kMwpcWestId)) hists->m_det_id->Fill(kMwpcWestId);
      if (map.hasHitInDetector(kMwpcEastId)) hists->m_det_id->Fill(kMwpcEastId);
      if (map.hasHitInDetector(kTpcSsdId)) hists->m_det_id->Fill(kTpcSsdId);
      if (map.hasHitInDetector(kTpcSvtId)) hists->m_det_id->Fill(kTpcSvtId);
      if (map.hasHitInDetector(kTpcSsdSvtId)) hists->m_det_id->Fill(kTpcSsdSvtId);
      if (map.hasHitInDetector(kSsdSvtId)) hists->m_det_id->Fill(kSsdSvtId);
      
      // these histogram additions are for Lanny's evr QA histograms
      hists->m_dcaToBeamXY->Fill(dcaToBeam.x(),dcaToBeam.y());
      hists->m_dcaToBeamZ1->Fill(dcaToBeam.z());
      hists->m_dcaToBeamZ2->Fill(dcaToBeam.z());
      hists->m_dcaToBeamZ3->Fill(dcaToBeam.z());
      if (map.trackTpcOnly()) {
	hists->m_zDcaTanl->Fill(dcaToBeam.z(),TMath::Tan(geom->dipAngle()));
	hists->m_zDcaZf->Fill(dcaToBeam.z(),firstPoint.z());
      }
      if (map.trackTpcSvt())
	hists->m_zDcaTanl->Fill(dcaToBeam.z(),TMath::Tan(geom->dipAngle()));
      if (map.trackTpcSvt() && radf>40)
	hists->m_zDcaZf->Fill(dcaToBeam.z(),firstPoint.z());
      hists->m_zDcaPsi->Fill(dcaToBeam.z(),psi);
      hists->m_zDcaPhi0->Fill(dcaToBeam.z(),orphi);
      
      // calculate the probability of a fit being correct
      // number of degrees of freedom = fitpoints-5 (5 params constrain track)
      Double_t ndf = 2*fTraits.numberOfFitPoints()-5;
      Double_t probability = TMath::Prob(chisq0*ndf,(int) ndf);
      hists->m_globtrk_fit_prob->Fill(probability);
      
      // now fill all TPC histograms ------------------------------------------------
      if (map.trackTpcOnly()) {
	
	cnttrkgT++;
	cnttrkgTTS++;
	// these are TPC only
	// m_glb_f0 uses hist class StMultiH1F
        hists->m_glb_f0->Fill(dif.x(),0.);
        hists->m_glb_f0->Fill(dif.y(),1.);
        hists->m_glb_f0->Fill(dif.z(),2.);
	
        hists->m_glb_xf0->Fill(dif.x());
        hists->m_glb_yf0->Fill(dif.y());
        hists->m_glb_zf0->Fill(dif.z());
	hists->m_glb_rzf0->Fill(azimdif,0.);
        hists->m_glb_rzf0->Fill(dif.z(),1.);
	hists->m_glb_rzl0->Fill(azimdifl,0.);
        hists->m_glb_rzl0->Fill(difl.z(),1.);
        hists->m_glb_impactT->Fill(logImpact,2.);
        hists->m_glb_simpactT->Fill(sImpact,2.);
        if ((firstPoint.z() < 0) && (lastPoint.z() < 0)) { // east-only
          hists->m_glb_impactT->Fill(logImpact,0.);
          hists->m_glb_simpactT->Fill(sImpact,0.);
        }
        if ((firstPoint.z() > 0) && (lastPoint.z() > 0)) { // west-only
          hists->m_glb_impactT->Fill(logImpact,1.);
          hists->m_glb_simpactT->Fill(sImpact,1.);
        }
        hists->m_glb_impactrT->Fill(globtrk->impactParameter());
        hists->m_glb_impactTTS->Fill(logImpact,1.);
        hists->m_glb_impactrTTS->Fill(globtrk->impactParameter(),1.);
	
	// TPC padrow histogram
	StTpcCoordinateTransform transformer(gStTpcDb);
	StGlobalCoordinate globalHitPosition(firstPoint);
	StTpcPadCoordinate padCoord;
	transformer(globalHitPosition,padCoord);
        hists->m_glb_padfT->Fill(padCoord.row());
	
        hists->m_pointT->Fill(detInfo->numberOfPoints());
        hists->m_max_pointT->Fill(globtrk->numberOfPossiblePoints());
        hists->m_fit_pointT->Fill(fTraits.numberOfFitPoints());
        hists->m_fit_pointTTS->Fill(fTraits.numberOfFitPoints(),1.);
        hists->m_glb_chargeT->Fill(geom->charge());
	
        hists->m_glb_r0T->Fill(origin.perp());
        hists->m_glb_phi0T->Fill(orphi);
	
	if (firstPoint.z() < 0) {
	  hists->m_glb_padfTEW->Fill(padCoord.row(),0.);
	  if (firstPoint.phi() < 0)
	    hists->m_glb_phifT->Fill(360+firstPoint.phi()/degree,0.);
	  else
	    hists->m_glb_phifT->Fill(firstPoint.phi()/degree,0.);
	}
	else if (firstPoint.z() > 0) {
	  hists->m_glb_padfTEW->Fill(padCoord.row(),1.);
	  if (firstPoint.phi() < 0)
	    hists->m_glb_phifT->Fill(360+firstPoint.phi()/degree,1.);
	  else
	    hists->m_glb_phifT->Fill(firstPoint.phi()/degree,1.);
	}
	
        hists->m_glb_z0T->Fill(origin.z());
        hists->m_glb_curvT->Fill(logCurvature);
	
        hists->m_glb_xfT->Fill(firstPoint.x());
        hists->m_glb_yfT->Fill(firstPoint.y());
        hists->m_glb_zfT->Fill(firstPoint.z());
        hists->m_glb_radfT->Fill(radf);
        hists->m_glb_ratioT->Fill(nfitntot);
        hists->m_glb_ratioTTS->Fill(nfitntot,1.);
        hists->m_glb_ratiomT->Fill(nfitnmax);
        hists->m_glb_ratiomTTS->Fill(nfitnmax,1.);
        hists->m_psiT->Fill(psi);
        hists->m_psiTTS->Fill(psi,1.);
        hists->m_tanlT->Fill(TMath::Tan(geom->dipAngle()));
        hists->m_glb_thetaT->Fill(thetad);
        hists->m_etaT->Fill(eta);
        hists->m_etaTTS->Fill(eta,1.);
        hists->m_pTT->Fill(pT);
        hists->m_pTTTS->Fill(lmevpt,1.);
        hists->m_momT->Fill(gmom);
        hists->m_lengthT->Fill(globtrk->length());
        hists->m_chisq0T->Fill(chisq0);
        hists->m_chisq0TTS->Fill(chisq0,1.);
        hists->m_chisq1T->Fill(chisq1);
        hists->m_chisq1TTS->Fill(chisq1,1.);
	
	if (firstPoint.z()<0)
	  hists->m_globtrk_xf_yfTE->Fill(firstPoint.x(),
					 firstPoint.y());
	else
	  hists->m_globtrk_xf_yfTW->Fill(firstPoint.x(),
					 firstPoint.y());
        hists->m_eta_trklengthT->Fill(eta,globtrk->length());
        hists->m_npoint_lengthT->Fill(globtrk->length(),
				      Float_t(detInfo->numberOfPoints()));
        hists->m_fpoint_lengthT->Fill(globtrk->length(),
				      Float_t(fTraits.numberOfFitPoints()));
        hists->m_fpoint_lengthTTS->Fill(globtrk->length(),
					Float_t(fTraits.numberOfFitPoints()));
	
        hists->m_pT_eta_recT->Fill(eta,lmevpt);
	if (event->primaryVertex() && fabs(rcircle)>0.) {
          double qwe = ::pow(firstPoint.x()-pvert.x(),2)
	  + ::pow(firstPoint.y()-pvert.y(),2);
          qwe = ::sqrt(qwe)/(2*rcircle);
	  if (qwe>0.9999) qwe = 0.999;
	  Float_t denom = 2*rcircle*(::asin(qwe));
	  if (denom>0)hists->m_tanl_zfT->Fill((firstPoint.z()-pvert.z())/denom,
				  Float_t(TMath::Tan(geom->dipAngle())));
	}
        hists->m_mom_trklengthT->Fill(globtrk->length(),lmevmom);
        hists->m_chisq0_momT->Fill(lmevmom,chisq0);
        hists->m_chisq1_momT->Fill(lmevmom,chisq1);
        hists->m_chisq0_etaT->Fill(eta,chisq0);
        hists->m_chisq1_etaT->Fill(eta,chisq1);
        hists->m_chisq0_dipT->Fill(TMath::Tan(geom->dipAngle()),chisq0);
        hists->m_chisq1_dipT->Fill(TMath::Tan(geom->dipAngle()),chisq1);
        hists->m_chisq0_zfT->Fill(firstPoint.z(),chisq0);
        hists->m_chisq1_zfT->Fill(firstPoint.z(),chisq1);
	hists->m_chisq0_phiT->Fill(orphi,chisq0);
        hists->m_nfptonpt_momT->Fill(lmevmom,nfitntot);
        hists->m_nfptonpt_etaT->Fill(eta,nfitntot);
        hists->m_psi_phiT->Fill(orphi,psi);
      }
      
      // now fill all TPC+SVT histograms --------------------------------------------
      
      else if (map.trackTpcSvt()) {
	
	cnttrkgTS++;
	cnttrkgTTS++;
	
        hists->m_glb_sptsTS->Fill(map.numberOfHits(kSvtId));
	
        hists->m_glb_f0TS->Fill(dif.x(),0.);
        hists->m_glb_f0TS->Fill(dif.y(),1.);
        hists->m_glb_f0TS->Fill(dif.z(),2.);
	
        hists->m_glb_xf0TS->Fill(dif.x());
        hists->m_glb_yf0TS->Fill(dif.y());
        hists->m_glb_zf0TS->Fill(dif.z());
	hists->m_glb_rzf0TS->Fill(azimdif,0.);
        hists->m_glb_rzf0TS->Fill(dif.z(),1.);
	hists->m_glb_rzl0TS->Fill(azimdifl,0.);
        hists->m_glb_rzl0TS->Fill(difl.z(),1.);
        hists->m_glb_impactTS->Fill(logImpact,2.);
        hists->m_glb_simpactTS->Fill(sImpact,2.);
        if ((firstPoint.z() < 0) && (lastPoint.z() < 0)) { // east-only
          hists->m_glb_impactTS->Fill(logImpact,0.);
          hists->m_glb_simpactTS->Fill(sImpact,0.);
        }
        if ((firstPoint.z() > 0) && (lastPoint.z() > 0)) { // west-only
          hists->m_glb_impactTS->Fill(logImpact,1.);
          hists->m_glb_simpactTS->Fill(sImpact,1.);
        }
        hists->m_glb_impactrTS->Fill(globtrk->impactParameter());
        hists->m_glb_impactTTS->Fill(logImpact,0.);
        hists->m_glb_impactrTTS->Fill(globtrk->impactParameter(),0.);
	
	// SVT barrel histogram - causes segmentation violation currently
	//   => use m_glb_radfTS for now. -CPL
	//StSvtCoordinateTransform transformer;
	//StGlobalCoordinate globalHitPosition(firstPoint);
	//StSvtLocalCoordinate layerCoord;
	//transformer(globalHitPosition,layerCoord);
        //hists->m_glb_layerfTS->Fill(layerCoord.layer())
	
        hists->m_pointTS->Fill(detInfo->numberOfPoints());
        hists->m_max_pointTS->Fill(globtrk->numberOfPossiblePoints());
        hists->m_fit_pointTS->Fill(fTraits.numberOfFitPoints());
        hists->m_fit_pointTTS->Fill(fTraits.numberOfFitPoints(),0.);
        hists->m_glb_chargeTS->Fill(geom->charge());
	
        hists->m_glb_r0TS->Fill(origin.perp());
	hists->m_glb_phi0TS->Fill(orphi);
        hists->m_glb_z0TS->Fill(origin.z());
        hists->m_glb_curvTS->Fill(logCurvature);
	
        hists->m_glb_xfTS->Fill(firstPoint.x());
        hists->m_glb_yfTS->Fill(firstPoint.y());
        if (radf<40) {
	  hists->m_glb_zfTS->Fill(firstPoint.z());
	  if (firstPoint.phi() < 0)
	    hists->m_glb_phifTS->Fill(360+firstPoint.phi()/degree);
	  else
	    hists->m_glb_phifTS->Fill(firstPoint.phi()/degree);
	}
	
        hists->m_glb_radfTS->Fill(radf);
        hists->m_glb_ratioTS->Fill(nfitntot);
        hists->m_glb_ratioTTS->Fill(nfitntot,0.);
        hists->m_glb_ratiomTS->Fill(nfitnmax);
        hists->m_glb_ratiomTTS->Fill(nfitnmax,0.);
        hists->m_psiTS->Fill(psi);
        hists->m_psiTTS->Fill(psi,0.);
        hists->m_tanlTS->Fill(TMath::Tan(geom->dipAngle()));
        hists->m_glb_thetaTS->Fill(thetad);
        hists->m_etaTS->Fill(eta);
        hists->m_etaTTS->Fill(eta,0.);
        hists->m_pTTS->Fill(pT);
        hists->m_pTTTS->Fill(lmevpt,0.);
        hists->m_momTS->Fill(gmom);
        hists->m_lengthTS->Fill(globtrk->length());
        hists->m_chisq0TS->Fill(chisq0);
        hists->m_chisq0TTS->Fill(chisq0,0.);
        hists->m_chisq1TS->Fill(chisq1);
        hists->m_chisq1TTS->Fill(chisq1,0.);
        hists->m_globtrk_xf_yfTS->Fill(firstPoint.x(),
				       firstPoint.y());
        hists->m_eta_trklengthTS->Fill(eta,globtrk->length());
        hists->m_npoint_lengthTS->Fill(globtrk->length(),
				       Float_t(detInfo->numberOfPoints()));
        hists->m_fpoint_lengthTS->Fill(globtrk->length(),
				       Float_t(fTraits.numberOfFitPoints()));
        hists->m_fpoint_lengthTTS->Fill(globtrk->length(),
					Float_t(fTraits.numberOfFitPoints()));
	
        hists->m_pT_eta_recTS->Fill(eta,lmevpt);
	if (event->primaryVertex()) {
          double qwe = ::pow(firstPoint.x()-pvert.x(),2)
	  + ::pow(firstPoint.y()-pvert.y(),2);
          qwe = ::sqrt(qwe)/(2*rcircle);
	  if (qwe>0.9999) qwe = 0.999;
	  Float_t denom = 2*rcircle*(::asin(qwe));
	  if (radf>40 && denom>0) {
	    hists->m_tanl_zfT->Fill((firstPoint.z()-pvert.z())/denom,
				    Float_t(TMath::Tan(geom->dipAngle())));
	  }
	  if (radf<40 && denom>0) {
	    hists->m_tanl_zfTS->Fill((firstPoint.z()-pvert.z())/denom,
				     Float_t(TMath::Tan(geom->dipAngle())));
	  }
	}
        hists->m_mom_trklengthTS->Fill(globtrk->length(),lmevmom);
        hists->m_chisq0_momTS->Fill(lmevmom,chisq0);
        hists->m_chisq1_momTS->Fill(lmevmom,chisq1);
        hists->m_chisq0_etaTS->Fill(eta,chisq0);
        hists->m_chisq1_etaTS->Fill(eta,chisq1);
        hists->m_chisq0_dipTS->Fill(TMath::Tan(geom->dipAngle()),chisq0);
        hists->m_chisq1_dipTS->Fill(TMath::Tan(geom->dipAngle()),chisq1);
        hists->m_chisq0_zfTS->Fill(firstPoint.z(),chisq0);
        hists->m_chisq1_zfTS->Fill(firstPoint.z(),chisq1);
	hists->m_chisq0_phiTS->Fill(orphi,chisq0);
	
        hists->m_nfptonpt_momTS->Fill(lmevmom,nfitntot);
        hists->m_nfptonpt_etaTS->Fill(eta,nfitntot);
        hists->m_psi_phiTS->Fill(orphi,psi);
      }
      
      // now fill all FTPC East histograms ------------------------------------------
      else if (map.trackFtpcEast()) {
	
	cnttrkgFE++;
	
	// east and west in same histogram
        hists->m_pointF->Fill(detInfo->numberOfPoints(),0.);
        hists->m_max_pointF->Fill(globtrk->numberOfPossiblePoints(),0.);
        hists->m_glb_chargeF->Fill(geom->charge(),0.);
        hists->m_glb_xfF->Fill(firstPoint.x(),0.);
        hists->m_glb_yfF->Fill(firstPoint.y(),0.);
        hists->m_glb_zfF->Fill(firstPoint.z(),0.);
        hists->m_glb_radfF->Fill(radf,0.);
        hists->m_glb_ratiomF->Fill(nfitnmax,0.);
        hists->m_psiF->Fill(psi,0.);
        hists->m_etaF->Fill(fabs(eta),0.);
        hists->m_pTF->Fill(pT,0.);
        hists->m_momF->Fill(gmom,0.);
        hists->m_lengthF->Fill(globtrk->length(),0.);
        hists->m_chisq0F->Fill(chisq0,0.);
        hists->m_chisq1F->Fill(chisq1,0.);
        hists->m_glb_impactF->Fill(logImpact,0.);
        hists->m_glb_impactrF->Fill(globtrk->impactParameter(),0.);
	
	// FTPC plane histogram - there are no FTPC transformation utilities yet.
	//   => use m_glb_zfF for now.  -CPL
	//StFtpcCoordinateTransform transformer();
	//StGlobalCoordinate globalHitPosition(firstPoint);
	//StFtpcLocalCoordinate planeCoord;
	//transformer(globalHitPosition,planeCoord);
        //hists->m_glb_planefF->Fill(planeCoord.plane());  
	
	if (fabs(firstPoint.z()-ftpcPadrowZPos[10])<=1)
	  hists->m_glb_planefF->Fill(11,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[11])<=1)
	  hists->m_glb_planefF->Fill(12,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[12])<=1)
	  hists->m_glb_planefF->Fill(13,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[13])<=1)
	  hists->m_glb_planefF->Fill(14,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[14])<=1)
	  hists->m_glb_planefF->Fill(15,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[15])<=1)
	  hists->m_glb_planefF->Fill(16,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[16])<=1)
	  hists->m_glb_planefF->Fill(17,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[17])<=1)
	  hists->m_glb_planefF->Fill(18,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[18])<=1)
	  hists->m_glb_planefF->Fill(19,0.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[19])<=1)
	  hists->m_glb_planefF->Fill(20,0.);
	
	// east and west in separate histograms
        hists->m_pointFE->Fill(detInfo->numberOfPoints());
        hists->m_max_pointFE->Fill(globtrk->numberOfPossiblePoints());
        hists->m_glb_chargeFE->Fill(geom->charge());
        hists->m_glb_xfFE->Fill(firstPoint.x());
        hists->m_glb_yfFE->Fill(firstPoint.y());
        hists->m_glb_zfFE->Fill(firstPoint.z());
        hists->m_glb_radfFE->Fill(radf);
        hists->m_glb_ratiomFE->Fill(nfitnmax);
        hists->m_psiFE->Fill(psi);
        hists->m_etaFE->Fill(eta);
        hists->m_pTFE->Fill(pT);
        hists->m_momFE->Fill(gmom);
        hists->m_lengthFE->Fill(globtrk->length());
        hists->m_chisq0FE->Fill(chisq0);
        hists->m_chisq1FE->Fill(chisq1);
	
        hists->m_pT_eta_recFE->Fill(eta,lmevpt);
        hists->m_globtrk_xf_yfFE->Fill(firstPoint.x(),
				       firstPoint.y());
        hists->m_eta_trklengthFE->Fill(eta,globtrk->length());
        hists->m_npoint_lengthFE->Fill(globtrk->length(),
				       Float_t(detInfo->numberOfPoints()));
      }
      // now fill all FTPC West histograms ------------------------------------------
      else if (map.trackFtpcWest()) {
	
	cnttrkgFW++;
	
	// east and west in same histogram
        hists->m_pointF->Fill(detInfo->numberOfPoints(),1.);
        hists->m_max_pointF->Fill(globtrk->numberOfPossiblePoints(),1.);
        hists->m_glb_chargeF->Fill(geom->charge(),1.);
        hists->m_glb_xfF->Fill(firstPoint.x(),1.);
        hists->m_glb_yfF->Fill(firstPoint.y(),1.);
        hists->m_glb_zfF->Fill(firstPoint.z(),1.);
        hists->m_glb_radfF->Fill(radf,1.);
        hists->m_glb_ratiomF->Fill(nfitnmax,1.);
        hists->m_psiF->Fill(psi,1.);
        hists->m_etaF->Fill(fabs(eta),1.);
        hists->m_pTF->Fill(pT,1.);
        hists->m_momF->Fill(gmom,1.);
        hists->m_lengthF->Fill(globtrk->length(),1.);
        hists->m_chisq0F->Fill(chisq0,1.);
        hists->m_chisq1F->Fill(chisq1,1.);
        hists->m_glb_impactF->Fill(logImpact,1.);
        hists->m_glb_impactrF->Fill(globtrk->impactParameter(),1.);
	
	if (fabs(firstPoint.z()-ftpcPadrowZPos[0])<=1)
	  hists->m_glb_planefF->Fill(1,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[1])<=1)
	  hists->m_glb_planefF->Fill(2,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[2])<=1)
	  hists->m_glb_planefF->Fill(3,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[3])<=1)
	  hists->m_glb_planefF->Fill(4,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[4])<=1)
	  hists->m_glb_planefF->Fill(5,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[5])<=1)
	  hists->m_glb_planefF->Fill(6,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[6])<=1)
	  hists->m_glb_planefF->Fill(7,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[7])<=1)
	  hists->m_glb_planefF->Fill(8,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[8])<=1)
	  hists->m_glb_planefF->Fill(9,1.);
	if (fabs(firstPoint.z()-ftpcPadrowZPos[9])<=1)
	  hists->m_glb_planefF->Fill(10,1.);
	
	// east and west in separate histograms
        hists->m_pointFW->Fill(detInfo->numberOfPoints());
        hists->m_max_pointFW->Fill(globtrk->numberOfPossiblePoints());
        hists->m_glb_chargeFW->Fill(geom->charge());
        hists->m_glb_xfFW->Fill(firstPoint.x());
        hists->m_glb_yfFW->Fill(firstPoint.y());
        hists->m_glb_zfFW->Fill(firstPoint.z());
        hists->m_glb_radfFW->Fill(radf);
        hists->m_glb_ratiomFW->Fill(nfitnmax);
        hists->m_psiFW->Fill(psi);
        hists->m_etaFW->Fill(eta);
        hists->m_pTFW->Fill(pT);
        hists->m_momFW->Fill(gmom);
        hists->m_lengthFW->Fill(globtrk->length());
        hists->m_chisq0FW->Fill(chisq0);
        hists->m_chisq1FW->Fill(chisq1);
	
        hists->m_pT_eta_recFW->Fill(eta,lmevpt);
        hists->m_globtrk_xf_yfFW->Fill(firstPoint.x(),
				       firstPoint.y());
        hists->m_eta_trklengthFW->Fill(eta,globtrk->length());
        hists->m_npoint_lengthFW->Fill(globtrk->length(),
				       Float_t(detInfo->numberOfPoints()));
      }
    }
  }
  hists->m_globtrk_tot->Fill(cnttrk); 
  hists->m_globtrk_good->Fill(cnttrkg);
  hists->m_globtrk_good_sm->Fill(cnttrkg);
  hists->m_globtrk_goodTTS->Fill(cnttrkgTTS);
  hists->m_globtrk_goodF->Fill(cnttrkgFE,cnttrkgFW);
}

//_____________________________________________________________________________
/// histograms filled in MakeHistPID() method
void StEventQAMaker::MakeHistDE() {
  
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistPrim() {
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling primary track histograms ");
  
  Int_t cnttrk=0;
  Int_t cnttrkg=0;
  Int_t cnttrkgT=0;
  Int_t cnttrkgTS=0;
  Int_t cnttrkgFE=0;
  Int_t cnttrkgFW=0; 
  Int_t pTcnttrkgFE=0;
  Int_t pTcnttrkgFW=0; 
  Float_t mean_ptT=0;
  Float_t mean_ptTS=0;
  Float_t mean_ptFE=0;
  Float_t mean_ptFW=0;
  Float_t mean_etaT=0;
  Float_t mean_etaTS=0;
  Float_t mean_etaFE=0;
  Float_t mean_etaFW=0;
  
  StPrimaryVertex *primVtx = event->primaryVertex();
  UInt_t daughters=0;
  UInt_t currentNumber=0;
  StThreeVectorF pvert;
  if (primVtx) {
    pvert = primVtx->position();
    for (UInt_t v=0; v<event->numberOfPrimaryVertices(); v++) {
      currentNumber = event->primaryVertex(v)->numberOfDaughters();
      if (currentNumber > daughters) {
	daughters = currentNumber;
	primVtx = event->primaryVertex(v);
      }
    }
  }
  
  if (primVtx) {
    cnttrk = primVtx->numberOfDaughters();
    hists->m_primtrk_tot->Fill(cnttrk);
    hists->m_primtrk_tot_sm->Fill(cnttrk);

    for (UInt_t i=0; i<primVtx->numberOfDaughters(); i++) {
      StTrack *primtrk = primVtx->daughter(i);
      if (!primtrk) continue;

      // May 11, 2005: currently, primVtx->daughter(i) returns only
      // track from "primary" collection. Must see if there is also
      // an "estPrimary" version of track.
      // T. Ullrich says "every estPrimary has a primary", so asking
      // for estPrimary from node of primary should work.
      StTrack* estprimtrk = primtrk->node()->track(estPrimary);
      if (estprimtrk) primtrk = estprimtrk;
      if (primtrk->bad()) continue;
      hists->m_primtrk_iflag->Fill(primtrk->flag());
      
      if (primtrk->flag()>0) {
        StTrackGeometry* geom = primtrk->geometry();
	// due to variation on "kalman fitting" of primary tracks
	// we want to look at the hit residuals using the outerGeometry()
	// helix parameters (parameters at last point on track)
	StTrackGeometry* outerGeom = primtrk->outerGeometry();
        StTrackFitTraits& fTraits = primtrk->fitTraits();
        StTrackDetectorInfo* detInfo = primtrk->detectorInfo();
        const StTrackTopologyMap& map=primtrk->topologyMap();
	StPhysicalHelixD hx = geom->helix();
	StPhysicalHelixD ohx = outerGeom->helix();
	
	StTrack *gtrack = primtrk->node()->track(estGlobal);
        if (!gtrack || gtrack->bad()) {
          gtrack = primtrk->node()->track(global);
          if (!gtrack || gtrack->bad()) continue;
        }
	StTrackFitTraits& gfTraits = gtrack->fitTraits();
	Int_t nhit_prim_fit = fTraits.numberOfFitPoints();
	Int_t nhit_glob_fit = gfTraits.numberOfFitPoints();
	hists->m_primglob_fit->Fill((Float_t)nhit_prim_fit/((Float_t)nhit_glob_fit+1.e-10));
	
	n_prim_good++;
        cnttrkg++;
	Float_t pT = -999.;
	pT = geom->momentum().perp();
        Float_t lmevpt = TMath::Log10(1e-30+pT*1000.0);
	Float_t theta = TMath::ASin(1.) - geom->dipAngle();
	Float_t thetad = theta/degree;
	Float_t eta   = geom->momentum().pseudoRapidity();
	Float_t gmom = abs(geom->momentum());
        Float_t lmevmom = TMath::Log10(1e-30+gmom*1000.0); 
	Float_t chisq0 = fTraits.chi2(0);
	Float_t chisq1 = fTraits.chi2(1);
	Float_t nfitnmax = (Float_t(fTraits.numberOfFitPoints())) /
	  (Float_t(primtrk->numberOfPossiblePoints())+1.e-10);
        Float_t nfitntot = (Float_t(fTraits.numberOfFitPoints()))/
	  (Float_t(detInfo->numberOfPoints())+1.e-10);
	Float_t logCurvature = TMath::Log10(1e-30+geom->curvature());
	
        const StThreeVectorF& firstPoint = detInfo->firstPoint();
	const StThreeVectorF& lastPoint = detInfo->lastPoint();
	const StThreeVectorF& origin = geom->origin();
	Float_t psi = geom->psi()/degree;
	if (psi<0) psi+=360;
	Float_t orphi = origin.phi()/degree;
	if (orphi<0) orphi+=360;
	//const StThreeVectorF& outerOrigin = outerGeom->origin();
	
	// need to find position on helix closest to first point on track since
	// the primary vertex is used as the first point on helix for primary
	// tracks -CPL
	double sFirst = hx.pathLength(firstPoint);
	// get the helix position closest to the last point on track
	double sLast = hx.pathLength(lastPoint);
	
	StThreeVectorF dif = firstPoint - hx.at(sFirst);
	StThreeVectorF difl = lastPoint - hx.at(sLast);
	
	Float_t xcenter = hx.xcenter();
	Float_t ycenter = hx.ycenter();
	Float_t rcircle = 1./hx.curvature();
	Float_t centerOfCircleToFP = ::sqrt(::pow(xcenter-firstPoint.x(),2) +
					    ::pow(ycenter-firstPoint.y(),2));
	Float_t centerOfCircleToLP = ::sqrt(::pow(xcenter-lastPoint.x(),2) +
					    ::pow(ycenter-lastPoint.y(),2));
	Float_t azimdif = dif.perp();
	if (rcircle<centerOfCircleToFP) azimdif *= -1.;
	Float_t azimdifl = difl.perp();
	if (rcircle<centerOfCircleToLP) azimdifl *= -1.;
	
	// get the same information as above but from the outerGeometry()
	// ... this is so we can look at the hit residuals using the helix
	// parameters at the last point on a track
	double sFirstOuter = ohx.pathLength(firstPoint);
	double sLastOuter = ohx.pathLength(lastPoint);
	StThreeVectorF outerDif = firstPoint - ohx.at(sFirstOuter);
	StThreeVectorF outerDifl = lastPoint - ohx.at(sLastOuter);
	Float_t outerXcenter = ohx.xcenter();
	Float_t outerYcenter = ohx.ycenter();
	Float_t outerRcircle = 1./(ohx.curvature()+1.e-10);
	Float_t outerCenterOfCircleToFP = ::sqrt(::pow(outerXcenter-firstPoint.x(),2) +
						 ::pow(outerYcenter-firstPoint.y(),2));
	Float_t outerCenterOfCircleToLP = ::sqrt(::pow(outerXcenter-lastPoint.x(),2) +
						 ::pow(outerYcenter-lastPoint.y(),2));
	Float_t outerAzimdif = outerDif.perp();
	if (outerRcircle<outerCenterOfCircleToFP) outerAzimdif *= -1.;
	Float_t outerAzimdifl = outerDifl.perp();
	if (outerRcircle<outerCenterOfCircleToLP) outerAzimdifl *= -1.;
	
        Float_t radf = firstPoint.perp();
	
	// check if the track has hits in a detector -CPL
	if (map.hasHitInDetector(kUnknownId)) hists->m_pdet_id->Fill(kUnknownId);
	if (map.hasHitInDetector(kTpcId)) hists->m_pdet_id->Fill(kTpcId);
	if (map.hasHitInDetector(kSvtId)) hists->m_pdet_id->Fill(kSvtId);
	if (map.hasHitInDetector(kRichId)) hists->m_pdet_id->Fill(kRichId);
	if (map.hasHitInDetector(kFtpcWestId)) hists->m_pdet_id->Fill(kFtpcWestId);
	if (map.hasHitInDetector(kFtpcEastId)) hists->m_pdet_id->Fill(kFtpcEastId);
	if (map.hasHitInDetector(kTofId)) hists->m_pdet_id->Fill(kTofId);
	if (map.hasHitInDetector(kCtbId)) hists->m_pdet_id->Fill(kCtbId);
	if (map.hasHitInDetector(kSsdId)) hists->m_pdet_id->Fill(kSsdId);
	if (map.hasHitInDetector(kBarrelEmcTowerId)) hists->m_pdet_id->Fill(kBarrelEmcTowerId);
	if (map.hasHitInDetector(kBarrelEmcPreShowerId)) hists->m_pdet_id->Fill(kBarrelEmcPreShowerId);
	if (map.hasHitInDetector(kBarrelSmdEtaStripId)) hists->m_pdet_id->Fill(kBarrelSmdEtaStripId);
	if (map.hasHitInDetector(kBarrelSmdPhiStripId)) hists->m_pdet_id->Fill(kBarrelSmdPhiStripId);
	if (map.hasHitInDetector(kEndcapEmcTowerId)) hists->m_pdet_id->Fill(kEndcapEmcTowerId);
	if (map.hasHitInDetector(kEndcapEmcPreShowerId)) hists->m_pdet_id->Fill(kEndcapEmcPreShowerId);
	if (map.hasHitInDetector(kEndcapSmdUStripId)) hists->m_pdet_id->Fill(kEndcapSmdUStripId);
	if (map.hasHitInDetector(kEndcapSmdVStripId)) hists->m_pdet_id->Fill(kEndcapSmdVStripId);
	if (map.hasHitInDetector(kZdcWestId)) hists->m_pdet_id->Fill(kZdcWestId);
	if (map.hasHitInDetector(kZdcEastId)) hists->m_pdet_id->Fill(kZdcEastId);
	if (map.hasHitInDetector(kMwpcWestId)) hists->m_pdet_id->Fill(kMwpcWestId);
	if (map.hasHitInDetector(kMwpcEastId)) hists->m_pdet_id->Fill(kMwpcEastId);
	if (map.hasHitInDetector(kTpcSsdId)) hists->m_pdet_id->Fill(kTpcSsdId);
	if (map.hasHitInDetector(kTpcSvtId)) hists->m_pdet_id->Fill(kTpcSvtId);
	if (map.hasHitInDetector(kTpcSsdSvtId)) hists->m_pdet_id->Fill(kTpcSsdSvtId);
	if (map.hasHitInDetector(kSsdSvtId)) hists->m_pdet_id->Fill(kSsdSvtId);
	
	// now fill all TPC histograms ------------------------------------------------
        if (map.trackTpcOnly()) {
	  
	  cnttrkgT++;
	  mean_ptT += geom->momentum().perp();
	  mean_etaT += eta;
	  // these are TPC only
	  hists->m_prim_f0->Fill(outerDif.x(),0.);
	  hists->m_prim_f0->Fill(outerDif.y(),1.);
	  hists->m_prim_f0->Fill(outerDif.z(),2.);
	  
	  hists->m_prim_xf0->Fill(outerDif.x());
	  hists->m_prim_yf0->Fill(outerDif.y());
	  hists->m_prim_zf0->Fill(outerDif.z());
	  hists->m_prim_rzf0->Fill(outerAzimdif,0.);
	  hists->m_prim_rzf0->Fill(outerDif.z(),1.);
	  hists->m_prim_rzl0->Fill(outerAzimdifl,0.);
	  hists->m_prim_rzl0->Fill(outerDifl.z(),1.);
	  
	  // TPC gains histograms
	  if (event->summary()) {
	    mHitHist->clear();
	    mHitHist->setTrack(primtrk);
	    mHitHist->setBField(event->summary()->magneticField());
	    mHitHist->findHits();
	    mHitHist->fillHistograms();
	  }
	  
	  // these are TPC & FTPC
	  hists->m_ppointT->Fill(detInfo->numberOfPoints());
	  hists->m_pmax_pointT->Fill(primtrk->numberOfPossiblePoints());
	  hists->m_pfit_pointT->Fill(fTraits.numberOfFitPoints());
	  hists->m_prim_chargeT->Fill(geom->charge());
	  
	  hists->m_prim_r0T->Fill(origin.perp());
	  hists->m_prim_phi0T->Fill(orphi);
	  hists->m_prim_z0T->Fill(origin.z());
	  hists->m_prim_curvT->Fill(logCurvature);
	  
	  hists->m_prim_xfT->Fill(firstPoint.x());
	  hists->m_prim_yfT->Fill(firstPoint.y());
	  hists->m_prim_zfT->Fill(firstPoint.z());
	  hists->m_prim_radfT->Fill(radf);
	  hists->m_prim_ratioT->Fill(nfitntot);
	  hists->m_prim_ratiomT->Fill(nfitnmax);
	  hists->m_ppsiT->Fill(psi);
	  hists->m_ppsiTTS->Fill(psi,1.);
	  hists->m_ptanlT->Fill(TMath::Tan(geom->dipAngle()));
	  hists->m_prim_thetaT->Fill(thetad);
	  hists->m_petaT->Fill(eta);
	  hists->m_petaTTS->Fill(eta,1.);
	  hists->m_ppTT->Fill(pT);
	  hists->m_ppTTTS->Fill(pT,1.);
	  hists->m_pmomT->Fill(gmom);
	  hists->m_plengthT->Fill(primtrk->length());
	  hists->m_pchisq0T->Fill(chisq0);
	  hists->m_pchisq1T->Fill(chisq1);
	  hists->m_pchisq0TTS->Fill(chisq0,1.);
	  hists->m_pchisq1TTS->Fill(chisq1,1.);
	  
	  // these are for TPC & FTPC
	  if (firstPoint.z()<0)
	    hists->m_primtrk_xf_yfTE->Fill(firstPoint.x(),
					   firstPoint.y());
	  else
	    hists->m_primtrk_xf_yfTW->Fill(firstPoint.x(),
					   firstPoint.y());
	  hists->m_peta_trklengthT->Fill(eta,primtrk->length());
	  hists->m_pnpoint_lengthT->Fill(primtrk->length(),
					 Float_t(detInfo->numberOfPoints()));
	  hists->m_pfpoint_lengthT->Fill(primtrk->length(),
					 Float_t(fTraits.numberOfFitPoints()));
	  hists->m_pfpoint_lengthTTS->Fill(primtrk->length(),
					   Float_t(fTraits.numberOfFitPoints()));
	  
	  // these are TPC only
	  hists->m_ppT_eta_recT->Fill(eta,lmevpt);
          double qwe = ::pow(firstPoint.x()-pvert.x(),2)
	    + ::pow(firstPoint.y()-pvert.y(),2);
          qwe = ::sqrt(qwe)/(2*rcircle);
	  if (qwe>0.9999) qwe = 0.999;
	  Float_t denom = 2*rcircle*(::asin(qwe));
	  if (denom>0)hists->m_ptanl_zfT->Fill((firstPoint.z()-pvert.z())/denom,
				   Float_t(TMath::Tan(geom->dipAngle())));
	  hists->m_pmom_trklengthT->Fill(primtrk->length(),lmevmom);
	  hists->m_pchisq0_momT->Fill(lmevmom,chisq0);
	  hists->m_pchisq1_momT->Fill(lmevmom,chisq1);
	  hists->m_pchisq0_etaT->Fill(eta,chisq0);
	  hists->m_pchisq1_etaT->Fill(eta,chisq1);
	  hists->m_pchisq0_dipT->Fill(TMath::Tan(geom->dipAngle()),chisq0);
	  hists->m_pchisq1_dipT->Fill(TMath::Tan(geom->dipAngle()),chisq1);
	  hists->m_pchisq0_zfT->Fill(firstPoint.z(),chisq0);
	  hists->m_pchisq1_zfT->Fill(firstPoint.z(),chisq1);
	  hists->m_pnfptonpt_momT->Fill(lmevmom,nfitntot);
	  hists->m_pnfptonpt_etaT->Fill(eta,nfitntot);
	  hists->m_ppsi_phiT->Fill(orphi,psi);
	}
	
	// now fill all TPC+SVT histograms --------------------------------------------
	
        else if (map.trackTpcSvt()) {
	  
	  cnttrkgTS++;
	  mean_ptTS += geom->momentum().perp();
	  mean_etaTS += eta;
	  
	  hists->m_prim_f0TS->Fill(outerDif.x(),0.);
	  hists->m_prim_f0TS->Fill(outerDif.y(),1.);
	  hists->m_prim_f0TS->Fill(outerDif.z(),2.);
	  
	  hists->m_prim_xf0TS->Fill(outerDif.x());
	  hists->m_prim_yf0TS->Fill(outerDif.y());
	  hists->m_prim_zf0TS->Fill(outerDif.z());
	  hists->m_prim_rzf0TS->Fill(outerAzimdif,0.);
	  hists->m_prim_rzf0TS->Fill(outerDif.z(),1.);
	  hists->m_prim_rzl0TS->Fill(outerAzimdifl,0.);
	  hists->m_prim_rzl0TS->Fill(outerDifl.z(),1.);
	  
	  hists->m_ppointTS->Fill(detInfo->numberOfPoints());
	  hists->m_pmax_pointTS->Fill(primtrk->numberOfPossiblePoints());
	  hists->m_pfit_pointTS->Fill(fTraits.numberOfFitPoints());
	  hists->m_prim_chargeTS->Fill(geom->charge());
	  
	  hists->m_prim_r0TS->Fill(origin.perp());
	  hists->m_prim_phi0TS->Fill(orphi);
	  hists->m_prim_z0TS->Fill(origin.z());
	  hists->m_prim_curvTS->Fill(logCurvature);
	  
	  hists->m_prim_xfTS->Fill(firstPoint.x());
	  hists->m_prim_yfTS->Fill(firstPoint.y());
	  hists->m_prim_zfTS->Fill(firstPoint.z());
	  hists->m_prim_radfTS->Fill(radf);
	  hists->m_prim_ratioTS->Fill(nfitntot);
	  hists->m_prim_ratiomTS->Fill(nfitnmax);
	  hists->m_ppsiTS->Fill(psi);
	  hists->m_ppsiTTS->Fill(psi,0.);
	  hists->m_ptanlTS->Fill(TMath::Tan(geom->dipAngle()));
	  hists->m_prim_thetaTS->Fill(thetad);
	  hists->m_petaTS->Fill(eta);
	  hists->m_petaTTS->Fill(eta,0.);
	  hists->m_ppTTS->Fill(pT);
	  hists->m_ppTTTS->Fill(pT,0.);
	  hists->m_pmomTS->Fill(gmom);
	  hists->m_plengthTS->Fill(primtrk->length());
	  hists->m_pchisq0TS->Fill(chisq0);
	  hists->m_pchisq1TS->Fill(chisq1);
	  hists->m_pchisq0TTS->Fill(chisq0,0.);
	  hists->m_pchisq1TTS->Fill(chisq1,0.);
	  
	  hists->m_primtrk_xf_yfTS->Fill(firstPoint.x(),
					 firstPoint.y());
	  hists->m_peta_trklengthTS->Fill(eta,primtrk->length());
	  hists->m_pnpoint_lengthTS->Fill(primtrk->length(),
					  Float_t(detInfo->numberOfPoints()));
	  hists->m_pfpoint_lengthTS->Fill(primtrk->length(),
					  Float_t(fTraits.numberOfFitPoints()));
	  hists->m_pfpoint_lengthTTS->Fill(primtrk->length(),
					   Float_t(fTraits.numberOfFitPoints()));
	  
	  hists->m_ppT_eta_recTS->Fill(eta,lmevpt);
          double qwe = ::pow(firstPoint.x()-pvert.x(),2)
	    + ::pow(firstPoint.y()-pvert.y(),2);
          qwe = ::sqrt(qwe)/(2*rcircle);
	  if (qwe>0.9999) qwe = 0.999;
	  Float_t denom = 2*rcircle*(::asin(qwe));
	  if (radf>40 && denom>0) hists->m_ptanl_zfT->
	    Fill((firstPoint.z() - pvert.z())/denom,
		 Float_t(TMath::Tan(geom->dipAngle())));
	  if (radf<40 && denom>0) hists->m_ptanl_zfTS->
	    Fill((firstPoint.z() - pvert.z())/denom,
		 Float_t(TMath::Tan(geom->dipAngle())));
	  hists->m_pmom_trklengthTS->Fill(primtrk->length(),lmevmom);
	  hists->m_pchisq0_momTS->Fill(lmevmom,chisq0);
	  hists->m_pchisq1_momTS->Fill(lmevmom,chisq1);
	  hists->m_pchisq0_etaTS->Fill(eta,chisq0);
	  hists->m_pchisq1_etaTS->Fill(eta,chisq1);
	  hists->m_pchisq0_dipTS->Fill(TMath::Tan(geom->dipAngle()),chisq0);
	  hists->m_pchisq1_dipTS->Fill(TMath::Tan(geom->dipAngle()),chisq1);
	  hists->m_pchisq0_zfTS->Fill(firstPoint.z(),chisq0);
	  hists->m_pchisq1_zfTS->Fill(firstPoint.z(),chisq1);
	  hists->m_pnfptonpt_momTS->Fill(lmevmom,nfitntot);
	  hists->m_pnfptonpt_etaTS->Fill(eta,nfitntot);
	  hists->m_ppsi_phiTS->Fill(orphi,psi);
	}
	
	// now fill all FTPC East histograms ------------------------------------------
        else if (map.trackFtpcEast()) {
	  
          if ( pT<2 ) {
	    mean_ptFE += geom->momentum().perp();
	    pTcnttrkgFE++;
          }
	  cnttrkgFE++;
	  mean_etaFE += eta;
	  // these are TPC & FTPC
	  // east and west in same histogram
	  hists->m_ppointF->Fill(detInfo->numberOfPoints(),0.);
	  hists->m_pmax_pointF->Fill(primtrk->numberOfPossiblePoints(),0.);
	  hists->m_prim_chargeF->Fill(geom->charge(),0.);
	  hists->m_prim_xfF->Fill(firstPoint.x(),0.);
	  hists->m_prim_yfF->Fill(firstPoint.y(),0.);
	  hists->m_prim_zfF->Fill(firstPoint.z(),0.);
	  hists->m_prim_radfF->Fill(radf,0.);
	  hists->m_prim_ratiomF->Fill(nfitnmax,0.);
	  hists->m_ppsiF->Fill(psi,0.);
	  hists->m_petaF->Fill(fabs(eta),0.);
	  hists->m_ppTF->Fill(pT,0.);
	  hists->m_pmomF->Fill(gmom,0.);
	  hists->m_plengthF->Fill(primtrk->length(),0.);
	  hists->m_pchisq0F->Fill(chisq0,0.);	
          hists->m_pchisq1F->Fill(chisq1,0.);
	  // east and west in separate histograms
	  hists->m_ppointFE->Fill(detInfo->numberOfPoints());
	  hists->m_pmax_pointFE->Fill(primtrk->numberOfPossiblePoints());
	  hists->m_prim_chargeFE->Fill(geom->charge());
	  hists->m_prim_xfFE->Fill(firstPoint.x());
	  hists->m_prim_yfFE->Fill(firstPoint.y());
	  hists->m_prim_zfFE->Fill(firstPoint.z());
	  hists->m_prim_radfFE->Fill(radf);
	  hists->m_prim_ratiomFE->Fill(nfitnmax);
	  hists->m_ppsiFE->Fill(psi);
	  hists->m_petaFE->Fill(eta);
	  hists->m_ppTFE->Fill(pT);
	  hists->m_pmomFE->Fill(gmom);
	  hists->m_plengthFE->Fill(primtrk->length());
	  hists->m_pchisq0FE->Fill(chisq0);
          hists->m_pchisq1FE->Fill(chisq1);
	  
	  // these are for TPC & FTPC
	  hists->m_ppT_eta_recFE->Fill(eta,lmevpt);
	  hists->m_primtrk_xf_yfFE->Fill(firstPoint.x(),
					 firstPoint.y());
	  hists->m_peta_trklengthFE->Fill(eta,primtrk->length());
	  hists->m_pnpoint_lengthFE->Fill(primtrk->length(),
					  Float_t(detInfo->numberOfPoints()));
	}
	
	// now fill all FTPC West histograms ------------------------------------------
        else if (map.trackFtpcWest()) {
	  
          if ( pT<2 ) {
	    mean_ptFW += geom->momentum().perp();
	    pTcnttrkgFW++;
          }
	  cnttrkgFW++;
	  mean_etaFW += eta;
	  // these are TPC & FTPC
	  // east and west in same histogram
	  hists->m_ppointF->Fill(detInfo->numberOfPoints(),1.);
	  hists->m_pmax_pointF->Fill(primtrk->numberOfPossiblePoints(),1.);
	  hists->m_prim_chargeF->Fill(geom->charge(),1.);
	  hists->m_prim_xfF->Fill(firstPoint.x(),1.);
	  hists->m_prim_yfF->Fill(firstPoint.y(),1.);
	  hists->m_prim_zfF->Fill(firstPoint.z(),1.);
	  hists->m_prim_radfF->Fill(radf,1.);
	  hists->m_prim_ratiomF->Fill(nfitnmax,1.);
	  hists->m_ppsiF->Fill(psi,1.);
	  hists->m_petaF->Fill(fabs(eta),1.);
	  hists->m_ppTF->Fill(pT,1.);
	  hists->m_pmomF->Fill(gmom,1.);
	  hists->m_plengthF->Fill(primtrk->length(),1.);
	  hists->m_pchisq0F->Fill(chisq0,1.);
          hists->m_pchisq1F->Fill(chisq1,1.);
	  // east and west in separate histograms
	  hists->m_ppointFW->Fill(detInfo->numberOfPoints());
	  hists->m_pmax_pointFW->Fill(primtrk->numberOfPossiblePoints());
	  hists->m_prim_chargeFW->Fill(geom->charge());
	  hists->m_prim_xfFW->Fill(firstPoint.x());
	  hists->m_prim_yfFW->Fill(firstPoint.y());
	  hists->m_prim_zfFW->Fill(firstPoint.z());
	  hists->m_prim_radfFW->Fill(radf);
	  hists->m_prim_ratiomFW->Fill(nfitnmax);
	  hists->m_ppsiFW->Fill(psi);
	  hists->m_petaFW->Fill(eta);
	  hists->m_ppTFW->Fill(pT);
	  hists->m_pmomFW->Fill(gmom);
	  hists->m_plengthFW->Fill(primtrk->length());
	  hists->m_pchisq0FW->Fill(chisq0);
          hists->m_pchisq1FW->Fill(chisq1);
	  
	  // these are for TPC & FTPC
	  hists->m_ppT_eta_recFW->Fill(eta,lmevpt);
	  hists->m_primtrk_xf_yfFW->Fill(firstPoint.x(),
					 firstPoint.y());
	  hists->m_peta_trklengthFW->Fill(eta,primtrk->length());
	  hists->m_pnpoint_lengthFW->Fill(primtrk->length(),
					  Float_t(detInfo->numberOfPoints()));
	}
      }
    }
    hists->m_primtrk_good->Fill(cnttrkg);
    hists->m_primtrk_good_sm->Fill(cnttrkg);
    hists->m_primtrk_goodTTS->Fill(cnttrkgT+cnttrkgTS);
    hists->m_primtrk_goodF->Fill(cnttrkgFE,cnttrkgFW);
  }
  mean_ptT   /= (cnttrkgT   +1.e-10);
  mean_ptTS  /= (cnttrkgTS  +1.e-10);
  mean_ptFE  /= (pTcnttrkgFE+1.e-10);
  mean_ptFW  /= (pTcnttrkgFW+1.e-10);
  mean_etaT  /= (cnttrkgT   +1.e-10);
  mean_etaTS /= (cnttrkgTS  +1.e-10);
  mean_etaFE /= (cnttrkgFE  +1.e-10);
  mean_etaFW /= (cnttrkgFW  +1.e-10);
  hists->m_primtrk_meanptTTS->Fill(mean_ptTS,0.);
  hists->m_primtrk_meanptTTS->Fill(mean_ptT,1.);
  hists->m_primtrk_meanptF->Fill(mean_ptFE,0.);
  hists->m_primtrk_meanptF->Fill(mean_ptFW,1.);
  hists->m_primtrk_meanetaTTS->Fill(mean_etaTS,0.);
  hists->m_primtrk_meanetaTTS->Fill(mean_etaT,1.);
  hists->m_primtrk_meanetaF->Fill(fabs(mean_etaFE),0.);
  hists->m_primtrk_meanetaF->Fill(fabs(mean_etaFW),1.);
  
  // MakeHistPrim() must be called after MakeHistGlob for the following to work
  hists->m_primglob_good->Fill((Float_t)n_prim_good/((Float_t)n_glob_good+1.e-10));
}


//_____________________________________________________________________________
void StEventQAMaker::MakeHistPID() {
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling dE/dx histograms ");
  
  StSPtrVecTrackNode &theNodes = event->trackNodes();
  Int_t cntrows=0;
  for (UInt_t i=0; i<theNodes.size(); i++) {
    StTrack *theTrack = theNodes[i]->track(global);
    if (!theTrack) continue;
    cntrows++;
    StSPtrVecTrackPidTraits &trkPidTr = theTrack->pidTraits();
    StDedxPidTraits *dedxPidTr;
    
    for (unsigned int itrait=0; itrait<trkPidTr.size();itrait++) {
      dedxPidTr = 0;
      StTrackPidTraits *thisTrait = trkPidTr[itrait];
      dedxPidTr = dynamic_cast<StDedxPidTraits*>(thisTrait);
      
      if (dedxPidTr && dedxPidTr->method() == kTruncatedMeanId) {
	int ndedx = dedxPidTr->numberOfPoints();
	double dedx = dedxPidTr->mean();
	double error = dedxPidTr->errorOnMean();
	double p = abs(theTrack->geometry()->momentum());
	double trackLength = dedxPidTr->length();
	if (dedxPidTr->detector() == kTpcId) {
	  // using BetheBloch::Sirrf method for curve normalized to 2.4 keV/cm
	  Float_t pionExpectedBB = BetheBloch::Sirrf(theTrack->geometry()->momentum().mag()/
						     pion_minus_mass_c2,trackLength);
	  pionExpectedBB *= 1.e-6;
	  hists->m_dedxTTS->Fill(dedx/(pionExpectedBB+1.e-10));
	  hists->m_ndedxT->Fill(ndedx);
	  hists->m_dedx0T->Fill(dedx);
	  hists->m_dedx1T->Fill(error);
	  if (ndedx > 15) {
	    hists->m_p_dedx_rec->Fill((float)(p),(float)(dedx*1.e6));
	  }
	}
	if (dedxPidTr->detector() == kTpcSvtId) {
	  // using BetheBloch::Sirrf method for curve normalized to 2.4 keV/cm
	  Float_t pionExpectedBB = BetheBloch::Sirrf(theTrack->geometry()->momentum().mag()/
						     pion_minus_mass_c2,trackLength);
	  pionExpectedBB *= 1.e-6;
	  hists->m_dedxTTS->Fill(dedx/(pionExpectedBB+1.e-10));
	}
	if (dedxPidTr->detector() == kFtpcWestId) {
	  // east and west in same histogram
	  hists->m_ndedxF->Fill(ndedx,1.);
	  hists->m_dedx0F->Fill(dedx,1.);
	  // east and west in separate histograms
	  hists->m_ndedxFW->Fill(ndedx);
	  hists->m_dedx0FW->Fill(dedx);
	}
	if (dedxPidTr->detector() == kFtpcEastId) {
	  // east and west in same histogram
	  hists->m_ndedxF->Fill(ndedx,0.);
	  hists->m_dedx0F->Fill(dedx,0.);
	  // east and west in separate histograms
	  hists->m_ndedxFE->Fill(ndedx);
	  hists->m_dedx0FE->Fill(dedx);
	}
      }
    }
  }
  hists->m_ndedxr->Fill(cntrows);
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistVertex() {
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling vertex histograms ");
  
  Float_t m_prmass2 = (proton_mass_c2*proton_mass_c2);
  Float_t m_pimass2 = (pion_minus_mass_c2*pion_minus_mass_c2);
  Float_t m_lamass2 = (lambda_mass_c2*lambda_mass_c2);
  
  // primary vertex
  StPrimaryVertex *primVtx = event->primaryVertex();
  UInt_t daughters=0;
  UInt_t currentNumber=0;
  StThreeVectorF pvert;
  
  if (primVtx) {
    
    pvert = primVtx->position();
    // Decide true primary vertex by most daughters
    for (UInt_t v=0; v<event->numberOfPrimaryVertices(); v++) {
      currentNumber = event->primaryVertex(v)->numberOfDaughters();
      if (currentNumber > daughters) {
	daughters = currentNumber;
	primVtx = event->primaryVertex(v);
      }
    }
    
    for (UInt_t j=0; j<event->numberOfPrimaryVertices(); j++) {
      StPrimaryVertex *aPrimVtx = event->primaryVertex(j);
      
      if (aPrimVtx == primVtx) {
        hists->m_pv_vtxid->Fill(primVtx->type());
	if (!isnan(double(pvert.x())))
	  hists->m_pv_x->Fill(pvert.x());
	if (!isnan(double(pvert.y())))
	  hists->m_pv_y->Fill(pvert.y());
	if (!isnan(double(pvert.z())))
	  hists->m_pv_z->Fill(pvert.z());
        hists->m_pv_pchi2->Fill(primVtx->chiSquared());
        hists->m_pv_r->Fill(pvert.x()*pvert.x() +
			    pvert.y()*pvert.y());
	hists->m_pv_xy->Fill(pvert.x(),pvert.y());
      } else {
        StThreeVectorF apvert = aPrimVtx->position();
        hists->m_v_vtxid->Fill(aPrimVtx->type());
	if (!isnan(double(apvert.x())))
	  hists->m_v_x->Fill(apvert.x());     
	if (!isnan(double(apvert.y())))
	  hists->m_v_y->Fill(apvert.y());     
	if (!isnan(double(apvert.z())))
	  hists->m_v_z->Fill(apvert.z());     
        hists->m_v_pchi2->Fill(aPrimVtx->chiSquared());
        hists->m_v_r->Fill(apvert.x()*apvert.x() +
			   apvert.y()*apvert.y());
      }
    }
  }
  
  float z_svt = 999.;
  float z_tpc = -999.;
  for (UInt_t j=0; j<event->numberOfCalibrationVertices(); j++) {
    StCalibrationVertex *aCalibVtx = event->calibrationVertex(j);
    if (aCalibVtx->flag() == 101) z_tpc = aCalibVtx->position().z();
    if (aCalibVtx->flag() == 201) z_svt = aCalibVtx->position().z();
    if (aCalibVtx->type() == 6 ) {
      hists->m_vtx_FtpcEastTpc_xy->Fill(aCalibVtx->position().x() - pvert.x(),
					aCalibVtx->position().y() - pvert.y());
      hists->m_vtx_FtpcEastTpc_z->Fill(aCalibVtx->position().z() - pvert.z());
    }
    if (aCalibVtx->type() == 7 ) {
      hists->m_vtx_FtpcWestTpc_xy->Fill(aCalibVtx->position().x() - pvert.x(),
					aCalibVtx->position().y() - pvert.y());
      hists->m_vtx_FtpcWestTpc_z->Fill(aCalibVtx->position().z() - pvert.z());
    }
  }
  if (z_svt != 999. && z_tpc != -999.) {
    hists->m_vtx_z->Fill(z_tpc-z_svt);
    hists->m_pv_SvtvsTpc->Fill(z_svt,z_tpc);
  }
  
  // V0 vertices
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling dst_v0_vertex histograms ");
  
  StSPtrVecV0Vertex &v0Vtx = event->v0Vertices();
  hists->m_v0->Fill(v0Vtx.size());
  
  //  static TH1F v0PhiHist("voph","v0 Phi Hist",36,0.,360.);
  //  static TH1F v0PhiHist2("voph2","v0 Phi Hist2",36,180.,540.);
  //  static TSpectrum v0PhiSpec;
  //  v0PhiHist.Reset();
  //  v0PhiHist2.Reset();
  
  for (UInt_t k=0; k<v0Vtx.size(); k++) {
    StV0Vertex *v0 = v0Vtx[k];
    if ((v0) && (v0->dcaParentToPrimaryVertex() >= 0.)) {
      Float_t e1a = ::pow(abs(v0->momentumOfDaughter(positive)),2);
      Float_t e2 = ::pow(abs(v0->momentumOfDaughter(negative)),2);
      Float_t e1 = e1a + m_prmass2;
      e2 += m_pimass2;
      e1 = TMath::Sqrt(e1);
      e2 = TMath::Sqrt(e2);
      Float_t p = ::pow(abs(v0->momentum()),2);
      Float_t inv_mass_la = TMath::Sqrt((e1+e2)*(e1+e2) - p);
      e1 = e1a + m_pimass2;
      e1 = TMath::Sqrt(e1);
      Float_t inv_mass_k0 = TMath::Sqrt((e1+e2)*(e1+e2) - p);
      
      hists->m_ev0_lama_hist->Fill(inv_mass_la);
      hists->m_ev0_k0ma_hist->Fill(inv_mass_k0);
      
      hists->m_v_vtxid->Fill(v0->type());
      if (!isnan(double(v0->position().x())))
        hists->m_v_x->Fill(v0->position().x());     
      if (!isnan(double(v0->position().y())))
        hists->m_v_y->Fill(v0->position().y());     
      if (!isnan(double(v0->position().z())))
        hists->m_v_z->Fill(v0->position().z());     
      hists->m_v_pchi2->Fill(v0->chiSquared());
      hists->m_v_r->Fill(v0->position().x()*v0->position().x() +
			 v0->position().y()*v0->position().y());
      
      if (!(isnan(double(v0->position().x())) ||
            isnan(double(v0->position().y())))) {
        Float_t phi = atan2(v0->position().y() - pvert.y(),
	                    v0->position().x() - pvert.x())
	* 180./M_PI;
        if (phi<0.) phi += 360.;
        hists->m_vtx_phi_dist->Fill(phi);
	hists->m_vtx_z_dist->Fill(v0->position().z() - pvert.z());
	Float_t r_dist = ::sqrt(::pow(v0->position().x()-pvert.x(),2)+
				::pow(v0->position().y()-pvert.y(),2));
	hists->m_vtx_r_dist->Fill(r_dist);
	//        v0PhiHist.Fill(phi);
	//        if (phi<180.) phi += 360.;
	//        v0PhiHist2.Fill(phi);
      }
    }
  }
  
  /*
   if (v0PhiHist.GetEntries()>=144) {
     for (Float_t ssig=2.; ssig<100.; ssig+=2.) {
       Int_t npeaks = v0PhiSpec.Search(&v0PhiHist,
				       (Double_t) (ssig/v0PhiHist.GetBinWidth(0)));
       Float_t* pks = v0PhiSpec.GetPositionX();
       for (Int_t ipeak=0; ipeak<npeaks; ipeak++) {
	 if ((pks[ipeak]>=90.)&&(pks[ipeak]<270.)) {
	   hists->m_vtx_phi_dist->Fill(pks[ipeak], ssig);
	 }
       }
       
       npeaks = v0PhiSpec.Search(&v0PhiHist2,
				 (Double_t) (ssig/v0PhiHist2.GetBinWidth(0)));
       pks = v0PhiSpec.GetPositionX();
       for (Int_t ipeak=0; ipeak<npeaks; ipeak++) {
	 if ((pks[ipeak]>=270.)&&(pks[ipeak]<450.)) {
	   Float_t phi2 = pks[ipeak];
	   if (phi2>360.) phi2 -= 360.;
	   hists->m_vtx_phi_dist->Fill(phi2, ssig);
	 }
       }
     }
   }
   */
  
  // Xi vertices
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling dst_xi_vertex histograms ");
  
  StSPtrVecXiVertex &xiVtx = event->xiVertices();
  hists->m_xi_tot->Fill(xiVtx.size());
  
  for (UInt_t l=0; l<xiVtx.size(); l++) {
    StXiVertex *xi = xiVtx[l];
    if (xi) {
      const StThreeVectorF& pMom = xi->momentumOfBachelor();
      StThreeVectorF lMom = xi->momentumOfV0();
      StThreeVectorF xMom = lMom + pMom;
      Float_t pP2 = pMom.mag2();
      Float_t pL2 = lMom.mag2();
      Float_t pX2 = xMom.mag2();
      Float_t epi = ::sqrt(pP2 + m_pimass2);
      Float_t ela = ::sqrt(pL2 + m_lamass2);
      Float_t eXi = ela + epi;
      Float_t inv_mass_xi = ::sqrt(eXi*eXi - pX2);
      
      hists->m_xi_ma_hist->Fill(inv_mass_xi);
      
      hists->m_v_vtxid->Fill(xi->type());
      if (!isnan(double(xi->position().x())))
        hists->m_v_x->Fill(xi->position().x());     
      if (!isnan(double(xi->position().y())))
        hists->m_v_y->Fill(xi->position().y());     
      if (!isnan(double(xi->position().z())))
        hists->m_v_z->Fill(xi->position().z());     
      hists->m_v_pchi2->Fill(xi->chiSquared());
      hists->m_v_r->Fill(xi->position().x()*xi->position().x() +
			 xi->position().y()*xi->position().y());
    }
  }
  
  // Kink vertices
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling kink histograms ");
  
  StSPtrVecKinkVertex &kinkVtx = event->kinkVertices();
  hists->m_kink_tot->Fill(kinkVtx.size());
  
  for (UInt_t m=0; m<kinkVtx.size(); m++) {
    StKinkVertex *kink = kinkVtx[m];
    if (kink) {
      //hists->m_v_detid->Fill(kink->det_id); 
      hists->m_v_vtxid->Fill(kink->type());
      if (!isnan(double(kink->position().x())))
        hists->m_v_x->Fill(kink->position().x());     
      if (!isnan(double(kink->position().y())))
        hists->m_v_y->Fill(kink->position().y());     
      if (!isnan(double(kink->position().z())))
        hists->m_v_z->Fill(kink->position().z());     
      hists->m_v_pchi2->Fill(kink->chiSquared());
      hists->m_v_r->Fill(kink->position().x()*kink->position().x() +
			 kink->position().y()*kink->position().y());
    }
  }
  
  UInt_t cntrows = 0;
  cntrows = event->numberOfPrimaryVertices() + v0Vtx.size() +
    xiVtx.size() + kinkVtx.size(); //this gives 3 less than the DSTs!!
				   //->needs to be fixed !!!
  hists->m_v_num->Fill(cntrows);
  hists->m_v_num_sm->Fill(cntrows);
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistPoint() {
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling point histograms ");
  
  StTpcHitCollection *tpcHits = event->tpcHitCollection();
  StSvtHitCollection *svtHits = event->svtHitCollection();
  StFtpcHitCollection *ftpcHits = event->ftpcHitCollection();
  StSsdHitCollection *ssdHits = event->ssdHitCollection();
  
  ULong_t totalHits = 0;
  ULong_t ftpcHitsE = 0;
  ULong_t ftpcHitsW = 0;
  StThreeVectorF hitPos;
  Int_t rotator;
  
  if (tpcHits) {
    // z and phi dist of hits
    for (UInt_t i=0; i<tpcHits->numberOfSectors(); i++) {
      StTpcSectorHitCollection* tpcHitsSector = tpcHits->sector(i);
      for (UInt_t j=0; j<tpcHitsSector->numberOfPadrows(); j++) {
        StSPtrVecTpcHit& tpcHitsVec = tpcHitsSector->padrow(j)->hits();
	for (UInt_t k=0; k<tpcHitsVec.size(); k++) {
          hitPos = tpcHitsVec[k]->position();
	  Float_t x   = hitPos.x();
	  Float_t y   = hitPos.y();
	  Float_t z   = hitPos.z();
	  Float_t phi = hitPos.phi();
	  hists->m_z_hits->Fill(z);
          // TPC East is sectors 13-24, and z<0
          // TPC West is sectors  1-12, and z>0
          // In StEvent, sectors are mapped starting at 0 instead of 1
	  if (i>11) {
            rotator = 11-i;
	    if (phi<0)
	      hists->m_pnt_phiT->Fill(360+phi/degree,0.);
	    else
	      hists->m_pnt_phiT->Fill(phi/degree,0.);
	    hists->m_pnt_padrowT->Fill(j+1,0.); // physical padrow numbering starts at 1
	    hists->m_pnt_xyTE->Fill(x,y);
	  } else {
            rotator = i-11;
	    if (phi<0)
	      hists->m_pnt_phiT->Fill(360+phi/degree,1.);
	    else
	      hists->m_pnt_phiT->Fill(phi/degree,1.);
	    hists->m_pnt_padrowT->Fill(j+1,1.); // physical padrow numbering starts at 1
	    hists->m_pnt_xyTW->Fill(x,y);
	  }
          hitPos.rotateZ(((float) rotator)*TMath::Pi()/6.0);
	  x   = hitPos.x();
	  //y   = hitPos.y();
          //mTpcSectorPlot[i]->Fill(x,y);
          mTpcSectorPlot[i]->Fill(x,(float) (j+1));
	}
      }
    }
    hists->m_pnt_tpc->Fill(tpcHits->numberOfHits());
    totalHits += tpcHits->numberOfHits();
  }
  if (svtHits) {
    ULong_t totalSvtHits = 0;
    for (UInt_t i=0; i<svtHits->numberOfBarrels(); i++) {
      StSvtBarrelHitCollection* svtbarrel = svtHits->barrel(i);
      for (UInt_t j=0; j<svtbarrel->numberOfLadders(); j++) {
        StSvtLadderHitCollection* svtladder = svtbarrel->ladder(j);
	for (UInt_t k=0; k<svtladder->numberOfWafers(); k++) {
          StSPtrVecSvtHit& svtwaferhits = svtladder->wafer(k)->hits();
	  for (UInt_t l=0; l<svtwaferhits.size(); l++) {
	    StSvtHit* svthit = svtwaferhits[l];
	    if (svthit->flag() < 4) {
              hitPos = svthit->position();
	      Float_t x = hitPos.x();
	      Float_t y = hitPos.y();
	      Float_t z = hitPos.z();
	      Float_t phi = hitPos.phi();
	      hists->m_pnt_zS->Fill(z);
	      if (phi<0)
	        hists->m_pnt_phiS->Fill(360+phi/degree);
	      else
	        hists->m_pnt_phiS->Fill(phi/degree);
	      hists->m_pnt_barrelS->Fill(i+1); // physical barrel numbering starts at 1
	      hists->m_pnt_xyS->Fill(x,y);
              totalSvtHits++;
	      
            }
            // Laser spots:
            // barrel 3, ladder 15, wafer 7
            if (i==2 && j==14 && k==6 && svthit->hybrid() == 1 &&
                svthit->anode() >= 195 && svthit->anode() <= 201 &&
                svthit->timebucket() >= 90)
              hists->m_pnt_svtLaser->Fill(eventCount,svthit->timebucket());
            // barrel 3, ladder 7, wafer 1
            if (i==2 && j==6 && k==0 && svthit->hybrid() == 2 &&
                svthit->anode() >= 195 && svthit->anode() <= 200 &&
                svthit->timebucket() >= 90 && svthit->timebucket() <= 110)
              hists->m_pnt_svtLaser->Fill(eventCount,svthit->timebucket()/2.);
	  }
        }
      }
    }
    // totalSvtHits = svtHits->numberOfHits();
    hists->m_pnt_svt->Fill(totalSvtHits);
    totalHits += totalSvtHits;
  }
  if (ftpcHits) {
    // StFtpcHitCollection doesn't differentiate between W and E FTPCs
    // so it is up to the user to check this via plane number -CPL
    for (UInt_t i=0; i<ftpcHits->numberOfPlanes(); i++) {
      for (UInt_t j=0; j<ftpcHits->plane(i)->numberOfSectors(); j++)
	for (UInt_t k=0; k<ftpcHits->plane(i)->sector(j)->hits().size(); k++) {
          Float_t x  = ftpcHits->plane(i)->sector(j)->hits()[k]->position().x();
          Float_t y  = ftpcHits->plane(i)->sector(j)->hits()[k]->position().y();
	  Int_t npads = ftpcHits->plane(i)->sector(j)->hits()[k]->padsInHit();
	  Int_t ntimebins = ftpcHits->plane(i)->sector(j)->hits()[k]->timebinsInHit();
	  UInt_t trackReferenceCount = ftpcHits->plane(i)->sector(j)->hits()[k]->trackReferenceCount();
	  if (i<10) {
	    hists->m_pnt_planeF->Fill(i+1,1.); // physical numbering starts at 1
            hists->m_pnt_xyFW->Fill(x,y);
	    hists->m_pnt_padtimeFW->Fill(ntimebins,npads);
	    if (trackReferenceCount !=0 )hists->m_globtrk_padtimeFW->Fill(ntimebins,npads);
          } else {
	    hists->m_pnt_planeF->Fill(i+1,0.); // physical numbering starts at 1
            hists->m_pnt_xyFE->Fill(x,y);
	    hists->m_pnt_padtimeFE->Fill(ntimebins,npads);
	    if (trackReferenceCount !=0 )hists->m_globtrk_padtimeFE->Fill(ntimebins,npads);
          }
	}
	if (i<10)
	  ftpcHitsW += ftpcHits->plane(i)->numberOfHits();
	else
	  ftpcHitsE += ftpcHits->plane(i)->numberOfHits();
    }
    // east and west in same histogram
    hists->m_pnt_ftpc->Fill(ftpcHitsE,0.);
    hists->m_pnt_ftpc->Fill(ftpcHitsW,1.);
    // east and west in separate histograms
    hists->m_pnt_ftpcW->Fill(ftpcHitsW);
    hists->m_pnt_ftpcE->Fill(ftpcHitsE);
    totalHits += ftpcHits->numberOfHits();
  }
  if (ssdHits) {
    hists->m_pnt_ssd->Fill(ssdHits->numberOfHits());
    totalHits += ssdHits->numberOfHits();
  }
  hists->m_pnt_tot->Fill(totalHits);
  hists->m_pnt_tot_med->Fill(totalHits);
  hists->m_pnt_tot_sm->Fill(totalHits);
}


//_____________________________________________________________________________
void StEventQAMaker::MakeHistEMC() {
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling EMC histograms ");
  
  StEmcCollection* emccol = event->emcCollection();
  //cout <<"emccol = "<<emccol<<"\n";
  if (!emccol) return;
  
  //Get vertex
  StPrimaryVertex* pvert = event->primaryVertex(0);
  
  UInt_t i;
  
  //if (Debug()) 
  //  gMessMgr->Info(" *** in StEventQAMaker - filling EMC HITS ");
  
  for(i=0; i<4; i++){
    Int_t det = i+1;
    StDetectorId id = StEmcMath::detectorId(det);
    StEmcDetector* detector=emccol->detector(id);
    if(detector) {
      Float_t energy=0.0; // Energy for whole detector
      UInt_t  nh=0;         // Hits for whole detectors
      for(UInt_t j=1;j<121;j++){
        StEmcModule* module = detector->module(j);
        if(module) {
          StSPtrVecEmcRawHit& rawHit=module->hits();
	  
          Int_t m,e,s,adc;
          Float_t eta,phi,E;
          nh += rawHit.size();
          for(UInt_t k=0;k<rawHit.size();k++){
            m   = rawHit[k]->module();
            e   = rawHit[k]->eta();
            s   = rawHit[k]->sub();
            if (s == -1) s = 1; // case of smde
            adc = rawHit[k]->adc();
            E   = rawHit[k]->energy();
            emcGeom[i]->getEta(m, e, eta); 
            emcGeom[i]->getPhi(m, s, phi);
            hists->m_emc_hits[i]->Fill(eta,phi); 
            hists->m_emc_energy2D[i]->Fill(eta,phi,E); 
            hists->m_emc_adc[i]->Fill(float(adc)); 
            hists->m_emc_energy[i]->Fill(E);
            energy += E;
          }
	}
      }
      if(nh>0)     hists->m_emc_nhit->Fill(log10(1e-30+Double_t(nh)), Float_t(det));
      if(energy>0) hists->m_emc_etot->Fill(log10(1e-30+Double_t(energy)), Float_t(det));
    }
  }
  
  //if (Debug()) 
  //  gMessMgr->Info(" *** in StEventQAMaker - filling EMC Clusters ");
  
  for(i=0; i<4; i++) {  
    Int_t det = i+1, nh;
    StDetectorId id = StEmcMath::detectorId(det);
    StEmcDetector* detector = emccol->detector(id);
    if(detector)
      {
      StEmcClusterCollection* clusters=detector->cluster();
      if(clusters)
	{
        StSPtrVecEmcCluster& cluster = clusters->clusters();
	
        if(cluster.size()>0)
	  {
          hists->m_emc_ncl->Fill(log10(1e-30+Double_t(cluster.size())),(Float_t)det);
          Float_t Etot=0.0, eta, phi, sigEta, sigPhi, eCl;
          for(UInt_t j=0;j<cluster.size();j++){
            nh     = cluster[j]->nHits();
            eCl    = cluster[j]->energy();
            eta    = cluster[j]->eta();
            sigEta = cluster[j]->sigmaEta();
            phi    = cluster[j]->phi();
            sigPhi = cluster[j]->sigmaPhi();
            if(sigEta > 0)   hists->m_emc_sig_e->Fill(sigEta, Axis_t(det));          
            if(sigPhi > 0.0) hists->m_emc_sig_p->Fill(sigPhi, Axis_t(det));
	    
            hists->m_emc_cl[det-1]->Fill(Axis_t(eta), Axis_t(phi));
            hists->m_emc_energyCl[det-1]->Fill(Axis_t(eta), Axis_t(phi), eCl);
            hists->m_emc_HitsInCl[det-1]->Fill(Axis_t(nh));
            hists->m_emc_EnergyCl[det-1]->Fill(Axis_t(eCl));
            hists->m_emc_EtaInCl[det-1]->Fill(Axis_t(eta));
            hists->m_emc_PhiInCl[det-1]->Fill(Axis_t(phi));
            Etot  += eCl;
          }
          hists->m_emc_etotCl->Fill(log10(1e-30+Etot), Axis_t(det));
	  }
	}
      }
  }      
  
  // Get the hists from StEmcPoints
  //if (Debug()) 
  //  gMessMgr->Info(" *** in StEventQAMaker - filling EMC Points ");
  
  StSPtrVecEmcPoint& pointvec = emccol->barrelPoints();
  
  Int_t Point_Mult[4];
  for(i=0;i<4;i++) {Point_Mult[i]=0;}
  
  for(i=0;i<pointvec.size();i++) {
    
    StEmcPoint *point = (StEmcPoint*) pointvec[i];
    
    //const StThreeVectorF & posP = point->position();
    const StThreeVectorF & sizeP = point->size();
    
    Float_t eta=0.;
    Float_t phi=0.;
    if (pvert) {
      eta=StEmcMath::eta(point,(StMeasuredPoint*)pvert);
      phi=StEmcMath::phi(point,(StMeasuredPoint*)pvert);
    }
    
    // Get category (ncat) for this point
    
    Float_t EnergyDet[4];
    for(UInt_t ie=0;ie<4;ie++) {EnergyDet[ie]=0.0;}
    
    for(Int_t j=0;j<4;j++) {
      StDetectorId detid = static_cast<StDetectorId>(j+kBarrelEmcTowerId);
      EnergyDet[j] = point->energyInDetector(detid);
    }
    Int_t ncat=0;
    
    if(EnergyDet[2]==0 && EnergyDet[3] ==0) {
      ncat=0;
    } else if(EnergyDet[2]>0 && EnergyDet[3] ==0) {
      ncat=1;
    } else if(EnergyDet[2]==0 && EnergyDet[3]>0) {
      ncat=2;
    } else{
      ncat=3;
    }
    
    //Fill the hists
    Float_t energy=point->energy();
    Float_t sigmaeta=sizeP.x();
    Float_t sigmaphi=sizeP.y();
    Float_t trackmom=point->chiSquare();
    Float_t deltaeta=point->deltaEta();
    Float_t deltaphi=point->deltaPhi();
    if (ncat>3) ncat=3;
    Point_Mult[ncat]++;
    if (energy>0) hists->m_emc_point_energy[ncat]->Fill(energy);
    if (pvert) {
      hists->m_emc_point_eta[ncat]->Fill(eta);
      hists->m_emc_point_phi[ncat]->Fill(phi);
    }
    hists->m_emc_point_sigeta[ncat]->Fill(sigmaeta);
    hists->m_emc_point_sigphi[ncat]->Fill(sigmaphi);
    hists->m_emc_point_flag->Fill(Float_t(ncat+1));
    if (trackmom>0) {
      hists->m_emc_point_trmom[ncat]->Fill(trackmom);
      hists->m_emc_point_deleta[ncat]->Fill(deltaeta);
      hists->m_emc_point_delphi[ncat]->Fill(deltaphi);
    }
  }
  for(i=0;i<4;i++) {hists->m_emc_points[i]->Fill(Float_t(Point_Mult[i]));}
  
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - Finished filling EMC histograms ");
  
}
//_____________________________________________________________________________
void StEventQAMaker::MakeHistEval() {
  
  // requires StMcEvent
  if (Debug()) 
    gMessMgr->Info(" *** in StEventQAMaker - filling Eval histograms ");
  
  StMcEvent* mcEvent = (StMcEvent*) GetDataSet("StMcEvent");
  if (!mcEvent) return;
  StMcVertex* mcprimaryVertex = mcEvent->primaryVertex();
  StPrimaryVertex* primaryVertex = event->primaryVertex();
  if ((primaryVertex) && (mcprimaryVertex)) {
    Float_t geantX = mcprimaryVertex->position().x();
    Float_t geantY = mcprimaryVertex->position().y();
    Float_t geantZ = mcprimaryVertex->position().z();
    Float_t recoX = primaryVertex->position().x();
    Float_t recoY = primaryVertex->position().y();
    Float_t recoZ = primaryVertex->position().z();
    hists->m_geant_reco_pvtx_x->Fill(geantX-recoX);
    hists->m_geant_reco_pvtx_y->Fill(geantY-recoY);
    hists->m_geant_reco_pvtx_z->Fill(geantZ-recoZ);
    hists->m_geant_reco_vtx_z_z->Fill(geantZ-recoZ,recoZ);    
  }
  
}
//_____________________________________________________________________________
void StEventQAMaker::MakeHistBBC() {
  
  Int_t i;
  StTriggerDetectorCollection* trig = event->triggerDetectorCollection();
  if (!trig) return;
  StBbcTriggerDetector& bbc = trig->bbc();
  
  for (i=0; i<32; i++) {
    hists->m_bbc_adc[i/8]->Fill(bbc.adc(i),i%8);
    hists->m_bbc_tdc[i/8]->Fill(bbc.tdc(i),i%8);
  }
  
}
//_____________________________________________________________________________
void StEventQAMaker::MakeHistFPD() {
  
  Int_t i;
  StFpdCollection* fpd = event->fpdCollection();
  if (!fpd) return;
  unsigned short* dfpd = fpd->adc();
  if (!dfpd) return;
  
  for (i=0; i<16; i++) {
    hists->m_fpd_top[i/8]->Fill((float) dfpd[i],i%8);
    hists->m_fpd_bottom[i/8]->Fill((float) dfpd[i+16],i%8);
    hists->m_fpd_south[i/8]->Fill((float) dfpd[i+32],i%8);
    if (i<12) hists->m_fpd_north[i/6]->Fill((float) dfpd[i+48],i%6);
  }
  
  hists->m_fpd_sums[0]->Fill((float) fpd->sumAdcTop());
  hists->m_fpd_sums[1]->Fill((float) fpd->sumAdcBottom());
  hists->m_fpd_sums[2]->Fill((float) fpd->sumAdcSouth());
  hists->m_fpd_sums[3]->Fill((float) fpd->sumAdcNorth());
  hists->m_fpd_sums[4]->Fill((float) fpd->sumAdcSmdX());
  hists->m_fpd_sums[5]->Fill((float) fpd->sumAdcSmdY());
  hists->m_fpd_sums[6]->Fill((float) fpd->sumAdcPreShower1());
  hists->m_fpd_sums[7]->Fill((float) fpd->sumAdcPreShower2());
  
}
//_____________________________________________________________________________
void StEventQAMaker::MakeHistPMD() {
  
  // Code written by Subhasis, Nov. 2004

  Int_t runNumber=event->runId();
  Int_t eventID=event->id();
  if (runNumber != mRunNumber) {
    mRunNumber = runNumber;
    if (!mPmdGeom) mPmdGeom = new StPmdGeom();
    mPmdGeom->readBoardDetail(mRunNumber);
    if (maputil) delete maputil;
    maputil = new StPmdMapUtil();
    maputil->StoreMapInfo(mRunNumber);
  }
  //get PhmdCollection
  StPhmdCollection* phmdcl = (StPhmdCollection*) (event->phmdCollection());
  //if phmdcollection exists then loop over two detectors (pmd and cpv)
  if (phmdcl) {
    for (Int_t d=0;d<2;d++) {
      
      Int_t TOTAL_HIT_DETECTOR=0;
      Int_t TOTAL_ADC_DETECTOR=0;

      //kPhmdCpvId = enum for Cpv Id,(25) and kPhmdId enum for Pmd Id (26)
      StDetectorId pdet = static_cast<StDetectorId>(kPhmdCpvId+d);
      StPhmdDetector* detector = (StPhmdDetector*) (phmdcl->detector(StDetectorId(pdet)));
      if (detector) {
        for (UInt_t j=0;j<12;j++) {
	  Float_t TotalAdc=0;
	  Float_t TotalHit=0;
	  StPhmdModule* module = detector->module(j);
	  if (module) {
	    StSPtrVecPhmdHit& rawHit=module->hits();
	    if (rawHit.size()>0) {
	      for (Int_t k=0;k<(Int_t)rawHit.size();k++) {
		Int_t sm=rawHit[k]->superModule();
		Int_t row=rawHit[k]->row();
		Int_t col=rawHit[k]->column();
		Int_t adc=rawHit[k]->adc();
		TotalAdc+=adc;
		TotalHit++;
		Int_t chainR, channelOR, channelCR;
		if (d==0) {
		  maputil->ReverseChannelOriginal(sm+13,row+1,col+1,channelOR);
		  maputil->ReverseChannelConverted(sm+13,row+1,col+1,channelCR);
		  maputil->ChainNumber(sm+13,row+1,col+1,chainR);
		} else {
		  maputil->ReverseChannelOriginal(sm+1,row+1,col+1,channelOR);
		  maputil->ReverseChannelConverted(sm+1,row+1,col+1,channelCR);
		  maputil->ChainNumber(sm+1,row+1,col+1,chainR);
		}
		if (chainR>0 && channelCR>=0 && chainR<=49) {
		  hists->m_pmd_chain_adc[chainR/2]->Fill(channelCR,chainR%2,adc);
		  hists->m_pmd_chain_hit[chainR/2]->Fill(channelCR,chainR%2);
		}
                TOTAL_HIT_DETECTOR++;;
                TOTAL_ADC_DETECTOR+=adc;
	      }
	    } //rawHit.size()
	  } //module
	  Int_t smid=d*12+j;
	  hists->m_pmd_sm_hit[smid/2]->Fill(TotalHit,smid%2);
          if (TotalHit<=0) continue;
	  hists->m_pmd_sm_adc[smid/2]->Fill(TotalAdc/TotalHit,smid%2);
	}
      }
      if (d==0) {
	hists->m_pmd_total_hit->Fill(eventID,TMath::Log10(1e-30+TOTAL_HIT_DETECTOR));
	hists->m_pmd_total_adc->Fill(eventID,TMath::Log10(1e-30+TOTAL_ADC_DETECTOR));	
      } else {
	hists->m_cpv_total_hit->Fill(eventID,TMath::Log10(1e-30+TOTAL_HIT_DETECTOR));
	hists->m_cpv_total_adc->Fill(eventID,TMath::Log10(1e-30+TOTAL_ADC_DETECTOR));	
      }
    }
  }
}

//_____________________________________________________________________________
// $Id: StEventQAMaker.cxx,v 2.71 2006/02/16 20:37:03 perev Exp $
// $Log: StEventQAMaker.cxx,v $
// Revision 2.71  2006/02/16 20:37:03  perev
// 1/0
//
// Revision 2.70  2005/09/27 20:32:22  genevb
// New method for StMcEvent access
//
// Revision 2.69  2005/05/13 19:38:05  perev
// Defence agains 1/0 added
//
// Revision 2.68  2005/05/11 18:16:06  genevb
// Fixed problem of not using estPrimary tracks
//
// Revision 2.67  2005/02/22 19:38:05  genevb
// Global tracks: use estGlobal if it exists, otherwise fall back to global
//
// Revision 2.66  2005/02/08 17:22:46  genevb
// PMD histo changes, handle estGlobal/ITTF tracks
//
// Revision 2.65  2005/02/05 01:12:25  perev
// test for zero pt added
//
// Revision 2.64  2005/01/27 05:28:25  genevb
// PMD changes
//
// Revision 2.63  2004/12/13 15:52:36  genevb
// Numerous updates: PMD, primtrk, FPD, QAShift lists
//
// Revision 2.62  2004/10/04 16:40:42  genevb
// FTPC radial histos
//
// Revision 2.61  2004/05/29 12:41:42  genevb
// handle absence of tpc & ftpc software monitors
//
// Revision 2.60  2004/04/23 23:15:29  genevb
// Added signedDCA (Impact) plots for globals
//
// Revision 2.59  2004/03/25 21:11:10  genevb
// Fixed filling of eventClass=1 null vertex hist (generally minbias)
//
// Revision 2.58  2004/03/03 01:22:31  genevb
// Small addendum to previous checkin
//
// Revision 2.57  2004/03/03 01:19:29  genevb
// Updated triggerIDs for Jeff Landgraf's scheme
//
// Revision 2.56  2004/02/12 05:03:05  genevb
// Year 4 AuAu changes. New SVT histos.
//
// Revision 2.55  2004/02/05 19:04:30  genevb
// math touchup for arc length calcs
//
// Revision 2.54  2004/01/10 01:10:17  genevb
// Preparations for Year 5, added some svt plots
//
// Revision 2.53  2003/12/04 03:56:26  perev
// 1/0 fix
//
// Revision 2.52  2003/11/25 04:19:38  perev
// FPE protection
//
// Revision 2.51  2003/11/20 03:13:55  perev
// Fix FPE
//
// Revision 2.50  2003/10/25 00:48:49  perev
// Protect chrg_ftpc_tot[0]=0 (1/0)
//
// Revision 2.49  2003/09/19 22:58:11  genevb
// Initialize pointers to zero, some doxygenization
//
// Revision 2.48  2003/09/02 17:59:21  perev
// gcc 3.2 updates + WarnOff
//
// Revision 2.47  2003/04/14 15:20:38  genevb
// Add pp 2003 trigger (first attempt)
//
// Revision 2.46  2003/02/28 16:01:07  genevb
// Further improvements for previous check-in
//
// Revision 2.45  2003/02/28 06:17:55  genevb
// Allow StQAMakerBase::Make to be called for all events
//
// Revision 2.44  2003/02/20 20:09:53  genevb
// Several changes for new trigger scheme, dAu data
//
// Revision 2.43  2003/02/19 06:38:28  genevb
// Rework trigger and mult/event class sections
//
// Revision 2.42  2003/02/15 22:00:52  genevb
// Add tpcSectors, fix ftpc east/west charge
//
// Revision 2.41  2003/01/23 20:53:10  genevb
// Additional dAu changes
//
// Revision 2.40  2003/01/23 04:08:22  jeromel
// Include fixed
//
// Revision 2.39  2003/01/17 15:15:47  genevb
// Add AllTriggers() function to ignore trigger words
//
// Revision 2.38  2002/05/29 13:54:30  genevb
// Some changes to FTPC chisq histos
//
// Revision 2.37  2002/04/23 01:59:55  genevb
// Addition of BBC/FPD histos
//
// Revision 2.36  2002/04/03 21:13:11  lansdell
// primary track first, last point residuals now use outerGeometry() for helix parameters
//
// Revision 2.35  2002/02/23 00:31:26  lansdell
// bug fix: primary vertex check histograms for a multiplicity class did not reflect the correct number of good events
//
// Revision 2.34  2002/02/12 18:41:59  genevb
// Additional FTPC histograms
//
// Revision 2.33  2002/02/10 16:48:28  jeromel
// Attempt to prevent re-creation of mHitHist.
//
// Revision 2.32  2002/02/05 22:27:30  jeromel
// Modifications from David H. Int() -> InitRun().
//
// Revision 2.31  2001/12/28 09:19:12  genevb
// Adjustments for pp running
//
// Revision 2.30  2001/12/20 03:11:07  genevb
// pp trigger words 0x2XXX
//
// Revision 2.29  2001/11/20 21:53:45  lansdell
// added x-y dist of hits, tpc east&west histos
//
// Revision 2.28  2001/11/02 21:57:44  genevb
// Fix mistake in trigger word histogram
//
// Revision 2.27  2001/11/02 20:50:03  genevb
// Changed histogram ranges for momenta
//
// Revision 2.26  2001/10/31 22:08:40  suaide
// fixed EMC histograms
//
// Revision 2.25  2001/10/24 20:11:49  genevb
// Fixed trigger issue for year 1
//
// Revision 2.24  2001/10/15 16:15:02  pavlinov
// Clenup EMC stuff for production
//
// Revision 2.23  2001/09/10 18:00:12  genevb
// Another trigger word
//
// Revision 2.22  2001/09/01 14:24:40  genevb
// Allow trigger word=0 for MC data
//
// Revision 2.21  2001/08/31 21:29:50  genevb
// Check if trigger info exists
//
// Revision 2.20  2001/08/29 20:45:15  genevb
// Trigger word histos
//
// Revision 2.19  2001/08/23 17:57:36  genevb
// Added SVT hit flag
//
// Revision 2.18  2001/08/07 07:51:27  lansdell
// primvtx check for different multiplicities crashed for MC data, now fixed
//
// Revision 2.17  2001/08/03 20:33:55  lansdell
// added primvtx check histos for different multiplicities; separated x-y plot of first point on track, tpc into east and west histos
//
// Revision 2.16  2001/07/31 23:21:42  lansdell
// added last point, hit-helix histos
//
// Revision 2.15  2001/05/25 17:46:59  lansdell
// commented out unnecessary emccol cout statement
//
// Revision 2.14  2001/05/25 16:31:20  lansdell
// more updates to qa shift histograms
//
// Revision 2.13  2001/05/24 01:48:13  lansdell
// qa_shift histograms updated
//
// Revision 2.12  2001/05/23 00:14:52  lansdell
// more changes for qa_shift histograms
//
// Revision 2.11  2001/05/16 20:57:03  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.10  2001/05/02 16:10:46  lansdell
// changed some histogram limits
//
// Revision 2.9  2001/05/01 15:17:36  genevb
// Execute EMC code only if EMC libs loaded
//
// Revision 2.8  2001/04/30 19:09:27  genevb
// Catch missing EMC info
//
// Revision 2.7  2001/04/28 22:05:13  genevb
// Added EMC histograms
//
// Revision 2.6  2001/04/25 21:35:25  genevb
// Added V0 phi distributions
//
// Revision 2.5  2001/04/24 22:53:51  lansdell
// Removed redundant radial position of first hit histograms
//
// Revision 2.4  2001/04/24 21:33:05  genevb
// Use det_id to identify detectors, and some cleanup
//
// Revision 2.3  2000/12/08 18:37:22  genevb
// Change kTofPatchId->kTofId
//
// Revision 2.2  2000/09/08 18:55:53  lansdell
// turned on FTPC primary track histograms
//
// Revision 2.1  2000/09/01 16:59:02  genevb
// Change for V0 plots
//
// Revision 2.0  2000/08/25 16:02:39  genevb
// New revision: new structure, multiplicity classes
//
