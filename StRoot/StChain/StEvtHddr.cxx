#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "StEvtHddr.h"
#include "StarRoot/TUnixTime.h"

ClassImp(StEvtHddr)
//_____________________________________________________________________________
StEvtHddr::StEvtHddr(TDataSet *parent):TDataSet("EvtHddr",parent)
{
  SetDateTime(20330101,0);
  SetEventType("NONE");
  memset(&mRunNumber,0,(char*)(&mEventNumber)-(char*)&mRunNumber);
  mRunNumber=-1;mOldRunNumber=-2;mEventNumber=-1;
}
//_____________________________________________________________________________
StEvtHddr &StEvtHddr::operator=(const StEvtHddr &hddr)
{
  if (!GetName() [0]) SetName (hddr.GetName ());
  if (!GetTitle()[0]) SetTitle(hddr.GetTitle());
  Int_t oldRun = mRunNumber;
  memcpy(&mRunNumber,&hddr.mRunNumber,(char*)((&mEventNumber)+1)-(char*)&mRunNumber);
  mEventTime = hddr.mEventTime;
  mProdTime  = hddr.mProdTime;
  mEventType = hddr.mEventType;
  mOldRunNumber = oldRun;
  return *this;
}
//_____________________________________________________________________________
void StEvtHddr::FillTag(EvtHddr_st *tag)
{

  assert((char*)&mEventNumber     -(char*)&mRunNumber
       ==(char*)&tag->mEventNumber-(char*)&tag->mRunNumber);

  memcpy(tag,&mRunNumber,(char*)(&mEventNumber+1)-(char*)&mRunNumber);
  tag->mEventTime = mEventTime.GetDate() + mEventTime.GetTime()/1000000.;
  tag->mProdTime  =  mProdTime.GetDate() +  mProdTime.GetTime()/1000000. ;
  tag->mEventType[0] = 0;
  strncat(tag->mEventType,mEventType,15);
}
//_____________________________________________________________________________
  void StEvtHddr::SetGMTime(UInt_t ut)
{
   TUnixTime unixTime(ut);
   Int_t dat,tim;
   unixTime.GetGTime(dat,tim);
   mEventTime.Set(dat,tim);
}
//_____________________________________________________________________________
  UInt_t StEvtHddr::GetUTime() 	  const 
{
    TUnixTime unixTime;
    unixTime.SetGTime(mEventTime.GetDate(),mEventTime.GetTime());
    return unixTime.GetUTime();
}
//_____________________________________________________________________________
  void StEvtHddr::Print(Option_t*)
{
  printf("\n *********** Event Info **********************\n");
  printf("\tRun: \t%5d  Event %5d  TimeStamp %8d.%6d  Bunch %u\n"
         ,mRunNumber,mEventNumber,mEventTime.GetDate(),mEventTime.GetTime()
         ,mBunchCrossingNumber);
  printf("\t EvtSize: %d \tInpTrig: %4x OutTrig: %4x  \n"
         ,mEventSize,mInputTriggerMask,mTriggerMask);
  printf("\n *********************************************\n\n");
}
