#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "TCernLib.h"
#include "TVector3.h"
#include "StvDefaultSeedFinder.h"
#include "StMultiKeyMap.h"
#include "StarVMC/GeoTestMaker/StTGeoProxy.h"
#include "Stv/StvHit.h"
#include "THelixTrack.h"
//#define APPROX_DEBUG
#ifdef APPROX_DEBUG
#include "TCanvas.h"
#include "TH1F.h"
#include "TProfile.h"
#endif //APPROX_DEBUG
#include "StvSeedConst.h"
#include "Stv/StvConst.h"
#include "StvUtil/StvDebug.h"
#include "Stv/StvDraw.h"


void myBreak(int);
enum {kFstAng=65,kErrFakt=4,kLenFakt=4};
enum {kPhi=0,kRxy=1,kTanL=2,kZ=3};
static const double kFstTan = tan(kFstAng*M_PI/180);
static const double kMinTan = 0.1;

ClassImp(StvDefaultSeedFinder)

//_____________________________________________________________________________
StvDefaultSeedFinder::StvDefaultSeedFinder(const char *name):StvSeedFinder(name)
{
  memset(mBeg,0,mEnd-mBeg+1);
  fMultiHits	= new StMultiKeyMap(kNKeys);
  fMultiIter	= new StMultiKeyMapIter(0);
  f1stHitMap 	= new Stv1stHitMap;
  f1stHitMapIter= new Stv1stHitMapIter;
}  
//_____________________________________________________________________________
void StvDefaultSeedFinder::Clear(const char*)
{
  memset(mBeg,0,mMed-mBeg+1);
  f1stHitMap->clear();
  fMultiHits->Clear();
  *f1stHitMapIter = f1stHitMap->end();
  StvSeedFinder::Clear();
}
//_____________________________________________________________________________
void StvDefaultSeedFinder::Reset()
{
  memset(mBeg,0,mMed-mBeg+1);
  const StVoidArr *hitArr =  StTGeoProxy::Inst()->GetSeedHits();
  int nHits =  hitArr->size();
  for (int iHit=0;iHit<nHits;iHit++) {
    StvHit *stiHit = (StvHit*)(*hitArr)[iHit];
    if (stiHit->timesUsed()) continue;
    const float *x = stiHit->x();
//    float r2 = x[0]*x[0] + x[1]*x[1]+ x[2]*x[2];
    float r2 = x[0]*x[0] + x[1]*x[1] + 1e-2*x[2]*x[2];
    f1stHitMap->insert(std::pair<float,StvHit*>(-r2, stiHit));

#ifdef MultiPhiZMap
    float keys[kNKeys];
    keys[kPhi ] = atan2(x[1],x[0]);
    keys[kRxy ] = sqrt(x[0]*x[0] + x[1]*x[1]+ x[2]*x[2]);
    keys[kTanL] = x[2]/keys[kRxy];
    keys[kZ   ] = x[2];
    fMultiHits->Add(stiHit,keys);
#else
    fMultiHits->Add(stiHit,x);
#endif
  } 
   fMultiHits->MakeTree();
  *f1stHitMapIter = f1stHitMap->begin();
}    
//_____________________________________________________________________________
int StvDefaultSeedFinder::Again(int repeat)
{
  *f1stHitMapIter = f1stHitMap->begin();
   mNDejavu = 0;
   if (!repeat) return 1;
   Clear();
   Reset();
   return 1;
}
//_____________________________________________________________________________
//	Start of Local auxiliary routines
inline static void Lagrange3Int (float t,float T1,float T2,float coe[3])
{
   coe[0]=  (t-T1)*(t-T2)/(T1*(T2   )); 
   coe[1]= -(t   )*(t-T2)/(T1*(T2-T1)); 
   coe[2]=  (t   )*(t-T1)/(T2*(T2-T1));
}
//_____________________________________________________________________________
inline static void ZLine3Int (float t,float T1,float T2,float coe[3])
{
  float aT  = (T1   +T2   )/3;
  float aTT = (T1*T1+T2*T2)/3;
  float det = aTT-aT*aT;
  coe[0] = (1. + (  -aT)*(t-aT)/det)/3;
  coe[1] = (1. + (T1-aT)*(t-aT)/det)/3;
  coe[2] = (1. + (T2-aT)*(t-aT)/det)/3;
}  

//_____________________________________________________________________________
inline static void Lagrange3Der (float t,float T1,float T2,float coe[3])
{
   coe[0]=  ((t-T1)+(t-T2))/(T1*(T2   )); 
   coe[1]= -((t   )+(t-T2))/(T1*(T2-T1)); 
   coe[2]=  ((t   )+(t-T1))/(T2*(T2-T1));
}
//_____________________________________________________________________________
inline static void ZLine3Der (float T1,float T2,float coe[3])
{
  float aT  = (T1   +T2   )/3;
  float aTT = (T1*T1+T2*T2)/3;
  float det = aTT-aT*aT;
  coe[0] = (  -aT)/det/3;
  coe[1] = (T1-aT)/det/3;
  coe[2] = (T2-aT)/det/3;
}  
//_____________________________________________________________________________
inline static float Dot(const float dir[3],const float pnt[3])
{
   return dir[0]*pnt[0]+dir[1]*pnt[1]+dir[2]*pnt[2];
}
//_____________________________________________________________________________
inline static void Mul(const float a[3],float scale,float b[3])
{
  b[0]=a[0]*scale;b[1]=a[1]*scale;b[2]=a[2]*scale;
}
//_____________________________________________________________________________
inline static float Impact2(const float dir[3],const float pnt[3])
{
   float imp[3];
   imp[0] = dir[1]*pnt[2]-dir[2]*pnt[1];
   imp[1] = dir[2]*pnt[0]-dir[0]*pnt[2];
   imp[2] = dir[0]*pnt[1]-dir[1]*pnt[0];
   return imp[0]*imp[0]+imp[1]*imp[1]+imp[2]*imp[2];
}
//	End of Local auxiliary routines
//_____________________________________________________________________________
const THelixTrack* StvDefaultSeedFinder::NextSeed()
{
static int nCall = 0; nCall++;
std::vector<TObject*> mySeedObjs;

  StvHit *fstHit,*selHit=0; 
  int nTally = 0; 
  while ((*f1stHitMapIter)!=f1stHitMap->end()) {//1st hit loop
    fstHit = (*(*f1stHitMapIter)).second;
    if (fstHit->timesUsed() || mNDejavu>=kNDejavu) {		//1st hit is used
      ++(*f1stHitMapIter); mNDejavu = 0; continue;
    }
    fSeedHits.clear();
    mSel.Reset();
    selHit = fstHit;
    mSel.SetErr(sqrt(fstHit->err2())*kErrFakt);

    while (1) { //Search next hit 
//		Add info from selected hit
      nTally++;
      fSeedHits.push_back(selHit); selHit->addTimesUsed();fNUsed[0]++;
      if (fSeedHits.size()>=kMaxHits) break;
      const StHitPlane *hp = selHit->detector();
      const float *hd = hp->GetDir(selHit->x())[0];
      mSel.AddHit(selHit->x(),hd,hp->GetLayer());
      mSel.Prepare();
      for (int iLim=0;iLim<2; iLim++) {
        if (iLim) memcpy(mSel.mLim[0],mSel.mLim[2],2*kNKeys*sizeof(mSel.mLim[0][0]));
        if (mSel.mLim[0][0]>1000) continue;
        fMultiIter->Set(fMultiHits->GetTop(),mSel.mLim[0],mSel.mLim[1]);
	selHit=0; 
	for (StMultiKeyNode *node=0;(node = *(*fMultiIter)) ;++(*fMultiIter)) 
	{ 
	  StvHit *nexHit = (StvHit*)node->GetObj();
	  if (nexHit->timesUsed()) 	continue;
	  const StHitPlane *hpNex = nexHit->detector();
	  if (hpNex==hp) 		continue;
	  int dejavu = 0;
	  for (int j=0;j<mNDejavu;j++) {
            if (mDejavu[j]!=nexHit) 	continue;
            dejavu = 2013; 		break;
	  }
	  if (dejavu) 			continue;
	  int ans = mSel.Reject(nexHit->x(),hpNex);
	  if (ans>0) 			continue;

   //			Selecting the best
	  selHit=nexHit;
	  if (!ans)  			continue;
   //		Decrease size of searching box
	  mSel.Update();
	  fMultiIter->Update(mSel.mLim[0],mSel.mLim[1]);
	} //endMultiIter loop

	if (!selHit) 			continue; //No more hits 
      }//end iLim loop
      if (!selHit) 			break; //No more hits 
    }// end NextHit loop
//		Mark hits as unused when seed is created. Only tracker
//		has right to deside to use or not to use
    fSeedHits.unused();
//		If no hits found, go to next 1st hit
    if ((int)fSeedHits.size()<=1) {mNDejavu = 99; continue;}

//		Store second hit to skip it next time
    mDejavu[mNDejavu++]=fSeedHits[1];
//	If too short seed go to next 1st hit
    if (fSeedHits.size() < kMinHits) 		  continue;


    const THelixTrack *hel = Approx();
    if (hel) { fNSeeds[0]++;;return hel;}		//Good boy
 //		Bad seed
    fNUsed[0] -= fSeedHits.size();
  }// end 1st hit loop
  fNSeeds[1]+=fNSeeds[0]; fNUsed[1]+= fNUsed[0];
  return 0;
}
//_____________________________________________________________________________
//_____________________________________________________________________________
StvConeSelector::StvConeSelector()
{
  memset(mBeg,0,mBeg-mBeg+1);
}
//_____________________________________________________________________________
void StvConeSelector::AddHit(const float *x,const float *dir,float layer)
{
  mMinPrj = 1.e11; mMinImp = 1.e11; mHp = 0;
  mX[++mJst]=x;
  mHit = x;
  mLayer = layer;
  mHitDir = dir;
  assert(mJst<100);
}
//_____________________________________________________________________________
void StvConeSelector::Prepare()
{
static int nCall=0; nCall++;
StvDebug::Break(nCall);

  float stp=0;
  int kase = mJst; if (kase>2) kase = 2;

  switch(kase) {
  
    case 0: {
      
static int myDir = 1;
if      ( myDir == 0) { Mul(mX[0]  ,-1./sqrt(Dot(mX[0],mX[0])),mDir);}   
else if ( myDir == 1) { Mul(mHitDir,-1.                       ,mDir);}
else                  { mDir[0]= mX[0][0]; mDir[1]= mX[0][1];mDir[2]= mX[0][2]/2;
                        Mul(mDir   ,-1./sqrt(Dot(mDir ,mDir )),mDir);}
      float sgn = Dot(mHit,mDir);
      assert(sgn<0);
      mS[0]=0;
      mTan = kFstTan;
    }; break;

    case 1: {
      stp=0;
      for (int i=0;i<3;i++) {mDir[i]=mHit[i]-mX[mJst-1][i]; stp+=mDir[i]*mDir[i];}
      stp = sqrt(stp );
      for (int i=0;i<3;i++) {mDir[i]/=stp;}
      mS[1]=stp;
      mTan = mErr/stp;
    }; break;


    case 2: {
      stp=0;
      for (int i=0;i<3;i++) {float qwe=mHit[i]-mX[mJst-1][i]; stp+=qwe*qwe;}
      stp = sqrt(stp );
      mS[mJst]=stp;
      mTan = mErr/(mS[mJst]+mS[mJst-1])/sqrt(3.);
      float T1 = mS[mJst-1],T2 = mS[mJst]+T1;
      float coe[3];
      Lagrange3Der (T2,T1,T2,coe);

      mDir[0] = coe[0]*mX[mJst-2][0]+coe[1]*mX[mJst-1][0]+coe[2]*mHit[0];
      mDir[1] = coe[0]*mX[mJst-2][1]+coe[1]*mX[mJst-1][1]+coe[2]*mHit[1];
      mDir[2] = coe[0]*mX[mJst-2][2]+coe[1]*mX[mJst-1][2]+coe[2]*mHit[2];
      stp=0;
      for (int i=0;i<3;i++) {stp+=mDir[i]*mDir[i];}
      stp = sqrt(stp );
      for (int i=0;i<3;i++) {mDir[i]/=stp;}
    }; break;

    default: assert(0 && "Wrong case");
  }
assert(fabs(Dot(mDir,mDir)-1)<1e-5);
  mRxy2 = mHit[0]*mHit[0]+mHit[1]*mHit[1];
  mRxy = sqrt(mRxy2);
  mDelta = SEED_ERR(mRxy);
  mLen= mLayer*kLenFakt/(fabs(Dot(mHitDir,mDir))+1e-10);
  if (mLen>100) mLen = 100;
  UpdateLims();

}   
#ifndef MultiPhiZMap
//_____________________________________________________________________________
void  StvConeSelector::UpdateLims()
{
  mLim[2][0]=1e11;
  for (int i=0;i<3;i++) {
    float qwe = mLen*mDir[i];
    float asd = mLen*mTan*sqrt(fabs(1-mDir[i]*mDir[i]));
    float lim = qwe - asd - mErr;
    mLim[0][i] = (lim<0)? lim:-mErr;
    lim = qwe + asd + mErr;
    mLim[1][i] = (lim>0)? lim: mErr;
//		Move to global system 
    mLim[0][i]+= mHit[i];
    mLim[1][i]+= mHit[i];
  }

//		Account that all the hits inside of cylinder with Rxy
  for (int i=0;i<2;i++) {
    if (mLim[0][i]<-mRxy) mLim[0][i]=-mRxy;
    if (mLim[1][i]> mRxy) mLim[1][i]= mRxy;
  }

}
#endif
#ifdef MultiPhiZMap
//_____________________________________________________________________________
void  StvConeSelector::UpdateLims()
{
static int nCall=0; nCall++;
StvDebug::Break(nCall);
struct myLim_t {double Phi,Rxy,tanL,Z;};
//static const StvConst  *kons = StvConst::Inst();
static const double kMyMax = 220;

   TVector3 vHit(mHit);
   TVector3 nT(mDir);
   TVector3 nP(nT.Orthogonal());
   TVector3 nL(nT.Cross(nP));
   TVector3 Pnt[100];
   enum {kPNT = sizeof(Pnt)/sizeof(Pnt[0])};

   for (int ix=0;ix<3;ix++) {
     if (vHit[ix]+nT[ix]*mLen > kMyMax) {mLen = ( kMyMax-1-vHit[ix])/nT[ix];}
     if (vHit[ix]+nT[ix]*mLen <-kMyMax) {mLen = (-kMyMax+1-vHit[ix])/nT[ix];}
   }
   TVector3 vBas(vHit+nT*mLen);
   int nPnt=0;
   Pnt[nPnt++]=vHit;
   double myRad = mLen*mTan*sqrt(2.);
   for (int i=-1;i<2;i+=2) {
     Pnt[nPnt++]=vBas+nP*(i*myRad);
     Pnt[nPnt++]=vBas+nL*(i*myRad);
     assert(nPnt<=kPNT);
   }  

//	account points out of volume.
   TVector3* vHB[2]={&vHit,&vBas}; 
   int nPnt1st = nPnt;
   for (int ip=1;ip<nPnt1st;ip++) {//points loop
     TVector3 &P = Pnt[ip];
     double alfa,ALFA[2]={1,1};
     for (int ix=0;ix<3;ix++) { //account too big X,Y,Z
       double lim = 0;
            if (P[ix] >  kMyMax) {lim =  kMyMax-1;}
       else if (P[ix] < -kMyMax) {lim = -kMyMax+1;}
       if (!lim) continue;
       for (int jHB=0;jHB<2;jHB++) {
         alfa = (lim-(*vHB[jHB])[ix])/(P[ix]-(*vHB[jHB])[ix]);
         if (alfa<0.01) alfa=0.01;
         assert(alfa>=0 && alfa<1); 
         if (ALFA[jHB]>alfa) ALFA[jHB]=alfa;
     } } //End ix loop

     if (ALFA[0]<1) {Pnt[nPnt++] = P*ALFA[1] + *vHB[1]*(1-ALFA[1]);
                     P           = P*ALFA[0] + *vHB[0]*(1-ALFA[0]);}
//		Now upp and down in Y (to avoid problems with Phi = +-Pi)
     TVector3 *pHB[2]={Pnt+ip,Pnt+ip}; 
     if (ALFA[0]<1) pHB[1] = Pnt+nPnt-1;
     for (int jHB=0;jHB<2;jHB++) {
       if ((vHB[jHB]->y()<0)==(pHB[jHB]->y()<0)) continue;
       alfa = -vHB[jHB]->y()/(pHB[jHB]->y()-vHB[jHB]->y());
       for (;1;alfa*=0.99) {
         Pnt[nPnt] = (*pHB[jHB])*alfa + (*vHB[jHB])*(1-alfa);
         if ((vHB[jHB]->y()<0)==(Pnt[nPnt].y()<0)) break;
       } //end alfa loop
       nPnt++;  assert(nPnt<kPNT); 
     }//end jHB loop

   }// end of point loop



   myLim_t Dow[2]={{ 1e11, 1e11, 1e11, 1e11},{ 1e11, 1e11, 1e11, 1e11}};
   myLim_t Upp[2]={{-1e11,-1e11,-1e11,-1e11},{-1e11,-1e11,-1e11,-1e11}};
   for (int ix=0;ix<nPnt;ix++) {
     TVector3 &pnt= Pnt[ix];
     double phi = pnt.Phi();
     int jk = (phi <0)? 0:1;
     myLim_t &dow = Dow[jk];
     myLim_t &upp = Upp[jk];
     if (dow.Phi>phi) 		dow.Phi=phi;
     if (upp.Phi<phi) 		upp.Phi=phi;
     double rxy = pnt.Perp();
     if (dow.Rxy>rxy) 		dow.Rxy=rxy;
     if (upp.Rxy<rxy) 		upp.Rxy=rxy;
     double z = pnt.Z();
     if (dow.Z>z) 		dow.Z=z;
     if (upp.Z<z) 		upp.Z=z;
     double tanL = z/rxy;
     if (dow.tanL>tanL) 	dow.tanL=tanL;
     if (upp.tanL<tanL) 	upp.tanL=tanL;
   }
//			Rxy  cosP
   int kase = 0;
   if (Dow[0].Phi < -M_PI/2) kase |=1;
   if (Upp[0].Phi > -M_PI/2) kase |=2;
   if (Dow[1].Phi <  M_PI/2) kase |=4;
   if (Upp[1].Phi >  M_PI/2) kase |=8;

   mLim[2][0] = 1e11;
   switch (kase) {
     case 1:; case 2:;case 1|2:;
     mLim[0][kPhi ] = Dow[0].Phi ;
     mLim[0][kRxy ] = Dow[0].Rxy ;
     mLim[0][kTanL] = Dow[0].tanL;
     mLim[0][kZ   ] = Dow[0].Z   ;
     mLim[1][kPhi ] = Upp[0].Phi ;
     mLim[1][kRxy ] = Upp[0].Rxy ;
     mLim[1][kTanL] = Upp[0].tanL;
     mLim[1][kZ   ] = Upp[0].Z   ;
     break;
     case 4:; case 8:; case 4|8:;    
     
     mLim[0][kPhi ] = Dow[1].Phi ;
     mLim[0][kRxy ] = Dow[1].Rxy ;
     mLim[0][kTanL] = Dow[1].tanL;
     mLim[0][kZ   ] = Dow[1].Z   ;
     mLim[1][kPhi ] = Upp[1].Phi ;
     mLim[1][kRxy ] = Upp[1].Rxy ;
     mLim[1][kTanL] = Upp[1].tanL;
     mLim[1][kZ   ] = Upp[1].Z   ;
     break;
     
     case 1|4:;case 2|4:; case 1|2|4:; case 2|4|8:;case 2|8:
     mLim[0][kPhi ] = MIN(Dow[0].Phi ,Dow[1].Phi );
     mLim[0][kRxy ] = MIN(Dow[0].Rxy ,Dow[1].Rxy );
     mLim[0][kTanL] = MIN(Dow[0].tanL,Dow[1].tanL);
     mLim[0][kZ   ] = MIN(Dow[0].Z   ,Dow[1].Z   );
     mLim[1][kPhi ] = MAX(Upp[0].Phi ,Upp[1].Phi );
     mLim[1][kRxy ] = MAX(Upp[0].Rxy ,Upp[1].Rxy );
     mLim[1][kTanL] = MAX(Upp[0].tanL,Upp[1].tanL);
     mLim[1][kZ   ] = MAX(Upp[0].Z   ,Upp[1].Z   );
     break;
    
     default: 
     assert((kase&1) && (kase&8));
     mLim[0][kPhi ] = Dow[0].Phi ;
     mLim[0][kRxy ] = Dow[0].Rxy ;
     mLim[0][kTanL] = Dow[0].tanL;
     mLim[0][kZ   ] = Dow[0].Z   ;
     mLim[1][kPhi ] = Upp[0].Phi ;
     mLim[1][kRxy ] = Upp[0].Rxy ;
     mLim[1][kTanL] = Upp[0].tanL;
     mLim[1][kZ   ] = Upp[0].Z   ;
     mLim[2][kPhi ] = Dow[1].Phi ;
     mLim[2][kRxy ] = Dow[1].Rxy ;
     mLim[2][kTanL] = Dow[1].tanL;
     mLim[2][kZ   ] = Dow[1].Z   ;
     mLim[3][kPhi ] = Upp[1].Phi ;
     mLim[3][kTanL] = Upp[1].tanL;
     mLim[3][kZ   ] = Upp[1].Z   ;

     break;
     
  }


}
#endif
//_____________________________________________________________________________
int  StvConeSelector::Reject(const float x[3],const void* hp)
{
   if (x[0]*x[0]+x[1]*x[1]>mRxy2) return 1;

   float xx[3] = {x[0]-mHit[0],x[1]-mHit[1],x[2]-mHit[2]};

   float r2xy = xx[0]*xx[0]+xx[1]*xx[1];
   float z2 = xx[2]*xx[2];
   if (r2xy < (kMinTan*kMinTan)*z2) 	return 3;		
   mHitLen = (r2xy+z2);
   if (mHitLen  < 1e-8) 		return 4;
   mHitPrj = Dot(xx,mDir);
   if (mHitPrj>mLen) 			return 6;	//Outside of cone along
   float imp =mHitLen-mHitPrj*mHitPrj; if (imp<=0) imp = 0;
   float lim = (mErr) + mHitPrj*mTan;
   if (imp > lim*lim)          		return 7;	//Outside of cone aside
   int ans = 99;
   if (mHp != hp) { 					//different layers, only prj is important
     if (mHitPrj>mMinPrj) 		return 8;	//more far than best,along
     ans = -1;
   } else         {					//same layer, only impact is important
     if (imp>mMinImp) 			return 9;	//same plane but impact bigger
     ans = 0;
   }
   mMinPrj= mHitPrj; mMinImp=imp; mHp = hp;
   return ans;						//impact best but cone the same
}
//_____________________________________________________________________________
void StvConeSelector::Update()
{
   if (mLen<=mHitPrj) return;
   mLen = mHitPrj;
   UpdateLims();
}


