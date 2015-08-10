// $Id: StiSstHitLoader.cxx,v 1.1 2015/07/16 15:19:59 bouchet Exp $
// 
// $Log: StiSstHitLoader.cxx,v $
// Revision 1.1  2015/07/16 15:19:59  bouchet
// added SstHitLoader to load SST hits ; straight copy of StiSsdHitLoader
//
// Revision 1.16  2015/03/09 23:40:22  smirnovd
// Removed commented code
//
// Revision 1.15  2015/03/09 21:44:26  perev
// Remove redundant printouts
//
// Revision 1.14  2015/01/24 04:43:41  smirnovd
// StiSsdHitLoader: Just to be safe got rid of local variable 'ladder' shadowing the loop's counter having the same name
//
// Revision 1.13  2009/03/18 19:55:39  fisyak
// remove StiDetectorFinder class
//
// Revision 1.12  2005/10/26 21:59:12  fisyak
// get rid off dependencies from StMcEvent
//
// Revision 1.11  2005/06/21 15:31:47  lmartin
// CVS tags added
//
/*!
 * \class StiSstHitLoader
 * \author Christelle Roy
*/

#include "Stiostream.h"
#include <cmath>
#include <stdio.h>

#include "StEventTypes.h"
#include "StEvent.h"
#include "Sti/Base/Factory.h"
#include "StiSsd/StiSstHitLoader.h"
#include "Sti/StiHit.h"
#include "Sti/StiTrack.h"
#include "Sti/StiHitContainer.h"
#include "Sti/StiDetector.h"
#include "Sti/StiDetectorBuilder.h"
#include "Sti/StiDetector.h"

StiSstHitLoader::StiSstHitLoader()
  : StiHitLoader<StEvent,StiDetectorBuilder>("SstHitLoader")
{}
    
StiSstHitLoader::StiSstHitLoader(StiHitContainer* hitContainer,
				 Factory<StiHit>*hitFactory,
				 StiDetectorBuilder*detector)
  : StiHitLoader<StEvent,StiDetectorBuilder>("SstHitLoader",hitContainer,hitFactory,detector)
{}

StiSstHitLoader::~StiSstHitLoader()
{}

void StiSstHitLoader::loadHits(StEvent* source,
			       Filter<StiTrack> * trackFilter, 
			       Filter<StiHit> * hitFilter)
{
  if (!source)
    throw runtime_error("StiSstHitLoader::loadHits() - FATAL - source==0 ");
  StSstHitCollection* ssthits = source->sstHitCollection();
  if (!ssthits)
    {
      return;
    }
  int compt = 0;
  int layer = 0;
  StSstHit* hit;
  StiHit* stiHit;
  StiDetector* detector;
  if (!_hitContainer)
    throw runtime_error("StiSstHitLoader::loadHits() - FATAL - _hitContainer==0 ");

  
  for (unsigned int ladder = 0; ladder< ssthits->numberOfLadders(); ++ladder) 
    {
      StSstLadderHitCollection* ladderhits = ssthits->ladder(ladder);
      if (!ladderhits) break;
      
      for (unsigned int wafer = 0; wafer< ladderhits->numberOfWafers(); ++wafer) 
	{
	  StSstWaferHitCollection* waferhits = ladderhits->wafer(wafer);

	  if (!waferhits) break;
	  const StSPtrVecSstHit& hits = waferhits->hits(); 
	  
	  for (const_StSstHitIterator it=hits.begin(); it!=hits.end(); ++it) 
	    {
	      if (!*it) throw runtime_error("StiSstHitLoader::loadHits() - WARNING - *it==0");
	      hit = static_cast<StSstHit*>(*it);

	      if (!hit) throw runtime_error("StiSstHitLoader::loadHits() - WARNING - hit==0");

	      detector = _detector->getDetector(layer, hit->ladder() - 1);

	      if (hit && detector) 
		{
		  compt++;
		  stiHit = _hitFactory->getInstance();
		  stiHit->setGlobal(detector,hit,
				    hit->position().x(),
				    hit->position().y(),
				    hit->position().z(),
				    hit->charge() );
		  _hitContainer->add( stiHit );
		}
	    }
	}
    }

}
	

