//
// $id$
//
// $Log: StEmcPreClusterCollection.h,v $
// Revision 1.7  2001/04/17 23:51:43  pavlinov
// Clean up before MDC4
//
// Revision 1.6  2001/02/01 22:23:12  suaide
// Fixed some memory leaks
//
// Revision 1.5  2000/09/08 21:48:00  suaide
//
//
// See README for details
//
// Revision 1.4  2000/08/24 22:11:35  suaide
//
//
// restored some files for background compatibility
//
// Revision 1.3  2000/08/24 19:45:37  suaide
//
//
// small modifications: some cout has been removed
//
// Revision 1.2  2000/08/24 11:26:48  suaide
//
//
//
// by A. A. P. Suaide - 2000/08/24 07:25:00
//
// Notes:
//
// 1. Full StEvent Compatible
// 2. Read hits from StEvent object
// 3. Write clusters in StEvent format and old format to keep background
//    compatibility
// 4. Do clustering in bemc, bprs, bsmde, bsmdp
// 5. Included method StPreEclMaker::SetClusterCollection
//
// Revision 1.1  2000/05/15 21:24:00  subhasis
// initial version
//
// Author: Alexandre A. P. Suaide (version 2.0)
//         Subhasis Chattopadhyay,
//         Aleksei Pavlinov , July 1999
//
// 08/15/2000 - By A. A. P. Suaide
// included method checkClustersInModule to check the clusters


#ifndef STAR_StEmcPreClusterCollection
#define STAR_StEmcPreClusterCollection

#include "TObjArray.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include "TMatrix.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"
#include "St_TableSorter.h"
#include "StEmcPreCluster.h"
#include "StEvent/StEmcDetector.h"

class StEmcPreClusterCollection : public St_DataSet {
private:
  Int_t           mDetector;
  Float_t         mEnergySeed;
  Float_t         mEnergyAdd;
  Float_t         mEnergyThresholdAll;
  Int_t           mSizeMax;
  Int_t           mNclusters;
  TObjArray       mClusters;
  Bool_t          kIsOk;
  Bool_t          kCheckClustersOk;
  Bool_t          kStEvOk;
 
protected:   
public: 

                  StEmcPreClusterCollection();  
                  StEmcPreClusterCollection(const Char_t *);
                  StEmcPreClusterCollection(const Char_t *,StEmcDetector*);
  virtual         ~StEmcPreClusterCollection();

  Int_t           Detector() const;
  Float_t         EnergySeed() const;
  Float_t         EnergyAdd() const;
  Float_t         EnergyThresholdAll() const;
  Int_t           SizeMax() const;
  Int_t           Nclusters() const;
  TObjArray*      Clusters();
  Bool_t          IsOk();

  void            setDetector(Int_t);
  void            setEnergySeed(Float_t);
  void            setEnergyAdd(Float_t);
  void            setEnergyThresholdAll(Float_t);
  void            setSizeMax(Int_t);
  void            setNclusters(Int_t);  
  void            setCheckClusters(Bool_t);

  virtual  void   Browse(TBrowser *b);
  virtual  void   Delete(Option_t *opt="");

// methods for cluster finder
  Int_t           findClusters(); 
  virtual  void   findClustersInModule(Int_t);   
  virtual  Int_t  testOnNeighbor(Int_t);
  virtual  void   addPreCluster(Int_t , TArrayI*);
// methods for chisqr check
  void            checkClustersInModule(Int_t);  
  Int_t           applyProfile(Int_t,TArrayI);
  Float_t         calcChiSqrt(Int_t,TArrayF,TArrayF,Int_t,TArrayF,TArrayF);
  void            setNewClusters(Int_t,Int_t,Int_t,TArrayI);
  Float_t         profile(Float_t,Float_t,Float_t);
// methods for cluster printing 
  virtual  void   printCluster(Int_t, StEmcPreCluster*);
  virtual  void   printClusters(Int_t n=5, Int_t start=5, Int_t m=0);
  virtual  void   printClustersAll();
  virtual  void   printClusterFromModule(Int_t m=0);  // *MENU* 
           void   printConf();
  
  
  ClassDef(StEmcPreClusterCollection,2)// Base class for electromagnetic calorimeter cluster collection 
};

inline Int_t      StEmcPreClusterCollection::Detector()   const  {return mDetector;}
inline Float_t    StEmcPreClusterCollection::EnergySeed() const  {return mEnergySeed;}
inline Float_t    StEmcPreClusterCollection::EnergyAdd()  const {return mEnergyAdd;}
inline Float_t    StEmcPreClusterCollection::EnergyThresholdAll() const{return mEnergyThresholdAll;}
inline Int_t      StEmcPreClusterCollection::Nclusters()   const {return mNclusters;}
inline Int_t      StEmcPreClusterCollection::SizeMax() const {return mSizeMax;} 
inline TObjArray* StEmcPreClusterCollection::Clusters() {return &mClusters;}

inline void       StEmcPreClusterCollection::setDetector(Int_t var)  {mDetector = var;}
inline void       StEmcPreClusterCollection::setEnergySeed(Float_t var){mEnergySeed = var;}
inline void       StEmcPreClusterCollection::setEnergyAdd(Float_t var) {mEnergyAdd = var;}
inline void       StEmcPreClusterCollection::setEnergyThresholdAll(Float_t var){mEnergyThresholdAll = var;}
inline void       StEmcPreClusterCollection::setNclusters(Int_t var) {mNclusters = var;}
inline void       StEmcPreClusterCollection::setSizeMax(Int_t var) {mSizeMax = var;}
inline Bool_t     StEmcPreClusterCollection::IsOk(){return kIsOk;}
#endif




