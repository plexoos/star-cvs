/*!
 * \class StMaker
 *
 * StMaker virtual base class for Makers
 */

#ifndef STAR_StMaker
#define STAR_StMaker

#include <assert.h>
#include "Stypes.h"
#include "TDataSet.h"
#include "TDataSetIter.h"
#include "TObjectSet.h"
#include <TString.h>
#include <TDatime.h>
#include <TH1.h>
#include "StEvtHddr.h"
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#include <TStopwatch.h>

typedef TDataSet::EDataSetPass EDataSetPass;

class TList;
class TBrowser;
class TChain;
class TTree;
class TTable;
class TMemStat;

class StMaker : public TDataSet{
public:
   typedef  enum {kNormal, kDebug} EDebugLevel;
   enum {kSTAFCV_BAD, kSTAFCV_OK, kSTAFCV_ERR=2, kSTAFCV_FATAL=3} EModule_return_Status;
   enum {kInitBeg = BIT(1), kInitEnd = BIT(2)
        ,kMakeBeg = BIT(3), kCleaBeg = BIT(4)
        ,kFiniBeg = BIT(5), kFiniEnd = BIT(6)
        ,kActive  = BIT(7)};

protected:

   TDataSet     *m_DataSet;		//!  
   TDataSet     *m_ConstSet;		//!  
   TDataSet     *m_GarbSet;		//!  
   TDataSet     *m_Inputs;	 	//!list of logInput:ActualInput
   TDataSet     *m_Ouputs;	 	//!list of logOuput:ActualOuput
   TDataSet     *m_Runco;	 	//!Run Control parameters
   TList          *m_Histograms;	//!list of Histograms
   static StMaker *fgStChain;     	//!current pointer to StChain
   static StMaker *fgFailedMaker;     	//!current pointer to failed maker
   static Int_t fgTallyMaker[kStFatal+1];//!counters
   Int_t	   m_Mode;		//!Integer mode of maker
   Int_t           m_Number;        	//!Serial event number
   Int_t           m_LastRun;        	//!Last Run number
   Int_t           m_DebugLevel;    	//!Debug level
   Int_t           m_MakeReturn;    	//!Make() return flag
   TStopwatch      m_Timer;             //!Timer object
   TMemStat       *fMemStatMake;	//!TMemStat for Make
   TMemStat       *fMemStatClear;	//!TMemStat for Clear

public:

   /// Constructor & Destructor

                  	StMaker(const char *name="",const char *dummy=0);
   virtual       	~StMaker();
   virtual Int_t IsChain() const {return 0;}


   /// User defined functions
   virtual void   	Clear(Option_t *option="");
   virtual Int_t  	InitRun(int runumber);
   virtual Int_t  	Init();
   virtual void   	StartMaker();
   virtual Int_t  	Make();
   virtual Int_t  	IMake(int number){SetNumber(number);return Make();};
   virtual void   	EndMaker  (int ierr);
   virtual Int_t  	Finish();
   virtual Int_t  	FinishRun(int oldrunumber);
   virtual void	       	FatalErr(int Ierr, const char *Com);  
   virtual void   	PrintInfo();
   virtual void         NotifyMe(const char *about,const void *ptr){;}
   virtual void         AddMaker (StMaker *mk);

   virtual void   MakeDoc(const TString &stardir="$(STAR)",const TString &outdir="$(STAR)/StRoot/html",Bool_t baseClasses=kTRUE); 

   ///	User methods
   virtual TDataSet   *AddData (TDataSet *data=0,const char *dir=".data");
   virtual TObjectSet *AddObj  (TObject *obj,const char *dir);
   virtual void        	AddConst(TDataSet *data=0){AddData(data,".const");}
   virtual void        	AddHist(TH1 *h,const char *dir=0);
   virtual void        	AddGarb (TDataSet *data=0){AddData(data,".garb");};
   virtual void        	AddRunco (TDataSet *data=0){AddData(data,".runco");};
   virtual void        	AddRunco (double par,const char* name,const char* comment);
           void        	AddRunCont (TDataSet *data=0){AddRunco(data);};	//alias
   virtual TList       *GetHistList() const {return (TList*)GetDirObj(".hist");};
   virtual TH1         *GetHist(const Char_t *histName) const {TList *l=GetHistList(); return l?(TH1*)l->FindObject(histName):(TH1*)0;};
   virtual StMaker     *cd(){StMaker *ret = fgStChain; fgStChain=this; return ret;};
   virtual StMaker     *Cd(){return cd();};
   static  StMaker     *New(const Char_t *classname, const Char_t *name="", void *title=0);


   /// STAR methods
   virtual Int_t  	GetNumber() const ;
   virtual void   	SetNumber(Int_t number) ;
   static  StMaker     *GetChain(){return fgStChain;}
   static  StMaker     *GetFailedMaker(){return fgFailedMaker;}
   virtual StMaker     *GetParentChain() const;
   virtual Int_t        GetIventNumber() const ;
   virtual void         SetIventNumber(Int_t iv);
   virtual Int_t        GetEventNumber() const ;
   virtual Int_t        GetRunNumber() const ;
   virtual TDatime      GetDateTime() const;
   virtual Int_t     	GetDate()  const ;
   virtual Int_t     	GetTime()  const ;
   virtual const Char_t *GetEventType() const ;


   // Get methods
   virtual TDataSet  *GetData(const char *name, const char* dir=".data") const;
   virtual TDataSet  *GetDataSet (const char* logInput) const {return FindDataSet(logInput);}
   virtual TDataSet  *   DataSet (const char* logInput)   const 
                           {return GetDataSet(logInput);};
   virtual TDataSet  *GetInputDS (const char* logInput)   const 
                           {return GetDataSet(logInput);};

   virtual TDataSet  *GetDataBase(const char* logInput);
   virtual TDataSet  *GetInputDB (const char* logInput)
                          {return GetDataBase(logInput);};
   virtual Int_t   GetValidity(const TTable *tb, TDatime *val) const;


   virtual Int_t 	GetDebug() const {return m_DebugLevel;}
   virtual Int_t 	   Debug() const {return GetDebug();};
   virtual Int_t 	GetMakeReturn() const {return m_MakeReturn;}
   virtual TList       *Histograms()  const {return GetHistList();}
   virtual TString      GetAlias (const char* log, const char* dir=".aliases") const ;
   virtual TString      GetInput (const char* log) const {return GetAlias(log);};
   virtual TString      GetOutput(const char* log) const {return GetAlias(log,".aliases");};
   virtual TList       *GetMakeList() const ;
   virtual StMaker     *GetParentMaker () const;
   virtual StMaker     *GetMaker (const char *mkname);
   virtual Bool_t       IsActive() {return TestBit(kActive);}
   virtual StMaker     *Maker (const char *mkname){return GetMaker (mkname);};


   /// Setters for flags and switches
   virtual void         SetActive(Bool_t k=kTRUE) {if(k) {SetBit(kActive);} else {ResetBit(kActive);}} 
   virtual void        	SetDebug(Int_t l=1){m_DebugLevel = l;}   // *MENU*
   virtual void        	SetDEBUG(Int_t l=1);                     // *MENU*
   virtual void         SetFlavor(const char *flav,const char *tabname);  //Set DB Flavor
   virtual void         SetMakeReturn(Int_t ret){m_MakeReturn=ret;}  
   virtual void       	SetAlias(const char* log,const char* act,const char* dir=".aliases");
   virtual void       	AddAlias(const char* log,const char* act,const char* dir=".aliases");
   virtual void       	SetInput(const char* log,const char* act){SetAlias(log,act);};
   virtual void       	SetOutput(const char* log,const char* act){SetAlias(log,act,".aliases");};
   virtual void       	SetOutput(const char* log,TDataSet *ds);
   virtual void       	SetOutput(TDataSet *ds){SetOutput(0,ds);};
   virtual void       	SetOutputAll(TDataSet *ds,Int_t level=1);
   virtual void   	SetMode(Int_t mode=0)   {m_Mode=mode;}   // *MENU*
   virtual void         SetNotify(const char *about,StMaker *mk);

   virtual void         NotifyEm(const char *about,const void *ptr);

   virtual Double_t     RealTime(){ return m_Timer.RealTime();}
   virtual Double_t     CpuTime() { return m_Timer.CpuTime();}
   virtual void   	StartTimer(Bool_t reset = kFALSE){m_Timer.Start(reset);}
   virtual void   	StopTimer(){m_Timer.Stop();}
   virtual void   	PrintTimer(Option_t *option="");
   virtual void         PrintTotalTime(){}

   /// Static functions
   static  StMaker     *GetMaker(const TDataSet *ds)  ;
   static EDataSetPass  ClearDS (TDataSet* ds,void *user );


TObject        *GetDirObj(const char *dir) const;
void            SetDirObj(TObject *obj,const char *dir);


  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StMaker.h,v 1.59 2003/04/30 20:36:24 perev Exp $ built "__DATE__" "__TIME__ ; return cvs;}
protected:
   virtual TDataSet  *FindDataSet (const char* logInput,
                                    const StMaker *uppMk=0,
                                    const StMaker *dowMk=0) const ;



   ClassDef(StMaker, 0)   // base class to define  one step of the recontsruction chain
};

class StMakerIter 
{
public:
  StMakerIter(StMaker *mk, Int_t second = 0);
 ~StMakerIter();
  StMaker *NextMaker();
  StMaker *GetMaker () const {return fMaker;}
private:
  Int_t fState;			//!
  Int_t fSecond;		//!
  StMaker *fMaker;		//!
  StMakerIter *fMakerIter;	//!
  TDataSet *fItWas;		//!
  TDataSetIter *fIter;		//!
};  
#endif


// $Id: StMaker.h,v 1.59 2003/04/30 20:36:24 perev Exp $
// $Log: StMaker.h,v $
// Revision 1.59  2003/04/30 20:36:24  perev
// Warnings cleanup. Modified lines marked VP
//
// Revision 1.58  2002/04/14 21:51:12  perev
// Obsolete StBroadcast
//
// Revision 1.57  2002/02/23 00:02:49  perev
// NotyfyMe added
//
// Revision 1.56  2002/02/02 23:31:14  jeromel
// doxygenized. Added some text for the Make() method.
//
// Revision 1.55  2001/11/18 00:58:07  perev
// Broadcast method added
//
// Revision 1.54  2001/10/13 20:23:45  perev
// SetFlavor  working before and after Init()
//
// Revision 1.53  2001/08/14 16:42:48  perev
// InitRun call improved
//
// Revision 1.52  2001/06/01 02:47:31  perev
// Memory consumption measurement added
//
// Revision 1.51  2001/05/31 02:40:30  perev
// const(ing)
//
// Revision 1.50  2001/05/04 19:15:59  perev
// Fatal() -> FatalErr()
//
// Revision 1.49  2001/04/10 21:38:49  perev
// Maki(int) --> IMake(int)
//
// Revision 1.48  2000/07/04 02:36:01  perev
// AddMaker method added & gStChain removed
//
// Revision 1.47  2000/06/21 21:12:40  perev
// StMakerIter class added
//
// Revision 1.46  2000/05/20 01:11:07  perev
// IventNumber and BfcStatus added
//
// Revision 1.45  2000/04/13 02:53:35  perev
// StMaker::GetValidity added
//
// Revision 1.44  2000/04/03 23:46:49  perev
// Increased error check
//
// Revision 1.43  2000/03/23 00:15:22  fine
// Adjusted to libSTAR for ROOT 2.24
//
// Revision 1.42  2000/01/07 22:31:43  perev
// one more argument for SetOutputAll
//
// Revision 1.41  1999/12/06 01:57:30  fine
// Time statistic fixed
//
// Revision 1.39  1999/12/01 23:51:25  perev
// Alias AddRunco - AddRunCont
//
// Revision 1.38  1999/12/01 22:56:30  perev
// .runco directory & AddRunco method introduced
//
// Revision 1.37  1999/09/23 21:24:58  perev
// recovered debug level init(lost)
//
// Revision 1.36  1999/09/21 15:05:18  perev
// InitRun & FinishRun added
//
// Revision 1.35  1999/09/08 00:13:35  fisyak
// Add static *GetChain()
//
// Revision 1.34  1999/09/03 23:11:48  perev
// Add .runcont directory
//
// Revision 1.33  1999/09/02 22:27:12  fisyak
// Add SetDEBUG
//
// Revision 1.32  1999/08/13 01:12:25  fine
// StMaker::GetHist has been introduced
//
// Revision 1.31  1999/08/06 13:01:38  fisyak
// Add Active flag
//
// Revision 1.30  1999/07/29 01:05:23  fisyak
// move bfc to StBFChain
//
// Revision 1.29  1999/07/13 02:19:34  perev
// GetCVS,StEvtHddr,etc...
//
// Revision 1.28  1999/07/12 17:36:34  perev
// Spiros request to add error flags
//
// Revision 1.27  1999/07/12 02:27:10  perev
// GetCVS only
//
// Revision 1.26  1999/07/11 02:02:05  perev
// clash inside GetCVS resolved
//
// Revision 1.25  1999/07/11 01:59:04  perev
// add GetCVSTag again
//
// Revision 1.24  1999/07/09 22:00:22  perev
// GetCVS into StMaker
//
// Revision 1.23  1999/06/27 23:09:22  fisyak
// Add __DATE__ & __TIME__ to tag
//
// Revision 1.22  1999/05/23 04:05:03  fine
// The lost since 1.35 Wed Mar 10 20:23:58 timer functions have been re-introduced
//
// Revision 1.21  1999/05/10 15:37:52  perev
// Save of hisogramm in StMaker::Init
//
// Revision 1.20  1999/05/06 21:27:11  perev
// StMaker remove his from hdirectory
//
// Revision 1.19  1999/05/06 00:19:05  fine
// StMaker::MakeDoc method has been re-introduced for the 3d time
//
// Revision 1.18  1999/04/30 14:58:41  perev
//  cd() added to StMaker class
//
// Revision 1.17  1999/03/20 20:57:35  perev
// add StEvtHddr.h and fix Get/SetNumber in maker
//
// Revision 1.16  1999/03/19 20:30:50  perev
// GetCVSTag introduced
//
// Revision 1.15  1999/03/11 01:23:59  perev
// new schema StChain
//
// Revision 1.10  1998/12/21 19:42:51  fisyak
// Move ROOT includes to non system
//
// Revision 1.9  1998/11/19 01:23:57  fine
// StChain::MakeDoc has been introduced, StChain::MakeDoc has been fixed (see macros/bfc_doc.C macro
//
// Revision 1.8  1998/11/18 22:46:10  fine
// The lost MakeDoc method has been re-introduced
//
// Revision 1.7  1998/10/06 18:00:27  perev
// cleanup
//
// Revision 1.5  1998/08/18 14:05:02  fisyak
// Add to bfc dst
//
// Revision 1.4  1998/07/20 15:08:09  fisyak
// Add tcl and tpt
//
