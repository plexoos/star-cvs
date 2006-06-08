//////////////////////////////////////////////////////////////////////////
//
// StEmcTriggerMaker A. A. P. Suaide (C) 2001
//
// StEmcTriggerMaker R. Fatemi (2005)
//////////////////////////////////////////////////////////////////////////

#ifndef STAR_StEmcTriggerMaker
#define STAR_StEmcTriggerMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include <TH1.h>
#include <TH2.h>
#include "emc_def.h"
#include "StBemcTrigger.h"
#include "StEmcUtil/database/StBemcTables.h"

class StEvent;

class StEmcTriggerMaker : public StMaker
{
private:
    StBemcTrigger*    mBemcTrigger;
    bool              mSaveStEvent;
    bool              mPrint;

    int               isTrig[10];
    int               TowJetId[10];
    int               DsmAdc[10];
    int               numHT[5];
    int               numJP[5];
    int               HT12005array[kNTowers];//array of towers which pass trigger
    int               HT22005array[kNTowers];//array of towers which pass trigger
    int               JP12005array[kNJet];//array of JP's which pass trigger
    int               JP22005array[kNJet];//array of JP's which pass trigger


    TH2F*             mHTBefore;
    TH2F*             mPABefore;
    TH2F*             mHTCorrel;
    TH2F*             mPACorrel;
    TH2F*             mHT;
    TH2F*             mPA;

    int mIs2003HT1;
    int mIs2004HT1;
    int mIs2004JP1;
    int mIs2004HT2;
    int mIs2004JP2;
    int mIs2005HT1;
    int mIs2005JP1;
    int mIs2005HT2;
    int mIs2005JP2;
    int mIs2005ADJ;

    int HT1_ID_2003;
    int HT1_ID_2004;
    int HT2_ID_2004;
    int JP1_ID_2004;
    int JP2_ID_2004;
    int HT1_ID_2005;
    int HT2_ID_2005;
    int JP1_ID_2005;
    int JP2_ID_2005;
    int ADJ_ID_2005;

    int HT1_DSM_2003;
    int HT1_DSM_2004;
    int HT2_DSM_2004;
    int JP1_DSM_2004;
    int JP2_DSM_2004;
    int HT1_DSM_2005;
    int HT2_DSM_2005;
    int JP1_DSM_2005;
    int JP2_DSM_2005;
    int ADJ_DSM_2005;

    int numHT1_2005;
    int numHT2_2005;
    int numJP1_2005;
    int numJP2_2005;
    int numADJ_2005;
    int HT1_2005_array[4800];
    int HT2_2005_array[4800];
    int JP1_2005_array[12];
    int JP2_2005_array[12];
    int ADJ_2005_array[12];

protected:
public:
    StEmcTriggerMaker(const char *name="bemctrigger");
    virtual           ~StEmcTriggerMaker();

    virtual Int_t     Init();
    virtual Int_t     Make();
    virtual Int_t     Finish();

    void              fillStEvent(StEvent*);
    void              fillHistograms(StEvent*);
    void              saveHistograms(char*);
    void              setSaveStEvent(bool a) {mSaveStEvent = a;}
    void              setPrint(bool a){  mPrint = a;}

    int               is2003HT1() {return mIs2003HT1;}//1=true,0=false,-1=problem
    int               is2004HT1() {return mIs2004HT1;}
    int               is2004HT2() {return mIs2004HT2;}
    int               is2004JP1() {return mIs2004JP1;}
    int               is2004JP2() {return mIs2004JP2;}
    int               is2005HT1() {return mIs2005HT1;}
    int               is2005HT2() {return mIs2005HT2;}
    int               is2005JP1() {return mIs2005JP1;}
    int               is2005JP2() {return mIs2005JP2;}
    int               is2005ADJ() {return mIs2005ADJ;}

    int               get2003HT1_ID() {return HT1_ID_2003;}//tower/JP id of trigger
    int               get2004HT1_ID() {return HT1_ID_2004;}
    int               get2004HT2_ID() {return HT2_ID_2004;}
    int               get2004JP1_ID() {return JP1_ID_2004;}
    int               get2004JP2_ID() {return JP2_ID_2004;}
    int               get2005HT1_ID() {return HT1_ID_2005;}
    int               get2005HT2_ID() {return HT2_ID_2005;}
    int               get2005JP1_ID() {return JP1_ID_2005;}
    int               get2005JP2_ID() {return JP2_ID_2005;}
    int               get2005ADJ_ID() {return ADJ_ID_2005;}

    int               get2003HT1_ADC() {return HT1_DSM_2003;}//6 bit DSM ADC
    int               get2004HT1_ADC() {return HT1_DSM_2004;}
    int               get2004HT2_ADC() {return HT2_DSM_2004;}
    int               get2004JP1_ADC() {return JP1_DSM_2004;}
    int               get2004JP2_ADC() {return JP2_DSM_2004;}
    int               get2005HT1_ADC() {return HT1_DSM_2005;}
    int               get2005HT2_ADC() {return HT2_DSM_2005;}
    int               get2005JP1_ADC() {return JP1_DSM_2005;}
    int               get2005JP2_ADC() {return JP2_DSM_2005;}
    int               get2005ADJ_ADC() {return ADJ_DSM_2005;}

    void              get2005HT1_TOWS(int, int*);//array of tow ids passing HT1_2005 trig
    void              get2005HT2_TOWS(int, int*);//array of tow ids passing HT2_2005 trig
    int               get2005HT1_NTOWS() {return numHT1_2005;}//# tows passing HT1_2005 trig
    int               get2005HT2_NTOWS() {return numHT2_2005;}//# tows passing HT2_2005 trig

    void              get2005JP1_PATCHES(int, int*);//array of patches passing JP1_2005
    void              get2005JP2_PATCHES(int, int*);//array of patches passing JP2_2005
    int               get2005JP1_NPATCHES() {return numJP1_2005;}//# patches passing JP1_2005
    int               get2005JP2_NPATCHES() {return numJP2_2005;}//# patches passing JP2_2005


    StBemcTrigger*    getTrigger()
    {
        return mBemcTrigger;
    }

    void              setDbMaker(St_db_Maker *dbMk)
    {
        mBemcTrigger->setDbMaker(dbMk);
    }

    StBemcTables*     tables;

    void              setTableMaker(StBemcTables *bemcTab)
    {
        mBemcTrigger->setTableMaker(bemcTab);
    }

    int               trigPatch[300];

    ClassDef(StEmcTriggerMaker,0)
};
#endif
