/***************************************************************************
 *
 * $Id: StRchMaker.h,v 2.4 2003/09/10 19:47:29 perev Exp $
 *
 * Author: 
 ***************************************************************************
 *
 * Description: RICH offline software
 *              StRchMaker.h - ROOT/STAR Maker for offline chain.
 ***************************************************************************
 * $Log: StRchMaker.h,v $
 * Revision 2.4  2003/09/10 19:47:29  perev
 * ansi corrs
 *
 * Revision 2.3  2002/02/22 21:46:16  dunlop
 * Remove picket fence pixels once and for all
 *
 * Revision 2.2  2001/02/07 16:06:39  lasiuk
 * adc decoder modified for 11bit check
 * this-> for internal calls
 * hit ntuple extended to include pads in hit
 *
 * Revision 2.1  2000/09/29 18:59:33  lasiuk
 * addition of software monitor
 * write flags in persistent hit (reservedLong)
 * Histodefintions
 *
 * Revision 2.0  2000/08/09 16:22:11  gans
 * Cosmetic Changes. Naming convention for TDrawable objects
 *
 * Revision 1.13  2000/06/13 18:26:13  dunlop
 * Modified SetMode (public)
 *
 * Revision 1.12  2000/05/25 21:35:32  fisyak
 * Make rootcint happy
 *
 * Revision 1.11  2000/05/23 16:49:55  lasiuk
 * writing to StEvent/StRichCollection
 *
 * Revision 1.10  2000/05/18 21:57:19  lasiuk
 * dev patch
 *
 * Revision 1.9  2000/05/01 20:22:50  dunlop
 * Added in SetMode
 *
 * Revision 1.8  2000/04/05 21:24:28  lasiuk
 * with cf
 *
 * Revision 1.7  2000/02/14 20:50:29  lasiuk
 * use DAQ/sim interface with a switch settable at the c'tor
 *
 * Revision 1.6  2000/01/11 21:18:04  lasiuk
 * Fills new dst_rch_pixel;
 * debug macros;
 * used in first DAQ data
 *
 **************************************************************************/
#ifdef __ROOT__
#ifndef STAR_StRchMaker
#define STAR_StRchMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif

#define rCH_DEBUG 1
#define rCH_HISTOGRAM 1

#include "StRichDisplayActivate.h"

#ifndef __CINT__
#include <vector>
#ifndef ST_NO_NAMESPACES
using std::vector;
#endif
#endif

#ifdef RCH_HISTOGRAM
#include "TFile.h"
#include "TH1.h"
#include "TNtuple.h"
#endif
#ifndef __CINT__
#include "StRrsMaker/StRichSinglePixel.h"
#include "StRrsMaker/StRichSingleMCPixel.h"
#endif
// forward StEvent declaration
class StEvent;
class StRichCollection;

class StDAQReader;
class StRichReaderInterface;
class StRichGeometryDb;
class StRichClusterAndHitFinder;
class StRichHit;
class StRichSimpleHit;
class StRichSimpleHitCollection;
class StRichSinglePixel;


class StRchMaker : public StMaker {
    
private:
    Bool_t drawinit;
    
protected:
    
public: 
    StRchMaker(const char *name="rch", int daq=0, int matrix=0, int cf=0);
    virtual       ~StRchMaker();
    virtual Int_t  Init();
    virtual Int_t  Make();
    virtual void   PrintInfo();
    virtual Int_t  Finish();

    void setUseMatrix(int v);
    int  adcDecoder(unsigned long code, unsigned long* pad, unsigned long* row, unsigned long* adc);
    void setPedestalSubtract(int v,const char *file);
    void clearPadMonitor();

    void setRemovePicketFencePixels(int);
    
protected:
    void fillStEvent();
    
private:
    StDAQReader*           mTheDataReader;//!
    StRichReaderInterface* mTheRichReader;//!
    St_DataSet*            mTheRichData;//!

    StRichGeometryDb*      mGeometryDb;//!
    
    int mDaq;  // looking for DAQ data or not?
    int mPads;  // number of pads
    int mRows;
    int mNumberOfPads;
    int mEventNumber;
    int mUseMatrix;

    // flags
    short mRichCollectionPresent ;
    short mPixelCollectionPresent;
    short mClusterCollectionPresent;
    short mHitCollectionPresent;
    short mCfOnly                ;
    //pedestal
    short mPedestalSubtract;
    float mPedestal[160][96];
    float mSigma[160][96];
    const char* mPedestalFile;
    int mRemovePicketFencePixels;
    
#ifndef __CINT__    
    vector<StRichSinglePixel*> mPixelStore;//!
    typedef vector<StRichSinglePixel*>* pointerToPixelStoreType;
    
// Create a vector of picket fence pixels
    vector<StRichSinglePixel*> mPicketFencePixelStore;//!
    StRichClusterAndHitFinder*  mClusterFinder;//!
    // this is used only to pass the hits via the DataSet
    // We will access them via the richcollection
    StRichSimpleHitCollection*  mSimpleHitCollection;//!
    vector<StRichSimpleHit*>    mTheHits;//!
#endif    
    // From StEvent
    StEvent*                    mEvent;//!
    StRichCollection*           mTheRichCollection;//!

#ifdef RCH_HISTOGRAM
    TFile* mRchNTupleFile; //!
    TNtuple* mPadPlane;   //!
    TNtuple* mClusters;//!
    TNtuple* mHits;//!
    float mRawData[4];    //!
    float mCluster[6];//!
    float mHit[11];//!
    
    TH1F* mcc;//!
    TH1F* mmc;//!
    TH1F* mrms;//!
    TH1F* mpad;//!
    TH1F* mqpad;//!
    TH1F* mcratio;//!
    // hits
    TH1F* mhc;//!
    TH1F* mhmc;//!
    TH1F* mhc2m;//!
#endif
virtual const char *GetCVS() const	{
    static const char cvs[]=
	"Tag $Name:  $ $Id: StRchMaker.h,v 2.4 2003/09/10 19:47:29 perev Exp $ built "__DATE__" "__TIME__ ;
    return cvs;
}
public:
    virtual void SetMode(Int_t mode=0) {
	m_Mode = mode;
	mDaq = 1-mode; // mDaq has opposite behavior from corresponding
	// variable in St_tpcdaq_Maker, so reverse it. 
    }
private:
    // the following is a ROOT macro  that is needed in all ROOT code
    ClassDef(StRchMaker,0)   //StAF chain virtual base class for Makers
	};

inline void StRchMaker::setUseMatrix(int v) {mUseMatrix = v;}
inline void StRchMaker::setRemovePicketFencePixels(int v) {mRemovePicketFencePixels = v;}

#endif 
#endif /* __ROOT__ */
