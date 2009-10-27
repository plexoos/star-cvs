#ifndef STAR_StBTofHitMaker_H
#define STAR_StBTofHitMaker_H

/***************************************************************************
 *
 * $Id: StBTofHitMaker.h,v 1.7 2009/10/27 16:03:47 fine Exp $
 * StBTofHitMaker - class to fille the StEvent from DAQ reader
 *--------------------------------------------------------------------------
 *
 ***************************************************************************/
#include "StRTSBaseMaker.h"

#include <vector>
#ifndef ST_NO_NAMESPACES
using std::vector;
#endif

#define VHRBIN2PS 24.4140625  // Very High resolution mode, pico-second per bin
                              // 1000*25/1024 (ps/chn)
#define HRBIN2PS 97.65625     // High resolution mode, pico-second per bin
                              // 97.65625= 1000*100/1024  (ps/chn)

struct tof_t;

class StEvent;
class StBTofCollection;
class StBTofRawHit;
class StBTofRawHitCollection;
class StBTofHit;
class StBTofHitCollection;
#include "StBTofUtil/StBTofSortRawHit.h"
class StBTofINLCorr;
class StBTofDaqMap;

struct TofRawHit {
  unsigned int   tdc;               /// tdc time (in bin) per hit.
  unsigned int   dataword;          /// data word before unpack
  unsigned char  fiberid;           /// 0 1 2,3
  unsigned char  trayID;            /// 1,2,......,120,for tray, 121, 122 for upvpd
  unsigned char  globaltdcchan;     /// 0,1,......,191   
};

#if !defined(ST_NO_TEMPLATE_DEF_ARGS) || defined(__CINT__)
typedef vector<Int_t>  IntVec;
typedef vector<UInt_t>  UIntVec;
#else
typedef vector<Int_t, allocator<Int_t>>  IntVec;
typedef vector<UInt_t, allocator<UInt_t>>  UIntVec;
#endif


/**
   \class StBTofHitMaker
   
   Class to read in TOF data from DAQ and store into StBTofCollection   
*/
class StBTofHitMaker:public StRTSBaseMaker
{
   private: 
      StEvent *mStEvent;
      tof_t   *fTof;

      Int_t UnpackTofRawData();
      void fillBTofHeader();
      void fillBTofRawHitCollection();
      void fillBTofHitCollection();
      void fillStEvent();     //! ship collection to StEvent and check
      /// TOF Raw hits info. struct
      ///----------------------------------------------------
     vector<TofRawHit> TofLeadingHits;
     vector<TofRawHit> TofTrailingHits;

     Int_t                    mNValidTrays;          //! number of valid TOF trays
     unsigned int             mTriggerTimeStamp[4];  //! Trigger Time in 4 fibers
     StBTofCollection*        mBTofCollection;       //! pointer to StBTofCollection
     StBTofRawHitCollection*  mRawHitCollection;     //! pointer to StBTofRawHitCollection
     StBTofHitCollection*     mHitCollection;        //! pointer to StBTofHitCollection
     StBTofDaqMap*            mBTofDaqMap;           //! pointer to the TOF daq map
     StBTofINLCorr*           mBTofINLCorr;          //! INL corretion;
     StBTofSortRawHit*        mBTofSortRawHit;       //! to sort the TOF hits

     static const Int_t  mNVPD;          // = 19;
     static const Int_t  mWestVpdTrayId; // = 121;
     static const Int_t  mEastVpdTrayId; // = 122;
          
   protected:
      StRtsTable *GetNextRaw();
      
      StBTofCollection *GetBTofCollection();

   public:

     /// Default constructor
     StBTofHitMaker(const char *name="tof_raw");
     
    ~StBTofHitMaker() ;

     void   Clear(Option_t* option="");
     Int_t  Init();
     Int_t  InitRun(Int_t);
     Int_t  FinishRun(Int_t);
     Int_t  Finish();
     Int_t  Make();

     /// obtain the whole list of leading edge hits
     vector<TofRawHit> getLeadingHits();
     /// obtain the whole list of trainling edge hits
     vector<TofRawHit> getTrailingHits();
     
  /// cvs
  virtual const char *GetCVS() const
    {
      static const char cvs[]="Tag $Name:  $Id: built "__DATE__" "__TIME__ ; return cvs;
    }
  
  ClassDef(StBTofHitMaker, 1)    ///StBTofHitMaker - class to fille the StEvent from DAQ reader
};

inline vector<TofRawHit> StBTofHitMaker::getLeadingHits() { return TofLeadingHits; }
inline vector<TofRawHit> StBTofHitMaker::getTrailingHits() { return TofTrailingHits; }

#endif
