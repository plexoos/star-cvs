/*!
 * \class St_sce_Maker
 * \author B.Hippolyte 
 * \date 2000
 *
 *  Evaluation of the reconstruction for the Silicon Strip Detectors
 * 
 *  This maker controls the evaluation of the SSD reconstruction :
 *  the evaluation is perfomed by comparing the geant simulated hits
 *  to the output of the reconstruction. Efficiency and purity, as well
 *  as the the space point resolution can be determined. 
 * 
 * See documentation at http://star.in2p3.fr/STAR_informatique/hit_reconstruction.html#sce
 */
#ifndef STAR_St_sce_Maker
#define STAR_St_sce_Maker
#ifndef StMaker_H
#include "StMaker.h"
#endif

class TFile;
class TH1F;
class St_ssdDimensions;
class St_ssdWafersPosition;
class St_sce_ctrl;

class St_sce_Maker : public StMaker {
 private:
            St_ssdDimensions *m_geom_par;//!
	    St_ssdWafersPosition     *m_geom;//!
	    St_sce_ctrl     *m_ctrl;//!
 	    void resetSceStats(); //!
 	    void makeScfStats(); //!
 	    void showScfStats(); //!
 	    void makeScmStats(); //!
 	    void showScmStats(); //!
 	    void makeScmHistograms(); //!
	    
 protected:
	    TH1F  *devXl0;  //! distribution of local length deviation.
	    TH1F  *devXl1;  //! distribution of local width deviation.
	    TH1F  *devNrg;  //! distribution of energy deviation.
	    TH1F  *devXg0;  //! distribution of global x deviation.
	    TH1F  *devXg1;  //! distribution of global y deviation.
	    TH1F  *devXg2;  //! distribution of global z deviation.
	    Int_t statCluster[3][2]; //! statistique sur les clusters.
	    Int_t statSpt[3][5]; //! statistique sur les space points.

 public: 
	       St_sce_Maker(const char *name="sce_dspt");
   virtual       ~St_sce_Maker();
   virtual Int_t  Init();
   virtual Int_t  Make();
   virtual Int_t  Finish();
   virtual void   PrintInfo();

   virtual const char *GetCVS() const
     {static const char cvs[]="Tag $Name:  $ $Id: St_sce_Maker.h,v 1.11 2006/10/16 19:54:45 fisyak Exp $ built "__DATE__" "__TIME__ ; return cvs;}

   ClassDef(St_sce_Maker, 1)   //StAF chain virtual base class for Makers
};
#endif
/* $Id: St_sce_Maker.h,v 1.11 2006/10/16 19:54:45 fisyak Exp $
 *
 */
 /**************************************************************************
 *
 * $Log: St_sce_Maker.h,v $
 * Revision 1.11  2006/10/16 19:54:45  fisyak
 * St_DataSet => TDataSet
 *
 * Revision 1.10  2005/05/13 15:13:50  bouchet
 * reading ssd/geom and no more writeScmHistograms methods
 *
 * Revision 1.9  2005/05/12 08:22:11  lmartin
 * cvs tags added and histograms in the .hist branch
 *
 * Revision 1.8  2004/01/26 23:04:45  perev
 * WarnOff
 *
 * Revision 1.7  2003/10/08 15:44:19  suire
 * Removed a bug that happen during last commit
 *
 * Revision 1.5  2003/09/10 19:47:33  perev
 * ansi corrs
 *
 * Revision 1.4  2003/04/30 20:38:41  perev
 * Warnings cleanup. Modified lines marked VP
 *
 * Revision 1.3  2002/03/25 20:14:04  suire
 * Small memory leak fixes, doxygen documentation
 *
 *
 **************************************************************************/
