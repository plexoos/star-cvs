// @(#)root/gui:$Name:  $:$Id: TQtColorSelectButton.cxx,v 1.4 2007/06/29 20:24:31 fine Exp $
// Author: Bertrand Bellenot + Fons Rademakers   22/08/02

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
/**************************************************************************

    This file is part of xclass.
    Copyright (C) 2000, 2001, Hector Peraza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtColorFrame, TQt16ColorSelector, TQtColorPopup and TQtColorSelectButton.     //
//                                                                      //
// The TQtColorFrame is a small frame with border showing a specific     //
// color.                                                               //
//                                                                      //
// The TQt16ColorSelector is a composite frame with 16 TQtColorFrames.  //
//                                                                      //
// The TQtColorPopup is a popup containing a TQt16ColorSelector and a   //
// "More..." button which popups up a QColorDialog allowing custom      //
// color selection.                                                     //
//                                                                      //
// The TQtColorSelectButton widget is like a checkbutton but instead of the //
// check mark there is color area with a little down arrow. When        //
// clicked on the arrow the TQtColorPopup pops up.                       //
//                                                                      //
// Selecting a color in this widget will generate the event:            //
// kC_COLORSEL, kCOL_SELCHANGED, widget id, pixel.                      //
// and the signal:                                                      //
// ColorSelected(ULong_t pixel)                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// #include "TGClient.h"
#include "TGQt.h"
#include "TQtColorSelectButton.h"
#include "TQtColorSelect.h"
#include "TColor.h"

#include <qcolordialog.h> 
#include <qtooltip.h> 
#include <qcolor.h> 
#include <qapplication.h> 

#include <qstyle.h>
#include <qvariant.h>
#include <qlayout.h>
#if QT_VERSION < 0x40000
#   include <qwhatsthis.h>
#   include <qhgroupbox.h> 
#else /* QT_VERSION */
#   include <QWhatsThis>
#   include <QToolButton>
//Added by qt3to4:
#   include <QFrame>
#   include <QHBoxLayout>
#   include <QStyleOptionButton>
#   include <QButtonGroup>
#endif /* QT_VERSION */

//ClassImp(TQtColorFrame)
//ClassImp(TQt16ColorSelector)
//ClassImp(TQtColorPopup)
//ClassImp(TQtColorSelectButton)

static int boxSize = 22;

TQtColorPopup *TQtColorPopup::fgColorPopup = 0;

//________________________________________________________________________________
//TQtColorFrame::TQtColorFrame( QWidget *p, ULong_t color, Int_t /*n*/) 
//: QToolButton(p)
//{
//   setPaletteBackgroundColor (gQt->QtColor(color));
//   setPaletteForegroundColor (gQt->QtColor(color));
//   fColor = color;
//}

//________________________________________________________________________________
TQtColorFrame::TQtColorFrame(QWidget *p, const QColor &color, Int_t n): QToolButton(p),fActive(n),fColor(color)
{
#if QT_VERSION < 0x40000
   setPaletteBackgroundColor (fColor);
   setPaletteForegroundColor (fColor);
   if (boxSize == 0 ) {
     boxSize =   QApplication::style().subRect(QStyle::SR_PushButtonContents,this).height();
     setMinimumSize( QSize( boxSize, boxSize ) );
     setMaximumSize( QSize( boxSize, boxSize ) );
     setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
   }
#else
      QPalette palette;
      // palette.setColor(backgroundRole(), fColor);
      //palette.setColor(foregroundRole(), fColor);
      setPalette(palette);
      QPixmap px(2*boxSize,2*boxSize);
      px.fill(fColor);
      setIcon(px);
#endif

   languageChange();
#if QT_VERSION < 0x40000
   QToolTip::add(this,fColorTipLabel + fColor.name());
#else
   setToolTip(fColorTipLabel + fColor.name());
#endif      
  //  resize(10,10);
}
//________________________________________________________________________________
QSize TQtColorFrame::sizeHint () const 
{
   return QSize(boxSize,boxSize);
}
//________________________________________________________________________________
void TQtColorFrame::drawButtonLabel(QPainter *paint)
{
#if QT_VERSION < 0x40000
     QRect buttonDraw =  QApplication::style().subRect(QStyle::SR_PushButtonContents,this);
#else
     QStyleOptionButton button;
     QRect buttonDraw = QApplication::style()->subElementRect(QStyle::QStyle::SE_PushButtonContents,&button);
#endif

   buttonDraw.addCoords(1,1,-1,-2);
   paint->fillRect (buttonDraw, fColor);
}
//______________________________________________________________________________
void TQtColorFrame::languageChange()
{
   fColorTipLabel = tr("Color ");
}

//________________________________________________________________________________
TQt16ColorSelector::TQt16ColorSelector( QWidget *p,const char *name) :
   QFrame (p),fActive(-1)
{
   if (!name)
      setName("ColorSelect16");
   else 
      setName(name);
   // setGeometry(QRect(0,0,200,200));
   
#if QT_VERSION < 0x40000
   QButtonGroup *group=  new  QButtonGroup (4, Qt::Horizontal , this, "colorGroup");
   group->setExclusive(true);
#else /* QT_VERSION */
   QWidget *group          = this;
   QGridLayout *gridLayout = new QGridLayout(group);
   QButtonGroup *buttonGroup = new QButtonGroup(group);
   buttonGroup->setExclusive(true);
#endif /* QT_VERSION */
   group->setGeometry(QRect(0,0,2 + 4*(boxSize+1) +1, 2 + 4*(boxSize+1) +1));
   languageChange();
#if QT_VERSION < 0x40000
   group->setInsideMargin(2);  
   group->setInsideSpacing(1); 
#else
   gridLayout->setMargin(2);  
   gridLayout->setSpacing(1); 
#endif      
   Color_t  defColors[] = {  0, 1 , 2 , 3 
                           , 4, 5 , 6 , 7
                           , 8, 9 ,30 ,38
                           , 41,42,50 ,51 
   };
   int i =0;
//   int nDefColors = sizeof(defColors)/sizeof(Color_t);
   for (int k=0;k < 4;k++) {
      for (int j=0;j < 4;j++,i++) {
         fCe[i] = new TQtColorFrame(group,gQt->ColorIndex(defColors[i]), i);
#if QT_VERSION >= 0x40000
         gridLayout->addWidget(fCe[i],j,k);
         buttonGroup->addButton(fCe[i],i);
#endif     
      }
  }
#if QT_VERSION < 0x40000
  connect(group,SIGNAL(clicked(int)),this,SLOT(SetActiveSlot(int)));
#else  
  connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(SetActiveSlot(int)));
#endif  
 }
//________________________________________________________________________________
TQt16ColorSelector::~TQt16ColorSelector()
{ }
//________________________________________________________________________________
void TQt16ColorSelector::SetActiveSlot()
{
   TQtColorFrame *colorFrame = (TQtColorFrame*)sender();
   colorFrame->GetColor();
   emit ColorChanged(colorFrame->GetColor());
}
//________________________________________________________________________________
void TQt16ColorSelector::SetActiveSlot(Int_t newat)
{ 
   if (fActive != newat ) {
      fActive = newat;
      emit ColorChanged(GetActiveColor());
   }
}
//________________________________________________________________________________
void TQt16ColorSelector::SetActive(Int_t newat)
{
   if ( fActive != newat)  {
      SetActiveSlot(newat);
      // setButton(newat);
   }
}
//________________________________________________________________________________
const QColor  &TQt16ColorSelector::GetActiveColor() const
{
   const TQtColorFrame &activeButton = *fCe[GetActive()];
   return activeButton.GetColor();
}
//______________________________________________________________________________
void TQt16ColorSelector::languageChange()
{
    //setCaption( tr( "Select Color" ) );
    //fPushButton->setText( tr( "pushButton39" ) );
    //QToolTip::add( fPushButton, tr( "Current Color" ) );
    //QWhatsThis::add( fPushButton, tr( "Your current attribute fill color" ) );
}
//________________________________________________________________________________
TQtColorPopup::TQtColorPopup(QWidget *p, QColor &color,const char *name, bool modal, Qt::WFlags f) :
#if QT_VERSION < 0x40000
   QDialog(p,name,modal,f)
#else
   QDialog(p,f)
#endif            
   ,fCurrentColor(color)
{ 
   QFrame *inter = new QFrame(this);
   inter->setFrameShape(QFrame::Panel);
   QVBoxLayout *vLayout = new QVBoxLayout(inter);
#if QT_VERSION >= 0x40000
   setName(name);
   setModal(modal);
#endif /* QT_VERSION */
   inter->setGeometry( QRect( 2, 2, 2 + 4*(boxSize+1) +1 ,2 + 5*(boxSize+1) + boxSize/2 + 4 ) );
   TQt16ColorSelector *cs = new TQt16ColorSelector(inter);
   
   // The horizontal divider 
   QFrame *line1 = new QFrame( inter, "lineH" );
   line1->setFrameShape ( QFrame::HLine  );
   line1->setFrameShadow( QFrame::Sunken );

   QPushButton *other     = new QPushButton("Other...",inter);
   other->setAutoDefault(true); 
   
   vLayout->addWidget(cs);
   vLayout->addWidget(line1);
   vLayout->addWidget(other);
   
#if QT_VERSION < 0x40000
   QToolTip::add(other,"Popups up Color Selector");
#else
   vLayout->setMargin(0);
   vLayout->setSpacing(0);
   other->setToolTip(tr("Popups up Color Selector"));
#endif      
   connect(cs,SIGNAL(ColorChanged(const QColor &)), this, SLOT(ColorSelected(const QColor &)));
   connect(other,SIGNAL(clicked()), this, SLOT(ColorSelectDialog()));
   adjustSize();
}
//________________________________________________________________________________
TQtColorPopup *TQtColorPopup::Create(QWidget *p, QColor &color,const char *name, bool modal, Qt::WFlags f) 
{ 
  // Create the singletone object
  if (!fgColorPopup) 
     fgColorPopup =  new TQtColorPopup(p, color,name, modal, f);
  return fgColorPopup;
}
      
//________________________________________________________________________________
TQtColorPopup::~TQtColorPopup()
{ }

//________________________________________________________________________________
void TQtColorPopup::ColorSelectDialog()
{
   ColorSelected(QColorDialog::getColor(fCurrentColor, this));
}
//________________________________________________________________________________
void TQtColorPopup::ColorSelected(const QColor &color )
{   
   if (fCurrentColor != color) {
      fCurrentColor = color;
   }
   accept();
}
//______________________________________________________________________________
void TQtColorPopup::languageChange()
{
    //setCaption( tr( "Select Color" ) );
    //fPushButton->setText( tr( "pushButton39" ) );
    //QToolTip::add( fPushButton, tr( "Current Color" ) );
    //QWhatsThis::add( fPushButton, tr( "Your current attribute fill color" ) );
}
//________________________________________________________________________________
TQtColorSelectButton::TQtColorSelectButton( QWidget *p,  const char *name,Qt::WFlags f)
#if QT_VERSION < 0x40000      
    : QFrame(p,name,f)
#else /* QT_VERSION */
    : QFrame(p,f)
#endif /* QT_VERSION */
      ,fColor("grey"),fColorEmitter(0)
{
   if (name) setName(name);
   CreateWidget();
}

//________________________________________________________________________________
TQtColorSelectButton::TQtColorSelectButton( QWidget *p, UInt_t pixel, Int_t /*id*/,TColorEmit *emitter) 
#if QT_VERSION < 0x40000
    : QFrame(p,"ColorSelectButton",Qt::WStyle_Customize | Qt::WStyle_NoBorder|Qt::WStyle_StaysOnTop)
#else /* QT_VERSION */
    : QFrame(p)
#endif /* QT_VERSION */
      ,fColor(gQt->QtColor(pixel)),fColorEmitter(emitter)
{
#if QT_VERSION >= 0x40000
   setName("ColorSelectButton");
   setWindowFlags (Qt::WindowStaysOnTopHint);
#endif   
   CreateWidget();
}
//________________________________________________________________________________
TQtColorSelectButton::TQtColorSelectButton( QWidget *p, QColor &color, Int_t /*id*/,TColorEmit *emitter) 
    : QFrame(p),fColor(color),fColorEmitter(emitter)
{
   setName("ColorSelectButton");
   CreateWidget();
}
//________________________________________________________________________________
void TQtColorSelectButton::CreateWidget() 
{
    setSizePolicy(QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ));
    QRect r;
#if QT_VERSION < 0x40000
    r = QApplication::style().querySubControlMetrics(QStyle::CC_ComboBox, this, QStyle::SC_ComboBoxEditField);
          //QRect r = QStyle::visualRect( combo->style().querySubControlMetrics(QStyle::CC_ComboBox, combo,
	 //							        combo );
#else
      QStyleOptionComboBox editField;
      r = QApplication::style()->subControlRect(QStyle::CC_ComboBox, &editField, QStyle::SC_ComboBoxEditField);
#endif
    setMinimumSize( QSize( int(r.width()*0.75), r.height() ) );
    setLineWidth(1);
    //setFrameShape( QFrame::StyledPanel );
    setFrameShadow( QFrame::Raised );
    //setFrameShadow(QFrame::Plain);
    
    QHBoxLayout *layout = new QHBoxLayout( this, 0, 0, "layoutColorButton"); 
       
    //  Color Button
#if QT_VERSION < 0x40000
    fPushButton = new QButton( this, "currentColorButton" );        
    fPushButton->setPaletteBackgroundColor (fColor);
    fPushButton->setEraseColor(fColor);
#else /* QT_VERSION */
    fPushButton = new QPushButton(this);        
    QPalette palette;
    palette.setColor(fPushButton->backgroundRole(), fColor);
    fPushButton->setPalette(palette);
#endif /* QT_VERSION */

      layout->setStretchFactor(fPushButton,4); 
      layout->addWidget(fPushButton);
      connect(fPushButton,SIGNAL(clicked()),this, SLOT(PopupDialog()));


    // The vertical divider 
    QFrame *line1 = new QFrame( this, "line1" );
      line1->setFrameShape ( QFrame::VLine  );
      line1->setFrameShadow( QFrame::Sunken );
      layout->setStretchFactor(line1,1); 
      layout->addWidget( line1 );

    // The arrow button
#if QT_VERSION < 0x40000
      fArrowButton = new QToolButton( Qt::DownArrow,this,"arrowDownToolButton" );
#else
      fArrowButton = new QToolButton(this);
      fArrowButton->setArrowType(Qt::DownArrow);
#endif        
      fArrowButton->setFixedWidth(fArrowButton->sizeHint().width()+4);
      layout->setStretchFactor(fArrowButton,1); 
      layout->addWidget( fArrowButton );
      connect(fArrowButton,SIGNAL(clicked()),this, SLOT(PopupDialog()));
      
    languageChange();
#if QT_VERSION < 0x40000
    clearWState( WState_Polished );
#endif
}
//________________________________________________________________________________
TQtColorSelectButton::~TQtColorSelectButton()
{ }

//________________________________________________________________________________
void TQtColorSelectButton::PopupDialog()
{
   TQtColorPopup *popup = TQtColorPopup::Create(0,fColor);
   QPoint global = fArrowButton->mapToGlobal(QPoint(0,0));
   popup->move(global.x()
              ,global.y()+fArrowButton->frameSize().height());
   if ( popup->exec() == QDialog::Accepted ){
      // to emit the ROOT signal
      SetColor(popup->Color());
      ColorSelected();
      if (fColorEmitter) fColorEmitter->ColorEmit(fColor.pixel());
      emit colorSelected(fColor);
   }
}
//________________________________________________________________________________
void TQtColorSelectButton::SetColor(const QColor &color)
{
   // Set color.
   if (fColor != color) {
      fColor = color;
#if QT_VERSION < 0x40000
      fPushButton->setEraseColor(color);
      fPushButton->setPaletteBackgroundColor (color);
      fPushButton->setPaletteForegroundColor (color);
#else      
      QPalette palette;
      palette.setColor(fPushButton->backgroundRole(), color);
      palette.setColor(fPushButton->foregroundRole(), color);
      fPushButton->setPalette(palette);
#endif
      update();
   }
}

//______________________________________________________________________________
void TQtColorSelectButton::languageChange()
{
    setCaption( tr( "Select Color" ) );
    if (fPushButton) {
       fPushButton->setText( tr( "pushButton39" ) );
#if QT_VERSION < 0x40000
       QToolTip::add( fPushButton, tr( "Current Color" ) );
       QWhatsThis::add( fPushButton, tr( "Your current attribute fill color" ) );
#else /* QT_VERSION */
       fPushButton->setToolTip(tr( "Current Color" ) );
       fPushButton->setWhatsThis( tr( "Your current attribute fill color" ) );
#endif /* QT_VERSION */
    }
}
