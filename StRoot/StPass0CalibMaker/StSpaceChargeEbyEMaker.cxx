//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StSpaceChargeEbyEMaker performs event-by-event determination         //
// of the space charge correction for tracks, and sets it for           //
// the next event.                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "StSpaceChargeEbyEMaker.h"
#include "StDbUtilities/StMagUtilities.h"
#include "StEventTypes.h"
#include "StMessMgr.h"
#include "StTpcDb/StTpcDb.h"
#include "StTpcDb/StTpcDbMaker.h"
#include "St_db_Maker/St_db_Maker.h"
#include "tables/St_spaceChargeCor_Table.h"
#include "StEvent/StDcaGeometry.h"
#include "TUnixTime.h"

#include "TFile.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TNtuple.h"

// Histogram ranges:
const int   SCN = 80;
const float SCL = -0.015;
const float SCH =  0.025;

const int   DCN = 125;
const float DCL = -2.5;
const float DCH =  2.5;

const int   PHN = 72;
const float PI2 = TMath::TwoPi();

const int   EVN = 1024;

const int   ZN = 60;
const float ZL = -150.;
const float ZH = 150.;

const int   GN = 150;
const float GL = -0.3;
const float GH = 1.2;
const int   GZN = 17;
const float GZL = -200.;
const float GZH = 225.;

const float ZGGRID = 208.707;

static TF1 ga1("ga1","[0]*exp(-0.5*(x-[1])*(x-[1])/([2]*[2]))");
static TF1 ln1("ln1","[0]+((208.707-abs(x))*[1]/100.0)",-150.,150.);
//static TF1 ln1("ln1","[0]+((350.0-abs(x))*[1]/100.0)",-150.,150.);

ClassImp(StSpaceChargeEbyEMaker)
  
//_____________________________________________________________________________
StSpaceChargeEbyEMaker::StSpaceChargeEbyEMaker(const char *name):StMaker(name),
    event(0), Calibmode(kFALSE),
    PrePassmode(kFALSE), PrePassdone(kFALSE), QAmode(kFALSE), doNtuple(kFALSE),
    doReset(kTRUE), doGaps(kFALSE), inGapRow(0),
    m_ExB(0),
    scehist(0), timehist(0), myhist(0), myhistN(0), myhistP(0),
    myhistE(0), myhistW(0), dczhist(0), dcehist(0), dcphist(0),
    dcahist(0), dcahistN(0), dcahistP(0), dcahistE(0), dcahistW(0),
    gapZhist(0), gapZhistneg(0), gapZhistpos(0), ntup(0) {

  HN=96;  // max events used, cannot exceed 96 used in header file
  MINTRACKS=1500;
  //SCALER_ERROR = 0.0006; // by eye from hist: SCvsZDCEpW.gif (liberal)
  SCALER_ERROR = 0.0007; // by RMS from hist: SCvsZDCX.gif (liberal)

  // MAXDIFFE is maximum different in sc from last ebye sc
  MAXDIFFE =   SCALER_ERROR;
  // MAXDIFFA is maximum different in sc from last scaler sc
  MAXDIFFA = 2*SCALER_ERROR; // should be about equal to SCALER_ERROR, no?
           // Present uncetainties with scalers demands greater tolerance

  runid = 0;
  memset(evts,0,HN*sizeof(int));
  memset(times,0,HN*sizeof(int));
  memset(evtstbin,0,HN*sizeof(float));
  evtsnow = 0;

  SetMode(0); // default is mode 0 (no QA, no PrePass)
  //DoQAmode(); // For testing

  schist = new TH1F("SpCh","Space Charge",80,SCL,SCH);
  schist->SetDirectory(0);
  for (int i=0;i<HN;i++){
    schists[i] = new TH1F(Form("SpCh%d",i),"Space Charge",80,SCL,SCH);
    schists[i]->SetDirectory(0);
  }
}
//_____________________________________________________________________________
StSpaceChargeEbyEMaker::~StSpaceChargeEbyEMaker() {
  delete schist;
  for (int i=0;i<HN;i++) delete schists[i];
}
//_____________________________________________________________________________
Int_t StSpaceChargeEbyEMaker::Finish() {

  if (evt > 0) {
    if (PrePassmode) {
      if (PrePassdone) WriteTableToFile();
      else gMessMgr->Warning("StSpaceChargeEbyEMaker: NO SC => NO OUTPUT");
    }
    if ((!Calibmode)&&(!PrePassdone)) EvalCalib();
    WriteQAHists();
  } else {
    gMessMgr->Warning("StSpaceChargeEbyEMaker: NO EVENTS => NO OUTPUT");
  }

  return kStOk;
}
//_____________________________________________________________________________
Int_t StSpaceChargeEbyEMaker::Init() {

  // Use mode to set switches:
  switch (GetMode()) {
    case (1) : DoQAmode(); break;
    case (2) : DoPrePassmode(); break;
    case (3) : DoPrePassmode(); DoQAmode();  break;
    case (4) : DoCalib();  break;
    case (10): DoNtuple(); break;
    case (11): DoNtuple(); DontReset(); break;
    case (12): DoNtuple(); DoQAmode(); break;
    case (13): DoNtuple(); DontReset(); DoQAmode(); break;
    default  : {}
  }

  if (Calibmode) doReset = kFALSE;

  evt=0;
  oldevt=1;
  lastsc=0.;
  curhist=0;
  lasttime=0;
  did_auto=kTRUE;
  InitQAHists();
  if (QAmode) gMessMgr->Info("StSpaceChargeEbyEMaker: Initializing");
  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StSpaceChargeEbyEMaker::Make() {

  // On very first event
  if (PrePassmode && (tabname.Length() == 0)) SetTableName();
  
  // Get instance of StMagUtilities
  m_ExB = StMagUtilities::Instance();
  if (!m_ExB) {
    TDataSet *RunLog = GetDataBase("RunLog");
    if (!RunLog) gMessMgr->Warning("StSpaceChargeEbyEMaker: No RunLog found.");
    m_ExB = new StMagUtilities(gStTpcDb,RunLog,(kSpaceChargeR2 | kGridLeak));
  }
  lastsc = m_ExB->CurrentSpaceChargeR2();

  // Get StEvent and related info, determine if things are OK
  event = (StEvent*) GetInputDS("StEvent");
  if (!event) {
    gMessMgr->Warning("StSpaceChargeEbyEMaker: no StEvent; skipping event.");
    return kStWarn;
  }
  // Get runinfo, determine if the magnetic field is nonzero 
  // EbyE maker not currently able to handle zero B field
  runinfo = event->runInfo();
  if ((!runinfo) || (runinfo->magneticField() == 0)) {
    gMessMgr->Error("StSpaceChargeEbyEMaker: cannot run due to zero or unknown mag field!");
    // Look for any errant zero field SC values as a warning
    //   for processing even without EbyE
    if ((lastsc != 0) && (runinfo) && (runinfo->magneticField() == 0))
      gMessMgr->Warning() << "BE AWARE THAT A NONZERO VALUE OF SPACECHARGE\n"
        << "    WAS RETURNED BY DB CALL!\n    (could be a local DB file or"
        << " in the actual database)" << endm;
    return kStFatal;
  }

  // Select the highest ranked vertex
  StPrimaryVertex* pvtx = event->primaryVertex();
  if (!pvtx || pvtx->numberOfDaughters()<5 || pvtx->numMatchesWithBEMC()<1) return kStOk;
  StSPtrVecTrackNode& theNodes = event->trackNodes();
  unsigned int nnodes = theNodes.size();
  if (!nnodes) return kStOk;

  // Store and setup event-wise info
  evt++;
  int thistime = event->time();
  if (lasttime) {
    timehist->SetBinContent(evt,thistime-lasttime);
  } else {
    runid = event->runId();
  }
  if (doReset) {
    if (evt>1) curhist = imodHN(curhist+1);
    schists[curhist]->Reset();
    if (doGaps) {
      gapZhist->Reset();
      gapZhistpos->Reset();
      gapZhistneg->Reset();
    }
  } else {
    // Do not reset ntuple in calib mode
    if (doNtuple && !Calibmode) ntup->Reset();
  }

  // Keep time and event number
  times[curhist] = thistime;
  evts[curhist]=evt;

  // Keep track of # of events in the same time bin
  if (thistime == lasttime) evtsnow++;
  else evtsnow = 1;
  evtstbin[curhist] = evtsnow;

  if (QAmode) {
    gMessMgr->Info()
      << "used (for this event) SpaceCharge = "
      << lastsc << " (" << thistime << ")" << endm;
    gMessMgr->Info()
      << "zdc west+east = "
      << runinfo->zdcWestRate()+runinfo->zdcEastRate() << endm;
    gMessMgr->Info()
      << "zdc coincidence = "
      << runinfo->zdcCoincidenceRate() << endm;
  }

  // Fill the StEvent information for the SpaceCharge used in this event
  runinfo->setSpaceCharge(lastsc);
  runinfo->setSpaceChargeCorrectionMode(m_ExB->GetSpaceChargeMode());
  if (!inGapRow) {
    if (runinfo->runId() > 10000000) inGapRow = 13; // Run 9+
    else if (runinfo->runId() > 0) inGapRow = 12; // Run exists
    // else undefined
  }

  // Track loop
  unsigned int i,j,k;
  StThreeVectorD ooo = pvtx->position();

  for (i=0; i<nnodes; i++) {
      for (j=0; j<theNodes[i]->entries(global); j++) {
        StTrack* tri = theNodes[i]->track(global,j);
        if (!tri) continue;

          const StTrackTopologyMap& map = tri->topologyMap();
          //if (! map.trackTpcOnly()) continue;
          if (! map.hasHitInDetector(kTpcId)) continue;
          // Multiple silicon hits destroy sDCA <-> SpaceCharge correlation,
          // and single hit in SVT is unreliable. Only good config is NO SVT!
          if (map.hasHitInDetector(kSvtId)) continue;
          if (map.numberOfHits(kTpcId) < 25) continue;
          StTrackGeometry* triGeom = tri->geometry();

          StThreeVectorF xvec = triGeom->origin();
          if (!(xvec.x() || xvec.y() || xvec.z())) continue;
          StThreeVectorF pvec = triGeom->momentum();
          if (!(pvec.x() || pvec.y())) continue;

          float oldPt = pvec.perp();
          if (oldPt < 0.0001) continue;

	  int e_or_w = 0; // east is -1, west is +1
	  if (pvec.z() * xvec.z() > 0) e_or_w = ( (xvec.z() > 0) ? 1 : -1 );

          StPhysicalHelixD hh = triGeom->helix();

          Float_t eta=pvec.pseudoRapidity();
          //Float_t DCA=hh.geometricSignedDistance(0,0); // for testing only
          Float_t DCA3=-999;
          Float_t DCA2=-999;
          StDcaGeometry* triDcaGeom = ((StGlobalTrack*) tri)->dcaGeometry();
          if (triDcaGeom) {
            StPhysicalHelixD dcahh = triDcaGeom->helix();
            DCA3 = dcahh.distance(ooo,kFALSE);
            DCA2 = dcahh.geometricSignedDistance(ooo.x(),ooo.y());
          } else {
            DCA3 = hh.distance(ooo,kFALSE);
            DCA2 = hh.geometricSignedDistance(ooo.x(),ooo.y());
          }
          if (DCA3 > 4) continue; // cut out pileup tracks!
          Int_t ch = (int) triGeom->charge();

          Int_t PCT = 0;
          Float_t rerrors[64];
          Float_t rphierrors[64];
          memset(rerrors,64*sizeof(Float_t),0);
          memset(rphierrors,64*sizeof(Float_t),0);
          StPtrVecHit& hits = tri->detectorInfo()->hits();
          for (k=0;k<hits.size();k++) {
            StHit* hit = hits[k];
            unsigned int maskpos = 0;
            switch (hit->detector()) {
              case (kTpcId) :
                if ((hit->position().z() > 1 && ((StTpcHit*) hit)->sector() > 12) ||
                    (hit->position().z() <-1 && ((StTpcHit*) hit)->sector() < 13)) PCT++;
                maskpos = 7 + ((StTpcHit*) hit)->padrow(); break;
              case (kSvtId) :
                maskpos = ((StSvtHit*) hit)->layer(); break;
              case (kSsdId) :
                maskpos = 7; break;
              default :
                maskpos = 0;
            }
            if (maskpos) {
              StThreeVectorF herrVec = hit->positionError();
              Float_t herr = herrVec.perp();
              rerrors[maskpos] = herr;
              rphierrors[maskpos] = herr;
            }
          }
          if (PCT) continue; // Track has post-crossing hits
          
          Float_t space = 10000.;
          if (!(m_ExB->PredictSpaceChargeDistortion(ch,oldPt,ooo.z(),
	  //   eta,DCA2,map.data(0),map.data(1),space))) continue;
	     eta,DCA2,map.data(0),map.data(1),rerrors,rphierrors,space))) continue;

	  space += lastsc;  // Assumes additive linearity of space charge!
	  schists[curhist]->Fill(space);
          FillQAHists(DCA2,space,ch,hh,e_or_w);


          if ((doGaps) &&
              (map.hasHitInRow(kTpcId,inGapRow))&&(map.hasHitInRow(kTpcId,14)) &&
              (map.hasHitInRow(kTpcId,11))&&(map.hasHitInRow(kTpcId,15)) &&
              (e_or_w!=0) && (TMath::Abs(ch)==1) && (oldPt>0.3))
            FillGapHists(tri,hh,e_or_w,ch);

      } // loop over j tracks
  } // loop over i Nodes


  ntrks[curhist] = schists[curhist]->Integral();

  // Wrap it up and make a decision
  int result = DecideSpaceCharge(thistime);

  if (doGaps) DetermineGaps();
  if (doNtuple) {
      static float X[37];
      static float ntent = 0.0;
      static float nttrk = 0.0;

      if (ntent == 0.0) for (i=0; i<37; i++) X[i] = 0.0;
      ntent++;  // # entries since last reset, including this one
      float last_nttrk = nttrk;
      nttrk = ntrks[curhist];  // # tracks since last reset, including these
      float s0 = ( nttrk ? last_nttrk / nttrk : 0 );
      float s1 = 1.0 - s0; // fraction of tracks from current event

      if (QAmode) {
        gMessMgr->Info() << "reset = " << doReset << endm;
        gMessMgr->Info() << "nevts = " << ntent << endm;
        gMessMgr->Info() << "ntrks = " << nttrk << endm;
      }

      float ee;
      int fbin = evt + 1 - ((int) ntent);

      X[0]  = sc;
      X[1]  = FindPeak(dcehist->ProjectionY("_dy",fbin,evt),ee);
      X[2]  = s0*X[2] + s1*runinfo->zdcCoincidenceRate();
      X[3]  = s0*X[3] + s1*runinfo->zdcWestRate();
      X[4]  = s0*X[4] + s1*runinfo->zdcEastRate();
      X[5]  = s0*X[5] + s1*runinfo->bbcCoincidenceRate();
      X[6]  = s0*X[6] + s1*runinfo->bbcWestRate();
      X[7]  = s0*X[7] + s1*runinfo->bbcEastRate();
      X[8]  = s0*X[8] + s1*runinfo->bbcBlueBackgroundRate();
      X[9]  = s0*X[9] + s1*runinfo->bbcYellowBackgroundRate();
      X[10] = s0*X[10] + s1*runinfo->initialBeamIntensity(blue);   // west-bound
      X[11] = s0*X[11] + s1*runinfo->initialBeamIntensity(yellow); // east-bound
      X[12] = runinfo->beamFillNumber(blue);
      X[13] = runinfo->magneticField();
      X[14] = event->runId();
      X[15] = event->id();
      //X[16] = sceast;
      //X[17] = scwest;
      if ((QAmode) && (evt <= EVN)) {
        X[16] = FindPeak(dcahistN->ProjectionZ("_dnz",fbin,evt,1,PHN),ee);
        X[17] = FindPeak(dcahistP->ProjectionZ("_dpz",fbin,evt,1,PHN),ee);
        X[18] = FindPeak(dcahistE->ProjectionZ("_dez",fbin,evt,1,PHN),ee);
        X[19] = FindPeak(dcahistW->ProjectionZ("_dwz",fbin,evt,1,PHN),ee);
      }
      X[20] = gapZfitslope;
      X[21] = gapZfitintercept;
      X[22] = gapZdivslope;
      X[23] = gapZfitslopeneg;
      X[24] = gapZfitinterceptneg;
      X[25] = gapZdivslopeneg;
      X[26] = gapZfitslopepos;
      X[27] = gapZfitinterceptpos;
      X[28] = gapZdivslopepos;
      X[29] = gapZfitslopeeast;
      X[30] = gapZfitintercepteast;
      X[31] = gapZdivslopeeast;
      X[32] = gapZfitslopewest;
      X[33] = gapZfitinterceptwest;
      X[34] = gapZdivslopewest;
      X[35] = s0*X[35] + s1*runinfo->spaceCharge();
      X[36] = s0*X[36] + s1*((float) (runinfo->spaceChargeCorrectionMode()));
	      
      // In calib mode, only fill when doReset (we found an sc)
      if (doReset || !Calibmode) ntup->Fill(X);

      if (doReset) {ntent = 0.0; nttrk = 0.0; }

  }
	  
  lasttime = thistime;

  return result;
}
//_____________________________________________________________________________
Int_t StSpaceChargeEbyEMaker::DecideSpaceCharge(int time) {

  // curhist has only this event
  // curhist-1 has past two events...
  // curhist-x has past (x+1) events
  // curhist-(HN-1) == curhist+1 has past HN events

  Bool_t QAout = QAmode || PrePassmode;
  Bool_t do_auto = kTRUE;
  Bool_t few_stats = kTRUE;
  Bool_t large_err = kTRUE;
  Bool_t large_dif = kTRUE;

  // Cuts on difference from previous sc measure:
  // If last event was auto, use MAXDIFFA, else use between MAXDIFFE & MAXDIFFA
  //   scaled by oldness of previous sc measure (curhist-1)
  float maxdiff,dif=0;
  if (did_auto)
    maxdiff = MAXDIFFA;
  else
    maxdiff = MAXDIFFA - (MAXDIFFA-MAXDIFFE)*oldness(imodHN(curhist-1));

  // More than 30 seconds since last used event? Forget it...
  int timedif = time-lasttime;
  if (QAout) {
    gMessMgr->Info() << "time since last event = "
      << timedif << endm;
    gMessMgr->Info() << "curhist = "
      << curhist << endm;
  }
  float ntrkstot = 0; // running sum using oldness scale factor
  Bool_t decideFromData = ((PrePassmode) || (Calibmode) || (lasttime==0) || (timedif < 30));
  if (decideFromData) {
  
    int isc;
    static int iscMax = 1;  // use only one hist for calib mode, and...
    if (!Calibmode && iscMax<HN) iscMax = curhist+1; // don't use uninitialized
    for (isc=0; isc<iscMax; isc++) {
      int i = imodHN(curhist-isc);
      ntrkstot += ntrks[i] * oldness(i);
      if (QAout) {
	if (!isc) gMessMgr->Info("Building with: i, ni, oi, nt:");
	gMessMgr->Info() << "Building with: " << i << ", "
	  << ntrks[i] << ", " << oldness(i) << ", " << ntrkstot << endm;
      }

      // Too little data collected? Keep trying...
      few_stats = ntrkstot < MINTRACKS;
      if (!few_stats) {
	BuildHist(i);
	FindSpaceCharge();
	if (QAout) gMessMgr->Info()
	  << "sc = " << sc << " +/- " << esc << endm;
	large_err = (esc == 0) || (esc > SCALER_ERROR);
	if (!large_err) {
	  if (PrePassmode) { do_auto=kFALSE; break; }
	  // otherwise, check for big jumps
	  dif = TMath::Abs(sc-lastsc);
	  large_dif = dif > maxdiff;
	  if (!large_dif || Calibmode) {
	    oldevt = evts[i];
	    do_auto=kFALSE;
	    break;
	  }
	}
      }

      // shouldn't need to go past oldest event previously used?
      // tough to know - allow for now as of 11 Jan 2008
      // if (evts[i] <= oldevt)  break;
    }
    if (QAout && (isc == HN)) gMessMgr->Info()
      << "STORED DATA EXHAUSTED: "
      << HN << " events" << endm;
  }

  did_auto = do_auto;

  // In normal  mode, do_auto decides whether to use automatic SC from DB
  // In PrePass mode, do_auto decides when we're ready to stop
  // In Calib   mode, do_auto decides when to save entries and reset

  if (do_auto) {
    if (QAout && decideFromData) {
      if (few_stats) gMessMgr->Info()
        << "(RECENT) STATS TOO FEW: "
        << ntrkstot << " (" << MINTRACKS << ")" << endm;
      else if (large_err) gMessMgr->Info()
        << "FIT ERROR TOO LARGE: "
        << esc << " (" << SCALER_ERROR << ")" << endm;
      else if (large_dif) gMessMgr->Info()
        << "DIFFERENCE TOO LARGE: "
        << dif << " (" << maxdiff << ")" << endm;
    }
    gMessMgr->Info("using auto SpaceCharge");
    if (Calibmode) doReset = kFALSE;
    else m_ExB->AutoSpaceChargeR2();
  } else {
    gMessMgr->Info() << "using SpaceCharge = "
      << sc << " +/- " << esc << " (" << time << ")" << endm;
    scehist->SetBinContent(evt,sc);
    scehist->SetBinError(evt,esc);
    if (PrePassmode) {
      PrePassdone = kTRUE;
      return kStStop; // We're happy! Let's stop!
    }
    if (Calibmode) {
      doReset = kTRUE;
      return kStStop; // We're happy? Let's stop!
    }
    else m_ExB->ManualSpaceChargeR2(sc,m_ExB->CurrentSpaceChargeEWRatio());
  }
  return kStOk;
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::FindSpaceCharge() {
  esc = 0.;
  double res = FindPeak(schist,esc);
  if (res > -500.) sc = res;
}
//_____________________________________________________________________________
double StSpaceChargeEbyEMaker::FindPeak(TH1* hist,float& pkwidth) {

  pkwidth = 0.;
  if (hist->Integral() < 100.0) return -998.;
  double mn = hist->GetMean();
  double rms = TMath::Abs(hist->GetRMS());
  mn *= 1.1; rms *= 1.5;
  double lr = mn-rms;
  double ur = mn+rms;
  double pmax = hist->GetMaximum();
  ga1.SetParameters(pmax,mn,rms*0.5);
  ga1.SetRange(lr,ur);
  int fitResult = hist->Fit(&ga1,"LLR0Q");
  if (fitResult != 0) return -999.;
  pkwidth = TMath::Abs(ga1.GetParError(1));
  return ga1.GetParameter(1);

}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::InitQAHists() {

  scehist  = new TH1F("SpcChgEvt","SpaceCharge fit vs. Event",
		      EVN,0.,EVN);
  timehist = new TH1F("EvtTime","Event Times",
		      EVN,0.,EVN);
  dcehist  = new TH2F("DcaEve","psDCA vs. Event",
		      EVN,0.,EVN,DCN,DCL,DCH);
  dcphist  = new TH3F("DcaPhi","psDCA vs. Phi",
		      PHN,0,PI2,DCN,DCL,DCH,(QAmode ? 3 : 1),-1.5,1.5);

  AddHist(scehist);
  AddHist(timehist);
  AddHist(dcehist);
  AddHist(dcphist);

  if (QAmode) {
    myhist   = new TH3F("SpcEvt","SpaceCharge vs. Phi vs. Event",
			EVN,0.,EVN,PHN,0,PI2,SCN,SCL,SCH);
    dcahist  = new TH3F("DcaEvt","psDCA vs. Phi vs. Event",
			EVN,0.,EVN,PHN,0,PI2,DCN,DCL,DCH);
    dczhist  = new TH2F("DcaZ","psDCA vs. Z",
			//80,-200,200,250,-5.0,5.0);
			ZN,ZL,ZH,DCN,DCL,DCH);
    myhistN  = new TH3F("SpcEvtN","SpaceCharge vs. Phi vs. Event Neg",
			EVN,0.,EVN,PHN,0,PI2,SCN,SCL,SCH);
    myhistP  = new TH3F("SpcEvtP","SpaceCharge vs. Phi vs. Event Pos",
			EVN,0.,EVN,PHN,0,PI2,SCN,SCL,SCH);
    myhistE  = new TH3F("SpcEvtE","SpaceCharge vs. Phi vs. Event East",
			EVN,0.,EVN,PHN,0,PI2,SCN,SCL,SCH);
    myhistW  = new TH3F("SpcEvtW","SpaceCharge vs. Phi vs. Event West",
			EVN,0.,EVN,PHN,0,PI2,SCN,SCL,SCH);
    dcahistN = new TH3F("DcaEvtN","psDCA vs. Phi vs. Event Neg",
			EVN,0.,EVN,PHN,0,PI2,DCN,DCL,DCH);
    dcahistP = new TH3F("DcaEvtP","psDCA vs. Phi vs. Event Pos",
			EVN,0.,EVN,PHN,0,PI2,DCN,DCL,DCH);
    dcahistE = new TH3F("DcaEvtE","psDCA vs. Phi vs. Event East",
			EVN,0.,EVN,PHN,0,PI2,DCN,DCL,DCH);
    dcahistW = new TH3F("DcaEvtW","psDCA vs. Phi vs. Event West",
			EVN,0.,EVN,PHN,0,PI2,DCN,DCL,DCH);
    AddHist(myhist);
    AddHist(dcahist);
    AddHist(dczhist);
    AddHist(myhistN);
    AddHist(myhistP);
    AddHist(myhistE);
    AddHist(myhistW);
    AddHist(dcahistN);
    AddHist(dcahistP);
    AddHist(dcahistE);
    AddHist(dcahistW);
  }

  if (doNtuple) ntup = new TNtuple("SC","Space Charge",
    "sc:dca:zdcx:zdcw:zdce:bbcx:bbcw:bbce:bbcbb:bbcyb:intb:inty:fill:mag:run:event:dcan:dcap:dcae:dcaw:gapf:gapi:gapd:gapfn:gapin:gapdn:gapfp:gapip:gapdp:gapfe:gapie:gapde:gapfw:gapiw:gapdw:usc:uscmode");
    //"sc:dca:zdcx:zdcw:zdce:bbcx:bbcw:bbce:bbcbb:bbcyb:intb:inty:fill:mag:run:event:sce:scw:dcae:dcaw:gapf:gapd:gapfn:gapdn:gapfp:gapdp:gapfe:gapde:gapfw:gapdw:usc:uscmode");

  if (doGaps) {
    gapZhist = new TH2F("Gaps","Gaps",GN,GL,GH,GZN,GZL,GZH);
    gapZhistneg = new TH2F("Gapsneg","Gaps Neg",GN,GL,GH,GZN,GZL,GZH);
    gapZhistpos = new TH2F("Gapspos","Gaps Pos",GN,GL,GH,GZN,GZL,GZH);
  }

}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::WriteQAHists() {
// Only if QAmode or doNtuple or doGaps

  if (!(QAmode || doNtuple || doGaps)) return;

  if (runid == 0) {
    gMessMgr->Error("StSpaceChargeEbyEMaker: No runid => No output");
    return;
  }

  const char* f1 = GetName();
  runid -= 1000000*(runid/1000000); // Remove the year, optional
  runid *= 100; // limits one run to 100 files!
  TString fname = "./hists";
  if (PrePassmode) fname.Append("Pre");
  gSystem->cd(fname.Data());
  while (f1) {
    fname = Form("Hist%d.root",runid);
    f1 = gSystem->Which(".",fname.Data());
    runid++;
  }

  TFile ff(fname.Data(),"RECREATE");
  if (QAmode) {
    myhist->Write();
    dcehist->Write();
    dcphist->Write();
    dcahist->Write();
    dczhist->Write();
    myhistN->Write();
    dcahistN->Write();
    myhistP->Write();
    dcahistP->Write();
    myhistE->Write();
    dcahistE->Write();
    myhistW->Write();
    dcahistW->Write();
    scehist->Write();
    timehist->Write();
  }
  if (doGaps) {
    gapZhist->Write();
    gapZhistneg->Write();
    gapZhistpos->Write();
  }
  if (doNtuple) ntup->Write();
  ff.Close();

  gMessMgr->Info() << "QA hists file: " << fname.Data() << endm;

  gSystem->cd("..");

}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::FillQAHists(float DCA, float space, int ch,
					 StPhysicalHelixD& hh, int e_or_w) {
  // Find a "Phi" for the track
  pairD pls = hh.pathLength(97.0);
  double pl = pls.first;
  if (TMath::Abs(pls.second) < TMath::Abs(pls.first)) pl = pls.second;
  StThreeVectorD hh_at_pl = hh.at(pl);
  float Phi = hh_at_pl.phi();
  while (Phi < 0) Phi += PI2;
  while (Phi >= TMath::TwoPi()) Phi -= PI2;
  
  // To pile all sectors atop each other:
  // while (Phi >= TMath::Pi()/6.) Phi -= TMath::Pi()/6.;

  float evtn = ((float) evt) - 0.5;
  dcehist->Fill(evtn,DCA);
  dcphist->Fill(Phi,DCA,(float) e_or_w);

  if (QAmode) {
    myhist->Fill(evtn,Phi,space);
    dcahist->Fill(evtn,Phi,DCA);
    if (ch > 0) {
      myhistP->Fill(evtn,Phi,space);
      dcahistP->Fill(evtn,Phi,DCA);
    } else {
      myhistN->Fill(evtn,Phi,space);
      dcahistN->Fill(evtn,Phi,DCA);
    }
    if (e_or_w > 0) {
      myhistW->Fill(evtn,Phi,space);
      dcahistW->Fill(evtn,Phi,DCA);
    } else if (e_or_w < 0) {
      myhistE->Fill(evtn,Phi,space);
      dcahistE->Fill(evtn,Phi,DCA);
    }
    if ((e_or_w != 0) && (TMath::Abs(hh.dipAngle()) < 0.05)) dczhist->Fill(hh_at_pl.z(),DCA);
  }
}
//_____________________________________________________________________________
int StSpaceChargeEbyEMaker::imodHN(int i) {
  // Keep index in bounds of circular queue
  return ( i >= HN ? imodHN(i-HN) : (i < 0 ? imodHN(i+HN) : i) );
}
//_____________________________________________________________________________
float StSpaceChargeEbyEMaker::oldness(int i, int j) {
  // Deterime how to treat relative "age" of event
  // In PrePassmode, earliest events are most important!
  static float decay_const = -0.12;
  //static float decay_const = -0.15;
  float s = 1.0;
  if (!PrePassmode) { // Weight newest the most (or evenly for PrePass)
    if (j<0) j = curhist;

    // Weight in sub-second intervals by # of events because
    // times have only 1 second granularity (best we can do).
    // Method assumes time-ordering of events, which is violated
    // perhaps only occasionally from DAQ.
    int k = i;
    while (k!=j) {
      k = imodHN(k+1);
      if (times[k] != times[i]) { k = imodHN(k-1); break; }
    }
    // # seconds + fraction of a second:
    float time_factor = (times[j]-times[i]) + (1.-(evtstbin[i]/evtstbin[k]));
    //float time_factor = (times[j]-times[i]);
    s = exp( decay_const * time_factor );
  }
  return s;
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::BuildHist(int i) {
  // Build up one histogram from several events
  schist->Reset();
  int isc = curhist;
  schist->Add(schists[isc],1.0);
  while (isc != i) {
    isc = imodHN(isc-1);
    schist->Add(schists[isc],oldness(isc));
  }
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::SetTableName() {
  // Problem caused if first event comes later in time than other events.
  // Solution: subtract 10 seconds...
  TUnixTime ux(GetDateTime(),1);
  ux+=-10;
  int date,time;
  ux.GetGTime(date,time);
  gMessMgr->Info() << "first event date = " << date << endm;
  gMessMgr->Info() << "first event time = " << time << endm;
  tabname = Form("./StarDb/Calibrations/rich/spaceChargeCorR2.%08d.%06d.C",date,time);
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::WriteTableToFile(){
  gMessMgr->Info() << "Writing new table to:\n  "
    << tabname.Data() << endm;
  TString dirname = gSystem->DirName(tabname.Data());
  TString estr = dirname;
  estr.Prepend("mkdir -p ");
  gSystem->Exec(estr.Data());
  if (gSystem->OpenDirectory(dirname.Data())==0) {
    if (gSystem->mkdir(dirname.Data())) {
      gMessMgr->Warning() << "Directory creation failed for:\n  " << dirname
	<< "\n  Putting table file in current directory" << endm;
      tabname.Remove(0,tabname.Last('/')).Prepend(".");
    }
  }
  ofstream *out = new ofstream(tabname.Data());
  SCTable()->SavePrimitive(*out,"");
  return;
}
//_____________________________________________________________________________
St_spaceChargeCor* StSpaceChargeEbyEMaker::SCTable() {
  St_spaceChargeCor* table = new St_spaceChargeCor("spaceChargeCorR2",1);
  spaceChargeCor_st* row = table->GetTable();
  row->fullFieldB = sc;
  row->halfFieldB = sc;
  row->zeroField  = (float) evt;
  row->halfFieldA = sc;
  row->fullFieldA = sc;
  row->satRate = 1.0;
  row->factor = 1.0;
  row->detector = 3;
  row->offset = 0;
  row->ewratio = m_ExB->CurrentSpaceChargeEWRatio();
  table->SetNRows(1);
  return table;
}
//_____________________________________________________________________________
float StSpaceChargeEbyEMaker::FakeAutoSpaceCharge() {
  // Use this to mimic what is done with the scalers, using your own code
  float zdcsum = runinfo->zdcWestRate()+runinfo->zdcEastRate();
  float sc = 6e-8 * zdcsum;
  
  return sc;
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::FillGapHists(StTrack* tri, StPhysicalHelixD& hh,
					  int e_or_w, int ch) {
  float fsign = ( event->runInfo()->magneticField() < 0 ? -1 : 1 );
  StPtrVecHit hts = tri->detectorInfo()->hits(kTpcId);
  float gap = 0.; float zgap = 0.; int ct=0;
  for (UInt_t ht=0; ht<hts.size(); ht++) {
    StTpcHit* hit = (StTpcHit*) hts[ht];
    UInt_t prow = hit->padrow();
    if ((prow != inGapRow) && (prow != 14)) continue;
    float gsign = ( prow == 14 ? -1 : 1 );
    const StThreeVectorF& hp = hit->position();

    // Avoid sector edges
    float hphi = hp.phi() + TMath::TwoPi();
    while (hphi > TMath::Pi()/12.) hphi -= TMath::Pi()/6.;
    if (TMath::Abs(hphi) > 0.75*TMath::Pi()/12.) break;

    if (inGapRow==13) zgap += (hp.z() / 7.595) * ( prow == 14 ? 2.2 : 5.395 ); // ~z at gap
    else if (inGapRow==12) zgap += (hp.z() / 12.795) * ( prow == 14 ? 7.4 : 5.395 ); // ~z at gap
    else return;

    // Measurement method described at:
    // http://drupal.star.bnl.gov/STAR/blog/genevb/2010/feb/21/gridleak-update-using-residuals-along-padrows
    Double_t residual = hh.geometricSignedDistance(hp.x(),hp.y());

    UInt_t sec = hit->sector();
    Double_t sector_angle = (TMath::Pi()/6.) * (sec < 13 ? 3 - sec : sec - 21);
    Double_t pathlen = hh.pathLength(hp.x(),hp.y());
    Double_t theta = TMath::ATan2(hh.cy(pathlen),hh.cx(pathlen)) - sector_angle;
    Double_t phi = hp.phi() - sector_angle;

    Double_t Eff1 = TMath::Cos(theta);
    Double_t Eff2 = 1;
    Double_t Eff3 = 0;

    Float_t x1[3],x2[3];
    x1[0] = hp.perp()*TMath::Sin(phi);
    x1[1] = hp.perp()*TMath::Cos(phi);
    x1[2] = hp.z();
    m_ExB->Undo3DGridLeakDistortion(x1,x2);
    Double_t dX = x2[0]-x1[0];
    if (TMath::Abs(dX) > 1e-20) {
      // warning: no available GridLeak calculation may lead
      //   to slightly different results
      Eff3 = gsign * ((x2[1]-x1[1])/dX) * TMath::Sin(theta);
      x1[0] = 0;
      m_ExB->Undo3DGridLeakDistortion(x1,x2);
      Eff2 = (x2[0]-x1[0])/dX;
    }

    Double_t DistortionX = Eff2 * residual / (Eff1 + Eff3);
 
    gap += fsign * gsign * DistortionX;
    ct++;
  }

  float abs_zgap = TMath::Abs(zgap);
  if ((ct==2) && (abs_zgap<200.0) && (abs_zgap>10.0)) {
     gapZhist->Fill(gap,zgap);
     if (ch==1) gapZhistpos->Fill(gap,zgap);
     else gapZhistneg->Fill(gap,zgap);
     if (abs_zgap<150 && abs_zgap>25) { // Restrict the Z range further
       // normalize at z=100cm from ggrid
       float gap_scaled = (gap * 100.0) / (ZGGRID - abs_zgap);
       //float gap_scaled = (gap * 100.0) / (350.0 - abs_zgap);
       float z_beyond = ZGGRID+1.0;
       gapZhist->Fill(gap_scaled,z_beyond);
       if (ch==1) gapZhistpos->Fill(gap_scaled,z_beyond);
       else gapZhistneg->Fill(gap_scaled,z_beyond);
     }
   }
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::DetermineGaps() {
  DetermineGapHelper(gapZhistneg,gapZfitslopeneg,gapZfitinterceptneg,gapZdivslopeneg);
  DetermineGapHelper(gapZhistpos,gapZfitslopepos,gapZfitinterceptpos,gapZdivslopepos);
  DetermineGapHelper(gapZhist,gapZfitslope,gapZfitintercept,gapZdivslope);
}
//_____________________________________________________________________________
void StSpaceChargeEbyEMaker::DetermineGapHelper(TH2F* hh,
      float& fitslope, float& fitintercept, float& divslope) {

  ga1.SetParameters(hh->GetEntries()/(16.*2.*10.),0.,0.1);
  hh->FitSlicesX(&ga1,1,0,20,"LL0Q");
  const char* hn = hh->GetName();
  TH1D* GapsChi2 = (TH1D*) gDirectory->Get(Form("%s_chi2",hn));
  TH1D* GapsAmp  = (TH1D*) gDirectory->Get(Form("%s_0",hn));
  TH1D* GapsMean = (TH1D*) gDirectory->Get(Form("%s_1",hn));
  TH1D* GapsRMS  = (TH1D*) gDirectory->Get(Form("%s_2",hn));

  divslope = GapsMean->GetBinContent(GZN);
  egapZdivslope = TMath::Abs(GapsMean->GetBinError(GZN));

  // Fit only from |Z| = 25-150cm
  // Zero bins -25 thru 25cm (depends upon GZN,GZL,GZH definitions)
  GapsMean->SetBinContent(8,0); GapsMean->SetBinError(8,0);
  GapsMean->SetBinContent(9,0); GapsMean->SetBinError(9,0);

  GapsMean->Fit(&ln1,"R0Q");
  fitslope = ln1.GetParameter(1);
  egapZfitslope = TMath::Abs(ln1.GetParError(1));
  fitintercept = ln1.GetParameter(0);
  egapZfitintercept = TMath::Abs(ln1.GetParError(0));

  delete GapsChi2;
  delete GapsAmp;
  delete GapsMean;
  delete GapsRMS;
}
//_____________________________________________________________________________
#include "TPad.h"
float StSpaceChargeEbyEMaker::EvalCalib(TDirectory* hdir) {

  if (hdir) {
    dcehist = (TH2F*) (hdir->Get("DcaEve"));
    timehist = (TH1F*) (hdir->Get("EvtTime"));
    scehist = (TH1F*) (hdir->Get("SpcChgEvt"));
    if (!(dcehist && timehist && scehist)) {
      LOG_ERROR << "Problems finding SC histograms" << endm;
      return 999.;
    }
  }

  TH1D* dcaproj = dcehist->ProjectionY();

  // Initial fits to DCA distribution
  ga1.SetParameters(1.,0.,1.);
  dcaproj->Fit("ga1","Q");
  float hm = ga1.GetParameter(1);
  float hw = TMath::Abs(ga1.GetParameter(2));
  float hd = 0.6*hw;
  ga1.SetRange(hm-hd,hm+hd);
  dcaproj->Fit("ga1","RQ");
  float gm = ga1.GetParameter(1);
  float gw = TMath::Abs(ga1.GetParameter(2));
  float gm1 = gm;
  float gw1 = gw;

  // Iterate fit to get best answer
  ga1.SetRange(gm-0.9*gw,gm+0.9*gw);
  dcaproj->Fit("ga1","RQ");
  gm = ga1.GetParameter(1);
  gw = TMath::Abs(ga1.GetParameter(2));
  ga1.SetRange(gm-0.8*gw,gm+0.8*gw);
  dcaproj->Fit("ga1","RQ");
  gm = ga1.GetParameter(1);
  gw = TMath::Abs(ga1.GetParameter(2));
  ga1.SetRange(gm-0.7*gw,gm+0.7*gw);
  dcaproj->Fit("ga1","RQ");
  gm = ga1.GetParameter(1);
  gw = TMath::Abs(ga1.GetParameter(2));
  float gme = TMath::Abs(ga1.GetParError(1));
  float gwe = TMath::Abs(ga1.GetParError(2));

  // Average SC
  scehist->Fit("pol0","Q");
  TF1* pl0 = scehist->GetFunction("pol0");
  float pm = 0, pw = 0;
  if (pl0) {
    pm = pl0->GetParameter(0);
    pw = pl0->GetParError(0);
  }
  // Other counts
  float spc = (float) (scehist->GetEntries());
  float dcc = (float) (dcehist->GetEntries());
  float evc = (float) (timehist->GetEntries());
  timehist->GetXaxis()->SetRange(1,(int) evc);
  timehist->Fit("pol0","LQ");
  pl0 = timehist->GetFunction("pol0");
  float epsec = 0;
  if (pl0) epsec = pl0->GetParameter(0); // events per second

  // Quality measures
  float wid = TMath::Min(10.,TMath::Log10(gw1*gw1+gw*gw));
  float frac = spc/evc; // fraction of events for which SC was found

  float code=0;
  if (frac<0.2) code = 1. + frac;               // code = 1.x
  else if (wid>0) code = 2. + 0.1*wid;          // code = 2.x

  LOG_INFO << Form("SCeval: %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
    hm,hw,gm,gw,pm,pw,gm1,gw1,spc,dcc,evc,epsec,gme,gwe) << endm;

  if (code>0) {
    LOG_ERROR << "CheckFail: Break of SpaceCharge performance! code = " << code << endm;
  }

  return code;
}
//_____________________________________________________________________________
// $Id: StSpaceChargeEbyEMaker.cxx,v 1.27 2010/02/23 23:59:41 genevb Exp $
// $Log: StSpaceChargeEbyEMaker.cxx,v $
// Revision 1.27  2010/02/23 23:59:41  genevb
// Reduce positional biases in GridLeak measurements
//
// Revision 1.26  2010/01/27 15:11:00  fisyak
// eliminate access to StTpcDbMaker, use directly gStTpcDb
//
// Revision 1.25  2010/01/27 14:42:33  fisyak
// Use  StMagUtilities::Instance instead of asking tpcHitMoverMaker
//
// Revision 1.24  2009/11/16 22:02:19  genevb
// Loosen nDaughters cut, add BEMCmatch cut, PCT hits cut, enable padrow 13 for Run 9+
//
// Revision 1.23  2009/11/10 20:54:13  fisyak
// pams Cleanup
//
// Revision 1.22  2008/07/24 19:17:55  genevb
// SpaceChargeEWRatio must be written to prepass output table
//
// Revision 1.21  2008/07/21 21:17:10  genevb
// No call to EvalCalib() if Prepass runs successfully
//
// Revision 1.20  2008/07/17 23:35:33  jeromel
// Small format change
//
// Revision 1.19  2008/07/16 03:42:15  genevb
// Use CheckFail in failed performance check error message
//
// Revision 1.18  2008/07/15 22:30:38  genevb
// Added evaluation of calibration performance
//
// Revision 1.17  2008/04/30 14:52:15  genevb
// Reduce pileup contributions
//
// Revision 1.16  2008/01/14 19:22:49  genevb
// Fine tuning of parameters, removal of excess text in messages
//
// Revision 1.15  2007/01/25 19:04:04  perev
// GMT fix
//
// Revision 1.14  2007/01/24 21:42:22  perev
// GMT conversion fixed
//
// Revision 1.13  2006/12/16 01:00:58  genevb
// Better handling of zero magnetic field
//
// Revision 1.12  2006/08/15 23:40:59  genevb
// Averaging was done improperly in DontReset mode
//
// Revision 1.11  2006/07/17 20:13:08  genevb
// Disallow SVT points on tracks
//
// Revision 1.10  2006/07/02 23:22:36  genevb
// Allow for SVT/SSD hits on tracks (necessary for ITTF)
//
// Revision 1.9  2006/06/01 17:27:11  genevb
// Bug fix: gapd and gapf backwards; Improvements: gap fit intercepts, hist and fit ranges
//
// Revision 1.8  2006/01/05 19:12:53  genevb
// Added calib mode
//
// Revision 1.7  2005/12/07 19:45:46  perev
// Histos diconnected from directory. EndCrashFix
//
// Revision 1.6  2005/04/21 19:38:20  genevb
// Additional code for studying SpaceCharge
//
// Revision 1.5  2005/02/16 17:18:06  genevb
// Fill StEvent info on SpaceCharge
//
// Revision 1.4  2004/08/13 20:49:12  genevb
// Improve upon keeping method locked on for each event, and timestamp change
//
// Revision 1.3  2004/08/02 01:19:27  genevb
// minor fixes for getting directories correct
//
// Revision 1.2  2004/07/01 01:46:04  genevb
// Slightly larger margin for full vs half field differences
//
// Revision 1.1  2004/06/30 23:16:00  genevb
// Introduction of StSpaceChargeEbyEMaker
//
//
