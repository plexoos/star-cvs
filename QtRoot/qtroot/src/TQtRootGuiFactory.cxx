// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: TQtRootGuiFactory.cxx,v 1.1 2006/08/16 19:27:08 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

//________________________________________________________________________
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtRootGuiFactory                                                    //
//                                                                      //
// This class is a factory for Qt GUI components. It overrides          //
// the member functions of the ABS TGuiFactory.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TQtRootGuiFactory.h"
#include "TRootGuiFactory.h"
#include "TQtRConfig.h"

#include "TApplication.h"
#include "TQtApplication.h"

#include "TSystem.h"
#ifdef R__QTWIN32
# include "TWin32Application.h" 
#else
# include "TROOT.h"
# include "TQtRootApplication.h"
#endif

#include "TGClient.h"
#include "TRootCanvas.h"
#include "TRootBrowser.h"
#include "TRootControlBar.h"
#include "TContextMenu.h"
#include "TRootContextMenu.h"

#include "TGQt.h"

TGClient *TQtRootGuiFactory::gfQtClient = 0;
ClassImp(TQtRootGuiFactory)
//______________________________________________________________________________
TQtRootGuiFactory::TQtRootGuiFactory()
   : TGuiFactory("QtRootProxy","Qt-based ROOT GUI Factory"),fGuiProxy(0)
{
   // TQtRootGuiFactory ctor.
   // Restore the right TVirtulaX pointer      
   if (TGQt::GetVirtualX())  gVirtualX = TGQt::GetVirtualX();
   gSystem->Load("libGui");
   fGuiProxy = new TRootGuiFactory(); 
}

//______________________________________________________________________________
TQtRootGuiFactory::TQtRootGuiFactory(const char *name, const char *title)
   : TGuiFactory(name, title),fGuiProxy(0)
{
   // TQtRootGuiFactory ctor.
   // Restore the right TVirtulaX pointer      
   if (TGQt::GetVirtualX())  gVirtualX = TGQt::GetVirtualX();
   gSystem->Load("libGui");
   fGuiProxy = new TRootGuiFactory(name,title); 
}
//______________________________________________________________________________
TApplicationImp *TQtRootGuiFactory::CreateApplicationImp(const char *classname, int *argc, char **argv)
{
 TGQt::CreateQtApplicationImp();
#ifdef R__QTWIN32
  TApplicationImp *app = 
       new TWin32Application(classname, argc, argv);
#else
  TApplicationImp *app = 
       new TQtRootApplication (classname, argc, argv);
#endif
  CreateQClient();
  return app;        
}
//______________________________________________________________________________
void TQtRootGuiFactory::CreateQClient()
{ 
    gfQtClient = new TGClient();
   // ((TQt *)TQt::GetVirtualX())->SetQClientFilter(new TQClientFilter(new TGClient()));
}
//______________________________________________________________________________
TCanvasImp *TQtRootGuiFactory::CreateCanvasImp(TCanvas *c, const char *title, UInt_t width, UInt_t height)
{ return fGuiProxy ? fGuiProxy->CreateCanvasImp( c, title, width, height) : 0; }

//______________________________________________________________________________
TCanvasImp *TQtRootGuiFactory::CreateCanvasImp(TCanvas *c, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height)
{ return fGuiProxy ? fGuiProxy->CreateCanvasImp(c, title, x, y, width, height) : 0 ;}

//______________________________________________________________________________
TBrowserImp *TQtRootGuiFactory::CreateBrowserImp(TBrowser *b, const char *title, UInt_t width, UInt_t height)
{ return fGuiProxy ? fGuiProxy->CreateBrowserImp(b, title, width, height) : 0; }

//______________________________________________________________________________
TBrowserImp *TQtRootGuiFactory::CreateBrowserImp(TBrowser *b, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height)
{ return fGuiProxy ? fGuiProxy->CreateBrowserImp(b, title, x, y, width, height): 0 ; }

//______________________________________________________________________________
TContextMenuImp *TQtRootGuiFactory::CreateContextMenuImp(TContextMenu *c, const char *name, const char *title)
{ return fGuiProxy ? fGuiProxy->CreateContextMenuImp(c, name, title): 0;} 

//______________________________________________________________________________
TControlBarImp *TQtRootGuiFactory::CreateControlBarImp(TControlBar *c, const char *title)
{ return fGuiProxy ? fGuiProxy->CreateControlBarImp(c,title) : 0;}

//______________________________________________________________________________
TControlBarImp *TQtRootGuiFactory::CreateControlBarImp(TControlBar *c, const char *title, Int_t x, Int_t y)
{ return fGuiProxy ? fGuiProxy->CreateControlBarImp(c, title, x, y):0;}

//______________________________________________________________________________
TInspectorImp *TQtRootGuiFactory::CreateInspectorImp(const TObject *obj, UInt_t width, UInt_t height)
{ return fGuiProxy ? fGuiProxy->CreateInspectorImp(obj, width, height) :0 ;}
