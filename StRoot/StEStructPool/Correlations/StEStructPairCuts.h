/**********************************************************************
 *
 * $Id: StEStructPairCuts.h,v 1.4 2004/06/25 03:11:50 porter Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  Cut class for track-pair level quantities
 *
 *
 ***********************************************************************/
#ifndef __STEBYEPAIRCUTS_H
#define __STEBYEPAIRCUTS_H

#include "StEStructPool/AnalysisMaker/StEStructCuts.h"

#include "StEStructPool/EventMaker/StEStructTrack.h"
#include "Stiostream.h"


class StEStructPairCuts : public StEStructCuts {

protected:


  CutName mdphiName;
  CutName mdetaName;
  CutName mdmtName;
  CutName mqInvName;
  CutName mEntSepName;
  CutName mExitSepName;
  CutName mQualityName;
  CutName mMidTpcSepLSName;
  CutName mMidTpcSepUSName;

  float mdphi[2];
  float mdeta[2];
  float mdmt[2];
  float mqInv[2];
  float mEntSep[2];
  float mExitSep[2];
  float mQuality[2];
  float mMidTpcSepLS[2];
  float mMidTpcSepUS[2];

  bool  mdeltaPhiCut,    mdeltaEtaCut,    mdeltaMtCut;
  bool  mqInvCut,        mEntSepCut,      mExitSepCut,   mQualityCut;
  bool  mMidTpcSepLSCut, mMidTpcSepUSCut;
 
  int   mdphiCounter[4], mdetaCounter[4], mdmtCounter[4];
  int   mqInvCounter[4],   mEntSepCounter[4],  mExitSepCounter[4],  mQualityCounter[4];
  int   msplitLSCounter[4],  msplitUSCounter[4];

  // if data stored for subsequent use.... e.g. next cut, historamming
  float  mdeltaPhi, mdeltaEta, mdeltaMt;
  float  mqInvarient,  mEntranceSeparation, mExitSeparation, mQualityVal;
  float  mMidTpcSeparationLS, mMidTpcSeparationUS;

  int  mType;
  unsigned long mapMask0;
  unsigned long mapMask1;
  unsigned long bitI[32];

  int mretVal; //! just a dummy holder used a lot

  int  mcutMode;  // cut space definitions

  void init();
  void initCuts();
  void initNames();

  StEStructTrack* mTrack1;
  StEStructTrack* mTrack2;

public:

  StEStructPairCuts();
  StEStructPairCuts(const char* cutFileName);
  virtual ~StEStructPairCuts();

  virtual bool loadBaseCuts(const char* name, const char** vals, int nvals);
  virtual void loadUserCuts(const char* name, const char** vals, int nvals);
  virtual void printCuts(ostream& ofs);
  virtual void printCuts(const char* fname) { StEStructCuts::printCuts(fname); };
  virtual void printCuts(ostream& ofs, char* cutType,int c1, int c2);

  // StEStructPairCuts stuff

const  StEStructTrack*      Track1() const;
const  StEStructTrack*      Track2() const;
       void                 SetTrack1(const StEStructTrack* trkPtr);
       void                 SetTrack2(const StEStructTrack* trkPtr);


//-----For Auto Correlationn Study
       float                 DeltaMt()   const;
       float                 DeltaXt()   const;
       float                 DeltaYt()   const;
       float                 DeltaEta()   const;
       float                 DeltaPhi()   const;
       float                 SigmaMt()   const;
       float                 SigmaXt()   const;
       float                 SigmaYt()   const;
       float                 SigmaEta()   const;
       float                 SigmaPhi()   const;
       float                 SigmaPt()    const;


//-----For HBT Study
       StLorentzVectorF  fourMomentumDiff() const;
       StLorentzVectorF  fourMomentumSum()  const;
       float                 qInv()   const;
       float                 kT()     const;
       float                 mInv()   const;

//-----For Pair Cut
       double                  quality()                   const;
       double                  NominalTpcExitSeparation()    const;
       double                  NominalTpcEntranceSeparation() const;
       double                  OpeningAngle()               const;
       double                  MidTpcXYSeparation()    const;
       double                  MidTpcZSeparation()    const;
       double                  MidTpcSeparation() const;
       void setPairType(int type);
       int  cutPair();
       int  cutPair(bool doHistograms);
       int  cutPairHistograms();

	 // explicit cut coding... 
	 // for Pair cuts we have to break the model of
	 // non-duplicate codes simply because even checking a 
	 // bool for each cut is expensive when done N1*N2*Ncut times

        /* because of the expense 1st check wether we need to do further */

	 int  goodDeltaPhi();
	 int  goodDeltaEta();
         int  goodDeltaMt();

	 int  cutDeltaPhi();
	 int  cutDeltaEta();
         int  cutDeltaMt();
         int  cutqInvORNominalEntranceSep();
         int  cutqInv();
         int  cutEntranceSep();
	 int  cutMidTpcSepUS();
         int  cutMidTpcSepLS();
         int  cutExitSep();
         int  cutQuality();

	 // calls above set but fills histogramming variables
	 int  cutDeltaPhiH();
	 int  cutDeltaEtaH();
         int  cutDeltaMtH();
         int  cutqInvH();
         int  cutEntranceSepH();
	 int  cutMidTpcSepUSH();
         int  cutMidTpcSepLSH();
         int  cutExitSepH();
         int  cutQualityH();

	 // for new GodPair study

	 int correlationDepth();



  ClassDef(StEStructPairCuts,1)
};

inline void StEStructPairCuts::loadUserCuts(const char* name, const char** vals, int nvals){ }

inline void StEStructPairCuts::setPairType(int type) { mType=type; }
inline int  StEStructPairCuts::cutPair(bool doHistograms){
  if(!doHistograms) return cutPair();
  return cutPairHistograms();
}


inline void StEStructPairCuts::SetTrack1(const StEStructTrack* trkPtr){
  mTrack1=(StEStructTrack*)trkPtr;
}

inline void StEStructPairCuts::SetTrack2(const StEStructTrack* trkPtr){
  mTrack2=(StEStructTrack*)trkPtr;
}

inline const StEStructTrack* StEStructPairCuts::Track1() const {return mTrack1;}
inline const StEStructTrack* StEStructPairCuts::Track2() const {return mTrack2;}

inline float StEStructPairCuts::SigmaMt() const {
  return mTrack1->FourMomentum().mt()+mTrack2->FourMomentum().mt();
}
inline float StEStructPairCuts::SigmaXt() const {
  return mTrack1->Xt()+mTrack2->Xt();
}
inline float StEStructPairCuts::SigmaYt() const {
  return mTrack1->Yt()+mTrack2->Yt();
}
inline float StEStructPairCuts::SigmaEta() const {
  return mTrack1->Eta()+mTrack2->Eta();
}
inline float StEStructPairCuts::SigmaPhi() const {
  return mTrack1->Phi()+mTrack2->Phi();
}

inline float StEStructPairCuts::SigmaPt() const {
  return mTrack1->Pt()+mTrack2->Pt();
}

inline float StEStructPairCuts::DeltaMt() const {
  return fabs(mTrack1->FourMomentum().mt()-mTrack2->FourMomentum().mt());
}
inline float StEStructPairCuts::DeltaXt() const {
  return fabs(mTrack1->Xt()-mTrack2->Xt());
}
inline float StEStructPairCuts::DeltaYt() const {
  return fabs(mTrack1->Yt()-mTrack2->Yt());
}
inline float StEStructPairCuts::DeltaEta() const {
  return fabs(mTrack1->Eta()-mTrack2->Eta());
}
inline float StEStructPairCuts::DeltaPhi() const {
  float delphi=fabs(mTrack1->Phi()-mTrack2->Phi());
  //  float retVal= (delphi<M_PI) ? delphi : (2*M_PI)-delphi;
  //  return (retVal>-0.5*M_PI) ? retVal : retVal+2*M_PI;
  return (delphi<M_PI) ? delphi : (2*M_PI)-delphi;
}

inline float StEStructPairCuts::mInv() const {
  return abs(mTrack1->FourMomentum()+mTrack2->FourMomentum());
}

inline float StEStructPairCuts::kT() const {
  return 0.5*(mTrack1->FourMomentum()+mTrack2->FourMomentum()).perp();
}

inline float StEStructPairCuts::qInv() const {
  return -1.0*(mTrack1->FourMomentum()-mTrack2->FourMomentum()).m();
}

inline int StEStructPairCuts::goodDeltaPhi(){
  if ( mdeltaPhiCut &&
       ( (mdeltaPhi=fabs(DeltaPhi())) < mdphi[1])) return 0;
  return 1;
}

inline int StEStructPairCuts::goodDeltaEta(){
  if(mdeltaEtaCut &&
     ( (mdeltaEta=fabs(DeltaEta())) <mdeta[1])) return 0;
  return 1;
}

inline int StEStructPairCuts::goodDeltaMt(){
  if(mdeltaMtCut &&
     ( (mdeltaMt=fabs(DeltaMt())) <mdmt[1] )) return 0;
  return 1;
}


inline int StEStructPairCuts::cutDeltaPhi(){
  if ( mdeltaPhiCut &&
       ( (mdeltaPhi=DeltaPhi()) <mdphi[0] || mdeltaPhi>mdphi[1])  
      ) return ++(mdphiCounter[mType]);
  return 0;
}

inline int StEStructPairCuts::cutDeltaEta(){
  if(mdeltaEtaCut &&
     ( (mdeltaEta=DeltaEta()) <mdeta[0] || mdeltaEta>mdeta[1]) 
     ) return ++(mdetaCounter[mType]);
  return 0;
}


inline int StEStructPairCuts::cutDeltaMt(){
  if(mdeltaMtCut &&
     ( (mdeltaMt=DeltaMt()) <mdmt[0] || mdeltaMt>mdmt[1]) 
     ) return ++(mdmtCounter[mType]);
  return 0;
}

inline int StEStructPairCuts::cutqInvORNominalEntranceSep(){
  /* small qInv and entrance cut */

  if( mqInvCut && mEntSepCut &&
      (  (mqInvarient=qInv()) <mqInv[0]  &&
	 ((mEntranceSeparation=NominalTpcEntranceSeparation())<mEntSep[0]))
      ) return ++(mqInvCounter[mType]);
  return 0;
}

inline int StEStructPairCuts::cutqInv(){
  if( mqInvCut && 
      (  (mqInvarient=qInv())<mqInv[0] 
	 || mqInvarient>mqInv[1] 
	 )                  
      ) return ++(mqInvCounter[mType]);
  return 0;
}

inline int StEStructPairCuts::cutEntranceSep(){
  if( mEntSepCut && 
      ( (mEntranceSeparation=NominalTpcEntranceSeparation())<mEntSep[0] 
	|| mEntranceSeparation>mEntSep[1]  
	)                  
      ) return ++(mEntSepCounter[mType]);
  return 0;
}


inline int StEStructPairCuts::cutMidTpcSepUS(){
  if( mMidTpcSepUSCut ){
    double x1=mMidTpcSeparationUS=MidTpcSeparation();
    double x2=NominalTpcEntranceSeparation();
    double x3=NominalTpcExitSeparation();
    if(((x1+x2+x3)/3.)>mMidTpcSepUS[1])return 0; // ok, average is large
    if( x1<x2 && x1<x3 && x1<mMidTpcSepUS[0]) return ++(msplitUSCounter[mType]);
  }
  return 0;
}

inline int StEStructPairCuts::cutMidTpcSepLS(){
  if( mMidTpcSepLSCut ){
    double x1=mMidTpcSeparationLS=MidTpcSeparation();
    double x2=NominalTpcEntranceSeparation();
    double x3=NominalTpcExitSeparation();
    if(((x1+x2+x3)/3.)>mMidTpcSepLS[1])return 0; //ok, average is large
    if( x1<x2 && x1<x3 && x1<mMidTpcSepLS[0]) return ++(msplitLSCounter[mType]);  }
  return 0;
}


inline int StEStructPairCuts::cutExitSep(){
   if( mExitSepCut && 
       ( (mExitSeparation=NominalTpcExitSeparation()) <mExitSep[0])
       // || mExitSeparation>mExitSep[1] )
       ) return ++(mExitSepCounter[mType]);
   return 0;
}

inline int StEStructPairCuts::cutQuality(){
  if( mQualityCut && 
      ( (mQualityVal=quality()) <mQuality[0]
	|| mQualityVal>mQuality[1] )
      ) return ++(mQualityCounter[mType]);
  return 0;
}


inline int StEStructPairCuts::cutDeltaPhiH(){
  if(!mdeltaPhiCut) return 0;
  mretVal=cutDeltaPhi();;
  mvalues[mdphiName.idx]=mdeltaPhi;
  return mretVal;
}

inline int StEStructPairCuts::cutDeltaEtaH(){
  if(!mdeltaEtaCut){
    mdeltaEta=DeltaEta(); // still needed for other cuts
    return 0;
  }
  mretVal=cutDeltaEta();
  mvalues[mdetaName.idx]=mdeltaEta;
  return mretVal;
}

inline int StEStructPairCuts::cutDeltaMtH(){
  if(!mdeltaMtCut) return 0;
  mretVal=cutDeltaMt();;
  mvalues[mdmtName.idx]=mdeltaMt;
  return mretVal;
}

inline int StEStructPairCuts::cutqInvH(){
  if(!mqInvCut) return 0;
  mretVal=cutqInv();;
  mvalues[mqInvName.idx]=mqInvarient;
  return mretVal;
}

inline int StEStructPairCuts::cutEntranceSepH(){
  if(!mEntSepCut) return 0;
  mretVal=cutEntranceSep();;
  mvalues[mEntSepName.idx]=mEntranceSeparation;
  return mretVal;
}

inline int StEStructPairCuts::cutMidTpcSepLSH(){
  if(!mMidTpcSepLSCut) return 0;
  mretVal=cutMidTpcSepLS();;
  mvalues[mMidTpcSepLSName.idx]=mMidTpcSeparationLS;
  return mretVal;
}

inline int StEStructPairCuts::cutMidTpcSepUSH(){
  if(!mMidTpcSepUSCut) return 0;
  mretVal=cutMidTpcSepUS();;
  mvalues[mMidTpcSepUSName.idx]=mMidTpcSeparationUS;
  return mretVal;
}


inline int StEStructPairCuts::cutExitSepH(){
  if(!mExitSepCut) return 0;
  mretVal=cutExitSep();;
  mvalues[mExitSepName.idx]=mExitSeparation;
  return mretVal;
}

inline int StEStructPairCuts::cutQualityH(){
  if(!mQualityCut) return 0;
  mretVal=cutQuality();;
  mvalues[mQualityName.idx]=mQualityVal;
  return mretVal;
}


inline int StEStructPairCuts::correlationDepth(){

  unsigned int am1=mTrack1->TopologyMapData(0)&65535;
  unsigned int am2=mTrack2->TopologyMapData(0)&65535;
  if(am1==0 || am2==0) return 0;
  if(am1==am2) return 1;

  unsigned int msk=(65535<<16);

  unsigned int bm1=mTrack1->TopologyMapData(0)&msk;
  unsigned int bm2=mTrack2->TopologyMapData(0)&msk;
  bm1=(16>>bm1);
  bm2=(16>>bm2);

  if(am1==bm2 || bm1==am2) return 2;
  if(bm1==bm2){
    if(bm1==0) return 0;
    return 3;
  }

  unsigned int cm1=mTrack1->TopologyMapData(1)&65535;
  unsigned int cm2=mTrack2->TopologyMapData(1)&65535;
  if(cm1==cm2 && cm1==0) return 0;
 
  if(am1==cm2 || am2==cm1) return 4;
  if( ((bm1==cm2) && bm1!=0) || ((bm2==cm1)&&(bm2!=0)) ) return 4;
  if( cm1==cm2) return 5;

  unsigned int dm1=mTrack1->TopologyMapData(1)&msk;
  unsigned int dm2=mTrack2->TopologyMapData(1)&msk;
  dm1=(16>>dm1);
  dm2=(16>>dm2);
  if(dm1==dm2 && dm1==0) return 0;

  if(am1==dm2 || am2==dm1) return 6;
  if( ((bm1==dm2) && bm1!=0) || ((bm2==dm1)&&(bm2!=0)) ) return 6;
  if( ((cm1==dm2) && cm1!=0) || ((cm2==dm1)&&(cm2!=0)) ) return 6;
  if( dm1==dm2 && dm1!=0) return 7;

  return 0;
}



#endif

/***********************************************************************
 *
 * $Log: StEStructPairCuts.h,v $
 * Revision 1.4  2004/06/25 03:11:50  porter
 * New cut-binning implementation and modified pair-cuts for chunhui to review
 *
 * Revision 1.3  2004/06/16 20:00:43  chunhuih
 *
 * changed one more post-increment operator to pre-increment operator.
 *
 * Revision 1.2  2004/03/19 19:07:43  chunhuih
 *
 * Use pre-increment instead of post-increment operators for the return values
 * of a set of cut methods. This returns the correct cut value for the first
 * time the counting array is incremented.
 *
 * Revision 1.1  2003/10/15 18:20:46  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/







