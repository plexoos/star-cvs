// $Id: St_xdfin_Maker.cxx,v 1.36 2003/09/02 17:59:33 perev Exp $
// $Log: St_xdfin_Maker.cxx,v $
// Revision 1.36  2003/09/02 17:59:33  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.35  2001/01/03 23:09:27  perev
// Skip over EOF added
//
// Revision 1.34  2000/03/22 22:46:56  perev
// Printout of I/O actions
//
// Revision 1.33  2000/01/12 16:38:54  fisyak
// Add Id and Log
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_xdfin_Maker class for Makers                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <Stiostream.h>
#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_xdfin_Maker.h"
#include "St_XDFFile.h"

ClassImp(St_xdfin_Maker)

//_____________________________________________________________________________
  St_xdfin_Maker::St_xdfin_Maker(const char *name, const char *inputFile):
    StIOInterFace(name),
    m_InitDone(0)
{
  if (inputFile && inputFile[0]) SetFile(inputFile);
  fEvtHddr = (StEvtHddr*)GetDataSet("EvtHddr");
  if (!fEvtHddr) {
    fEvtHddr = new StEvtHddr(m_ConstSet);
    SetOutput(fEvtHddr);	//Declare this "EvtHddr" for output
  }
}
//_____________________________________________________________________________
St_xdfin_Maker::~St_xdfin_Maker(){
}
//_____________________________________________________________________________
Int_t St_xdfin_Maker::Init()
{
// 		Get run parameters from input file
  return Open();  
}
//_____________________________________________________________________________
Int_t St_xdfin_Maker::Open(const char*)
{
  if (m_InitDone) return 0;
  int iret = 0;
  m_InitDone=0;
  printf("*** St_xdfin_Maker::Init:  Open Input file %s ***\n",GetFile());
  iret = fXdfin.OpenXDF(GetFile(),"r");
  if (iret) return iret;
//  while (!m_InitDone && !iret) iret = Make();
  return 0;
}
//_____________________________________________________________________________
void St_xdfin_Maker::Close(Option_t *)
{
  Clear();
  m_ConstSet->Delete();
  m_InitDone=0;
  fXdfin.CloseXDF();
}
//_____________________________________________________________________________
Int_t St_xdfin_Maker::Make(){
  const char Skip[] = "ROSIE_RESET SLOW_CONTROL";
  const char *Patern[] = {
    	"dst"		,"event/data/global"	,0,
    	"run"		,0       		,"const",
	"Run"		,"run/geant"		,"const",  				
	"params"	,"run"			,"const", 
	"BEGIN_RUN"	,0			,"const",
	"Event"		,"event/geant"		,0,
	"event"		,0			,0,
	"TPC_DATA"	,0			,0,
	0};

  TString dsname;
  const char *mkdir = 0;
  St_DataSet *curdir = 0, *set = 0;
  Bool_t      CONST;
//  PrintInfo();
  fNIO++;
  St_DataSetIter local(m_DataSet);
  St_DataSetIter cons(m_ConstSet);

  while((set = fXdfin.ReadEvent())) {
    local.Reset();
    cons.Reset();
    dsname = set->GetName();

    if (strstr(Skip,dsname)) {//	Skip dataset
      printf("%s: Record %s is SKIPPED\n",GetName(),(const char*)dsname);
      delete set; continue;
    }
      
    mkdir = 0;
    CONST = kFALSE;
    for (int ipat=0; Patern[ipat]; ipat+=3)
    {
      if (strcmp(dsname,Patern[ipat])) continue;
      mkdir = Patern[ipat+1];
      CONST = Patern[ipat+2]!=0;
      break;
    }

    if (!strcmp("dst",dsname)) {
      CONST = (!set->Find("globtrk")) ;
      if (CONST) mkdir = "run";}

    St_DataSetIter *dsit = (CONST) ? &cons : &local;
    if (mkdir) {dsit->Mkdir(mkdir); dsit->Cd(mkdir);}
    curdir = dsit->Mkdir(dsname);
    
    if (GetDebug()) set->ls(1);
    curdir->Update(set);
    curdir->Purge();     if(GetDebug()) curdir->ls(99);
    if (!CONST && !strcmp("dst",dsname)) SetOutput("dst",curdir);
    delete set;
    if (!CONST) return kStOK;
  }
  return kStEOF;
}
//_____________________________________________________________________________
//_____________________________________________________________________________
Int_t St_xdfin_Maker::Skip(Int_t Nskip){
  St_DataSet *set;
  for (int i=0;Nskip; Nskip--,i++){
    if (GetDebug()) printf("St_xdfin_Maker skip record %i\n",i+1); 
    set = fXdfin.ReadEvent();
    if (!set) break;
    SafeDelete (set);
  }
  return Nskip;
}
