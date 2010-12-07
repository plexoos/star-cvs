#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "TCernLib.h"
#include "TMath.h"
#include "StvUtil/StvNodePars.h"
#include "Stv/StvToolkit.h"

  static const int idx66[6][6] =
  {{ 0, 1, 3, 6,10,15},{ 1, 2, 4, 7,11,16},{ 3, 4, 5, 8,12,17}
  ,{ 6, 7, 8, 9,13,18},{10,11,12,13,14,19},{15,16,17,18,19,20}};
static const double recvCORRMAX  = 0.99;
static const double chekCORRMAX  = 0.9999;

static double MAXTAN = 100;
//                              x   y   z  psi   pt  tan      cur
static double MAXNODPARS[]   ={555,555,555,6.66,111, MAXTAN+1, .1};
//                              h  z   a    l  ptin
static double MAXFITPARS[]   ={22,22, .5 , .5 ,20};
static double MAXFITERR[5]   ={.3,.3,0.03,0.03,1};
//____________________________________________________________
static double EmxSign(int n,const double *a); 



//______________________________________________________________________________ 
void Multiply(Mtx55D_t &res, const Mtx55D_t &A,const Mtx55D_t &B)
{
  memset(res[0],0,5*5*sizeof(res[0][0]));
  for (int j=0;j<5;j++) {
    const double *Bj = B[j];
    for (int i=0;i<5;i++) {
    const double &aij = A[i][j];
    if (!aij) continue;
          double *resi =res[i];
    for (int k=0;k<5;k++) {
      const double &bjk = Bj[k];
      if (!bjk) continue;
      resi[k] += aij*bjk;
  } } }
}
//______________________________________________________________________________ 
void Multiply(double *res, const Mtx55D_t &A,const double *B)
{
  for (int i=0;i<5;i++) {
    double S = 0;
    const double *Ai = A[i];
    for (int j=0;j<5;j++) {if (!Ai[j]) continue;S += Ai[j]*B[j];}
    res[i]=S;
  }
}
//______________________________________________________________________________ 
void Invert(Mtx55D_t &to, const Mtx55D_t &fr)
{
assert(0);
}
//______________________________________________________________________________ 
void Testik(const Mtx55D_t &tt)
{
   TMatrixD T(5,5,tt[0]);
   TMatrixD Tt(T); Tt.T();
   TMatrixD T1 = Tt*T;
   printf("Det = %g ============================\n",T.Determinant());
   T.Print();
   T1.Print();
}
#if 0
//______________________________________________________________________________ 
double StvTrackNode::sinX(double x)
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
void StvTrackNode::mult6(double Rot[kNPars][kNPars],const double Pro[kNPars][kNPars]) 
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
#endif //0
//______________________________________________________________________________
int StvNodePars::check(const char *pri) const
{

  int ierr=0;
//?? temp test
  double tmp = _curv - _ptin* _hz;
//		1km for 1GeV is a zero field
  if (fabs(_hz)>=1e-5 && fabs(tmp)> 1e-3*fabs(_curv)) {ierr=1001; 	   	goto FAILED;}
  for (int i=0;i<=kNPars;i++){if (fabs(P[i]) > MAXNODPARS[i]) {ierr = i+ 1;	goto FAILED;}} 

  for (int i=-2;i<0;i++)     {if (fabs(P[i]) > 1.0001)        {ierr = i+12;	goto FAILED;}} 
  tmp = fabs(cos(_psi)-_cosCA);
  ierr = 1002; if (tmp>1e-4) 							goto FAILED;
  tmp = fabs(sin(_psi)-_sinCA);
  ierr = 1003; if (tmp>1e-4) 							goto FAILED;
  ierr = 1004; if (fabs(_z) <= 1e-10) 						goto FAILED;
  return 0;
FAILED: 
  if (!pri ) return ierr;
  printf("StvNodePars::check(%s) == FAILED(%d)\n",pri,ierr);  print();
  return ierr;
} 
//______________________________________________________________________________
StvNodePars &StvNodePars::merge(double wt,StvNodePars &other)
{
   double wt0 = 1.-wt;
   for (int i=0;i<kNPars+1;i++) {P[i] = wt0*P[i] + wt*other.P[i];}
   ready();
   return *this;
}

//______________________________________________________________________________
void StvNodePars::set(const THelixTrack *th, double Hz)
{
  memcpy(&_x,th->Pos(),3*sizeof(_x));
  _psi = atan2(th->Dir()[1],th->Dir()[0]);
  double sinL = th->Dir()[2];
  double cosL = sqrt((1-sinL)*(1+sinL));
  _tanl = sinL/cosL;
  
  _cosCA = th->Dir()[0]/cosL;
  _sinCA = th->Dir()[1]/cosL;
  _curv = th->GetRho();
  _hz =  Hz;
   if (fabs(_curv) > fabs(1e-6*_hz)) 	{_ptin = _curv/_hz;              } 
   else 				{_ptin = 1e-6; _curv = _ptin*_hz;}
} 

//______________________________________________________________________________
void StvNodePars::get(THelixTrack *th) const
{
  double dir[3]={_cosCA,_sinCA,_tanl};
  th->Set(&_x,dir,_curv);
}
//______________________________________________________________________________
void StvNodePars::move(double dLxy)
{
  double dcCA,dsCA,dC,dS,dX,dY,dZ;
  double dPhi = _curv*dLxy;

  if (fabs(dPhi) < 0.2) {
    
    dC = -dPhi*dPhi/2; dS = dPhi*(1-dPhi*dPhi/6);
    dX = dLxy*(_cosCA*(1-dPhi*dPhi/6) - _sinCA*(dPhi/2));
    dY = dLxy*(_sinCA*(1-dPhi*dPhi/6) + _cosCA*(dPhi/2));
  } else {

    dC = cos(dPhi)-1; dS = sin(dPhi);
    dX = (_cosCA*dS + _sinCA*dC)/_curv;
    dY = (_sinCA*dS - _cosCA*dC)/_curv;
  }
  dZ = dLxy*_tanl;
  dcCA = _cosCA*dC - _sinCA*dS;
  dsCA = _sinCA*dC + _cosCA*dS;
  _cosCA+=dcCA; _sinCA+=dsCA; _x+=dX; _y+=dY; _z+=dZ; _psi+=dPhi;
}
//______________________________________________________________________________
void StvNodePars::moveToR(double R)
{
  double myR2  = _x*_x     + _y*_y;
  double myDot = _x*_cosCA + _y*_sinCA;
  double dR2 = R*R-myR2;
  double dis = myDot*myDot+dR2;
  if (dis<0) dis = 0;
  dis = sqrt(dis);
  double dL = (dR2)/(dis+fabs(myDot));
  if (myDot<0) dL = -dL;
  move(dL);
}
//------------------------------------------------------------------------------
void StvNodePars::reverse( Mtx55D_t &fitDerI, const Mtx55D_t &fitDer) const
{
static const int mius[]= {1,2,5,8,9,10,13,14,0};
  if (fitDerI != fitDer) memcpy(fitDerI[0],fitDer[0],sizeof(Mtx55D_t));
  for (int i=0;mius[i];i++) {fitDerI[0][mius[i]]=-fitDerI[0][mius[i]];}
}

//______________________________________________________________________________
const StvFitPars &StvNodePars::operator-(const StvNodePars& sub) const
{
static StvFitPars fp;
  double cos2L = 1./(1+sub._tanl*sub._tanl); 
  double cosL  = sqrt(cos2L);
  double sinL  = sub._tanl*cosL;
  double dx = _x-sub._x;
  double dy = _y-sub._y;
  double dz = _z-sub._z;
  
  fp.mH = dx*(     -sub._sinCA)+ dy*(      sub._cosCA);
  fp.mZ = dx*(-sinL*sub._cosCA)+ dy*(-sinL*sub._sinCA) +dz*cosL;
  fp.mA = (_psi -sub._psi );
  fp.mC = (_ptin-sub._ptin);
  double tL = (_tanl-sub._tanl)/(1+_tanl*sub._tanl);
  if (fabs(tL)<0.05) { fp.mL = tL*(1-tL*tL/3); }
  else               { fp.mL = atan(tL)      ; }

  if      (fp.mL<0     ) {fp.mL = -fp.mL	; fp.mA=-fp.mA;}
  else if (fp.mL>M_PI/2) {fp.mL = fp.mL-M_PI/2	; fp.mA=-fp.mA;}
  assert(!fp.Check());
  return fp;
}
//______________________________________________________________________________
void StvNodePars::operator+=(const StvFitPars &fp)
{
  double cos2L = 1./(1+_tanl*_tanl); 
  double cosL  = sqrt(cos2L);
  double sinL  = _tanl*cosL;
  _x += -_sinCA*fp.mH - sinL*_cosCA*fp.mZ;
  _y +=  _cosCA*fp.mH - sinL*_sinCA*fp.mZ;
  _z +=                 cosL       *fp.mZ;

  double a = fp.mA,cA,sA;
  if (fabs(a) < 0.1) { sA = a*(1-a*a/6); cA = 1-a*a/2;}
  else               { sA = sin(a)     ; cA = cos(a) ;}                
 _psi   += a;
  double cosCA = _cosCA;
  _cosCA = cosCA*cA-_sinCA*sA;
  _sinCA = cosCA*sA+_sinCA*cA;

  _ptin  += fp.mC;

  double l = fp.mL,tL;
  if (fabs(l) < 0.1) { tL = l*(1+l*l/3); }
  else               { tL = tan(l)      ;}                
  _tanl = (tL+_tanl)/(1.-_tanl*tL);
  if (_tanl < -MAXTAN) _tanl = -MAXTAN;
  if (_tanl >  MAXTAN) _tanl =  MAXTAN;
  _curv   = _hz *_ptin;
  if (fabs( _cosCA)>1 || fabs( _sinCA)>1) ready();
  assert(!check());
}
//______________________________________________________________________________
void StvNodePars::print() const
{
static const char* tit[]={"cosCA","sinCA","X","Y","Z","Eta","Ptin","TanL","Curv",0};
  for (int i=-2;i<kNPars+1;i++) {printf("%s = %g, ",tit[i+2],P[i]);}
  printf("\n");
}   
//______________________________________________________________________________
void StvNodePars::convert( Mtx55D_t &fitDer, const Mtx55D_t &hlxDer) const
{
  double cosL = 1/sqrt(1+_tanl*_tanl);
  double sinL = _tanl*cosL;
  
  fitDer[0][0] = hlxDer[0][0];
  fitDer[0][1] = hlxDer[0][3]*(1/cosL) + hlxDer[0][1]*(_tanl*_curv);
  fitDer[0][2] = hlxDer[0][1];
  fitDer[0][3] = hlxDer[0][4];
  fitDer[0][4] = hlxDer[0][2]*_hz;
  fitDer[1][0] = cosL*hlxDer[3][0];
  fitDer[1][1] = cosL*(hlxDer[3][3]*(1/cosL) + hlxDer[3][1]*(_tanl*_curv));
  fitDer[1][2] = cosL*hlxDer[3][1];
  fitDer[1][3] = cosL*hlxDer[3][4];
  fitDer[1][4] = cosL*hlxDer[3][2]*_hz;
  fitDer[2][0] = (-sinL*_curv)*hlxDer[3][0] + hlxDer[1][0];
  fitDer[2][1] = (-sinL*_curv)*(hlxDer[3][3]*(1/cosL) + hlxDer[3][1]*(_tanl*_curv)) + hlxDer[1][3]*(1/cosL) + hlxDer[1][1]*(_tanl*_curv);
  fitDer[2][2] = (-sinL*_curv)*hlxDer[3][1] + hlxDer[1][1];
  fitDer[2][3] = (-sinL*_curv)*hlxDer[3][4] + hlxDer[1][4];
  fitDer[2][4] = (-sinL*_curv)*hlxDer[3][2]*_hz + hlxDer[1][2]*_hz;
  fitDer[3][0] = hlxDer[4][0];
  fitDer[3][1] = hlxDer[4][3]*(1/cosL) + hlxDer[4][1]*(_tanl*_curv);
  fitDer[3][2] = hlxDer[4][1];
  fitDer[3][3] = hlxDer[4][4];
  fitDer[3][4] = hlxDer[4][2]*_hz;
  fitDer[4][0] = (1/_hz)*hlxDer[2][0];
  fitDer[4][1] = (1/_hz)*(hlxDer[2][3]*(1/cosL) + hlxDer[2][1]*(_tanl*_curv));
  fitDer[4][2] = (1/_hz)*hlxDer[2][1];
  fitDer[4][3] = (1/_hz)*hlxDer[2][4];
  fitDer[4][4] = hlxDer[2][2];
}
//______________________________________________________________________________
//______________________________________________________________________________
void StvHitErrs::rotate(double angle)
{
  double t[2][2];
  t[0][0] = cos(angle); t[0][1] = -sin(angle);
  t[1][0] = -t[0][1]  ; t[1][1] = t[0][0];
  double r[3];
  TCL::trasat(t[0],&hXX,r,2,2);
  TCL::ucopy(r,&hXX,3);
}


//______________________________________________________________________________
//______________________________________________________________________________
double StvFitErrs::Sign() const {return EmxSign(5,Arr());}
//______________________________________________________________________________
const StvFitErrs &StvFitErrs::operator*(const Mtx55D_t &how) const
{
static StvFitErrs myFitErrs;
  TCL::trasat(how[0],Arr(),myFitErrs.Arr(),5,5);
  assert(!myFitErrs.Check());
  return myFitErrs;
}  
//______________________________________________________________________________
void StvFitErrs::Reset()
{
  memset(this,0,sizeof(*this));
  mHH =  MAXFITERR[0]*MAXFITERR[0];
  mZZ =  MAXFITERR[1]*MAXFITERR[1];
  mAA =  MAXFITERR[2]*MAXFITERR[2];
  mLL =  MAXFITERR[3]*MAXFITERR[3];
  mCC =  MAXFITERR[4]*MAXFITERR[4];
}

//______________________________________________________________________________
void StvFitErrs::Set(const THelixTrack *he, double hz)
{
mHz = hz;
const THEmx_t *emx = he->Emx();
double  cosL = he->GetCos();
double  sinL = he->GetSin();
double  curv = he->GetRho();
double dSdZbyCur= -sinL*curv;
mHH = emx->mHH ;
mHZ = emx->mHZ*cosL ;
mZZ = emx->mZZ*cosL*cosL ;
mHA = emx->mHA + emx->mHZ*dSdZbyCur;
mZA = (emx->mAZ +emx->mZZ*dSdZbyCur)*cosL;
mAA = emx->mAA +2*emx->mAZ*dSdZbyCur + emx->mZZ*dSdZbyCur*dSdZbyCur;
mHL = emx->mHL ;
mZL = emx->mZL*cosL ;
mAL = emx->mAL + emx->mZL*dSdZbyCur;
mLL = emx->mLL ;
mHC = emx->mHC/mHz ;
mZC = emx->mCZ/mHz*cosL ;
mAC = (emx->mAC+emx->mCZ*dSdZbyCur)/mHz ;
mLC = emx->mCL/mHz ;
mCC = emx->mCC/mHz/mHz ;
}  
//______________________________________________________________________________
void StvFitErrs::Get(THelixTrack *he) const
{
  assert(mHz);
  he->SetEmx(0);
  THEmx_t *emx = he->Emx();
  double  cosL = he->GetCos();
  double  sinL = he->GetSin();
  double  curv = he->GetRho();
  double dSdZbyCur= sinL/cosL*curv;
  emx->mHH = mHH ;
  emx->mHZ = mHZ/cosL ;
  emx->mZZ = mZZ/cosL/cosL ;
  emx->mHA = (mHA +mHZ*dSdZbyCur);
  emx->mAZ = (mZA+mZZ*dSdZbyCur)/cosL ;
  emx->mAA = (mAA +2*mZA*dSdZbyCur+mZZ*dSdZbyCur*dSdZbyCur);
  emx->mHL = mHL ;
  emx->mZL = mZL/cosL ;
  emx->mAL = (mAL + mZL*dSdZbyCur);
  emx->mLL = mLL ;
  emx->mHC = mHC*mHz ;
  emx->mCZ = mZC*mHz/cosL ;
  emx->mAC = (mAC+mZC*dSdZbyCur)*mHz ;
  emx->mCL = mLC*mHz ;
  emx->mCC = mCC*mHz*mHz ;
}  
//_____________________________________________________________________________
void StvFitErrs::Set(const StvFitErrs &fr,double errFactor)
{
  Reset();
  mHz = fr.mHz;  
  double const *e =fr.Arr();
  double       *ee=   Arr();
  for (int i=0,li=0;i< 5;li+=++i) {ee[li+i] = e[li+i]*errFactor;}
}
//_____________________________________________________________________________
void StvFitErrs::Backward()
{
  mHA*=-1; mAC*=-1; mHZ*=-1; mZC*=-1; mAL*=-1; mZL*=-1;
}
//_____________________________________________________________________________
int StvFitErrs::Check(const char *tit) const
{

  int ierr=0;
  double dia[5];const double *e=&mHH;
  for (int i=0,li=0;i< 5;li+=++i) {
    dia[i]=e[li+i];
    if (dia[i]< 1e-8*MAXFITERR[i]*MAXFITERR[i]) {ierr = i+1; goto ERR;}
    if (dia[i]> 9999*MAXFITERR[i]*MAXFITERR[i]) {ierr = i+6; goto ERR;}
    for (int j=0;j<i;j++) {
       if (e[li+j]*e[li+j]>=dia[i]*dia[j]){ierr = 100+10*i+j;goto ERR;}
    } }
  return 0;
ERR: if (!tit) return ierr;
  printf("StvFitErrs::Check(%s)=%d\n",tit,ierr);
  Print();
  return ierr;
}     

//_____________________________________________________________________________
 void StvFitErrs::Print(const char *tit) const
{
static const char *N="HZALC";
  if (!tit) tit = "";
  printf("StvFitErrs::Print(%s) ==\n",tit);
  const double *e = &mHH;
  for (int i=0,li=0;i< 5;li+=++i) {
    printf("%c ",N[i]);
    for (int j=0;j<=i;j++) {
    printf("%g\t",e[li+j]);} 
    printf("\n");
  }
}
//_____________________________________________________________________________
int StvFitPars::Check() const
{
  for (int i=0;i<5;i++) { if (fabs(Arr()[i]) > MAXFITPARS[i]) return i+1; }
  return 0;
}
//_____________________________________________________________________________

//_____________________________________________________________________________
/**
   returns the node information
   double x[6],  : state, for a definition, in radial implementation
                   rad  - radius at start (cm). See also comments
                   phi  - azimuthal angle  (in rad)      
                   z    - z-coord. (cm)                 
                   tanl - tan(dip) =pz/pt               
                   psi  - azimuthal angle of pT vector (in rads)     
                   pti  - signed invert Pt.//  pti = curv/hz 
   double cc[15] : error matrix of the state "x" rad is fixed
                       code definition adopted here, where:
   PhiPhi;
   ZPhi     ,ZZ;                       
   TanlPhi  ,TanlZ ,TanlTanl,                 
   PhiPsi   ,ZPsi  ,TanlPsi , PsiPsi ,           
   PhiPti  ,ZPti ,TanlPti, PsiPti, PtiPti     

*/
//_____________________________________________________________________________
void StvNodePars::GetRadial(double radPar[6],double radErr[15],const StvFitErrs *fitErr) const
{
//Remind StvFitPars:
//double mH;	// direction perpendicular movement and Z
//double mZ;	// Pseudo Z, direction perpendicular movement & H
//double mA;	// Angle in XY. cos(A),sin(A),T moving direction
//double mL;	// Angle lambda in Rxy/Z
//double mC;	// Curvature

  enum {jRad=0,jPhi,jZ,jTan,jPsi,jPti};
  enum {jPhiPhi=0
       ,jPhiZ  ,jZZ
       ,jPhiTan,jZTan,jTanTan
       ,jPhiPsi,jZPsi,jTanPsi,jPsiPsi,
       ,jPhiPti,jZPti,jTanPti,jPsiPti,jPtiPti};
  double rxy = getRxy();
  radPar[jRad] = rxy;
  radPar[jPhi] = atan2(_y,_x);
  radPar[jZ  ] = _z;
  radPar[jTan] = _tanl;
  radPar[jPsi] = _psi;
  radPar[jPti] = _ptin;
  if (!radErr) return;

//double R2 = radPar[jRad]*radPar[jRad];
  double cos2L = 1./(1+_tanl*_tanl);
  double cosL = sqrt(cos2L);
  double sinL = _tanl*cosL;
			
  double dcaFrame[3][3]={{  cosL*_cosCA, cosL*_sinCA, sinL}	
                        ,{      -_sinCA,      _cosCA,    0}
                        ,{ -sinL*_cosCA,-sinL*_sinCA, cosL}};
			

  double newFrame[3][3]={{  _x/radPar[jRad],_y/radPar[jRad], 0}	
                        ,{ -_y/radPar[jRad],_x/radPar[jRad], 0}
                        ,{                0,              0, 1}};

  double qwe[3][3];
  TCL::mxmpy1(newFrame[0],dcaFrame[0],qwe[0],3,3,3);
  double pivo = qwe[0][0];
  assert (fabs(pivo)>1e-6);
  TCL::vlinco(qwe[1],1.,qwe[0],-qwe[1][0]/pivo,qwe[1],3);
  TCL::vlinco(qwe[2],1.,qwe[0],-qwe[2][0]/pivo,qwe[2],3);

  double T[5][5] = {{qwe[1][1]/rxy, qwe[1][2]/rxy, 0       ,0,0}
                   ,{qwe[2][1]    , qwe[2][2]    , 0       ,0,0}
                   ,{            0,             0, 1./cos2L,0,0}
                   ,{            0,             0, 0       ,1,0}
                   ,{            0,             0, 0       ,0,1}};
                   


  TCL::trasat(T[0],fitErr->Arr(),radErr,5,5); 


}
//_____________________________________________________________________________
void StvNodePars::GetImpact(StvImpact *imp,const StvFitErrs *fe)  const
{
    /// signed impact parameter; Signed in such a way that:
    ///     x =  -impact*sin(Psi)
    ///     y =   impact*cos(Psi)
  imp->mImp = _x*(-_sinCA) + _y*(_cosCA);
  imp->mZ   = _z;
  imp->mPsi = _psi;
  imp->mPti = _ptin;
  imp->mTan = _tanl;
  imp->mCurv= _curv;
  if (!fe) return;

  float cos2L = 1./(1+_tanl*_tanl);
  imp->mImpImp=fe->mHH;
  imp->mZImp  =fe->mHZ; imp->mZZ  =fe->mZZ;
  imp->mPsiImp=fe->mHA; imp->mPsiZ=fe->mHZ; imp->mPsiPsi=fe->mAA;
  imp->mPtiImp=fe->mHC; imp->mPtiZ=fe->mZC; imp->mPtiPsi=fe->mAC; imp->mPtiPti=fe->mCC;
  imp->mTanImp=fe->mHL/cos2L; 
  imp->mTanZ  =fe->mZL/cos2L; 
  imp->mTanPsi=fe->mAL/cos2L; 
  imp->mTanPti=fe->mLC/cos2L;
  imp->mTanTan=fe->mLL/cos2L/cos2L;

}
//______________________________________________________________________________
const StvFitPars &StvFitPars::operator*(Mtx55D_t &t) const  
{
static StvFitPars myPars;
//??  Multiply(myPars.Arr(),t,Arr());
  TCL::vmatl(t[0],Arr(),myPars.Arr(),5,5);
  assert(!myPars.Check());
  return myPars;
}
//_____________________________________________________________________________
void StvFitErrs::Get(const StvNodePars *np,  StvNodeErrs *ne) const
{
  double cos2L = 1./(np->_tanl*np->_tanl+1);
  double dTdL = 1./cos2L;
  double cosL = sqrt(cos2L);

//                       d/dH     d/dZ       d/dA     d/dLam d/dPti
  double T[6][5] = {{ np->_cosCA,   0,         0,         0,  0}   //dX
  		   ,{ np->_sinCA,   0,         0,         0,  0}   //dY
  		   ,{         0,    1/cosL,    0,         0,  0}   //dZ
  		   ,{         0,    0,         1,         0,  0}   //dEta
  		   ,{         0,    0,         0,         0,  1}   //dPti
  		   ,{         0,    0,         0,      dTdL,  0}}; //dTan

  TCL::trasat(T[0],this->Arr(),ne->A,5,6); 

}
#if 0 //
//____________________________________________________________
double EmxSign(int n,const double *a) 
{
   double ans=3e33;
   double buf[55];
   double *B = (n<=10) ? buf : new double[n];
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
            if (sum<0.) goto RETN;
            dc = sqrt(sum);
            b[kpiv] = dc;
            if (r__ > 0.)  r__ = (double)1. / dc;
         }
         kpiv += j;
      }

   } while  (i__ < n);

RETN: if (B!=buf) delete B; 
   return ans;
} /* trchlu_ */
#endif //0

//_____________________________________________________________________________
//_____________________________________________________________________________
#include "TRandom.h"
#include "TMatrixTSym.h"
#include "TMatrixT.h"
#include "TVectorT.h"
#include "StarRoot/TRandomVector.h"
//_____________________________________________________________________________
ClassImp(StvNodeParsTest);
void StvNodeParsTest::Test()
{
  double thPars[7+15],Hz=0.000299792458 * 4.98478;
  for (int i=0;i<7+15;i++) {thPars[i]=gRandom->Rndm();}
  THelixTrack th(thPars,thPars+3,thPars[6]),thh;
  memcpy(thPars+3,th.Dir(),3*sizeof(double));
  th.SetEmx(thPars+7);
  
  StvNodePars pars;
  StvFitErrs  errs;
  pars.set(&th,Hz);
  errs.Set(&th,Hz);
  pars.reverse();
  errs.Backward();
  pars.get(&thh);
  errs.Get(&thh);
  thh.Backward();

  double thhPars[7+15];
  memcpy(thhPars,thh.Pos(),7*sizeof(double));
  memcpy(thhPars+7,thh.Emx()->Arr(),15*sizeof(double));
  int nerr=0;
  for (int i=0;i<7+15;i++) {
    if (fabs(thhPars[i]-thPars[i]) <1e-6) continue;
    nerr++;printf("%d = %g %g \n",i,thPars[i],thhPars[i]);}
  printf("nmErrs = %d\n",nerr);
  
}
//_____________________________________________________________________________
void StvNodeParsTest::TestGetRadial(int nEv)
{
StvFitErrs fE;
  fE.mHH = 1.0*1.0;
  fE.mZZ = 2.0*2.0;
  fE.mAA = 0.01*0.01;
  fE.mLL = 0.01*0.01;
  fE.mCC = 0.1*0.1;
  double dia[5];
  double *e = &fE.mHH;
  for (int i=0,li=0;i< 5;li+=++i) {
    dia[i]=e[li+i];
    for (int j=0;j<i;j++) {
       e[li+j] = sqrt(dia[i]*dia[j])*(0.5-gRandom->Rndm())*0.2;
//       printf("[%d][%d]=%d ",i,j,e[li+j]);
    } 
  }
  TMatrixTSym<double> S(5);
  for (int i=0,li=0;i< 5;li+=++i) {
    for (int j=0;j<=i;j++    ) {
       S[i][j]=e[li+j]; S[j][i]=e[li+j];
    } }
  S.Print();
  StvNodePars node;
  node.reset();
  double myRad = 100;
  double phi = gRandom->Rndm()*2*M_PI;
  node._x    = myRad*cos(phi);
  node._y    = myRad*sin(phi);
  node._z    = (gRandom->Rndm()-0.5)*200;
  node._hz   = 0.000299792458 * 4.98478;
  node._psi  = phi + (gRandom->Rndm()-0.5);
  node._tanl = gRandom->Rndm();
  node._ptin = (gRandom->Rndm()-0.5);
  node.ready();

  TRandomVector RV(S);
  double radPar[6],radErr[15];
  node.GetRadial(radPar,radErr,&fE);
  TMatrixTSym<double> SS(5);
  for (int i=0,li=0;i< 5;li+=++i) {
    for (int j=0;j<=i;j++    ) {
       SS[i][j]=radErr[li+j]; SS[j][i]=radErr[li+j];
  } }
  SS.Print();

  double rE[15]={0};
  for (int ev=0;ev<nEv;ev++) 
  {
    const TVectorT<double> &res = RV.Gaus();
    StvNodePars myNode(node);
    StvFitPars  fitPars(res.GetMatrixArray());
    myNode +=fitPars;
//		Project it to x*x+y*y=r*r
    myNode.moveToR(myRad);
    double rP[6];
    myNode.GetRadial(rP);
    
    for (int i=0,li=0;i< 5;li+=++i) {
      for (int j=0;j<=i;j++    ) {
      rE[li+j]+= (rP[i+1]-radPar[i+1])*(rP[j+1]-radPar[j+1]);
    } }
  }  //End events
  for (int j=0;j<15;j++) {rE[j]/=(nEv);}
   
  double qA=0,qAmax=0;
  for (int i=0,li=0;i< 5;li+=++i) {
    dia[i]=radErr[li+i];
    for (int j=0;j<=i;j++) {
    double dif = (rE[li+j]-radErr[li+j])/sqrt(dia[i]*dia[j]);
    printf("(%d %d) \t%g = \t%g \t%g\n",i,j,radErr[li+j],rE[li+j],dif);
    dif = fabs(dif);
    qA+= (dif); if (dif>qAmax) qAmax=dif;
  } }
  qA/=15;
  printf("Quality %g < %g < 1\n",qA,qAmax);

}
//_____________________________________________________________________________
void StvNodeParsTest::TestErrProp(int nEv)
{

StvNodePars iP,iPR,oP,oPR;
double dia[5],*e,*er,vtx[3];

  iP._cosCA = 0.051522195951218416; iP._sinCA = -0.99867184876021664;  iP._x   = 56.80456301948584; 
  iP._y     = -179.95090442478528;  iP._z     = 16.833129146428401;    iP._psi = -1.5192513089402997; iP._ptin = -4.286089548109465; 
  iP._tanl  = -0.71077992742240803; iP._curv  = -0.0063779138641975935;iP._hz=(0.0014880496061989194);
  
StvFitErrs iE,oE,oER;
  iE.mHH = 0.0025928369042255385; iE.mHZ = -4.9934860023454386e-11; iE.mZZ = 0.014598355970801268; iE.mHA = -0.00059887440419442305; 
  iE.mZA = 1.0958739205478152e-11; iE.mAA = 0.00026524379894739812; iE.mHL = 3.463001237863329e-12; iE.mZL = -0.0016525557966380938; 
  iE.mAL = 8.3669926017237923e-13; iE.mLL = 0.00041855110437868546; iE.mHC = 0.0043962440767417576; iE.mZC = -2.904206508909407e-11; 
  iE.mAC = -0.0041320793241820105; iE.mLC = -2.5031139398137018e-12; iE.mCC = 0.78568815092933286; iE.SetHz(0.0014880496061989194);

  e = iE.Arr();
  for (int i=0,li=0;i< 5;li+=++i) {
    dia[i]=e[li+i];
    for (int j=0;j<i;j++) {
      e[li+j] = sqrt(dia[i]*dia[j])*0.001*((j&2)-1);
  } }


  TMatrixTSym<double> S(5);
  e = &iE.mHH;
  for (int i=0,li=0;i< 5;li+=++i) {
    for (int j=0;j<=i;j++    ) {
       S[i][j]=e[li+j]; S[j][i]=e[li+j];
    } }
  TRandomVector RV(S);
  memset(&oER,0,sizeof(oER));
  THelixTrack ht;
  for (int ev=0;ev <= nEv;ev++) {
    iPR = iP;
    if (ev) {
      const TVectorT<double> &res = RV.Gaus();
      StvFitPars fp(res.GetMatrixArray());
      iPR+=fp;
    }
    iPR.get(&ht);
    ht.SetEmx(0);
    if (!ev) {iE.Get(&ht);}
    ht.Backward();
    if (!ev) {
      ht.Move(100.);
      memcpy(vtx,ht.Pos(),sizeof(vtx));
    } else {
      double my100 = ht.Path(vtx);
      ht.Move(my100);
    }

    oPR.set(&ht,iP._hz);

    if (!ev) {
      oP=oPR;
      oE.Set(&ht,iP._hz);
      
    } else {
      StvFitPars fp = (oPR-oP);
      double *e = oER.Arr();
      for (int i=0,li=0;i< 5;li+=++i) {
         for (int j=0;j<=i;j++    ) {
         e[li+j]+=fp.Arr()[i]*fp.Arr()[j];
    } } }
  }//EndEvts
  for (int j=0;j<15;j++) { oER.Arr()[j] /=nEv;}


  e = oE.Arr();
  er = oER.Arr();
  double qA=0,qAmax=0;
  for (int i=0,li=0;i< 5;li+=++i) {
    dia[i]=e[li+i];
    for (int j=0;j<=i;j++) {
    double dif = (er[li+j]-e[li+j])/sqrt(dia[i]*dia[j]);
    printf("(%d %d) \t%g = \t%g \t%g\n",i,j,er[li+j],e[li+j],dif);
    dif = fabs(dif);
    qA+= (dif); if (dif>qAmax) qAmax=dif;
  } }
  qA/=15;
  printf("Quality %g < %g < 1\n",qA,qAmax);

}
//_____________________________________________________________________________
#include "TMatrixT.h"
#include "TMatrixTSym.h"
#include "TVectorT.h"
//_____________________________________________________________________________
double EmxSign(int n,const double *e)
{
  TMatrixTSym<double> S(n);  

  for (int i=0,li=0;i< n;li+=++i) {
    for (int j=0;j<=i;j++    ) {
       S[i][j]=e[li+j]; S[j][i]=e[li+j];
  } }
  TMatrixT<double> EigMtx(n,n);
  TVectorT<double> EigVal(n);  
  EigMtx = S.EigenVectors(EigVal);

  double ans = 3e33;
  for (int i=0;i<n;i++) {if (EigVal[i]<ans) ans = EigVal[i];}
  return ans;
} 
