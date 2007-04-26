//StJetSkimEvent
//M.L. Miller (MIT)
//12/06

#ifndef StJetSkimEvent_HH
#define StJetSkimEvent_HH

#include "TObject.h"
#include "TClonesArray.h"
#include "TArrayI.h"
#include "TObjString.h"
#include "TRef.h"
#include "TLorentzVector.h"

#include <map>

class StJetSkimTrigHeader : public TObject
{
public:
    StJetSkimTrigHeader();
    StJetSkimTrigHeader(const StJetSkimTrigHeader & other);
    virtual ~StJetSkimTrigHeader();
    StJetSkimTrigHeader& operator=(const StJetSkimTrigHeader & rhs);
    
    void Clear(const char *option="");
    
    Int_t       runId;
    Int_t       trigId;
    
    Float_t     prescale;
    
    Int_t       eastBarrelTowerThreshold;
    Int_t       eastBarrelTriggerPatchThreshold;
    Int_t       eastBarrelJetPatchThreshold;
    
    Int_t       westBarrelTowerThreshold;
    Int_t       westBarrelTriggerPatchThreshold;
    Int_t       westBarrelJetPatchThreshold;
    
    Int_t       endcapTowerThreshold;
    Int_t       endcapTriggerPatchThreshold;
    Int_t       endcapJetPatchThreshold;
    
    Int_t       totalEnergyThreshold;
    
private:
    void init();
    
    ClassDef(StJetSkimTrigHeader,1);
};

class StJetSkimTrig : public TObject
{
public:
    StJetSkimTrig();
    StJetSkimTrig(const StJetSkimTrig& other);
    virtual ~StJetSkimTrig();
    StJetSkimTrig& operator=(const StJetSkimTrig& rhs);
    
    void clear();
    void Clear(const char *option="");
    
    int trigId() const;
    bool didFire() const;    
    int shouldFire() const;
    
    //detector:  0 == BEMC, 1 == EEMC
    std::map<int,int>& towersAboveThreshold(int detector) const;
    std::map<int,int>& triggerPatchesAboveThreshold(int detector) const;
    std::map<int,int>& jetPatchesAboveThreshold(int detector) const;
    
    int totalEnergy() const;
    
    //setters
    void setTrigId(int aTrigId);
    void setDidFire(bool aFire);
    void setShouldFire(int aFire);
    
    //detector:  0 == BEMC, 1 == EEMC
    void addTowerAboveThreshold(int detector, int aID, int aADC);    
    void addTriggerPatchAboveThreshold(int detector, int aID, int aADC);
    void addJetPatchAboveThreshold(int detector, int aID, int aADC);
    
    void setTotalEnergy(int aEnergy);
    
private:
    void init();
    
    Int_t       mTrigId;
    Int_t       mDidFire;
    Int_t       mShouldFire;
    
    map<int,int> mTowers;
    map<int,int> mTriggerPatches;
    map<int,int> mJetPatches;
    
    Int_t mTotalEnergy;
    
    ClassDef(StJetSkimTrig,3);
};
    
inline int StJetSkimTrig::trigId() const {return mTrigId;}
inline bool StJetSkimTrig::didFire() const  {return (mDidFire > 0) ? true : false;}
inline int StJetSkimTrig::shouldFire() const  {return mShouldFire;}
inline int StJetSkimTrig::totalEnergy() const {return mTotalEnergy;}

inline void StJetSkimTrig::setTrigId(int aTrigId) {mTrigId = aTrigId;}
inline void StJetSkimTrig::setDidFire(bool aFire) {mDidFire = aFire;}
inline void StJetSkimTrig::setShouldFire(int aFire) {mShouldFire = aFire;}
inline void StJetSkimTrig::setTotalEnergy(int aEnergy) {mTotalEnergy = aEnergy;}

class StJetSkimVert : public TObject {
    
public:
    StJetSkimVert();
    virtual ~StJetSkimVert() {};
    
    //action
    void clear();
    void Clear(const char *option="");
    bool operator==(const StJetSkimVert &rhs) const;
    
    //sets
    void setPosition(float* x);
    void setError(float* y);
    
    void setVertexFinderId(int i) {mVertexFinderId = i;}
    void setRanking(Float_t i) { mRanking = i;}
    void setNTracksUsed(UShort_t i) {mNTracksUsed = i;}
    void setNCTBMatch(UShort_t i) { mNCTBMatch = i;}
    void setNBEMCMatch(UShort_t i) { mNBEMCMatch = i;}
    void setNEEMCMatch(UShort_t i) { mNEEMCMatch = i;}
    void setNCrossingCentralMembrane(UShort_t i) { mNCrossCentralMembrane = i;}
    void setSumTrackPt(Float_t i) { mSumTrackPt = i;}
    void setMeanDip(Float_t i) { mMeanDip = i;}
    void setChiSquared(Float_t i) { mChiSquared = i;}
    
    // RefMult fields
    void setRefMultNeg( UShort_t i) { mRefMultNeg = i;}
    void setRefMultPos(UShort_t i) { mRefMultPos = i;}
    void setRefMultFtpcWest(UShort_t i) { mRefMultFtpcWest = i;}
    void setRefMultFtpcEast( UShort_t i) { mRefMultFtpcEast = i;}
    
    //gets
    const float* position() const { return mPosition;}
    const float*  posError() const { return mPosError;}
    int vertexFinderId() const { return mVertexFinderId;} 
    Float_t ranking()  const { return mRanking;}
    UShort_t nTracksUsed() const { return mNTracksUsed;}
    UShort_t nCTBMatch() const { return mNCTBMatch;}
    UShort_t nBEMCMatch() const { return mNBEMCMatch;}
    UShort_t nEEMCMatch() const { return mNEEMCMatch;}
    UShort_t nCrossCentralMembrane() const  {return mNCrossCentralMembrane;}
    Float_t sumTrackPt() const { return mSumTrackPt; }
    Float_t meanDip() const { return mMeanDip; }
    Float_t chiSquared() const { return mChiSquared; }
    
    UShort_t refMultPos() const { return mRefMultPos; }
    UShort_t refMultNeg() const { return mRefMultNeg; }
    UShort_t refMult() const { return refMultPos() + refMultNeg(); }
    UShort_t refMultFtpcEast() const { return mRefMultFtpcEast; }
    UShort_t refMultFtpcWest() const { return mRefMultFtpcWest; }
    UShort_t refMultFtpc() const { return refMultFtpcEast() + refMultFtpcWest(); }
    
private:

    float mPosition[3];
    float mPosError[3];
    
    int mVertexFinderId;
    Float_t mRanking;
    UShort_t mNTracksUsed;
    UShort_t mNCTBMatch;
    UShort_t mNBEMCMatch;
    UShort_t mNEEMCMatch;
    UShort_t mNCrossCentralMembrane;
    Float_t mSumTrackPt;
    Float_t mMeanDip;
    Float_t mChiSquared;
    
    // RefMult fields
    UShort_t mRefMultNeg;
    UShort_t mRefMultPos;
    UShort_t mRefMultFtpcWest;
    UShort_t mRefMultFtpcEast;
    
    ClassDef(StJetSkimVert,1);
};

class StJetSkimParton : public TLorentzVector {
public:
    StJetSkimParton();
    StJetSkimParton(const StJetSkimParton &other);
    virtual ~StJetSkimParton();
    StJetSkimParton& operator=(const StJetSkimParton &rhs);
    
private:
    UInt_t mFlavor;
    Float_t mStatus;
    Float_t mMother1;
    Float_t mMother2;
    Float_t mDaughter1;
    Float_t mDaughter2;
    
    ClassDef(StJetSkimParton,1);
};

class StJetSkimMcEvent : public TObject {
public:
    StJetSkimMcEvent();
    StJetSkimMcEvent(const StJetSkimMcEvent &other);
    virtual ~StJetSkimMcEvent();
    StJetSkimMcEvent& operator=(const StJetSkimMcEvent &rhs);
    
//    void Clear(const char *option="");
    
private:
    UShort_t mSubProcessID;
    Float_t mS;
    Float_t mT;
    Float_t mU;
    Float_t mX1;
    Float_t mX2;
    Float_t mPartonicPt;
    Float_t mCosTheta;
    
    Double_t mPartonicAsymmetry;
    Double_t mPolarizedPDF1[5];
    Double_t mPolarizedPDF2[5];
    Double_t mUnpolarizedPDF1[2];
    Double_t mUnpolarizedPDF2[2];

    StJetSkimParton mParton1;
    StJetSkimParton mParton2;
    StJetSkimParton mParton3;
    StJetSkimParton mParton4;
    
    ClassDef(StJetSkimMcEvent,1);
};

class StJetSkimEvent : public TObject
{
public:
    
    StJetSkimEvent();
    StJetSkimEvent(const StJetSkimEvent &other);
    virtual ~StJetSkimEvent();
    StJetSkimEvent& operator=(const StJetSkimEvent &rhs);
    
    //action
    void clear();
    void Clear(const char *option="");
    
    //sets
    void setTrig(const StJetSkimTrig& );
    void setVert(const StJetSkimVert& );
    
    void setTrigHeaderArray(TClonesArray* array) {mTrigHeaderArrayRef = array;}
    void setTrigHeaderArray(TRef arrayRef) {mTrigHeaderArrayRef = arrayRef;}
    
    //these require vertices clones array to be filled first
    void setBestVert(const StJetSkimVert & v);
    void setBestVert(int clonesArrayIndex);
    
    void setFill(float i) {mFill = i;}
    void setRunId(int i) {mRunId = i;}
    void setEventId(int i) {mEventId = i;}
    void setMudstFileName(const TObjString& i) {mMudstFileName = i;}
    
    void setBx7(int i) {mbx7 = i;}
    void setBx48(int i) {mbx48 = i;}
    void setSpinBits(int i) {mSpinBits = i;}    
    
    void setEbbc(int i) {mEbbc = i;}
    void setWbbc(int i) {mWbbc = i;}
    void setBbcTimeBin(int i) {mBbcTimeBin = i;}
    
    void setIsValid(int i) {mIsValid = i;}
    void setIsPolLong(int i) {mIsPolLong = i;}
    void setIsPolTrans(int i) {mIsPolTrans = i;}
    void setIsMaskedUsingBx48(int i) {mIsMaskedUsingBx48 = i;}
    void setOffsetBx48minusBX7(int i) {mOffsetBx48minusBX7 = i;}
    void setSpin4UsingBx48(int i) {mSpin4usingBx48 = i;}
    
    //!void setL2Result(int* vals);
    void setL2Result(const TArrayI& rhs) {mL2Result=rhs;}
    
    //gets
    float fill() const {return mFill;}
    int runId() const {return mRunId;}
    int eventId() const {return mEventId;}
    TObjString mudstFileName() const {return mMudstFileName;}
    
    int bx7() const {return mbx7;}
    int bx48() const {return mbx48;}
    int spinBits() const {return mSpinBits;}
    
    int eBbc() const {return mEbbc;}
    int wBbc() const {return mWbbc;}
    int bbcTimeBin() const {return mBbcTimeBin;}
    
    int isValid() const {return mIsValid;}
    int isPolLong() const {return mIsPolLong;}
    int isPolTrans() const {return mIsPolTrans;}
    int isMaskedUsingBx48() const {return mIsMaskedUsingBx48;}
    int offsetBx48minusBX7() const {return mOffsetBx48minusBX7;}
    int spin4usingBx48() const {return mSpin4usingBx48;}

    const TClonesArray* triggers() const {return mTriggers;}
    TClonesArray* triggers() {return mTriggers;}
    StJetSkimTrig* trigger(int trigId);
    
    TClonesArray* trigHeaders() {return (TClonesArray*)mTrigHeaderArrayRef.GetObject();}
    StJetSkimTrigHeader* trigHeader(int trigId);
    
    const TClonesArray* vertices() const {return mVertices;}
    StJetSkimVert* bestVert() const;

    const TArrayI& l2Result() const {return mL2Result;}
    
    StJetSkimMcEvent* mcEvent();
    
private:
    
    float mFill;
    int mRunId;
    int mEventId;
    TObjString mMudstFileName;
    
    StJetSkimMcEvent* mMcEvent;
    
    TClonesArray* mTriggers;
    TRef mTrigHeaderArrayRef;
    
    TClonesArray* mVertices;
    StJetSkimVert* mBestVert;
    TRef mBestVertRef;
    
    //bunch x-ing info from MuDSt
    int mbx7;
    int mbx48;
    int mSpinBits;
    
    //bbc info:
    int mEbbc; //E-bbc summed ADC
    int mWbbc; //W-bbc summed ADC
    int mBbcTimeBin;
    
    //spin db
    int mIsValid;
    int mIsPolLong;
    int mIsPolTrans;
    int mIsMaskedUsingBx48;
    int mOffsetBx48minusBX7;
    int mSpin4usingBx48;
    
    ///L2 Trigger array:
    ///Direct copy from StMuEvent::L2Result()
    //!int mL2Result[32]; 
    TArrayI mL2Result;
    
    ClassDef(StJetSkimEvent,3);
};

#endif
