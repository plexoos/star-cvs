/***************************************************************************
 *
 * $Id: StMuDstFilterMaker.cxx,v 1.7 2004/10/21 02:57:25 mvl Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 ***************************************************************************/
#include "StMuDstFilterMaker.h"

#include "StMuDSTMaker/COMMON/StMuTypes.hh"


#include "StEvent/StEventTypes.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <vector>
#include <algorithm>

#define __COMPRESSION__ 9
#define __BUFFER__ 65536
#define __SPLIT__ 99
#define __AUTOSAVE__ 1000000



StMuDstFilterMaker::StMuDstFilterMaker(const char* name) : StMaker(name), mMuDstMaker(0) {
  DEBUGMESSAGE2("");
}

/**
 * Gets pointer to the StMuDstMaker
 * Create output file and the TClonesArrays needed
 */
int StMuDstFilterMaker::Init() {
  if (!mMuDstMaker) throw StMuExceptionNullPointer("pointer to the StMuDstMaker not set",__PRETTYF__);
  mFile = new TFile(mFileName.c_str(),"RECREATE","StMuDst");
  if (!mFile) throw StMuExceptionNullPointer("no file openend",__PRETTYF__);
  mFile->SetCompressionLevel(__COMPRESSION__);
  
  // Create the TClonesArrays
  createArrays();

  // Create a ROOT Tree and one superbranch
  DEBUGMESSAGE2("now create trees and branches");
  
  TBranch* branch;
  mTTree = new TTree("MuDst", "StMuDst",__SPLIT__);
  if (!mTTree) throw StMuExceptionNullPointer("can not create tree",__PRETTYF__);
  mTTree->SetAutoSave(__AUTOSAVE__);  // autosave when 1 Mbyte written
  //  muDst stuff
  DEBUGMESSAGE2("arrays");
  for ( int i=0; i<__NARRAYS__; i++) {
    DEBUGVALUE2(i);
    branch = mTTree->Branch(StMuArrays::arrayNames[i],&mArrays[i], __BUFFER__, __SPLIT__);
  }
  // strange stuff
  DEBUGMESSAGE2("strange arrays");
  for ( int i=0; i<__NSTRANGEARRAYS__; i++) {
    DEBUGVALUE2(i);
    branch = mTTree->Branch(StMuArrays::strangeArrayNames[i],&mStrangeArrays[i],  __BUFFER__, __SPLIT__);
  }
  // emc stuff
  DEBUGMESSAGE2("emc arrays");
  for ( int i=0; i<__NEMCARRAYS__; i++) {
    DEBUGVALUE2(i);
    branch = mTTree->Branch(StMuArrays::emcArrayNames[i],&mEmcArrays[i],  __BUFFER__, __SPLIT__);
  }
  return 0;
}

/**
 * Writes the tree to disk and closes the output file
 */
void StMuDstFilterMaker::close(){
  if (mTTree) mTTree->AutoSave(); 
  mTTree = 0;
  if (mFile) mFile->Close();
  mFile = 0;
}


StMuDstFilterMaker::~StMuDstFilterMaker() { 
  /* no=op */
}
    
 

int StMuDstFilterMaker::Make(){  ///< create a StEvent from the muDst and put it into the .data tree 
    DEBUGMESSAGE1("");
    clear();
    if ( !mMuDstMaker ) return 0;
    
    StMuDst* muDst = mMuDstMaker->muDst();
    if ( !muDst ) return 0;

    /* In this example we want to write only primary tracks with pT>2,
     * the corresponding globals tracks and the event-wise information
     * and the event wise information. We will discarde the event completely if
     * the vertex is not |z|<50cm and if it has no tracks above 2GeV/c
     */
    // this is the function that decides whether the whole event shall be discarded or not
    if ( filter(muDst)==false ) return 0;

    /*
     * Now apply filters to the individual TClonesArrays.
     */

    //the event wise information first
    if ( filter( muDst->event() ) ) { 
	 addType( mArrays[muEvent], *(muDst->event()) );
    }

    //The tracks are the most difficult part, because the different typ of tracks are related via their ids.
    //For all primary tracks that are accepted, I also want to right the global track
    vector<int> ids; 
    int numberOfTracks = 0;
    numberOfTracks = muDst->primaryTracks()->GetEntries();
    for ( int i=0; i<numberOfTracks; i++) {
	StMuTrack* track = muDst->primaryTracks(i);
	if ( filter( track )==true ) {
	    ids.push_back( track->id() );
	    addType( mArrays[muPrimary], *track );
	}
    }
    sort(ids.begin(),ids.end()); // sort so that we can use the fast binary search
    numberOfTracks = muDst->globalTracks()->GetEntries();
    for ( int i=0; i<numberOfTracks; i++) {
	StMuTrack* track = muDst->globalTracks(i);
	if ( binary_search(ids.begin(),ids.end(),track->id()) ) { // if the id is in the list
	    addType( mArrays[muGlobal], *track );
	}
    }
    // primary tracks are pointing to the global tracks, we have to set the indecies right
    StMuDst::fixTrackIndices( mArrays[muPrimary],mArrays[muGlobal] );

    // the emc collection
    StMuEmcCollection* emc = muDst->muEmcCollection();
    if ( filter(emc) ) {
	//addType( mEmcArrays[0], *emc ); this doesn't work since the StMuEmcCollection is not cloneable
    }
    
    // write the event only if it has at least one primary track
    if ( mArrays[muPrimary]->GetEntries()>0) mTTree->Fill();
    return 0;
}

template <class T>
int StMuDstFilterMaker::addType(TClonesArray* tcaTo , T t) {
  int counter =-1;
  if (tcaTo) {
    counter = tcaTo->GetEntries();
    new((*tcaTo)[counter]) T( t );
  }
  return counter;
}


int StMuDstFilterMaker::Finish() { 
    close();
    return 0;
}

/**
 * Clears all TClonesArrays
 */
void StMuDstFilterMaker::clear(){
  DEBUGMESSAGE2("");

  mMuDstMaker->clearArrays();
}

/**
 * Creates all the TClonesArrays.
 * There are three set of arrays: the "regular", the "strangeness" and the "emc" arrays
 */
void StMuDstFilterMaker::createArrays() {
  /// regular stuff
    int dummy;
    for ( int i=0; i<__NARRAYS__; i++) {
	mArrays[i] = 0;
	DEBUGVALUE2(mArrays[i]);
	mArrays[i]= mMuDstMaker->clonesArray(mArrays[i],StMuArrays::arrayTypes[i],StMuArrays::arraySizes[i],dummy);
	DEBUGVALUE2(mArrays[i]);
    }
    /// from strangeness group
    for ( int i=0; i<__NSTRANGEARRAYS__; i++) {
	mStrangeArrays[i] = 0;
	mStrangeArrays[i]= mMuDstMaker->clonesArray(mStrangeArrays[i],StMuArrays::strangeArrayTypes[i],StMuArrays::strangeArraySizes[i],dummy);
    }
    /// from emc group
    for ( int i=0; i<__NEMCARRAYS__; i++) {
	mEmcArrays[i] = 0;
	mEmcArrays[i]= mMuDstMaker->clonesArray(mEmcArrays[i],StMuArrays::emcArrayTypes[i],StMuArrays::emcArraySizes[i],dummy);
  }
}

/*
 * Here I define my cuts by specializing the
 * template<class T> bool filter(T*)
 * function
 */
/// and I want to keep all tracks with p > 1 and 0<eta<1
bool StMuDstFilterMaker::filter(StMuTrack* track) {
    return ( track->p().mag()>1. ) && ( fabs(track->eta())<1.0 );
}


ClassImp(StMuDstFilterMaker)

/***************************************************************************
 *
 * $Log: StMuDstFilterMaker.cxx,v $
 * Revision 1.7  2004/10/21 02:57:25  mvl
 * Changed call to getter for StMuEmcCollection
 *
 * Revision 1.6  2004/10/19 01:47:57  mvl
 * Changed calls to clear TClonesArray in StMuDstMaker (not tested ;-()
 *
 * Revision 1.5  2004/02/17 04:56:36  jeromel
 * Extended help, added crs support, restored __GNUC__ for PRETTY_FUNCTION(checked once
 * more and yes, it is ONLY defined in GCC and so is __FUCTION__),  use of a consistent
 * internal __PRETTYF__, return NULL if no case selected (+message) and protected against
 * NULL mChain.
 *
 * Revision 1.4  2003/11/24 23:36:36  laue
 * commented the StMuEmcCollection out
 *
 * Revision 1.3  2003/09/07 03:49:03  perev
 * gcc 3.2 + WarnOff
 *
 * Revision 1.2  2003/08/04 14:38:10  laue
 * Alex Suaide's updated for the EMC. Now EEMC is included.
 *
 * Revision 1.1  2003/04/15 18:48:35  laue
 * Minor changes to be able to filter MuDst.root files and an example
 * how to do this. The StMuDstFilterMaker is just an example, it has to be
 * customized (spoilers, chrome weels, etc.) by the user.
 *
 *
 **************************************************************************/
