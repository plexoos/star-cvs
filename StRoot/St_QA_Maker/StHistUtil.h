//! $Id: StHistUtil.h,v 1.4 1999/12/07 21:54:15 kathy Exp $
//! $Log: StHistUtil.h,v $
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
  TCanvas       *m_HistCanvas;       //!
  Int_t          m_PadColumns;     // Number of the columns (TPad's) on the single Canvas
  Int_t          m_PadRows;        // Number of the columns (TPad's) on the single Canvas
  
  Int_t          m_PaperWidth;     // Paper size in cm
  Int_t          m_PaperHeight;    // Paper size in cm
  
  TString        m_FirstHistName;
  TString        m_LastHistName;
  
  TString        m_PsFileName;     // Name of the PostScipt file to plot hist's out
  
  TString        m_GlobalTitle;     // Title at top of each page of output
  
  TList         *m_ListOfLog;      //! list of histogram names that will be drawn with logy scale
  StMaker       *m_PntrToMaker;    //! pointer to an St_Maker, so can find histograms

 protected:

 public: 
  StHistUtil();
  virtual        ~StHistUtil();
  virtual Int_t   DrawHists(Char_t *dirName="QA");
  virtual Int_t   ListHists(Char_t *dirName="QA");
  virtual TList*  FindHists(Char_t *dirName="QA");
  virtual void    SetDefaultLogYList(Char_t *dirName="QA");
  virtual Int_t   AddToLogYList(const Char_t *HistName="");
  virtual Int_t   RemoveFromLogYList(const Char_t *HistName="");
  virtual Int_t   ExamineLogYList();


// Inline methods
  virtual void    SetHistsNamesDraw(const Char_t *firstName="*", const Char_t *lastName="*");
  virtual void    SetZones(Int_t columns=2, Int_t rows=3);   
     // SetPaperSize -->  A4 is 20,26  US letter is 20,24
  virtual void    SetPaperSize(Int_t width=20, Int_t height=24); 
  virtual void    SetPostScriptFile(const Char_t *psFileName="");
  virtual void    SetPntrToMaker(StMaker *m1);
  virtual void    SetGlobalTitle(const Char_t *globalTitle="");

  
// the following is a ROOT macro  that is needed in all ROOT code
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StHistUtil.h,v 1.4 1999/12/07 21:54:15 kathy Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StHistUtil, 1)   //needed for all code that will be used in CINT
    };
    
#endif
    
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



