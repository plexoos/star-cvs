// @(#)root/ged:$Name:  $:$Id: TQtGedFrames.cxx,v 1.2 2009/08/03 18:03:03 fine Exp $
// Author: Marek Biskup, Ilka Antcheva   22/07/03

/****************************************************************************
** Copyright (C) 2004 by Valeri Fine.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

/*************************************************************************
 * This source is based on TGedEditor, a ROOT GUI toolkit.               *
 * Author: Marek Biskup, Ilka Antcheva   22/07/03                        *
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TQtGedAttFrame, TQtGedAttNameFrame, TQtGedAttFillFrame,             //
//  TQtGedAttLineFrame, TQtGedAttTextFrame, TQtGedAttMarkerFrame        //
//                                                                      //
//  Frames with object attributes, just like on TAttCanvases.           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TQtGedFrames.h"
#include "TColor.h"
#include "TCint.h"
#include "TCanvas.h"
#include "TVirtualMutex.h"
#include "TVirtualPad.h"
#include "TAttMarker.h"
#include "TAttLine.h"
#include "TAttText.h"
#include "TAttFill.h"
#include "TAxis.h"

#include "TGQt.h"
#include "TQtPatternSelect.h"
#include "TQtColorSelect.h"
#include "TQtStyleComboBox.h"
#include "TQtColorSelectButton.h"
#include "TQtFloatSpinBox.h"

#include <qlabel.h> 
#include <qhbox.h> 
#include <qvbox.h> 
#include <qcombobox.h> 
#include <qlayout.h> 
#include <qmainwindow.h>
#include <qobjectlist.h>
#include <qtooltip.h>

#include "Api.h"
#include "TPaveLabel.h"
#include <snprintf.h>

// ClassImp(TQtGedAttInterface)
// ClassImp(TQtGedAttFrame)
ClassImp(TQtGedAttFillFrame)
ClassImp(TQtGedAttLineFrame)
ClassImp(TQtGedAttTextFrame)

//______________________________________________________________________________
//
//                  TQtGedAttNameFrame
//______________________________________________________________________________
TQtGedAttNameFrame::TQtGedAttNameFrame( QMainWindow  *p, TCanvas *canvas, Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TObject>(p, "Name",canvas, id, width, height, options, back)
{
   
}

//______________________________________________________________________________
TQtGedAttNameFrame::TQtGedAttNameFrame( TCanvas *canvas, QWidget  *p, Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TObject>("Name", p, canvas, id, width, height, options, back)
{
   
}
//______________________________________________________________________________
void TQtGedAttNameFrame::BuildView(QWidget  *editorPanel)
{
   // Create the frame of the selected object info.
   fLabel = new QLabel(editorPanel);   
//   fprintf(stderr,"TQtGedAttNameFrame::TQtGedAttNameFrame %d %d \n", boxLayout()->spacing(),  boxLayout()->margin());
}
//______________________________________________________________________________
void TQtGedAttNameFrame::ChangeView()
{

   fLabel->setPaletteForegroundColor(QColor("#ff0000"));
   fLabel->setText(QString(fModel->GetName()) + QString("::") + QString(fModel->ClassName()));
}
//______________________________________________________________________________
void TQtGedAttNameFrame::ConnectSignals2Slots()
{
   // Connect signals to slots.
   // There is no signal for this class to connect
}
//______________________________________________________________________________
//
//                  TQtGedAttFillFrame
//______________________________________________________________________________
TQtGedAttFillFrame::TQtGedAttFillFrame(QMainWindow  *p, TCanvas *canvas, Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TAttFill>(p, "Fill", canvas, id, width, height, options, back)
{
   // Constructor of fill attributes GUI.
   
}
//______________________________________________________________________________
TQtGedAttFillFrame::TQtGedAttFillFrame(TCanvas *canvas, QWidget *p,Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TAttFill>("Fill",p, canvas, id, width, height, options, back)
{
   // Constructor of fill attributes GUI.
   

}
//______________________________________________________________________________
void TQtGedAttFillFrame::BuildView(QWidget  *editorPanel )
{
   fColorSelect   = new TQtColorSelect  (editorPanel, 0);
   fPatternSelect = new TQtPatternSelect(editorPanel,1);
}
//______________________________________________________________________________
void TQtGedAttFillFrame::ChangeView()
{
   // Pick up the used fill attributes.

   fPatternSelect->SetPattern(fModel->GetFillStyle());
   fColorSelect  ->SetColor  (fModel->GetFillColor());
}
//______________________________________________________________________________
void TQtGedAttFillFrame::ConnectSignals2Slots()
{
   // Connect signals to slots.

   ConnectView(fColorSelect,SIGNAL(ColorSelected(Pixel_t)),this, SLOT(SetColor(Pixel_t)));
   ConnectView(fPatternSelect,SIGNAL(PatternSelected(Style_t)),this, SLOT(SetPattern(Style_t)));
}
//______________________________________________________________________________
void TQtGedAttFillFrame::SetColor(Pixel_t pixel) 
{
   // Change the model fill color
   fModel->SetFillColor(TColor::GetColor(pixel));
}
//______________________________________________________________________________
void TQtGedAttFillFrame::SetPattern(Style_t pattern) 
{
   // Change the model fill pattern
   fModel->SetFillStyle(pattern);
}

//______________________________________________________________________________
//
//                  TQtGedAttLineFrame
//______________________________________________________________________________
TQtGedAttLineFrame::TQtGedAttLineFrame(QMainWindow  *p, TCanvas *canvas, Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TAttLine>(p, "Line", canvas, id, width, height, options, back)
{
   
}
//______________________________________________________________________________
TQtGedAttLineFrame::TQtGedAttLineFrame( TCanvas *canvas, QWidget  *p,Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TAttLine>("Line", p, canvas, id, width, height, options, back)
{
   
}

//______________________________________________________________________________
void  TQtGedAttLineFrame::BuildView(QWidget  *editorPanel)
{
   // Constructor of line attributes GUI.

   QVBox *vbox = new QVBox(editorPanel); vbox->setSpacing(3);
   QHBox *hbox = new QHBox(vbox);      hbox->setSpacing(3);
   
   fColorSelect = new TQtColorSelect  (hbox, 0);
   fWidthCombo  = new TQtLineWidthComboBox(hbox);
   QToolTip::add(fWidthCombo,"Select the line width.");
   fStyleCombo  = new TQtLineStyleComboBox(vbox);
   QToolTip::add(fWidthCombo,"Select the line style.");

   fStyleCombo->setMaximumWidth(int(EditorDefaultWidth()*0.98) );
   fWidthCombo->setMaximumWidth(int(EditorDefaultWidth()*0.6)  );

#if 0

   TGCompositeFrame *f2 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);

   fStyleCombo->Resize(137, 20);
   fWidthCombo->Resize(91, 20);
#endif
}

//______________________________________________________________________________
void TQtGedAttLineFrame::ChangeView()
{
   // Pick up the used line attributes.

    fStyleCombo ->SetCurrentItem((int)fModel->GetLineStyle());
    fWidthCombo ->SetCurrentItem((int)TMath::Abs(fModel->GetLineWidth()%100));    
    fColorSelect->SetColor(fModel->GetLineColor());
}
//______________________________________________________________________________
void TQtGedAttLineFrame::ConnectSignals2Slots()
{
   // Connect signals to slots.

   ConnectView(fColorSelect,SIGNAL(ColorSelected(Pixel_t)),this, SLOT(SetColor(Pixel_t)));
   ConnectView(fWidthCombo, SIGNAL(activated(int)),this,         SLOT(SetSize(int))     );
   ConnectView(fStyleCombo, SIGNAL(activated(int)),this,         SLOT(SetStyle(int))    );
}
//______________________________________________________________________________
void TQtGedAttLineFrame::SetColor(Pixel_t pixel) 
{
   // Change the model line color
   fModel->SetLineColor(TColor::GetColor(pixel));
}

//______________________________________________________________________________
void TQtGedAttLineFrame::SetSize(int width) 
{
   // Change the model line width
   width++;
   Int_t shadowWidth = TMath::Abs(fModel->GetLineWidth())/100;
   Width_t size = TMath::Sign(Width_t(100*shadowWidth+width),fModel->GetLineWidth());
   fModel->SetLineWidth(size);
}

//______________________________________________________________________________
void TQtGedAttLineFrame::SetStyle(int style) 
{
   // Change the model line style
   fModel->SetLineStyle(Style_t(style+1));
}

//______________________________________________________________________________
//
//                  TQtGedAttTextFrame
//______________________________________________________________________________
TQtGedAttTextFrame::TQtGedAttTextFrame(QMainWindow  *p, TCanvas *canvas, Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TAttText>(p, "Text", canvas, id, width, height, options, back)
{
   
}
//______________________________________________________________________________
TQtGedAttTextFrame::TQtGedAttTextFrame(TCanvas *canvas, QWidget *p,Int_t id, Int_t width,
                                   Int_t height, UInt_t options, Pixel_t back)
   : TQtGedAttFrame<TAttText>("Text", p, canvas, id, width, height, options, back)
{
   
}
//______________________________________________________________________________
void TQtGedAttTextFrame::BuildView(QWidget  *editorPanel) 
{
// Constructor of text attributes GUI.
   QVBox *vbox = new QVBox(editorPanel); vbox->setSpacing(2);
   QHBox *hbox = new QHBox(vbox); hbox->setSpacing(1);
   fColorSelect = new TQtColorSelect  (hbox, 0);

   fSizeCombo = BuildFontSizeComboBox(hbox);
   // fSizeCombo->Resize(91, 20);
   fTypeCombo = new TQtFontComboBox(vbox);
   // fTypeCombo->Resize(137, 20);
   fAlignCombo  = BuildTextAlignComboBox(vbox);
   // fAlignCombo->Resize(137, 20);
}

//______________________________________________________________________________
void TQtGedAttTextFrame::ChangeView()
{
   // Pick up the values of used text attributes.

   // fTypeCombo->setCurrentItem(fModel->GetTextFont() / 10);

   Float_t s = fModel->GetTextSize();
   Float_t dy;

   TPaveLabel *paveLabel = dynamic_cast<TPaveLabel *>(fModel);
   if (paveLabel) {
       TBox *pl = (TBox*)paveLabel;
       dy = s * (pl->GetY2() - pl->GetY1());
   } else {
       dy = s * (fPad->GetY2() - fPad->GetY1());
   }

   Int_t size = fPad->YtoPixel(0.0) - fPad->YtoPixel(dy);
   if (size > 50) size = 50;
   else if (size < 0)  size = 0;

   fTypeCombo->SetCurrentItem((int)fModel->GetTextFont()/10);

   fSizeCombo->setCurrentItem(size-1);

   int index =  fModel->GetTextAlign()/10 + (fModel->GetTextAlign()%10-1)*3 -1;
   fAlignCombo->setCurrentItem(index);
    // Pixel_t p = TColor::Number2Pixel(fModel->GetTextColor());
   fColorSelect->SetColor(fModel->GetTextColor());
}
//______________________________________________________________________________
void TQtGedAttTextFrame::ConnectSignals2Slots()
{
   // Connect signals to slots.

   ConnectView(fColorSelect,SIGNAL(ColorSelected(Pixel_t)),this, SLOT(SetColor(Pixel_t)));
   ConnectView(fTypeCombo,  SIGNAL(activated(int)),        this, SLOT(SetStyle(int)) );
   ConnectView(fAlignCombo, SIGNAL(activated(int)),        this, SLOT(SetAlign(int)) );
   ConnectView(fSizeCombo , SIGNAL(activated(int)),        this, SLOT(SetSize(int)) );
}
//______________________________________________________________________________
void TQtGedAttTextFrame::SetColor(Pixel_t pixel) 
{
   // Change the model color
   fModel->SetTextColor(TColor::GetColor(pixel));
}

//______________________________________________________________________________
void TQtGedAttTextFrame::SetSize(int size) 
{
   // Change the model size
   size++;
   Float_t dy = fPad->AbsPixeltoY(0) - fPad->AbsPixeltoY(size);
   Float_t TextSize;
   TPaveLabel *paveLabel = dynamic_cast<TPaveLabel *>(fModel);
      if (paveLabel) {
         TBox *pl = (TBox*) paveLabel;
         TextSize = dy/(pl->GetY2() - pl->GetY1());
      }
      else
         TextSize = dy/(fPad->GetY2() - fPad->GetY1());
   fModel->SetTextSize(TextSize);
}

//______________________________________________________________________________
void TQtGedAttTextFrame::SetStyle(int style) 
{
   // Change the model font style
   fModel->SetTextFont((style+1)*10);
}

//______________________________________________________________________________
void TQtGedAttTextFrame::SetAlign(Int_t /*align*/) 
{
   // Change the model font alignment
   QString ii = fAlignCombo->currentText().section(' ',0,0);
   int i = ii.toInt();
   fModel->SetTextAlign(i);
}

//______________________________________________________________________________
QComboBox* TQtGedAttTextFrame::BuildFontSizeComboBox(QWidget* parent,Int_t /* id */)
{
   // Create text size combo box.

   QComboBox *c = new QComboBox(FALSE,parent);
   c->insertItem("Default", 0);
   for (int i = 1; i <= 50; i++) 
      c->insertItem(QString::number(i), i);
   return c;
}

//______________________________________________________________________________
QComboBox* TQtGedAttTextFrame::BuildTextAlignComboBox(QWidget* parent,Int_t /* id */)
{
   // Create text align combo box.

   QComboBox *c = new QComboBox("FALSE",parent);

   c->insertItem("11 Bottom, Left");   // 11);
   c->insertItem("21 Bottom, Middle"); // 21);
   c->insertItem("31 Bottom, Right");  // 31);
   c->insertItem("12 Middle, Left");   // 12);
   c->insertItem("22 Middle, Middle"); // 22);
   c->insertItem("32 Middle, Right");  // 32);
   c->insertItem("13 Top, Left");      // 13);
   c->insertItem("23 Top, Middle");    // 23);
   c->insertItem("33 Top, Right");     // 33);

   return c;
}


// Register the GED factory interfaces:
static TQtGedFactory<TQtGedAttNameFrame>   gQtGedAttNameFrame;
static TQtGedFactory<TQtGedAttFillFrame>   gQtGedAttFillFrame;
static TQtGedFactory<TQtGedAttLineFrame>   gQtGedAttLineFrame;
static TQtGedFactory<TQtGedAttTextFrame>   gQtGedAttTextFrame;
