/***************************************************************************
 *
 * $Id: StMuFilter.h,v 1.4 2003/01/23 21:59:50 laue Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 ***************************************************************************/

#ifndef StMuFilter_h
#define StMuFilter_h

#include "StMuCut.h"
#include "StEvent/StEnumerations.h"
#include "StTrackMethod.h"

#include <list>
#if !defined(ST_NO_NAMESPACES)
using namespace std;
#endif


#if defined(ST_NO_TEMPLATE_DEF_ARGS)
typedef list<unsigned short, allocator<unsigned short> >            UnsignedShortCollection;
typedef list<unsigned short, allocator<unsigned short> >::iterator  UnsignedShortIterator;
#else 
typedef list<unsigned short>            UnsignedShortCollection;
typedef list<unsigned short>::iterator  UnsignedShortIterator;
#endif


class StMuFilter : public StMuCut {
 public:
    void addEncodedMethod(unsigned short method) { mEncodedMethods.push_back(method); }
    void addEncodedMethod(StTrackFinderMethod find, StTrackFittingMethod fit) { mEncodedMethods.push_back( fit + (1<<find)); }
 private:
    UnsignedShortCollection mEncodedMethods;  
    
    virtual bool accept( const StEvent*);
    virtual bool accept( const StTrack*);
    virtual bool accept( const StV0Vertex*);
    virtual bool accept( const StXiVertex*);
    virtual bool accept( const StKinkVertex*);
    virtual bool accept( const StV0MuDst*);
    virtual bool accept( const StXiMuDst*);
    virtual bool accept( const StKinkMuDst*);
       
    ClassDef(StMuFilter,1)
};

#endif

/***************************************************************************
 *
 * $Log: StMuFilter.h,v $
 * Revision 1.4  2003/01/23 21:59:50  laue
 * Modification to compile on Solaris.
 *
 * Revision 1.3  2002/09/11 21:02:42  laue
 * added cut on track encoded method for ITTF
 *
 * Revision 1.2  2002/05/04 23:56:30  laue
 * some documentation added
 *
 * Revision 1.1  2002/03/08 17:04:18  laue
 * initial revision
 *
 *
 **************************************************************************/
