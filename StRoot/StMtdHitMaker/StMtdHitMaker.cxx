/***************************************************************************
 *
 * $Id: StMtdHitMaker.cxx,v 1.9 2013/04/25 14:52:06 geurts Exp $ 
 *
 * Author: Frank Geurts (Rice)
 ***************************************************************************
 *
 * Description:  Create the MTD data coming from the RTS_READER
 *
 * Input:  RTS_Reader
 * Output: MTD StEvent objects
 *
 ***************************************************************************
 *
 **************************************************************************/
#include "StMtdHitMaker.h"
#include "StEventTypes.h"
#include "StEvent/StMtdCollection.h"
#include "StEvent/StMtdHeader.h"
#include "StEvent/StMtdRawHit.h"
#include "StEvent/StEvent.h"
#include "StDAQMaker/StDAQReader.h"
#include "StRtsTable.h"
#include "DAQ_MTD/daq_mtd.h"
#include "StBTofUtil/StBTofINLCorr.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuMtdCollection.h"
#include "StMuDSTMaker/COMMON/StMuMtdRawHit.h"
#include "StMuDSTMaker/COMMON/StMuMtdHeader.h"
ClassImp(StMtdHitMaker);

//_____________________________________________________________
	StMtdHitMaker::StMtdHitMaker(const char *name):StRTSBaseMaker("mtd",name)
	, mStEvent(0),fMtd(0)
	, mNValidTrays(-1)          //! number of valid MTD trays
															 , mMtdCollection(0)        //! pointer to StMtdCollection
{
	Int_t mStripMap[24]={21,12,32,20,14,35,25,13,30,24,11,31,34,22,10,37,27,17,33,23,16,36,26,15};
	for(int i=0;i<24;i++) mtdStrip[i] = mStripMap[i];
	mUseMuDst = 0;
	mTriggerWndSelection = kFALSE;
	hxhyhz=new TH3D("hxhyhz","hxhyhz",10,19.5,29.5,10,-0.5,9.5,30,-0.5,29.5);
	LOG_DEBUG << "StMtdHitMaker::ctor"  << endm;
}


//_____________________________________________________________
StMtdHitMaker::~StMtdHitMaker() 
{ }


//_____________________________________________________________
void StMtdHitMaker::Clear(Option_t* option)  { 
	MtdLeadingHits.clear();
	MtdTrailingHits.clear();
	memset(mTriggerTimeStamp,0,2);
}


//_____________________________________________________________
Int_t StMtdHitMaker::Init() {
	Clear("");
	/// Initialize Tray-to-Tdig map (-1 means no TDIG board)
	memset(mTray2TdigMap,-1,sizeof(mTray2TdigMap));
	memset(mTrayId,0,sizeof(mTrayId));
	memset(mTdigId,0,sizeof(mTdigId));

	for(Int_t i=0;i<mNALLTRAY;i++){
		//int backleg = i/5+1;
		//int tray = i%5+1;
		mTriggerTimeWindow[i][0] = 2600;
		mTriggerTimeWindow[i][1] = 2900;
	}
	return kStOK;
}


//_____________________________________________________________
Int_t StMtdHitMaker::InitRun(Int_t runnumber) {
	/// Find out what year we're in
        mYear= (Int_t)(runnumber/1000000 -1);

	/// The Run-12 entries will all move to the database
	if (mYear == 12){
		/// TDIG/tray/backleg mapping
		/// note: index runs from 0-29 for backlegs 1-30, same for tray#
		mTray2TdigMap[25][1] = 0; /// backleg #26 (3 trays)
		mTray2TdigMap[25][2] = 1;
		mTray2TdigMap[25][3] = 4;

		mTray2TdigMap[26][0] = 0; /// backleg #27 (5 trays)
		mTray2TdigMap[26][1] = 1;
		mTray2TdigMap[26][2] = 2;
		mTray2TdigMap[26][3] = 5;
		mTray2TdigMap[26][4] = 4;

		mTray2TdigMap[27][0] = 0; /// backleg #28 (5 trays)
		mTray2TdigMap[27][1] = 1;
		mTray2TdigMap[27][2] = 2;
		mTray2TdigMap[27][3] = 5;
		mTray2TdigMap[27][4] = 4;

		/// Run-12 TrayId map (this follows the UT-Austin 200-scheme)
		mTrayId[25][1] = 200; /// backleg #26 (3 trays)
		mTrayId[25][2] = 211;
		mTrayId[25][3] = 210;

		mTrayId[26][0] = 212; /// backleg #27 (5 trays)
		mTrayId[26][1] = 207;
		mTrayId[26][2] = 206;
		mTrayId[26][3] = 204;
		mTrayId[26][4] = 209;

		mTrayId[27][0] = 208; /// backleg #28 (5 trays)
		mTrayId[27][1] = 205;
		mTrayId[27][2] = 202;
		mTrayId[27][3] = 201;
		mTrayId[27][4] = 203;

		/// TrayID/TDIGId mapping
		/// one[0] TDIG per MTD tray (data distilled from UT Austin database, cf. run12/INL/tdigs_120106.txt) 
		Int_t mTdigIdRun12[13] = { 494, 1150, 1151, 1152, 1153, 1141, 1143, 1149, 1155, 1134, 1136, 1140, 1145};
		for (int i=0;i<13;i++) mTdigId[i]=mTdigIdRun12[i];
	}
	else if(mYear==11){ 
		Int_t mTdigIdRun11[5] = { 1049, 1048, 1138, 1161, 1163};
		for (int i=0;i<5;i++) mTdigId[i]=mTdigIdRun11[i];
	}
	else if(mYear==13){ 
		for(int i=0;i<30;i++){
			if(i<8||i==9||i==21||i>23){
				mTray2TdigMap[i][0] = 0; /// backleg (5 trays)
				mTray2TdigMap[i][1] = 1;
				mTray2TdigMap[i][2] = 2;
				mTray2TdigMap[i][3] = 5;
				mTray2TdigMap[i][4] = 4;
			}
		}
	}
	else{
		LOG_INFO << "No InitRun for Run " << mYear << endm;
	}

	/// INL Table provided by TOF
	LOG_DEBUG << "Initializing INL table:" << endm;
	mINLCorr = new StBTofINLCorr();
	mINLCorr->initFromDbase(this);

	return kStOK;
}


//_____________________________________________________________
Int_t StMtdHitMaker::FinishRun(Int_t runnumber) {
	/// clean up several maps
	memset(mTray2TdigMap,-1,sizeof(mTray2TdigMap));
	memset(mTrayId,0,sizeof(mTrayId));
	memset(mTdigId,0,sizeof(mTdigId));

	if(mINLCorr) delete mINLCorr;
	mINLCorr = 0;

	return kStOK;
}


//-------------------------------------------------------------
Int_t StMtdHitMaker::Finish() {
	Clear("");
	return kStOK;
}


//_____________________________________________________________
/*!
 * This method is to obtain the mtdCollection from StEvent.
 * If StEvent is in the chain, retrieve it; if no StEvent in the chain,
 * a new StEvent is created.
 */
StMtdCollection *StMtdHitMaker::GetMtdCollection() {
	/// Get StEvent if any at once
	StMtdCollection *mtdCollection = 0;
	mStEvent = dynamic_cast<StEvent *> (GetInputDS("StEvent"));

	if (mStEvent) {
		mtdCollection = mStEvent->mtdCollection();

		/// Need to create the hit collection
		if ( !mtdCollection )  {
			///  Save the hit collection to StEvent
			mtdCollection = new StMtdCollection();
			mStEvent->setMtdCollection(mtdCollection);
			if(mUseMuDst){
				StMuDstMaker *mMuDstMaker = (StMuDstMaker *)GetMaker("MuDst");
				if(!mMuDstMaker){
					LOG_INFO << "No MuDstMaker ..bye-bye..."<<endm;
					return mtdCollection;
				}
				StMuDst* mMuDst=mMuDstMaker->muDst();
				if(!mMuDst){
					LOG_INFO << "No Mudst ... bye-bye" <<endm;
					return mtdCollection;
				}
				StMuMtdCollection* MuMtdCollection = mMuDst->MtdCollection();
				StMuMtdHeader* MuMtdHeader = MuMtdCollection->mtdHeader();
				StMtdHeader *MtdHeader = new StMtdHeader();
				for(int i=0;i<nTHUB;i++){ 
					mTriggerTimeStamp[i] = MuMtdHeader->triggerTime(i+1);
					MtdHeader->setTriggerTime(MuMtdHeader->triggerTime(i+1),i+1);
				}
				mtdCollection->setHeader(MtdHeader);
				for(Int_t i=0;i<MuMtdCollection->rawHitsPresent();i++){
					StMuMtdRawHit* ahit= MuMtdCollection->RawMtdHit(i);
					mtdCollection->addRawHit(new StMtdRawHit(ahit->flag(),ahit->backleg(),ahit->channel(),ahit->tdc()));
				}
			}
		}
	}
	else {LOG_WARN << "No StEvent found" << endm; }

	return mtdCollection;
}


//_____________________________________________________________
StRtsTable *StMtdHitMaker::GetNextRaw(int sec) {
	assert(0 && "RTS doesn't provide sector by sector legacy mtd banks yet");
	StRtsTable *daqMtdTable = GetNextLegacy(sec);
	if (daqMtdTable) {
		fMtd = (mtd_t*)*DaqDta()->begin();
	}
	return daqMtdTable;
}


//_____________________________________________________________
StRtsTable *StMtdHitMaker::GetNextRaw() {
	/// Query  RTS/mtd/raw cluster data from DAQ system
	LOG_DEBUG  << "GetNextRaw()" << endm;

	StRtsTable *daqMtdTable = GetNextLegacy();
	if (daqMtdTable) {
		fMtd = (mtd_t*)*DaqDta()->begin();
	}
	return daqMtdTable;
}


//_____________________________________________________________
Int_t StMtdHitMaker::Make() {

	mMtdCollection = GetMtdCollection();
	LOG_DEBUG << " getting the mtd collection " << mMtdCollection << endm;
	if (mMtdCollection) {
		if(!mUseMuDst){
			if ( GetNextRaw() ) {
				/// Unpack MTD raw data from daq structure
				int errorType=UnpackMtdRawData(); 
				if(errorType>0) { 
					LOG_WARN<<"MTD_READER::Unpack MTD Data ERROR!"<<endm;
				}

			}
		}
		fillMtdRawHitCollection();
		fillMtdHeader(); 
		fillMtdHitCollection();
		fillStEvent();
	}
	return kStOk;
}


//____________________________________________________
/*!
 * The unpack function for MTD daq data. 
 * Please refer to the MTD fiber data format document.
 */
//____________________________________________________
Int_t StMtdHitMaker::UnpackMtdRawData() {
	if(!fMtd) return 1;    /// fail to unpack MTD data

	/// Initialize raw hits vector.
	MtdLeadingHits.clear();
	MtdTrailingHits.clear();

	/// Loop over MTD THUBs (each connect by fiber to an RDO
	for(int ifib=0;ifib<nTHUB;ifib++){
		int nword=fMtd->ddl_words[ifib];
		if(nword <= 0) continue;
		int halfbacklegid    = -99;
		int backlegid        = -99;
		mTriggerTimeStamp[ifib] = 0;

		LOG_DEBUG << "RDO " << ifib+1 << ": words " << fMtd->ddl_words[ifib] << endm;

		/// process data word seperately, get TDC information from data words.
		for (int iword=0;iword<nword;iword++) {
			unsigned int dataword=fMtd->ddl[ifib][iword];
			LOG_DEBUG << "DATAWORD: 0x" << hex << dataword << dec << endm;

			/// Trigger time
			if( (dataword&0xF0000000)>>28 == 0x2) {
				// Save the first trigger time in each fiber
				if(mTriggerTimeStamp[ifib]==0) mTriggerTimeStamp[ifib] = dataword;
				continue; 
			}

			if( (dataword&0xF0000000)>>28 == 0xD) continue;  /// header tag word
			if( (dataword&0xF0000000)>>28 == 0xE) continue;  /// TDIG separator word
			if( (dataword&0xF0000000)>>28 == 0xA) continue;   /// header trigger data flag

			/// geographical data
			if( (dataword&0xF0000000)>>28 == 0xC) {
				halfbacklegid =  dataword&0x01;    
				backlegid     = (dataword&0x0FE)>>1;
				continue;
			}
			// range checks
			if(halfbacklegid<0) continue;
			if(backlegid<1 || backlegid >124) {
				LOG_ERROR<<"StMtdHitMaker::DATA ERROR!! unexpected backlegid ! "<<endm;
				continue;
			}


			if( (dataword&0xF0000000)>>28 == 0x6) continue; //error

			/// Look for edge type (4=leading, 5=trailing)
			int edgeid =int( (dataword & 0xF0000000)>>28 );
			if((edgeid !=4) && (edgeid!=5)) continue;   /// not leading or trailing 

			/// From here on assume TDC data, and decode accordingly

			/// decode TDIG-Id ...
			int tdcid=(dataword & 0x0F000000)>>24;  /// range: 0-15

			/// MTD backlegs 27/28
			int tdigid=((tdcid & 0xC)>>2) + halfbacklegid*4;

			/// decode HPTDC id ...
			//int ihptdcid = tdcid&0x3;

			/// decode TDC channel ...
			int tdcchan=(dataword&0x00E00000)>>21; /// tdcchan range: 0-7

			/// decode TDC time bin ...
			unsigned int timeinbin = ((dataword&0x7ffff)<<2)+((dataword>>19)&0x03);  /// time in tdc bin


			/// lookup corresponding tray# for TDIG-Id
			int itray;
			for (itray=1;itray<=5;itray++){
				if (mTray2TdigMap[backlegid-1][itray-1] == tdigid) break;
			}

			/// Fill MTD raw hit structures
			MtdRawHit temphit={0};
			memset(&temphit,0,sizeof(temphit));
			temphit.fiberid = (UChar_t)ifib;
			temphit.backlegid  = (UChar_t)backlegid;
			temphit.tdc     = timeinbin;
			/// global channel number here, 
			if (mYear<12)
				temphit.globaltdcchan = (UChar_t)(tdcChan2globalStrip11(tdigid,tdcid,tdcchan,backlegid)); 
			else
				temphit.globaltdcchan = (UChar_t)(tdcChan2globalStrip(itray, tdigid,tdcid,tdcchan)); 
			temphit.dataword      = dataword;

			if(edgeid == 4) {     /// leading edge data
				MtdLeadingHits.push_back(temphit);
			} else if (edgeid==5){     /// trailing edge data
				MtdTrailingHits.push_back(temphit);
			}  else {
				LOG_WARN<<"StMtdHitMaker::Unknown TDC data type ! "<<endm;
				continue;
			}
			/// end of unpack all data words.        
		}  /// end loop data words    
		///
	} /// end loop fibers
	///
	return -1;
}


//____________________________________________
/*!
 * Map TDC channel to a global strip coordinate (Run 12 and later)
 */
Int_t StMtdHitMaker::tdcChan2globalStrip(int itray, int tdigBoardId, int tdcId, int tdcChan) {

	/// Make sure this mapping is not called for older Runs
	if (mYear<12){
		LOG_WARN << "calling Run12++ tdc mapping for Run" << mYear << ". Not good." << endm;
	}

	Int_t globalStripId=-1;
	int Hnum=tdcId%4+1;
	int globalTdcChan=Hnum*10+tdcChan;
	for(int i=0;i<24;i++){
		if(mtdStrip[i]==globalTdcChan) {globalStripId=i+1;break;}
	}
	if(tdigBoardId>3)
		globalStripId = (globalStripId>12)? globalStripId-12:globalStripId+12;

	/// offset stripId by trayId
	globalStripId += (itray-1)*24;

	return globalStripId;
}


//____________________________________________
/*!
 * Map TDC channel to a global strip coordinate (prior to Run 12)
 */
Int_t StMtdHitMaker::tdcChan2globalStrip11(int tdigBoardId,int tdcId,int tdcChan,int backlegId) {

	/// This function is only useful before Run 12.
	if (mYear>11) {
		LOG_WARN << "calling pre-Run12 tdc mapping for Run" << mYear << ". Not good." << endm; 
	}

	/// 
	Int_t globalStripId=-1;
	if(backlegId==26){
		if (tdcId>3) tdcId=tdcId-4; //scale to H#
		int globalTdcChan=(tdcId+1)*10+tdcChan;
		for(int i=0;i<24;i++){
			if(mtdStrip[i]==globalTdcChan) {globalStripId=i+1; break;}
		}
	}

	if(backlegId==1){
		int globalTdcChan=(tdcId+1)*10+tdcChan;
		int mtdStripRun11[18]=  {34,22,10,37,27,17, 32,20,30,24,11,31, 33,23,16,36,26,15};
		for(int i=0;i<18;i++){
			if(mtdStripRun11[i]==globalTdcChan) {globalStripId=i+1;break;}
		}
	}

	return globalStripId;
}
//____________________________________________
/*!
 * Fill the data from DAQ into MtdRawHit Collection in StEvent
 */
void StMtdHitMaker::fillMtdRawHitCollection() {
	/// MtdRawdata collection.
	if(!mUseMuDst){
		for (unsigned int i=0;i<MtdLeadingHits.size();i++){  ///  Leading Edge
			char           flag   = (+1)*(MtdLeadingHits[i].fiberid+1);
			unsigned char  backlegid = MtdLeadingHits[i].backlegid;
			unsigned char  chn    = MtdLeadingHits[i].globaltdcchan;
			unsigned int   tdc    = MtdLeadingHits[i].tdc;
			LOG_DEBUG << " mtd raw hit LE flag:" << (short)flag << " backlegid:"<< (short)backlegid << " chn:" << (short)chn << " tdc:"<< tdc << endm; 
			mMtdCollection->addRawHit(new StMtdRawHit(flag,backlegid,chn,tdc));
		}

		for (unsigned int i=0;i<MtdTrailingHits.size();i++){  ///  Trailing Edge
			char           flag   = (-1)*(MtdTrailingHits[i].fiberid+1);
			unsigned char  backlegid = MtdTrailingHits[i].backlegid;
			unsigned char  chn    = MtdTrailingHits[i].globaltdcchan;
			unsigned int   tdc    = MtdTrailingHits[i].tdc;
			LOG_DEBUG << " mtd raw hit TE flag:" << (short)flag << " backlegid:"<< (short)backlegid << " chn:" << (short)chn << " tdc:"<< tdc << endm; 
			mMtdCollection->addRawHit(new StMtdRawHit(flag,backlegid,chn,tdc));
		} 
	}else{

		MtdLeadingHits.clear();
		MtdTrailingHits.clear();
		StSPtrVecMtdRawHit& mtdRawHitVec = mMtdCollection->mtdRawHits();
		for(unsigned int i=0;i<mtdRawHitVec.size();i++){
			StMtdRawHit* aRawHit=mtdRawHitVec[i];
			MtdRawHit rawHit;
			rawHit.tdc=aRawHit->tdc();
			rawHit.backlegid=aRawHit->backleg();
			rawHit.fiberid=aRawHit->fiberId();
			rawHit.globaltdcchan=aRawHit->channel();
			if(aRawHit->flag()>0){
				MtdLeadingHits.push_back(rawHit);
			}
			else{
				MtdTrailingHits.push_back(rawHit);
			}
		}
	}

	fillMtdSingleHits();
}
//____________________________________________
/*!
 * Fill the data into MtdSingleHitVec in StEvent
 */
void StMtdHitMaker::fillMtdSingleHits() {

	for(int i=0;i<mNALLTRAY;i++) mSingleHitVec[i].clear();
	for(UInt_t i=0;i<MtdLeadingHits.size();i++){
		unsigned char  ifiber    = MtdLeadingHits[i].fiberid;
		int  ibackleg = MtdLeadingHits[i].backlegid;
		unsigned char  chn    = MtdLeadingHits[i].globaltdcchan;
		unsigned int   tdc    = MtdLeadingHits[i].tdc;
		int itray = (chn-1)/24+1;
		int ichan = (chn-1)%24;
		if (ibackleg>mNBACKLEG || itray<=0 || itray>mNTRAY || ichan<0 || ichan>=mNCHAN || ifiber>mNFIBER ) {
			LOG_FATAL << " StMtdHitMaker::fillMtdSingleHits() "
				<< ": ibackleg=" << ibackleg 
				<< ": itray=" << itray 
				<< ": ichan=" << ichan 
				<< ": ifiber=" << ifiber
				<< endm;
			continue;
		}
		bool iexist = kFALSE;
		int igtray = (ibackleg-1)*mNTRAY+itray;
		for(size_t ii=0; ii<mSingleHitVec[igtray-1].size(); ii++) {
			if(ibackleg==mSingleHitVec[igtray-1][ii].backleg&&itray==mSingleHitVec[igtray-1][ii].tray && ichan==mSingleHitVec[igtray-1][ii].channel) {
				iexist = kTRUE;
				break;
			}
		}
		if(iexist) continue;
		MTDSingleHit aSingleHit;
		aSingleHit.fiberId = ifiber;
		aSingleHit.backleg = ibackleg;
		aSingleHit.tray 	 = itray;
		aSingleHit.channel = ichan;
		aSingleHit.leadingEdgeTime.push_back(tdc);
		for(size_t j=i+1;j<MtdLeadingHits.size();j++) {
			unsigned char  jchn    = MtdLeadingHits[j].globaltdcchan;
			int jbackleg = MtdLeadingHits[j].backlegid;
			int jtray = (jchn-1)/24+1; 
			int jchan = (jchn-1)%24; 
			int jtdc  = MtdLeadingHits[j].tdc; 
			if(jbackleg == ibackleg 
					&& jtray == itray
					&& jchan == ichan
					&& jtdc){
				aSingleHit.leadingEdgeTime.push_back(jtdc);
			}
		}
		for(size_t j=0;j<MtdTrailingHits.size();j++) {
			unsigned char  jchn    = MtdTrailingHits[j].globaltdcchan;
			int jbackleg = MtdTrailingHits[j].backlegid;
			int jtray = (jchn-1)/24+1; 
			int jchan = (jchn-1)%24; 
			int jtdc  = MtdTrailingHits[j].tdc; 
			if(jbackleg == ibackleg 
					&& jtray == itray
					&& jchan == ichan
					&& jtdc){
				aSingleHit.trailingEdgeTime.push_back(jtdc);
			}
		}
		if(aSingleHit.trailingEdgeTime.size()) mSingleHitVec[igtray-1].push_back(aSingleHit);
	}

	//debug
	if(Debug()){
	for(int i=0;i<mNALLTRAY;i++){
		for(size_t m=0;m<mSingleHitVec[i].size();m++){
			LOG_INFO<<" idx backleg = "<<i/5+1<<" tray = "<< i%5+1 <<" channel = "<<mSingleHitVec[i][m].channel<<endm;
			LOG_INFO<<" vec backleg = "<<mSingleHitVec[i][m].backleg<<" tray = "<< mSingleHitVec[i][m].tray <<" channel = "<<mSingleHitVec[i][m].channel<<endm;
			LOG_INFO<<" leading tdcs = " <<endm;
			for(size_t j=0;j<mSingleHitVec[i][m].leadingEdgeTime.size();j++) {
				LOG_INFO << " " << mSingleHitVec[i][m].leadingEdgeTime[j] << endm;
			}
			LOG_INFO<<" trailing tdcs = " <<endm;
			for(size_t j=0;j<mSingleHitVec[i][m].trailingEdgeTime.size();j++) {
				LOG_INFO << " " << mSingleHitVec[i][m].trailingEdgeTime[j] <<endm;
			}

		}
	}
	}
}

IntVec StMtdHitMaker::GetValidTrays() {
	IntVec gTrayVec;
	for(int i=0;i<mNALLTRAY;i++){
		if(mSingleHitVec[i].size()>0){
			gTrayVec.push_back(i+1);
		}
	}
	return gTrayVec;
}

IntVec StMtdHitMaker::GetValidChannel(int backleg, int tray) {
	IntVec chanVec;
	int igtray = (backleg-1)*mNTRAY+tray;
	for(size_t i=0 ; i<mSingleHitVec[igtray-1].size() ; i++) {
		LOG_DEBUG<<" GetValidChannel() backleg "<<mSingleHitVec[igtray-1][i].backleg 
			<< " tray "<<mSingleHitVec[igtray-1][i].tray <<endm;
		if( mSingleHitVec[igtray-1][i].tray == tray
				&& mSingleHitVec[igtray-1][i].backleg == backleg )
			chanVec.push_back(mSingleHitVec[igtray-1][i].channel);
	}
	return chanVec;
}

UIntVec StMtdHitMaker::GetLeadingTdc(int backleg, int tray, int channel, bool triggerevent)
{
	UIntVec leTdc;
	int igtray = (backleg-1)*mNTRAY+tray;
	for(size_t i=0 ; i<mSingleHitVec[igtray-1].size() ; i++) {
		if(mSingleHitVec[igtray-1][i].backleg!=backleg) continue;
		if(mSingleHitVec[igtray-1][i].tray!=tray) continue;
		if(mSingleHitVec[igtray-1][i].channel!=channel) continue;
		for(size_t j=0; j<mSingleHitVec[igtray-1][i].leadingEdgeTime.size(); j++) {
			int fiberId = mSingleHitVec[igtray-1][i].fiberId;
			float trgTime = 25.*(mTriggerTimeStamp[fiberId] & 0xfff);
			float timeDiff = mSingleHitVec[igtray-1][i].leadingEdgeTime[j]*25./1024 - trgTime;
			while(timeDiff<0) timeDiff += 51200;
			if(igtray<=mNALLTRAY){  //trays, keep all hits
				if(triggerevent){ 
					LOG_DEBUG << " backleg "<<backleg<<" tray "<<tray<<" index " << igtray-1 << " trigger window "<< mTriggerTimeWindow[igtray-1][0] <<" "<<mTriggerTimeWindow[igtray-1][1]<<endm;
					if(timeDiff>=mTriggerTimeWindow[igtray-1][0]&&timeDiff<=mTriggerTimeWindow[igtray-1][1])
						leTdc.push_back(mSingleHitVec[igtray-1][i].leadingEdgeTime[j]);   
				}
				else {
					leTdc.push_back(mSingleHitVec[igtray-1][i].leadingEdgeTime[j]);
				}
			}
		}
	}
	return leTdc;
}

UIntVec StMtdHitMaker::GetTrailingTdc(int backleg, int tray, int channel, bool triggerevent)
{
	UIntVec teTdc;
	int igtray = (backleg-1)*mNTRAY+tray;
	for(size_t i=0 ; i<mSingleHitVec[igtray-1].size() ; i++) {
		if(mSingleHitVec[igtray-1][i].backleg!=backleg) continue;
		if(mSingleHitVec[igtray-1][i].tray!=tray) continue;
		if(mSingleHitVec[igtray-1][i].channel!=channel) continue;
		for(size_t j=0; j<mSingleHitVec[igtray-1][i].trailingEdgeTime.size(); j++) {
			int fiberId = mSingleHitVec[igtray-1][i].fiberId;
			float trgTime = 25.*(mTriggerTimeStamp[fiberId] & 0xfff);
			float timeDiff = mSingleHitVec[igtray-1][i].trailingEdgeTime[j]*25./1024 - trgTime;
			while(timeDiff<0) timeDiff += 51200;
			if(igtray<=mNALLTRAY){  //trays, keep all hits
				if(triggerevent){ 
					if(timeDiff>=mTriggerTimeWindow[igtray-1][0]&&timeDiff<=mTriggerTimeWindow[igtray-1][1]+40)
						teTdc.push_back(mSingleHitVec[igtray-1][i].trailingEdgeTime[j]);   
				}
				else {
					teTdc.push_back(mSingleHitVec[igtray-1][i].trailingEdgeTime[j]);
				}
			}
		}
	}
	return teTdc;
}


//____________________________________________
/*!
 * Fill the data into MtdHeader in StEvent
 */
void StMtdHitMaker::fillMtdHeader() {
	/// fill the MTD header
	StMtdHeader *mtdHeader = new StMtdHeader();
	for(int i=0;i<nTHUB;i++){
		mtdHeader->setTriggerTime(mTriggerTimeStamp[i], i);
		LOG_DEBUG << "Trigger Time Stamp "<< i+1 <<": " << (unsigned int)mTriggerTimeStamp[i] << endm;
	}
	mMtdCollection->setHeader(mtdHeader);
}


//____________________________________________
/*!
 * Fill the data from MtdRawHit into MtdHit Collection in StEvent
 */
void StMtdHitMaker::fillMtdHitCollection() {

	vector<MTDOneSideHit> mOneSideHits;
	IntVec validtray = GetValidTrays();
	LOG_DEBUG << " Number of fired trays " << validtray.size() <<endm;
	for(size_t i=0;i<validtray.size();i++){
		int igtray = validtray[i];
		int backleg = (igtray-1)/mNTRAY+1;
		int tray = (igtray-1)%mNTRAY+1;
		IntVec validchan = GetValidChannel(backleg,tray);
		LOG_DEBUG << "fired backleg " << backleg << " tray " << tray << endm;
		LOG_DEBUG <<" total channels "<<validchan.size() <<endm;
		for(size_t iv=0;iv<validchan.size();iv++){

			UIntVec leTdc = GetLeadingTdc(backleg, tray, validchan[iv], mTriggerWndSelection);
			UIntVec teTdc = GetTrailingTdc(backleg, tray, validchan[iv], mTriggerWndSelection);

			LOG_DEBUG <<" leTdc.size():"<<leTdc.size()<<" teTdc.size():"<<teTdc.size()<<endl;
			if(!leTdc.size() || !teTdc.size()) continue;
			int chan = validchan[iv]; // global tdc chan (0-23)
			int channel = chan;
			int tdigboardid = getTdigBoardId(backleg,tray,chan);
			int ilocalchan = getLocalTdcChan(backleg,tray,chan);
			unsigned int tmptdc = leTdc[0];
			int bin = tmptdc&0x3ff;
			double tmptdc_f = tmptdc+mINLCorr->getTdigINLCorr(tdigboardid,ilocalchan,bin);
			double letime = tmptdc_f*VHRBIN2PS / 1000.;

			tmptdc = teTdc[0];
			bin = tmptdc&0x3ff;
			tmptdc_f = tmptdc+mINLCorr->getTdigINLCorr(tdigboardid,ilocalchan,bin);
			double tetime = tmptdc_f*VHRBIN2PS / 1000.;

			MTDOneSideHit tmpHit;
			tmpHit.backleg  = backleg;
			tmpHit.tray	  = tray;
			tmpHit.channel  = channel;
			tmpHit.leadingEdgeTime  = letime;
			tmpHit.trailingEdgeTime = tetime;
			mOneSideHits.push_back(tmpHit);
		} // end channel
	} // end tray

	int nMtdHits = 0;
	int nHits = mOneSideHits.size();
	LOG_DEBUG <<" one side hits # = "<<nHits<<endm;
	for(int i=0;i<nHits;i++){
		int iBackLeg = mOneSideHits[i].backleg;
		int iTray	 = mOneSideHits[i].tray;
		int iCell	 = mOneSideHits[i].channel;
		float iLeadingEdgeTime  = mOneSideHits[i].leadingEdgeTime;
		float iTrailingEdgeTime = mOneSideHits[i].trailingEdgeTime;
		for(int j=i+1;j<nHits;j++){
			int jBackLeg = mOneSideHits[j].backleg;
			int jTray	 = mOneSideHits[j].tray;
			int jCell	 = mOneSideHits[j].channel;
			float jLeadingEdgeTime  = mOneSideHits[j].leadingEdgeTime;
			float jTrailingEdgeTime = mOneSideHits[j].trailingEdgeTime;
			if(abs(iCell-jCell)!=12) continue;
			if(iBackLeg != jBackLeg) continue;
			if(iTray  != jTray) continue;
			UChar_t mBackLeg = iBackLeg;
			UChar_t mModule  = iTray;
			UChar_t mCell    = 99;
		    pair<Double_t,Double_t>  mLeadingEdgeTime;
		    pair<Double_t,Double_t>  mTrailingEdgeTime;

			if(iCell<12){ 
				mCell 		= iCell;
				mLeadingEdgeTime.first  = iLeadingEdgeTime;//west 
				mLeadingEdgeTime.second = jLeadingEdgeTime;//east 
				mTrailingEdgeTime.first  = iTrailingEdgeTime;//west 
				mTrailingEdgeTime.second = jTrailingEdgeTime;//east 
			}else{
				mCell 		= jCell;
				mLeadingEdgeTime.first  = jLeadingEdgeTime;//west 
				mLeadingEdgeTime.second = iLeadingEdgeTime;//east 
				mTrailingEdgeTime.first  = jTrailingEdgeTime;//west 
				mTrailingEdgeTime.second = iTrailingEdgeTime;//east 
			}
			StMtdHit* aHit= new StMtdHit();
			aHit->setBackleg(mBackLeg);
			aHit->setModule(mModule);
			aHit->setCell(mCell);
			aHit->setLeadingEdgeTime(mLeadingEdgeTime);
			aHit->setTrailingEdgeTime(mTrailingEdgeTime);
			mMtdCollection->addHit(aHit);
			nMtdHits++;
			LOG_DEBUG << " backleg " << mBackLeg*1 
				<< " module "<< mModule*1
				<< " cell " <<mCell*1 
				<< " leading.first "<<mLeadingEdgeTime.first
				<< " leading.second "<<mLeadingEdgeTime.second
				<< " trailing.first "<<mTrailingEdgeTime.first
				<< " trailing.second "<<mTrailingEdgeTime.second<<endm;
			
		}
	}
	LOG_DEBUG << " matched hits " << nMtdHits <<endm;

}

//_________________________________________________________________________
/*!
 * Fill and store MTD Collections in StEvent. Create MtdCollection if necessary
 */
void StMtdHitMaker::fillStEvent() {

	LOG_DEBUG << "fillStEvent() Starting..." << endm;

	/// make sure we have a mtdcollection
	if(!mMtdCollection){
		LOG_WARN << "No MtdCollection ... creating one in StEvent" << endm;
		mMtdCollection = new StMtdCollection();
		mStEvent->setMtdCollection(mMtdCollection);
	}

	//
	StMtdCollection* mtdCollection = mStEvent->mtdCollection();
	if(mtdCollection){
		if(mtdCollection->rawHitsPresent()) {
			StSPtrVecMtdRawHit& rawMtdVec = mtdCollection->mtdRawHits();
			if(Debug()) {
				LOG_INFO << "MtdRawHitCollection: " << rawMtdVec.size() << " entries" << endm;
				for(size_t i=0;i<rawMtdVec.size();i++) {
					LOG_DEBUG << (*rawMtdVec[i]) << endm;
				}
			}
		}
		else {
			LOG_DEBUG << "No MtdRawHitCollection" << endm;
		}

		if(mtdCollection->hitsPresent()) {
			StSPtrVecMtdHit& mtdVec = mtdCollection->mtdHits();  
			if(Debug()) {
				LOG_INFO << "MtdHitCollection: " << mtdVec.size() << " entries..." << endm;
				for(size_t i=0;i<mtdVec.size();i++) {
					LOG_DEBUG << (*mtdVec[i]) << endm; 
				}
			}  
		}    
		else {
			LOG_DEBUG << "No MtdHitCollection" << endm;
		}

	}
	else {
		LOG_WARN << "No MtdCollection" << endm;
		LOG_WARN << "No MtdRawHitCollection" << endm;
		LOG_WARN << "No MtdHitCollection" << endm;
	}
}


Int_t StMtdHitMaker::getTdigBoardId(Int_t backlegid, Int_t tray, Int_t chn) {

	int   itdigid = mTray2TdigMap[backlegid-1][tray-1]; 

	Int_t tdigboardid = -1;
	if(mYear==11){
		if(backlegid==1){
			if(itdigid==0) tdigboardid = mTdigId[0];
			if(itdigid==4) tdigboardid = mTdigId[1];
		}                           
		else if(backlegid==26) {                  
			if(itdigid==0) tdigboardid = mTdigId[2];
			if(itdigid==1) tdigboardid = mTdigId[3];
			if(itdigid==4) tdigboardid = mTdigId[4];
		}   

	}
	if(mYear==12){

		int itray;
		for (itray=1;itray<=5;itray++){
			if (mTray2TdigMap[backlegid-1][itray-1] == itdigid) break;
		}
		Int_t idx = mTrayId[backlegid-1][itray-1]-200;
		tdigboardid = mTdigId[idx];
	}
	return tdigboardid;
}

Int_t StMtdHitMaker::getLocalTdcChan(Int_t backlegid, Int_t tray, Int_t chn) {
	int   channel = chn;
	int   itdigid = mTray2TdigMap[backlegid-1][tray-1]; 
	if(itdigid>3) channel = (channel>11)?channel-12:channel+12;

	int    iglobalChan = mtdStrip[channel];
	int    ilocalChan = (iglobalChan/10-1)*8+iglobalChan%10;

	return ilocalChan;
}
