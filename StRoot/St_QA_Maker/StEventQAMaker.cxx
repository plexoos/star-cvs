// $Id: StEventQAMaker.cxx,v 1.53 2000/08/17 21:13:55 lansdell Exp $
// $Log: StEventQAMaker.cxx,v $
// Revision 1.53  2000/08/17 21:13:55  lansdell
// loop over all TPC hits for the z-hit distribution histogram
//
// Revision 1.52  2000/08/17 18:52:29  lansdell
// added z distribution of hits histogram to StEventQA set
//
// Revision 1.51  2000/08/17 17:47:53  lansdell
// for real data, only process events with # global tracks between 50 and 500 -> will later add code for various multiplicities
//
// Revision 1.50  2000/08/08 16:18:34  lansdell
// combined MakeHistDE and MakeHistPID code and simplified dE/dx code (factor of 7 faster)
//
// Revision 1.49  2000/07/31 19:29:48  lansdell
// primary vertex check histogram now contains entries for events with or without a primary vertex (with = 1, without = -1)
//
// Revision 1.48  2000/07/28 22:05:45  lansdell
// added a few more primary vertex checks for redundancy
//
// Revision 1.47  2000/07/28 19:46:04  lansdell
// changed Encap names
//
// Revision 1.46  2000/07/28 19:25:20  lansdell
// added histogram of number of events without a primary vertex
//
// Revision 1.45  2000/07/26 19:57:49  lansdell
// new histograms and functionality added (e.g., overlay several histograms, new printlist option qa_shift)
//
// Revision 1.44  2000/07/11 18:10:38  lansdell
// replaced call to pidTraits with numberOfHits to get detector ids for tracks
//
// Revision 1.43  2000/06/29 05:18:40  lansdell
// removed unnecessary cout statement
//
// Revision 1.42  2000/06/23 15:44:23  lansdell
// restore PID method to using global tracks & retitle PID histogram
//
// Revision 1.41  2000/06/13 00:36:14  lansdell
// PID histogram now only uses primary tracks
//
// Revision 1.40  2000/06/13 00:32:38  lansdell
// added SVT,TPC vertex resolution check; check that pidTraits()[0] exists
//
// Revision 1.39  2000/06/02 19:59:58  lansdell
// added null pointer check for MakeHistRich
//
// Revision 1.38  2000/06/02 01:11:51  lansdell
// added several x,y,z-dca to beam axis histograms
//
// Revision 1.37  2000/05/25 15:27:05  lansdell
// changed primtrk iflag check: 300<=iflag<400 (TPC), 600<=iflag<700 (TPC+SVT)
//
// Revision 1.36  2000/05/25 04:02:51  lansdell
// fill primtrk TPC histograms for iflag>0
//
// Revision 1.34  2000/03/15 20:20:38  lansdell
// added Craig's changes to pid histogram
//
// Revision 1.33  2000/03/13 21:59:04  lansdell
// changed good global tracks to include iflag=500 for SVT+TPC histograms
//
// Revision 1.32  2000/03/13 20:15:24  uid3118
// changed globtrk, primtrk det. id to come from first element of pidTraits() vector to match table output; include tracks with iflag=100 as good global tracks
//
// Revision 1.31  2000/02/11 04:30:56  lansdell
// removed hit detector id loops from MakeHistPoint
//
// Revision 1.30  2000/02/11 01:48:21  lansdell
// fixed phi histograms; added new histograms as per St_QA_Maker.cxx
//
// Revision 1.29  2000/02/10 03:19:26  lansdell
// filled new histograms as per St_QA_Maker.cxx
//
// Revision 1.28  2000/02/07 19:49:05  kathy
// removed L3 trigger histograms and methods that created them - this table is no longer standard on the DST; created methods BookHistEval and MakeHistEval for geant vs reco evaluation histograms; filled geant vs reco evaluation histograms for table-based data
//
// Revision 1.27  2000/02/02 17:01:49  lansdell
// changed range on phi; fixed psi vs phi histo
//
// Revision 1.26  2000/02/02 16:35:22  kathy
// fixing some histograms - booking params
//
// Revision 1.25  2000/02/02 01:37:54  lansdell
// log base 10 now applied to curvature and impact parameter values
//
// Revision 1.24  2000/02/01 21:35:09  kathy
// fix code for xi mass in StEvent histograms; change curvature and impact param histograms so it's log of the value, plotted in linear scale
//
// Revision 1.23  2000/01/31 22:15:24  kathy
// added Gene's code to make mass plot for Xi's in table and StEvent versions
//
// Revision 1.22  2000/01/14 23:14:23  lansdell
// for primtrk, now find residuals correctly
//
// Revision 1.21  2000/01/12 19:09:33  lansdell
// fixed FTPC hits indexing bug
//
// Revision 1.20  2000/01/12 02:05:27  lansdell
// fixed minor for-loop problem
//
// Revision 1.19  2000/01/11 23:50:51  lansdell
// now filling FTPC hits histograms
//
// Revision 1.18  2000/01/11 00:10:08  lansdell
// totalled hits from detectors; made code for xdif, ydif, zdif, radf more readable (search for dif and radf)
//
// Revision 1.17  2000/01/08 03:27:34  lansdell
// fixed nfit/nmax ratio in Tab version; separated hits by detector; changed vertex histograms to allow for events with 0 vertices
//
// Revision 1.16  2000/01/07 20:35:00  kathy
// make some corrections to filling hist; add point hist for each det separately
//
// Revision 1.15  2000/01/07 01:02:55  lansdell
// fixed theta histogram and filled psi vs phi
//
// Revision 1.14  2000/01/05 23:20:50  lansdell
// changed sigma() to errorOnMean() for dedx histos
//
// Revision 1.13  1999/12/16 22:14:03  lansdell
// corrected how I get r0 and phi0 for global tracks
//
// Revision 1.12  1999/12/16 04:27:34  lansdell
// histogram of psi values now in degrees instead of radians
//
// Revision 1.11  1999/12/16 03:56:19  lansdell
// mirrored Kathy's changes in St_QA_Maker.cxx: separated tpc and tpc+svt histograms for global tracks using StEvent; added r0,phi0,z0,curvature histograms for global tracks in the tpc
//
// Revision 1.10  1999/12/14 18:33:21  kathy
// removed 4 ftpc histograms as per Janet's request
//
// Revision 1.9  1999/12/13 20:08:37  lansdell
// added pt vs eta in ftpc histogram to match table QA changes; updated logy scale histograms
//
// Revision 1.8  1999/12/08 03:07:20  lansdell
// made corresponding tpc/ftpc split in dedx histograms for StEventQAMaker
//
// Revision 1.7  1999/12/07 23:14:17  kathy
// fix primary vtx histograms for dst tables; split apart the ftpc and tpc in the dedx histograms
//
// Revision 1.6  1999/12/07 18:56:00  lansdell
// added a few more histograms and updated the default list of logy plots
//
// Revision 1.5  1999/12/06 22:25:05  kathy
// split apart the tpc and ftpc (east & west) histograms for the globtrk table; had to add characters to end of each histogram pointer to differentiate the different ones; updated the default list of hist to be plotted with logy scale
//
// Revision 1.4  1999/12/02 19:38:50  lansdell
// some more histograms are filled now, but still more to go!
//
// Revision 1.3  1999/11/23 19:00:50  lansdell
// Reorganized Make() and include files (Gene)
//
//
///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  StEventQAMaker class for QA Histograms using StEvent                 //
//     adapted from St_QA_Maker                                          //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include "PhysicalConstants.h"
#include <math.h>
#include "TMath.h"
#include "SystemOfUnits.h"
#include "TH1.h"
#include "TH2.h"
#include "StEventQAMaker.h"
#include "StEventTypes.h"
#include "StTpcDedxPidAlgorithm.h"
#include "StMultiH1F.h"
#include "StDbUtilities/StCoordinates.hh"
#include "HitHistograms.h"
#include "StTpcDb/StTpcDb.h"
#include "StarClassLibrary/StTimer.hh"

ClassImp(StEventQAMaker)

//_____________________________________________________________________________
StEventQAMaker::StEventQAMaker(const char *name, const char *title):StQABookHist(name,title,"StE") {
  
}

//_____________________________________________________________________________
StEventQAMaker::~StEventQAMaker() {

}

//_____________________________________________________________________________
Int_t StEventQAMaker::Finish() {

  return StMaker::Finish();
}

//_____________________________________________________________________________
Int_t StEventQAMaker::Init() {

// StEventQAMaker - Init; book histograms and set defaults for member functions

  mHitHist = new HitHistograms("QaDedxAllSectors","dE/dx for all TPC sectors",100,0.,1.e-5,2,-0.5,1.5);
  return StQABookHist::Init();
}

//_____________________________________________________________________________
Int_t StEventQAMaker::Make() {
// StEventQAMaker - Make; fill histograms
  
  event = (StEvent *)GetInputDS("StEvent");
  if (event) {
    // only process if a primary vertex exists !!!
    //cout << "event type : " << event->info()->type() << endl;
    if (event->primaryVertex()) {
      int multiplicity = event->trackNodes().size();
      mNullPrimVtx->Fill(1);
      // process real events depending on multiplicity
      if (event->info()->type() == "NONE") {
	if (multiplicity >= 50 && multiplicity <= 500) {
	  // fill low multiplicity histogram set
	  return StQABookHist::Make();
	}
	//if (multiplicity > 500 && multiplicity <= 1000) {
	  // fill medium multiplicity histogram set
	//}
	//if (multiplicity > 1000) {
	  // fill high multiplicity histogram set
	//}
	// skip event if not in multiplicity range
	else {
	  return kStOk;
	}
      }
      // process Monte Carlo events
      else {
	return StQABookHist::Make();
      }
    }
    else {
      cout << "Warning in StEventQAMaker::Make(): no primary vertex found!" << endl;
      mNullPrimVtx->Fill(-1);
      return kStOk;
    }
  }
  else {
    cout << "Error in StEventQAMaker::Make(): no event found!" << endl;
    return kStErr;
  }
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistEvSum() {
  // Fill histograms for event summary

  //PrintInfo();
  if (Debug()) cout << " *** in StEventQAMaker - filling event summary histograms " << endl;

  StEventSummary *event_summary = event->summary();
  if (event_summary) {
    Float_t trk_tot =   event_summary->numberOfTracks();
    Float_t trk_good =  event_summary->numberOfGoodTracks();
    Float_t trk_plus =  event_summary->numberOfGoodTracks(positive);
    Float_t trk_minus = event_summary->numberOfGoodTracks(negative);

    m_trk_tot_gd->Fill(trk_good/trk_tot); 
    m_glb_trk_tot->Fill(trk_tot);
    m_glb_trk_tot_sm->Fill(trk_tot);
    m_glb_trk_plusminus->Fill(trk_plus/trk_minus);
    m_glb_trk_plusminus_sm->Fill(trk_plus/trk_minus);
    m_glb_trk_prim->Fill(event_summary->numberOfGoodPrimaryTracks());
    m_glb_trk_prim_sm->Fill(event_summary->numberOfGoodPrimaryTracks());
    m_vert_total->Fill(event_summary->numberOfVertices());
    m_vert_total_sm->Fill(event_summary->numberOfVertices());
    m_mean_pt->Fill(event_summary->meanPt());
    m_mean_pt_sm->Fill(event_summary->meanPt());
    m_mean_eta->Fill(event_summary->meanEta());
    m_rms_eta->Fill(event_summary->rmsEta());

    m_prim_vrtr->Fill(event_summary->primaryVertexPosition().perp());
    m_prim_vrtx0->Fill(event_summary->primaryVertexPosition()[0]);
    m_prim_vrtx1->Fill(event_summary->primaryVertexPosition()[1]);
    m_prim_vrtx2->Fill(event_summary->primaryVertexPosition()[2]);
  }

  if (event->softwareMonitor()) {
    StTpcSoftwareMonitor *tpcMon = event->softwareMonitor()->tpc();
    Float_t tpcChgWest=0;
    Float_t tpcChgEast=0;
    for (UInt_t i=0; i<24; i++) {
      if (i<12)
	tpcChgWest += tpcMon->chrg_tpc_in[i]+tpcMon->chrg_tpc_out[i];
      else
	tpcChgEast += tpcMon->chrg_tpc_in[i]+tpcMon->chrg_tpc_out[i];
    }
    m_glb_trk_chg->Fill(tpcChgEast/tpcChgWest);
  }
}

//-----------------------------------------------------------------
void StEventQAMaker::MakeHistGlob() {

  if (Debug()) cout << " *** in StEventQAMaker - filling global track histograms " << endl;

  StSPtrVecTrackNode &theNodes = event->trackNodes();
  Int_t cnttrk=0;
  Int_t cnttrkg=0;

  for (UInt_t i=0; i<theNodes.size(); i++) {
    StTrack *globtrk = theNodes[i]->track(global);
    if (!globtrk) continue;
    cnttrk += theNodes[i]->entries(global);
    m_globtrk_iflag->Fill(globtrk->flag());
    if (globtrk->flag()>0) {
      cnttrkg++;
      Float_t pT = -999.;
      pT = globtrk->geometry()->momentum().perp();
      Float_t lmevpt = TMath::Log10(pT*1000.0);
      Float_t theta = TMath::ASin(1.) - globtrk->geometry()->dipAngle();
      Float_t thetad = theta/degree;
      Float_t eta = globtrk->geometry()->momentum().pseudoRapidity();
      Float_t gmom = abs(globtrk->geometry()->momentum());
      Float_t lmevmom = TMath::Log10(gmom*1000.0);
      Float_t chisq0 = globtrk->fitTraits().chi2(0);
      Float_t chisq1 = globtrk->fitTraits().chi2(1);
      Float_t nfitntot = (Float_t(globtrk->fitTraits().numberOfFitPoints())) /
	                 (Float_t(globtrk->detectorInfo()->numberOfPoints()));
      Float_t nfitnmax = (Float_t(globtrk->fitTraits().numberOfFitPoints())) /
                         (Float_t(globtrk->numberOfPossiblePoints()));
      StThreeVectorF dif = globtrk->detectorInfo()->firstPoint() -
	                   globtrk->geometry()->origin();
      Float_t radf = globtrk->detectorInfo()->firstPoint().perp();

      Float_t logImpact = TMath::Log10(globtrk->impactParameter());
      Float_t logCurvature = TMath::Log10(globtrk->geometry()->curvature());

      // pathLength(double x,double y) should return path length at
      // DCA in the xy-plane to a given point
      double S = globtrk->geometry()->helix().pathLength(0,0);
      StThreeVectorD dcaToBeam = globtrk->geometry()->helix().at(S);
      // these histogram additions are for Lanny's evr QA histograms
      m_dcaToBeamXY->Fill(dcaToBeam.x(),dcaToBeam.y());
      m_dcaToBeamZ1->Fill(dcaToBeam.z());
      m_dcaToBeamZ2->Fill(dcaToBeam.z());
      m_dcaToBeamZ3->Fill(dcaToBeam.z());
      m_zDcaTanl->Fill(dcaToBeam.z(),TMath::Tan(globtrk->geometry()->dipAngle()));
      m_zDcaZf->Fill(dcaToBeam.z(),globtrk->detectorInfo()->firstPoint().z());
      m_zDcaPsi->Fill(dcaToBeam.z(),globtrk->geometry()->psi()/degree);
      if (globtrk->geometry()->origin().phi() < 0)
	m_zDcaPhi0->Fill(dcaToBeam.z(),360+globtrk->geometry()->origin().phi()/degree);
      else
	m_zDcaPhi0->Fill(dcaToBeam.z(),globtrk->geometry()->origin().phi()/degree);

// from Lanny on 2 Jul 1999 9:56:03
//1. x0,y0,z0 are coordinates on the helix at the starting point, which
//   should be close to the first TPC hit position assigned to the track.
//   The latter, different quantity is in x_first[3].

// from Helen on 14 Jul 1999 - she now fills chisq0,1 with chisq/dof
// so it doesn't need to be calculated here 

      // check if the track has hits in a detector -CPL
      if (globtrk->topologyMap().numberOfHits(kUnknownId)>0) m_det_id->Fill(kUnknownId);
      if (globtrk->topologyMap().numberOfHits(kTpcId)>0) m_det_id->Fill(kTpcId);
      if (globtrk->topologyMap().numberOfHits(kSvtId)>0) m_det_id->Fill(kSvtId);
      if (globtrk->topologyMap().numberOfHits(kRichId)>0) m_det_id->Fill(kRichId);
      if (globtrk->topologyMap().numberOfHits(kFtpcWestId)>0) m_det_id->Fill(kFtpcWestId);
      if (globtrk->topologyMap().numberOfHits(kFtpcEastId)>0) m_det_id->Fill(kFtpcEastId);
      if (globtrk->topologyMap().numberOfHits(kTofPatchId)>0) m_det_id->Fill(kTofPatchId);
      if (globtrk->topologyMap().numberOfHits(kCtbId)>0) m_det_id->Fill(kCtbId);
      if (globtrk->topologyMap().numberOfHits(kSsdId)>0) m_det_id->Fill(kSsdId);
      if (globtrk->topologyMap().numberOfHits(kBarrelEmcTowerId)>0) m_det_id->Fill(kBarrelEmcTowerId);
      if (globtrk->topologyMap().numberOfHits(kBarrelEmcPreShowerId)>0) m_det_id->Fill(kBarrelEmcPreShowerId);
      if (globtrk->topologyMap().numberOfHits(kBarrelSmdEtaStripId)>0) m_det_id->Fill(kBarrelSmdEtaStripId);
      if (globtrk->topologyMap().numberOfHits(kBarrelSmdPhiStripId)>0) m_det_id->Fill(kBarrelSmdPhiStripId);
      if (globtrk->topologyMap().numberOfHits(kEndcapEmcTowerId)>0) m_det_id->Fill(kEndcapEmcTowerId);
      if (globtrk->topologyMap().numberOfHits(kEndcapEmcPreShowerId)>0) m_det_id->Fill(kEndcapEmcPreShowerId);
      if (globtrk->topologyMap().numberOfHits(kEndcapSmdUStripId)>0) m_det_id->Fill(kEndcapSmdUStripId);
      if (globtrk->topologyMap().numberOfHits(kEndcapSmdVStripId)>0) m_det_id->Fill(kEndcapSmdVStripId);
      if (globtrk->topologyMap().numberOfHits(kZdcWestId)>0) m_det_id->Fill(kZdcWestId);
      if (globtrk->topologyMap().numberOfHits(kZdcEastId)>0) m_det_id->Fill(kZdcEastId);
      if (globtrk->topologyMap().numberOfHits(kMwpcWestId)>0) m_det_id->Fill(kMwpcWestId);
      if (globtrk->topologyMap().numberOfHits(kMwpcEastId)>0) m_det_id->Fill(kMwpcEastId);
      if (globtrk->topologyMap().numberOfHits(kTpcSsdId)>0) m_det_id->Fill(kTpcSsdId);
      if (globtrk->topologyMap().numberOfHits(kTpcSvtId)>0) m_det_id->Fill(kTpcSvtId);
      if (globtrk->topologyMap().numberOfHits(kTpcSsdSvtId)>0) m_det_id->Fill(kTpcSsdSvtId);
      if (globtrk->topologyMap().numberOfHits(kSsdSvtId)>0) m_det_id->Fill(kSsdSvtId);

      // z-dist of hits
      //for (UInt_t i=0; i<globtrk->detectorInfo()->hits().size(); i++) {
      //m_z_hits->Fill(globtrk->detectorInfo()->hits()[i]->position().z());
      //}

// now fill all TPC histograms ------------------------------------------------
      if (globtrk->flag()>=100 && globtrk->flag()<200) {

// these are TPC only
	// m_glb_f0 uses hist class StMultiH1F
        m_glb_f0->Fill(dif.x(),0.);
        m_glb_f0->Fill(dif.y(),1.);
        m_glb_f0->Fill(dif.z(),2.);

	m_glb_xf0->Fill(dif.x());
	m_glb_yf0->Fill(dif.y());
	m_glb_zf0->Fill(dif.z());
	m_glb_impactT->Fill(logImpact);
	m_glb_impactrT->Fill(globtrk->impactParameter());

	// TPC padrow histogram
	StTpcCoordinateTransform transformer(gStTpcDb);
	StGlobalCoordinate globalHitPosition(globtrk->detectorInfo()->firstPoint());
	StTpcPadCoordinate padCoord;
	transformer(globalHitPosition,padCoord);
	m_glb_padfT->Fill(padCoord.row());

// these are TPC & FTPC
	m_pointT->Fill(globtrk->detectorInfo()->numberOfPoints());
	m_max_pointT->Fill(globtrk->numberOfPossiblePoints());
	m_fit_pointT->Fill(globtrk->fitTraits().numberOfFitPoints());
	m_glb_chargeT->Fill(globtrk->geometry()->charge());

	m_glb_r0T->Fill(globtrk->geometry()->origin().perp());
	if (globtrk->geometry()->origin().phi() < 0)
	  m_glb_phi0T->Fill(360+globtrk->geometry()->origin().phi()/degree);
	else
	  m_glb_phi0T->Fill(globtrk->geometry()->origin().phi()/degree);
	m_glb_z0T->Fill(globtrk->geometry()->origin().z());
	m_glb_curvT->Fill(logCurvature);

	m_glb_rfT->Fill(globtrk->detectorInfo()->firstPoint().perp());
	m_glb_xfT->Fill(globtrk->detectorInfo()->firstPoint().x());
	m_glb_yfT->Fill(globtrk->detectorInfo()->firstPoint().y());
	m_glb_zfT->Fill(globtrk->detectorInfo()->firstPoint().z());
	m_glb_radfT->Fill(radf);
	m_glb_ratioT->Fill(nfitntot);
        m_glb_ratiomT->Fill(nfitnmax);
	m_psiT->Fill(globtrk->geometry()->psi()/degree);
	m_tanlT->Fill(TMath::Tan(globtrk->geometry()->dipAngle()));
	m_glb_thetaT->Fill(thetad);
	m_etaT->Fill(eta);
	m_pTT->Fill(pT);
	m_momT->Fill(gmom);
	m_lengthT->Fill(globtrk->length());
	m_chisq0T->Fill(chisq0);
	m_chisq1T->Fill(chisq1);

// these are for TPC & FTPC
	m_globtrk_xf_yfT->Fill(globtrk->detectorInfo()->firstPoint().x(),
			       globtrk->detectorInfo()->firstPoint().y());
	m_eta_trklengthT->Fill(eta,globtrk->length());
	m_npoint_lengthT->Fill(globtrk->length(),
	      		       Float_t(globtrk->detectorInfo()->numberOfPoints()));
	m_fpoint_lengthT->Fill(globtrk->length(),
			       Float_t(globtrk->fitTraits().numberOfFitPoints()));

// these are TPC only
	m_pT_eta_recT->Fill(eta,lmevpt);
	if (event->primaryVertex()) {
	  m_tanl_zfT->Fill(globtrk->detectorInfo()->firstPoint().z() -
			   event->primaryVertex()->position().z(),
			   Float_t(TMath::Tan(globtrk->geometry()->dipAngle())));
	}
	m_mom_trklengthT->Fill(globtrk->length(),lmevmom);
	m_chisq0_momT->Fill(lmevmom,chisq0);
	m_chisq1_momT->Fill(lmevmom,chisq1);
	m_chisq0_etaT->Fill(eta,chisq0);
	m_chisq1_etaT->Fill(eta,chisq1);
	m_chisq0_dipT->Fill(TMath::Tan(globtrk->geometry()->dipAngle()),chisq0);
	m_chisq1_dipT->Fill(TMath::Tan(globtrk->geometry()->dipAngle()),chisq1);
	m_chisq0_zfT->Fill(globtrk->detectorInfo()->firstPoint().z(),chisq0);
	m_chisq1_zfT->Fill(globtrk->detectorInfo()->firstPoint().z(),chisq1);
	if (globtrk->geometry()->origin().phi() < 0)
	  m_chisq0_phiT->Fill(360+globtrk->geometry()->origin().phi()/degree,chisq0);
	else
	  m_chisq0_phiT->Fill(globtrk->geometry()->origin().phi()/degree,chisq0);
	m_nfptonpt_momT->Fill(lmevmom,nfitntot);
	m_nfptonpt_etaT->Fill(eta,nfitntot);
	// had to make psi_deg and phi_deg b/c ROOT won't compile otherwise
	// for some strange reason... -CPL
	Float_t phi_deg;
	if (globtrk->geometry()->origin().phi() < 0)
	  phi_deg = 360+globtrk->geometry()->origin().phi()/degree;
	else
	  phi_deg = globtrk->geometry()->origin().phi()/degree;
	Float_t psi_deg = globtrk->geometry()->psi()/degree;
	m_psi_phiT->Fill(phi_deg,psi_deg);
      }

// now fill all TPC+SVT histograms --------------------------------------------

      if (globtrk->flag()>=500 && globtrk->flag()<600 ) {

        m_glb_f0TS->Fill(dif.x(),0.);
        m_glb_f0TS->Fill(dif.y(),1.);
        m_glb_f0TS->Fill(dif.z(),2.);

        m_glb_xf0TS->Fill(dif.x());
        m_glb_yf0TS->Fill(dif.y());
        m_glb_zf0TS->Fill(dif.z());
	m_glb_impactTS->Fill(logImpact);
	m_glb_impactrTS->Fill(globtrk->impactParameter());

	m_pointTS->Fill(globtrk->detectorInfo()->numberOfPoints());
	m_max_pointTS->Fill(globtrk->numberOfPossiblePoints());
	m_fit_pointTS->Fill(globtrk->fitTraits().numberOfFitPoints());
	m_glb_chargeTS->Fill(globtrk->geometry()->charge());

	m_glb_r0TS->Fill(globtrk->geometry()->origin().perp());
	if (globtrk->geometry()->origin().phi() < 0)
	  m_glb_phi0TS->Fill(360+globtrk->geometry()->origin().phi()/degree);
	else
	  m_glb_phi0TS->Fill(globtrk->geometry()->origin().phi()/degree);
	m_glb_z0TS->Fill(globtrk->geometry()->origin().z());
	m_glb_curvTS->Fill(logCurvature);

	m_glb_rfTS->Fill(globtrk->detectorInfo()->firstPoint().perp());
	m_glb_xfTS->Fill(globtrk->detectorInfo()->firstPoint().x());
	m_glb_yfTS->Fill(globtrk->detectorInfo()->firstPoint().y());
	m_glb_zfTS->Fill(globtrk->detectorInfo()->firstPoint().z());
	m_glb_radfTS->Fill(radf);
	m_glb_ratioTS->Fill(nfitntot);
        m_glb_ratiomTS->Fill(nfitnmax);
	m_psiTS->Fill(globtrk->geometry()->psi()/degree);
	m_tanlTS->Fill(TMath::Tan(globtrk->geometry()->dipAngle()));
	m_glb_thetaTS->Fill(thetad);
	m_etaTS->Fill(eta);
	m_pTTS->Fill(pT);
	m_momTS->Fill(gmom);
	m_lengthTS->Fill(globtrk->length());
	m_chisq0TS->Fill(chisq0);
	m_chisq1TS->Fill(chisq1);
	m_globtrk_xf_yfTS->Fill(globtrk->detectorInfo()->firstPoint().x(),
			       globtrk->detectorInfo()->firstPoint().y());
	m_eta_trklengthTS->Fill(eta,globtrk->length());
	m_npoint_lengthTS->Fill(globtrk->length(),
	      		       Float_t(globtrk->detectorInfo()->numberOfPoints()));
	m_fpoint_lengthTS->Fill(globtrk->length(),
			       Float_t(globtrk->fitTraits().numberOfFitPoints()));

	m_pT_eta_recTS->Fill(eta,lmevpt);
	if (event->primaryVertex()) {
	  m_tanl_zfTS->Fill(globtrk->detectorInfo()->firstPoint().z() -
			    event->primaryVertex()->position().z(),
			    Float_t(TMath::Tan(globtrk->geometry()->dipAngle())));
	}
	m_mom_trklengthTS->Fill(globtrk->length(),lmevmom);
	m_chisq0_momTS->Fill(lmevmom,chisq0);
	m_chisq1_momTS->Fill(lmevmom,chisq1);
	m_chisq0_etaTS->Fill(eta,chisq0);
	m_chisq1_etaTS->Fill(eta,chisq1);
	m_chisq0_dipTS->Fill(TMath::Tan(globtrk->geometry()->dipAngle()),chisq0);
	m_chisq1_dipTS->Fill(TMath::Tan(globtrk->geometry()->dipAngle()),chisq1);
	m_chisq0_zfTS->Fill(globtrk->detectorInfo()->firstPoint().z(),chisq0);
	m_chisq1_zfTS->Fill(globtrk->detectorInfo()->firstPoint().z(),chisq1);
	if (globtrk->geometry()->origin().phi() < 0)
	  m_chisq0_phiTS->Fill(360+globtrk->geometry()->origin().phi()/degree,chisq0);
	else
	  m_chisq0_phiTS->Fill(globtrk->geometry()->origin().phi()/degree,chisq0);

	m_nfptonpt_momTS->Fill(lmevmom,nfitntot);
	m_nfptonpt_etaTS->Fill(eta,nfitntot);
	// had to make psi_deg and phi_deg b/c ROOT won't compile otherwise
	// for some strange reason... -CPL
	Float_t phi_deg;
	if (globtrk->geometry()->origin().phi() < 0)
	  phi_deg = 360+globtrk->geometry()->origin().phi()/degree;
	else
	  phi_deg = globtrk->geometry()->origin().phi()/degree;
	Float_t psi_deg = globtrk->geometry()->psi()/degree;
	m_psi_phiTS->Fill(phi_deg,psi_deg);
      }

// now fill all FTPC East histograms ------------------------------------------
      if (globtrk->flag()>700 && globtrk->flag()<800 && globtrk->topologyMap().numberOfHits(kFtpcEastId)>0) {

// these are TPC & FTPC
	m_pointFE->Fill(globtrk->detectorInfo()->numberOfPoints());
	m_max_pointFE->Fill(globtrk->numberOfPossiblePoints());
	m_fit_pointFE->Fill(globtrk->fitTraits().numberOfFitPoints());
	m_glb_chargeFE->Fill(globtrk->geometry()->charge());
	m_glb_rfFE->Fill(globtrk->detectorInfo()->firstPoint().perp());
	m_glb_xfFE->Fill(globtrk->detectorInfo()->firstPoint().x());
	m_glb_yfFE->Fill(globtrk->detectorInfo()->firstPoint().y());
	m_glb_zfFE->Fill(globtrk->detectorInfo()->firstPoint().z());
	m_glb_radfFE->Fill(radf);
	m_glb_ratioFE->Fill(nfitntot);
        m_glb_ratiomFE->Fill(nfitnmax);
	m_psiFE->Fill(globtrk->geometry()->psi()/degree);
	m_etaFE->Fill(eta);
	m_pTFE->Fill(pT);
	m_momFE->Fill(gmom);
	m_lengthFE->Fill(globtrk->length());
	m_chisq0FE->Fill(chisq0);
	m_chisq1FE->Fill(chisq1);

// these are for TPC & FTPC
	m_pT_eta_recFE->Fill(eta,lmevpt);
	m_globtrk_xf_yfFE->Fill(globtrk->detectorInfo()->firstPoint().x(),
			       globtrk->detectorInfo()->firstPoint().y());
	m_eta_trklengthFE->Fill(eta,globtrk->length());
	m_npoint_lengthFE->Fill(globtrk->length(),
	      		       Float_t(globtrk->detectorInfo()->numberOfPoints()));
	m_fpoint_lengthFE->Fill(globtrk->length(),
			       Float_t(globtrk->fitTraits().numberOfFitPoints()));

      }
// now fill all FTPC West histograms ------------------------------------------
      if (globtrk->flag()>700 && globtrk->flag()<800 && globtrk->topologyMap().numberOfHits(kFtpcWestId)>0) {

// these are TPC & FTPC
	m_pointFW->Fill(globtrk->detectorInfo()->numberOfPoints());
	m_max_pointFW->Fill(globtrk->numberOfPossiblePoints());
	m_fit_pointFW->Fill(globtrk->fitTraits().numberOfFitPoints());
	m_glb_chargeFW->Fill(globtrk->geometry()->charge());
	m_glb_rfFW->Fill(globtrk->detectorInfo()->firstPoint().perp());
	m_glb_xfFW->Fill(globtrk->detectorInfo()->firstPoint().x());
	m_glb_yfFW->Fill(globtrk->detectorInfo()->firstPoint().y());
	m_glb_zfFW->Fill(globtrk->detectorInfo()->firstPoint().z());
	m_glb_radfFW->Fill(radf);
	m_glb_ratioFW->Fill(nfitntot);
        m_glb_ratiomFW->Fill(nfitnmax);
	m_psiFW->Fill(globtrk->geometry()->psi()/degree);
	m_etaFW->Fill(eta);
	m_pTFW->Fill(pT);
	m_momFW->Fill(gmom);
	m_lengthFW->Fill(globtrk->length());
	m_chisq0FW->Fill(chisq0);
	m_chisq1FW->Fill(chisq1);

// these are for TPC & FTPC
	m_pT_eta_recFW->Fill(eta,lmevpt);
	m_globtrk_xf_yfFW->Fill(globtrk->detectorInfo()->firstPoint().x(),
			       globtrk->detectorInfo()->firstPoint().y());
	m_eta_trklengthFW->Fill(eta,globtrk->length());
	m_npoint_lengthFW->Fill(globtrk->length(),
	      		       Float_t(globtrk->detectorInfo()->numberOfPoints()));
	m_fpoint_lengthFW->Fill(globtrk->length(),
			       Float_t(globtrk->fitTraits().numberOfFitPoints()));

      }
    }
  }
  m_globtrk_tot->Fill(cnttrk);
  m_globtrk_tot_sm->Fill(cnttrk);
  m_globtrk_good->Fill(cnttrkg);
  m_globtrk_good_sm->Fill(cnttrkg);
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistDE() {
  // histograms filled in MakeHistPID() method
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistPrim() {

  if (Debug()) cout << " *** in StEventQAMaker - filling primary track histograms " << endl;

  Int_t cnttrk=0;
  Int_t cnttrkg=0;

  StPrimaryVertex *primVtx = event->primaryVertex();
  UInt_t daughters=0;
  UInt_t currentNumber=0;
  if (primVtx) {
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
    m_primtrk_tot->Fill(cnttrk);
    m_primtrk_tot_sm->Fill(cnttrk);

    for (UInt_t i=0; i<primVtx->numberOfDaughters(); i++) {
      StTrack *primtrk = primVtx->daughter(i);
      m_primtrk_iflag->Fill(primtrk->flag());

      if (primtrk->flag()>0) {
        cnttrkg++;
	Float_t pT = -999.;
	pT = primtrk->geometry()->momentum().perp();
        Float_t lmevpt = TMath::Log10(pT*1000.0);
	Float_t theta = TMath::ASin(1.) - primtrk->geometry()->dipAngle();
	Float_t thetad = theta/degree;
	Float_t eta   = primtrk->geometry()->momentum().pseudoRapidity();
	Float_t gmom = abs(primtrk->geometry()->momentum());
        Float_t lmevmom = TMath::Log10(gmom*1000.0); 
	Float_t chisq0 = primtrk->fitTraits().chi2(0);
	Float_t chisq1 = primtrk->fitTraits().chi2(1);
	Float_t nfitnmax = (Float_t(primtrk->fitTraits().numberOfFitPoints())) /
	                   (Float_t(primtrk->numberOfPossiblePoints()));
        Float_t nfitntot = (Float_t(primtrk->fitTraits().numberOfFitPoints()))/
	                   (Float_t(primtrk->detectorInfo()->numberOfPoints()));

	Float_t logImpact = TMath::Log10(primtrk->impactParameter());
	Float_t logCurvature = TMath::Log10(primtrk->geometry()->curvature());


	// need to find position on helix closest to first point on track since
	// the primary vertex is used as the first point on helix for primary
	// tracks -CPL
	double s = primtrk->geometry()->helix().
	           pathLength(primtrk->detectorInfo()->firstPoint());
	StThreeVectorF dif = primtrk->detectorInfo()->firstPoint() -
	                     primtrk->geometry()->helix().at(s);
        Float_t radf = primtrk->detectorInfo()->firstPoint().perp();

	// check if the track has hits in a detector -CPL
	if (primtrk->topologyMap().numberOfHits(kUnknownId)>0) m_pdet_id->Fill(kUnknownId);
	if (primtrk->topologyMap().numberOfHits(kTpcId)>0) m_pdet_id->Fill(kTpcId);
	if (primtrk->topologyMap().numberOfHits(kSvtId)>0) m_pdet_id->Fill(kSvtId);
	if (primtrk->topologyMap().numberOfHits(kRichId)>0) m_pdet_id->Fill(kRichId);
	if (primtrk->topologyMap().numberOfHits(kFtpcWestId)>0) m_pdet_id->Fill(kFtpcWestId);
	if (primtrk->topologyMap().numberOfHits(kFtpcEastId)>0) m_pdet_id->Fill(kFtpcEastId);
	if (primtrk->topologyMap().numberOfHits(kTofPatchId)>0) m_pdet_id->Fill(kTofPatchId);
	if (primtrk->topologyMap().numberOfHits(kCtbId)>0) m_pdet_id->Fill(kCtbId);
	if (primtrk->topologyMap().numberOfHits(kSsdId)>0) m_pdet_id->Fill(kSsdId);
	if (primtrk->topologyMap().numberOfHits(kBarrelEmcTowerId)>0) m_pdet_id->Fill(kBarrelEmcTowerId);
	if (primtrk->topologyMap().numberOfHits(kBarrelEmcPreShowerId)>0) m_pdet_id->Fill(kBarrelEmcPreShowerId);
	if (primtrk->topologyMap().numberOfHits(kBarrelSmdEtaStripId)>0) m_pdet_id->Fill(kBarrelSmdEtaStripId);
	if (primtrk->topologyMap().numberOfHits(kBarrelSmdPhiStripId)>0) m_pdet_id->Fill(kBarrelSmdPhiStripId);
	if (primtrk->topologyMap().numberOfHits(kEndcapEmcTowerId)>0) m_pdet_id->Fill(kEndcapEmcTowerId);
	if (primtrk->topologyMap().numberOfHits(kEndcapEmcPreShowerId)>0) m_pdet_id->Fill(kEndcapEmcPreShowerId);
	if (primtrk->topologyMap().numberOfHits(kEndcapSmdUStripId)>0) m_pdet_id->Fill(kEndcapSmdUStripId);
	if (primtrk->topologyMap().numberOfHits(kEndcapSmdVStripId)>0) m_pdet_id->Fill(kEndcapSmdVStripId);
	if (primtrk->topologyMap().numberOfHits(kZdcWestId)>0) m_pdet_id->Fill(kZdcWestId);
	if (primtrk->topologyMap().numberOfHits(kZdcEastId)>0) m_pdet_id->Fill(kZdcEastId);
	if (primtrk->topologyMap().numberOfHits(kMwpcWestId)>0) m_pdet_id->Fill(kMwpcWestId);
	if (primtrk->topologyMap().numberOfHits(kMwpcEastId)>0) m_pdet_id->Fill(kMwpcEastId);
	if (primtrk->topologyMap().numberOfHits(kTpcSsdId)>0) m_pdet_id->Fill(kTpcSsdId);
	if (primtrk->topologyMap().numberOfHits(kTpcSvtId)>0) m_pdet_id->Fill(kTpcSvtId);
	if (primtrk->topologyMap().numberOfHits(kTpcSsdSvtId)>0) m_pdet_id->Fill(kTpcSsdSvtId);
	if (primtrk->topologyMap().numberOfHits(kSsdSvtId)>0) m_pdet_id->Fill(kSsdSvtId);

// now fill all TPC histograms ------------------------------------------------
	if (primtrk->flag()>=300 && primtrk->flag()<400) {

// these are TPC only
	  m_prim_f0->Fill(dif.x(),0.);
	  m_prim_f0->Fill(dif.y(),1.);
	  m_prim_f0->Fill(dif.z(),2.);

	  m_prim_xf0->Fill(dif.x());
	  m_prim_yf0->Fill(dif.y());
	  m_prim_zf0->Fill(dif.z());
	  m_prim_impactT->Fill(logImpact);
	  m_prim_impactrT->Fill(primtrk->impactParameter());

	  // TPC gains histograms
	  if (event->summary()) {
	    mHitHist->clear();
	    mHitHist->setTrack(primtrk);
	    mHitHist->setBField(event->summary()->magneticField());
	    mHitHist->findHits();
	    mHitHist->fillHistograms();
	  }

// these are TPC & FTPC
	  m_ppointT->Fill(primtrk->detectorInfo()->numberOfPoints());
	  m_pmax_pointT->Fill(primtrk->numberOfPossiblePoints());
	  m_pfit_pointT->Fill(primtrk->fitTraits().numberOfFitPoints());
	  m_prim_chargeT->Fill(primtrk->geometry()->charge());

	  m_prim_r0T->Fill(primtrk->geometry()->origin().perp());
	  if (primtrk->geometry()->origin().phi() < 0)
	    m_prim_phi0T->Fill(360+primtrk->geometry()->origin().phi()/degree);
	  else
	    m_prim_phi0T->Fill(primtrk->geometry()->origin().phi()/degree);
	  m_prim_z0T->Fill(primtrk->geometry()->origin().z());
	  m_prim_curvT->Fill(logCurvature);

	  m_prim_xfT->Fill(primtrk->detectorInfo()->firstPoint().x());
	  m_prim_yfT->Fill(primtrk->detectorInfo()->firstPoint().y());
	  m_prim_zfT->Fill(primtrk->detectorInfo()->firstPoint().z());
	  m_prim_radfT->Fill(radf);
	  m_prim_ratioT->Fill(nfitntot);
	  m_prim_ratiomT->Fill(nfitnmax);
	  m_ppsiT->Fill(primtrk->geometry()->psi()/degree);
	  m_ptanlT->Fill(TMath::Tan(primtrk->geometry()->dipAngle()));
	  m_prim_thetaT->Fill(thetad);
	  m_petaT->Fill(eta);
	  m_ppTT->Fill(pT);
	  m_pmomT->Fill(gmom);
	  m_plengthT->Fill(primtrk->length());
	  m_pchisq0T->Fill(chisq0);
	  m_pchisq1T->Fill(chisq1);

// these are for TPC & FTPC
	  m_primtrk_xf_yfT->Fill(primtrk->detectorInfo()->firstPoint().x(),
				 primtrk->detectorInfo()->firstPoint().y());
	  m_peta_trklengthT->Fill(eta,primtrk->length());
	  m_pnpoint_lengthT->Fill(primtrk->length(),
				  Float_t(primtrk->detectorInfo()->numberOfPoints()));
	  m_pfpoint_lengthT->Fill(primtrk->length(),
				  Float_t(primtrk->fitTraits().numberOfFitPoints()));

// these are TPC only
	  m_ppT_eta_recT->Fill(eta,lmevpt);
	  m_ptanl_zfT->Fill(primtrk->detectorInfo()->firstPoint().z() -
			    event->primaryVertex()->position().z(),
			    Float_t(TMath::Tan(primtrk->geometry()->dipAngle())));
	  m_pmom_trklengthT->Fill(primtrk->length(),lmevmom);
	  m_pchisq0_momT->Fill(lmevmom,chisq0);
	  m_pchisq1_momT->Fill(lmevmom,chisq1);
	  m_pchisq0_etaT->Fill(eta,chisq0);
	  m_pchisq1_etaT->Fill(eta,chisq1);
	  m_pchisq0_dipT->Fill(TMath::Tan(primtrk->geometry()->dipAngle()),chisq0);
	  m_pchisq1_dipT->Fill(TMath::Tan(primtrk->geometry()->dipAngle()),chisq1);
	  m_pchisq0_zfT->Fill(primtrk->detectorInfo()->firstPoint().z(),chisq0);
	  m_pchisq1_zfT->Fill(primtrk->detectorInfo()->firstPoint().z(),chisq1);
	  m_pnfptonpt_momT->Fill(lmevmom,nfitntot);
	  m_pnfptonpt_etaT->Fill(eta,nfitntot);
	  // had to make psi_deg and phi_deg b/c ROOT won't compile otherwise
	  // for some strange reason... -CPL
	  Float_t phi_deg;
	  if (primtrk->geometry()->origin().phi() < 0)
	    phi_deg = 360+primtrk->geometry()->origin().phi()/degree;
	  else
	    phi_deg = primtrk->geometry()->origin().phi()/degree;
	  Float_t psi_deg = primtrk->geometry()->psi()/degree;
	  m_ppsi_phiT->Fill(phi_deg,psi_deg);
	}

// now fill all TPC+SVT histograms --------------------------------------------

	if (primtrk->flag()>=600 && primtrk->flag()<700) {

	  m_prim_f0TS->Fill(dif.x(),0.);
	  m_prim_f0TS->Fill(dif.y(),1.);
	  m_prim_f0TS->Fill(dif.z(),2.);

	  m_prim_xf0TS->Fill(dif.x());
	  m_prim_yf0TS->Fill(dif.y());
	  m_prim_zf0TS->Fill(dif.z());
	  m_prim_impactTS->Fill(logImpact);
	  m_prim_impactrTS->Fill(primtrk->impactParameter());

	  m_ppointTS->Fill(primtrk->detectorInfo()->numberOfPoints());
	  m_pmax_pointTS->Fill(primtrk->numberOfPossiblePoints());
	  m_pfit_pointTS->Fill(primtrk->fitTraits().numberOfFitPoints());
	  m_prim_chargeTS->Fill(primtrk->geometry()->charge());

	  m_prim_r0TS->Fill(primtrk->geometry()->origin().perp());
	  if (primtrk->geometry()->origin().phi() < 0)
	    m_prim_phi0TS->Fill(360+primtrk->geometry()->origin().phi()/degree);
	  else
	    m_prim_phi0TS->Fill(primtrk->geometry()->origin().phi()/degree);
	  m_prim_z0TS->Fill(primtrk->geometry()->origin().z());
	  m_prim_curvTS->Fill(logCurvature);

	  m_prim_xfTS->Fill(primtrk->detectorInfo()->firstPoint().x());
	  m_prim_yfTS->Fill(primtrk->detectorInfo()->firstPoint().y());
	  m_prim_zfTS->Fill(primtrk->detectorInfo()->firstPoint().z());
	  m_prim_radfTS->Fill(radf);
	  m_prim_ratioTS->Fill(nfitntot);
	  m_prim_ratiomTS->Fill(nfitnmax);
	  m_ppsiTS->Fill(primtrk->geometry()->psi()/degree);
	  m_ptanlTS->Fill(TMath::Tan(primtrk->geometry()->dipAngle()));
	  m_prim_thetaTS->Fill(thetad);
	  m_petaTS->Fill(eta);
	  m_ppTTS->Fill(pT);
	  m_pmomTS->Fill(gmom);
	  m_plengthTS->Fill(primtrk->length());
	  m_pchisq0TS->Fill(chisq0);
	  m_pchisq1TS->Fill(chisq1);

	  m_primtrk_xf_yfTS->Fill(primtrk->detectorInfo()->firstPoint().x(),
				  primtrk->detectorInfo()->firstPoint().y());
	  m_peta_trklengthTS->Fill(eta,primtrk->length());
	  m_pnpoint_lengthTS->Fill(primtrk->length(),
				   Float_t(primtrk->detectorInfo()->numberOfPoints()));
	  m_pfpoint_lengthTS->Fill(primtrk->length(),
				   Float_t(primtrk->fitTraits().numberOfFitPoints()));

	  m_ppT_eta_recTS->Fill(eta,lmevpt);
	  m_ptanl_zfTS->Fill(primtrk->detectorInfo()->firstPoint().z() -
			     event->primaryVertex()->position().z(),
			     Float_t(TMath::Tan(primtrk->geometry()->dipAngle())));
	  m_pmom_trklengthTS->Fill(primtrk->length(),lmevmom);
	  m_pchisq0_momTS->Fill(lmevmom,chisq0);
	  m_pchisq1_momTS->Fill(lmevmom,chisq1);
	  m_pchisq0_etaTS->Fill(eta,chisq0);
	  m_pchisq1_etaTS->Fill(eta,chisq1);
	  m_pchisq0_dipTS->Fill(TMath::Tan(primtrk->geometry()->dipAngle()),chisq0);
	  m_pchisq1_dipTS->Fill(TMath::Tan(primtrk->geometry()->dipAngle()),chisq1);
	  m_pchisq0_zfTS->Fill(primtrk->detectorInfo()->firstPoint().z(),chisq0);
	  m_pchisq1_zfTS->Fill(primtrk->detectorInfo()->firstPoint().z(),chisq1);
	  m_pnfptonpt_momTS->Fill(lmevmom,nfitntot);
	  m_pnfptonpt_etaTS->Fill(eta,nfitntot);
	  // had to make psi_deg and phi_deg b/c ROOT won't compile otherwise
	  // for some strange reason... -CPL
	  Float_t phi_deg;
	  if (primtrk->geometry()->origin().phi() < 0)
	    phi_deg = 360+primtrk->geometry()->origin().phi()/degree;
	  else
	    phi_deg = primtrk->geometry()->origin().phi()/degree;
	  Float_t psi_deg = primtrk->geometry()->psi()/degree;
	  m_ppsi_phiTS->Fill(phi_deg,psi_deg);
	}

/* The following are for the FTPC, which doesn't do primary tracking yet.
// now fill all FTPC East histograms ------------------------------------------
	if (primtrk->flag()>700 && primtrk->flag()<800 && primtrk->topologyMap().numberOfHits(kFtpcEastId)>0) {

// these are TPC & FTPC
	  m_ppointFE->Fill(primtrk->detectorInfo()->numberOfPoints());
	  m_pmax_pointFE->Fill(primtrk->numberOfPossiblePoints());
	  m_pfit_pointFE->Fill(primtrk->fitTraits().numberOfFitPoints());
	  m_prim_chargeFE->Fill(primtrk->geometry()->charge());
	  m_prim_xfFE->Fill(primtrk->detectorInfo()->firstPoint().x());
	  m_prim_yfFE->Fill(primtrk->detectorInfo()->firstPoint().y());
	  m_prim_zfFE->Fill(primtrk->detectorInfo()->firstPoint().z());
	  m_prim_radfFE->Fill(radf);
	  m_prim_ratioFE->Fill(nfitntot);
	  m_prim_ratiomFE->Fill(nfitnmax);
	  m_ppsiFE->Fill(primtrk->geometry()->psi()/degree);
	  m_petaFE->Fill(eta);
	  m_ppTFE->Fill(pT);
	  m_pmomFE->Fill(gmom);
	  m_plengthFE->Fill(primtrk->length());
	  m_pchisq0FE->Fill(chisq0);
	  m_pchisq1FE->Fill(chisq1);

// these are for TPC & FTPC
	  m_ppT_eta_recFE->Fill(eta,lmevpt);
	  m_primtrk_xf_yfFE->Fill(primtrk->detectorInfo()->firstPoint().x(),
				  primtrk->detectorInfo()->firstPoint().y());
	  m_peta_trklengthFE->Fill(eta,primtrk->length());
	  m_pnpoint_lengthFE->Fill(primtrk->length(),
				   Float_t(primtrk->detectorInfo()->numberOfPoints()));
	  m_pfpoint_lengthFE->Fill(primtrk->length(),
				   Float_t(primtrk->fitTraits().numberOfFitPoints()));
	}

// now fill all FTPC West histograms ------------------------------------------
	if (primtrk->flag()>700 && primtrk->flag()<800 && primtrk->topologyMap().numberOfHits(kFtpcWestId)>0) {

// these are TPC & FTPC
	  m_ppointFW->Fill(primtrk->detectorInfo()->numberOfPoints());
	  m_pmax_pointFW->Fill(primtrk->numberOfPossiblePoints());
	  m_pfit_pointFW->Fill(primtrk->fitTraits().numberOfFitPoints());
	  m_prim_chargeFW->Fill(primtrk->geometry()->charge());
	  m_prim_xfFW->Fill(primtrk->detectorInfo()->firstPoint().x());
	  m_prim_yfFW->Fill(primtrk->detectorInfo()->firstPoint().y());
	  m_prim_zfFW->Fill(primtrk->detectorInfo()->firstPoint().z());
	  m_prim_radfFW->Fill(radf);
	  m_prim_ratioFW->Fill(nfitntot);
	  m_prim_ratiomFW->Fill(nfitnmax);
	  m_ppsiFW->Fill(primtrk->geometry()->psi()/degree);
	  m_petaFW->Fill(eta);
	  m_ppTFW->Fill(pT);
	  m_pmomFW->Fill(gmom);
	  m_plengthFW->Fill(primtrk->length());
	  m_pchisq0FW->Fill(chisq0);
	  m_pchisq1FW->Fill(chisq1);

// these are for TPC & FTPC
	  m_ppT_eta_recFW->Fill(eta,lmevpt);
	  m_primtrk_xf_yfFW->Fill(primtrk->detectorInfo()->firstPoint().x(),
				  primtrk->detectorInfo()->firstPoint().y());
	  m_peta_trklengthFW->Fill(eta,primtrk->length());
	  m_pnpoint_lengthFW->Fill(primtrk->length(),
				   Float_t(primtrk->detectorInfo()->numberOfPoints()));
	  m_pfpoint_lengthFW->Fill(primtrk->length(),
				   Float_t(primtrk->fitTraits().numberOfFitPoints()));
	}
*/
      }
    }
    m_primtrk_good->Fill(cnttrkg);
    m_primtrk_good_sm->Fill(cnttrkg);
  }
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistGen() {

  //  StEvent does not have data corresponding to the DST particle table
  //  so this method is not used in StEventQAMaker.  However, this
  //  information can be found from StMcEvent.

  if (Debug()) cout << " *** in StEventQAMaker - filling particle histograms " << endl;

}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistPID() {

  if (Debug()) cout << " *** in StEventQAMaker - filling dE/dx histograms " << endl;

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
	if (dedxPidTr->detector() == kTpcId) {
	  m_ndedxT->Fill(ndedx);
	  m_dedx0T->Fill(dedx);
	  m_dedx1T->Fill(error);
	  if (ndedx > 15) {
	    m_p_dedx_rec->Fill((float)(p),(float)(dedx*1.e6));
	  }
	}
	if (dedxPidTr->detector() == kFtpcWestId) {
	  m_ndedxFW->Fill(ndedx);
	  m_dedx0FW->Fill(dedx);
	  m_dedx1FW->Fill(error);
	}
	if (dedxPidTr->detector() == kFtpcEastId) {
	  m_ndedxFE->Fill(ndedx);
	  m_dedx0FE->Fill(dedx);
	  m_dedx1FE->Fill(error);
	}
      }
    }
  }
  m_ndedxr->Fill(cntrows);
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistVertex() {

  if (Debug()) cout << " *** in StEventQAMaker - filling vertex histograms " << endl;

  Float_t m_prmass2 = (proton_mass_c2*proton_mass_c2);
  Float_t m_pimass2 = (pion_minus_mass_c2*pion_minus_mass_c2);
  Float_t m_lamass2 = (lambda_mass_c2*lambda_mass_c2);

  // primary vertex
  StPrimaryVertex *primVtx = event->primaryVertex();
  UInt_t daughters=0;
  UInt_t currentNumber=0;

  if (primVtx) {
    for (UInt_t v=0; v<event->numberOfPrimaryVertices(); v++) {
      currentNumber = event->primaryVertex(v)->numberOfDaughters();
      if (currentNumber > daughters) {
	daughters = currentNumber;
	primVtx = event->primaryVertex(v);
      }
    }

    float z_svt = 999.;
    float z_tpc = -999.;
    for (UInt_t j=0; j<event->numberOfPrimaryVertices(); j++) {
      StPrimaryVertex *aPrimVtx = event->primaryVertex(j);

      if (aPrimVtx->flag() == 201) z_svt = aPrimVtx->position().z();
      if (aPrimVtx->flag() == 101) z_tpc = aPrimVtx->position().z();

      if (aPrimVtx == primVtx) {
	m_pv_vtxid->Fill(primVtx->type());
	if (!isnan(double(primVtx->position().x())))
	  m_pv_x->Fill(primVtx->position().x());
	if (!isnan(double(primVtx->position().y())))
	  m_pv_y->Fill(primVtx->position().y());
	if (!isnan(double(primVtx->position().z())))
	  m_pv_z->Fill(primVtx->position().z());
	m_pv_pchi2->Fill(primVtx->chiSquared());
	m_pv_r->Fill(primVtx->position().x()*primVtx->position().x() +
		     primVtx->position().y()*primVtx->position().y());
      }
      else {
	m_v_vtxid->Fill(aPrimVtx->type());
	if (!isnan(double(aPrimVtx->position().x())))
	  m_v_x->Fill(aPrimVtx->position().x());     
	if (!isnan(double(aPrimVtx->position().y())))
	  m_v_y->Fill(aPrimVtx->position().y());     
	if (!isnan(double(aPrimVtx->position().z())))
	  m_v_z->Fill(aPrimVtx->position().z());     
	m_v_pchi2->Fill(aPrimVtx->chiSquared());
	m_v_r->Fill(aPrimVtx->position().x()*aPrimVtx->position().x() +
		    aPrimVtx->position().y()*aPrimVtx->position().y());
      }
    }
    m_vtx_z->Fill(z_tpc-z_svt);
  }

  // V0 vertices
  if (Debug()) cout << " *** in StEventQAMaker - filling dst_v0_vertex histograms " << endl;

  StSPtrVecV0Vertex &v0Vtx = event->v0Vertices();
  m_v0->Fill(v0Vtx.size());

  for (UInt_t k=0; k<v0Vtx.size(); k++) {
    StV0Vertex *v0 = v0Vtx[k];
    if (v0) {
      Float_t e1a = pow(abs(v0->momentumOfDaughter(positive)),2);
      Float_t e2 = pow(abs(v0->momentumOfDaughter(negative)),2);
      Float_t e1 = e1a + m_prmass2;
      e2 += m_pimass2;
      e1 = TMath::Sqrt(e1);
      e2 = TMath::Sqrt(e2);
      Float_t p = pow(abs(v0->momentum()),2);
      Float_t inv_mass_la = TMath::Sqrt((e1+e2)*(e1+e2) - p);
      e1 = e1a + m_pimass2;
      e1 = TMath::Sqrt(e1);
      Float_t inv_mass_k0 = TMath::Sqrt((e1+e2)*(e1+e2) - p);

      m_ev0_lama_hist->Fill(inv_mass_la);
      m_ev0_k0ma_hist->Fill(inv_mass_k0);

      m_v_vtxid->Fill(v0->type());
      if (!isnan(double(v0->position().x())))
	m_v_x->Fill(v0->position().x());     
      if (!isnan(double(v0->position().y())))
	m_v_y->Fill(v0->position().y());     
      if (!isnan(double(v0->position().z())))
	m_v_z->Fill(v0->position().z());     
      m_v_pchi2->Fill(v0->chiSquared());
      m_v_r->Fill(v0->position().x()*v0->position().x() +
		  v0->position().y()*v0->position().y());
    }
  }

  // Xi vertices
  if (Debug()) cout << 
   " *** in StEventQAMaker - filling dst_xi_vertex histograms " << endl;

  StSPtrVecXiVertex &xiVtx = event->xiVertices();
  m_xi_tot->Fill(xiVtx.size());

  for (UInt_t l=0; l<xiVtx.size(); l++) {
    StXiVertex *xi = xiVtx[l];
    if (xi) {
      const StThreeVectorF& pMom = xi->momentumOfBachelor();
      StThreeVectorF lMom = xi->momentumOfV0();
      StThreeVectorF xMom = lMom + pMom;
      Float_t pP2 = pMom.mag2();
      Float_t pL2 = lMom.mag2();
      Float_t pX2 = xMom.mag2();
      Float_t epi = sqrt(pP2 + m_pimass2);
      Float_t ela = sqrt(pL2 + m_lamass2);
      Float_t eXi = ela + epi;
      Float_t inv_mass_xi = sqrt(eXi*eXi - pX2);

      m_xi_ma_hist->Fill(inv_mass_xi);

      m_v_vtxid->Fill(xi->type());
      if (!isnan(double(xi->position().x())))
	m_v_x->Fill(xi->position().x());     
      if (!isnan(double(xi->position().y())))
	m_v_y->Fill(xi->position().y());     
      if (!isnan(double(xi->position().z())))
	m_v_z->Fill(xi->position().z());     
      m_v_pchi2->Fill(xi->chiSquared());
      m_v_r->Fill(xi->position().x()*xi->position().x() +
		  xi->position().y()*xi->position().y());
    }
  }

  // Kink vertices
  if (Debug()) cout << " *** in StEventQAMaker - filling kink histograms " << endl;

  StSPtrVecKinkVertex &kinkVtx = event->kinkVertices();
  m_kink_tot->Fill(kinkVtx.size());

  for (UInt_t m=0; m<kinkVtx.size(); m++) {
    StKinkVertex *kink = kinkVtx[m];
    if (kink) {
      //m_v_detid->Fill(kink->det_id); 
      m_v_vtxid->Fill(kink->type());
      if (!isnan(double(kink->position().x())))
	m_v_x->Fill(kink->position().x());     
      if (!isnan(double(kink->position().y())))
	m_v_y->Fill(kink->position().y());     
      if (!isnan(double(kink->position().z())))
	m_v_z->Fill(kink->position().z());     
      m_v_pchi2->Fill(kink->chiSquared());
      m_v_r->Fill(kink->position().x()*kink->position().x() +
		  kink->position().y()*kink->position().y());
    }
  }

  UInt_t cntrows = 0;
  cntrows = event->numberOfPrimaryVertices() + v0Vtx.size() +
            xiVtx.size() + kinkVtx.size(); //this gives 3 less than the DSTs!!
                                           //->needs to be fixed !!!
  m_v_num->Fill(cntrows);
  m_v_num_sm->Fill(cntrows);
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistPoint() {

  if (Debug()) cout << " *** in StEventQAMaker - filling point histograms " << endl;

  StTpcHitCollection *tpcHits = event->tpcHitCollection();
  StSvtHitCollection *svtHits = event->svtHitCollection();
  StFtpcHitCollection *ftpcHits = event->ftpcHitCollection();
  StSsdHitCollection *ssdHits = event->ssdHitCollection();

  ULong_t totalHits = 0;
  ULong_t ftpcHitsE = 0;
  ULong_t ftpcHitsW = 0;

  if (tpcHits) {
    // z-dist of hits
    for (UInt_t i=0; i<24; i++)
      for (UInt_t j=0; j<45; j++)
	for (UInt_t k=0; k<tpcHits->sector(i)->padrow(j)->hits().size(); k++)
	  m_z_hits->Fill(tpcHits->sector(i)->padrow(j)->hits()[k]->position().z());
    m_pnt_tpc->Fill(tpcHits->numberOfHits());
    totalHits += tpcHits->numberOfHits();
  }
  if (svtHits) {
    m_pnt_svt->Fill(svtHits->numberOfHits());
    totalHits += svtHits->numberOfHits();
  }
  if (ftpcHits) {
    // StFtpcHitCollection doesn't differentiate between W and E FTPCs
    // so it is up to the user to check this via plane number -CPL
    for (UInt_t i=0; i<20; i++) {
      if (i<10)
	ftpcHitsW += ftpcHits->plane(i)->numberOfHits();
      else
	ftpcHitsE += ftpcHits->plane(i)->numberOfHits();
    }
    m_pnt_ftpcW->Fill(ftpcHitsW);
    m_pnt_ftpcE->Fill(ftpcHitsE);
    totalHits += ftpcHits->numberOfHits();
  }
  if (ssdHits) {
    m_pnt_ssd->Fill(ssdHits->numberOfHits());
    totalHits += ssdHits->numberOfHits();
  }
  m_pnt_tot->Fill(totalHits);
  m_pnt_tot_med->Fill(totalHits);
  m_pnt_tot_sm->Fill(totalHits);
}

//_____________________________________________________________________________
void StEventQAMaker::MakeHistRich() {

  if (Debug()) cout << " *** in StEventQAMaker - filling Rich histograms " << endl;

  if (event->softwareMonitor()->rich())
    m_rich_tot->Fill(event->softwareMonitor()->rich()->mult_rich_tot);
}

//_____________________________________________________________________________

void StEventQAMaker::MakeHistEval() {

  // requires StMcEvent
  if (Debug()) cout << " *** in StEventQAMaker - filling Eval histograms " << endl;

}

//_____________________________________________________________________________
