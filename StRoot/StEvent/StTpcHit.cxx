/***************************************************************************
 *
 * $Id: StTpcHit.cxx,v 2.7 2000/07/28 23:29:42 calderon Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTpcHit.cxx,v $
 * Revision 2.7  2000/07/28 23:29:42  calderon
 * Added handling of Fit Flag: use this flag to tell if the point
 * is used in the fit.
 *
 * Revision 2.6  2000/06/01 21:39:07  ullrich
 * Added member mFlag and access member flag() and setFlag().
 *
 * Revision 2.5  1999/12/01 15:56:28  ullrich
 * Renamed xxxInCluster() methods to xxxInHit()
 *
 * Revision 2.4  1999/11/11 10:19:52  ullrich
 * Inlined sector() and padrow().
 *
 * Revision 2.3  1999/11/09 19:35:25  ullrich
 * Memory now allocated using StMemoryPool via overloaded new/delete
 *
 * Revision 2.2  1999/11/04 21:41:00  ullrich
 * Added missing default constructor
 *
 * Revision 2.1  1999/10/28 22:27:07  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:48  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "StTpcHit.h"
#include "StTrack.h"
#include "tables/St_dst_point_Table.h"

static const char rcsid[] = "$Id: StTpcHit.cxx,v 2.7 2000/07/28 23:29:42 calderon Exp $";

StMemoryPool StTpcHit::mPool(sizeof(StTpcHit));

ClassImp(StTpcHit)

StTpcHit::StTpcHit() { /* noop */ }

StTpcHit::StTpcHit(const StThreeVectorF& p,
                   const StThreeVectorF& e,
                   ULong_t hw, Float_t q, UChar_t c)
    : StHit(p, e, hw, q, c)
{ /* noop */ }

StTpcHit::StTpcHit(const dst_point_st& pt)
{
    //
    // Unpack charge and status flag
    //
    const ULong_t iflag = pt.charge/(1L<<16);
    const ULong_t tpcq  = pt.charge - iflag*(1L<<16);
    mCharge = Float_t(tpcq)/(1<<25);
    mFlag = static_cast<UChar_t>(iflag&255);  // First 8 bits
    mFitFlag = static_cast<UChar_t>(iflag>>8);// Last  8 bits
    //
    // Unpack position in xyz
    //
    const Float_t maxRange   = 220;
    const Float_t mapFactor  = 2380;
    ULong_t tpcy11 = pt.position[0]/(1L<<20);
    ULong_t tpcz   = pt.position[1]/(1L<<10);
    ULong_t tpcx   = pt.position[0] - (1L<<20)*tpcy11;
    ULong_t tpcy10 = pt.position[1] - (1L<<10)*tpcz;
    ULong_t tpcy   = tpcy11 + (1L<<10)*tpcy10;
    mPosition.setX(Float_t(tpcx)/mapFactor - maxRange);
    mPosition.setY(Float_t(tpcy)/mapFactor - maxRange);
    mPosition.setZ(Float_t(tpcz)/mapFactor - maxRange);
    
    //
    // Unpack error on position in xyz
    //
    tpcy11 = pt.pos_err[0]/(1L<<20);
    tpcz   = pt.pos_err[1]/(1L<<10);
    tpcx   = pt.pos_err[0] - (1L<<20)*tpcy11;
    tpcy10 = pt.pos_err[1] - (1L<<10)*tpcz;
    tpcy   = tpcy11 + (1L<<10)*tpcy10;
    mPositionError.setX(Float_t(tpcx)/(1L<<17));
    mPositionError.setY(Float_t(tpcy)/(1L<<17));
    mPositionError.setZ(Float_t(tpcz)/(1L<<17));

    //
    // The hardware position stays at it is
    //
    mHardwarePosition = pt.hw_position;

    
}

StTpcHit::~StTpcHit() {/* noop */}

StObject*
StTpcHit::clone() { return new StTpcHit(*this); }

ULong_t
StTpcHit::padsInHit() const
{
    return bits(15, 7);   // bits 15-21
}

ULong_t
StTpcHit::pixelsInHit() const
{
    return bits(22, 10);   // bits 22-31
}
