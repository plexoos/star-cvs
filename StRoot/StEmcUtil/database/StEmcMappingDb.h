#ifndef STAR_StEmcMappingDb
#define STAR_StEmcMappingDb

// $Id: StEmcMappingDb.h,v 1.2 2009/01/30 18:09:04 kocolosk Exp $

/*****************************************************************************
 * @class StEmcMappingDb
 * @author Adam Kocoloski
 *
 * Wrapper around b*Map tables in Calibrations_emc. Relies on St_db_Maker to 
 * choose the correct tables if the Maker is present, otherwise it calls 
 * StDbManager directly. The date and time MUST match St_db_Maker's responses
 * for GetDate() and GetTime().
 *****************************************************************************/

#include "TObject.h"
#include "TDatime.h"

#include "StEvent/StEnumerations.h"

#include "bemcMap.h"
#include "bprsMap.h"
#include "bsmdeMap.h"
#include "bsmdpMap.h"

class St_bemcMap;
class St_bsmdeMap;
class St_bsmdpMap;
class St_bprsMap;
 
class StDbTable;
class StDataBaseI;
class St_db_Maker;

class StEmcMappingDb : public TObject 
{
public:
    StEmcMappingDb(int date=20330101, int time=0);
    virtual ~StEmcMappingDb();
    
    /// St_db_Maker-compatible interface
    void SetDateTime(int date, int time);
    void SetFlavor(const char *flavor, const char *tablename=NULL);
    void SetMaxEntryTime(int date, int time);
    
    const bemcMap_st & bemc(int softId) const;
    const bemcMap_st & btow(int softId) const { return bemc(softId); }
    const bprsMap_st & bprs(int softId) const;
    const bsmdeMap_st & bsmde(int softId) const;
    const bsmdpMap_st & bsmdp(int softId) const;
    
    int softIdFromMES(StDetectorId det, int m, int e, int s) const;
    
    // these 3 will crash if det != kBarrelEmcTowerId
    int softIdFromCrate(StDetectorId det, int crate, int channel) const;
    int softIdFromDaqId(StDetectorId det, int daqId) const;
    int softIdFromTDC(StDetectorId det, int TDC, int channel) const;
    
    // this will crash if det == kBarrelEmcTowerId
    int softIdFromRDO(StDetectorId det, int rdo, int channel) const;

private:
    St_bemcMap      *mBemcTTable;
    St_bprsMap      *mBprsTTable;
    St_bsmdeMap     *mSmdeTTable;
    St_bsmdpMap     *mSmdpTTable;
    
    Int_t           mBemcValidity;
    Int_t           mBprsValidity;
    Int_t           mSmdeValidity;
    Int_t           mSmdpValidity;
    
    StDbTable       *mBemcTable;
    StDbTable       *mBprsTable;
    StDbTable       *mSmdeTable;
    StDbTable       *mSmdpTable;
    
    StDataBaseI     *mDBI;
    
    St_db_Maker     *mDbMk;
    
    TDatime         mBeginTime;
    void            reload();
    bool            mGlobalDirty;
    bool            isDirty(StDbTable*);
    
    ClassDef(StEmcMappingDb, 1)
};

#endif

/*****************************************************************************
 * $Log: StEmcMappingDb.h,v $
 * Revision 1.2  2009/01/30 18:09:04  kocolosk
 * use version returned by StMaker::GetValidity to skip table reload if possible
 *
 * Revision 1.1  2009/01/08 02:16:19  kocolosk
 * move StEmcMappingDb/StEmcDecoder to StEmcUtil/database
 *
 * Revision 2.2  2009/01/02 03:34:34  kocolosk
 * use default date==20330101 like St_db_Maker to suppress spurious error msgs
 *
 * Revision 2.1  2008/12/05 19:05:32  kocolosk
 * new DB-backed implementation of StEmcDecoder
 *
 *****************************************************************************/
