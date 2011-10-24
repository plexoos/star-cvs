// $Id: StObject.cxx,v 1.25 2011/10/24 19:20:22 perev Exp $
// $Log: StObject.cxx,v $
// Revision 1.25  2011/10/24 19:20:22  perev
// assert for dead object added
//
// Revision 1.24  2011/10/05 23:13:19  perev
// Remove redundant now cleaning
//
// Revision 1.23  2011/07/19 19:20:21  perev
// More accurate counter handling
//
// Revision 1.22  2009/08/26 20:44:08  fine
// fix the compilation issues under SL5_64_bits  gcc 4.3.2
//
// Revision 1.21  2006/08/10 03:34:38  perev
// Assert==>assert
//
// Revision 1.20  2005/10/21 21:13:52  perev
// test added to avoid copy to itself. Make walgrin happy
//
// Revision 1.19  2004/05/03 23:31:46  perev
// Possible non init WarnOff
//
// Revision 1.18  2003/09/02 17:59:24  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.17  2002/11/26 02:23:38  perev
// new ROOT adoptation
//
// Revision 1.16  2002/01/27 23:46:49  perev
// Zombie test added
//
// Revision 1.15  2001/05/30 17:46:41  perev
// StEvent branching
//
// Revision 1.14  2000/09/30 17:48:27  perev
// Zombies cons and loop for stru vector
//
// Revision 1.13  2000/09/15 15:11:58  perev
// Zombie for StEvent
//
// Revision 1.12  2000/07/30 01:49:03  perev
// StObject vers restored
//
// Revision 1.10  2000/06/19 01:28:26  perev
// STL StEvent
//
// Revision 1.9  2000/04/23 01:00:45  perev
// StEvent monolitic I/O
//
// Revision 1.8  2000/04/20 14:24:09  perev
// StArray fixes
//
// Revision 1.7  2000/04/18 02:57:25  perev
// StEvent browse
//
// Revision 1.6  1999/12/21 15:42:58  fine
// remove compilation warning
//
// Revision 1.5  1999/12/13 21:40:41  perev
// Remove warnings
//
// Revision 1.4  1999/11/17 14:22:10  perev
// bug in dtor fix
//
// Revision 1.3  1999/11/15 23:09:10  perev
// Streamer for StrArray and auto remove
//
// Revision 1.2  1999/06/23 20:31:04  perev
// StArray I/O + browser
//
// Revision 1.1  1999/04/30 13:15:55  fisyak
// Ad StObject, modification StArray for StRootEvent
//
#include <assert.h>
#include "StObject.h"
#include "TDataSetIter.h"
#include "TROOT.h"
#include "TError.h"
#include "TMath.h"
#include "TBrowser.h"
#include "TClass.h"
#include "StArray.h"
#include "StAutoBrowse.h"
#include "TSystem.h"

StXRefManager 	 *StXRefManager::fgManager=0;
UInt_t 	          StObject::fgTally=0;
StXRefManagerList StXRefManager::fgManagerList;



ClassImp(StObject)
//_____________________________________________________________________________
StObject::~StObject()
{
}
//_____________________________________________________________________________
void StObject::Browse(TBrowser *tb)
{
  StAutoBrowse::Browse(this,tb);
}
//_____________________________________________________________________________
Bool_t StObject::IsFolder() const
{
  return StAutoBrowse::Browse((TObject*)this,0);
}
//______________________________________________________________________________
void StObject::Streamer(TBuffer &R__b)
{
//	Stream an object of class StObject.
  unsigned char uc=0;

  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion();

    switch (R__v){
      case 1:  TObject::Streamer(R__b); 			return;
      case 2:  R__b >> uc; if (uc) TObject::Streamer(R__b); 	return;
      default: TObject::Streamer(R__b);				return;
     }

  } else {
    assert(TestBit(kNotDeleted));
    R__b.WriteVersion(StObject::Class());
    if (fgTally) {
      UInt_t udx = GetUniqueID();
      if (!udx) { udx = ++fgTally; SetUniqueID(udx);}
    } 
    TObject::Streamer(R__b);
  }
}
//______________________________________________________________________________
UInt_t StObject::Ztreamer(TBuffer &R__b)
{
   UInt_t udx = GetUniqueID();
   if (!udx) { udx = ++fgTally; SetUniqueID(udx);}
   R__b << udx;
   return udx;
}

ClassImp(StUUId)
//_____________________________________________________________________________
StUUId::StUUId()
{
   memset(fID,0,16);
}
//_____________________________________________________________________________
void StUUId::Generate()
{
   static UInt_t uu[4] = {0,0,0,0};
   if (!uu[0]) {
     uu[3]  = TMath::Hash(gSystem->HostName());
     uu[3] ^= TMath::Hash(gSystem->WorkingDirectory());
     uu[2]  = (gSystem->GetPid())<<16;
   }
   if (fID[0])	return;
   fID[3] = uu[3]; 
   fID[2] = uu[2]++;
   fID[1] = (UInt_t)((ULong_t)this);
   fID[0] = (UInt_t)((ULong_t)gSystem->Now());
}

//_____________________________________________________________________________
void StUUId::Streamer(TBuffer &R__b)
{ 
  if (R__b.IsReading()) {
     R__b.ReadFastArray (fID,4);
  } else {
     R__b.WriteFastArray(fID,4);
  } 
}
//_____________________________________________________________________________
StUUId &StUUId::operator=(const StUUId &from)
{
  if (this != &from) memcpy(fID,from.fID,sizeof(fID));
  return *this;
}
//_____________________________________________________________________________
StUUId &StUUId::operator=(const char  *from )
{
  memcpy(fID,from    ,16); return *this;
}
//_____________________________________________________________________________
int StUUId::Compare(const StUUId &u2) const
{
  return memcmp(fID,u2.fID,16);
}


//_____________________________________________________________________________
ClassImp(StXRef)
//_____________________________________________________________________________
StXRef::StXRef(const char *brName, StXRefMain *evt,UInt_t tally)
 : TDataSet(brName,evt)
{
   SetMain(evt);
   if (evt) SetUUId(evt->GetUUId());
   SetTally(tally);
   
}
//_____________________________________________________________________________
StXRef::~StXRef()
{
}

//_____________________________________________________________________________
void StXRef::Streamer(TBuffer &R__b)
{
   UInt_t R__s,R__c;
   
   if (R__b.IsReading() ) { //READ
      Version_t R__v = R__b.ReadVersion(&R__s,&R__c); if (R__v){};
      fUUId.Streamer(R__b);
      StXRefManager::Open(this);
      TDataSet::Streamer(R__b);
      R__b >> fTally;
      StXRefManager::Close(this);
      Synchro(1);
      R__b.CheckByteCount(R__s,R__c,Class());

   } else {

      assert(!fUUId.IsNull());
      Synchro(0);
      R__c = R__b.WriteVersion(Class(),kTRUE);
      fUUId.Streamer(R__b);
      StXRefManager::Open(this);
      TDataSet::Streamer(R__b);
      StXRefManager::Close(this);
      R__b << fTally;
      R__b.SetByteCount(R__c,kTRUE);
   }
}
//_____________________________________________________________________________
StXRefMain *StXRef::GetMain()
{
  if (!fMain) {
    fMain = MakeMain();
    fMain->SetUUId(fUUId);
  }
  return fMain;
}
//_____________________________________________________________________________
void StXRef::Add(TDataSet *ds)
{
   if (ds == this) 		return;
   if (ds->GetParent() == this) return;
   TDataSet *os = FindByName(ds->GetName());
   if (os == ds) 		return;
   if (os){
     assert(os->IsA()==ds->IsA());
     TDataSetIter   Next(this);
     StXRef *xr;
     while((xr = (StXRef*)Next())) {
       if (!xr->InheritsFrom(Class())) 	continue;
       if (fUUId.Compare(xr->GetUUId()))continue;
       Remove(xr);
     }
   }  
   if (ds->InheritsFrom(Class())) 
      assert(!fUUId.Compare(((StXRef*)ds)->GetUUId()));
   ds->Shunt(0); TDataSet::Add(ds);
}

//_____________________________________________________________________________
ClassImp(StXRefMain)

StXRefMain::~StXRefMain()
{
  assert(!GetUUId().IsNull());
  StXRefManager::Cd(this);
  delete StXRefManager::fgManager;
}
//______________________________________________________________________________
void StXRefMain::Streamer(TBuffer &R__b)
{
   StXRef::Streamer(R__b);  
   StXRefManager::fgManager->Clear("");
}
//_____________________________________________________________________________
StXRefManager::StXRefManager(const StUUId &id)
{
   fTally = 0;
   fUpd = 0;
   fLev = 0;
   fUUId = id;
   fMain=0;
   fgManagerList.push_front(this);
   fColList.push_front(0);
}
//_____________________________________________________________________________
StXRefManager::~StXRefManager()
{
   fgManagerList.remove(this);
   if (fgManager==this) { fgManager=0; StObject::fgTally=0;}
   fMain=0; fTally=0;
   for (StCollListIter it = fColList.begin(); *it ;it++) {delete *it;}
}


//_____________________________________________________________________________
void StXRefManager::Cd(StXRef *xref)
{
  StXRefManager *man = fgManager;

  if (!man || man->fUUId.Compare(xref->GetUUId())!=0) {
     StXRefManagerListIter it;
     man = 0;
     for (it=fgManagerList.begin();it!=fgManagerList.end();it++){
       StXRefManager *m=*it;
       if (m->fUUId.Compare(xref->GetUUId())!=0)	continue;
       man = m; break;
  }  }
  fgManager = man;
}
//_____________________________________________________________________________
void StXRefManager::Open(StXRef *xref)
{
  Cd(xref);
  StXRefManager *man = fgManager;
  if (!man) {
     man = new StXRefManager(xref->GetUUId());
     fgManagerList.push_front(man);
     fgManager   = man;
  }
  if (man->fTally > StObject::fgTally) StObject::fgTally=man->fTally;
  man->fLev++;
  if (man->fMain==0) {
     man->fMain  = xref->GetMain();
  } else {
     xref->SetMain(man->fMain);
  }
}
//_____________________________________________________________________________
void StXRefManager::Close(StXRef *xref)
{
  Cd(xref);
  StXRefManager *man = fgManager;
  assert(man);
  man->Update();
  if (man->fMain) man->fMain->Add(xref);
  man->Update(StObject::fgTally);
  man->fLev--;
  if (!man->fLev) StObject::fgTally = 0;
}  


//_____________________________________________________________________________
void StXRefManager::AddColl (StProxyUrr *rarr)
{   
   fUpd=1; fColList.push_front(rarr);
}
//_____________________________________________________________________________
void StXRefManager::AddColl (const StStrArray *sarr)
{   
   int size = sarr->size();
   if (!size) 	return;
   fUpd=1;
   UInt_t u,uMax=0;
   const TObject *to, **p;
   const_VecTObjIter it= sarr->begin(); 
   for(int i=0;i<size;i++) {
     if (!(to = it[i]))			continue;
     if (!(u = to->GetUniqueID()))	continue;
     if (uMax<u)uMax=u;
     p = (const TObject**)fObjTab.GET(u);
     if (*p) 	{// Used already		
       if (*p == to)			continue;
       assert(to->IsA() == (*p)->IsA()); 
     }
     *p = to;
   }
   Update(uMax);
}
//_____________________________________________________________________________
void StXRefManager::Update ()
{
  if(!fUpd) return;
  fUpd = 0;
  StObjArray *arr;
  StProxyUrr *urr;
  StCollListIter it;
  UInt_t idx,udx,udxMax=0,sizeUrr,lst=999999;
  TObject **p; 
  for (it = fColList.begin(); (urr = *it);) {//List
    sizeUrr = urr->size(); 
    switch (urr->GetType()) {

      case 1: 		//Link case
      assert(sizeUrr==1);
      lst = 2;
      udx = (*urr)[0];
      p = (TObject**)fObjTab.Get(udx);
      if (!p || !(*p)) 		break;
      urr->GetAdr()[0] = *p;
      lst = 0;			break;

      case 0: 		//refArray case 
      lst=0;
      arr = (StObjArray*)urr->GetArr(); assert(arr);
      for (idx=0;idx<sizeUrr;idx++) { //RefArray loop 
         udx = (*urr)[idx];
         p = (TObject**)fObjTab.Get(udx);
         if (!p || !(*p))	{(*urr)[lst++] = udx; continue;}
         arr->push_back(*p); 
      }//end RefArray loop
      if (udxMax<udx) udxMax=udx;
    }
    if (lst) { urr->resize(lst-1);     it++;      }
    else     {it = fColList.erase(it); delete urr;}
  }//end List
  Update(udxMax);
  StObject::fgTally = fTally;
}
//_____________________________________________________________________________
void StXRefManager::Clear (Option_t*)
{
//   ULong_t **page = (ULong_t**)fObjTab.GetList();
//   if (!page) return;
//   for (;page; page = (ULong_t**)page[0]) {
//     if (!page) 		break;
//     if (!page[1]) 	continue;
//     for (int i=0;i<(int)TPageMap::kPAGE;i++) {
//       if (!page[i+2]) 	break;
//       ((TObject*)page[i+2])->SetUniqueID(0);
//     }
//   }
//   fTally = 0;
//   fObjTab.Clear();
//   StObject::fgTally=0;
}

//_____________________________________________________________________________
TDataSet *StXRefManager::GetMain()
{
   if (!fgManager) return 0;
   return fgManager->fMain;
}
//_____________________________________________________________________________
TPageMap::TPageMap()
{

  fList = 0;
  fTopPage = NewPage();
  fLstPage = 0;
  fLstUdx  = 0;
}
//_____________________________________________________________________________


TPageMap::~TPageMap()
{
  Clear();
}
//_____________________________________________________________________________
void TPageMap::Clear() {
   ULong_t *p,*n=0;
   for (p = fList; p ; p = n) 
   { n = (ULong_t*)p[0]; free(p);}
   fList = 0;
   fTopPage = NewPage();
   fLstPage = 0;
   fLstUdx  = 0;
}     
//_____________________________________________________________________________
ULong_t *TPageMap::NewPage()  
{
   int n = sizeof(ULong_t)*(kPAGE+2);
   ULong_t *p = (ULong_t*)malloc(n); memset(p,0,n);
   p[0] = (ULong_t)fList; fList = p;
   return p+2;
}

//_____________________________________________________________________________
ULong_t *TPageMap::Get(UInt_t udx) 
{
   if ((udx&kLAST) == fLstUdx) {
      if (!fLstPage) 	return 0;

   }  else {

      fLstPage = 0;
      fLstUdx = (udx&kLAST);
      ULong_t *b = fTopPage;
      UInt_t   u,s=kBITZ;
      while(2001) {
        u = (udx>>s)&kMASK;
        b = (ULong_t*)b[u];
        if (!b) 		return 0;
        if (!(s -=kBITS))  	break;;
      }
      fLstPage = b;
   }
    return fLstPage + (udx&kMASK);
}
//_____________________________________________________________________________
ULong_t *TPageMap::GET(UInt_t udx) 
{
   if ((udx&kLAST) != fLstUdx || fLstPage==0) {
      fLstUdx = (udx&kLAST);
      ULong_t *b = fTopPage,*a;
      UInt_t   u,s=kBITZ;
      while(2001) {
        u = (udx>>s)&kMASK;
        if (!(a = (ULong_t*)b[u])) 	{((ULong_t**)b)[u] = a = NewPage();}   
        b = a;
        if (!(s -=kBITS))  		break;;
      }
      fLstPage = b; fLstPage[-1]=1;
   }
   return fLstPage + (udx&kMASK);
}
//_____________________________________________________________________________
void TPageMap::Test()
{
  TPageMap map;
  
  UInt_t range = 10000000;
  UInt_t step  = range/1000;
  UInt_t u;
  for (u=1; u < range; u+=step)
  {
     ULong_t *p = map.GET(u);
     assert(p);
     assert(!*p);
     *p = u;
  }
  for (u=1; u < range; u+=step)
  {
     ULong_t *p = map.Get(u);
     assert(p);
     assert(*p);
     assert(*p==u);
  }
  printf(" TPageMap::Test() OK\n");
}  
  
  
   
     





