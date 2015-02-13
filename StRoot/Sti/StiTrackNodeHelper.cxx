#include <stdio.h>
#include <stdlib.h>
#include "StiUtilities/StiDebug.h"
#include "StiTrackNodeHelper.h"
#include "StiElossCalculator.h"
#include "StDetectorDbMaker/StiHitErrorCalculator.h"
#include "StMessMgr.h"
#include "TArrayD.h"
#include "StarRoot/THelixTrack.h"
#if ROOT_VERSION_CODE < 331013
#include "TCL.h"
#else
#include "TCernLib.h"
#endif

#define NICE(a) ( ((a) <= -M_PI)? ((a)+2*M_PI) :\
                  ((a) >   M_PI)? ((a)-2*M_PI) : (a))

#define sinX(a) StiTrackNode::sinX(a)
static const double kMaxEta = 1.5;
static const double kMaxCur = 0.2;

static const double DY=0.9,DZ=0.9,DEta=0.1,DPti=3,DTan=0.1;
static const double MAXSTEP[]={0,DY,DZ,DEta,DPti,DTan};
static const double ERROR_FACTOR = 2.;
int StiTrackNodeHelper::_debug = 0;
int StiTrackNodeHelper::mgCutStep=0;
//______________________________________________________________________________
int errTest(StiNodePars &predP,StiNodeErrs &predE,
            const StiHit *hit,StiHitErrs &hitErr,
            StiNodePars &fitdP,StiNodeErrs &fitdE,double chi2);

//______________________________________________________________________________
void StiTrackNodeHelper::set(double chi2Max,double chi2Vtx,double errConfidence,int iter)
{
  reset();
  mChi2Max = chi2Max;
  mChi2Vtx = chi2Vtx;
  mNodeErrFactor = 10;
  mHitsErrFactor = 1.;
  if (errConfidence>0.1) {
    mNodeErrFactor = (1./(errConfidence));
    mHitsErrFactor = (1./(1. - errConfidence));
  }
  mIter = iter;
  if (!mIter) mFlipFlopNode = 0;
}
//______________________________________________________________________________
void StiTrackNodeHelper::reset()
{ 
  memset(mBeg,'A',mEnd-mBeg+1);
  mWorstNode =0;
  mVertexNode=0;
  mUsed = 0;
  mCurvQa.reset();
  mTanlQa.reset();
}

//______________________________________________________________________________
void StiTrackNodeHelper::set(StiKalmanTrackNode *pNode,StiKalmanTrackNode *sNode)		
{
  if(!pNode) reset();
  mParentNode = pNode;
  mTargetNode = sNode;
  mTargetHz = mTargetNode->getHz();
  mParentHz = mTargetHz;
  mWallx = mTargetNode->getWallx();
  if (mParentNode) {
    mParentHz = mParentNode->getHz();
    assert(fabs(mParentHz-mParentNode->mFP.hz()) < kEC*0.1); // allow the difference in 100 Gauss. TODO check why 10 is not enough
    mParentNode->mFP.check("2StiTrackNodeHelper::set");
  }

  mDetector   = mTargetNode->getDetector();
  if (!mDetector) mVertexNode = mTargetNode;
  mHit        = mTargetNode->getHit();
  if (mHit) {
    double time = 0;
    if (mHit->vy() || mHit->vz()) time = mTargetNode->getTime();
    mHitPars[0]=mHit->x();
    mHitPars[1]=mHit->y(time);
    mHitPars[2]=mHit->z(time);
  }

  if (!mIter) mTargetNode->mFlipFlop=0;
}
//______________________________________________________________________________
int StiTrackNodeHelper::propagatePars(const StiNodePars &parPars
                                     ,      StiNodePars &rotPars
			             ,      StiNodePars &proPars)
{
  int ierr = 0;
  mAlpha = mTargetNode->_alpha - mParentNode->_alpha;
  mCa=1;mSa=0;
  if (parPars.check()) return 1;;
  rotPars = parPars;
  if (fabs(mAlpha) > 1.e-6) { //rotation part
    rotPars.rotate(mAlpha);
  }// end of rotation part
  
//  	Propagation 
  mX1 = rotPars.x();
  mX2 = mWallx;
  mDx = mX2-mX1;
  mRho = 0.5*(mTargetHz*rotPars.ptin()+rotPars.curv());
  mDSin = mRho*mDx;
  mSinCA2=rotPars._sinCA + mDSin; 
  if (mSinCA2> 0.95) mSinCA2= 0.95;
  if (mSinCA2<-0.95) mSinCA2=-0.95;
  mCosCA2 = ::sqrt((1.-mSinCA2)*(1.+mSinCA2));
  mSumSin   = rotPars._sinCA+mSinCA2;
  mSumCos   = rotPars._cosCA+mCosCA2;
  if (fabs(mSumCos) <1e-4) {mCosCA2*=-1; mSumCos   = rotPars._cosCA+mCosCA2;}
  mDy = mDx*(mSumSin/mSumCos);
  mY2 = rotPars.y()+mDy;
  mDl0 = rotPars._cosCA*mDx+rotPars._sinCA*mDy;
  mSind = mDl0*mRho;
  double cosd = mCosCA2*rotPars._cosCA+mSinCA2*rotPars._sinCA;
  if (fabs(mSind) < 0.1 && cosd  >0) { //tiny angle
    mDl = mDl0*(1.+mSind*mSind/6);
  } else {
    mDl = atan2(mSind,cosd)/mRho;
  }

  proPars.x() = mX2;
  proPars.y() = mY2;
  proPars.z() = rotPars.z() + mDl*rotPars.tanl();
  proPars.eta() = (rotPars.eta()+mRho*mDl);  					
  proPars.eta() = NICE(proPars.eta());  					
  proPars.ptin() = rotPars.ptin();
  proPars.hz()   = mTargetHz;
  proPars.curv() = proPars.ptin()*mTargetHz;
  proPars.tanl() = rotPars.tanl();
  proPars._sinCA   = mSinCA2;
  proPars._cosCA   = mCosCA2;
  ierr = proPars.check();
  assert(!proPars.nan());
  assert(!proPars.nan());
  if (ierr) return 2;
  return 0;
} 
//______________________________________________________________________________
int StiTrackNodeHelper::propagateFitd()
{
//		account eloss in matrix of transofrmation
//		remember that in matrix diagonal and correction factors
//              1. subtructed.
   int ierr = 0;
   mBestPars.ptin() *= (1+mMcs._ptinCorr);
   mBestPars.curv() *= (1+mMcs._ptinCorr);
   mMtx.A[4][4] = (mMtx.A[4][4]+1)*(1+mMcs._ptinCorr) -1;

   StiNodePars rotPars;
   ierr = propagatePars(mFitdParentPars,rotPars,mPredPars);
   if (ierr) return 1;
//   cutStep(&mBestPars,&mPredPars);
   mPredPars.hz() = mTargetHz;
   mPredPars.ptin() *= (1+mMcs._ptinCorr);
   mPredPars.curv() *= (1+mMcs._ptinCorr);
   mPredPars.ready();
   ierr = mPredPars.check();
   if (ierr) return 2;
   return 0;
}



//______________________________________________________________________________
int StiTrackNodeHelper::propagateMtx()
{
//  	fYE == dY/dEta
  double fYE= mDx*(1.+mBestParentRotPars._cosCA*mCosCA2+mBestParentRotPars._sinCA*mSinCA2)/(mSumCos*mCosCA2);
//	fZE == dZ/dEta
  double dLdEta = mDy/mCosCA2;
  double fZE =  mBestPars.tanl()*dLdEta;
//  	fZT == dZ/dTanL; 
  double fZT= mDl; 


//	fEC == dEta/dRho
  double fEC = mDx/mCosCA2;
//	fYC == dY/dRho
  double fYC=(mDl0)/mSumCos*fEC;
// 	fZC == dZ/dRho
  double dang = mDl*mRho;
  double C2LDX = mDl*mDl*(
               0.5*mSinCA2*pow((1+pow(dang/2,2)*sinX(dang/2)),2) +
                   mCosCA2*dang*sinX(dang));

  double fZC = mBestPars.tanl()*C2LDX/mCosCA2;

  fEC*=mTargetHz; fYC*=mTargetHz;fZC*=mTargetHz;
    

  mMtx.reset();
//  X related derivatives
  mMtx.A[0][0] = -1;
  mMtx.A[1][0] = -mSinCA2/mCosCA2; 
  mMtx.A[2][0] = -mBestPars.tanl()/mCosCA2 ;
  mMtx.A[3][0] = -mBestPars.curv()/mCosCA2 ;       ;

  mMtx.A[1][3]=fYE; mMtx.A[1][4]=fYC; mMtx.A[2][3]=fZE;
  mMtx.A[2][4]=fZC; mMtx.A[2][5]=fZT; mMtx.A[3][4]=fEC;
  double fYX = mMtx.A[1][0]; 
  mMtx.A[1][0] = fYX*mCa-mSa;
  mMtx.A[1][1] = fYX*mSa+mCa-1;
  return 0;
}
//______________________________________________________________________________
int StiTrackNodeHelper::propagateError()
{
static int nCall = 0; nCall++; StiDebug::Break(nCall);
  mPredErrs = mFitdParentErrs;
  StiTrackNode::errPropag6(mPredErrs.A,mMtx.A,kNPars);

  mPredErrs.recov();
  mPredErrs._cEE+=mMcs._cEE;		//add err to <eta*eta> eta crossing angle//add err to <eta*eta> eta crossing angle
  mPredErrs._cPP+=mMcs._cPP;    	//add err to <curv*curv>		 //add err to <curv*curv>
  mPredErrs._cTP+=mMcs._cTP;    	//add err to <tanL*curv>		 //add err to <tanL*curv>
  mPredErrs._cTT+=mMcs._cTT;    	//add err to <tanL*tanL>		 //add err to <tanL*tanL>
  int ierr = mPredErrs.check();
  if (ierr) return 1;
  return 0;
}

//______________________________________________________________________________
int StiTrackNodeHelper::makeFit(int smooth)
{
static int nCall=0;
nCall++;
StiDebug::Break(nCall);
  int ierr=0;
  mState = 0;
  mChi2 = 1e13;
  if (mParentNode) {
//		Select the best params to make matrix

//		Get best params for derivatives
    if (smooth) {//joined pars always the best
      mBestParentPars = mJoinPars;
      mBestParentErrs = mJoinErrs;
      mBestDelta      = mJoinErrs.getDelta();
    } else {
      double delta = mFitdErrs.getDelta();
      double er1 = delta*delta;
      double er2 = mSavdDelta*mSavdDelta;
      double wt = er1/(er1+er2);
      mBestParentPars = mFitdPars;	
      mBestParentPars.merge(wt,mSavdParentPars);
      mBestDelta = sqrt(er1*er2/(er1+er2));

    }
    mBestParentPars.check("1makeFit"); 
    mFitdParentPars = mFitdPars;
    mFitdParentErrs = mFitdErrs;
    mFitdParentPars.check("2makeFit");
    mFitdParentErrs.check("3makeFit");

    ierr = propagatePars(mBestParentPars,mBestParentRotPars,mBestPars);
    if(ierr) return 1;
    ierr = propagateMtx();	if(ierr) return 2;
    ierr = propagateMCS();	if(ierr) return 3;
    ierr = propagateFitd();	if(ierr) return 4;
    ierr = propagateError();	if(ierr) return 5;

  } 
  mState = StiTrackNode::kTNReady;

//		Save parameters for future, 
//		when target node will became parent one
  mSavdParentPars = mTargetNode->mFP;
  mSavdDelta = (mTargetNode->isValid())? mTargetNode->mFE.getDelta():3e33;

  if (!mParentNode) {
    if (!smooth) mgCutStep = 0;
    mPredErrs = mTargetNode->mFE;
    ierr = mPredErrs.check(); 	if (ierr) return 11;
    mPredPars = mTargetNode->mFP;
    ierr = mPredPars.check();	if (ierr) return 12;
    mBestPars = mPredPars;
    mBestDelta = mPredErrs.getDelta();
    mJoinPars = mPredPars;
    resetError(mNodeErrFactor);
  }
// 		Set fitted pars to predicted for the absent hit case
  mFitdPars = mPredPars;
  mFitdErrs = mPredErrs;

  int ians = 1;
  mChi2 =0;
  do {//technical (fake) loop
    nudge();
    if (!mHit) 		break;
    setHitErrs();
    mChi2 = 3e33;
    double chi2 = evalChi2();
    if (mTargetNode == mVertexNode) {
      if (chi2>mChi2Vtx) 		return 14;
    } else {
      if (chi2>mChi2Max)		break;
    }
    mChi2 = chi2; if (mChi2>999) mChi2=999;
    ians = updateNode();
    if (debug() & 8) {cout << Form("%5d ",ians); StiKalmanTrackNode::PrintStep();}
    if (!ians) 	break;
    if (mTargetNode == mVertexNode)	return 15;
    mState = StiTrackNode::kTNReady;
    mFitdPars = mPredPars;
    mFitdErrs = mPredErrs;
    mChi2 = 3e33;
  }while(0);

  ierr  = (!smooth)? save():join();
  if (ierr) 				return 16;


  do { //fake loop
    if (!smooth) 			break;
    if (!mHit) 				break;
    if (mDetector && (!mFlipFlopNode || mTargetNode->mFlipFlop > mFlipFlopNode->mFlipFlop))
       {mFlipFlopNode=mTargetNode;}
    if(mState!=StiTrackNode::kTNFitEnd) break;
    mUsed++;
    if (mDetector && (!mWorstNode || mChi2>mWorstNode->getChi2())) 
       {mWorstNode=mTargetNode;}
    if (!mParentNode) 			break;
    double accu,errr;
    accu = mJoinPars.ptin() - mBestParentPars.ptin()*(1+mMcs._ptinCorr);
    errr = sqrt(0.5*(fabs(mJoinErrs._cPP)+fabs(mBestParentErrs._cPP)));
    if (errr > 0) mCurvQa.add(accu/errr);    
    accu = mJoinPars.tanl() - mBestParentPars.tanl();
    errr = sqrt(0.5*(fabs(mJoinErrs._cTT)+fabs(mBestParentErrs._cTT)));
    if (errr > 0) mTanlQa.add(accu/errr);    
  }while(0);
    
  return 0;
}  
  
//______________________________________________________________________________
int StiTrackNodeHelper::join()
{
static int nCall=0;nCall++;
StiDebug::Break(nCall);

  enum {kOLdValid=1,kNewFitd=2,kJoiUnFit=4};
//  if (!mParentNode) return 0;


  int ierr = 0;
  double chi2;		
    
  StiDebug::Break(mTargetNode->mId);
  int kase = mTargetNode->isValid();
  if (mState==StiTrackNode::kTNFitEnd) kase |=kNewFitd;
static int oldJoinPrim = StiDebug::iFlag("StiOldJoinPrim");
if (!oldJoinPrim) {
  if (mTargetNode==mVertexNode) kase = kNewFitd; //ignore old info for primVtx
						 //Hack to accoont specific 
						 //fit to primVtx
}
  do {
    switch(kase) {
      case 0:					// Old invalid & New UnFitd
        mChi2 = (mHit)? 3e33:0;
        mState = StiTrackNode::kTNReady;

      case kNewFitd: 				// Old invalid & New Fitd
        mJoinPars = mFitdPars;
        mJoinErrs = mFitdErrs;
        kase = -1; break;

      case kOLdValid|kNewFitd|kJoiUnFit:	// Old valid & New Fitd & Join UnFit
        mChi2 = 3e33;
        mFitdPars = mPredPars;
        mFitdErrs = mPredErrs;
        mState = StiTrackNode::kTNReady;
     
      case kOLdValid:;				// Old valid & New UnFitd
      case kOLdValid|kNewFitd:;			// Old valid & New Fitd

        mTargetNode->mPE().recov();
        mFitdErrs.recov();
	chi2 = joinTwo(kNPars,mTargetNode->mPP().P,mTargetNode->mPE().A
                      ,kNPars,mFitdPars.P         ,mFitdErrs.A
                             ,mJoinPars.P         ,mJoinErrs.A);
        mJoinPars.hz() = mTargetHz;
	mJoinErrs.recov();


	if (kase == (kOLdValid|kNewFitd)) {	//Check errors improvements
//VP	  assert(mHrr.hYY > mJoinErrs._cYY);
//VP	  assert(mHrr.hZZ > mJoinErrs._cZZ);
          if (mHrr.hYY <= mJoinErrs._cYY) {
            LOG_DEBUG << Form("StiTrackNodeHelper::updateNode() WRONG hYY(%g) < nYY(%g)"
                   ,mHrr.hYY,mFitdErrs._cYY)<< endm;
            return -13;
	  }
          if (mHrr.hZZ <= mJoinErrs._cZZ) {
            LOG_DEBUG << Form("StiTrackNodeHelper::updateNode() WRONG hZZ(%g) < nZZ(%g)"
                  ,mHrr.hZZ,mFitdErrs._cZZ) << endm;
            return -14;
	  }
        } //End  Check errors improvements
	mJoinPars.ready();
	ierr = mJoinPars.check(); 	if (ierr) return 2;
	if (kase!=(kOLdValid|kNewFitd)) {kase = -1; break;}		

        mChi2 = 3e33;
//	chi2 = joinChi2();		//Test join Chi2
	chi2 = recvChi2();		//Test join Chi2
        mChi2 = 3e33;
        if (chi2>mChi2Max && mTargetNode!=mVertexNode) 
	   				{ kase |=kJoiUnFit; break;} //join Chi2 too big
        mChi2 = (chi2>999)? 999:chi2;
        mState = StiTrackNode::kTNFitEnd;
        kase = -1; break;
	
       default: assert(0);
     }//end Switch
  } while(kase>=0);

   assert(fabs(mJoinPars.hz()-mTargetHz)<=1e-10);
   assert(fabs(mTargetNode->getHz()-mTargetHz)<=1e-10);


  mTargetNode->mFE   = mJoinErrs;
  mTargetNode->mPE() = mJoinErrs;
  mTargetNode->mFP   = mJoinPars;
  mTargetNode->mPP() = mJoinPars;
  mTargetNode->mHrr  = mHrr;
  if (mTargetNode!=mVertexNode) mTargetNode->mUnTouch = mUnTouch;
  mTargetNode->_state = mState;
  if (mHit && ((mChi2<1000) != (mTargetNode->getChi2()<1000))) mTargetNode->mFlipFlop++;
  if (mTargetNode!=mVertexNode) mTargetNode->setChi2(mChi2);

//	Sanity check
  double myMaxChi2 = (mTargetNode==mVertexNode)? 1000:mChi2Max;
  if (mState ==  StiTrackNode::kTNFitEnd) {
    assert(mChi2 <myMaxChi2);
  } else {
    assert(mChi2 <=0 || mChi2 >=myMaxChi2);
  }

  return 0;
}
//______________________________________________________________________________
double StiTrackNodeHelper::joinTwo(int nP1,const double *P1,const double *E1
                                  ,int nP2,const double *P2,const double *E2
	                          ,              double *PJ,      double *EJ)
{

  assert(nP1<=nP2);
  int nE1 = nP1*(nP1+1)/2;
  int nE2 = nP2*(nP2+1)/2;
  TArrayD ard(nE2*6);
  double *a = ard.GetArray();  
  double *sumE 		= (a);
  double *sumEI 	= (a+=nE2);
  double *e1sumEIe1 	= (a+=nE2);
  double *subP 		= (a+=nE2);
  double *sumEIsubP	= (a+=nE2);
  double chi2=3e33,p,q;

// Choose the smalest errors
  const double *p1 = P1, *p2 = P2, *e1 = E1, *e2 = E2, *t;
  double choice = (nP1==nP2)? 0:1;
  if (!choice   ) {
    for (int i=0,n=1;i<nE2;i+=(++n)) {
    p=fabs(e1[i]);q=fabs(e2[i]);choice += (p-q)/(p+q+1e-10);
  }}
  if ( choice >0) {t = p2; p2 = p1; p1 = t; t = e2; e2 = e1; e1 = t;}

  do {//empty loop
//  	Join errors
    TCL::vadd(e1,e2,sumE,nE1);
    int negati = sumE[2]<0;
    if (negati) TCL::vcopyn(sumE,sumE,nE1);
    int ign0re = sumE[0]<=0;
    if (ign0re) sumE[0] = 1;
    TCL::trsinv(sumE,sumEI,nP1);
    if (ign0re) {sumE[0]  = 0; sumEI[0] = 0;}
    if (negati) {TCL::vcopyn(sumE,sumE,nE1);TCL::vcopyn(sumEI,sumEI,nE1);}
    TCL::vsub(p2       ,p1   ,subP       ,nP1);
    TCL::trasat(subP,sumEI,&chi2,1,nP1); 
    if (!EJ) break;
    TCL::trqsq (e1  ,sumEI,e1sumEIe1,nP2); 
    TCL::vsub(e1,e1sumEIe1,EJ,nE2);
  } while(0);
//  	Join params
  if (PJ) {
    TCL::tras(subP     ,sumEI,sumEIsubP,1,nP1);
    TCL::tras(sumEIsubP,e1   ,PJ       ,1,nP2);
    TCL::vadd(PJ       ,p1   ,PJ         ,nP2);
  }
  return chi2;
}
#if 0
//______________________________________________________________________________
double StiTrackNodeHelper::joinVtx(const double *P1,const double *E1
                                  ,const double *P2,const double *E2
	                          ,      double *PJ,      double *EJ)
{
  enum {nP1=3,nE1=6,nP2=kNPars,nE2=kNErrs};
  double E1m[nE1],E2m[nE2];

  TCL::vzero(E1m       ,nE1);
  TCL::ucopy(E2,E2m    ,nE2);
  TCL::vadd (E1,E2m,E2m,nE1);
  return joinTwo(nP1,P1,E1m,nP2,P2,E2m,PJ,EJ);

}
#endif//0
#if 0
//______________________________________________________________________________
double StiTrackNodeHelper::joinVtx(const double *P1,const double *E1
                                  ,const double *P2,const double *E2
	                          ,      double *PJ,      double *EJ)
{
  enum {nP1=3,nE1=6,nP2=kNPars,nE2=kNErrs};

  TArrayD ard(nE2*7);
  double *p = ard.GetArray();
  double *sumE 			= (p);
  double *sumEI 		= (p+=nE2);
  double *sumEI_E1_sumEI	= (p+=nE2);
  double *E2_sumEI_E2		= (p+=nE2);
  double *E2_sumEI_E1_sumEI_E2	= (p+=nE2);
  double *sumEIsubP		= (p+=nE2);
  double *subP 			= (p+=nE2);

  double chi2=3e33;


  do {//empty loop
//  	Join errors
    TCL::ucopy(E2,sumE,nE1);
    int ign0re = sumE[0]<=0;
    if (ign0re) sumE[0] = 1;
    TCL::trsinv(sumE,sumEI,nP1);
    if (ign0re) {sumE[0]  = 0; sumEI[0] = 0;}
    TCL::vsub(P1       ,P2   ,subP       ,nP1);
    TCL::trasat(subP,sumEI,&chi2,1,nP1); 
    if (!EJ) break;
    TCL::trqsq (E2  ,sumEI,E2_sumEI_E2,nP2); 
    TCL::vsub(E2,E2_sumEI_E2,EJ,nE2);
//   	Now account errors of vertex itself
    TCL::trqsq (sumEI,E1,sumEI_E1_sumEI,nP1); 
    TCL::trqsq (E2,sumEI_E1_sumEI,E2_sumEI_E1_sumEI_E2,nP2); 
    TCL::vadd(EJ,E2_sumEI_E1_sumEI_E2,EJ,nE2);

  } while(0);
//  	Join params
  if (PJ) {
    TCL::tras(subP     ,sumEI,sumEIsubP,1,nP1);
    TCL::tras(sumEIsubP,E2   ,PJ       ,1,nP2);
    TCL::vadd(PJ       ,P2   ,PJ         ,nP2);
  }
  return chi2;
}
#endif//1
//______________________________________________________________________________
double StiTrackNodeHelper::joinVtx(const double      *Y,const StiHitErrs  &B
                                  ,const StiNodePars &X,const StiNodeErrs &A
	                          ,      StiNodePars *M,      StiNodeErrs *C)
{
// Y : x,y,z of vertex.  	B: error matrix of Y
// X : track parameters. 	A: error matrix of X
// M : track parameters of X+Y.	C: error matrix of M


  enum {nP1=3,nE1=6,nP2=6,nE2=21};

  StiNodeErrs Ai=A;	//Inverted A
  
  Ai._cXX=1;
  TCL::trsinv(Ai.A,Ai.A,nP2);
  Ai._cXX=0;


  double Ai11i[6],Ai10[3][3],T10[3][3],dif[6],m[6];
  Ai.get11(Ai11i);  
  Ai.get10(Ai10[0]);  
  TCL::trsinv(Ai11i ,Ai11i,3);
  TCL::trsa  (Ai11i,Ai10[0],T10[0],3,3);	//Ai11*Ai10
  TCL::ucopy(Y,m,3);
  TCL::vsub (X.P,Y,dif,3);
  TCL::mxmpy(T10[0],dif,m+3,3,3,1);
  TCL::vadd(X.P+3,m+3,m+3,3);			//m:   resulting params
  if (M) {TCL::ucopy(m,M->P,nP2); M->ready();}	//fill resulting params

  TCL::vsub(X.P,m,dif,nP2);			//dif = X - M
  double chi2;
  TCL::trasat(dif,Ai.A,&chi2,1,nP2);		//calc chi2
  if (!C) return chi2;
		// Error matrix calculation
  C->reset();

  double TX[nP1][nP2];memset(TX[0],0,sizeof(TX));
  for (int i=0;i<3;i++) {TCL::ucopy(T10[i],TX[i],3); TX[i][i+3]=1;}
  double C11[nE1];
  TCL::trasat(TX[0],A.A,C11,nP1,nP2);
  C->set11(C11);
  
  double TY[nP2][nP1];memset(TY[0],0,sizeof(TX));
  for (int i=0;i<3;i++) {TCL::vcopyn(T10[i],TY[i+3],3); TY[i][i]=1;}
  TY[0][0] = 0;
  double CYY[nE2];
  TCL::trasat(TY[0],B.A,CYY,nP2,nP1);
  TCL::vadd(CYY,C->A,C->A,nE2);
  return chi2;
}
//______________________________________________________________________________
int StiTrackNodeHelper::save()
{
   assert(fabs(mPredPars.hz()-mTargetHz)<=1e-10);
   assert(fabs(mFitdPars.hz()-mTargetHz)<=1e-10);
   assert(fabs(mTargetNode->getHz()-mTargetHz)<=1e-10);
   
   mTargetNode->mPP() = mPredPars;
   mTargetNode->mFP   = mFitdPars;
   mTargetNode->mPE() = mPredErrs;
   mTargetNode->mFE   = mFitdErrs;
   mTargetNode->_state = mState;
   if (mHit && ((mChi2<1000) != (mTargetNode->getChi2()<1000))) mTargetNode->mFlipFlop++;
   if (mTargetNode!=mVertexNode) mTargetNode->setChi2(mChi2);
   return 0;
}
//______________________________________________________________________________
/*! Calculate the effect of MCS on the track error matrix.
  <p>
  The track is assumed to propagate from (x0,y0,z0) to (mX1,y1,z1). The calculation
  is performed for the given mass hypothesis which given a momentum determines the
  speed "beta" of the particle. The calculation of the average scattering angle
  is delegated to the function "mcs2". The calculation of energy loss is done
  by the function eloss.
 */
//______________________________________________________________________________
int StiTrackNodeHelper::propagateMCS()
{  
static int nCall = 0; nCall++;
StiDebug::Break(nCall);
static const int keepElossBug = StiDebug::iFlag("keepElossBug");
  mMcs.reset();
  if (!mDetector) 			return 0;
  mMcs._ptinCorr =  0;
  if (fabs(mBestPars.ptin())<=1e-3)	return 0;
  double pt     = 1./(fabs(mBestPars.ptin())+1e-11);//???2
assert(pt<1e3);
  double relRadThickness;
  // Half path length in previous node
  double pL1,pL2,pL3,d1,d2,d3,dxEloss,mDx;
  pL1=0.5*pathIn(mParentNode->getDetector(),&mBestParentPars);
  // Half path length in this node
  pL3=0.5*pathIn(mDetector,&mBestPars);
  // Gap path length
  pL2= fabs(mDl);
  double x0p = 1e11,x0Gas=1e11,x0=1e11;
  mDx = mBestPars.x() - mBestParentRotPars.x();
  double tanl   = mBestPars.tanl();
  double pti    = mBestPars.ptin(); 
  double p2     = (1.+tanl*tanl)*pt*pt;
  double m      = StiKalmanTrackFinderParameters::instance()->getMassHypothesis();
  double m2     = m*m;
  double e2     = p2+m2;
  double beta2  = p2/e2;
if (keepElossBug) {	//Old Eloss bug prezerved

  d1    = mParentNode->getDensity();
  x0p   = mParentNode->getX0();
  d3    = mDetector->getMaterial()->getDensity();
  x0    = mDetector->getMaterial()->getX0();

  if (pL2> (pL1+pL3)) {

      pL2=pL2-pL1-pL3;
      if (mDx>0)  {
	x0Gas = mDetector->getGas()->getX0();
	d2    = mDetector->getGas()->getDensity();
      } else {
	x0Gas = mParentNode->getGasX0(); 
	d2    = mParentNode->getGasDensity();
      }

      relRadThickness = 0.;
      dxEloss = 0;

      if (x0p>0.) {
	 relRadThickness += pL1/x0p;
	 dxEloss += d1*pL1;
      }
      if (x0Gas>0.) {
	relRadThickness += pL2/x0Gas;
	dxEloss += d2*pL2;
      }
      if (x0>0.){ 
	relRadThickness += pL3/x0;
	dxEloss += d3*pL3;
      }
    } else { //  pL2<=(pL1+pL3)
      pL2 = 0;
      relRadThickness = 0.; 
      dxEloss = 0;
      if (x0p>0.) {
	relRadThickness += pL1/x0p;
	dxEloss += d1*pL1;
      }
      if (x0>0.){
	relRadThickness += pL3/x0;
	dxEloss += d3*pL3;
      }
    }
  double theta2 = StiKalmanTrackNode::mcs2(relRadThickness,beta2,p2);
  double cos2Li = (1.+ tanl*tanl);  // 1/cos(lamda)**2
  double f = mHitsErrFactor; 
  mMcs._cEE = cos2Li 		*theta2*f;
  mMcs._cPP = tanl*tanl*pti*pti	*theta2*f;
  mMcs._cTP = pti*tanl*cos2Li	*theta2*f;
  mMcs._cTT = cos2Li*cos2Li	*theta2*f;

  double dE=0;
  double sign = (mDx>0)? 1:-1;

//  const static double I2Ar = (15.8*18) * (15.8*18) * 1e-18; // GeV**2
  StiElossCalculator * calculator = mDetector->getMaterial()->getElossCalculator();
assert(calculator);
  double eloss = calculator->calculate(1.,m, beta2);
  dE = sign*dxEloss*eloss;
//		save detLoss and gasLoss for investigation only
//  mTargetNode->setELoss(2*sign*d3*eloss,sign*d2*eloss);



  mMcs._ptinCorr = ::sqrt(e2)*dE/p2;
if (fabs(mMcs._ptinCorr)>1e-4) {


}   


  if (fabs(mMcs._ptinCorr)>0.1) mMcs._ptinCorr = (dE<0)? -0.1:0.1;

} else { // ELoss bug fixed =====================================================================


  const StiMaterial 		*curMat = mDetector->getMaterial();
  const StiElossCalculator	*curLos = curMat->getElossCalculator();
  d3 =(curLos) ? curLos->calculate(1.,m, beta2):0;
  x0 = curMat->getX0();
  const StiMaterial		*curGas = mDetector->getGas();


  const StiDetector 		*preDet = mParentNode->getDetector();
  const StiMaterial *preMat = curGas,*preGas=curGas;
  const StiElossCalculator *preLos = 0;
  if (preDet) {
    preMat = preDet->getMaterial();
    preGas = preDet->getGas();
    if (preMat) {
      preLos = preMat->getElossCalculator();
      x0p    = preMat->getX0();
      if (preLos) {
        d1  = preLos->calculate(1.,m, beta2);
  } } }
//		Gas is UNDER detector
  const StiMaterial		*gasMat = (mDx>0)?curGas : preGas;
  if (gasMat) {
    x0Gas = gasMat->getX0();
    const StiElossCalculator	*gasLos = gasMat->getElossCalculator();
    if (gasLos) {
      d2 = gasLos->calculate(1.,m, beta2);
  } }


  pL2=pL2-pL1-pL3; if (pL2<0) pL2=0;
  relRadThickness = pL1/x0p+pL2/x0Gas+pL3/x0;

  dxEloss         =  d1*pL1+ d2*pL2  + d3*pL3;

  double theta2 = StiKalmanTrackNode::mcs2(relRadThickness,beta2,p2);
  double cos2Li = (1.+ tanl*tanl);  // 1/cos(lamda)**2
  double f = mHitsErrFactor; 
  mMcs._cEE = cos2Li 		*theta2*f;
  mMcs._cPP = tanl*tanl*pti*pti	*theta2*f;
  mMcs._cTP = pti*tanl*cos2Li	*theta2*f;
  mMcs._cTT = cos2Li*cos2Li	*theta2*f;
assert(mMcs._cPP>0);
assert(mMcs._cTT>0);
assert(mMcs._cEE>0);


  int sign = ( mDx>=0)? 1:-1;
  double dE = sign*dxEloss;
//		save detLoss and gasLoss for investigation only
  StiELoss *el = mTargetNode->getELoss();
  el[0].mELoss = 2*d3*pL3;
  el[0].mLen   = 2*pL3;
  el[0].mDens  = curMat->getDensity();
  el[0].mX0    = x0;
  el[1].mELoss = 2*d2*pL2;
  el[1].mLen   = 2*pL2;
  el[1].mDens  = gasMat->getDensity();
  el[1].mX0    = x0Gas;

  mMcs._ptinCorr = ::sqrt(e2)*dE/p2;
  if (fabs(mMcs._ptinCorr)>0.1) mMcs._ptinCorr = (dE<0)? -0.1:0.1;
}


  return 0;
}
//______________________________________________________________________________
/*! Calculate the increment of chi2 caused by the addition of this node to the track.
  <p>
  Uses the track extrapolation to "_x", and hit position to evaluate and return the 
  increment to the track chi2.
  The chi2 is not stored internally in this node. 
  <p>
  <h3>Notes</h3>
  <ol>
  <li>Use full error matrices.</li>
  <li>Return increment in chi2 implied by the node/hit assocition.</li>
  <li>Throws an exception if numerical problems arise.</li>
  </ol>
*/
//______________________________________________________________________________
double StiTrackNodeHelper::evalChi2() 
{
  double r00, r01,r11,chi2;
  //If required, recalculate the errors of the detector hits.
  //Do not attempt this calculation for the main vertex.
  if (fabs(mPredPars._sinCA)>0.99        )	return 1e41;
  if (fabs(mPredPars.eta())       >kMaxEta) 	return 1e41;
  if (fabs(mPredPars.curv())      >kMaxCur)      return 1e41;
  if (!mDetector) 	{ //Primay vertex
    mHitPars[0] = mPredPars.x();
//    chi2 = joinVtx(mHitPars,mHrr.A,mPredPars.P,mPredErrs.A);
    chi2 = joinVtx(mHitPars,mHrr,mPredPars,mPredErrs);
  } else 		{ //Normal hit

    r00=mPredErrs._cYY+mHrr.hYY;
    r01=mPredErrs._cZY+mHrr.hZY;
    r11=mPredErrs._cZZ+mHrr.hZZ;
    mDetm = r00*r11 - r01*r01;
    if (mDetm<r00*r11*1.e-5) {
      LOG_DEBUG << Form("StiTrackNodeHelper::evalChi2 *** zero determinant %g",mDetm)<< endm;
      return 1e60;
    }
    double tmp=r00; r00=r11; r11=tmp; r01=-r01;  

    double dyt=(mPredPars.y()-mHitPars[1]);
    double dzt=(mPredPars.z()-mHitPars[2]);
    chi2 = (dyt*r00*dyt + 2*r01*dyt*dzt + dzt*r11*dzt)/mDetm;
  }
  return chi2;
}
//______________________________________________________________________________
/*
 * double StiTrackNodeHelper::joinChi2() 
 * {
 *   double chi2;
 *   double mergPars[3],mHitPars[3],mergErrs[6];
 *   chi2 = joinTwo(3,mPredPars.P         ,mPredErrs.A
 *                 ,3,mTargetNode->mPP().P,mTargetNode->mPE().A
 *                   ,mergPars            ,mergErrs);
 *   
 *   mHitPars[0] = mHit->x();
 *   mHitPars[1] = mHit->y();
 *   mHitPars[2] = mHit->z();
 *   chi2 = joinTwo(3,mergPars,mergErrs,3,mHitPars,mHrr.A);
 * // 	Save untouched by current hit node's y,z & errors for alignment
 *   mUnTouch.mPar[0] = mergPars[1];
 *   mUnTouch.mPar[1] = mergPars[2];
 *   mUnTouch.mErr[0] = mergErrs[2];
 *   mUnTouch.mErr[1] = mergErrs[4];
 *   mUnTouch.mErr[2] = mergErrs[5];
 *   return chi2;
 * }
 */
//______________________________________________________________________________
double StiTrackNodeHelper::recvChi2() 
{
  if (fabs(mJoinPars._sinCA)>0.99        )	return 1e41;
  if (fabs(mJoinPars.eta())       >kMaxEta) 	return 1e41;
  if (fabs(mJoinPars.curv())      >kMaxCur)      return 1e41;
  if (!mDetector) {//Primary vertex
//  double chi2 =joinVtx(mHitPars,mHrr.A,mPredPars.P,mPredErrs.A);
    double chi2 =joinVtx(mHitPars,mHrr  ,mPredPars  ,mPredErrs  );
    return chi2;
  }

  StiHitErrs  myHrr = mHrr;
  StiNodeErrs recovErrs;
  StiNodePars recovPars;
  double f = -(1./mHitsErrFactor);
  myHrr*=f;
  double r11,r12,r22;
  if ((r11=myHrr.hYY+mJoinErrs._cYY) >=0) 	return 1e41;
  if ((r22=myHrr.hZZ+mJoinErrs._cZZ) >=0) 	return 1e41;
  r12 =myHrr.hZY+mJoinErrs._cZY;
  if (r11*r22-r12*r12<0)			return 1e41;	  


  double chi2 = joinTwo(3,mHitPars    ,    myHrr.A
                       ,3,mJoinPars.P,mJoinErrs.A
		         ,recovPars.P,recovErrs.A);
  if (fabs(recovPars.y()-mHitPars[1])>10) StiDebug::Break(-1);
  if (fabs(recovPars.z()-mHitPars[2])>10) StiDebug::Break(-1);

  mUnTouch.set(recovPars,recovErrs);
  return -chi2; //account that result is negative
}
//______________________________________________________________________________
int StiTrackNodeHelper::setHitErrs() 
{
  getHitErrors(mHit,&mFitdPars,&mHrr);
  mHrr*=mHitsErrFactor;
  return 0;
}
//______________________________________________________________________________
/*! Update the track parameters using this node.
  <p>
  This method uses the hit contained by node to update the track 
  parameters contained by this node and thus complete the propagation
  of this track to the location x=_x.
  <p>
  <OL>
  <li>Throw a runtime_error exception if no hit is actually associated with this node.</li>
  <li>Compute the measurement error matrix "r". Invert it.
  <li>Update the measurement matrix "k" and calculate updated curvature, eta, and pitch.
  <li>Update track error matrix.</li>
  </OL>
  <p>
  <h3>Notes</h3>
  <ol>
  <li>Throw logic_error if no hit is associated with this node.</li>
  <li>Throw runtime_error if determinent of "r" matrix is null.
  </ol>
*/
//______________________________________________________________________________
int StiTrackNodeHelper::updateNode() 
{
static int nCall=0; nCall++;
  mState = StiTrackNode::kTNFitBeg;
  double r00,r01,r11;
  StiDebug::Break(mTargetNode->mId);
  if (!mDetector)	{ //Primary vertex
    mHitPars[0] = mPredPars.x();
//  double chi2 = joinVtx(mHitPars,mHrr.A,mPredPars.P,mPredErrs.A,mFitdPars.P,mFitdErrs.A);
    double chi2 = joinVtx(mHitPars,mHrr,mPredPars,mPredErrs,&mFitdPars,&mFitdErrs);
    mFitdPars.curv() = mTargetHz*mFitdPars.ptin();
    assert(chi2>900 || fabs(mChi2-chi2)<1e-10);
    if (debug()) {
      StiKalmanTrackNode::ResetComment(Form("Vertex                        "));
    }
  } else 		{ //Normal Hit
    r00=mHrr.hYY+mPredErrs._cYY;
    r01=mHrr.hZY+mPredErrs._cZY;
    r11=mHrr.hZZ+mPredErrs._cZZ;
    mDetm=r00*r11 - r01*r01;
    if (!(mDetm>(r00*r11)*1.e-5)) return 99;
    assert(mDetm>(r00*r11)*1.e-5);

    // inverse matrix
    double tmp=r00; r00=r11/mDetm; r11=tmp/mDetm; r01=-r01/mDetm;
    // update error matrix
    double k00=mPredErrs._cYY*r00+mPredErrs._cZY*r01, k01=mPredErrs._cYY*r01+mPredErrs._cZY*r11;
    double k10=mPredErrs._cZY*r00+mPredErrs._cZZ*r01, k11=mPredErrs._cZY*r01+mPredErrs._cZZ*r11;
    double k20=mPredErrs._cEY*r00+mPredErrs._cEZ*r01, k21=mPredErrs._cEY*r01+mPredErrs._cEZ*r11;
    double k30=mPredErrs._cPY*r00+mPredErrs._cPZ*r01, k31=mPredErrs._cPY*r01+mPredErrs._cPZ*r11;
    double k40=mPredErrs._cTY*r00+mPredErrs._cTZ*r01, k41=mPredErrs._cTY*r01+mPredErrs._cTZ*r11;

    double myY = mPredPars.y();
    double myZ = mPredPars.z();
    double dyt  = mHitPars[1] - myY;
    double dzt  = mHitPars[2] - myZ;
    double dPt  = k30*dyt + k31*dzt;
    double dEt  = k20*dyt + k21*dzt;
    double dTa  = k40*dyt + k41*dzt;
    double eta  = NICE(mPredPars.eta() + dEt);
    double pti  = mPredPars.ptin() + dPt;
    double tanl = mPredPars.tanl() + dTa;
    // Check if any of the quantities required to pursue the update
    // are infinite. If so, it means the tracks cannot be update/propagated
    // any longer and should therefore be abandoned. Just return. This is 
    // not a big but rather a feature of the fact a helicoidal tracks!!!
    // update Kalman state
    double p0 = myY + k00*dyt + k01*dzt;
    double p1 = myZ + k10*dyt + k11*dzt;
    //mPredPars._tanl += k40*dyt + k41*dzt;
    double sinCA  =  sin(eta);
    // The following test introduces a track propagation error but happens
    // only when the track should be aborted so we don't care...

    mFitdPars.hz() = mTargetHz;
    mFitdPars.x()  = mPredPars.x();
    mFitdPars.y()  = p0;
    mFitdPars.z()  = p1;
    mFitdPars.eta()   = eta;
    mFitdPars.ptin() = pti;
    mFitdPars.curv()  = mTargetHz*pti;
    mFitdPars.tanl()  = tanl;
    mFitdPars._sinCA = sinCA;
    mFitdPars._cosCA = ::sqrt((1.-mFitdPars._sinCA)*(1.+mFitdPars._sinCA)); 
    if (!mDetector) 
      assert(fabs(mFitdPars.y()-mHitPars[1])>1e-10 ||  fabs(mHitPars[0])<4);
//??    cutStep(&mFitdPars,&mPredPars);
//??    cutStep(&mFitdPars,&mBestPars);
    assert(mFitdPars.x()>0);
    if (mFitdPars.check()) return -11;
  // update error matrix
    double c00=mPredErrs._cYY;                       
    double c10=mPredErrs._cZY, c11=mPredErrs._cZZ;                 
    double c20=mPredErrs._cEY, c21=mPredErrs._cEZ;//, c22=mPredErrs._cEE;           
    double c30=mPredErrs._cPY, c31=mPredErrs._cPZ;//, c32=mPredErrs._cPE, c33=mPredErrs._cPP;     
    double c40=mPredErrs._cTY, c41=mPredErrs._cTZ;//, c42=mPredErrs._cTE, c43=mPredErrs._cTP, c44=mPredErrs._cTT;
    mFitdErrs._cYY-=k00*c00+k01*c10;
    mFitdErrs._cZY-=k10*c00+k11*c10;mFitdErrs._cZZ-=k10*c10+k11*c11;
    mFitdErrs._cEY-=k20*c00+k21*c10;mFitdErrs._cEZ-=k20*c10+k21*c11;mFitdErrs._cEE-=k20*c20+k21*c21;
    mFitdErrs._cPY-=k30*c00+k31*c10;mFitdErrs._cPZ-=k30*c10+k31*c11;mFitdErrs._cPE-=k30*c20+k31*c21;mFitdErrs._cPP-=k30*c30+k31*c31;
    mFitdErrs._cTY-=k40*c00+k41*c10;mFitdErrs._cTZ-=k40*c10+k41*c11;mFitdErrs._cTE-=k40*c20+k41*c21;mFitdErrs._cTP-=k40*c30+k41*c31;mFitdErrs._cTT-=k40*c40+k41*c41;
    if (debug()) {
      StiKalmanTrackNode::ResetComment(Form("%30s ",mDetector->getName().c_str()));
    }
  }
  if (mFitdErrs.check()) return -12;
//  mFitdErrs.recov();
  

static int ERRTEST=0;
if(ERRTEST) errTest(mPredPars,mPredErrs,mHit,mHrr,mFitdPars,mFitdErrs,mChi2);

assert(!mFitdPars.nan());
assert(!mFitdErrs.nan());

//prod  assert(mHrr.hYY > mFitdErrs._cYY);
//prod  assert(mHrr.hZZ > mFitdErrs._cZZ);
  if (mDetector) { //Not a primary
    if (mHrr.hYY <= mFitdErrs._cYY) {
      LOG_DEBUG << Form("StiTrackNodeHelper::updateNode() WRONG hYY(%g) < nYY(%g)"
            ,mHrr.hYY,mFitdErrs._cYY)<< endm;
      return -13;
    }  
    if (mHrr.hZZ <= mFitdErrs._cZZ) {
      LOG_DEBUG << Form("StiTrackNodeHelper::updateNode() WRONG hZZ(%g) < nZZ(%g)"
            ,mHrr.hZZ,mFitdErrs._cZZ)<< endm;
      return -14;
    }  
  } //EndIf Not a primary	  
  if (debug()) StiKalmanTrackNode::comment += Form(" chi2 = %6.2f",mChi2);
  if (mTargetNode && debug()) {
    mTargetNode->PrintpT("U");
  }
  mState = StiTrackNode::kTNFitEnd;
  return 0; 
}
 
//______________________________________________________________________________
void StiTrackNodeHelper::resetError(double fk)
{ 
  if (fk) do {//fake loop
    if(mPredErrs._cYY>DY*DY) 		break;
    if(mPredErrs._cZZ>DZ*DZ) 		break;
    if(mPredErrs._cEE>DEta*DEta) 	break;
    if(mPredErrs._cPP>DPti*DPti) 	break;
    if(mPredErrs._cTT>DTan*DTan) 	break;
    mPredErrs*=fk;
    return;
  }while(0);

  mPredErrs.reset();
  mPredErrs._cYY=DY*DY;
  mPredErrs._cZZ=DZ*DZ;
  mPredErrs._cEE=DEta*DEta;
  mPredErrs._cPP=DPti*DPti;
  mPredErrs._cTT=DTan*DTan;
}
//______________________________________________________________________________
int StiTrackNodeHelper::cutStep(StiNodePars *pars,StiNodePars *base)
{
  double fact=1,dif,fak;
  double cuts[kNPars];
  memcpy(cuts,MAXSTEP,sizeof(cuts));
  if (mBestDelta<DY) {cuts[1]=mBestDelta;cuts[2]=mBestDelta;} 

  for (int jx=0;jx<kNPars;jx++) {
    dif =(fabs((*pars)[jx]-(*base)[jx]));
    fak = (dif >cuts[jx]) ? cuts[jx]/dif:1;
    if (fak < fact) fact = fak;
  }
  if (fact>=1) return 0;
  mgCutStep++;
  for (int jx=0;jx<kNPars;jx++) {
    dif =(*pars)[jx]-(*base)[jx];
    (*pars)[jx] = (*base)[jx] +dif*fact;
  }
  pars->ready();
  return 1;
}
//______________________________________________________________________________
int StiTrackNodeHelper::nudge()
{
  double endVal = (mHit)? mHitPars[0]:mWallx;
  StiNodePars *pars = &mBestPars;
  for (int i=0;i<2;i++,pars =&mPredPars) {
    double deltaX = endVal-pars->x();
    if (fabs(deltaX) <1e-6) continue;
    double deltaL = deltaX/pars->_cosCA;
    double deltaE = pars->curv()*deltaL;
    pars->x()      = endVal;
    pars->y()     += pars->_sinCA *deltaL;
    pars->z()     += pars->tanl() *deltaL;
    pars->eta()   +=               deltaE;
    if (fabs(deltaE)>0.001) {pars->ready();}
    else                    {
      double cosCA = pars->_cosCA;
      pars->_cosCA -= pars->_sinCA *deltaE;
      pars->_sinCA +=        cosCA *deltaE;
    if (fabs(pars->_cosCA)>=0.99
      ||fabs(pars->_sinCA)>=0.99) pars->ready();
    }
  if (pars->check()) return 1;
  }
  return 0;
}
//______________________________________________________________________________
double StiTrackNodeHelper::pathIn(const StiDetector *det,StiNodePars *pars)
{
static int nCall =0;nCall++;
  if (!det) return 0.; 
  StiShape *shape = det->getShape();
  double thickness = shape->getThickness();
  double t = pars->tanl();
  double c = fabs(pars->_cosCA);
  double pla = (thickness*::sqrt(1.+t*t)) / c;///???
  if (shape->getShapeCode()==kPlanar) return (thickness*::sqrt(1.+t*t)) / c;

 //cyl or sector
  double r = shape->getOuterRadius() - thickness/2;
  double maxStep = 0.1/(fabs(pars->curv())+1e-3);
  if (maxStep>0.1*r) maxStep=0.1*r;
  double dir[3] = {pars->_cosCA,pars->_sinCA,pars->tanl()};
  THelixTrack hlx(pars->P,dir,pars->curv());
  double *X = hlx.Pos();
  double *D = hlx.Dir();
  double Dot,totS=0,s;
  int it = 10;
  for (; it>0;it--) {
    double rxy = sqrt(X[0]*X[0]+X[1]*X[1]);
    Dot = (X[0]*D[0]+X[1]*D[1])/rxy;
    s = (r*r-X[0]*X[0]-X[1]*X[1])/(2*Dot);
    if (fabs(s)>maxStep) s= (s<0)? -maxStep:maxStep;
    hlx.Move(s); totS += s;
    if (fabs(s)<0.1*thickness+1e-4) break;
  } 
  if (!it) return 0;
  double cyl = (thickness)/Dot;///???
  StiDebug::Count("Cyl-Pla",cyl);
  assert(fabs(cyl)<100);
  return fabs(thickness/Dot);
}
#if 0
//______________________________________________________________________________
double StiTrackNodeHelper::pathIn(const StiDetector *det,StiNodePars *pars)
{
  if (!det) return 0.; 
  StiShape *shape = det->getShape();
  double thickness = shape->getThickness();
  double t = pars->tanl();
  double c = fabs(pars->_cosCA);
  if (shape->getShapeCode()>kPlanar) { //cyl or sector
    double r = shape->getOuterRadius() - thickness/2;
    double sur[7] = { -r*r,0,0,0,1,1,0}; 
    double dir[3] = {pars->_cosCA,pars->_sinCA,pars->tanl()};
    THelixTrack hlx(pars->P,dir,pars->curv());
    double s = hlx.Path(1000.,sur,7,0,0,1);
assert(fabs(s)<100);
    hlx.Move(s);
    double nor = sqrt(pow(hlx.Pos()[0],2)+pow(hlx.Pos()[1],2));
    double myDir[2]={hlx.Pos()[0]/nor,hlx.Pos()[1]/nor};

    double old = (thickness*::sqrt(1.+t*t)) / c;


    c = fabs(hlx.Dir()[0]*myDir[0]+hlx.Dir()[1]*myDir[1]);
    double cyl = (thickness)/c;
assert(fabs(cyl-old)<100);
    StiDebug::Count("Cyl-Pla",cyl-old);
    return (thickness)/c;
  }

  return (thickness*::sqrt(1.+t*t)) / c;
}
#endif

//______________________________________________________________________________
int StiTrackNodeHelper::getHitErrors(const StiHit *hit,const StiNodePars *pars,StiHitErrs *hrr)
{
  hrr->reset();
  const StiDetector *det = hit->detector();
  const StiHitErrorCalculator *calc = (det)? det->getHitErrorCalculator():0;
  if (calc) {//calculate it
     calc->calculateError(pars,hrr->hYY,hrr->hZZ);
  } else    {//get from hit
    const float *ermx = hit->errMtx();    
    for (int i=0;i<6;i++){hrr->A[i]=ermx[i];}
  }
  return (!det);
}
//______________________________________________________________________________
int errTest(StiNodePars &predP,StiNodeErrs &predE,
            const StiHit *hit ,StiHitErrs  &hitErr,
            StiNodePars &fitdP,StiNodeErrs &fitdE, double chi2)
{

  StiNodePars mineP,hittP;
  StiNodeErrs mineE,hittE;
  hittP.x() = hit->x();
  hittP.y() = hit->y();
  hittP.z() = hit->z();
  memcpy(hittE.A,hitErr.A,sizeof(StiNodeErrs));
  
  double myChi2 = StiTrackNodeHelper::joinTwo(
                  3,hittP.P,hittE.A,
		  6,predP.P,predE.A,
		    mineP.P,mineE.A);


  int ndif = 0;
  for (int i=0;i<kNPars;i++) {
    double diff = fabs(mineP.P[i]-fitdP.P[i]);
    if (diff < 1e-10) continue;
    diff/=0.5*(fabs(mineP.P[i])+fabs(fitdP.P[i]));
    if (diff < 1e-5 ) continue;
    ndif++;
    LOG_DEBUG << Form("errTest(P): %g(%d) - %g(%d) = %g",mineE.A[i],i,fitdE.A[i],i,diff)<< endm;
  }
  if (ndif){ mineP.print();fitdP.print();}

  for (int i=0;i<kNErrs;i++) {
    double diff = fabs(mineE.A[i]-fitdE.A[i]);
    if (diff < 1e-10) continue;
    diff/=0.5*(fabs(mineE.A[i])+fabs(fitdE.A[i]));
    if (diff < 1e-5 ) continue;
    ndif+=100;
    LOG_DEBUG << Form("errTest(E): %g(%d) - %g(%d) = %g",mineE.A[i],i,fitdE.A[i],i,diff)<< endm;
  }
  if (ndif>=100){ mineE.print();fitdE.print();}
  
  double diff = fabs((chi2-myChi2)/(chi2+myChi2));
  if (diff > 1e-5 ) {
    ndif+=1000;
    LOG_DEBUG << Form("errTest(C): %g() - %g() = %g",myChi2,chi2,diff)<< endm;
  }

  return ndif;
} 

//_____________________________________________________________________________
void QaFit::add(double val)
{
  double v = val; double p = mPrev; mPrev = v;
  int n = (mTally)? 2:1;
  mTally++;
  for (int i=0;i<n;i++) {
    mAver[i] +=v;
    mErrr[i] +=v*v;
    if (mMaxi[i]<fabs(v)) mMaxi[i]=fabs(v);
    if (v<0) mNega[i]++;
    v = v*p;
  }
}
//_____________________________________________________________________________
void QaFit::finish()
{
  if( mEnded) return;
  if(!mTally) return;
  mEnded = 1;
  int n = mTally;
  for (int i=0;i<2;i++) {
    mAver[i]/= n;
    mErrr[i]/= n;
    if (mErrr[i]<1e-10) mErrr[i]=1e-10;
    mErrr[i] = sqrt(mErrr[i]);
    if (!(--n)) break;
  }
}
//_____________________________________________________________________________
double QaFit::getAccu(int k)
{
  finish();
  if (mTally-k<=1) return 0;
  return mErrr[k];
}  
//_____________________________________________________________________________
double QaFit::getNStd(int k)
{
  finish();
  int n = mTally-k;
  if (n <= 0) return 0;
  return mAver[k]*sqrt(double(n));
}  
//_____________________________________________________________________________
double QaFit::getNSgn(int k)
{
  finish();
  int n = mTally-k;
  if (n <= 0) return 0;
  return (n-2*mNega[k])/sqrt(double(n));
}  
//_____________________________________________________________________________
void QaFit::getInfo(double *info)
{
  for (int i=0;i<2;i++) {
    int l = i*4;
    info[l+0]=getAccu(i);
    info[l+1]=getNStd(i);
    info[l+2]=getNSgn(i);
    info[l+3]=getMaxi(i);
  }
}  
  
  
  
