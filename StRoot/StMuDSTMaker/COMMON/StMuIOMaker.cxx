/***************************************************************************
 *
 * $Id: StMuIOMaker.cxx,v 1.12 2004/10/19 01:46:46 mvl Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 * Made it integrated to StIOMaker for applying Grid Collector 
 *                                            Wei-Ming Zhang KSU 3/8/2004 
 *
 **************************************************************************/
#include "StarClassLibrary/StTimer.hh"
//#include "StMuEmcUtil.h"
#include "StMuDebug.h"
#include "StMuIOMaker.h"
#include "StMuDst.h"
#include "StMuEvent.h"

#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"

#include "THack.h"
ClassImp(StMuIOMaker)

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuIOMaker::StMuIOMaker(const char* name, const char *ioFile) : 
   StMuDstMaker(name) 
{
  mOutFile	=0;    


  mNumberOfEvents=0; 	//! holds the # of events in the current chain (file)
  mCurrentIndex	=0;   	//! holds the index of the last event read
  mEventCounter	=0;   	//! a counter holding the total # of events read

  mSplit	=99;          
  mCompression	=9;    
  mBufferSize	=65536*4;  
  mMuSave	=0;        
  mBadInFile	=0;       
  mIoMode       =0;
  mCloseWrite	=1;       




}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuIOMaker::~StMuIOMaker() {
  DEBUGMESSAGE("");
  if(mMuSave && !mBadInFile && !mCloseWrite) closeMuWrite();
  DEBUGMESSAGE3("out");
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/**
   The Init() routine is used to establish contact to other makers. As it is STAR 
   habit (but really really bad coding) we identify the 
   other makers by names (instead of passing pointers). Here, the names are 
   hard-wired because they have to be identical to the names 
   the bfc is assining to the makers. Do not alter these names unless you know 
   what you are doing.
*/
int StMuIOMaker::Init(){
  DEBUGMESSAGE("");
  TObjectSet *muDstSet =  AddObj(mStMuDst,".const");   ///< added for Valeri to be able to pick it up in other makers 
  if (muDstSet ) muDstSet ->SetName("MuDst");          ///< added for Valeri to be able to pick it up in other makers 

// borrow StIOMaker::TString TreeName to pass a save flag for MuDst  
  if(fTreeName == "MuSave") mMuSave = true;

  DEBUGVALUE(mMuSave);
  DEBUGMESSAGE2("out");
  return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuIOMaker::Make(){
// same as "Read mode" of StTreeMaker
    int iret=0,ntry=13;
    while(1999) {
      iret = MakeRead();
      if (iret!=kStErr && ntry--) break;
      Warning("Make","%d *** ReadError ***\n",ntry);
    }
    return iret;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/**
	This is the function that actually reads a new event. All other 
	implementations of Make will eventually call this function.
	@param  the index of the event to read
	@return kStEOF if the requested index is greater that the number of events in the chain
	@return kStErr if the requested index does not exists
	@return kStOk if the requested event was read successfully 
	If for any reason no event is read, the pointers to the TClonesArrays 
	are set to 0. It will cause a crash if you will try to access 
	information from the pointer to the StMuDst.  This is done for ,
	so that nobody reads the same (last) event over and over again without 
	noticing. Please check the return values of Make(), only kStOk means a 
	an event was read succesfull and data can be extracted.
 */
int StMuIOMaker::Make(int index){
  DEBUGMESSAGE("");
// as default clear(0)
  //clear();
  DEBUGVALUE3(index);
  mCurrentIndex = index;
  DEBUGVALUE3(mNumberOfEvents);
  if ( mCurrentIndex >= mNumberOfEvents) return kStEOF;
  if ( mCurrentIndex < 0 ) return kStErr;
  if (!mChain) return kStEOF;
  int bytes = mChain->GetEntry(mCurrentIndex);

  DEBUGVALUE(mMuSave);
  if(mMuSave) mOutTree->Fill();
// One event would be skipped in StChain if making increment here!!!!
//  mCurrentIndex++;
  DEBUGVALUE3(bytes);
  mStMuDst->set(this);
  mEventCounter++;
  fillHddr();
  return kStOk;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuIOMaker::Make(int major, int minor){
  DEBUGMESSAGE("");
  DEBUGVALUE3(major);
  DEBUGVALUE3(minor);
  int index = mChain->GetEntryNumberWithIndex(major,minor);
  DEBUGMESSAGE("out");
  return Make(index);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuIOMaker::Make(const StUKey& key){
  DEBUGMESSAGE("");
  int index = mChain->GetEntryNumberWithIndex(key.GetRunId(),key.GetEventId());
  DEBUGVALUE3(index);
  DEBUGMESSAGE("out");
  return Make(index);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuIOMaker::Finish() {
  DEBUGMESSAGE("");
  cout << "###### StMuIOMaker read " << mEventCounter << " events"  << endl;
  if(mMuSave && !mBadInFile && !mCloseWrite) { 
    if (mOutTree) mOutTree->AutoSave(); 
    closeMuWrite();
  }
  DEBUGMESSAGE3("out");
  return kStOK;
}

// implementaion of virtual methods of base StIOInterFace
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Int_t StMuIOMaker:: MakeRead(const StUKey &key) {
    DEBUGMESSAGE("");
    if(!mBadInFile) {
      if(key.IsNull()) { // sequential reading in normal case
         DEBUGMESSAGE("Seq");
         return Make(mCurrentIndex+1);
      }
      else  {            // GC
         DEBUGMESSAGE("GC");
         return Make(key);
      }
    }
    else {               // Bad input file
      DEBUGMESSAGE("BadFile");
      return kStEOF;
    }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// Open() in BASE StIOInterace returns 1999 if not implemented in DERIVED here
Int_t StMuIOMaker::Open(const char*) { 
  DEBUGMESSAGE("");
  mBadInFile = false;
  mCloseWrite = true;
  int iret = openRead();
  if(mMuSave && !mBadInFile) { 
    openMuWrite();
    mCloseWrite = false;
  }
  return iret; 
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// Close() in BASE StIOInterace will quit if not implemented in DERIVED here
void StMuIOMaker::Close(Option_t *) { 
  DEBUGMESSAGE("");
  if(mMuSave && !mBadInFile && !mCloseWrite) closeMuWrite(); 
  closeRead(); 
  DEBUGMESSAGE3("out");
}

// private methods
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int StMuIOMaker::openRead() {
  DEBUGMESSAGE("");
  DEBUGVALUE(fFile.Data());
  mChain = new TChain("MuDst");  

  mChain->Add(fFile.Data());
  DEBUGVALUE3(mChain);

  mNumberOfEvents = (int)mChain->GetEntries();

  if(mNumberOfEvents == 1234567890) {
     cout << "#### " << fFile.Data() << " Corrupted!" << endl;
     mBadInFile = true;
     closeRead();
     return kStOK;
  }

  StTimer timer;
  timer.reset();
  timer.start();

  setBranchAddresses(mChain);
  mChain->BuildIndex("MuEvent.mEventInfo.mRunId","MuEvent.mEventInfo.mId");
  timer.stop();
  cout << " Index of " << mNumberOfEvents << " events buit in " 
       << timer.elapsedTime() << " seconds " << endl;


  mCurrentIndex = -1;
  DEBUGMESSAGE3("out");
  return kStOK;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuIOMaker::openMuWrite() {
  DEBUGMESSAGE("");
  mOutFileName = fFile.Data(); 
  while (mOutFileName.find("/")!=string::npos) {
      int pos = mOutFileName.find("/");
      mOutFileName.erase(0,pos+1);
  }
  int pos = mOutFileName.find("MuDst");
  mOutFileName = mOutFileName.insert(pos,"Sel.");
  DEBUGVALUE(mOutFileName.c_str());

  mOutFile = new TFile(mOutFileName.c_str(),"RECREATE","StMuDst");
  mOutFile->SetCompressionLevel(mCompression);

  TTree *tree;
  tree = mChain->GetTree();
  mOutTree = new TTree("MuDst", "StMuDst", mSplit);
  mOutTree->SetAutoSave(1000000);
  mOutTree = tree->CloneTree(0);

  DEBUGMESSAGE3("out");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void StMuIOMaker::closeMuWrite(){
  DEBUGMESSAGE("");
  cout << __PRETTY_FUNCTION__ << endl;
  if (mOutTree && mOutFile) {
      cout << " ##### " << __PRETTY_FUNCTION__ << " ";
      cout << " ##### " << endl;
      cout << " ##### File=" << mOutFile->GetName() << " "; 
      cout << " ##### " << endl;
      cout << " ##### NumberOfEvents= " << mOutTree->GetEntries() << " ";
      cout << " ##### " << endl;
  }
  StTimer timer;
  timer.reset();
  timer.start();

  if(mOutTree) mOutTree->Write();
  mOutTree = 0;
  if(mOutFile) mOutFile->Close();
  mOutFile = 0;

  timer.stop();
  cout << " Writing took " << timer.elapsedTime() << " seconds " << endl;
  mCloseWrite = true;
  DEBUGMESSAGE3("out");
}
/***************************************************************************
 *
 * $Log: StMuIOMaker.cxx,v $
 * Revision 1.12  2004/10/19 01:46:46  mvl
 * Removed call to StMuDstMaker::clear() in Make() (Obsolete)
 *
 * Revision 1.11  2004/07/02 01:51:09  perev
 * Wei-Ming Zhang developments
 *
 * Revision 1.10  2004/04/20 18:49:16  perev
 * Big reorganization, now StMuIOMkaer inherits from StMuDstMaker
 *
 * Revision 1.5  2004/04/02 03:24:54  jeromel
 * Changes implements PMD and TOF.  TOF is clearly incomplete.
 *
 * Revision 1.4  2004/02/17 04:56:36  jeromel
 * Extended help, added crs support, restored __GNUC__ for PRETTY_FUNCTION(checked once
 * more and yes, it is ONLY defined in GCC and so is __FUCTION__),  use of a consistent
 * internal __PRETTYF__, return NULL if no case selected (+message) and protected against
 * NULL mChain.
 *
 * Revision 1.3  2003/09/11 05:49:20  perev
 * ansi corrs
 *
 * Revision 1.2  2003/09/09 18:16:53  laue
 * StMuIOMaker: embedded documentation added
 * StMuTimer: name of define changed (was same as StTimer)
 *
 **************************************************************************/



