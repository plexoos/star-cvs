//*-- Author : Alexandre Suaide
// 
// $Id: StEmcADCtoEMaker.cxx,v 1.7 2001/10/24 14:41:44 suaide Exp $
// $Log: StEmcADCtoEMaker.cxx,v $
// Revision 1.7  2001/10/24 14:41:44  suaide
// ruge change on StEmcADCtoEMaker to a different software.
// The other version is kept as *.old for future debug
//
// Revision 1.13  2000/05 16:07:01  
// Add README
//
#include "StEmcADCtoEMaker.h"
#include "StChain.h"
#include "St_DataSetIter.h"
#include "StEventTypes.h"
#include "StEvent.h"
#include <iostream.h>
#include <math.h>
#include "StMessMgr.h"
#include "StEmcSimulatorMaker/StEmcSimulatorMaker.h"
#include "St_ems_Maker/St_ems_Maker.h"
#include "StEmcUtil/emcDetectorName.h"
#include "StEmcUtil/StEmcGeom.h"
#include "TStopwatch.h"
// DAQ Libraries
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDaqLib/EMC/EMC_Reader.hh"
#include "StDAQMaker/StDAQReader.h"

ClassImp(StEmcADCtoEMaker)

StEmcGeom*           geo[MAXDET];

//_____________________________________________________________________________
StEmcADCtoEMaker::StEmcADCtoEMaker(const char *name):StMaker(name)
{
  for(Int_t i=0;i<MAXDET;i++) kCalib[i]=kFALSE; // all detectors off
  kCalib[0]=kTRUE;   // bemc only
  isDaqFile=kTRUE;
  subtractPedestal=kTRUE;
}
//_____________________________________________________________________________
StEmcADCtoEMaker::~StEmcADCtoEMaker()
{
  for(Int_t i=0;i<MAXDET;i++) if(kCalib[i]) delete geo[i];
}
//_____________________________________________________________________________
Int_t StEmcADCtoEMaker::Init()
{   
  gMessMgr->SetLimit("StEmcADCtoEMaker",1000);
  
  //Making QA histgrams
  const Int_t   nx[] = {40,40,300,20,12,12,12,12};
  const Float_t xl[] = {-1.0,-1.0,-1.0,-1.0, 0.5 , 0.5, 0.5, 0.5};
  const Float_t xu[] = { 1.0, 1.0, 1.0, 1.0, 12.5,12.5,12.5,12.5};
  const Int_t   ny[] = {120, 120, 60, 900, 60, 60, 60, 60};
 
  m_nhit = new TH2F("EmcNHitsVsDet" ,"Number of hit(log) with energy > 0 .vs. Detector #",500,0.0,15.0,8,0.5,8.5);
  m_etot = new TH2F("EmcEtotVsDet" ,"Total energy(log) .vs. Detector #",500,-4.0,15.0,8,0.5,8.5);

  for (Int_t i=0; i<MAXDET; i++) if(kCalib[i])
  {
    TString name_h = detname[i] + "_Hits";
    TString name_e = detname[i] + "_Energy";
    TString name_a = detname[i] + "_ADC";
    TString name_a1= detname[i] + "_ADC1D";
    TString title_h= detname[i] + " Hits distribution with energy > 0";
    TString title_e= detname[i] + " Energy distribution";
    TString title_a= detname[i] + " ADC distribution";
    TString title_a1= detname[i] +" ADC distribution (log)";
    Float_t rpi = M_PI + 0.00001; 
    m_hits[i]   = new TH2F(name_h,title_h,nx[i],xl[i],xu[i],ny[i],-rpi, rpi);
    m_energy[i] = new TH2F(name_e,title_e,nx[i],xl[i],xu[i],ny[i],-rpi, rpi);
    m_adc[i]    = new TH2F(name_a,title_a,nx[i],xl[i],xu[i],ny[i],-rpi, rpi);
    m_adc1d[i]  = new TH1F(name_a1,title_a1,1000,0,8);

    // creating geometry ...
    geo[i]=new StEmcGeom(detname[i].Data());

  }
  
  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StEmcADCtoEMaker::Finish()
{
  return StMaker::Finish();
}
//_____________________________________________________________________________
Int_t StEmcADCtoEMaker::Make()
{  
  TStopwatch clock;
  clock.Start();
  
  if(!GetEmcEvent())
  {
    gMessMgr->Warning("StEmcADCtoEMaker::Make() - Can not get pointer to EMC Event");
    return kStWarn;
  }
   
  // loop over detectors...
  for(Int_t det=0;det<MAXDET;det++)
    if(kCalib[det])
    {
      cout <<"**** Calibrating detector "<<detname[det].Data()<<endl;
      
      m_CalibDb=NULL;
      TString DbName="Calibrations/emc/"+detname[det];      
      m_CalibDb=GetInputDB(DbName.Data());
      if(!m_CalibDb) // try old calibration format
      {
        gMessMgr->Warning("StEmcADCtoEMaker::Make() - Can not get new calib tables.... try old format");
        DbName="Calibrations/emc";
        m_CalibDb=GetInputDB(DbName.Data());
        if(!m_CalibDb)
        {
          gMessMgr->Warning("StEmcADCtoEMaker::Make() - Can not get pointer to Calibrations/emc");
          return kStWarn;
        }        
      }
            
      if(subtractPedestal) 
      {
        Bool_t pedOk=SubtractPedestal(det);
        if(!pedOk)
        {
          gMessMgr->Warning("StEmcADCtoEMaker::Make() - Can not subtract pedestal");
          return kStWarn;
        }
      }
            
      Int_t nhits=0;
      Float_t energy=0;
      Bool_t CalOk=Calibrate(det,&nhits,&energy);
      if(nhits>0 && CalOk) FillHistograms(det,nhits,energy);
    }
    
  if(!FillStEvent()) gMessMgr->Warning("StEmcADCtoEMaker::Make() - No StEvent to save EmcCollection");
  
  clock.Stop();
  cout <<"Time to run StEmcADCtoEMaker::Make() real = "<<clock.RealTime()<<"  cpu = "<<clock.CpuTime()<<" \n";

  return kStOK;
}
//_____________________________________________________________________________
void StEmcADCtoEMaker::GetStatus(Int_t det)
{
  m_StatusDb=GetInputDB("Conditions/emc");
  if(m_StatusDb)
  {
    //return;
  } 

  if(det==0) // bemc
  {
    for(Int_t i=1;i<=4800;i++)
    { 
      status[i-1]=0;
      if (i>=1861 && i<=2340) status[i-1]=1; // initial 2001 configuration
      if (i>=2021 && i<=2100) status[i-1]=0; // initial 2001 configuration    
    } 
  
    status[2309-1]=0;
    status[1986-1]=0;
    status[1979-1]=0;
  }
  else for(Int_t i=0;i<18000;i++) status[i]=0;
  return;

}
//_____________________________________________________________________________
StEmcCollection* StEmcADCtoEMaker::GetEmcCollectionFromDaq(TDataSet* daq)
{
  UInt_t eta[]={20,20,150,10};
  UInt_t sub[]={2,2,1,15};

  StEmcCollection* emcDaqUtil=new StEmcCollection();

  StDAQReader* TheDataReader=(StDAQReader*)(daq->GetObject());
  if(!TheDataReader) return 0;
  if(!TheDataReader->EMCPresent()) return 0;

  StEMCReader* TheEmcReader=TheDataReader->getEMCReader();
  if(!TheEmcReader) return 0;
  

  for(Int_t det=0;det<4;det++) if(kCalib[det]) 
  {
    GetStatus(det);
    StDetectorId id = static_cast<StDetectorId>(det+kBarrelEmcTowerId);
    StEmcDetector* detector = new StEmcDetector(id,120);

    for(UInt_t m=1;m<=120;m++)
      for(UInt_t e=1;e<=eta[det];e++)
        for(UInt_t s=1;s<=sub[det];s++)
        {
          unsigned short ADC=0;
          if(det==0) if(!TheEmcReader->getTowerADC((int)m,(int)e,(int)s,ADC)) goto next;
          if(det==2) if(!TheEmcReader->getSMDE_ADC((int)m,(int)e,ADC)) goto next;
          if(det==3) if(!TheEmcReader->getSMDP_ADC((int)m,(int)e,(int)s,ADC)) goto next;
          Int_t idh;
          geo[det]->getId(m,e,s,idh);
          if(ADC>0 && status[idh-1]==1)
          {
            StEmcRawHit* hit=new StEmcRawHit(id,m,e,s,(UInt_t)ADC);
            detector->addHit(hit);
          }
          next: continue;
        }

    emcDaqUtil->setDetector(detector);
  }

  return emcDaqUtil;
}
//_____________________________________________________________________________
Bool_t StEmcADCtoEMaker::GetEmcEvent()
{
  StEmcCollection* emctemp=NULL;
  
// check if there is event from DAQ
  if(isDaqFile)
  {
    TDataSet* mTheEmcData   = GetDataSet("StDAQReader");
 	  if(!mTheEmcData) return kFALSE;
    emctemp = GetEmcCollectionFromDaq(mTheEmcData);
    if(!emctemp) return kFALSE;
    m_emc=emctemp;
    return kTRUE;
  }
  else
  {  
    // check if there is event from StEmcSimulator
    cout <<"Trying to get event from Simulator\n";
    StEmcSimulatorMaker* simnew = (StEmcSimulatorMaker*)GetMaker("emcRaw");
    if(simnew)
    { 
      emctemp = (StEmcCollection*)simnew->getEmcCollection();
      if(emctemp)
      {
        m_emc=emctemp;
        return kTRUE;
      }
    }
    else 
    {
      St_ems_Maker* simold = (St_ems_Maker*)GetMaker("emc_raw");;
      if(simold) 
      {
        emctemp = (StEmcCollection*)simold->getEmcCollection();
        if(emctemp)
        {
          m_emc=emctemp;
          return kTRUE;
        }
      }
    } 
    
    // check if there is event from StEvent
    cout <<"Trying to get event from StEvent\n";
    StEvent* event = (StEvent*)GetInputDS("StEvent");
    if(!event) return kFALSE;
    emctemp=event->emcCollection();
    if(!emctemp)  return kFALSE;
  
    m_emc=emctemp;
  }

  return kTRUE;
  
}
//_____________________________________________________________________________
Bool_t StEmcADCtoEMaker::SubtractPedestal(Int_t detnum)
{
  cout <<"subtracting pedestals for detector "<<detname[detnum].Data()<<endl;
  TString TableName=detname[detnum]+"Pedestal";
  StDetectorId id = static_cast<StDetectorId>(detnum+kBarrelEmcTowerId);  
  StEmcDetector* detector=m_emc->detector(id);
  if(!detector) return kFALSE;
  
  emcPedestal_st *emcpedst=NULL;
  smdPedestal_st *smdpedst=NULL;
   
  if(detnum<2) //bemc and bprs
  {
    St_emcPedestal* ped=(St_emcPedestal*)m_CalibDb->Find(TableName.Data());
    if(!ped) return kFALSE;
    emcpedst=ped->GetTable();
    if(!emcpedst) return kFALSE;
  }
  else // SMD
  {
    St_smdPedestal* ped=(St_smdPedestal*)m_CalibDb->Find(TableName.Data());
    if(!ped) return kFALSE;
    smdpedst=ped->GetTable();
    if(!smdpedst) return kFALSE;  
  }
  
  for(UInt_t j=1;j<121;j++)
  {
    StEmcModule* module = detector->module(j);
    if(module)
    {
      StSPtrVecEmcRawHit& rawHit=module->hits();
      for(UInt_t k=0;k<rawHit.size();k++)
      {
        Int_t m=rawHit[k]->module();
        Int_t e=rawHit[k]->eta();
        Int_t s=abs(rawHit[k]->sub());
        Float_t adc=(Float_t) rawHit[k]->adc();
       
        Int_t idh;
        geo[detnum]->getId(m,e,s,idh);
        Float_t pedestal=0;
        
        if(detnum<2) pedestal=emcpedst[idh-1].AdcPedestal;
        else pedestal=smdpedst[idh-1].AdcPedestal[0];
        
        adc-=pedestal;
        if(adc<0) adc=0;
        rawHit[k]->setAdc(adc);
      }
    }
  }
  return kTRUE;
}
//_____________________________________________________________________________
Bool_t StEmcADCtoEMaker::Calibrate(Int_t detnum,Int_t* NHITS,Float_t* ENERGY)
{
  cout <<"applying calibration for detector "<<detname[detnum].Data()<<endl;
  Int_t nhits=0;
  Float_t totalenergy=0;

  StDetectorId id = static_cast<StDetectorId>(detnum+kBarrelEmcTowerId);
  StEmcDetector* detector=m_emc->detector(id);
  
  if(!detector) return kFALSE;
  
  TString TableName=detname[detnum]+"Calibration";
  St_emcCalibration* cal=(St_emcCalibration*)m_CalibDb->Find(TableName.Data());
  if(!cal) return kFALSE;

  emcCalibration_st* cal_st=cal->GetTable();
  if(!cal_st) return kFALSE;

  for(UInt_t j=1;j<121;j++)
  {
    StEmcModule* module = detector->module(j);
    if(module)
    {
      StSPtrVecEmcRawHit& rawHit=module->hits();
      for(UInt_t k=0;k<rawHit.size();k++)
      {
        Int_t m=rawHit[k]->module();
        Int_t e=rawHit[k]->eta();
        Int_t s=abs(rawHit[k]->sub());
        Float_t adc=(Float_t) rawHit[k]->adc();
       
        Int_t idh;
        geo[detnum]->getId(m,e,s,idh);
        Float_t energy=0;
        Bool_t st=kFALSE;
        if(cal_st[idh-1].Status==1 && cal_st[idh-1].CalibStatus==1)
        {
           st=kTRUE;
           Float_t adcpower=1;
           for(Int_t i=0;i<5;i++) {energy+=cal_st[idh-1].AdcToE[i]*adcpower; adcpower*=adc;}            
        }
        if (energy<0) energy=0;
        rawHit[k]->setEnergy(energy);
        if (st)
        {          
          rawHit[k]->setCalibrationType(1);
          nhits++;
          totalenergy+=energy;
        }
        //cout <<"hit "<<k<<"  m = "<<m<<"  e = "<<e<<"  s = "<<s<<"  adc = "<<adc<<"  en = "<<energy<<endl;
      }
    }
  }
  *NHITS=nhits;
  *ENERGY=totalenergy;
  return kTRUE;
}
//_____________________________________________________________________________
Bool_t StEmcADCtoEMaker::FillHistograms(Int_t detnum,Int_t nhits,Float_t energy)
{
  if(nhits>0)  m_nhit->Fill(log((Float_t)nhits),(Float_t)detnum+1);
  if(energy>0) m_etot->Fill(log(energy),(Float_t)detnum+1);
    
  StDetectorId id = static_cast<StDetectorId>(detnum+kBarrelEmcTowerId);
  StEmcDetector* detector=m_emc->detector(id);
  if(!detector) return 0;
  
  Float_t totaladc=0;
  
  for(UInt_t j=1;j<121;j++)
  {
    StEmcModule* module = detector->module(j);
    if(module)
    {
      StSPtrVecEmcRawHit& rawHit=module->hits();
      for(UInt_t k=0;k<rawHit.size();k++)
      {
        Int_t m=rawHit[k]->module();
        Int_t e=rawHit[k]->eta();
        Int_t s=abs(rawHit[k]->sub());
        Float_t energy=rawHit[k]->energy();
        Float_t adc=(Float_t)rawHit[k]->adc();
        Int_t idh;
        geo[detnum]->getId(m,e,s,idh);
        Float_t eta,phi;
        geo[detnum]->getEtaPhi(idh,eta,phi);
        if(adc>0) {m_hits[detnum]->Fill(eta,phi); m_adc[detnum]->Fill(eta,phi);totaladc+=adc;}
        if(energy>0) m_energy[detnum]->Fill(eta,phi,energy);
      }
    }
  }
  if(totaladc>0) m_adc1d[detnum]->Fill(log(totaladc));
  return kTRUE;
}
//_____________________________________________________________________________
Bool_t StEmcADCtoEMaker::FillStEvent()
{  
  StEvent* event = (StEvent*)GetInputDS("StEvent");
  if(!event) return kFALSE;
  StEmcCollection* emctemp=event->emcCollection();
  if(emctemp) delete emctemp;
  event->setEmcCollection(m_emc);
  return kTRUE;
}







