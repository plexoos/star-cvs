//*-- Author :    Valery Fine   25/05/99  (E-mail: fine@bnl.gov)
//
// Copyright (C)  Valery Fine, Brookhaven National Laboratory, 1999. All right reserved
//
// $Id: StPadControlPanel.cxx,v 1.1 2003/01/17 01:49:42 fine Exp $
//

////////////////////////////////////////////////////////////////////////
//
// This macro generates a Controlbar panel: 
// begin_html  <P ALIGN=CENTER> <IMG SRC="gif/PadControlPanel.gif" ></P> end_html
//
// To execute an item, click with the left mouse button.
//  
// Just start this macro wheneven you want:
//
//  From Root/Cint macro:
//  --------------------
//   .x PadControlPanel.C
//   .L PadControlPanel.C
// or
//   gROOT->LoadMacro("PadControlPanel.C");
//
//  From the compiled C++ code:
//  --------------------
//   gROOT->LoadMacro("PadControlPanel.C");
//
//  After that one may "click" <4 views> button to get from the single "view"
//  the expanded view as follows:
//  begin_html  <P ALIGN=CENTER> <IMG SRC="gif/FourStarView.gif" ></P> end_html
//  To cutomize the default bar the dirived class with the custom void UserBar()
//  method can be done.
//         TGButtonGroup *Bar(){ return fBar;}  
//  method can be used.
//
//  Example:
//    TGButtonGroup *myBar =   __aa__.Bar();
//     myBar->AddButton("My custom","printf(\"here is my custom action\n\");","To add your own action replace the second parameter"); 
//
//  Note:  If you don't like what it does make your private copy 
//  ====   change it with your favorite text editor and load it right
//         away.
//         NO EXTRA STEP like : compilation, linking, loading required 
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
//  PadControl panel is the set of the static methods to control 
//  TView of any "current" TPad with some "primitive"
//  operation:
//
///////////////////////////////////////////////////////////////////////

#include "StPadControlPanel.h"
#ifdef R__QT
#include "TVirtualPad.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TView.h"
#include "TList.h"
#include "TString.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TAxis3D.h" 
#include "StEventDisplayMaker.h"

#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>

QButtonGroup *mainBar=0;


//_______________________________________________________________________________________
StPadControlPanel::StPadControlPanel(QWidget *parent) { Build(parent);}
//_______________________________________________________________________________________
void StPadControlPanel::AddButt(const Char_t *buttonName, const Char_t *command)
{   
   new QPushButton(buttonName,fBar,command);
}
//_______________________________________________________________________________________
void  StPadControlPanel::Build(QWidget *parent)
{
   const char *fills[] = {
    "Black background",	"StPadControlPanel::SetBackround(kBlack);"
   ,"White background",	"StPadControlPanel::SetBackround    (19);"
   ,"Adjust scales",	"StPadControlPanel::AdjustScales    ();"
   ,"Centered",		"StPadControlPanel::Centered3DImages();"
   ,"Scale +",		"StPadControlPanel::Inscrease3DScale();"
   ,"Scale -",		"StPadControlPanel::Decrease3DScale ();"
   ,"Top View (X-Y)",	"StPadControlPanel::TopView     ();"
   ,"Side View (Y-Z)",	"StPadControlPanel::SideView    ();"
   ,"Front View (X-Z)",	"StPadControlPanel::FrontView   ();"
   ,"4 views",		"StPadControlPanel::MakeFourView();"
   ,"Add Axes",		"StPadControlPanel::AddAxes     ();"
   ,"Rulers",		"StPadControlPanel::ToggleRulers();"
   ,"Zoom",		"StPadControlPanel::ToggleZoom  ();"
  // ,"End of Chain",	"StEventDisplayMaker::MakeLoop(3);"
   ,0};


   fBar = new QVButtonGroup("Pad Control Panel",parent,"Pad");
   fBar->setCaption("Pad Control Panel");
   mainBar=fBar;
   for (int i=0;fills[i];i+=2) {AddButt(fills[i],fills[i+1]);}
   connect(fBar,SIGNAL(clicked(int)),this,SLOT(Clicked(int)));
   fBar->show();
}
//_______________________________________________________________________________________
StPadControlPanel::~StPadControlPanel(){ delete fBar; }
//_______________________________________________________________________________________
QButtonGroup *StPadControlPanel::Bar() const { return fBar;}  

//_______________________________________________________________________________________
/* public slot: */ 
void StPadControlPanel::Clicked(int id) { 
  QButton *b = fBar->find(id);
  if (b) {
     switch(id) {
        case 0: StPadControlPanel::SetBackround(kBlack); break;
        case 1: StPadControlPanel::SetBackround    (19); break;
        case 2: StPadControlPanel::AdjustScales    (); break;
        case 3: StPadControlPanel::Centered3DImages(); break;
        case 4: StPadControlPanel::Inscrease3DScale(); break;
        case 5: StPadControlPanel::Decrease3DScale (); break;
        case 6: StPadControlPanel::TopView     (); break;
        case 7: StPadControlPanel::SideView    (); break;
        case 8: StPadControlPanel::FrontView   (); break;
        case 9: StPadControlPanel::MakeFourView(); break;
        case 10: StPadControlPanel::AddAxes     (); break;
        case 11: StPadControlPanel::ToggleRulers(); break;
        case 12: StPadControlPanel::ToggleZoom  (); break;
      //  case 13: StEventDisplayMaker::MakeLoop(3); break;
      //  case 13: StEventDisplayMaker::MakeLoop(1); break;
      //  case 14: StEventDisplayMaker::MakeLoop(2); break;
        default:  break;
     }
//     gROOT->ProcessLine(b->name());   // CDictionary doesn't contain any StPadControlPanel ???
  }
}  
//_______________________________________________________________________________________
void StPadControlPanel::SetBackround(Color_t color, TVirtualPad *pad)
{
  TVirtualPad *thisPad = pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad)  {
    thisPad->SetFillColor(color);
    thisPad->Modified();
    thisPad->Update();
  }
}
//_______________________________________________________________________________________
void StPadControlPanel::SetBackroundStyle(TVirtualPad *pad)
{
  TVirtualPad *thisPad = pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad) thisPad->SetFillAttributes();  
}

//_______________________________________________________________________________________
void StPadControlPanel::RotateView(Float_t phi, Float_t theta, TVirtualPad *pad)
{
  TVirtualPad *thisPad = pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad) {
    TView *view = thisPad->GetView(); 
    if (view) {
      Int_t iret;
      Float_t p = phi;
      Float_t t = theta;
      view->SetView(p, t, 90, iret);
//      view->SetView(p, t, 0, iret);
      thisPad->SetPhi(-90-p);
      thisPad->SetTheta(90-t);
      thisPad->Modified();
      thisPad->Update();
    }
  }
}

//_______________________________________________________________________________________
void StPadControlPanel::SideView(TVirtualPad *pad){
  RotateView(0,90.0,pad);
}
//_______________________________________________________________________________________
void StPadControlPanel::FrontView(TVirtualPad *pad){
  RotateView(270.0,90.0,pad);
}
//_______________________________________________________________________________________
void StPadControlPanel::TopView(TVirtualPad *pad){
  RotateView(270.0,0.0,pad);
}
//_______________________________________________________________________________________
void StPadControlPanel::ToggleRulers(TVirtualPad *pad)
{
  TAxis3D::ToggleRulers(pad);
}

//_______________________________________________________________________________________
void StPadControlPanel::ToggleZoom(TVirtualPad *pad)
{
  TAxis3D::ToggleZoom(pad);
}

//_______________________________________________________________________________________
void StPadControlPanel::AddGrid()
{ 
  TVirtualPad *thisPad = gPad;

  if (thisPad) {
 
    TView *view = thisPad->GetView(); 
    if (!view) return;
    Double_t min[3],max[3];
    view->GetRange(min,max);

    TList *list      = thisPad->GetListOfPrimitives();
    TString histName = thisPad->GetName();
    TH2F *m_DummyHist = 0; 
    const Char_t *dummyName = "Axis3D";
    histName += dummyName;
    m_DummyHist = (TH2F *)list->FindObject(histName.Data());
    if (!m_DummyHist) { 
      m_DummyHist = new TH2F(histName.Data(),"",1,min[0],max[0],1,min[1],max[1]);
      m_DummyHist->SetDirectory(0);
      m_DummyHist->Draw("surf,same");
    }
    m_DummyHist->GetXaxis()->SetLimits(min[0],max[0]);
    m_DummyHist->GetYaxis()->SetLimits(min[1],max[1]);
    m_DummyHist->GetZaxis()->SetLimits(min[2],max[2]);
 
    thisPad->Modified();
    thisPad->Update();
  }
}
//_______________________________________________________________________________________
void StPadControlPanel::AdjustScales()
{
  TVirtualPad *thisPad = gPad;
  if (thisPad) {
    TView *view = thisPad->GetView(); 
    if (!view) return;
    Double_t min[3],max[3];
    view->GetRange(min,max);
    int i;
    Double_t maxSide = 0;
    // Find the largest side
    for (i=0;i<3; i++) maxSide = TMath::Max(maxSide,max[i]-min[i]);
    //Adjust scales:
    for (i=0;i<3; i++) max[i] += maxSide - (max[i]-min[i]);
    view->SetRange(min,max);
    thisPad->Modified();
    thisPad->Update();
 }
}
//_______________________________________________________________________________________
void StPadControlPanel::Centered3DImages()
{
  // This macro prints out the sizes of the sekected 3d pad
  TVirtualPad *thisPad = gPad;
  if (thisPad) {
    TView *view = thisPad->GetView(); 
    if (!view) return;
    Double_t min[3],max[3];
    view->GetRange(min,max);
    int i;
    for (i=0;i<3; i++) min[i]=-max[i];
    view->SetRange(min,max);
    thisPad->Modified();
    thisPad->Update();
 }
}

//_______________________________________________________________________________________
void StPadControlPanel::Decrease3DScale()
{
  TVirtualPad *thisPad = gPad;
  if (thisPad) {
    TView *view = thisPad->GetView(); 
    if (!view) return;
    Double_t min[3],max[3];
    view->GetRange(min,max);
    int i;
    for (i=0;i<3; i++) {max[i] /= 0.8; min[i]=max[i]*0.1;}
    view->SetRange(min,max);
    thisPad->Modified();
    thisPad->Update();
 }
}

//_______________________________________________________________________________________
void StPadControlPanel::Inscrease3DScale()
{
  TVirtualPad *thisPad = gPad;
  if (thisPad) {
    TView *view = thisPad->GetView(); 
    if (!view) return;
    Double_t min[3],max[3];
    view->GetRange(min,max);
    int i;
    for (i=0;i<3; i++) {max[i] *= 0.8; min[i]=max[i]*0.1;}
    view->SetRange(min,max);
    thisPad->Modified();
    thisPad->Update();
  }
}
//_______________________________________________________________________________________
void StPadControlPanel::MakeFourView(TVirtualPad *pad)
{
//  Creates 4 pads view of the pad (or gPad)
//   ------------------------------
//   |              |             |
//   |              |             |
//   |              |             |
//   |    Front     |   Top       |
//   |    view      |   view      |
//   |              |             |
//   |              |             |
//   |              |             |
//   ---------------+-------------
//   |              |             |
//   |              |             |
//   |              |             |
//   |    Side      |  Spacial    |
//   |    view      |   view      |
//   |              |             |
//   |              |             |
//   |              |             |
//   ------------------------------
// begin_html  <P ALIGN=CENTER> <IMG SRC="gif/FourStarView.gif" ></P> end_html
//
  TVirtualPad *thisPad = pad;
  if (!thisPad) thisPad = gPad;
  TView *view = 0; 
  TList *thisPrimitives = 0; 
  if (thisPad && (thisPrimitives = thisPad->GetListOfPrimitives()) && (view =  thisPad->GetView()) ) 
  {
    Double_t min[3],max[3];
    view->GetRange(min,max);
    Int_t system = view->GetSystem();
    TCanvas *c = new TCanvas(" 4 views", thisPad->GetTitle(),600,600);
    c->Divide(2,2);
    TIter *next=  new TIter(thisPrimitives);
    for (int i =1; i <= 4; i++) {
      c->cd(i);
      TList *newPrimitives = gPad->GetListOfPrimitives();
      TObject *obj = 0;
      while ( (obj = next->Next()) ) newPrimitives->Add(obj);
      TView *newView = new TView(system);
      newView->SetRange(min,max);
      next->Reset();
   }
   delete next;
   // set separate view;
   // Fron view
    Int_t j = 1;
    c->cd(j++); FrontView();
    c->cd(j++); TopView();
    c->cd(j++); SideView();
    c->cd(j++); RotateView(-30.0,60.0,0);
    c->Modified();
    c->Update();
  }
}
//_______________________________________________________________________________________
void StPadControlPanel::AddAxes(TVirtualPad *pad)
{
  // Add red, green, blue - X, Y, Z axice to the "pad"
  TVirtualPad *thisPad = pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad) {
    if (!gROOT->GetClass("St_PolyLine3D"))  gSystem->Load("St_base");
    if ( gROOT->GetClass("St_PolyLine3D"))  gROOT->ProcessLineFast("St_PolyLine3D::Axis();");
  }
}

// StPadControlPanel __StPadControlPanel__;
#endif
