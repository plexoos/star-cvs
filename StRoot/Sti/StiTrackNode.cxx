#include <stdio.h>
#include <stdlib.h>
#include "StiTrackNode.h"
#include "TMath.h"
#include "TRMatrix.h"
#include "TRVector.h"
#include "StMessMgr.h"
#include "StiUtilities/StiDebug.h"

#define NICE(a) ( ((a) <= -M_PI)? ((a)+2*M_PI) :\
                  ((a) >   M_PI)? ((a)-2*M_PI) : (a))

int StiTrackNode::mgFlag=0;
  static const int idx66[6][6] =
  {{ 0, 1, 3, 6,10,15},{ 1, 2, 4, 7,11,16},{ 3, 4, 5, 8,12,17}
  ,{ 6, 7, 8, 9,13,18},{10,11,12,13,14,19},{15,16,17,18,19,20}};
static const double MAX1ERR[]={3,3,3,0.1,3.,0.1};
static const double MAX2ERR[]={MAX1ERR[0]*MAX1ERR[0]
                              ,MAX1ERR[1]*MAX1ERR[1]
                              ,MAX1ERR[2]*MAX1ERR[2]
                              ,MAX1ERR[3]*MAX1ERR[3]
                              ,MAX1ERR[4]*MAX1ERR[4]
                              ,MAX1ERR[5]*MAX1ERR[5]};

static const double MIN1ERR[]={1e-4,1e-4,1e-4,1e-5,1e-3,1e-5};
static const double MIN2ERR[]={MIN1ERR[0]*MIN1ERR[0]
                              ,MIN1ERR[1]*MIN1ERR[1]
                              ,MIN1ERR[2]*MIN1ERR[2]
                              ,MIN1ERR[3]*MIN1ERR[3]
                              ,MIN1ERR[4]*MIN1ERR[4]
                              ,MIN1ERR[5]*MIN1ERR[5]};
static const double recvCORRMAX  = 0.99999;
static const double chekCORRMAX  = 0.99999;
static const double baddCORRMAX  = 1.1;
static double MAXPARS[]={500,500,500,3.15,100,100};

//______________________________________________________________________________
void StiTrackNode::errPropag6( double G[21],const double F[6][6],int nF )
{
  enum {NP=6,NE=21};

  double g[NE];      memcpy(g,    G,sizeof( g));
  double fg[NP][NP]; memset(fg[0],0,sizeof(fg));

  double myF[6][6];  memcpy(myF[0],F[0],sizeof(fg));
  for (int i=0;i<NP;i++) {myF[i][i]+=1.;}

  for (int i=0;i<nF;i++) {
  for (int j=0;j<nF;j++) {
    if (!F[i][j]) 	continue;
    for (int k=0;k<NP;k++) {
      int jk = idx66[j][k];
      if (!g[jk])	continue;
      fg[i][k] += F[i][j]*g[jk]; 
  }}}

  for (int i=0;i<NP;i++) {        
  for (int k=i;k<NP;k++) {        
    int ik = idx66[i][k];
    double s = 0; 
    for (int j=0;j<NP;j++) {
      if (!F[k][j])	continue;
      s += fg[i][j]*F[k][j];
    }
    G[ik] += (s + fg[i][k] + fg[k][i]);
  }}

}       

//______________________________________________________________________________
void StiHitContino::reset()
{
   memset(this,0,sizeof(StiHitContino));
   mChi2[0]=1e61;
}
//______________________________________________________________________________
void StiHitContino::add(StiHit *hit,double chi2,double detr)
{
   int i=0;
   for (;i<kMaxSize;i++) {
     if (!mHits[i]) 		break;
     if (chi2 > mChi2[i]) 	continue;
     for (int jr = kMaxSize-2;jr>=i;jr--) 
       {mHits[jr+1]=mHits[jr]; mChi2[jr+1]=mChi2[jr];mDetr[jr+1]=mDetr[jr];}
     				break;
   }
   if (i>=kMaxSize) 		return;
   mHits[i]=hit; mChi2[i]=chi2;mDetr[i]=detr;
}
//______________________________________________________________________________
void StiHitContino::print(const char* tit) const
{
  if (!tit || !*tit) tit ="print()";
  int n=getNHits();	
  LOG_DEBUG << Form(" ****	StiHitContino::%s nHits=%d",tit,n)<< endm;
  for (int i=0;i<n;i++) {
    LOG_DEBUG << Form("%3d - hit=%p chi2 = %g",i,(void*)mHits[i],mChi2[i]);}
    LOG_DEBUG << endm;
}
//______________________________________________________________________________
int StiHitContino::getNHits() const
{ int n=0; for(int i=0;i<kMaxSize;i++) {if (mHits[i]) n++;}; return n;}	
//______________________________________________________________________________
int StiTrackNode::cylCross(const double Xp[2],const double Dp[2], const double Rho
                          ,const double r    ,int dir,            double out[2][3])
{
//Circles crossing
//==========================================================

// Rho -curvature
// r - cyl radius
//L - distance between centers
//d - distance of crossing line d<r
// X0,Y0 start track with R
// Dx,Dy direction of it
// Nx,Ny = -Dy,Dx
// Cx,Cy = direction to center
/// 
// 
// r**2-d**2 == R**2-(L-d)**2
// r**2 == R**2- L**2 +2*L*d 
// 
// r**2 +L**2-R**2=  2*L*d
// 
// d = (r**2 +L**2-R**2)/(2*L)
// 
// L**2 = (X0+N*R)**2 = X0**2+R**2 +2*(X0*N)*R
// L**2-R**2 = X0**2+ 2*(X0*N)*R
// 
// 
// d = (r**2 +X0**2+ 2*(X0*N)*R)/(2*sqrt(X0**2+R**2 +2*(X0*N)*R)
// d = (r**2 +X0**2+ 2*(X0*N)*R)/(2*sqrt(X0**2+R**2 +2*(X0*N)*R)
// 
// 
static int nCall=0;nCall++;
StiDebug::Break(nCall);


int sRho = (Rho<0) ? -1:1;
double aRho = fabs(Rho), rr=r*r,d=0;
TVector3 D(Dp[0],Dp[1],0.),X(Xp[0],Xp[1],0.);
TVector3 C,Cd,Cn,N;
double XX,XN,L;
N[0] = -D[1]; N[1] = D[0];
XX = X*X; XN = X*N;

double LLmRR = XX*aRho+2*XN*sRho;
double LL = LLmRR*aRho+1; L = sqrt(LL);
d = (rr*aRho+LLmRR)/(2*L);

double p = ((r-d)*(r+d));
if (p<=0) return 0;
p = sqrt(p);

C = X*aRho+N*sRho;
Cd = C.Unit(); Cn[0] = -Cd[1];   Cn[1] = Cd[0];

TVector3 Out[2];
for (int ix = 0;ix<2; ix++) {
  Out[ix] = Cd*d + Cn*p; p = -p;
}

for (int ix = 0;ix<2; ix++) {
  double len = (Out[ix]-X).Mag();
  if (len*aRho > 0.1) len = 2*asin(0.5*len*aRho)/aRho;
  if ((Out[ix]-X).Dot(D)<0) len = -len;

  double tst = (X-Out[ix])*D;
  if (dir) tst = -tst;
//VP  if (tst<0) len = M_PI*2*aR-len;
  out[ix][2] = len; 
  out[ix][0] = Out[ix][0];
  out[ix][1] = Out[ix][1];
}
  if (fabs(out[0][2])>fabs(out[1][2])) { 	//wrong order
    for (int j=0;j<3;j++)  { 
      double t=out[0][j]; 
      out[0][j] = out[1][j]; 
      out[1][j] = t; 
  } }



  for (int i=0;i<2;i++) {
//  printf("x=%g y=%g len=%g\n",out[i][0],out[i][1],out[i][2]);
  double dif = (Out[i]*aRho-C).Mag()-1.;
//  printf("SolAcc=%g\n",dif);
  assert(fabs(dif)<1e3);
  dif = (Out[i]).Mag()/r-1;
//  printf("SolAcc=%g\n",dif);
  assert(fabs(dif)<1e3);
  }
  return 2;
}


//______________________________________________________________________________
 
double StiTrackNode::sinX(double x)
{
  double x2 = x*x;
  if (x2>0.5) return (sin(x)-x)/x2/x;
  double nom = -1./6;
  double sum = nom;
  for (int it=4;1;it+=2) {
    nom = -nom*x2/(it*(it+1));
    sum +=nom;
    if (fabs(nom) <= 1e-10*fabs(sum)) break;
  }
  return sum;
} 
//______________________________________________________________________________
void StiTrackNode::mult6(double Rot[kNPars][kNPars],const double Pro[kNPars][kNPars]) 
{
  double T[kNPars][kNPars];

  if (!Rot[0][0]) {memcpy(Rot[0],Pro[0],sizeof(T)); return;}

  memcpy(T[0],Pro[0],sizeof(T));

  for (int i=0;i<kNPars;i++) {
  for (int j=0;j<kNPars;j++) {
    if(!Rot[i][j]) continue;
    for (int k=0;k<kNPars;k++) {
      if (!Pro[k][i]) continue;
      T[k][j] += Pro[k][i]*Rot[i][j];
  }}} 
  for (int i=0;i<kNPars;i++) {
  for (int k=0;k<kNPars;k++) {
    Rot[i][k] += T[i][k];
}}
}     
//______________________________________________________________________________
double StiTrackNode::getRefPosition() const
{
  if(!_detector) {
    return x();
  } else {
    StiPlacement * place = _detector->getPlacement();
    assert(place);
    return place->getLayerRadius();
  }
}
//______________________________________________________________________________
  double StiTrackNode::getLayerAngle()  const
{
  assert(_detector);
  StiPlacement * place = _detector->getPlacement();
  assert(place);
  return place->getLayerAngle();
}

//______________________________________________________________________________
double StiNodeErrs::operator()(int i,int j) const
{
  return A[idx66[i][j]];
}
//______________________________________________________________________________
StiNodeErrs &StiNodeErrs::merge(double wt,StiNodeErrs &other)
{
   double wt0 = 1.-wt;
   for (int i=0;i<kNErrs;i++) {A[i] = wt0*A[i] + wt*other.A[i];}
//assert(sign()>0); ///

   return *this;
}
//______________________________________________________________________________
void StiNodeErrs::get00(      double *a) const
{
   memcpy(a,A,6*sizeof(double));
}
//______________________________________________________________________________
void StiNodeErrs::set00(const double *a) 
{
   memcpy(A,a,6*sizeof(double));

}
//______________________________________________________________________________
void StiNodeErrs::get10(double *a) const
{
// 0: 00
// 1: 10 11 
// 3: 20 21 22
// 6: 30 31 32 33
//10: 40 41 42 43 44
//15: 50 51 52 53 54 55
  memcpy(a+0,A+ 6,3*sizeof(double));
  memcpy(a+3,A+10,3*sizeof(double));
  memcpy(a+6,A+15,3*sizeof(double));
}
//______________________________________________________________________________
void StiNodeErrs::set10(const double *a) 
{
  memcpy(A+ 6,a+0,3*sizeof(double));
  memcpy(A+10,a+3,3*sizeof(double));
  memcpy(A+15,a+6,3*sizeof(double));
}
//______________________________________________________________________________
void StiNodeErrs::get11(      double *a) const
{
  memcpy(a+0,A+ 9,1*sizeof(double));
  memcpy(a+1,A+13,2*sizeof(double));
  memcpy(a+3,A+18,3*sizeof(double));
}
//______________________________________________________________________________
void StiNodeErrs::set11(const double *a) 
{
  memcpy(A+ 9,a+0,1*sizeof(double));
  memcpy(A+13,a+1,2*sizeof(double));
  memcpy(A+18,a+3,3*sizeof(double));
}
//______________________________________________________________________________
void StiNodeErrs::zeroX() 
{ 
  for (int i=0;i<kNPars;i++) {A[idx66[i][0]]=0;}
}
//____________________________________________________________
void StiNodeErrs::rotate(double alpha,const StiNodePars &pars)
{
// it is rotation by -alpha

  double ca = cos(alpha),sa=sin(alpha);
  double dX = (fabs(pars._cosCA)<1e-5)? 1e-5:pars._cosCA; 
  double dYdX = pars._sinCA/dX;
  double dZdX = pars.tanl()/dX;

  double F[6][6]= {{         -1,            0,0,0,0,0}
                  ,{-ca*dYdX-sa,-sa*dYdX+ca-1,0,0,0,0}
                  ,{-ca*dZdX   ,-sa*dZdX     ,0,0,0,0}
                  ,{          0,            0,0,0,0,0}
                  ,{          0,            0,0,0,0,0}
                  ,{          0,            0,0,0,0,0}};
  		  
  StiTrackNode::errPropag6( A,F,kNPars );
  for (int i=0,li=0;i<kNPars ;li+=++i) {
    assert(fabs(A[li+0]) <1e-6);
  }

}
//______________________________________________________________________________
void StiNodeErrs::recov() 
{
static int nCall = 0; nCall++;
StiDebug::Break(nCall);

//   double s = sign(); ///??? 
//   if (s<0) printf("##################### StiNodeErrs::recov() sign=%g\n",s);
  int i0=1,li0=1,isMod=0;
  if (_cXX>0) {i0=0;li0=0;}

   double dia[kNPars],fak[kNPars]={1,1,1,1,1,1},corrMax=1;;
   int isTouched[kNPars]={0};
   for (int i=i0,li=li0;i<kNPars ;li+=++i) {
     double &aii = A[li+i];
     if (aii < MIN2ERR[i]) aii = MIN2ERR[i];
     if (aii > MAX2ERR[i]) { fak[i] = sqrt(MAX2ERR[i]/aii); aii = MAX2ERR[i]; isMod=2014;}
     dia[i] = aii;
     for (int j=i0;j<i;j++) {
       double &aij = A[li+j];
       if (isMod) aij*=fak[i]*fak[j];
       if (aij*aij <=    dia[i]*dia[j]*chekCORRMAX) continue;
       double qwe = aij*aij/(dia[i]*dia[j]);
       if (corrMax>=qwe) continue;
       corrMax=qwe; isTouched[i]=1; isTouched[j]=1;
   } } 
   if (corrMax<=1) return;
   corrMax = sqrt(corrMax/recvCORRMAX);
   
   for (int i=i0,li=li0;i<kNPars ;li+=++i) {
     for (int j=i0;j<i;j++) {
       A[li+j]/=corrMax;
   } } 
//    while (sign()<=0) {
//     for (int i=i0,li=li0;i<kNPars ;li+=++i) {
//       for (int j=i0;j<i;j++) {
//         A[li+j]*=0.9;
//    } } }

}
//______________________________________________________________________________
void StiNodeErrs::print() const
{
   const double *d = A;
   for (int n=1;n<=6;n++) {
     LOG_DEBUG << Form("%d - ",n);
     for (int i=0;i<n;i++){LOG_DEBUG << Form("%g\t",*(d++));}; LOG_DEBUG << endm;
   }  
}     

//______________________________________________________________________________
int StiNodeErrs::check(const char *pri) const
{
// assert(sign()>0); ///
  int i=-2008,j=2009,kase=0;
  double aii=-20091005,ajj=-20101005,aij=-20111005;
  int i0=0; if (!_cXX) i0 = 1;
  for (i=i0;i<kNPars;i++) {
    aii = A[idx66[i][i]];
    if (aii<0) {kase = 1; break;}	//Diagonal must be positive
  }
  if (kase) goto RETN;
  for (i=i0;i<kNPars;i++) {
    aii = A[idx66[i][i]];
    for (j=i+1;j<kNPars ;j++) {
      ajj = A[idx66[j][j]];
      if (ajj<=0) continue;
      aij = A[idx66[i][j]];
      if ((aij*aij)> aii*ajj) {kase = 2; break;}
    }
    if (kase) break;
  }  
RETN:
//  if (!kase && sign()<0 ) kase = 3;  //It is rather slow. For debug only
  if ((kase == 3) && (!pri || !pri[0])) pri = "QWERTY";

  if (!kase) 	return kase;
  if (!pri ) 	return kase;
  switch(kase) {
  
    case 1: LOG_DEBUG << Form("StiNodeErrs::check(%s) FAILED: Negative diagonal %g[%d][%d]",pri,aii,i,i)<< endm;  
          break;
    case 2: LOG_DEBUG << Form("StiNodeErrs::check(%s) FAILED: Correlation too big %g[%d][%d]>%g"
                ,pri,aij,i,j,sqrt(aii*ajj))<<endm; 	  
          break;
    case 3: LOG_DEBUG << Form("StiNodeErrs::check(%s) FAILED: Non Positive matrix",pri)<<endm;  
  }    
// assert(sign()>0); /// 
  return kase;
}  
//____________________________________________________________
double StiNodeErrs::sign() const
{
   enum {n=kNPars};
   double ans=3e33;
   const double *a = A;
   double *xx = (double *)A;
   double save = *xx; if (!save) *xx = 1;
   double B[kNErrs];
         double *b = B;
   // trchlu.F -- translated by f2c (version 19970219).
   //
   //see original documentation of CERNLIB package F112 

   /* Local variables */
   int ipiv, kpiv, i__, j;
   double r__, dc;
   int id, kd;
   double sum;


   /* CERN PROGLIB# F112    TRCHLU          .VERSION KERNFOR  4.16  870601 */
   /* ORIG. 18/12/74 W.HART */


   /* Parameter adjuTments */
   --b;    --a;

   /* Function Body */
   ipiv = 0;

   i__ = 0;

   do {
      ++i__;
      ipiv += i__;
      kpiv = ipiv;
      r__ = a[ipiv];

      for (j = i__; j <= n; ++j) {
         sum = 0.;
         if (i__ == 1)       goto L40;
         if (r__ == 0.)      goto L42;
         id = ipiv - i__ + 1;
         kd = kpiv - i__ + 1;

         do {
            sum += b[kd] * b[id];
            ++kd;   ++id;
         } while (id < ipiv);

L40:
         sum = a[kpiv] - sum;
L42:
         if (j != i__) b[kpiv] = sum * r__;
         else {
            if (sum<ans) ans = sum;
            if (sum<=0.) goto RETN;
            dc = sqrt(sum);
            b[kpiv] = dc;
            if (r__ > 0.)  r__ = (double)1. / dc;
         }
         kpiv += j;
      }

   } while  (i__ < n);

RETN: *xx=save; 
   return ans;
} /* trchlu_ */
//______________________________________________________________________________
int StiNodeErrs::nan() const
{
  for (int i=0; i<kNPars;i++) {
    if (!finite(A[i])) return 100+i;
  }
  return 0;
}

//______________________________________________________________________________
int StiNodePars::check(const char *pri) const
{

  int ierr=0;
//?? temp test
  assert(fabs(_cosCA) <=1 && fabs(_sinCA)<=1);
  double tmp = (fabs(curv())<1e-6)? 0: curv()-ptin()*hz();
//		1km for 1GeV is a zero field
//  assert(fabs(_hz)<1e-5 || fabs(tmp)<= 1e-3*fabs(_curv));
  if (fabs(hz())>=1e-5 && fabs(tmp)> 1e-3*fabs(curv()))    {ierr=1313; goto FAILED;}
  for (int i=0;i<kNPars;i++) {if (fabs(P[i]) > MAXPARS[i]) {ierr = i+1 ; break;}} 
  if(ierr) goto FAILED;
  for (int i=-2;i<0;i++)     {if (fabs(P[i]) > 1.)         {ierr = i+12; break;}} 
FAILED: 
  if (!ierr) return ierr;
  if (!pri ) return ierr;
  LOG_DEBUG << Form("StiNodePars::check(%s) == FAILED(%d)",pri,ierr)<<endm;
  print();
  return ierr;
} 
//______________________________________________________________________________
StiNodePars &StiNodePars::merge(double wt,StiNodePars &other)
{
   double wt0 = 1.-wt;
   for (int i=0;i<kNPars+1;i++) {P[i] = wt0*P[i] + wt*other.P[i];}
   ready();
   return *this;
}
//______________________________________________________________________________
StiNodePars &StiNodePars::operator=(const StiNodePars &fr)
{
  assert(fabs(fr._sinCA)<=1);
  assert(fabs(fr._cosCA)<=1);
  memcpy (this,&fr,sizeof(fr));
  return *this;
}
//______________________________________________________________________________
void StiNodePars::rotate(double alpha)
{
// actually it is rotation by -alpha

  double xt1=x(); 
  double yt1=y(); 
  double cosCA0 = _cosCA;
  double sinCA0 = _sinCA;

  double ca = cos(alpha);
  double sa = sin(alpha);

  x() =  xt1*ca + yt1*sa;
  y() = -xt1*sa + yt1*ca;
  _cosCA =  cosCA0*ca+sinCA0*sa;
  _sinCA = -cosCA0*sa+sinCA0*ca;
  double nor = 0.5*(_sinCA*_sinCA+_cosCA*_cosCA +1);
  _cosCA /= nor;
  _sinCA /= nor;
  eta()= NICE(eta()-alpha); 
}   
//______________________________________________________________________________
int StiNodePars::nan() const
{
  for (int i=-2; i<=kHz;i++) {
    if (!finite(P[i])) return 100+i;
  }
  return 0;
}
//______________________________________________________________________________
void StiNodePars::print() const
{
static const char* tit[]={"cosCA","sinCA","X","Y","Z","Eta","Ptin","TanL","Curv",0};
  for (int i=-2;i<kNPars+1;i++) {LOG_DEBUG << Form("%s = %g, ",tit[i+2],P[i]);}
  LOG_DEBUG << endm;
}   
//______________________________________________________________________________
void StiHitErrs::rotate(double angle)
{
  double t[2][2];
  t[0][0] = cos(angle); t[0][1] = -sin(angle);
  t[1][0] = -t[0][1]  ; t[1][1] = t[0][0];
  double r[3];
  TCL::trasat(t[0],&hXX,r,2,2);
  TCL::ucopy(r,&hXX,3);
}
//______________________________________________________________________________
void StiNode2Pars::set(const StiNodePars &pars,StiNodeErrs &errs)
{
  mPar[0]=pars.y(); mPar[1]=pars.z();
  mErr[0]=errs._cYY;
  mErr[1]=errs._cZY;
  mErr[2]=errs._cZZ;
}

