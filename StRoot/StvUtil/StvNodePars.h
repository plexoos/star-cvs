#ifndef __StvNodePars_h_
#define __StvNodePars_h_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "THelixTrack.h"

class StvFitPars;
class StvFitErrs;
class StvImpact;
//------------------------------------------------------------------------------
typedef double Mtx55D_t[5][5];
void Multiply(Mtx55D_t &res, const Mtx55D_t &A,const Mtx55D_t &B);
void Multiply(double res[5], const Mtx55D_t &A,const double B[5]);
inline void Copy(Mtx55D_t &to,const Mtx55D_t &fr){memcpy(to[0],fr[0],5*5*sizeof(to[0][0]));}
void Invert(Mtx55D_t &to,const Mtx55D_t &fr);
void Testik(const Mtx55D_t &tt);
//------------------------------------------------------------------------------
class StvFitPars
{
public:	
  StvFitPars():mH(0),mZ(0),mA(0),mL(0),mC(0){}
  StvFitPars(double h,double z):mH(h),mZ(z),mA(0),mL(0),mC(0){}
  StvFitPars(const double *arr){memcpy(&mH,arr,5*sizeof(mH));}
const StvFitPars &operator*(Mtx55D_t &t) const;    
         int Check() const;
      double *Arr() 		{return &mH;}
const double *Arr()  const 	{return &mH;}
public:	
double mH;	// direction perpendicular movement and Z
double mZ;	// Pseudo Z, direction perpendicular movement & H
double mA;	// Angle in XY. cos(A),sin(A),T moving direction
double mL;	// Angle lambda in Rxy/Z
double mC;	// Curvature
};


//------------------------------------------------------------------------------
class StvNodePars {
public:	
  enum eNodePars {kNPars=5};
  void reset();
  void ready();
  void set(const THelixTrack *ht, double Hz);
  void get(      THelixTrack *ht) const;
double getPt() const			{ return 1./fabs(_ptin); }
  void getMom(double p[3]) const; 
  void getDir(double d[3]) const; 
double getP2() const;
double getRxy() const;
  void reverse(); 

///		convert THelixTrack derivativ matrix into StvFitPar one
  void convert( Mtx55D_t &fitDer , const Mtx55D_t &hlxDer) const;
///		invert or reverse StvFitPar derivativ matrix
  void reverse( Mtx55D_t &fitDerI, const Mtx55D_t &fitDer) const;
//		move point along helix  
  void move(double dLxy); 
//		move point along helix  up to given radius in xy
  void moveToR(double Rxy); 
StvNodePars &merge(double wt,StvNodePars &other);


double  operator[](int idx) const {return P[idx];}
double &operator[](int idx)       {return P[idx];}
    int getCharge() const {return (_ptin > 0) ? -1 : 1;}
    int     check(const char *pri=0) const;
void  operator+=(const StvFitPars &add);
const StvFitPars &operator-(const StvNodePars& sub) const;
void    print() const;
  void GetRadial(double radPar[6],double *radErr=0,const StvFitErrs *fE=0)  const;
  void GetImpact(StvImpact *imp,const StvFitErrs *fE=0)  const;

  enum {kX=0,kY,kZ,kPhi,kCurv,kTanL};
public:	
  /// sine and cosine of cross angle
  double _cosCA;
  double _sinCA;
  union{double P[1];double _x;};
  double _y; 
  double _z;
  double _psi;
  /// signed invert pt [sign = sign(-qB)]
  double _ptin;  
  /// tangent of the track momentum dip angle
  double _tanl;
  /// signed curvature [sign = sign(-qB)]
  double _curv;  
  /// Z component magnetic field in units Pt(Gev) = Hz * RCurv(cm)
  double _hz;  
};
class StvNodeErrs {
public:	
void reset()				{memset(this,0,sizeof(StvNodeErrs));}
public:	
union{double A[1];double _cXX;};
  double _cYX,_cYY;                       
  double _cZX,_cZY, _cZZ;                 
  double _cEX,_cEY, _cEZ, _cEE;           
  double _cPX,_cPY, _cPZ, _cPE, _cPP;     
  double _cTX,_cTY, _cTZ, _cTE, _cTP, _cTT;
};  

//------------------------------------------------------------------------------
class StvFitErrs
{
public:	
  enum eFitErrs {kNErrs=15};
  StvFitErrs(double hh=0,double hz=0,double zz=0):mHH(hh),mHZ(hz),mZZ(zz){}
  void Reset();
  void Set(const StvFitErrs &fr,double errFactor);
  void Set(const THelixTrack *he,double hz);
  void Get(      THelixTrack *he)     const;
  void Get(const StvNodePars *np,  StvNodeErrs *ne)     const;
double GetHz() const 		{ return mHz ;}
  void SetHz(double hz)  	{ mHz=hz     ;}
  const double *Arr() const 	{ return &mHH;}
        double *Arr()       	{ return &mHH;}
  StvFitErrs &operator*=(double f) {for (int i=0;i<kNErrs;i++){Arr()[i]*=f;}; return *this;}
  void Backward();
const StvFitErrs &operator*(const Mtx55D_t &mtx) const; 
double Sign() const;
   int Check(const char *tit=0) const;
  void Print(const char *tit=0) const;
public:	
double
mHH,
mHZ, mZZ,
mHA, mZA, mAA,
mHL, mZL, mAL, mLL,
mHC, mZC, mAC, mLC, mCC;
//protected:
double mHz;
};  

//------------------------------------------------------------------------------
class StvHitErrs{
public:
StvHitErrs()			 {reset();}
void reset()			 {memset(this,0,sizeof(*this));}
StvHitErrs &operator*=(double f) {for (int i=0;i<6;i++){A[i]*=f;};return *this;}
void rotate(double angle);
union{
  double hXX;		double A[1];};
  double hXY,hYY;                       
  double hXZ,hYZ,hZZ;                 
};
//------------------------------------------------------------------------------
class StvImpact {
public:    

public:    
    /// signed impact parameter; Signed in such a way that:
    ///     x =  -impact*sin(Psi)
    ///     y =   impact*cos(Psi)
    float  mImp;
    ///  Z-coordinate of this track (reference plane)
    float  mZ;
    ///  Psi angle of the track
    float  mPsi;
    /// signed invert pt [sign = sign(-qB)]
    float  mPti;
    /// tangent of the track momentum dip angle
    float  mTan;
    /// signed curvature
    float  mCurv;
    
    /// pars errors
    float  mImpImp;
    float  mZImp, mZZ;
    float  mPsiImp, mPsiZ, mPsiPsi;
    float  mPtiImp, mPtiZ, mPtiPsi, mPtiPti;
    float  mTanImp, mTanZ, mTanPsi, mTanPti, mTanTan;
};
//------------------------------------------------------------------------------
class StvNodeParsTest
{
public:
static void Test();
static void TestGetRadial(int nEv=10000);
static void TestErrProp  (int nEv=10000);
ClassDef(StvNodeParsTest,0)
};





//------------------------------------------------------------------------------
// 		StvNodePars::inlines
inline void StvNodePars::reset(){memset(this,0,sizeof(StvNodePars));_cosCA=1;}
//------------------------------------------------------------------------------
inline void StvNodePars::ready(){_cosCA=cos(_psi);_sinCA=sin(_psi);_curv = _hz*_ptin;}
//------------------------------------------------------------------------------
inline void StvNodePars::getMom(double p[3]) const 
{ 
  double pt = 1./fabs(_ptin); 
  p[0]=pt*_cosCA;p[1]=pt*_sinCA;p[2]=pt*_tanl;
}
//------------------------------------------------------------------------------
inline double StvNodePars::getRxy() const 
{ 
   return sqrt(_x*_x+_y*_y);
}
//------------------------------------------------------------------------------
inline void StvNodePars::getDir(double d[3]) const 
{ 
  double nor = sqrt(1.+_tanl*_tanl);
  d[0]=_cosCA/nor;d[1]=_sinCA/nor;d[2]=_tanl/nor;
}
//------------------------------------------------------------------------------
inline double StvNodePars::getP2() const 
{ return 1./(_ptin*_ptin)/(1.+_tanl*_tanl);}

//------------------------------------------------------------------------------
inline void StvNodePars::reverse() 
{
 _cosCA = -_cosCA; _sinCA=-_sinCA; _psi+= M_PI; 
 while (_psi> M_PI) {_psi-=2*M_PI;}
 while (_psi<-M_PI) {_psi+=2*M_PI;}
 _tanl  = -_tanl ; _curv = -_curv ; _ptin = -_ptin;
}

#endif
