#ifndef StDetectorId_hh
#define StDetectorId_hh
#include "StDetectorDefinitions.h"
enum StDetectorId {
  kUnknownId = kUnknownIdentifier,
  kTpcId = kTpcIdentifier,
  kSvtId = kSvtIdentifier,
  kRichId = kRichIdentifier,
  kFtpcWestId = kFtpcWestIdentifier,
  kFtpcEastId = kFtpcEastIdentifier,
  kTofId = kTofIdentifier,
  kCtbId = kCtbIdentifier,
  kSsdId = kSsdIdentifier,
  kBarrelEmcTowerId = kBarrelEmcTowerIdentifier,
  kBarrelEmcPreShowerId = kBarrelEmcPreShowerIdentifier,
  kBarrelSmdEtaStripId = kBarrelSmdEtaStripIdentifier,
  kBarrelSmdPhiStripId = kBarrelSmdPhiStripIdentifier,
  kEndcapEmcTowerId = kEndcapEmcTowerIdentifier,
  kEndcapEmcPreShowerId = kEndcapEmcPreShowerIdentifier,
  kEndcapSmdUStripId = kEndcapSmdUStripIdentifier,
  kEndcapSmdVStripId = kEndcapSmdVStripIdentifier,
  kZdcWestId   = kZdcWestIdentifier,
  kZdcEastId   = kZdcEastIdentifier,
  kMwpcWestId  = kMwpcWestIdentifier,
  kMwpcEastId  = kMwpcEastIdentifier,
  kTpcSsdId    = kTpcSsdIdentifier,
  kTpcSvtId    = kTpcSvtIdentifier,        
  kTpcSsdSvtId = kTpcSsdSvtIdentifier,        
  kSsdSvtId    = kSsdSvtIdentifier,
  kPhmdCpvId   = kPhmdCpvIdentifier,
  kPhmdId      = kPhmdIdentifier
};
// maximal sizes of tracking part of STAR in cm
enum StarMaxSize { kStarMaxR=200, kStarMaxZ=269 };
#endif


// $Id: StDetectorId.h,v 2.2 2004/10/17 03:29:33 perev Exp $
//
// $Log: StDetectorId.h,v $
// Revision 2.2  2004/10/17 03:29:33  perev
// enum StarMaxSize added
//
// Revision 2.1  2004/04/26 16:35:19  fisyak
// Move enumerations from pams/global/inc => StEvent
//
// Revision 1.9  2002/12/18 02:28:48  lbarnby
// PMD identifiers added
//




