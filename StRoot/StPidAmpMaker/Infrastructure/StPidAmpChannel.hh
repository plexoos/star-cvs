/***************************************************************************
 *
 * $Id: StPidAmpChannel.hh,v 1.5 2000/07/22 22:27:13 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             StPidAmpChannel contains multiple StPidAmpNet.
 *             Only tracks satisfying channel condition can be filled into 
 *             nets inside that channel
 ***************************************************************************
 *
 * $Log: StPidAmpChannel.hh,v $
 * Revision 1.5  2000/07/22 22:27:13  aihong
 * move files from StPidAmpMaker to StEventUtilities
 *
 * Revision 1.4  2000/05/01 16:59:25  aihong
 * clean up
 *
 * Revision 1.3  2000/04/11 15:34:23  aihong
 * change to adapt dividing trks by channel for faster filling
 *
 * Revision 1.2  2000/04/09 16:46:05  aihong
 * change for adapting NHitsDcaNet added
 *
 * Revision 1.1.1.1  2000/03/09 17:48:34  aihong
 * Installation of package
 *
 **************************************************************************/


#ifndef StPidAmpChannel_hh
#define StPidAmpChannel_hh

#include <iostream.h>
#include "TH1.h"

#include "StPidAmpMaker/Infrastructure/StPidParamVector.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpNetVector.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpChannelInfo.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpParticle.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpNet.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpChannelCollection.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpDefaultNet.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpNHitsNet.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpNHitsDcaNet.hh"
#include "StPidAmpMaker/Include/StPidAmpEnum.hh"
#include "StPidAmpMaker/StPidAmpTrkVector.h"
#include "StEventUtilities/StPidAmpChannelInfoOut.h"


class StPidAmpChannel {

public:

      StPidAmpChannel();
      StPidAmpChannel(StPidAmpChannelInfo& channelInfo,StPidAmpNetType netType=noDependent);

      virtual ~StPidAmpChannel();



  void drawFittings(); //draw bandFits and ampFits.
  void filterAndFillTrks(StPidAmpTrkVector* trks); //pick up trks in this channel
  void fillBGNet(StPidAmpChannelCollection* set);
  void setBandParams4Nets(StPidParamVector pars);

  void setBandFitsDraw(bool br);
  void setAmpFitsDraw(bool br);
  void setResoFitsDraw(bool br);
  void processChannel(bool fitBd, bool fitPth, bool fitAp, bool fitLr);

  StPidAmpNetVector*     netVector();
  StPidAmpChannelInfoOut channelInfoOut() const;
      
  static void setDefaultBandParams(StPidParamVector& bandPars);
  //set StPidAmpNet::mDefaultBandParams.

private:

      void setUp(StPidAmpNetType netType); //set nets. called by constructor
      void fillChannelInfoOut(); //fill mChannelInfoOut for out put to disk.


      StPidAmpNetVector* mNetCollect;
      StPidAmpTrkVector* mTrks;

      StPidAmpChannelInfo    mChannelInfo;
      StPidAmpChannelInfoOut mChannelInfoOut;

      bool mDrawBandFits;
      bool mDrawAmpFits;
      bool mDrawResoFits;

 
   
};

ostream& operator<<(ostream& s, StPidAmpChannel& channel);

inline void StPidAmpChannel::setBandFitsDraw(bool br){mDrawBandFits=br;}
inline void StPidAmpChannel::setAmpFitsDraw(bool br){mDrawAmpFits=br;}
inline void StPidAmpChannel::setResoFitsDraw(bool br){mDrawResoFits=br;}
inline StPidAmpNetVector* StPidAmpChannel::netVector(){return mNetCollect;}

inline StPidAmpChannelInfoOut StPidAmpChannel::channelInfoOut() const {return mChannelInfoOut;}

#endif
