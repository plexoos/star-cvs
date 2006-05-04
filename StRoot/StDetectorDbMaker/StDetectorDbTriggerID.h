#ifndef StDetectorDbTriggerID_h
#define StDetectorDbTriggerID_h

#include "StMaker.h"
#include <map>
struct triggerID_st;
struct trigPrescales_st;
struct L0TriggerInfo_st;
struct defaultTrgLvl_st;
struct trigL3Expanded_st;
struct dsmPrescales_st;



enum { kDbTriggerBadID = 999 };

class StDetectorDbTriggerID{
public:
    static StDetectorDbTriggerID*  instance();
/*!
 Table RunLog/onl triggerID 
*/
    unsigned int               getIDRunNumber();
    unsigned int               getIDNumRows();
    unsigned int               getIdxTrg(unsigned int entry = 0);
    unsigned int               getDaqTrgId(unsigned int entry = 0);
    unsigned int               getOfflineTrgId(unsigned int entry = 0);
    unsigned int               getTrgNameVersion(unsigned int entry = 0);
    unsigned int               getTrgVersion(unsigned int entry = 0);
    unsigned int               getThreashVersion(unsigned int entry = 0);
    unsigned int               getPsVersion(unsigned int entry = 0);

/*!
 Table RunLog/onl trigPrescales.  
 These are prescales applied at L1,L2,L3; usually 1. 
*/
 
    int                        getSRunNumber();
    unsigned int               getSNumRows();
    int                        getIdxTrigger(unsigned int entry = 0);
    int                        getIdxLevel(unsigned int entry = 0);
    int                        getId(unsigned int entry = 0);
    float                      getPs(unsigned int entry = 0);

/*!
 Table RunLog/onl L0TriggerInfo.  
 This holds various information, including prescale at L0. 
*/

    int                        getL0RunNumber();
    unsigned int               getL0NumRows();
    int                        getL0DaqTrgId(unsigned int entry = 0);
    int                        getL0OfflineTrgId(unsigned int entry = 0);
    int                        getPsL0(unsigned int entry = 0);
    char*                      getName(unsigned int entry = 0);
    unsigned int               getDetectorLiveOnBits(unsigned int entry = 0);
    unsigned int               getDetectorLiveOffBits(unsigned int entry = 0);
    unsigned int               getDetectorRequest(unsigned int entry = 0);

/*!
 Table RunLog/onl trigL3Expanded.  
 New table for run 6, allows to unpack multiple meanings of L2 trigger ids.
 Also holds prescales applied for certain algorithm results in L2. 
*/
    int                        getTrigL3ExpandedRunNumber();
    unsigned int               getTrigL3ExpandedNumRows();
    char*                      getTrigL3ExpandedL2TriggerResultType(unsigned int entry=0);
    int                        getTrigL3ExpandedL3TrgId(unsigned int entry=0);
    int                        getTrigL3ExpandedL3ExpandedTrgId(unsigned int entry=0);
    int                        getTrigL3ExpandedL2Algo(unsigned int entry=0);
    float                      getTrigL3ExpandedL2Ps(unsigned int entry=0);
    char*                      getTrigL3ExpandedName(unsigned int entry=0);
        
/*!
  Table RunLog/onl dsmPrescales.
  New table for run 6.  Holds the prescale applied at DSM level, before L0.
*/
    int                        getDsmPrescalesRunNumber();
    unsigned int               getDsmPrescalesNumRows();
    int                        getDsmPrescalesTrgId(unsigned int entry=0);
    int                        getDsmPrescalesDsmPrescale(unsigned int entry=0);

/*! 
  Table Calibrations/trg defaultTrgLvl.
  Holds the level to which nominal() trigger ids should be applied (i.e. L1,L2,L3).
  With the exception of early on in run 3, always will point to L3.
  Note that this is superseded in run 6 with L3Expanded 
*/
    unsigned int              getDefaultTriggerLevel();
    
/*! 
  These two functions allow one to get the total prescale,
  i.e. DSM*L0*L1*L2*L3*L3Expanded
*/
    float                     getTotalPrescaleByTrgId(int trgId); /**< This will multiply the prescales at dsm*L0*L2; should be used by everybody */
    map<int,float>            getTotalPrescales(); /**< This returns all prescales active in the run */
    
    
    friend ostream& operator<<(ostream& os, StDetectorDbTriggerID& v);

    // These fuction will be public
    // but should be used only for debugging
    void update(StMaker*);
    friend class nobody; // for virtual ~
    
protected:
// members of triggerID 
    virtual ~StDetectorDbTriggerID();
    StDetectorDbTriggerID();
    triggerID_st* mTriggerID; // points to triggerID struct
    TTable* mIDTable; // points to table, need to re-intilize mTriggerID every event
    unsigned int mIDNumRows;

// members of trigPrescales
    trigPrescales_st* mTrigPrescales; // points to prescales struct
    TTable* mSTable; // points to table, need to re-intilize mTrigPrescales every event
    unsigned int mSNumRows;

// members of L0TriggerInfo
    L0TriggerInfo_st* mL0TriggerInfo; // points to L0TriggerInfo struct
    TTable* mL0Table; // points to table, need to re-intilize mL0TriggerInfo every event
    unsigned int mL0NumRows;

// members of defaultTrgLvl 
    defaultTrgLvl_st* mDefaultTriggerLevel; // points to prescales struct
    TTable* mDefTrgLvlTable; // points to table, need to re-intilize mDefaultTriggerLevel every event

// members of trigL3ExpandedTrgId 
    trigL3Expanded_st* mTrigL3Expanded; // points to prescales struct
    TTable* mTrigL3ExpandedTable; // points to table, need to re-intilize very event
    unsigned int mTrigL3ExpandedNumRows;
    
// members of dsmPrescales 
    dsmPrescales_st* mDsmPrescales; // points to prescales struct
    TTable* mDsmPrescalesTable; // points to table, need to re-intilize mDefaultTriggerLevel every event
    unsigned int mDsmPrescalesNumRows;
    
    StMaker* mMaker; // Holds pointer to maker
private:
    static StDetectorDbTriggerID* sInstance;
    
};

#endif
