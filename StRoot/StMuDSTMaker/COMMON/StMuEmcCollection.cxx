//###########################################################
// EMC Micro Event
// Author: Alexandre A. P. Suaide
// initial version 08/2001
//
// See README for details
//########################################################### 
#include <string.h>

#include "StMuEmcCollection.h"
#include "Stiostream.h"
#include "StMuEmcUtil.h"
static StMuEmcUtil util; // to ease decoding of EEMC hits
ClassImp(StMuEmcCollection)

StMuEmcCollection::StMuEmcCollection()
{    
  int n = (char*)&mEndcapEmcPoints - (char*)&mTowerADC + sizeof(mEndcapEmcPoints);
  memset(&mTowerADC,0,n);
} 

StMuEmcCollection::StMuEmcCollection(StMuEmcCollection& o) {
  memcpy( mTowerADC,  o.mTowerADC,  sizeof(mTowerADC)  );
  memcpy( mEndcapTowerADC,  o.mEndcapTowerADC,  sizeof(mEndcapTowerADC)  );

  for ( int i=0; i<2; i++) {
    mSmdHits[i] = (TClonesArray*)o.mSmdHits[i]->Clone();
    mEndcapSmdHits[i] = (TClonesArray*)o.mEndcapSmdHits[i]->Clone();
  } 
  for ( int i=0; i<4; i++) {
    mEmcClusters[i] = (TClonesArray*)o.mEmcClusters[i]->Clone();
    mEndcapEmcClusters[i] = (TClonesArray*)o.mEndcapEmcClusters[i]->Clone();
  }
  mPrsHits = (TClonesArray*)o.mPrsHits->Clone();
  mEmcPoints = (TClonesArray*)o.mEmcPoints->Clone(); 
  mEndcapPrsHits = (TClonesArray*)o.mEndcapPrsHits->Clone();
  mEndcapEmcPoints = (TClonesArray*)o.mEndcapEmcPoints->Clone(); 
}

void StMuEmcCollection::init() 
{
  if (mPrsHits) return;
  mEmcPoints      = new TClonesArray("StMuEmcPoint",0);
  mPrsHits        = new TClonesArray("StMuEmcHit",0);
  mEndcapEmcPoints= new TClonesArray("StMuEmcPoint",0);
  mEndcapPrsHits  = new TClonesArray("StMuEmcHit",0);
  for(int i=0;i<4;i++) 
  { 
    mEmcClusters[i]       = new TClonesArray("StMuEmcCluster",0);
    mEndcapEmcClusters[i] = new TClonesArray("StMuEmcCluster",0);
    if(i>=2) continue;
    mSmdHits[i]         = new TClonesArray("StMuEmcHit",0);
    mEndcapSmdHits[i]   = new TClonesArray("StMuEmcHit",0);
  }
}


StMuEmcCollection::~StMuEmcCollection()
{
  int n = &mEndcapEmcPoints-&mPrsHits +1;
  TClonesArray **arr = &mPrsHits;
  for(int i=0;i<n;i++) { delete arr[i]; arr[i] = 0;} 
}
void StMuEmcCollection::DeleteThis()
{
    for ( int i=0; i<2; i++) mSmdHits[i]->Delete();    
    for ( int i=0; i<4; i++) mEmcClusters[i]->Delete();    
    mEmcPoints->Delete();  
}

void StMuEmcCollection::clear(Option_t *option)
{
  int n = &mEndcapEmcPoints-&mPrsHits +1;
  TClonesArray **arr = &mPrsHits;
  for(int i=0;i<n;i++) { if (arr[i]) arr[i]->Clear();} 

/*  for(int i=0;i<7200;i++) mTowerADC[i]=0;
  for(int i=0;i<1080;i++) mEndcapTowerADC[i]=0;
  // deleting points ...
  for(int i=0;i<getNPoints();i++)
  {
    StMuEmcPoint *point = getPoint(i);
    if(point) delete point;
  }
  for(int i=0;i<getNEndcapPoints();i++)
  {
    StMuEmcPoint *point = getEndcapPoint(i);
    if(point) delete point;
  }
  // deleting clusters and hits
  for(int d=0;d<4;d++)
  {
    for(int i=0; i<getNClusters(d);i++)
    {
      StMuEmcCluster* cluster = getCluster(d,i);
      if(cluster) delete cluster;
    }
    for(int i=0; i<getNEndcapClusters(d);i++)
    {
      StMuEmcCluster* cluster = getEndcapCluster(d,i);
      if(cluster) delete cluster;
    }
  }
  for(int d=3;d<5;d++)
  {
    for(int i=0; i<getNSmdHits(d);i++)
    {
      StMuEmcHit* hit = getSmdHit(d,i);
      if(hit) delete hit;
    }
    for(int i=0; i<getNEndcapSmdHits(d+3);i++)
    {
      StMuEmcHit* hit = getEndcapSmdHit(d+3,i);
      if(hit) delete hit;
    }
  }
  for(int i=0; i<getNPrsHits();i++)
  {
    StMuEmcHit* hit = getPrsHit(i);
    if(hit) delete hit;
  }
  for(int i=0; i<getNEndcapPrsHits();i++)
  {
    StMuEmcHit* hit = getEndcapPrsHit(i);
    if(hit) delete hit;
  }
  
  for(int i=0;i<4;i++) mEndcapEmcClusters[i]->Delete();
  for(int i=0;i<2;i++) mEndcapSmdHits[i]->DElete();
  for(int i=0;i<4;i++) mEmcClusters[i]->Delete();
  for(int i=0;i<2;i++) mSmdHits[i]->Delete();
  mPrsHits->Delete();
  mEmcPoints->Delete();
  mEndcapPrsHits->Delete();
  mEndcapEmcPoints->Delete();*/
  return;
}
void StMuEmcCollection::packbits(unsigned char *data, unsigned int value, unsigned int nbits, unsigned int index)
{
  unsigned int start     = index*nbits;
  unsigned int startByte = start/8;
  unsigned int startBit  = start%8;  
  unsigned int a         = 0;
  unsigned int s         = 1;
  for(int i=0;i<4;i++)  { a+=data[startByte+i]*s; s*=256; }
  unsigned int mask = ((unsigned int)(1<<nbits)-1);
  unsigned int b = ((value&mask)<<startBit) | (a&(~(mask<<startBit)));  
  data[startByte+0] = (unsigned char)((b & 0x000000FF));
  data[startByte+1] = (unsigned char)((b & 0x0000FF00)>>8);
  data[startByte+2] = (unsigned char)((b & 0x00FF0000)>>16);
  data[startByte+3] = (unsigned char)((b & 0xFF000000)>>24);
  return;
}
unsigned int StMuEmcCollection::unpackbits(unsigned char *data, unsigned int nbits, unsigned int index)
{
  unsigned int start     = index*nbits;
  unsigned int startByte = start/8;
  unsigned int startBit  = start%8;  
  unsigned int a         = 0;
  unsigned int s         = 1;
  for(int i=0;i<4;i++)  { a+=data[startByte+i]*s; s*=256; }  
  unsigned int mask = ((unsigned int)(1<<nbits)-1);
  unsigned int b = (unsigned int)(a&(mask<<startBit))>>startBit;
  return b;
}
int StMuEmcCollection::getTowerADC(int id, int detector)
{
  if(detector == bemc)
  {
    if(id<1 || id>4800) return 0;
    return (int)unpackbits(mTowerADC,12,(unsigned int)(id-1));
  }
  if(detector == eemc)
  {
    if(id<1 || id>720) return 0;
    return (int)unpackbits(mEndcapTowerADC,12,(unsigned int)(id-1));
  }
  return 0;
}


int StMuEmcCollection::getNSmdHits(int detector)
{
  TClonesArray *tca = NULL;
  if (!mPrsHits) return 0;
  if(detector==bsmde || detector==bsmdp) tca = mSmdHits[detector-bsmde];
  if(detector==esmdu || detector==esmdv) tca = mEndcapSmdHits[detector-esmdu];
  if(tca) return tca->GetEntries();
  else    return 0;
}
StMuEmcHit* StMuEmcCollection::getSmdHit(int hitId,int detector)
{
  TClonesArray *tca = NULL;
  if (!mPrsHits) return 0;
  if(detector==bsmde || detector==bsmdp) tca = mSmdHits[detector-bsmde];
  if(detector==esmdu || detector==esmdv) tca = mEndcapSmdHits[detector-esmdu];
  if(tca)
  {
    int counter = tca->GetEntries();
    if(hitId<0 || hitId>counter) return NULL;
    return (StMuEmcHit*)tca->At(hitId);
  }
  return NULL;
}
int StMuEmcCollection::getNPrsHits(int detector)
{
  if (!mPrsHits) return 0;
  TClonesArray *tca = NULL;
  if(detector == bprs) tca = mPrsHits;
  if(detector == eprs) tca = mEndcapPrsHits;
  if(tca) return tca->GetEntries();
  else    return 0;
}
StMuEmcHit* StMuEmcCollection::getPrsHit(int hitId, int detector)
{
  if (!mPrsHits) return 0;
  TClonesArray *tca = NULL;
  if(detector == bprs) tca = mPrsHits;
  if(detector == eprs) tca = mEndcapPrsHits;
  if(tca)
  {
    int counter = tca->GetEntries();
    if(hitId<0 || hitId>counter) return NULL;
    return (StMuEmcHit*)tca->At(hitId);
  }
  return NULL;
}
int StMuEmcCollection::getNClusters(int detector)
{
  if (!mPrsHits) return 0;
  if(detector<bemc && detector>esmdv) return 0;
  TClonesArray *tca =NULL;
  if(detector>=bemc && detector <= bsmdp) tca = mEmcClusters[detector-bemc];
  else tca = mEndcapEmcClusters[detector-eemc];

  //cout << "DEBUG :: detector bemc bsmdp " 
  //     << detector << " " << bemc << " " << bsmdp << endl;

  if (tca) return tca->GetEntries();
  else     return 0;
}

StMuEmcCluster* StMuEmcCollection::getCluster(int clusterId,int detector)
{
  if (!mPrsHits) return 0;
  if(detector<bemc && detector>esmdv) return NULL;
  TClonesArray *tca = NULL;
  if(detector>=bemc && detector <= bsmdp) tca = mEmcClusters[detector-bemc];
  else tca = mEndcapEmcClusters[detector-eemc];
  int counter = tca->GetEntries();
  if(clusterId<0 || clusterId>counter) return NULL;
  return (StMuEmcCluster*)tca->At(clusterId);
}
int StMuEmcCollection::getNPoints()
{
  if (!mPrsHits) return 0;
  TClonesArray *tca =mEmcPoints;
  if (tca)  return tca->GetEntries();
  else      return 0;
}
int StMuEmcCollection::getNEndcapPoints()
{
  if (!mPrsHits) return 0;
  TClonesArray *tca =mEndcapEmcPoints;
  if (tca)  return tca->GetEntries();
  else      return 0;
}
StMuEmcPoint* StMuEmcCollection::getPoint(int pointId)
{
  if (!mPrsHits) return 0;
  TClonesArray *tca =mEmcPoints;
  int counter = tca->GetEntries();
  if(pointId<0 || pointId>counter) return NULL;
  return (StMuEmcPoint*)tca->At(pointId);
}
StMuEmcPoint* StMuEmcCollection::getEndcapPoint(int pointId)
{
  if (!mPrsHits) return 0;
  TClonesArray *tca =mEndcapEmcPoints;

  if (tca){
    int counter = tca->GetEntries();
    if(pointId<0 || pointId>counter) return NULL;
    return (StMuEmcPoint*)tca->At(pointId);
  } else {
    return NULL;
  }
}



void StMuEmcCollection::setTowerADC(int id,int adc, int detector)
{
  if (!mPrsHits) init();
  if(detector == bemc)
  {
    if(id<1 || id>4800) return;
    packbits(mTowerADC,adc,12,(unsigned int)(id-1));
  }
  if(detector == eemc)
  {
    if(id<1 || id>720) return;
    packbits(mEndcapTowerADC,adc,12,(unsigned int)(id-1));
  }
  return;
}
void StMuEmcCollection::addSmdHit(int detector)
{
  if (!mPrsHits) init();
  TClonesArray *tca = NULL;
  if(detector==bsmde || detector==bsmdp) tca = mSmdHits[detector-bsmde];
  if(detector==esmdu || detector==esmdv) tca = mEndcapSmdHits[detector-esmdu];
  if(tca) 
  {
    int counter = tca->GetEntries();
    new ((*tca)[counter]) StMuEmcHit();   
  }
  return;
}
void StMuEmcCollection::addPrsHit(int detector)
{
  if (!mPrsHits) init();
  TClonesArray *tca = NULL;
  if(detector == bprs) tca = mPrsHits;
  if(detector == eprs) tca = mEndcapPrsHits;
  if(tca)
  {
    int counter = tca->GetEntries();
    new ((*tca)[counter]) StMuEmcHit();
  }
  return;
}
void StMuEmcCollection::addCluster(int detector)
{
  if(detector<bemc && detector>esmdv) return;
  if (!mPrsHits) init();
  TClonesArray *tca =NULL;
  if(detector>=bemc && detector <= bsmdp) tca = mEmcClusters[detector-bemc];
  else tca = mEndcapEmcClusters[detector-eemc];
  int counter = tca->GetEntries();
  new ((*tca)[counter]) StMuEmcCluster();
  return;
}
void StMuEmcCollection::addPoint()
{
  if (!mPrsHits) init();
  TClonesArray *tca =mEmcPoints;
  int counter = tca->GetEntries();
  new ((*tca)[counter]) StMuEmcPoint();
  return;
}
void StMuEmcCollection::addEndcapPoint()
{
  if (!mPrsHits) init();
  TClonesArray *tca =mEndcapEmcPoints;
  int counter = tca->GetEntries();
  new ((*tca)[counter]) StMuEmcPoint();
  return;
}


void StMuEmcCollection
::getEndcapTowerADC(int ihit1, int &adc, int &sec, int &sub, int & eta)
{
  int ihit=ihit1+1;  // it was not my idea to abort on index=0, JB
  adc=getTowerADC(ihit,eemc);
  if(! util.getEndcapBin(eemc,ihit,sec,eta,sub))  return ;
  adc=sec=sub=eta=-1;
  return;
}


StMuEmcHit * StMuEmcCollection
::getEndcapPrsHit(int ihit, int &sec, int &sub, int & eta, int &pre)
{
  if (!mPrsHits) return 0;
  StMuEmcHit * h =  getPrsHit(ihit,eprs);
  int ssub;
  if( util.getEndcapBin(eprs,h->getId(),sec,eta,ssub)) return 0;
  pre=1+(ssub-1)/5;
  sub=1+(ssub-1)%5;
  return h;
}


int StMuEmcCollection::getNEndcapSmdHits(char uv)
{
  if(uv!='U' &&  uv!='V') return 0;
  return getNSmdHits((int)esmdu+uv-'U');
}


StMuEmcHit * StMuEmcCollection
::getEndcapSmdHit(char uv, int ihit,int &sec, int &strip)
{
  if(uv!='U' &&  uv!='V') return 0;
  if (!mPrsHits) return 0;
  int det=(int)esmdu+uv-'U';
  StMuEmcHit * h =getSmdHit(ihit,det);  
  int idum;
  if( util.getEndcapBin(det,h->getId(),sec,strip,idum)) return 0;
  return h;
}

