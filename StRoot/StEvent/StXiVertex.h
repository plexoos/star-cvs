/***************************************************************************
 *
 * $Id: StXiVertex.h,v 2.5 2001/05/30 17:45:55 perev Exp $
 *
 * Author: Gene Van Buren, Feb 1999, revised Thomas Ullrich Sep 99
 ***************************************************************************
 *
 * Description: vertex class for cascades
 *
 ***************************************************************************
 *
 * $Log: StXiVertex.h,v $
 * Revision 2.5  2001/05/30 17:45:55  perev
 * StEvent branching
 *
 * Revision 2.4  2001/04/05 04:00:47  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.3  2001/03/24 03:35:01  perev
 * clone() -> clone() const
 *
 * Revision 2.2  1999/11/04 13:31:19  ullrich
 * Changed order of constructor arguments
 *
 * Revision 2.1  1999/10/28 22:28:18  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:37  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#ifndef StXiVertex_hh
#define StXiVertex_hh
#include "StVertex.h"
#include "StEnumerations.h"

class dst_vertex_st;
class dst_xi_vertex_st;
class StV0Vertex;

class StXiVertex : public StVertex {
public:
    StXiVertex();
    StXiVertex(const dst_vertex_st&, const dst_xi_vertex_st&);
    // StXiVertex(const StXiVertex&);            use default
    // StXiVertex& operator=(const StXiVertex&); use default
    ~StXiVertex();

    StVertexId            type() const;
    unsigned int          numberOfDaughters() const;
    StTrack*              daughter(unsigned int = 0);
    const StTrack*        daughter(unsigned int = 0) const;
    StPtrVecTrack         daughters(StTrackFilter&);

    float                 dcaBachelorToPrimaryVertex() const;
    float                 dcaV0ToPrimaryVertex() const;
    float                 dcaDaughters() const;
    float                 dcaParentToPrimaryVertex() const;
    const StThreeVectorF& momentumOfBachelor() const;
    StThreeVectorF        momentumOfV0() const;
    StThreeVectorF        momentum() const;
    StV0Vertex*           v0Vertex() const;
    StTrack*              bachelor();
    double                chargeOfBachelor();

    void setDcaBachelorToPrimaryVertex(float);
    void setMomentumOfBachelor(const StThreeVectorF&);
    void setDcaDaughters(float);
    void setDcaParentToPrimaryVertex(float);
    void setV0Vertex(StV0Vertex*);
    void addDaughter(StTrack*);
    void removeDaughter(StTrack*);

protected:
//  StTrack*               mDaughter;                   //$LINK
#ifdef __CINT__
    StObjLink              mDaughter;                   
#else
    StLink<StTrack>        mDaughter;                   
#endif //__CINT__
    Float_t                mDcaBachelorToPrimaryVertex;
    StThreeVectorF         mMomentumOfBachelor;
    Float_t                mDcaDaughters;
    Float_t                mDcaParentToPrimaryVertex;
//  StV0Vertex*            mV0Vertex;                   //$LINK
#ifdef __CINT__
    StObjLink              mV0Vertex;                   
#else
    StLink<StV0Vertex>     mV0Vertex;                   
#endif //__CINT__

    StObject* clone() const;
    ClassDef(StXiVertex,2)
};
#endif
