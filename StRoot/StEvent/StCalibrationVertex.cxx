/***************************************************************************
 *
 * $Id: StCalibrationVertex.cxx,v 2.3 2004/11/04 15:43:22 ullrich Exp $
 *
 * Author: Thomas Ullrich, Nov 2001
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StCalibrationVertex.cxx,v $
 * Revision 2.3  2004/11/04 15:43:22  ullrich
 * Added set funyion for type.
 *
 * Revision 2.2  2004/07/15 16:36:23  ullrich
 * Removed all clone() declerations and definitions. Use StObject::clone() only.
 *
 * Revision 2.1  2001/11/10 23:52:13  ullrich
 * Initial Revision.
 *
 **************************************************************************/
#include "StCalibrationVertex.h"
#include "tables/St_dst_vertex_Table.h"
#include "StTrack.h"
#if !defined(ST_NO_NAMESPACES)
using std::copy;
#endif

ClassImp(StCalibrationVertex)

static const char rcsid[] = "$Id: StCalibrationVertex.cxx,v 2.3 2004/11/04 15:43:22 ullrich Exp $";

StCalibrationVertex::StCalibrationVertex()
{
    mType = kOtherVtxId;
}

StCalibrationVertex::StCalibrationVertex(const dst_vertex_st& v)
    : StVertex(v) { mType = kOtherVtxId; }

StCalibrationVertex::~StCalibrationVertex() {/* noop */};

StVertexId
StCalibrationVertex::type() const { return mType; }

unsigned int
StCalibrationVertex::numberOfDaughters() const { return 0; }

StTrack*
StCalibrationVertex::daughter(unsigned int) { return 0; }

const StTrack*
StCalibrationVertex::daughter(unsigned int) const { return 0; }

StPtrVecTrack
StCalibrationVertex::daughters(StTrackFilter&)
{
    StPtrVecTrack vec;
    return vec;
}

void
StCalibrationVertex::addDaughter(StTrack*) { /* noop */ }

void
StCalibrationVertex::removeDaughter(StTrack*) { /* noop */ }

void
StCalibrationVertex::setType(StVertexId val) { mType = val; }
