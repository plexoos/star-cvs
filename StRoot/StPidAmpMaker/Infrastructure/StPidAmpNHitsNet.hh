/***************************************************************************
 *
 * $Id: StPidAmpNHitsNet.hh,v 1.2 2000/05/01 16:59:26 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             StPidAmpNHitsNet is a basic unit for building a
 *             NHits instance of StPidAmpChannelCollection
 *             
 ***************************************************************************
 *
 * $Log: StPidAmpNHitsNet.hh,v $
 * Revision 1.2  2000/05/01 16:59:26  aihong
 * clean up
 *
 * Revision 1.1.1.1  2000/03/09 17:48:34  aihong
 * Installation of package
 *
 **************************************************************************/


#ifndef StPidAmpNHitsNet_hh
#define StPidAmpNHitsNet_hh

#include "StPidAmpMaker/Infrastructure/StPidAmpNet.hh"

class StPidAmpNHitsNet : public StPidAmpNet {

public:

  StPidAmpNHitsNet();
  StPidAmpNHitsNet(StPidAmpParticle def, StPidAmpChannelInfo channelInfo);

   void fitBand();
   void fitAPath(StPidAmpPath& path, StPidAmpTrkVector* trks);
   void fitAmp(StPidAmpTrkVector* trks);
   void fitReso();
   void fillPathFittedSlices();
   ostream& put(ostream& s) const;// for calling the right put() by operator.
private:

   double ( *funcAmpPt) (double *, double *);
   double ( *funcResoPt) (double *, double *);


};



#endif
