/***************************************************************************
 *
 * $Id: StTrackTopologyMap.cxx,v 2.10 2000/12/08 20:21:08 genevb Exp $
 *
 * Author: Thomas Ullrich, Aug 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackTopologyMap.cxx,v $
 * Revision 2.10  2000/12/08 20:21:08  genevb
 * Changed kTofPatchId -> kTofId
 *
 * Revision 2.9  2000/07/28 19:49:28  akio
 * Change in Detector Id for Endcap SMD
 *
 * Revision 2.8  2000/05/17 17:21:34  ullrich
 * New method largestGap() and new output operator.
 *
 * Revision 2.7  2000/04/12 19:44:03  genevb
 * Reimplement mMap data members as individual unsigned ints
 *
 * Revision 2.6  2000/04/10 19:59:33  genevb
 * StRoot/StEvent/doc/tex/
 *
 * Revision 2.5  2000/03/29 00:16:30  ullrich
 * Fixed off-by-one error.
 *
 * Revision 2.4  2000/01/07 18:20:34  ullrich
 * Buf fixed. Wrong indices for mMap in bit().
 *
 * Revision 2.3  1999/12/13 20:16:36  ullrich
 * Changed numbering scheme for hw_position unpack methods (STAR conventions).
 *
 * Revision 2.2  1999/10/28 22:27:55  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.1  1999/10/13 19:45:46  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StTrackTopologyMap.h"
#include <vector>
#include <algorithm>
#include <numeric>
#if !defined(ST_NO_NAMESPACES)
using std::vector;
using std::adjacent_difference;
using std::max_element;
#endif

static const char rcsid[] = "$Id: StTrackTopologyMap.cxx,v 2.10 2000/12/08 20:21:08 genevb Exp $";

ClassImp(StTrackTopologyMap)

StTrackTopologyMap::StTrackTopologyMap()
{
    mMap0 = mMap1 = 0;
}

StTrackTopologyMap::StTrackTopologyMap(ULong_t m1, ULong_t m2)
{
    mMap0 = static_cast<Int_t>(m1);
    mMap1 = static_cast<Int_t>(m2);
}

StTrackTopologyMap::StTrackTopologyMap(const ULong_t* m)
{
    mMap0 = static_cast<Int_t>(m[0]);
    mMap1 = static_cast<Int_t>(m[1]);
}

StTrackTopologyMap::~StTrackTopologyMap() { /* noop */ }

Bool_t
StTrackTopologyMap::bit(Int_t i) const
{
    return i>31 ? (mMap1>>(i-32) & 1U) : (mMap0>>i & 1U);
}

Bool_t
StTrackTopologyMap::ftpcFormat() const
{
    return bit(63);
}

ULong_t
StTrackTopologyMap::data(UInt_t i) const
{
    return static_cast<ULong_t>(i<2 ? (i<1 ? mMap0 : mMap1) : 0);
}

Bool_t
StTrackTopologyMap::primaryVertexUsed() const { return bit(0); }

Bool_t
StTrackTopologyMap::turnAroundFlag() const { return bit(62); }

Bool_t
StTrackTopologyMap::hasHitInSvtLayer(UInt_t layer) const
{
    if (ftpcFormat())
        return kFALSE;
    else
        return bit(layer);
}

Bool_t
StTrackTopologyMap::hasHitInRow(StDetectorId id, UInt_t row) const
{
    switch (id) {
    case kTpcId:
        return !ftpcFormat() && bit(row+7);
        break;
    case kFtpcWestId:
        return ftpcFormat() && bit(row+10);
        break;
    case kFtpcEastId:
        return ftpcFormat() && bit(row);
        break;
    default:
        return kFALSE;
        break;
    }
}

UInt_t
StTrackTopologyMap::numberOfHits(StDetectorId id) const
{
    if (ftpcFormat() &&
        !(id == kFtpcWestId || id == kFtpcEastId))
        return 0;
    
    int i;
    int n = 0;

    switch (id) {
    case kSvtId:
        for (i=1; i<7; i++)
            if (hasHitInSvtLayer(i)) n++;
        break;
    case kSsdId:
        if (bit(7)) n++;
        break;
    case kFtpcWestId:
    case kFtpcEastId:
        for (i=1; i<11; i++)
            if (hasHitInRow(id, i)) n++;
        break;
    case kTpcId:
        for (i=1; i<46; i++)
            if (hasHitInRow(id, i)) n++;
        break;
    case kMwpcWestId:
    case kMwpcEastId:
        if (bit(53)) n++;
        break;
    case kCtbId:
        if (bit(54)) n++;
        break;
    case kTofId:
        if (bit(55)) n++;
        break;
    case kRichId:
        if (bit(56)) n++;
        break;
    case kBarrelEmcTowerId:
    case kBarrelEmcPreShowerId:
    case kBarrelSmdEtaStripId:
    case kBarrelSmdPhiStripId:
        if (bit(57)) n++;
        break;
    case kEndcapEmcTowerId:
    case kEndcapEmcPreShowerId:
    case kEndcapSmdUStripId:
    case kEndcapSmdVStripId:
        if (bit(58)) n++;
        break;
    default:
        n = 0;
        break;
    }
    return n;
}

Int_t
StTrackTopologyMap::largestGap(StDetectorId id) const
{
    if (ftpcFormat() && !(id == kFtpcWestId || id == kFtpcEastId))
	return -1;

    vector<int> rows;
    int i;

    switch (id) {
    case kSvtId:
        for (i=1; i<7; i++)
            if (hasHitInSvtLayer(i)) rows.push_back(i);
        break;
    case kFtpcWestId:
    case kFtpcEastId:
        for (i=1; i<11; i++)
            if (hasHitInRow(id, i)) rows.push_back(i);
        break;
    case kTpcId:
        for (i=1; i<46; i++)
            if (hasHitInRow(id, i)) rows.push_back(i);
        break;
    default:
        return -1;
    }

    if (rows.size() < 2) return -1;

    vector<int> diffs(rows.size());
    adjacent_difference(rows.begin(), rows.end(), diffs.begin());
    return *max_element(diffs.begin()+1, diffs.end()) - 1;  // skip first
}

ostream& operator<< (ostream& os, const StTrackTopologyMap& m)
{
    for (int i=0; i<64; i++) {
        if (i>31)
            os << ((m.data(1)>>(i-32) & 1U) ? 1 : 0);
        else
            os << ((m.data(0)>>i & 1U) ? 1 : 0);
    }
    return os;
}

