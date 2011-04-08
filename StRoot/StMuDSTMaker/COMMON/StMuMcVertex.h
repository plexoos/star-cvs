#ifndef __StMuMcVertex_h__
#define __StMuMcVertex_h__
#include "tables/St_g2t_vertex_Table.h" 

class StMuMcVertex : public TObject {
 public:
  StMuMcVertex(const g2t_vertex_st &v) : mId(v.id), mNoDaughters(v.n_daughter), mIdParTrk(v.parent_p), 
    mIsInterm(v.is_itrmd), mTime(v.ge_tof), mXyzV(v.ge_x) {}
  StMuMcVertex() {}
  virtual ~StMuMcVertex() {}
  Int_t Id()                   const {return mId;}          /* primary key */			
  Int_t NoDaughters()          const {return mNoDaughters;} /* Number of daughter tracks */	
  Int_t IsIntermedate()        const {return mIsInterm;}    /* flags intermediate vertex */	
  Int_t Time()                 const {return mTime;}        /* GEANT vertex production time */
  const StThreeVectorF &XyzV() const {return *&mXyzV;}      /* GEANT vertex coordinate (Cartesian) */
 private:
  Int_t   mId;
  Int_t   mNoDaughters;
  Int_t   mIdParTrk;
  Int_t   mIsInterm;                    
  Float_t mTime;
  StThreeVectorF mXyzV;
  ClassDef(StMuMcVertex,1)
};
#endif
