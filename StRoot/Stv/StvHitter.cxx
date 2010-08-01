#include "StvHitter.h"
#include "Stv/StvHit.h"
#include "TGeoVolume.h"
#include "StvHitter.h"
#include "StarVMC/GeoTestMaker/StTGeoHelper.h"
#include "Stv/StvNodePars.h"
#include "StarRoot/StMultiKeyMap.h"
//_____________________________________________________________________________
//_____________________________________________________________________________
StvHitter::StvHitter()
{
  fMultiIter = new StMultiKeyMapIter(0);
}
//_____________________________________________________________________________
StvHitter::~StvHitter()
{
  delete fMultiIter;
}
//_____________________________________________________________________________
void StvHitter::Reset()
{
  mHitPlane=0;
}
//_____________________________________________________________________________
const StvHits *StvHitter::GetHits(const StvNodePars *pars, const float gate[2])
{
  mHits.clear();
  const StHitPlane *myHitPlane = StTGeoHelper::Inst()->GetCurrentHitPlane();
  if (!myHitPlane) 		return 0;
  if (!myHitPlane->GetNHits()) 	return &mHits;
  if (mHitPlane == myHitPlane)  return 0;
  mHitPlane = myHitPlane;
  mHitMap  = mHitPlane->GetHitMap();
  float xNode[3]={pars->_x,pars->_y,pars->_z};
  mOrg =  mHitPlane->GetOrg(xNode);
  mDir = &mHitPlane->GetDir(xNode);
  float tau =0,den=0;
  float const *ort =(*mDir)[0];
  float mom[3]={pars->_cosCA,pars->_sinCA,pars->_tanl};

  for (int j=0;j<3;j++) {tau+=(mOrg[j]-xNode[j])*ort[j];
                         den+= mom[j]           *ort[j];}
  tau/=den;
  for (int j=0;j<3;j++) {xNode[j]+=mom[j]*tau;}
  float lNode[3],lim[2][2];

  mHitPlane->ToLocal(xNode,lNode);
  for (int j=0;j<2;j++) {lim[0][j]=lNode[1+j]-gate[j];
                         lim[1][j]=lNode[1+j]+gate[j];}
  
  fMultiIter->Set(mHitMap->GetTop(),lim[0],lim[1]);
  for (StMultiKeyNode *node=0;(node = *(*fMultiIter)) ;++(*fMultiIter)) 
  { 
    StvHit *nexHit = (StvHit*)node->GetObj();
//??????    if (nexHit->timesUsed()) continue;
    mHits.push_back(nexHit);
  }
  return &mHits;
}
//_____________________________________________________________________________
const StHitPlane*  StvHitter::GetHitPlane() const {return mHitPlane;}

//_____________________________________________________________________________
StDetectorId StvHitter::GetDetId() const
{ 
  if (!mHitPlane) return (StDetectorId)0;
  return mHitPlane->GetDetId();
}


