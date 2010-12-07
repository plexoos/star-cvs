// $Id: StdEdxY2Maker.cxx,v 1.69 2010/12/07 16:52:19 fisyak Exp $
//#define dChargeCorrection
//#define SpaceChargeQdZ
//#define SeparateSums
//#define CompareWithToF 
#include <Stiostream.h>		 
#include "StdEdxY2Maker.h"
// ROOT
#include "TMinuit.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH2.h"
#include "TH3.h"
#include "TStyle.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TClonesArray.h"
#include "TArrayI.h"
#include "TArrayD.h"
// StUtilities
#include "StMagF.h"
#include "StDbUtilities/StMagUtilities.h"
#include "StMessMgr.h" 
#include "BetheBloch.h"
#include "StBichsel/Bichsel.h"
#include "StDetectorId.h"
#include "StDedxMethod.h"
// StarClassLibrary
#include "StTimer.hh"
#include "SystemOfUnits.h"
#ifndef ST_NO_NAMESPACES
using namespace units;
#endif
#include "StarClassLibrary/StParticleTypes.hh"

// StDb
#include "StDbUtilities/StTpcCoordinateTransform.hh"
#include "StDbUtilities/StCoordinates.hh" 
#include "StTpcDb/StTpcDb.h"
// StEvent 
#include "StEventTypes.h"
#include "StProbPidTraits.h"
#ifdef CompareWithToF
#include "StTofPidTraits.h"
#endif
#include "StTpcDedxPidAlgorithm.h"
#include "StDetectorDbMaker/St_tss_tssparC.h"
#include "StDetectorDbMaker/St_tpcAnodeHVavgC.h"
#include "StDetectorDbMaker/St_tpcAvCurrentC.h"
#include "StDetectorDbMaker/St_trigDetSumsC.h"
const static StPidParticle NHYPS = kPidHe3;//kPidTriton;
const static Int_t tZero= 19950101;
const static Int_t tMin = 20090301;
const static Int_t tMax = 20100705;
const static TDatime t0(tZero,0);
const static Int_t timeOffSet = t0.Convert();
const static Int_t NdEdxMax  = 60;
Int_t     StdEdxY2Maker::NdEdx = 0;
dEdxY2_t *StdEdxY2Maker::CdEdx = 0;
dEdxY2_t *StdEdxY2Maker::FdEdx = 0;
dEdxY2_t *StdEdxY2Maker::dEdxS = 0;
static Int_t numberOfSectors = 0;
static Int_t numberOfTimeBins = 0;
static Int_t NumberOfRows = 0;
static Int_t NumberOfInnerRows = 0;
static Int_t NoPads = 0;
static Double_t innerSectorPadPitch = 0;
static Double_t outerSectorPadPitch = 0;

const static Double_t pMomin = 0.4; // range for dE/dx calibration
const static Double_t pMomax = 0.5;
Bichsel *StdEdxY2Maker::m_Bichsel = 0;
#include "dEdxTrackY2.h"
//______________________________________________________________________________
// QA histogramss
const static Int_t  fNZOfBadHits = 11;
static TH1F **fZOfBadHits = 0;
static TH1F *fZOfGoodHits = 0;
static TH1F *fPhiOfBadHits = 0;
static TH1F *fTracklengthInTpcTotal = 0;
static TH1F *fTracklengthInTpc = 0;
//______________________________________________________________________________
ClassImp(StdEdxY2Maker);
//_____________________________________________________________________________
StdEdxY2Maker::StdEdxY2Maker(const char *name):
  StMaker(name), 
  m_Minuit(0), m_TpcdEdxCorrection(0),  m_Mask(-1), 
  mHitsUsage(0)
{
  memset (beg, 0, end-beg);
  //  SETBIT(m_Mode,kOldClusterFinder); 
  SETBIT(m_Mode,kPadSelection); 
  m_Minuit = new TMinuit(2);
  memset(mNormal[0]        ,0,sizeof(mNormal     ));
  memset(mRowPosition[0][0],0,sizeof(mRowPosition));
  memset(mPromptNormal[0]     ,0,sizeof(mPromptNormal  ));
  memset(mPromptPosition[0][0],0,sizeof(mPromptPosition));
}
//_____________________________________________________________________________
Int_t StdEdxY2Maker::Init(){
  if (IAttr("EmbeddingShortCut")) {
    m_Mode = 0;
    SETBIT(m_Mode,kEmbedding);
    SETBIT(m_Mode,kPadSelection); 
    SETBIT(m_Mask,StTpcdEdxCorrection::kTpcLast);
  }
  if (Debug()) {
    if (! TESTBIT(m_Mode, kOldClusterFinder)) 
      gMessMgr->Warning() << "StdEdxY2Maker::Init use new Cluster Finder parameterization" << endm;
    else                  
      gMessMgr->Warning() << "StdEdxY2Maker::Init use old Cluster Finder parameterization" << endm;
    if (TESTBIT(m_Mode, kPadSelection))     
      gMessMgr->Warning() << "StdEdxY2Maker::Init Pad Selection is ON" << endm;
    if (TESTBIT(m_Mode, kDoNotCorrectdEdx))     
      gMessMgr->Warning() << "StdEdxY2Maker::Init Don't Correct dEdx" << endm;
    if (TESTBIT(m_Mode, kEmbedding))     
      gMessMgr->Warning() << "StdEdxY2Maker::Init This is embedding run" << endm;
  }
  if (! m_Bichsel) m_Bichsel = Bichsel::Instance();
  
  gMessMgr->SetLimit("StdEdxY2Maker:: mismatched Sector",20);
  gMessMgr->SetLimit("StdEdxY2Maker:: pad/TimeBucket out of range:",20);
  gMessMgr->SetLimit("StdEdxY2Maker:: Helix Prediction",20);
  gMessMgr->SetLimit("StdEdxY2Maker:: Coordinates",20);
  gMessMgr->SetLimit("StdEdxY2Maker:: Prediction",20);
  gMessMgr->SetLimit("StdEdxY2Maker:: NdEdx",20);
  gMessMgr->SetLimit("StdEdxY2Maker:: Illegal time for scalers",20);
  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StdEdxY2Maker::InitRun(Int_t RunNumber){
  static Int_t DoOnce = 0;
  if (!gStTpcDb) {
    cout << "Database Missing! Can't initialize StdEdxY2Maker" << endl;
    return kStFatal;
  }
  // 		TPG parameters
  numberOfSectors   = gStTpcDb->Dimensions()->numberOfSectors();
  numberOfTimeBins  = gStTpcDb->Electronics()->numberOfTimeBins();
  NumberOfRows      = gStTpcDb->PadPlaneGeometry()->numberOfRows();
  NumberOfInnerRows = gStTpcDb->PadPlaneGeometry()->numberOfInnerRows();
  NoPads = TMath::Max(gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(NumberOfInnerRows),
		      gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(NumberOfRows));	  
  innerSectorPadPitch = gStTpcDb->PadPlaneGeometry()->innerSectorPadPitch();
  outerSectorPadPitch = gStTpcDb->PadPlaneGeometry()->outerSectorPadPitch();

  if ( ! St_trigDetSumsC::instance() ) gMessMgr->Error() << "StdEdxY2Maker:: Cannot find trigDetSums" << endm;
  else {
    if (!St_trigDetSumsC::instance()->GetNRows()) gMessMgr->Error() << "StdEdxY2Maker:: trigDetSums has not data" << endm;
    else {
      UInt_t date = GetDateTime().Convert();
      if (date < St_trigDetSumsC::instance()->timeOffset()) {
	gMessMgr->Error() << "StdEdxY2Maker:: Illegal time for scalers = " 
			  << St_trigDetSumsC::instance()->timeOffset() << "/" << date
			  << " Run " << St_trigDetSumsC::instance()->runNumber() << "/" << GetRunNumber() << endm;
      }
    }
  }
  if (! DoOnce) {
    DoOnce = 1;
    if (TESTBIT(m_Mode, kCalibration)) {// calibration mode
      if (Debug()) gMessMgr->Warning() << "StdEdxY2Maker::InitRun Calibration Mode is On (make calibration histograms)" << endm;
      TFile *f = GetTFile();
      if (f) {
	f->cd();
	if ((TESTBIT(m_Mode, kGASHISTOGRAMS))) {
	  if (Debug()) gMessMgr->Warning() << "StdEdxY2Maker::InitRun Gas Histograms is ON" << endm;
	  TrigHistos();
	}
	Histogramming();
	if ((TESTBIT(m_Mode, kSpaceChargeStudy))) SpaceCharge();
	if ((TESTBIT(m_Mode, kXYZcheck))) XyzCheck();
      }
    }
    QAPlots(0);
  }
  SafeDelete(m_TpcdEdxCorrection);
  m_TpcdEdxCorrection = new StTpcdEdxCorrection(m_Mask, Debug());
  
  StTpcCoordinateTransform transform(gStTpcDb);
  for (Int_t sector = 1; sector<= numberOfSectors; sector++) {
    for (Int_t row = 1; row <= NumberOfRows; row++) {
      //      if (! St_tpcAnodeHVavgC::instance()->livePadrow(sector,row)) continue;
      if (Debug()>1) cout << "========= sector/row ========" << sector << "/" << row << endl;
      StTpcLocalSectorDirection  dirLS(0.,1.,0.,sector,row);  if (Debug()>1) cout << "dirLS\t" << dirLS << endl;
      StTpcLocalDirection        dirL;      
      StTpcLocalSectorAlignedDirection  dirLSA;
      transform(dirLS,dirLSA);   if (Debug()>1) cout << "dirLSA\t" << dirLSA << endl;
      transform(dirLSA,dirL);                     if (Debug()>1) cout << "dirL\t" << dirL << endl;
      StGlobalDirection          dirG;
      transform(dirL,dirG);                       if (Debug()>1) cout << "dirG\t" << dirG << endl;
      SafeDelete(mNormal[sector-1][row-1]);
      mNormal[sector-1][row-1] = new StThreeVectorD(dirG.position().unit());
      if (Debug()>1) cout << "Normal[" << sector-1 << "][" << row-1 << "] = " << *mNormal[sector-1][row-1] << endl;
      Double_t padlength;
      if (row<14) padlength = gStTpcDb->PadPlaneGeometry()->innerSectorPadLength();
      else 	  padlength = gStTpcDb->PadPlaneGeometry()->outerSectorPadLength();
      for (Int_t l = 0; l < 3; l++) {
	SafeDelete(mRowPosition[sector-1][row-1][l]); 
	Double_t y = transform.yFromRow(row);
	if (l == 1) y += padlength/2.;
	if (l == 2) y -= padlength/2.;
	StTpcLocalSectorCoordinate  lsCoord(0., y, 10.,sector,row); if (Debug()>1) cout << lsCoord << endl;
	StGlobalCoordinate  gCoord; 
	StTpcLocalSectorAlignedCoordinate lsCoordA;  
	transform(lsCoord,lsCoordA);                       if (Debug()>1) cout << lsCoordA << endl;                   
	transform(lsCoordA, gCoord);                       if (Debug()>1) cout << gCoord << endl;                   
	SafeDelete(mRowPosition[sector-1][row-1][l]);
	mRowPosition[sector-1][row-1][l] = 
	  new  StThreeVectorD(gCoord.position().x(),gCoord.position().y(),gCoord.position().z());
	if (Debug()>1) cout << "mRowPosition[" << sector-1 << "][" << row-1 << "][" << l << "] = " 
			    << *mRowPosition[sector-1][row-1][l] << endl;
      }
    }
  }
  for (Int_t iWestEast = 0; iWestEast < 2; iWestEast++) {
    for (Int_t io = 0; io < 2; io++) {
      if (Debug()>1) cout << "========= West (0) or  East(1) / Inner(0) or Outer (1)  ========" 
			  << iWestEast << "/" << io << endl;
      Int_t sector = (iWestEast == 0) ? 12 : 24;
      Int_t row    = (io    == 0) ?  1 : 14;
      StTpcLocalSectorDirection  dirLS(0.,0.,1.0,sector,row);  if (Debug()>1) cout << "dirLS\t" << dirLS << endl;
      StTpcLocalDirection        dirL;      
      StTpcLocalSectorAlignedDirection  dirLSA;
      transform(dirLS,dirLSA);   if (Debug()>1) cout << "dirLSA\t" << dirLSA << endl;
      transform(dirLSA,dirL);    if (Debug()>1) cout << "dirL\t" << dirL << endl;
      StGlobalDirection dirG;
      transform(dirL,dirG);      if (Debug()>1) cout << "dirG\t" << dirG << endl;
      SafeDelete(mPromptNormal[iWestEast][io]);
      mPromptNormal[iWestEast][io] = new StThreeVectorD(dirG.position().unit());
      if (Debug()>1) cout << "mPromptNormal[" << iWestEast << "][" << io << "] = " << *mPromptNormal[iWestEast][io] << endl;
      //      Double_t zGG = gStTpcDb->Dimensions()->gatingGridZ(); // outerSectorPadPlaneZ is 210.107 cm in Db instead of 209.99 on Drawings ?
      Double_t z[2][3] = { 
	// Anodes         GG          Pads
	{ -0.6 - 0.2,     0,  -0.6 - 2*0.2}, // Inner
 	{ -0.6 - 0.4,     0,  -0.6 - 2*.04}  // Outer
      };
      for (Int_t l = 0; l < 3; l++) {
	SafeDelete(mPromptPosition[iWestEast][io][l]); 
	Double_t y = transform.yFromRow(row);
	StTpcLocalSectorCoordinate  lsCoord(0., y, z[io][l],sector,row); if (Debug()>1) cout << lsCoord << endl;
	StGlobalCoordinate  gCoord; 
	StTpcLocalSectorAlignedCoordinate lsCoordA;  
	transform(lsCoord,lsCoordA);                       if (Debug()>1) cout << lsCoordA << endl;                   
	transform(lsCoordA, gCoord);                       if (Debug()>1) cout << gCoord << endl;                   
	SafeDelete(mPromptPosition[iWestEast][io][l]);
	mPromptPosition[iWestEast][io][l] = 
	  new  StThreeVectorD(gCoord.position().x(),gCoord.position().y(),gCoord.position().z());
	if (Debug()>1) cout << "mPromptPosition[" << sector-1 << "][" << row-1 << "][" << l << "] = " 
			    << *mPromptPosition[iWestEast][io][l] << endl;
      }
    }
  }
  return kStOK;
}
//_____________________________________________________________________________
Int_t StdEdxY2Maker::FinishRun(Int_t OldRunNumber) {
  // Move Clean up to InitRun
  
  for (int i = 0; i < 24; i++) 
    for (int j = 0; j < 45; j++) {
      SafeDelete(mNormal[i][j]);
      for (Int_t k = 0; k < 3; k++) 
	SafeDelete(mRowPosition[i][j][k]);
    }
  
  SafeDelete(m_TpcdEdxCorrection);
  return StMaker::FinishRun(OldRunNumber);
}
//_____________________________________________________________________________
Int_t StdEdxY2Maker::Finish() {
  FinishRun(0);
  SafeDelete(m_TpcdEdxCorrection);
  SafeDelete(m_Minuit);
  SafeDelete(m_Bichsel);
  return StMaker::Finish();
}
//_____________________________________________________________________________
Int_t StdEdxY2Maker::Make(){ 
  static const Double_t RA[2]        = { 154.484, 81.42}; // Outer/ Inner average Radii
  static const Double_t WireLenth[2] = {   3.6e5, 1.6e5}; 
  Double_t              QAcumm[2]    = {St_tpcAvCurrentC::instance()->chargeO(), St_tpcAvCurrentC::instance()->chargeI()};
  Double_t              QL[2]        = {QAcumm[0]*RA[0]*RA[0]/WireLenth[0], QAcumm[1]*RA[1]*RA[1]/WireLenth[1]};
  Double_t              Current[2]   = {St_tpcAvCurrentC::instance()->currentO(),St_tpcAvCurrentC::instance()->currentI()};
  Double_t              CL[2]        = {Current[0]*RA[0], Current[1]*RA[1]};
  static  StTimer timer;
  static  StTpcLocalSectorCoordinate        localSect[4];
  static  StTpcPadCoordinate                PadOfTrack, Pad;
  static  StTpcLocalSectorDirection         localDirectionOfTrack;
  static  StTpcLocalSectorAlignedDirection  localADirectionOfTrack;
  static  StTpcLocalSectorAlignedCoordinate localA, lANext;
  static  StThreeVectorD xyz[4];
  static  StThreeVectorD dirG;
  static  Double_t s[2], s_in[2], s_out[2], w[2], w_in[2], w_out[2], s_inP[2], s_outP[2], dx;
  if (Debug() > 0) timer.start();
  static dEdxY2_t CdEdxT[180];//,FdEdxT[NdEdxMax],dEdxST[NdEdxMax];
  CdEdx = CdEdxT; 
  FdEdx = CdEdxT + NdEdxMax; 
  dEdxS = CdEdxT + 2*NdEdxMax; 
  St_tpcGas  *tpcGas = m_TpcdEdxCorrection->tpcGas();
  if (TESTBIT(m_Mode, kCalibration) && tpcGas) TrigHistos(1);
  StTpcCoordinateTransform transform(gStTpcDb);
  Double_t bField = 0;
  StEvent* pEvent = dynamic_cast<StEvent*> (GetInputDS("StEvent"));
  if (!pEvent) {
    gMessMgr->Info() << "StdEdxY2Maker: no StEvent " << endm;
    return kStOK;        // if no event, we're done
  }
  if (pEvent->runInfo()) bField = pEvent->runInfo()->magneticField()*kilogauss;
  
  if (fabs(bField) < 1.e-5*kilogauss) return kStOK;
  
  if ((TESTBIT(m_Mode, kSpaceChargeStudy))) SpaceCharge(1,pEvent);
  // no of tpc hits
  Int_t TotalNoOfTpcHits = 0;
  Int_t NoOfTpcHitsUsed  = 0;
  StTpcHitCollection* TpcHitCollection = pEvent->tpcHitCollection();
  if (! TpcHitCollection) {
    gMessMgr->Info() << "StdEdxY2Maker: no TpcHitCollection " << endm;
    return kStOK;        // if no event, we're done
  }
  TotalNoOfTpcHits = TpcHitCollection->numberOfHits();
  StSPtrVecTrackNode& trackNode = pEvent->trackNodes();
  UInt_t nTracks = trackNode.size();
  StTrackNode *node=0;
  for (UInt_t i=0; i < nTracks; i++) {
    node = trackNode[i]; 
    if (!node) continue;
    StGlobalTrack  *gTrack = static_cast<StGlobalTrack *>(node->track(global));
    StPrimaryTrack *pTrack = static_cast<StPrimaryTrack*>(node->track(primary));
#if 1 /* No primary track cut */
    if (TESTBIT(m_Mode, kCalibration)) {
      if (! pTrack) continue; // reject non primary tracks
#if 1
      if (pTrack->vertex() != pEvent->primaryVertex()) continue; // only the first primary vertex
#else
      if ( ((StPrimaryVertex *) pTrack->vertex() )->numMatchesWithBEMC() <= 0) continue;
#endif
    }
#endif
    StTrack *track = 0;
    StTrack *tracks[2] = {gTrack, pTrack};
    if (! TESTBIT(m_Mode, kDoNotCorrectdEdx)) {
      // clean up old Tpc traits if any
      // clean old PiD traits
      for (int l = 0; l < 2; l++) {
	track = tracks[l]; 
	if (track) {
	  StSPtrVecTrackPidTraits &traits = track->pidTraits();
	  UInt_t size = traits.size();
	  if (size) {
	    for (UInt_t i = 0; i < size; i++) {
	      StDedxPidTraits *pid = dynamic_cast<StDedxPidTraits*>(traits[i]);
	      if (! pid) continue;
	      if (pid->detector() != kTpcId) continue;
	      traits[i]->makeZombie(1);
	    }
	  }
	}
      }
    }
    if (! gTrack ||  gTrack->flag() <= 0) continue;
    StPhysicalHelixD helixO = gTrack->outerGeometry()->helix();
    StPhysicalHelixD helixI = gTrack->geometry()->helix();
    if (Debug() > 1) {
      cout << "Track:" << i 
	   << "\ttype " << gTrack->type()
	   << "\tvertex " << gTrack->vertex()
	   << "\tkey " << gTrack->key()
	   << "\tflag " << gTrack->flag()
	   << "\tencodedMethod " << gTrack->encodedMethod()
	   << "\timpactParameter " << gTrack->impactParameter()
	   << "\tlength " << gTrack->length()
	   << "\tnumberOfPossiblePoints " << gTrack->numberOfPossiblePoints() << endl;
      cout << "pxyzI:        " << gTrack->geometry()->momentum() << "\tmag " << gTrack->geometry()->momentum().mag() << endl;
      cout << "pxyzO:        " << gTrack->outerGeometry()->momentum() << "\tmag " << gTrack->outerGeometry()->momentum().mag() << endl;
      cout << "start Point: " << helixI.at(0) << endl;
      cout << "end   Point: " << helixO.at(0) << endl;
    }
    //    StPtrVecHit hvec = gTrack->detectorInfo()->hits(kTpcId);
    StPtrVecHit hvec = gTrack->detectorInfo()->hits();
    // if no hits than make only histograms. Works if kDoNotCorrectdEdx mode is set
    if (hvec.size() && ! TESTBIT(m_Mode, kDoNotCorrectdEdx)) {
      Int_t Id = gTrack->key();
      Int_t NoFitPoints = gTrack->fitTraits().numberOfFitPoints(kTpcId);
      NdEdx = 0;
      Double_t TrackLength70 = 0;
      Double_t TrackLength = 0;
      Double_t TrackLengthTotal = 0;
      for (UInt_t j=0; j<hvec.size(); j++) {// hit loop
	if (hvec[j]->detector() != kTpcId) continue;
	StTpcHit *tpcHit = static_cast<StTpcHit *> (hvec[j]);
	if (! tpcHit) continue;
	if (Debug() > 1) {tpcHit->Print();}
	Int_t sector = tpcHit->sector();
	Int_t row    = tpcHit->padrow();
	if (! St_tpcAnodeHVavgC::instance()->livePadrow(sector,row)) continue;
	xyz[3] = StThreeVectorD(tpcHit->position().x(),tpcHit->position().y(),tpcHit->position().z());
	//________________________________________________________________________________      
	const StThreeVectorD &normal = *mNormal[sector-1][row-1];
	const StThreeVectorD &middle = *mRowPosition[sector-1][row-1][0];
	const StThreeVectorD &upper  = *mRowPosition[sector-1][row-1][1];
	const StThreeVectorD &lower  = *mRowPosition[sector-1][row-1][2];
	// Check that Voltage above "-100V" from nominal, mark as unrecoverable
	Double_t V = St_tpcAnodeHVavgC::instance()->voltagePadrow(sector,row);
	if ((row <= 13 && 1170 - V > 100) || 
	    (row >  13 && 1390 - V > 100)) {BadHit(9,tpcHit->position()); continue;}
	// check that helix prediction is consistent with measurement
	if (Propagate(middle,normal,helixI,helixO,bField,xyz[0],dirG,s,w)) {BadHit(2,tpcHit->position()); continue;}
	if (Debug() > 1) {
	  cout << " Prediction:\t" << xyz[0] 
	       << "\tat s=\t" << s[0] << "/" << s[1] 
	       << "\tw = " << w[0] << "/" << w[1] << endl;
	}
	StThreeVectorD dif = xyz[3] - xyz[0];
	if (dif.perp() > 2.0) {if (Debug() > 1) {cout << "Prediction is to far from hit:\t" << xyz[3] << endl;}
	  continue;
	}
	if (Propagate(upper,normal,helixI,helixO,bField,xyz[1],dirG,s_out,w_out)) {BadHit(2,tpcHit->position()); continue;}
	if (Propagate(lower,normal,helixI,helixO,bField,xyz[2],dirG,s_in ,w_in )) {BadHit(2,tpcHit->position()); continue;}
	dx = ((s_out[0] - s_in[0])*w[1] + (s_out[1] - s_in[1])*w[0]);
	if (dx <= 0.0) {if (Debug() > 1) {cout << "negative dx " << dx << endl;}
	  continue;
	}

	StGlobalDirection  globalDirectionOfTrack(dirG);
	for (Int_t l = 0; l < 4; l++) {
	  StGlobalCoordinate globalOfTrack(xyz[l].x(),xyz[l].y(),xyz[l].z());
	  if (Debug() > 2) {
	    Int_t k = l;
	    if (l == 3) k = 0;
	    Double_t D = - (*mRowPosition[sector-1][row-1][k])*normal;
	    Double_t A = normal*normal;
	    Double_t delta = (xyz[l]*normal + D)/TMath::Sqrt(A);
	    if (TMath::Abs(delta) > 1.e-2) {
	      cout << "Out of Plane by " << delta << "\tPlane " 
		   << (*mRowPosition[sector-1][row-1][k]) << "\tNormal " << normal << endl;
	      cout << "Track/hit : " << endl; 
	      cout << "\txyz[0] " << xyz[0] << "\t s = "     << s[0]     << "/" << s[1]     << endl;
	      cout << "\txyz[1] " << xyz[1] << "\t s_out = " << s_out[0] << "/" << s_out[1] << endl; 
	      cout << "\txyz[2] " << xyz[2] << "\t s_in = "  << s_in[0]  << "/" << s_in[1]  <<  endl;
	      cout << "\txyz[3] " << xyz[3] << endl;
	    }
	  }
	  transform(globalOfTrack,localA,sector,row);
	  transform(localA,localSect[l]);
	}
	Double_t zP = TMath::Abs(xyz[0].z());
	//----------------------------- Prompt Hits ? ------------------------------
	if (zP > 205.0 && zP < 215.) {
	  Int_t iWestEast = 0;
	  if (sector > 12) iWestEast = 1;
	  Int_t io = 0;
	  if (row > 13) io = 1;
	  const StThreeVectorD &PromptNormal = *mPromptNormal[iWestEast][io];
	  const StThreeVectorD &anode = *mPromptPosition[iWestEast][io][0];
	  const StThreeVectorD &gg    = *mPromptPosition[iWestEast][io][1];
	  const StThreeVectorD &pads  = *mPromptPosition[iWestEast][io][2];
	  // check that helix prediction is consistent with measurement
	  if (Propagate(anode,PromptNormal,helixI,helixO,bField,xyz[0],dirG,s,w)) {BadHit(2,tpcHit->position()); continue;}
	  if (Debug() > 1) {
	    cout << " Prediction:\t" << xyz[0] 
		 << "\tat s=\t" << s[0] << "/" << s[1] 
		 << "\tw = " << w[0] << "/" << w[1] << endl;
	  }
	  dif = xyz[3] - xyz[0];
	  if (dif.perp() > 2.0) {if (Debug() > 1) {cout << "Prediction is to far from hit:\t" << xyz[3] << endl;}
	    continue;
	  }
	  if (Propagate(pads,PromptNormal,helixI,helixO,bField,xyz[1],dirG,s_outP,w_out)) {BadHit(2,tpcHit->position()); continue;}
	  if (Propagate(gg  ,PromptNormal,helixI,helixO,bField,xyz[2],dirG,s_inP ,w_in )) {BadHit(2,tpcHit->position()); continue;}
	  s_out[0] = TMath::Min(s_outP[0], s_out[0]);
	  s_out[1] = TMath::Min(s_outP[1], s_out[1]);
	  s_in[0]  = TMath::Max(s_inP[0] , s_in[0] );
	  s_in[1]  = TMath::Max(s_inP[1] , s_in[1] );
	  dx = ((s_out[0] - s_in[0])*w[1] + (s_out[1] - s_in[1])*w[0]);
	  if (dx <= 0.0) {if (Debug() > 1) {cout << "negative dx " << dx << endl;}
	    continue;
	  }
	  StGlobalDirection  globalDirectionOfTrack(dirG);
	  for (Int_t l = 0; l < 4; l++) {
	    StGlobalCoordinate globalOfTrack(xyz[l].x(),xyz[l].y(),xyz[l].z());
	    if (Debug() > 2) {
	      Int_t k = l;
	      if (l == 3) k = 0;
	      Double_t D = - (*mRowPosition[sector-1][row-1][k])*PromptNormal;
	      Double_t A = PromptNormal*PromptNormal;
	      Double_t delta = (xyz[l]*PromptNormal + D)/TMath::Sqrt(A);
	      if (TMath::Abs(delta) > 1.e-2) {
		cout << "Out of Plane by " << delta << "\tPlane " 
		     << (*mRowPosition[sector-1][row-1][k]) << "\tNormal " << PromptNormal << endl;
		cout << "Track/hit : " << endl; 
		cout << "\txyz[0] " << xyz[0] << "\t s = "     << s[0]     << "/" << s[1]     << endl;
		cout << "\txyz[1] " << xyz[1] << "\t s_out = " << s_out[0] << "/" << s_out[1] << endl; 
		cout << "\txyz[2] " << xyz[2] << "\t s_in = "  << s_in[0]  << "/" << s_in[1]  <<  endl;
	      }
	    }
	    transform(globalOfTrack,localA,sector,row);
	    transform(localA,localSect[l]);
	  }
	}
	transform(localSect[0],PadOfTrack);
	transform(globalDirectionOfTrack,localADirectionOfTrack,sector,row);
	transform(localADirectionOfTrack,localDirectionOfTrack);
	transform(localSect[3],Pad);
	CdEdx[NdEdx].Reset();
	CdEdx[NdEdx].resXYZ[0] = localSect[3].position().x() - localSect[0].position().x();
	CdEdx[NdEdx].resXYZ[1] = localSect[3].position().y() - localSect[0].position().y();
	CdEdx[NdEdx].resXYZ[2] = localSect[3].position().z() - localSect[0].position().z();
	TrackLengthTotal += dx;
	if (! tpcHit->usedInFit()) {
	  BadHit(0,tpcHit->position());
	  continue;}
	if (  tpcHit->flag()) {
	  BadHit(1,tpcHit->position());
	  continue;}
	//________________________________________________________________________________      
	Int_t iokCheck = 0;
	if (sector != Pad.sector() || // ? && TMath::Abs(xyz[0].x()) > 20.0 ||
	    row    != Pad.row()) {
	  gMessMgr->Warning() << "StdEdxY2Maker:: mismatched Sector " 
			      << Pad.sector() << " / " << sector
			      << " Row " << Pad.row() << " / " << row 
			      << "pad " << Pad.pad() << " TimeBucket :" << Pad.timeBucket() 
			      << endm;
	  iokCheck++;
	}
	Double_t pad = tpcHit->pad();
	if (pad == 0) pad = Pad.pad();
	Double_t edge = pad;
	if (edge > 0.5*gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(row)) 
	  edge -= gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(row) + 1;
	if (Pad.timeBucket() < 0         ||
	    Pad.timeBucket() >= numberOfTimeBins) {
	  gMessMgr->Warning() << "StdEdxY2Maker:: TimeBucket out of range: " 
			      << Pad.timeBucket() << endm;
	  iokCheck++;
	}
	if (sector != PadOfTrack.sector() || 
	    row != PadOfTrack.row() ||	
	    TMath::Abs(Pad.pad()-PadOfTrack.pad()) > 5) {
	  if (Debug() > 1) {
	    gMessMgr->Warning() << "StdEdxY2Maker::	Helix Prediction " 
				<< "Sector = " 
				<< PadOfTrack.sector() << "/" 
				<< sector 
				<< " Row = " << PadOfTrack.row() << "/" 
				<< row 
				<< " Pad = " << PadOfTrack.pad() << "/" 
				<< Pad.pad() 
				<< " from Helix  is not matched with point/" << endm;;
	    gMessMgr->Warning() << "StdEdxY2Maker:: Coordinates Preiction: " 
				<< xyz[0] << "/Hit " << tpcHit->position()
				<< endm;
	  }
	  iokCheck++;
	}
	if (tpcHit->charge() <= 0) {
	  gMessMgr->Warning() << "StdEdxY2Maker:: deposited charge : " <<  tpcHit->charge() 
			      << " <= 0" << endm;
	  iokCheck++;
	}
	//	if ((TESTBIT(m_Mode, kXYZcheck)) && (TESTBIT(m_Mode, kCalibration))) XyzCheck(&global, iokCheck);
	if ((TESTBIT(m_Mode, kPadSelection)) && iokCheck) {BadHit(3, tpcHit->position()); continue;}
	if ((TESTBIT(m_Mode, kPadSelection)) && (dx < 0.5 || dx > 25.)) {BadHit(4, tpcHit->position()); continue;}
	StTpcdEdxCorrection::ESector kTpcOutIn = StTpcdEdxCorrection::kTpcOuter;
	if (row <= NumberOfInnerRows) kTpcOutIn = StTpcdEdxCorrection::kTpcInner;
	// Corrections
	CdEdx[NdEdx].Reset();
	CdEdx[NdEdx].DeltaZ = 5.2; 
	CdEdx[NdEdx].QRatio = -2;
	CdEdx[NdEdx].QRatioA = -2.;
	CdEdx[NdEdx].QSumA = 0;
	CdEdx[NdEdx].sector = sector; 
	CdEdx[NdEdx].row    = row;
	CdEdx[NdEdx].pad    = Pad.pad();
	CdEdx[NdEdx].edge   = edge;
	CdEdx[NdEdx].pad    = (Int_t) Pad.pad();
	CdEdx[NdEdx].edge   = CdEdx[NdEdx].pad;
	if (CdEdx[NdEdx].edge > 0.5*gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(row)) 
	  CdEdx[NdEdx].edge += 1 - gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(row);
	CdEdx[NdEdx].Npads  = tpcHit->padsInHit();
	CdEdx[NdEdx].Ntbins = tpcHit->pixelsInHit();
	//	CdEdx[NdEdx].dE     = tpcHit->chargeModified();
	CdEdx[NdEdx].dE     = tpcHit->charge();
	//	CdEdx[NdEdx].dCharge= tpcHit->chargeModified()/tpcHit->charge() - 1.;
	CdEdx[NdEdx].dCharge= tpcHit->chargeModified() - tpcHit->charge();
	StTpcdEdxCorrection::ESector kTpcInOut     = StTpcdEdxCorrection::kTpcOuter;
	if (row <= 13) kTpcInOut = StTpcdEdxCorrection::kTpcInner;
	Int_t p1 = tpcHit->minPad();
	Int_t p2 = tpcHit->maxPad();
	Int_t t1 = tpcHit->minTmbk();
	Int_t t2 = tpcHit->maxTmbk();
	CdEdx[NdEdx].rCharge=  0.5*m_TpcdEdxCorrection->Adc2GeV()*TMath::Pi()/4.*(p2-p1+1)*(t2-t1+1);
	if (TESTBIT(m_Mode, kEmbeddingShortCut) && 
	    (tpcHit->idTruth() && tpcHit->qaTruth() > 95)) CdEdx[NdEdx].lSimulated = tpcHit->idTruth();
	CdEdx[NdEdx].dx     = dx;
	CdEdx[NdEdx].dxH    = 0;
	CdEdx[NdEdx].xyz[0] = localSect[3].position().x();
	CdEdx[NdEdx].xyz[1] = localSect[3].position().y();
	CdEdx[NdEdx].xyz[2] = localSect[3].position().z();
	Double_t probablePad = gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(row)/2;
	Double_t pitch = (row<14) ?
	  gStTpcDb->PadPlaneGeometry()->innerSectorPadPitch() :
	  gStTpcDb->PadPlaneGeometry()->outerSectorPadPitch();
	Double_t PhiMax = TMath::ATan2(probablePad*pitch, gStTpcDb->PadPlaneGeometry()->radialDistanceAtRow(row));
	CdEdx[NdEdx].PhiR   = TMath::ATan2(CdEdx[NdEdx].xyz[0],CdEdx[NdEdx].xyz[1])/PhiMax;
	CdEdx[NdEdx].xyzD[0] = localDirectionOfTrack.position().x();
	CdEdx[NdEdx].xyzD[1] = localDirectionOfTrack.position().y();
	CdEdx[NdEdx].xyzD[2] = localDirectionOfTrack.position().z();
	CdEdx[NdEdx].ZdriftDistance = localSect[3].position().z();
	CdEdx[NdEdx].zG      = tpcHit->position().z();
	CdEdx[NdEdx].Qcm     = 1e6*QL[kTpcOutIn]/tpcHit->position().perp2(); // uC/cm
	CdEdx[NdEdx].Crow    =     CL[kTpcOutIn]/tpcHit->position().perp();
	if (St_trigDetSumsC::instance())	CdEdx[NdEdx].Zdc     = St_trigDetSumsC::instance()->zdcX();
	Bool_t doIT = kTRUE;
	if (TESTBIT(m_Mode,kEmbedding)) doIT = kFALSE;
	Int_t iok = m_TpcdEdxCorrection->dEdxCorrection(CdEdx[NdEdx],doIT);
	if (iok) {BadHit(4+iok, tpcHit->position()); continue;} 
	//	if ((TESTBIT(m_Mode, kSpaceChargeStudy))) SpaceCharge(2,pEvent,&global,&CdEdx[NdEdx]);
	if (fZOfGoodHits) fZOfGoodHits->Fill(tpcHit->position().z());
	if (NdEdx < NdEdxMax) {
	  TrackLength         += CdEdx[NdEdx].dx;
	  NdEdx++; 
	  NoOfTpcHitsUsed++; 	
	}
	if (NdEdx > NoFitPoints) 
	  gMessMgr->Error() << "StdEdxY2Maker:: NdEdx = " << NdEdx 
			    << ">  NoFitPoints ="<< NoFitPoints << endm;
      }
      if (fTracklengthInTpcTotal) fTracklengthInTpcTotal->Fill(TrackLengthTotal);
      if (fTracklengthInTpc)      fTracklengthInTpc->Fill(TrackLength);
      SortdEdx();
      Double_t I70 = 0, D70 = 0;
      Int_t N70 = NdEdx - (int) (0.3*NdEdx + 0.5); 
      if (N70 > 1) {
	Int_t k;
	for (k = 0; k < N70; k++) {
	  I70 += dEdxS[k].dEdx;
	  D70 += dEdxS[k].dEdx*dEdxS[k].dEdx;
	  TrackLength70 += dEdxS[k].dx;
	}
	I70 /= N70; D70 /= N70;
	D70  = TMath::Sqrt(D70 - I70*I70);
	D70 /= I70;
	dst_dedx_st dedx;
	dedx.id_track  =  Id;
	dedx.det_id    =  kTpcId;    // TPC track 
	dedx.method    =  kEnsembleTruncatedMeanId; // == kTruncatedMeanId+1;
	dedx.ndedx     =  N70 + 100*((int) TrackLength);
	dedx.dedx[0]   =  I70;
	dedx.dedx[1]   =  D70;
	if ((TESTBIT(m_Mode, kCalibration)))  // uncorrected dEdx
	  for (int l = 0; l < 2; l++) {
	    if (tracks[l]) tracks[l]->addPidTraits(new StDedxPidTraits(dedx));
	  }
	if (! TESTBIT(m_Mode, kDoNotCorrectdEdx)) { 
	  m_TpcdEdxCorrection->dEdxTrackCorrection(StTpcdEdxCorrection::kTpcLengthCorrection,0,dedx); 
	  m_TpcdEdxCorrection->dEdxTrackCorrection(StTpcdEdxCorrection::kTpcdEdxCor,0,dedx); 
	  dedx.method    =  kTruncatedMeanId;
	  for (int l = 0; l < 2; l++) {if (tracks[l]) tracks[l]->addPidTraits(new StDedxPidTraits(dedx));}
	}
	if ((TESTBIT(m_Mode, kCalibration))) { // 30% truncated for Outer only
	  Double_t I70A = 0;
	  Double_t D70A = 0;
	  Int_t Nouter = 0;
	  for (k = 0; k < NdEdx; k++) if (dEdxS[k].row > 13) Nouter++;
	  Int_t N70outer = Nouter - (int) (0.3*Nouter + 0.5);
	  Double_t TrackLengthA = 0;
	  if (N70outer > 1) {
	    Int_t N = 0;
	    for (k = 0; k < N70outer; k++) {
	      if (dEdxS[k].row <= 13 || N > N70outer) continue;
	      N++;
	      I70A += dEdxS[k].dEdx;
	      D70A += dEdxS[k].dEdx*dEdxS[k].dEdx;
	      TrackLengthA += dEdxS[k].dx;
	    }
	    if (N > 0) {
	      I70A /= N; D70A /= N;
	      // Arguably, negative D70A is more than odd considering the
	      // definition but a Sqrt( -0.0000 ) is detected by Insure so
	      // the addition of this check. 
	      D70A  = D70A - I70A*I70A;
	      if (D70A > 0.0){
		D70A = TMath::Sqrt(D70A)/I70A;
	      } else {
		D70A = 0;
	      }
	      dedx.id_track  =  Id;
	      dedx.det_id    =  kTpcId;    // TPC track 
	      dedx.method    =  kOtherMethodIdentifier; // == kTruncatedMeanId+1;
	      dedx.ndedx     =  N + 100*((int) TrackLengthA);
	      dedx.dedx[0]   =  I70A;
	      dedx.dedx[1]   =  D70A;
	      for (int l = 0; l < 2; l++) {if (tracks[l]) tracks[l]->addPidTraits(new StDedxPidTraits(dedx));}
	    }
	  }
	}
	// likelihood fit
	Double_t chisq, fitZ, fitdZ;
	DoFitZ(chisq, fitZ, fitdZ);
	if (chisq >0 && chisq < 10000.0) {
	  dedx.id_track  =  Id;
	  dedx.det_id    =  kTpcId;    // TPC track 
	  dedx.method    =  kWeightedTruncatedMeanId;// == kLikelihoodFitId+1;
	  dedx.ndedx     =  NdEdx + 100*((int) TrackLength);
	  dedx.dedx[0]   =  TMath::Exp(fitZ);
	  dedx.dedx[1]   =  fitdZ; 
	  if ((TESTBIT(m_Mode, kCalibration)))  // uncorrected dEdx
	    for (int l = 0; l < 2; l++) {if (tracks[l]) tracks[l]->addPidTraits(new StDedxPidTraits(dedx));}
	  if (! TESTBIT(m_Mode, kDoNotCorrectdEdx)) { 
 	    m_TpcdEdxCorrection->dEdxTrackCorrection(StTpcdEdxCorrection::kTpcLengthCorrection,1,dedx); 
 	    m_TpcdEdxCorrection->dEdxTrackCorrection(StTpcdEdxCorrection::kTpcdEdxCor,1,dedx); 
	    dedx.method    =  kLikelihoodFitId;
	    for (int l = 0; l < 2; l++) {if (tracks[l]) tracks[l]->addPidTraits(new StDedxPidTraits(dedx));}
	  }
	}
	if (! TESTBIT(m_Mode, kDoNotCorrectdEdx)) { 
	  StThreeVectorD g3 = gTrack->geometry()->momentum(); // p of global track
	  Double_t pMomentum = g3.mag();
	  Float_t Chisq[NHYPS];
	  for (int hyp = 0; hyp < NHYPS; hyp++) {
	    Double_t bgL10 = TMath::Log10(pMomentum/StProbPidTraits::mPidParticleDefinitions[hyp]->mass());
	    Chisq[hyp] = LikeliHood(bgL10,NdEdx,FdEdx);
	  }
	  for (int l = 0; l < 2; l++) {if (tracks[l]) tracks[l]->addPidTraits(new StProbPidTraits(NdEdx,kTpcId,NHYPS,Chisq));}
	}
      }
    }
    if ((TESTBIT(m_Mode, kCalibration))) Histogramming(gTrack);
    QAPlots(gTrack);
  }
  if (Debug() > 1) {
    gMessMgr->QAInfo() << "StdEdxY2Maker:"
		       << "  Type: " << pEvent->type()
		       << "  Run: " << pEvent->runId() 
		       << "  Event: " << pEvent->id()
		       << "  # track nodes: "
		       << pEvent->trackNodes().size() << endm;
  }
  if (Debug()) {
    timer.stop();
    gMessMgr->QAInfo() << "CPU time for StdEdxY2Maker::Make(): "
		       << timer.elapsedTime() << " sec\n" << endm;
  }
  if (mHitsUsage) mHitsUsage->Fill(TMath::Log10(TotalNoOfTpcHits+1.), TMath::Log10(NoOfTpcHitsUsed+1.));
  return kStOK;
}
//________________________________________________________________________________
void StdEdxY2Maker::SortdEdx() {
  Int_t i;
  TArrayI idxT(NdEdx); Int_t *idx = idxT.GetArray();
  TArrayD dT(NdEdx);   Double_t *d = dT.GetArray();
  for (i = 0; i < NdEdx; i++) d[i] = CdEdx[i].dEdx;
  TMath::Sort(NdEdx,d,idx,0);
  for (i=0;i<NdEdx;i++) dEdxS[i] = CdEdx[idx[i]];
  TArrayI rowT(NdEdx); Int_t *r = rowT.GetArray();
  for (i = 0; i < NdEdx; i++) r[i] = CdEdx[i].row;
  TMath::Sort(NdEdx,r,idx,0);
  for (i=0;i<NdEdx;i++) FdEdx[i] = CdEdx[idx[i]];
}
//________________________________________________________________________________
void StdEdxY2Maker::Histogramming(StGlobalTrack* gTrack) {
  const static Double_t GeV2keV = TMath::Log(1.e-6);
  // Histograms
#if 0
  static TProfile2D *ETA = 0;
  static TH3F *ETA3 = 0;
#endif
  static TH2F *Time = 0, *TimeP = 0, *TimeC = 0;
  static TH3F *Pressure = 0, *PressureC = 0, *PressureA = 0;
  static TH3F *PressureT = 0, *PressureTC = 0, *PressureTA = 0;
  static TH3F *Voltage = 0, *VoltageC = 0;
  static TH3F *AcCharge = 0; 
  static TH3F *AvCurrent = 0; 
  static TH3F *Qcm = 0;
  // ZBGX
  static TH3F **zbgx = 0;
  // end of ZBGX
  static TH3F *MulRow = 0;
  static TH3F *MulRowC = 0;
  static TH3F *Phi3 = 0;
  static TH3F *Phi3D = 0, *Theta3D = 0;
  //  static TH2F *GainMonitor = 0;
  static TH3F *SecRow3 = 0, *SecRow3C = 0;//, *SecRow3A = 0;
  static TH3F *Zdc3C = 0;
  static TH3F *Z3C = 0, *Z3OC = 0;
#if 0
  static TH3F *Z3 = 0, *Z3A = 0;
  static TH3F *Z3O = 0, *Z3OA = 0;
  static TH3F *Z3OW = 0, *Z3OWC = 0, *Z3OWA = 0;
#endif
  // AdcHistos
  static TH2F *AdcI = 0, *AdcO = 0, *AdcIC = 0, *AdcOC = 0, *Adc3I = 0, *Adc3O = 0, *Adc3IC = 0, *Adc3OC = 0;
  static TH3F *AdcIZP = 0, *AdcOZP = 0, *AdcIZN = 0, *AdcOZN = 0;
  static TH2F **Adc3Ip = 0, **Adc3Op = 0;
  // end of AdcHistos
  static TH2F *ZdcCP = 0, *BBCP = 0, *MultiplicityPI = 0, *MultiplicityPO = 0;
  static TH3F *BBC3  = 0;
  //  static TH2F *ctbWest = 0, *ctbEast = 0, *ctbTOFp = 0, *zdcWest = 0, *zdcEast = 0;
  static TH2F *bbcYellowBkg = 0, *bbcBlueBkg = 0;
  // Mip 
  static TH3F *SecRow3Mip = 0;
  // Anode Currents
  // end of Mip
  static TH1F *hdEI = 0, *hdEUI = 0, *hdERI = 0, *hdEPI = 0, *hdETI = 0, *hdESI = 0, *hdEZI = 0, *hdEMI = 0;
  static TH1F *hdEO = 0, *hdEUO = 0, *hdERO = 0, *hdEPO = 0, *hdETO = 0, *hdESO = 0, *hdEZO = 0, *hdEMO = 0;
#if 0
  //  static TH2F *PointsB = 0, *Points70B = 0, *PointsBU = 0, *Points70BU = 0, *Points70BA = 0; 
  //  static TH2F *TPointsB = 0, *TPoints70B = 0, *TPointsBU = 0, *TPoints70BU = 0, *TPoints70BA = 0;
  //  static TH2F *MPointsB = 0, *MPoints70B = 0, *MPointsBU = 0, *MPoints70BU = 0, *MPoints70BA = 0;
  static TH2F *Points[25][5];
#endif
  static TH2F *TPoints[25][5];
#if 0
  static TH2F *MPoints[25][5];
#endif
  static TH2F *hist70[NHYPS][2], *histz[NHYPS][2];
  static TH2F *hist70B[NHYPS][2], *histzB[NHYPS][2];
  static TH2F *hist70BT[NHYPS][2], *histzBT[NHYPS][2];
  static TProfile *histB[NHYPS][2], *histBB[NHYPS][2]; 
  static TH2F *FitPull = 0, *Pull70 = 0;
  static TTree *ftree = 0;
  static TH2F *ffitZ[NHYPS],  *ffitP[NHYPS], *ffitZU = 0, *ffitZU3 = 0, *ffitZA = 0;
  const static Int_t Nlog2dx = 140;
  const static Double_t log2dxLow = 0.0, log2dxHigh = 3.5;
  static TH2F *inputTPCGasPressureP = 0, *nitrogenPressureP = 0, *gasPressureDiffP = 0, *inputGasTemperatureP = 0;
  static TH2F *outputGasTemperatureP = 0, *flowRateArgon1P = 0, *flowRateArgon2P = 0;
  static TH2F *flowRateMethaneP = 0;
  static TH2F *percentMethaneInP = 0, *percentMethaneInPC = 0, *percentMethaneInPA = 0;
  static TH2F *ppmOxygenInP = 0, *flowRateExhaustP = 0;
  static TH2F *flowRateRecirculationP = 0; // *ppmOxygenOutP = 0,
  static TH2F *ppmWaterOutP = 0, *ppmWaterOutPC = 0, *ppmWaterOutPA = 0;
  // ProbabilityPlot
  static TH3F *Prob = 0;
  // end of ProbabilityPlot
  static TH3F *dXdE  = 0, *dXdEA  = 0, *dXdEC  = 0;
  // end of CORRELATION
  static TH2F *BaddEdxZPhi70[2], *BaddEdxZPhiZ[2];
  static TH1F *BaddEdxMult70[2], *BaddEdxMultZ[2];
  static TH3D *Edge3 = 0;
  static dEdxTrackY2 *ftrack = 0;
  static int hMade = 0;
  
  if (! gTrack && !hMade) {
    hMade=2004;
    // book histograms
    Int_t      nZBins = 200;
    Double_t ZdEdxMin = -5;
    Double_t ZdEdxMax =  5;
#if 0
    Z3  = new TH3F("Z3",
		   "log(dEdx/Pion) versus row and Drift Distance",
		   NumberOfRows,0.5, NumberOfRows+0.5,105,0.,210.,nZBins,ZdEdxMin,ZdEdxMax);
    Z3A = new TH3F("Z3A",
		   "log(dEdx/Pion) just after corection versus row and Drift Distance",
		   NumberOfRows,0.5, NumberOfRows+0.5,105,0.,210.,nZBins,ZdEdxMin,ZdEdxMax);
#endif
    Z3C = new TH3F("Z3C",
		   "log(dEdx/Pion) corrected versus row and Drift Distance",
		   NumberOfRows,0.5, NumberOfRows+0.5,105,0.,210.,nZBins,ZdEdxMin,ZdEdxMax);
#if 0
    Z3O = new TH3F("Z3O",
		   "log(dEdx/Pion) versus row and (Drift)*ppmO2In",
		   NumberOfRows,0.5, NumberOfRows+0.5,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
    Z3OA= new TH3F("Z3OA",
		   "log(dEdx/Pion) just after correction versus row and (Drift)*ppmO2In",
		   NumberOfRows,0.5, NumberOfRows+0.5,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
#endif
    Z3OC= new TH3F("Z3OC",
		   "log(dEdx/Pion) corrected versus row and (Drift)*ppmO2In",
		   NumberOfRows,0.5, NumberOfRows+0.5,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
#if 0
    Z3OW = new TH3F("Z3OW",
		    "log(dEdx/Pion) versus row and (Drift)*ppmO2In*ppmWaterOut*",
		    NumberOfRows,0.5, NumberOfRows+0.5,100,0.,1.2e5,nZBins,ZdEdxMin,ZdEdxMax);
    Z3OWA= new TH3F("Z3OWA",
		    "log(dEdx/Pion) just after correction versus row and (Drift)*ppmO2In*ppmWaterOut",
		    NumberOfRows,0.5, NumberOfRows+0.5,100,0.,1.2e5,nZBins,ZdEdxMin,ZdEdxMax);
    Z3OWC= new TH3F("Z3OWC",
		    "log(dEdx/Pion) corrected versus row and (Drift)*ppmO2In*ppmWaterOut",
		    NumberOfRows,0.5, NumberOfRows+0.5,100,0.,1.2e5,nZBins,ZdEdxMin,ZdEdxMax);
#endif
    Edge3 = new TH3D("Edge3",
		     "log(dEdx/Pion) versus row and Edge",
		     NumberOfRows,0.5, NumberOfRows+0.5, 400,-100,100,nZBins,ZdEdxMin,ZdEdxMax);
#if 0
    // eta
    ETA   = new TProfile2D("ETA",
			   "log(dEdx/Pion) versus Sector I/O and #{eta}",
			   NumberOfRows,0.5, NumberOfRows+0.5, 135,-2.25,2.25);
    ETA3  = new TH3F("ETA3",
		     "log(dEdx/Pion) versus Sector I/O and #{eta}",
		     NumberOfRows,0.5, NumberOfRows+0.5, 135,-2.25,2.25,nZBins,ZdEdxMin,ZdEdxMax);
#endif
    SecRow3= new TH3F("SecRow3","<log(dEdx/Pion)> (uncorrected) versus sector and row",
		      numberOfSectors,0.5, numberOfSectors+0.5, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    SecRow3->SetXTitle("Sector number");
    SecRow3->SetYTitle("Row number");
    SecRow3C= new TH3F("SecRow3C","<log(dEdx/Pion)> (corrected) versus sector and row",
		       numberOfSectors,0.5, numberOfSectors+0.5, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    SecRow3C->SetXTitle("Sector number");
    SecRow3C->SetYTitle("Row number");
#if 0
    SecRow3A= new TH3F("SecRow3A","<log(dEdx/Pion)> (just after correction) versus sector and row",
		       numberOfSectors,0.5, numberOfSectors+0.5, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    SecRow3A->SetXTitle("Sector number");
    SecRow3A->SetYTitle("Row number");
#endif
    Zdc3C   = new TH3F("Zdc3C","<log(dEdx/Pion)> versus row and  ZdcCoincidenceRate (log10)",
		       NumberOfRows,0.5, NumberOfRows+0.5,100,0,10,nZBins,ZdEdxMin,ZdEdxMax);
    MultiplicityPI = new TH2F("MultiplicityPI","Multiplicity (log10) Inner",100,0,10,nZBins,ZdEdxMin,ZdEdxMax);
    MultiplicityPO = new TH2F("MultiplicityPO","Multiplicity (log10) Outer",100,0,10,nZBins,ZdEdxMin,ZdEdxMax);
    if ((TESTBIT(m_Mode, kAdcHistos))) {
      gMessMgr->Warning() << "StdEdxY2Maker::Histogramming Adc Histograms" << endm;
      AdcI    = new TH2F("AdcI",
			 "log10dE (keV measured) versus log10dE(Predicted) for Inner rows",
			 120,-5.7,-3.3,185,-7.2,-3.5);
      AdcO    = new TH2F("AdcO",
			 "log10dE (keV measured) versus log10dE(Predicted) for Outer rows",
			 150,-5.5,-2.5,165,-6.5,-3.0);
      AdcIC   = new TH2F("AdcIC",
			 "log10dE (keV measured corrected) versus log10dE(Predicted) for Inner rows",
			 120,-5.7,-3.3,185,-7.2,-3.5);
      AdcOC   = new TH2F("AdcOC",
			 "log10dE (keV measured corrected) versus log10dE(Predicted) for Outer rows",
			 150,-5.5,-2.5,165,-6.5,-3.0);
      Adc3I    = new TH2F("Adc3I",
			  "Uniq 3*sigma log10dE (keV measured) versus log10dE(Predicted) for Inner rows",
			  120,-5.7,-3.3,185,-7.2,-3.5);
      Adc3O    = new TH2F("Adc3O",
			  "Uniq 3*sigma log10dE (keV measured) versus log10dE(Predicted) for Outer rows",
			  150,-5.5,-2.5,165,-6.5,-3.0);
      Adc3IC   = new TH2F("Adc3IC",
			  "Uniq 3*sigma log10dE (keV measured corrected) versus log10dE(Predicted) for Inner rows",
			  120,-5.7,-3.3,185,-7.2,-3.5);
      Adc3OC   = new TH2F("Adc3OC",
			  "Uniq 3*sigma log10dE (keV measured corrected) versus log10dE(Predicted) for Outer rows",
			  150,-5.5,-2.5,165,-6.5,-3.0);
      Adc3Ip    = new TH2F*[NHYPS];
      Adc3Op    = new TH2F*[NHYPS];
      for (int hyp = 0; hyp < NHYPS; hyp++) {
	TString nameP(StProbPidTraits::mPidParticleDefinitions[hyp]->name().data());
	nameP.ReplaceAll("-","");
	Adc3Ip[hyp] = new 
	  TH2F(Form("Adc3I%s",nameP.Data()), 
	       Form("%s Uniq 3*sigma log10dE (keV meas.cor.) versus log10dE(Predicted) for Inner rows",
		    nameP.Data()),
	       120,-5.7,-3.3,185,-7.2,-3.5);
	Adc3Op[hyp] = new 
	  TH2F(Form("Adc3O%s",nameP.Data()), 
	       Form("%s Uniq 3*sigma log10dE (keV meas.cor.) versus log10dE(Predicted) for Outer rows",
		    nameP.Data()),
	       120,-5.7,-3.3,185,-7.2,-3.5);
      }
      AdcIZP    = new TH3F("AdcIZP","z (Positive measured) versus dE(Predicted) and Z for Inner rows",
			   200,0,200,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
      AdcOZP    = new TH3F("AdcOZP","z (Positive measured) versus dE(Predicted) and Z for Outer rows",
			   500,0,500,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
      AdcIZN    = new TH3F("AdcIZN","z (Positive measured) versus dE(Predicted) and Z for Inner rows",
			   200,0,200,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
      AdcOZN    = new TH3F("AdcOZN","z (Positive measured) versus dE(Predicted) and Z for Outer rows",
			   500,0,500,100,0.,1.e4,nZBins,ZdEdxMin,ZdEdxMax);
    } // AdcHistos
    ZdcCP  = new TH2F("ZdcCP","ZdcCoincidenceRate (log10)",100,0,10,nZBins,ZdEdxMin,ZdEdxMax);
    BBCP   = new TH2F("BBCP","BbcCoincidenceRate (log10)",60,0,6,nZBins,ZdEdxMin,ZdEdxMax);
    BBC3   = new TH3F("BBC3","BbcCoincidenceRate (log10) and row ",NumberOfRows,0.5, NumberOfRows+0.5,60,0,6,nZBins,ZdEdxMin,ZdEdxMax);
    bbcYellowBkg = new TH2F("bbcYellowBkg","(BBC Eastdelayed) and (BBC West) (log10)",
			    100,0,10,nZBins,ZdEdxMin,ZdEdxMax);
    bbcBlueBkg   = new TH2F("bbcBlueBkg","(BBC Westdelayed) and (BBC East) (log10)",
			    100,0,10,nZBins,ZdEdxMin,ZdEdxMax);
    if ((TESTBIT(m_Mode, kMip))) {
      gMessMgr->Warning() << "StdEdxY2Maker::Histogramming Mip Histograms" << endm;
      SecRow3Mip = new TH3F
	("SecRow3Mip",
	 "<log(dEdx/Pion)>/sigma (corrected) versus row and log2(dx) for MIP particle)",
	 NumberOfRows,0.5, NumberOfRows+0.5,Nlog2dx, log2dxLow, log2dxHigh, 200,-5,15);
    } // Mip
    MulRow = new TH3F("MulRow","log(dEdx/Pion) versus log10(Multplicity) and row",
		      100,0,10, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    MulRowC = new TH3F("MulRowC","log(dEdx/Pion) versus log10(Multplicity) and row corrected",
		       100,0,10, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    Phi3    = new TH3F("Phi3","log(dEdx/Pion) versus Phi (coordinates, relative) (degrees) and row",
		       210,-1.05,1.05, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    Phi3D   = new TH3F("Phi3D","log(dEdx/Pion) versus Phi (direction) and row",
		       480,-60,60, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    Theta3D   = new TH3F("Theta3D","log(dEdx/Pion) versus Theta (direction) (degrees) and row",
			 560,-60,80, NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    const Char_t *N[5] = {"B","70B","BU","70BU","70BA"};
    const Char_t *T[5] = {"dEdx(fit)/Pion",
			  "dEdx(fit_uncorrected)/Pion ",
			  "dEdx(I70)/Pion",
			  "dEdx(I70_uncorrected)/Pion",
			  "dEdx(I70A_uncorrected)/Pion"};
    
    Int_t NZ = 1;
    for (Int_t t = 0; t < 5; t++) {
      for (Int_t z = 0; z < NZ; z++) {
	TString ZN("");
	TString ZT("all");
	if (z > 0) {
	  ZN = Form("%02i",z);
	  ZT = Form("Sector %02i",z);
	}
#if 0
	Points[z][t]    = new TH2F(Form("Points%s%s",N[t],ZN.Data()),
				   Form("%s/sigma versus no. of measured points for %s",T[t],ZT.Data()),
				   50,0,50., 500,-5.,20.);
	MPoints[z][t]   = new TH2F(Form("MPoints%s%s",N[t],ZN.Data()),
				   Form("%s versus no. of measured points for %s",T[t],ZT.Data()),
				   150,10,160., 500,-1.,4.);
#endif
	TPoints[z][t]   = new TH2F(Form("TPoints%s%s",N[t],ZN.Data()),
				   Form("%s versus no. of measured points for %s",T[t],ZT.Data()),
				   150,10,160., 500,-1.,4.);
      }
    }
    if ((TESTBIT(m_Mode, kZBGX))) {
      gMessMgr->Warning() << "StdEdxY2Maker::Histogramming make zbgx histograms" << endm;
      zbgx = new TH3F*[2*NHYPS]; 
    }
    for (int hyp=0; hyp<NHYPS;hyp++) {
      TString nameP("fit");
      nameP += StProbPidTraits::mPidParticleDefinitions[hyp]->name().data();
      nameP.ReplaceAll("-","");
      TString title = "fitZ - Pred. for ";
      title += StProbPidTraits::mPidParticleDefinitions[hyp]->name().data();
      title.ReplaceAll("-","");
      title += " versus log10(beta*gamma) for pion";
      ffitZ[hyp]  = new TH2F(nameP.Data(),title.Data(),100,-1,4,100,-5,5);
      ffitZ[hyp]->SetMarkerColor(hyp+2);
      ffitP[hyp] = new TH2F(*ffitZ[hyp]);
      nameP.ReplaceAll("fit","fitP");
      ffitP[hyp]->SetName(nameP.Data());
      for (int sCharge = 0; sCharge < 2; sCharge++) {
	if ((TESTBIT(m_Mode, kZBGX))) {
	  nameP = "zbgx";
	  nameP += StProbPidTraits::mPidParticleDefinitions[hyp]->name().data();
	  nameP.ReplaceAll("-","");
	  if (sCharge == 0) nameP += "P";
	  else              nameP += "N";
	  if (zbgx) 
	    zbgx[2*hyp+sCharge] = new TH3F(nameP.Data(),"z = log(dE/dx) versus log10(beta*gamma) and log2(dx) for unique hyps",
					   100,-1,4,Nlog2dx,log2dxLow,log2dxHigh,320,-2,6);
	} // ZBGX
	nameP = StProbPidTraits::mPidParticleDefinitions[hyp]->name().data();
	nameP.ReplaceAll("-","");
	if (sCharge == 0) nameP += "P";
	else              nameP += "N";
	TString name = nameP;
	name += "70";
	title = "log(dE/dx70/I(";
	title += nameP;
	title += ")) versus log10(p/m)";
	hist70[hyp][sCharge] = new TH2F(name.Data(),title.Data(),100,-1.,4.,600,-2.,4.);
	name += "B";
	title += " Bichsel";
	hist70B[hyp][sCharge] = new TH2F(name.Data(),title.Data(),100,-1.,4.,600,-2.,4.);
	name += "T";
	title += " Unique";
	hist70BT[hyp][sCharge] = new TH2F(name.Data(),title.Data(),100,-1.,4.,600,-2.,4.);
	name = nameP;
	name += "z";
	title = "zFit - log(I(";
	title += nameP;
	title += ")) versus log10(p/m)";
	histz[hyp][sCharge] = new TH2F(name.Data(),title.Data(),100,-1.,4.,600,-2.,4.);
	name += "B";
	title += " Bichsel";
	histzB[hyp][sCharge] = new TH2F(name.Data(),title.Data(),100,-1.,4.,600,-2.,4.);
	name += "T";
	title += " Unique";
	histzBT[hyp][sCharge] = new TH2F(name.Data(),title.Data(),100,-1.,4.,600,-2.,4.);
	name = nameP;
	name += "B";
	title = "log(I_{Sirrf}(";
	title += nameP;
	title += ")) versus log10(p/m)";
	histB[hyp][sCharge] = new TProfile(name.Data(),title.Data(),100,-1.,4.);
	name += "B";
	title = "log(I_{BB}(";
	title += nameP;
	title += ")) versus log10(p/m) Bichsel";
	histBB[hyp][sCharge] = new TProfile(name.Data(),title.Data(),100,-1.,4.);
      }
    }
    TDatime t1(tMin,0); // min Time and
    TDatime t2(tMax,0); // max 
    
    UInt_t i1 = t1.Convert() - timeOffSet;
    UInt_t i2 = t2.Convert() - timeOffSet;
    Int_t Nt = (i2 - i1)/(3600); // each hour 
    Pressure   = new TH3F("Pressure","log(dE/dx)_{uncorrected} - log(I(pi)) versus Row & Log(Pressure)", 
			  NumberOfRows,0.5, NumberOfRows+0.5,150, 6.84, 6.99,nZBins,ZdEdxMin,ZdEdxMax);
    PressureA  = new TH3F("PressureA","log(dE/dx)_{just after correction} log(I(pi)) versus Log(Pressure)", 
			  NumberOfRows,0.5, NumberOfRows+0.5,150, 6.84, 6.99,nZBins,ZdEdxMin,ZdEdxMax);
    PressureC  = new TH3F("PressureC","log(dE/dx)_{corrected} - row & log(I(pi)) versus Log(Pressure)", 
			  NumberOfRows,0.5, NumberOfRows+0.5,150, 6.84, 6.99,nZBins,ZdEdxMin,ZdEdxMax);
    PressureT   = new TH3F("PressureT","log(dE/dx)_{uncorrected} - log(I(pi)) versus Row & Log(Pressure*298.2/outputGasTemperature)", 
			   NumberOfRows,0.5, NumberOfRows+0.5,150, 6.84, 6.99,nZBins,ZdEdxMin,ZdEdxMax);
    PressureTA  = new TH3F("PressureTA","log(dE/dx)_{just after correction} log(I(pi)) versus Log(Pressure*298.2/outputGasTemperature)", 
			   NumberOfRows,0.5, NumberOfRows+0.5,150, 6.84, 6.99,nZBins,ZdEdxMin,ZdEdxMax);
    PressureTC  = new TH3F("PressureTC","log(dE/dx)_{corrected} - row & log(I(pi)) versus Log(Pressure*298.2/outputGasTemperature)", 
			   NumberOfRows,0.5, NumberOfRows+0.5,150, 6.84, 6.99,nZBins,ZdEdxMin,ZdEdxMax);
    Voltage   = new TH3F("Voltage","log(dE/dx)_{uncorrected} - log(I(pi)) versus Sector*Row and Voltage - Voltage_{nominal}", 
			 numberOfSectors*NumberOfRows,0.5, numberOfSectors*NumberOfRows+0.5,6,-110,10,nZBins,ZdEdxMin,ZdEdxMax);
    VoltageC  = new TH3F("VoltageC","log(dE/dx)_{corrected} - row & log(I(pi)) versus Row and Voltage", 
			 NumberOfRows,0.5, NumberOfRows+0.5,410,990.,1400.,nZBins,ZdEdxMin,ZdEdxMax);
    AcCharge  = new TH3F("AcCharge","log(dE/dx)_{corrected} - row & log(I(pi)) versus Row and Accumulated Charge [C]", 
			 NumberOfRows,0.5, NumberOfRows+0.5,110,5.,100.,nZBins,ZdEdxMin,ZdEdxMax);
    AvCurrent  = new TH3F("AvCurrent","log(dE/dx)_{corrected} - row & log(I(pi)) versus Row and Average Current [#{mu}A]", 
			 NumberOfRows,0.5, NumberOfRows+0.5,200,0.,200.,nZBins,ZdEdxMin,ZdEdxMax);
    Qcm  = new TH3F("Qcm","log(dE/dx)_{corrected} - row & log(I(pi)) versus Row and Accumulated Charge [uC/cm]", 
		    NumberOfRows,0.5, NumberOfRows+0.5,500,0.,1000.,nZBins,ZdEdxMin,ZdEdxMax);
    //     GainMonitor  = new TH2F("GainMonitor","log(dE/dx)_{corrected} - log(I(pi)) versus GainMonitor", 
    // 			    100,70.,120.,nZBins,ZdEdxMin,ZdEdxMax);
    Time   = new TH2F("Time","log(dE/dx)_{uncorrected} - log(I(pi)) versus Date& Time", 
		      Nt,i1,i2,nZBins,ZdEdxMin,ZdEdxMax);
    TimeC  = new TH2F("TimeC","log(dE/dx)_{corrected} - log(I(pi)) versus Date& Time after correction", 
		      Nt,i1,i2,nZBins,ZdEdxMin,ZdEdxMax);
    TimeP  = new TH2F("TimeP","log(dE/dx)_{after pressure correction} - log(I(pi)) versus Date& Time", 
		      Nt,i1,i2,nZBins,ZdEdxMin,ZdEdxMax);
    FitPull= new TH2F("FitPull","(zFit - log(I(pi)))/dzFit  versus track length", 
		      150,10.,160,nZBins,ZdEdxMin,ZdEdxMax);
    Pull70 = new TH2F("Pull70","log(I70/I(pi)))/D70  versus track length", 
		      150,10.,160,nZBins,ZdEdxMin,ZdEdxMax);
    TString title("");
    title = "log(dE/dx/Pion) vs inputTPCGasPressure (mbar)";
    inputTPCGasPressureP = new TH2F("inputTPCGasPressureP","log(dE/dx/Pion) vs inputTPCGasPressure (mbar)",100,2.0,2.2,nZBins,ZdEdxMin,ZdEdxMax);
    nitrogenPressureP = new TH2F("nitrogenPressureP","log(dE/dx/Pion) vs nitrogenPressure (mbar)",100,0.9,1.1,nZBins,ZdEdxMin,ZdEdxMax);
    gasPressureDiffP = new TH2F("gasPressureDiffP","log(dE/dx/Pion) vs gasPressureDiff (mbar)",100,0.6,1.,nZBins,ZdEdxMin,ZdEdxMax);
    inputGasTemperatureP = new TH2F("inputGasTemperatureP","log(dE/dx/Pion) vs inputGasTemperature (degrees C)",100,295.,300.,nZBins,ZdEdxMin,ZdEdxMax);
    outputGasTemperatureP = new TH2F("outputGasTemperatureP","log(dE/dx/Pion) vs outputGasTemperature (degrees C)",100,295.,300.,nZBins,ZdEdxMin,ZdEdxMax);
    flowRateArgon1P = new TH2F("flowRateArgon1P","log(dE/dx/Pion) vs flowRateArgon1 (liters/min)",100,14.95,15.0,nZBins,ZdEdxMin,ZdEdxMax);
    flowRateArgon2P = new TH2F("flowRateArgon2P","log(dE/dx/Pion) vs flowRateArgon2 (liters/min)",100,0.,0.25,nZBins,ZdEdxMin,ZdEdxMax);
    flowRateMethaneP = new TH2F("flowRateMethaneP","log(dE/dx/Pion) vs flowRateMethane (liters/min)",100,1.34,1.37,nZBins,ZdEdxMin,ZdEdxMax);
    percentMethaneInP = new TH2F("percentMethaneInP","log(dE/dx/Pion) vs percentMethaneIn (percent)",100,9.6,10.6,nZBins,ZdEdxMin,ZdEdxMax);
    percentMethaneInPC = new TH2F("percentMethaneInPC","log(dE/dx/Pion)(corrected) vs percentMethaneIn (percent)",100,9.6,10.6,nZBins,ZdEdxMin,ZdEdxMax);
    percentMethaneInPA = new TH2F("percentMethaneInPA","log(dE/dx/Pion)(just after correction) vs percentMethaneIn (percent)",
				  100,9.6,10.6,nZBins,ZdEdxMin,ZdEdxMax);
    ppmOxygenInP = new TH2F("ppmOxygenInP","log(dE/dx/Pion) vs ppmOxygenIn (ppm)",240,0.,60.,nZBins,ZdEdxMin,ZdEdxMax);
    flowRateExhaustP = new TH2F("flowRateExhaustP","log(dE/dx/Pion) vs flowRateExhaust (liters/min)",100,5.,20.,nZBins,ZdEdxMin,ZdEdxMax);
    ppmWaterOutP = new TH2F("ppmWaterOutP","log(dE/dx/Pion) vs ppmWaterOut (ppm)",100,0.,20.,nZBins,ZdEdxMin,ZdEdxMax);
    ppmWaterOutPC = new TH2F("ppmWaterOutPC","log(dE/dx/Pion) corrected vs ppmWaterOut (ppm)",100,0.,20.,nZBins,ZdEdxMin,ZdEdxMax);
    ppmWaterOutPA = new TH2F("ppmWaterOutPA","log(dE/dx/Pion) just after correction vs ppmWaterOut (ppm)",100,0.,20.,nZBins,ZdEdxMin,ZdEdxMax);
    //    ppmOxygenOutP = new TH2F("ppmOxygenOutP","log(dE/dx/Pion) vs ppmOxygenOut (ppm)",100,0,20,nZBins,ZdEdxMin,ZdEdxMax);
    flowRateRecirculationP = new TH2F("flowRateRecirculationP","log(dE/dx/Pion) vs flowRateRecirculation (liters/min)",
				      100,515.,545.,nZBins,ZdEdxMin,ZdEdxMax);
    ffitZU = new TH2F("fitZU","fitZ - PredPi Unique versus log10(beta*gamma)",100,-1,4,100,-5,5);
    ffitZU->SetMarkerColor(7);
    ffitZU3 = new TH2F("fitZU3","fitZ - PredPi Unique and 3 sigma away versus log10(beta*gamma)",100,-1,4,100,-5,5);
    ffitZU3->SetMarkerColor(6);
    ffitZA = new TH2F("fitZA","fitZ - PredPi All versus log10(beta*gamma)",100,-1,4,100,-5,5);
    ffitZA->SetMarkerColor(1);
    hdEI  = new TH1F("hdEI","log10(dE) Inner after calibration",100,-8.,-3.);
    hdEUI = new TH1F("hdEUI","log10(dEU) Inner before correction",100,-8.,-3.);
    hdERI = new TH1F("hdERI","log10(dER) Inner after row correction  correction",100,-8.,-3.);
    hdEPI = new TH1F("hdEPI","log10(dEP) Inner after Pressure correction",100,-8.,-3.);
    hdETI = new TH1F("hdETI","log10(dET) Inner after TimeScale",100,-8.,-3.);
    hdESI = new TH1F("hdESI","log10(dES) Inner after after TimeScale + SecRow corrections",100,-8.,-3.);
    hdEZI = new TH1F("hdEZI","log10(dEZ) Inner after TimeScale + SecRow + Sec Z corrections ",100,-8.,-3.);
    hdEMI = new TH1F("hdEMI","log10(dEM) Inner after TimeScale + SecRow + Sec Z + Multiplicity corrections",100,-8.,-3.);
    hdEO  = new TH1F("hdEO","log10(dE) Outer after calibration",100,-8.,-3.);
    hdEUO = new TH1F("hdEUO","log10(dEU) Outer before correction",100,-8.,-3.);
    hdERO = new TH1F("hdERO","log10(dER) Outer after row correction  correction",100,-8.,-3.);
    hdEPO = new TH1F("hdEPO","log10(dEP) Outer after Pressure correction",100,-8.,-3.);
    hdETO = new TH1F("hdETO","log10(dET) Outer after TimeScale",100,-8.,-3.);
    hdESO = new TH1F("hdESO","log10(dES) Outer after after TimeScale + SecRow corrections",100,-8.,-3.);
    hdEZO = new TH1F("hdEZO","log10(dEZ) Outer after TimeScale + SecRow + Sec Z corrections ",100,-8.,-3.);
    hdEMO = new TH1F("hdEMO","log10(dEM) Outer after TimeScale + SecRow + Sec Z + Multiplicity corrections",100,-8.,-3.);
    if ((TESTBIT(m_Mode, kProbabilityPlot))) {
      gMessMgr->Warning() << "StdEdxY2Maker::Histogramming Probability Histograms" << endm;
      Prob = new TH3F("Prob","Z(=log(I70/Bichsel)) versun log10(bg) for pion and Probability",
		      100,-1.,4.,10*NHYPS+1,-.1,NHYPS,600,-2.,4.);
    } // ProbabilityPlot
    dXdE  = new TH3F("dXdE","log(dEdx/Pion) versus dX and row",
		     100,0.,5., NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    dXdEA = new TH3F("dXdEA","log(dEdx/Pion) just after correction versus dX and row",
		     100,0.,5., NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    dXdEC = new TH3F("dXdEC","log(dEdx/Pion) corrected versus dX and row",
		     100,0.,5., NumberOfRows,0.5, NumberOfRows+0.5,nZBins,ZdEdxMin,ZdEdxMax);
    //    Z3->SetTitle(Form("%s p in [%4f.1,%4f.1]",Z3->GetTitle(),pMomin,pMomax);
    // Create a ROOT Tree and one superbranch
    BaddEdxZPhi70[0] = new TH2F("BaddEdxZPhi700","Z and Phi for I70 below any limits by 5 s.d.",210,-210,210,360,-180.,180.);
    BaddEdxZPhi70[1] = new TH2F("BaddEdxZPhi701","Z and Phi for I70 above any limits by 5 s.d.",210,-210,210,360,-180.,180.);
    BaddEdxMult70[0] = new TH1F("BaddEdxMult700","Multiplicity (log10) for I70 below any limits by 5 s.d.",100,0.,10.);
    BaddEdxMult70[1] = new TH1F("BaddEdxMult701","Multiplicity (log10) for I70 above any limits by 5 s.d.",100,0.,10.);
    BaddEdxZPhiZ[0] = new TH2F("BaddEdxZPhiZ0","Z and Phi for Ifit below any limits by 5 s.d.",210,-210,210,360,-180.,180.);
    BaddEdxZPhiZ[1] = new TH2F("BaddEdxZPhiZ1","Z and Phi for Ifit above any limits by 5 s.d.",210,-210,210,360,-180.,180.);
    BaddEdxMultZ[0] = new TH1F("BaddEdxMultZ0","Multiplicity (log10) for Ifit below any limits by 5 s.d.",100,0.,10.);
    BaddEdxMultZ[1] = new TH1F("BaddEdxMultZ1","Multiplicity (log10) for Ifit above any limits by 5 s.d.",100,0.,10.);
    if ((TESTBIT(m_Mode, kMakeTree))&& !ftree) { 
      gMessMgr->Warning() << "StdEdxY2Maker::Histogramming Make Tree" << endm;
      ftree = new TTree("dEdxT","dEdx tree");
      ftree->SetAutoSave(1000000000);  // autosave when 1 Gbyte written
      Int_t bufsize = 64000;
      Int_t split = 99;
      if (split)  bufsize /= 4;
      ftrack = new dEdxTrackY2();
      TTree::SetBranchStyle(1); //new style by default
      TBranch *branch = ftree->Branch("dEdxTrackY2", "dEdxTrackY2", &ftrack, bufsize,split);
      branch->SetAutoDelete(kFALSE);
    }
    return;
  }
  // fill histograms 
  tpcGas_st           *tpcGas = 0;
  if ( m_TpcdEdxCorrection && m_TpcdEdxCorrection->tpcGas()) tpcGas = m_TpcdEdxCorrection->tpcGas()->GetTable();
  
  StThreeVectorD g3 = gTrack->geometry()->momentum(); // p of global track
  Double_t pMomentum = g3.mag();
  Int_t sCharge = 0;
  if (gTrack->geometry()->charge() < 0) sCharge = 1;
  //  StTpcDedxPidAlgorithm tpcDedxAlgo;
  // dE/dx
  //  StPtrVecTrackPidTraits traits = gTrack->pidTraits(kTpcId);
  StDedxPidTraits *pid, *pid70 = 0, *pidF = 0, *pid70U = 0, *pidFU = 0, *pid70A = 0;
  StProbPidTraits *pidprob = 0, *p = 0;
#ifdef CompareWithToF
  Int_t NoFitPoints = gTrack->fitTraits().numberOfFitPoints(kTpcId);
  StTofPidTraits* pidTof  = 0;
  static const Int_t IdxH[4] = {kPidProton,kPidKaon,kPidPion,kPidElectron};
#endif
  Double_t I70 = 0, D70 = 0, I70U = 0, D70U = 0, I70A = 0, D70A = 0;
  Double_t fitZ = 0, fitdZ = 1e10, fitZU = 0, fitdZU = 1e10;
  Int_t N70 = 0, NF = 0, N70A = 0;
  StSPtrVecTrackPidTraits &traits = gTrack->pidTraits();
  Double_t TrackLength70 = 0, TrackLength = 0;
  UInt_t size = traits.size();
  if (size) {
    for (UInt_t i = 0; i < size; i++) {
      if (! traits[i]) continue;
      if ( traits[i]->IsZombie()) continue;
      pid = dynamic_cast<StDedxPidTraits*>(traits[i]);
      if (pid && pid->detector() == kTpcId) {
	switch (pid->method()) {
	case kTruncatedMeanId:
	  pid70 = pid; I70 = pid70->mean(); N70 = pid70->numberOfPoints();
	  TrackLength70 = pid70->length(); D70 = pid70->errorOnMean();
	  break;
	case kEnsembleTruncatedMeanId: // == kTruncatedMeanId+1 uncorrected
	  pid70U = pid; I70U = pid70U->mean(); D70U = pid70U->errorOnMean();
	  break;
	case kLikelihoodFitId:
	  pidF = pid;
	  fitZ = TMath::Log(pidF->mean()); NF = pidF->numberOfPoints(); 
	  TrackLength = pidF->length(); fitdZ = pidF->errorOnMean(); 
	  break;
	case kWeightedTruncatedMeanId: // == kLikelihoodFitId+1; uncorrected
	  pidFU = pid; fitZU = TMath::Log(pidFU->mean()); fitdZU = pidFU->errorOnMean(); 
	  break;
	case kOtherMethodIdentifier:
	  pid70A = pid; I70A = pid70A->mean(); D70A = pid70A->errorOnMean(); N70A = pid70A->numberOfPoints();
	  break;
	default:
	  break;
	}
      } else {
	p = dynamic_cast<StProbPidTraits*>(traits[i]);
	if (p) {pidprob = p; continue;}
      }
    }
  }
#ifdef CompareWithToF
  // Tof for primary track only
  StTrackNode *node = gTrack->node();
  if (node) {
    StPrimaryTrack *pTrack = static_cast<StPrimaryTrack*>(node->track(primary));
    if (pTrack) {
      StSPtrVecTrackPidTraits &traitsp = pTrack->pidTraits();
      UInt_t size = traitsp.size();
      for (UInt_t i = 0; i < size; i++) {
	if (traitsp[i]->detector() != kTofId) continue;
	StTofPidTraits* p = dynamic_cast<StTofPidTraits*>(traitsp[i]);
	if (p) {pidTof = p;}
      }
    }
  }
#endif  
  if (pid70 && ! pidF) TrackLength = TrackLength70;
  Double_t Pred[NHYPS],  Pred70[NHYPS];
  Double_t PredB[NHYPS], Pred70B[NHYPS];
  Double_t PredBMN[2], Pred70BMN[2]; 
  Double_t date = GetDateTime().Convert() - timeOffSet;
  Double_t devZ[NHYPS], devZs[NHYPS], devToF[NHYPS];
  memset (devZ, 0, NHYPS*sizeof(Double_t));
  memset (devZs, 0, NHYPS*sizeof(Double_t));
  memset (devToF, 0, NHYPS*sizeof(Double_t));
  Double_t bg = TMath::Log10(pMomentum/StProbPidTraits::mPidParticleDefinitions[kPidPion]->mass());
  Double_t bghyp[NHYPS];
  Int_t l;
  PredBMN[0] = Pred70BMN[0] =  1;
  PredBMN[1] = Pred70BMN[1] = -1;
  for (l = kPidElectron; l < NHYPS; l += 1) {
    bghyp[l] = TMath::Log10(pMomentum/StProbPidTraits::mPidParticleDefinitions[l]->mass());
    PredB[l]   = 1.e-6*TMath::Exp(m_Bichsel->GetMostProbableZ(bghyp[l],1.0)); 
    if (PredB[l] < PredBMN[0]) PredBMN[0] = PredB[l];
    if (PredB[l] > PredBMN[1]) PredBMN[1] = PredB[l];
    Pred70B[l] = 1.e-6*m_Bichsel->GetI70(bghyp[l],1.0); 
    if (Pred70B[l] < Pred70BMN[0]) Pred70BMN[0] = Pred70B[l];
    if (Pred70B[l] > Pred70BMN[1]) Pred70BMN[1] = Pred70B[l];
    Pred[l] = 1.e-6*BetheBloch::Sirrf(pMomentum/StProbPidTraits::mPidParticleDefinitions[l]->mass(),60.,l==3); 
    Pred70[l] = Pred[l];
    
    if (pid70 && TrackLength70 > 40.) {
      hist70[l][sCharge]->Fill(bghyp[l],TMath::Log(I70/Pred70[l]));
      histB[l][sCharge]->Fill(bghyp[l],TMath::Log(Pred[l]));
      hist70B[l][sCharge]->Fill(bghyp[l],TMath::Log(I70/Pred70B[l]));
      histBB[l][sCharge]->Fill(bghyp[l],TMath::Log(PredB[l]));
      devZ[l] = TMath::Log(I70/Pred70B[l]);
    }
    if (pidF && TrackLength > 40.) {
      histz[l][sCharge]->Fill(bghyp[l],fitZ - TMath::Log(Pred[l]));
      histzB[l][sCharge]->Fill(bghyp[l],fitZ - TMath::Log(PredB[l]));
      devZs[l] = TMath::Abs(devZ[l])/fitdZ; //D70;
    }
  }
  Double_t devZmin = 999;
  Int_t PiDStatus = 0;
  Int_t  PiDkey   = -1; // best
  Int_t  PiDkeyU  = -1; // only one with devZs<3, 
  Int_t  PiDkeyU3 = -1; // -"- and devZs > 5 for all others 
  Int_t lBest  = -1;
  if (pidF && TrackLength > 40.) {
    // Bad dE/dx
    Double_t L10Mult = -1;
    if (St_trigDetSumsC::instance()) L10Mult = St_trigDetSumsC::instance()->mult();
    //    StThreeVectorD pxyz = gTrack->geometry()->momentum();
    StThreeVectorD  xyz = gTrack->geometry()->helix().at(0);
    Double_t ZG  = xyz.z();
    Double_t PhiDG = 180*xyz.phi();
    if (Pred70BMN[1] > 0 && D70 > 0) {
      if (TMath::Log(I70/Pred70BMN[0]) < -5*D70) {
	BaddEdxZPhi70[0]->Fill(ZG,PhiDG);
	BaddEdxMult70[0]->Fill(L10Mult);
      }
      if (TMath::Log(I70/Pred70BMN[1]) > 5*D70) {
	BaddEdxZPhi70[1]->Fill(ZG,PhiDG);
	BaddEdxMult70[1]->Fill(L10Mult);
      }
    }
    if (PredBMN[1] > 0 && fitdZ > 0) {
      if (fitZ - TMath::Log(PredBMN[0]) < -5*fitdZ) {
	BaddEdxZPhiZ[0]->Fill(ZG,PhiDG);
	BaddEdxMultZ[0]->Fill(L10Mult);
      }
      if (fitZ - TMath::Log(PredBMN[1]) > 5*fitdZ) {
	BaddEdxZPhiZ[1]->Fill(ZG,PhiDG);
	BaddEdxMultZ[1]->Fill(L10Mult);
      }
    }
#ifdef CompareWithToF
    // use ToF 
    if (pidTof) {
      for (l = kPidElectron; l < NHYPS; l++) {
	switch (l) {
	case kPidElectron:
	  devToF[l] = pidTof->sigmaElectron();
	  break;
	case kPidProton:
	  devToF[l] = pidTof->sigmaProton();
	  break;
	case kPidKaon:
	  devToF[l] = pidTof->sigmaKaon();
	  break;
	case kPidPion:
	  devToF[l] = pidTof->sigmaPion();
	  break;
	default:
	  devToF[l] = 999.;
	  break;
	}
	devToF[l] = TMath::Abs(devToF[l]);
      }
      for (l = kPidElectron; l < NHYPS; l++) {
	if (l == kPidMuon) continue;
	if (devToF[l] < 3.0) PiDStatus += 1<<l;
	if (devToF[l] < devZmin) {devZmin = devToF[l]; lBest = l;}
      }
      if (lBest >=0) {
	if (devToF[lBest] < 3.0) {
	  PiDkey = lBest;
	  Int_t lNext = -1;
	  devZmin = 999;
	  for (l = kPidElectron; l < NHYPS; l += 1) {
	    if (l == kPidMuon) continue;
	    if (l == lBest) continue;
	    if (devToF[l] < devZmin) {devZmin = devToF[l]; lNext = l;}
	  }
	  if (lNext >= 0) {
	    if (devToF[lNext] > 3.) {
	      PiDkeyU = PiDkey;
	      if (devToF[lNext] > 5.) PiDkeyU3 = PiDkeyU;
	      //	    else  {if (devZs[lBest] < devZs[lNext] + 5.) PiDkeyU3 = lBest;}
	    }
	  }
	}
      }
    }
#else  
    for (l = kPidElectron; l < NHYPS; l++) {
      if (l == kPidMuon) continue;
      if (devZs[l] < 3.0) PiDStatus += 1<<l;
      if (devZs[l] < devZmin) {devZmin = devZs[l]; lBest = l;}
    }
    if (lBest >=0) {
      if (devZs[lBest] < 3.0) {
	PiDkey = lBest;
	Int_t lNext = -1;
	devZmin = 999;
	for (l = kPidElectron; l < NHYPS; l += 1) {
	  if (l == kPidMuon) continue;
	  if (l == lBest) continue;
	  if (devZs[l] < devZmin) {devZmin = devZs[l]; lNext = l;}
	}
	if (lNext >= 0) {
	  if (devZs[lNext] > 3.) {
	    PiDkeyU = PiDkey;
	    if (devZs[lNext] > 5.) PiDkeyU3 = PiDkeyU;
	    //	    else  {if (devZs[lBest] < devZs[lNext] + 5.) PiDkeyU3 = lBest;}
	  }
	}
      }
    }
#endif
  }
  if (PiDkeyU3 >= 0) {
    l = PiDkeyU3;
    if (pid70 && TrackLength70 > 40.) {
      hist70BT[l][sCharge]->Fill(bghyp[l],TMath::Log(I70/Pred70B[l]));
    }
    if (pidF && TrackLength > 40.) {
      histzBT[l][sCharge]->Fill(bghyp[l],fitZ - TMath::Log(PredB[l]));
    }
  }
  if ((TESTBIT(m_Mode, kProbabilityPlot))) {
    if (TrackLength70 > 40) { 
      Double_t Z70 = TMath::Log(I70/Pred70[kPidPion]);
      Prob->Fill(bghyp[kPidPion],-0.05,Z70);
      if (pidprob) {
	Int_t N = pidprob->GetPidArray()->GetSize();
	for (int i = 0; i < N; i++) {
	  Double_t p = pidprob->GetProbability(i);
	  if (p > 0.99) p = 0.99;
	  Prob->Fill(bghyp[kPidPion],i+p,Z70);
	}
      }
    }
  } // ProbabilityPlot
  //  if (D70 > 0.05 && D70 < 0.15) {
  if (pidF && fitdZ > 0.05 && fitdZ < 0.15) {
    ffitZA->Fill(bg,devZ[kPidPion]);
    for (l = kPidElectron; l < NHYPS; l += 1) {
      ffitZ[l]->Fill(bg,devZ[kPidPion]);
      if (pidprob && pidprob->GetSum() > 0 && pidprob->GetProbability(l) > 0.9) ffitP[l]->Fill(bg,devZ[kPidPion]);
      if (PiDkeyU  >= 0) ffitZU->Fill(bghyp[kPidPion],devZ[kPidPion]);
      if (PiDkeyU3 >= 0) ffitZU3->Fill(bghyp[kPidPion],devZ[kPidPion]);
    }
  }
  if (Pred[kPidPion] <= 0) {
    gMessMgr->Warning() << "StdEdxY2Maker:: Prediction for p = " 
			<< pMomentum << " and TrackLength = " << TrackLength 
			<< " is wrong = " << Pred[kPidPion] << " <<<<<<<<<<<<<" << endl;
    return;
  };
  if (pidF) {
    TPoints[0][0]->Fill(TrackLength,fitZ-TMath::Log(PredB[kPidPion]));
#if 0
    Points[0][0]->Fill(NdEdx,(fitZ-TMath::Log(PredB[kPidPion]))/fitdZ);
    if (pMomentum > pMomin && pMomentum < pMomax) MPoints[0][0]->Fill(TrackLength,fitZ-TMath::Log(PredB[kPidPion]));
#endif
    FitPull->Fill(TrackLength,(fitZ - TMath::Log(PredB[kPidPion]))/fitdZ);
    if (pidFU) {
      TPoints[0][2]->Fill(TrackLength,fitZU-TMath::Log(PredB[kPidPion]));
#if 0
      Points[0][2]->Fill(NdEdx,(fitZU-TMath::Log(PredB[kPidPion]))/fitdZ);
      if (pMomentum > pMomin && pMomentum < pMomax) MPoints[0][2]->Fill(TrackLength,fitZU-TMath::Log(PredB[kPidPion]));
#endif
    }
  }
  if (pid70) {
    TPoints[0][1]->Fill(TrackLength,TMath::Log(I70/Pred70B[kPidPion]));
#if 0
    Points[0][1]->Fill(N70,TMath::Log(I70/PredB[kPidPion])/D70);
    if (pMomentum > pMomin && pMomentum < pMomax) MPoints[0][1]->Fill(TrackLength,TMath::Log(I70/Pred70B[kPidPion]));
#endif
    Pull70->Fill(TrackLength,TMath::Log(I70/Pred70B[kPidPion])/D70);
    if (pid70U) {
      TPoints[0][3]->Fill(TrackLength,TMath::Log(I70U/Pred70B[kPidPion]));
#if 0
      Points[0][3]->Fill(N70,TMath::Log(I70U/PredB[kPidPion])/D70);
      if (pMomentum > pMomin && pMomentum < pMomax) MPoints[0][3]->Fill(TrackLength,TMath::Log(I70U/Pred70B[kPidPion]));
#endif
    }
    if (pid70A) {
      TPoints[0][4]->Fill(TrackLength,TMath::Log(I70A/Pred70B[kPidPion]));
#if 0
      Points[0][4]->Fill(N70A,TMath::Log(I70A/PredB[kPidPion])/D70);
      if (pMomentum > pMomin && pMomentum < pMomax) MPoints[0][4]->Fill(TrackLength,TMath::Log(I70A/Pred70B[kPidPion]));
#endif
    }
  }
  if (TrackLength > 20) { 
    //  if (NoFitPoints >= 20) { 
    Int_t k;
    for (k = 0; k < NdEdx; k++) {
      FdEdx[k].zP = 
	m_Bichsel->GetMostProbableZ(TMath::Log10(pMomentum/StProbPidTraits::mPidParticleDefinitions[kPidPion]->mass()),
				    TMath::Log2(FdEdx[k].dx));
      FdEdx[k].sigmaP = 
	m_Bichsel->GetRmsZ(TMath::Log10(pMomentum/StProbPidTraits::mPidParticleDefinitions[kPidPion]->mass()),
			   TMath::Log2(FdEdx[k].dx));
      Double_t predB  = 1.e-6*TMath::Exp(FdEdx[k].zP);
      FdEdx[k].dEdxN  = TMath::Log(FdEdx[k].dEdx /predB);
      for (Int_t l = 0; l <= StTpcdEdxCorrection::kTpcLast; l++) {
	if (FdEdx[k].C[l].dEdx > 0)
	  FdEdx[k].C[l].dEdxN = TMath::Log(FdEdx[k].C[l].dEdx/predB);
      }
      if (FdEdx[k].row < 14) {
	hdEI->Fill(TMath::Log10(FdEdx[k].dE));
	hdEUI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	hdERI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kAdcCorrection].dE));
	hdEPI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure].dE));
	hdETI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::ktpcTime].dE));
	hdESI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kTpcSecRowB].dE));
	hdEZI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kzCorrection].dE));
	hdEMI->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kMultiplicity].dE));
      }
      else {
	hdEO->Fill(TMath::Log10(FdEdx[k].dE));
	hdEUO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	hdERO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kAdcCorrection].dE));
	hdEPO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure].dE));
	hdETO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::ktpcTime].dE));
	hdESO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kTpcSecRowB].dE));
	hdEZO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kzCorrection].dE));
	hdEMO->Fill(TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kMultiplicity].dE));
      }
      if ((TESTBIT(m_Mode, kAdcHistos))) {
	if (PiDkeyU3 >= 0) {
	  Double_t betaXgamma = pMomentum/StProbPidTraits::mPidParticleDefinitions[PiDkeyU3]->mass();
	  Double_t zA = m_Bichsel->GetMostProbableZ(TMath::Log10(betaXgamma),TMath::Log2(FdEdx[k].dx));
	  Double_t PredA = 1.e-6*TMath::Exp(zA);
	  Double_t PredE =  PredA*FdEdx[k].dx;
	  Double_t PredEL = TMath::Log10(PredE);
	  if (FdEdx[k].row < 14) {
	    if (AdcI)        AdcI->Fill(PredEL,TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	    if (AdcIC)      AdcIC->Fill(PredEL,TMath::Log10(FdEdx[k].dE));
	  }
	  else {
	    if (AdcO)        AdcO->Fill(PredEL,TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	    if (AdcOC)      AdcOC->Fill(PredEL,TMath::Log10(FdEdx[k].dE));
	  }
	  Double_t PredEU =  PredE;
	  Double_t PredEUL = TMath::Log10(PredEU);
	  Double_t PredEULN = TMath::Log(PredEU);
	  Double_t PredEUkeV = 1.e6*PredE;
	  if (tpcGas) {
	    if (FdEdx[k].row < 14) {
	      if (Adc3I)   Adc3I->Fill(PredEUL,TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	      if (Adc3IC) Adc3IC->Fill(PredEUL,TMath::Log10(FdEdx[k].dE));
	      if (Adc3Ip && Adc3Ip[PiDkeyU3]) 
		Adc3Ip[PiDkeyU3]->Fill(PredEUL,TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	      if (sCharge == 0) 
		AdcIZP->Fill(PredEUkeV,FdEdx[k].ZdriftDistanceO2,TMath::Log(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE)-PredEULN);
	      else 
		AdcIZN->Fill(PredEUkeV,FdEdx[k].ZdriftDistanceO2,TMath::Log(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE)-PredEULN);
	    }
	    else {		   					                            
	      if (Adc3O)   Adc3O->Fill(PredEUL,TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	      if (Adc3OC) Adc3OC->Fill(PredEUL,TMath::Log10(FdEdx[k].dE));
	      if (Adc3Op && Adc3Op[PiDkeyU3]) 
		Adc3Op[PiDkeyU3]->Fill(PredEUL,TMath::Log10(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE));
	      if (sCharge == 0) 
		AdcOZP->Fill(PredEUkeV,FdEdx[k].ZdriftDistanceO2,TMath::Log(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE)-PredEULN);
	      else 
		AdcOZN->Fill(PredEUkeV,FdEdx[k].ZdriftDistanceO2,TMath::Log(FdEdx[k].C[StTpcdEdxCorrection::kUncorrected].dE)-PredEULN);
	    }
	  }
	}
      } // AdcHistos
      if (pMomentum > pMomin && pMomentum < pMomax &&TrackLength > 40 ) { // Momentum cut
	if (tpcGas) {
	  if (inputTPCGasPressureP) inputTPCGasPressureP->Fill(tpcGas->inputTPCGasPressure,FdEdx[k].dEdxN);
	  if (nitrogenPressureP) nitrogenPressureP->Fill(tpcGas->nitrogenPressure,FdEdx[k].dEdxN);
	  if (gasPressureDiffP) gasPressureDiffP->Fill(tpcGas->gasPressureDiff,FdEdx[k].dEdxN);
	  if (inputGasTemperatureP) inputGasTemperatureP->Fill(tpcGas->inputGasTemperature,FdEdx[k].dEdxN);
	  if (outputGasTemperatureP) outputGasTemperatureP->Fill(tpcGas->outputGasTemperature,FdEdx[k].dEdxN);
	  if (flowRateArgon1P) flowRateArgon1P->Fill(tpcGas->flowRateArgon1,FdEdx[k].dEdxN);
	  if (flowRateArgon2P) flowRateArgon2P->Fill(tpcGas->flowRateArgon2,FdEdx[k].dEdxN);
	  if (flowRateMethaneP)  flowRateMethaneP->Fill(tpcGas->flowRateMethane,FdEdx[k].dEdxN);
	  if (percentMethaneInP) 
	    percentMethaneInP->Fill(tpcGas->percentMethaneIn*1000./tpcGas->barometricPressure,
				    FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure-1].dEdxN);
	  if (percentMethaneInPA) 
	    percentMethaneInPA->Fill(tpcGas->percentMethaneIn*1000./tpcGas->barometricPressure,
				     FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure].dEdxN);
	  if (percentMethaneInPC) 
	    percentMethaneInPC->Fill(tpcGas->percentMethaneIn*1000./tpcGas->barometricPressure,
				     FdEdx[k].dEdxN);
	  if (ppmOxygenInP) ppmOxygenInP->Fill(tpcGas->ppmOxygenIn,FdEdx[k].dEdxN);
	  if (flowRateExhaustP) flowRateExhaustP->Fill(tpcGas->flowRateExhaust,FdEdx[k].dEdxN);
	  if (ppmWaterOutP)  ppmWaterOutP->Fill(tpcGas->ppmWaterOut,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure-1].dEdxN);
	  if (ppmWaterOutPA) ppmWaterOutPA->Fill(tpcGas->ppmWaterOut,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure].dEdxN);
	  if (ppmWaterOutPC) ppmWaterOutPC->Fill(tpcGas->ppmWaterOut,FdEdx[k].dEdxN);
	  //	  if (ppmOxygenOutP) ppmOxygenOutP->Fill(tpcGas->ppmOxygenOut,FdEdx[k].dEdxN);
	  if (flowRateRecirculationP) flowRateRecirculationP->Fill(tpcGas->flowRateRecirculation,FdEdx[k].dEdxN);
	}
	if (St_trigDetSumsC::instance()) {
	  if (St_trigDetSumsC::instance()->zdcX() > 0 && ZdcCP) ZdcCP->Fill(TMath::Log10(St_trigDetSumsC::instance()->zdcX()), FdEdx[k].dEdxN);
	  if (St_trigDetSumsC::instance()->bbcYellowBkg() > 0 && bbcYellowBkg) 
	    bbcYellowBkg->Fill(TMath::Log10(St_trigDetSumsC::instance()->bbcYellowBkg()), FdEdx[k].dEdxN);
	  if (St_trigDetSumsC::instance()->bbcBlueBkg() > 0 && bbcBlueBkg) 
	    bbcBlueBkg->Fill(TMath::Log10(St_trigDetSumsC::instance()->bbcBlueBkg()), FdEdx[k].dEdxN);
	  if (St_trigDetSumsC::instance()->bbcX() > 0)  {
	    if (BBCP) BBCP->Fill(TMath::Log10(St_trigDetSumsC::instance()->bbcX()), FdEdx[k].dEdxN);
	    if (BBC3) BBC3->Fill(FdEdx[k].row,TMath::Log10(St_trigDetSumsC::instance()->bbcX()), FdEdx[k].dEdxN);
	  }
	  if (St_trigDetSumsC::instance()->mult() > 0) {
	    if (MultiplicityPI && FdEdx[k].row < 14) MultiplicityPI->Fill(TMath::Log10(St_trigDetSumsC::instance()->mult()), FdEdx[k].dEdxN);
	    if (MultiplicityPO && FdEdx[k].row > 13) MultiplicityPO->Fill(TMath::Log10(St_trigDetSumsC::instance()->mult()), FdEdx[k].dEdxN);
	  }
	}
	Double_t Pad2Edge = FdEdx[k].edge;
	if (TMath::Abs(Pad2Edge) > 5) {
	  if (SecRow3 )  SecRow3->Fill(FdEdx[k].sector,FdEdx[k].row,FdEdx[k].C[StTpcdEdxCorrection::kTpcSecRowB-1].dEdxN);
#if 0
	  if (SecRow3A) SecRow3A->Fill(FdEdx[k].sector,FdEdx[k].row,FdEdx[k].C[StTpcdEdxCorrection::kTpcSecRowB].dEdxN);
#endif
	  if (SecRow3C) SecRow3C->Fill(FdEdx[k].sector,FdEdx[k].row,FdEdx[k].dEdxN);
	}
	if (Zdc3C && FdEdx[k].Zdc > 0) Zdc3C->Fill(FdEdx[k].row,TMath::Log10(FdEdx[k].Zdc),FdEdx[k].dEdxN);
	//Double_t xyz[3]  = {FdEdx[k].xyz[0],FdEdx[k].xyz[1],FdEdx[k].xyz[2]};
	Double_t xyzD[3] = {FdEdx[k].xyzD[0],FdEdx[k].xyzD[1],FdEdx[k].xyzD[2]};
	//Double_t Phi  = 180./TMath::Pi()*TMath::ATan2(xyz[0],xyz[1]);
	Double_t PhiD = 180./TMath::Pi()*TMath::ATan2(xyzD[0],xyzD[1]); 
	Double_t ThetaD = 180./TMath::Pi()*TMath::ATan2(-xyzD[2],TMath::Sqrt(xyzD[0]*xyzD[0]+xyzD[1]*xyzD[1]));
	if (Phi3)	  Phi3->Fill(FdEdx[k].PhiR,FdEdx[k].row,FdEdx[k].dEdxN);
	if (Phi3D) 	  Phi3D->Fill(PhiD,FdEdx[k].row,FdEdx[k].dEdxN);
	if (Theta3D) 	  Theta3D->Fill(ThetaD,FdEdx[k].row,FdEdx[k].dEdxN);
	if ((TESTBIT(m_Mode, kMip))) {
	  if (SecRow3Mip && TMath::Abs(devZs[kPidPion]) < 2) 
	    SecRow3Mip->Fill(FdEdx[k].row,
			     TMath::Log(FdEdx[k].dx)/TMath::Log(2.),
			     FdEdx[k].dEdxN);// /FdEdx[k].sigmaP);
	} // Mip
	if (tpcGas && Pressure) {
	  Double_t p     = tpcGas->barometricPressure;
	  Double_t t     = tpcGas->inputGasTemperature/298.2;
	  if (p > 0) {
	    Double_t press = TMath::Log(p);
	    if (Pressure)  Pressure ->Fill(FdEdx[k].row,press,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure-1].dEdxN);
	    if (PressureA) PressureA->Fill(FdEdx[k].row,press,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure].dEdxN);
	    if (PressureC) PressureC->Fill(FdEdx[k].row,press,FdEdx[k].dEdxN);
	  }
	  Double_t V = St_tpcAnodeHVavgC::instance()->voltagePadrow(FdEdx[k].sector,FdEdx[k].row);
	  Double_t VN = (FdEdx[k].row <= 13) ? V - 1170 : V - 1390;
	  if (V > 0) {
	    if (Voltage)  Voltage ->Fill(45*(FdEdx[k].sector-1)+FdEdx[k].row,
					 VN,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure-1].dEdxN);
	    if (VoltageC) VoltageC->Fill(FdEdx[k].row,
					 V,FdEdx[k].dEdxN);
	  }
	  Double_t               Q = St_tpcAvCurrentC::instance()->chargeI();
	  if (FdEdx[k].row > 13) Q = St_tpcAvCurrentC::instance()->chargeO();
	  AcCharge->Fill(FdEdx[k].row,Q,FdEdx[k].dEdxN);
	  Qcm->Fill(FdEdx[k].row,FdEdx[k].Qcm,FdEdx[k].dEdxN);
	  AvCurrent->Fill(FdEdx[k].row,FdEdx[k].Crow,FdEdx[k].dEdxN);
	  if (p*t > 0) {
	    Double_t temp = TMath::Log(p/t);
	    if (PressureT)  PressureT ->Fill(FdEdx[k].row,temp,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure-1].dEdxN);
	    if (PressureTA) PressureTA->Fill(FdEdx[k].row,temp,FdEdx[k].C[StTpcdEdxCorrection::ktpcPressure].dEdxN);
	    if (PressureTC) PressureTC->Fill(FdEdx[k].row,temp,FdEdx[k].dEdxN);
	  }
	}
	if (Time)    Time->Fill(date,FdEdx[k].C[StTpcdEdxCorrection::ktpcTime-1].dEdxN);
	if (TimeP)  TimeP->Fill(date,FdEdx[k].C[StTpcdEdxCorrection::ktpcTime].dEdxN);
	if (TimeC)  TimeC->Fill(date,FdEdx[k].dEdxN);
#if 0
	if (Z3O)  Z3O->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistanceO2,FdEdx[k].C[StTpcdEdxCorrection::kDrift-1].dEdxN);
	if (Z3OA)Z3OA->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistanceO2,FdEdx[k].C[StTpcdEdxCorrection::kDrift].dEdxN);
#endif
	if (Z3OC)Z3OC->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistanceO2,FdEdx[k].dEdxN);
#if 0
	if (Z3OW)  Z3OW->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistanceO2W,FdEdx[k].C[StTpcdEdxCorrection::kDrift-1].dEdxN);
	if (Z3OWA)Z3OWA->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistanceO2W,FdEdx[k].C[StTpcdEdxCorrection::kDrift].dEdxN);
	if (Z3OWC)Z3OWC->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistanceO2W,FdEdx[k].dEdxN);
	if (Z3)    Z3->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistance,  FdEdx[k].C[StTpcdEdxCorrection::kzCorrection-1].dEdxN);
	if (Z3A)  Z3A->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistance,  FdEdx[k].C[StTpcdEdxCorrection::kzCorrection].dEdxN);
#endif
	if (Z3C)  Z3C->Fill(FdEdx[k].row,FdEdx[k].ZdriftDistance,  FdEdx[k].dEdxN);
#if 0
	if (ETA)  ETA->Fill(FdEdx[k].row,eta,FdEdx[k].dEdxN);
	if (ETA3)ETA3->Fill(FdEdx[k].row,eta,FdEdx[k].dEdxN);
#endif
	if (Edge3) Edge3->Fill(FdEdx[k].row,FdEdx[k].edge,  FdEdx[k].dEdxN);
	if (St_trigDetSumsC::instance() && St_trigDetSumsC::instance()->mult() > 0) {
	  if (MulRow)   MulRow->Fill(TMath::Log10(St_trigDetSumsC::instance()->mult()),FdEdx[k].row,FdEdx[k].C[StTpcdEdxCorrection::kMultiplicity-1].dEdxN);
	  if (MulRowC) MulRowC->Fill(TMath::Log10(St_trigDetSumsC::instance()->mult()),FdEdx[k].row,FdEdx[k].dEdxN);
	}
	if (dXdE )  dXdE->Fill(FdEdx[k].dx,FdEdx[k].row,FdEdx[k].C[StTpcdEdxCorrection::kdXCorrection-1].dEdxN);
	if (dXdEA) dXdEA->Fill(FdEdx[k].dx,FdEdx[k].row,FdEdx[k].C[StTpcdEdxCorrection::kdXCorrection].dEdxN);
	if (dXdEC) dXdEC->Fill(FdEdx[k].dx,FdEdx[k].row,FdEdx[k].dEdxN);
      }
      if (TESTBIT(m_Mode, kZBGX) && PiDkeyU3 >= 0 && zbgx) 
	zbgx[2*PiDkeyU3+sCharge]->Fill(bghyp[PiDkeyU3],TMath::Log(FdEdx[k].dx)/TMath::Log(2.),FdEdx[k].dEdxL-GeV2keV);
    }
    if ((TESTBIT(m_Mode, kCORRELATION))) Correlations();
  }
#ifdef CompareWithToF
  // dE/dx tree
  if (NdEdx > 0  && ftrack && ftree && TrackLength70 > 20.0 && pidTof) {
    ftrack->Clear();
    ftrack->sCharge = 1 - 2*sCharge;
    ftrack->p = pMomentum;
    ftrack->pX = g3.x();
    ftrack->pY = g3.y();
    ftrack->pZ = g3.z();
    ftrack->R0 = gTrack->geometry()->origin().mag();
    ftrack->Z0 = gTrack->geometry()->origin().z();
    ftrack->Phi0 = gTrack->geometry()->origin().phi();
    ftrack->NoFitPoints = NoFitPoints;
    ftrack->NdEdx = NdEdx;
    ftrack->N70 = N70;
    ftrack->I70 = I70;
    ftrack->D70 = D70;
    ftrack->TrackLength70 = TrackLength70;
    ftrack->fitZ = fitZ;
    ftrack->fitdZ = fitdZ;
    ftrack->TrackLength = TrackLength;
    Double_t *h = 0; 
    for (Int_t l = 0; l < 4; l++) {
      Int_t k = IdxH[l];
      if      (k == kPidProton  ) h = &ftrack->PredP;
      else if (k == kPidKaon    ) h = &ftrack->PredK;
      else if (k == kPidPion    ) h = &ftrack->Predpi;
      else if (k == kPidElectron) h = &ftrack->PredE;
      else continue;
      h[0] = PredB[k];
      h[1] = Pred70B[k];
      h[2] = devZs[k];
      h[3] = devZ[k];
      h[4] = devToF[k];
      if (pidprob) h[5] = pidprob->GetProbability(k);
      else         h[5] = -1;
    }
    for (Int_t k = 0; k < NdEdx; k++) {
      ftrack->AddPoint(FdEdx[k]);
    }
    ftree->Fill();
  }
#endif
  return;
}
//_____________________________________________________________________________
void StdEdxY2Maker::PrintdEdx(Int_t iop) {
  const Int_t NOpts = 20;
  const Char_t *Names[NOpts] = {"CdEdx","FdEdx","dEdxS","dEdxU","dEdxR",
				"dEdxS","dEdxS","dEdxP","dEdxt","dEdxO",
				"dEdxM","dEdxZ","dEdxm","dEdxT","dEdxW",
				"dEdxC","dEdxE","dEdxp","dEdxX","dEdxd"};
  if (iop < 0 || iop >= NOpts) return;
  dEdxY2_t *pdEdx = 0; 
  Double_t dEdx;
  Double_t I = 0;
  Int_t N70 = NdEdx - (int) (0.3*NdEdx + 0.5); 
  Int_t N60 = NdEdx - (int) (0.4*NdEdx + 0.5);
  Double_t I70 = 0, I60 = 0;
  Double_t avrz = 0;
  for (int i=0; i< NdEdx; i++) {
    dEdx = 0;
    if (iop == 0)      {pdEdx = &CdEdx[i]; dEdx = CdEdx[i].dEdx;}
    else if (iop == 1) {pdEdx = &FdEdx[i]; dEdx = FdEdx[i].dEdx;}
    else if (iop == 2) {pdEdx = &dEdxS[i]; dEdx = dEdxS[i].dEdx;}
    else if (iop >= 3) {pdEdx = &FdEdx[i]; dEdx = FdEdx[i].C[StTpcdEdxCorrection::kUncorrected+iop-3].dEdx;}
    I = (i*I +  pdEdx->dEdx)/(i+1);
    //     cout << Names[iop] << " " << i << " S/R " << dEdx->sector << "/" << dEdx->row
    // 	 << " dEdx(keV/cm) " << 1.e6*dEdx->dEdx << " dx " << dEdx->dx 
    //       //	 << " dx " << dEdx->dxH 
    // 	 << " x[" << dEdx->xyz[0] << "," << dEdx->xyz[1] << "," << dEdx->xyz[2] << "]" 
    // 	 << " d[" << dEdx->xyzD[0] << "," << dEdx->xyzD[1] << "," << dEdx->xyzD[2] << "]" 
    // 	 << " R[" << dEdx->resXYZ[0] << "," << dEdx->resXYZ[1] << "," << dEdx->resXYZ[2] << "]" 
    // 	 << " Sum " << 1.e6*I << "(keV)"
    // 	 << " Prob " << dEdx->Prob << endl;
    cout << Form("%s %2i  S/R %2i/%2i dEdx(keV/cm) %8.2f dx %5.2f x[%8.2f,%8.2f,%8.2f]", 
		 Names[iop],i,pdEdx->sector,pdEdx->row,1.e6*dEdx, pdEdx->dx, pdEdx->xyz[0], pdEdx->xyz[1], pdEdx->xyz[2]);
    cout << Form(" d[%8.2f,%8.2f,%8.2f] Sum %8.2f Prob %8.5f", pdEdx->xyzD[0], pdEdx->xyzD[1], pdEdx->xyzD[2],1.e6*I,pdEdx->Prob) << endl;
    if (iop == 2) {
      if (i < N60) I60 += dEdx;
      if (i < N70) I70 += dEdx;
      if (i == N60 - 1) {
	I60 /= N60;
	cout << " ======================= I60 \t" << I60 << endl;
      }
      if (i == N70 - 1) {
	I70 /= N70;
	cout << " ======================= I70 \t" << I70 << endl;
      }
    }
    avrz += TMath::Log(dEdx);
  }
  if (NdEdx) avrz /= NdEdx;
  cout << "mean dEdx \t" << I << "\tExp(avrz)\t" << TMath::Exp(avrz) << endl;
}
//________________________________________________________________________________
Double_t StdEdxY2Maker::LikeliHood(Double_t Xlog10bg, Int_t NdEdx, dEdxY2_t *dEdx) {
  //SecRowMipFitpHist298P02gh1.root  correction to most probable value vs log2(dx)
  //  static const Double_t probdx2[3] = {-3.58584e-02, 4.16084e-02,-1.45163e-02};// 
  const static Double_t ProbCut = 1.e-4;
  const static Double_t GeV2keV = TMath::Log(1.e-6);
  Double_t f = 0;
  St_tpcCorrectionC *dXCorrection = 0;
  if (m_TpcdEdxCorrection)  dXCorrection = m_TpcdEdxCorrection->dXCorrection();
  Int_t k = 0;
  if (dXCorrection) {
    tpcCorrection_st *dXC = ((St_tpcCorrection *)dXCorrection->Table())->GetTable();
    if (dXC) {
      Int_t N = dXC->nrows;
      if (N > 3) k = 3; 
    } 
  }
  for (int i=0;i<NdEdx; i++) {
    Double_t Ylog2dx = TMath::Log2(dEdx[i].dx);
    StTpcdEdxCorrection::ESector l = StTpcdEdxCorrection::kTpcInner;
    if (dEdx[i].row > NumberOfInnerRows) l = StTpcdEdxCorrection::kTpcOuter;
    Double_t sigmaC = 0;
    if (dXCorrection) sigmaC = dXCorrection->CalcCorrection(k,Ylog2dx); 
    Double_t zMostProb = m_Bichsel->GetMostProbableZ(Xlog10bg,Ylog2dx);
    Double_t sigma     = m_Bichsel->GetRmsZ(Xlog10bg,Ylog2dx) + sigmaC;
    Double_t xi = (dEdx[i].dEdxL - GeV2keV - zMostProb)/sigma;
    Double_t  Phi = m_Bichsel->GetProbability(Xlog10bg,Ylog2dx,xi);
    dEdx[i].Prob = Phi/sigma;
    if (dEdx[i].Prob < ProbCut) {
      dEdx[i].Prob = ProbCut; 
    }
    f -= 2*TMath::Log( dEdx[i].Prob );
  }
  return f;
}
//________________________________________________________________________________
void StdEdxY2Maker::Landau(Double_t x, Double_t *val){
  //  TF1 *LandauF = new TF1("LandauF","exp([0]-0.5*((x-[1])/[2])**2+exp([3]-0.5*((x-[4])/[5])**2+exp([6]-0.5*((x-[7])/[8])**2)))",-4,10);
  Double_t params[9] = {
    -7.74975e+00,
    6.53414e+00,
    1.21524e+00,
    3.31409e+00,
    -2.58291e+00,
    3.51463e+00,
    -3.47755e+00,
    3.77698e-02,
    6.67913e-01};
  Double_t dev1 = (x-params[1])/params[2];
  Double_t dev2 = (x-params[4])/params[5];
  Double_t dev3 = (x-params[7])/params[8];
  Double_t d    = TMath::Exp(params[6]-0.5*dev3*dev3);
  Double_t dp   = -dev3/params[8]*d;
  Double_t c    = TMath::Exp(params[3]-0.5*dev2*dev2 + d);
  Double_t cp   = (-dev2/params[5] + dp)*c;
  val[0]        = params[0]-0.5*dev1*dev1+c;
  val[1]        = - dev1/params[2]+cp;
}
//________________________________________________________________________________
void StdEdxY2Maker::fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  Double_t Val[2];
  // P10
  // dEdxS->Draw("sigma_z:log(x)/log(2)","","prof")
  static Double_t sigma_p[3] = { 5.66734e-01,   -1.24725e-01,   1.96085e-02};
  f = 0.;
  gin[0] = 0.;
  gin[1] = 0.;
#if 0
  Double_t weightT = 0;
#endif
  for (int i=0;i<NdEdx; i++) {
    //    Double_t sigma = StTpcdEdxCorrection::SumSeries(TMath::Log(FdEdx[i].dx),3,sigma_p);
    Double_t X = TMath::Log(FdEdx[i].dx);
    Double_t sigma = sigma_p[2];
    for (int n = 1; n>=0; n--) sigma = X*sigma + sigma_p[n];
    FdEdx[i].zdev    = (FdEdx[i].dEdxL-par[0])/sigma;
    Landau(FdEdx[i].zdev,Val);
    FdEdx[i].Prob = TMath::Exp(Val[0]);
#if 0
    Double_t weight = FdEdx[i].Weight;
    weightT += weight;
    f      -= Val[0]*weight;
    gin[0] += Val[1]/sigma*weight;
#else
    f      -= Val[0];
    gin[0] += Val[1]/sigma;
#endif
  }
#if 0
  weightT /= NdEdx;
  f /= weightT;
  gin[0] /= weightT;
#endif
}
//________________________________________________________________________________
void StdEdxY2Maker::DoFitZ(Double_t &chisq, Double_t &fitZ, Double_t &fitdZ){
  Double_t avz = 0;
  for (int i=0;i<NdEdx;i++) avz += FdEdx[i].dEdxL;
  if (NdEdx>5) {
    avz /= NdEdx;
    Double_t arglist[10];
    Int_t ierflg = 0;
    m_Minuit->SetFCN(fcn);
    //    m_Minuit->SetPrintLevel(-1);
    if (Debug() < 2) {
      arglist[0] = -1;
      m_Minuit->mnexcm("set print",arglist, 1, ierflg);
    }
    m_Minuit->mnexcm("set NOW",arglist, 0, ierflg);
    m_Minuit->mnexcm("CLEAR",arglist, 0, ierflg);
    arglist[0] = 0.5;
    m_Minuit->mnexcm("SET ERR", arglist ,1,ierflg);
    m_Minuit->mnparm(0, "mean", avz, 0.01,0.,0.,ierflg); //First Guess
    if (Debug() < 2)       arglist[0] = 1.;   // 1.
    else                   arglist[0] = 0.;   // Check gradient 
    m_Minuit->mnexcm("SET GRAD",arglist,1,ierflg);
    arglist[0] = 500;
    arglist[1] = 1.;
    m_Minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
    m_Minuit->mnexcm("HESSE  ",arglist,0,ierflg);
    Double_t edm,errdef;
    Int_t nvpar,nparx,icstat;
    m_Minuit->mnstat(chisq,edm,errdef,nvpar,nparx,icstat);
    m_Minuit->GetParameter(0, fitZ, fitdZ);
  }
  else {
    fitZ = fitdZ = chisq =  -999.;
  }
}
//________________________________________________________________________________
void StdEdxY2Maker::TrigHistos(Int_t iok) {
  static TProfile *BarPressure = 0, *inputTPCGasPressure = 0;
  static TProfile *nitrogenPressure = 0, *gasPressureDiff = 0, *inputGasTemperature = 0;
  static TProfile *outputGasTemperature = 0, *flowRateArgon1 = 0, *flowRateArgon2 = 0, *flowRateMethane = 0;
  static TProfile *percentMethaneIn = 0, *ppmOxygenIn = 0, *flowRateExhaust = 0;
  static TProfile *ppmWaterOut = 0, *ppmOxygenOut = 0, *flowRateRecirculation = 0;
  //  static TProfile *Center = 0, *Height = 0, *Width = 0,  *CenterPressure = 0;
  static TH1F *Multiplicity; // mult rate 
#if 0
  static TH2F *Zdc  = 0; // ZdcEastRate versus ZdcWestRate
#endif
  static TH1F *ZdcC = 0; // ZdcCoincidenceRate
  static TH1F *BBC   = 0; // BbcCoincidenceRate
  if (! iok && !BarPressure) {
    TDatime t1(tMin,0); /// min Time and
      TDatime t2(tMax,0); /// max 
	UInt_t i1 = t1.Convert() - timeOffSet;
	UInt_t i2 = t2.Convert() - timeOffSet;
	Int_t Nt = (i2 - i1)/(3600); // each hour 
	BarPressure           = new TProfile("BarPressure","barometricPressure (mbar) versus time",Nt,i1,i2);                    
	inputTPCGasPressure   = new TProfile("inputTPCGasPressure","inputTPCGasPressure (mbar) versus time",Nt,i1,i2);           
	nitrogenPressure      = new TProfile("nitrogenPressure","nitrogenPressure (mbar) versus time",Nt,i1,i2);                 
	gasPressureDiff       = new TProfile("gasPressureDiff","gasPressureDiff (mbar) versus time",Nt,i1,i2);                   
	inputGasTemperature   = new TProfile("inputGasTemperature","inputGasTemperature (degrees C) versus time",Nt,i1,i2);      
	outputGasTemperature  = new TProfile("outputGasTemperature","outputGasTemperature (degrees C) versus time",Nt,i1,i2);    
	flowRateArgon1        = new TProfile("flowRateArgon1","flowRateArgon1 (liters/min) versus time",Nt,i1,i2);               
	flowRateArgon2        = new TProfile("flowRateArgon2","flowRateArgon2 (liters/min) versus time",Nt,i1,i2);               
	flowRateMethane       = new TProfile("flowRateMethane","flowRateMethane (liters/min) versus time",Nt,i1,i2);             
	percentMethaneIn      = new TProfile("percentMethaneIn","percentMethaneIn (percent) versus time",Nt,i1,i2);              
	ppmOxygenIn           = new TProfile("ppmOxygenIn","ppmOxygenIn (ppm) versus time",Nt,i1,i2);                            
	flowRateExhaust       = new TProfile("flowRateExhaust","flowRateExhaust (liters/min) versus time",Nt,i1,i2);             
	ppmWaterOut           = new TProfile("ppmWaterOut","ppmWaterOut (ppm) versus time",Nt,i1,i2);                             
	ppmOxygenOut          = new TProfile("ppmOxygenOut","ppmOxygenOut (ppm) versus time",Nt,i1,i2);
	flowRateRecirculation = new TProfile("flowRateRecirculation","flowRateRecirculation (liters/min) versus time",Nt,i1,i2);
	//     CenterPressure        = new TProfile("CenterPressureP","log(center) vs log(Pressure)",150, 6.84, 6.99);
	//     Center                = new TProfile("Center","Tpc Gain Monitor center versus Time",Nt,i1,i2);
	//     Height 		  = new TProfile("Height","Tpc Gain Monitor height versus Time",Nt,i1,i2);
	//     Width  		  = new TProfile("Width","Tpc Gain Monitor width versus Time",Nt,i1,i2);  
	// trigDetSums histograms
#if 0
	Zdc                   = new TH2F("Zdc","ZdcEastRate versus ZdcWestRate (log10)",100,0,10,100,0,10);
#endif
	ZdcC                  = new TH1F("ZdcC","ZdcCoincidenceRate (log10)",100,0,10);
	Multiplicity          = new TH1F("Multiplicity","Multiplicity (log10)",100,0,10);
	BBC                   = new TH1F("BBC","BbcCoincidenceRate (log10)",100,0,10);
  }
  else {
    tpcGas_st  *tpcgas = m_TpcdEdxCorrection && m_TpcdEdxCorrection->tpcGas() ? m_TpcdEdxCorrection->tpcGas()->GetTable():0;
    Double_t date = GetDateTime().Convert() - timeOffSet;
    if (tpcgas) {
      if (BarPressure)           BarPressure->Fill(date,tpcgas->barometricPressure);             
      if (inputTPCGasPressure)   inputTPCGasPressure->Fill(date,tpcgas->inputTPCGasPressure);    
      if (nitrogenPressure)      nitrogenPressure->Fill(date,tpcgas->nitrogenPressure);          
      if (gasPressureDiff)       gasPressureDiff->Fill(date,tpcgas->gasPressureDiff);            
      if (inputGasTemperature)   inputGasTemperature->Fill(date,tpcgas->inputGasTemperature);    
      if (outputGasTemperature)  outputGasTemperature->Fill(date,tpcgas->outputGasTemperature);  
      if (flowRateArgon1)        flowRateArgon1->Fill(date,tpcgas->flowRateArgon1);              
      if (flowRateArgon2)        flowRateArgon2->Fill(date,tpcgas->flowRateArgon2);              
      if (flowRateMethane)       flowRateMethane->Fill(date,tpcgas->flowRateMethane);            
      if (percentMethaneIn)      
	percentMethaneIn->Fill(date,tpcgas->percentMethaneIn*1000./tpcgas->barometricPressure);          
      if (ppmOxygenIn)           ppmOxygenIn->Fill(date,tpcgas->ppmOxygenIn);                    
      if (flowRateExhaust)       flowRateExhaust->Fill(date,tpcgas->flowRateExhaust);            
      if (ppmWaterOut)           ppmWaterOut->Fill(date,tpcgas->ppmWaterOut);                    
      if (ppmOxygenOut)          ppmOxygenOut->Fill(date,tpcgas->ppmOxygenOut);                
      if (flowRateRecirculation) flowRateRecirculation->Fill(date,tpcgas->flowRateRecirculation);
    }
    if (St_trigDetSumsC::instance()) {
#if 0
      if (St_trigDetSumsC::instance()->zdcWest() > 0 &&
	  St_trigDetSumsC::instance()->zdcEast() > 0 && Zdc) Zdc->Fill(TMath::Log10(St_trigDetSumsC::instance()->zdcWest()),
								       TMath::Log10(St_trigDetSumsC::instance()->zdcEast()));
#endif
      if (St_trigDetSumsC::instance()->zdcX() > 0 && ZdcC) ZdcC->Fill(TMath::Log10(St_trigDetSumsC::instance()->zdcX()));
      if (St_trigDetSumsC::instance()->bbcX() > 0 && BBC) BBC->Fill(TMath::Log10(St_trigDetSumsC::instance()->bbcX()));
      if (St_trigDetSumsC::instance()->mult() > 0 && Multiplicity) Multiplicity->Fill(TMath::Log10(St_trigDetSumsC::instance()->mult()));
    }
  } // (TESTBIT(m_Mode, kGASHISTOGRAMS))n
}
//________________________________________________________________________________
void StdEdxY2Maker::SpaceCharge(Int_t iok, StEvent* pEvent, StGlobalCoordinate *global, dEdxY2_t *CdEdx) {
  // SpaceChargeStudy
  static TProfile2D *SpaceCharge = 0, *SpaceChargeU = 0, *SpaceChargeT = 0;
  static TH2F *Space2Charge = 0, *Space2ChargeU = 0, *Space2ChargeT = 0;
  static TH1F *TimeShift = 0;
  static TH3F *Space3Charge = 0, *Space3ChargePRZ = 0,*Space3ChargeShifted = 0;
  if (iok == 0 && !SpaceCharge) {
    if (Debug()) gMessMgr->Warning() << "StdEdxY2Maker::SpaceCharge Space Charge Histograms" << endm;
    SpaceCharge   = new TProfile2D("SpaceCharge","dE versus R and Z",85,40.,210.,92,-230.,230.);
    SpaceChargeU  = new TProfile2D("SpaceChargeU","dEU versus R and Z",85,40.,210.,92,-230.,230.);
    SpaceChargeT  = new TProfile2D("SpaceChargeT","dEU (all) versus R and Z",85,40.,210.,92,-230.,230.);
    Space2Charge  = new TH2F("Space2Charge","dE versus R and Z",85,40.,210.,92,-230.,230.);
    Space2ChargeU = new TH2F("Space2ChargeU","dEU versus R and Z",85,40.,210.,92,-230.,230.);
    Space2ChargeT = new TH2F("Space2ChargeT","dEU (all) versus R and Z",85,40.,210.,92,-230.,230.);
    TimeShift     = new TH1F("TimeShift","Shift in time wrt collsion",200,0,2000.);
    Space3Charge  = new TH3F("Space3Charge","Space charged versus Sector, Row and Z",
			     numberOfSectors,0.5, numberOfSectors+0.5, NumberOfRows,0.5, NumberOfRows+0.5,105,0.,210.);
    Space3Charge->Sumw2();
    Space3ChargePRZ  = new TH3F("Space3ChargePRZ","Space charged versus Phi(rads), Rho and Z",
				36,-TMath::Pi(), TMath::Pi(), 85, 40., 210.,105,-210.,210.);
    Space3ChargePRZ->Sumw2();
    Space3ChargeShifted = new TH3F("Space3ChargeShifted","Space charged shifted in time versus Sector, Row and Z",
				   numberOfSectors,0.5, numberOfSectors+0.5, NumberOfRows,0.5, NumberOfRows+0.5,105,0.,210.);
    Space3ChargeShifted->Sumw2();
    return;
  }
  if (iok == 1 && pEvent) {
    StTpcHitCollection* TpcHitCollection = pEvent->tpcHitCollection();
    if (TpcHitCollection) {
      UInt_t numberOfSectors = TpcHitCollection->numberOfSectors();
      const Int_t EventId  = GetEventNumber();
      Int_t Ids[24];
      Int_t secs[24];
      Double_t zshift[24];
      const Int_t NMixEvts = 1152;
      for (UInt_t ev = 0; ev< numberOfSectors; ev++) {
	Ids[ev] = (EventId + gRandom->Integer(NMixEvts))%NMixEvts;
	if (TimeShift) TimeShift->Fill(Ids[ev]);
	secs[ev] = gRandom->Integer(24);
	zshift[ev] = 0.18*Ids[ev];
      }
      for (UInt_t i = 0; i< numberOfSectors; i++) {
	StTpcSectorHitCollection* sectorCollection = TpcHitCollection->sector(i);
	if (sectorCollection) {
	  Int_t numberOfPadrows = sectorCollection->numberOfPadrows();
	  for (int j = 0; j< numberOfPadrows; j++) {
	    StTpcPadrowHitCollection *rowCollection = TpcHitCollection->sector(i)->padrow(j);
	    if (rowCollection) {
	      UInt_t NoHits = rowCollection->hits().size();
	      for (UInt_t k = 0; k < NoHits; k++) {
		StTpcHit* tpcHit = TpcHitCollection->sector(i)->padrow(j)->hits().at(k);
		SpaceChargeT->Fill(tpcHit->position().perp(),tpcHit->position().z(),tpcHit->charge());
		if (Space2ChargeT) 
		  Space2ChargeT->Fill(tpcHit->position().perp(),tpcHit->position().z(),tpcHit->charge());
		if (Space3Charge) {
		  Double_t z = TMath::Abs(tpcHit->position().z());
		  Space3Charge->Fill(i+1,j+1,z,tpcHit->charge());
		  if (Space3ChargeShifted) {
		    for (UInt_t ev = 0; ev<numberOfSectors; ev++) {
		      Double_t zz = z - zshift[ev];
		      if (zz > 0) {
			int sec = (i + secs[ev])%24;
			Space3ChargeShifted->Fill(sec+1,j+1,zz,tpcHit->charge());
		      }
		    }
		  }
		}
		if (Space3ChargePRZ) {
		  Space3ChargePRZ->Fill(tpcHit->position().phi(),
					tpcHit->position().perp(),
					tpcHit->position().z(),tpcHit->charge());
		} 
	      }
	    }
	  }
	}
      }
    }
    return;
  }
  if (iok == 2 && global && CdEdx) {
    if (SpaceCharge) SpaceCharge->Fill(global->position().perp(),global->position().z(),CdEdx->dE);
    if (SpaceChargeU) SpaceChargeU->Fill(global->position().perp(),global->position().z(),CdEdx->C[StTpcdEdxCorrection::kUncorrected].dE);
    if (Space2Charge) Space2Charge->Fill(global->position().perp(),global->position().z(),CdEdx->dE);
    if (Space2ChargeU) Space2ChargeU->Fill(global->position().perp(),global->position().z(),CdEdx->C[StTpcdEdxCorrection::kUncorrected].dE);
  }
  return;
}
//________________________________________________________________________________
void StdEdxY2Maker::XyzCheck(StGlobalCoordinate *global, Int_t iokCheck) {
  static TH3F *XYZ = 0, *XYZbad = 0;
  if (! global && !XYZ) {
    if (Debug()) gMessMgr->Warning() << "StdEdxY2Maker::XyzCheck XYZ check Histograms" << endm;
    XYZ    = new TH3F("XYZ","xyz for clusters",80,-200,200,80,-200,200,84,-210,210);
    XYZbad = new TH3F("XYZbad","xyz for clusters with mismatched sectors",
		      80,-200,200,80,-200,200,84,-210,210);
  }
  else 
    if (XYZ) XYZ->Fill( global->position().x(), global->position().y(), global->position().z());
  if (iokCheck && XYZbad) XYZbad->Fill( global->position().x(), global->position().y(), global->position().z());
}
//________________________________________________________________________________
void StdEdxY2Maker::QAPlots(StGlobalTrack* gTrack) {
  static TH2F *fTdEdxP70 = 0, *fTdEdxP70pi = 0, *fTdEdxP70e = 0, *fTdEdxP70K = 0, *fTdEdxP70P = 0;
  static TH2F *fTdEdxPF = 0, *fTdEdxPFpi = 0, *fTdEdxPFe = 0, *fTdEdxPFK = 0, *fTdEdxPFP = 0;
  static StTpcDedxPidAlgorithm PidAlgorithm;
  static StElectron* Electron = StElectron::instance();
  static StPionPlus* Pion = StPionPlus::instance();
  static StKaonPlus* Kaon = StKaonPlus::instance();
  static StProton* Proton = StProton::instance();
  static const Double_t Log10E = TMath::Log10(TMath::Exp(1.));
  static int first=0;
  if (! gTrack) {
    TFile *f = 0;
    if (TESTBIT(m_Mode, kCalibration)) {
      f = GetTFile();
      if (f) f->cd();
    }
    if (!first) {
      fZOfBadHits = new TH1F*[fNZOfBadHits];
      static const Char_t *BadCaseses[fNZOfBadHits] = 
      {"it is not used in track fit",     // 0
       "it is flagged ",                  // 1
       "track length is inf ",            // 2
       "it does not pass check ",         // 3
       "dx is out interval [0.5,25]",     // 4
       "Sector/Row gain < 0",             // 5 iok + 4
       "drift distance < min || drift distance > max", // 6
       "dE < 0 or dx < 0",                // 7
       "Edge effect",                     // 8
       "Anode Voltage problem",           // 9
       "Total no.of rejected clusters"    // 10
      };
      for (Int_t i = 0; i < fNZOfBadHits; i++) 
	fZOfBadHits[i] = new TH1F(Form("ZOfBadHits%i",i),
				  Form("Z of rejected clusters  because %s",BadCaseses[i]),
				  100,-210,210);                        
      fZOfGoodHits = new TH1F("ZOfGoodHits","Z of accepted clusters",100,-210,210);                        
      fPhiOfBadHits = new TH1F("PhiOfBadHits","Phi of rejected clusters",100, -TMath::Pi(), TMath::Pi());
      fTracklengthInTpcTotal = new TH1F("TracklengthInTpcTotal","Total track in TPC",100,0,200);         
      fTracklengthInTpc = new TH1F("TracklengthInTpc","Track length in TPC used for dE/dx",100,0,200);   
      fTdEdxPF    = new TH2F("TdEdxPF","log10(dE/dx(fit)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm", 
			     150,-1.,2., 500,0.,2.5);
      fTdEdxP70    = new TH2F("TdEdxP70","log10(dE/dx(I70)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm", 
			      150,-1.,2., 500,0.,2.5);
      fTdEdxP70pi  = new TH2F("TdEdxP70pi","log10(dE/dx(I70)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm |nSigmaPion| < 1", 
			      150,-1.,2., 500,0.,2.5);
      fTdEdxP70pi->SetMarkerColor(2);
      fTdEdxP70e   = new TH2F("TdEdxP70e","log10(dE/dx(I70)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm |nSigmaElectron| < 1", 
			      150,-1.,2., 500,0.,2.5);
      fTdEdxP70e->SetMarkerColor(3);
      fTdEdxP70K   = new TH2F("TdEdxP70K","log10(dE/dx(I70)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm |nSigmaKaon| < 1", 
			      150,-1.,2., 500,0.,2.5);
      fTdEdxP70K->SetMarkerColor(4);
      fTdEdxP70P   = new TH2F("TdEdxP70P","log10(dE/dx(I70)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm |nSigmaProton| < 1", 
			      150,-1.,2., 500,0.,2.5);
      fTdEdxP70P->SetMarkerColor(6);
      
      fTdEdxPFpi  = new TH2F("TdEdxPFpi","log10(dE/dx(Ifit)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm and Prob > 68.3", 
			     150,-1.,2., 500,0.,2.5);
      fTdEdxPFpi->SetMarkerColor(2);
      fTdEdxPFe   = new TH2F("TdEdxPFe","log10(dE/dx(Ifit)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm and Prob > 68.3", 
			     150,-1.,2., 500,0.,2.5);
      fTdEdxPFe->SetMarkerColor(3);
      fTdEdxPFK   = new TH2F("TdEdxPFK","log10(dE/dx(Ifit)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm and Prob > 68.3", 
			     150,-1.,2., 500,0.,2.5);
      fTdEdxPFK->SetMarkerColor(4);
      fTdEdxPFP   = new TH2F("TdEdxPFP","log10(dE/dx(Ifit)(keV/cm)) versus log10(p(GeV/c)) for Tpc TrackLength > 40 cm and Prob > 68.3", 
			     150,-1.,2., 500,0.,2.5);
      fTdEdxPFP->SetMarkerColor(6);
      mHitsUsage  = new TH2F("HitsUsage","log10(No.of Used in dE/dx hits) versus log10(Total no. of Tpc Hits",
			     80,0,8,60,0,6);
    }
    if (! f && !first) {
      for (Int_t i = 0; i < fNZOfBadHits; i++) AddHist(fZOfBadHits[i]);           
      AddHist(fZOfGoodHits);
      AddHist(fPhiOfBadHits);         
      AddHist(fTracklengthInTpcTotal);
      AddHist(fTracklengthInTpc);     
      AddHist(fTdEdxP70);
      AddHist(fTdEdxP70pi);
      AddHist(fTdEdxP70e);
      AddHist(fTdEdxP70K);
      AddHist(fTdEdxP70P);
      AddHist(fTdEdxPF);
      AddHist(fTdEdxPFpi);
      AddHist(fTdEdxPFe);
      AddHist(fTdEdxPFK);
      AddHist(fTdEdxPFP);
      AddHist(mHitsUsage);
    }
  }
  else {
    StSPtrVecTrackPidTraits &traits = gTrack->pidTraits();
    static StDedxPidTraits *pid, *pid70 = 0, *pidF = 0;
    static Double_t TrackLength70, TrackLength, I70, D70,  fitZ, fitdZ;
    static StProbPidTraits *pidprob = 0;
    static Int_t N70, NF;
    pid = pid70 = pidF = 0;
    TrackLength70 =  TrackLength =  I70 =  D70 = fitZ = fitdZ = 0; 
    N70 = NF = 0;
    for (UInt_t i = 0; i < traits.size(); i++) {
      if (! traits[i]) continue;
      if ( traits[i]->IsZombie()) continue;
      pid = dynamic_cast<StDedxPidTraits*>(traits[i]);
      if (pid) {
	if (pid->method() == kTruncatedMeanId) {
	  pid70 = pid; I70 = pid70->mean(); N70 = pid70->numberOfPoints();
	  TrackLength70 = pid70->length(); D70 = pid70->errorOnMean();
	}
	if (pid->method() == kLikelihoodFitId) {
	  pidF = pid;
	  fitZ = TMath::Log(pidF->mean()+3e-33); NF = pidF->numberOfPoints(); 
	  TrackLength = pidF->length(); fitdZ = pidF->errorOnMean(); 
	}
      }
      else pidprob = dynamic_cast<StProbPidTraits*>(traits[i]);
    }
    StThreeVectorD g3 = gTrack->geometry()->momentum(); // p of global track
    Double_t pMomentum = g3.mag();
    if (pid70 && ! pidF) TrackLength = TrackLength70;
    if (pid70 && TrackLength70 > 40. && I70 > 0) { 
      fTdEdxP70->Fill(TMath::Log10(pMomentum), TMath::Log10(I70)+6.);
      const StParticleDefinition* pd = gTrack->pidTraits(PidAlgorithm);
      if (pd) {
	if (TMath::Abs(PidAlgorithm.numberOfSigma(Electron)) < 1) fTdEdxP70e->Fill(TMath::Log10(pMomentum), TMath::Log10(I70)+6.);
	if (TMath::Abs(PidAlgorithm.numberOfSigma(Pion)) < 1) fTdEdxP70pi->Fill(TMath::Log10(pMomentum), TMath::Log10(I70)+6.);
	if (TMath::Abs(PidAlgorithm.numberOfSigma(Kaon)) < 1) fTdEdxP70K->Fill(TMath::Log10(pMomentum), TMath::Log10(I70)+6.);
	if (TMath::Abs(PidAlgorithm.numberOfSigma(Proton)) < 1) fTdEdxP70P->Fill(TMath::Log10(pMomentum), TMath::Log10(I70)+6.);
      }
    }
    if (pidF &&pidprob && TrackLength70 > 40.) { 
      fTdEdxPF->Fill(TMath::Log10(pMomentum), Log10E*fitZ + 6.);
      const StParticleDefinition* pd = gTrack->pidTraits(PidAlgorithm);
      if (pd) {
	if (pidprob->GetChi2Prob(kPidElectron) > 0.683) fTdEdxPFe->Fill(TMath::Log10(pMomentum), Log10E*fitZ + 6.);
	if (pidprob->GetChi2Prob(kPidPion)     > 0.683) fTdEdxPFpi->Fill(TMath::Log10(pMomentum), Log10E*fitZ + 6.);
	if (pidprob->GetChi2Prob(kPidKaon)     > 0.683) fTdEdxPFK->Fill(TMath::Log10(pMomentum), Log10E*fitZ + 6.);
	if (pidprob->GetChi2Prob(kPidProton)   > 0.683) fTdEdxPFP->Fill(TMath::Log10(pMomentum), Log10E*fitZ + 6.);
      }
    }
  }
  first = 2004;
}
//________________________________________________________________________________
void StdEdxY2Maker::BadHit(Int_t iFlag, const StThreeVectorF &xyz) {
  if (iFlag >= 0 && iFlag < fNZOfBadHits && fZOfBadHits[iFlag]) fZOfBadHits[iFlag]->Fill(xyz.z());
  if (fZOfBadHits[fNZOfBadHits-1]) fZOfBadHits[fNZOfBadHits-1]->Fill(xyz.z());
  if (fPhiOfBadHits!= 0) fPhiOfBadHits->Fill(TMath::ATan2(xyz.y(),xyz.x()));
}
//________________________________________________________________________________
void StdEdxY2Maker::Correlations() {
  // CORRELATION
  static TH2F *corrI = 0, *corrO = 0, *corrI2 = 0, *corrO2 = 0, *corrI5 = 0, *corrO5 = 0;
  static TH2F *corrIw = 0, *corrOw = 0, *corrI2w = 0, *corrO2w = 0, *corrI5w = 0, *corrO5w = 0;
  static TH1F *corrI1w = 0, *corrO1w = 0;
  if (! corrI) {
    gMessMgr->Warning() << "StdEdxY2Maker::Histogramming make Correlation histograms" << endm;
    corrI   = new TH2F("corrI","Correlation for Inner Sector for pair of nearest rows",
		       100,-10.,10., 100,-10.,10.);
    corrO   = new TH2F("corrO","Correlation for Outer Sector for pair of nearest rows",
		       100,-10.,10., 100,-10.,10.);
    corrI2   = new TH2F("corrI2","Correlation for Inner Sector for pair rows & row + 2",
			100,-10.,10., 100,-10.,10.);
    corrO2   = new TH2F("corrO2","Correlation for Outer Sector for pair rows & row + 2",
			100,-10.,10., 100,-10.,10.);
    corrI5   = new TH2F("corrI5","Correlation for Inner Sector for pair rows & row + 5",
			100,-10.,10., 100,-10.,10.);
    corrO5   = new TH2F("corrO5","Correlation for Outer Sector for pair rows & row + 5",
			100,-10.,10., 100,-10.,10.);
    corrIw   = new TH2F("corrIw","Weighted correlation for Inner Sector for pair of nearest rows",
			100,-10.,10., 100,-10.,10.);
    corrOw   = new TH2F("corrOw","Weighted correlation for Outer Sector for pair of nearest rows",
			100,-10.,10., 100,-10.,10.);
    corrI1w   = new TH1F("corrI1w","Weighted distribution for Inner Sector",100,-10.,10.);
    corrO1w   = new TH1F("corrO1w","Weighted distribution for Outer Sector",100,-10.,10.);
    corrI2w   = new TH2F("corrI2w","Weighted correlation for Inner Sector for pair rows & row + 2",
			 100,-10.,10., 100,-10.,10.);
    corrO2w   = new TH2F("corrO2w","Weighted correlation for Outer Sector for pair rows & row + 2",
			 100,-10.,10., 100,-10.,10.);
    corrI5w   = new TH2F("corrI5w","Weighted correlation for Inner Sector for pair rows & row + 5",
			 100,-10.,10., 100,-10.,10.);
    corrO5w   = new TH2F("corrO5w","Weighted correlation for Outer Sector for pair rows & row + 5",
			 100,-10.,10., 100,-10.,10.);
  } // CORRELATION
  for (Int_t k = 0; k < NdEdx; k++) {
    Double_t zk  = FdEdx[k].zdev;
    if (FdEdx[k].Prob > 1.e-12) {
      if (FdEdx[k].row > 13) corrO1w->Fill(zk,1./FdEdx[k].Prob);
      else                   corrI1w->Fill(zk,1./FdEdx[k].Prob);
    }
    for (Int_t m = 0; m < NdEdx; m++){
      if (k == m) continue;
      Double_t zl  = FdEdx[m].zdev;
      if (FdEdx[m].row%2 == 1 && FdEdx[m].row - FdEdx[k].row  == 1) {
	if (FdEdx[k].row > 13) {
	  corrO->Fill(zk,zl); 
	  if (FdEdx[k].Prob*FdEdx[m].Prob > 1.e-12) 
	    corrOw->Fill(zk,zl,1./(FdEdx[k].Prob*FdEdx[m].Prob));
	}
	else {
	  corrI->Fill(zk,zl); 
	  if (FdEdx[k].Prob*FdEdx[m].Prob > 1.e-12) 
	    corrIw->Fill(zk,zl,1./(FdEdx[k].Prob*FdEdx[m].Prob));
	}
      }
      if (FdEdx[m].row%2 == 1 && FdEdx[m].row - FdEdx[k].row  == 2) {
	if (FdEdx[k].row > 13) {
	  corrO2->Fill(zk,zl); 
	  if (FdEdx[k].Prob*FdEdx[m].Prob > 1.e-12) 
	    corrO2w->Fill(zk,zl,1./(FdEdx[k].Prob*FdEdx[m].Prob));
	}
	else {
	  corrI2->Fill(zk,zl); 
	  if (FdEdx[k].Prob*FdEdx[m].Prob > 1.e-12) 
	    corrI2w->Fill(zk,zl,1./(FdEdx[k].Prob*FdEdx[m].Prob));
	}
      }
      if (FdEdx[m].row%2 == 1 && FdEdx[m].row - FdEdx[k].row  == 5) {
	if (FdEdx[k].row > 13) {
	  corrO5->Fill(zk,zl); 
	  if (FdEdx[k].Prob*FdEdx[m].Prob > 1.e-12) 
	    corrO5w->Fill(zk,zl,1./(FdEdx[k].Prob*FdEdx[m].Prob));
	}
	else {
	  corrI5->Fill(zk,zl); 
	  if (FdEdx[k].Prob*FdEdx[m].Prob > 1.e-12) 
	    corrI5w->Fill(zk,zl,1./(FdEdx[k].Prob*FdEdx[m].Prob));
	}
      }
    } // end of l loop
  }
}
//________________________________________________________________________________
Int_t StdEdxY2Maker::Propagate(const StThreeVectorD &middle,const StThreeVectorD &normal,
			       const StPhysicalHelixD &helixI, const StPhysicalHelixD &helixO,
			       Double_t bField, 
			       StThreeVectorD &xyz, StThreeVectorD &dirG, Double_t s[2], Double_t w[2]) {
  xyz  = StThreeVectorD();
  dirG = StThreeVectorD();
  s[0] = helixI.pathLength(middle, normal);
  s[1] = helixO.pathLength(middle, normal);
  Double_t sA[2] = {s[0]*s[0], s[1]*s[1]};
  if (sA[0] > 1.e6 || sA[1] > 1.e6) {return 1;}
  Double_t sN = sA[0] + sA[1];
  w[0] = sA[0]/sN;
  w[1] = sA[1]/sN;
  if (w[0] > 1.e-4) {xyz += w[0]*helixO.at(s[1]); dirG += w[0]*helixO.momentumAt(s[1],bField);}
  if (w[1] > 1.e-4) {xyz += w[1]*helixI.at(s[0]); dirG += w[1]*helixI.momentumAt(s[0],bField);}
  return 0;
}
