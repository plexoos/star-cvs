//*-- Author :    Valery Fine   14/08/01  (E-mail: fine@bnl.gov)
//
// Copyright (C)  Valery Fine, Brookhaven National Laboratory, 1999. All right reserved
//
// $Id: StEventControlPanel.h,v 1.2 2003/01/17 01:36:16 fine Exp $
//


///////////////////////////////////////////////////////////////////////
//
//  PadControl panel is the set of the static methods to control 
//  TView of any "current" TPad with some "primitive"
//  operation:
//
///////////////////////////////////////////////////////////////////////
#ifdef R__QT
#ifndef STAR_StEventControlPanel
#define STAR_StEventControlPanel

#include "TObject.h"
#include <qcursor.h>

class StChain;
class StMaker;
class TBrowser;
class StEventDisplayMaker;
class StEventDisplayInfo;

class QWidget;
class QButton;
class StFilterDialog;
class QPushButton;
 
#if !defined(__CINT__)
# include <qobject.h>
#endif

class StEventControlPanel
#if !defined(__CINT__)
   : public QObject 
#endif
{
#if !defined(__CINT__)
Q_OBJECT
#endif
protected:
   QWidget *fBar;  
   TObject *fFilter;
   StFilterDialog *fDialog;
   TBrowser    *fBrowser;
   QPushButton *fRedrawButton;
   QPushButton *fNextEventButton;
   QCursor      fSavedCursor;


public:
   static StChain             *fgChain;
   static StEventDisplayMaker *fgDispMk;
   static StEventDisplayInfo  *fgHlp;  
public slots:
     void Clicked(int id);
     void Clicked();
     void ClickedVolume();
     void PrntNames();
     void ShowFilter();
     void NextEvent();
     void Redraw();
     void SuspendTopWidget();
     void ResumeTopWidget();
     void ShowVolumeBrowser();


protected slots:
   void Disconnect();
   void DisconnectBrowser();

protected:
   void AddButt(const Char_t *buttonName, const Char_t *command);
   void Build();

public:
   StEventControlPanel();
   virtual ~StEventControlPanel();

   static void PrintNames();
   QWidget *Bar(){ return fBar;}
   void Refresh();
   void AddFilter(TObject *filter);
   void Show();

};

// StEventControlPanel __StEventControlPanel__;

#endif
#endif
