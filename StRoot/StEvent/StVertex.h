/***************************************************************************
 *
 * $Id: StVertex.h,v 2.5 2000/03/08 14:29:56 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVertex.h,v $
 * Revision 2.5  2000/03/08 14:29:56  ullrich
 * New method probChiSquared() added.
 *
 * Revision 2.4  2000/02/10 18:49:08  ullrich
 * Fixed typo introduced at last check-in.
 *
 * Revision 2.3  2000/02/10 16:32:21  ullrich
 * flag changed from unsigned to signed long
 *
 * Revision 2.2  2000/01/11 19:22:14  ullrich
 * Added non-const parent() method.
 *
 * Revision 2.1  1999/10/28 22:28:09  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:32  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#ifndef StVertex_hh
#define StVertex_hh
#include "StMeasuredPoint.h"
#include "StEnumerations.h"
#include "StMatrixF.hh"
#include "StContainers.h"

class dst_vertex_st;
class StTrack;
class StTrackFilter;

class StVertex : public StMeasuredPoint {
public:
    StVertex();
    StVertex(const dst_vertex_st&);
    // StVertex(const StVertex&);            use default
    // StVertex& operator=(const StVertex&); use default
    virtual ~StVertex();
    
    Int_t operator==(const StVertex&) const;
    Int_t operator!=(const StVertex&) const;

    virtual StVertexId     type() const = 0;
    Long_t                 flag() const;
    Float_t                chiSquared() const;
    Float_t                probChiSquared() const;
    StMatrixF              covariantMatrix() const;  // overwrite inherited
    StThreeVectorF         positionError() const;    // overwrite inherited
    StTrack*               parent();
    const StTrack*         parent() const;
    virtual UInt_t         numberOfDaughters() const = 0;
    virtual StTrack*       daughter(UInt_t) = 0;
    virtual const StTrack* daughter(UInt_t) const = 0;
    virtual StPtrVecTrack  daughters(StTrackFilter&) = 0;

    virtual void setFlag(Long_t);
    virtual void setCovariantMatrix(Float_t[6]);
    virtual void setChiSquared(Float_t);
    virtual void setProbChiSquared(Float_t);
    virtual void setParent(StTrack*);
    virtual void addDaughter(StTrack*) = 0;
    virtual void removeDaughter(StTrack*) = 0;

protected:
    StVertexId    mType;
    Long_t        mFlag;
    Float_t       mCovariantMatrix[6];
    Float_t       mChiSquared;
    Float_t       mProbChiSquared;
    StTrack*      mParent;             //$LINK

    ClassDef(StVertex,1)
};
#endif
