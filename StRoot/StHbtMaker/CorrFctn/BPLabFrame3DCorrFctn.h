/***************************************************************************
 *
 * $Id: BPLabFrame3DCorrFctn.h,v 1.1 2000/07/31 01:19:23 lisa Exp $
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *   This one does 3D Bertsch-Pratt decomposition in the Lab (STAR c.m.) frame
 *
 ***************************************************************************
 *
 * $Log: BPLabFrame3DCorrFctn.h,v $
 * Revision 1.1  2000/07/31 01:19:23  lisa
 * add PairCut which contains collection of PairCuts - also 3D bertsch-pratt CorrFctn
 *
 *
 **************************************************************************/

#ifndef BPLabFrame3DCorrFctn_hh
#define BPLabFrame3DCorrFctn_hh

#include "StHbtMaker/Base/StHbtCorrFctn.hh"
//#include "StHbtMaker/Infrastructure/StHbtCoulomb.h"
//#include "StHbtMaker/Infrastructure/StHbtHisto.hh"

class BPLabFrame3DCorrFctn : public StHbtCorrFctn {
public:
  BPLabFrame3DCorrFctn(char* title, const int& nbins, const float& QLo, const float& QHi);
  virtual ~BPLabFrame3DCorrFctn();

  virtual StHbtString Report();
  virtual void AddRealPair(const StHbtPair*);
  virtual void AddMixedPair(const StHbtPair*);

  virtual void Finish();

  StHbt3DHisto* Numerator();
  StHbt3DHisto* Denominator();
  StHbt3DHisto* Ratio();


  // here are get and set for the range over which the correlation function 
  // is normalized (in Qinv).  The range is set to 0.15..0.18 in the constuctor
  // by default, but the Set's below override this
  void SetNormRangeLo(float qLo);
  void SetNormRangeHi(float qHi);
  float GetNormRangeLo();
  float GetNormRangeHi();

  //  void SetCoulombCorrection(StHbtCoulomb* Correction);


private:
  StHbt3DHisto* mNumerator;
  StHbt3DHisto* mDenominator;
  StHbt3DHisto* mRatio;

  // upper and lower bounds of Qinv region where to do normalization
  float mQinvNormLo;
  float mQinvNormHi;

  // and here are the number of pairs in that region...
  unsigned long int mNumRealsNorm;
  unsigned long int mNumMixedNorm;

  //  StHbtCoulomb* mCorrection;


#ifdef __ROOT__
  ClassDef(BPLabFrame3DCorrFctn, 1)
#endif
};

inline  StHbt3DHisto* BPLabFrame3DCorrFctn::Numerator(){return mNumerator;}
inline  StHbt3DHisto* BPLabFrame3DCorrFctn::Denominator(){return mDenominator;}
inline  StHbt3DHisto* BPLabFrame3DCorrFctn::Ratio(){return mRatio;}
inline  void BPLabFrame3DCorrFctn::SetNormRangeLo(float qLo){mQinvNormLo = qLo;}
inline  void BPLabFrame3DCorrFctn::SetNormRangeHi(float qHi){mQinvNormHi = qHi;}
inline  float BPLabFrame3DCorrFctn::GetNormRangeLo(){return mQinvNormLo;}
inline  float BPLabFrame3DCorrFctn::GetNormRangeHi(){return mQinvNormHi;}
//inline  void BPLabFrame3DCorrFctn::SetCoulombCorrection(StHbtCoulomb* Correction){mCorrection = Correction;}

#endif

