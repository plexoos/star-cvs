#include <stdio.h>
#include <stdlib.h>
#include "TROOT.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TDataMember.h"
#include "TMethod.h"
#include "TMethodArg.h"
#include "TDataType.h"
#include "Api.h"
#include "TMemberInspector.h"
#include "TExMap.h"
#include "TCollection.h"
#include "TRegexp.h"
#include "TRandom.h"
#include "TError.h"
#include "TPoints3DABC.h"
#include "TInterpreter.h"
#include "TSystem.h"

#include "StEvent.h"
#include "StHit.h"
#include "StTrack.h"
#include "StVertex.h"
#include "StTrackGeometry.h"
#include "StTrackDetectorInfo.h"
#include "StEventHelper.h"

void Break(){printf("InBreak\n");}


class StEventInspector : public TMemberInspector {
public:
StEventInspector(TExMap *map,Int_t &count):fCount(count){fMap = map;};
virtual ~StEventInspector(){};
virtual void Inspect(TClass* cl, const char* parent, const char* name, void* addr);
        void CheckIn(TObject *obj,const char *bwname="");

Int_t &fCount;
TExMap *fMap;
};      

//______________________________________________________________________________
void StEventInspector::Inspect(TClass* kl, const char* tit , const char* name, void* addr)
{
  if(tit && strchr(tit,'.'))	return ;

  TString ts;

  if (!kl) return;
  if (name[0] == '*') name++;
  int ln = strcspn(name,"[ ");
  TString iname(name,ln);
  const char *iName=iname.Data();
  if (iName[1]=='P' && strcmp(iName,"fParent"       )==0) 	return;
  if (iName[0]=='G' && strcmp(iName,"G__virtualinfo")==0)	return;

  G__ClassInfo *classInfo = kl->GetClassInfo();  	
  if (!classInfo)		return;
  G__ClassInfo &cl = *classInfo;


// 		Browse data members
  G__DataMemberInfo m(cl);
  int found=0;
  const char *mName=0;
  while (m.Next()) {	// MemberLoop
     mName = m.Name();
     if (mName[1] != iName[1])	continue;
     if (strcmp(mName,iName) ) 	continue;
     found = 1; break;
  }     
  assert(found);

  // we skip: non TObjects
  //  - the member G__virtualinfo inserted by the CINT RTTI system

  long prop = m.Property() | m.Type()->Property();
  if (prop & G__BIT_ISFUNDAMENTAL) 	return;
  if (prop & G__BIT_ISSTATIC) 		return;
  if (prop & G__BIT_ISENUM) 		return;
  if (strcmp(m.Type()->Fullname(),"TObject") && !m.Type()->IsBase("TObject"))
  					return;

  int  size = sizeof(void*);
  if (!(prop&G__BIT_ISPOINTER)) size = m.Type()->Size(); 

  int nmax = 1;
  if (prop & G__BIT_ISARRAY) {
    for (int dim = 0; dim < m.ArrayDim(); dim++) nmax *= m.MaxIndex(dim);
  }

  for(int i=0; i<nmax; i++) {
    char *ptr = (char*)addr + i*size;
    TObject *obj = (prop&G__BIT_ISPOINTER) ? *((TObject**)ptr) : (TObject*)ptr;
    if (!obj) 		continue;
    const char *bwname = obj->ClassName();
    if (!bwname[0] || strcmp(bwname,obj->ClassName())==0) {
      bwname = name;
	int l = strcspn(bwname,"[ ");
	if (bwname[l]=='[') {
          char cbuf[12]; sprintf(cbuf,"[%02d]",i);
          ts.Replace(0,999,bwname,l);
          ts += cbuf;
          bwname = (const char*)ts;
      }  
    }  

    CheckIn(obj,bwname);

  }

}    
//______________________________________________________________________________
void StEventInspector::CheckIn(TObject *obj,const char *bwname)
{
  if (!obj) return;
  TObject *inobj=0;
  if (obj->InheritsFrom(StRefArray::Class())) return;  
  if (obj->InheritsFrom( StObjLink::Class())) return;  

  if (obj->InheritsFrom(TCollection::Class())){
    TCollection *tcol = (TCollection*)obj;  
    TIter next(tcol);
    while ((inobj=next())) {CheckIn(inobj);}  
    return;
  }

  if (obj->InheritsFrom(StXRef::Class())){

     Long_t &inmap = (*fMap)(TMath::Hash(&obj,sizeof(void*)),(Long_t)obj);
     if (inmap) return;
     inmap = 1;fCount++;
  }

  if (obj->InheritsFrom(StStrArray::Class())){
//     if (obj->IsA()==StSPtrVecTrackNode::Class()) Break();
     Long_t &inmap = (*fMap)(TMath::Hash(&obj,sizeof(void*)),(Long_t)obj);
     if (inmap) return;
     inmap = 2; fCount++;
     int vecobj = ( obj->IsA() == StSPtrVecObject::Class());  
//     printf("%8d %p %s::%s\n",fLevel,obj,obj->GetName(),bwname);
     StStrArray *arr = (StStrArray*)obj; 
     int sz = arr->size();
     for (int idx=0;idx<sz; idx++) {
       inobj = arr->at(idx);
       Int_t count = fCount;
       CheckIn(inobj);
       if (count==fCount && !vecobj) break;  //if no action was made, no sense to continue
     }
     return;
  }

  char cbuf[1000];*cbuf=0;
  StEventInspector insp(fMap,fCount);
  obj->ShowMembers(insp,cbuf);
}
   


//______________________________________________________________________________
ClassImp(StEventHelper)
//______________________________________________________________________________
StEventHelper::StEventHelper(const TObject *evt)
{
   fMap = new TExMap();
   fObject = 0;
   Reset(evt);
}
//______________________________________________________________________________
StEventHelper::~StEventHelper()
{
   delete fMap; fMap=0;
   Clear();
}
//______________________________________________________________________________
void StEventHelper::Clear(Option_t *opt)
{
}
//______________________________________________________________________________
void StEventHelper::Reset(const TObject *evt)
{
   if (fObject == evt) return;
   fObject = (TObject *)evt;
   Clear();
   fMap->Delete();
   if (!fObject) return;
   int kount=0;
   StEventInspector insp(fMap,kount);
   char cbuf[100];
   
   fObject->ShowMembers(insp,cbuf);
}
//______________________________________________________________________________
void StEventHelper::ls(Option_t* option) const
{
   typedef struct { int nb; int sz; const char *tenant; } QWE;
   QWE *qwe=0;

   TExMap map;
   TExMapIter  it(fMap); 
   Long_t key,val;
   while( it.Next(key,val) ) {
     if (val != 2) continue;
     StStrArray *a = (StStrArray *)key;
     Long_t &cnt = map((Long_t)a->IsA());
//     printf("%s %p\n",a->ClassName(),a);
     if (!cnt) {
       qwe = new QWE;
       cnt = (Long_t)qwe; 
       qwe->nb=0; qwe->sz=0;qwe->tenant=0;
     }
     qwe = (QWE*)cnt;
     qwe->nb++; qwe->sz += a->size();
     if (qwe->tenant==0 && a->size()) {
        TObject *to = a->front();
        if (to) qwe->tenant = to->ClassName();
     } 
 
   }
   TExMapIter  itt(&map);
   printf("\n      StEvent(%p)\n",fObject);

   while( itt.Next(key,val) ) {
     TObject *kl = (TObject *)key;
     qwe = (QWE*)val;
     printf ("%8d(%8d) - %s (%s)\n",qwe->nb,qwe->sz,kl->GetName(),qwe->tenant);
     delete qwe;
   }
   printf("\n");

}
//______________________________________________________________________________
TObjArray *StEventHelper::SelConts(const char *sel)
{
  TObjArray *tarr = new TObjArray;
  TRegexp reg(sel);

  TExMapIter  it(fMap); 
  Long_t key,val;
  while( it.Next(key,val) ) {
     if (val == 1) 	continue;
     StStrArray *a = (StStrArray *)key;
     if(a->size()==0)				continue;
     int n =0;
     if (reg.Index(a->ClassName(),&n)<0)	continue;   
     tarr->Add(a);
  }
  return tarr;   
}   
//______________________________________________________________________________
TObjArray *StEventHelper::SelTracks(const char *sel)
{

  int needTrk = (strstr(sel,"Track")!=0);
  int needHit = (strstr(sel,"Hit")  !=0);

  TObjArray *conts = SelConts("^StSPtrVecTrack$");
  TObjArray *traks = new TObjArray();
  Int_t ilast = conts->GetLast();
  for (int idx=0;idx<=ilast;idx++) {
    StObjArray *arr = (StObjArray *)conts->At(idx);
    if (!arr)		continue;
    if (!arr->size())	continue;
    StTrack *trk = (StTrack*)arr->front();
    Assert(trk->InheritsFrom(StTrack::Class()));
    if (!trk)	continue;
    if (needTrk) {
      StTrackPoints *trp = new StTrackPoints(trk);
      traks->Add(trp);
    }

    if (!needHit) 	continue;
    StTrackDetectorInfo *tdi = trk->detectorInfo();
    if (!tdi)		continue;
    StPtrVecHit *hits = &tdi->hits();
    if (!hits) 		continue;
    if (!hits->size())	continue;
    StHitPoints *hip = new StHitPoints(hits);
    traks->Add(hip);
  }         
  delete conts;
  return traks;
}
//______________________________________________________________________________
TObjArray *StEventHelper::SelHits(const char *sel)
{
  const char *RegEx = 0;
  if (!sel) sel ="";

  switch (sel[0]) {
    case 'A': RegEx = "^StSPtrVec.*Hit$"  ;	break;
    case 'T': RegEx = "^StSPtrVecTpcHit$" ;	break;
    case 'R': RegEx = "^StSPtrVecRichHit$";	break;
    default: Warning("SelHits","Unknown selection %s",sel);
             return 0;
  }
  int used = (sel[1]=='U');

  TObjArray *conts = SelConts(RegEx);
  TObjArray *hits = new TObjArray();
  Int_t ilast = conts->GetLast();
  
  for (int idx=0;idx<=ilast;idx++) {
    StObjArray *arr = (StObjArray *)conts->At(idx);
    if (!arr)		continue;
    int sz = arr->size();
    if (!sz)		continue;
    for(int ih=0;ih<sz; ih++) {
      StHit *hit = (StHit*)arr->at(ih);
      if (!hit) 	continue;
      Assert(hit->InheritsFrom(StHit::Class()));

      if ((hit->trackReferenceCount()!=0) !=used) continue ;
      StHitPoints *hip = new StHitPoints(hit);
      hits->Add(hip);
    }
  }         
  delete conts;
  return hits;
}
//______________________________________________________________________________
TObjArray *StEventHelper::SelVertex(const char *sel)
{
  char title[100],name[100];
  const char *cont = 0;
  if (sel[0]=='K') cont= "^StSPtrVecKinkVertex$";
  if (sel[0]=='X') cont= "^StSPtrVecXiVertex$";
  if (sel[0]=='P') cont= "^StSPtrVecPrimaryVertex$";
  if (sel[0]=='V') cont= "^StSPtrVecV0Vertex$";
  int thFlag = 0;
  if (sel[1]=='T' || sel[2]=='T' ) thFlag |=1;
  if (sel[1]=='H' || sel[2]=='H' ) thFlag |=2;
  if (thFlag==0)		   thFlag  =1;
  
  StTrack *trk; StTrackPoints *trp;StVertexPoints *vxp;

  TObjArray *conts = SelConts(cont);
  TObjArray *traks = new TObjArray();
  Int_t ilast = conts->GetLast();
  int nvtx =0;
  for (int idx=0;idx<=ilast;idx++) {
    StObjArray *arr = (StObjArray *)conts->At(idx);
    if (!arr)	continue;
    int sz = arr->size();
    if (!sz)	continue;
    for (int ivx=0; ivx<sz; ivx++) {
      StVertex *vx = (StVertex*)arr->at(ivx);
      if (!vx) 	continue;
      nvtx++;
      vxp = new StVertexPoints(vx);
      trk = vx->parent();
      if (trk ) {
        Assert(trk->InheritsFrom(StTrack::Class()));
         
        

        strcpy(name,trk->ClassName());
        sprintf(title,"parent%d",nvtx);
        trp = new StTrackPoints(trk,name,title);
        traks->Add(trp);}
        
      int nd = vx->numberOfDaughters();
      for (int id=0;id<nd;id++) {
        trk = vx->daughter(id);
        if (!trk)	continue;
        Assert(trk->InheritsFrom(StTrack::Class()));
        strcpy(name,trk->ClassName());
        sprintf(title,"daughter%d",id);
        if (thFlag&1) { 
          trp = new StTrackPoints(trk,name,title);
          traks->Add(trp);
        }
        if (thFlag&2) { 
          StTrackDetectorInfo *tdi = trk->detectorInfo();
          if (!tdi)		continue;
          StPtrVecHit *hits = &tdi->hits();
          if (!hits) 		continue;
          if (!hits->size())	continue;
          StHitPoints *hip = new StHitPoints(hits);
          traks->Add(hip);
        }
      }
    }
  }         
  delete conts;
  return traks;
}

//______________________________________________________________________________
ClassImp(StPoints3DABC)
//______________________________________________________________________________
ClassImp(StTrackPoints)
//______________________________________________________________________________
StTrackPoints::StTrackPoints(StTrack *st,const char *name,const char *title)
:StPoints3DABC(name,title,st),fTrack((StTrack*&)fObj)
{
  Init();
}
//______________________________________________________________________________
void StTrackPoints::Init() 
{  
  if (fXYZ) return;
  float len = fTrack->length();   
  StTrackGeometry *geo = fTrack->geometry(); 
  Assert(geo);
  double curva = fabs(geo->curvature());
  double dip   = geo->dipAngle();
  fSize = abs(int(len*cos(dip)*curva*20))+2;   
  fXYZ = new Float_t[fSize*3];
  StPhysicalHelixD hel = geo->helix();
  Double_t step = len/(fSize-1);
  Double_t ss = 0;
  
  for (int i =0;i<fSize;i++,ss+=step)
  {
     fXYZ[i*3+0] = hel.x(ss); 
     fXYZ[i*3+1] = hel.y(ss); 
     fXYZ[i*3+2] = hel.z(ss); 
  }
}
  
//______________________________________________________________________________
ClassImp(StVertexPoints)
//______________________________________________________________________________
StVertexPoints::StVertexPoints(StVertex *sv,const char *name,const char *title)
:StPoints3DABC(name,title,sv),fVertex((StVertex*&)fObj)
{
  fVertex = sv;
  fSize = 1;
  fXYZ = new Float_t[3];
  fXYZ[0] = fVertex->position().x(); 
  fXYZ[1] = fVertex->position().y(); 
  fXYZ[2] = fVertex->position().z(); 
}
   
   
ClassImp(StHitPoints)
//______________________________________________________________________________
StHitPoints::StHitPoints(StHit *sh,const char *name,const char *title)
:StPoints3DABC(name,title,sh)
{
  fSize = 1;
  fObj = sh;
  Init();
}
//______________________________________________________________________________
StHitPoints::StHitPoints(StRefArray *ar,const char *name,const char *title)
:StPoints3DABC(name,title,ar)
{
  fSize = ar->size();
  if (!fSize) return;
  fObj = (fSize==1) ? ar->front() : ar;
  Init();
}
//______________________________________________________________________________
void StHitPoints::Init() 
{  
  if (fXYZ) return;
  fXYZ = new Float_t[fSize*3];
  
  for (int i =0;i<fSize;i++)
  {
     StHit *hit= (fSize==1) ? (StHit*)fObj: (StHit*)((StRefArray*)fObj)->at(i);
     StThreeVectorF v3 = hit->position();
     fXYZ[i*3+0] = v3.x(); 
     fXYZ[i*3+1] = v3.y(); 
     fXYZ[i*3+2] = v3.z(); 
  }
}
   
//______________________________________________________________________________
ClassImp(StFilterABC)

int StFilterABC::fgDial=0;
//______________________________________________________________________________
StFilterABC::StFilterABC(const char *name):TNamed(name,"")
{
   char cbuf[200];
   sprintf(cbuf,"__StEventControlPanel__.AddFilter((TObject*)%p);",this);
   gInterpreter->ProcessLine(cbuf);
}
//______________________________________________________________________________
void StFilterABC::SetDefs()
{
  for (int i=0; GetNams()[i]; i++) {GetPars()[i]=GetDefs()[i];}
}


//______________________________________________________________________________
void   StFilterABC::Update()
{
   char cbuf[200];
   float       *pars    = GetPars();
   const float *defs    = GetDefs();
   const char **namval  = GetNams();
   int flagg = 2001;
   if (!fgDial++) gInterpreter->LoadMacro("FilterDialog.C");
   sprintf(cbuf
  ,"new FilterDialog((char*)%p,(char**)%p,(float*)%p,(float*)%p,(int*)%p);"
                    ,GetName(),    namval,      defs,      pars,  &flagg);
   printf("%s\n",cbuf);
   void *dial = (void*)gInterpreter->Calc(cbuf);
   printf("StFilterABC::Update() Waiting for update\n");
   while(flagg) {gSystem->DispatchOneEvent(1);}
   sprintf(cbuf,"delete ((FilterDialog*)%p);",dial);
   printf("StFilterABC::Update: %s\n",cbuf);
   gInterpreter->ProcessLine(cbuf);
   printf("StFilterABC::Update() update finished\n");
}
   
//______________________________________________________________________________
ClassImp(StFilterDef)
StFilterDef::StFilterDef(const char *name):StFilterABC(name)
{
  SetDefs();
  
}
//______________________________________________________________________________
const char  **StFilterDef::GetNams() const
{
  static const char *nams[] = {
   "  RandomSelect = ",
   "  RxyMin       = ",
   "  RxyMax       = ",
   "  ZMin         = ",
   "  ZMax         = ",
   "  PhiMin       = ",
   "  PhiMax       = ",
   "  LenMin       = ",
   "  LenMax       = ",
   "  PtMin        = ",
   "  PtMax        = ",
   "  QMin         = ",
   "  QMax         = ", 
   0};
  return nams;
}
//______________________________________________________________________________
const float  *StFilterDef::GetDefs() const
{
  static const float defs[] = {
   /*  RandomSelect=*/    1.00,
   /*  RxyMin      =*/    0.00,
   /*  RxyMax      =*/  200.00,
   /*  ZMin        =*/ -200.00,
   /*  ZMax        =*/ +200.00,
   /*  PhiMin      =*/ -180.00,
   /*  PhiMax      =*/ +180.00,
   /*  LenMin      =*/   +0.00,
   /*  LenMax      =*/ +999.00,
   /*  PtMin       =*/    0.00,
   /*  PtMax       =*/  999.00,
   /*  QMin        =*/   -1   ,
   /*  QMax        =*/   +1   ,
   0};
  return defs;   
}   
//______________________________________________________________________________
Int_t StFilterDef::Accept(StPoints3DABC *pnt) 
{
   static TRandom rrr;
   float x,y,z,r2xy,phid,len,pt,q;
   TObject *to;
   StTrack *trk;
   
   int cut = 1;
   if (fRandomSelect < 1. && fRandomSelect < rrr.Rndm())return 0;


   z = pnt->GetZ(0);				
   cut = 2;
   if (fZMin >z || z > fZMax)				goto SKIP;	

   x = pnt->GetX(0);
   y = pnt->GetY(0);
   r2xy = x*x+y*y;
   cut = 3;
   if (fRxyMin*fRxyMin > r2xy || r2xy > fRxyMax*fRxyMax)goto SKIP;
   phid = atan2(y,x)*(180./M_PI);
   cut = 4;
   if (fPhiMin > phid || phid > fPhiMax) 		goto SKIP;
   to = pnt->GetObject();
   if (!to) 						return 1;
   if (!to->InheritsFrom(StTrack::Class()))		return 1;
   trk = (StTrack*)to;
   len = trk->length();
   cut = 5;
   if (fLenMin >len || len > fLenMax)			goto SKIP;	
   pt = trk->geometry()->momentum().perp();
   cut = 6;
   if (fPtMin >pt || pt > fPtMax)			goto SKIP;	
   q = trk->geometry()->charge();
   cut = 7;
   if (fQMin >q || q > fQMax)				goto SKIP;	
   return 1;   

SKIP: return 0;

}





