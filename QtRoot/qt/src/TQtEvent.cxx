// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: TQtEvent.cxx,v 1.1 2006/08/16 19:27:06 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#include "TQtEvent.h"
#include "TWaitCondition.h"
#include "qobject.h"
#if QT_VERSION >= 0x40000
//Added by qt3to4:
#include <QCustomEvent>
#include <QEvent>
#endif /* QT_VERSION */
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////
//
//  class TQtEvent to send an event between two Qt threads
//
//////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TQtEvent::TQtEvent(int code):QCustomEvent(QEvent::User+code), fCondition(0), fResult(0)
         , fReceiver(0),fThatEvent(0)
{ }
//______________________________________________________________________________
TQtEvent::TQtEvent(QObject *o, QEvent *e): QCustomEvent(QEvent::User), fCondition(0)
         , fResult(0), fReceiver(o),fThatEvent(e)
{ }
//______________________________________________________________________________
bool TQtEvent::Notify()
{
  bool r = FALSE;
  if (fReceiver)
  {
    r = fReceiver->event(fThatEvent);
    SetResult();
  }
  return r;
}
//______________________________________________________________________________
void TQtEvent::SetResult(ULong_t e)
{
  if (fResult)   *fResult = e;
  if (fCondition) fCondition->wakeOne();
}
//______________________________________________________________________________
void TQtEvent::SetWait(TWaitCondition &condition)
{
  fCondition = &condition;
}
//______________________________________________________________________________
void TQtEvent::SetWait(TWaitCondition &condition, ULong_t &result)
{
  SetWait(condition);
  fResult    = &result;
}
