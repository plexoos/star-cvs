//*-- Author : Alexandre Suaide and Marcia Moura
// 
// $Id: StEmcFit.cxx,v 1.1 2001/10/17 13:51:31 suaide Exp $
// $Log: StEmcFit.cxx,v $
// Revision 1.1  2001/10/17 13:51:31  suaide
// new modifications to work with real data
//
// Revision 1.13  2000/05 16:07:01  
// Add README
//
#include "StEmcFit.h"
#include <iostream.h>
#include <math.h>
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

ClassImp(StEmcFit)

//_____________________________________________________________________________
StEmcFit::StEmcFit()
{
  ClearAll();
}
//_____________________________________________________________________________
StEmcFit::~StEmcFit()
{
}
//_____________________________________________________________________________
void StEmcFit::ClearAll()
{
  mfit=0;
  chisq=0;
  chisqr=0;
  temp=0;
  ma=0;
  ndata=0;
  ochisq=0;
  alamda=-1;
}
//_____________________________________________________________________________
void StEmcFit::gaussj(Int_t n, Int_t m)
{
  Int_t i,icol=0,irow=0,j,k,l,ll;
  Float_t big,dum,pivinv,temp;
  Int_t indxc[5000],indxr[5000],ipiv[5000];
  for (j=1;j<=n;j++) ipiv[j]=0;
  for (i=1;i<=n;i++) 
  { 
    big=0.0;
    for (j=1;j<=n;j++) 
      if (ipiv[j] != 1)
        for (k=1;k<=n;k++) 
        {
          if (ipiv[k] == 0) 
          {
            if (fabs(covar[j][k]) >= big) 
            {
              big=fabs(covar[j][k]);
              irow=j;
              icol=k;
            }
          } 
          else if (ipiv[k] > 1) {/*cout <<"gaussj: Singular Matrix-1"<<endl;*/ return;}
        }
    ++(ipiv[icol]);
    if (irow != icol) 
    {
      for (l=1;l<=n;l++) SWAP(covar[irow][l],covar[icol][l])
      for (l=1;l<=m;l++) SWAP(oneda[irow][l],oneda[icol][l])
    }
    indxr[i]=irow;  
    indxc[i]=icol;
    if (covar[icol][icol] == 0.0) {/*cout <<"gaussj: Singular Matrix-2"<<endl;*/ return;}
    pivinv=1.0/covar[icol][icol];
    covar[icol][icol]=1.0;
    for (l=1;l<=n;l++) covar[icol][l] *= pivinv;
    for (l=1;l<=m;l++) oneda[icol][l] *= pivinv;
    for (ll=1;ll<=n;ll++) 
      if (ll != icol) 
      { 
        dum=covar[ll][icol];
        covar[ll][icol]=0.0;
        for (l=1;l<=n;l++) covar[ll][l] -= covar[icol][l]*dum;
        for (l=1;l<=m;l++) oneda[ll][l] -= oneda[icol][l]*dum;
      }
  }
  for (l=n;l>=1;l--) 
  {
    if (indxr[l] != indxc[l])
    for (k=1;k<=n;k++)
      SWAP(covar[k][indxr[l]],covar[k][indxc[l]]);
  } 
}
//_____________________________________________________________________________
void StEmcFit::covsrt(Int_t mfit)
{
  Int_t i,j,k;
  for (i=mfit+1;i<=ma;i++)
    for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
  k=mfit;
  for (j=ma;j>=1;j--) 
  {
    if (ia[j]) 
    {
      for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j])
      for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
      k--;
    }
  }
}
//_____________________________________________________________________________
void StEmcFit::mrqcof()
{
  Int_t i,j,k,l,m,mfit=0;
  Float_t ymod,wt,sig2i,dy;
  for (j=1;j<=ma;j++)
    if (ia[j]) mfit++;
  for (j=1;j<=mfit;j++) 
  { 
    for (k=1;k<=j;k++) alpha[j][k]=0.0;
    beta[j]=0.0;
  }
  chisq=0.0;
  for (i=1;i<=ndata;i++) 
  { 
    
    if(functype==1) gaussians(x[i],&ymod);
    if(functype==2) gaussPlusExp(x[i],&ymod);
    if(functype==3) exponential(x[i],&ymod);
    
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;
    
    j=0;
    for (l=1;l<=ma;l++) 
    {
      if (ia[l]) 
      {
        wt=dyda[l]*sig2i;
        j++;k=0;
        for (m=1;m<=l;m++)
          if (ia[m]) alpha[j][++k] += wt*dyda[m];
        beta[j] += dy*wt;
      }
    }
    chisq += dy*dy*sig2i; 
  }
  for (j=2;j<=mfit;j++) 
    for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
}
//_____________________________________________________________________________
void StEmcFit::mrqmin()
{
  Int_t j,k,l;
  if (alamda < 0.0) 
  { 
    for (mfit=0,j=1;j<=ma;j++)
      if (ia[j]) mfit++;
    alamda=0.001;
    for (j=1;j<=ma;j++) atry[j]=a[j]; 
    mrqcof();
    ochisq=chisq;
    return;
  }
  for (j=1;j<=mfit;j++) 
  { 
    for (k=1;k<=mfit;k++) covar[j][k]=alpha[j][k];
    covar[j][j]=alpha[j][j]*(1.0+(alamda));
    oneda[j][1]=beta[j];
  }
  gaussj(mfit,1); 
  for (j=1;j<=mfit;j++) da[j]=oneda[j][1]; 
  if (alamda == 0.0) 
  { 
    covsrt(mfit);
    covsrt(mfit); 
    return;
  }
  j=0;
  for (l=1;l<=ma;l++) 
    if (ia[l]) atry[l]=a[l]+da[++j];
  mrqcof();
  if (chisq < ochisq) 
  { 
    alamda *= 0.1;
    ochisq=chisq;
    for (j=1;j<=mfit;j++) 
    {
      for (k=1;k<=mfit;k++) alpha[j][k]=covar[j][k];
      beta[j]=da[j];
    }
    for (l=1;l<=ma;l++) a[l]=atry[l];
  } 
  else 
  { 
    alamda *= 10.0;
    chisq=ochisq;
  }
}
//_____________________________________________________________________________
void StEmcFit::Fit()
{
  alamda=-1;
  Int_t co=0;
  loop:
    mrqmin();
    //cout <<"chi = "<<chisq<<"  alamda = "<<alamda<<endl;
    co++;
    if(co<1000 && alamda>1e-17 && alamda <1e37) goto loop;
  cout <<"FIT: Niteractions = "<<co<<"  alamda = "<<alamda<<endl;
  alamda=0;  
  mrqmin();
}
//_____________________________________________________________________________
void StEmcFit::Fit(Int_t max)
{
  alamda=-1;
  Int_t co=0;
  loop:
    mrqmin();
    //cout <<"chi = "<<chisq<<"  alamda = "<<alamda<<endl;
    co++;
    if(co<max && alamda>1e-17 && alamda <1e37) goto loop;
  cout <<"FIT: Niteractions = "<<co<<"  alamda = "<<alamda<<endl;
  alamda=0;  
  mrqmin();
}
//_____________________________________________________________________________
Float_t StEmcFit::Y(Float_t xx)
{
  Float_t ytemp;
  if(functype==1) gaussians(xx,&ytemp);
  if(functype==2) gaussPlusExp(xx,&ytemp);
  if(functype==3) exponential(xx,&ytemp);
  return ytemp;
}
//_____________________________________________________________________________
void StEmcFit::gaussians(Float_t x, Float_t *y)
/*y(x; a) is the sum of na/3 Gaussians (15.5.16). The amplitude, center, and width of the
Gaussians are stored in consecutive locations of a: a[i] = Bk , a[i+1] = Ek , a[i+2] = Gk ,
k = 1; :::; na/3. The dimensions of the arrays are a[1..na], dyda[1..na].*/
{
  Int_t i;
  Float_t fac,ex,arg;
  *y=0.0;
  for (i=1;i<=ma-1;i+=3) 
  {
    arg=(x-atry[i+1])/atry[i+2];
    ex=exp(-arg*arg);
    fac=atry[i]*ex*2.0*arg;
    *y += atry[i]*ex;
    dyda[i]=ex;
    dyda[i+1]=fac/atry[i+2];
    dyda[i+2]=fac*arg/atry[i+2];
  }
}
//_____________________________________________________________________________
void StEmcFit::gaussPlusExp(Float_t x, Float_t *y)
/*
  par 1 = amplitude
  par 2 = center
  par 3 = width
  par 4 = exp amplitude
  par 5 = decay cte
*/
{
  Float_t fac,ex,arg,arg2;
  *y=0.0;
// gaussian
    arg=(x-atry[2])/atry[3];
    ex=exp(-arg*arg);
    fac=atry[1]*ex*2.0*arg;
    *y += atry[1]*ex;
    dyda[1]=ex;
    dyda[2]=fac/atry[3];
    dyda[3]=fac*arg/atry[3];
// exponential
    arg2=atry[4]*exp(atry[5]*x);
    *y+=arg2;
    dyda[4]=exp(atry[5]*x);
    dyda[5]=arg2*x;
}
//_____________________________________________________________________________
void StEmcFit::exponential(Float_t x, Float_t *y)
/*
  par 1 = exp amplitude
  par 2 = decay cte
*/
{
  Float_t arg2;
  *y=0.0;
// exponential
    arg2=atry[1]*exp(atry[2]*x);
    *y+=arg2;
    dyda[1]=exp(atry[2]*x);
    dyda[2]=arg2*x;
}






