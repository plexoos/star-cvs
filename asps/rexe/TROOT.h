 
#ifndef ROOT_TROOT
#define ROOT_TROOT
 
// Copyright (C) 1994 CodeCERN. All rights reserved.
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TROOT                                                                //
//                                                                      //
// The TROOT object is the entry point to the system.                   //
// The single instance of TROOT is accessable via the global gROOT.     //
// Using the gROOT pointer one has access to basically every object     //
// created in a ROOT based program. The TROOT object is essentially a   //
// "dispatcher" with several lists pointing to the ROOT main objects.   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
#ifndef ROOT_TDirectory
//*KEEP,TDirectory.
#include "TDirectory.h"
//*KEND.
#endif
#ifndef ROOT_TList
//*KEEP,TList.
#include "TList.h"
//*KEND.
#endif
 
class TClass;
class TColor;
class TDataType;
class TFile;
class TF1;
class TStyle;
class TDirectory;
class TVirtualPad;
class TGeometry;
class TApplication;
class TInterpreter;
class TBrowser;
class TGlobal;
class TFunction;
 
 
class TROOT : public TDirectory {
 
friend class TCint;
 
private:
   static Bool_t   fgRootInit;            //Singleton initialization flag
   Int_t           fLineIsProcessing;     //To synchronize multi-threads
 
protected:
   TString         fVersion;              //ROOT version (from CMZ VERSQQ) ex 0.05/01
   Int_t           fVersionInt;           //ROOT version in integer format (501)
   Int_t           fVersionDate;          //Date of ROOT version (ex 951226)
   Int_t           fVersionTime;          //Time of ROOT version (ex 1152)
   Int_t           fTimer;                //Timer flag
   TApplication    *fApplication;         //Pointer to current application
   TInterpreter    *fInterpreter;         //Command interpreter
   TFile           *fCurrentFile;         //Current file
   TDirectory      *fCurrentDirectory;    //Current directory
   TVirtualPad     *fCurrentCanvas;       //Current graphics canvas
   TVirtualPad     *fCurrentPad;          //Current graphics pad
   TStyle          *fCurrentStyle;        //Current graphics style
   void            *fCurrentFunction;     //Current function
   Bool_t          fBatch;                //True if session without graphics
   Bool_t          fEditHistograms;       //True if histograms can be edited with the mouse
   Bool_t          fFromPopUp;            //True if command executed from a popup menu
   Bool_t          fMustClean;            //True if object destructor scans canvases
   Bool_t          fReadingBasket;        //True while reading a basket buffer
   Bool_t          fForceStyle;           //Force setting of current style when reading objects
   Int_t           fEditorMode;           //Current Editor mode
   Int_t           *fCounter;             //Array of counters
   TObject         *fPrimitive;           //Currently selected primitive
   TVirtualPad     *fSelectPad;           //Currently selected pad
   TSeqCollection  *fClasses;             //List of classes definition
   TSeqCollection  *fTypes;               //List of data types definition
   TSeqCollection  *fGlobals;             //List of global variables
   TSeqCollection  *fGlobalFunctions;     //List of global functions
   TSeqCollection  *fFiles;               //List of files
   TSeqCollection  *fMappedFiles;         //List of memory mapped files
   TSeqCollection  *fSockets;             //List of network sockets
   TSeqCollection  *fCanvases;            //List of canvases
   TSeqCollection  *fStyles;              //List of styles
   TSeqCollection  *fFunctions;           //List of analytic functions
   TSeqCollection  *fProcesses;           //List of connected processes
   TSeqCollection  *fColors;              //List of colors
   TSeqCollection  *fGeometries;          //List of geometries
   TSeqCollection  *fBrowsers;            //List of browsers
   TSeqCollection  *fSpecials;            //List of special objects
   TList           *fBrowsables;          //List of browsables
   TString         fDefCanvasName;        //Name of default canvas
 
   static VoidFuncPtr_t fgMakeDefCanvas;  //Pointer to default canvas constructor
 
                   TROOT();               //Only used by Dictionary
   void            InitSystem();          //Operating System interface
 
   void           *operator new(size_t l) { return TObject::operator new(l); }
 
public:
                     TROOT(Text_t *name, Text_t *title, VoidFuncPtr_t *initfunc = 0);
   virtual           ~TROOT();
   void              Browse(TBrowser *b);
   void              DisplayColorTable();
   TObject           *FindObject(Text_t *name) { void *dum; return FindObject(name, dum); }
   TObject           *FindObject(Text_t *name, void *&where);
   const Text_t      *FindObjectClassName(Text_t *name) const;
   void              ForceStyle(Bool_t force=kTRUE) {fForceStyle = force;}
   Bool_t            FromPopUp() {return fFromPopUp;}
   TApplication      *GetApplication() {return fApplication;}
   TClass            *GetClass(const Text_t *name, Bool_t load=kTRUE);
   TColor            *GetColor(Int_t color);
   Int_t             *GetCounter() {return fCounter;}
   Int_t             GetCount(Int_t i) {return fCounter[i];}
   const Text_t      *GetDefCanvasName() const {return fDefCanvasName.Data();}
   Bool_t            GetEditHistograms() {return fEditHistograms;}
   Int_t             GetEditorMode() {return fEditorMode;}
   Bool_t            GetForceStyle() {return fForceStyle;}
   VoidFuncPtr_t     GetMakeDefCanvas();
   Int_t             GetVersionDate() {return fVersionDate;}
   Int_t             GetVersionTime() {return fVersionTime;}
   Int_t             GetVersionInt() {return fVersionInt;}
   const Text_t      *GetVersion() const {return fVersion.Data();}
   TSeqCollection    *GetListOfClasses()   {return fClasses;}
   TSeqCollection    *GetListOfColors()    {return fColors;}
   TSeqCollection    *GetListOfTypes();
   TSeqCollection    *GetListOfGlobals(Bool_t load = kFALSE);
   TSeqCollection    *GetListOfGlobalFunctions(Bool_t load = kFALSE);
   TSeqCollection    *GetListOfFiles()      {return fFiles;}
   TSeqCollection    *GetListOfMappedFiles(){return fMappedFiles;}
   TSeqCollection    *GetListOfSockets()    {return fSockets;}
   TSeqCollection    *GetListOfCanvases()   {return fCanvases;}
   TSeqCollection    *GetListOfStyles()     {return fStyles;}
   TSeqCollection    *GetListOfFunctions()  {return fFunctions;}
   TSeqCollection    *GetListOfGeometries() {return fGeometries;}
   TSeqCollection    *GetListOfBrowsers()   {return fBrowsers;}
   TSeqCollection    *GetListOfSpecials()   {return fSpecials;}
   TList             *GetListOfBrowsables() {return fBrowsables;}
   TDataType         *GetType(const Text_t *name);
   TFile             *GetFile() {return fFile;}
   TFile             *GetFile(const Text_t *name);
   TStyle            *GetStyle(const Text_t *name);
   TF1               *GetFunction(const Text_t *name);
   TGlobal           *GetGlobal(const Text_t *name, Bool_t load = kFALSE);
   TGlobal           *GetGlobal(TObject *obj, Bool_t load = kFALSE);
   TFunction         *GetGlobalFunction(const Text_t *name, const Text_t *params = 0, Bool_t load = kFALSE);
   TFunction         *GetGlobalFunctionWithPrototype(const Text_t *name, const Text_t *proto = 0, Bool_t load = kFALSE);
   TGeometry         *GetGeometry(const Text_t *name);
   TObject           *GetSelectedPrimitive() {return fPrimitive;}
   TVirtualPad       *GetSelectedPad() {return fSelectPad;}
   Int_t             GetNclasses() {return fClasses->GetSize();}
   Int_t             GetNtypes() {return fTypes->GetSize();}
   void              Idle(UInt_t idleTimeInSec, const Text_t *command=0);
   Bool_t            IsBatch() const { return fBatch; }
   Bool_t            IsFolder() {return kTRUE;}
   Bool_t            IsLineProcessing() const { return fLineIsProcessing; }
   void              ls(Option_t *option="");
   void              LoadMacro(const Text_t *filename);
   Int_t             Macro(const Text_t *filename);
   Bool_t            MustClean() {return fMustClean;}
   void              ProcessLine(const Text_t *line);
   Int_t             ProcessLineFast(const Text_t *line);
   void              Proof(const Text_t *cluster = "proof");
   Bool_t            ReadingBasket() {return fReadingBasket;}
   void              Reset(Option_t *option="");
   void              SaveContext();
   void              SetApplication(TApplication *app) {fApplication = app;}
   void              SetBatch(Bool_t batch=kTRUE) { fBatch = batch; }
   void              SetCount(Int_t i=-1, Int_t value=0);
   void              SetDefCanvasName(Text_t *name="c1") {fDefCanvasName = name;}
   void              SetEditHistograms(Bool_t flag=kTRUE) {fEditHistograms=flag;}
   void              SetEditorMode(const Text_t *mode="");
   void              SetFromPopUp(Bool_t flag=kTRUE) {fFromPopUp = flag;}
   void              SetLineIsProcessing() {fLineIsProcessing++;}
   void              SetLineHasBeenProcessed() {if (fLineIsProcessing) fLineIsProcessing--;}
   void              SetReadingBasket(Bool_t flag=kTRUE) {fReadingBasket = flag;}
   void              SetMustClean(Bool_t flag=kTRUE) {fMustClean=flag;}
   void              SetSelectedPrimitive(TObject *obj) {fPrimitive = obj;}
   void              SetSelectedPad(TVirtualPad *pad) {fSelectPad = pad;}
   void              SetStyle(const Text_t *stylename="Default");
   void              Time(Int_t casetime=1) {fTimer = casetime;}
   Int_t             Timer() {return fTimer;}
 
   static const char *GetMacroPath();
   static Bool_t      Initialized();
   static void        SetMakeDefCanvas(VoidFuncPtr_t makecanvas);
 
   ClassDef(TROOT,0)  //Top level (or root) structure for all classes
};
 
 
EXTERN TROOT  *gROOT;
 
#endif
 
