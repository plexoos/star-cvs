/***************************************************************************
 *
 * $Id: StTriggerData.h,v 2.9 2004/10/20 18:56:22 ullrich Exp $
 *
 * Author: Akio Ogawa & Mirko Planinic, Feb 2003
 ***************************************************************************
 *
 * Description: abstract class for trigger data
 *
 ***************************************************************************
 *
 * $Log: StTriggerData.h,v $
 * Revision 2.9  2004/10/20 18:56:22  ullrich
 * Add method getRawSize().
 *
 * Revision 2.8  2004/08/03 17:22:16  ullrich
 * Major update by Akio and Marco.
 *
 * Revision 2.7  2004/07/20 18:02:26  jeromel
 * Updates from Akio to fix CTB issues.
 *
 * Revision 2.6  2004/02/11 01:39:51  ullrich
 * Use enumeration StBeamDirector for east/west. Add member for ZDC vertex.
 *
 * Revision 2.5  2004/01/28 00:29:49  ullrich
 * Methods to retrieve ZDC data added.
 *
 * Revision 2.4  2003/12/23 21:58:28  ullrich
 * Modifications to handle StTruggerData2004.
 *
 * Revision 2.3  2003/07/16 19:58:31  perev
 * Cleanup of StTriggerData2003 at all
 *
 * Revision 2.2  2003/05/21 03:58:44  ullrich
 * Added more methods to retrieve spin bits.
 *
 * Revision 2.1  2003/04/16 17:47:41  ullrich
 * Initial Revision.
 *
 **************************************************************************
 *
 * prepost argument should go between -5 and 5, and 0 is triggered crossing
 *
 **************************************************************************/

#ifndef StTriggerData_hh
#define StTriggerData_hh

#include "StObject.h"
#include "StEnumerations.h"
#include "StMessMgr.h"

class StTriggerData : public StObject {
public:
    StTriggerData();
    virtual ~StTriggerData();
    
    virtual void dump() const = 0;   //dump data into text
    
    // version and data type information
    virtual int year() const;                          // year of the data
    virtual unsigned int version() const = 0;          // TrgDataType Version Number 
    virtual unsigned int numberOfPreXing() const = 0;  // # of pre xing data for detectors
    virtual unsigned int numberOfPostXing() const = 0; // # of post xing data for detectors

    // generic trigger infomations
    virtual unsigned int token() const = 0;
    virtual unsigned int triggerWord() const = 0;
    virtual unsigned int actionWord() const = 0;  
    virtual unsigned short busyStatus() const;
    virtual unsigned short dsmInput() const;
    virtual unsigned short trgToken() const;
    virtual unsigned short dsmAddress() const;
    virtual unsigned short mAddBits() const;
    virtual unsigned short bcData(int channel) const;

    // bunch and spin bits
    virtual unsigned int bunchCounterHigh() const;
    virtual unsigned int bunchCounterLow() const;
    virtual unsigned int bunchId48Bit() const;
    virtual unsigned int bunchId7Bit() const;
    virtual unsigned int spinBit() const;
    virtual unsigned int spinBitYellowFilled() const;
    virtual unsigned int spinBitYellowUp() const;
    virtual unsigned int spinBitYellowDown() const;
    virtual unsigned int spinBitYellowUnpol() const;
    virtual unsigned int spinBitBlueFilled() const;
    virtual unsigned int spinBitBlueUp() const;
    virtual unsigned int spinBitBlueDown() const;
    virtual unsigned int spinBitBlueUnpol() const;
  
    // high level DSM infos
    virtual unsigned short tcuBits() const = 0;
    virtual unsigned short lastDSM(int channel) const;
    virtual unsigned short vertexDSM(int channel) const;
    virtual unsigned short ctbLayer1DSM(int channel) const;
    virtual unsigned short ctbLayer2DSM(int channel) const;
    virtual unsigned short bemcLayer1DSM(int channel, int prepost=0) const;
    virtual unsigned short eemcLayer1DSM(int channel, int prepost=0) const;
    virtual unsigned short emcLayer2DSM(int channel) const;
    virtual unsigned short fpdLayer1DSMRaw(StBeamDirection eastwest, int channel, int prepost=0) const;
    virtual unsigned short fpdLayer1DSM(StBeamDirection eastwest, int module, int board, int prepsot=0) const;
    virtual unsigned short fpdLayer2DSMRaw(int channel) const;
    virtual unsigned short fpdLayer2DSM(StBeamDirection eastwest, int module) const;

    // CTB
    virtual unsigned short ctbRaw(int address, int prepost=0) const;
    virtual unsigned short ctb(int pmt, int prepost=0) const;
    virtual unsigned short ctbTraySlat(int tray, int slat, int prepost=0) const;
    virtual unsigned short ctbSum(int prepost=0) const;

    // MWC
    virtual unsigned short mwc(int sector, int prepost=0) const;

    // ZDC 
    virtual unsigned short zdcAtChannel(int channel, int prepost=0) const;
    virtual unsigned short zdcAtAddress(int address, int prepost=0) const;
    virtual unsigned short zdcUnAttenuated(StBeamDirection eastwest, int prepost=0) const;
    virtual unsigned short zdcAttenuated(StBeamDirection eastwest, int prepost=0) const;
    virtual unsigned short zdcADC(StBeamDirection eastwest, int pmt, int prepost=0) const;
    virtual unsigned short zdcTDC(StBeamDirection eastwest, int prepost=0) const;
    virtual unsigned short zdcHardwareSum(int prepost=0) const;

    //ZDCSMD
    virtual unsigned short zdcSMD(StBeamDirection eastwest, int verthori, int strip, int prepost=0) const;
  
    // EMC
    virtual unsigned char bemcHighTower(int patch_id, int prepost=0) const;
    virtual unsigned char bemcJetPatch (int patch_id, int prepost=0) const;
    virtual unsigned char eemcHighTower(int patch_id, int prepost=0) const;
    virtual unsigned char eemcJetPatch (int patch_id, int prepost=0) const;
    virtual unsigned char bemcHighestTowerADC(int prepost=0) const;
    virtual unsigned char eemcHighestTowerADC(int prepost=0) const;

    // BBC bbcTDC
    virtual unsigned short bbcADC(StBeamDirection eastwest, int pmt, int prepost=0) const;
    virtual unsigned short bbcTDC(StBeamDirection eastwest, int pmt, int prepost=0) const;
    virtual unsigned short bbcADCSum(StBeamDirection eastwest, int prepost=0) const;
    virtual unsigned short bbcADCSumLargeTile(StBeamDirection eastwest, int prepost=0) const;
    virtual unsigned short bbcEarliestTDC(StBeamDirection eastwest, int prepost=0) const;
    virtual unsigned short bbcTimeDifference() const;
  
    // FPD  module #: north=0, south=1, top=2, bottom=3, north preshower=4, south preshower=5
    virtual unsigned short fpd(StBeamDirection eastwest, int module, int pmt, int prepost=0) const; 
    virtual unsigned short fpdSum(StBeamDirection eastwest, int module) const;

    // auxiliary information
    float zdcVertexZ() const;
    void  setZdcVertexZ(float);
    
    // Experts only!
    virtual char* getTriggerStructure() = 0;
    virtual int getRawSize() const = 0;

protected:
    int   mYear;
    float mZdcVertexZ;
    int prepostAddress(int prepost) const; //get pre&post xsing addess, return negative if bad.
        
    // Service routine to decode EMC layer0 DSM info into 12bit input values
    unsigned short decodeEmc12bit(const int dsm, const int channel, const unsigned char *raw) const;

    ClassDef(StTriggerData,2) 
};

//
//  Inline functions. Most of them return a default value (zero). Not all 
//  of them will be overwritten by classes inheriting from StTriggerData.
//
inline int StTriggerData::year() const {return mYear;}
inline float StTriggerData::zdcVertexZ() const {return mZdcVertexZ;}
inline void StTriggerData::setZdcVertexZ(float val) {mZdcVertexZ = val;}
inline unsigned short StTriggerData::dsmInput() const {return 0;}
inline unsigned short StTriggerData::trgToken() const {return 0;}
inline unsigned short StTriggerData::dsmAddress() const {return 0;}
inline unsigned short StTriggerData::mAddBits() const {return 0;}
inline unsigned short StTriggerData::bcData(int address) const {return 0;}
inline unsigned short StTriggerData::busyStatus() const {return 0;}
inline unsigned int StTriggerData::bunchCounterHigh() const {return 0;}
inline unsigned int StTriggerData::bunchCounterLow() const {return 0;}
inline unsigned int StTriggerData::bunchId48Bit() const {return 0;}
inline unsigned int StTriggerData::bunchId7Bit() const {return 0;}
inline unsigned int StTriggerData::spinBit() const {return 0;}
inline unsigned int StTriggerData::spinBitYellowFilled() const {return 0;}
inline unsigned int StTriggerData::spinBitYellowUp() const {return 0;}
inline unsigned int StTriggerData::spinBitYellowDown() const {return 0;}
inline unsigned int StTriggerData::spinBitYellowUnpol() const {return 0;}
inline unsigned int StTriggerData::spinBitBlueFilled() const {return 0;}
inline unsigned int StTriggerData::spinBitBlueUp() const {return 0;}
inline unsigned int StTriggerData::spinBitBlueDown() const {return 0;}
inline unsigned int StTriggerData::spinBitBlueUnpol() const {return 0;}
inline unsigned short StTriggerData::lastDSM(int channel) const {return 0;};
inline unsigned short StTriggerData::vertexDSM(int channel) const {return 0;}
inline unsigned short StTriggerData::ctbLayer1DSM(int channel) const {return 0;}
inline unsigned short StTriggerData::ctbLayer2DSM(int channel) const {return 0;}
inline unsigned short StTriggerData::bemcLayer1DSM(int channel, int prepost) const {return 0;}
inline unsigned short StTriggerData::eemcLayer1DSM(int channel, int prepost) const {return 0;}
inline unsigned short StTriggerData::emcLayer2DSM(int channel) const {return 0;}
inline unsigned short StTriggerData::fpdLayer1DSMRaw(StBeamDirection eastwest, int channel, int prepost) const {return 0;}
inline unsigned short StTriggerData::fpdLayer1DSM(StBeamDirection eastwest, int module, int board, int prepost) const {return 0;}
inline unsigned short StTriggerData::fpdLayer2DSMRaw(int channel) const {return 0;}
inline unsigned short StTriggerData::fpdLayer2DSM(StBeamDirection eastwest, int module) const {return 0;}
inline unsigned short StTriggerData::ctbRaw(int address, int prepost) const {return 0;}
inline unsigned short StTriggerData::ctb(int pmt, int prepost) const {return 0;}
inline unsigned short StTriggerData::ctbTraySlat(int tray, int slat, int prepost) const {return 0;}
inline unsigned short StTriggerData::ctbSum(int prepost) const {return 0;}
inline unsigned short StTriggerData::mwc(int sector, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcAtChannel(int channel, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcAtAddress(int address, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcUnAttenuated(StBeamDirection eastwest, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcAttenuated(StBeamDirection eastwest, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcADC(StBeamDirection eastwest, int pmt, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcTDC(StBeamDirection eastwest, int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcHardwareSum(int prepost) const {return 0;}
inline unsigned short StTriggerData::zdcSMD(StBeamDirection eastwest, int verthori, int strip, int prepost) const {return 0;}
inline unsigned char  StTriggerData::bemcHighTower(int patch_id, int prepost) const {return 0;}
inline unsigned char  StTriggerData::bemcJetPatch (int patch_id, int prepost) const {return 0;}
inline unsigned char  StTriggerData::eemcHighTower(int patch_id, int prepost) const {return 0;}
inline unsigned char  StTriggerData::eemcJetPatch (int patch_id, int prepost) const {return 0;}
inline unsigned char  StTriggerData::bemcHighestTowerADC(int prepost) const {return 0;}
inline unsigned char  StTriggerData::eemcHighestTowerADC(int prepost) const {return 0;}
inline unsigned short StTriggerData::bbcADC(StBeamDirection eastwest, int pmt, int prepost) const {return 0;}
inline unsigned short StTriggerData::bbcTDC(StBeamDirection eastwest, int pmt, int prepost) const {return 0;}
inline unsigned short StTriggerData::bbcADCSum(StBeamDirection eastwest, int prepost) const {return 0;}
inline unsigned short StTriggerData::bbcADCSumLargeTile(StBeamDirection eastwest, int prepost) const {return 0;}
inline unsigned short StTriggerData::bbcEarliestTDC(StBeamDirection eastwest, int prepost) const {return 0;}
inline unsigned short StTriggerData::bbcTimeDifference() const {return 0;}
inline unsigned short StTriggerData::fpd(StBeamDirection eastwest, int module, int pmt, int prepost) const {return 0;} 
inline unsigned short StTriggerData::fpdSum(StBeamDirection eastwest, int module) const {return 0;}
  
#endif

