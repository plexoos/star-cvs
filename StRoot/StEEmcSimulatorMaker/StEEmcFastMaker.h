// $Id: StEEmcFastMaker.h,v 1.1 2003/01/28 23:12:59 balewski Exp $
// $Log: StEEmcFastMaker.h,v $
// Revision 1.1  2003/01/28 23:12:59  balewski
// star
//

/* \class StEEmcFastMaker        
\author Jan Balewski

 Fast simulator of the E-EMC tower response converts energy loss in the tower tails generated by GEANT directly to ADC using eta-dependent conversion factor. The following assumptions are made:

   <li> energy deposit in any tail is equal to the sum of contribution from all particles passing its volume. Number of fibers within the tail and type of scintillator is ignored.
   <li> all fibers transport to PMT 100% of energy deposited, except tails with a total deposit below 100 keV.
   <li> total Geant energy deposit is recorded as in StEvent
   <li> ADC=4095 * geantEnergy /0.05 /60GeV / cosh(eta) is recorded as in StEvent 

<pre>
 Details of the code
-----------------------------

Decoding of E-EMC hits in StEvent
<hr>
 __ENDCAP__        
   TOWERS: det = kEndcapEmcTowerId
     sector 1-12   -->module 0-11
     subsector A-E -->submodule 0-4  
     jeta 1-12     -->eta  0-11
  
   PRE1-, PRE2-, POST-shower: all det = kEndcapEmcPreshowerId
     sector,subsector,eta : the same
     add 0, 5, or 10 to submodule for PRE1-, PRE2-, POST-shower

   SmdU : det=kEndcapSmdUStripId
   SmdV : det=kEndcapSmdVStripId
     sector 1-12    -->module 0-11
     stripID 1-288  -->eta 0-287
                    -->submodule=0 (not used)

---------------------------------------
StEEmcFastMaker::Make() {

 EEmcMCData  *evIN->readEventFromChain(this); // acquire E-EMC hit/track list

 EEevent eeveRaw; 
 evIN->write(&eeveRaw); // copy E-EMC hit/track to local TTree

 EEevent  eeveRaw.sumRawMC(eeve); //convert it to hit/tail list

  EE2ST(eeve, stevent); // add hit/tail list to StEvent

}

----------------------------------
usage in bfc.C
  

 StEEmcFastMaker *myMk=new StEEmcFastMaker;
 Char_t *after = "BbcSimulation";
 StMaker *xMk = chain->GetMaker(after);
 assert(xMk);
 chain->AddAfter(after,myMk);

</pre>
Exampl how to read back E-EMC data from StEvent:
www.star.bnl.gov/STAR/eemc -->How To

*/

#ifndef STAR_StEEmcFastMaker
#define STAR_StEEmcFastMaker


#ifndef StMaker_H
#include "StMaker.h"
#endif
class EEevent;
class StEvent;
class EEmcMCData;

/* Translation of StEvent names:  EEMC -->BEMC

  WARN: preserve sub<16=2^4, eta<512=2^9, mod<128=2^7

*/


class StEEmcFastMaker : public StMaker {
 private:
  EEmcMCData  *mevIN; ///< decoded raw .fzd event
  EEevent *meeve;    ///<  result stored in TTRee 
  void mEE2ST(EEevent*, StEvent*); ///< TTree-->StEvent
  void mST2EE(EEevent*, StEvent*); ///< StEvent -->TTree
  int mdbg;
  float msamplingFraction; ///< for Towers
  float * mfixTgain; ///<  (adc=g*de )ideal gains for Towers
  float mfixPgain; ///< (adc=g*de ) fixed gain for pre/post shower
  float mfixSMDgain; ///< (adc=g*de ) fixed gain for SMD

  StEvent *mlocalStEvent; ///< for test only

  // static Char_t  m_VersionCVS = "$Id: StEEmcFastMaker.h,v 1.1 2003/01/28 23:12:59 balewski Exp $";
  
 protected:
 public: 
  StEEmcFastMaker(const char *name="EEmcFastEvent");
  virtual       ~StEEmcFastMaker();
  virtual Int_t Init();
  virtual Int_t  Make();
  void SetLocalStEvent();
  void SetDbg(int k){mdbg=k;}
  void SetSamplingFraction(float x){ msamplingFraction=x;}; ///<default 0.05

  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StEEmcFastMaker.h,v 1.1 2003/01/28 23:12:59 balewski Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }
  
  ClassDef(StEEmcFastMaker, 1)   
};
    
#endif








