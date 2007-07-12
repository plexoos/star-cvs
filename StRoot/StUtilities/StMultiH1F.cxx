//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  StMultiH1F allows multiple similar TH1F histograms to be            //
//  easily plotted on one graph                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifdef __HP_aCC
#include <Stiostream.h>
#else
#include "Stiostream.h"
#endif
#include "StMultiH1F.h"
#include "TString.h"
#include "TLegend.h"
#include "TPad.h"
#include "TDirectory.h"
#include "TMath.h"
ClassImp(StMultiH1F)

StMultiH1F::StMultiH1F() : fMOffset(0.) {}

StMultiH1F::StMultiH1F(const char *name,const char *title,Int_t nbinsx,
		       Axis_t xlow,Axis_t xup ,Int_t nbinsy) :
  TH2F(name,title,nbinsx,xlow,xup,nbinsy,-0.5,-0.5+nbinsy), fMOffset(0.) {}

StMultiH1F::StMultiH1F(const char *name,const char *title,Int_t nbinsx,
		       Double_t *xbins,Int_t nbinsy) :
  TH2F(name,title,nbinsx,xbins,nbinsy,-0.5,-0.5+nbinsy), fMOffset(0.) {}

void StMultiH1F::Draw(Option_t *option) {

  TAxis* axis = GetXaxis();
  Int_t x0 = axis->GetFirst();
  Int_t x1 = axis->GetLast();
  axis->SetRange();
  Int_t ybins = GetNbinsY();
  if (ybins == 1) {
    TH1F* temp0 = XProjection(GetName());
    temp0->SetStats((!TestBit(kNoStats)));
    TAxis* taxis = temp0->GetXaxis();
    axis->Copy(*taxis);
    taxis->SetRange(x0,x1);
    axis->SetRange(x0,x1);
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
  Double_t maxval = -1e31;
  Double_t minval = 1e31;
  Int_t maxbin = -1;
  Int_t minbin = -1;
  Float_t offset = fMOffset;
  if (fMOffset && gPad->GetLogy()) {
    Float_t max_offset = TMath::Power(
      1.0e10*GetNonZeroMinimum()/GetNonZeroMaximum(),
      1.0/(ybins-1.0));
    if (offset > max_offset) offset = max_offset;
  }

  // dummy histogram pointer
  TH1F** temp = new TH1F*[ybins];

  TString n0;
  for (ybin=0; ybin<ybins; ybin++) {
    if ((ybin >= 10) || (names[ybin].IsNull())) n0 = GetName();
    else n0 = names[ybin];
    Int_t slice = ybin+1;
    temp[ybin] = XProjection(n0.Data(),slice);
    temp[ybin]->SetLineStyle(slice);
    temp[ybin]->SetStats(kFALSE);
    TAxis* taxis = temp[ybin]->GetXaxis();
    axis->Copy(*taxis);
    taxis->SetRange(x0,x1);

    if (fMOffset && ybin) {
      temp[ybin]->SetLineColor(slice);
      if (gPad->GetLogy()) {
        temp[ybin]->Scale(TMath::Power(offset,ybin));
      } else {
        for (Int_t xbin=0; xbin<GetNbinsX(); xbin++)
          temp[ybin]->AddBinContent(xbin,offset*ybin);
      }
    }

    Double_t binmax = temp[ybin]->GetMaximum();
    Double_t binmin = temp[ybin]->GetMinimum();
    if (binmax > maxval) { maxval = binmax; maxbin = ybin; }
    if (binmin < minval) { minval = binmin; minbin = ybin; }
    legend->AddEntry(temp[ybin],n0.Data(),"l");
  }

  // can't use the option argument in Draw() since this is called from
  // StHistUtil::DrawHists(), which defaults 2D histograms to a box plot
  if (maxbin == minbin) {
    temp[maxbin]->Draw();
  } else {
    TH1F* tempb = new TH1F(*(temp[maxbin]));
    tempb->SetBinContent(1,maxval);
    tempb->SetBinContent(2,minval);
    tempb->SetMarkerStyle(1); tempb->SetMarkerColor(0);
    tempb->Draw("p");
    maxbin = -1;
  }
  for (ybin=0; ybin<ybins; ybin++) {
    if (ybin != maxbin) temp[ybin]->Draw("same");
  }

  // Draw statistics for full set if stats are turned on
  if (!TestBit(kNoStats)) {
    temp[0] = XProjection(GetName());
    temp[0]->SetEntries(GetEntries());
    temp[0]->SetStats(kTRUE);
    temp[0]->Draw("boxsames");
    legend->SetX1(0.59);
    legend->SetX2(0.77);
  }

  legend->Draw();

  axis->SetRange(x0,x1);
}

TH1F* StMultiH1F::XProjection(const char* name, Int_t ybin) {
  static char buf[256];
  if (ybin<0) sprintf(buf,"%s.",name);
  else sprintf(buf,"%s.%d.%s",GetName(),ybin,name);

  TList* tgList = gDirectory->GetList();
  TH1F* temp = (TH1F*) tgList->FindObject(buf);
  if (temp) tgList->Remove(temp);

  if (ybin<0) temp = (TH1F*) ProjectionX(buf);
  else temp = (TH1F*) ProjectionX(buf,ybin,ybin);
  TAttLine::Copy(*temp);
  TAttFill::Copy(*temp);
  TAttMarker::Copy(*temp);
  return temp;
}

void StMultiH1F::SetBarOffset(Float_t offset) {
  if (offset == 0.25) {
    fMOffset = 1.2 * (GetMaximum() - GetMinimum());
    if (!fMOffset) fMOffset = 10.0;
  } else {
    fMOffset = offset;
  }
}

Double_t StMultiH1F::GetNonZeroMinimum() const {
  Double_t value, minimum = GetMinimum();
  if (minimum) return minimum;
  minimum = GetMaximum();
  Int_t bin, binx, biny, binz;
  Int_t xfirst  = fXaxis.GetFirst();
  Int_t xlast   = fXaxis.GetLast();
  Int_t yfirst  = fYaxis.GetFirst();
  Int_t ylast   = fYaxis.GetLast();
  Int_t zfirst  = fZaxis.GetFirst();
  Int_t zlast   = fZaxis.GetLast();
  for (binz=zfirst;binz<=zlast;binz++) {
     for (biny=yfirst;biny<=ylast;biny++) {
        for (binx=xfirst;binx<=xlast;binx++) {
           bin = GetBin(binx,biny,binz);
           value = GetBinContent(bin);
           if (value && value < minimum) minimum = value;
        }
     }
  }
  if (!minimum) minimum = -1.0;
  return minimum;
}

Double_t StMultiH1F::GetNonZeroMaximum() const {
  Double_t value, maximum = GetMaximum();
  if (maximum) return maximum;
  maximum = GetMinimum();
  Int_t bin, binx, biny, binz;
  Int_t xfirst  = fXaxis.GetFirst();
  Int_t xlast   = fXaxis.GetLast();
  Int_t yfirst  = fYaxis.GetFirst();
  Int_t ylast   = fYaxis.GetLast();
  Int_t zfirst  = fZaxis.GetFirst();
  Int_t zlast   = fZaxis.GetLast();
  for (binz=zfirst;binz<=zlast;binz++) {
     for (biny=yfirst;biny<=ylast;biny++) {
        for (binx=xfirst;binx<=xlast;binx++) {
           bin = GetBin(binx,biny,binz);
           value = GetBinContent(bin);
           if (value && value > maximum) maximum = value;
        }
     }
  }
  if (!maximum) maximum = -1.0;
  return maximum;
}

// $Id: StMultiH1F.cxx,v 1.12 2007/07/12 20:26:03 fisyak Exp $
// $Log: StMultiH1F.cxx,v $
// Revision 1.12  2007/07/12 20:26:03  fisyak
// Add includes for ROOT 5.16
//
// Revision 1.11  2007/04/24 17:45:32  genevb
// Patched for problems with limited axis ranges
//
// Revision 1.10  2007/04/06 20:05:30  genevb
// Allow for lower minima
//
// Revision 1.9  2003/09/02 17:59:20  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.8  2003/01/21 18:33:27  genevb
// Better handling of temporary hists
//
// Revision 1.7  2002/04/23 01:59:16  genevb
// New offset abilities
//
// Revision 1.6  2000/09/15 21:23:36  fisyak
// HP does not have iostream
//
// Revision 1.5  2000/08/28 19:21:05  genevb
// Improved projection code
//
// Revision 1.4  2000/08/28 18:47:50  genevb
// Better handling of 1 y-bin case
//
// Revision 1.3  2000/08/25 22:03:39  genevb
// Fixed entries problem
//
// Revision 1.2  2000/08/25 15:46:42  genevb
// Added stats box, legend names
//
// Revision 1.1  2000/07/26 22:00:27  lansdell
// new multi-hist class for superimposing the x-projections of y-bins (of a TH2F histogram) into one TH1F histogram
//

