/***************************************************************************
 *
 * $Id: StuProbabilityPidAlgorithm.h,v 1.11 2000/12/20 16:55:16 aihong Exp $
 *
 * Author:Aihong Tang, Richard Witt(FORTRAN version). Kent State University
 *        Send questions to aihong@cnr.physics.kent.edu 
 ***************************************************************************
 *
 * Description: A functor that do PID base on Probability (Amplitude) info.
 *              
 ***************************************************************************
 *
 * $Log: StuProbabilityPidAlgorithm.h,v $
 * Revision 1.11  2000/12/20 16:55:16  aihong
 * let it survive when no support PIDTable is present
 *
 * Revision 1.10  2000/12/18 23:22:58  aihong
 * big revision. eta bin and centrality bin added
 *
 * Revision 1.6  2000/08/16 12:46:07  aihong
 * bug killed
 *
 * Revision 1.5  2000/08/15 23:04:18  aihong
 * speed it up by looking up table
 *
 * Revision 1.4  2000/07/22 22:45:27  aihong
 * change include path
 *
 * Revision 1.3  2000/05/05 19:25:39  aihong
 * modified ctor
 *
 * Revision 1.2  2000/03/09 20:44:56  aihong
 * add head for Log
 *
 **************************************************************************/
#ifndef StuProbabilityPidAlgorithm_hh
#define StuProbabilityPidAlgorithm_hh


#include "TString.h"
#include "TVectorD.h"


#include "StEventTypes.h"
#include "StParticleTable.hh"
#include "StParticleTypes.hh"

class StEvent;
class TString;
class TVectorD;
class TF1;


class StuProbabilityPidAlgorithm : public StPidAlgorithm {

 public:

      StuProbabilityPidAlgorithm(StEvent& ev);
      virtual  ~StuProbabilityPidAlgorithm();



      StParticleDefinition* mostLikelihoodParticle();
      StParticleDefinition* secondLikelihoodParticle();     
      StParticleDefinition* thirdLikelihoodParticle();

      StParticleDefinition* getParticle(int i);


      int mostLikelihoodParticleGeantID()    const;
      int secondLikelihoodParticleGeantID()  const;     
      int thirdLikelihoodParticleGeantID()   const;
      int getParticleGeantID(int i)          const;

      double getProbability(int i);
      double mostLikelihoodProbability();
      double secondLikelihoodProbability();
      double thirdLikelihoodProbability();
  
      double beingPionMinusProb()  const { return  mPionMinusProb;}
      double beingElectronProb()   const { return  mElectronProb; }
      double beingKaonMinusProb()  const { return  mKaonMinusProb;}
      double beingAntiProtonProb() const { return  mAntiProtonProb;}
     
      double beingPionPlusProb()   const { return  mPionPlusProb; }
      double beingPositronProb()   const { return  mPositronProb; }
      double beingKaonPlusProb()   const { return  mKaonPlusProb; }
      double beingProtonProb()     const { return  mProtonProb; }

      bool isExtrap();

      StParticleDefinition*
      operator() (const StTrack&, const StSPtrVecTrackPidTraits&);
      
      static void setDedxMethod(StDedxMethod method);

      static void readParametersFromFile(TString fileName);
      //      static void readParametersFromTable(St_Table* tb);
                  //from data base.

 private:
     
   void fillPIDByLookUpTable(double myCentrality, double myDca, int myCharge, double myRig, double myEta, int myNhits, double myDedx);

     void    fillPIDHypothis();
 


      void   fill(double prob, int geantId);
      void   fillAsUnknown();
      void   lowRigPID(double rig,double dedx,int theCharge);
      double getCentrality(int theMult);
      int    getCentralityBin(double theCent);
      int    getCalibPosition(double theEta, int theNHits);
      void   setCalibrations(double theEta, int theNhits);

      StParticleTable* table;

      int      PID[4];
      double   mProb[4];

      bool     mExtrap;

      StEvent* mEvent; 

    double          mPionMinusProb;
    double          mElectronProb;
    double          mKaonMinusProb;
    double          mAntiProtonProb;
    double          mPionPlusProb;
    double          mPositronProb;
    double          mKaonPlusProb;
    double          mProtonProb;

    /////////////


   static int thisMultBins;
   static int thisDcaBins;
   static int thisChargeBins;


   static int thisPBins;
   static int thisEtaBins;
   static int thisNHitsBins;

////////////////////////

   static double  thisDedxStart;
   static double  thisDedxEnd;
   static double  thisPStart;
   static double  thisPEnd;
   static double  thisEtaStart;
   static double  thisEtaEnd;
   static double  thisNHitsStart;
   static double  thisNHitsEnd;

  static  bool    mPIDTableRead;

  static  StDedxMethod mDedxMethod;

  static TVectorD*     mEAmp;
  static TVectorD*     mECenter;
  static TVectorD*     mESigma;

  static TVectorD*     mPiAmp;
  static TVectorD*     mPiCenter;
  static TVectorD*     mPiSigma;

  static TVectorD*     mKAmp;
  static TVectorD*     mKCenter;
  static TVectorD*     mKSigma;

  static TVectorD*     mPAmp;
  static TVectorD*     mPCenter;
  static TVectorD*     mPSigma;


  static TVectorD*     mEqualyDividableRangeStartSet;
  static TVectorD*     mEqualyDividableRangeEndSet;
  static TVectorD*     mEqualyDividableRangeNBinsSet;
  static TVectorD*     mNoEqualyDividableRangeNBinsSet;

  static TVectorD*     mMultiBinEdgeSet;  // % central
  static TVectorD*     mDcaBinEdgeSet;    // in centimeter

  static TVectorD*     mBBScale; //BetheBloch scale par.
  static TVectorD*     mBBOffSet; //BetheBolch offset par.

  TF1*        myBandBGFcn;

};

inline   int StuProbabilityPidAlgorithm::mostLikelihoodParticleGeantID()    const { return PID[0];}
inline   int StuProbabilityPidAlgorithm::secondLikelihoodParticleGeantID()  const { return PID[1];}     
inline   int StuProbabilityPidAlgorithm::thirdLikelihoodParticleGeantID()   const { return PID[2];}  
inline   int StuProbabilityPidAlgorithm::getParticleGeantID(int i)          const {
          if (i<4 && i>=0) return PID[i]; else return -1;}

#endif

