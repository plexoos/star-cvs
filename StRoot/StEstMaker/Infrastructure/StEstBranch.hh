/***************************************************************************
 *
 * $Id: StEstBranch.hh,v 1.3 2001/01/26 10:16:44 lmartin Exp $
 *
 * Author: PL,AM,LM,CR (Warsaw,Nantes)
 ***************************************************************************
 *
 * Description: Header file of StEstBranch Class
 *
 ***************************************************************************
 *
 * $Log: StEstBranch.hh,v $
 * Revision 1.3  2001/01/26 10:16:44  lmartin
 * Minor changes. Unused mLastLay data member removed. Short description of the data members added.
 *
 * Revision 1.2  2001/01/25 18:13:27  lmartin
 * Minor changes in the GetHit and SetHelix methods
 *
 * Revision 1.1  2000/12/07 11:14:27  lmartin
 * First CVS commit
 *
 **************************************************************************/
#ifndef StEstBranch_hh
#define StEstBranch_hh

#include "StMaker.h"
class StHelix;
class StEstTrack;
class StEstHit;
class StEstTracker;

class StEstBranch {
  
protected:
  
  StEstTrack* mTrack; // pointer to the mother track
  StEstHit**  mHits; // list of pointers to the hits in the branch
  StHelix* mHelix; // pointer to the branch helix
  long int mNHits; // number of hits in the branch
  long int mMaxHits; //maximum number of hits in the branch 
  long int mNFit; // number of hits used in the fit
  long int mLastFitStatus; // Status flag return by the egr fitting routine
  double mChisq; // Total chisq
  double mChisqCir; // Circular chisq
  double mChisqLin; // Linear chisq
  double *mDist;  // table of distances between hits and branch projections
  int mDebugLevel; // Printing level for the methods
  int mStep; // Step (10*mSuperPass+mPass) when the branch is formed
  int mIsGood; // The branch matches the ideal branch pattern at a given step
  int mIsGoodOld; //The branch matches the ideal branch pattern at the previous step
  int mHitPosition; //Rank of the selected hit according to the hits-projection distance
  
public :
  StEstBranch(StEstTrack* tr=NULL, long int maxhits=1, long int nf=0, long int nh=0, StEstHit** hit=NULL, double *dist=0, int isgood=1);
  ~StEstBranch();
  int AddHit(StEstHit *hit, double dist);
  int RemoveHit(long int nr);
  int RemoveHit(StEstHit* hit);
  long int GetNHits();
  StEstHit* GetHit(long int nr);
  int CheckAvailability();
  StEstBranch* Duplicate();
  int JoinTrack(StEstTrack *tr, int IsIdeal);
  void LeaveTrack();
  StEstTrack* GetTrack();
  void SetHelix(StHelix *hel);
  StHelix* GetHelix();
  void SetChiSq(double chi);
  void SetChiSqCir(double chi);
  void SetChiSqLin(double chi);
  void SetNFit(long int nf);
  double GetChiSq();
  double GetChiSqCir();
  double GetChiSqLin();
  double GetDist(long nr);
  double GetDist(StEstHit* hit);
  long int GetNFit();
  void SetDebugLevel(int deb);
  void SetStep(int step);
  int GetStep();
  void SetIsGood(int isgood);
  int GetIsGood();
  void SetIsGoodOld(int isgood);
  int GetIsGoodOld();
  friend class StEstTracker;
};
 
inline int StEstBranch::CheckAvailability() {
  if (mNHits<mMaxHits) return 1;
  else return 0;
}

inline long int StEstBranch::GetNHits()             {return mNHits;};
inline StHelix* StEstBranch::GetHelix()             {return mHelix;};
inline void     StEstBranch::SetChiSq(double chi)   {mChisq = chi;};
inline void     StEstBranch::SetChiSqLin(double chi)   {mChisqLin = chi;};
inline void     StEstBranch::SetChiSqCir(double chi)   {mChisqCir = chi;};
inline double   StEstBranch::GetChiSq()             {return mChisq;};
inline double   StEstBranch::GetChiSqCir()             {return mChisqCir;};
inline double   StEstBranch::GetChiSqLin()             {return mChisqLin;};
inline double   StEstBranch::GetDist(long nr)          {return mDist[nr];};
inline void     StEstBranch::SetDebugLevel(int deb) {mDebugLevel = deb;};
inline void     StEstBranch::SetStep(int step) {mStep = step;};
inline int      StEstBranch::GetStep() {return mStep;};
inline void     StEstBranch::SetIsGood(int isgood) {mIsGood = isgood;};
inline int      StEstBranch::GetIsGood() {return mIsGood;};
inline void     StEstBranch::SetIsGoodOld(int isgood) {mIsGoodOld = isgood;};
inline int      StEstBranch::GetIsGoodOld() {return mIsGoodOld;};
inline StEstTrack* StEstBranch::GetTrack() {return mTrack;};
inline void     StEstBranch::SetNFit(long nf) {mNFit=nf;};
inline long int StEstBranch::GetNFit() {return mNFit;};



#endif
