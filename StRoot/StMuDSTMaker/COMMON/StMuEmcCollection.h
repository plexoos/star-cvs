/*!\class StMuEmcCollection
\author Alexandre A. P. Suaide

This is the EMC micro collection structure. From here it is possible
to get all EMC information (hits, clusters or points). <br><br>
EmcDet is defined as follow:
  - 1 = bemc
  - 2 = bprs
  - 3 = bsmde
  - 4 = bsmdp
Hit id, cluster id and point id starts from zero.
Tower id starts from 1 and goes to 4800 to match EMC Id
*/
#ifndef StMuEmcCollection__h
#define StMuEmcCollection__h
 
#include "TObject.h"
#include "TClonesArray.h"
#include "StMuEmcHit.h"
#include "StMuEmcCluster.h"
#include "StMuEmcPoint.h"
#include "StMuEmcTowerData.h"
#include "StEnumerations.h"
#include "Stiostream.h"

//enum {bemc=1, bprs=2, bsmde=3, bsmdp=4, eemc=5, eprs=6, esmdu=7, esmdv=8};

class StMuEmcCollection: public TObject
{
  public:
                      StMuEmcCollection();
                      StMuEmcCollection(StMuEmcCollection&);
    virtual           ~StMuEmcCollection();
    void              clear(Option_t *option="");     
    void              Clear(Option_t *option=""){clear();}     
    void              DeleteThis();
    
    int               getTowerADC(int id, int detector = bemc);    
    StEmcCrateStatus  getCrateStatus(int crate, int detector = bemc) { if (mTowerData) return mTowerData->crateStatus(crate,detector); else return crateUnknown; }    
    int               getNSmdHits(int detector);
    StMuEmcHit*       getSmdHit(int hitId, int detector = bsmde);    
    TClonesArray*     getPrsHits(int detector = bprs) { if (detector==bprs) return mPrsHits; else if (detector==eprs) return mEndcapPrsHits; else return 0; }
    int               getNPrsHits(int detector = bprs);
    StMuEmcHit*       getPrsHit(int hitId, int detector = bprs);    
    int               getNClusters(int detector);
    StMuEmcCluster*   getCluster(int clusterId,int detector);    
    int               getNPoints();
    StMuEmcPoint*     getPoint(int);
    int               getNEndcapPoints();
    StMuEmcPoint*     getEndcapPoint(int);

    // EEMC utility methods
    int   getNEndcapTowerADC(){ return 720;}
    void  getEndcapTowerADC(int ihit, 
			    int &adc, int &sec, int &sub, int & eta);

    int   getNEndcapPrsHits(){ return getNPrsHits(eprs);}
    StMuEmcHit *getEndcapPrsHit(int ihit, 
			  int &sec, int &sub, int & eta, int &pre);

    int   getNEndcapSmdHits(char uv); // set 'U' or 'V'
    StMuEmcHit * getEndcapSmdHit(char uv, int ihit,
				 int &sec, int &strip);
        
    void              setTowerADC(int,int, int detector = bemc);
    void              addSmdHit(int detector);
    void              addPrsHit(int detector = bprs);
    void              addCluster(int detector);
    void              addPoint();
    void              addEndcapPoint();
    void              setPrsArray(int detector, TClonesArray *cla);
    void              setSmdArray(int detector, TClonesArray *cla);
    void              setTowerData(StMuEmcTowerData *tow_dat) {mTowerData=tow_dat;}
    void              setCrateStatus(StEmcCrateStatus status, int crate, int detector = bemc) { if (mTowerData) mTowerData->setCrateStatus(status,crate,detector);}

  protected:
    void              init();
    void              packbits(unsigned char*, unsigned int, unsigned int, unsigned int);
    unsigned int      unpackbits(unsigned char*, unsigned int, unsigned int);
    
    // The char arrays are obsolete, but needed for backward compatibility
    unsigned char     mTowerADC[7200];
    unsigned char     mEndcapTowerADC[1080];
    StMuEmcTowerData* mTowerData; // This replaces the above arrays

    TClonesArray*     mPrsHits;
    TClonesArray*     mSmdHits[2];
    TClonesArray*     mEmcClusters[4];
    TClonesArray*     mEmcPoints;
    
    TClonesArray*     mEndcapPrsHits;
    TClonesArray*     mEndcapSmdHits[2];
    TClonesArray*     mEndcapEmcClusters[4];
    TClonesArray*     mEndcapEmcPoints;
    
    ClassDef(StMuEmcCollection,3)
};



#endif  
    
