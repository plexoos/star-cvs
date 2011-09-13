// -*- mode: C++ -*-
// $Id: StPythiaEvent.h,v 1.6 2011/09/13 20:03:51 pibero Exp $

// Pibero Djawotho <pibero@indiana.edu>
// Indiana University
// 12 July 2007
//
// $Log: StPythiaEvent.h,v $
// Revision 1.6  2011/09/13 20:03:51  pibero
// DSSV2009 -> DSSV2009a
//
// Revision 1.5  2011/09/13 16:24:21  pibero
// Added DSSV2009 grid
//
// Revision 1.4  2010/10/04 19:18:29  pibero
// Fix copy constructor and assignment operator. Thanks, Alice!
//
// Revision 1.3  2009/12/14 01:22:47  pibero
// const correctness
//
// Revision 1.2  2009/12/08 15:14:24  pibero
// Added Pythia tune per Helen Caines request.
//
// Revision 1.1  2008/06/01 05:31:42  tai
// moved StPythiaEvent to StSpinPool/StJetSkimEvent
//
// Revision 1.9  2008/06/01 04:59:22  tai
// removed the dependency of StPythiaEvent on St_particle_Table.h
//
// Revision 1.8  2008/06/01 04:33:33  tai
// *** empty log message ***
//
// Revision 1.7  2008/05/02 02:36:48  rfatemi
// update ClassDef
//
// Revision 1.6  2008/05/01 01:36:39  rfatemi
// check in D. Staszak modifications - additional grids
//
// Revision 1.5  2008/02/03 01:27:17  rfatemi
// Included Gehrmann-Stirling PDFs
//
// Revision 1.4  2007/11/17 20:11:33  kocolosk
// remove printf
//
// Revision 1.3  2007/11/01 02:48:38  rfatemi
// Dave Staszak update with additional GRSV grids
//
// Revision 1.2  2007/08/06 17:06:05  rfatemi
// set default GRSV to standard
//
// Revision 1.1  2007/07/19 01:40:41  kocolosk
// use Pibero's StPythiaEvent class to supply mcAsymMaker results to user
//

#ifndef ST_PYTHIA_EVENT
#define ST_PYTHIA_EVENT

#include "TParticle.h"
#include "TClonesArray.h"

class StPythiaEvent : public TObject {
public:
  StPythiaEvent();
  virtual ~StPythiaEvent();
  StPythiaEvent(const StPythiaEvent& other);
  StPythiaEvent& operator=(const StPythiaEvent& rhs);
  enum { NPDF=31 };
  enum PDF { LO=0, NLO=1, STD=1, ZERO=2, MAX=3, MIN=4, M015=5, M030=6, M045=7, M060=8, M075=9, M090=10, M105=11, P030=12, P045=13, P060=14, P070=15, GS_NLOA=16, GS_NLOB=17, GS_NLOC=18, DSSV=19, LSS1=20, LSS2=21, LSS3=22, AAC1=23, AAC2=24, AAC3=25, BB1=26, BB2=27, DNS1=28, DNS2=29, DSSV2009a=30 };
  int runId() const;
  int eventId() const;
  int processId() const;
  int tune() const;
  const TVector3& vertex() const;
  float s() const;
  float t() const;
  float u() const;
  float pt() const;
  float cosTheta() const;
  float x1() const;
  float x2() const;
  float Q2() const;
  float partonALL() const;
  float dF1(PDF scenario = STD) const;
  float dF2(PDF scenario = STD) const;
  float f1(PDF scenario = STD) const;
  float f2(PDF scenario = STD) const;
  float ALL(PDF scenario = STD) const;

  const TClonesArray* particles() const;
  int numberOfParticles() const;
  const TParticle* particle(int i) const;
  
  void Clear(Option_t* option = "");
  void setRunId(int id);
  void setEventId(int id);
  void setProcessId(int id);
  void setTune(int tune);
  void setVertex(const TVector3& v);
  void setS(float s);
  void setT(float t);
  void setU(float u);
  void setPt(float pt);
  void setCosTheta(float cosTheta);
  void setX1(float x1);
  void setX2(float x2);
  void setPartonALL(float a);
  void setDF1(PDF scenario, float val);
  void setDF2(PDF scenario, float val);
  void setF1(PDF scenario, float val);
  void setF2(PDF scenario, float val);
  
  void addParticle(const TParticle& particle);
  
private:
  int mRunId;
  int mEventId;
  int mProcessId;
  int mTune;
  TVector3 mVertex;
  float mS;
  float mT;
  float mU;
  float mPt;
    float mCosTheta;
  float mX1;
  float mX2;
  float mPartonALL;
  float mDF1[NPDF];  //[LO][NLO][ZERO][MAX][MIN][M015][M030][M045][M060][M075][M090][M105][P030][P045][P060][P070][NLOA][NLOB][NLOC][DSSV][LSS1][LSS2][LSS3][AAC1][AAC2][AAC3][BB1][BB2][DNS1][DNS2][DSSV2009a]
  float mDF2[NPDF];  //[LO][NLO][ZERO][MAX][MIN][M015][M030][M045][M060][M075][M090][M105][P030][P045][P060][P070][NLOA][NLOB][NLOC][DSSV][LSS1][LSS2][LSS3][AAC1][AAC2][AAC3][BB1][BB2][DNS1][DNS2][DSSV2009a]
  float mF1[2];   //[LO][NLO]
  float mF2[2];   //[LO][NLO]
  
  TClonesArray* mParticles;
  
  ClassDef(StPythiaEvent,5);
};

inline int StPythiaEvent::runId() const { return mRunId; }
inline int StPythiaEvent::eventId() const { return mEventId; }
inline int StPythiaEvent::processId() const { return mProcessId; }
inline int StPythiaEvent::tune() const { return mTune; }
inline const TVector3& StPythiaEvent::vertex() const { return mVertex; }
inline float StPythiaEvent::s() const { return mS; }
inline float StPythiaEvent::t() const { return mT; }
inline float StPythiaEvent::u() const { return mU; }
inline float StPythiaEvent::pt() const { return mPt; }
inline float StPythiaEvent::cosTheta() const { return mCosTheta; }
inline float StPythiaEvent::x1() const { return mX1; }
inline float StPythiaEvent::x2() const { return mX2; }
inline float StPythiaEvent::Q2() const { return mPt * mPt; }
inline float StPythiaEvent::partonALL() const { return mPartonALL; }
inline float StPythiaEvent::dF1(PDF scenario) const { return mDF1[scenario]; }
inline float StPythiaEvent::dF2(PDF scenario) const { return mDF2[scenario]; }

inline float StPythiaEvent::f1(PDF scenario) const 
{
    if(scenario == LO) return mF1[0];
    return mF1[1];
}

inline float StPythiaEvent::f2(PDF scenario) const 
{
    if(scenario == LO) return mF2[0];
    return mF2[1];
}

inline float StPythiaEvent::ALL(PDF scenario) const
{
  switch(scenario) 
    {
    case(LO):   return (mDF1[0]*mDF2[0]*mPartonALL) / (mF1[0]*mF2[0]);
    case(NLO):  return (mDF1[1]*mDF2[1]*mPartonALL) / (mF1[1]*mF2[1]);
    case(ZERO): return (mDF1[2]*mDF2[2]*mPartonALL) / (mF1[1]*mF2[1]);
    case(MAX):  return (mDF1[3]*mDF2[3]*mPartonALL) / (mF1[1]*mF2[1]);
    case(MIN):  return (mDF1[4]*mDF2[4]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M015):  return (mDF1[5]*mDF2[5]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M030):  return (mDF1[6]*mDF2[6]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M045):  return (mDF1[7]*mDF2[7]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M060):  return (mDF1[8]*mDF2[8]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M075):  return (mDF1[9]*mDF2[9]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M090):  return (mDF1[10]*mDF2[10]*mPartonALL) / (mF1[1]*mF2[1]);
    case(M105):  return (mDF1[11]*mDF2[11]*mPartonALL) / (mF1[1]*mF2[1]);
    case(P030):  return (mDF1[12]*mDF2[12]*mPartonALL) / (mF1[1]*mF2[1]);
    case(P045):  return (mDF1[13]*mDF2[13]*mPartonALL) / (mF1[1]*mF2[1]);
    case(P060):  return (mDF1[14]*mDF2[14]*mPartonALL) / (mF1[1]*mF2[1]);
    case(P070):  return (mDF1[15]*mDF2[15]*mPartonALL) / (mF1[1]*mF2[1]);
    case(GS_NLOA):  return (mDF1[16]*mDF2[16]*mPartonALL) / (mF1[1]*mF2[1]);
    case(GS_NLOB):  return (mDF1[17]*mDF2[17]*mPartonALL) / (mF1[1]*mF2[1]);
    case(GS_NLOC):  return (mDF1[18]*mDF2[18]*mPartonALL) / (mF1[1]*mF2[1]);
    case(DSSV):  return (mDF1[19]*mDF2[19]*mPartonALL) / (mF1[1]*mF2[1]);
    case(LSS1):  return (mDF1[20]*mDF2[20]*mPartonALL) / (mF1[1]*mF2[1]);
    case(LSS2):  return (mDF1[21]*mDF2[21]*mPartonALL) / (mF1[1]*mF2[1]);
    case(LSS3):  return (mDF1[22]*mDF2[22]*mPartonALL) / (mF1[1]*mF2[1]);
    case(AAC1):  return (mDF1[23]*mDF2[23]*mPartonALL) / (mF1[1]*mF2[1]);
    case(AAC2):  return (mDF1[24]*mDF2[24]*mPartonALL) / (mF1[1]*mF2[1]);
    case(AAC3):  return (mDF1[25]*mDF2[25]*mPartonALL) / (mF1[1]*mF2[1]);
    case(BB1):  return (mDF1[26]*mDF2[26]*mPartonALL) / (mF1[1]*mF2[1]);
    case(BB2):  return (mDF1[27]*mDF2[27]*mPartonALL) / (mF1[1]*mF2[1]);
    case(DNS1):  return (mDF1[28]*mDF2[28]*mPartonALL) / (mF1[1]*mF2[1]);
    case(DNS2):  return (mDF1[29]*mDF2[29]*mPartonALL) / (mF1[1]*mF2[1]);
    case(DSSV2009a): return (mDF1[DSSV2009a]/mF1[1])*(mDF2[DSSV2009a]/mF2[1])*mPartonALL;
    default:    return -999;
    }
}

inline const TClonesArray* StPythiaEvent::particles() const { return mParticles; }
inline int StPythiaEvent::numberOfParticles() const { return mParticles->GetEntriesFast(); }
inline const TParticle* StPythiaEvent::particle(int i) const { return (TParticle*)mParticles->At(i); }

inline void StPythiaEvent::addParticle(const TParticle& particle)
{
  new ((*mParticles)[mParticles->GetEntriesFast()]) TParticle(particle);
}

inline void StPythiaEvent::Clear(Option_t* option)
{
    mRunId = 0;
    mEventId = 0;
    mProcessId = 0;
    mTune = 0;
    mVertex.SetXYZ(0, 0, 0);
    mS = 0;
    mT = 0;
    mU = 0;
    mPt = 0;
    mCosTheta = 0;
    mX1 = 0;
    mX2 = 0;
    mPartonALL = 0;
    for (int ii=0; ii<30; ii++) {
      mDF1[ii] = 0;
      mDF2[ii] = 0;
    }
    mF1[0] = 0; mF1[1] = 0;
    mF2[0] = 0; mF2[1] = 0;

    if (mParticles) mParticles->Clear(option);
}

inline void StPythiaEvent::setRunId(int id) { mRunId = id; }
inline void StPythiaEvent::setEventId(int id) { mEventId = id; }
inline void StPythiaEvent::setProcessId(int id) { mProcessId = id; }
inline void StPythiaEvent::setTune(int tune) { mTune = tune; }
inline void StPythiaEvent::setVertex(const TVector3& v) { mVertex = v; }
inline void StPythiaEvent::setS(float s) { mS = s; }
inline void StPythiaEvent::setT(float t) { mT = t; }
inline void StPythiaEvent::setU(float u) { mU = u; }
inline void StPythiaEvent::setPt(float pt) { mPt = pt; }
inline void StPythiaEvent::setCosTheta(float cosTheta) { mCosTheta = cosTheta; }
inline void StPythiaEvent::setX1(float x1) { mX1 = x1; }
inline void StPythiaEvent::setX2(float x2) { mX2 = x2; }
inline void StPythiaEvent::setPartonALL(float a) { mPartonALL = a; }
inline void StPythiaEvent::setDF1(PDF scenario, float val) { mDF1[scenario] = val; }
inline void StPythiaEvent::setDF2(PDF scenario, float val) { mDF2[scenario] = val; }

inline void StPythiaEvent::setF1(PDF scenario, float val) 
{
    if(scenario == LO) mF1[0] = val;
    mF1[1] = val;
}

inline void StPythiaEvent::setF2(PDF scenario, float val) 
{
    if(scenario == LO) mF2[0] = val;
    mF2[1] = val;
}

#endif
