// @(#)root/gui:$Name:  $:$Id: TQtRootApplication.h,v 1.3.2.1 2016/04/27 16:45:02 jeromel Exp $
// Author: Fons Rademakers   15/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


#ifndef ROOT_TQtRootApplication
#define ROOT_TQtRootApplication

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtRootApplication                                                   //
//                                                                      //
// This class create the dummy version of the ROOT                      //
// application environment. This in contrast the Win32 version.         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TApplicationImp
#include "TApplicationImp.h"
#endif



class TQtRootApplication : public TApplicationImp {

private:
   char        *fDisplay;       // display server to conntect to

   TQtRootApplication() { fDisplay = 0; }
   void GetOptions(Int_t *argc, char **argv);

public:
   TQtRootApplication(const char *appClassName, Int_t *argc, char **argv);
   virtual ~TQtRootApplication();

   void    Show() { }
   void    Hide() { }
   void    Iconify() { }
   Bool_t  IsCmdThread() { return kTRUE; } // by default (for UNIX) ROOT is a single thread application
   void    Init() { }
   void    Open() { }
   void    Raise() { }
   void    Lower() { }

//   ClassDef(TQtRootApplication,0)  // ROOT native GUI application environment
};

#endif
