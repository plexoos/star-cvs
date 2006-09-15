// $Id: StSsdBarrel.hh,v 1.8 2006/09/15 21:03:14 bouchet Exp $
//
// $Log: StSsdBarrel.hh,v $
// Revision 1.8  2006/09/15 21:03:14  bouchet
// id_mctrack is using for setIdTruth and propagated to the hit
//
// Revision 1.7  2005/04/23 08:56:20  lmartin
// physics and pedestal data processing separated
//
// Revision 1.6  2005/03/22 13:45:01  lmartin
// new member mActiveLadders added
//
// Revision 1.5  2005/03/18 14:04:06  lmartin
// missing CVS header added
//

#ifndef STSSDBARREL_HH
#define STSSDBARREL_HH

class TFile;
class ssdDimensions_st;
class ssdConfiguration_st;
class ssdWafersPosition_st;

class St_ssdWafersPosition;
class St_ssdStripCalib;
class St_ssdPedStrip;
class St_spa_strip;
class St_sdm_calib_db;
class St_scf_cluster;
class St_scm_spt;
class StSsdClusterControl;
class StSsdDynamicControl;
class StSsdLadder;

class StSsdWafer;

class StSsdHitCollection;

class StSsdBarrel
{
 public:
  StSsdBarrel(ssdDimensions_st  *dimensions, ssdConfiguration_st *config);
  ~StSsdBarrel();

  StSsdBarrel(const StSsdBarrel & originalBarrel);
  StSsdBarrel& operator=(const StSsdBarrel  originalBarrel);

  void  initLadders(St_ssdWafersPosition *wafpos);
  void  initLadders(ssdWafersPosition_st *position, int positionSize);
//   int   readDeadStripFromTable(table_head_st *condition_db_h, sdm_condition_db_st *condition_db); 
  int   readStripFromTable(St_spa_strip *spa_strip);
  int   readNoiseFromTable(St_sdm_calib_db *spa_noise, StSsdDynamicControl *dynamicControl);
  int   readNoiseFromTable(St_ssdStripCalib *strip_noise, StSsdDynamicControl *dynamicControl);
  int   writeNoiseToFile(St_spa_strip *spa_strip);
  int   writeNoiseToFile(St_ssdPedStrip *pedStrip, char myLabel[]);
  int   readClusterFromTable(St_scf_cluster *scf_cluster);
  int   writeClusterToTable(St_scf_cluster *cluster);
  int   writeClusterToTable(St_scf_cluster *scf_cluster,St_spa_strip *spa_strip);
  int   writePointToContainer(St_scm_spt *scm_spt,StSsdHitCollection *ssdHitColl);
  int   writePointToContainer(St_scm_spt *scm_spt, StSsdHitCollection* ssdHitColl,St_scf_cluster *scf_cluster);    
  void  doSideClusterisation(int *numberOfCluster,StSsdClusterControl *clusterControl);   
  int   doClusterMatching(ssdDimensions_st *dimensions,StSsdClusterControl *clusterControl);
  void  convertDigitToAnalog(StSsdDynamicControl *dynamicControl);
  void  convertUFrameToOther(ssdDimensions_st *dimensions);
  void  sortListStrip();
  void  sortListCluster();
  int   getNumberOfLadders();
  int   isActiveLadder(int i);
  void  debugUnPeu(int monLadder, int monwafer);

  StSsdLadder** mLadders;
//   StSsdWafer** mWafers;
//   int** mDeadStripP;
//   int** mDeadStripN;
  
 private:
  int    mSsdLayer;
  int    mNLadder;
  int    mNWaferPerLadder;
  int    mNStripPerSide;
  int    mActiveLadders[20];
  int idWaferToWaferNumb(int idWafer);
  int waferNumbToIdWafer(int waferNumb);
};
inline int StSsdBarrel::getNumberOfLadders() { return mNLadder;}
#endif
