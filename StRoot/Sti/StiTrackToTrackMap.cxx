#include "Sti/StiTrackToTrackMap.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiHitContainer.h"

StiTrackToTrackMap::StiTrackToTrackMap(Factory<StiTrackAssociation> * associationFactory)
{
  _associationFactory = associationFactory;
}

StiTrackToTrackMap::~StiTrackToTrackMap()
{
}

void StiTrackToTrackMap::build(StiTrackContainer * firstTrackContainer,
			       StiHitToHitMap    * hitToHitMap,
			       StiHitToTrackMap  * hitToTrackMap)
{
  vector<StiTrack*>::const_iterator firstTrackIter;
  StiTrackAssociation * association;
  StiHit * secondHit=0;
  StiTrack * track;
  for (firstTrackIter=firstTrackContainer->begin();
       firstTrackIter!=firstTrackContainer->end();
       ++firstTrackIter)
    {
      association = _associationFactory->getInstance();
      insert( TrackToTrackAssociationMapType::value_type(*firstTrackIter, association) );
      const vector<StiHit*> & hits = (*firstTrackIter)->getHits();
      for (vector<StiHit*>::const_iterator firstHitIter=hits.begin();
	   firstHitIter!=hits.end();
	   ++firstHitIter)
	{
	  //secondHit = hitToHitMap->getHit(*firstHitIter);
	  if (secondHit)
	    {
	      //track = hitToTrackMap->getTrack(secondHit);
	      if (track)
		{
		  //association->add(track);
		}
	    }
	}
    }
}

