//
// File generated by rootcint at Wed Oct  6 14:55:03 1999.
// Do NOT change. Changes will be lost next time file is generated
//
#include "FtfTrackDict.h"

#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TError.h"

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, FtfTrack *&obj)
{
   // Read a pointer to an object of class FtfTrack.

   ::Error("FtfTrack::operator>>", "objects not inheriting from TObject need a specialized operator>> function"); if (obj) { }
   return buf;
}

//______________________________________________________________________________
void FtfTrack::Streamer(TBuffer &R__b)
{
   // Stream an object of class FtfTrack.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      FtfBaseTrack::Streamer(R__b);
      R__b >> lastXyAngle;
      R__b >> s11Xy;
      R__b >> s12Xy;
      R__b >> s22Xy;
      R__b >> g1Xy;
      R__b >> g2Xy;
      R__b >> s11Sz;
      R__b >> s12Sz;
      R__b >> s22Sz;
      R__b >> g1Sz;
      R__b >> g2Sz;
      R__b >> ddXy;
      R__b >> a1Xy;
      R__b >> a2Xy;
      R__b >> ddSz;
      R__b >> a1Sz;
      R__b >> a2Sz;
      R__b >> trackLength;
   } else {
      R__b.WriteVersion(FtfTrack::IsA());
      FtfBaseTrack::Streamer(R__b);
      R__b << lastXyAngle;
      R__b << s11Xy;
      R__b << s12Xy;
      R__b << s22Xy;
      R__b << g1Xy;
      R__b << g2Xy;
      R__b << s11Sz;
      R__b << s12Sz;
      R__b << s22Sz;
      R__b << g1Sz;
      R__b << g2Sz;
      R__b << ddXy;
      R__b << a1Xy;
      R__b << a2Xy;
      R__b << ddSz;
      R__b << a1Sz;
      R__b << a2Sz;
      R__b << trackLength;
   }
}

//______________________________________________________________________________
void FtfTrack::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class FtfTrack.

   TClass *R__cl  = FtfTrack::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*nxatrk", &nxatrk);
   R__insp.Inspect(R__cl, R__parent, "lastXyAngle", &lastXyAngle);
   R__insp.Inspect(R__cl, R__parent, "*refHit", &refHit);
   R__insp.Inspect(R__cl, R__parent, "s11Xy", &s11Xy);
   R__insp.Inspect(R__cl, R__parent, "s12Xy", &s12Xy);
   R__insp.Inspect(R__cl, R__parent, "s22Xy", &s22Xy);
   R__insp.Inspect(R__cl, R__parent, "g1Xy", &g1Xy);
   R__insp.Inspect(R__cl, R__parent, "g2Xy", &g2Xy);
   R__insp.Inspect(R__cl, R__parent, "s11Sz", &s11Sz);
   R__insp.Inspect(R__cl, R__parent, "s12Sz", &s12Sz);
   R__insp.Inspect(R__cl, R__parent, "s22Sz", &s22Sz);
   R__insp.Inspect(R__cl, R__parent, "g1Sz", &g1Sz);
   R__insp.Inspect(R__cl, R__parent, "g2Sz", &g2Sz);
   R__insp.Inspect(R__cl, R__parent, "ddXy", &ddXy);
   R__insp.Inspect(R__cl, R__parent, "a1Xy", &a1Xy);
   R__insp.Inspect(R__cl, R__parent, "a2Xy", &a2Xy);
   R__insp.Inspect(R__cl, R__parent, "ddSz", &ddSz);
   R__insp.Inspect(R__cl, R__parent, "a1Sz", &a1Sz);
   R__insp.Inspect(R__cl, R__parent, "a2Sz", &a2Sz);
   R__insp.Inspect(R__cl, R__parent, "trackLength", &trackLength);
   FtfBaseTrack::ShowMembers(R__insp, R__parent);
}

/********************************************************
* FtfTrackDict.cxx
********************************************************/

#ifdef G__MEMTEST
#undef malloc
#endif

extern "C" void G__cpp_reset_tagtableFtfTrackDict();

extern "C" void G__set_cpp_environmentFtfTrackDict() {
  G__add_compiledheader("TROOT.h");
  G__add_compiledheader("TMemberInspector.h");
  G__add_compiledheader("FtfTrack.h");
  G__cpp_reset_tagtableFtfTrackDict();
}
extern "C" int G__cpp_dllrevFtfTrackDict() { return(51111); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* FtfTrack */
static int G__FtfTrack_add_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->add((FtfHit*)G__int(libp->para[0]),(int)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_add_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->add((FtfTrack*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_buildTrack_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->buildTrack((FtfHit*)G__int(libp->para[0]),(VOLUME*)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_dEdx_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->dEdx();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_deleteCandidate_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->deleteCandidate();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_fill_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->fill();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_fillPrimary_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->fillPrimary(libp->para[0].ref?*(double*)libp->para[0].ref:G__Mdouble(libp->para[0]),libp->para[1].ref?*(double*)libp->para[1].ref:G__Mdouble(libp->para[1])
,libp->para[2].ref?*(double*)libp->para[2].ref:G__Mdouble(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_fillSecondary_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->fillSecondary(libp->para[0].ref?*(double*)libp->para[0].ref:G__Mdouble(libp->para[0]),libp->para[1].ref?*(double*)libp->para[1].ref:G__Mdouble(libp->para[1])
,libp->para[2].ref?*(double*)libp->para[2].ref:G__Mdouble(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_follow_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->follow((VOLUME*)G__int(libp->para[0]),(int)G__int(libp->para[1])
,(int)G__int(libp->para[2])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_followHitSelection_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->followHitSelection((FtfHit*)G__int(libp->para[0]),(FtfHit*)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_mergePrimary_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->mergePrimary((AREA*)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_reset_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->reset();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_seekNextHit_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfTrack*)(G__getstructoffset()))->seekNextHit((VOLUME*)G__int(libp->para[0]),(FtfHit*)G__int(libp->para[1])
,(int)G__int(libp->para[2]),(int)G__int(libp->para[3])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_segment_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->segment((VOLUME*)G__int(libp->para[0]),(int)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_segmentHitSelection_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->segmentHitSelection((FtfHit*)G__int(libp->para[0]),(FtfHit*)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_nextHit_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->nextHit();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_DeclFileName_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((FtfTrack*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_DeclFileLine_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_ImplFileName_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((FtfTrack*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_ImplFileLine_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfTrack*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_Class_Version_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((FtfTrack*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_Class_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfTrack*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_IsA_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfTrack*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_ShowMembers_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_Streamer_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfTrack_Dictionary_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfTrack*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic default constructor
static int G__FtfTrack_FtfTrack_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   FtfTrack *p;
   if(0!=libp->paran) ;
   if(G__getaryconstruct()) p=new FtfTrack[G__getaryconstruct()];
   else                    p=new FtfTrack;
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__FtfTrack_FtfTrack_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   FtfTrack *p;
   if(1!=libp->paran) ;
   p=new FtfTrack(*(FtfTrack*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__FtfTrack_wAFtfTrack_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (FtfTrack *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         delete (FtfTrack *)((G__getstructoffset())+sizeof(FtfTrack)*i);
   else  delete (FtfTrack *)(G__getstructoffset());
      G__setnull(result7);
   return(1 || funcname || hash || result7 || libp) ;
}


/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/* FtfTrack */

/*********************************************************
* Global function Stub
*********************************************************/

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncFtfTrackDict {
 public:
  G__Sizep2memfuncFtfTrackDict() {p=&G__Sizep2memfuncFtfTrackDict::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncFtfTrackDict::*p)();
};

size_t G__get_sizep2memfuncFtfTrackDict()
{
  G__Sizep2memfuncFtfTrackDict a;
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
extern "C" void G__cpp_setup_inheritanceFtfTrackDict() {

   /* Setting up class inheritance */
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack))) {
     FtfTrack *G__Lderived;
     G__Lderived=(FtfTrack*)0x1000;
     {
       FtfBaseTrack *G__Lpbase=(FtfBaseTrack*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack),G__get_linked_tagnum(&G__FtfTrackDictLN_FtfBaseTrack),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
   }
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableFtfTrackDict() {

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

   /* FtfTrack */
static void G__setup_memvarFtfTrack(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack));
   { FtfTrack *p; p=(FtfTrack*)0x1000; if (p) { }
   G__memvar_setup((void*)((long)(&p->nxatrk)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack),-1,-1,1,"nxatrk=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->lastXyAngle)-(long)(p)),102,0,0,-1,-1,-1,1,"lastXyAngle=",0,"Angle in the xy plane of line connecting to last hits        ");
   G__memvar_setup((void*)((long)(&p->refHit)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfTrackDictLN_FtfHit),-1,-1,1,"refHit=",0,"Hit use as reference for secondary tracks");
   G__memvar_setup((void*)((long)(&p->s11Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"s11Xy=",0,"Fit Parameters");
   G__memvar_setup((void*)((long)(&p->s12Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"s12Xy=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->s22Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"s22Xy=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->g1Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"g1Xy=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->g2Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"g2Xy=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->s11Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"s11Sz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->s12Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"s12Sz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->s22Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"s22Sz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->g1Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"g1Sz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->g2Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"g2Sz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->ddXy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"ddXy=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->a1Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"a1Xy=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->a2Xy)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"a2Xy=",0,"fit par in xy */");
   G__memvar_setup((void*)((long)(&p->ddSz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"ddSz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->a1Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"a1Sz=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->a2Sz)-(long)(p)),100,0,0,-1,G__defined_typename("vfit"),-1,1,"a2Sz=",0,"fit par in sz */");
   G__memvar_setup((void*)((long)(&p->trackLength)-(long)(p)),102,0,0,-1,-1,-1,1,"trackLength=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__FtfTrackDictLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}

extern "C" void G__cpp_setup_memvarFtfTrackDict() {
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
static void G__setup_memfuncFtfTrack(void) {
   /* FtfTrack */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack));
   G__memfunc_setup("add",297,G__FtfTrack_add_0_0,121,-1,-1,0,2,1,1,0,
"U 'FtfHit' - 0 - thisHit i - - 0 - way",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("add",297,G__FtfTrack_add_1_0,121,-1,-1,0,1,1,1,0,"U 'FtfTrack' - 0 - thisTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("buildTrack",1029,G__FtfTrack_buildTrack_2_0,105,-1,-1,0,2,1,1,0,
"U 'FtfHit' - 0 - firstHit U 'VOLUME' - 0 - volume",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("dEdx",389,G__FtfTrack_dEdx_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("deleteCandidate",1520,G__FtfTrack_deleteCandidate_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("fill",423,G__FtfTrack_fill_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("fillPrimary",1163,G__FtfTrack_fillPrimary_6_0,121,-1,-1,0,3,1,1,0,
"d - - 1 - xc d - - 1 - yc "
"d - - 1 - rc",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("fillSecondary",1359,G__FtfTrack_fillSecondary_7_0,121,-1,-1,0,3,1,1,0,
"d - - 1 - xc d - - 1 - yc "
"d - - 1 - rc",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("follow",659,G__FtfTrack_follow_8_0,105,-1,-1,0,3,1,1,0,
"U 'VOLUME' - 0 - volume i - - 0 - way "
"i - - 0 - rowToStop",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("followHitSelection",1886,G__FtfTrack_followHitSelection_9_0,105,-1,-1,0,2,1,1,0,
"U 'FtfHit' - 0 - baseHit U 'FtfHit' - 0 - candidateHit",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("mergePrimary",1268,G__FtfTrack_mergePrimary_0_1,105,-1,-1,0,1,1,1,0,"U 'AREA' - 0 - trackArea",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("reset",547,G__FtfTrack_reset_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("seekNextHit",1132,G__FtfTrack_seekNextHit_2_1,85,G__get_linked_tagnum(&G__FtfTrackDictLN_FtfHit),-1,0,4,1,1,0,
"U 'VOLUME' - 0 - volume U 'FtfHit' - 0 - baseHit "
"i - - 0 - nradiusSteps i - - 0 - whichFunction",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("segment",755,G__FtfTrack_segment_3_1,105,-1,-1,0,2,1,1,0,
"U 'VOLUME' - 0 - volume i - - 0 - way",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("segmentHitSelection",1982,G__FtfTrack_segmentHitSelection_4_1,105,-1,-1,0,2,1,1,0,
"U 'FtfHit' - 0 - baseHit U 'FtfHit' - 0 - candidateHit",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("nextHit",740,G__FtfTrack_nextHit_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__FtfTrack_DeclFileName_6_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__FtfTrack_DeclFileLine_7_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__FtfTrack_ImplFileName_8_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__FtfTrack_ImplFileLine_9_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__FtfTrack_Class_Version_0_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__FtfTrack_Class_1_2,85,G__get_linked_tagnum(&G__FtfTrackDictLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__FtfTrack_IsA_2_2,85,G__get_linked_tagnum(&G__FtfTrackDictLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__FtfTrack_ShowMembers_3_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__FtfTrack_Streamer_4_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Dictionary",1046,G__FtfTrack_Dictionary_5_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   // automatic default constructor
   G__memfunc_setup("FtfTrack",789,G__FtfTrack_FtfTrack_6_2,(int)('i'),G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   // automatic copy constructor
   G__memfunc_setup("FtfTrack",789,G__FtfTrack_FtfTrack_7_2,(int)('i'),G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack),-1,0,1,1,1,0,"u 'FtfTrack' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~FtfTrack",915,G__FtfTrack_wAFtfTrack_8_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncFtfTrackDict() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_globalFtfTrackDict() {

   /* Setting up global variables */
   G__resetplocal();


   G__resetglobalenv();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_funcFtfTrackDict() {
   G__lastifuncposition();


   G__resetifuncposition();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__FtfTrackDictLN_TClass = { "TClass" , 99 , -1 };
G__linked_taginfo G__FtfTrackDictLN_FtfBaseTrack = { "FtfBaseTrack" , 99 , -1 };
G__linked_taginfo G__FtfTrackDictLN_FtfTrack = { "FtfTrack" , 99 , -1 };
G__linked_taginfo G__FtfTrackDictLN_FtfHit = { "FtfHit" , 99 , -1 };

/* Reset class/struct taginfo */
extern "C" void G__cpp_reset_tagtableFtfTrackDict() {
  G__FtfTrackDictLN_TClass.tagnum = -1 ;
  G__FtfTrackDictLN_FtfBaseTrack.tagnum = -1 ;
  G__FtfTrackDictLN_FtfTrack.tagnum = -1 ;
  G__FtfTrackDictLN_FtfHit.tagnum = -1 ;
}

extern "C" void G__cpp_setup_tagtableFtfTrackDict() {

   /* Setting up class,struct,union tag entry */
   G__tagtable_setup(G__get_linked_tagnum(&G__FtfTrackDictLN_FtfTrack),sizeof(FtfTrack),-1,0,(char*)NULL,G__setup_memvarFtfTrack,G__setup_memfuncFtfTrack);
}
extern "C" void G__cpp_setupFtfTrackDict() {
  G__check_setup_version(51111,"G__cpp_setupFtfTrackDict()");
  G__set_cpp_environmentFtfTrackDict();
  G__cpp_setup_tagtableFtfTrackDict();

  G__cpp_setup_inheritanceFtfTrackDict();

  G__cpp_setup_typetableFtfTrackDict();

  G__cpp_setup_memvarFtfTrackDict();

  G__cpp_setup_memfuncFtfTrackDict();
  G__cpp_setup_globalFtfTrackDict();
  G__cpp_setup_funcFtfTrackDict();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncFtfTrackDict();
  return;
}
class G__cpp_setup_initFtfTrackDict {
  public:
    G__cpp_setup_initFtfTrackDict() { G__add_setup_func("FtfTrackDict",&G__cpp_setupFtfTrackDict); }
   ~G__cpp_setup_initFtfTrackDict() { G__remove_setup_func("FtfTrackDict"); }
};
G__cpp_setup_initFtfTrackDict G__cpp_setup_initializerFtfTrackDict;

