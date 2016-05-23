// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: TQtObjectDialog.h,v 1.1.4.1 2016/05/23 18:33:08 jeromel Exp $
**
** Copyright (C) 2002 by Valeri Fine.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TQtObjectDialog                                                            //
//                                                                            //
// This class provides an interface to  context sensitive popup menus.        //
// These menus pop up when the user hits  the right mouse button,  and        //
// are destroyed when the menu pops downs.                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TQtObjectDialog
#define ROOT_TQtObjectDialog

#include <qdialog.h>

class TObject;
class TMethod;
class TObjArray;

class TQtObjectDialog : public QDialog {
  Q_OBJECT
protected:
  TObjArray *fParArray;
public :
 TQtObjectDialog(TObject *object=0, TMethod *method=0);
 TObjArray *GetParamList(){ return fParArray;}
public slots:
  void accept();
  void emitHelpRequest();
signals:
  void helpRequested();
};

#endif
