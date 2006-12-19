/*!
 * \class StiPullEvent 
 * \author Victor Perev, Jan 2006
 */
/***************************************************************************
 *
 * $Id: StiPullEvent.h,v 1.3 2006/12/19 19:44:41 perev Exp $
 *
 * Author: Victor Perev, Jan 2006
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StiPullEvent.h,v $
 * Revision 1.3  2006/12/19 19:44:41  perev
 * tracks added
 *
 * Revision 1.2  2006/12/18 01:33:35  perev
 * + branch mHitsR(nd) +nHitCand +iHitCand
 *
 * Revision 1.1  2006/04/07 17:34:41  perev
 * StiPullEvent class added
 *
 * Revision 1.1  2006/02/14 19:02:09  perev
 * Svt self alignment maker
 *
 *
 **************************************************************************/
#ifndef StiPullEvent_hh
#define StiPullEvent_hh
#include "TObject.h"
#include "TClonesArray.h"
#include "TDatime.h"


class StiPullTrk : public TObject {
public:
    StiPullTrk();
   ~StiPullTrk(){}
    void Clear(const char *opt = "");
    void Print(const char* option = "") const;
int TestIt();
public:
char mBeg[1];
short mTrackNumber; 		//track number of hit
unsigned char nAllHits; 	//number of all hits in track
unsigned char nTpcHits; 	//number of tpc hits in track
unsigned char nSvtHits; 	//number of svt hits in track
unsigned char nSsdHits; 	//number of ssd hits in track
unsigned char mL; 		//Length of track

float mChi2;
float mCurv;        		//curvature
float mPt;        		//pt
float mPsi;			//track Psi(around beam)  in global  Sti frame
float mDip;			//track Dip in global  Sti frame
float mRxy;			//Rxy of track begining 
float mPhi;			//Phi angle of track begining
float mZ;
char mEnd[1];
  ClassDef(StiPullTrk,1);
};

class StiPullHit : public TObject {
public:
    StiPullHit();
   ~StiPullHit(){}
    void Clear(const char *opt = "");
    void Print(const char* option = "") const;
int TestIt();
public:
char mBeg[1];
short mTrackNumber; 		//track number of hit
unsigned char nAllHits; 	//number of all hits in track
unsigned char nTpcHits; 	//number of tpc hits in track
unsigned char nSvtHits; 	//number of svt hits in track
unsigned char nSsdHits; 	//number of ssd hits in track
unsigned char mDetector;	//see StHit.h
unsigned char nHitCand;	        //number of Hit Candidates
unsigned char iHitCand;	        //number of selected  Hit Candidate.
				// 0=smallest Xi2


UInt_t mHardwarePosition;	//see StHit.h
float mNormalRefAngle;	//rotation angle in Sti style
float mNormalRadius;    // >= 0
float mNormalYOffset;
float mZCenter;
float mChi2;
float mCurv;        		//curvature
float mPt;        		//pt
//		locals
float lXHit;			// x of  Hit  in local  Sti frame
float lYHit;			// y of  Hit  in local  Sti frame
float lZHit;			// z of  Hit  in local  Sti frame
float lYHitErr;			// y Hit Err in local  Sti frame
float lZHitErr;			// z Hit Err in local  Sti frame
float lHitEmx[3];		// hit error mtx:yy,yz,zz

float lXFit;			// x of  Fit  in local  Sti frame
float lYFit;			// y of  Fit  in local  Sti frame
float lZFit;			// z of  Fit  in local  Sti frame
float lYFitErr;			// y Fit Err in local  Sti frame
float lZFitErr;			// z Fit Err in local  Sti frame
float lFitEmx[3];		// hit error mtx:yy,yz,zz

float lYPul;			// dy of  Pul  in local  Sti frame
float lZPul;			// dz of  Pul  in local  Sti frame
float lYPulErr;			// dy Pul Err in local  Sti frame
float lZPulErr;			// dz Pul Err in local  Sti frame
float lPulEmx[3];			// hit error mtx:yy,yz,zz

float lPsi;			//  track Psi in local  Sti frame
float lDip;			//  track Dip in local  Sti frame

//		Globals
float gRHit;			//  Rxy of Hit  in global Sti frame
float gPHit;			//  Phi of Hit  in global Sti frame
float gZHit;			//  Z   of Hit  in global Sti frame
float gPHitErr;			//  Phi Hit err in global Sti frame
float gZHitErr;			//  Z   Hit err in global Sti frame
float gHitEmx[3];		//  hit error mtx:PhiPhi,PhiZ,ZZ

float gRFit;			//  Rxy of Fit  in global Sti frame
float gPFit;			//  Phi of Fit  in global Sti frame
float gZFit;			//  Z   of Fit  in global Sti frame
float gPFitErr;			//  Phi Fit err in global Sti frame
float gZFitErr;			//  Z   Fit err in global Sti frame
float gFitEmx[3];			//  hit error mtx:PhiPhi,PhiZ,ZZ

float gPPul;			//  dPhi*Rxy of Pul  in global Sti frame
float gZPul;			//  dZ       of Pul  in global Sti frame
float gPPulErr;			//  dPhi Pul err in global Sti frame
float gZPulErr;			//  dZ   Pul err in global Sti frame
float gPulEmx[3];			//  hit error mtx:PhiRPhiR,PhiRZ,ZZ

float gPsi;			//  track Psi in global  Sti frame
float gDip;			//  track Dip in global  Sti frame
char mEnd[1];
  ClassDef(StiPullHit,1);
};

class StiPullEvent : public TObject {
public:
   StiPullEvent();
  ~StiPullEvent()			{;}
void Clear(const char *opt = "");	
void Add(StiPullHit &ph,int gloPrim);
void Add(StiPullTrk &pt,int gloPrim=0);
const int *GetNHits() const;
public:
   int   mRun;
   int   mEvt;
   TDatime mDate;	//DAQ time (GMT)

   float mVtx[3];	//Primary vertex position in global frame
   float mEtx[6];	//errors xx,yx,yy,zx,zy,zz
   float mChi2;         //Chi square of vertex fit
   int   mNTrks[2];     //N glob,N Prim tracks 
   int   mNHits[4];     //nTpc,nSvt,nSsd,nRnd hits

TClonesArray mTrksG;	//global  tracks
TClonesArray mTrksP;	//primary  tracks
TClonesArray mHitsG;	//StiPullHits for global  tracks
TClonesArray mHitsP;	//StiPullHits for primary tracks
TClonesArray mHitsR;	//StiPullHits for Rnd detectors
  ClassDef(StiPullEvent,4);
};

  
#endif
