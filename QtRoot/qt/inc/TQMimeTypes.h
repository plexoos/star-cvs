// @(#)root/gui:$Name:  $:$Id: TQMimeTypes.h,v 1.2 2007/01/19 02:13:38 fine Exp $
// Author: Valeri Fine   21/01/2003
/****************************************************************************
** $Id: TQMimeTypes.h,v 1.2 2007/01/19 02:13:38 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#ifndef ROOT_TQMimeTypes
#define ROOT_TQMimeTypes


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQMimeTypes and TQMime                                               //
//                                                                      //
// This class handles mime types, used by browsers to map file types    //
// to applications and icons. TQMime is internally used by TQMimeTypes. //
//                                                                      //
// This classes are based on TGMimeTypes and TGMime class from          //
// ROOT "gui"  package                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TString.h"
#ifndef __CINT__
#  include "qglobal.h"
#  if QT_VERSION < 0x40000
#    include <qfiledialog.h>
#  else /* QT_VERSION */
#    include <q3filedialog.h>
#  endif /* QT_VERSION */
#else
  class QFileIconProvider;  // Qt3 class to be deleted
  class Q3FileIconProvider;
#endif

class TOrdCollection;
class TRegexp;

#if (QT_VERSION > 0x39999)
   class QIcon;
#else /* QT_VERSION */
   class QIconSet;
#endif /* QT_VERSION */
class TSystemFile;

class TQMime : public TObject {

friend class TQMimeTypes;

private:
   TString   fType;      // mime type
   TString   fPattern;   // filename pattern
   TString   fAction;    // associated action
#if (QT_VERSION > 0x39999)
   QIcon  *fIcon;     // associated icon set
#else /* QT_VERSION */
   QIconSet  *fIcon;     // associated icon set
#endif /* QT_VERSION */
   TRegexp   *fReg;      // pattern regular expression

public:
  ~TQMime();
};


class TQMimeTypes : public TObject {
private:
#if !defined(_MSC_VER)  || _MSC_VER >= 1310
      void operator=(const TQMimeTypes&) const {}
#endif
      void operator=(const TQMimeTypes&)  {}
      TQMimeTypes(const TQMimeTypes&) : TObject()  {}

protected:
   TString          fIconPath;   // the path to the icon directory
   TString          fFilename;   // file name of mime type file
   Bool_t           fChanged;    // true if file has changed
   TOrdCollection  *fList;       // list of mime types
#if (QT_VERSION > 0x39999)
   Q3FileIconProvider fDefaultProvider; // Default provider of the system icons;
#else /* QT_VERSION */
   QFileIconProvider fDefaultProvider; // Default provider of the system icons;
#endif /* QT_VERSION */

   TQMime    *Find(const char *filename) const;
#if (QT_VERSION > 0x39999)
   const QIcon *AddType(const TSystemFile *filename);
#else /* QT_VERSION */
   const QIconSet *AddType(const TSystemFile *filename);
#endif /* QT_VERSION */

public:
   TQMimeTypes(const char *iconPath, const char *file);
   virtual ~TQMimeTypes();
   void   SaveMimes();
   Bool_t HasChanged() const { return fChanged; }
   void   AddType(const char *type, const char *pat, const char *icon, const char *sicon, const char *action);
   void   Print(Option_t *option="") const;
   Bool_t GetAction(const char *filename, char *action) const;
   Bool_t GetType(const char *filename, char *type) const;
#if (QT_VERSION > 0x39999)
   const  QIcon *GetIcon(const char *filename) const;
   const  QIcon *GetIcon(const TSystemFile *filename);
#else /* QT_VERSION */
   const  QIconSet *GetIcon(const char *filename) const;
   const  QIconSet *GetIcon(const TSystemFile *filename);
#endif /* QT_VERSION */


#ifndef Q_MOC_RUN
//MOC_SKIP_BEGIN
   ClassDef(TQMimeTypes,0)  // Pool of mime type objects
//MOC_SKIP_END
#endif
};

#endif
