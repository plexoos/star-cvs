// $Id: StSvtClusterMaker.cxx,v 1.2 2000/08/21 13:06:58 caines Exp $
// $Log: StSvtClusterMaker.cxx,v $
// Revision 1.2  2000/08/21 13:06:58  caines
// Much improved hit finding and fitting
//
// Revision 1.1  2000/07/06 03:50:34  caines
// First version of cluster finder and fitter
//
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StSvtClusterMaker class                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "StChain.h"
#include "TH1.h"
#include "TH2.h"
#include "St_DataSetIter.h"
#include "TObjectSet.h"
#include "StSequence.hh"

#include "StSvtClassLibrary/StSvtHybridCollection.hh"
#include "StSvtClassLibrary/StSvtHybridData.hh"
#include "StSvtClassLibrary/StSvtData.hh"
#include "StSvtHybridCluster.hh"
#include "StSvtClusterMaker.h"



//_____________________________________________________________________________
StSvtClusterMaker::StSvtClusterMaker(const char *name) : StMaker(name)
{
 mSvtEvent = NULL;
 mHybridData = NULL;
 mHybridCluster = NULL;
 mClusterColl = NULL;
 mClusterSet = NULL;
 mClusterFinder = NULL;
}
//_____________________________________________________________________________
StSvtClusterMaker::~StSvtClusterMaker()
{

 delete mHybridCluster;
 delete mClusterColl;
 delete mClusterFinder;
}

//_____________________________________________________________________________
Int_t StSvtClusterMaker::Init(){

  printf("In StSvtClusterMaker::Init() ...\n");

  St_DataSet *dataSet = GetDataSet("StSvtData");
  assert(dataSet); 
  mSvtEvent = (StSvtData*)(dataSet->GetObject());
  assert(mSvtEvent);

  SetSvtCluster();
  mClusterFinder = new StSvtClusterFinder();

  return StMaker::Init();

}

//_____________________________________________________________________________
Int_t StSvtClusterMaker::SetSvtCluster()
{
  mClusterSet = new St_ObjectSet("StSvtCluster");
  AddData(mClusterSet);  
  SetOutput(mClusterSet); //Declare for output

  if (!mClusterColl) {
    mClusterColl = new StSvtHybridCollection(mSvtEvent->getConfiguration());
    mClusterSet->SetObject((TObject*)mClusterColl);
  } 

  return kStOK;
}


//_____________________________________________________________________________
Int_t StSvtClusterMaker::Make(){
  
  printf("In StSvtClusterMaker::Make() ...\n");
  
  printf("Running now \n");

  SetHybridClusters();
   
 return kStOK;
}

//______________________________________________________________________________________
Int_t StSvtClusterMaker::SetHybridClusters()
{
 int index =0;
 
 for(int barrel = 1;barrel <= mSvtEvent->getNumberOfBarrels();barrel++) {
   //cout<<mSvtEvent->getNumberOfBarrels()<<endl;
   for (int ladder = 1;ladder <= mSvtEvent->getNumberOfLadders(barrel);ladder++) {
     //cout<<mSvtEvent->getNumberOfLadders(barrel)<<endl;
      for (int wafer = 1;wafer <= mSvtEvent->getNumberOfWafers(barrel);wafer++) {
	// cout<<mSvtEvent->getNumberOfWafers(barrel)<<endl;
	for (int hybrid = 1;hybrid <=mSvtEvent->getNumberOfHybrids();hybrid++){
          //cout<<mSvtEvent->getNumberOfHybrids()<<endl;  
           
          index = mSvtEvent->getHybridIndex(barrel,ladder,wafer,hybrid);
          if(index < 0) continue;

          mHybridData = (StSvtHybridData *)mSvtEvent->at(index);
	  if( !mHybridData) continue;

          mClusterFinder->setHybridPointer(mHybridData);

          mClusterFinder->ClusterFinder();
          

	  mHybridCluster = (StSvtHybridCluster* )mClusterColl->at(index);
	  if( mHybridCluster){
	    delete mHybridCluster;
	  }
	  
	  mHybridCluster = new StSvtHybridCluster();
	  
	  mHybridCluster->setCluster(mClusterFinder);
	  mClusterColl->at(index) = mHybridCluster;
          mClusterFinder->ResetContainers();
	  
            
         
          }
      }
    }
  }

  return kStOK;
}


//_____________________________________________________________________________

Int_t StSvtClusterMaker::Finish(){

  printf("In StSvtClusterMaker::Finish() ...\n"); 

  return kStOK;
}

//_____________________________________________________________________________
ClassImp(StSvtClusterMaker)










