//*-- Author :    Valery Fine   25/05/99  (E-mail: fine@bnl.gov)
//
// Copyright (C)  Valery Fine, Brookhaven National Laboratory, 1999. All right reserved
//
// $Id: PadControlPanel.h,v 1.1 2002/12/13 00:47:40 fine Exp $
//

////////////////////////////////////////////////////////////////////////
//
// This macro generates a Controlbar panel: 
// begin_html  <P ALIGN=CENTER> <IMG SRC="gif/PadControlPanel.gif" ></P> end_html
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
//  PadControl panel is the set of the static methods to control 
//  TView of any "current" TPad with some "primitive"
//  operation:
//
///////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "Gtypes.h"

class QButtonGroup;
class TVirtualPad;

class StPadControlPanel {
  private:
     QButtonGroup *fBar;  
//   TGLayoutHints *fL1;

protected:
  void AddButt(const Char_t *buttonName, const Char_t *command);

public:

  StPadControlPanel();
  void  Build();
  ~StPadControlPanel();
  QButtonGroup *Bar() const;

  void Clicked(int id);

  static void SetBackround(Color_t color, TVirtualPad *pad=0);
  static void SetBackroundStyle(TVirtualPad *pad=0);
  static void RotateView(Float_t phi, Float_t theta, TVirtualPad *pad=0);
  static void SideView(TVirtualPad *pad=0);
  static void FrontView(TVirtualPad *pad=0);
  static void TopView(TVirtualPad *pad=0);
  static void ToggleRulers(TVirtualPad *pad=0);
  static void ToggleZoom(TVirtualPad *pad=0);
  static void AddGrid();
  static void AdjustScales();
  static void Centered3DImages();
  static void Decrease3DScale();
  static void Inscrease3DScale();
  void MakeFourView(TVirtualPad *pad=0);
  void AddAxes(TVirtualPad *pad=0);
#if 0
 ClassDef(StPadControlPanel,0);
#endif
};

// StPadControlPanel __StPadControlPanel__;

