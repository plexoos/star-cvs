//
// File generated by rootcint at Thu Sep  9 10:06:49 1999.
// Do NOT change. Changes will be lost next time file is generated
//
#include "FtfFinderDict.h"

#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TError.h"

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, FtfFinder *&obj)
{
   // Read a pointer to an object of class FtfFinder.

   ::Error("FtfFinder::operator>>", "objects not inheriting from TObject need a specialized operator>> function"); if (obj) { }
   return buf;
}

//______________________________________________________________________________
void FtfFinder::Streamer(TBuffer &R__b)
{
   // Stream an object of class FtfFinder.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      R__b >> nHits;
      R__b >> maxHits;
      R__b >> nTracks;
      para.Streamer(R__b);
      R__b >> maxTracks;
      R__b >> nMcTracks;
      R__b >> cpuTime;
      R__b >> realTime;
   } else {
      R__b.WriteVersion(FtfFinder::IsA());
      R__b << nHits;
      R__b << maxHits;
      R__b << nTracks;
      para.Streamer(R__b);
      R__b << maxTracks;
      R__b << nMcTracks;
      R__b << cpuTime;
      R__b << realTime;
   }
}

//______________________________________________________________________________
void FtfFinder::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class FtfFinder.

   TClass *R__cl  = FtfFinder::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "nHits", &nHits);
   R__insp.Inspect(R__cl, R__parent, "maxHits", &maxHits);
   R__insp.Inspect(R__cl, R__parent, "*hit", &hit);
   R__insp.Inspect(R__cl, R__parent, "nTracks", &nTracks);
   R__insp.Inspect(R__cl, R__parent, "*track", &track);
   para.ShowMembers(R__insp, strcat(R__parent,"para.")); R__parent[R__ncp] = 0;
   R__insp.Inspect(R__cl, R__parent, "maxTracks", &maxTracks);
   R__insp.Inspect(R__cl, R__parent, "nMcTracks", &nMcTracks);
   R__insp.Inspect(R__cl, R__parent, "*mcTrack", &mcTrack);
   R__insp.Inspect(R__cl, R__parent, "*volume", &volume);
   R__insp.Inspect(R__cl, R__parent, "*rowk", &rowk);
   R__insp.Inspect(R__cl, R__parent, "*trackArea", &trackArea);
   R__insp.Inspect(R__cl, R__parent, "cpuTime", &cpuTime);
   R__insp.Inspect(R__cl, R__parent, "realTime", &realTime);
   R__insp.Inspect(R__cl, R__parent, "*currentTrack", &currentTrack);
}

/********************************************************
* FtfFinderDict.cxx
********************************************************/

#ifdef G__MEMTEST
#undef malloc
#endif

extern "C" void G__cpp_reset_tagtableFtfFinderDict();

extern "C" void G__set_cpp_environmentFtfFinderDict() {
  G__add_compiledheader("TROOT.h");
  G__add_compiledheader("TMemberInspector.h");
  G__add_compiledheader("FtfFinder.h");
  G__cpp_reset_tagtableFtfFinderDict();
}
extern "C" int G__cpp_dllrevFtfFinderDict() { return(51111); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* FtfFinder */
static int G__FtfFinder_FtfFinder_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   FtfFinder *p=NULL;
   if(G__getaryconstruct()) p=new FtfFinder[G__getaryconstruct()];
   else                    p=new FtfFinder;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_dEdx_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfFinder*)(G__getstructoffset()))->dEdx();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_getTracks_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfFinder*)(G__getstructoffset()))->getTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_mergePrimaryTracks_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfFinder*)(G__getstructoffset()))->mergePrimaryTracks();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_process_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,100,(double)((FtfFinder*)(G__getstructoffset()))->process());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_reset_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfFinder*)(G__getstructoffset()))->reset());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_setConformalCoordinates_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfFinder*)(G__getstructoffset()))->setConformalCoordinates());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_setPointers_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfFinder*)(G__getstructoffset()))->setPointers());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_CpuTime_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,100,(double)((FtfFinder*)(G__getstructoffset()))->CpuTime());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_RealTime_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,100,(double)((FtfFinder*)(G__getstructoffset()))->RealTime());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((FtfFinder*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfFinder*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((FtfFinder*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfFinder*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((FtfFinder*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfFinder*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfFinder*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfFinder*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfFinder*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfFinder_Dictionary_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfFinder*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__FtfFinder_FtfFinder_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   FtfFinder *p;
   if(1!=libp->paran) ;
   p=new FtfFinder(*(FtfFinder*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__FtfFinder_wAFtfFinder_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (FtfFinder *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         delete (FtfFinder *)((G__getstructoffset())+sizeof(FtfFinder)*i);
   else  delete (FtfFinder *)(G__getstructoffset());
      G__setnull(result7);
   return(1 || funcname || hash || result7 || libp) ;
}


/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/* FtfFinder */

/*********************************************************
* Global function Stub
*********************************************************/

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncFtfFinderDict {
 public:
  G__Sizep2memfuncFtfFinderDict() {p=&G__Sizep2memfuncFtfFinderDict::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncFtfFinderDict::*p)();
};

size_t G__get_sizep2memfuncFtfFinderDict()
{
  G__Sizep2memfuncFtfFinderDict a;
  G__setsizep2memfunc((int)a.sizep2memfunc());
  return((size_t)a.sizep2memfunc());
}


/*********************************************************
* virtual base class offset calculation interface
*********************************************************/

   /* Setting up class inheritance */

/*********************************************************
* Inheritance information setup/
*********************************************************/
extern "C" void G__cpp_setup_inheritanceFtfFinderDict() {

   /* Setting up class inheritance */
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableFtfFinderDict() {

   /* Setting up typedef entry */
   G__search_typename2("Char_t",99,-1,0,
-1);
   G__setnewtype(-1,"Signed Character 1 byte",0);
   G__search_typename2("UChar_t",98,-1,0,
-1);
   G__setnewtype(-1,"Unsigned Character 1 byte",0);
   G__search_typename2("Short_t",115,-1,0,
-1);
   G__setnewtype(-1,"Signed Short integer 2 bytes",0);
   G__search_typename2("UShort_t",114,-1,0,
-1);
   G__setnewtype(-1,"Unsigned Short integer 2 bytes",0);
   G__search_typename2("Int_t",105,-1,0,
-1);
   G__setnewtype(-1,"Signed integer 4 bytes",0);
   G__search_typename2("UInt_t",104,-1,0,
-1);
   G__setnewtype(-1,"Unsigned integer 4 bytes",0);
   G__search_typename2("Seek_t",105,-1,0,
-1);
   G__setnewtype(-1,"File pointer",0);
   G__search_typename2("Long_t",108,-1,0,
-1);
   G__setnewtype(-1,"Signed long integer 8 bytes",0);
   G__search_typename2("ULong_t",107,-1,0,
-1);
   G__setnewtype(-1,"Unsigned long integer 8 bytes",0);
   G__search_typename2("Float_t",102,-1,0,
-1);
   G__setnewtype(-1,"Float 4 bytes",0);
   G__search_typename2("Double_t",100,-1,0,
-1);
   G__setnewtype(-1,"Float 8 bytes",0);
   G__search_typename2("Text_t",99,-1,0,
-1);
   G__setnewtype(-1,"General string",0);
   G__search_typename2("Bool_t",98,-1,0,
-1);
   G__setnewtype(-1,"Boolean (0=false, 1=true)",0);
   G__search_typename2("Byte_t",98,-1,0,
-1);
   G__setnewtype(-1,"Byte (8 bits)",0);
   G__search_typename2("Version_t",115,-1,0,
-1);
   G__setnewtype(-1,"Class version identifier",0);
   G__search_typename2("Option_t",99,-1,0,
-1);
   G__setnewtype(-1,"Option string",0);
   G__search_typename2("Ssiz_t",105,-1,0,
-1);
   G__setnewtype(-1,"String size",0);
   G__search_typename2("Real_t",102,-1,0,
-1);
   G__setnewtype(-1,"TVector and TMatrix element type",0);
   G__search_typename2("VoidFuncPtr_t",89,-1,0,
-1);
   G__setnewtype(-1,"pointer to void function",0);
   G__search_typename2("FreeHookFun_t",89,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("ReAllocFun_t",81,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("ReAllocCFun_t",81,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("Axis_t",102,-1,0,
-1);
   G__setnewtype(-1,"Axis values type",0);
   G__search_typename2("Stat_t",100,-1,0,
-1);
   G__setnewtype(-1,"Statistics type",0);
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */

   /* FtfFinder */
static void G__setup_memvarFtfFinder(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder));
   { FtfFinder *p; p=(FtfFinder*)0x1000; if (p) { }
   G__memvar_setup((void*)((long)(&p->nHits)-(long)(p)),105,0,0,-1,-1,-1,1,"nHits=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->maxHits)-(long)(p)),105,0,0,-1,-1,-1,1,"maxHits=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->hit)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_FtfHit),-1,-1,1,"hit=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->nTracks)-(long)(p)),105,0,0,-1,-1,-1,1,"nTracks=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->track)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_FtfTrack),-1,-1,1,"track=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->para)-(long)(p)),117,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_FtfPara),-1,-1,1,"para=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->maxTracks)-(long)(p)),105,0,0,-1,-1,-1,1,"maxTracks=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->nMcTracks)-(long)(p)),105,0,0,-1,-1,-1,1,"nMcTracks=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->mcTrack)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_FtfMcTrack),-1,-1,1,"mcTrack=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->volume)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_VOLUME),-1,-1,1,"volume=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->rowk)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_ROW),-1,-1,1,"rowk=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->trackArea)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_AREA),-1,-1,1,"trackArea=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->cpuTime)-(long)(p)),100,0,0,-1,-1,-1,1,"cpuTime=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->realTime)-(long)(p)),100,0,0,-1,-1,-1,1,"realTime=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_FtfTrack),-1,-1,4,"currentTrack=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__FtfFinderDictLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,108,0,0,-1,-1,-1,4,"G__virtualinfo=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}

extern "C" void G__cpp_setup_memvarFtfFinderDict() {
}
/***********************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
***********************************************************/

/*********************************************************
* Member function information setup for each class
*********************************************************/
static void G__setup_memfuncFtfFinder(void) {
   /* FtfFinder */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder));
   G__memfunc_setup("FtfFinder",888,G__FtfFinder_FtfFinder_0_0,105,G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("dEdx",389,G__FtfFinder_dEdx_1_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("getTracks",936,G__FtfFinder_getTracks_2_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("mergePrimaryTracks",1884,G__FtfFinder_mergePrimaryTracks_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("process",767,G__FtfFinder_process_4_0,100,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("reset",547,G__FtfFinder_reset_5_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("setConformalCoordinates",2408,G__FtfFinder_setConformalCoordinates_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("setPointers",1184,G__FtfFinder_setPointers_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CpuTime",695,G__FtfFinder_CpuTime_8_0,100,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RealTime",787,G__FtfFinder_RealTime_9_0,100,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__FtfFinder_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__FtfFinder_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__FtfFinder_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__FtfFinder_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__FtfFinder_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__FtfFinder_Class_5_1,85,G__get_linked_tagnum(&G__FtfFinderDictLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__FtfFinder_IsA_6_1,85,G__get_linked_tagnum(&G__FtfFinderDictLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__FtfFinder_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__FtfFinder_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Dictionary",1046,G__FtfFinder_Dictionary_9_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   // automatic copy constructor
   G__memfunc_setup("FtfFinder",888,G__FtfFinder_FtfFinder_0_2,(int)('i'),G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder),-1,0,1,1,1,0,"u 'FtfFinder' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~FtfFinder",1014,G__FtfFinder_wAFtfFinder_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncFtfFinderDict() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_globalFtfFinderDict() {

   /* Setting up global variables */
   G__resetplocal();


   G__resetglobalenv();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_funcFtfFinderDict() {
   G__lastifuncposition();


   G__resetifuncposition();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__FtfFinderDictLN_TClass = { "TClass" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_FtfPara = { "FtfPara" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_FtfTrack = { "FtfTrack" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_FtfHit = { "FtfHit" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_VOLUME = { "VOLUME" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_ROW = { "ROW" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_AREA = { "AREA" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_FtfFinder = { "FtfFinder" , 99 , -1 };
G__linked_taginfo G__FtfFinderDictLN_FtfMcTrack = { "FtfMcTrack" , 99 , -1 };

/* Reset class/struct taginfo */
extern "C" void G__cpp_reset_tagtableFtfFinderDict() {
  G__FtfFinderDictLN_TClass.tagnum = -1 ;
  G__FtfFinderDictLN_FtfPara.tagnum = -1 ;
  G__FtfFinderDictLN_FtfTrack.tagnum = -1 ;
  G__FtfFinderDictLN_FtfHit.tagnum = -1 ;
  G__FtfFinderDictLN_VOLUME.tagnum = -1 ;
  G__FtfFinderDictLN_ROW.tagnum = -1 ;
  G__FtfFinderDictLN_AREA.tagnum = -1 ;
  G__FtfFinderDictLN_FtfFinder.tagnum = -1 ;
  G__FtfFinderDictLN_FtfMcTrack.tagnum = -1 ;
}

extern "C" void G__cpp_setup_tagtableFtfFinderDict() {

   /* Setting up class,struct,union tag entry */
   G__tagtable_setup(G__get_linked_tagnum(&G__FtfFinderDictLN_FtfFinder),sizeof(FtfFinder),-1,0,(char*)NULL,G__setup_memvarFtfFinder,G__setup_memfuncFtfFinder);
}
extern "C" void G__cpp_setupFtfFinderDict() {
  G__check_setup_version(51111,"G__cpp_setupFtfFinderDict()");
  G__set_cpp_environmentFtfFinderDict();
  G__cpp_setup_tagtableFtfFinderDict();

  G__cpp_setup_inheritanceFtfFinderDict();

  G__cpp_setup_typetableFtfFinderDict();

  G__cpp_setup_memvarFtfFinderDict();

  G__cpp_setup_memfuncFtfFinderDict();
  G__cpp_setup_globalFtfFinderDict();
  G__cpp_setup_funcFtfFinderDict();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncFtfFinderDict();
  return;
}
class G__cpp_setup_initFtfFinderDict {
  public:
    G__cpp_setup_initFtfFinderDict() { G__add_setup_func("FtfFinderDict",&G__cpp_setupFtfFinderDict); }
   ~G__cpp_setup_initFtfFinderDict() { G__remove_setup_func("FtfFinderDict"); }
};
G__cpp_setup_initFtfFinderDict G__cpp_setup_initializerFtfFinderDict;

