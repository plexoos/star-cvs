/***************************************************************************
 *
 * $Id: StVertex.cxx,v 2.9 2003/09/02 17:58:06 perev Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVertex.cxx,v $
 * Revision 2.9  2003/09/02 17:58:06  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.8  2001/05/30 17:45:55  perev
 * StEvent branching
 *
 * Revision 2.7  2001/04/05 04:00:59  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.6  2000/03/08 14:29:54  ullrich
 * New method probChiSquared() added.
 *
 * Revision 2.5  2000/02/10 16:32:19  ullrich
 * flag changed from unsigned to signed long
 *
 * Revision 2.4  2000/01/11 19:22:12  ullrich
 * Added non-const parent() method.
 *
 * Revision 2.3  1999/12/21 15:09:23  ullrich
 * Modified to cope with new compiler version on Sun (CC5.0).
 *
 * Revision 2.2  1999/11/22 15:04:43  ullrich
 * Forgot to fill mPosition in constructor. Fixed now.
 *
 * Revision 2.1  1999/10/28 22:28:07  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:29  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include <algorithm>
#include "TClass.h"
#include "TFile.h"
#include "StVertex.h"
#include "tables/St_dst_vertex_Table.h"
#include "StTrack.h"
#if !defined(ST_NO_NAMESPACES)
using std::fill_n;
using std::copy;
#endif

ClassImp(StVertex)

static const char rcsid[] = "$Id: StVertex.cxx,v 2.9 2003/09/02 17:58:06 perev Exp $";

StVertex::StVertex()
{
    mType = kUndefinedVtxId;
    mFlag = 0;
    mChiSquared     = 0;
    mProbChiSquared = 0;
    mParent = 0;
    fill_n(mCovariantMatrix, 6, 0);
}

StVertex::StVertex(const dst_vertex_st& v)
{
    mType = kUndefinedVtxId;
    mFlag = v.iflag;
    copy(v.covar+0, v.covar+6, mCovariantMatrix);
    mChiSquared     = v.chisq[0];
    mProbChiSquared = v.chisq[1];
    mParent = 0;
    mPosition.setX(v.x);
    mPosition.setY(v.y);
    mPosition.setZ(v.z);
}

StVertex::~StVertex() {/* noop */};

int
StVertex::operator==(const StVertex& v) const
{
    return mType == v.mType &&
        mFlag == v.mFlag &&
        mPosition == v.mPosition &&
        mChiSquared == v.mChiSquared;
}

int
StVertex::operator!=(const StVertex& v) const
{
    return !(v == *this);
}

int
StVertex::flag() const { return mFlag; }

float
StVertex::chiSquared() const { return mChiSquared; }

float
StVertex::probChiSquared() const { return mProbChiSquared; }

StMatrixF
StVertex::covariantMatrix() const
{
    StMatrixF m(3,3);
    m(1,1) = mCovariantMatrix[0];
    m(1,2) = m(2,1) = mCovariantMatrix[1];
    m(2,2) = mCovariantMatrix[2];
    m(1,3) = m(3,1) = mCovariantMatrix[3];
    m(2,3) = m(3,2) = mCovariantMatrix[4];
    m(3,3) = mCovariantMatrix[5];
    return m;
}
    
StThreeVectorF
StVertex::positionError() const
{
    return StThreeVectorF(::sqrt(mCovariantMatrix[0]), ::sqrt(mCovariantMatrix[2]), ::sqrt(mCovariantMatrix[5]));
}

StTrack*
StVertex::parent() { return mParent; }

const StTrack*
StVertex::parent() const { return mParent; }

void
StVertex::setFlag(int val) { mFlag = val; }

void
StVertex::setCovariantMatrix(float val[6]) { copy(val, val+6, mCovariantMatrix); }

void
StVertex::setChiSquared(float val) { mChiSquared = val; }

void
StVertex::setProbChiSquared(float val) { mProbChiSquared = val; }

void
StVertex::setParent(StTrack* val) { mParent = val; }

void StVertex::Streamer(TBuffer &R__b)
{
    // Stream an object of class .

    if (R__b.IsReading()) {
       UInt_t R__s, R__c;
       Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
       if (R__v > 1) {
          Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
          return;
       }
       //====process old versions before automatic schema evolution
       StMeasuredPoint::Streamer(R__b);
       R__b >> (Int_t&)mType;
       R__b >> mFlag;
       Int_t dumy;
       if (gFile && gFile->GetVersion() < 30000) {R__b >> dumy;}
       R__b.ReadFastArray(mCovariantMatrix,6);
       R__b >> mChiSquared;
       R__b >> mProbChiSquared;
//     R__b >> mParent;
       R__b >> (StTrack*&)mParent;

       R__b.CheckByteCount(R__s, R__c, Class());
       //====end of old versions
      
    } else {
       Class()->WriteBuffer(R__b,this);
    }
} 



