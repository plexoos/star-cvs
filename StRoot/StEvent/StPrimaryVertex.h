/***************************************************************************
 *
 * $Id: StPrimaryVertex.h,v 2.2 1999/10/28 22:26:19 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StPrimaryVertex.h,v $
 * Revision 2.2  1999/10/28 22:26:19  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.2  1999/10/28 22:26:19  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.1  1999/10/13 19:43:33  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StPrimaryVertex_hh
#define StPrimaryVertex_hh
#include "StVertex.h"

class StPrimaryVertex : public StVertex {
public:
    StPrimaryVertex();
    StPrimaryVertex(const dst_vertex_st&);
    // StPrimaryVertex(const StPrimaryVertex&);            use default
    StVertexId       type() const;
    UInt_t           numberOfDaughters() const;
    StTrack*         daughter(UInt_t);
    const StTrack*   daughter(UInt_t) const;
    StPtrVecTrack    daughters(StTrackFilter&);
    void             addDaughter(StTrack*);
    void             removeDaughter(StTrack*);
    const StSPtrVecPrimaryTrack& daughters() const;
    void                         addDaughter(StTrack*);
    void                         removeDaughter(StTrack*);

    void setParent(StTrack*);     // overwrite inherited

    friend class StPrimaryTrack;
    
protected:
    StSPtrVecPrimaryTrack mDaughters;
    void unlink(StTrack*);

    StObject* clone();
    ClassDef(StPrimaryVertex,1)
};
#endif
