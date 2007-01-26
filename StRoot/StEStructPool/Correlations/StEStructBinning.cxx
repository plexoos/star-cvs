/**********************************************************************
 *
 * $Id: StEStructBinning.cxx,v 1.9 2007/01/26 17:17:07 msd Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  Binning defs as c-structs for 2pt-analysis
 *
 *
 ***********************************************************************/
#include "StEStructBinning.h"
#include <math.h>

ClassImp(StEStructBinning)

StEStructBinning* StEStructBinning::mInstance = 0;

StEStructBinning::StEStructBinning(){

  //--> Q ranges <--
  minQ=0.;
  maxQ=1.1;
  nQ = ESTRUCT_Q_BINS-1;
  dQ  = (maxQ-minQ)/(float)nQ;

  //--> phi ranges <--

  minPhi=-M_PI;
  maxPhi=M_PI;
  nPhi  = ESTRUCT_PHI_BINS-1;
  dPhi  = (maxPhi-minPhi)/(float)nPhi;

  // NOTE: We store delta_phi from 0 to pi in an array, but when we
  //       copy to histogram this is unfolded and "shifted" so histogram
  //       is from -pi/2 to 3pi/2. If you want to do something else
  //       you will need to change some of the routines.
  nDPhi = ESTRUCT_DPHI_BINS-1;
  dDPhi = M_PI/((float)nDPhi-1.0);

  maxSPhi=2*maxPhi;
  minSPhi=2*minPhi;
  nSPhi = ESTRUCT_SPHI_BINS-1;
  dSPhi=(maxSPhi-minSPhi)/(float)nSPhi;

  //--> eta ranges <--
  setEtaRange(-1.0,1.0);

  //--> yt ranges <--
  
  minYt=0.9; //0.9; //0.15;
  maxYt=4.5; //5.0;//4.5; // 0.925;
  nYt = ESTRUCT_YT_BINS-1;
  dYt = (maxYt-minYt)/(float)nYt;

  maxDYt=4.;//maxYt;
  minDYt=0.;//-maxYt;// 0; //-maxYt;
  nDYt= ESTRUCT_DYT_BINS-1;
  dDYt=(maxDYt-minDYt)/(float)nDYt; 

  maxSYt=10.; //2 * maxYt + 1.0;
  minSYt=0.9; //2 * minYt;
  nSYt = ESTRUCT_SYT_BINS-1;
  dSYt=(maxSYt-minSYt)/(float)nSYt;
  
 
  //--> xt ranges <--
  
  minXt= 1 - exp(-(sqrt(0.15*0.15+0.139*0.139)-0.139)/0.4);  // from Aya's code
  maxXt=0.99; // from Aya's code
  nXt = ESTRUCT_XT_BINS-1;
  dXt = (maxXt-minXt)/(float)nXt;

  //--> pt ranges <--
  
  minMeanPt= 0.0;
  maxMeanPt=10.0;
  nmeanPt = ESTRUCT_MEANPT_BINS-1;
  dmeanPt = (maxMeanPt-minMeanPt)/(float)nmeanPt;

  minPt=0.15; //0.15;
  maxPt=6.0;//4.5; // 0.925;
  nPt = ESTRUCT_PT_BINS-1;
  dPt = (maxPt-minPt)/(float)nPt;

  maxDPt=5.85;//maxPt;
  minDPt=-5.85;//-maxPt;// 0; //-maxPt;
  nDPt= ESTRUCT_DPT_BINS-1;
  dDPt=(maxDPt-minDPt)/(float)nDPt; 

  maxSPt=12.; //2 * maxPt + 1.0;
  minSPt=0.3; //2 * minPt;
  nSPt = ESTRUCT_SPT_BINS-1;
  dSPt=(maxSPt-minSPt)/(float)nSPt;

   //--> TPC Separation ranges <--
  maxTPCSep = 50;  //cm
  minTPCSep = 0;
  nTPCSep = ESTRUCT_TPCSEP_BINS - 1;
  dTPCSep = (maxTPCSep-minTPCSep)/(float)nTPCSep;

   //--> dEdx ranges <--
  maxdEdx = 15.0e-6;  //ionization units?
  mindEdx = 0;
  ndEdx = ESTRUCT_DEDX_BINS - 1;
  ddEdx = (maxdEdx-mindEdx)/(float)ndEdx;

   //--> ptot ranges (for use with dEdx) <--
  maxPtot = 1.5;  //GeV/c
  minPtot = 0;
  nPtot = ESTRUCT_PTOT_BINS - 1;
  dPtot = (maxPtot-minPtot)/(float)nPtot;

   //--> QAEta ranges <--
  maxQAEta = +2.0;
  minQAEta = -2.0;
  nQAEta = ESTRUCT_QAETA_BINS - 1;
  dQAEta = (maxQAEta-minQAEta)/(float)nQAEta;

   //--> QAPhi ranges <--
  maxQAPhi = +M_PI;
  minQAPhi = -M_PI;
  nQAPhi = ESTRUCT_QAPHI_BINS - 1;
  dQAPhi = (maxQAPhi-minQAPhi)/(float)nQAPhi;

   //--> QAPt ranges <--
  maxQAPt = +6.0;
  minQAPt =  0.0;
  nQAPt = ESTRUCT_QAPT_BINS - 1;
  dQAPt = (maxQAPt-minQAPt)/(float)nQAPt;

};

//-----------------------------------------------------------
void StEStructBinning::setEtaRange(float xmin, float xmax){

  //--> eta ranges <--

  minEta=xmin;
  maxEta=xmax;
  nEta = ESTRUCT_ETA_BINS-1;
  dEta= (maxEta-minEta)/(float)nEta;

  maxDEta=2*maxEta;//2*maxEta;
  minDEta=0.;//0; //2*minEta;
  nDEta=ESTRUCT_DETA_BINS-1;
  dDEta=(maxDEta-minDEta)/((float)nDEta-0.5);

  calculateDEtaWeights(); // --> MUST do whenever setEtaRange is called!!!!

  maxSEta=2*maxEta;
  minSEta=2*minEta;
  nSEta = ESTRUCT_SETA_BINS-1;
  dSEta=(maxSEta-minSEta)/(float)nSEta;

}


//------------------------------------------------------------
void StEStructBinning::calculateDEtaWeights() {

    // init to 0
    for(int i=0;i<ESTRUCT_DETA_BINS;i++) {
      mdetaWeights.x.deta[i]=0.;
    }

    // Assume first bin will be centered on 0.
    // (Depends on ideta(), detaVal() and minDEta.)

    // calculate weight at cent of bin unless it is a center bin
    // then use 1/4 shift ...
    double fdeta0 = dDEta/4.0;
    mdetaWeights.x.deta[0]  = 1.0/(1.0 - (fdeta0/maxDEta));
    for(int i=1;i<ESTRUCT_DETA_BINS-1;i++){
        fdeta0 = fabs(detaVal(i));
        mdetaWeights.x.deta[i]  = 1.0/(1.0 - (fdeta0/maxDEta));
    }
}	
   

/***********************************************************************
 *
 * $Log: StEStructBinning.cxx,v $
 * Revision 1.9  2007/01/26 17:17:07  msd
 * Implemented new binning scheme: dEta stored in array with bin centered at zero, dPhi array has bins centered at zero and pi.  Final DEtaDPhi has 25x25 bins with dPhi bin width of pi/12 so all major angles are centered in bins.
 *
 * Revision 1.8  2006/10/02 22:20:57  prindle
 * Store only quadrant of eta_Delta - phi_Delta array/histogram.
 * Store half of eta_Sigma - phi_Delta array/histogram.
 * This required modifications in Binning.
 * I had a bug in the pair loop (which left +- not fully symmetrized)
 * and had to make changes in cut bins for mode 5 (and 3 I think)
 * when I fixed this.
 * Also change crossing cut to use only two parameters, the sign of
 * the magnetic field being taken from the MuDst.
 *
 * Revision 1.7  2006/04/10 23:42:32  porter
 * Added sameSide() & awaySide() methods to PairCut (so only defined in 1 place)
 * and added the eta_delta weighting as a binned correctin defined by the eta-limits in
 * the StEStructBinning object
 *
 * Revision 1.6  2006/04/04 22:10:10  porter
 * a handful of changes (specific to correlations)
 *  - added StEStructQAHists so that if NOT input frm Maker, each analysis has its own
 *  - used ability to get any max,min val from the cut class - or z-vertex binning
 *  - put z-vertex binning into 1 place
 *  - switched back 1st line of pair cut method to keep pair if good, not to reject if bad.
 *  - Pair cut object is now pointer in correlations
 *  - some diagnostic printouts available from macro
 *  - Duncan's delta-phi binning change
 *
 * Revision 1.5  2006/02/22 22:05:14  prindle
 * Removed all references to multRef (?)
 * Added cut mode 5 for particle identified correlations.
 * Other cut modes should be same as before
 *
 * Revision 1.4  2005/09/14 17:14:21  msd
 * Large update, added new pair-cut system, added pair density plots for new analysis mode (4), added event mixing cuts (rewrote buffer for this)
 *
 * Revision 1.3  2005/03/03 01:30:43  porter
 * updated StEStruct2ptCorrelations to include pt-correlations and removed
 * old version of pt-correlations from chunhuih (StEStruct2ptPtNbar)
 *
 * Revision 1.2  2004/06/25 03:11:49  porter
 * New cut-binning implementation and modified pair-cuts for chunhui to review
 *
 * Revision 1.1  2003/10/15 18:20:46  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/









