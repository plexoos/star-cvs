/*
 * $Id: StiTrackingPlots.h,v 2.11 2004/11/10 21:45:11 pruneau Exp $
 *
 *
 * $Log: StiTrackingPlots.h,v $
 * Revision 2.11  2004/11/10 21:45:11  pruneau
 * added diagnostic plots
 *
 * Revision 2.10  2003/11/14 22:31:57  andrewar
 * Added isPrimary() cut so only Primary tracks are histogrammed
 * (not a mix of Primary and global).
 *
 * Revision 2.9  2003/07/30 19:19:24  pruneau
 * sigh
 *
 * Revision 2.7  2003/05/01 20:46:52  pruneau
 * changed error parametrization
 *
 * Revision 2.6  2003/04/29 18:48:34  pruneau
 * *** empty log message ***
 *
 * Revision 2.5  2003/04/04 14:44:25  pruneau
 * Fix to the hit error calculator and the getCharge methods.
 *
 * Revision 2.4  2003/03/31 17:19:02  pruneau
 * various
 *
 * Revision 2.3  2003/03/13 21:21:29  pruneau
 * getPhase() fixed. MUST inclde -helicity()*pi/2
 *
 * Revision 2.2  2003/03/13 18:59:16  pruneau
 * various updates
 *
 * Revision 2.1  2003/03/12 16:36:28  andrewar
 * Sti tracking plots package.
 *
 */
#ifndef StiTrackingPlots_H_INCLUDED
#define StiTrackingPlots_H_INCLUDED

class TH1D;
class TH2D;
class TH3D;
class TFile;
#include "Sti/Base/HistogramGroup.h"

class StiTrackingPlots : public HistogramGroup
{
 public:
  StiTrackingPlots();
  StiTrackingPlots(const string & name, const string & description);
  ~StiTrackingPlots();

  void initialize();
  void setOutFileName(string nme){mOutFile = nme;}
  void fill(StiTrackContainer* mTrackStore);
  void addFilter(StiDefaultTrackFilter *filter){mFilter.push_back(filter);}

 private:
  vector<StiDefaultTrackFilter *> mFilter;
  string mOutFile;

  TH1D *numTracks;
  TH1D * _eta;
  TH1D * _etaPlus;
  TH1D * _etaMinus;
  TH1D * _phi;
  TH1D * _phiPlus;
  TH1D * _phiMinus;
  TH1D * _pt;
  TH1D * _ptPlus;
  TH1D * _ptMinus;
  TH1D * _dca40;
  TH1D * _dca40Plus;
  TH1D * _dca40Minus;
  TH1D * _globalDca;
  TH1D * _globalDcaPlus;
  TH1D * _globalDcaMinus;

  TH2D *_nptsVsPt;
  TH2D *_nptsVsPtPlus;
  TH2D *_nptsVsPtMinus;
  TH2D *_nptsVsEtaPtGt200;
  TH2D *_nptsVsEtaPtLt200;

  TH2D *_nptsVsEta;
  TH2D *_nptsVsEtaPlus;
  TH2D *_nptsVsEtaMinus;

  TH2D *_nptsVsPhi;
  TH2D *_nptsVsPhiPlus;
  TH2D *_nptsVsPhiMinus;

  //rad length maps
  TH2D * radLengthZ;
  TH2D * radLengthPhi;
  TH2D * radLengthEta;

  TH2D *_xLastHitVsXLastNode;
  TH2D *_xLastHitVsXLastNode1;
  TH2D *_xLastHitVsXLastNode2;

  TH1D * _chi2;
  TH2D * _chi2VsNpts;

  //track kinematics & helix parameters
  //make all plots 3D - value,Phi,Eta - then cut 
  TH1D * mPx;

  TH3D * mCurv;
  TH3D * mHeli;
  TH3D * mMomX;
  TH3D * mMomY;
  TH3D * mMomZ;
  TH3D * mPhase;

  TH3D * mGDcavNptsvEtaA;
  //TH3D * mPDcavNptsvEtaA;
  TH3D * mGDcavNptsvPtA;
  //TH3D * mPDcavNptsvPtA;
  TH3D * mNptsvPtvEtaA;
  TH3D * mGDcavEtavPtA;
  //TH3D * mPDcavEtavPtA;
  TH3D * mGDcavNptsvEtaP;
  //TH3D * mPDcavNptsvEtaP;
  TH3D * mGDcavNptsvPtP;
  //TH3D * mPDcavNptsvPtP;
  TH3D * mNptsvPtvEtaP;
  TH3D * mGDcavEtavPtP;
  //TH3D * mPDcavEtavPtP;
  TH3D * mGDcavNptsvEtaM;
  //TH3D * mPDcavNptsvEtaM;
  TH3D * mGDcavNptsvPtM;
  //TH3D * mPDcavNptsvPtM;
  TH3D * mNptsvPtvEtaM;
  TH3D * mGDcavEtavPtM;
  //TH3D * mPDcavEtavPtM;

  TH1D* _etaSvt[3]        ;
  TH1D* _phiSvt[3]        ;
  TH1D* _ptSvt[3]         ;
  TH1D* _globalDcaSvt[3]  ;
  TH2D* _nptsVsPtSvt[3]   ;
  TH2D* _nptsVsEtaSvt[3]  ;
  TH2D* _nptsVsPhiSvt[3]  ;
  TH1D* _chi2Svt[3]       ;
  TH2D* _chi2VsNptsSvt[3] ;

  TH2D* _chi2VsDca;
  TH2D* _chi2VsDcaSvt[3];

  
};


#endif
