//! $Id: StHistUtil.h,v 1.12 2000/07/26 19:57:48 lansdell Exp $
//! $Log: StHistUtil.h,v $
//! Revision 1.12  2000/07/26 19:57:48  lansdell
//! new histograms and functionality added (e.g., overlay several histograms, new printlist option qa_shift)
//!
//! Revision 1.11  2000/07/07 03:52:32  genevb
//! AddHist improvements
//!
//! Revision 1.10  2000/06/29 04:46:19  lansdell
//! removed virtual from inline methods
//!
//! Revision 1.9  2000/06/23 20:55:40  kathy
//! moved #endif to end where it's supposed to be
//!
//! Revision 1.8  2000/06/23 18:05:36  kathy
//! add new method PrintInfoHists which prints info about hist to screen & file -- name,entries,mean,rms
//!
//! Revision 1.7  2000/06/23 15:53:00  kathy
//! change hardwared max num copied histograms to 512
//!
//! Revision 1.6  2000/06/23 15:26:22  kathy
//! added method to return the copied array & it's size
//!
//! Revision 1.5  2000/06/23 14:31:53  kathy
//! put 2 new methods in: CopyHists (must be used first), AddHists
//!
//! Revision 1.4  2000/01/28 17:53:42  lansdell
//! split overlay method into Overlay1D and Overlay2D
//!
//! Revision 1.3  2000/01/27 18:30:12  kathy
//! add Curtis' new method which reads in 2 histograms and overlays them - Overlay2Hists
//!
//! Revision 1.2  2000/01/26 19:29:27  kathy
//! add methods SetDefaultLogXList,AddToLogXList,ExamineLogXList,RemoveFromLogXList - requested by T.Trainor - impact param hists are now draw with LogX scale
//!
//! Revision 1.1  2000/01/18 16:42:40  kathy
//! move StHistUtil class from St_QA_Maker directory and put into StAnalysisUtilities
//!
//! Revision 1.5  2000/01/12 16:49:04  kathy
//! add new methods so that one can set a list which will be used to print,draw a subset of the histograms corresponding to a given maker; new methods are SetDefaultPrintList,AddToPrintList,RemoveFromPrintList,ExaminePrintList; can't test it yet because seems can't find directory of histograms in DEV anymore and there are conflicts in NEW; updates to DrawHist method to use this new list are not done yet
//!
//! Revision 1.4  1999/12/07 21:54:15  kathy
//! added date and time to DrawHist method in StHistUtil class so that this is printed at bottom right of histogram output
//!
//! Revision 1.3  1999/11/05 22:26:01  kathy
//! now allow setting of global title from a method
//!
//! Revision 1.2  1999/11/05 21:51:58  kathy
//! write title at top of each page of histograms in DrawHists method
//!
//! Revision 1.1  1999/09/20 20:12:16  kathy
//! moved the histogram utility methods out of St_QA_Maker and into StHistUtil because they can really be used by any Maker and associated histograms
//!

///////////////////////////////////////////////////////////////////////////////
// Histogram Utility methods for use with star makers and bfc output
///////////////////////////////////////////////////////////////////////////////

#ifndef STAR_StHistUtil
#define STAR_StHistUtil


#ifndef ROOT_TH1
#include "TH1.h"
#endif

#ifndef ROOT_TH2
#include "TH2.h"
#endif

#include "TList.h"
#include "TString.h"


//  - if not using the methods of the class, then can just put class TCanvas;
//   -  however, if we are using the methods of TCanvas, then put include "TCanvas.h"
class TCanvas;
class StMaker;
class TPaveLabel;
class TDatime;

class StHistUtil {

 private:
  // Data-members to make up the output Canvases and Postscript files
  TCanvas *m_HistCanvas; //!
  Int_t   m_PadColumns;  // Number of the columns (TPad's) on the single Canvas
  Int_t   m_PadRows;     // Number of the columns (TPad's) on the single Canvas
  Int_t   m_PaperWidth;  // Paper size in cm
  Int_t   m_PaperHeight; // Paper size in cm
  TString m_FirstHistName;
  TString m_LastHistName;
  TString m_PsFileName;  // Name of the PostScipt file to plot hist's out
  TString m_GlobalTitle; // Title at top of each page of output
  TList   *m_ListOfLogY; //! list of histogram names that will be drawn with logY scale
  TList   *m_ListOfLogX; //! list of histogram names that will be drawn with logX scale
  TList   *m_ListOfPrint;//! list of histogram names that will be drawn,printed
  StMaker *m_PntrToMaker;//! pointer to an St_Maker, so can find histograms
  static const Int_t maxHistCopy=512; //! size of array of new histograms 
  TH1     *newHist[maxHistCopy]; //! array of new histograms that other will be copied into


 protected:


 public: 
  StHistUtil();
  virtual        ~StHistUtil();
  virtual Int_t   DrawHists(Char_t *dirName="QA");
  virtual Int_t   ListHists(Char_t *dirName="QA");
  virtual TList*  FindHists(Char_t *dirName="QA");
  virtual Int_t   CopyHists(TList  *dirList);
  virtual Int_t   AddHists(TList  *dirList, Int_t nHistCopy=-1);
  virtual Int_t   PrintInfoHists(TList  *dirList,  const Char_t *fname="printinfo.out");

  virtual void    SetDefaultLogYList(Char_t *dirName="QA");
  virtual Int_t   AddToLogYList(const Char_t *HistName="");
  virtual Int_t   RemoveFromLogYList(const Char_t *HistName="");
  virtual Int_t   ExamineLogYList();

  virtual void    SetDefaultLogXList(Char_t *dirName="QA");
  virtual Int_t   AddToLogXList(const Char_t *HistName="");
  virtual Int_t   RemoveFromLogXList(const Char_t *HistName="");
  virtual Int_t   ExamineLogXList();

  virtual void    SetDefaultPrintList(Char_t *dirName="QA",Char_t *analType="FullTable");
  virtual Int_t   AddToPrintList(const Char_t *HistName="");
  virtual Int_t   RemoveFromPrintList(const Char_t *HistName="");
  virtual Int_t   ExaminePrintList();

  virtual Int_t   Overlay1D(Char_t *dirName,Char_t *inHist1,Char_t *inHist2);
  virtual Int_t   Overlay2D(Char_t *dirName,Char_t *inHist1,Char_t *inHist2);

// Inline methods
  void SetHistsNamesDraw(const Char_t *firstName="*", const Char_t *lastName="*");
  void SetZones(Int_t columns=2, Int_t rows=3);   
  // SetPaperSize -->  A4 is 20,26  US letter is 20,24
  void SetPaperSize(Int_t width=20, Int_t height=24); 
  void SetPostScriptFile(const Char_t *psFileName="");
  void SetPntrToMaker(StMaker *m1);
  void SetGlobalTitle(const Char_t *globalTitle="");
  TH1** getNewHist();
  Int_t getNewHistSize();
  
// the following is a ROOT macro  that is needed in all ROOT code
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StHistUtil.h,v 1.12 2000/07/26 19:57:48 lansdell Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StHistUtil, 1)   //needed for all code that will be used in CINT
    };
        
inline void StHistUtil::SetHistsNamesDraw(const Char_t *firstName, const Char_t *lastName)
  { m_FirstHistName = firstName;  m_LastHistName  = lastName; }

inline void StHistUtil::SetZones(Int_t columns, Int_t rows)
  { m_PadColumns =columns; m_PadRows = rows;}

inline void StHistUtil::SetPaperSize(Int_t width, Int_t height)
  { m_PaperWidth = width; m_PaperHeight = height;}

inline void StHistUtil::SetPostScriptFile(const Char_t *psFileName)
  { m_PsFileName = psFileName;}

inline void StHistUtil::SetPntrToMaker(StMaker *m1) 
  {m_PntrToMaker = m1;}

inline void StHistUtil::SetGlobalTitle(const Char_t *globalTitle)
  { m_GlobalTitle = globalTitle;}

inline TH1**  StHistUtil::getNewHist()
  {return newHist;}

inline Int_t StHistUtil::getNewHistSize()
  {return maxHistCopy; }


#endif
