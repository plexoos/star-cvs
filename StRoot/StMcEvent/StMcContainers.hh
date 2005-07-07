/***************************************************************************
 *
 * $Id: StMcContainers.hh,v 2.10 2005/07/07 18:20:48 calderon Exp $
 *
 * Author: Manuel Calderon de la Barca Sanchez, Oct 1999
 ***************************************************************************
 *
 * Description: Containers for StMcEvent objects
 *
 ***************************************************************************
 *
 * $Log: StMcContainers.hh,v $
 * Revision 2.10  2005/07/07 18:20:48  calderon
 * Added support for IGT detector.
 *
 * Revision 2.9  2005/04/18 20:11:32  calderon
 * Addition of Fgt and Fst files.  Modified other files to accomodate changes.
 *
 * Revision 2.8  2004/09/14 05:00:29  calderon
 * Added support for Ist, Ssd and changes to Pixel, from "El Kai".
 *
 * Revision 2.7  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Pixel classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 * Revision 2.6  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.6  2003/02/18 00:00:00  gans
 * Introduction of the CTB classes.  Modified several classes
 * accordingly.
 *
 * $Log: StMcContainers.hh,v $
 * Revision 2.10  2005/07/07 18:20:48  calderon
 * Added support for IGT detector.
 *
 * Revision 2.9  2005/04/18 20:11:32  calderon
 * Addition of Fgt and Fst files.  Modified other files to accomodate changes.
 *
 * Revision 2.8  2004/09/14 05:00:29  calderon
 * Added support for Ist, Ssd and changes to Pixel, from "El Kai".
 *
 * Revision 2.7  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Pixel classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 * Revision 2.6  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.5  2000/06/06 02:58:40  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.4  2000/03/06 18:05:21  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.3  2000/01/18 20:52:31  calderon
 * Works with CC5
 *
 * Revision 2.2  1999/12/14 07:04:49  calderon
 * Numbering scheme as per SVT request.
 *
 * Revision 2.1  1999/11/19 19:06:31  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:00:58  calderon
 * Completely revised for new StEvent
 *
 **************************************************************************/
#ifndef StMcContainers_hh
#define StMcContainers_hh

#include <vector>

#ifndef ST_NO_NAMESPACES
using std::vector;
#endif


class StMcHit;
class StMcCalorimeterHit;
class StMcVertex;
class StMcSvtHit;
class StMcSsdHit;
class StMcTpcHit;
class StMcFtpcHit;
class StMcRichHit;
class StMcCtbHit;
class StMcPixelHit;
class StMcIstHit;
class StMcIgtHit;
class StMcFstHit;
class StMcFgtHit;
class StMcTofHit;
class StMcTrack;
   
#ifndef ST_NO_TEMPLATE_DEF_ARGS
// Owners
typedef vector<StMcHit*>     StSPtrVecMcHit;    //! 
typedef vector<StMcVertex*>  StSPtrVecMcVertex; //!
typedef vector<StMcCalorimeterHit*>     StSPtrVecMcCalorimeterHit;  //! 
typedef vector<StMcSvtHit*>  StSPtrVecMcSvtHit; //!
typedef vector<StMcSsdHit*>  StSPtrVecMcSsdHit; //!
typedef vector<StMcTpcHit*>  StSPtrVecMcTpcHit; //!
typedef vector<StMcFtpcHit*> StSPtrVecMcFtpcHit; //!
typedef vector<StMcRichHit*> StSPtrVecMcRichHit; //!
typedef vector<StMcCtbHit*>  StSPtrVecMcCtbHit; //!
typedef vector<StMcPixelHit*> StSPtrVecMcPixelHit; //!
typedef vector<StMcIstHit*>  StSPtrVecMcIstHit; //!
typedef vector<StMcIgtHit*>  StSPtrVecMcIgtHit; //!
typedef vector<StMcFstHit*>  StSPtrVecMcFstHit; //!
typedef vector<StMcFgtHit*>  StSPtrVecMcFgtHit; //!
typedef vector<StMcTofHit*>  StSPtrVecMcTofHit; //!
typedef vector<StMcTrack*>   StSPtrVecMcTrack;  //!
// Not owners
typedef vector<StMcVertex*>  StPtrVecMcVertex; //!
typedef vector<StMcCalorimeterHit*>     StPtrVecMcCalorimeterHit;  //! 
typedef vector<StMcHit*>     StPtrVecMcHit;    //! 
typedef vector<StMcSvtHit*>  StPtrVecMcSvtHit; //!
typedef vector<StMcSsdHit*>  StPtrVecMcSsdHit; //!
typedef vector<StMcTpcHit*>  StPtrVecMcTpcHit; //!
typedef vector<StMcFtpcHit*> StPtrVecMcFtpcHit; //!
typedef vector<StMcRichHit*> StPtrVecMcRichHit; //!
typedef vector<StMcCtbHit*>  StPtrVecMcCtbHit; //!
typedef vector<StMcPixelHit*> StPtrVecMcPixelHit; //!
typedef vector<StMcIstHit*>  StPtrVecMcIstHit; //!
typedef vector<StMcIgtHit*>  StPtrVecMcIgtHit; //!
typedef vector<StMcFstHit*>  StPtrVecMcFstHit; //!
typedef vector<StMcFgtHit*>  StPtrVecMcFgtHit; //!
typedef vector<StMcTofHit*>  StPtrVecMcTofHit; //!
typedef vector<StMcTrack*>   StPtrVecMcTrack; //!
#else
//Owners
typedef vector<StMcHit*, allocator<StMcHit*> >          StSPtrVecMcHit; //!
typedef vector<StMcVertex*, allocator<StMcVertex*> >    StSPtrVecMcVertex; //!
typedef vector<StMcCalorimeterHit*, allocator<StMcCalorimeterHit*> > 
StSPtrVecMcCalorimeterHit;  //!
typedef vector<StMcSvtHit*, allocator<StMcSvtHit*> >    StSPtrVecMcSvtHit; //!
typedef vector<StMcSsdHit*, allocator<StMcSsdHit*> >    StSPtrVecMcSsdHit; //!
typedef vector<StMcTpcHit*, allocator<StMcTpcHit*> >    StSPtrVecMcTpcHit; //!
typedef vector<StMcFtpcHit*, allocator<StMcFtpcHit*> >  StSPtrVecMcFtpcHit; //!
typedef vector<StMcRichHit*, allocator<StMcRichHit*> >  StSPtrVecMcRichHit; //!
typedef vector<StMcCtbHit*, allocator<StMcCtbHit*> >    StSPtrVecMcCtbHit; //!
typedef vector<StMcPixelHit*, allocator<StMcPixelHit*> >  StSPtrVecMcPixelHit; //!
typedef vector<StMcIstHit*, allocator<StMcIstHit*> >    StSPtrVecMcIstHit; //!
typedef vector<StMcIgtHit*, allocator<StMcIgtHit*> >    StSPtrVecMcIgtHit; //!
typedef vector<StMcFstHit*, allocator<StMcFstHit*> >    StSPtrVecMcFstHit; //!
typedef vector<StMcFgtHit*, allocator<StMcFgtHit*> >    StSPtrVecMcFgtHit; //!
typedef vector<StMcTofHit*, allocator<StMcTofHit*> >    StSPtrVecMcTofHit; //!
typedef vector<StMcTrack*, allocator<StMcTrack*> >      StSPtrVecMcTrack; //!
// Not owners
typedef vector<StMcVertex*, allocator<StMcVertex*> >    StPtrVecMcVertex; //!
typedef vector<StMcCalorimeterHit*, allocator<StMcCalorimeterHit*> > StPtrVecMcCalorimeterHit;  //!
typedef vector<StMcHit*, allocator<StMcHit*> >          StPtrVecMcHit; //!
typedef vector<StMcSvtHit*, allocator<StMcSvtHit*> >    StPtrVecMcSvtHit; //!
typedef vector<StMcSsdHit*, allocator<StMcSsdHit*> >    StPtrVecMcSsdHit; //!
typedef vector<StMcTpcHit*, allocator<StMcTpcHit*> >    StPtrVecMcTpcHit; //!
typedef vector<StMcFtpcHit*, allocator<StMcFtpcHit*> >  StPtrVecMcFtpcHit; //!
typedef vector<StMcRichHit*, allocator<StMcRichHit*> >  StPtrVecMcRichHit; //!
typedef vector<StMcCtbHit*, allocator<StMcCtbHit*> >    StPtrVecMcCtbHit; //!
typedef vector<StMcPixelHit*, allocator<StMcPixelHit*> >  StPtrVecMcPixelHit; //!
typedef vector<StMcIstHit*, allocator<StMcIstHit*> >    StPtrVecMcIstHit; //!
typedef vector<StMcIgtHit*, allocator<StMcIgtHit*> >    StPtrVecMcIgtHit; //!
typedef vector<StMcFstHit*, allocator<StMcFstHit*> >    StPtrVecMcFstHit; //!
typedef vector<StMcFgtHit*, allocator<StMcFgtHit*> >    StPtrVecMcFgtHit; //!
typedef vector<StMcTofHit*, allocator<StMcTofHit*> >    StPtrVecMcTofHit; //!
typedef vector<StMcTrack*, allocator<StMcTrack*> >      StPtrVecMcTrack; //!

#endif // no template default arguments
//Iterators
typedef StPtrVecMcVertex::iterator  StMcVertexIterator; //!
typedef StPtrVecMcCalorimeterHit::iterator  StMcCalorimeterHitIterator; //!
typedef StPtrVecMcHit::iterator     StMcHitIterator; //!
typedef StPtrVecMcSvtHit::iterator  StMcSvtHitIterator; //!
typedef StPtrVecMcSsdHit::iterator  StMcSsdHitIterator; //!
typedef StPtrVecMcTpcHit::iterator  StMcTpcHitIterator; //!
typedef StPtrVecMcFtpcHit::iterator StMcFtpcHitIterator; //!
typedef StPtrVecMcRichHit::iterator StMcRichHitIterator; //!
typedef StPtrVecMcCtbHit::iterator  StMcCtbHitIterator; //!
typedef StPtrVecMcPixelHit::iterator StMcPixelHitIterator; //!
typedef StPtrVecMcIstHit::iterator  StMcIstHitIterator; //!
typedef StPtrVecMcIgtHit::iterator  StMcIgtHitIterator; //!
typedef StPtrVecMcFstHit::iterator  StMcFstHitIterator; //!
typedef StPtrVecMcFgtHit::iterator  StMcFgtHitIterator; //!
typedef StPtrVecMcTofHit::iterator  StMcTofHitIterator; //!
typedef StPtrVecMcTrack::iterator   StMcTrackIterator; //!
//Const Iterators
typedef StPtrVecMcVertex::const_iterator  StMcVertexConstIterator; //!
typedef StPtrVecMcCalorimeterHit::const_iterator  StMcCalorimeterHitConstIterator;//!
typedef StPtrVecMcHit::const_iterator     StMcSvtConstIterator; //!
typedef StPtrVecMcSvtHit::const_iterator  StMcSvtHitConstIterator; //!
typedef StPtrVecMcSsdHit::const_iterator  StMcSsdHitConstIterator; //!
typedef StPtrVecMcTpcHit::const_iterator  StMcTpcHitConstIterator; //!
typedef StPtrVecMcFtpcHit::const_iterator StMcFtpcHitConstIterator; //!
typedef StPtrVecMcRichHit::const_iterator StMcRichHitConstIterator; //!
typedef StPtrVecMcCtbHit::const_iterator  StMcCtbHitConstIterator; //!
typedef StPtrVecMcPixelHit::const_iterator StMcPixelHitConstIterator; //!
typedef StPtrVecMcIstHit::const_iterator  StMcIstHitConstIterator; //!
typedef StPtrVecMcIgtHit::const_iterator  StMcIgtHitConstIterator; //!
typedef StPtrVecMcFstHit::const_iterator  StMcFstHitConstIterator; //!
typedef StPtrVecMcFgtHit::const_iterator  StMcFgtHitConstIterator; //!
typedef StPtrVecMcTofHit::const_iterator  StMcTofHitConstIterator; //!
typedef StPtrVecMcTrack::const_iterator   StMcTrackConstIterator; //!

#endif //StMcContainers
