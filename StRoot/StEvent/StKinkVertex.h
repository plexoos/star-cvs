/***************************************************************************
 *
 * $Id: StKinkVertex.h,v 2.5 2001/04/05 04:00:38 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StKinkVertex.h,v $
 * Revision 2.5  2001/04/05 04:00:38  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.4  2001/03/24 03:34:52  perev
 * clone() -> clone() const
 *
 * Revision 2.3  1999/11/04 16:33:26  ullrich
 * Fixed wrong option to ClassDef macro.
 *
 * Revision 2.2  1999/10/28 22:25:56  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.1  1999/10/13 19:43:22  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StKinkVertex_hh
#define StKinkVertex_hh
#include "StVertex.h"

class dst_tkf_vertex_st;
class StParticleDefinition;

class StKinkVertex : public StVertex {
public:
    StKinkVertex();
    StKinkVertex(const dst_vertex_st&, const dst_tkf_vertex_st&);
    // StKinkVertex(const StKinkVertex&);            use default
    // StKinkVertex& operator=(const StKinkVertex&); use default
    ~StKinkVertex();
    
    StVertexId     type() const;
    unsigned int   numberOfDaughters() const;
    StTrack*       daughter(unsigned int = 0);
    const StTrack* daughter(unsigned int = 0) const;
    StPtrVecTrack  daughters(StTrackFilter&);

    StParticleDefinition* pidParent() const;
    StParticleDefinition* pidDaughter() const;
    unsigned short        geantIdParent() const;
    unsigned short        geantIdDaughter() const;
    float                 dcaParentDaughter() const;
    float                 dcaDaughterPrimaryVertex() const;
    float                 dcaParentPrimaryVertex() const;
    float                 hitDistanceParentDaughter() const;
    float                 hitDistanceParentVertex() const;
    float                 dE(unsigned int i) const;
    float                 decayAngle() const;
    float                 decayAngleCM() const;
    const StThreeVectorF& parentMomentum() const;
    StThreeVectorF&       parentMomentum();
    const StThreeVectorF& daughterMomentum() const;
    StThreeVectorF&       daughterMomentum();

    void setGeantIdParent(unsigned short);
    void setGeantIdDaughter(unsigned short);
    void setDcaParentDaughter(float);
    void setDcaDaughterPrimaryVertex(float);
    void setDcaParentPrimaryVertex(float);
    void setHitDistanceParentDaughter(float);
    void setHitDistanceParentVertex(float);
    void setdE(unsigned int, float);
    void setDecayAngle(float);
    void setDecayAngleCM(float);
    void setParentMomentum(const StThreeVectorF&);
    void setDaughterMomentum(const StThreeVectorF&);
    void addDaughter(StTrack*);
    void removeDaughter(StTrack*);

protected:
    StTrack*       mDaughter;         //$LINK
    UShort_t       mParentGeantId;
    UShort_t       mDaughterGeantId;
    Float_t        mDcaParentDaughter;
    Float_t        mDcaDaughterPrimaryVertex;
    Float_t        mDcaParentPrimaryVertex;
    Float_t        mHitDistanceParentDaughter;
    Float_t        mHitDistanceParentVertex;
    Float_t        mDeltaEnergy[3];
    Float_t        mDecayAngle;
    Float_t        mDecayAngleCM;
    StThreeVectorF mParentMomentum;
    StThreeVectorF mDaughterMomentum;

    StObject* clone() const;
    ClassDef(StKinkVertex,1)
};
#endif
