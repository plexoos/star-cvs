// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: TQtEvent.h,v 1.1 2006/08/16 19:29:07 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

#ifndef ROOT_TQtEvent
#define ROOT_TQtEvent

#include "qevent.h"
#if QT_VERSION >= 0x40000
//Added by qt3to4:
#include <QCustomEvent>
#endif /* QT_VERSION */

#include "TVirtualX.h"

class TQtObject;
class TWaitCondition;
//______________________________________________________________________________
class TQtEvent : public QCustomEvent 
{

private:
    TWaitCondition *fCondition;
    ULong_t *fResult;   // QApplication owns QEvent and will destroy it
    QObject *fReceiver;
    QEvent  *fThatEvent;

public:
    TQtEvent(int code);
    TQtEvent(QObject *o, QEvent *e);
    virtual ~TQtEvent(){}
    void SetWait(TWaitCondition &condition,ULong_t &result);
    void SetWait(TWaitCondition &condition);
    void SetResult(ULong_t e=0);
 //   QEvent *WaitResult(); too dangerous
    bool Notify();
    virtual void ExecuteCB(){;}
};

#endif
