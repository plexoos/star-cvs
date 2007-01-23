// @(#)root/qt:$Name:  $:$Id: TQtRootSlot.h,v 1.2 2007/01/23 19:27:55 fine Exp $
// Author: Valery Fine      18/01/2007

/****************************************************************************
** $Id: TQtRootSlot.h,v 1.2 2007/01/23 19:27:55 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/
//
// The TQRootSlot singleton class introduces the global SLOT to invoke
// the  ROOT command line from the GUI signals
// Optionally one can execute TApplication::Terminate method directly
//
// It provides a Qt slot to attach the the CINT C++ interpreter 
// to any Qt signal
// To execute any C++ statement from the GUI oen should connect 
// his/her Qt signal with the Qt slot of the global instance of this class
//

#ifndef ROOT_TQRootSlot
#define ROOT_TQRootSlot

#ifndef __CINT__
#include <qobject.h>
#else
class QObject;
#define Q_OBJECT
#define slots
#endif

class TQtRootSlot : public QObject {
   Q_OBJECT
private:
   TQtRootSlot (const TQtRootSlot &);
   void operator=(const TQtRootSlot &);
protected:
   static TQtRootSlot *fgTQtRootSlot;
   TQtRootSlot () {} 
public:
   static TQtRootSlot *CintSlot();
   virtual ~TQtRootSlot() {}
   
public slots:
   void ProcessLine(const char *);
   void Terminate(int status) const;
   void Terminate()           const;
   void TerminateAndQuit()    const;

#ifndef __CINT__
signals:
   void Error(int error);
#endif 
};

bool QConnectCint(const QObject *sender, const char *signal);
bool QConnectTerminate( const QObject *sender, const char *signal);
#endif
