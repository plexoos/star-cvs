// 
// $Id: StBemcRaw.cxx,v 1.10 2004/12/21 12:53:48 suaide Exp $
// $Log: StBemcRaw.cxx,v $
// Revision 1.10  2004/12/21 12:53:48  suaide
// moved StBemcTables to StEmcUtil
// corrected for y2005 PSD data banks
//
// Revision 1.9  2004/12/14 11:32:11  suaide
// added histograms for status tables creation
//
// Revision 1.8  2004/11/22 12:46:22  suaide
// added new flags for hit reconstruction. Status are not checked
// dureing production anymore in order to avoid bad status loaded in
// DB
//
// Revision 1.7  2004/11/12 21:17:56  suaide
// non initialization of some variables were fixed
//
// Revision 1.6  2004/11/02 03:23:09  suaide
// small changes in order to fix a bug
//
// Revision 1.5  2004/10/21 00:01:42  suaide
// small changes in histogramming and messages for BEMC
// Complete version for EEMC done by Jan Balewski
//
// Revision 1.4  2004/10/20 15:45:19  suaide
// few bugs fixed
//
// Revision 1.3  2004/10/20 14:24:21  suaide
// small fix to crateUnknown status in old files
//
// Revision 1.2  2004/10/19 17:53:00  suaide
// code clean up
//
// Revision 1.1  2004/10/18 18:20:06  suaide
// New Maker. Will replace StEmcADCtoEMaker in production.
// It reads only DAQ structures. Output is StEvent.
//
#include "StBemcRaw.h"
#include "Stiostream.h"
#include "StEmcUtil/others/emcDetectorName.h"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "TString.h"
#include "StEventTypes.h"
#include "StEvent.h"
// DAQ Libraries
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDaqLib/EMC/EMC_Reader.hh"
#include "StDAQMaker/StDAQReader.h"
#include "StDaqLib/EMC/StEmcDecoder.h"
#include "StMessMgr.h"

ClassImp(StBemcRaw)

//_____________________________________________________________________________
/* 
   Default constructor. Set Initial values for some variables
*/
StBemcRaw::StBemcRaw():TObject()
{  
  mSaveAllStEvent = kFALSE;
  mDecoder = 0;
  mDate = 0;
  mTables = new StBemcTables();    
  mControlADCtoE = new controlADCtoE_st();
  Int_t   calib[]      = {1, 1, 1, 1, 0, 0, 0, 0};
  Int_t   pedSub[]     = {1, 1, 1, 1, 0, 0, 0, 0};
  Float_t cut[]        = {-1, 1.5, 1.5, 1.5, -1, -1, -1, -1};
  Int_t   cutType[]    = {0, 1, 1, 1, 0, 0, 0, 0};
  Int_t   onlyCal[]    = {0, 0, 0, 0, 0, 0, 0, 0};
  Int_t   status[]     = {0, 0, 0, 0, 0, 0, 0, 0};
  Int_t   crate[]      = {1, 1, 1, 1, 0, 0, 0, 0};
  
  for(Int_t i=0; i<MAXDETBARREL; i++)
  {
    mControlADCtoE->Calibration[i]=calib[i];
    mControlADCtoE->DeductPedestal[i]=pedSub[i];  
    mControlADCtoE->CutOff[i]=cut[i];
    mControlADCtoE->CutOffType[i]=cutType[i];
    mControlADCtoE->OnlyCalibrated[i]=onlyCal[i];
    mControlADCtoE->CheckStatus[i]=status[i];
    mControlADCtoE->CheckCrate[i]=crate[i];
    mBarrelQAHisto[i] = 0;
  }

}
//_____________________________________________________________________________
/*! 
   Default destructor
*/
StBemcRaw::~StBemcRaw()
{
  if(mTables) delete mTables;
  if(mDecoder) delete mDecoder;
  if(mControlADCtoE) delete mControlADCtoE;
}
void StBemcRaw::printConf()
{ 
  gMessMgr->Info()<<"Configuration for BEMC hit reconstruction "<<endm;
  for(Int_t i=0;i<MAXDETBARREL;i++)
  {
    gMessMgr->Info() <<"  Configuration for detector "<<detname[i].Data()<<endm;
    gMessMgr->Info() <<"     switch for deducting pedestal     = "<<mControlADCtoE->DeductPedestal[i]<<endm;
    gMessMgr->Info() <<"     switch for calibration            = "<<mControlADCtoE->Calibration[i]<<endm;
    gMessMgr->Info() <<"     cutoff type                       = "<<mControlADCtoE->CutOffType[i]<<endm;
    gMessMgr->Info() <<"     cutoff value                      = "<<mControlADCtoE->CutOff[i]<<endm;
    gMessMgr->Info() <<"     save only calibrated hits         = "<<mControlADCtoE->OnlyCalibrated[i]<<endm;
    gMessMgr->Info() <<"     save only if status is ok         = "<<mControlADCtoE->CheckStatus[i]<<endm;
    gMessMgr->Info() <<"     save only if crate is ok          = "<<mControlADCtoE->CheckCrate[i]<<endm;
    gMessMgr->Info() <<"     SAVE ALL FLAG (overwrites above)  = "<<(Int_t)mSaveAllStEvent<<endm;
  }
}
void StBemcRaw::createDecoder(Int_t date, Int_t time)
{
  if(mDecoder) delete mDecoder;
  mDecoder = new StEmcDecoder(date,time);
}
void StBemcRaw::initHisto()
{
  mBarrelNHitHist         = new TH2F("BarrelNHit","BarrelNHit",500,0.0,18000.0,4,0.5,4.5);
  mBarrelEtotHist         = new TH2F("BarrelEtot","BarrelEtot",500,0.0,10000.0,4,0.5,4.5);
  mBarrelAdcSumHist       = new TH2F("BarrelAdcSum","BarrelAdcSum",500,0.0,1000000.0,4,0.5,4.5);
  mBarrelNCratesHist      = new TH2F("BarrelNCrates","BarrelNCrates",31,0.0,31.0,4,0.5,4.5);
  mBarrelCrateStatusHist  = new TH2F("BarrelCrateStatus","BarrelCrateStatus",6,-0.5,5.5,30,0.5,30.5);
}
void StBemcRaw::initQAHisto()
{
  for(Int_t det = 1;det<=MAXDETBARREL; det++)
  {
    Int_t N = BTOWCH;
    if(det>2) N= BSMDCH;  
    mBarrelQAHisto[det-1] = new TH2F(detname[det-1].Data(),detname[det-1].Data(),N,0.5,(Float_t)N+0.5,500,-0.5,499.5);
  }
}
void StBemcRaw::fillHisto()
{
  for(Int_t det = 1;det<=MAXDETBARREL; det++)
  {
    mBarrelNHitHist->Fill(getTotalSaved(det),det);
    mBarrelEtotHist->Fill(getTotalE(det),det);
    mBarrelAdcSumHist->Fill(getTotalADC(det),det);
    mBarrelNCratesHist->Fill(getNCratesOK(det),det);
  }
  for(Int_t crate = 1;crate<=MAXCRATES; crate++)
  {
    mBarrelCrateStatusHist->Fill(getCrateStatus(BTOW,crate),crate);
  }
}
Bool_t StBemcRaw::make(TDataSet* TheData, StEvent* event)
{
  if(!TheData) return kFALSE;
  if(!event)   return kFALSE;
  StEmcCollection* emc = event->emcCollection();
  if(!emc) return kFALSE;
  StEmcRawData *bemcRaw = emc->bemcRawData();
  if(!bemcRaw) return kFALSE;
  if(!convertFromDaq(TheData,bemcRaw)) return kFALSE;  
  return make(bemcRaw,event);
}
Bool_t StBemcRaw::make(StEmcRawData* bemcRaw, StEvent* event)
{
  if(!bemcRaw) return kFALSE;
  if(!event)   return kFALSE;
  StEmcCollection* emc = event->emcCollection();
  if(!emc) return kFALSE;
  
  checkHeaders(bemcRaw);
  emptyEmcCollection(emc);
  
  Int_t cap=0,crate=0;
  Int_t ADC=0;
  Float_t E=0;
  
  for(Int_t det = 1; det<=MAXDETBARREL; det++)
  {
    Int_t nch = BTOWCH;
    if(det>2) nch=BSMDCH;
    
    clearStats(det);    
    for(Int_t id = 1; id<=nch; id++) 
    {
      ADC = getBemcADCRaw(det,id,bemcRaw,crate,cap);
      Int_t S = makeHit(emc,det,id,ADC,crate,cap,E);
      updateStats(det,S,ADC,E);
    }
    printStats(det);
    StDetectorId did = static_cast<StDetectorId>(det+kBarrelEmcTowerId-1);
    StEmcDetector* detector=emc->detector(did);
    if(detector)
    {
      for(Int_t crate = 1;crate<=MAXCRATES;crate++)
        detector->setCrateStatus(crate,(StEmcCrateStatus)mCrateStatus[det-1][crate-1]);
    }
  }
  
  return kTRUE;
}
//_____________________________________________________________________________
/*!
  Read EMC from DAQ structure
*/
Bool_t StBemcRaw::convertFromDaq(TDataSet* DAQ, StEmcRawData* RAW)
{
  if(!DAQ) { gMessMgr->Warning() <<"Could not find DAQ DataSet "<<endm; return kFALSE; }
  if(!RAW) { gMessMgr->Warning() <<"Could not find StEmcRawData pointer for BEMC"<<endm; return kFALSE; }
	
  StDAQReader* TheDataReader=(StDAQReader*)(DAQ->GetObject());
  if(!TheDataReader || !TheDataReader->EMCPresent()) { gMessMgr->Warning() <<"Data Reader is not present "<<endm; return kFALSE; }

  StEMCReader* TheEmcReader=TheDataReader->getEMCReader();
  if(!TheEmcReader) { gMessMgr->Warning() <<"Could not find BEMC Reader "<<endm; return kFALSE; }
	
  EMC_Reader* reader = TheEmcReader->getBemcReader();
  if(!reader) {gMessMgr->Warning() <<"Could not find Barrel Reader "<<endm; return kFALSE; }
      
  if(reader->isTowerPresent())
  {
    Bank_BTOWERADCR& tower = reader->getBTOWERADCR();
    if(RAW->header(BTOWBANK)) RAW->deleteBank(BTOWBANK);
    RAW->createBank(0,BTOWHEADER,BTOWSIZE);
    for(Int_t i = 0; i<BTOWHEADER  ;i++) RAW->setHeader(BTOWBANK,i,tower.TDCHeader[i]);
    for(Int_t i = 0; i<BTOWSIZE ;i++) RAW->setData(BTOWBANK,i,tower.TowerADCArray[i]);
  }		
  // smd data  
  if(reader->isSmdPresent())
  {
    Bank_BSMDADCR& smd =  reader->getSMD_ADCR();
    Int_t NSMD = MAXSMDCRATES;
    // there is only 4 SMD Crates before that data and some
    // of them are PSD crates. For Y2004 AuAu runs PSD do
    // not have its own data format and it is being read as 
    // SMD
    if(mDate<20040701) NSMD = 4;
  
    for(Int_t i = 0; i<NSMD; i++)
    {      
      if(smd.HasData[i]==1)
      {
        Int_t bank = i+BSMDOFFSET;
        if(RAW->header(bank)) RAW->deleteBank(bank);
        RAW->createBank(bank,BSMDHEADER,BSMDSIZE);
        for(Int_t j=0; j<BSMDHEADER;  j++) RAW->setHeader(bank,j,smd.SmdHeader[i][j]);
        for(Int_t j=0; j<BSMDSIZE; j++) RAW->setData(bank,j,smd.SMDADCArray[i][j]);
      }
    }
    /////////////////////////////////////////////////////////////////////
    // read Pre Shower data for Y2004 AuAu data. This year, the PSD data
    // is read as SMD data for fibers 4 and 5.
    //
    // For y2005 data, PSD data is on SMD banks 8 to 12
    // 
    // This is a temporary solution while the PSD data format is not
    // decided by Tonko. He needs to have a decision on some
    // hardware issues before the data format is decided
    //
    // AAPSUAIDE 20040318
    //
    if(mDate>20040101)
    {
      for(Int_t RDO = 0; RDO<4; RDO++)
      {
        Int_t SMDRDO = RDO+NSMD;
        if(smd.HasData[SMDRDO]==1) 
        {
          Int_t bank = RDO+BPRSOFFSET;
          if(RAW->header(bank)) RAW->deleteBank(bank);
          RAW->createBank(bank,BPRSHEADER,BPRSSIZE);
          for(Int_t i = 0; i<BPRSHEADER;  i++) RAW->setHeader(bank,i,smd.SmdHeader[SMDRDO][i]);
          for(Int_t i = 0; i<BPRSSIZE; i++) RAW->setData(bank,i,smd.SMDADCArray[SMDRDO][i]);
        }
      }
    }
    /////////////////////////////////////////////////////////////////////
  }
  return kTRUE;    
}
void StBemcRaw::checkHeaders(StEmcRawData* RAW)
{
  for(Int_t det=1;det<=MAXDETBARREL; det++)
  {
    for(Int_t crate = 1; crate<=MAXCRATES;crate++) mCrateStatus[det-1][crate-1] = crateUnknown;
    mIsCorrupted[det-1] = kFALSE;
  }
  checkBtowCrates(RAW);
	
  mNCRATESOK[BPRS-1]=mNCRATESOK[BSMDE-1]=mNCRATESOK[BSMDP-1]=0;
  // smd data
  for(Int_t i = 0; i<MAXSMDCRATES; i++)
  {
    UShort_t *header = RAW->header(i+BSMDOFFSET);
    if(header)   
    {      
      mCrateStatus[BSMDE-1][i] = crateOK;
      mCrateStatus[BSMDP-1][i] = crateOK;
      mNCRATESOK[BSMDE-1]++;
      mNCRATESOK[BSMDP-1]++;
    }
  }
  // PSD data
  for(Int_t i = 0; i<MAXBPRSCRATES; i++)
  {
    UShort_t *header = RAW->header(i+BPRSOFFSET);
    if(header)   
    {      
      mCrateStatus[BPRS-1][i] = crateOK;
      mNCRATESOK[BPRS-1]++;
    }
  }
}
void StBemcRaw::emptyEmcCollection(StEmcCollection *emc)
{
  if(!emc) return;
  StSPtrVecEmcPoint& pvec = emc->barrelPoints();
  if(pvec.size()>0)  pvec.clear(); 
 
  for(Int_t i=0; i<MAXDETBARREL; i++)
  {
    StDetectorId id = static_cast<StDetectorId>(i+kBarrelEmcTowerId);
    StEmcDetector* detector=emc->detector(id);
    if(detector)
    {
      if(detector->cluster())
      {
      	StSPtrVecEmcCluster& cluster=detector->cluster()->clusters();
      	if(cluster.size()>0) cluster.clear();  
      }
      for(UInt_t j=1;j<=detector->numberOfModules() ;j++)
      {
        StEmcModule *module = detector->module(j);
        if(module)
        {
          StSPtrVecEmcRawHit&  hits=module->hits();
          hits.clear();
        }
      }
    }
  }
  return;
}
//_____________________________________________________________________________
/*!
  Check tower crates header
*/
void StBemcRaw::checkBtowCrates(StEmcRawData* RAW)
{
  if(!RAW) return;
  if(!mDecoder) return;
  UShort_t *header = RAW->header(BTOWBANK);
  if(!header) return;
  mNCRATESOK[0] = 0;
  for(Int_t crate = 1;crate<=MAXCRATES;crate++)
  {
    Int_t TDC;
    mDecoder->GetTowerTDCFromCrate(crate,TDC);
    Int_t sum = header[TDC];
    Int_t err = header[TDC+BTOWTDCERROFFSET];
    mCrateStatus[BTOW-1][crate-1] = crateUnknown;
    if(sum==BTOWBYTESUM && err == BTOWERRFLAG) mCrateStatus[BTOW-1][crate-1] = crateOK;
    else mCrateStatus[BTOW-1][crate-1] = crateHeaderCorrupt;
    
    if(sum==BTOWNOTPRESENT && err == BTOWNOTPRESENT) mCrateStatus[BTOW-1][crate-1] = crateNotPresent;
    
    if(mCrateStatus[BTOW-1][crate-1]==crateOK) mNCRATESOK[BTOW-1]++;
    
    if(mCrateStatus[BTOW-1][crate-1]==crateHeaderCorrupt) mIsCorrupted[BTOW-1] = kTRUE;
  }
  return;
}
void StBemcRaw::clearStats(Int_t det)
{
  mNZ[det-1] = 0;
  mNCRATE[det-1] = 0;
  mNSTATUS[det-1] = 0;
  mNRMS[det-1] = 0;
  mNPED[det-1] = 0;
  mNOK[det-1] = 0;
  mNTOTAL[det-1] = 0;
  mADCSUM[det-1] = 0;
  mTOTALE[det-1] = 0;
}
void StBemcRaw::updateStats(Int_t det,Int_t S,Int_t ADC, Float_t E)
{
  if(S==kZero) mNZ[det-1]++;
  else if(S==kCrate) mNCRATE[det-1]++;
  else if(S==kStatus) mNSTATUS[det-1]++;
  else if(S==kRms) mNRMS[det-1]++;
  else if(S==kPed) mNPED[det-1]++;
  else if(S==kOK) mNOK[det-1]++;
  mNTOTAL[det-1]++;
  if(S==kOK) 
  {
    mADCSUM[det-1]+=ADC;
    mTOTALE[det-1]+=E;
  }
}
void StBemcRaw::printStats(Int_t det)
{
      gMessMgr->Info() <<"Statistics for detector  "<<detname[det-1].Data()<<endm;
      gMessMgr->Info() <<"   Total number of crates with header ok = "<<mNCRATESOK[det-1]<<endm;
      gMessMgr->Info() <<"   Total number of hits                  = "<<mNTOTAL[det-1]<<endm;
      gMessMgr->Info() <<"   Total hits removed because of crates  = "<<mNCRATE[det-1]<<endm;
      gMessMgr->Info() <<"   Total hits removed because ADC = 0    = "<<mNZ[det-1]<<endm;
      gMessMgr->Info() <<"   Total hits removed by Pedestal        = "<<mNPED[det-1]+mNRMS[det-1]<<endm;
      gMessMgr->Info() <<"   Total hits removed by Status          = "<<mNSTATUS[det-1]<<endm;
      gMessMgr->Info() <<"   Total number of hits saved            = "<<mNOK[det-1]<<endm;
      gMessMgr->Info() <<"   Total ADCSUM of valid hits            = "<<mADCSUM[det-1]<<endm;
      gMessMgr->Info() <<"   Total Energy of valid hits            = "<<mTOTALE[det-1]<<endm;
}    
//_____________________________________________________________________________
/*!
  Get BEMC ADC Value from StEmcRawData
*/
Int_t StBemcRaw::getBemcADCRaw(Int_t det, Int_t softId, StEmcRawData* RAW, Int_t& CRATE, Int_t& CAP)
{
  CAP = 0;
  CRATE = 0;
  if(!RAW) {gMessMgr->Warning() <<"Could not find StEmcRawData pointer for BEMC "<<endm; return 0; }
  if(!mDecoder) {gMessMgr->Warning() <<"Could not find StEmcDecoderoder pointer "<<endm; return 0; }
  if(det==BTOW) // tower
  {
    Int_t daq;
    if(mDecoder->GetDaqIdFromTowerId(softId,daq)==1 && RAW->header(BTOWBANK)) 
    {
      Int_t CR,INDEX;
      mDecoder->GetTowerCrateFromDaqId(daq,CR,INDEX);
      CRATE = CR;
      CAP = 0;
      return RAW->data(BTOWBANK,daq);    
    }
    return 0;
  }
  else if(det==BPRS) // PSD
  {
    Int_t RDO=0,index=0;
    Int_t S = mDecoder->GetPsdRDO(softId,RDO,index);
    CRATE = RDO+1;
    //cout <<det<<"  "<<RDO<<" "<<softId<<"   "<<RAW->header(RDO+BPRSOFFSET)<<endl;
    if(RDO<0 || RDO>=MAXBPRSCRATES) return 0;
    if(S==1 && RAW->header(RDO+BPRSOFFSET))     
    {
      CAP = RAW->header(RDO+BPRSOFFSET,SMDCAPACITOR);
      return RAW->data(RDO+BPRSOFFSET,index); 
    }
    return 0;
  }
  else if(det==BSMDE) // SMDE
  {
    StEmcGeom *geo = StEmcGeom::instance("bsmde");
    Int_t m=0,e=0,s=0;
    if(geo->getBin(softId,m,e,s)==1) return 0;
    Int_t RDO=0,index=0;
    Int_t S = mDecoder->GetSmdRDO(BSMDE,m,e,s,RDO,index);
    CRATE = RDO+1;
    if(S==1 && RAW->header(RDO+BSMDOFFSET) && RDO>=0 && RDO<MAXSMDCRATES) 
    {
      CAP = RAW->header(RDO+BSMDOFFSET,SMDCAPACITOR);
      return RAW->data(RDO+BSMDOFFSET,index); 
    }
    return 0;
  }
  else if(det==BSMDP) // SMDP
  {
    StEmcGeom *geo = StEmcGeom::instance("bsmdp");
    Int_t m=0,e=0,s=0;
    if(geo->getBin(softId,m,e,s)==1) return 0;
    Int_t RDO=0,index=0;
    Int_t S = mDecoder->GetSmdRDO(BSMDP,m,e,s,RDO,index);
    CRATE = RDO+1;
    if(S==1 && RAW->header(RDO+BSMDOFFSET) && RDO>=0 && RDO<MAXSMDCRATES)     
    {
      CAP = RAW->header(RDO+BSMDOFFSET,SMDCAPACITOR);
      return RAW->data(RDO+BSMDOFFSET,index); 
    }
    return 0;
  }
  return 0;
}
//_____________________________________________________________________________
/*!
  Construct the StEmcRawHit. Checks if the hit is going to be saved, subtract
  pedestal and apply the calibration, if this is the case.
*/
Int_t StBemcRaw::makeHit(StEmcCollection* emc, Int_t det, Int_t id, Int_t ADC, Int_t CRATE, Int_t CAP,Float_t& E)
{    
  E=0;
    
  if(CRATE>0 && CRATE<=MAXCRATES && mControlADCtoE->CheckCrate[det-1]==1) 
    if((mCrateStatus[det-1][CRATE-1]!=crateOK && 
        mCrateStatus[det-1][CRATE-1]!=crateUnknown) && 
        !mSaveAllStEvent) return kCrate;
  
  if(ADC==0 && !mSaveAllStEvent) return kZero;
  
  if(mControlADCtoE->CheckStatus[det-1]==1)
  {
    Int_t STATUS;
    mTables->getStatus(det,id,STATUS);
    if(STATUS!=STATUS_OK && !mSaveAllStEvent) return kStatus;
  }
        
  Float_t PEDESTAL = 0,RMS = 0;
  if(mControlADCtoE->DeductPedestal[det-1]>0) 
  {
    mTables->getPedestal(det,id,CAP,PEDESTAL,RMS);    
    // do not consider hits wih capacitor number CAP1 and CAP2 for
    // PSD and SMD as valid hits
    if(det>=BPRS && !mSaveAllStEvent) if(CAP==CAP1 || CAP==CAP2) return kPed;
  }
 
  if(mControlADCtoE->CutOffType[det-1]==1 && !mSaveAllStEvent) // pedestal cut
  {
    if(RMS<=0) return kRms;
    Float_t x = (ADC-PEDESTAL)/RMS;
    if(x<=mControlADCtoE->CutOff[det-1]) return kPed;
  }

  if(mControlADCtoE->Calibration[det-1]==1)
  {
    Float_t ADCP = 1;
    Float_t C;
    for(Int_t i=0;i<5;i++)
    {
      mTables->getCalib(det,id,i,C);
      E+=ADCP*C;
      ADCP*=(Float_t)(ADC-PEDESTAL);
    }          
    mTables->getGain(det,id,C);
    E*=C;
  
    if(mControlADCtoE->CutOffType[det-1]==2 && !mSaveAllStEvent) // energy cut
    {
      if(E<mControlADCtoE->CutOff[det-1]) return kEn;
    }
  }
  
  if(mControlADCtoE->OnlyCalibrated[det-1]>0 && E==0 && !mSaveAllStEvent) return kCalib;
  
  StDetectorId did = static_cast<StDetectorId>(det+kBarrelEmcTowerId-1);
  StEmcDetector* detector=emc->detector(did);
  StEmcGeom *geo = StEmcGeom::instance(det);
  if(!detector)
  {
    detector = new StEmcDetector(did,BEMCMODULES); 
    emc->setDetector(detector);
  }
  Int_t m,e,s;
  geo->getBin(id,m,e,s);
  StEmcRawHit* hit=new StEmcRawHit(did,m,e,s,(UInt_t)ADC);
  if(mBarrelQAHisto[det-1] && !isCorrupted(det)) mBarrelQAHisto[det-1]->Fill((Float_t)id,(Float_t)ADC);
  hit->setEnergy(E);
  hit->setCalibrationType(CAP);
  detector->addHit(hit);
  return kOK;  
}
