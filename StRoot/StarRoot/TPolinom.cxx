#include <stdlib.h>
#include <math.h>
#include "TError.h"
#include "TArrayD.h"
#include "TCL.h"
#include "TPolinom.h"
ClassImp(TPolinom)
//_____________________________________________________________________________
TPolinom::TPolinom(int npw,const double *coefs)
{
  fNP=0; fCoe=0,fEmx=0;
  SetCoefs(npw,coefs);
}
//_____________________________________________________________________________
TPolinom::TPolinom(const TPolinom& from )
{
  fNP=0; fCoe=0;fEmx=0;
  *this = from;
}
//_____________________________________________________________________________
TPolinom &TPolinom::operator=(const TPolinom& from )
{
  Clear();
  SetCoefs(from.fNP,from.fCoe);
  if (!from.fEmx) return *this;
  int n = (fNP+1)*(fNP+2)/2;
  fEmx= new double[n];
  TCL::ucopy(from.fEmx,fEmx,n);
  return *this;
}
//_____________________________________________________________________________
TPolinom::TPolinom(double c0)
{
  fNP=0; fCoe=0,fEmx=0;
  SetCoefs(0,&c0);
}
//_____________________________________________________________________________
TPolinom::TPolinom(double c0,double c1)
{
  fNP=0; fCoe=0,fEmx=0;
  SetCoefs(1,0);
  SetCoeff(0,c0);
  SetCoeff(1,c1);
}
//_____________________________________________________________________________
TPolinom::TPolinom(double c0,double c1,double c2)
{
  fNP=0; fCoe=0,fEmx=0;
  SetCoefs(2,0);
  SetCoeff(0,c0);
  SetCoeff(1,c1);
  SetCoeff(2,c2);
}
//_____________________________________________________________________________
TPolinom::~TPolinom()
{
  Clear();
}
//_____________________________________________________________________________
void TPolinom::Clear(const char *)
{
  if (fCoe!=f2Coe)  delete [] fCoe; fCoe=0;
  delete [] fEmx; fEmx=0;
  fNP=-1;
}
//_____________________________________________________________________________
void TPolinom::Print(const char*) const
{
  Info("Print","Power %d ",fNP);
  if (!fCoe) return;
  double *e = fEmx;
  for (int i=0,n=1;i<=fNP;i++,e+=n++) {
    double err = (fEmx)? sqrt(e[0]):0; 
    Info("Print","Coef[%d] = %g +- %g",i,fCoe[i],err);
  }
}


//_____________________________________________________________________________
void TPolinom::SetCoefs(int npw,const double *coefs)
{
  if (fNP!=npw || !fCoe) {
    fNP = npw;
    if (fCoe !=f2Coe) delete [] fCoe;
    fCoe=f2Coe;
    if(fNP>2) {
      fCoe = new double[fNP+1];
      memset(fCoe,0,(fNP+1)*sizeof(double));
    }
  }
  if (coefs) {memcpy(fCoe,coefs,(fNP+1)*sizeof(double));}
  else       {memset(fCoe,0    ,(fNP+1)*sizeof(double));}
}
//_____________________________________________________________________________
double TPolinom::Eval(double x,int n,double *coe) 
{
  double res = 0;
  for (int i = n; i>=0; i--) {res = coe[i]+x*res;}
  return res;
}  
//_____________________________________________________________________________
double TPolinom::Eval(double x) const
{
  return Eval(x,fNP,fCoe);
}  
//_____________________________________________________________________________
double TPolinom::Deriv(double x) const
{
  double res = 0;
  for (int i = fNP; i>0; i--) {res = fCoe[i]*i+x*res;}
  return res;
}  
//_____________________________________________________________________________
void TPolinom::Move(double x)
{
  if (!fNP) return;
  int il = fNP+1;
  if (fNP==1) {
    fCoe[0]+=fCoe[1]*x;
    if (!fEmx) return;
    fEmx[0]+=x*(2*fEmx[1]+x*fEmx[2]);
    fEmx[1]+=x*fEmx[2];
    return;
  }


  TArrayD arr((il*(il*3+1))/2);
  double *tra = arr.GetArray();
  double *tmp = tra+il*il;
  tra[0]=1; for (int i=1;i<il;i++) tra[i]=tra[i-1]*x;
  double *t=tra;
  int div=1,bak=il+1;
  for (int irow=1;irow<il;irow++) {
    div *=irow; t+=il;
    for (int icol=irow;icol<il;icol++) {
       t[icol] = (t[icol-bak]*icol)/div;
  } }

  TCL::vmatl(tra,fCoe,tmp ,il);
  TCL::ucopy(tmp,     fCoe,il);
  if (!fEmx) return;
  TCL::trasat(tra,fEmx,tmp,il,il);
  TCL::ucopy(tmp, fEmx ,il*(il+1)/2);
}
//_____________________________________________________________________________
void TPolinom::Backward() 
{
  for (int i=1;i<=fNP;i+=2) { fCoe[i]=-fCoe[i];}
  if (!fEmx) return;
  for (int i=0,li=0;i<=fNP;li+=++i) {
    for (int j=0   ;j<=i  ;j++)     {if ((i^j)&1) fEmx[li+j]*=-1;}}
}
//_____________________________________________________________________________
double TPolinom::GetEmx(int i,int j) const
{
  if (!fEmx) return 0;
  int ii = i,jj=j;
  if (i<j) {ii=j;jj=i;}
  return fEmx[((ii+1)*ii)/2+jj];
}
//_____________________________________________________________________________
double TPolinom::Evrr(double x) const
{
  if (!fEmx) return 0;
  TArrayD arr(fNP+1);
  double *xx = arr.GetArray();
  xx[0]=1;
  for (int i=1;i<=fNP;i++) xx[i]=xx[i-1]*x;
  double res;
  TCL::trasat(xx,fEmx,&res,1,fNP+1);
  return res;
}  
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
ClassImp(TPoliFitter)
//_____________________________________________________________________________
enum EPoliFitter {kX,kY,kW,kXYW=3};

//_____________________________________________________________________________
TPoliFitter::TPoliFitter(int np):TPolinom(np),fArr(100)
{
  Clear();
}
//_____________________________________________________________________________
TPoliFitter::TPoliFitter(const TPoliFitter &fr):TPolinom(fr),fArr(fr.fArr)
{
  memcpy(fBeg,fr.fBeg,fEnd-fBeg+1);
  fDat = fArr.GetArray();
  fC   = fDat+fN; fP = fC+fNP+1;
}
//_____________________________________________________________________________
void TPoliFitter::Add(double x, double y,double err2)
{
  if (fArr.GetSize()<=fN) { fArr.Set(fN*2); fDat = fArr.GetArray();}
  fArr[fN+kX]=x;
  fArr[fN+kY]=y;
  fArr[fN+kW]=1./err2;
  fN+=kXYW;fNuse++;
}
//_____________________________________________________________________________
void TPoliFitter::Skip(int idx)
{
   fWtot -= fArr[idx*kXYW+kW]*fWtot;
   fArr[idx*kXYW+kW]=0;
   double norm=0;
   for (int l=0;l<fN;l+=kXYW) {norm+=fArr[l+kW];}
   for (int l=0;l<fN;l+=kXYW) {fArr[l+kW]/=norm;}
   fNuse--;
   fNdf--;
}
//_____________________________________________________________________________
void TPoliFitter::AddErr(double err2)
{
  fArr[fN-kXYW+kW]=1./err2;
}
//_____________________________________________________________________________
const double *TPoliFitter::GetX(int i) const 	
{return fDat+i*kXYW;}
//_____________________________________________________________________________
      double *TPoliFitter::GetX(int i)
{return fDat+i*kXYW;}
//_____________________________________________________________________________
void TPoliFitter::Clear(const char *)
{
  memset(fBeg,0,fEnd-fBeg+1);
  fArr.Reset();
  fDat = fArr.GetArray();
}
//_____________________________________________________________________________
void TPoliFitter::Print(const char*) const
{
  Info("Print","NPoints %d ",fN/kXYW);
  TPolinom::Print();
}
//_____________________________________________________________________________
void TPoliFitter::Prepare()
{
  int need = fN+(fNP+1) + (fNP+1)*(fNP+2)/2;
  if (need > fArr.GetSize()) fArr.Set(need);
  fDat = fArr.GetArray();
  fC   = fDat+fN; fP = fC+fNP+1;
  TCL::vzero(fC,(fNP+1) + (fNP+1)*(fNP+2)/2);
  if (!fWtot) {
    for (int l=0;l<fN;l+=kXYW) {fWtot+=fDat[l+kW];}
    for (int l=0;l<fN;l+=kXYW) {fDat[l+kW]/=fWtot;}
  }
  fP[0]=1;
  int lNew = 1,lLst=0,pLst=0,lOld,pOld;
  for (int pNew=1;pNew<=fNP;) {
    TCL::vzero(fC,pNew+1);
    for (int l=0;l<fN;l+=kXYW) {//Data loop
      double x = fDat[l+kX],wt=fDat[l+kW];
      if (wt<=0) continue;
      double yNew = Eval(x,pLst,fP+lLst)*x;
      fC[pNew]+=yNew*yNew*wt;
      yNew*=wt;
      for (lOld = lLst,pOld=pLst;pOld>=0;lOld-=pOld,pOld--) {//Old loop
        fC[pOld]+= yNew * TPolinom::Eval(x,pOld,fP+lOld); 
      }//end old loop
    }//end Data loop
//	New ort polinom    	
    fP[lNew]=0;
    TCL::ucopy(fP+lLst,fP+lNew+1,pLst+1);
    for (lOld = lLst,pOld=pLst;pOld>=0;lOld-=pOld,pOld--) {//Old loop
      TCL::vlinco(fP+lNew,1.,fP+lOld,-fC[pOld],fP+lNew,pOld+1);
    }
    double nor = fC[pNew]-TCL::vdot(fC,fC,pNew);
    nor = sqrt(nor);
    TCL::vscale(fP+lNew,1./nor,fP+lNew,pNew+1);
    lLst = lNew; pLst=pNew;
    lNew += pNew+1; pNew++;
  }
}  
//_____________________________________________________________________________
double TPoliFitter::Fit()   
{
  Prepare();
  int lp,np;

  TCL::vzero(fC,fNP+1);
  fChi2=0;
  for (int l=0;l<fN;l+=kXYW) {//Data loop
    double x = fDat[l+kX];
    double y = fDat[l+kY];
    double w = fDat[l+kW];
    if (w<=0) continue;
    fChi2 += y*y*w;
    for (lp=0,np=0;np<=fNP; lp+=++np ) {
      fC[np]+=Eval(x,np,fP+lp)*y*w;
    }
  }
  TCL::vzero(fCoe,fNP+1);
  for (lp=0,np=0;np<=fNP; np++, lp+=np ) {
    fChi2-=fC[np]*fC[np];
    TCL::vlinco(fCoe,1.,fP+lp,fC[np],fCoe,np+1);
  }
  fChi2*=fWtot;
  fNdf = fNuse-(fNP+1);
  if (fNdf>0) fChi2/=fNdf;
  return fChi2;
}
//_____________________________________________________________________________
void   TPoliFitter::MakeErrs()
{
  delete fEmx;
  int n = (fNP+1)*(fNP+2)/2;
  fEmx = new double[n];
  TCL::trsmul(fP,fEmx,fNP+1);
//TCL::trsmlu(fP,fEmx,fNP+1);
  TCL::vscale(fEmx,1./fWtot,fEmx,n);
}
//_____________________________________________________________________________
double TPoliFitter::EvalChi2() 
{
  double Xi2=0; int n=0;
  for (int l=0;l<fN;l+=kXYW) {//Data loop
    double x = fDat[l+kX];
    double y = fDat[l+kY];
    double w = fDat[l+kW];
    if (w<=0) continue;
    double ye = Eval(x);
    Xi2+= (y-ye)*(y-ye)*w;
    n++;
  }
  Xi2*=fWtot;
  if (fNdf) Xi2/=fNdf;
  return Xi2;
}
//_____________________________________________________________________________
void TPoliFitter::FixY0(double y0) 
{
   assert(fEmx);
   double lamda = (y0-fCoe[0])/fEmx[0];
   double *e = fEmx;
   for (int i=0,n=1;i<=fNP;i++,e+=n++) {fCoe[i]+= e[0]*lamda;}
   TCL::trsinv(fEmx,fEmx,fNP+1);
   e = fEmx;
   for (int i=0,n=1;i<=fNP;i++,e+=n++) {e[0]=0;}
   fEmx[0]=1;
   TCL::trsinv(fEmx,fEmx,fNP+1);
   fEmx[0]=0;
   fNdf++;
}
//_____________________________________________________________________________
void TPoliFitter::DCoeDy(int iy,double *dcoe)
{
//      derivative dCoe/dY[idat]
  TCL::vzero(dcoe,fNP+1);
  TArrayD ac(fNP+1); double *c=ac.GetArray();
  int l = iy*kXYW;
  double x = fDat[l+kX];
  double w = fDat[l+kW];
  if (w<=0) return;
  for (int lp=0,np=0;np<=fNP; lp+=++np ) {
    c[np]=Eval(x,np,fP+lp)*w;
  }
  for (int lp=0,np=0;np<=fNP; np++, lp+=np ) {
    TCL::vlinco(dcoe,1.,fP+lp,c[np],dcoe,np+1);
  }
}
//_____________________________________________________________________________
double TPoliFitter::EvalOrt(int idx,double x) const
{
  int lp = (idx*(idx+1))/2;
  return Eval(x,idx,fP+lp);
}  
//_____________________________________________________________________________
//_____________________________________________________________________________
#include "TCanvas.h"
#include "TH1F.h"
#include "TSystem.h"
#include "TRandom.h"
//_____________________________________________________________________________
void TPoliFitter::Test(int kase)   
{
static TCanvas* myCanvas=0;
static TH1F *hh[5]={0,0,0,0,0};
static const char *hNams[]={"dA0","dA1","dA2","dY","Xi2","Xi2E",0};
  if(!myCanvas)  myCanvas=new TCanvas("C1","",600,800);
  myCanvas->Clear();
  myCanvas->Divide(1,6);

  for (int i=0;i<6;i++) { 
    int low = (i>=4)? 0:-5;
    delete hh[i]; hh[i]= new TH1F(hNams[i],hNams[i],100,low,5);
    myCanvas->cd(i+1); hh[i]->Draw();
  }



  double A[3]={1,2,3},B[3]={1,2,3};
  if (kase==1) {
    B[0] = A[0]+ 5*(A[1]+5*A[2]);
    B[1] = A[1] + 2*A[2]*5;
    B[2] = A[2];
  }

  double y5 = B[0] + 5.5*(B[1]+5.5*B[2]);
  for (int ievt=0;ievt <10000; ievt++) {
  
    TPoliFitter pf(2);
    int nPts=20;
    for (double x=0;x<nPts;x++) {
      double y = A[0]+x*(A[1]+x*A[2]);
      double dy = gRandom->Gaus(0,0.1);
      pf.Add(x,y+dy,0.1*0.1);
    }
    double Xi2 = pf.Fit();
    hh[4]->Fill(Xi2);
    pf.MakeErrs();
    if (kase==1) { pf.Move(5)    ;}
    if (kase==2) { pf.FixY0(A[0]);}
    double Xi2E = pf.EvalChi2();
    if (kase==2) Xi2E*=(nPts-3.)/(nPts-2.);
    hh[5]->Fill(Xi2E);


    const double *c = pf.Coe();
//    printf("Xi2 = %g Coe= %g %g %g\n",Xi2,c[0],c[1],c[2]);
    double del = pf.Eval(5.5)-y5;
    del /= sqrt(pf.Evrr(5.5));
    hh[3]->Fill(del);
    for (int i=0;i<3;i++) {
      del = (c[i]-B[i])/sqrt(pf.GetEmx(i,i)+1e-10);  
      hh[i]->Fill(del);
    }
  }

  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 
}
//_____________________________________________________________________________
void TPoliFitter::TestCorr()   
{
static TCanvas* myCanvas=0;
static TH1F *hh[6]={0,0,0,0,0,0};
static const char *hNams[]={"C01","C01-","C02","C02-","C12","C12-",0};
  if(!myCanvas)  myCanvas=new TCanvas("C1","",600,800);
  myCanvas->Clear();
  myCanvas->Divide(1,6);

  for (int i=0;i<6;i++) { 
    delete hh[i]; hh[i]= new TH1F(hNams[i],hNams[i],100,-1,1);
    myCanvas->cd(i+1); hh[i]->Draw();
  }



  double A[3]={1,2,3},B[3]={1,2,3};

  for (int ievt=0;ievt <1000; ievt++) {
  
    TPoliFitter pf(2);
    for (double x=0;x<10;x++) {
      double y = A[0]+x*(A[1]+x*A[2]);
      double dy = gRandom->Gaus(0,0.1);
      pf.Add(x,y+dy,0.1*0.1);
    }
    double Xi2 = pf.Fit(); if (Xi2){};
    pf.MakeErrs();

    const double *c = pf.Coe();
    int ih = 0;
    for (int i=0;i<3;i++) {
      double deli = (c[i]-B[i]);
//      double diai = pf.GetEmx(i,i);
    for (int j=i+1;j<3;j++) {
      double delj = (c[j]-B[j]);
//      double diaj = pf.GetEmx(j,j);
//       hh[ih+0]->Fill(deli*delj/sqrt(diai*diaj));
//       hh[ih+1]->Fill((pf.GetEmx(i,j))/sqrt(diai*diaj));
      hh[ih+0]->Fill(deli*delj*100);
      hh[ih+1]->Fill((deli*delj-pf.GetEmx(i,j))*100);
      ih+=2;
    } }
  }

  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 
}
//_____________________________________________________________________________
void TPoliFitter::Dest(int kase)   
{
if (kase){};
static TCanvas* myCanvas=0;
static TH1F *hh[5]={0,0,0,0,0};
static const char *hNams[]={"Der0","Der1","Der2",0};
  if(!myCanvas)  myCanvas=new TCanvas("C1","",600,800);
  myCanvas->Clear();
  myCanvas->Divide(1,3);

  for (int i=0;i<3;i++) { 
    delete hh[i]; hh[i]= new TH1F(hNams[i],hNams[i],100,-1,1);
    myCanvas->cd(i+1); hh[i]->Draw();
  }

  double A[3]={1,2,3};

  for (int ievt=0;ievt <1000; ievt++) {
  
    TPoliFitter pf(2);
    int nPts=20;
    for (double x=0;x<nPts;x++) {
      double y = A[0]+x*(A[1]+x*A[2]);
      double dy = gRandom->Gaus(0,0.1);
      pf.Add(x,y+dy,0.1*0.1);
    }
    double Xi2 = pf.Fit();
    pf.MakeErrs();
    double coe[3],doe[3];
    TCL::ucopy(pf.Coe(),coe,3);
    
    for (int ip=0;ip<nPts;ip++) {
      TPoliFitter pff(pf);
      pf.DCoeDy(ip,doe);
      double y=pf.GetX(ip)[1];
      double dy = 0.1*y;
      pff.GetX(ip)[1]=y+dy;
      Xi2 = pff.Fit();
      for (int ih=0;ih<3;ih++) {
        double tst = (pff.Coe()[ih]-coe[ih])/dy;
        tst = (tst-doe[ih])/(fabs(doe[ih])+1e-10);
	hh[ih]->Fill(tst);
      }
    }  
  }

  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 
}





  
  
  
  
