// $Id: StSvtHitMaker.h,v 1.7 2001/09/22 01:07:09 caines Exp $
// $Log: StSvtHitMaker.h,v $
// Revision 1.7  2001/09/22 01:07:09  caines
// Fixes now that AddData() is cleared everyevent
//
// Revision 1.6  2001/08/07 20:52:16  caines
// Implement better packing of svt hardware and charge values
//
// Revision 1.5  2001/03/22 20:46:54  caines
// Comment out some of the QA histograms
//
// Revision 1.4  2000/11/30 20:42:08  caines
// Some more evaluation and use dataase
//
// Revision 1.3  2000/08/26 20:36:28  caines
// Adjustments for StSvtCoordinateTransform calls
//
// Revision 1.2  2000/08/24 04:26:56  caines
// Printout for debugging
//
// Revision 1.1  2000/08/21 13:03:40  caines
// First version of cluster->STAR hit maker
//
//
#ifndef STAR_StSvtHit
#define STAR_StSvtHit
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StSvtHit base class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif

#include "tables/St_scs_spt_Table.h"
#include "tables/St_srs_srspar_Table.h"
#include "tables/St_svg_shape_Table.h"
#include "tables/St_svg_geom_Table.h"


#include "TH2.h"

class TFile;
class TNtuple;

class St_scs_spt;
class St_svg_shape;
class St_svg_geom;
class St_srs_srspar;
class StSvtHybridCollection;
class StSvtAnalysedHybridClusters;
class StSvtData;
class StSvtGeantHits;
 
class StSvtHitMaker : public StMaker
{
 public: 
  StSvtHitMaker(const char *name = "SvtHit");
  StSvtHitMaker(StSvtHitMaker& svthit);
  ~StSvtHitMaker();

  virtual Int_t Init();
  virtual Int_t Make();
  virtual Int_t Finish();
  Int_t FillHistograms();
  Int_t GetSvtRawData();
  Int_t GetSvtClusterData();
  void TransformIntoSpacePoint();
  void SaveIntoTable(int numOfCluster, int index);
  void SaveIntoNtuple(int numOfCluster, int index);
  void SetWriteNtuple(int iwrite){iWrite = iwrite;};
  Int_t Eval();
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StSvtHitMaker.h,v 1.7 2001/09/22 01:07:09 caines Exp $ built "__DATE__" "__TIME__ ; return cvs;}


 protected:

  int iWrite;

  St_svg_shape    *m_shape; //!
  St_svg_geom     *m_geom;  //!
  St_srs_srspar   *m_srs_srspar; //!

  StSvtHybridCollection *mSvtCluColl; //!
  StSvtHybridCollection *mSvtGeantHitColl; //!

  StSvtAnalysedHybridClusters  *mSvtBigHit;  //!
  
  StSvtData *mSvtData; //!
  StSvtGeantHits *mSvtGeantHit;  //!
  
  TH2F     *m_x_vs_y;  //! x vs y of Si points
  TH2F     **m_waf_no;  //! ladder no vs z of Si hit

  TNtuple *m_ClusTuple;                       //!
  TFile   *m_hfile;                           //!

  // Evaluation histos
  TH1F *mTimeHitResolution;  //!
  TH1F *mAnodeHitResolution;  //!
  TH1F *mXHitResolution;  //!
  TH1F *mYHitResolution;  //!
  TH1F *mZHitResolution;  //!
  TH2F *mHitResolution;   //!

  ClassDef(StSvtHitMaker,1)   //virtual base class for Makers

};


#endif
