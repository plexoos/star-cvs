/***************************************************************************
 *
 * $Id: StDedxPidTraits.cxx,v 2.11 2004/03/01 17:44:37 fisyak Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StDedxPidTraits.cxx,v $
 * Revision 2.11  2004/03/01 17:44:37  fisyak
 * Add Print method
 *
 * Revision 2.10  2003/04/30 18:05:55  fisyak
 * Add P03ia flag, which fixes P03ia MuDst
 *
 * Revision 2.9  2001/04/05 04:00:47  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.8  2001/03/24 03:34:40  perev
 * clone() -> clone() const
 *
 * Revision 2.7  2000/12/18 17:25:13  fisyak
 * Add track length used in dE/dx calculations
 *
 * Revision 2.6  2000/01/05 16:04:11  ullrich
 * Changed method name sigma() to errorOnMean().
 *
 * Revision 2.5  1999/11/29 17:07:24  ullrich
 * Moved method() from StTrackPidTraits to StDedxPidTraits.cxx
 *
 * Revision 2.4  1999/11/23 15:56:23  ullrich
 * Added clone() const method. Was pure virtual.
 *
 * Revision 2.3  1999/11/16 14:11:38  ullrich
 * Changed variance to sigma.
 *
 * Revision 2.2  1999/10/28 22:25:01  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.1  1999/10/13 19:44:31  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "tables/St_dst_dedx_Table.h"
#include "StDedxPidTraits.h"
#include "StBichsel/Bichsel.h"
#include "TMath.h"
#include "Stiostream.h"
ClassImp(StDedxPidTraits)

static const char rcsid[] = "$Id: StDedxPidTraits.cxx,v 2.11 2004/03/01 17:44:37 fisyak Exp $";

StDedxPidTraits::StDedxPidTraits() :
    mNumberOfPoints(0), mDedx(0), mSigma(0), mMethod(0) { /* noop */ }

StDedxPidTraits::StDedxPidTraits(StDetectorId det, short meth,
                                 unsigned short n, float dedx, float sig) :
    StTrackPidTraits(det),
    mNumberOfPoints(n), mDedx(dedx), mSigma(sig), mMethod(meth) { /* noop */ }

StDedxPidTraits::StDedxPidTraits(const dst_dedx_st& t) :
    StTrackPidTraits(t),
    mNumberOfPoints(t.ndedx), mDedx(t.dedx[0]),
    mSigma(t.dedx[1]), mMethod(t.method){ /* noop */ }

StDedxPidTraits::~StDedxPidTraits() { /* noop */ }

unsigned short
StDedxPidTraits::numberOfPoints() const { return mNumberOfPoints%100; }
float
StDedxPidTraits::length() const { return (mNumberOfPoints/100); }

float
StDedxPidTraits::mean() const { 
#ifdef P03ia
  static const Int_t N70 = 6;
  static const Double_t T70[N70] = {
    -1.55822e+01,
    2.15621e+01,
    -1.24423e+01,
    3.68329e+00,
    -5.50226e-01,
    3.27863e-02
  };
  float dEdx = mDedx;
  if (method() == kTruncatedMeanId) {
    Double_t x = TMath::Log(length());
    Double_t Corr = T70[N70-1]; for (int i = N70-2; i >= 0; i--) Corr = Corr*x + T70[i];
    dEdx *= TMath::Exp(-Corr);
  }
  return dEdx;
#else
  return mDedx; 
#endif
}

float
StDedxPidTraits::errorOnMean() const { 
#ifndef P03ia
  return mSigma; 
#else
  static const Int_t N70 = 6;
  static const Double_t T70[N70] = {
    1.06856e+01,
    -1.52626e+01,
    8.62097e+00,
    -2.37276e+00,
    3.17727e-01,
    -1.65739e-02,
  };
  float Sigma;
  if (method() == kTruncatedMeanId) {
    Double_t x = TMath::Log(length());
    Double_t Corr = T70[N70-1]; for (int i = N70-2; i >= 0; i--) Corr = Corr*x + T70[i];
    Sigma = Corr;
    return Sigma;
  }
  else return mSigma; 
#endif
}

StObject*
StDedxPidTraits::clone() const { return new StDedxPidTraits(*this); }

short
StDedxPidTraits::encodedMethod() const { return mMethod; }

StDedxMethod
StDedxPidTraits::method() const
{
    switch (mMethod) {
    case kTruncatedMeanId:
        return kTruncatedMeanId;
        break;
    case kEnsembleTruncatedMeanId:
        return kEnsembleTruncatedMeanId;
        break;
    case kLikelihoodFitId:
        return kLikelihoodFitId;
        break;
    case kWeightedTruncatedMeanId:
        return kWeightedTruncatedMeanId;
        break;
    case kOtherMethodId:
        return kOtherMethodId;
        break;
    default:
        return kUndefinedMethodId;
        break;
    }
}
//________________________________________________________________________________
void StDedxPidTraits::Print(Option_t *opt) const {
  cout << "StDedxPidTraits : \t method" << method()
       << "\t encodedMethod :  " << encodedMethod()
       << "\t numberOfPoints : " << numberOfPoints() 
       << "\t length : "         << length()         
       << "\t mean : "           << mean()           
       << "\t errorOnMean : "    << errorOnMean()  << endl;
       
 }

