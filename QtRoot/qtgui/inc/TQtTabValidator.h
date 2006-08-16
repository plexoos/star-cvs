// Author: Valeri Fine   10/08/2004
/****************************************************************************
** $Id: TQtTabValidator.h,v 1.1 2006/08/16 19:33:21 fine Exp $
**
** Copyright (C) 2003 by Valeri Fine.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

#ifndef ROOT_TQtTabValidator
#define ROOT_TQtTabValidator

#ifndef __CINT__
#  include <qvalidator.h> 
#else
  class QObject;
  class QValidator;
  typedef int State;
#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtTabValidator                                                      //
//                                                                      //
// A Qt QValidator to use the ROOT TTabCom class to validate the string //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


class QString;
class TTabCom;

class TQtTabValidator  : public QValidator {
private:
    TQtTabValidator(const TQtTabValidator &t) : QValidator (0) {}
    void operator=(const TQtTabValidator &) {}
protected:
   static TTabCom *fgTabCom;  
   TQtTabValidator() : QValidator(0) {}
public:
   TQtTabValidator(QObject *parent, const char *name=0) : QValidator(parent,name){}
   static void Clear();

   virtual State validate(QString &input, int &pos) const;
};
#endif
