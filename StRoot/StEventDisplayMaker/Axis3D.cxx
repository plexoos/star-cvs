//*CMZ :          28/11/99  01.06.19  by  Valery Fine(fine@bnl.gov)
//*-- Author :    Valery Fine(fine@bnl.gov)   27/11/99
// ***********************************************************************
// * C++ class library to paint 3D axice "arround" TView object
//                     and zoom 3D objects  as well
// * Copyright(c) 1997~1999  [BNL] Brookhaven National Laboratory, STAR, All rights reserved
// * Author                  Valerie Fine  (fine@bnl.gov)
// * Copyright(c) 1997~1999  Valerie Fine  (fine@bnl.gov)
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// *
// * Permission to use, copy, modify and distribute this software and its
// * documentation for any purpose is hereby granted without fee,
// * provided that the above copyright notice appear in all copies and
// * that both that copyright notice and this permission notice appear
// * in supporting documentation.  Brookhaven National Laboratory makes no
// * representations about the suitability of this software for any
// * purpose.  It is provided "as is" without express or implied warranty.
// ************************************************************************
//
// $Id: Axis3D.cxx,v 1.12 1999/12/16 00:36:29 fine Exp $ 
//

#include <iostream.h>
#include <ctype.h>
#include <assert.h>

#include "TClass.h"
#include "TAxis3D.h"
#include "Hoption.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TView.h" 
#include "TVirtualPad.h"
#include "TBrowser.h"

//______________________________________________________________________________
//   The 3D axis painter class
//   ==========================
//
//  This class provide up to 3 axice to any 3D ROOT plot and
//  "ZOOM" service.
//  ExecuteEvent() method does provide zooming and moving a projection
//  3D object within TPad client area
//
//  To add the 3D rulers to any 3D view one has to create
//  an instance of this class and Draw it.
//
//   TAxis3D rulers;
//   rulers.Draw();
//
//  One can use a static method to create ruler and attach it to the current gPad
//
//   TAxis3D::ToggleRulers(); // Brings the 3D axice up
//   TAxis3D::ToggleRulers(); // next calls remove the rulers from the TPad etc
//
//   To activate Zoomer one may call
//
//   TAxis3D::ToggleZoom(); 
//
//  each time one needs move or zoom the image. Then the user can:
//    -  move:
// Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/MovePicture.gif"> </P> End_Html 
//    -  zoom:
// Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/MovePicture.gif"> </P> End_Html 
//  its 3D view with <left-mouse button> press / move.
//  The "Zoom" deactivates itself just the user release the <left-mouse button>
//
//  To change attributes of the rulers attached to the current Pad, one may 
//  query its pointer first:
//
//  TAxis3D *axis = TAxis3D::GetPadAxis(); // Ask axis pointer
//  if (axis) {
//    TAxis3D::ToggleRulers()     // To pop axice down
//    axis->SetLabelColor(kBlue); // Paint the axice labels with blue color
//    axis->SetAxisColor(kRed);   // Paint the axice itself with blue color
//    TAxis3D::ToggleRulers()     // To pop axice up
//  }
//
// The attributes of the created axice are affected by the current style
// (see TStyle class ) and Set... methods of this class
//
//  For example:
//
//   gStyle->SetAxisColor(kYellow,"X");
//   gStyle->SetAxisColor(kYellow,"Y");
//   gStyle->SetAxisColor(kYellow,"Z");
//
//   gStyle->SetLabelColor(kYellow,"X");
//   gStyle->SetLabelColor(kYellow,"Y");
//   gStyle->SetLabelColor(kYellow,"Z");
//
//   TAxis3D::ToggleRulers(); 
//   TAxis3D::ToggleRulers(); 
//
//  will draw all axice and labels with yellow color.
//
 
const Char_t *TAxis3D::rulerName = "axis3druler";
ClassImp(TAxis3D)

Hoption_t Hopt;
 
//______________________________________________________________________________
TAxis3D::TAxis3D() : TNamed(TAxis3D::rulerName,"ruler"){
  fSelected = 0;
  fZoomMode = kFALSE;
  InitSet();
}
//______________________________________________________________________________
TAxis3D::TAxis3D(Option_t *): TNamed(TAxis3D::rulerName,"ruler")
{
  fSelected = 0;
  InitSet();
  fZoomMode = kFALSE;
}
 
//______________________________________________________________________________
void TAxis3D::InitSet()
{ 
  fAxis[0].SetName("xaxis");
  fAxis[1].SetName("yaxis");
  fAxis[2].SetName("zaxis");

  fAxis[0].Set(1,0.,1.);
  fAxis[1].Set(1,0.,1.);
  fAxis[2].Set(1,0.,1.);
  UseCurrentStyle(); 
}

//______________________________________________________________________________
void TAxis3D::Browse(TBrowser *b){
   for (Int_t i=0;i<3;i++) b->Add(&fAxis[i],fAxis[i].GetTitle());
}
//______________________________________________________________________________
void TAxis3D::Copy(TObject &obj)
{
//*-*-*-*-*-*-*Copy this histogram structure to newth1*-*-*-*-*-*-*-*-*-*-*-*
//*-*          =======================================
  TNamed::Copy(obj);
  for (Int_t i=0;i<2;i++) fAxis[i].Copy(((TAxis3D&)obj).fAxis[i]);

}
//______________________________________________________________________________
Int_t TAxis3D::DistancetoPrimitive(Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*-*Compute distance from point px,py to a line*-*-*-*-*-*
//*-*                  ===========================================
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Int_t dist = 9999999; 
   if (fZoomMode) return 0;
   for (int i=0;i<3;i++) {
     Int_t axDist = fAxis[i].DistancetoPrimitive(px,py);
     if (dist > axDist) { dist = axDist; fSelected = &fAxis[i]; }
   }
   return dist;
}
//______________________________________________________________________________
void TAxis3D::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================
//*-*  This member function is called when an axis is clicked with the locator
//*-*
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 if (fSelected) fSelected->ExecuteEvent(event,px,py);
 //  Execute action corresponding to the mouse event

   static Float_t x0, y0, x1, y1;

   static Int_t pxold, pyold;
   static Int_t px0, py0;
   static Int_t linedrawn;
//   Float_t temp;
#if 0
   if (!fZoomMode && gPad->GetView()) {
      gPad->GetView()->ExecuteRotateView(event, px, py);
      return;
   }
#else
   if (!fZoomMode) return;
#endif

   // something to zoom ?
//   fPad->SetCursor(kCross);
   gPad->SetCursor(kCross);
   
   switch (event) {

   case kButton1Down:
      gVirtualX->SetLineColor(-1);
      gPad->TAttLine::Modify();  //Change line attributes only if necessary
      ((TPad *)gPad)->AbsPixeltoXY(px,py,x0,y0);
      px0   = px; py0   = py;
      pxold = px; pyold = py;
      linedrawn = 0;
      break;

   case kButton1Motion:
      if (linedrawn) gVirtualX->DrawBox(px0, py0, pxold, pyold, TVirtualX::kHollow);
      pxold = px;
      pyold = py;
      linedrawn = 1;
      gVirtualX->DrawBox(px0, py0, pxold, pyold, TVirtualX::kHollow);
      break;

   case kButton1Up: {
        Int_t i;
        gPad->GetCanvas()->FeedbackMode(kFALSE);
        TView *view = gPad->GetView();
        if (!view) break;                       // no 3D view yet 

        Float_t min[3],max[3],viewCenter[3],viewCenterNDC[3];

        view->GetRange(min,max);
        for (i =0; i<3;i++) viewCenter[i] = (max[i]+min[i])/2;
        view->WCtoNDC(viewCenter,viewCenterNDC);
        // Define the center
        Axis_t center[3],pointNDC[3],size[3],oldSize[3];
        ((TPad *)gPad)->AbsPixeltoXY(px,py,x1,y1);
        pointNDC[0] = (x0+x1)/2; pointNDC[1] = (y0+y1)/2;
        pointNDC[2] = viewCenterNDC[2];
        view->NDCtoWC(pointNDC, center); 

        for (i =0; i<3;i++) oldSize[i] = size[i]= (max[i]-min[i])/2;

        // If there was a small motion move the center only, do not change a scale
        if (TMath::Abs(px-px0)+TMath::Abs(py - py0) > 4 ) {
           Float_t newEdge[3];
           for (i =0; i<3;i++) size[i] = -1;

           pointNDC[0] = x0; pointNDC[1] = y0;

           view->NDCtoWC(pointNDC, newEdge); 
           for (i =0; i<3;i++) {
             Float_t newSize = TMath::Abs(newEdge[i]-center[i]);
             if ( newSize/oldSize[i] > 0.002)
               size[i] = TMath::Max(size[i], newSize);
             else
               size[i] = oldSize[i];
           }

           pointNDC[0] = x1; pointNDC[1] = y1;

           view->NDCtoWC(pointNDC, newEdge); 
           for (i =0; i<3;i++) {
             Float_t newSize = TMath::Abs(newEdge[i]-center[i]);
             if ( newSize/oldSize[i] > 0.002)
               size[i] = TMath::Max(size[i], newSize);
             else
               size[i] = oldSize[i];
           }
#if 0
          if (x1 < x0) {temp = x0; x0 = x1; x1 = temp;}
          if (y1 < y0) {temp = y0; y0 = y1; y1 = temp;}
          gPad->Range(x0,y0,x1,y1);
          if (fZooms < kMAXZOOMS-1) {
             fZooms++;
             fZoomX0[fZooms] = x0;
             fZoomY0[fZooms] = y0;
             fZoomX1[fZooms] = x1;
             fZoomY1[fZooms] = y1;
          }
#endif
        }
        for (i =0; i<3;i++) {
          max[i] = center[i] + size[i];
          min[i] = center[i] - size[i];
        }
        view->SetRange(min,max);

        SwitchZoom();
        gPad->Modified(kTRUE);
        gPad->Update();
        break;
      }
      default: break;
   }
}
 
 
//______________________________________________________________________________
Text_t *TAxis3D::GetObjectInfo(Int_t , Int_t )
{
//   Redefines TObject::GetObjectInfo.
//   Displays the histogram info (bin number, contents, integral up to bin
//   corresponding to cursor position px,py
//
  return StrDup("axis");
}
 
//______________________________________________________________________________
void TAxis3D::Paint(Option_t *)
{
 
  //  if (!MakeChopt(option)) return; //check options and fill Hoption structure
  
  TGaxis *axis = new TGaxis();
  PaintAxis(axis, 90);
  delete axis; 
}
 
//______________________________________________________________________________
void TAxis3D::PaintAxis(TGaxis *axis, Float_t ang)
{
//*-*-*-*-*-*-*Draw the axis for legos and surface plots*-*-*-*-*-*-*-*-*-*
//*-*          =========================================
//*-*
 
    static Float_t epsil = 0.001;
 
    Double_t cosa, sina;
    Float_t bmin, bmax;
    Float_t r[24]       /* was [3][8] */;
    Int_t ndiv, i;
    Float_t x1[3], x2[3], y1[3], y2[3], z1[3], z2[3], av[24]  /*  was [3][8] */;
    char chopax[8];
    Int_t ix1, ix2, iy1, iy2, iz1, iz2;
    Double_t rad;
 
//*-*-----------------------------------------------------------------------
 
//--    if (Hopt.System != kCARTESIAN) return ;
 
    TView *view = gPad->GetView();
    if (!view) {
        Error("PaintLegoAxis", "no TView in current pad");
        return;
    }

    rad  = TMath::ATan(1.) * (float)4. / (float)180.;
    cosa = TMath::Cos(ang*rad);
    sina = TMath::Sin(ang*rad);
  
    view->AxisVertex(ang, av, ix1, ix2, iy1, iy2, iz1, iz2);
    for (i = 1; i <= 8; ++i) {
        r[i*3 - 3] = av[i*3 - 3] + av[i*3 - 2]*cosa;
        r[i*3 - 2] = av[i*3 - 2]*sina;
        r[i*3 - 1] = av[i*3 - 1];
    }
 
    view->WCtoNDC(&r[ix1*3 - 3], x1);
    view->WCtoNDC(&r[ix2*3 - 3], x2);
    view->WCtoNDC(&r[iy1*3 - 3], y1);
    view->WCtoNDC(&r[iy2*3 - 3], y2);
    view->WCtoNDC(&r[iz1*3 - 3], z1);
    view->WCtoNDC(&r[iz2*3 - 3], z2);
 
    view->SetAxisNDC(x1, x2, y1, y2, z1, z2);
 
    Float_t *rmin = view->GetRmin();
    Float_t *rmax = view->GetRmax();
   
    axis->SetLineWidth(1);
 
    for (i=0;i<3;i++) {
//*-*-          X axis drawing
 
      Float_t ax[2], ay[2];
      Bool_t logAx = kFALSE;
      memset(chopax,0,sizeof(chopax));
      switch (i) {
        case 0 :  ax[0] = x1[0]; ax[1] = x2[0];
                  ay[0] = x1[1]; ay[1] = x2[1];
                  logAx = gPad->GetLogx();
                  break;
        case 1 :  
                  if (TMath::Abs(y1[0] - y2[0]) < epsil)  y2[0] = y1[0];                  
                  ax[0] = y1[0]; ax[1] = y2[0];
                  ay[0] = y1[1]; ay[1] = y2[1];
                  logAx = gPad->GetLogy();
                  break;                
        case 2 :  ax[0] = z1[0]; ax[1] = z2[0];
                  ay[0] = z1[1]; ay[1] = z2[1];
                  strcpy(chopax, "SDH+=");
                  logAx = gPad->GetLogz();
                  break;
        default:  assert(0);
                  continue;
      };

      // If the axis is too short - skip it
      if ( ( TMath::Abs(ax[0] - ax[1]) + TMath::Abs(ay[0] - ay[1]))  < epsil  ) continue;
   
      if (i != 2 ) {
        if (ax[0] > ax[1]) strcpy(chopax, "SDHV=+");
        else               strcpy(chopax, "SDHV=-");
      }

      if (i==1 && (TMath::Abs(z1[0] - z2[0]) + TMath::Abs(z1[1] - z2[1])) < epsil) 
                            strcpy(chopax, "SDH+=");

       //*-*-  Initialize the axis options
       if (logAx) {
          strcat(chopax,"G");
          bmin = TMath::Power(10, rmin[i]);
          bmax = TMath::Power(10, rmax[i]);
       } else {
          bmin = rmin[i];
          bmax = rmax[i];
       }
      
       axis->SetLineColor(  fAxis[i].GetAxisColor());
       axis->SetTextFont(   fAxis[i].GetLabelFont());
       axis->SetTextColor(  fAxis[i].GetLabelColor());
       axis->SetTickSize(   fAxis[i].GetTickLength());
       axis->SetLabelFont(  fAxis[i].GetLabelFont());
       axis->SetLabelOffset(fAxis[i].GetLabelOffset()+fAxis[i].GetTickLength());
       axis->SetLabelSize(  fAxis[i].GetLabelSize());
       axis->SetTitle(      fAxis[i].GetTitle());
       axis->SetTitleOffset(fAxis[i].GetTitleOffset());
       axis->SetTitleSize(  fAxis[i].GetTitleSize());
       enum { kCenterTitle = BIT(12) }; // to be removed with the last version of ROOT
       axis->SetBit(kCenterTitle, fAxis[i].TestBit(kCenterTitle));

       //*-*-    Initialize the number of divisions. If the
       //*-*-    number of divisions is negative, option 'N' is required.
       ndiv = fAxis[i].GetNdivisions();
       if (ndiv < 0) {
         ndiv = -ndiv;
         chopax[6] = 'N';
       }          
      
//*-*-             Option time display is required ?
       if (fAxis[i].GetTimeDisplay()) {
          strcat(chopax,"t");
          if (strlen(fAxis[i].GetTimeFormat()) == 0) {
             axis->SetTimeFormat(fAxis[i].ChooseTimeFormat(bmax-bmin));
          } else {
             axis->SetTimeFormat(fAxis[i].GetTimeFormat());
          }
       }
       axis->SetOption(chopax);
       axis->PaintAxis(ax[0], ay[0], ax[1], ay[1], bmin, bmax, ndiv, chopax);
    }
}
//______________________________________________________________________________
Axis_t *TAxis3D::PixeltoXYZ(Axis_t px, Axis_t py, Axis_t *point3D, TView *view)
{
  // Convert "screen pixel" coordinates to some center of 3D WC coordinate
  // if view and gPad present
  Axis_t *thisPoint = 0;
  if (!view && gPad) view = gPad->GetView();
  if (view) {
    Axis_t x[3] = {px,py,0.5}; // ((TPad *)thisPad)->AbsPixeltoXY(px,py,x[0],x[1]);
    Float_t min[3], max[3];
    view->GetRange(min,max);
    Int_t i;
    for (i =0; i<3;i++) min[i] = (max[i]+min[i])/2;
    view->WCtoNDC(min,max);
    min[0] = x[0]; min[1] = x[1];
    min[2] = max[2];
    view->NDCtoWC(min, x);  
    for (i=0;i<3;i++) point3D[i] = x[i];
    thisPoint = point3D;
  }
  return thisPoint;
}

//______________________________________________________________________________
void TAxis3D::SavePrimitive(ofstream &out, Option_t *)
{
    // Save primitive as a C++ statement(s) on output stream out
 
//   char quote = '"';
//   out<<"   "<<endl;
 
   fAxis[0].SaveAttributes(out,GetName(),"->GetXaxis()");
   fAxis[1].SaveAttributes(out,GetName(),"->GetYaxis()");
   fAxis[2].SaveAttributes(out,GetName(),"->GetZaxis()");
 
//   out <<GetName()<<"->Draw("
//      <<quote<<option<<quote<<");"<<endl;
}
 
//______________________________________________________________________________
void TAxis3D::UseCurrentStyle()
{
//*-*-*-*-*-*Replace current attributes by current style*-*-*-*-*
//*-*        ===========================================
 
   fAxis[0].ResetAttAxis("X");
   fAxis[1].ResetAttAxis("Y");
   fAxis[2].ResetAttAxis("Z");

   fAxis[0].SetTitle("x"); fAxis[0].SetLabelColor(kRed);  fAxis[0].SetAxisColor(kRed);
                           fAxis[1].SetLabelColor(kGreen);fAxis[1].SetAxisColor(kGreen);
                           fAxis[2].SetLabelColor(kBlue); fAxis[2].SetAxisColor(kBlue);

}
  
//______________________________________________________________________________
Int_t TAxis3D::AxisChoice( Option_t *axis)
{
   char achoice = toupper(axis[0]);
   if (achoice == 'X') return 0;
   if (achoice == 'Y') return 1;
   if (achoice == 'Z') return 2;
   return -1;
}
 
//______________________________________________________________________________
Int_t TAxis3D::GetNdivisions( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetNdivisions();
}
 
//______________________________________________________________________________
Color_t TAxis3D::GetAxisColor( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetAxisColor();
}
 
//______________________________________________________________________________
Color_t TAxis3D::GetLabelColor( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetLabelColor();
}
 
//______________________________________________________________________________
Style_t TAxis3D::GetLabelFont( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetLabelFont();
   return 0;
}
 
//______________________________________________________________________________
Float_t TAxis3D::GetLabelOffset( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetLabelOffset();
}
 
//______________________________________________________________________________
Float_t TAxis3D::GetLabelSize( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetLabelSize();
}
 
//______________________________________________________________________________
Float_t TAxis3D::GetTickLength( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   return fAxis[ax].GetTickLength();
}
 
//______________________________________________________________________________
Float_t TAxis3D::GetTitleOffset( Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   fAxis[ax].GetTitleOffset();
   return 0;
}
 
//______________________________________________________________________________
#define AXISCHOICE                \
   Int_t i = AxisChoice(axis);    \
   Int_t nax = 1;                 \
   if (i == -1) { i = 0; nax = 3;}\
   for (Int_t ax=i;ax<nax+i;ax++)

//______________________________________________________________________________
void TAxis3D::SetNdivisions(Int_t n, Option_t *axis)
{
   AXISCHOICE {fAxis[ax].SetNdivisions(n);}
}
 
//______________________________________________________________________________
void TAxis3D::SetAxisColor(Color_t color, Option_t *axis)
{
   AXISCHOICE {fAxis[ax].SetAxisColor(color);}
}
 
//______________________________________________________________________________
void TAxis3D::SetAxisRange(Float_t xmin, Float_t xmax, Option_t *axis)
{
   Int_t ax = AxisChoice(axis);
   TAxis *theAxis = &fAxis[ax];
   Int_t bin1 = theAxis->FindBin(xmin);
   Int_t bin2 = theAxis->FindBin(xmax);
   theAxis->SetRange(bin1, bin2);
}
 
//______________________________________________________________________________
void TAxis3D::SetLabelColor(Color_t color, Option_t *axis)
{
   AXISCHOICE { fAxis[ax].SetLabelColor(color); }
}
 
//______________________________________________________________________________
void TAxis3D::SetLabelFont(Style_t font, Option_t *axis)
{
   AXISCHOICE { fAxis[ax].SetLabelFont(font); }
}
 
//______________________________________________________________________________
void TAxis3D::SetLabelOffset(Float_t offset, Option_t *axis)
{
   AXISCHOICE { fAxis[ax].SetLabelOffset(offset); }
}
 
//______________________________________________________________________________
void TAxis3D::SetLabelSize(Float_t size, Option_t *axis)
{
   AXISCHOICE { fAxis[ax].SetLabelSize(size); }
}
 
//______________________________________________________________________________
void TAxis3D::SetTickLength(Float_t length, Option_t *axis)
{
   AXISCHOICE { fAxis[ax].SetTickLength(length); }
}
 
//______________________________________________________________________________
void TAxis3D::SetTitleOffset(Float_t offset, Option_t *axis)
{
   AXISCHOICE { fAxis[ax].SetTitleOffset(offset); }
}
#undef AXISCHOICE

//_______________________________________________________________________________________
TAxis3D *TAxis3D::GetPadAxis(TVirtualPad *pad)
{
 // returns the "pad" Axis3D object pointer if any
  TObject *o = 0;
  TVirtualPad *thisPad=pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad) {
    // Find axis in the current thisPad 
    TList *l = thisPad->GetListOfPrimitives();
    o = l->FindObject(TAxis3D::rulerName);
    if (!(o && o->InheritsFrom(Class()->GetName()))) o = 0;
  }
  return (TAxis3D *)o;
}

//_______________________________________________________________________________________
TAxis3D *TAxis3D::ToggleRulers(TVirtualPad *pad)
{
  // Turn ON / OFF the "Ruler", TAxis3D object attached
  // to the current pad
  TAxis3D *ax = 0;
  TVirtualPad *thisPad=pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad && thisPad->GetView() ) {
    TAxis3D *a =  GetPadAxis(pad);
    if (a)  delete a; 
    else {
      ax = new TAxis3D;
      ax->SetBit(kCanDelete);
      ax->Draw();      
    }          
    thisPad->Modified();
    thisPad->Update();
  }
  return ax;
}
//_______________________________________________________________________________________
TAxis3D *TAxis3D::ToggleZoom(TVirtualPad *pad)
{ 
  // Turn ON / OFF the "Ruler", TAxis3D object attached
  // to the current pad
  TAxis3D *ax = 0;
  TVirtualPad *thisPad=pad;
  if (!thisPad) thisPad = gPad;
  if (thisPad && thisPad->GetView()) {
    // Find axis in the current thisPad 
    TList *l = thisPad->GetListOfPrimitives();
    TObject *o = l->FindObject(TAxis3D::rulerName);
    if (o && o->InheritsFrom(Class()->GetName())) { // Find axis     
      if (o != l->Last()) { // make sure the TAxis3D is the last object of the Pad.
        l->Remove(o);
        l->AddLast(o);
      }
      ax = (TAxis3D *)o;
    }
    else { // There is no 
      ax = new TAxis3D;
      ax->SetBit(kCanDelete);
      ax->Draw();      
    }
    ax->SwitchZoom();
  }          
  return ax;
}
//_______________________________________________________________________________________
//
//   Axis3D.cxx history
//_______________________________________________________________________________________

// $Log: Axis3D.cxx,v $
// Revision 1.12  1999/12/16 00:36:29  fine
// new comment-html-gif file introduced
//
// Revision 1.11  1999/12/13 22:51:52  fine
// Fixes to make Sun C++ compiler happy
//
// Revision 1.10  1999/12/12 17:25:51  fine
// smart zooming
//
// Revision 1.9  1999/12/12 01:07:21  fine
// remove the compilation warnings
//
// Revision 1.8  1999/12/10 02:30:25  fine
// Centered
//
// Revision 1.7  1999/12/09 20:43:00  fine
// Zoom
//
// Revision 1.6  1999/12/02 02:46:58  fine
// Axis coloring
//
// Revision 1.5  1999/11/30 20:09:53  fine
// new static method to present rulers
//
// Revision 1.4  1999/11/30 03:01:02  fine
// clean ups
//
// Revision 1.3  1999/11/30 01:44:23  fine
// Z axis fixed
//
// Revision 1.2  1999/11/29 19:57:59  fine
// Missing ROOT constant kCenterTitle hard coded. Should be removed later
//
// Revision 1.1  1999/11/29 19:49:56  fine
// ROOT class: TAxis3D. To be moved to ROOT later
//_______________________________________________________________________________________

