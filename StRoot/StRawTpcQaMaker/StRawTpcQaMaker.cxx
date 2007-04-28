//! $Id: StRawTpcQaMaker.cxx,v 1.9 2007/04/28 17:56:47 perev Exp $
//! $Log: StRawTpcQaMaker.cxx,v $
//! Revision 1.9  2007/04/28 17:56:47  perev
//! Redundant StChain.h removed
//!
//! Revision 1.8  2003/09/02 17:58:50  perev
//! gcc 3.2 updates + WarnOff
//!
//! Revision 1.7  2003/07/18 18:31:48  perev
//! test for nonexistance of XXXReader added
//!
//! Revision 1.6  2000/08/04 21:03:47  perev
//! Leaks + Clear() cleanup
//!
//! Revision 1.5  2000/06/20 20:02:20  kathy
//! commented out the SetDirectory for histograms that Sergei put in - otherwise histograms go to never-never land
//!
//! Revision 1.3  2000/06/19 19:01:21  kathy
//! put in Sergei's new versions of the code
//!

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StRawTpcQaMaker.cxx                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <Stiostream.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <math.h>


#include "StRawTpcQaMaker.h"
#include "St_DataSetIter.h"

#include "StDAQMaker/StDAQReader.h"
#include "StDaqLib/TPC/trans_table.hh"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

#include "hardWired.h"

StDAQReader *victorPrelim;
StTPCReader *victor;


ClassImp(StRawTpcQaMaker)
  
//_____________________________________________________________________________

  StRawTpcQaMaker::StRawTpcQaMaker(const char *name):
    StMaker(name)
{
  cout << endl << " in constructor for StRawTpcQaMaker " << endl << endl;
}


//_____________________________________________________________________________

StRawTpcQaMaker::~StRawTpcQaMaker(){}

//_____________________________________________________________________________

Int_t StRawTpcQaMaker::Init(){

  cout <<"  in StRawTpcQaMaker Init"<<endl;

  St_DataSet *herb;
  herb=GetDataSet("StDAQReader");
  assert(herb);

  victorPrelim=(StDAQReader*)(herb->GetObject());
  assert(victorPrelim);
 
//  cout << " herb = " << herb << endl;
//  cout << " victorPrelim = " << victorPrelim <<  endl;

//  cout << " Initializing histograms 1" <<  endl;

   mhist_1 = new TH1F("hist_1","sectors", 25, 0., 25.);   

// create histograms for each sector
   for (int i=0; i< N__SECTORS; i++) { // loop over sectors
    mSector[i]=0;

//    cout << " Initializing histograms set 2  #" << i <<  endl;

      char name[128];
      sprintf(name," Sector# %d ",i);
      mSector[i]= new TH2D(name, name, 182, -.5, 181.5, 45, -0.5, 44.5);
//      mSector[i]->SetDirectory(0);
      mSector[i]->SetOption("LineWidth=2");
      mSector[i]->Sumw2();
    
  } //loop over sectors

  return StMaker::Init();

}

//_____________________________________________________________________________

Int_t StRawTpcQaMaker::Make(){

  cout << endl << " in StRawTpcQaMaker::Make() " << endl << endl;

  //
   TFile *hfiler = new TFile("test.hist.root","recreate");
   hfiler->cd();

  //
  victor=victorPrelim->getTPCReader();
  if(!victor) return kStWarn;


  // Now you have a pointer named "victor".  It is a pointer to class
  // StTPCReader, which is defined in $STAR/StDAQMaker/StDAQReader.h.
  // You can use "victor" as follows:
  //         victor->getPadList(....
  //         victor->getSequences(....

  //
  int npad;
  unsigned char* padList;
  int iret;
  int aPad;
  int nseq;
  TPCSequence* sequence;

  for (int sectorIndex=0; sectorIndex < N__SECTORS; sectorIndex++) {// loop over sectors
    // cout << "working on sector# ";
    //    cout << " " << sectorIndex << "===>"<< endl;


    for ( int r=0; r < ROWS__PER__SECTOR; r++) { // loop over rows
      npad = victor->getPadList(sectorIndex+1, r+1 ,padList);
      mhist_1->Fill((Float_t)(sectorIndex));
      for(int p=0; p<npad; p++) { // loop over pads with data in that row 
	aPad = padList[p]-1;
//	cout << "zero   sector# " << sectorIndex << " row# " << r << " pad# " << aPad << endl; 
	 iret = victor->getSequences(sectorIndex+1, r+1, aPad+1, nseq, sequence);
	if (iret!=0) continue;

	for(int seq=0; seq < nseq; seq++) { //sequences
	  int start  = (sequence+seq)->startTimeBin;
	  int length = (sequence+seq)->Length;
	  //	  int stop   = start+length-1; 
	  //  cout << endl << " startTimeBin " << (sequence+seq)->startTimeBin;
	  //	cout << " Length       " << (sequence+seq)->Length << " : " ;
	  unsigned char*  firstAdc = (sequence+seq)->FirstAdc;
	  for ( int i=0; i < length; i++) {// length
	    unsigned char* adc=firstAdc+i;
	    if (start+i < TIME__BUCKETS) { 
	      mSector[sectorIndex]->Fill(p,r,*adc);
	    }
	  } // length
	} // loop over sequences	
      } // loop over pads
    } // loop over rows 

  }// loop over sectors

  return kStOK;
}
//_____________________________________________________________________________
void StRawTpcQaMaker::PrintInfo(){
  printf("**************************************************************\n");
  printf("* $Id: StRawTpcQaMaker.cxx,v 1.9 2007/04/28 17:56:47 perev Exp $\n");
  printf("**************************************************************\n");
  if (Debug()) StMaker::PrintInfo();
}

//_____________________________________________________________________________

Int_t StRawTpcQaMaker::Finish(){

cout<<" StRawTpcQaMaker::Finish " << endl;

  return kStOK;
}















