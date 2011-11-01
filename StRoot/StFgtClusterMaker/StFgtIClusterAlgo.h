// $Id: StFgtIClusterAlgo.h,v 1.6 2011/11/01 18:46:30 sgliske Exp $
// $Log: StFgtIClusterAlgo.h,v $
// Revision 1.6  2011/11/01 18:46:30  sgliske
// Updated to correspond with StEvent containers, take 2.
//
// Revision 1.5  2011/10/03 19:39:46  avossen
// compiling version of simple cluster maker, changed PushBack->pushBack energy->charge in ClusterArray and Cluster
//
// Revision 1.4  2011/09/27 22:14:27  avossen
// cluster maker compiles
//
// Revision 1.3  2011/09/27 17:19:05  avossen
// simple cluster makers
//
// Revision 1.2  2011/08/24 14:30:44  avossen
// Continued raw maker development
//
// Revision 1.1  2011/08/23 03:05:09  avossen
// *** empty log message ***
//
//
//author Anselm Vossen
//
//abstract base class for cluster algorithm implementation
//
//
#ifndef STAR_StFgtIClusterAlgo_HH
#define STAR_StFgtIClusterAlgo_HH

#include "Stypes.h"
class StFgtStripCollection;
class StFgtHitCollection;

class StFgtIClusterAlgo
{
 public:
  //subclasses must implement this function that takes raw hits from StEvent and fills the Cluster collection
  virtual Int_t doClustering( StFgtStripCollection&, StFgtHitCollection& )=0;
  virtual Int_t Init()=0;

 private:
  ClassDef( StFgtIClusterAlgo, 1 );  
};

#endif
