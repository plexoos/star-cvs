/***************************************************************************
 *
 * $Id: StTrackFitTraits.cxx,v 2.10 2001/05/04 19:49:51 perev Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackFitTraits.cxx,v $
 * Revision 2.10  2001/05/04 19:49:51  perev
 * Streamer to account old ROOT2
 *
 * Revision 2.9  2001/04/09 22:57:05  perev
 * forget STAR I/O
 *
 * Revision 2.8  2001/04/05 04:00:58  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.7  2001/03/24 03:35:00  perev
 * clone() -> clone() const
 *
 * Revision 2.6  2001/03/16 20:57:44  ullrich
 * Covariant matrix now stored in TArrayF.
 *
 * Revision 2.5  2000/02/22 23:24:08  ullrich
 * Fixed bug in covariantMatrix().
 *
 * Revision 2.4  2000/01/20 14:43:39  ullrich
 * Fixed bug in numberOfFitPoints(). Sum was wrong.
 *
 * Revision 2.3  1999/12/21 15:09:18  ullrich
 * Modified to cope with new compiler version on Sun (CC5.0).
 *
 * Revision 2.2  1999/11/01 12:45:14  ullrich
 * Modified unpacking of point counter
 *
 * Revision 2.1  1999/10/28 22:27:32  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:59  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "TFile.h"
#include <algorithm>
#include "StTrackFitTraits.h"
#include "StParticleTypes.hh"
#include "StParticleTable.hh"
#include "tables/St_dst_track_Table.h"
#if !defined(ST_NO_NAMESPACES)
using std::fill_n;
using std::copy;
#endif

ClassImp(StTrackFitTraits)

static const char rcsid[] = "$Id: StTrackFitTraits.cxx,v 2.10 2001/05/04 19:49:51 perev Exp $";

StTrackFitTraits::StTrackFitTraits()
{
    mPidHypothesis = 0;
    mNumberOfFitPoints = 0;
    fill_n(mChi2, 2, 0);
}

StTrackFitTraits::StTrackFitTraits(const dst_track_st& t)
{
    mPidHypothesis = t.pid;
    mNumberOfFitPoints = t.n_fit_point;
    copy(t.chisq+0, t.chisq+2, mChi2);
    mCovariantMatrix.Set(15, (float*)t.covar);        //tempHackVP
}

StTrackFitTraits::StTrackFitTraits(unsigned short pid, unsigned short nfp,
                 float chi[2], float cov[15])
{
    mPidHypothesis = pid;
    mNumberOfFitPoints = nfp;
    copy(chi, chi+2, mChi2);
    mCovariantMatrix.Set(15, cov);
}

StTrackFitTraits::~StTrackFitTraits() {/* noop */}

unsigned short
StTrackFitTraits::numberOfFitPoints() const
{
    return (numberOfFitPoints(kTpcId) +
            numberOfFitPoints(kSvtId) +
            numberOfFitPoints(kSsdId));
}

unsigned short
StTrackFitTraits::numberOfFitPoints(StDetectorId det) const
{
    // 1*tpc + 1000*svt + 10000*ssd (Helen/Spiros Oct 29, 1999)
    switch (det) {
    case kFtpcWestId:
    case kFtpcEastId:
    case kTpcId:
        return mNumberOfFitPoints%1000;
        break;
    case kSvtId:
        return (mNumberOfFitPoints%10000)/1000;
        break;
    case kSsdId:
        return mNumberOfFitPoints/10000;
        break;
    default:
        return 0;
    }
}

StParticleDefinition*
StTrackFitTraits::pidHypothesis() const
{
    return StParticleTable::instance()->findParticleByGeantId(mPidHypothesis);
}

double
StTrackFitTraits::chi2(unsigned int i) const
{
    if (i < 2)
        return mChi2[i];
    else
        return 0;
}

StMatrixF
StTrackFitTraits::covariantMatrix() const
{
    StMatrixF m(5,5);
    if (mCovariantMatrix.GetSize() == 15) {
#define mCovariantMatrix ((TArrayF&)mCovariantMatrix)         //temporary HACK VP
        m(1,1) = mCovariantMatrix[0];
        m(1,2) = m(2,1) = mCovariantMatrix[1];
        m(1,3) = m(3,1) = mCovariantMatrix[2];
        m(1,4) = m(4,1) = mCovariantMatrix[3];
        m(1,5) = m(5,1) = mCovariantMatrix[4];
        m(2,2) = mCovariantMatrix[5];
        m(2,3) = m(3,2) = mCovariantMatrix[6];
        m(2,4) = m(4,2) = mCovariantMatrix[7];
        m(2,5) = m(5,2) = mCovariantMatrix[8];
        m(3,3) = mCovariantMatrix[9];
        m(3,4) = m(4,3) = mCovariantMatrix[10];
        m(3,5) = m(5,3) = mCovariantMatrix[11];
        m(4,4) = mCovariantMatrix[12];
        m(4,5) = m(5,4) = mCovariantMatrix[13];
        m(5,5) = mCovariantMatrix[14];
#undef mCovariantMatrix                                 //temporary HACK VP
    }
    return m;
}

void
StTrackFitTraits::clearCovariantMatrix() {mCovariantMatrix.Set(0);}

//______________________________________________________________________________
void StTrackFitTraits::Streamer(TBuffer &R__b)
{
//        Stream an object of class StTrackFitTraits.

  Version_t R__v = 0;
  if (R__b.IsReading()) {
    R__v = R__b.ReadVersion();
    StObject::Streamer(R__b);

    R__b >> (unsigned short&)mPidHypothesis;
    R__b >> (unsigned short&)mNumberOfFitPoints;

    if (R__v==2 && gFile && gFile->GetVersion()%100000<30000)
       { Int_t dumy; R__b >> dumy;}

    R__b.ReadFastArray(mChi2,2);
    mCovariantMatrix.Streamer(R__b);

  } else {
    R__b.WriteVersion(Class());
    StObject::Streamer(R__b);
    R__b << (unsigned short )mPidHypothesis;
    R__b << (unsigned short )mNumberOfFitPoints;
    R__b.WriteFastArray(mChi2, 2);
    mCovariantMatrix.Streamer(R__b);
  }
}


