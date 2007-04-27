/***************************************************************************
 *
 * $Id: StRchMaker.cxx,v 2.16 2007/04/27 11:26:45 hippolyt Exp $
 *
 * Author:  bl
 ***************************************************************************
 *
 * Description: RICH offline software:
 *              StRchMaker.cxx - ROOT/STAR Maker for offline chain.
 *              Incorporation of cluster finder here
 ***************************************************************************
 *
 * See Log Comments at bottom
 ***************************************************************************/

#include <Stiostream.h>
#include "Stiostream.h"
#include "StMessMgr.h"

#include "StRchMaker.h"
#include "StChain.h"
#include "St_DataSetIter.h"

#include "StGlobals.hh"
#include "StThreeVectorF.hh"

// StEvent
#include "StEvent/StEvent.h"
#include "StEvent/StContainers.h"
#include "StEvent/StRichCollection.h"
#include "StEvent/StRichHit.h"
#include "StEvent/StRichMCHit.h"
#include "StEvent/StRichPixel.h"
#include "StEvent/StRichMCPixel.h"
#include "StEvent/StSoftwareMonitor.h"
#include "StEvent/StRichSoftwareMonitor.h"
//
// Interfaces
//
// DAQ Libraries
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDaqLib/RICH/RICH_Reader.hh"
#include "StDAQMaker/StDAQReader.h"
// Simulation
#include "StRrsMaker/StRrsReader.h"
#include "StRrsMaker/StRichPadPlane.h"
#include "StRrsMaker/StRichSinglePixel.h"
#include "StRrsMaker/StRichSingleMCPixel.h"

// Database
#include "StRrsMaker/StRichGeometryDb.h"
#include "StRichDisplayActivate.h"

#ifdef RICH_WITH_PAD_MONITOR
#include "StRichDisplayMaker/StRichPadMonitor.h"
#endif

// Internal Rch
#include "StRichSimpleHit.h"
#include "StRichClusterAndHitFinder.h"
#include "StRichSimpleHitCollection.h"

//
// dst tables in $STAR/include/tables/
//#include "tables/St_g2t_rch_hit_Table.h"
//#include "tables/St_dst_rch_pixel_Table.h"

//
// line items of the table
// in $STAR/include
//idl in $STAR/include/../pams/global/idl/dst_rch_pixel.idl
//#include "dst_rch_pixel.h"
ClassImp(StRchMaker) // macro
   
//-----------------------------------------------------------------

    StRchMaker::StRchMaker(const char *name, int daq, int matrix, int cf)
	: StMaker(name), mDaq(daq), mUseMatrix(matrix), mCfOnly(cf)
{
  mTheDataReader = 0;
  mTheRichReader = 0;
  mTheRichData   = 0;
  mGeometryDb    = 0;

  mDaq           = 0;
  mPads          = 0;
  mRows          = 0;
  mNumberOfPads  = 0;
  mEventNumber   = 0;
  mUseMatrix     = 0;

  // flags
  mRichCollectionPresent    = 0;
  mPixelCollectionPresent   = 0;
  mClusterCollectionPresent = 0;
  mHitCollectionPresent     = 0;
  mCfOnly                   = 0;
  //pedestal
  mPedestalSubtract         = 0;
  Int_t i                   = 0;        
  Int_t j                   = 0;        
  for(i=0;i<160;i++){
    for(j=0;j<96;j++){
      mPedestal[i][j] = 0;
      mSigma[i][j]    = 0;
    }
  }
  mPedestalFile             = 0;
  mRemovePicketFencePixels  = 1;
    
#ifndef __CINT__    
  //  mPixelStore               = 0;
  //  pointerToPixelStoreType   = 0;
  //  mPicketFencePixelStore    = 0;
  mClusterFinder            = 0;
  mSimpleHitCollection      = 0;
  //  mTheHits                  = 0;
#endif    

  mEvent                    = 0;
  mTheRichCollection        = 0;

#ifdef RCH_HISTOGRAM
    mRchNTupleFile          = 0;
    mPadPlane               = 0;
    mClusters               = 0;
    mHits                   = 0;
    for(i=0;i< 4;i++) mRawData[i] = 0;
    for(i=0;i< 6;i++) mCluster[i] = 0;
    for(i=0;i<11;i++) mHit[i]     = 0;
    
    mcc       = 0;
    mmc       = 0;
    mrms      = 0;
    mpad      = 0;
    mqpad     = 0;
    mcratio   = 0;
    mhc       = 0;
    mhmc      = 0;
    mhc2m     = 0;
#endif
    drawinit=kFALSE;
}

//-----------------------------------------------------------------

StRchMaker::~StRchMaker() {}

//-----------------------------------------------------------------

Int_t StRchMaker::Init() {

  { LOG_INFO << "StRchMaker::Init()" << endm; }
    //
    // either DAQ or SIM data.  MACRO switchable!
    //

    //
    // Read in parameters from the dataBase!
    // For now we can hard-code it!
    mGeometryDb = StRichGeometryDb::getDb();
    
    mPads =  mGeometryDb->numberOfPadsInARow(); //160
    mRows = mGeometryDb->numberOfRowsInAColumn(); //96
    
    mNumberOfPads = mGeometryDb->numberOfPads();
//     PR(mNumberOfPads);
    mEventNumber = 0;

    //
    // cluster finder
    //
    mClusterFinder = new StRichClusterAndHitFinder();
    
    //
    // the hit Collection
    //
    mSimpleHitCollection = new  StRichSimpleHitCollection();
    AddConst(new St_ObjectSet("richHits",mSimpleHitCollection));
    
#ifdef RCH_HISTOGRAM
    mRchNTupleFile = new TFile("RchData.root","RECREATE","Rch Ntuples");
    mPadPlane      = new TNtuple("rawNtuple", "raw data", "row:pad:adc:evt");
    mClusters      = new TNtuple("clusters", "cluster data","q:qmax:rms2:pads:max:evt");
    mHits          = new TNtuple("hits", "hit data", "q:qmax:evt:dc:cnumber:pads:x:dx:y:dy:pad");
    mcc            = new TH1F("ccharge","Cluster Charge",50,0,3000);
    mmc            = new TH1F("cmaxadc","Cluster max ADC",50,0,1024);
    mrms           = new TH1F("crms2","Cluster RMS2",50,0,1000);
    mpad           = new TH1F("cpads","Cluster pads",15,0,15);
    mqpad          = new TH1F("caveq","Cluster q/pad",20,0,150);
    mcratio        = new TH1F("cq2max","Cluster q/maxadc",50,0,5);
    
    mhc            = new TH1F("hcharge","Hit Charge",50,0,2500);
    mhmc           = new TH1F("hmaxadc","Hit max ADC",50,0,1024);
    mhc2m          = new TH1F("hq2maxadc","Hit q/maxADC",50,0,5);
#endif
    
    //
    // Access to DataBase for Pedestal
    // and GAIN correction goes here
    //
    if (mPedestalSubtract) {
	ifstream pedfile;
	pedfile.open(mPedestalFile);
	if (!pedfile) {
	  { LOG_ERROR << "StRchMaker::Init(): Can not open ped file: " << mPedestalFile << endm; }
	}
	else {
	    for (unsigned int channelnum=0; channelnum<960; ++channelnum) {
		for (unsigned int cramblock=0; cramblock<16; ++cramblock) {
		    pedfile >> mPedestal[channelnum/6][95 - (cramblock*6 + channelnum%6)] 
			    >> mSigma[channelnum/6][95 - (cramblock*6 + channelnum%6)];
		}
	    }
	    pedfile.close();
	    { LOG_INFO << "StRchMaker::Init(): Read pedestals " << endm; }
  	    for (unsigned int pad=0; pad < 160; ++pad) {
  		for (unsigned int row=0; row < 96; ++row) {
		  { LOG_DEBUG << " pad " << pad << " row " << row<< " ped " << mPedestal[pad][row] << " sig " << mSigma[pad][row] << endm; }
  		}
  	    }
	}
   }
    
    return StMaker::Init();
}


int StRchMaker::adcDecoder(unsigned long code,
			   unsigned long* pad, unsigned long* row, unsigned long* adc)
{
    *pad = ( code        & 0xff);
    *row = ((code >> 8)  & 0xff);
    //--> Used to be *adc = ((code >> 16) & 0x3ff);
    *adc = ( (code>>26) & 0x1 ) ? 1024 : ( (code>>16) & 0x3ff);

    return 0;
}

//-----------------------------------------------------------------

Int_t StRchMaker::Make() {
  { LOG_INFO << "StRchMaker::Make()" << endm; }
    //
    // Initialize Flags
    //
    mRichCollectionPresent    = 0;
    mPixelCollectionPresent   = 0;
    mClusterCollectionPresent = 0;
    mHitCollectionPresent     = 0;


    //
    // ptr initialization for StEvent
    //
    mTheRichCollection = 0;
    
    //
    // increase event counter
    //
    mEventNumber++;

    //
    // Load the pixels into a container for
    // input into the CF
    //
    mPixelStore.clear();
    
    mPicketFencePixelStore.clear();
    
    //
    // Pause for Event Display Inspection
    //
//          cout << "Next Event? <ret>: " << endl;
//          do {
//            if(getchar()) break;
//          } while (true);

    //
    // Try get StEvent Structure
    //
    mEvent = (StEvent *) GetInputDS("StEvent");

    //
    // Interogate StEvent structure
    //
    if(!mEvent) {
	  { LOG_ERROR <<"StEvent structure does not exist %n - Processing of RICH DATA CANNOT OCCUR %n - StEventMaker must run before StRchMaker %n - Aborting..." << endm; }
	abort();
    }
    mTheRichCollection = mEvent->richCollection();

    //
    // if there is no rich collection, check for embedding data to use,
    // otherwise try access data via the interfaces
    //
    
    if(!mTheRichCollection) {
	St_ObjectSet *embeddedData =
	    (St_ObjectSet*)GetDataSet("richMixer/.data/richMixedEvent");

	if(!embeddedData) {
	  { LOG_ERROR << "No RICH Embedded data %n StEvent richCollection DOES NOT Exist %n StEvent RichPixelCollection DOES NOT Exist %n \tStEvent RichClusterCollection DOES NOT Exist %n StEvent RichHitCollection DOES NOT Exist" << endm; }
	}
	else {
	    mTheRichCollection = (StRichCollection*)(embeddedData->GetObject());
	    if(mTheRichCollection->pixelsPresent()) {
		mPixelCollectionPresent = 1;
		{ LOG_INFO << " %l : " << (mTheRichCollection->getRichPixels().size()) << "pixels from the StRichMixerMaker" << endm; }
	    }
	}
    } // check the rich collection
    else {
	if(mTheRichCollection->pixelsPresent()) {
	  { LOG_INFO << "StEvent RichPixelCollection Exists" << endm; }
	  mPixelCollectionPresent = 1;
	}
	else {
	  { LOG_ERROR << "StEvent RichPixelCollection DOES NOT Exist" << endm; }
	}
	if(mTheRichCollection->clustersPresent()) {
	  { LOG_INFO << "StEvent RichClusters Exist" << endm; }
	  mClusterCollectionPresent = 1;
	}
	else {
	  { LOG_ERROR << "StEvent RichClusters DO NOT Exist" << endm; }
	}
    }

    //
    // If there are no pixels, get the interface access to the data
    //
    if(!mPixelCollectionPresent) {
	  { LOG_INFO << " %F No Pixel Collection!!!  -->  TRY GET THE DATA SETS!" << endm; }
	if(!mDaq) {
	  { LOG_DEBUG << "Simulation Mode" << endm; }
	    St_ObjectSet *rrsEvent =
		(St_ObjectSet*)GetDataSet("Rrs/.const/richPixels");
	    if(!rrsEvent) {
	      { LOG_INFO << "StRchMaker::Maker() %n DataSet: rrsEvent not there %n Skip this event" << endm; }
		this->clearPadMonitor();
		return kStWarn;
	    }

	    StRichPadPlane* theRichSimData =
		(StRichPadPlane*)(rrsEvent->GetObject());
	    if(!theRichSimData) {
	      { LOG_INFO << "StRchMaker::Maker() %n RichSimData: not there %n Skip this event" << endm; }
	      this->clearPadMonitor();
	      return kStWarn;
	    }
	    mTheRichReader = new StRrsReader(theRichSimData, -9);
	}
	else {
	  { LOG_DEBUG << "DAQ Mode" << endm; }
	    mTheRichData   = GetDataSet("StDAQReader");
	    if(!mTheRichData) {
	      { LOG_INFO << "StRchMaker::Maker() %n DataSet: StDAQReader not there %n Skip this event" << endm; }
	      this->clearPadMonitor();
		
	      return kStWarn;
	    }

	    mTheDataReader = (StDAQReader*)(mTheRichData->GetObject());
	    if(!mTheDataReader) {
	      { LOG_INFO << "StRchMaker::Maker() %n DataSet: StDAQReader not there %n Skip this event" << endm; }
	      this->clearPadMonitor();
	      return kStWarn;
	    }
	    if (!(mTheDataReader->RICHPresent())) {
	      { LOG_INFO << "StRchMaker::Maker() %n RICH not in datastream %n Skip this event" << endm; }
	      this->clearPadMonitor();
	      return kStWarn;
	    }
	    mTheRichReader = mTheDataReader->getRICHReader();
	}

	if(mTheRichReader) {
	  { LOG_DEBUG << "Got the reader" << endm; }
	}
	else {
	  { LOG_INFO << "StRchMaker::Maker() %n Could not get a Reader %n Skip this event" << endm; }
	  this->clearPadMonitor();
	  return kStWarn;
	}

	//
	// In possession of a RichReader
	//  --> use the interface to extract the pixels
	//     and load the PixelStore for the CF
	// The decoding is done such that it is a
	// saturated pad if the 11th bit is set
	//
	if (mRemovePicketFencePixels) {
	  { LOG_INFO << "StRchMaker::Make(): removing picket fence pixels" << endm; }
	}
	
	bool saturatedPad = false;
	
	for(int iPad=0; iPad<mPads; iPad++) {  //x--> 160
	    for(int iRow=0; iRow<mRows; iRow++) { // y -> 96
		
		saturatedPad = false;
		
		unsigned long theADCValue =
		    mTheRichReader->GetADCFromCoord(iPad,iRow);

		if(theADCValue>1023) {
		    saturatedPad = true;
		    theADCValue = 1024; // anything higher is junk
		}
		
		if (mPedestalSubtract && (iPad == 0) &&  (iRow%6==5)) {
		    unsigned long theCut = static_cast<unsigned long>(mPedestal[iPad][iRow] + mSigma[iPad][iRow]);
			
			if (theADCValue > theCut) {
			    theADCValue -= static_cast<unsigned long>(mPedestal[iPad][iRow]);
			}
			else {
			    theADCValue = 0;
			}
		}
		
		
		//pack adc/row/pad into a single long.  Use:
		// the first 8 bits for the Pad (0-159)  --> 0-255
		// the next  8 bits for the Row (0-96)   --> 0-255
		// the next 11 bits for the ADC (0-1023) --> 0-2047
		
		if(theADCValue) {
		    unsigned long codedValue = 0;
		    codedValue = (theADCValue << 16) | (iRow << 8) | iPad;

		    pointerToPixelStoreType thePixelPointer = 0;
// Deal with removing picket fence pixels
		    if (mRemovePicketFencePixels) {
			
			if (iPad == 0 && iRow%6==5 &&theADCValue) {
			    thePixelPointer = &(mPicketFencePixelStore);
			}
			else {
			    thePixelPointer = &(mPixelStore);
			}
		    }
		    else {
			thePixelPointer = &(mPixelStore);
		    }
		    
		    //
		    // fill the pixel store
		    //
		    if(dynamic_cast<StRrsReader*>(mTheRichReader)) {
			// ...from simulation
			anIDList mcInfo =
			    dynamic_cast<StRrsReader*>(mTheRichReader)->GetMCDetectorInfo(iPad, iRow);
			thePixelPointer->push_back(new StRichSingleMCPixel(iPad,
								      iRow,
								      theADCValue,
								      mcInfo));
		    }
		    else {
			// ...from data
			thePixelPointer->push_back(new StRichSinglePixel(iPad,iRow,theADCValue));
		    }
		    if(saturatedPad) {
			thePixelPointer->back()->setBit(eSaturatedPixel);
			//cout << "p/r/q " << iPad << "/" << iRow << "/" << theADCValue << endl;
		    }

#ifdef RCH_DEBUG
		    if (theADCValue > 0) {
			raw << "pad: "  << iPad
			    << " row: " << iRow
			    << " adc: " << theADCValue
			    << " code " << codedValue << endl;
			
			unsigned long decodepad;
			unsigned long decoderow;
			unsigned long decodeadc;
			if(adcDecoder(codedValue,&decodepad,&decoderow,&decodeadc)) {
			    raw << decodepad << "/"
				<< decoderow << "/"
				<< decodeadc << endl;
			}
		    }
#endif	    
		} // if (theADCvalue) ?fill the dst structure 
	    } // loop over rows
	} // loop over pads
    }
    
    //
    // If the StRichPixelCollection hasn't been retrieved from StEvent
    // the interface must be used to access the pixels
    // --> Fill the pixel Store
    //
    else {
	const StSPtrVecRichPixel& thePixels = mTheRichCollection->getRichPixels();
	StSPtrVecRichPixelConstIterator iter;
	bool saturatedPad = false;
	for (iter  = thePixels.begin();
	     iter != thePixels.end();
	     ++iter) {
	    saturatedPad = false;
	    UShort_t iPad = (*iter)->pad();
	    UShort_t iRow = (*iter)->row();
	    UShort_t theADCValue = (*iter)->adc();
	    if(theADCValue>1023) {
		saturatedPad = true;
		theADCValue = 1024; // anything higher is junk
	    }
	    
	    if (mPedestalSubtract && (iPad==0) && (iRow%6==5)) {
		unsigned long theCut =
		    static_cast<unsigned long>(mPedestal[iPad][iRow] + mPedestalSubtract* mSigma[iPad][iRow]);
		if (theADCValue > theCut) {
		    theADCValue -= static_cast<unsigned long>(mPedestal[iPad][iRow]);
		}
		else {
		    theADCValue = 0;
		}
	    }
	    if (theADCValue) {
		
		StRichMCPixel* p = dynamic_cast<StRichMCPixel*>(*iter);
		pointerToPixelStoreType thePixelPointer=0;
		if (mRemovePicketFencePixels) {
		    
		    if (iPad == 0 && iRow%6==5 &&theADCValue) {
			thePixelPointer = &(mPicketFencePixelStore);
		    }
		    else {
			thePixelPointer = &(mPixelStore);
		    }
		}
		else {
		    thePixelPointer = &(mPixelStore);
		}
		
		if (p) {
		    anIDList mcInfo;
		    
		    const StSPtrVecRichMCInfo& theInfo = p->getMCInfo();
		    StSPtrVecRichMCInfoConstIterator i;
		    for (i  = theInfo.begin();
			 i != theInfo.end();
			 ++i) {
			int id = (*i)->id();
			int gid = (*i)->gid();
			int trackp = (*i)->trackp();
			int charge = static_cast<int>((*i)->charge());
			StRichSignalType process = static_cast<StRichSignalType>((*i)->process());
			
			mcInfo.push_back(StRichID(id,
						  gid,
						  trackp,
						  charge,
						  process));
		    }
		    thePixelPointer->push_back(new StRichSingleMCPixel(iPad,iRow,theADCValue, mcInfo));
		}
		else {
		    thePixelPointer->push_back(new StRichSinglePixel(iPad,iRow,theADCValue));
		}

		if(saturatedPad) {
		    thePixelPointer->back()->setBit(eSaturatedPixel);
		    //cout << "p/r/q " << iPad << "/" << iRow << "/" << theADCValue << endl;
		    
		}
	    }
	}
    }
    

    //
    // This is perhaps a future development
    // If we only want to run the cluster finder...load the pixels from
    // StEvent...
    //
//     if(mCfOnly) {
// 	// Load the pixels from StEvent
// 	// get the pixels
// 	cout << "WARNING:\n";
// 	cout << "\tLoad the *CODED* pixels from StEvent\n";
// 	cout << "\tNo MC info is available"  << endl;
// 	for(unsigned int ii=0; ii< theCodedCollection->size(); ii++) {
// 	    StRichPixel aPixel = theCodedCollection->pixel(ii);
// 	    mPixelStore.push_back(new StRichSinglePixel(aPixel.pad(),
// 						       aPixel.row(),
// 						       aPixel.adc()));
// 	}
//     }
    
    
    //
    // the cluster finder stuff
    //
//     cout << "StRchMaker::Make()\n\tAt the cluster finder" << endl;
    mClusterFinder->clearAndDestroyAll();
    mClusterFinder->loadPixels(mPixelStore);
    mClusterFinder->setBorderFlags();
    
#ifdef RICH_WITH_PAD_MONITOR
    { LOG_INFO << "Try get the pad monitor" << endm; }
    StRichPadMonitor* thePadMonitor = StRichPadMonitor::getInstance(mGeometryDb);
    //
    // Clears The Old Data. Must be done here and RRS maker
    //
    thePadMonitor->clearHits();
    thePadMonitor->clearTracks();
    //cout << "mDaq = " << mDaq << endl;
    if(mDaq) {
	thePadMonitor->clearPads();
	for(unsigned int jj=0; jj<mPixelStore.size(); jj++)
	    thePadMonitor->drawPad(*mPixelStore[jj]);
	
	//cout << "StRchMaker:Make()\n\tDrawing Pixels" << endl;
    }
    thePadMonitor->update();
#endif
    
    
#ifdef RCH_DEBUG
    mClusterFinder->printList(raw);
#endif
    
    mClusterFinder->makeTheClustersAndFilter();
    
#ifdef RCH_DEBUG
    mClusterFinder->printList(raw);
    mClusterFinder->dumpClusterInformation(raw);
#endif

    //
    // Do the hit finding
    //
    if(!mClusterFinder->simpleHitsFromClusters()) {
    { LOG_INFO << "==> simple hits from clusters failed!" << endm; }
    }
    
    mClusterFinder->calculateHitsInLocalCoordinates();
    mClusterFinder->calculateHitsInGlobalCoordinates();
    
#ifdef RCH_DEBUG
    { LOG_DEBUG << "Dump Hit Info==>size: " << mClusterFinder->getHits().size() << endm; }
    mClusterFinder->dumpHitInformation(raw);
#endif

    //
    // get the hits for 2 reasons:
    // - pass via the data set
    // - histogram and padMonitor
    //

    //HitVector
    mTheHits = mClusterFinder->getHits();

#ifdef RICH_WITH_PAD_MONITOR
    for(unsigned int jj=0; jj<mTheHits.size(); jj++) {
      { LOG_INFO << "StRchMaker::drawHit() " << *mTheHits[jj] << endm; }
	thePadMonitor->drawHit(mTheHits[jj]);
    }
    thePadMonitor->update();
#endif

   
#ifdef RCH_HISTOGRAM
    unsigned int kk;

    // pixels
    StRichSinglePixelCollection myPixels = mClusterFinder->getPixels();

    for(kk=0; kk<myPixels.size(); kk++) {
	mRawData[0] = myPixels[kk]->row();
	mRawData[1] = myPixels[kk]->pad();
	mRawData[2] = myPixels[kk]->charge();
	mRawData[3] = mEventNumber;

	mPadPlane->Fill(mRawData);
    }
    
    // cluster
    ClusterVector myClusters = mClusterFinder->getClusters();
    
    for(kk=0; kk<myClusters.size(); kk++) {
	mcc->Fill(myClusters[kk]->amplitudeSum());
	mmc->Fill(myClusters[kk]->minimumAmplitudeOfLocalMax());
	mrms->Fill(myClusters[kk]->rms2());
	mpad->Fill(myClusters[kk]->numberOfPads());
	mqpad->Fill(myClusters[kk]->amplitudeSum()/myClusters[kk]->numberOfPads());
	mcratio->Fill(myClusters[kk]->amplitudeSum()/myClusters[kk]->minimumAmplitudeOfLocalMax());
	
	mCluster[0] = myClusters[kk]->amplitudeSum();
	mCluster[1] = myClusters[kk]->minimumAmplitudeOfLocalMax();
	mCluster[2] = myClusters[kk]->rms2();
	mCluster[3] = myClusters[kk]->numberOfPads();
	mCluster[4] =  myClusters[kk]->numberOfLocalMax();
	mCluster[5] = mEventNumber;
	mClusters->Fill(mCluster);
    }
    
    // hits
    for(kk=0; kk<mTheHits.size(); kk++) {

	if(mTheHits[kk]->internal().x() !=0) {
	    mhc->Fill(mTheHits[kk]->charge());
	    mhmc->Fill(mTheHits[kk]->maxAmplitude());
	    mhc2m->Fill(mTheHits[kk]->charge()/mTheHits[kk]->maxAmplitude());
	}
	mHit[0] = mTheHits[kk]->charge();
	mHit[1] = mTheHits[kk]->maxAmplitude();
	mHit[2] = mEventNumber;
	mHit[3] = mTheHits[kk]->isSet(eDeconvoluted);
	mHit[4] = mTheHits[kk]->clusterNumber();
	mHit[5] = mTheHits[kk]->numberOfPads();
	mHit[6] = mTheHits[kk]->internal().x();
	mHit[7] = mTheHits[kk]->sigma().x();
	mHit[8] = mTheHits[kk]->internal().y();
	mHit[9] = mTheHits[kk]->sigma().y();
	mHit[10] = mTheHits[kk]->numberOfPads();
	mHits->Fill(mHit);
	
    }
#endif
    
    
    //
    // Here is where the Hit Collection should be written out
    // ...pass the hits via the data set
    //
//     PR(mTheHits.size());
    mSimpleHitCollection->mTheHits = mTheHits;
//     PR(mSimpleHitCollection->mTheHits.size());
    
    //
    // Write the hits into StEvent: StRichHitCollection
    //
    // If any is present, clear it.
    //
    if (mPixelCollectionPresent   ||
	mClusterCollectionPresent || 
	mHitCollectionPresent) {
	//StSPtrVecRichPixel& thePixels = mTheRichCollection->getRichPixels();
  	//thePixels.clear();
  	//StSPtrVecRichCluster& theClusters = mTheRichCollection->getRichClusters();
  	//theClusters.clear();
  	//StSPtrVecRichHit& theHits = mTheRichCollection->getRichHits();
  	//theHits.clear();	
	//delete mTheRichCollection;
	mTheRichCollection = 0;
    }

    //
    // If the StEvent Structure Exists, fill it:
    //  this includes:
    //   i StRichCollection
    //  ii StRichSoftwareMonitor
    //
    if(mEvent) {
	this->fillStEvent();
    }

    //
    // Cleanup:  A new reader is made for each event
    //           if we are using the daq libraries
    //
    if(!mDaq) {
	if(mTheRichReader) {
	    delete mTheRichReader;
	}
	mTheRichReader = 0;
    }
    
    // clear this out here.  Don't want to clean out the pixelstore.
    for (size_t ipicket=0; ipicket<mPicketFencePixelStore.size(); ++ipicket) {
	delete(mPicketFencePixelStore[ipicket]);
	mPicketFencePixelStore[ipicket] = 0;
    }
    
    mPicketFencePixelStore.clear();
    
    return kStOK;
}

void StRchMaker::fillStEvent()
{
  //
  // This function means there is a dependency on the
  // StEvent classes
  //
  { LOG_INFO << "StRchMaker::fillStEvent()" << endm; }
  StRichCollection *richCollection;

  if(!mTheRichCollection) {
  { LOG_INFO << "StRchMaker::Make a new collection" << endm; }
    richCollection = new StRichCollection();
  }
  else {
  { LOG_INFO << "StRchMaker::Make use the already existing collection" << endm; }
  richCollection = mTheRichCollection;
  }

    //
    // Add all the pixels...ordered from the ClusterFinder
    //
    int totalCharge = 0;
// Not a reference.  A copy, which will go away at the end of this function. 
    StRichSinglePixelCollection thePixels;
//     if(!(richCollection->pixelsPresent())) {
	
    if (1) {
	thePixels = mClusterFinder->getPixels();
// Add back in the picket fences for later analysis. 
	for (unsigned int ipicket=0; ipicket<mPicketFencePixelStore.size(); ++ipicket) {
	    unsigned int ipad = mPicketFencePixelStore[ipicket]->pad();
	    unsigned int irow = mPicketFencePixelStore[ipicket]->row();
	    // Need to make a new one because the SinglePixelCollection owns things
	    if (!thePixels(ipad,irow)) {
		thePixels.push_back(new StRichSinglePixel(*(mPicketFencePixelStore[ipicket])));
	    }
	    
	}
	
	{ LOG_INFO << " StRchMaker::fillStEvent() pixels " << thePixels.size() << endm; }
	for(size_t ii=0; ii<thePixels.size(); ii++) {
 	    unsigned long codedValue = 0;
 	    unsigned long adc = static_cast<unsigned long>(thePixels[ii]->charge());
 	    unsigned long row = thePixels[ii]->row();
 	    unsigned long pad = thePixels[ii]->pad();

	    totalCharge += adc;
 	    codedValue = (adc << 16) | (row << 8) | pad;

  	    if(dynamic_cast<StRichSingleMCPixel*>(thePixels[ii])) {
  		//cout << " ::fillStEvent() -> MC pixel" << endl;
		StRichMCPixel* persistentPixel = new StRichMCPixel(codedValue);
	        anIDList mcInfo = dynamic_cast<StRichSingleMCPixel*>(thePixels[ii])->MCInfo();

		for(size_t jj=0; jj<mcInfo.size(); jj++) {
 		    persistentPixel->addInfo(new StRichMCInfo(mcInfo[jj].mHitID,
							      mcInfo[jj].mG_ID,
							      mcInfo[jj].mTrackp,
							      mcInfo[jj].mCharge,
							      static_cast<unsigned short>(mcInfo[jj].mSignalType)));
		}

		richCollection->addPixel(persistentPixel);
	    }
 	    else {
 		//cout << " ::fillStEvent() -> data pixel" << endl;
 		richCollection->addPixel(new StRichPixel(codedValue));
 	    }
  	}

    }
    
    //
    // Add the clusters
    //
    ClusterVector theClusters;
    //PR(richCollection->clustersPresent());
//    if(!richCollection->clustersPresent()) {
    if(1) {
	theClusters = mClusterFinder->getClusters();
	{ LOG_INFO << " StRchMaker::fillStEvent() clusters " << theClusters.size() << endm; }
	for(size_t ii=0; ii<theClusters.size(); ii++) {
	    StRichCluster* thePersistentCluster = new StRichCluster(theClusters[ii]->numberOfPads(),
								    theClusters[ii]->numberOfLocalMax(),
								    theClusters[ii]->firstPad(),
								    theClusters[ii]->amplitudeSum(),
								    theClusters[ii]->amplitude2Sum(),
								    theClusters[ii]->rms2());
	    thePersistentCluster->setMinimumAmplitudeOfLocalMax(theClusters[ii]->minimumAmplitudeOfLocalMax());
	    
	    richCollection->addCluster(thePersistentCluster);
	}
        
    }

    //
    // Add the hits
    // USE THE OUTPUT from the Cluster finder explicitly
    //
//     if(!richCollection->hitsPresent()) {
    if(1) {
      { LOG_INFO << " StRchMaker::fillStEvent() hits " << mTheHits.size() << endm; }
	for(size_t ii=0; ii<mTheHits.size(); ii++) {
	    if(dynamic_cast<StRichSimpleMCHit*>(mTheHits[ii])) {
		//cout << "mchit ";
		StRichMCHit* thePersistentHit = new StRichMCHit(StThreeVectorF(mTheHits[ii]->global().x(),
									       mTheHits[ii]->global().y(),
									       mTheHits[ii]->global().z()),
								StThreeVectorF(mTheHits[ii]->localError().x(),
									       mTheHits[ii]->localError().y(),
									       mTheHits[ii]->localError().z()),
								kRichId,
								mTheHits[ii]->charge(),
								mTheHits[ii]->maxAmplitude(),
								static_cast<unsigned char>(0));
		
		//Set the single MCInfo
		StRichID theID = (dynamic_cast<StRichSimpleMCHit*>(mTheHits[ii]))->getMCInfo();
		
		thePersistentHit->setMCInfo(
		    StRichMCInfo(theID.mHitID,
				 theID.mG_ID,
				 theID.mTrackp,
				 theID.mCharge,
				 static_cast<unsigned short>(theID.mSignalType)));

		// Add it
		richCollection->addHit(thePersistentHit);
	    }
	    else {
		//cout << "hit ";
		StRichHit* thePersistentHit = new StRichHit(StThreeVectorF(mTheHits[ii]->global().x(),
									   mTheHits[ii]->global().y(),
									   mTheHits[ii]->global().z()),
							    StThreeVectorF(mTheHits[ii]->localError().x(),
									   mTheHits[ii]->localError().y(),
									   mTheHits[ii]->localError().z()),
							    kRichId,
							    mTheHits[ii]->charge(),
							    mTheHits[ii]->maxAmplitude(),
							    static_cast<unsigned char>(0));
		// Add it
		richCollection->addHit(thePersistentHit);
	    }

	    //
	    // Fill in the stuff missing in the constructor
	    //

	    richCollection->getRichHits().back()->setClusterNumber(mTheHits[ii]->clusterNumber());
	    richCollection->getRichHits().back()->setReservedLong(mTheHits[ii]->flags());	    
	    richCollection->getRichHits().back()->local() 
		= StThreeVectorF(mTheHits[ii]->local().x(),
				 mTheHits[ii]->local().y(),
				 mTheHits[ii]->local().z());
	    richCollection->getRichHits().back()->setNumberOfPads(mTheHits[ii]->numberOfPads());
	    
	    richCollection->getRichHits().back()->internal() 
		= StThreeVectorF( mTheHits[ii]->internal().x(),
				  mTheHits[ii]->internal().y(),
				  mTheHits[ii]->internal().z());
	}
    }

    //
    // Store the rich collection into StEvent
    { LOG_INFO << "Write to StEvent" << endm; }
    //PR(richCollection);
    //PR(mEvent);
    mEvent->setRichCollection(richCollection);
    //PR(mEvent->richCollection());

    //
    // Pass the collection via the data set now as well
    // This should be removed at a later date
    //
    AddData(new St_ObjectSet("StRichEvent", richCollection,0));
    if (mEvent->softwareMonitor()) {
	
	StRichSoftwareMonitor* theMonitor = mEvent->softwareMonitor()->rich();
	if(theMonitor) {
	    theMonitor->setNumberOfPixels(static_cast<Long_t>(thePixels.size()));
	    theMonitor->setNumberOfClusters(theClusters.size());
	    theMonitor->setNumberOfHits(mTheHits.size());
	    theMonitor->setTotalCharge(totalCharge);
	}
	else {
	  { LOG_ERROR <<"StRichSoftwareMonitor Does not Exist" << endm; }
	}
    }
    else {
	  { LOG_ERROR <<"StRichSoftwareMonitor Does not Exist" << endm; }
    }
    
}
//-----------------------------------------------------------------
void StRchMaker::PrintInfo() 
{
  { LOG_INFO <<"* $Id: StRchMaker.cxx,v 2.16 2007/04/27 11:26:45 hippolyt Exp $\n"<<endm; }
}

//-----------------------------------------------------------------


Int_t StRchMaker::Finish() {

  { LOG_INFO << "StRchMaker::Finish() - Delete the cluster finder" << endm; }
    delete mClusterFinder;
    mClusterFinder = 0;
	
#ifdef RCH_HISTOGRAM
  { LOG_INFO << "Close the Histogram files!!!!!!" << endm; }
    mRchNTupleFile->Write();
    mRchNTupleFile->Close();
#endif
    return StMaker::Finish();
}

void StRchMaker::setPedestalSubtract (int v, const char* file="/home/daq/data/crams_1172044.ped") 
{
    mPedestalSubtract = v;
    if (mPedestalSubtract) {
	mPedestalFile = file;
    }
    
}



void StRchMaker::clearPadMonitor(){
#ifdef RICH_WITH_PAD_MONITOR
    StRichPadMonitor* thePadMonitor = StRichPadMonitor::getInstance(mGeometryDb);
    thePadMonitor->clearAll();
#endif  
}

/****************************************************************************
 *
 * $Log: StRchMaker.cxx,v $
 * Revision 2.16  2007/04/27 11:26:45  hippolyt
 * Star logger recommendations
 *
 * Revision 2.15  2003/09/24 02:20:08  hippolyt
 * init pointers and arrays
 *
 * Revision 2.14  2003/09/23 03:31:15  jeromel
 * Get read of a READ_UNINIT_MEM(read) [zeroed in constructor]
 *
 * Revision 2.13  2003/09/02 17:58:51  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.12  2002/02/23 02:25:08  dunlop
 * Removed the junk data at the end of a saturated pixel before
 * filling into the clusterfinder.  If pixel has value > 1023, make
 * it equal to 1024.
 *
 * Revision 2.11  2002/02/22 21:55:22  dunlop
 * Fixed bug in saturated pad.  Wasn't being filled for one type of data
 *
 * Revision 2.10  2002/02/22 21:46:46  dunlop
 * Remove picket fence pixels from cluster finding once and for all.
 * Switchable.
 *
 * Revision 2.9  2002/02/03 20:09:00  lasiuk
 * Embedded Data will be searched for in the richmixer/.data/richMixedEvent
 * data set
 *
 * Revision 2.8  2001/10/21 21:52:51  dunlop
 * Protection against null event->softwareMonitor()
 *
 * Revision 2.7  2001/09/27 00:53:54  perev
 * TObjectSet is not an owner
 *
 * Revision 2.6  2001/02/07 16:06:38  lasiuk
 * adc decoder modified for 11bit check
 * this-> for internal calls
 * hit ntuple extended to include pads in hit
 *
 * Revision 2.5  2000/11/30 23:27:03  lasiuk
 * change the hardware position of the hit to kRichId
 *
 * Revision 2.4  2000/11/01 16:50:32  lasiuk
 * set the number of pads used in constructing a hit
 *
 * Revision 2.3  2000/09/29 18:59:31  lasiuk
 * addition of software monitor
 * write flags in persistent hit (reservedLong)
 * Histodefintions
 *
 * Revision 2.2  2000/09/13 21:14:20  lasiuk
 * pause commented
 *
 * Revision 2.1  2000/09/13 21:01:12  lasiuk
 * adjust to interface of new CF
 *
 * Revision 2.0  2000/08/09 16:22:11  gans
 * Cosmetic Changes. Naming convention for TDrawable objects
 *
 * Revision 1.25  2000/06/16 20:52:14  dunlop
 * Fixed another segfault when dataset not present
 *
 * Revision 1.24  2000/06/16 20:34:11  dunlop
 * Fixed segfault again.  Was stomped last checkin
 *
 * Revision 1.23  2000/06/16 02:05:38  lasiuk
 * include paths; drawing cleanup macros
 *
 * Revision 1.21  2000/06/13 18:13:59  dunlop
 * Commented out verbosity under real conditions
 *
 * Revision 1.20  2000/06/01 21:10:40  dunlop
 * filled cluster piece not in ctor
 *
 * Revision 1.19  2000/05/31 19:26:15  dunlop
 * Filling non-ctor entries in persistent hits + support for this
 *
 * Revision 1.18  2000/05/23 16:49:51  lasiuk
 * writing to StEvent/StRichCollection
 *
 * Revision 1.17  2000/05/18 21:57:19  lasiuk
 * dev patch
 *
 * Revision 1.16  2000/05/18 11:42:20  lasiuk
 * mods for pre StEvent writing
 *
 * Revision 1.15  2000/04/05 21:25:18  lasiuk
 * with CF
 *
 * Revision 1.14  2000/03/12 23:49:26  lasiuk
 * order of arguments for the reader
 *
 * Revision 1.13  2000/02/21 23:20:10  lasiuk
 * debug output formatting and reduce output to screen
 *
 * Revision 1.12  2000/02/14 20:50:29  lasiuk
 * use DAQ/sim interface with a switch settable at the c'tor
 *
 * Revision 1.11  2000/01/12 16:52:58  lasiuk
 * comment out assert statement
 *
 * Revision 1.10  2000/01/11 21:18:04  lasiuk
 * Fills new dst_rch_pixel;
 * debug macros;
 * used in first DAQ data
 *
 * Revision 1.9  1999/09/24 01:23:22  fisyak
 * Reduced Include Path
 **************************************************************************/
