// $Id: StMultiH1F.cxx,v 1.2 2000/08/25 15:46:42 genevb Exp $
// $Log: StMultiH1F.cxx,v $
// Revision 1.2  2000/08/25 15:46:42  genevb
// Added stats box, legend names
//
// Revision 1.1  2000/07/26 22:00:27  lansdell
// new multi-hist class for superimposing the x-projections of y-bins (of a TH2F histogram) into one TH1F histogram
//

#include <iostream>
#include "StMultiH1F.h"
#include "TString.h"
#include "TLegend.h"
#include "TPad.h"

ClassImp(StMultiH1F)

StMultiH1F::StMultiH1F() {}

StMultiH1F::StMultiH1F(const char *name,const char *title,Int_t nbinsx,
		       Axis_t xlow,Axis_t xup ,Int_t nbinsy) :
  TH2F(name,title,nbinsx,xlow,xup,nbinsy,-0.5,-0.5+nbinsy) {}

StMultiH1F::StMultiH1F(const char *name,const char *title,Int_t nbinsx,
		       Double_t *xbins,Int_t nbinsy) :
  TH2F(name,title,nbinsx,xbins,nbinsy,-0.5,-0.5+nbinsy) {}

void StMultiH1F::Draw(Option_t *option) {

  Int_t ybins = GetNbinsY();
  if (ybins == 1) {
    TH1F* temp0 = (TH1F*) ProjectionX(GetName(),0,(ybins-1));
    temp0->SetStats((!TestBit(kNoStats)));
    temp0->Draw();
    return;
  }

  // overlay the y bins of the 2d histogram into a 1d histogram
  // using different line styles

  // make a legend
  TLegend *legend = new TLegend(0.80,0.84,0.98,0.98,"Legend","NDC");
  legend->SetFillColor(0);
  legend->SetFillStyle(4000);
  legend->SetMargin(0.25);

  Int_t ybin;
  Double_t maxval = -999999.;
  Int_t maxbin = -1;

  // dummy histogram pointer
  TH1F** temp = new TH1F*[ybins];

  for (ybin=0; ybin<ybins; ybin++) {
    TString n0;
    if ((ybin >= 10) || (names[ybin].IsNull())) n0 = GetName();
    else n0 = names[ybin];
    Int_t slice = ybin+1;
    temp[ybin] = (TH1F*)ProjectionX(n0.Data(),slice,slice);
    temp[ybin]->SetLineStyle(slice);
    temp[ybin]->SetLineWidth(5);
    temp[ybin]->SetStats(kFALSE);
    Double_t binmax = temp[ybin]->GetMaximum();
    if (binmax > maxval) {
      maxval = binmax;
      maxbin = ybin;
    }
    legend->AddEntry(temp[ybin],n0.Data(),"l");
  }

  // can't use the option argument in Draw() since this is called from
  // StHistUtil::DrawHists(), which defaults 2D histograms to a box plot
  temp[maxbin]->Draw();
  for (ybin=0; ybin<ybins; ybin++) {
    if (ybin != maxbin) temp[ybin]->Draw("same");
  }

  // Draw statistics for full set if stats are turned on
  if (!TestBit(kNoStats)) {
    temp[0] = (TH1F*) ProjectionX(GetName(),0,(ybins-1));
    temp[0]->SetStats(kTRUE);
    temp[0]->Draw("boxsames");
    legend->SetX1(0.59);
    legend->SetX2(0.77);
  }

  legend->Draw();
}
