/*!
 * \class StPrimaryTrack 
 * \author Thomas Ullrich, Sep 1999
 */
/***************************************************************************
 *
 * $Id: StPrimaryTrack.h,v 2.5 2002/02/22 22:56:49 jeromel Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StPrimaryTrack.h,v $
 * Revision 2.5  2002/02/22 22:56:49  jeromel
 * Doxygen basic documentation in all header files. None of this is required
 * for QM production.
 *
 * Revision 2.4  2001/05/30 17:45:54  perev
 * StEvent branching
 *
 * Revision 2.3  2001/03/24 03:34:53  perev
 * clone() -> clone() const
 *
 * Revision 2.2  1999/10/28 22:26:13  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.1  1999/10/13 19:43:31  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StPrimaryTrack_hh
#define StPrimaryTrack_hh

#include "StTrack.h"
class StPrimaryVertex;

class StPrimaryTrack : public StTrack {
public:
    StPrimaryTrack();
    StPrimaryTrack(const dst_track_st&);
    StPrimaryTrack(const StPrimaryTrack&);
    StPrimaryTrack& operator=(const StPrimaryTrack&);
    ~StPrimaryTrack();

    StTrackType      type() const;
    const StVertex*  vertex() const;

    void setVertex(StVertex*);

protected:
    StObject* clone() const;
    
private:
//  StPrimaryVertex*         	mVertex; 	//$LINK
#ifdef __CINT__
    StObjLink  		     	mVertex; 	
#else
    StLink<StPrimaryVertex>  	mVertex; 	
#endif //__CINT__
    
    ClassDef(StPrimaryTrack,2)
};
#endif
