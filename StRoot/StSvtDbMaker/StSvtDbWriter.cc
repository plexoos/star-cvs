/***************************************************************************
 *
 * $Id: StSvtDbWriter.cc,v 1.3 2004/01/30 07:22:07 munhoz Exp $
 *
 * Author: Marcelo Munhoz
 ***************************************************************************
 *
 * Description: SVT DB access Maker
 *
 ***************************************************************************
 *
 * $Log: StSvtDbWriter.cc,v $
 * Revision 1.3  2004/01/30 07:22:07  munhoz
 * adding rms and daq parameters reading
 *
 * Revision 1.2  2001/11/07 16:49:48  caines
 *  Add _st s added by stic for our comfort and convenience
 *
 * Revision 1.1  2001/10/29 18:53:14  munhoz
 * starting SVT Data base
 *
 *
 **************************************************************************/

#include "StSvtDbWriter.hh"

#include "StSvtClassLibrary/StSvtHybridDriftVelocity.hh"
#include "StSvtClassLibrary/StSvtHybridPed.hh"
#include "StSvtClassLibrary/StSvtHybridCollection.hh"

#include "StDbLib/StDbManager.hh"                      // Database Libraries
#include "StDbLib/StDbConfigNode.hh"                   //
#include "StDbLib/StDbTable.h"                         //

#include "svtDriftVelocity.h"
#include "svtPedestals.h"
#include "svtRms.h"
#include "svtConfiguration.h"

#define N_INJECTOR_LINES 4

#ifdef __ROOT__
ClassImp(StSvtDbWriter)
#endif

//_____________________________________________________________________________
StSvtDbWriter::StSvtDbWriter(Text_t *timestamp)
{
  // set time stamp
  mTimeStamp = timestamp;
  mUnixTimeStamp = 0;

  // set DB manager
  setDbManager();
}

//_____________________________________________________________________________
StSvtDbWriter::StSvtDbWriter(Int_t timestamp)
{
  // set time stamp
  mTimeStamp = 0;
  mUnixTimeStamp = timestamp;

  // set DB manager
  setDbManager();
}

//_____________________________________________________________________________
StSvtDbWriter::~StSvtDbWriter()
{
  if (mConfigCalib)
    delete mConfigCalib;
  if (mConfigGeom)
    delete mConfigGeom;
  if (mConfigCond)
    delete mConfigCond;
}

//_____________________________________________________________________________
void StSvtDbWriter::setDbManager()
{
  //->get the singleton manager
  //
  mDbMgr = StDbManager::Instance();

  mDbMgr -> setVerbose(true);             // set Verbose mode for debug

  //-> connect to the db & get an empty container
  //
  mConfigCalib = mDbMgr -> initConfig(dbCalibrations,dbSvt);
  mConfigGeom  = mDbMgr -> initConfig(dbGeometry,dbSvt);
  //  mConfigCond  = mDbMgr -> initConfig(dbConditions,dbSvt);

  // set the store time. (need to be only once for an ensemble of tables)
  //
  if (mTimeStamp)
    mDbMgr->setStoreTime(mTimeStamp);
  else if (mUnixTimeStamp)
    mDbMgr->setStoreTime(mUnixTimeStamp);
}

//_____________________________________________________________________________
void  StSvtDbWriter::addDriftVelocity(StSvtHybridCollection* svtColl)
{
  cout << "StSvtDbWriter::WriteToDb" << endl;
  //-> add a table to the container with descriptor given by Database
  //
  StDbTable* svtDriftVelocTable = mConfigCalib -> addDbTable("svtDriftVelocity");
  if (svtDriftVelocTable == 0 )
   {
     cout << " No Table : svtDriftVelocity  " << endl;
     return;
   };
  
  // Fill the c-struct somehow
  //
  const int numberOfHybrids = svtColl->getTotalNumberOfHybrids();
  int* indexHybrid = new int[numberOfHybrids];
  int nHybrids=0;

  svtDriftVelocity_st *driftVelocity = new svtDriftVelocity_st[numberOfHybrids];
  StSvtHybridDriftVelocity* hybridDriftVeloc;

  // Loop over barrels, ladders, wafers and hybrids
  for (int barrel = 1;barrel <= svtColl->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= svtColl->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= svtColl->getNumberOfWafers(barrel);wafer++) {
	for (int hybrid = 1;hybrid <= svtColl->getNumberOfHybrids();hybrid++) {

  	  indexHybrid[nHybrids] = svtColl->getHybridIndex(barrel, ladder, wafer, hybrid); 
  	  if (indexHybrid < 0) continue;

	  hybridDriftVeloc = (StSvtHybridDriftVelocity*)svtColl->at(indexHybrid[nHybrids]);
	  
	  if (!hybridDriftVeloc) continue;
	  
	  //for (int anode=1;anode<=hybridDriftVeloc->getSvtConfig()->getNumberOfAnodes();anode++) 
	  for (int anode=1;anode<=240;anode++) 
	    {
	      //driftVelocity->injectionDriftVelocityFocus[anode] = hybridDriftVeloc->getV1(anode);
	      //driftVelocity->injectionDriftVelocity[anode] = hybridDriftVeloc->getV2(anode);
	      //driftVelocity->averageDriftVelocity[anode] = hybridDriftVeloc->getV3(anode);
	      driftVelocity[nHybrids].injectionDriftVelocityFocus[anode-1] = 0.625;
	      driftVelocity[nHybrids].injectionDriftVelocity[anode-1] = 0.625;
	      driftVelocity[nHybrids].averageDriftVelocity[anode-1] = 0.625;
	    }

	  nHybrids++;
	  
	}  // end of loop over hybrids
      }  // end of loop over wafers
    }  // end of loop over ladders
  }  // end of loop over barrels

  svtDriftVelocTable->SetTable((char*)driftVelocity,nHybrids,indexHybrid);   // put data in local table on memory
  //svtDriftVelocTable->SetNRows(nHybrids);                 // Set number of rows on table in database
  //svtDriftVelocTable->setElementID(indexHybrid,nHybrids);        // set element ID to hybridIndex

  // put the data on memory to database serever.
  //
  mDbMgr->storeDbTable(svtDriftVelocTable);  
}

//_____________________________________________________________________________
void  StSvtDbWriter::addPedestals(StSvtHybridCollection* svtColl)
{
  cout << "StSvtDbWriter::WriteToDb" << endl;
  //-> add a table to the container with descriptor given by Database
  //
  StDbTable* svtPedTable = mConfigCalib -> addDbTable("svtPedestals");
  if (svtPedTable == 0 )
   {
     cout << " No Table : svtPedestals  " << endl;
     return;
   };
  
  // Fill the c-struct somehow
  //
  const int numberOfHybrids = svtColl->getTotalNumberOfHybrids();
  int* indexHybrid = new int[numberOfHybrids];
  int nHybrids=0;

  svtPedestals_st *pedestals = new svtPedestals_st[numberOfHybrids];
  StSvtHybridPed* hybridPed;

  // Loop over barrels, ladders, wafers and hybrids
  for (int barrel = 1;barrel <= svtColl->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= svtColl->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= svtColl->getNumberOfWafers(barrel);wafer++) {
	for (int hybrid = 1;hybrid <= svtColl->getNumberOfHybrids();hybrid++) {

  	  indexHybrid[nHybrids] = svtColl->getHybridIndex(barrel, ladder, wafer, hybrid); 

  	  if (indexHybrid[nHybrids] < 0) continue;

	  hybridPed = (StSvtHybridPed*)svtColl->at(indexHybrid[nHybrids]);
	  
	  cout << "nHybrids = " << nHybrids << ", indexHybrid[nHybrids] = " << indexHybrid[nHybrids] << ", hybridPed = " << hybridPed << endl;

	  if (!hybridPed) {
	    nHybrids++;
	    continue;
	  }
	  
	  //for (int anode = 1; anode <= hybridPed->getSvtConfig()->getNumberOfAnodes(); anode++) 
	  //  for (int time = 0; time < hybridPed->getSvtConfig()->getNumberOfTimeBins(); time++) 
	  for (int anode=1;anode<=240;anode++) 
	    for (int time=0;time<128;time++) {
	      pedestals[indexHybrid[nHybrids]].pedestal[anode-1][time] = hybridPed->getPixelContent(anode,time);
	      if ((anode == 180) && (time == 64))
		cout << anode << "  " << time << "  " << pedestals[indexHybrid[nHybrids]].pedestal[anode-1][time] << endl;
	    }

	  pedestals[indexHybrid[nHybrids]].rms = hybridPed->getRMS();

	  nHybrids++;
	  
	}  // end of loop over hybrids
      }  // end of loop over wafers
    }  // end of loop over ladders
  }  // end of loop over barrels

  svtPedTable->SetTable((char*)pedestals,nHybrids,indexHybrid);   // put data in local table on memory

  // put the data on memory to database serever.
  //
  mDbMgr->storeDbTable(svtPedTable);  
}

//_____________________________________________________________________________
void  StSvtDbWriter::addRms(StSvtHybridCollection* svtColl)
{
  cout << "StSvtDbWriter::WriteToDb" << endl;
  //-> add a table to the container with descriptor given by Database
  //
  StDbTable* svtRmsTable = mConfigCalib -> addDbTable("svtRms");
  if (svtRmsTable == 0 )
   {
     cout << " No Table : svtRms  " << endl;
     return;
   };
  
  // Fill the c-struct somehow
  //
  const int numberOfHybrids = svtColl->getTotalNumberOfHybrids();
  int* indexHybrid = new int[numberOfHybrids];
  int nHybrids=0;

  svtRms_st *rms = new svtRms_st[numberOfHybrids];
  StSvtHybridPixels* hybridRms;

  // Loop over barrels, ladders, wafers and hybrids
  for (int barrel = 1;barrel <= svtColl->getNumberOfBarrels();barrel++) {
    for (int ladder = 1;ladder <= svtColl->getNumberOfLadders(barrel);ladder++) {
      for (int wafer = 1;wafer <= svtColl->getNumberOfWafers(barrel);wafer++) {
	for (int hybrid = 1;hybrid <= svtColl->getNumberOfHybrids();hybrid++) {

  	  indexHybrid[nHybrids] = svtColl->getHybridIndex(barrel, ladder, wafer, hybrid); 

  	  if (indexHybrid[nHybrids] < 0) continue;

	  hybridRms = (StSvtHybridPixels*)svtColl->at(indexHybrid[nHybrids]);
	  
	  cout << "nHybrids = " << nHybrids << ", indexHybrid[nHybrids] = " << indexHybrid[nHybrids] << ", hybridRms = " << hybridRms << endl;

	  if (!hybridRms) {
	    nHybrids++;
	    continue;
	  }
	  
	  //for (int anode = 1; anode <= hybridRms->getSvtConfig()->getNumberOfAnodes(); anode++) 
	  //  for (int time = 0; time < hybridRms->getSvtConfig()->getNumberOfTimeBins(); time++) 
	  for (int anode=1;anode<=240;anode++) 
	    for (int time=0;time<128;time++) {
	      rms[indexHybrid[nHybrids]].rms[anode-1][time] = hybridRms->getPixelContent(anode,time);
	      if ((anode == 180) && (time == 64))
		cout << anode << "  " << time << "  " << rms[indexHybrid[nHybrids]].rms[anode-1][time] << endl;
	    }

	  nHybrids++;
	  
	}  // end of loop over hybrids
      }  // end of loop over wafers
    }  // end of loop over ladders
  }  // end of loop over barrels

  svtRmsTable->SetTable((char*)rms,nHybrids,indexHybrid);   // put data in local table on memory

  // put the data on memory to database serever.
  //
  mDbMgr->storeDbTable(svtRmsTable);  
}

void  StSvtDbWriter::addConfiguration()
{
  cout << "StSvtDbWriter::WriteToDb" << endl;
  //-> add a table to the container with descriptor given by Database
  //
  StDbTable* svtConfigTable = mConfigGeom -> addDbTable("svtConfiguration");
  if (svtConfigTable == 0 )
   {
     cout << " No Table :  svtConfiguration " << endl;
     return;
   };

  svtConfiguration_st config;

  config.numberOfBarrels = 3;
  config.numberOfLadders = 2;
  config.numberOfWafers = 7;
  config.numberOfHybrids = 14;
  
  config.numberOfLaddersPerBarrel[0] = 0;
  config.numberOfLaddersPerBarrel[1] = 0;
  config.numberOfLaddersPerBarrel[2] = 2;
  config.numberOfWafersPerLadder[0] = 0;
  config.numberOfWafersPerLadder[1] = 0;
  config.numberOfWafersPerLadder[2] = 7;
  config.numberOfHybridsPerWafer = 2;
  
  svtConfigTable->SetTable((char*)&config,1);

  // put the data on memory to database serever.
  //
  mDbMgr->storeDbTable(svtConfigTable);  
}
