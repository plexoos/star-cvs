//
// $id$
//
// $Log: StPointCollection.h,v $
// Revision 1.6  2003/05/26 13:44:35  suaide
// added setPrint() method
//
// Revision 1.5  2001/12/01 02:44:50  pavlinov
// Cleanp for events with zero number of tracks
//
// Revision 1.4  2001/11/06 23:35:27  suaide
// fixed bug in the way we get magnetic field
//
// Revision 1.3  2000/12/01 17:05:42  subhasis
// track matching after assignment, PRS, deltaeta,deltaphi added to StEmcPoint
//
// Revision 1.2  2000/08/29 20:13:39  subhasis
// Modified to accept StEvent input and writing out StEvent output for Emc
//
// Revision 1.1  2000/05/15 21:18:33  subhasis
// initial version
//
// Pi0 Candidate Finder Maker for EMC
//
//
// Author: Subhasis Chattopadhyay, Jan 2000
//

#ifndef STAR_StPointCollection
#define STAR_StPointCollection

#ifndef HEP_SYSTEM_OF_UNITS_H
#include "SystemOfUnits.h"                                                     
#endif

#include "StEpcConstants.h"                                                     
#include "StEpcCut.h"                                                     

#include <vector>
#include <algorithm>  // min() max()
#include <utility>    // pair

#if !defined(ST_NO_NAMESPACES)
using std::vector;
using namespace units;
using std::min;
using std::max; 
#endif  

#ifdef ST_NO_TEMPLATE_DEF_ARGS
// Syntax currently required by Solaris compiler
#define StVector(T) vector<T, allocator<T> >
typedef vector<int, allocator<int> > intVector;
#else
#define StVector(T) vector<T>
typedef vector<int> intVector;
#endif

#include "TObjArray.h"
#include "TDataSet.h"
#include "TDataSetIter.h"
#include "TTableSorter.h"
#include "StPi0Candidate.h"
#include "StEpcMaker.h"

class TBrowser;
class StEmcClusterCollection;
class StEmcCluster;

typedef StVector(StEmcCluster*) StMatchVecClus;
typedef StVector(StEmcCluster*)::iterator StMatchVecClusIter;

#include "tables/St_dst_track_Table.h"

class StPointCollection : public TDataSet 
{

  private:
    TObjArray         mPoints;
    TObjArray         mPointsReal;
    Int_t             mNPoints;
    Int_t             mNPointsReal;
    double            mBField;
    Bool_t            mPrint;
  protected:   

  public: 

                      StPointCollection();
                      StPointCollection(const Char_t *);
    virtual           ~StPointCollection();
    virtual  void     Browse(TBrowser* b); // mPoints .. 

    void              SetBField(double B) { mBField=B; }
    Int_t             NPoints() const;
    const TObjArray*  Points() const;
    Int_t             NPointsReal() const;
    const TObjArray*  PointsReal() const;

    Int_t             findEmcPoints(StEmcClusterCollection*,
                                    StEmcClusterCollection*,
                                    StEmcClusterCollection*,
                                    StEmcClusterCollection*,
                                    StTrackVec &);

    virtual Int_t     TrackSort(const StTrackVec &) const;

    void              ClusterSort(StEmcClusterCollection*,
                                  StEmcClusterCollection*,
                                  StEmcClusterCollection*,
                                  StEmcClusterCollection*);

    virtual Int_t     MatchClusterAndTrack(const StMatchVecClus,
                                           const StMatchVecClus,
                                           const StMatchVecClus,
                                           const StMatchVecClus,
                                           const FloatVector,
                                           const FloatVector,
                                           const FloatVector,
                                           Int_t *);

    virtual Int_t     addPoints(Float_t*);
    void    setPrint(Bool_t a) {mPrint = a;}

  ClassDef(StPointCollection,1)// Base class for electromagnetic calorimeter Point collection 
};
//
inline        Int_t        StPointCollection::NPointsReal() const {return mNPointsReal;}
inline  const TObjArray*   StPointCollection::PointsReal()  const {return &mPointsReal;}
inline        Int_t        StPointCollection::NPoints()     const {return mNPoints;}
inline  const TObjArray*   StPointCollection::Points()      const {return &mPoints;}

#endif





