/*!
  StChain                                                              

 Main class to control the StChain program.                           
                                                                      
 This class was done on the base of <a href="http://root.cern.ch/root/Atlfast.html"> ATLFAST </a> 
 C++ class library           
 This class is a general framework for programs that needs to:       
    - Initialise some parameters                                      
    - Loop on events                                                  
    - Print results and save histograms, etc                          
                                                                      
 The event processor StChain::Make loops on a list of Makers          
 where each maker performs some task on the event data and generates  
 results.                                                             
 New Makers can be inserted by a user without modifying this class.   
 Note that the order in which the Makers are called is the order      
 of insertion in the list of Makers.                                  
                                                                      
 Makers must derive from the base class StMaker.                      
 StMaker provides a common interface to all Makers.                   
 Each Maker is responsible for defining its own parameters and        
 histograms.                                                          
 Each Maker has its own list of histograms.                           
 The pointer supporting the created object(s) is defined in StMaker   
   GetDataSet() points to a DataSet owned by the Maker                
                                                                      
 The function StChain::Maketree must be called after the creation     
 of the StChain object to create a Root Tree (not yet implemented).   
                                                                      
*/
#define   STAR_TRACKING 1


#include <stdlib.h>
#include <stdio.h>
#include "TROOT.h"
#include "TBrowser.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "StChain.h"
#include "StEvtHddr.h"
#include "StMessMgr.h"
#include "TMemStat.h"

ClassImp(StChain)

//_____________________________________________________________________________
StChain::StChain(const char *name,  const Bool_t UseOwnHeader):
StMaker(name),m_EvtHddr(0)
{
   m_Version     = 100;       //StChain  version number and release date
   m_VersionDate = 180698;
   mNTotal = 0; mNFailed = 0;
   if ( UseOwnHeader || !(dynamic_cast<StEvtHddr*>(GetDataSet("EvtHddr"))))  
			  m_EvtHddr = new StEvtHddr(m_ConstSet); 
   gROOT->GetListOfBrowsables()->Add(this,GetName());

}

//_____________________________________________________________________________
StChain::~StChain()
{
}
//_____________________________________________________________________________
void StChain::Streamer(TBuffer &)
{ Error("Streamer"," attempt to write %s\n ",GetName());
  assert(0);
}
//_____________________________________________________________________________
void StChain::Clear(Option_t *option)
{
 // StartTimer();
 StMaker::Clear(option);
 // TCollection::EmptyGarbageCollection();
 // StopTimer();
}
//_____________________________________________________________________________
Int_t StChain::Finish(){
 // StartTimer();
 if (TestBIT(kFiniEnd)){ 
   Warning("Finish","chain %s.%s Finished twice, Ignore it"
           ,GetName(),ClassName());
   return 1;
 }
 Int_t res = StMaker::Finish();
 // TCollection::EmptyGarbageCollection();
 SetBIT  (kFiniEnd);
 // StopTimer();
 PrintTotalTime();
 return res;
}
//_____________________________________________________________________________
Int_t StChain::Init()
{
 // StartTimer();
 Int_t res = StMaker::Init();
 // StopTimer();
 return res;
}
//_____________________________________________________________________________
Int_t StChain::Make() {
 // StartTimer();
  if (m_EvtHddr) m_EvtHddr->SetProdDateTime();
 Int_t res = StMaker::Make();
 // StopTimer();
 return res;
}
//_____________________________________________________________________________
Int_t StChain::MakeEvent() 
{
  // Make next event from the TBrowser TContextMenu
  Clear();
  return StMaker::IMake(GetNumber()+1);
}
//_____________________________________________________________________________
Int_t StChain::EventLoop(Int_t jBeg,Int_t jEnd, StMaker *outMk) 
{
  TBenchmark evnt;
  int jCur=0,iMake=0;
#ifdef STAR_TRACKING     
// Add a record to MySQL tracking Db     

  LOG_QA << "Events="       << mNTotal
         << ",Failed="      << mNFailed
         << ",StepEventId=" << "'Start'"
         << ",StepContext=" << "'MemUsed',"  << "MessageId='='"
         << ",ProgrammMessage='" << int(TMemStat::Used())
         << "'" << endm;
         
  LOG_QA << "Events="       << mNTotal
         << ",Failed="      << mNFailed
         << ",StepEventId=" << "'Start'"
         << ",StepContext=" << "'ProgSize',"  << "MessageId='='"
         << ",ProgrammMessage='" << int(TMemStat::ProgSize())
         << "'" << endm;
#endif                
  for (jCur=jBeg; jCur<=jEnd; jCur++) {
     evnt.Reset(); evnt.Start("QAInfo:");

     Clear();
     iMake = Make(jCur);

     if (outMk && iMake == kStErr) {/*outMk->IMake(jCur);*/ mNFailed++;}
     if (iMake%10 == kStEOF || iMake%10==kStFatal)	break;
     mNTotal++;
     evnt.Stop("QAInfo:");
     //  evnt.Show("QAInfo:");
     LOG_QA << Form
     /*printf */ ("QAInfo: Done with Event [no. %d/run %d/evt. %d/Date.Time %d.%d/sta %d] Real Time = %10.2f seconds Cpu Time =  %10.2f seconds",
	jCur,GetRunNumber(),GetEventNumber(),GetDate(), GetTime(),
	     iMake,evnt.GetRealTime("QAInfo:"),evnt.GetCpuTime("QAInfo:")) 
     << endm;
#ifdef STAR_TRACKING     
// Add a record to MySQL tracking Db     
  LOG_QA << "Events="       << mNTotal
         << ",Failed="      << mNFailed
         << ",StepEventId=" << "'Finish'"
         << ",RealTime="    << evnt.GetRealTime("QAInfo:")            
         << ",StepContext=" << "'Cpu',"  << "MessageId='='"
         << ",ProgrammMessage='" << evnt.GetCpuTime("QAInfo:")
         << "'" << endm;

  LOG_QA << "Events="       << mNTotal
         << ",Failed="      << mNFailed
         << ",StepEventId=" << "'Finish'"
         << ",RealTime="    << evnt.GetRealTime("QAInfo:")            
         << ",StepContext=" << "'RealTime',"  << "MessageId='='"
         << ",ProgrammMessage='" << evnt.GetRealTime("QAInfo:")
         << "'" << endm;
#endif                
  }

  LOG_QA << Form
 /*printf */ ("QAInfo:EventLoop completed code %d",iMake)
  << endm;
  gSystem->Exec("date");
  TDatime t;
  LOG_QA << Form
 /* printf */ ("QAInfo:Run is finished at Date/Time %i/%i; Total events processed :%i and not completed: %i",
	    t.GetDate(),t.GetTime(),mNTotal,mNFailed)
        << endm;

#ifdef STAR_TRACKING     
// Add a record to MySQL tracking Db     

  LOG_QA << "Events="       << mNTotal
         << ",Failed="      << mNFailed
         << ",StepEventId=" << "'Finish'"
         << ",Cpu="         << evnt.GetCpuTime("QAInfo:")
         << ",RealTime="    << evnt.GetRealTime("QAInfo:")            
         << ",StepContext=" << "'MemUsed',"  << "MessageId='='"
         << ",ProgrammMessage='" << int(TMemStat::Used())
         << "'" << endm;

  LOG_QA << "Events="       << mNTotal
         << ",Failed="      << mNFailed
         << ",StepEventId=" << "'Finish'"
         << ",StepContext=" << "'ProgSize',"  << "MessageId='='"
         << ",ProgrammMessage='" << int(TMemStat::ProgSize())
         << "'" << endm;

#endif                
  fflush(stdout);
  return iMake;
}


// $Id: StChain.cxx,v 1.58 2006/05/16 18:54:23 fine Exp $
// $Log: StChain.cxx,v $
// Revision 1.58  2006/05/16 18:54:23  fine
// fix StChain and MySql
//
// Revision 1.57  2006/05/12 18:48:48  fine
// reshape jobn tracking. remove the redundand table columns
//
// Revision 1.56  2006/05/12 18:08:14  fine
// fix the MySQLAppender problem and re-shape the trakDb messages
//
// Revision 1.55  2006/05/09 23:31:20  fine
// Reshape the job tracking Db tables and add a few LOQ_QA message to record it with the Job tracking Db
//
// Revision 1.54  2006/03/28 02:09:19  fine
// Add SIMS_USER SUMS_AUTHENTICATED_USER SUMS_JOBNAME
//
// Revision 1.53  2006/02/05 01:41:23  fine
// Add the tracking information from the STAR chain
//
// Revision 1.52  2005/08/29 21:42:20  fisyak
// switch from fBits to fStatus for StMaker control bits
//
// Revision 1.51  2005/08/12 21:27:31  perev
// Remove call output in the case or read error
//
// Revision 1.50  2004/11/04 22:26:38  fine
// populate the package with save/restore the logger and edit some messages
//
// Revision 1.49  2004/08/03 17:18:46  perev
// EventLoop corrected according to current policy
//
// Revision 1.48  2002/11/26 02:16:39  perev
// EventLoop added
//
// Revision 1.47  2002/03/12 21:19:00  fisyak
// Set only one StEvtHddr as default option (due to Embedding)
//
// Revision 1.46  2002/02/02 23:31:13  jeromel
// doxygenized. Added some text for the Make() method.
//
// Revision 1.45  2001/04/10 21:38:49  perev
// Maki(int) --> IMake(int)
//
// Revision 1.44  2000/11/27 13:31:23  fisyak
// Add Production time set
//
// Revision 1.43  2000/03/23 00:15:21  fine
// Adjusted to libSTAR for ROOT 2.24
//
// Revision 1.42  1999/12/06 01:57:29  fine
// Time statistic fixed
//
// Revision 1.41  1999/12/03 01:24:39  fine
// Advanced timer has been introduced
//
// Revision 1.40  1999/07/14 15:26:18  fine
// Context menu MakeEvent method has been introduced
//
// Revision 1.39  1999/07/13 02:19:33  perev
// GetCVS,StEvtHddr,etc...
//
// Revision 1.38  1999/07/11 20:40:35  perev
// Move Clear from StChain to StMaker
//
// Revision 1.37  1999/06/11 17:45:56  perev
// assert StMaker::Streamer to forbid to write it
//
// Revision 1.36  1999/03/19 20:30:48  perev
// GetCVSTag introduced
//
// Revision 1.35  1999/03/11 01:23:58  perev
// new schema StChain
//
// Revision 1.23  1998/12/21 19:42:50  fisyak
// Move ROOT includes to non system
//
// Revision 1.22  1998/11/29 20:01:09  fisyak
// Fix typo with Run/run
//
// Revision 1.21  1998/11/25 21:58:21  fisyak
// Cleanup
//
// Revision 1.20  1998/11/22 18:28:05  fisyak
// Add name of tag
//
// Revision 1.19  1998/11/19 01:23:56  fine
// StChain::MakeDoc has been introduced, StChain::MakeDoc has been fixed (see macros/bfc_doc.C macro
//
// Revision 1.18  1998/10/31 00:21:30  fisyak
// Makers take care about branches
//
// Revision 1.17  1998/10/07 18:43:57  perev
// Add Spy classes for Farm Monitor
//
// Revision 1.16  1998/10/06 18:00:26  perev
// cleanup
//
// Revision 1.15  1998/09/23 20:22:51  fisyak
// Prerelease SL98h
//
// Revision 1.14  1998/09/16 14:29:33  love
// St_DataSetIter.h added
//
// Revision 1.13  1998/08/26 12:15:08  fisyak
// Remove asu & dsl libraries
//
// Revision 1.12  1998/08/18 14:05:01  fisyak
// Add to bfc dst
//
// Revision 1.11  1998/08/07 19:34:53  fisyak
// Add St_run_Maker
//
// Revision 1.10  1998/07/23 21:03:30  fisyak
// Add more comments
//
// Revision 1.9  1998/07/23 11:32:11  fisyak
// Add comments
//
// Revision 1.8  1998/07/20 15:08:08  fisyak
// Add tcl and tpt
//
// Revision 1.7  1998/07/19 21:16:29  fisyak
// add log information
//
// Revision 1.6  1998/07/19 21:14:48  fisyak
// add log information
//
