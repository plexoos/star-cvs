#include "histogram.h"
#include "qpixmap.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"

//_______________________________________________________________________
Histogram::Histogram(QWidget *parent): TQtWidget(parent), histogram_(0)
{
   gROOT->SetStyle("video");
}
//_______________________________________________________________________
void Histogram::create(const char *title,int  nbins,double xlow, double xup)
{
    if (histogram_) delete histogram_;
    histogram_ = new TH1F("Histogram", title,nbins,xlow,xup);
    cd(); histogram_->Draw("surf4");
}

//_______________________________________________________________________
void Histogram::create(const char *title, int nxbins,double xlow, double xup
                                         ,int nybins, double ylow, double yup)
{
    if (histogram_) delete histogram_;
    histogram_ = new TH2F("Histogram", title,nxbins,xlow,xup
                                            ,nybins,ylow,yup);
    histogram_ -> SetMaximum(1000);
    histogram_->Draw("surf4");
}

//_______________________________________________________________________
void Histogram::init()
{
    if (histogram_) histogram_->Reset();
}

//_______________________________________________________________________
void Histogram::animate(bool)
{
   Canvas()->Modified(); 
   Canvas()->Update();
}

//_______________________________________________________________________
void Histogram::fill(float x)
{
   if (histogram_)  histogram_->Fill(x);
}

//_______________________________________________________________________
void Histogram::fill(float x, float y)
{
  if (histogram_)  histogram_->Fill(x,y);
}

