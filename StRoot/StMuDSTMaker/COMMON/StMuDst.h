/***************************************************************************
 *
 * $Id: StMuDst.h,v 1.20 2004/08/25 04:05:56 mvl Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

#ifndef StMuDst_h
#define StMuDst_h

#include "TObject.h"
#include "TClonesArray.h"

class StMuDstMaker;
class StMuEvent;
class StMuTrack;
class StRichSpectra;
class StDetectorState;
class StL3AlgorithmInfo;

class StStrangeEvMuDst;
class StV0MuDst;
class StXiMuDst;
class StKinkMuDst;
class StV0Mc;
class StXiMc;
class StKinkMc;
class StStrangeAssoc;
class TCut;

class StMuEmcCollection;
class StMuPmdCollection;

class StEvent;
class StTrack;
class StTrackGeometry;

class StMuTofHit;
class StTofData;

class StPhysicalHelixD;

#include "TObject.h"
#include "StMuArrays.h"
#include "StMuException.hh"



#define ARRAY(NAME)  static TClonesArray* (NAME)##s() { return tca_##NAME##s;}
#define OBJECT(TYPE,FUNC) static TYPE FUNC##(unsigned int i=0) { if (FUNC##s() && (i<(unsigned int)FUNC##s()->GetEntries()) ) return (##TYPE##)FUNC##s()->UncheckedAt(i); return 0;}

#define DO(TYPE,NAME) ARRAY(NAME)    OBJECT(TYPE,NAME)


/** 
    @class StMuDst
    Top class of the 'dataformat'. This class exists only in memory and is not written/read to/from disk.
    However, this class is used to hold the pointers to all the TClonesArrays that have been read from disk.
    The class is used to navigate within a 'physics' event (to access tracks, detector info, etc).  
    
*/
class StMuDst : public TObject {
public:
  /// constructor
  StMuDst();
  /// set the pointers to the TClonesArrays
  void set(StMuDstMaker* maker);
  /// set the pointers to the TClonesArrays
  void set(TClonesArray**, TClonesArray**, TClonesArray** emc=0, TClonesArray** pmd=0, TClonesArray** tof=0);
  // subhasis: commented to include tof again
  // void set(TClonesArray**, TClonesArray**, TClonesArray** emc=0, TClonesArray** pmd=0);

  /// resets the pointers to the TClonesArrays to 0
  void unset();
  /// checks and if necessary corrects the indecies of elements pointing to each other (e.g., a primary track's index to the corresponding global track)
  static void fixTrackIndices(TClonesArray* primary, TClonesArray* global);
  /// checks and if necessary corrects the indecies of elements pointing to each other (e.g., a primary track's index to the corresponding global track)
  void fixTrackIndices();
  /// creates a StEvent from the StMuDst (this) and returns a pointer to it. (This function is not yet finished)  
  StEvent* createStEvent();
  /// helper function to create a StTrackGeometry
  StTrackGeometry* trackGeometry(int q, StPhysicalHelixD* h);
  /// creates a StTrack from an StMuTrack and return pointer to it
  StTrack* createStTrack(StMuTrack*);

 protected:
  /// array of TClonesArrays
  static TClonesArray** arrays;
  /// array of TClonesArrays for the stuff inherited from the StStrangeMuDst
  static TClonesArray** strangeArrays;
  /// array of TClonesArrays for the stuff inherited from the Emc
  static TClonesArray** emcArrays;
  /// array of TClonesArrays for the stuff inherited from the Pmd 
  static TClonesArray** pmdArrays;
  /// array of TClonesArrays for the stuff inherited from the TOF
  static TClonesArray** tofArrays;

public:
  /// returns pointer to the n-th TClonesArray 
  static TClonesArray* array(int type) { return arrays[type]; }
  /// returns pointer to the n-th TClonesArray from the strangeness arrays
  static TClonesArray* strangeArray(int type) { return strangeArrays[type]; }
  /// returns pointer to the n-th TClonesArray from the emc arrays
  static TClonesArray* emcArray(int type) { return emcArrays[type]; }
  /// returns pointer to the n-th TClonesArray from the pmd arrays
  static TClonesArray* pmdArray(int type) { return pmdArrays[type]; }
  /// returns pointer to the n-th TClonesArray from the tof arrays
  static TClonesArray* tofArray(int type) { return tofArrays[type]; }

  /// returns pointer to the primary tracks list
  static TClonesArray* primaryTracks() { return arrays[muPrimary]; }
  /// returns pointer to the global tracks list
  static TClonesArray* globalTracks() { return arrays[muGlobal]; }
  /// returns pointer to the other tracks list (all tracks that are not flagged as primary of global)
  static TClonesArray* otherTracks() { return arrays[muOther]; }
  /// returns pointer to the l3Tracks list
  static TClonesArray* l3Tracks() { return arrays[muL3]; }
  /// returns pointer to the list of rich spectra
  static TClonesArray* richSpectra() { return arrays[muRich]; }
  /// returns pointer to the list of detector states
  static TClonesArray* detectorStates() { return arrays[muState]; }
  /// returns pointer to list of accepted l3 algorithms 
  static TClonesArray* l3AlgoAccept() { return arrays[muAccept]; }
  /// returns pointer to list rejected l3 algorithms 
  static TClonesArray* l3AlgoReject() { return arrays[muReject]; }

  /// returns pointer to current StMuEvent (class holding the event wise information, e.g. event number, run number)
  static StMuEvent* event() { return (StMuEvent*)arrays[muEvent]->UncheckedAt(0); }
  /// return pointer to i-th primary track 
  static StMuTrack* primaryTracks(int i) { return (StMuTrack*)arrays[muPrimary]->UncheckedAt(i); }
  /// return pointer to i-th global track 
  static StMuTrack* globalTracks(int i) { return (StMuTrack*)arrays[muGlobal]->UncheckedAt(i); }
  /// return pointer to i-th other track  (track that is not flagged as primary of global)
  static StMuTrack* otherTracks(int i) { return (StMuTrack*)arrays[muOther]->UncheckedAt(i); }
  /// return pointer to i-th l3 track
  static StMuTrack* l3Tracks(int i) { return (StMuTrack*)arrays[muL3]->UncheckedAt(i); }
  /// returns pointer to i-th StRichSpectra
  static StRichSpectra* richSpectra(int i) { return (StRichSpectra*)arrays[muRich]->UncheckedAt(i); }
  /// returns pointer to i-th StDetectorState
  static StDetectorState* detectorStates(int i) { return (StDetectorState*)arrays[muState]->UncheckedAt(i); }
  /// returns pointer to i-th accepted StL3AlgorithmInfo
  static StL3AlgorithmInfo* l3AlgoAccept(int i) { return (StL3AlgorithmInfo*)arrays[muAccept]->UncheckedAt(i); }
  /// returns pointer to i-th rejected StL3AlgorithmInfo
  static StL3AlgorithmInfo* l3AlgoReject(int i) { return (StL3AlgorithmInfo*)arrays[muReject]->UncheckedAt(i); }

  /// returns pointer to current StStrangeEvMuDst (class holding the event wise information, e.g. event number, run number)
  static StStrangeEvMuDst* strangeEvent() { return (StStrangeEvMuDst*)strangeArrays[smuEv]->UncheckedAt(0); }
  /// returns pointer to the v0 list
  static TClonesArray* v0s() { return strangeArrays[smuV0]; }
  /// returns pointer to the mc v0 list
  static TClonesArray* v0sMc() { return strangeArrays[smuV0Mc]; }
  /// returns pointer to the v0 association list
  static TClonesArray* v0Assoc() { return strangeArrays[smuV0Assoc]; }
  /// returns pointer to the xi list
  static TClonesArray* xis() { return strangeArrays[smuXi]; }
  /// returns pointer to the mc xi list
  static TClonesArray* xisMc() { return strangeArrays[smuXiMc]; }
  /// returns pointer to the xi association list
  static TClonesArray* xiAssoc() { return strangeArrays[smuXiAssoc]; }
  /// returns pointer to the kink list
  static TClonesArray* kinks() { return strangeArrays[smuKink]; }
  /// returns pointer to the mc kink list
  static TClonesArray* kinksMc() { return strangeArrays[smuKinkMc]; }
  /// returns pointer to the kink association list
  static TClonesArray* kinkAssoc() { return strangeArrays[smuKinkAssoc]; }
  /// returns pointer to the list of strangeCuts
  static TClonesArray* strangeCuts() { return strangeArrays[smuCut]; }
  /// returns pointer to the i-th v0
  static StV0MuDst* v0s(int i) { return (StV0MuDst*)strangeArrays[smuV0]->UncheckedAt(i); }
  static StV0Mc* v0sMc(int i) { return (StV0Mc*)strangeArrays[smuV0Mc]->UncheckedAt(i); }
  static StStrangeAssoc* v0Assoc(int i) { return (StStrangeAssoc*)strangeArrays[smuV0Assoc]->UncheckedAt(i); }
  /// returns pointer to the i-th xi
  static StXiMuDst* xis(int i) { return (StXiMuDst*)(void*)strangeArrays[smuXi]->UncheckedAt(i); }
  static StXiMc* xisMc(int i) { return (StXiMc*)strangeArrays[smuXiMc]->UncheckedAt(i); }
  static StStrangeAssoc* xiAssoc(int i) { return (StStrangeAssoc*)strangeArrays[smuXiAssoc]->UncheckedAt(i); }
  /// returns pointer to the i-th kink
  static StKinkMuDst* kinks(int i) { return (StKinkMuDst*)(void*)strangeArrays[smuKink]->UncheckedAt(i); }
  static StKinkMc* kinksMc(int i) { return (StKinkMc*)strangeArrays[smuKinkMc]->UncheckedAt(i); }
  static StStrangeAssoc* kinkAssoc(int i) { return (StStrangeAssoc*)strangeArrays[smuKinkAssoc]->UncheckedAt(i); }
  /// returns pointer to the i-th stranneCut (of type TCut)
  static TCut* strangeCuts(int i) { return (TCut*)strangeArrays[smuCut]->UncheckedAt(i); }

  /// returns pointer to current StMuEmcCollection
  static StMuEmcCollection* emcCollection() { return (StMuEmcCollection*)emcArrays[muEmc]->UncheckedAt(0); }
  /// returns pointer to current StMuPmdCollection
  static StMuPmdCollection* pmdCollection() { return (StMuPmdCollection*)pmdArrays[muPmd]->UncheckedAt(0); }

  /// returns pointer to the i-th muTofHit
  static StMuTofHit* tofHit(int i) { return (StMuTofHit*)tofArrays[muTofHit]->UncheckedAt(i); }
  /// returns pointer to the i-th tofData
  static StTofData* tofData(int i) { return (StTofData*)tofArrays[muTofData]->UncheckedAt(i); }

  static unsigned int numberOfPrimaryTracks()  { return arrays[muPrimary]->GetEntries(); }
  static unsigned int numberOfGlobalTracks()   { return arrays[muGlobal]->GetEntries(); }
  static unsigned int numberOfOtherTracks()    { return arrays[muOther]->GetEntries(); }
  static unsigned int numberOfL3Tracks()       { return arrays[muL3]->GetEntries(); }
  static unsigned int numberOfRichSpectras()   { return arrays[muRich]->GetEntries(); }
  static unsigned int numberOfDetectorStates() { return arrays[muState]->GetEntries(); }
  static unsigned int numberOfL3AlgoAccepts()  { return arrays[muAccept]->GetEntries(); }
  static unsigned int numberOfL3AlgoRejects()  { return arrays[muReject]->GetEntries(); }
  static unsigned int numberOfV0s()            { return strangeArrays[smuV0]->GetEntries(); }
  static unsigned int numberOfV0sMc()          { return strangeArrays[smuV0Mc]->GetEntries(); }
  static unsigned int numberOfV0Assoc()        { return strangeArrays[smuV0Assoc]->GetEntries(); }
  static unsigned int numberOfXis()            { return strangeArrays[smuXi]->GetEntries(); }
  static unsigned int numberOfXisMc()          { return strangeArrays[smuXiMc]->GetEntries(); }
  static unsigned int numberOfXiAssoc()        { return strangeArrays[smuXiAssoc]->GetEntries(); }  
  static unsigned int numberOfKinks()          { return strangeArrays[smuKink]->GetEntries(); }
  static unsigned int numberOfKinksMc()        { return strangeArrays[smuKinkMc]->GetEntries(); } 
  static unsigned int numberOfKinkAssoc()      { return strangeArrays[smuKinkAssoc]->GetEntries(); }
  static unsigned int numberOfStrangeCuts()    { return strangeArrays[smuCut]->GetEntries(); }

  // tofr
  static unsigned int numberOfTofHit()        { return tofArrays[muTofHit]->GetEntries(); }
  static unsigned int numberOfTofData()       { return tofArrays[muTofData]->GetEntries(); }

  static unsigned int GetNPrimaryTrack()    { return numberOfPrimaryTracks(); }  
  static unsigned int GetNGlobalTrack()     { return numberOfGlobalTracks(); }   
  static unsigned int GetNOtherTrack()      { return numberOfOtherTracks(); }    
  static unsigned int GetNL3Track()         { return numberOfL3Tracks(); }       
  static unsigned int GetNRichSpectra()     { return numberOfRichSpectras(); }   
  static unsigned int GetNDetectorState()   { return numberOfDetectorStates(); } 
  static unsigned int GetNL3AlgoAccept()    { return numberOfL3AlgoAccepts(); }  
  static unsigned int GetNL3AlgoReject()    { return numberOfL3AlgoRejects(); }  
  static unsigned int GetNV0()              { return numberOfV0s(); }            
  static unsigned int GetNV0Mc()            { return numberOfV0sMc(); }            
  static unsigned int GetNV0Assoc()         { return numberOfV0Assoc(); }            
  static unsigned int GetNXi()              { return numberOfXis(); }            
  static unsigned int GetNXiMc()            { return numberOfXisMc(); }            
  static unsigned int GetNXiAssoc()         { return numberOfXiAssoc(); }            
  static unsigned int GetNKink()            { return numberOfKinks(); }
  static unsigned int GetNKinkMc()          { return numberOfKinksMc(); }            
  static unsigned int GetNKinkAssoc()       { return numberOfKinkAssoc(); }            
  static unsigned int GetNStrangeCut()      { return numberOfStrangeCuts(); }    

  static unsigned int GetNTofHit()          { return numberOfTofHit(); }
  static unsigned int GetNTofData()         { return numberOfTofData(); }

  ClassDef(StMuDst,2)
};

#endif

/***************************************************************************
 *
 * $Log: StMuDst.h,v $
 * Revision 1.20  2004/08/25 04:05:56  mvl
 * Added getters for StStrangeAssocs
 *
 * Revision 1.19  2004/07/27 02:35:23  mvl
 * Added access methods for Strangeness Monte-Carlo arrays
 *
 * Revision 1.18  2004/05/02 04:10:13  perev
 * private => protected
 *
 * Revision 1.17  2004/04/20 18:41:11  perev
 * Change arrays to pointer to StMuDstMaker::arrays StMuDst.h
 *
 * Revision 1.16  2004/04/09 22:04:52  subhasis
 * after tof createevent fix by Xin
 *
 * Revision 1.15  2004/04/09 03:36:14  jeromel
 * Removed TOF support entirely for now as we need a working version ... Will
 * revisit later.
 *
 * Revision 1.14  2004/04/02 03:24:53  jeromel
 * Changes implements PMD and TOF.  TOF is clearly incomplete.
 *
 * Revision 1.13  2003/10/08 21:17:15  laue
 * StMuEmcUtil updates from Alex Suaide
 * StMuDst and StMuDstMaker fixes to take the double inheritance of the
 * StKinkMuDsts into account. A void* had to be introduced when casting
 * TObject* to StKinkMuDst*.
 *
 * Revision 1.12  2003/09/10 22:33:41  perev
 * Grid for MuDst corrections
 *
 * Revision 1.11  2003/04/15 18:48:34  laue
 * Minor changes to be able to filter MuDst.root files and an example
 * how to do this. The StMuDstFilterMaker is just an example, it has to be
 * customized (spoilers, chrome weels, etc.) by the user.
 *
 * Revision 1.10  2003/01/23 21:59:50  laue
 * Modification to compile on Solaris.
 *
 * Revision 1.9  2003/01/09 18:59:45  laue
 * initial check in of new EMC classes and the changes required
 *
 * Revision 1.8  2002/08/23 17:30:18  laue
 * additional member functions added (Helen Caines' request)
 *
 * Revision 1.7  2002/08/20 19:55:49  laue
 * Doxygen comments added
 *
 * Revision 1.6  2002/05/20 17:23:31  laue
 * StStrangeCuts added
 *
 * Revision 1.5  2002/04/01 22:42:30  laue
 * improved chain filter options
 *
 * Revision 1.4  2002/03/20 16:04:11  laue
 * minor changes, mostly added access functions
 *
 * Revision 1.3  2002/03/14 04:12:44  laue
 * bug fix: StMuL3EventSummary.cxx
 * update: StMuDst.h StMuDst.cxx
 *
 * Revision 1.2  2002/03/08 20:04:31  laue
 * change from two trees to 1 tree per file
 *
 * Revision 1.1  2002/03/08 17:04:17  laue
 * initial revision
 *
 *
 **************************************************************************/
