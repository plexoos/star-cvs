// $Id: StMcCalorimeterHit.hh,v 2.3 2003/09/02 17:58:41 perev Exp $
//
// $Log: StMcCalorimeterHit.hh,v $
// Revision 2.3  2003/09/02 17:58:41  perev
// gcc 3.2 updates + WarnOff
//
// Revision 2.2  2000/06/06 02:58:40  calderon
// Introduction of Calorimeter classes.  Modified several classes
// accordingly.
//
// Revision 2.1  2000/05/05 14:54:19  calderon
// Initial revision
//
//                                                                           
#ifndef StMcCalorimeterHit_hh
#define StMcCalorimeterHit_hh

#include <Stiostream.h>
//#include "StMemoryPool.hh"

class StMcTrack;

class StMcCalorimeterHit {
public:
    StMcCalorimeterHit();
    StMcCalorimeterHit(int, int, int, float);
    StMcCalorimeterHit(int, int, int, float, StMcTrack*);
    virtual ~StMcCalorimeterHit();
    
    int operator==(const StMcCalorimeterHit&) const;
    int operator!=(const StMcCalorimeterHit&) const;
    void operator+=(const StMcCalorimeterHit&);

    bool sameCell(const StMcCalorimeterHit&) const;

  // "Get" Methods
    virtual int                          module() const;
    virtual int                             eta() const;
    virtual int                             sub() const;
    virtual float                            dE() const;
    virtual StMcTrack*              parentTrack() const;	

  // "Set" Methods

    virtual void setModule(int);
    virtual void setEta(int);
    virtual void setSub(int);
    virtual void setdE(float);
    virtual void setParentTrack(StMcTrack*);
    
  //  void* operator new(size_t)     { return mPool.alloc(); }
  //  void  operator delete(void* p) { mPool.free(p); }
    
protected:
    int                  mModule;
    int                  mEta;
    int                  mSub;
    float                mdE;
    StMcTrack*           mParentTrack;

  //private:
  //  static StMemoryPool mPool; //!
};

ostream&  operator<<(ostream& os, const StMcCalorimeterHit&);

inline int  StMcCalorimeterHit::module() const {return mModule; }
inline int  StMcCalorimeterHit::eta() const {return mEta; }
inline int  StMcCalorimeterHit::sub() const {return mSub; }
inline float StMcCalorimeterHit::dE()  const {return mdE; }
inline StMcTrack* StMcCalorimeterHit::parentTrack() const {return mParentTrack; }

#endif

