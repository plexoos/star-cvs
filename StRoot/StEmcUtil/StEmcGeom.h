/***************************************************************************
 *
 * $Id: StEmcGeom.h,v 1.5 2001/03/22 21:50:35 pavlinov Exp $
 *
 * Author:  Aleksei Pavlinov
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEmcGeom.h,v $
 * Revision 1.5  2001/03/22 21:50:35  pavlinov
 * Clean up for mdc4
 *
 * Revision 1.4  2001/03/15 23:47:20  pavlinov
 * Fixed error on SUN compiler
 *
 * Revision 1.3  2001/03/15 20:56:11  pavlinov
 * Jose's scheme is default
 *
 * Revision 1.2  2001/03/15 00:57:28  pavlinov
 * Created new methods getBinEnv and getIdEnv
 *
 * Revision 1.1  2000/06/20 17:11:25  pavlinov
 * Move StEmcGeom.h from St_emc_Maker to StEmcUtil
 *
 * Revision 1.11  2000/05/23 14:35:01  pavlinov
 * Clean up for SUN
 *
 * Revision 1.10  2000/05/18 17:07:30  pavlinov
 * Fixed error for methods getXYZ(...)
 *
 * Revision 1.9  2000/04/27 01:39:15  pavlinov
 * Cleanup for SUN
 *
 * Revision 1.8  2000/04/25 17:02:06  pavlinov
 * Added methods for gettinng x,y,z from volume ID
 *
 * Revision 1.7  2000/04/21 17:43:01  pavlinov
 * Added methods for for decoding Geant volume Id
 *
 * Revision 1.6  2000/04/18 20:38:09  pavlinov
 * Added ctor from Geant geometry
 *
 * Revision 1.5  2000/04/11 19:48:40  pavlinov
 * Merge versions of Pavlinov and Ogawa
 *
 * Revision 1.4  2000/01/29 00:04:57  akio
 * temprary fix for endcap. need more work, but no more junk messages and crash
 *
 * Revision 1.3  1999/07/16 18:03:45  pavlinov
 * Little correction for StEclMake
 *
 * Revision 1.2  1999/07/02 03:01:56  pavlinov
 * Little corrections for Linux
 *
 * Revision 1.1  1999/07/01 16:17:57  pavlinov
 * class StEmcGeom was created and maker was remade for new maker scheme
 *
 **************************************************************************/
#ifndef STAR_StEmcGeom
#define STAR_StEmcGeom
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StEmcGeom main class for <FONT COLOR="RED"> Geometry of BEMC for OFFline </FONT>      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include "math_constants.h"
#include <math.h>
#include <TArrayF.h>
#include <TString.h>
#include "tables/St_calb_calg_Table.h"
#include "tables/St_calb_calr_Table.h"

class StBFChain;
class TDataSet;

class StEmcGeom {
private:
  StBFChain*    mChain;    //!
  TDataSet*     mGeantGeom;//!
  St_calb_calg* mCalg;     //!
  calb_calg_st* mCalg_st;  //!
  St_calb_calr* mCalr;     //!  
  calb_calr_st* mCalr_st;  //!

  void    defineDefaultCommonConstants();
  void    defineCommonConstants();
  void    defineModuleGridOnPhi();
  Float_t relativeError(Float_t, Float_t);

protected:
  TString mMode;     // Empty, "geant" or "db"
  Int_t   mDetector; // Detectors number => see emc/inc/emc_def.h
  Int_t   mNModule;  // Number of modeles (120 is default)
  Int_t   mNEta;     // Number of eta bins
  Int_t   mNSub;     // Subdivision in eta bin
  Int_t   mNes;      // mNes  = mNEta * mNSubStEmcGeom.d
  Int_t   mNRaw;     // mNraw = mNes * mNModule
  Float_t mRadius;   // Distance to beam axis
  Float_t mYWidth;   // Half Width of module for mRadius
  Float_t mEtaMax;
  TArrayF mPhiOffset;  // Phi offset from numbering scheme for center of tower
  TArrayF mPhiStep;    // Step for transition to Star Cordinate System(-/+2pi/60)
  TArrayF mPhiBound;   // Phi offset from numbering scheme for edge of tower
  Float_t mPhiStepHalf; // 2pi/(60*2)
  
  TArrayF mZlocal;    // Array of z   coordinates (system of single module) 
  TArrayF mYlocal;    // Array of y   coordinates (system of single module) 
  TArrayF mEta;       // Array of eta coordinates (system of single module)
  TArrayF mPhi;       // Array of phi coordinates (system of single module)
  TArrayF mPhiModule; // Angle of center of module in STAR system

  Int_t   mMaxAdc;    // Range of ADC for each detector

public: 
  StEmcGeom(const Int_t );
  StEmcGeom(const Char_t*);
  StEmcGeom(const Int_t ,const Char_t*);
  virtual ~StEmcGeom();
  
  TString* Mode();
  Int_t    Detector() const;
  Int_t    NModule()  const;
  Int_t    NEta()  const;
  Int_t    NSub()  const;
  Int_t    Nes()  const;
  Int_t    NRaw()  const;
  Float_t  Radius()  const;
  Float_t  YWidth() const;
  Float_t  EtaMax() const;
  Float_t* PhiModule();
  Float_t* PhiOffset();
  Float_t* PhiStep();
  Float_t* PhiBound();
  Float_t* Zlocal(); 
  Float_t* Eta();
  Float_t* Ylocal();  
  Float_t* Phi();


  void     setDetector(const Int_t);
  void     setRadius(const Float_t);
  void     setYWidth(const Float_t);

  Int_t    checkModule(const Int_t );
  Int_t    checkEta(const Int_t );
  Int_t    checkSub(const Int_t );
  Int_t    checkId(const Int_t );

  Int_t    getBin(const Float_t phi, const Float_t eta, Int_t &m,Int_t &e,Int_t &s);
  Int_t    getBin(const Int_t rid, Int_t &m, Int_t &e, Int_t &s);
  Int_t    getId(const Int_t m,const Int_t e,const Int_t s,Int_t &rid);

  Int_t    getVolIdBemc(const Int_t,Int_t&, Int_t &,Int_t &,Int_t &);
  Int_t    getVolIdBsmd(const Int_t,Int_t&, Int_t &,Int_t &,Int_t &);
  Int_t    getVolId(const Int_t,Int_t&, Int_t &,Int_t &,Int_t &);
  
  Int_t    getZlYl(const Int_t,Float_t &,Float_t &);
  void     getXYZ(const Int_t,const Int_t,const Int_t,
                  Float_t &,Float_t &,Float_t &);
  Int_t    getXYZ(const Int_t, Float_t &,Float_t &,Float_t &);
  Int_t    getXYZfromGeant(const Int_t, Float_t &,Float_t &,Float_t &);

  Int_t    getEta(const Int_t, const Int_t, Float_t &);
  Int_t    getTheta(const Int_t, const Int_t, Float_t &);
  Int_t    getPhi(const Int_t, const Int_t, Float_t &);
  Int_t    getEtaPhi(const Int_t, Float_t &,  Float_t &);
  Int_t    getPhiModule(const Int_t, Float_t &);

  Int_t    getMaxAdc() {return mMaxAdc;}

  void     initGeom(const Int_t);
  void     initBEMCorBPRS();
  void     initBSMDE();
  void     initBSMDP();
  void     initEEMCorEPRS();
  void     initESMDE();
  void     initESMDP();
  void     printGeom();
  void     print() {printGeom();}
  void     compare(StEmcGeom &, Bool_t);
  void     compare(StEmcGeom *g, Bool_t key) {compare(*g,key);};
  void     printError(Float_t);

  Float_t  toDeg(const Float_t angR) {return C_DEG_PER_RAD*angR;} // Service
  Float_t  toRad(const Float_t angD) {return angD/C_DEG_PER_RAD;} // functions

  ClassDef(StEmcGeom,1)                      // Standard Root macro;
};

inline TString* StEmcGeom::Mode() {return &mMode;}
inline Int_t   StEmcGeom::Detector() const {return mDetector;}
inline Int_t   StEmcGeom::NModule()  const {return mNModule;}
inline Int_t   StEmcGeom::NEta()     const {return mNEta;}
inline Int_t   StEmcGeom::NSub()     const {return mNSub;}
inline Int_t   StEmcGeom::Nes()      const {return mNes;}
inline Int_t   StEmcGeom::NRaw()     const {return mNRaw;}
inline Float_t StEmcGeom::Radius()   const {return mRadius;}
inline Float_t StEmcGeom::YWidth()   const {return mYWidth;}
inline Float_t StEmcGeom::EtaMax()   const {return mEtaMax;}
inline Float_t* StEmcGeom::PhiModule() {return mPhiModule.GetArray();} 
inline Float_t* StEmcGeom::PhiOffset() {return mPhiOffset.GetArray();}
inline Float_t* StEmcGeom::PhiStep()   {return mPhiStep.GetArray();}
inline Float_t* StEmcGeom::PhiBound()  {return mPhiBound.GetArray();}
inline Float_t* StEmcGeom::Zlocal() {return mZlocal.GetArray();} 
inline Float_t* StEmcGeom::Eta() {return mEta.GetArray();} 
inline Float_t* StEmcGeom::Ylocal()  {return mYlocal.GetArray();}
inline Float_t* StEmcGeom::Phi() {return mPhi.GetArray();} 

inline void    StEmcGeom::setDetector(const Int_t val) { mDetector = val;} 
inline void    StEmcGeom::setRadius(const Float_t val) { mRadius = val;}
inline void    StEmcGeom::setYWidth(const Float_t val) { mYWidth = val;}

// _____________________________________________________________________
inline Int_t   StEmcGeom::checkModule(const Int_t m)
{
  if(m>=1 && m<=mNModule) return 0;
  else cout<<" Bad module# "<<m<<"/"<<mNModule<<" in Detector "<<mDetector<<endl; return 1;
}
// _____________________________________________________________________
inline Int_t   StEmcGeom::checkEta(const Int_t e)
{
  if(e>=1 && e<=mNEta) return 0;
  else cout<<" Bad eta# "<<e<<endl; return 1;
}
// _____________________________________________________________________
inline Int_t   StEmcGeom::checkSub(const Int_t s)
{
  if(s>=1 && s<=mNSub) return 0;
  else cout<<" Bad sub# "<<s<<endl; return 1;
}
// _____________________________________________________________________
inline Int_t   StEmcGeom::checkId(const Int_t rid)
{
  if(rid>=1 && rid<=mNRaw) return 0; // 15-mar-2001 => use Jose scheme
  else cout<<" Bad raw# "<<rid<<endl; return 1;
}

// _____________________________________________________________________
inline Int_t StEmcGeom::getBin(const Float_t phi, const Float_t eta, Int_t &m, Int_t &e, Int_t &s)
{
//
/// Transition from phi and eta to bin # => 3-Aug-1999 for StTbmMaker
//  (only for Barrel now !!!! )
//

  Float_t phiw, sw;
  if(0.0<=eta && eta<=mEtaMax) {      // First Barrel
    e    = int(mNEta*eta) + 1;
    phiw = mPhiBound[0] - phi;
    if(phiw<0.0) phiw = phiw + C_2PI; // 0<phiw<=2.*pi =>must be
    if(phiw<0.0 || phiw>C_2PI) printf(" phi %f eta %f \n",phi,eta); // For testing 
    m  = int(-phiw/mPhiStep[0]) + 1;
    sw = fmod(phiw, -mPhiStep[0]);
    if(sw<mPhiStepHalf) s=1; else s = 2;
    return 0;
  }
  else if(-mEtaMax<=eta && eta<0.0) { // Second Barrel
    e = int(-mNEta*eta) + 1;
    phiw = mPhiBound[1] - phi;
    if(phiw<0.0) phiw = phiw + C_2PI; // 0<phiw<=2.pi =>must be
    if(phiw<0.0 || phiw>C_2PI) printf(" phi %f eta %f \n",phi,eta); // For testing 
    m  = 120 - int(phiw/mPhiStep[1]);
    sw = fmod(phiw, mPhiStep[1]);
    if(sw<mPhiStepHalf) s = 2; else s = 1;
    return 0;
  }
  else return 1;                      // Out of Bemc

}

// _____________________________________________________________________
inline Int_t StEmcGeom::getZlYl(const Int_t rid, Float_t &zl, Float_t &yl)
{
  Int_t m, e, s;

  if(!getBin(rid,  m,  e, s)){
    zl = mZlocal[e-1]; 
    yl = mYlocal[s-1];
    return 0;
  }
  else return 1;
} 
// _____________________________________________________________________
inline void StEmcGeom::getXYZ(const Int_t m, const Int_t e, const Int_t s,
                               Float_t &x,Float_t &y,Float_t &z)
{
  Float_t phi;
  if(m<=60) z = mZlocal[e-1];
  else      z =-mZlocal[e-1];
  getPhi(m,s,phi);
  x = mRadius*cos(phi); 
  y = mRadius*sin(phi);
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getXYZ(const Int_t rid, Float_t &x,Float_t &y,Float_t &z)
{
  Int_t m, e, s;
  if(!getBin(rid,  m,  e, s)){
    getXYZ(m,e,s, x,y,z);
    return 0;
  }
  else return 1;
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getXYZfromGeant(const Int_t ivid,Float_t &x,Float_t &y,Float_t &z)
{
  Int_t m, e, s, det;
  if(getVolId(ivid, m,e,s,det) == 0){
    getXYZ(m,e,s, x,y,z);
    return 0;
  }
  else return 1;
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getEta(const Int_t m, const Int_t  e, Float_t &eta)
{
  if(!checkModule(m) && !checkEta(e)){
    if(m <= mNModule/2) eta =  mEta[e-1];  // West part of EMC
    else                eta = -mEta[e-1];  // East part of EMC
    return 0;
  }
  else return 1;
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getTheta(const Int_t m, const Int_t  e, Float_t &theta)
{
  Float_t etaw;
  if(!getEta(m,e, etaw)) { theta = 2.*atan(exp(-etaw)); return 0;}  
  else return 1; 
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getPhi(const Int_t m, const Int_t  s, Float_t &phi)
{
  //
  // -pi <= phi < pi
  //
  Int_t iphi, im;
  if(!checkModule(m) && !checkSub(s)){
    Double_t phiW;       // phi in system of module

    if(m <= mNModule/2) {phiW = mPhi[s-1]; im = 0; iphi=m-1;}  // West part of EMC
    else {phiW = -mPhi[s-1]; im = 1; iphi=m-mNModule/2-1;}     // East part of EMC    
    
    phiW += mPhiOffset[im] + mPhiStep[im]*iphi;

    while(phiW >= C_PI) phiW -= C_2PI;
    while(phiW < -C_PI) phiW += C_2PI;
    if(phiW > (C_PI-0.0001)) phiW = -C_PI; // -pi<=phi<phi

    phi = phiW;
    return 0;
  }
  else return 1;
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getPhiModule(const Int_t m, Float_t &phi)
{
  if(!checkModule(m)) { phi=mPhiModule[m-1]; return 0;}
  else return 1;
}
// _____________________________________________________________________
inline Int_t StEmcGeom::getEtaPhi(const Int_t rid, Float_t &eta, Float_t &phi)
{
  Int_t m=0, e=0, s=0;
  if(!getBin(rid, m, e, s)) {
    getEta(m, e, eta);
    getPhi(m, s, phi);
    return 0;
  }
  else return 1;
}

inline Int_t 
StEmcGeom::getBin(const Int_t rid,Int_t &m,Int_t &e,Int_t &s)
{
  //
  // 15-mar-2001 for transition from environment (Jose) numeration to 
  // usual EMC'c numbering. This is standard now !!!
  // It is work only for BEMC and BPRS (15-mar-2001)
  //
  if(mDetector<1 || mDetector>3) {cout<<" Wrong number of detector "
                                      <<mDetector<<endl; return 1;}
  if(checkId(rid) == 1) return 1;

  Int_t idw;
  m   = (rid - 1) / 40 + 1; // Module number 
  idw = (rid - 1) % 40;     // change from 0 to 39
  s   = idw/20 + 1;
  e   = 20 - idw%20;
  return 0;                   // zero is good
}

inline Int_t 
StEmcGeom::getId(const Int_t m, const Int_t e, const Int_t s,Int_t &rid)
{
  if(!checkModule(m) && !checkEta(e) && !checkSub(s)){
    rid = 40*(m-1) + 20*(s-1) + (21-e); // only for bemc and bprs
    return 0;
  }
  else {
    printf("<W> Det %i bad index m %i e %i s %i \n", mDetector, m, e, s); 
    return 1;
  }
}

#endif
