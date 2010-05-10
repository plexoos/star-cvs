// Author: Valery Fine  09/08/2004
/****************************************************************************
** $Id: TQtTimer.cxx,v 1.4 2010/05/10 22:51:26 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#include <qapplication.h>
#include "TQtTimer.h"
#include "TSystem.h"

////////////////////////////////////////////////////////////////////////////////
//
// TQtTimer is a singelton singleshot QTimer to awake the ROOT event loop from Qt event loop
//
////////////////////////////////////////////////////////////////////////////////

ClassImp(TQtTimer)

TQtTimer *TQtTimer::fgQTimer=0;
//______________________________________________________________________________
void TQtTimer::AwakeRootEvent(){
     // proceess the ROOT events inside of Qt event loop
     gSystem->DispatchOneEvent(kFALSE);
     start(300);
}
//______________________________________________________________________________
TQtTimer * TQtTimer::Create(QObject *parent)
{
   // Create a singelton object TQtTimer
   if (!fgQTimer) {
      fgQTimer = new  TQtTimer(parent);
      fgQTimer->setSingleShot(true);
      connect(fgQTimer,SIGNAL(timeout()),fgQTimer,SLOT(AwakeRootEvent()) );
   }
   return fgQTimer;
}

//______________________________________________________________________________
TQtTimer *TQtTimer::QtTimer()
{
   // Return the singleton TQtTimer object
   return fgQTimer;
}

