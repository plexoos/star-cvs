// $Id: StEmcADCtoEMaker.h,v 1.7 2001/10/24 14:47:16 suaide Exp $
// $Log: StEmcADCtoEMaker.h,v $
// Revision 1.7  2001/10/24 14:47:16  suaide
// type correction
//
// Revision 1.6  2001/10/24 14:41:45  suaide
// huge change on StEmcADCtoEMaker to a different software.
// The other version is kept as *.old for future debug
//
//
#ifndef STAR_StEmcADCtoEMaker
#define STAR_StEmcADCtoEMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif

#include <TH1.h>
#include <TH2.h>

#include "tables/St_emcCalibration_Table.h"
#include "tables/St_emcPedestal_Table.h"
#include "tables/St_smdPedestal_Table.h"

#include "tables/St_controlADCtoE_Table.h"
#include "StEmcUtil/emcInternalDef.h"

class StEmcCollection;

class StEmcADCtoEMaker : public StMaker 

{
  private: 
           TH2F              *m_nhit;           //! 
           TH2F              *m_etot;           //!
           TH2F              *m_hits[MAXDET];   //!
           TH2F              *m_adc[MAXDET];   //!
           TH2F              *m_energy[MAXDET]; //!
           TH1F              *m_adc1d[MAXDET]; //!
           TDataSet          *m_CalibDb;
           TDataSet          *m_StatusDb;
           StEmcCollection   *m_emc;            //!
           
           Int_t             status[18000];
           
           StEmcCollection   *GetEmcCollectionFromDaq(TDataSet* daq);           
           void              GetStatus(Int_t);
           
  protected:    
    
  public: 
           Bool_t            kCalib[MAXDET];
           Bool_t            isDaqFile;
           Bool_t            subtractPedestal;
                 
                             StEmcADCtoEMaker(const char *name="Eread");
   virtual                   ~StEmcADCtoEMaker();
   virtual Int_t             Init();
   virtual Int_t             Make();
   virtual Int_t             Finish();
           Bool_t            GetEmcEvent();
           Bool_t            SubtractPedestal(Int_t);
           Bool_t            Calibrate(Int_t,Int_t*,Float_t*);
           Bool_t            FillHistograms(Int_t,Int_t,Float_t);
           Bool_t            FillStEvent();
   
   static  controlADCtoE_st  *getControlTable()  {return NULL;}
           StEmcCollection   *getEmcCollection() {return m_emc;}
           void              clearStEventStaf()  { m_emc = 0;}

   ClassDef(StEmcADCtoEMaker, 1)  
};

#endif
