//Z,+KEEP,TAxis3D,T=C++..
//*CMZ :          28/11/99  00.18.25  by  Valery Fine(fine@mail.cern.ch)
//*-- Author :    Valery Fine(fine@bnl.gov)   27/11/99
#ifndef ROOT_TAxis3D
#define ROOT_TAxis3D
// $Id: TAxis3D.h,v 1.4 1999/12/02 02:46:58 fine Exp $ 
// ***********************************************************************
// *  C++ class library to paint axis "arround" TView object
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
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TAxis3D                                                              //
//                                                                      //
// 3D axice                                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
 
#ifndef ROOT_TAxis
//Z,+SEQ,TAxis.
//*KEEP,TAxis.
#include "TAxis.h"
//*KEND.
#endif

#ifndef ROOT_TAttLine
//Z,+SEQ,TAttLine.
//*KEEP,TAttLine.
#include "TAttLine.h"
//*KEND.
#endif
 
#ifndef ROOT_TAttFill
//Z,+SEQ,TAttFill.
//*KEEP,TAttFill.
#include "TAttFill.h"
//*KEND.
#endif
 
#ifndef ROOT_TAttMarker
//Z,+SEQ,TAttMarker.
//*KEEP,TAttMarker.
#include "TAttMarker.h"
//*KEND.
#endif
 
 
class TF1;
class TBrowser;
class TGaxis;
class TVirtualPad;
 
class TAxis3D : public TNamed  {
 
protected:
    TAxis       fAxis[3];         //X axis descriptor
    TString     fOption;
    static  const Char_t *rulerName;
private:
    Int_t   AxisChoice(Option_t *axis);
    void    Build();
 
protected:
            TAxis  *fSelected; //!  The selected axis to play with
    virtual void    Copy(TObject &hnew);
            void    InitSet();
 
public:
    TAxis3D();
    TAxis3D(Option_t *option);
//    TAxis3D(const Text_t *name,const Text_t *title,Int_t nbinsx,Axis_t xlow,Axis_t xup);
//    TAxis3D(const Text_t *name,const Text_t *title,Int_t nbinsx,Axis_t *xbins);
    virtual ~ TAxis3D(){;}

    virtual void     Browse(TBrowser *b);

    virtual Int_t    DistancetoPrimitive(Int_t px, Int_t py);
//    virtual void     Draw(Option_t *option="");
    virtual void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
 
    virtual Int_t    GetNdivisions(Option_t *axis="X");
    virtual Color_t  GetAxisColor(Option_t *axis="X");
    virtual Color_t  GetLabelColor(Option_t *axis="X");
    virtual Style_t  GetLabelFont(Option_t *axis="X");
    virtual Float_t  GetLabelOffset(Option_t *axis="X");
    virtual Float_t  GetLabelSize(Option_t *axis="X");
    static  TAxis3D *GetPadAxis(TVirtualPad *pad=0);
    virtual Float_t  GetTitleOffset(Option_t *axis="X");
    virtual Float_t  GetTickLength(Option_t *axis="X");
   
    virtual void     GetCenter(Axis_t *center){fAxis[0].GetCenter(center);}
 
    virtual void     GetLowEdge(Axis_t *edge){fAxis[0].GetLowEdge(edge);}
 
    virtual Text_t  *GetObjectInfo(Int_t px, Int_t py);
 
    Option_t        *GetOption() const {return fOption.Data();}
 
    virtual TAxis   *GetXaxis() {return &fAxis[0];}
    virtual TAxis   *GetYaxis() {return &fAxis[1];}
    virtual TAxis   *GetZaxis() {return &fAxis[2];}
    virtual Bool_t    IsFolder(){ return kTRUE;}
    virtual void     Paint(Option_t *option="");
            void     PaintAxis(TGaxis *axis, Float_t ang);
//    virtual void     Print(Option_t *option="");
    virtual void     SavePrimitive(ofstream &out, Option_t *option);
 
    virtual void     SetAxisColor(Color_t color=1, Option_t *axis="*");
    virtual void     SetAxisRange(Float_t xmin, Float_t xmax, Option_t *axis="*");
 
    virtual void     SetLabelColor(Color_t color=1, Option_t *axis="*");
    virtual void     SetLabelFont(Style_t font=62, Option_t *axis="*");
    virtual void     SetLabelOffset(Float_t offset=0.005, Option_t *axis="*");
    virtual void     SetLabelSize(Float_t size=0.02, Option_t *axis="*");
 
//    virtual void     SetName(const Text_t *name); // *MENU*
    virtual void     SetNdivisions(Int_t n=510, Option_t *axis="*");
    virtual void     SetOption(Option_t *option=" ") {fOption = option;}
    virtual void     SetTickLength(Float_t length=0.02, Option_t *axis="*");
    virtual void     SetTitleOffset(Float_t offset=1, Option_t *axis="*");
    virtual void     SetXTitle(Text_t *title) {fAxis[0].SetTitle(title);}
    virtual void     SetYTitle(Text_t *title) {fAxis[1].SetTitle(title);}
    virtual void     SetZTitle(Text_t *title) {fAxis[2].SetTitle(title);}
    static  void     ToggleRulers(TVirtualPad *pad=0);
    void             UseCurrentStyle();
 
    ClassDef(TAxis3D,1)  //3-D ruler painting class
};
 
// $Log: TAxis3D.h,v $
// Revision 1.4  1999/12/02 02:46:58  fine
// Axis coloring
//
// Revision 1.3  1999/11/30 20:09:53  fine
// new static method to present rulers
//
// Revision 1.2  1999/11/30 01:44:23  fine
// Z axis fixed
//
// Revision 1.1  1999/11/29 19:49:57  fine
// ROOT class: TAxis3D. To be moved to ROOT later
//
#endif
