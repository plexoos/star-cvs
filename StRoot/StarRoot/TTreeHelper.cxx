// Author: Victor Perev   08/04/01

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TTreeHelper                                                          //
//                                                                      //
// is a helper class for TTree.                          		//
// It is analysis tool to TTree.                                      	//
//  Functionality is similar to TTree::MakeClass()                      //
//  But:                                                                //
//  1. user do not need to create special class, TTreeHelper is         //
//     universal;                                                       //
//                                                                      //
//  2. AUTOMATICALLY, only branches wich user needs to use are READ.    //
//     For complicated TTree it is much faster.                         //
//                                                                      //
//                                                                      //
//  Example (see tutorials/tree4.C) :                                                            //
//    TFile f("t4.root");                                           	//
//    TTree* t4 = (TTree*)f.Get("t4");              			//
//    TTreeHelper TH(t4);                                               //
//                                                                      //
// init user variables							//
//    const Float_t  &temp = TH("fTemperature");   //temperature	//
//    const Int_t    &ntrk = TH("fTracks");        //size of clone array//
//    const Float_t* &pz   = TH("fTracks.fPz");	   //pz array     	//
//                                                                      //
//    TH1F *hz = new TH1F("PX","Px distr", 100,-.5,.5)        		//
//  //loop								//
//    while(TH.Next()) {                                                //
//                                                                      //
//      if (temp >100 ) continue;   // too hot                        	//
//      for (int itr=0; itr<ntrk; itr++) {hz->Fill( pz[itr] );}         //
//    }                                                                 //
//    TH.Reset(); //ready for next loop                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TROOT.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(3,00,0)
#define __OLDROOT__
#endif

#include "TFile.h"
#include "TKey.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#ifndef __OLDROOT__
#include "TBranchElement.h"
#include "TFriendElement.h"
#endif
#include "TLeaf.h"
#include "TTreeHelper.h"
#include "TList.h"
#include "TObjArray.h"
#include "TNamed.h"
#include "TSystem.h"
#include "TRegexp.h"
#include "assert.h"
#include "TError.h"

enum ETTH { kUnknown=0,
      kChar   =  1,  kShort   =  2,  kInt     =  3,  kLong    =  4,  kFloat = 5, kDouble  =  8,
      kUChar  = 11,  kUShort  = 12,  kUInt    = 13,  kULong   = 14};

const char* NTTH[] = {"Unknown"
,"Char_t"	,"Short_t"	,"Int_t"	,"Long_t"	,"Float_t"
,"Wrong"	,"Wrong"	,"Double_t"	,"Wrong_t"	,"Wrong_t"
,"UChar_t"	,"UShort_t"	,"UInt_t"       ,"ULong_t"	,0 
,"Wrong_t"      ,"Wrong_t"      ,"Wrong_t"      ,"Wrong_t"      ,"Wrong_t"      
,"Char_t*"	,"Short_t*"	,"Int_t*"	,"Long_*t"	,"Float_t*"
,"Wrong_t*"	,"Wrong_t*"	,"Double_t*"	,"Wrong_t*"	,"Wrong_t*"
,"UChar_t*"	,"UShort_t*"	,"UInt_t*"	,"ULong_t*"	,0};




//______________________________________________________________________________


void *TTreeHelperCast::Addr(Int_t outType)
{
  void *v = fV;
  if (fT != outType) {
     printf("*** Wrong cast:variable %s %s to %s IGNORED ***\n",
          TTreeHelper::TypeName(fT),fN,TTreeHelper::TypeName(outType));
     v = 0;
  }
  if (!v) fE[0]++;
  return v;
}

//______________________________________________________________________________
TTreeHelperCast::operator const Char_t		&()
{return *((const Char_t*)Addr(kChar));}
//______________________________________________________________________________
TTreeHelperCast::operator const Short_t 	&()
{return *((const Short_t*)Addr(kShort));}
//______________________________________________________________________________
TTreeHelperCast::operator const Int_t  		&()
{return *((const Int_t*)Addr(kInt));}
//______________________________________________________________________________
TTreeHelperCast::operator const Long_t  	&()
{return *((const Long_t*)Addr(kLong));}
//______________________________________________________________________________
TTreeHelperCast::operator const Float_t 	&()
{return *((const Float_t*)Addr(kFloat));}
//______________________________________________________________________________
TTreeHelperCast::operator const Double_t 	&()
{return *((const Double_t*)Addr(kDouble));}

//______________________________________________________________________________
TTreeHelperCast::operator const UChar_t 	&()
{return *((const UChar_t*)Addr(kUChar));}
//______________________________________________________________________________
TTreeHelperCast::operator const UShort_t 	&()
{return *((const UShort_t*)Addr(kUShort));}
//______________________________________________________________________________
TTreeHelperCast::operator const UInt_t  	&()
{return *((const UInt_t*)Addr(kUInt));}
//______________________________________________________________________________
TTreeHelperCast::operator const ULong_t  	&()
{return *((const ULong_t*)Addr(kULong));}

//______________________________________________________________________________
TTreeHelperCast::operator const Char_t	       *&()
{return *((const Char_t**)Addr(kChar+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const Short_t        *&()
{return *((const Short_t**)Addr(kShort+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const Int_t  	       *&()
{return *((const Int_t**)Addr(kInt+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const Long_t         *&()
{return *((const Long_t**)Addr(kLong+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const Float_t        *&()
{return *((const Float_t**)Addr(kFloat+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const Double_t       *&()
{return *((const Double_t**)Addr(kDouble+20));}

//______________________________________________________________________________
TTreeHelperCast::operator const UChar_t        *&()
{return *((const UChar_t**)Addr(kUChar+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const UShort_t       *&()
{return *((const UShort_t**)Addr(kUShort+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const UInt_t         *&()
{return *((const UInt_t**)Addr(kUInt+20));}
//______________________________________________________________________________
TTreeHelperCast::operator const ULong_t        *&()
{return *((const ULong_t**)Addr(kULong+20));}





//______________________________________________________________________________
class TTreeHelperMem  : public TNamed { //special class for TTreeHelper
public:
   Int_t  fType;
   Int_t  fUnits;
   Int_t  fSize;
   Char_t *fMem;

public:
   TTreeHelperMem(const char *name,Int_t type,Int_t units);
  ~TTreeHelperMem(){ delete [] fMem;}
   void 	**Alloc(int units=-1);
   void 	**GetMem(){return (void**)&fMem;}   
};
//______________________________________________________________________________
TTreeHelperMem::TTreeHelperMem(const char *name,Int_t type,Int_t units)
                :TNamed(name,"")
{
  fType  = type;
  fUnits = units;
  fSize  = 0;
  fMem   = 0;
  Alloc();
}

//______________________________________________________________________________
void **TTreeHelperMem::Alloc(int units)
{
   if (units>-1) fUnits = units;
   if (!fUnits) fUnits=1;
   delete [] fMem;
   fSize = TTreeHelper::TypeSize(fType)*fUnits;
   fMem = new char[fSize+8];
   memset(fMem,0,fSize);
   strcpy(fMem+fSize,"Perev");
   return (void**)&fMem;
} 

//______________________________________________________________________________
ClassImp(TTreeHelper)
//______________________________________________________________________________

TTreeHelper::TTreeHelper(TTree *tree):fCast(&fNErr)
{
  fTree  = tree;
  Init();
}
//______________________________________________________________________________

TTreeHelper::TTreeHelper(const char *treeName):fCast(&fNErr)
{
  fTree = 0;
  if (treeName && treeName[0] && treeName[0]!=' ') fTree  = new TChain(treeName);
  Init();
}
//______________________________________________________________________________

void TTreeHelper::Init()
{
  fCint  = 0;
  fNErr  = 0;
  fEntry = 0;
  fUnits = 0;
  fChain = 0;
  fTreeNumb = 0;
  if (fTree==0) return;
  if (fTree->IsA()==TChain::Class()) fChain=(TChain*)fTree;
#ifndef __OLDROOT__
  fTree->SetMakeClass(1);
#endif
  fTree->SetBranchStatus("*",0);
  if (fTree->IsA()==TChain::Class()) 
    ((TChain*)fTree)->SetNotify(this);

}
//______________________________________________________________________________
TTreeHelper::~TTreeHelper()
{
  fEntry = 0;
  fTree  = 0;
  fMemList.Delete();
  
}

//______________________________________________________________________________
void TTreeHelper::GetInfo(const TBranch *tbp, const char *&tyName
                        ,Int_t &units,void  *&add, Int_t &brType) 
{
   brType =0;
   TBranch *tb = (TBranch*)tbp;
   add = tb->GetAddress();
   units = 0;
   char *nxt=0;
   const char *des = strchr(tb->GetName(),'[');
   if (des) {
     units = 1;
     while(des){
       int ii = strtol(des+1,&nxt,10);
       if (des+1 != nxt) 	units *=ii;
       if ( !*nxt) 		break;
       des = nxt;
   } }

   int max = 0;
#ifndef __OLDROOT__
   if (tb->IsA()==TBranchElement::Class()) {
     TBranchElement *te = (TBranchElement*)tb;
     max = te->GetMaximum();
     brType  = te->GetType();
   } 
     else 
#endif
   {

     TLeaf *lf = (TLeaf*)tb->GetListOfLeaves()->First();
     TLeaf *lc = 0;
     if (lf) lc = lf->GetLeafCount();
     if (lc) max = lc->GetMaximum();
   }
   if (max) {if (!units) units = 1; units *= max;}
   if (brType==3) units=0;
   




   TObjArray *lfList = tb->GetListOfLeaves();
   TLeaf *tl = (lfList) ? (TLeaf*)lfList->UncheckedAt(0):0;
   tyName= (tl) ? tl->GetTypeName():0;
}
//______________________________________________________________________________
TBranch *TTreeHelper::GetBranch(const char* brName,TTree *tree)
{
  TBranch *branch=0;
  if (!tree) return 0;
  branch = GetBranch(brName,tree->GetListOfBranches(),0);
  if (branch) return branch;
  branch = GetBranch(brName,tree->GetListOfLeaves()  ,1);
  if (branch) return branch;
#ifndef __OLDROOT__
  branch = GetBranch(brName,tree->GetListOfFriends() ,2);
#endif
  return branch;
}
//______________________________________________________________________________
TBranch *TTreeHelper::GetBranch(const char* brName,TSeqCollection *brList,Int_t flag)
{
   TBranch *branch;
   TTree   *tree;
   TObject *to;
   int otLen = strcspn(brName,"[ " );
   int otDot = strcspn(brName,"[ .");
   int otDOT = (otDot != otLen);
   TIter next(brList);
   while((to=next())) {
      switch(flag) {
        case 0: branch = (TBranch*)to; 			break;
        case 1: branch = ((TLeaf*)to)->GetBranch();	break;
#ifndef __OLDROOT__
        case 2: tree = ((TFriendElement*)to)->GetTree();
                branch = GetBranch(brName,tree);
                if (branch) return branch;		break;
#endif
      }
      int inLen = strcspn(branch->GetName(),"[ ");
      int inDot = strcspn(branch->GetName(),"[ .");
      int inDOT = (inDot != inLen);
      if (inDOT==otDOT) {	// both same complexity
        if (otLen == inLen 
        && strncmp(brName,branch->GetName(),inLen)==0) 	return branch;
      } else if (inDOT) {	// in more complex
        if (otLen == inLen-inDot-1 
        && strncmp(brName,branch->GetName()+inDot+1,otLen)==0) 
        						return branch;
      }   
     
      branch = GetBranch(brName,branch->GetListOfBranches(),0);
      if (branch)  					return branch;
   }
   return 0;
}
//______________________________________________________________________________
TTreeHelperCast &TTreeHelper::operator() (const TString varname)
{
   fCint = 1;
   return operator() (varname.Data());
}
//______________________________________________________________________________
TTreeHelperCast &TTreeHelper::operator() (const char *varname)
{
   fCast.Set(0,0,varname);
//   TBranch *br = GetBranch(varname);
   TBranch *br = fTree->GetBranch(varname);
   if (!br) {
     Warning("operator()","Branch %s NOT FOUND",varname);
     return fCast;
   }
   void *addr,**pddr;
   const char *tyName;
   Int_t brType;
   GetInfo(br,tyName,fUnits,addr,brType);  

   int tyCode = TypeCode(tyName);
   if (!tyCode) {
     Warning("operator()","Branch %s of UNKNOWN %s type",varname,tyName);
     return fCast;
   }
   TTreeHelperMem *mem;
   mem = (TTreeHelperMem*)fMemList.FindObject(br->GetName());
   if (!mem) {
     mem = new TTreeHelperMem(br->GetName(),tyCode,fUnits);
     fMemList.Add(mem);
     pddr = mem->GetMem();
     fTree->SetBranchAddress(br->GetName(),*pddr);
     br->ResetBit  (kDoNotProcess);
     br->SetBit(1);
     fBraList.Add(br);
   } else {
     pddr = mem->GetMem();
   }
   addr = *pddr;
   if (fUnits) { tyCode+=20; addr = (void*)pddr;}
   fCast.Set(addr,tyCode,varname);

   if (fCint)  {
     fCint = 0;
     TTreeHelperCast *v =(TTreeHelperCast*)addr;
     return *v;//CINT workaround
   }
   return fCast;
}     
//______________________________________________________________________________
Int_t TTreeHelper::Next(Int_t entry)
{
  if (fNErr) {
    Warning("Next","It was %d errors in Init. Loop ignored",fNErr);
    fEntry=0; return 0;}

  int ientry,jentry;
  ientry = jentry = (entry >= 0) ? entry:fEntry++;
  if (fChain) {
    ientry = fChain->LoadTree(jentry);
    if (ientry<0) return 0;
    if (fTreeNumb != fChain->GetTreeNumber()) {
      fTreeNumb = fChain->GetTreeNumber();
      Notify();
  } }

  Int_t ans = 0;
  if (ientry==0) {
//		1st time standard read
    ans = fTree->GetEntry(jentry);
  }  else	 {
//   		more fast read all other times
    int n = fBraList.GetEntriesFast();
    for (int i=0;i<n;i++) {
      TBranch *b = (TBranch*)fBraList.UncheckedAt(i);
      ans +=b->GetEntry(ientry); 
  } }

  Assert(!IsCorrupted());
  if (ans) return ans;
  fEntry=0;
  return 0;

}
//______________________________________________________________________________
Bool_t TTreeHelper::Notify()
{
  const char *tyName;
  Int_t units,brType;
  void  *add;
  Assert(!IsCorrupted());
  fBraList.Clear();
  int n = fMemList.GetEntriesFast();
  for (int i=0;i<n;i++) {
    TTreeHelperMem *t = (TTreeHelperMem*)fMemList.UncheckedAt(i);
//VP    TBranch *b = GetBranch(t->GetName());
    TBranch *b = fTree->GetBranch(t->GetName());
    Assert(b);
    b->ResetBit  (kDoNotProcess);
    Assert(!b->TestBit(1));
    b->SetBit(1);
    GetInfo(b,tyName,units,add,brType);
    if (units > t->fUnits) {
      void **pddr = t->Alloc(units);
      fTree->SetBranchAddress(b->GetName(),*pddr);
    }
    fBraList.Add(b);
  }
  fTreeNumb = fChain->GetTreeNumber();
  return 0;
}
//______________________________________________________________________________
const char *TTreeHelper::IsCorrupted() const
{

  int n = fMemList.GetEntriesFast();
  for (int i=0;i<n;i++) {
    TTreeHelperMem *t = (TTreeHelperMem*)fMemList.UncheckedAt(i);
    char *perev = t->fMem+t->fSize;
    if (strcmp(perev,"Perev") ==0 ) continue;
    Error("IsCorrupted","Branch=%s Units=%d Mem=%p ***\n",t->GetName(),fUnits,perev);
    return t->GetName();
  }
  return 0;
}
//______________________________________________________________________________
void TTreeHelper::ls(const TObjArray *brList,Int_t lvl,Option_t* option)
{
   TBranch *branch;
   if (!brList) return;
   
   Int_t nb = brList->GetEntriesFast();
   for (int iBr=0;iBr<nb;iBr++) {
      branch = (TBranch*)brList->UncheckedAt(iBr);
      if (!branch)	continue;
      Print(branch,lvl);

      ls(branch->GetListOfBranches(),lvl+1,option);
   }
}
//______________________________________________________________________________
void TTreeHelper::ls(const TTree *ttp, Option_t* option)
{
  TTree *tt = (TTree *)ttp;
  if (!tt) return;
  ls(tt->GetListOfBranches(),0,option);
}
//______________________________________________________________________________
void TTreeHelper::ls(Option_t* option) const
{
   ls(fTree,option);
}
//______________________________________________________________________________
void TTreeHelper::Print(Option_t* option) const
{
   ls(fTree,option);
}
//______________________________________________________________________________
void TTreeHelper::Print(const TBranch *tb,Int_t lvl, Option_t* option)
{
   const char *tyName;
   Int_t units,brType;
   char active[2]={0,0};
   void  *add;

   GetInfo(tb,tyName,units,add,brType);
   active[0]='*';
   if (tb->TestBit(kDoNotProcess)) active[0]=0;
   
   printf("%10p(%10p)  -  ",tb,add);
   for (int i=0;i<lvl;i++){printf("    ");}
   
   printf("%s%s(%s)",active,tb->GetName(),tb->ClassName());
   
   printf("\t //  Max=%d Type=%s,brType=%d",units,tyName,brType);
   printf("\n");
}
//______________________________________________________________________________
const char* TTreeHelper::TypeName(Int_t ity)
{
  return NTTH[ity];
}
//______________________________________________________________________________
Int_t TTreeHelper::TypeSize(Int_t ity)
{
  int t = ity%10;
  switch(t) {
     case kChar:; case kShort:; return t;
     case kInt:   		return sizeof(Int_t);
     case kFloat: 		return sizeof(Float_t);
     case kDouble: 		return sizeof(Double_t);
     default: 			return 0;
  }
}

//______________________________________________________________________________
Int_t  TTreeHelper::TypeCode(const char *typeName)
{
   for (int i=1; NTTH[i]; i++) {if (strcmp(typeName,NTTH[i])==0) return i;}
   printf("*** TypeCode ERROR: %s is UNKNOWN ***\n",typeName);
   return 0;
} 
//______________________________________________________________________________
void TTreeHelper::Streamer(TBuffer &) {Assert(0);}
//_____________________________________________________________________________
Int_t TTreeHelper::AddFile(const Char_t *file)
{
  Int_t num = 0;
  TString tfile,tdir,tname,tbase,fullname;
  const char *name; char *cc;


  tfile = file;
  tdir  = gSystem->DirName(tfile);
  tbase = gSystem->BaseName(tfile);
  gSystem->ExpandPathName(tdir);


  void *dir = gSystem->OpenDirectory(tdir);
  if (!dir) {
    Warning("AddFile","*** IGNORED Directory %s does NOT exist ***\n",
    (const Char_t *)tdir);
    return 0;}

  while ((name = gSystem->GetDirEntry(dir))) {
//              skip some "special" names
    if (strcmp(name,"..")==0 || strcmp(name,".")==0) continue;
    tname = name;

    cc = gSystem->ConcatFileName(tdir,name);
    fullname = cc; delete [] cc;

    Long_t idqwe,sizeqwe,flags,modtimeqwe;
    gSystem->GetPathInfo(fullname,&idqwe,&sizeqwe,&flags,&modtimeqwe);
    if (flags&2 || !flags&4) continue;

//              prepare simple regular expression
    TRegexp rexp(tbase,kTRUE);
    int len=0;
    if (rexp.Index(tname,&len)!=0)      continue;
    if (len!=tname.Length())            continue;
    num++;
    printf("%02d -  TTreeHelper::AddFile %s\n",num,fullname.Data());
    if (fChain == 0) WhichTree(fullname);
    fChain->Add(fullname);
  }
  gSystem->FreeDirectory(dir);
  return num;
}
//______________________________________________________________________________
void TTreeHelper::WhichTree(const char *fileName)
{
   TString fileNameS = fileName;
   gSystem->ExpandPathName(fileNameS);
//   printf(" fileName = %s\n",fileNameS.Data());
   
   

   TFile *tfile = new TFile(fileNameS.Data());
   if (tfile->IsZombie()) {
     printf("*** Can NOT open %s ***\n",fileNameS.Data());
     return;}
   
   TList *keyList = tfile->GetListOfKeys();
   TListIter NextKey(keyList);
   TKey *key; const char *ttName=0;
   while ( (key = (TKey*)NextKey()) ) 
   { 
     if (strcmp("TTree"  ,key->GetClassName())!=0
     &&  strcmp("TNtuple",key->GetClassName())!=0) continue;
     ttName = key->GetName(); break;
   }  
   printf(" Got TTree = %s\n",ttName);
   if (ttName==0) return;

   fTree = new TChain(ttName);
   delete tfile;
   Init();
}



