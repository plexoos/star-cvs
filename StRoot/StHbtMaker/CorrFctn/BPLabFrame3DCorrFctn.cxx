/***************************************************************************
 *
 * $Id: BPLabFrame3DCorrFctn.cxx,v 1.2 2000/08/02 01:25:10 lisa Exp $
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *   3D Bertsch-Pratt decomposition in the LAB (STAR c.m.) frame
 *
 ***************************************************************************
 *
 * $Log: BPLabFrame3DCorrFctn.cxx,v $
 * Revision 1.2  2000/08/02 01:25:10  lisa
 * Add Coulomb correction capability to 3D Bertsch-Pratt CorrFctn
 *
 * Revision 1.1  2000/07/31 01:19:23  lisa
 * add PairCut which contains collection of PairCuts - also 3D bertsch-pratt CorrFctn
 *
 *
 **************************************************************************/

#include "StHbtMaker/CorrFctn/BPLabFrame3DCorrFctn.h"
//#include "StHbtMaker/Infrastructure/StHbtHisto.hh"
#include <cstdio>

#ifdef __ROOT__ 
ClassImp(BPLabFrame3DCorrFctn)
#endif

//____________________________
BPLabFrame3DCorrFctn::BPLabFrame3DCorrFctn(char* title, const int& nbins, const float& QLo, const float& QHi){

  // set some stuff...
  mQinvNormLo = 0.15;
  mQinvNormHi = 0.18;
  mNumRealsNorm = 0;
  mNumMixedNorm = 0;
  mCorrection = 0;  // pointer to Coulomb Correction object

  // set up numerator
  char TitNum[100] = "Num";
  strcat(TitNum,title);
  mNumerator = new StHbt3DHisto(TitNum,title,nbins,QLo,QHi,nbins,QLo,QHi,nbins,QLo,QHi);
  // set up denominator
  char TitDen[100] = "Den";
  strcat(TitDen,title);
  mDenominator = new StHbt3DHisto(TitDen,title,nbins,QLo,QHi,nbins,QLo,QHi,nbins,QLo,QHi);
  // set up ratio
  char TitRat[100] = "Rat";
  strcat(TitRat,title);
  mRatio = new StHbt3DHisto(TitRat,title,nbins,QLo,QHi,nbins,QLo,QHi,nbins,QLo,QHi);

  // to enable error bar calculation...
  mNumerator->Sumw2();
  mDenominator->Sumw2();
  mRatio->Sumw2();

}

//____________________________
BPLabFrame3DCorrFctn::~BPLabFrame3DCorrFctn(){
  delete mNumerator;
  delete mDenominator;
  delete mRatio;
}
//_________________________
void BPLabFrame3DCorrFctn::Finish(){
  // here is where we should normalize, fit, etc...
  double NumFact = double(mNumRealsNorm);
  double DenFact = double(mNumMixedNorm);

  mRatio->Divide(mNumerator,mDenominator,DenFact,NumFact);
}

//____________________________
StHbtString BPLabFrame3DCorrFctn::Report(){
  string stemp = "Labframe Bertsch-Pratt 3D Correlation Function Report:\n";
  char ctemp[100];
  sprintf(ctemp,"Number of entries in numerator:\t%E\n",mNumerator->GetEntries());
  stemp += ctemp;
  sprintf(ctemp,"Number of entries in denominator:\t%E\n",mDenominator->GetEntries());
  stemp += ctemp;
  sprintf(ctemp,"Number of entries in ratio:\t%E\n",mRatio->GetEntries());
  stemp += ctemp;
  sprintf(ctemp,"Normalization region in Qinv was:\t%E\t%E\n",mQinvNormLo,mQinvNormHi);
  stemp += ctemp;
  sprintf(ctemp,"Number of pairs in Normalization region was:\n");
  stemp += ctemp;
  sprintf(ctemp,"In numerator:\t%u\t In denominator:\t%u\n",mNumRealsNorm,mNumMixedNorm);
  stemp += ctemp;
  if (mCorrection)
    {
      float radius = mCorrection->GetRadius();
      sprintf(ctemp,"Coulomb correction used radius of\t%E\n",radius);
    }
  else
    {
      sprintf(ctemp,"No Coulomb Correction applied to this CorrFctn\n");
    }
  stemp += ctemp;
  //  
  StHbtString returnThis = stemp;
  return returnThis;
}
//____________________________
void BPLabFrame3DCorrFctn::AddRealPair(const StHbtPair* pair){
  double Qinv = fabs(pair->qInv());   // note - qInv() will be negative for identical pairs...
  if ((Qinv < mQinvNormHi) && (Qinv > mQinvNormLo)) mNumRealsNorm++;
  double qOut = fabs(pair->qOutBf());
  double qSide = fabs(pair->qSideBf());
  double qLong = fabs(pair->qLongBf());

  mNumerator->Fill(qOut,qSide,qLong);
}
//____________________________
void BPLabFrame3DCorrFctn::AddMixedPair(const StHbtPair* pair){
  double weight=1.0;
  if (mCorrection)
    {
      weight = mCorrection->CoulombCorrect(pair);
    }
  double Qinv = fabs(pair->qInv());   // note - qInv() will be negative for identical pairs...
  if ((Qinv < mQinvNormHi) && (Qinv > mQinvNormLo)) mNumMixedNorm++;
  double qOut = fabs(pair->qOutBf());
  double qSide = fabs(pair->qSideBf());
  double qLong = fabs(pair->qLongBf());

  mDenominator->Fill(qOut,qSide,qLong,weight);
}


