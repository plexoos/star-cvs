#include "Stiostream.h"
#include "StvVertexFinder.h"
#include "Stv/StvHit.h"
#include "StvToolkit.h"

//______________________________________________________________________________
StvVertexFinder::StvVertexFinder(const char* name): TNamed(name,"")
{
  mResulted=0;
  cout <<"StvVertexFinder::StvVertexFinder() -I- Started :" << name<<endl;
}

//______________________________________________________________________________
StvVertexFinder::~StvVertexFinder()
{}
//______________________________________________________________________________
void StvVertexFinder::Clear(const char *)
{
  mResulted=0;mResult.clear();
}
//______________________________________________________________________________
const StvHits& StvVertexFinder::Result()
{
  if (mResulted) return mResult;	
  mResulted=1; mResult.clear();
  StvToolkit *kit = StvToolkit::Inst();
  double Xd[9]; float  Xf[9];

  for (int i=0;(!GetVertex(i,Xd,Xd+3));i++) {
    StvHit *hit=kit->GetHit();
    for (int j=0;j<9;j++) {Xf[j]=Xd[j];}
    hit->setGlobal(0,0,Xf,Xf+3);
    mResult.push_back(hit);}	
  return mResult; 
}
