/***************************************************************************
 *
 * $Id $
 *
 * Author: Manuel Calderon de la Barca Sanchez Aug 2000
 ***************************************************************************
 *
 * Description:
 * Function to give a standard reference multiplicity for 
 * an event, uncorrected of course.  The agreed upon cuts are:
 * primary tracks only
 * flag > 0
 * charge < 0
 * fit points >= 10
 * abs(eta) < 0.5
 * dca < 3 cm
 ***************************************************************************
 *
 * $Log: StuRefMult.hh,v $
 * Revision 1.1  2000/08/24 17:53:57  calderon
 * Function to obtain a reference (uncorrected) multiplicity
 * from StEvent.  This counts track that satisfy the so called "Cut Set 1"
 * described in the header and also in the README file.
 *
 *
 **************************************************************************/
#ifndef StuRefMult_hh
#define StuRefMult_hh

#include "StEventTypes.h"
inline unsigned int
uncorrectedMultiplicity(StEvent& evt)
{
    StPrimaryVertex* primVtx = evt.primaryVertex();

    if (!primVtx) return 0;
    
    const StSPtrVecPrimaryTrack& tracks = primVtx->daughters();
    size_t countedTracks = 0;
    for (StSPtrVecPrimaryTrackConstIterator iter = tracks.begin(); iter != tracks.end(); iter++) {
	StTrack* track = (*iter);
	// these first 3 checks are easy, save time
	if (track->flag()<0 || track->geometry()->charge()>0 || track->fitTraits().numberOfFitPoints()<10 ) continue; 
	// check eta, a bit more elaborate
	if (fabs(track->geometry()->momentum().pseudoRapidity())>0.5) continue;
	// finally, check dca, if a track satisfies gets inside the if, count it.
	if (track->geometry()->helix().distance(primVtx->position())<3) ++countedTracks;
    }
    return countedTracks;
}
#endif
