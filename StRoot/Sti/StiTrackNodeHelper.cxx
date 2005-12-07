#include <stdio.h>
#include <stdlib.h>
#include "StiUtilities/StiDebug.h"
#include "StiTrackNodeHelper.h"
#include "StiElossCalculator.h"
#include "TCL.h"

#define NICE(a) ( ((a) <= -M_PI)? ((a)+2*M_PI) :\
                  ((a) >   M_PI)? ((a)-2*M_PI) : (a))

#define sinX(a) StiTrackNode::sinX(a)
static const double kMaxEta = 1.5;
static const double kMaxCur = 0.2;

static const double DY=0.3,DZ=0.3,DEta=0.03,DRho=0.01,DTan=0.05;
static const double MAXSTEP[]={0,DY,DZ,DEta,DRho,DTan};
static const double MAXERRS[]={3,3,3,0.1,0.1,0.1};
static const double ERROR_FACTOR = 2.;

int StiTrackNodeHelper::mgCutStep=0;
static int errTest(StiNodeErrs &pred,StiHitErrs &hite,StiNodeErrs &fitd);
static double joinTwo(int nP,double *P1,double *E1,double *P2,double *E2,double *PJ=0,double *EJ=0);


//______________________________________________________________________________
void StiTrackNodeHelper::set(double chi2Max,double errConfidence,int iter)
{
  reset();
  mChi2Max = chi2Max;
  mErrConfiDefault = errConfidence;
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
//??  mErrConfidence = (mParentNode)? 0:mErrConfiDefault;
  mErrConfidence = mErrConfiDefault;
  mDetector   = mTargetNode->getDetector();
  if (!mDetector) mVertexNode = mTargetNode;
  mHit        = mTargetNode->getHit();
  if (!mIter) mTargetNode->mFlipFlop=0;
}
//______________________________________________________________________________
int StiTrackNodeHelper::propagateBest()
{
  int ierr = 0;
  alpha = mTargetNode->_alpha - mParentNode->_alpha;
  ca=1;sa=0;
  mBestParentPars.check("1propagateBest");
  mBestParentRotPars = mBestParentPars;
  if (fabs(alpha) > 1.e-6) { //rotation part

    double xt1=mBestParentPars._x; 
    double yt1=mBestParentPars._y; 
    double cosCA0 = mBestParentPars._cosCA;
    double sinCA0 = mBestParentPars._sinCA;

    ca = cos(alpha);
    sa = sin(alpha);

    mBestParentRotPars._x = xt1*ca + yt1*sa;
    mBestParentRotPars._y= -xt1*sa + yt1*ca;
    mBestParentRotPars._cosCA =  cosCA0*ca+sinCA0*sa;
    mBestParentRotPars._sinCA = -cosCA0*sa+sinCA0*ca;
    double nor = 0.5*(mBestParentRotPars._sinCA*mBestParentRotPars._sinCA+mBestParentRotPars._cosCA*mBestParentRotPars._cosCA +1);
    mBestParentRotPars._cosCA /= nor;
    mBestParentRotPars._sinCA /= nor;
    mBestParentRotPars._eta= NICE(mBestParentPars._eta-alpha); 
  }// end of rotation part
  ierr = mBestParentRotPars.check();
  if (ierr) return 1;
  
//  	Propagation 
  x1 = mBestParentRotPars._x;
  x2 = (mDetector)? mDetector->getPlacement()->getNormalRadius():mHit->x();
  dx = x2-x1;
  rho = mBestParentRotPars._curv;
  dsin = rho*dx;
  sinCA2=mBestParentRotPars._sinCA + dsin; 
  if (sinCA2> 0.95) sinCA2= 0.95;
  if (sinCA2<-0.95) sinCA2=-0.95;
  cosCA2 = ::sqrt((1.-sinCA2)*(1.+sinCA2));
  sumSin   = mBestParentRotPars._sinCA+sinCA2;
  sumCos   = mBestParentRotPars._cosCA+cosCA2;
  dy = dx*(sumSin/sumCos);
  y2 = mBestParentRotPars._y+dy;
  dl0 = mBestParentRotPars._cosCA*dx+mBestParentRotPars._sinCA*dy;
  sind = dl0*rho;
  if (fabs(dsin) < 0.02 && mBestParentRotPars._cosCA >0) { //tiny angle
    dl = dl0*(1.+sind*sind/6);
  } else {
    double cosd = cosCA2*mBestParentRotPars._cosCA+sinCA2*mBestParentRotPars._sinCA;
    dl = atan2(sind,cosd)/rho;
  }

  mBestPars._x = x2;
  mBestPars._y = y2;
  mBestPars._z= mBestParentRotPars._z + dl*mBestParentRotPars._tanl;
  mBestPars._eta = (mBestParentRotPars._eta+rho*dl);  					
  mBestPars._eta = NICE(mBestPars._eta);  					
  mBestPars._curv = mBestParentRotPars._curv;
  mBestPars._tanl = mBestParentRotPars._tanl;
  mBestPars._sinCA   = sinCA2;
  mBestPars._cosCA   = cosCA2;
  ierr = mBestPars.check();
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
   double *m = &mMtx.A[StiNodePars::kCurv][StiNodePars::kCurv];
   *m+= (*m+1)*mMcs._curvCorr;
   mBestPars._curv *= (1+mMcs._curvCorr);
#if 1
   double diff[kNPars];
   for (int j=0;j<kNPars;j++) {diff[j]=mFitdParentPars.P[j]-mBestParentPars.P[j];}
   
   for (int i=0;i<kNPars;i++) {
     double s = 0;
     for (int j=0;j<kNPars;j++) {
       if(!mMtx.A[i][j]) continue;
       s += mMtx.A[i][j]*diff[j];
     }
     mPredPars.P[i] = mBestPars.P[i]+s;
   }
#endif//0
#if 0
   StiNodePars sBestPars 		= mBestPars;
   StiNodePars sBestParentPars		= mBestParentPars;
   StiNodePars sBestParentRotPars	= mBestParentRotPars;
   mBestParentPars 			= mFitdParentPars;
   ierr = propagateBest();
   mPredPars = mBestPars;
   mPredPars._curv *= (1+mMcs._curvCorr);

   mBestPars 		= sBestPars;
   mBestParentPars	= sBestParentPars;
   mBestParentRotPars	= sBestParentRotPars;
#endif//1
   cutStep(&mBestPars,&mPredPars);
   mPredPars.ready();
   ierr = mPredPars.check();
   if (ierr) return 1;
   return 0;
}



//______________________________________________________________________________
int StiTrackNodeHelper::propagateMtx()
{
//  	fYE == dY/dEta
  double fYE= dx*(1.+mBestParentRotPars._cosCA*cosCA2+mBestParentRotPars._sinCA*sinCA2)/(sumCos*cosCA2);
//	fEC == dEta/dRho
  double fEC = dx/cosCA2;
//	fYC == dY/dRho
  double fYC=(dl0)/sumCos*fEC;
//	fZE == dZ/dEta
  double dLdEta = dy/cosCA2;
  double fZE =  mBestPars._tanl*dLdEta;

// 	fZC == dZ/dRho
  double dang = dl*rho;
  double C2LDX = dl*dl*(
               0.5*sinCA2*pow((1+pow(dang/2,2)*sinX(dang/2)),2) +
                   cosCA2*dang*sinX(dang));

  double fZC = mBestPars._tanl*C2LDX/cosCA2;

//  	fZT == dZ/dTanL; 
  double fZT= dl; 

  
  mMtx.reset();
//  X related derivatives
  mMtx.A[0][0] = -1;
  mMtx.A[1][0] = -sinCA2/cosCA2; 
  mMtx.A[2][0] = -mBestPars._tanl/cosCA2 ;
  mMtx.A[3][0] = -mBestPars._curv/cosCA2 ;       ;

  mMtx.A[1][3]=fYE; mMtx.A[1][4]=fYC; mMtx.A[2][3]=fZE;
  mMtx.A[2][4]=fZC; mMtx.A[2][5]=fZT; mMtx.A[3][4]=fEC;
  double fYX = mMtx.A[1][0]; 
  mMtx.A[1][0] = fYX*ca-sa;
  mMtx.A[1][1] = fYX*sa+ca-1;
  return 0;
}
//______________________________________________________________________________
int StiTrackNodeHelper::propagateError()
{
  mPredErrs = mFitdParentErrs;
  StiTrackNode::errPropag6(mPredErrs.A,mMtx.A,kNPars);
  mPredErrs.recov(MAXERRS);
  mPredErrs._cEE+=mMcs._cEE;		//add err to <eta*eta> eta crossing angle//add err to <eta*eta> eta crossing angle
  mPredErrs._cCC+=mMcs._cCC;    	//add err to <curv*curv>		 //add err to <curv*curv>
  mPredErrs._cTC+=mMcs._cTC;    	//add err to <tanL*curv>		 //add err to <tanL*curv>
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

    ierr = propagateBest();	if(ierr) return 1;
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
    mgCutStep = 0;
    mPredErrs = mTargetNode->mFE;
    ierr = mPredErrs.check(); 	if (ierr) return 11;
    mPredPars = mTargetNode->mFP;
    ierr = mPredPars.check();	if (ierr) return 12;
    mBestPars = mPredPars;
    mBestDelta = mPredErrs.getDelta();
    mJoinPars = mPredPars;
    resetError(1./(mErrConfidence));
  }
// 		Set fitted pars to predicted for the absent hit case
  mFitdPars = mPredPars;
  mFitdErrs = mPredErrs;

  int ians = 1;
  mChi2 =0;
  do {//technical (fake) loop
    if (!mHit) 		break;
    setHitErrs();
    if (nudge())			  return 13;
    mChi2 = 3e33;
    double chi2 = evalChi2();
    if (chi2>mChi2Max && mTargetNode != mVertexNode)	break;
    mChi2 = chi2; if (mChi2>999) mChi2=999;

    ians = updateNode();

    if (!ians) 	break;
    mState = StiTrackNode::kTNReady;
    mFitdPars = mPredPars;
    mFitdErrs = mPredErrs;
    mChi2 = 3e33;
  }while(0);
  ierr  = (!smooth)? save():join();
  if (ierr) return 13;


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
    accu = mJoinPars._curv - mBestParentPars._curv*(1+mMcs._curvCorr);
    errr = sqrt(0.5*(fabs(mJoinErrs._cCC)+fabs(mBestParentErrs._cCC)));
    mCurvQa.add(accu/errr);    
    accu = mJoinPars._tanl - mBestParentPars._tanl;
    errr = sqrt(0.5*(fabs(mJoinErrs._cTT)+fabs(mBestParentErrs._cTT)));
    mTanlQa.add(accu/errr);    
  }while(0);
    
  return 0;
}  
  
//______________________________________________________________________________
int StiTrackNodeHelper::join()
{
  enum {kOLdValid=1,kNewFitd=2,kJoiUnFit=4};
  if (!mParentNode) return 0;
  int ierr = 0;
  double chi2;		
  
  int kase = mTargetNode->isValid();
  if (mState==StiTrackNode::kTNFitEnd) kase |=kNewFitd;
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

        chi2 = joinTwo(kNPars,mTargetNode->mPP().P,mTargetNode->mPE().A
                             ,mFitdPars.P         ,mFitdErrs.A
                             ,mJoinPars.P         ,mJoinErrs.A);
	mJoinErrs.recov();


	if (kase == (kOLdValid|kNewFitd)) {	//Check errors improvements
	  assert(mHrr.hYY > mJoinErrs._cYY);
	  assert(mHrr.hZZ > mJoinErrs._cZZ);
          if (mHrr.hYY <= mJoinErrs._cYY) {
            printf("StiTrackNodeHelper::updateNode() WRONG hYY(%g) < nYY(%g)\n"
                   ,mHrr.hYY,mFitdErrs._cYY);
            return -13;
	  }
          if (mHrr.hZZ <= mJoinErrs._cZZ) {
            printf("StiTrackNodeHelper::updateNode() WRONG hZZ(%g) < nZZ(%g)\n"
                  ,mHrr.hZZ,mFitdErrs._cZZ);
            return -14;
	  }
  } //End  Check errors improvements
	mJoinPars.ready();
	ierr = mJoinPars.check(); 	if (ierr) return 2;
	if (kase!=(kOLdValid|kNewFitd)) {kase = -1; break;}		

        mChi2 = 3e33;
	chi2 = joinChi2();		//Test join Chi2
        if (chi2>mChi2Max && mTargetNode!=mVertexNode) 
	   				{ kase |=kJoiUnFit; break;} //join Chi2 too big
        mChi2 = (chi2>999)? 999:chi2;
        mState = StiTrackNode::kTNFitEnd;
        kase = -1; break;
	
       default: assert(0);
     }//end Switch
  } while(kase>=0);

  mTargetNode->mFE   = mJoinErrs;
  mTargetNode->mPE() = mJoinErrs;
  mTargetNode->mFP   = mJoinPars;
  mTargetNode->mPP() = mJoinPars;
  mTargetNode->mUnTouch = mUnTouch;
  mTargetNode->_state = mState;
  if (mHit && ((mChi2<1000) != (mTargetNode->getChi2()<1000))) mTargetNode->mFlipFlop++;
  mTargetNode->setChi2(mChi2);

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
double joinTwo(int nP,double *P1,double *E1,double *P2,double *E2,double *PJ,double *EJ)
{

  int nE = nP*(nP+1)/2;
  TArrayD ard(nE*3+nP*3);
  double *sumE 		= ard.GetArray();
  double *sumEI 	= sumE	+nE;
  double *qwe 		= sumEI	+nE;
  double *subP 		= qwe	+nE;
  double *sumEIsubP	= subP	+nP;
  double chi2=3e33;

// Choose the smalest errors
  double *p1 = P1, *p2 = P2, *e1 = E1, *e2 = E2;
  double choice =0;
  for (int i=0,n=1;i<nE;i+=(++n)) {choice += (e1[i]-e2[i])/(e1[i]+e2[i]+1e-10); }
  if (choice >0) {double *t = p2; p2 = p1; p1 = t; t = e2; e2 = e1; e1 = t;}

  do {//empty loop
//  	Join errors
    TCL::vadd(e1,e2,sumE,nE);
    int ign0re = sumE[0]<=0;
    if (ign0re) sumE[0] = 1;
    TCL::trsinv(sumE,sumEI,nP);
    if (ign0re) {sumE[0]  = 0; sumEI[0] = 0;}
    TCL::vsub(p2       ,p1   ,subP       ,nP);
    TCL::trasat(subP,sumEI,&chi2,1,nP); 
    if (!EJ) break;
    TCL::trqsq (e1  ,sumEI,qwe,nP); 
    TCL::vsub(e1,qwe,EJ,nE);
  } while(0);
//  	Join params
  if (PJ) {
    TCL::tras(subP     ,sumEI,sumEIsubP,1,nP);
    TCL::tras(sumEIsubP,e1   ,PJ       ,1,nP);
    TCL::vadd(PJ       ,p1   ,PJ         ,nP);
  }
  return chi2;
}
//______________________________________________________________________________
int StiTrackNodeHelper::save()
{
   mTargetNode->mPP() = mPredPars;
   mTargetNode->mFP   = mFitdPars;
   mTargetNode->mPE() = mPredErrs;
   mTargetNode->mFE   = mFitdErrs;
   mTargetNode->_state = mState;
   if (mHit && ((mChi2<1000) != (mTargetNode->getChi2()<1000))) mTargetNode->mFlipFlop++;
   mTargetNode->setChi2(mChi2);
   return 0;
}
//______________________________________________________________________________
/*! Calculate the effect of MCS on the track error matrix.
  <p>
  The track is assumed to propagate from (x0,y0,z0) to (x1,y1,z1). The calculation
  is performed for the given mass hypothesis which given a momentum determines the
  speed "beta" of the particle. The calculation of the average scattering angle
  is delegated to the function "mcs2". The calculation of energy loss is done
  by the function eloss.
 */
//______________________________________________________________________________
int StiTrackNodeHelper::propagateMCS()
{  
  mMcs.reset();
  if (!mDetector) 	return 0;

  double relRadThickness;
  // Half path length in previous node
  double pL1,pL2,pL3,d1,d2,d3,dxEloss,dx;
  pL1=0.5*pathIn(mParentNode->getDetector(),&mBestParentPars);
  // Half path length in this node
  pL3=0.5*pathIn(mDetector,&mBestPars);
  // Gap path length
  pL2= fabs(dl);
  double x0p =-1;
  double x0Gas=-1;
  double x0=-1;
  dx = mBestPars._x - mBestParentRotPars._x;
  d1    = mParentNode->getDensity();
  x0p   = mParentNode->getX0();
  d3    = mDetector->getMaterial()->getDensity();
  x0    = mDetector->getMaterial()->getX0();


  if (pL2> (pL1+pL3)) {

      pL2=pL2-pL1-pL3;
      if (dx>0)  {
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
  double pt     = mParentNode->getPt();
  double tanl   = mBestPars._tanl;
  double rho    = mBestPars._curv; 
  double p2     = (1.+tanl*tanl)*pt*pt;
  double m      = StiKalmanTrackNode::pars->getMassHypothesis();
  double m2     = m*m;
  double e2     = p2+m2;
  double beta2  = p2/e2;
  double theta2 = StiKalmanTrackNode::mcs2(relRadThickness,beta2,p2);
  double cos2Li = (1.+ tanl*tanl);  // 1/cos(lamda)**2
  double f = 1./(1-mErrConfidence); 
  mMcs._cEE = cos2Li 		*theta2*f;
  mMcs._cCC = tanl*tanl*rho*rho	*theta2*f;
  mMcs._cTC = rho*tanl*cos2Li	*theta2*f;
  mMcs._cTT = cos2Li*cos2Li	*theta2*f;

  double dE=0;
  double sign = (dx>0)? 1:-1;

//  const static double I2Ar = (15.8*18) * (15.8*18) * 1e-18; // GeV**2
  StiElossCalculator * calculator = mDetector->getElossCalculator();
  double eloss = calculator->calculate(1.,m, beta2);
  dE = sign*dxEloss*eloss;

  mMcs._curvCorr = ::sqrt(e2)*dE/p2;
  if (fabs(mMcs._curvCorr)>0.1) mMcs._curvCorr = (dE<0)? -0.1:0.1;
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
  double r00, r01,r11;
  //If required, recalculate the errors of the detector hits.
  //Do not attempt this calculation for the main vertex.
  if (fabs(mPredPars._sinCA)>0.99        )	return 1e41;
  if (fabs(mPredPars._eta)       >kMaxEta) 	return 1e41;
  if (fabs(mPredPars._curv)      >kMaxCur)      return 1e41;

  r00=mPredErrs._cYY+mHrr.hYY;
  r01=mPredErrs._cZY+mHrr.hZY;
  r11=mPredErrs._cZZ+mHrr.hZZ;
  mDetm = r00*r11 - r01*r01;
  if (mDetm<r00*r11*1.e-5) {
    printf("StiTrackNodeHelper::evalChi2 *** zero determinant %g\n",mDetm);
    return 1e60;
  }
  double tmp=r00; r00=r11; r11=tmp; r01=-r01;  

  double dyt=(mPredPars._y-mHit->y());
  double dzt=(mPredPars._z-mHit->z());
  double chi2 = (dyt*r00*dyt + 2*r01*dyt*dzt + dzt*r11*dzt)/mDetm;
  return chi2;
}
//______________________________________________________________________________
double StiTrackNodeHelper::joinChi2() 
{
  double chi2;
  double mergPars[3],hitPars[3],mergErrs[6];
  chi2 = joinTwo(3,mPredPars.P         ,mPredErrs.A
                  ,mTargetNode->mPP().P,mTargetNode->mPE().A
                  ,mergPars            ,mergErrs);
  
  hitPars[0] = mHit->x();
  hitPars[1] = mHit->y();
  hitPars[2] = mHit->z();
  chi2 = joinTwo(3,mergPars,mergErrs,hitPars,mHrr.A);
// 	Save untouched by current hit node's y,z & errors for alignment
  mUnTouch.mPar[0] = mergPars[1];
  mUnTouch.mPar[1] = mergPars[2];
  mUnTouch.mErr[0] = mergErrs[2];
  mUnTouch.mErr[1] = mergErrs[4];
  mUnTouch.mErr[2] = mergErrs[5];
  return chi2;
}
//______________________________________________________________________________
int StiTrackNodeHelper::setHitErrs() 
{
  mHrr.reset();
  if (mDetector) {
    mHrr.hYY = mTargetNode->eyy ;
    mHrr.hZZ = mTargetNode->ezz ;
    mHrr.hZY = 0;
  } else { 
    mHrr.hYY = 1e-4;
    mHrr.hZZ = 1e-4;
    mHrr.hZY = 0;
  }
  if (mErrConfidence<=0) return 0;
  double hitErrorFactor = 1./(1.-mErrConfidence);
  for (int i=0;i<6;i++) {mHrr.A[i]*=hitErrorFactor;}

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
  assert(mPredErrs._cXX<1e-8);
  double r00,r01,r11;
  r00=mHrr.hYY+mPredErrs._cYY;
  r01=mHrr.hZY+mPredErrs._cZY;
  r11=mHrr.hZZ+mPredErrs._cZZ;
  mDetm=r00*r11 - r01*r01;
  assert(mDetm>(r00*r11)*1.e-5);

  // inverse matrix
  double tmp=r00; r00=r11/mDetm; r11=tmp/mDetm; r01=-r01/mDetm;
  // update error matrix
  double k00=mPredErrs._cYY*r00+mPredErrs._cZY*r01, k01=mPredErrs._cYY*r01+mPredErrs._cZY*r11;
  double k10=mPredErrs._cZY*r00+mPredErrs._cZZ*r01, k11=mPredErrs._cZY*r01+mPredErrs._cZZ*r11;
  double k20=mPredErrs._cEY*r00+mPredErrs._cEZ*r01, k21=mPredErrs._cEY*r01+mPredErrs._cEZ*r11;
  double k30=mPredErrs._cCY*r00+mPredErrs._cCZ*r01, k31=mPredErrs._cCY*r01+mPredErrs._cCZ*r11;
  double k40=mPredErrs._cTY*r00+mPredErrs._cTZ*r01, k41=mPredErrs._cTY*r01+mPredErrs._cTZ*r11;

  double myY = mPredPars._y;
  double myZ = mPredPars._z;
  double dyt  = mHit->y() - myY;
  double dzt  = mHit->z() - myZ;
  double dCu  = k30*dyt + k31*dzt;
  double dEt  = k20*dyt + k21*dzt;
  double dTa  = k40*dyt + k41*dzt;
  double eta  = NICE(mPredPars._eta + dEt);
  double cur  = mPredPars._curv + dCu;
  double tanl = mPredPars._tanl + dTa;
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

  mFitdPars._x  = mPredPars._x;
  mFitdPars._y  = p0;
  mFitdPars._z  = p1;
  mFitdPars._eta   = eta;
  mFitdPars._curv  = cur;
  mFitdPars._tanl  = tanl;
  mFitdPars._sinCA = sinCA;
  mFitdPars._cosCA = ::sqrt((1.-mFitdPars._sinCA)*(1.+mFitdPars._sinCA)); 
  cutStep(&mFitdPars,&mPredPars);
  cutStep(&mFitdPars,&mBestPars);
  if (mFitdPars.check()) return -11;
// update error matrix
  double c00=mPredErrs._cYY;                       
  double c10=mPredErrs._cZY, c11=mPredErrs._cZZ;                 
  double c20=mPredErrs._cEY, c21=mPredErrs._cEZ;//, c22=mPredErrs._cEE;           
  double c30=mPredErrs._cCY, c31=mPredErrs._cCZ;//, c32=mPredErrs._cCE, c33=mPredErrs._cCC;     
  double c40=mPredErrs._cTY, c41=mPredErrs._cTZ;//, c42=mPredErrs._cTE, c43=mPredErrs._cTC, c44=mPredErrs._cTT;
  mFitdErrs._cYY-=k00*c00+k01*c10;
  mFitdErrs._cZY-=k10*c00+k11*c10;mFitdErrs._cZZ-=k10*c10+k11*c11;
  mFitdErrs._cEY-=k20*c00+k21*c10;mFitdErrs._cEZ-=k20*c10+k21*c11;mFitdErrs._cEE-=k20*c20+k21*c21;
  mFitdErrs._cCY-=k30*c00+k31*c10;mFitdErrs._cCZ-=k30*c10+k31*c11;mFitdErrs._cCE-=k30*c20+k31*c21;mFitdErrs._cCC-=k30*c30+k31*c31;
  mFitdErrs._cTY-=k40*c00+k41*c10;mFitdErrs._cTZ-=k40*c10+k41*c11;mFitdErrs._cTE-=k40*c20+k41*c21;mFitdErrs._cTC-=k40*c30+k41*c31;mFitdErrs._cTT-=k40*c40+k41*c41;
  mFitdErrs.recov(MAXERRS);
  
  if (mFitdErrs.check()) return -12;

static int ERRTEST=0;
  if(ERRTEST) errTest(mPredErrs,mHrr,mFitdErrs);

//prod  assert(mHrr.hYY > mFitdErrs._cYY);
//prod  assert(mHrr.hZZ > mFitdErrs._cZZ);
  if (mHrr.hYY <= mFitdErrs._cYY) {
    printf("StiTrackNodeHelper::updateNode() WRONG hYY(%g) < nYY(%g)\n"
          ,mHrr.hYY,mFitdErrs._cYY);
    return -13;
  }  
  if (mHrr.hZZ <= mFitdErrs._cZZ) {
    printf("StiTrackNodeHelper::updateNode() WRONG hZZ(%g) < nZZ(%g)\n"
          ,mHrr.hZZ,mFitdErrs._cZZ);
    return -14;
  }  
	  
  
  mState = StiTrackNode::kTNFitEnd;
  return 0; 
}
 
//______________________________________________________________________________
void StiTrackNodeHelper::resetError(double fk)
{ 
  if (fk) do {//fake loop
    mPredErrs*=fk;
    if(mPredErrs._cYY>DY*DY) 		break;
    if(mPredErrs._cZZ>DZ*DZ) 		break;
    if(mPredErrs._cEE>DEta*DEta) 	break;
    if(mPredErrs._cCC>DRho*DRho) 	break;
    if(mPredErrs._cTT>DTan*DTan) 	break;
    return;
  }while(0);

  mPredErrs.reset();
  mPredErrs._cYY=DY*DY;
  mPredErrs._cZZ=DZ*DZ;
  mPredErrs._cEE=DEta*DEta;
  mPredErrs._cCC=DRho*DRho;
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
  if(!mHit) return 0;
  StiNodePars *pars = &mBestPars;
  for (int i=0;i<2;i++,pars =&mPredPars) {
    double deltaX = mHit->x()-pars->_x;
    if (fabs(deltaX) <1e-6) continue;
    double deltaL = deltaX/pars->_cosCA;
    double deltaE = pars->_curv*deltaL;
    pars->_x      = mHit->x();
    pars->_y     += pars->_sinCA *deltaL;
    pars->_z     += pars->_tanl  *deltaL;
    pars->_eta   +=               deltaE;
    pars->_cosCA -= pars->_sinCA *deltaE;
    pars->_sinCA += pars->_cosCA *deltaE;
    if (pars->_cosCA>=1.) pars->ready();
    if (pars->check()) return 1;
  }
  return 0;
}
//______________________________________________________________________________
double StiTrackNodeHelper::pathIn(const StiDetector *det,StiNodePars *pars)
{
  if (!det) return 0.; 
  double thickness = det->getShape()->getThickness();
  double t = pars->_tanl;
  double c = pars->_cosCA;
  return (thickness*::sqrt(1.+t*t)) / c;
}
//______________________________________________________________________________
int errTest(StiNodeErrs &pred,StiHitErrs &hite,StiNodeErrs &fitd)
{

  pred._cXX = 1;
  StiNodeErrs predI; 
  TCL::trsinv(pred.A,predI.A,kNPars);
  pred._cXX = 0;predI._cXX = 0;

  hite.hXX = 1;
  StiHitErrs hiteI;
  TCL::trsinv(hite.A,hiteI.A,3);
  hite.hXX = 0;hiteI.hXX=0;

  StiNodeErrs sumI = predI;
  TCL::vadd(sumI.A,hiteI.A,sumI.A,6);
  sumI._cXX = 1;
  StiNodeErrs sum;
  TCL::trsinv(sumI.A,sum.A,kNPars);
  sum._cXX = 0;

  int ndif = 0;
  for (int i=0;i<kNErrs;i++) {
    double diff = fabs(sum.A[i]-fitd.A[i]);
    if (diff < 1e-10) continue;
    diff/=0.5*(fabs(sum.A[i])+fabs(fitd.A[i]));
    if (diff < 1e-5 ) continue;
    ndif++;
    printf("errTest: %g(%d) - %g(%d) = %g\n",sum.A[i],i,fitd.A[i],i,diff);
  }
  if (!ndif) return 0;
  sum.print();
  fitd.print();
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
  if (!n) return 0;
  return mAver[k]*sqrt(double(n));
}  
//_____________________________________________________________________________
double QaFit::getNSgn(int k)
{
  finish();
  int n = mTally-k;
  if (!n) return 0;
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
  
  
  
