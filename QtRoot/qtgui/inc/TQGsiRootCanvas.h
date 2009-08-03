// @(#)root/qtgsi:$Name:  $:$Id: TQGsiRootCanvas.h,v 1.4 2009/08/03 18:03:08 fine Exp $
// Author: Denis Bertini, M. AL-Turany  01/11/2000

/*************************************************************************
 * Copyright (C) 1995-2006, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TQRootCanvas
#define ROOT_TQRootCanvas

///////////////////////////////////////////////////////////////////////
//
// TQRootCanvas
//
// This class replicates the interface of the TQRootCanvas class from qtgsi 
// package  developed by 
// @authors Denis Bertini <d.bertini@gsi.de>
//	   M. AL-Turany  <m.al-turany@gsi.de>
//version 2.0
//
// to allow the QtGSI application works with Qt-layer ON.
//
// It eliminates the need for the TContextMenuImp hack of the original GSI code
// The context menu implementation is defined by ".rootrc: file and 
// provided by ROOT GUI TGuiFactory factory
//
//////////////////////////////////////////////////////////////////////

#include "TQtWidget.h"

#ifndef ROOT_TPad
#include "TPad.h"
#endif


class QAction;
class QMouseEvent;
class QResizeEvent;
class QPaintEvent;
class TQCanvasMenu;
class TContextMenu;
class TControlBar;
class TCanvas;
class TBrowser;
class QWidget;
class QDropEvent;
class QDragEnterEvent;
class QCloseEvent;
class QEvent;
#ifdef __CINT__
#  undef  Q_OBJECT
#  define Q_OBJECT
#endif

class TQRootCanvas : public TQtWidget
{
#ifndef WIN32
   Q_OBJECT
#endif
private:
   TQRootCanvas(const TQRootCanvas &);
   TQRootCanvas& operator=(const TQRootCanvas &);
      
public:

   TQRootCanvas( QWidget *parent = 0, const char *name = 0 ,TCanvas *c=0);
   TQRootCanvas( QWidget *parent, QWidget* tabWin , const char *name = 0 ,TCanvas *c=0);
   virtual ~TQRootCanvas();
   Int_t GetRootWid() { return fWid;}
   Bool_t GetCanvasOwner(){ return fEmbedded; }
   QWidget* GetParent() { return fParent;}
   QWidget* GetTabWin() { return parentWidget() ;}
   virtual void dropEvent( QDropEvent *Event );
   virtual void dragEnterEvent( QDragEnterEvent *e );

signals:
#ifndef __CINT__
   void SelectedPadChanged(TPad *);
#endif
public slots:
   virtual void      Browse(TBrowser *b);
   void              Clear(Option_t *option="");
   void              Close(Option_t *option="");
   virtual void      Draw(Option_t *option="");
   virtual TObject  *DrawClone(Option_t *option="");
   virtual TObject  *DrawClonePad();
   virtual void      EditorBar();
   void              EnterLeave(TPad *prevSelPad, TObject *prevSelObj);
   void              FeedbackMode(Bool_t set);
   void              Flush();
   void              UseCurrentStyle();
   void              ForceUpdate() ;
   const char       *GetDISPLAY();
   TContextMenu     *GetContextMenu() ;
   Int_t             GetDoubleBuffer() ;
   Color_t           GetHighLightColor() ;
   TVirtualPad      *GetPadSave() ;
   Option_t         *GetSelectedOpt() ;
   Bool_t            GetShowEventStatus() ;
   Bool_t            GetAutoExec() ;
   Size_t            GetXsizeUser()  ;
   Size_t            GetYsizeUser()  ;
   Size_t            GetXsizeReal()  ;
   Size_t            GetYsizeReal()  ;
   Int_t             GetCanvasID()  ;
   Int_t             GetWindowTopX();
   Int_t             GetWindowTopY();
   UInt_t            GetWindowWidth() ;
   UInt_t            GetWindowHeight()  ;
   UInt_t            GetWw() ;
   UInt_t            GetWh() ;
   virtual void      GetCanvasPar(Int_t &wtopx, Int_t &wtopy, UInt_t &ww, UInt_t &wh);
   virtual void      HandleInput(EEventType button, Int_t x, Int_t y);
   Bool_t            HasMenuBar()  ;
   void              Iconify() ;
   Bool_t            IsBatch() ;
   Bool_t            IsRetained() ;
   virtual void      ls(Option_t *option="") ;
   void              MoveOpaque(Int_t set=1);
   Bool_t            OpaqueMoving() ;
   Bool_t            OpaqueResizing() ;
   virtual void      Paint(Option_t *option="");
   virtual TPad     *Pick(Int_t px, Int_t py, TObjLink *&pickobj) ;
   virtual TPad     *Pick(Int_t px, Int_t py, TObject *prevSelObj);
   virtual void      Resize(Option_t *option="");
   void              ResizeOpaque(Int_t set=1) ;
   void              SaveSource(const char *filename="", Option_t *option="");
   virtual void      SetCursor(ECursor cursor);
   virtual void      SetDoubleBuffer(Int_t mode=1);
   void              SetWindowPosition(Int_t x, Int_t y) ;
   void              SetWindowSize(UInt_t ww, UInt_t wh) ;
   void              SetCanvasSize(UInt_t ww, UInt_t wh);
   void              SetHighLightColor(Color_t col);
   void              SetSelected(TObject *obj) ;
   void              SetSelectedPad(TPad *pad) ;
   void              Show() ;
   virtual void      Size(Float_t xsizeuser=0, Float_t ysizeuser=0);
   void              SetBatch(Bool_t batch=kTRUE);
   void              SetRetained(Bool_t retained=kTRUE);
   void              SetTitle(const char *title="");
   virtual void      ToggleEventStatus();
   virtual void      ToggleAutoExec();
   virtual void      Update();
//////////////////////////////////////////////////////////////////////
   Bool_t NeedsResize(){return fNeedResize;}
   void SetNeedsResize(Bool_t yes) {fNeedResize=yes;}

protected:
  ////////////////////////////////////
#ifndef __CINT__      
   TQCanvasMenu *fContextMenu;   // Qt Context menu for this canvas
#endif
   Int_t fWid;                   // Windows Id of the Canvas
   Bool_t fNeedResize;           // Resize flag
   QWidget *fParent;             // parent widgets
   
   ClassDef(TQRootCanvas,1)  // QTGSI interface to TCanvas.
};

#endif






