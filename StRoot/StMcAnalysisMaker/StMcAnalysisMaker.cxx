/*************************************************
 *
 * $Id: StMcAnalysisMaker.cxx,v 1.21 2003/09/02 17:58:41 perev Exp $
 * $Log: StMcAnalysisMaker.cxx,v $
 * Revision 1.21  2003/09/02 17:58:41  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.20  2000/05/11 21:57:34  calderon
 * Book the histograms before creating the file so that Kathy's macros
 * can pick them up.
 *
 * Revision 1.19  2000/05/11 16:21:26  calderon
 * Write only one V0 matched pair to supress the screen output.
 *
 * Revision 1.18  2000/04/20 21:31:52  calderon
 * More checks for the cases where a track has no partner, Thanks Janet.
 *
 * Revision 1.17  2000/04/20 16:59:47  calderon
 * Pick up the makers with the new names
 * Change the name from "McAnalysis" to "StMcAnalysisMaker"
 * No longer use the helix, use the primary track momentum when available
 * (also avoids dependency on StEventSummary)
 * Denominator for mom. resolution histograms is now simulated momentum.
 *
 * Revision 1.16  2000/04/12 21:33:57  calderon
 * check case where no multimaps are found
 *
 * Revision 1.15  2000/04/04 23:18:12  calderon
 * B -> B * kilogauss
 *
 * Revision 1.14  2000/03/28 02:28:04  calderon
 * Return to calculating momentum at the origin, instead of taking the
 * momentum from the first point.
 *
 * Revision 1.13  2000/03/06 21:47:56  calderon
 * Add Lee's V0 example
 *
 * Revision 1.12  2000/02/07 16:43:37  calderon
 * Find the first hit wherever it may be, instead of assuming a sector and padrow.
 *
 * Revision 1.11  2000/01/24 22:22:24  calderon
 * use delete [] for the array of floats
 *
 * Revision 1.10  1999/12/14 07:08:48  calderon
 * First version to work with new StEvent, StMcEvent & StAssociationMaker.
 * Need to add more examples of all the new maps.
 *
 * Revision 1.9  1999/10/01 14:11:15  calderon
 * Chech to see whether StEvent has primary vertex
 * before trying to use it.  If no primary vertex is
 * found, assume its position is (0,0,0).
 *
 * Revision 1.8  1999/09/28 15:03:29  didenko
 * Cleanup dependencies on non existing h-files
 *
 * Revision 1.7  1999/09/23 21:25:48  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 * Revision 1.6  1999/09/10 19:11:15  calderon
 * Write the Ntuple in StMcAnalysisMaker into a file.
 * This way it can be accessed after the macro finishes,
 * otherwise it gets deleted.
 *
 * Revision 1.5  1999/09/09 23:59:37  calderon
 * Made the following changes:
 *
 * -book histograms and ntuple in Init()
 * -do not delete the histograms, they are supposed to be
 *  deleted automatically by the chain
 * -don't create the canvas here, now done in the macro
 *
 * Revision 1.4  1999/07/30 16:19:19  calderon
 * Use value_type typedef for inserting pairs in multimaps, Victor corrected iterators on HP in SL99h, Improved use of const for HP compilation
 *
 * Revision 1.3  1999/07/29 15:08:33  calderon
 * Include Mom. Resolution example (Histograms & Ntuple)
 *
 * Revision 1.2  1999/07/28 20:27:30  calderon
 * Version with SL99f libraries
 *
 *
 * Examples that use the structures of
 * StMcEvent and StAssociationMaker
 *
 *************************************************/

#include <Stiostream.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <math.h>

#include "TStyle.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"
#include "TFile.h"

#include "StMcAnalysisMaker.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"

#include "StMessMgr.h"

#include "StChain.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"

#include "StAssociationMaker/StAssociationMaker.h"
#include "StAssociationMaker/StTrackPairInfo.hh"

#include "StThreeVectorF.hh"

#include "StEventTypes.h"

#include "StMcEventTypes.hh"

#include "StEventMaker/StEventMaker.h"
#include "StMcEventMaker/StMcEventMaker.h"

// Define data Members for the histograms
const Int_t   StMcAnalysisMaker::mNumDeltaX = 50;
const Int_t   StMcAnalysisMaker::mNumDeltaZ = 50;
const Float_t StMcAnalysisMaker::mMinDeltaX = -0.52;
const Float_t StMcAnalysisMaker::mMaxDeltaX =  0.52;
const Float_t StMcAnalysisMaker::mMinDeltaZ = -0.52;
const Float_t StMcAnalysisMaker::mMaxDeltaZ =  0.52;

ClassImp(StMcAnalysisMaker)


//_________________________________________________
StMcAnalysisMaker::StMcAnalysisMaker(const char *name, const char *title):StMaker(name,title)
{
    //  StMcAnalysisMaker Constructor
    // - zero all pointers defined in the header file
    mAssociationCanvas = 0;
    mMomResolution  = 0;
    mHitResolution  = 0;   
    coordRec        = 0;  
    coordMcPartner  = 0;
    mTrackNtuple    = 0;
}

//_________________________________________________
StMcAnalysisMaker::~StMcAnalysisMaker()
{
    //  StMcAnalysisMaker Destructor
    //  delete the histograms
    cout << "Inside StMcAnalysisMaker Destructor" << endl;
    SafeDelete(mAssociationCanvas);
//     SafeDelete(mMomResolution);
//     SafeDelete(mHitResolution);
//     SafeDelete(coordRec);
//     SafeDelete(coordMcPartner);
//     SafeDelete(mTrackNtuple);
}

//_____________________________________________________________________________

void StMcAnalysisMaker::Clear(const char*)
{
    // StMcAnalysisMaker - Clear,
    // Don't delete the canvas, so that it stays if needed
    
    StMaker::Clear();
}

//_________________________________________________
Int_t StMcAnalysisMaker::Finish()
{
    mNtupleFile->Write();
    mNtupleFile->Close();
    return StMaker::Finish();
}


//_________________________________________________
Int_t StMcAnalysisMaker::Init()
{
    // StMcAnalysisMaker - Init
    
    SetZones();  // This is my method to set the zones for the canvas.

    // Book Histograms Here so they can be found and deleted by Victor's chain (I hope).
    mHitResolution = new TH2F("hitRes","Delta Z Vs Delta X for Hits",
			     mNumDeltaX,mMinDeltaX,mMaxDeltaX,mNumDeltaZ,mMinDeltaZ,mMaxDeltaZ);
    mHitResolution->SetXTitle("Delta X (cm)");
    mHitResolution->SetYTitle("Delta Z (cm)");

    mMomResolution = new TH1F("momRes","(|p| - |pmc|)/|pmc|",100,-1.,1.);
    mMomResolution->SetXTitle("Resolution (%)");

    coordRec = new TH2F("coordRc","X vs Y pos. of Hits", 100, -200, 200, 100, -200, 200);
    coordRec->SetXTitle("X (cm)");
    coordRec->SetYTitle("Y (cm)");
    
    coordMcPartner = new TH2F("coordMc","X vs Y pos. of Hits", 100, -200, 200, 100, -200, 200);
    coordMcPartner->SetXTitle("X (cm)");
    coordMcPartner->SetYTitle("Y (cm)");

    // Define the file for the Ntuple, otherwise it won't be available later.
    
    mNtupleFile = new TFile("TrackMapNtuple.root","RECREATE","Track Ntuple");

    char* vars = "px:py:pz:p:pxrec:pyrec:pzrec:prec:commTpcHits:hitDiffX:hitDiffY:hitDiffZ";
    mTrackNtuple = new TNtuple("TrackNtuple","Track Pair Info",vars);
    mTrackNtuple->SetAutoSave(100000000);
    //cout << "Defined Momentum Res. Histogram & Ntuple" << endl;


    return StMaker::Init();
}
//_________________________________________________
Int_t StMcAnalysisMaker::Make()
{
    // Get the pointers we need, we have to use the titles we gave them in the
    // macro.  I just used the defaults.

    // StEvent
    StEvent* rEvent = 0;
    rEvent = (StEvent*) GetInputDS("StEvent");

    // StMcEvent
    StMcEvent* mEvent = 0;
    mEvent = ((StMcEventMaker*) GetMaker("StMcEvent"))->currentMcEvent();

    // StAssociationMaker
    StAssociationMaker* assoc = 0;
    assoc = (StAssociationMaker*) GetMaker("StAssociationMaker");

    // the Multimaps...
    rcTpcHitMapType* theHitMap = 0;
    theHitMap = assoc->rcTpcHitMap();
    rcTrackMapType* theTrackMap = 0;
    theTrackMap = assoc->rcTrackMap();
    mcV0MapType* theMcV0Map = 0;
    theMcV0Map = assoc->mcV0Map(); 

    if (!theHitMap) {
	gMessMgr->Warning() << "----------WARNING----------\n"
			    << "No Hit Map found for this event!" << endm;
	return kStWarn;
    }
    // Example: look at the position of the primary vertex
    //          Map is not needed for this, but it's a good check,
    //          tracking will not be good if primary vertex was not well placed.

    // First check whether the Primary Vertex is there at all.
    StThreeVectorD VertexPos(0,0,0);
    if (rEvent->primaryVertex()) {
	VertexPos = rEvent->primaryVertex()->position();
	cout << "Position of Primary Vertex from StEvent:" << endl;
	cout << VertexPos << endl;
    }
    else {
	cout << "----------- WARNING ------------" << endl;
	cout << "No primary vertex from StEvent!!" << endl;
	cout << "Assume it is at (0,0,0)         " << endl;
	
    }
    cout << "Position of Primary Vertex from StMcEvent:" << endl;
    cout << mEvent->primaryVertex()->position() << endl;
    
    // Example: look at hits associated with 1st REC hit in Tpc Hit collection.

    StTpcHit*     firstHit;
    Bool_t        gotOneHit;
    StTpcHitCollection* tpcColl = rEvent->tpcHitCollection();
    unsigned int j,k, nhits;
    nhits = tpcColl->numberOfHits();
        if (tpcColl && nhits) {
	    gotOneHit = kFALSE;
	    for (k=0; !gotOneHit && k<tpcColl->numberOfSectors(); k++)
		for (j=0; !gotOneHit && j<tpcColl->sector(k)->numberOfPadrows(); j++)
		    if (tpcColl->sector(k)->padrow(j)->hits().size()) {
			
			firstHit = tpcColl->sector(k)->padrow(j)->hits()[0];
			cout << "First Hit Found in Sector "
			     << firstHit->sector() << " Padrow "
			     << firstHit->padrow() << endl;
			gotOneHit = kTRUE;
		    }
	    cout << "This hit has " <<  theHitMap->count(firstHit) << " MC Hits associated with it."<< endl;
	    // To get the associated hits of the first hit we use equal_range(key), which returns
	    // 2 iterators, the lower bound and upper bound, so that then we can loop over them.
	    
	    cout << "Position of First Rec. Hit and Associated (if any) MC Hit:" << endl;
	    pair<rcTpcHitMapIter,rcTpcHitMapIter> hitBounds = theHitMap->equal_range(firstHit);
	    
	    for (rcTpcHitMapIter it=hitBounds.first; it!=hitBounds.second; ++it) 
		cout << "[" << (*it).first->position() << ", " << (*it).second->position() << "]" << endl;
	    
	}
	else {
	    cout << "There are no reconstructed TPC Hits in this event" << endl;
	}
    

    // Example: Make a histogram using the Hit Map.
    
    
    // Fill histogram from map

    float DeltaX;
    float DeltaZ;
    
    StTpcHit* keyHit;
    StTpcHitCollection* recHits = rEvent->tpcHitCollection();
     
    cout << "Making Hit Resolution Histogram..." << endl;
    // Loop over Rec Hits
    
    for (unsigned int iSector=0; iSector< recHits->numberOfSectors(); iSector++) {
	for (unsigned int iPadrow=0; iPadrow<recHits->sector(iSector)->numberOfPadrows();
	     iPadrow++) {
	    for (StTpcHitIterator iter = recHits->sector(iSector)->padrow(iPadrow)->hits().begin();
		 iter != recHits->sector(iSector)->padrow(iPadrow)->hits().end();
		 iter++) {
		keyHit = *iter;
		pair<rcTpcHitMapIter,rcTpcHitMapIter>
		    recBounds = theHitMap->equal_range(keyHit);
	
		for (rcTpcHitMapIter it2=recBounds.first; it2!=recBounds.second; ++it2){
		    
		    DeltaX = (*it2).second->position().x() - (*it2).first->position().x();
		    DeltaZ = (*it2).second->position().z() - (*it2).first->position().z();

		    mHitResolution->Fill(DeltaX,DeltaZ);
		    
		}//Mc Hits assoc. w/ rec.
	    } // Rec hits loop
	} // padrow
    } // sector
    keyHit = 0;
    cout << "Finished Making Histogram." << endl;
    
    if (!theTrackMap) {
	gMessMgr->Warning() << "----------WARNING----------\n"
			    << "No Track Map found for this event!" << endm;
	return kStWarn;
    }
    // Example: look at the magnitude of the momentum of
    //          the MC track associated with first track in track Collection

    StSPtrVecTrackNode& rcTrackNodes = rEvent->trackNodes();
    StTrackNode*        firstTrackNode = *(rcTrackNodes.begin());
    StGlobalTrack*      firstTrack = dynamic_cast<StGlobalTrack*>(firstTrackNode->track(global));
    
    if (firstTrack) {
	pair<rcTrackMapIter,rcTrackMapIter> trackBounds = theTrackMap->equal_range(firstTrack);

	
	cout << "MC Tracks associated with first Track in collection: " << theTrackMap->count(firstTrack) << endl;
	cout << "Momentum of First Track and of Associated Tracks (if there are any):" << endl;
	// Get the momentum of the track and compare it to MC Track.
	// Use primary track if available
	StPrimaryTrack* pTrk = dynamic_cast<StPrimaryTrack*>(firstTrack->node()->track(primary));
	StThreeVectorD recMom(0,0,0);
	if (pTrk)
	    recMom = pTrk->geometry()->momentum();
	else
	    recMom = firstTrack->geometry()->momentum();
	for (rcTrackMapIter trkIt=trackBounds.first; trkIt!=trackBounds.second; ++trkIt) { 
	    cout << "[" << abs(recMom) << ", ";
	    cout << abs((*trkIt).second->partnerMcTrack()->momentum()) << "]" << endl;
	    cout << "These tracks have : \n";
	    cout << (*trkIt).second->commonTpcHits() << " TPC  hits in common." << endl;
	    cout << (*trkIt).second->commonSvtHits() << " SVT  hits in common." << endl;
	    cout << (*trkIt).second->commonFtpcHits() <<" FTPC hits in common." << endl;
	}
    }
    else {
	cout << "First Node doesn't have a global Track!" << endl;
    }
    
    // Example: Make a histogram of the momentum resolution of the event
    //          Make an Ntuple with rec & monte carlo mom, mean hit difference, and # of common hits
    StGlobalTrack* recTrack;
    StPrimaryTrack* primTrk;
    StMcTrack*     mcTrack;
    StThreeVectorD p(0,0,0);
    StThreeVectorD pmc(0,0,0);
    float diff =0;

    float* values = new float[12];
    
    for (rcTrackMapIter tIter=theTrackMap->begin();
	 tIter!=theTrackMap->end(); ++tIter){

	recTrack = (*tIter).first;
	if ((*tIter).second->commonTpcHits()<10) continue;
	mcTrack = (*tIter).second->partnerMcTrack();
	
	pmc = mcTrack->momentum();
	for (int k=0; k<3; k++) values[k] = pmc[k];
	values[3]=pmc.mag();
	
	primTrk = dynamic_cast<StPrimaryTrack*>(recTrack->node()->track(primary));
	if (primTrk)
	    p = primTrk->geometry()->momentum();
	else
	    p = recTrack->geometry()->momentum();
		
	for (int j=0; j<3; j++) values[j+4] = p[j];
	values[7]=p.mag();
	values[8]=(*tIter).second->commonTpcHits();
	// Fill 1d Mom. resolution Histogram
	
	diff = (p.mag() - pmc.mag())/pmc.mag();
	mMomResolution->Fill(diff,1.);
	// Loop to get Mean hit position diff.
	StThreeVectorF rHitPos(0,0,0);
	StThreeVectorF mHitPos(0,0,0);

	StPtrVecHit recTpcHits = recTrack->detectorInfo()->hits(kTpcId);
	
	for (StHitIterator hi=recTpcHits.begin();
	     hi!=recTpcHits.end(); hi++) {
	    StHit* hit = *hi;
	    StTpcHit* rHit = dynamic_cast<StTpcHit*>(hit);
	    if (!rHit) { cout << "This Hit is not a TPC Hit"<< endl; continue;}
	    pair<rcTpcHitMapIter,rcTpcHitMapIter> rBounds = theHitMap->equal_range(rHit);
	    for (rcTpcHitMapIter hIter=rBounds.first; hIter!=rBounds.second; hIter++) {
		const StMcTpcHit* mHit = (*hIter).second;
		if (mHit->parentTrack() != mcTrack) continue;
		rHitPos += rHit->position();
		mHitPos += mHit->position();
	    }// Associated Hits Loop.
	
	} // Hits of rec. Track Loop
       
	rHitPos /=(float) (*tIter).second->commonTpcHits();
	mHitPos /=(float) (*tIter).second->commonTpcHits();
	for (int jj=0; jj<3; jj++) values[9+jj] = rHitPos[jj] - mHitPos[jj];
	mTrackNtuple->Fill(values);
    } // Tracks in Map Loop
    cout << "Finished Track Loop, Made Ntuple" << endl;
    //delete vars;
    delete [] values;
    //mNtupleFile->Write(); // Write the Ntuple to the File in Finish(), not here

    // Example: Make 2 Histograms
    // - x and y positions of the hits from the reconstructed track.
    // - x and y positions of the hits from the  Monte Carlo  track.

    unsigned int maxCommonTpcHits = 0;
    StMcTrack* partner = 0;
    pair<rcTrackMapIter,rcTrackMapIter> trackBounds = theTrackMap->equal_range(firstTrack);
    for (rcTrackMapIter rcIt = trackBounds.first;
	 rcIt != trackBounds.second;
	 ++rcIt) {
	if ((*rcIt).second->commonTpcHits() >  maxCommonTpcHits) {
	    partner = (*rcIt).second->partnerMcTrack();
	    maxCommonTpcHits = (*rcIt).second->commonTpcHits();
	}
    }
    StHitIterator rcHitIt;
    StMcTpcHitIterator mcHitIt;
    StPtrVecHit theHits = firstTrack->detectorInfo()->hits(kTpcId);
    for (rcHitIt  = theHits.begin();
	 rcHitIt != theHits.end();
	 rcHitIt++) coordRec->Fill((*rcHitIt)->position().x(),(*rcHitIt)->position().y());
    if (partner)
	for (mcHitIt  = partner->tpcHits().begin();
	     mcHitIt != partner->tpcHits().end();
	     mcHitIt++) coordMcPartner->Fill((*mcHitIt)->position().x(),(*mcHitIt)->position().y());
    
    if (!theMcV0Map) {
	gMessMgr->Warning() << "----------WARNING----------\n"
			    << "No V0 Map found for this event!" << endm;
	return kStWarn;
    }
    //Example: Print out position of V0 vertices that have been associated.
    //         (LSB)
    StSPtrVecMcVertex& mcVertices = mEvent->vertices();
    StV0Vertex* rcV0Partner;
    StMcVertexIterator mcVertexIt;

    //Loop over all MC vertices
    bool foundV0Pair = false;
    for (mcVertexIt = mcVertices.begin(); mcVertexIt != mcVertices.end();
	 mcVertexIt++){
	// Get the upper and lower bounds.
	pair<mcV0MapIter,mcV0MapIter> mcV0Bounds = theMcV0Map->equal_range(*mcVertexIt);

	// Print out MC vertex position if there is an associated V0.
	
	if (mcV0Bounds.first != mcV0Bounds.second) {
	    cout << "Printing Position of a V0 pair:\n";
	    cout << "Position of MC V0 vertex: " << (*mcVertexIt)->position() << endl;
	    foundV0Pair = true;
	}
	//Now loop over the bounds      
	for(mcV0MapIter mcV0MapIt = mcV0Bounds.first;
	    mcV0MapIt != mcV0Bounds.second; ++mcV0MapIt){
	    rcV0Partner = (*mcV0MapIt).second;
	    cout << "Position of rc V0 vertex: " << rcV0Partner->position() << endl;
	   
	}
	if (foundV0Pair) break; // Only print the information of 1 reconstructed vertex, to avoid a lot of output.
    }

    
    
    //mAssociationCanvas = new TCanvas("mAssociationCanvas", "Histograms",200,10,900,500);

    return kStOK;
}
