/***************************************************************************
 *
 * $Id: StSvtAnalysis.hh,v 1.3 2000/08/21 13:06:58 caines Exp $
 *
 * Author: 
 ***************************************************************************
 *
 * Description: 
 *
 ***************************************************************************
 *
 * $Log: StSvtAnalysis.hh,v $
 * Revision 1.3  2000/08/21 13:06:58  caines
 * Much improved hit finding and fitting
 *
 **************************************************************************/


#ifndef STSVTANALYSIS_HH
#define STSVTANALYSIS_HH

#include  "myPoint.h" 
#include  "StSvtHybridCluster.hh"

class StSvtHybridData;
class StSequence;

class StSvtAnalysis 
{

public:
  StSvtAnalysis(int numOfHybrids);
  virtual ~StSvtAnalysis();

  void   SetPointers(StSvtHybridData* hybAdjData,StSvtHybridData* hybRawData,
                     StSvtHybridCluster* hybClu, int numOfHybrids,
		     int PedOffset);
  void   FirstAndLastAnodes();
  void   CluFirstTimeBin();
  void   CluLastTimeBin();
  void   MomentAnalysis();
  int    GetFirstAnode(int clu);
  int    GetLastAnode(int clu);
  int    GetFirstTimeBin(int clu);
  int    GetLastTimeBin(int clu);
  int    GetCluCharge(int clu);
  int    GetCluFlag(int clu);
  int    GetCluPeakAdc(int clu);
  int    GetCluNumAnodes(int clu);
  int    GetCluNumPixels(int clu);
  int    GetnSvtClu();
  int    GetCluID(int clu);
  int    GetCluDeconvID(int clu);
  double GetMeanClusterAnode(int clu);
  double GetMeanClusterTimeBin(int clu);
  double GetSecondMomClusterAnode(int clu);
  double GetSecondMomClusterTimeBin(int clu);
  double GetCluXCov(int clu);
  double GetCluYCov(int clu);
  void   Report(int index);
  void   ResetMeanValues();
  void   SetBadAnTb(int numClus);

  int    Print_Pixels(int iRows, int iCols, int clu);
  int    Fill_Pixel_Array(int clu);
  void   free_matrix_d (int**, int);
  int    **malloc_matrix_d (int iRows, int iCols);
  POINT  *Find_Peaks (int iRows, int iCols, int *iNumPeaks);
  float  IsValidPeak (int iRows, int iCols, POINT  *Peaks, int iNumPeaks);
  int    BlockOut (int x, int y);
  int    Fit_Peaks(int iRows, int iCols, int iNumPeaks, POINT *Peaks, int clu);
  int    CatagorizeCluster(int iRows, int iCols, int igt3, int clu);
  int    Deconvolve_Cluster(int iRows, int iCols, int clu);
  void   SetHybIndex(int index);
  int    FillRawAdc();
  void   ClearRawAdc();

private:

  int m_clu;                               //!
  int m_row_p;                             //!
  int m_col_p;                             //!
  int m_adc_p;                             //!
  int m_SvtEvt;                            //!

  int m_deconv;                            //!
  int m_nWrkBkt;                           //!
  int m_nGt8;                              //!
  int m_nUndBkt;                           //! 
  int m_nSig;                              //! 

  int m_hybIndex;                          //!

  StSvtHybridData* mHybridData;            //!
  StSvtHybridData* mHybridRawData;         //!
  StSvtHybridCluster* mHybridCluster;      //!
  StSequence* mSvtSequence;                //!
  int numOfClusters, numOfMembers;
  int* mCluFirstAnode;                     //!
  int* mCluLastAnode;                      //!
  int* mCluFirstTimeBin;                   //!
  int* mCluLastTimeBin;                    //!
  int* mCluCharge;                         //!
  int* mCluFlag;                           //!
  int* mCluPeakAdc;                        //!
  int* mCluNumPixels;                      //!
  int* mCluNumAnodes;                      //!
  int* mHybridNum;                         //!
  int* mCluID;                             //!
  int* mCluDeconvID;                       //!
  double* mMeanClusterTimeBin;             //!
  double* mMeanClusterAnode;               //!
  double* mSecondMomClusterTimeBin;        //!
  double* mSecondMomClusterAnode;          //!
  double* mCluXCov;                        //!
  double* mCluYCov;                        //!

  int** m_countBadAn;                      //!
  int** m_countBadTb;                      //!

  StSvtClusterMemberInfo** tempMemberInfo; //!

  int** m_Pixels;                          //!
  int** m_Shadow;                          //!
  int** m_Raw;                             //!
   
  int mPedOffset;

  };

#endif
