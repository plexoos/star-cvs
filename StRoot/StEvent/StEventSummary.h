/***************************************************************************
 *
 * $Id: StEventSummary.h,v 2.3 2001/04/05 04:00:36 ullrich Exp $
 *
 * Author: Thomas Ullrich, July 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEventSummary.h,v $
 * Revision 2.3  2001/04/05 04:00:36  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.2  2000/01/14 19:06:51  ullrich
 * Made code more robust if read-in table is not well defined.
 *
 * Revision 2.1  1999/10/13 19:43:02  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StEventSummary_hh
#define StEventSummary_hh

#include <iostream.h>
#include "StObject.h"
#include "StEnumerations.h"
#include "StThreeVectorF.hh"
#include "TString.h"
#include "TArrayF.h"
#include "TArrayL.h"

class dst_event_summary_st;
class dst_summary_param_st;

class StEventSummary : public StObject {
public:
    StEventSummary();
    StEventSummary(const dst_event_summary_st&,
                   const dst_summary_param_st&);
    virtual ~StEventSummary();
    
    // StEventSummary& operator=(const StEventSummary&); use default
    // StEventSummary(const StEventSummary&);            use default
    
    int    numberOfTracks() const;
    int    numberOfGoodTracks() const;
    int    numberOfGoodTracks(StChargeSign) const;
    int    numberOfGoodPrimaryTracks() const;
    int    numberOfExoticTracks() const;
    int    numberOfVertices() const;
    int    numberOfVerticesOfType(StVertexId) const;
    int    numberOfPileupVertices() const;
    float  meanPt() const;
    float  meanPt2() const;
    float  meanEta() const;
    float  rmsEta() const;
    double magneticField() const;
    
    const StThreeVectorF& primaryVertexPosition() const;

    unsigned int   numberOfBins() const;
    int    tracksInEtaBin(unsigned int) const;
    int    tracksInPhiBin(unsigned int) const;
    int    tracksInPtBin(unsigned int) const;
    float  energyInEtaBin(unsigned int) const;
    float  energyInPhiBin(unsigned int) const;

    float  lowerEdgeEtaBin(unsigned int) const;
    float  upperEdgeEtaBin(unsigned int) const;
    float  lowerEdgePhiBin(unsigned int) const;
    float  upperEdgePhiBin(unsigned int) const;
    float  lowerEdgePtBin(unsigned int) const;
    float  upperEdgePtBin(unsigned int) const;
    
    void setNumberOfTracks(int);
    void setNumberOfGoodTracks(int);
    void setNumberOfGoodTracks(StChargeSign, int);
    void setNumberOfGoodPrimaryTracks(int);
    void setNumberOfNegativeTracks(int);
    void setNumberOfExoticTracks(int);
    void setNumberOfVertices(int);
    
    void setNumberOfVerticesForType(StVertexId, int);
    void setNumberOfPileupVertices(int);
    void setMeanPt(float);
    void setMeanPt2(float);
    void setMeanEta(float);
    void setRmsEta(float);
    void setPrimaryVertexPosition(const StThreeVectorF&);
    void setMagneticField(double);

protected:
    enum { mVertexTypeArraySize = 5,
           mPhiBinsSize = 10,
           mPtAndEtaBinsSize = 9,
           mHistogramSize = 10 };
    
    Int_t          mNumberOfTracks;
    Int_t          mNumberOfGoodTracks;
    Int_t          mNumberOfGoodPrimaryTracks;
    Int_t          mNumberOfPositiveTracks;
    Int_t          mNumberOfNegativeTracks;
    Int_t          mNumberOfExoticTracks;
    Int_t          mNumberOfVertices;
    TArrayL        mNumberOfVertexTypes;
    Int_t          mNumberOfPileupVertices;
    Float_t        mMeanPt;
    Float_t        mMeanPt2;
    Float_t        mMeanEta;
    Float_t        mRmsEta;
    StThreeVectorF mPrimaryVertexPos;

    TArrayF        mEtaBins;
    TArrayF        mPtBins;
    TArrayF        mPhiBins;

    TArrayL        mEtaOfTracksHisto;
    TArrayL        mPtOfTracksHisto;
    TArrayL        mPhiOfTracksHisto;
    TArrayF        mEneryVsEtaHisto;
    TArrayF        mEnergyVsPhiHisto;
    Double_t       mMagneticFieldZ;
    
    ClassDef(StEventSummary,1)
};

#endif
