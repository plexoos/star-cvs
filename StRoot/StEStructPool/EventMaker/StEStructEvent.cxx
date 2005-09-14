/**********************************************************************
 *
 * $Id: StEStructEvent.cxx,v 1.5 2005/09/14 17:21:14 msd Exp $
 *
 * Author: Jeff Porter as rewrite of Ebye code by Jeff Reid
 *
 **********************************************************************
 *
 * Description:  Event quantities + list of (primary) tracks
 *
 **********************************************************************/

#include "StEStructEvent.h"
#include "StEStructTrack.h"
#include "StEStructCentrality.h"

ClassImp(StEStructEvent)

//-------------------------------------------------------
StEStructEvent::StEStructEvent() {
  
  fTracks = new TClonesArray("StEStructTrack", 1200);  // 1200 is not the max size, just an initial suggestion for ROOT
  mNtrack = 0;

  mTrackCollectionM=new StEStructTrackCollection();
  mTrackCollectionP=new StEStructTrackCollection();
}

//-------------------------------------------------------
StEStructEvent::StEStructEvent(StEStructEvent& e){

  mRunID      =e.RunID();
  mEventTime  =e.EventTime();
  mOrigMult   =e.OrigMult();
  mCentMult   =e.CentMult();
  mVx         =e.Vx();
  mVy         =e.Vy();
  mVz         =e.Vz();
  mBField     =e.BField();
  mZDCe       =e.ZDCe();
  mZDCw       =e.ZDCw();

  mNtrack=0;
  fTracks = new TClonesArray("StEStructTrack", 1200);
  for(int i=0;i<e.Ntrack();i++){
    StEStructTrack* t=(StEStructTrack*)e.Tracks()->UncheckedAt(i);
    AddTrack(t);
  }

  mTrackCollectionM=new StEStructTrackCollection();
  mTrackCollectionP=new StEStructTrackCollection();
  
  FillChargeCollections();

};


//-------------------------------------------------------
StEStructEvent::~StEStructEvent(){

  Clear();
  delete fTracks;

};  

//-------------------------------------------------------
void StEStructEvent::AddTrack(StEStructTrack* inputTrack) {

  // Add a new track to the list of tracks for this StEStructEvent.
  // To avoid calling the very time consuming operator new for each track,
  // the standard but not well know C++ operator "new with placement"
  // is called. If tracks[i] is 0, a new Track object will be created
  // otherwise the previous Track[i] will be overwritten.
  //  [Note: new with placement is now the only way to fill a TClonesArray,
  //  see the ROOT class documentation for details] 

  TClonesArray &tracks = *fTracks;  // this is a reference:  tracks = *fTracks
  new(tracks[mNtrack++]) StEStructTrack(inputTrack);  
}


//-------------------------------------------------------
void StEStructEvent::Clear(Option_t *option) {
  fTracks->Clear(option);
  mNtrack=0;
}

//-------------------------------------------------------
// Feb 26, 2004 djp Change from UInt_t to Double_t so we can use
//                  impact parameter in case data comes from an event generator.
void StEStructEvent::SetCentrality(Double_t impact) {
  if(StEStructCentrality::Instance()->numCentralities()>0)
     mCentrality=StEStructCentrality::Instance()->centrality(impact);
  if(StEStructCentrality::Instance()->numPtCentralities()>0)
     mPtCentrality=StEStructCentrality::Instance()->ptCentrality(impact);
}

//-------------------------------------------------------
void StEStructEvent::FillChargeCollections(){

  int num=Ntrack();
  if(num<=0) return;

  for(int i=0;i<num;i++){
    StEStructTrack* aTrack=(StEStructTrack*)Tracks()->UncheckedAt(i);
    if(!aTrack->isComplete()){
      aTrack->FillTransientData();
      aTrack->FillTpcReferencePoints();
      aTrack->SetComplete();
    }

    if(aTrack->Charge()==-1){
      TrackCollectionM()->push_back(aTrack);
    } else if(aTrack->Charge()==1){
      TrackCollectionP()->push_back(aTrack);
    } else {
      cout<<" Track Charge = "<<aTrack->Charge()<<endl;
    }
  }

}


//-------------------------------------------------------
StEStructTrackCollection * StEStructEvent::TrackCollectionM() const { return mTrackCollectionM;}; 
StEStructTrackCollection * StEStructEvent::TrackCollectionP() const { return mTrackCollectionP;};


/**********************************************************************
 *
 * $Log: StEStructEvent.cxx,v $
 * Revision 1.5  2005/09/14 17:21:14  msd
 * Simplified helix fitting by taking helix from mudst instead of calculating from scratch
 *
 * Revision 1.4  2004/06/25 03:13:41  porter
 * updated centrality methods and put a test in StEStructEvent.cxx
 *
 * Revision 1.3  2004/06/09 22:39:09  prindle
 * Expanded centrality class.
 * Call to set centrality from event reader.
 *
 *
 * CVS :nded ----------------------------------------------------------------------
 *
 * Revision 1.2  2004/02/27 02:28:04  prindle
 *
 * Small modification to StEStructCentrality in EventMaker branch.
 * Many modifications to Fluctuations branch, although that branch is not
 * stable yet.
 *
 * Revision 1.1  2003/10/15 18:20:51  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/
