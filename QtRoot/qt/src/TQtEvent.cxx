// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: TQtEvent.cxx,v 1.3 2010/05/10 22:51:26 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#include <QObject>
#include "TQtEvent.h"
#include "TWaitCondition.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////
//
//  class TQtEvent to send an event between two Qt threads
//
//////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TQtEvent::TQtEvent(int code):QEvent(QEvent::Type(QEvent::User+code)), fCondition(0), fResult(0)
         , fReceiver(0),fThatEvent(0)
{ }
//______________________________________________________________________________
TQtEvent::TQtEvent(QObject *o, QEvent *e): QEvent(QEvent::User), fCondition(0)
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
void TQtEvent::SetResult(unsigned long e)
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
void TQtEvent::SetWait(TWaitCondition &condition,unsigned long &result)
{
  SetWait(condition);
  fResult    = &result;
}
