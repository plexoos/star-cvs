/***************************************************************************
 *
 * $Id: StTrackFitTraits.cxx,v 2.7 2001/03/24 03:35:00 perev Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackFitTraits.cxx,v $
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

static const char rcsid[] = "$Id: StTrackFitTraits.cxx,v 2.7 2001/03/24 03:35:00 perev Exp $";

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
    mCovariantMatrix.Set(15, (float*)t.covar);	//tempHackVP
}

StTrackFitTraits::StTrackFitTraits(UShort_t pid, UShort_t nfp,
                 Float_t chi[2], Float_t cov[15])
{
    mPidHypothesis = pid;
    mNumberOfFitPoints = nfp;
    copy(chi, chi+2, mChi2);
    mCovariantMatrix.Set(15, cov);
}

StTrackFitTraits::~StTrackFitTraits() {/* noop */}

UShort_t
StTrackFitTraits::numberOfFitPoints() const
{
    return (numberOfFitPoints(kTpcId) +
	    numberOfFitPoints(kSvtId) +
	    numberOfFitPoints(kSsdId));
}

UShort_t
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

Double_t
StTrackFitTraits::chi2(UInt_t i) const
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
#define mCovariantMatrix ((TArrayF&)mCovariantMatrix) 	//temporary HACK VP
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
#undef mCovariantMatrix 				//temporary HACK VP
    }
    return m;
}

void
StTrackFitTraits::clearCovariantMatrix() {mCovariantMatrix.Set(0);}

//______________________________________________________________________________
void StTrackFitTraits::Streamer(TBuffer &R__b)
{
//	Stream an object of class StTrackFitTraits.

Version_t R__f = R__b.GetVersion();
TBEvol R__e(Class(),&R__b);
  Version_t R__v = 0;
  if (R__b.IsReading()) {
    R__v = R__b.ReadVersion();
    {  R__e.MemberBegin();
       StObject::Streamer(R__b);
       R__e.MemberEnd();} 

    R__b >> (unsigned short&)mPidHypothesis;
    R__b >> (unsigned short&)mNumberOfFitPoints;
    R__b.ReadStaticArray((float*)mChi2);
    
    if (R__v > 1)
      {R__e.MemberBegin();
       mCovariantMatrix.Streamer(R__b);
       R__e.MemberEnd();
    } else {
      Float_t tmp[15];
      R__b.ReadStaticArray(tmp);
      mCovariantMatrix.Set(15,tmp);
    }

  } else {
    R__b.WriteVersion(Class());
      {R__e.MemberBegin();
       StObject::Streamer(R__b);
       R__e.MemberEnd();} 
    R__b << (unsigned short )mPidHypothesis;
    R__b << (unsigned short )mNumberOfFitPoints;
    int R__i=0;if(R__i){};
 R__b.WriteArray((float*)mChi2, 2);
      {R__e.MemberBegin();
      mCovariantMatrix.Streamer(R__b);
      R__e.MemberEnd();}
   }
}


