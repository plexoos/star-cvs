/***************************************************************************
 * $id: EMC Barrel Tower daq reader.
 * Author: Subhasis and Herbert Ward
 ***************************************************************************
 *  Navigates through pointers and Fills Barrel Structs
 *
 **************************************************************************/

#include "EMC_BarrelReader.hh"
#include <assert.h>
#define MAX_ADC 0xFFF

EMC_BarrelReader::EMC_BarrelReader(EventReader *er,Bank_EMCP *pEMCP):pBankEMCP(pEMCP),ercpy(er)
{
  cout<<"ctor EMC_Barrelreader**"<<endl;
}


void EMC_BarrelReader::Initialize()
{
  mTheTowerAdcR.NTowerHits=0;
  for(int i=0;i<4800;i++)
  {
   mTheTowerAdcR.TowerADCArray[i]=0;
   mTheTowerAdcD.TowerADCArray[i]=0;
   mTheTowerPedR.TowerADCArray[i]=0;
   mTheTowerRMSR.TowerADCArray[i]=0; 
  }

  // Initialize TOWERDATA array to 0's
  for(int i = 0 ; i <120 ; i++) {
    for(int j = 0 ; j < 20; j++) {
      for(int k = 0 ; k < 2; k++) {
        mTheTowerAdcR.TowerMatrix[i][j][k] = 0;
        mTheTowerAdcD.TowerMatrix[i][j][k] = 0;
        mTheTowerPedR.TowerMatrix[i][j][k] = 0;
        mTheTowerRMSR.TowerMatrix[i][j][k] = 0;
      }
    }
  }
}

////////////////////////////////////////////////////////////
// conversion from FEE Tower number to m,e,s for Tower

int EMC_BarrelReader::getTowerBin(const int rid,int &m,int &e,int &s)
{
  //Transiion from environment Id to m,e,s to fill the arrays
  // Copy of StEmcGeom version

  if(checkTowerId(rid) == 0) return 0;
 
  int idw;
  m   = (rid - 1) / 40 + 1; // Module number
  idw = (rid - 1) % 40;     // change from 0 to 39
  s   = idw/20 + 1;
  e   = 20 - idw%20;
  return 1;                   // zero is bad
}

int EMC_BarrelReader::checkTowerId(const int rid)
{
  int MaxRaw=4800;
  if(rid>=1 && rid<=MaxRaw) return 1; 
  else cout<<" Bad TOWER raw# "<<rid<<"Max  "<<MaxRaw<<endl; return 0;
}

////////////////////////////////////////////////////////////////////

  Bank_EMCSECP* EMC_BarrelReader::getBarrelSection(const Bank_EMCP* pBankEMCP,int section)
{

  if((!pBankEMCP->EMCSecPointer[section].offset) ||
     (!pBankEMCP->EMCSecPointer[section].length))
  {
        char str0[40];
        sprintf(str0,"getBarrelSection(hs %d)",section);
        ercpy->fprintError(INFO_MISSING_BANK,__FILE__,__LINE__,str0);
    cout<<" getBarrelSection** , no offset or length for section**"<<section<<endl;
    return NULL;
  } 


Bank_EMCSECP * ptr=(Bank_EMCSECP*)(((INT32 *)pBankEMCP)+pBankEMCP->EMCSecPointer[0].offset); 
 
//put some checks
  if(strncmp(ptr->header.BankType,"EMCSECP",7)) {
    char str0[40];
    cout<<" error in header name**"<<endl;
        sprintf(str0,"getBarrelsection(hs )");
        ercpy->fprintError(ERR_BAD_HEADER,__FILE__,__LINE__, str0); return NULL;
  }
   if(!ptr->test_CRC()) {
    char str0[40];
    cout<<"error in CRC**"<<endl;
        sprintf(str0,"getBarrelsection(hs )");
        ercpy->fprintError(ERR_CRC,__FILE__,__LINE__,str0); return NULL;
  }
  printf("Byte order of header for EMCSECP Before swap**: %x\n",ptr->header.ByteOrder);
           if(ptr->swap() < 0) {
    char str0[40];
    cout<<"error in swap**"<<endl;
        sprintf(str0,"getBarrelsection(hs)");
        ercpy->fprintError(ERR_SWAP,__FILE__,__LINE__,str0); return NULL;
  }

   ptr->header.CRC = 0;
   return ptr;
}

  Bank_EMCRBP* EMC_BarrelReader::getBarrelTowerFiber(Bank_EMCSECP* secp,int section)
{

  if((!secp->FiberPointer[section].offset) ||
     (!secp->FiberPointer[section].length))
  {
        char str0[40];
        sprintf(str0,"getBarrelTowerFiber(hs %d)",section);
        ercpy->fprintError(INFO_MISSING_BANK,__FILE__,__LINE__,str0);
    cout<<" getBarrelSection** , no offset or length for section**"<<section<<endl;
    return NULL;
  } 

 Bank_EMCRBP* ptr =(Bank_EMCRBP*)(((INT32 *)secp)+secp->FiberPointer[section].offset); 
 
  // some checks and swap

  if(strncmp(ptr->header.BankType,"EMCRBP",6)) {
    char str0[40];
    cout<<" error in header name**"<<endl;
        sprintf(str0,"getBarrelTowerFiber(hs %d)",section);
        ercpy->fprintError(ERR_BAD_HEADER,__FILE__,__LINE__, str0); return NULL;
  }
   if(!ptr->test_CRC()) {
    char str0[40];
    cout<<"error in CRC**"<<endl;
        sprintf(str0,"getBarrelTowerFiber(hs %d)",section);
        ercpy->fprintError(ERR_CRC,__FILE__,__LINE__,str0); return NULL;
  }
           if(ptr->swap() < 0) {
    char str0[40];
    cout<<"error in swap**"<<endl;
        sprintf(str0,"getBarrelTowerFiber(hs %d)",section);
        ercpy->fprintError(ERR_SWAP,__FILE__,__LINE__,str0); return NULL;
  }

   ptr->header.CRC = 0;
   return ptr;
}



  Bank_TOWERADCR* EMC_BarrelReader::getBarrelADC(Bank_EMCRBP* rbp)
{
  if((!rbp->EMCADCR.offset) ||
     (!rbp->EMCADCR.length))
  {
        char str0[40];
        sprintf(str0,"getBarrelADC(hs)");
        ercpy->fprintError(INFO_MISSING_BANK,__FILE__,__LINE__,str0);
    cout<<" getBarrelADC** , no offset or length for ADCR**"<<endl;
    return NULL;
  } 


 Bank_TOWERADCR* pADCR =(Bank_TOWERADCR*)(((INT32 *)rbp)+rbp->EMCADCR.offset); 

  // some checks and swap

  if(strncmp(pADCR->header.BankType,"EMCADCR",7)) {
    char str0[40];
    cout<<" error in header name**"<<endl;
        sprintf(str0,"getBarrelADC(hs)");
        ercpy->fprintError(ERR_BAD_HEADER,__FILE__,__LINE__, str0); return NULL;
  }
   if(!pADCR->test_CRC()) {
    char str0[40];
    cout<<"error in CRC**"<<endl;
        sprintf(str0,"getBarrelADC(hs)");
        ercpy->fprintError(ERR_CRC,__FILE__,__LINE__,str0); return NULL;
  }
           if(pADCR->swap() < 0) {
    char str0[40];
    cout<<"error in swap**"<<endl;
        sprintf(str0,"getBarrelADC(hs)");
        ercpy->fprintError(ERR_SWAP,__FILE__,__LINE__,str0); return NULL;
  }

   pADCR->header.CRC = 0; 
return pADCR;
}



   int EMC_BarrelReader::FillBarrelTower(Bank_TOWERADCR* pADCR)
{

 /*
 //Bank Header
 for(int i=0;i<10;i++){
   cout<<"Bankheader "<<i<<" "<<pADCR->bankHeader[i]<<endl;
 }
 */

 //fiber header
 for(int i=0;i<64;i++){
//  printf("Fiber header**: %x\n",pADCR->fiberHeader[i]);
  //   cout<<"Fiberheader "<<i<<" "<<pADCR->fiberHeader[i]<<endl;
   //   cout<<"fiberheader "<<i<<" "<<pADCR->fiberHeader[i]<<endl;
 }
 for(int i=0;i<120;i++){
//  printf("Fiber header**: %x\n",pADCR->TDCHeader[i]);
//  cout<<"TDC header**:"<<pADCR->TDCHeader[i]<<endl;
 }
 //
 // Fiber data and fill the struct

  mTheTowerAdcR.BankType="TOWRADCR\n";
  mTheTowerAdcR.DetFlag=1; //Detector flag for BEMC=1,
  mTheTowerAdcR.EventNumber=pADCR->header.Token; //Token number
  mTheTowerAdcR.PedFlag=0; //Pedestal subtracted (1) or not (0)
  mTheTowerAdcR.TDCErrorFlag=0; // Error from TDC (0=good)

 //fiberdata
 for(int i1=0;i1<160;i1++){
//   cout<<"***********ADC"<<i1<<"******"<<endl;
 for(int i=0;i<30;i++){
   int index=i1*30+i;
    //Fill towerrawdata struct
   mTheTowerAdcR.TowerADCArray[index]=pADCR->fiberData[index];
//         if(i<1)cout<<"index "<<index<<" data  "<<pADCR->fiberData[index]<<endl;
   if(pADCR->fiberData[index]>0)
    {mTheTowerAdcR.NTowerHits++;} //Total number of channels
//   int index_jose=i*160+i1;
//
  int index_jose=-1;
   int stat_index=get_index_jose(index,index_jose);

// Index-jose runs here from 0 to 4799

   int m=0,e=0,s=0;
//  int binstat=getTowerBin(index+1,m,e,s);
  if(stat_index){
   int binstat=getTowerBin(index_jose+1,m,e,s);
   if(!binstat)
    {
    cout<<" problem in bin conversion "<<index<<endl;
    }
   else
   {
   mTheTowerAdcR.TowerMatrix[m-1][e-1][s-1]=pADCR->fiberData[index];
//         if(i<5 && i1<10)cout<<"Channel no"<<i+1<<"m "<<m<<"e "<<e<<"s "<<s<<" data  "<<pADCR->fiberData[index]<<endl;
   }
   mTheTowerAdcR.TowerADCArray[index]=pADCR->fiberData[index];
   }//stat_index

  } 
 }
 return 1;
}



 int EMC_BarrelReader::ProcessBarrelTower(const Bank_EMCP* EmcPTR)
   {
  // First Barrel Tower
    Bank_EMCSECP* barreltower=getBarrelSection(EmcPTR,0);

  if(barreltower){
    Bank_EMCRBP* towerfiber=getBarrelTowerFiber(barreltower,0);

  if(towerfiber){
      Bank_TOWERADCR* toweradc=getBarrelADC(towerfiber);

      if(toweradc){
         int fillstat= FillBarrelTower(toweradc);
      }
      else{
	cout<<" ADCR absent , looking for ADCD"<<endl;
      }
      toweradc=0;
  }
  else{
    cout<<" BANK_EMRBP absent**"<<endl;
    return 0;
  }
  }
  else{
    cout<<" BANK_EMCP absent**"<<endl;
    return 0;
  }
  return 1;
   }


void EMC_BarrelReader::PrintTowerArray()
{

  cout<<"BankType **"<<mTheTowerAdcR.BankType<<endl;
  cout<<"Det flag **"<<mTheTowerAdcR.DetFlag<<endl;
  cout<<"Event no**"<<mTheTowerAdcR.EventNumber<<endl;
  cout<<"Pedflag**"<<mTheTowerAdcR.PedFlag<<endl;
  cout<<"TDCERR **"<<mTheTowerAdcR.TDCErrorFlag<<endl;
  for(int i=0;i<120;i++){
    for(int j=0;j<20;j++){
      for(int k=0;k<2;k++){
	if(mTheTowerAdcR.TowerMatrix[i][j][k]>0)cout<<"ADCDATA** mod"<<i+1<<"eta "<<j+1<<"phi "<<k+1<<"ADC "<<mTheTowerAdcR.TowerMatrix[i][j][k]<<endl;
      }
    }
  }
}

Bank_BTOWERADCR& EMC_BarrelReader::getBTOWERADCR()
 {
   return mTheTowerAdcR;
 }


   int EMC_BarrelReader::get_index_jose(int daq_tower,int& index_jose)
{
 
int Init_Crate[15]={2180,2020,1860,1700,1540,1380,1220,1060,900,740,580,420,260,100,2340};
int TDC2Crate[]= {3,2,1,15,14,0,0,0,0,0,0,0,0,0,0};
 
int crate_seq=0;
int Crate=-1;
if(daq_tower<0 || daq_tower>4799)return 0;
 
int tdc=GetCrate(daq_tower,crate_seq);
if(tdc<1 || tdc>15)return 0;

if(tdc>0 && tdc<15)Crate=TDC2Crate[tdc-1];
 
if(Crate<15 && Crate>=0){
int start=Init_Crate[Crate];
int index_jose=Getjose_tower(start,crate_seq);
}
else
{
return 0;
}
 
if(Crate==0)cout<<"daq_tower  "<<daq_tower<<"crate "<<Crate<<"jose_tower "<<index_jose<<endl;
return 1;
}
 
 
int EMC_BarrelReader::GetCrate(int daq_tower,int& crate_seq)
 
{
 
int Crate=daq_tower%30;
crate_seq=daq_tower/30;
//if(Crate>14)Crate-=15;
return Crate;
 
}
 
int EMC_BarrelReader::Getjose_tower(int start,int& crate_seq)
 
{
 
int card=crate_seq/32;
int card_seq=31-(crate_seq%32);
if(start==2181)cout<<"card  "<<card<<"card_seq  "<<card_seq<<endl;
int channel_seq=card_seq/4;
int channel=card_seq-(channel_seq*4)+1;
int jose_tower=start+channel_seq*20+card*4+channel;
if(jose_tower>2400)jose_tower-=2400;
return jose_tower;
}                                                                                              
