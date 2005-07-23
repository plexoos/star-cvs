 /***************************************************************************
 *
 * $Id: StSvtSimulationMaker.cxx,v 1.28 2005/07/23 03:37:34 perev Exp $
 *
 * Author: Selemon Bekele
 ***************************************************************************
 *
 * Description: Svt Slow Simulator Maker class
 *
 ***************************************************************************
  * Revision 1.23  2004/04/08 15:11:27  caines
 * Ensure array is initialised to zeros
 *
 * Revision 1.22  2004/04/06 20:18:19  caines
 * Initialise variable counter in the constructor to NULL to avoid crash
 *
 * Revision 1.21  2004/03/30 21:27:12  caines
 * Remove asserts from code so doesnt crash if doesnt get parameters it just quits with kStErr
 *
 * $Log: StSvtSimulationMaker.cxx,v $
 * Revision 1.28  2005/07/23 03:37:34  perev
 * IdTruth + Cleanup
 *
 * Revision 1.26  2005/02/09 14:33:35  caines
 * New electron expansion routine
 *
 * Revision 1.20  2004/02/24 15:53:22  caines
 * Read all params from database
 *
 * Revision 1.19  2004/01/27 02:45:42  perev
 * LeakOff
 *
 * Revision 1.18  2004/01/22 16:30:47  caines
 * Getting closer to a final simulation
 *
 * Revision 1.17  2003/11/30 20:51:48  caines
 * New version of embedding maker and make OnlSeqAdj a stand alone maker
 *
 * Revision 1.16  2003/11/15 20:24:29  caines
 * fixes to remove warnings at compilation
 *
 * Revision 1.15  2003/11/14 17:33:19  caines
 * DOnt read from pedmaker for now
 *
 * Revision 1.14  2003/11/13 16:24:59  caines
 * Further improvements to get simulator looking like reality
 *
 * Revision 1.12  2003/07/31 19:18:10  caines
 * Petrs improved simulation code
 *
 * Revision 1.10  2001/11/06 20:12:06  caines
 * Add include for new compiler
 *
 * Revision 1.9  2001/08/13 15:34:18  bekele
 * Debugging tools added
 *
 * Revision 1.7  2001/04/12 20:34:54  caines
 * Add check for if nosvt hits present
 *
 * Revision 1.6  2001/04/03 15:24:24  caines
 * Increase hit space size again
 *
 * Revision 1.5  2001/03/19 22:25:53  caines
 * Catch wrong wafer ids more elegantly
 *
 * Revision 1.4  2001/03/15 15:12:09  bekele
 * added a method to fill the whole SVT hybrid with background
 *
 * Revision 1.3  2001/02/18 00:10:42  caines
 * Improve and use StSvtConfig
 *
 * Revision 1.2  2001/02/07 19:13:51  caines
 * Small fixes to allow to run without setup from command line
 *
 * Revision 1.1  2000/11/30 20:47:49  caines
 * First version of Slow Simulator - S. Bekele
 *
 **************************************************************************/


#include <string>
#include <cmath>
using namespace std;

#include "St_DataSetIter.h"
#include "St_ObjectSet.h"
#include "StMessMgr.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"

#include "TFile.h"
#include "TDirectory.h"
#include "TNtuple.h"

#include "StSequence.hh"
#include "StDbUtilities/StSvtCoordinateTransform.hh"
#include "StDbUtilities/StGlobalCoordinate.hh"
#include "StDbUtilities/StSvtLocalCoordinate.hh"
#include "StDbUtilities/StSvtWaferCoordinate.hh"
//#include "StSvtClassLibrary/StSvtHybridCollection.hh"
#include "StSvtClassLibrary/StSvtHybridData.hh"
#include "StSvtClassLibrary/StSvtData.hh"
#include "StSvtClassLibrary/StSvtHybridPixelsD.hh"
#include "StSvtClassLibrary/StSvtHybridPixelsC.hh"
#include "StSvtClassLibrary/StSvtConfig.hh"
#include "StSvtClassLibrary/StSvtGeometry.hh"
#include "StSvtClassLibrary/StSvtWaferGeometry.hh"
#include "StSvtClassLibrary/StSvtT0.hh"
#include "StSvtClassLibrary/StSvtHybridDriftVelocity.hh"
#include "StSvtClassLibrary/StSvtHybridPed.hh"
#include "StSvtClassLibrary/StSvtDaq.hh"
#include "StSvtDaqMaker/StSvtHybridDaqPed.hh"
#include "StSvtCalibMaker/StSvtPedMaker.h"
#include "StSvtAngles.hh"
#include "StSvtElectronCloud.hh"
#include "StSvtSignal.hh"
#include "StSvtSimulation.hh"
#include "StSvtGeantHits.hh"

#include "tables/St_g2t_svt_hit_Table.h"
//#include "StSvtConversionTable.h"

#include "StSvtSimulationMaker.h"

ClassImp(StSvtSimulationMaker)

  /*! hardvired constants
   * mTimeBinSize = 0.04;  // Micro Secs -  this is quite accurate according to Dave Lynn
   * mDefaultDriftVelocity = 1.E-5*675000;  // [mm/us] used only if there is no database
   * mDiffusionConst=0.0035;   // [mm**2/micro seconds] default=0.0035 (for silicon)
   * mLifeTime=1000000.0;     // [us]   default =1000000.0
   * mTrapConst=4.0e-5;       // [us]   default =0
  */

#define cTimeBinSize 0.04
#define cDefaultDriftVelocity 1.E-5*675000
#define cDiffusionConst 0.0035
#define cLifeTime 1000000.0
#define cTrapConst 4.0e-5
//___________________________________________________________________________
/// the only place where electron cloud expansioin constants are set
StSvtSimulationMaker::StSvtSimulationMaker(const char *name):StMaker(name)
{ 
   if (Debug()) gMessMgr->Info() << "StSvtSimulationMaker::constructor"<<endm;
  //electron cloud settings - these can be tuned;
  mDiffusionConst= cDiffusionConst;   // [mm**2/micro seconds] default=0.0035 (for silicon)
  mLifeTime=cLifeTime;     // [us]   //default =1000000.0
  mTrapConst=cTrapConst;       // [us]   //default =0

  //options - can be set by setOptions
  mSigOption = 0;          // use both PASA codes
  
  
  //initial cleanup
  mNumOfHybrids = 0; 

  mSvtGeantHitColl = NULL;
  mSvtSimPixelColl = NULL;
  mSvt8bitPixelColl = NULL; //final data 
  mDriftSpeedColl=NULL;
  
  mCoordTransform = NULL;

  counter = NULL;
  if (Debug()) gMessMgr->Info() << "StSvtSimulationMaker::constructor...END"<<endm;
}

//____________________________________________________________________________
StSvtSimulationMaker::~StSvtSimulationMaker()
{
  if (Debug()) gMessMgr->Info() << "StSvtSimulationMaker::destructor"<<endm;

  delete mSvtAngles;
  delete mSvtSimulation;
  delete mElectronCloud;
  delete mCoordTransform;

  if (Debug()) gMessMgr->Info() << "StSvtSimulationMaker::destructor...END"<<endm; 
}

//____________________________________________________________________________

Int_t StSvtSimulationMaker::setConst(double timBinSize, double anodeSize)
{
  mTimeBinSize = timBinSize ;
  mAnodeSize = anodeSize;    
  return kStOK;
}


//____________________________________________________________________________

void StSvtSimulationMaker::setElectronLifeTime(double tLife)
{
  mLifeTime = tLife;  //  [micro seconds] 

}
//____________________________________________________________________________
void StSvtSimulationMaker::setTrappingConst(double trapConst)
{
  mTrapConst = trapConst;  //  [micro seconds] 

}

//____________________________________________________________________________
void StSvtSimulationMaker::setDiffusionConst(double diffConst)
{
  mDiffusionConst = diffConst;  
}

//____________________________________________________________________________
//mainly for debugging
Int_t StSvtSimulationMaker::setOptions(int SigOption)
{
  mSigOption = SigOption;

  return kStOK;
}


//____________________________________________________________________________
Int_t StSvtSimulationMaker::Init()
{
      
  if(Debug()) gMessMgr->Info() << "In StSvtSimulationMaker::Init() ..."<<endm;
    
  // init objects that do parts of simulation
  mSvtAngles =  new StSvtAngles();
 
  mElectronCloud = new StSvtElectronCloud();
  mElectronCloud->setElectronLifeTime(mLifeTime);
  mElectronCloud->setDiffusionConst(mDiffusionConst);
  mElectronCloud->setTrappingConst(mTrapConst);

  mSvtSimulation = new StSvtSimulation();
  mSvtSimulation->setOptions(mSigOption);
  mSvtSimulation->setElCloud(mElectronCloud);
 
  mCoordTransform=new StSvtCoordinateTransform();  
   
  if(Debug()) gMessMgr->Info() << "In StSvtSimulationMaker::Init() -End"<<endm;

  return  StMaker::Init();
}

//__________________________________________________________________________
///all database dependent data are read here 
Int_t StSvtSimulationMaker::InitRun(int runumber)
{ //when the run changes
  if(Debug()) gMessMgr->Info() <<"StSvtSimulationMaker::InitRun()"<<endm;
  
  //read from database
  Int_t res;
  getConfig();
  if ((res=getSvtGeometry())!=kStOk) return res;
  if ((res=getSvtDriftSpeeds())!=kStOk) return res;
  if ((res=getSvtT0())!=kStOk) return res;
  if ((res=getPedestalOffset())!=kStOk) return res; 

  setSvtPixelData();
  //Set up coordinate transformation 
  mCoordTransform->setParamPointers(mSvtGeom, mConfig,mDriftSpeedColl,mT0);
  
  //mTimeBinSize = 1.E6/mSvtSrsPar->fsca;  // Micro Secs
  //mAnodeSize = mSvtSrsPar->pitch*10;  // mm
  // *****values hard wired for the time being - should be in database
  mTimeBinSize = cTimeBinSize;  // Micro Secs - Petr: this is quite accurate according to Dave
  mAnodeSize = mSvtGeom->getAnodePitch()*10;  // mm
  mDefaultDriftVelocity = cDefaultDriftVelocity;  // used only if there is no database
  
  //set default drift speeds - if drift speed data exist it will be overriden later
  mSvtSimulation->setAnodeTimeBinSizes(mTimeBinSize , mAnodeSize);
  mSvtSimulation->setDriftVelocity(mDefaultDriftVelocity);
 
  //set size of hits-otherwise default is false,8
  //mSvtSimulation->setPasaSigAttributes(kFALSE,8)


  cout<<"StSvtSimulationMaker::InitRun info:"<<endl;
  cout<<"  Anode size="<<mAnodeSize<<" ,time bin size="<<mTimeBinSize<<endl;
  cout<<"  default drift velocity="<<mDefaultDriftVelocity<<endl;
  cout<<"  pedestal offset(from database)="<<mPedOffset<<endl;
  cout<<"  T0(from database)= "<<mT0->getT0(1)<<endl;
  

  if(Debug()) gMessMgr->Info()<<"StSvtSimulationMaker::InitRun()-END"<<endm;
 
  return StMaker::InitRun(runumber);
}

//____________________________________________________________________________

Int_t  StSvtSimulationMaker:: FinishRun(int oldrunumber){
  gMessMgr->Info()<<"StSvtSimulationMaker::FinishRun()"<<endm;
 
  TDataSet *set;
  if ((set=GetDataSet("StSvtPixelData"))) delete set;
  if ((set=GetDataSet("StSvt8bitPixelData"))) delete set;   
 
  gMessMgr->Info()<<"StSvtSimulationMaker::FinishRun() - END"<<endm;
  return StMaker::FinishRun(oldrunumber);
}


//____________________________________________________________________________

void  StSvtSimulationMaker::resetPixelData(){
  //this resets mSvtSimPixelColl and mSvt8bitPixelColl
 
   StSvtHybridPixelsD* tmpPixels;
   StSvtHybridPixelsC* tmp8bitPixels;

   for(int Barrel = 1;Barrel <= mSvtSimPixelColl->getNumberOfBarrels();Barrel++) {
     for (int Ladder = 1;Ladder <= mSvtSimPixelColl->getNumberOfLadders(Barrel);Ladder++) {
       for (int Wafer = 1;Wafer <= mSvtSimPixelColl->getNumberOfWafers(Barrel);Wafer++) {
         for( int Hybrid = 1;Hybrid <= mSvtSimPixelColl->getNumberOfHybrids();Hybrid++){
           
           int index = mSvtSimPixelColl->getHybridIndex(Barrel,Ladder,Wafer,Hybrid);
           if( index < 0) continue; 
           
           tmpPixels  = (StSvtHybridPixelsD*)mSvtSimPixelColl->at(index);
           tmp8bitPixels = (StSvtHybridPixelsC*)mSvt8bitPixelColl->at(index);

           if(!tmpPixels) {
             tmpPixels = new StSvtHybridPixelsD(Barrel, Ladder, Wafer, Hybrid);
             mSvtSimPixelColl->put_at(tmpPixels,index);
           }
	   if(!tmp8bitPixels) {
             tmp8bitPixels = new StSvtHybridPixelsC(Barrel, Ladder, Wafer, Hybrid);
             mSvt8bitPixelColl->put_at(tmp8bitPixels,index);
           }

           tmpPixels->setPedOffset(mPedOffset);
           tmpPixels->reset();

         }
       }
     }
   }
}


//____________________________________________________________________________
///create output data and put them into the chain
void  StSvtSimulationMaker::setSvtPixelData()
{ //add pixeldata to chain->.data
  if (GetDataSet("StSvtPixelData")) cout<<"Error: Found StSvtSimPIxels in the chain - should have been deleted"<<endl;
     
  St_ObjectSet *set = new St_ObjectSet("StSvtPixelData");
  AddConst(set); 
  mSvtSimPixelColl = new StSvtData(mConfig->getConfiguration());
  set->SetObject((TObject*)mSvtSimPixelColl);
  
  set = new St_ObjectSet("StSvt8bitPixelData");
  AddConst(set); 
  mSvt8bitPixelColl = new StSvtData(mConfig->getConfiguration());
  set->SetObject((TObject*)mSvt8bitPixelColl);

  mNumOfHybrids = mSvtSimPixelColl->getTotalNumberOfHybrids(); 
}


//__________________________________________________________________________________________________
void  StSvtSimulationMaker::setGeantData()
{
  St_ObjectSet* set=(St_ObjectSet*)GetDataSet("StSvtGeantHits");

  if (set) {
    cout<<"Found StSvtGeantHits in the chain- replacing"<<endl;
    set->SetObject(0);
  } 
  else{
    set =  new St_ObjectSet("StSvtGeantHits");
    AddData(set);
  }

  if (mSvtGeantHitColl) cout<<"!!!!!!m SvtGeantHitColl already exists in SvtSimulationMaker.cxx:setEval"<<endl;
  else{
    //owned by the SvtData
    mSvtGeantHitColl = new StSvtData(mConfig->getConfiguration());
    set->SetObject((TObject*)mSvtGeantHitColl);
  }

//+++++++++++++++++
//for debugging purposes
  if(!counter){
    counter = new int[mNumOfHybrids];
    for( int ii=0; ii<mNumOfHybrids; ii++){
      counter[ii] = 0;
    }
  }
}

//__________________________________________________________________________________________________
Int_t  StSvtSimulationMaker::getSvtGeometry()
{
    
  St_DataSet* dataSet;
  dataSet = GetDataSet("StSvtGeometry");
  if (!dataSet){
    gMessMgr->Error()<<"BIG TROUBLE:No SVT geometry -impossible to run!!!!"<<endm;
    return kStFatal;
    }
 

  mSvtGeom = (StSvtGeometry*)dataSet->GetObject();
  if (!mSvtGeom){
    gMessMgr->Error()<<"BIG TROUBLE:No SVT geometry -impossible to run!!!!"<<endm;
    return kStFatal;
    }
  
  
  return kStOk;
}

//__________________________________________________________________________________________________
Int_t  StSvtSimulationMaker::getPedestalOffset()
{
    
  St_DataSet* dataSet;
  dataSet = GetDataSet("StSvtDaq");
  if (dataSet==NULL){
    gMessMgr->Error()<<"BIG TROUBLE:No DAQ parameters for SVT!!!!"<<endm;
    return kStFatal;
  }
 

  StSvtDaq *daq = (StSvtDaq*)dataSet->GetObject();
  if (daq==NULL){
    gMessMgr->Error()<<"BIG TROUBLE:No DAQ parameters are empty!!!!"<<endm;
    return kStFatal;
    }


  mPedOffset=daq->getPedOffset();

  return kStOk;
}

//____________________________________________________________________________
Int_t StSvtSimulationMaker::getSvtDriftSpeeds()
{
  mDriftSpeedColl =NULL;
  St_DataSet* dataSet;
  dataSet = GetDataSet("StSvtDriftVelocity");
  if (!dataSet){
    cout<<"Warning: no SVT drift velocity data available - using default drift speed:"<<mDefaultDriftVelocity<<endl;
    return kStErr;
  } //this might be obsolete, maybe it's better to give an error instead of running on

  mDriftSpeedColl = (StSvtHybridCollection*)dataSet->GetObject();
  if (! mDriftSpeedColl) cout<<"Warning: SVT drift velocity data empty - using default drift speed:"<<mDefaultDriftVelocity<<endl;
    
  return kStOk;
}


//____________________________________________________________________________
Int_t StSvtSimulationMaker::getSvtT0()
{
  mT0=NULL;
  St_DataSet* dataSet;
  dataSet = GetDataSet("StSvtT0");
  if (!dataSet){
    cout<<"Warning: no SVT T0 data available -using defalt T0 = 0"<<endl;
    return kStErr;
  } //this might be obsolete, maybe it's better to give an error instead of running on
  
  mT0 = (StSvtT0*)dataSet->GetObject();
  if (! mT0) cout<<"Warning: SVT T0 data empty - using default T0 = 0"<<endl;
  
  return kStOk;
}

//____________________________________________________________________________
Int_t StSvtSimulationMaker::getConfig()
{
  mConfig=NULL;
  St_DataSet *dataSet = NULL;
  dataSet = GetDataSet("StSvtConfig");
  
  if (!dataSet)
    {
      gMessMgr->Warning() << " No SvtConfig  data set" << endm;
      dataSet = new St_ObjectSet("StSvtConfig");                                                               
      AddConst(dataSet);
      mConfig=NULL;
    }
  
  mConfig=((StSvtConfig*)(dataSet->GetObject()));
 
  if (!mConfig) {
    gMessMgr->Warning() << "SvtConfig data set is empty- seting default full configuration" << endm;
    mConfig=new StSvtConfig();
    mConfig->setConfiguration("FULL");
    dataSet->SetObject(mConfig);
  } 

  return kStOk;
}


//____________________________________________________________________________
Int_t StSvtSimulationMaker::Make()
{
  if (Debug()) gMessMgr->Info() << "In StSvtSimulationMaker::Make()" << endm;
   
  int volId ,barrel, layer, ladder, wafer, hybrid;
  double px,py,pz;
  Int_t NumOfHitsPerHyb=0;
  StThreeVector<double> VecG(0,0,0);
  StThreeVector<double> VecL(0,0,0);

  StSvtHybridPixelsD *svtSimDataPixels;
 
  //########## initiating data structures ##########################
  resetPixelData();
  setGeantData(); //if this removed geant data need to be dealocated in Clear() 
  
  StSvtWaferCoordinate waferCoord (0,0,0,0,0,0);
  StSvtLocalCoordinate localCoord (0,0,0);
  StGlobalCoordinate globalCor(0,0,0);

  //
  //################  get geant hit table ##########################
  // 
  
  St_DataSet *g2t_svt_hit =  GetDataSet("g2t_svt_hit");
  St_DataSetIter g2t_svt_hit_it(g2t_svt_hit);
  St_g2t_svt_hit *g2t_SvtHit = (St_g2t_svt_hit *) g2t_svt_hit_it.Find("g2t_svt_hit");

  g2t_svt_hit_st *trk_st=0;
  if( !g2t_SvtHit) {
    gMessMgr->Warning() << "No SVT hits" << endm;
    NumOfHitsPerHyb = 0;
  }
  else{  
  trk_st = g2t_SvtHit->GetTable();    
  NumOfHitsPerHyb = g2t_SvtHit->GetNRows();
  }

  //
  //################  Loop over geant hits ##########################
  //
 
  cout<<"mNumOfGeantHits = "<<NumOfHitsPerHyb<<endl;
  int tmpBadCount=0;
  if (NumOfHitsPerHyb>0) 
   for (int j=0;j<NumOfHitsPerHyb ;j++)
    {
      double anode,time;
      volId = trk_st[j].volume_id;
      int trackId = trk_st[j].track_p;
      //cout <<"geant hit #"<<j<<" volumeID="<< volId << " x=" << trk_st[j].x[0] << " y=" << trk_st[j].x[1] << " z=" <<  trk_st[j].x[2]<<endl;
      //outGeantSvtGeom<< volId <<endl;
      if( volId > 7000) continue; // SSD hit
      /*
        if (int(volId/1000) == 3)
        volId = volId - 3000 + 4000;
        else if (int(volId/1000) == 4)
        volId = volId - 4000 + 3000;
      */

   
      VecG.setX( trk_st[j].x[0]);VecG.setY( trk_st[j].x[1]);VecG.setZ( trk_st[j].x[2]);
      px = trk_st[j].p[0];  py = trk_st[j].p[1];  pz = trk_st[j].p[2];
      //double energy = 96000.; 
      double  energy = trk_st[j].de*1e9; 
      globalCor.setPosition(VecG);

      mCoordTransform->operator()(globalCor,waferCoord);
      

      layer = waferCoord.layer(); ladder = waferCoord.ladder();
      wafer = waferCoord.wafer(); hybrid = waferCoord.hybrid();     
      time = waferCoord.timebucket();
      double driftTime=time - mT0->getT0();
      anode = waferCoord.anode();
      //cout<<"time pos of hit(according to CoordTransform):"<<time<<" ,anodepos:"<<anode<<endl;
    
      if(driftTime < 0.0 || time > 128.0 || anode < 0.0 || anode > 240.0)
	{ tmpBadCount++; continue;}
      
      mCoordTransform->operator()(globalCor,localCoord);
      VecL.setX(localCoord.position().x());
      VecL.setY(localCoord.position().y());
      VecL.setZ(localCoord.position().z());
      
      
      //########### get barrel and ladder numbers correctly #################
      
static const int barrels[]={3,1,1,2,2};
      barrel = 3;
      if (layer<=4) barrel = barrels[layer];

      if ( !strncmp(mConfig->getConfiguration(), "Y1L", strlen("Y1L")) ) {
        if ((wafer == 1) || (wafer == 2) || (wafer == 3))
          ladder = 2;
      }	   
        
      //if(Debug()) mNTuple->Fill(time,anode,trk_st[j].x[0],trk_st[j].x[1],trk_st[j].x[2],0 ,0,0,0,0,0);
      
      if( 1000*layer+100*wafer+ladder != volId){
        cout << "trouble - skipping hit" << volId <<"\t"<< trk_st[j].x[0] << "\t" 
             << trk_st[j].x[1] <<"\t and our calc"<<"\t" << layer << " " 
             << wafer << "\t" << ladder << "\t" << j <<endl;
        continue;
      }
      
      int index = mSvtSimPixelColl->getHybridIndex(barrel,ladder,wafer,hybrid);
      if( index < 0) continue; 
      svtSimDataPixels  = (StSvtHybridPixelsD*)mSvtSimPixelColl->at(index);
	     
      mSvtAngles->calcAngles(mSvtGeom,px,py,pz,layer,ladder,wafer);
      double theta = mSvtAngles->getTheta();
      double phi = mSvtAngles->getPhi();

      //seting drift speed for simulation
      double vd=0;
      if (mDriftSpeedColl){
        vd = ((StSvtHybridDriftVelocity*)mDriftSpeedColl->at(index))->getV3(1);
        if (vd<=0) vd=mDefaultDriftVelocity;
        else vd=vd*1e-5;
      }
      //cout<<"drift velocity used: = "<<vd<<" (default would be "<<mDefaultDriftVelocity<<")"<<endl;
     
      mSvtSimulation->setDriftVelocity(vd);
      mSvtSimulation->doCloud(driftTime,energy,theta,phi,trackId);
      mSvtSimulation->fillBuffer(anode,time,svtSimDataPixels);
           
      if (Debug()) 
	FillGeantHit(barrel,ladder,wafer,hybrid,&waferCoord,&VecG,&VecL,mSvtSimulation->getPeak(),trackId);
      
    }
  int nSimDataPixels = mSvtSimPixelColl->size();
  for (int index=0;index<nSimDataPixels;index++) {
    svtSimDataPixels  = (StSvtHybridPixelsD*)mSvtSimPixelColl->at(index);
    if (!svtSimDataPixels) continue;
    svtSimDataPixels->updateTruth();
  }
  if (Debug()) gMessMgr->Info() <<"bad hits:"<<tmpBadCount<<endm;
  if (Debug()) gMessMgr->Info() << "In StSvtSimulationMaker::Make()...END" << endm;
  return kStOK;
}


//____________________________________________________________________________
void StSvtSimulationMaker::FillGeantHit(int barrel, int ladder, int wafer, int hybrid,
                    StSvtWaferCoordinate* waferCoord,StThreeVector<double>* VecG,
		    StThreeVector<double>* VecL, double peak,int idtrk)
{ 
  StSvtGeantHits* geantHit;
  
  int index = mSvtGeantHitColl->getHybridIndex(barrel,ladder,wafer,hybrid);
  if (index < 0) return;
  
  geantHit = (StSvtGeantHits*)mSvtGeantHitColl->at(index);
  if(!geantHit) { //actually, it should be empty
    geantHit = new StSvtGeantHits(barrel,ladder,wafer,hybrid);
    mSvtGeantHitColl->put_at(geantHit,index);
  }
  
  geantHit->setGeantHit(counter[index],waferCoord);
  geantHit->setLocalCoord(counter[index],VecL);
  geantHit->setGlobalCoord(counter[index],VecG);
  geantHit->setPeak(counter[index],peak);
  geantHit->setTrackId(counter[index],idtrk);
  ++counter[index];
  geantHit->setNumOfHits(counter[index]);
}


//_____________________________________________________________________________
void StSvtSimulationMaker::Clear(const char*)
{
 
  if (Debug()) gMessMgr->Info() << "In StSvtSimulationMaker::Clear" << endm;
  
  
  //all will be deleted by StMaker::Clear()
  mSvtGeantHitColl = NULL;

  delete counter; counter=NULL;

  StMaker::Clear();
  
  if (Debug()) gMessMgr->Info() << "In StSvtSimulationMaker::Clear..END" << endm; 
}


//____________________________________________________________________________

Int_t StSvtSimulationMaker::Finish()
{
  if (Debug()) gMessMgr->Info()<< "In StSvtSimulationMaker::Finish()"<< endm;

  /*
  if (Debug()&&(mNtFile)) {
    mNtFile->Write(); 
    mNtFile->Close(); 
  }
  */
  //mSvtSimulation->closeFiles(); 
  //mElectronCloud->closeFiles();

  if (Debug()) gMessMgr->Info()<< "In StSvtSimulationMaker::Finish() ...END"<< endm;
  return kStOK;
}

//____________________________________________________________________________



