// $Id: StQABookHist.cxx,v 1.47 2000/07/28 19:25:20 lansdell Exp $ 
// $Log: StQABookHist.cxx,v $
// Revision 1.47  2000/07/28 19:25:20  lansdell
// added histogram of number of events without a primary vertex
//
// Revision 1.46  2000/07/26 19:57:50  lansdell
// new histograms and functionality added (e.g., overlay several histograms, new printlist option qa_shift)
//
// Revision 1.45  2000/06/23 15:44:24  lansdell
// restore PID method to using global tracks & retitle PID histogram
//
// Revision 1.44  2000/06/13 00:32:38  lansdell
// added SVT,TPC vertex resolution check; check that pidTraits()[0] exists
//
// Revision 1.43  2000/06/02 01:11:52  lansdell
// added several x,y,z-dca to beam axis histograms
//
// Revision 1.42  2000/05/25 03:52:11  lansdell
// mirrored globtrk histograms for primtrk; removed ev0_eval, vertex: detector id histograms; added generator pT for TPC (|eta|<1), vertex: radial position histograms; merged vertex methods
//
// Revision 1.41  2000/03/17 20:46:25  kathy
// change scale on the globtrk iflag histogram as requested by Spencer
//
// Revision 1.40  2000/02/17 22:00:50  kathy
// changed ranges of histograms and labelled axes for diff geant vs reco vtx hist
//
// Revision 1.39  2000/02/11 21:31:59  kathy
// adjust booking range for geant vs reco plots
//
// Revision 1.38  2000/02/11 15:56:04  kathy
// change limits on number of hits in detector histograms; fill number of hits in detector histograms
//
// Revision 1.37  2000/02/10 23:02:44  kathy
// changed limits on linear impact param hist; added new hist of detector id values for dst_point table
//
// Revision 1.36  2000/02/10 21:31:29  kathy
// add another set of impact param hist so we can see them in linear scale too
//
// Revision 1.35  2000/02/10 19:49:40  kathy
// use kEventVtxId to select primary verteices instead of value 1
//
// Revision 1.34  2000/02/07 19:49:06  kathy
// removed L3 trigger histograms and methods that created them - this table is no longer standard on the DST; created methods BookHistEval and MakeHistEval for geant vs reco evaluation histograms; filled geant vs reco evaluation histograms for table-based data
//
// Revision 1.33  2000/02/04 19:53:56  kathy
// added 2 more histograms - for med and small range of # hits in detector
//
// Revision 1.32  2000/02/03 22:02:30  kathy
// adding histograms for Akio - needed smaller ranges of some of them for use by peripheral collisions group
//
// Revision 1.31  2000/02/02 16:35:23  kathy
// fixing some histograms - booking params
//
// Revision 1.30  2000/02/01 21:35:10  kathy
// fix code for xi mass in StEvent histograms; change curvature and impact param histograms so it's log of the value, plotted in linear scale
//
// Revision 1.29  2000/01/31 22:15:25  kathy
// added Gene's code to make mass plot for Xi's in table and StEvent versions
//
// Revision 1.28  2000/01/31 17:23:00  kathy
// change scale of chisq plots for tpc,svt in globtrk,primtrk tables - change to be 5
//
// Revision 1.27  2000/01/27 22:02:12  kathy
// changed titles of some hist to make clearer
//
// Revision 1.26  2000/01/13 22:10:03  kathy
// change histogram scales for angles so they can be used for DST tables & StEvent
//
// Revision 1.25  2000/01/11 15:28:32  kathy
// limits on residual histograms changed; St_QA_Maker changed to give proper inputs to routine prop_one_track which is from pams/global/egr - NOTE! Now St_QA_Maker class requires that you load the St_global librarycvs -n update!
//
// Revision 1.24  2000/01/10 21:22:29  kathy
// now use Spiros' new code in pams/global/egr/prop_one_track to get primary track residuals - note must now load St_global library! - don't have magnetic field working from the dst yet...
//
// Revision 1.23  2000/01/07 20:35:00  kathy
// make some corrections to filling hist; add point hist for each det separately
//
// Revision 1.22  1999/12/29 17:52:30  kathy
// changes to hist limits and list of logY scales
//
// Revision 1.21  1999/12/21 23:10:59  kathy
// unpack number of points correctly in primtrk table; change some limits
//
// Revision 1.20  1999/12/17 22:11:32  kathy
// add psi vs phi hist, change limits
//
// Revision 1.19  1999/12/16 23:12:03  kathy
// fixed list of default histograms, had wrong title and added a few for tables and stevent; rescaled some histograms and fixed titles in booking
//
// Revision 1.18  1999/12/16 19:52:36  kathy
// fix hist titles in QABookHist; unpack n_point,n_fit_point,n_max_point correctly for globtrk table - must still fix for primtrk table - in St_QA_Maker
//
// Revision 1.17  1999/12/15 20:32:17  kathy
// separated the tpc and tpc+svt histograms for globtrk table; had to book and fill new histograms, add histograms to default logy list AND had to change what values of iflag I cut on for filling each different type of track in makehistglob method
//
// Revision 1.16  1999/12/15 18:31:05  kathy
// added 4 new histogram to globtrk for tpc - r0,phi0,z0,curvature; also put 3 of these in default logY list; also changed scale on iflag hist. for globtrk & primtrk
//
// Revision 1.15  1999/12/15 17:17:32  kathy
// changed the dedx histograms to the scale GeV/cm - which is the scale in the dst table
//
// Revision 1.14  1999/12/14 18:33:23  kathy
// removed 4 ftpc histograms as per Janet's request
//
// Revision 1.13  1999/12/12 23:09:47  kathy
// add pt vs eta in ftpc histogram as per Janet
//
// Revision 1.12  1999/12/12 17:17:24  kathy
// fixed limits on ftpc  histograms
//
// Revision 1.11  1999/12/10 22:28:19  kathy
// fix limits of evsum histogram
//
// Revision 1.10  1999/12/10 18:11:18  kathy
// changed limits of some histograms so we can at least see all the data; changed booking order for globtrk histograms
//
// Revision 1.9  1999/12/10 17:38:24  kathy
// now reprint canvas on each page of postscript output file; also changed some histogram limits
//
// Revision 1.8  1999/12/08 22:58:17  kathy
// changed histogram limits and made names smaller
//
// Revision 1.7  1999/12/07 23:14:18  kathy
// fix primary vtx histograms for dst tables; split apart the ftpc and tpc in the dedx histograms
//
// Revision 1.6  1999/12/07 16:50:43  kathy
// fix limits on histograms
//
// Revision 1.5  1999/12/06 22:25:05  kathy
// split apart the tpc and ftpc (east & west) histograms for the globtrk table; had to add characters to end of each histogram pointer to differentiate the different ones; updated the default list of hist to be plotted with logy scale
//
// Revision 1.4  1999/11/29 21:50:36  kathy
// remove St_QATestTables_Maker class - not used anywhere; remove SetDraw method from StQABookHist method - not needed
//
// Revision 1.3  1999/11/23 19:00:51  lansdell
// Reorganized Make() and include files (Gene)
//
// Revision 1.2  1999/11/22 22:46:41  lansdell
// update to identify histogram method used (StEvent or DST tables) by Gene; StEventQAMaker code partially completed (run bfcread_dst_EventQAhist.C)
//
// Revision 1.1  1999/11/19 22:44:42  kathy
// took histogram booking out of St_QA_Maker as per Thomas' request and put it into separate class StQABookHist which can now be used also by Curtis' class to book histograms - thanks for your help Gene!
// 
///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  StQABookHist abstract base class for QA Histogram Makers             //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include "TH1.h"
#include "TH2.h"
#include "StQABookHist.h"
#include "StMultiH1F.h"

// tables  from geant
//#include "tables/St_g2t_vertex_Table.h"

const Int_t   StQABookHist::nxpT = 50;
const Int_t   StQABookHist::nyeta = 50;
const Float_t StQABookHist::xminpT = 0.0;
const Float_t StQABookHist::xmaxpT = 5.0;
//const Float_t StQABookHist::ymineta = -2.0;
//const Float_t StQABookHist::ymaxeta =  2.0;

const Int_t StQABookHist::nchisq = 50;
//const Int_t StQABookHist::nmass  = 40;
//const Int_t StQABookHist::ntau   = 40;
const Int_t StQABookHist::ndedx  = 50;
const Int_t StQABookHist::npnt   = 50;
const Int_t StQABookHist::nleng  = 50;
const Int_t StQABookHist::npsi   = 36;
const Int_t StQABookHist::knpsi  = 42;
const Int_t StQABookHist::ntrk   = 50;
const Int_t StQABookHist::nvrt   = 100;
const Int_t StQABookHist::nmnpt  = 50;
const Int_t StQABookHist::nmneta = 50;
const Int_t StQABookHist::nxyz   = 50;
const Int_t StQABookHist::knyeta = 60;
const Int_t StQABookHist::knid   = 10;
const Int_t StQABookHist::cnp   = 50;
const Int_t StQABookHist::cndedx = 50;    

const Float_t StQABookHist::kminnid  = 0.0;
const Float_t StQABookHist::kmaxnid  = 10.0;
const Float_t StQABookHist::minpsi   = 0.0;
const Float_t StQABookHist::kminpsi  = -60.0;
const Float_t StQABookHist::maxpsi   = 360.0;
const Float_t StQABookHist::minchisq = 0.;
const Float_t StQABookHist::maxchisq = 10.0;
const Float_t StQABookHist::minmass  = 0.0;
const Float_t StQABookHist::maxmass  = 2.0;
const Float_t StQABookHist::minpnt   = 0.0;
const Float_t StQABookHist::maxpnt   = 50.0;
const Float_t StQABookHist::minleng  = 0.0;
const Float_t StQABookHist::maxleng  = 200.0;
const Float_t StQABookHist::mintau   = 0.0;
const Float_t StQABookHist::maxtau   = 20.0;
const Float_t StQABookHist::mintrk   = 0.0;
const Float_t StQABookHist::maxtrk   = 8000.0;
const Float_t StQABookHist::minvrt   = 2000.0;
const Float_t StQABookHist::maxvrt   = 4000.0;
const Float_t StQABookHist::minmpt   = 0.0;
const Float_t StQABookHist::maxmpt   = 2.0;
const Float_t StQABookHist::minmeta  = -0.2;
const Float_t StQABookHist::maxmeta  = 0.2;
const Float_t StQABookHist::kmineta  = -3.0;
const Float_t StQABookHist::kmaxeta  = 3.0;
const Float_t StQABookHist::minxyz   = 0.0;
const Float_t StQABookHist::maxxyz   = 50.0;
const Float_t StQABookHist::cminp = 0.0;
const Float_t StQABookHist::cmaxp = 2.0;
const Float_t StQABookHist::cmindedx = 0.0;
const Float_t StQABookHist::cmaxdedx =  0.1e-04*1e6; // change from GeV to keV per cm

ClassImp(StQABookHist)
  
//_____________________________________________________________________________
StQABookHist::StQABookHist(const char *name, const char *title, const char* type) : StMaker(name,title), QAHistType(type) {

//  - zero all pointers defined in the header file

  mNullPrimVtx = 0; //! histogram for number of events without primary vertex

// for method MakeEvSum - from table event_summary
  m_trk_tot_gd = 0;         //! number of good global tracks divided by total
  m_glb_trk_tot=0;          //! # tracks total from globtrk
  m_glb_trk_tot_sm=0;       //! # tracks total from globtrk, small range
  m_glb_trk_plusminus=0;    //! # trks pos/neg. 
  m_glb_trk_plusminus_sm=0; //! # trks pos/neg., small range
  m_glb_trk_chg=0;          //! all charge east/west, tpc
  m_glb_trk_prim=0;         //! # trks from primaries
  m_glb_trk_prim_sm=0;      //! # trks from primaries, small range
  m_vert_total=0;    //! total number of vertices
  m_vert_total_sm=0; //! total number of vertices, small range
  m_mean_pt=0;       //! mean pt value
  m_mean_pt_sm=0;    //! mean pt value, small range
  m_mean_eta=0;      //! mean eta value 
  m_rms_eta=0;       //! rms eta value 
  m_prim_vrtr=0;     //! primary vrtx r position
  m_prim_vrtx0=0;    //! primary vrtx x position
  m_prim_vrtx1=0;    //! primary vrtx y position
  m_prim_vrtx2=0;    //! primary vrtx z position

// for method MakeGlob - from table globtrk
  m_globtrk_tot=0;
  m_globtrk_tot_sm=0;
  m_globtrk_good=0;
  m_globtrk_good_sm=0;
  m_globtrk_iflag=0;
  m_det_id=0;
  m_dcaToBeamXY=0;
  m_dcaToBeamZ1=0;
  m_dcaToBeamZ2=0;
  m_dcaToBeamZ3=0;
  m_zDcaTanl=0;
  m_zDcaZf=0;
  m_zDcaPsi=0;
  m_zDcaPhi0=0;

  m_pointT=0;
  m_pointFE=0;
  m_pointFW=0;
  m_max_pointT=0;
  m_max_pointFE=0;
  m_max_pointFW=0;
  m_fit_pointT=0;
  m_fit_pointFE=0;
  m_fit_pointFW=0;
  m_glb_ratioT=0;
  m_glb_ratioFE=0;
  m_glb_ratioFW=0;
  m_glb_ratiomT=0;
  m_glb_ratiomFE=0;
  m_glb_ratiomFW=0;
  m_glb_chargeT=0;
  m_glb_chargeFE=0;
  m_glb_chargeFW=0;
  m_glb_r0T=0;
  m_glb_phi0T=0;
  m_glb_z0T=0;
  m_glb_curvT=0;
  m_glb_rfT=0;
  m_glb_rfFE=0;
  m_glb_rfFW=0;
  m_glb_padfT=0;  
  m_glb_xf0=0;
  m_glb_xfT=0;
  m_glb_xfFE=0;
  m_glb_xfFW=0;
  m_glb_yf0=0;
  m_glb_yfT=0;  
  m_glb_yfFE=0;  
  m_glb_yfFW=0;     
  m_glb_zf0=0;     
  m_glb_zfT=0; 
  m_glb_zfFE=0; 
  m_glb_zfFW=0;
  m_glb_f0=0;
  m_glb_radfT=0;
  m_glb_radfFE=0;
  m_glb_radfFW=0;
  m_psiT=0;  
  m_psiFE=0;  
  m_psiFW=0;        
  m_tanlT=0;   
  m_glb_thetaT=0;  
  m_etaT=0;    
  m_etaFE=0;    
  m_etaFW=0;        
  m_pTT=0;
  m_pTFE=0;
  m_pTFW=0;
  m_momT=0; 
  m_momFE=0; 
  m_momFW=0;        
  m_lengthT=0;  
  m_lengthFE=0;  
  m_lengthFW=0;  
  m_chisq0T=0;     
  m_chisq0FE=0;     
  m_chisq0FW=0;     
  m_chisq1T=0;
  m_chisq1FE=0;
  m_chisq1FW=0;
  m_glb_impactT=0; 
  m_glb_impactrT=0; 

  m_pointTS=0;        
  m_max_pointTS=0;    
  m_fit_pointTS=0;    
  m_glb_ratioTS=0;    
  m_glb_ratiomTS=0;   
  m_glb_chargeTS=0;   
  m_glb_r0TS=0;       
  m_glb_phi0TS=0;     
  m_glb_z0TS=0;       
  m_glb_curvTS=0;     
  m_glb_rfTS=0;       
  m_glb_xfTS=0;       
  m_glb_yfTS=0;       
  m_glb_zfTS=0;       
  m_glb_xf0TS=0;      
  m_glb_yf0TS=0;      
  m_glb_zf0TS=0;      
  m_glb_f0TS=0;      
  m_glb_radfTS=0;     
  m_psiTS=0;          
  m_tanlTS=0;         
  m_glb_thetaTS=0;    
  m_etaTS=0;          
  m_momTS=0;          
  m_pTTS=0;           
  m_lengthTS=0;       
  m_chisq0TS=0;       
  m_chisq1TS=0;       
  m_glb_impactTS=0;   
  m_glb_impactrTS=0;   

  m_pT_eta_recT = 0;
  m_pT_eta_recFE = 0;
  m_pT_eta_recFW = 0;
  m_globtrk_xf_yfT = 0;
  m_globtrk_xf_yfFE = 0;
  m_globtrk_xf_yfFW = 0;
  m_tanl_zfT  = 0;
  m_mom_trklengthT = 0;
  m_eta_trklengthT = 0;
  m_eta_trklengthFE = 0;
  m_eta_trklengthFW = 0;
  m_npoint_lengthT = 0;	
  m_npoint_lengthFE = 0;	
  m_npoint_lengthFW = 0;		  
  m_fpoint_lengthT = 0;
  m_fpoint_lengthFE = 0;
  m_fpoint_lengthFW = 0;
  m_chisq0_momT = 0;
  m_chisq1_momT = 0;
  m_chisq0_etaT = 0;
  m_chisq1_etaT = 0;
  m_chisq0_dipT = 0;
  m_chisq1_dipT = 0;
  m_chisq0_zfT = 0;
  m_chisq1_zfT = 0;
  m_chisq0_phiT = 0;
  m_nfptonpt_momT = 0;
  m_nfptonpt_etaT = 0;

  m_pT_eta_recTS= 0;
  m_globtrk_xf_yfTS = 0;
  m_tanl_zfTS  = 0;
  m_mom_trklengthTS = 0;
  m_eta_trklengthTS = 0;
  m_npoint_lengthTS = 0;	
  m_fpoint_lengthTS = 0;
  m_chisq0_momTS = 0;
  m_chisq1_momTS = 0;
  m_chisq0_etaTS = 0;
  m_chisq1_etaTS = 0;
  m_chisq0_dipTS = 0;
  m_chisq1_dipTS = 0;
  m_chisq0_zfTS = 0;
  m_chisq1_zfTS = 0;
  m_chisq0_phiTS = 0;
  m_nfptonpt_momTS = 0;
  m_nfptonpt_etaTS = 0;

// for method MakeDE - from table dst_dedx
  m_ndedxr=0;        //! number of tracks with dedx info

  m_ndedxT=0;         //! number of point to find dE/dx
  m_dedx0T=0;         //! dE/dx [0] - mean
  m_dedx1T=0;         //! dE/dx [1] - sigma
  
  m_ndedxFE=0;         //! number of point to find dE/dx
  m_dedx0FE=0;         //! dE/dx [0] - mean
  m_dedx1FE=0;         //! dE/dx [1] - sigma
  
  m_ndedxFW=0;         //! number of point to find dE/dx
  m_dedx0FW=0;         //! dE/dx [0] - mean
  m_dedx1FW=0;         //! dE/dx [1] -sigma
  
// for method MakeHistPrim - from table primtrk
  m_primtrk_tot=0;
  m_primtrk_tot_sm=0;
  m_primtrk_good=0;
  m_primtrk_good_sm=0;
  m_primtrk_iflag=0;
  m_pdet_id=0;

  m_ppointT=0;
  //m_ppointFE=0;
  //m_ppointFW=0;
  m_pmax_pointT=0;
  //m_pmax_pointFE=0;
  //m_pmax_pointFW=0;
  m_pfit_pointT=0;
  //m_pfit_pointFE=0;
  //m_pfit_pointFW=0;
  m_prim_ratioT=0;
  //m_prim_ratioFE=0;
  //m_prim_ratioFW=0;
  m_prim_ratiomT=0;
  //m_prim_ratiomFE=0;
  //m_prim_ratiomFW=0;
  m_prim_chargeT=0;
  //m_prim_chargeFE=0;
  //m_prim_chargeFW=0;
  m_prim_r0T=0;
  m_prim_phi0T=0;
  m_prim_z0T=0;
  m_prim_curvT=0;
  m_prim_xf0=0;
  m_prim_xfT=0;
  //m_prim_xfFE=0;
  //m_prim_xfFW=0;
  m_prim_yf0=0;
  m_prim_yfT=0;  
  //m_prim_yfFE=0;  
  //m_prim_yfFW=0;     
  m_prim_zf0=0;     
  m_prim_zfT=0; 
  //m_prim_zfFE=0; 
  //m_prim_zfFW=0;
  m_prim_f0=0;
  m_prim_radfT=0;
  //m_prim_radfFE=0;
  //m_prim_radfFW=0;
  m_ppsiT=0;  
  //m_ppsiFE=0;  
  //m_ppsiFW=0;        
  m_ptanlT=0;   
  m_prim_thetaT=0;  
  m_petaT=0;    
  //m_petaFE=0;    
  //m_petaFW=0;        
  m_ppTT=0;
  //m_ppTFE=0;
  //m_ppTFW=0;
  m_pmomT=0; 
  //m_pmomFE=0; 
  //m_pmomFW=0;        
  m_plengthT=0;  
  //m_plengthFE=0;  
  //m_plengthFW=0;  
  m_pchisq0T=0;     
  //m_pchisq0FE=0;     
  //m_pchisq0FW=0;     
  m_pchisq1T=0;
  //m_pchisq1FE=0;
  //m_pchisq1FW=0;     
  m_prim_impactT=0; 
  m_prim_impactrT=0; 

  m_ppointTS=0;        
  m_pmax_pointTS=0;    
  m_pfit_pointTS=0;    
  m_prim_ratioTS=0;    
  m_prim_ratiomTS=0;   
  m_prim_chargeTS=0;   
  m_prim_r0TS=0;       
  m_prim_phi0TS=0;     
  m_prim_z0TS=0;       
  m_prim_curvTS=0;     
  m_prim_xfTS=0;       
  m_prim_yfTS=0;       
  m_prim_zfTS=0;       
  m_prim_xf0TS=0;      
  m_prim_yf0TS=0;
  m_prim_zf0TS=0;
  m_prim_f0TS=0;
  m_prim_radfTS=0;
  m_ppsiTS=0;          
  m_ptanlTS=0;         
  m_prim_thetaTS=0;    
  m_petaTS=0;          
  m_momTS=0;          
  m_ppTTS=0;           
  m_plengthTS=0;       
  m_pchisq0TS=0;       
  m_pchisq1TS=0;       
  m_prim_impactTS=0;   
  m_prim_impactrTS=0;   

  m_ppT_eta_recT = 0;
  //m_ppT_eta_recFE = 0;
  //m_ppT_eta_recFW = 0;
  m_primtrk_xf_yfT = 0;
  //m_primtrk_xf_yfFE = 0;
  //m_primtrk_xf_yfFW = 0;
  m_ptanl_zfT  = 0;
  m_pmom_trklengthT = 0;
  m_peta_trklengthT = 0;
  //m_peta_trklengthFE = 0;
  //m_peta_trklengthFW = 0;
  m_pnpoint_lengthT = 0;	
  //m_pnpoint_lengthFE = 0;	
  //m_pnpoint_lengthFW = 0;		  
  m_pfpoint_lengthT = 0;
  //m_pfpoint_lengthFE = 0;
  //m_pfpoint_lengthFW = 0;
  m_pchisq0_momT = 0;
  m_pchisq1_momT = 0;
  m_pchisq0_etaT = 0;
  m_pchisq1_etaT = 0;
  m_pchisq0_dipT = 0;
  m_pchisq1_dipT = 0;
  m_pchisq0_zfT = 0;
  m_pchisq1_zfT = 0;
  m_pnfptonpt_momT = 0;
  m_pnfptonpt_etaT = 0;

  m_ppT_eta_recTS= 0;
  m_primtrk_xf_yfTS = 0;
  m_ptanl_zfTS  = 0;
  m_pmom_trklengthTS = 0;
  m_peta_trklengthTS = 0;
  m_pnpoint_lengthTS = 0;	
  m_pfpoint_lengthTS = 0;
  m_pchisq0_momTS = 0;
  m_pchisq1_momTS = 0;
  m_pchisq0_etaTS = 0;
  m_pchisq1_etaTS = 0;
  m_pchisq0_dipTS = 0;
  m_pchisq1_dipTS = 0;
  m_pchisq0_zfTS = 0;
  m_pchisq1_zfTS = 0;
  m_pnfptonpt_momTS = 0;
  m_pnfptonpt_etaTS = 0;

// for method MakeHistGen - from table particle
  m_H_pT_eta_gen=0; //! pT versus eta Spectra for generated
  m_H_pT_gen=0;     //! pT Spectra for generated
  m_H_pT_genT=0;    //! pT Spectra for generated, tpc
  m_H_eta_gen=0;    //! eta Spectra for generated
  m_H_vtxx=0;       //! production vertex (mm)
  m_H_vtxy=0;       //! production vertex (mm)
  m_H_vtxz=0;       //! production vertex (mm)
  m_H_npart=0;      //! total num particles generated
  m_H_npart_sm=0;   //! total num particles generated,small range
  m_H_ncpart=0;     //! number of charged e,mu,proton,kaon,pion
  m_H_ncpart_sm=0;  //! number of charged e,mu,proton,kaon,pion,small range

// for MakeHistPID - from tables primtrk & dst_dedx 
  m_p_dedx_rec=0;   //! dedx vs p
  
  // for method MakeHistVertex - from table dst_vertex
  m_v_num=0;   //! number of vertices
  m_v_num_sm=0;//! number of vertices,small range
  m_v_vtxid=0; //! vertex type
  m_v_x=0;     //! vertex coordinates in
  m_v_y=0;     //!  STAR reference 
  m_v_z=0;     //!   system
  m_v_pchi2=0; //! chisq per dof of vertex fit
  m_v_r=0;     //! radius to vertex
  
  m_pv_vtxid=0; //! row1-vertex type
  m_pv_x=0;     //! row1-vertex coordinates in
  m_pv_y=0;     //!  STAR reference 
  m_pv_z=0;     //!   system
  m_pv_pchi2=0; //! row1-chisq per dof of vertex fit
  m_pv_r=0;     //! radius to primary vertex

  m_vtx_z=0;    //! SVT vertex finder resolution relative to main finder

  m_v0           =0; //! # v0 vertices
  m_ev0_lama_hist=0; //! Lambda mass
  m_ev0_k0ma_hist=0; //! K0 mass  
  m_xi_tot=0;        //! number of vertices
  m_xi_ma_hist=0;    //! Xi Mass
  m_kink_tot=0;      //! number of kinks  

// for method MakeHistPoint
  m_pnt_tot=0;     //! number of hits
  m_pnt_tot_med=0; //! number of hits, med range
  m_pnt_tot_sm=0;  //! number of hits, small range
  m_pnt_id=0;      //! detector id of the hit
  m_pnt_tpc=0;     //! number of hits tpc
  m_pnt_svt=0;     //! number of hits svt
  m_pnt_ssd=0;     //! number of hits ssd
  m_pnt_ftpcE=0;   //! number of hits ftpcE
  m_pnt_ftpcW=0;   //! number of hits ftpcW

// for method MakeHistRich
  m_rich_tot=0;   //! number of rich hits

// for method MakeHistEval  
  m_geant_reco_pvtx_x=0;  //! prim vtx x, diff geant - reco
  m_geant_reco_pvtx_y=0;  //! prim vtx y, diff geant - reco
  m_geant_reco_pvtx_z=0;  //! prim vtx z, diff geant - reco
  m_geant_reco_vtx_z_z=0; //! prim vtx z, diff geant - reco vs reco z
}
//_____________________________________________________________________________
Int_t StQABookHist::Init(){
 
  cout << " In StQABookHist::Init  " << endl;

//book histograms --------------
  mNullPrimVtx = new TH1F("QaNullPrimVtx","num. of events missing primary vertex",5,-1,1);
  mNullPrimVtx->SetYTitle("# of events");

  BookHistEvSum();
  BookHistGlob();
  BookHistPrim();
  BookHistDE();
  BookHistGen();
  BookHistPID();
  BookHistVertex();
  BookHistPoint();
  BookHistRich();
  BookHistEval();
  
  return StMaker::Init();

}
//_____________________________________________________________________________
Int_t StQABookHist::Make(){

  // Call methods to fill histograms
  
  // histograms from table event_summary
  MakeHistEvSum();
  // histograms from table globtrk
  MakeHistGlob();
  // histograms from table dst_dedx
  MakeHistDE();
  // histograms from table primtrk
  MakeHistPrim();
  // histograms from table particle
  MakeHistGen();  
  // histograms from table primtrk & dst_dedx
  MakeHistPID();
  // histograms from table dst_vertex,dst_v0_vertex,dst_xi_vertex,dst_kinkVertex
  MakeHistVertex();
  // histograms from table point
  MakeHistPoint();
  // histograms from table g2t_rch_hit
  MakeHistRich();
  // histograms from geant and reco tables 
  MakeHistEval();

  return kStOK;

}
//_____________________________________________________________________________
StQABookHist::~StQABookHist(){

}
//_____________________________________________________________________________
const char* StQABookHist::NameIt(const char* name) {

  return ((QAHistName=QAHistType) += name).Data();

}
//_____________________________________________________________________________
const char* StQABookHist::TitleIt(const char* name) {

  return (((QAHistTitle=QAHistType) += " ") += name).Data();

}
//_____________________________________________________________________________
TH1F* StQABookHist::QAH1F(const Text_t* name, const Text_t* title,
   Int_t nbinsx, Axis_t xlow, Axis_t xup) {

  return (new TH1F(NameIt(name),TitleIt(title),nbinsx,xlow,xup));

}
//_____________________________________________________________________________
TH2F* StQABookHist::QAH2F(const Text_t* name, const Text_t* title,
   Int_t nbinsx, Axis_t xlow, Axis_t xup,
   Int_t nbinsy, Axis_t ylow, Axis_t yup) {

  return (new TH2F(NameIt(name),TitleIt(title),nbinsx,xlow,xup,nbinsy,ylow,yup));

}
//_____________________________________________________________________________
TH2F* StQABookHist::QAMH1F(const Text_t* name, const Text_t* title,
   Int_t nbinsx, Axis_t xlow, Axis_t xup,
   Int_t nbinsy, Axis_t ylow, Axis_t yup) {

  return (TH2F*) (new StMultiH1F(NameIt(name),TitleIt(title),nbinsx,xlow,xup,nbinsy,ylow,yup));

}
//_____________________________________________________________________________
void StQABookHist::BookHistEvSum(){
  
// for method MakeEvSum - from table event_summary

  m_trk_tot_gd     = QAH1F("QaEvsumTrkGoodDTotal","evsum: num good global tracks over total",55,0.,1.1);
    m_trk_tot_gd->SetXTitle("number of good/total tracks");
  m_glb_trk_tot    = QAH1F("QaEvsumTrkTot","evsum: num tracks total ",ntrk,0.,10000.);
  m_glb_trk_tot_sm = QAH1F("QaEvsumTrkTotsm","evsum: num tracks total ",ntrk,0.,20.);
  m_glb_trk_plusminus     = QAH1F("QaEvsumPlusMinusTrk","evsum: num pos. over neg trks",ntrk,0.8,1.4);
  m_glb_trk_plusminus_sm  = QAH1F("QaEvsumPlusMinusTrksm","evsum: num pos. over neg trks",ntrk,0.,4.);
  m_glb_trk_chg    = QAH1F("QaEvsumTotChg","softmon: all charge east/west,tpc",60,-3,3);
  m_glb_trk_prim   = QAH1F("QaEvsumTrkPrim","evsum: num good tracks from primaries ",80,0.,4000.);
  m_glb_trk_prim_sm       = QAH1F("QaEvsumTrkPrimsm","evsum: num good tracks from primaries ",80,0.,20.);
  m_vert_total     = QAH1F("QaEvsumVertTot","evsum: total num of vertices",80,0.,2000.);
  m_vert_total_sm  = QAH1F("QaEvsumVertTotsm","evsum: total num of vertices",80,0.,20.);
  m_mean_pt        = QAH1F("QaEvsumMeanPt","evsum: mean pt", nmnpt,0.,2.0);
  m_mean_pt_sm     = QAH1F("QaEvsumMeanPtsm","evsum: mean pt", nmnpt,0.,0.5);
  m_mean_eta   = QAH1F("QaEvsumMeanEta","evsum: mean eta", nmneta,-0.4,0.4);
  m_rms_eta    = QAH1F("QaEvsumRmsEta","evsum: rms eta", nmneta,0.,2.5);
  m_prim_vrtr  = QAH1F("QaEvsumPrimVertR","evsum: R of primary vertex",40,-1.,1.);
  m_prim_vrtx0 = QAH1F("QaEvsumPrimVertX","evsum: X of primary vertex",40,-1.,1.);
  m_prim_vrtx1 = QAH1F("QaEvsumPrimVertY","evsum: Y of primary vertex",40,-1.,1.);
  m_prim_vrtx2 = QAH1F("QaEvsumPrimVertZ","evsum: Z of primary vertex",nxyz,-50.,50.);
}
//_____________________________________________________________________________
void StQABookHist::BookHistGlob(){
  
// for method MakeGlob - from table globtrk

// general

  m_globtrk_tot     = QAH1F("QaGtrkTot","globtrk: tot num tracks - all",40,0.,10000.);
  m_globtrk_tot_sm  = QAH1F("QaGtrkTotsm","globtrk: tot num tracks - all",40,0.,20.);
  m_globtrk_iflag   = QAH1F("QaGtrkFlag","globtrk: iflag - all ",200,-999.,1001.);
  m_globtrk_good    = QAH1F("QaGtrkGood","globtrk: tot good tracks - all",40,0.,10000.);
  m_globtrk_good_sm = QAH1F("QaGtrkGoodsm","globtrk: tot good tracks - all",40,0.,20.);
  m_det_id          = QAH1F("QaGtrkDetId","globtrk: Detector ID good tracks - all",25,0.,25.);
  m_dcaToBeamXY     = QAH2F("QaGtrkDcaBeamXY","globtrk: xy-DCA to Beam Axis (z=0)",80,-4,4,80,-4,4);
  m_dcaToBeamZ1     = QAH1F("QaGtrkDcaBeamZ1","globtrk: z-DCA to Beam Axis, coarse scale",100,-200,0);
  m_dcaToBeamZ2     = QAH1F("QaGtrkDcaBeamZ2","globtrk: z-DCA to Beam Axis, coarse scale",100,0,200);
  m_dcaToBeamZ3     = QAH1F("QaGtrkDcaBeamZ3","globtrk: z-DCA to Beam Axis, near z=0",80,-20,20);
  m_zDcaTanl        = QAH2F("QaGtrkZdcaTanl","globtrk: z-DCA to Beam Axis vs tanl",80,-20,20,32,-4,4);
  m_zDcaZf          = QAH2F("QaGtrkZdcaZf","globtrk: z-DCA to Beam Axis vs z-first",80,-20,20,50,-300,300);
  m_zDcaPsi         = QAH2F("QaGtrkZdcaPsi","globtrk: z-DCA to Beam Axis vs psi",80,-20,20,64,0,360);
  m_zDcaPhi0        = QAH2F("QaGtrkZdcaPhi0","globtrk: z-DCA to Beam Axis vs azimuth (phi0) at start",80,-20,20,64,0,360);

// 1D tpc

  m_pointT      = QAH1F("QaGtrkNPntT",   "globtrk: N points on trk,tpc", 50, 0.,50.);
  m_max_pointT  = QAH1F("QaGtrkNPntMaxT","globtrk: N max pnts on trk, tpc", 50, 0.,100.);
  m_fit_pointT  = QAH1F("QaGtrkNPntFitT","globtrk: N fit pnts on trk, tpc", 50, 0.,50.);
  m_glb_ratioT  = QAH1F("QaGtrkRnfT",    "globtrk: ratio Nfit/tot pnt, tpc", 55,0.,1.1);
    m_glb_ratioT->SetMinimum(10);
  m_glb_ratiomT = QAH1F("QaGtrkRnmT",    "globtrk: ratio Nfit/max pnt, tpc", 55, 0., 1.1);
    m_glb_ratioT->SetMinimum(10);
  m_glb_chargeT = QAH1F("QaGtrkChrgT",   "globtrk: charge, tpc ", 20,-2.,2.);
  m_glb_r0T     = QAH1F("QaGtrkR0T",     "globtrk: radius at start (cm), tpc ", 50,0.,200.);
    m_glb_r0T->SetMinimum(100);
  m_glb_phi0T   = QAH1F("QaGtrkPhi0T",   "globtrk: azimuth (phi) at start (deg,force 0,360), tpc ", 64,0.,360.);
  m_glb_z0T     = QAH1F("QaGtrkZ0T",     "globtrk: z-coord at start (cm), tpc ", 50, -300.,300.);
    m_glb_z0T->SetMinimum(500);
  m_glb_curvT   = QAH1F("QaGtrkCurvT",   "globtrk: log10 curvature (1/cm), tpc ", 80,-3.5,0.5);
  m_glb_rfT     = QAH1F("QaGtrkRfT",     "globtrk: radial position of first hit, tpc", 100,40.,200.);
  m_glb_padfT   = QAH1F("QaGtrkPadfT",   "globtrk: padrow of first hit on trk, tpc", 45,0.,45.);
  m_glb_f0      = QAMH1F("QaGtrkf0",     "globtrk: first point: hit-helix, tpc",60,-3.,3.,3,-0.5,2.5);
  m_glb_xfT     = QAH1F("QaGtrkXfT",     "globtrk: x of first hit on trk, tpc", 50,-200.,200.);
  m_glb_xf0     = QAH1F("QaGtrkXf0",     "globtrk: first point: x_hit - x_helix, tpc",60,-3.,3.);
  m_glb_yfT     = QAH1F("QaGtrkYfT",     "globtrk: y of first hit on trk, tpc", 50,-200.,200.);
  m_glb_yf0     = QAH1F("QaGtrkYf0",     "globtrk: first point: y_hit - y_helix, tpc",60,-3.,3.);
  m_glb_zfT     = QAH1F("QaGtrkZfT",     "globtrk: z of first hit on trk, tpc", 50,-300.,300.);
  m_glb_zf0     = QAH1F("QaGtrkZf0",     "globtrk: first point: z_hit - z_helix, tpc",60,-3.,3.);
  m_glb_radfT   = QAH1F("QaGtrkRT",      "globtrk: radial position of first hit, tpc", 50,0.,200.);
  m_lengthT     = QAH1F("QaGtrkLengthT", "globtrk: track length, tpc", 50,0.,300.);
  m_psiT        = QAH1F("QaGtrkPsiT",    "globtrk: psi, tpc (deg)", 64, 0.,360.);
  m_tanlT       = QAH1F("QaGtrkTanlT",   "globtrk: tanl, tpc",32,-4.,4.);
  m_glb_thetaT  = QAH1F("QaGtrkThetaT",  "globtrk: theta (degrees), tpc",36,0.,180.);
  m_etaT        = QAH1F("QaGtrkEtaT",    "globtrk: eta, tpc",40,-2.,2.);
  m_pTT         = QAH1F("QaGtrkPtT",     "globtrk: pT, tpc",50,0.,5.);
  m_momT        = QAH1F("QaGtrkPT",      "globtrk: momentum, tpc",50,0.,5.);
  m_chisq0T     = QAH1F("QaGtrkChisq0T", "globtrk: chisq0, tpc", 50, 0.,5.);
  m_chisq1T     = QAH1F("QaGtrkChisq1T", "globtrk: chisq1, tpc", 50, 0.,5.);
  m_glb_impactT = QAH1F("QaGtrkImpactT", "globtrk: log10 impact param from prim vtx, tpc",120,-3.0,3.0);
  m_glb_impactrT = QAH1F("QaGtrkImpactrT", "globtrk: impact param from prim vtx, tpc",100,0.,500.0);


// 2D - tpc

  m_pT_eta_recT = QAH2F("QaGtrkPtVsEtaT","globtrk: log pT vs eta, tpc", 20,-2.,2.,40,1.,4.);
    m_pT_eta_recT->SetXTitle("eta");
    m_pT_eta_recT->SetYTitle(" log pT (MeV)");

  m_globtrk_xf_yfT = QAH2F("QaGtrkXfYfT",  "globtrk: Y vs X of first hit on trk, tpc", 40,-200.,200.,40,-200.,200.);
    m_globtrk_xf_yfT->SetXTitle("x first");
    m_globtrk_xf_yfT->SetYTitle("y first");


  m_tanl_zfT = QAH2F("QaGtrkTanlzf","globtrk: tanl(dip) versus zfirst-zvtx, tpc",50,-250.,250.,60,-3.,3.);
    m_tanl_zfT->SetXTitle("zfirst-zvtx");
    m_tanl_zfT->SetYTitle("tanl");

  m_mom_trklengthT = QAH2F("QaGtrkPVsTrkLength","globtrk: log mom vs trk length, tpc",
			     50,0.,250.,40,1.,4.);
    m_mom_trklengthT->SetXTitle("trk length");  
    m_mom_trklengthT->SetYTitle("log P (MeV)");

  m_eta_trklengthT = QAH2F("QaGtrkLengthVEtaT","globtrk: trk length vs eta, tpc",
			     20,-2.,2.,50,0.,250.);
    m_eta_trklengthT->SetXTitle("eta");
    m_eta_trklengthT->SetYTitle("length");

  m_npoint_lengthT = QAH2F("QaGtrkNPntLengthT","globtrk: N pnts vs length, tpc",
			     25,0.,250.,25,0.,50.);
    m_npoint_lengthT->SetXTitle("trk length");
    m_npoint_lengthT->SetYTitle("Npoints on trk");

  m_fpoint_lengthT = QAH2F("QaGtrkFitPntLengthT","globtrk: N fit pnts vs length, tpc",
			     25,0.,250.,25,0.,50.);
    m_fpoint_lengthT->SetXTitle("trk length");
    m_fpoint_lengthT->SetYTitle("Npoints on trk");

  m_chisq0_momT = QAH2F("QaGtrkChi0MomT","globtrk: Chisq0 vs log mom, tpc",40,1.,4.,50,0.,5.);
    m_chisq0_momT->SetXTitle("log P (MeV)");
    m_chisq0_momT->SetYTitle("chisq0") ;

  m_chisq1_momT = QAH2F("QaGtrkChi1MomT","globtrk: Chisq1 vs log mom, tpc",40,1.,4.,50,0.,5.);
    m_chisq1_momT->SetXTitle("log P (MeV)");
    m_chisq1_momT->SetYTitle("chisq1");

  m_chisq0_etaT = QAH2F("QaGtrkChi0EtaT","globtrk: Chisq0 vs eta, tpc",20,-2.,2.,20,0.,5.);
    m_chisq0_etaT->SetXTitle("eta");
    m_chisq0_etaT->SetYTitle("chisq0");

  m_chisq1_etaT = QAH2F("QaGtrkChi1EtaT","globtrk: Chisq1 vs eta, tpc",20,-2.,2.,20,0.,5.);
    m_chisq1_etaT->SetXTitle("eta");
    m_chisq1_etaT->SetYTitle("chisq1");

  m_chisq0_dipT = QAH2F("QaGtrkChi0TanlT","globtrk: Chisq0 vs tanl(dip), tpc",20,-3.2,3.2,20,0.,5.);
    m_chisq0_dipT->SetXTitle("dip angle");
    m_chisq0_dipT->SetYTitle("chisq0");

  m_chisq1_dipT = QAH2F("QaGtrkChi1TanlT","globtrk: Chisq1 vs tanl(dip), tpc",20,-5.,5.,20,0.,5.);
    m_chisq1_dipT->SetXTitle("dip angle");
    m_chisq1_dipT->SetYTitle("chisq1");

  m_chisq0_zfT = QAH2F("QaGtrkChi0zfT","globtrk: Chisq0 vs zfirst, tpc",20,-250.,250.,20,0.,5.);
    m_chisq0_zfT->SetXTitle("zfirst");
    m_chisq0_zfT->SetYTitle("chisq0");

  m_chisq1_zfT = QAH2F("QaGtrkChi1zfT","globtrk: Chisq1 vs zfirst, tpc",20,-250.,250.,20,0.,5.);
    m_chisq1_zfT->SetXTitle("zfirst");
    m_chisq1_zfT->SetYTitle("chisq1");

  m_chisq0_phiT = QAH2F("QaGtrkChi0PhiT","globtrk: Chisq0 vs phi, tpc",20,0.,360,20,0.,5.);
    m_chisq0_etaT->SetXTitle("phi");
    m_chisq0_etaT->SetYTitle("chisq0");

  m_nfptonpt_momT = QAH2F("QaGtrkRPntMomT","globtrk: ratio Nfitpnt,Npnt vs log mom., tpc",40,1.,4.,50,0.,1.2005); 
     m_nfptonpt_momT->SetXTitle("log P (MeV)");
     m_nfptonpt_momT->SetYTitle("Ratio Nfitpnt/Npnt");

  m_nfptonpt_etaT = QAH2F("QaGtrkRPntEtaT","globtrk: ratio Nfitpnt,Npnt vs Eta, tpc",40,-2.,2.,50,0.,1.2005); 
     m_nfptonpt_etaT->SetXTitle("eta");
     m_nfptonpt_etaT->SetYTitle("Ratio Nfitpnt/Npnt");


  m_psi_phiT = QAH2F("QaGtrkPsiPhiT","globtrk: psi vs phi, tpc",36, 0.,360.,36,0.,360.);
     m_psi_phiT->SetXTitle("phi");
     m_psi_phiT->SetYTitle("psi");


// 1D tpc + silicon (svt+ssd)

  m_pointTS      = QAH1F("QaGtrkNPntTS",   "globtrk: N points on trk,tpc+svt", 60, 0.,60.);
  m_max_pointTS  = QAH1F("QaGtrkNPntMaxTS","globtrk: N max pnts on trk, tpc+svt", 50, 0.,100.);
  m_fit_pointTS  = QAH1F("QaGtrkNPntFitTS","globtrk: N fit pnts on trk, tpc+svt", 60, 0.,60.);
  m_glb_ratioTS  = QAH1F("QaGtrkRnfTS",    "globtrk: ratio Nfit/tot pnt, tpc+svt", 55, 0., 1.1);
    m_glb_ratioTS->SetMinimum(10);
  m_glb_ratiomTS = QAH1F("QaGtrkRnmTS",    "globtrk: ratio Nfit/max pnt, tpc+svt", 55, 0., 1.1);
  m_glb_chargeTS = QAH1F("QaGtrkChrgTS",   "globtrk: charge, tpc+svt ", 20,-2.,2.);
  m_glb_r0TS     = QAH1F("QaGtrkR0TS",     "globtrk: radius at start (cm), tpc+svt", 50,0.,200.);
    m_glb_r0TS->SetMinimum(100);
  m_glb_phi0TS   = QAH1F("QaGtrkPhi0TS",   "globtrk: azimuth (phi) at start (deg,force 0-360),tpc+svt", 64, 0.,360.);
  m_glb_z0TS     = QAH1F("QaGtrkZ0TS",     "globtrk: z-coord at start (cm), tpc+svt", 50, -300.,300.);
    m_glb_z0TS->SetMinimum(500);
  m_glb_rfTS     = QAH1F("QaGtrkRfTS",     "globtrk: radial position of first hit, tpc+svt", 100,40,200.);
  m_glb_curvTS   = QAH1F("QaGtrkCurvTS",   "globtrk: log10 curvature (1/cm), tpc+svt", 80,-3.5,0.5);
  m_glb_f0TS     = QAMH1F("QaGtrkf0TS",    "globtrk: first point: hit-helix, tpc+svt",50,-5.,5.,3,-0.5,2.5);
  m_glb_xfTS     = QAH1F("QaGtrkXfTS",     "globtrk: x of first hit on trk, tpc+svt", 50,-200.,200.);
  m_glb_xf0TS    = QAH1F("QaGtrkXf0TS",    "globtrk: x of first hit - on helix at start, tpc+svt",50,-5.,5.);
  m_glb_yfTS     = QAH1F("QaGtrkYfTS",     "globtrk: y of first hit on trk, tpc+svt", 50,-200.,200.);
  m_glb_yf0TS    = QAH1F("QaGtrkYf0TS",    "globtrk: y of first hit - on helix at start, tpc+svt",50,-5.,5.);
  m_glb_zfTS     = QAH1F("QaGtrkZfTS",     "globtrk: z of first hit on trk, tpc+svt", 50,-300.,300.);
  m_glb_zf0TS    = QAH1F("QaGtrkZf0TS",    "globtrk: z of first hit - on helix at start, tpc+svt",50,-5.,5.);
  m_glb_radfTS   = QAH1F("QaGtrkRTS",      "globtrk: radial position of first hit, tpc+svt", 50,0.,200.);
  m_lengthTS     = QAH1F("QaGtrkLengthTS", "globtrk: track length, tpc+svt", 50,0.,300.);
  m_psiTS        = QAH1F("QaGtrkPsiTS",    "globtrk: psi, tpc+svt (deg) ", 64, 0.,360.);
  m_tanlTS       = QAH1F("QaGtrkTanlTS",   "globtrk: tanl, tpc+svt",32,-4.,4.);
  m_glb_thetaTS  = QAH1F("QaGtrkThetaTS",  "globtrk: theta (degrees), tpc+svt",36,0.,180.);
  m_etaTS        = QAH1F("QaGtrkEtaTS",    "globtrk: eta, tpc+svt",40,-2.,2.);
  m_pTTS         = QAH1F("QaGtrkPtTS",     "globtrk: pT, tpc+svt",50,0.,5.);
  m_momTS        = QAH1F("QaGtrkPTS",      "globtrk: momentum, tpc+svt",50,0.,5.);
  m_chisq0TS     = QAH1F("QaGtrkChisq0TS", "globtrk: chisq0, tpc+svt", 50, 0.,5.);
  m_chisq1TS     = QAH1F("QaGtrkChisq1TS", "globtrk: chisq1, tpc+svt", 50, 0.,5.);
  m_glb_impactTS = QAH1F("QaGtrkImpactTS", "globtrk: log10 impact param from prim vtx, tpc+svt",
                            120,-3.0,3.0);
  m_glb_impactrTS = QAH1F("QaGtrkImpactrTS", "globtrk: impact param from prim vtx, tpc+svt",
                            100,0.,50.);


// 2D - tpc + silicon (svt + ssd)

  m_pT_eta_recTS = QAH2F("QaGtrkPtVsEtaTS","globtrk: log pT vs eta, tpc+svt", 20,-2.,2.,40,1.,4.);
    m_pT_eta_recTS->SetXTitle("eta");
    m_pT_eta_recTS->SetYTitle(" log pT (MeV)");

  m_globtrk_xf_yfTS = QAH2F("QaGtrkXfYfTS",  "globtrk: Y vs X of first hit on trk, tpc+svt", 40,-200.,200.,40,-200.,200.);
    m_globtrk_xf_yfTS->SetXTitle("x first");
    m_globtrk_xf_yfTS->SetYTitle("y first");

  m_tanl_zfTS = QAH2F("QaGtrkTanlzfTS","globtrk: tanl(dip) versus zfirst-zvtx, tpc+svt",50,-250.,250.,60,-3.,3.);
    m_tanl_zfTS->SetXTitle("zfirst-zvtx");
    m_tanl_zfTS->SetYTitle("tanl");

  m_mom_trklengthTS = QAH2F("QaGtrkPVsTrkLTS","globtrk: log mom vs trk length, tpc+svt",50,0.,250.,40,1.,4.);
    m_mom_trklengthTS->SetXTitle("trk length");  
    m_mom_trklengthTS->SetYTitle("log P (MeV)");

  m_eta_trklengthTS = QAH2F("QaGtrkLVEtaTS","globtrk: trk length vs eta, tpc+svt",20,-2.,2.,50,0.,250.);
    m_eta_trklengthTS->SetXTitle("eta");
    m_eta_trklengthTS->SetYTitle("length");

  m_npoint_lengthTS = QAH2F("QaGtrkNPntLTS","globtrk: N pnts vs length, tpc+svt",25,0.,250.,25,0.,50.);
    m_npoint_lengthTS->SetXTitle("trk length");
    m_npoint_lengthTS->SetYTitle("Npoints on trk");

  m_fpoint_lengthTS = QAH2F("QaGtrkFitPntLTS","globtrk: N fit pnts vs length, tpc+svt",25,0.,250.,25,0.,50.);
    m_fpoint_lengthTS->SetXTitle("trk length");
    m_fpoint_lengthTS->SetYTitle("Npoints on trk");

  m_chisq0_momTS = QAH2F("QaGtrkChi0MomTS","globtrk: Chisq0 vs log mom, tpc+svt",40,1.,4.,50,0.,5.);
    m_chisq0_momTS->SetXTitle("log P (MeV)");
    m_chisq0_momTS->SetYTitle("chisq0") ;

  m_chisq1_momTS = QAH2F("QaGtrkChi1MomTS","globtrk: Chisq1 vs log mom, tpc+svt",40,1.,4.,50,0.,5.);
    m_chisq1_momTS->SetXTitle("log P (MeV)");
    m_chisq1_momTS->SetYTitle("chisq1");

  m_chisq0_etaTS = QAH2F("QaGtrkChi0EtaTS","globtrk: Chisq0 vs eta, tpc+svt",20,-2.,2.,20,0.,5.);
    m_chisq0_etaTS->SetXTitle("eta");
    m_chisq0_etaTS->SetYTitle("chisq0");

  m_chisq1_etaTS = QAH2F("QaGtrkChi1EtaTS","globtrk: Chisq1 vs eta, tpc+svt",20,-2.,2.,20,0.,5.);
    m_chisq1_etaTS->SetXTitle("eta");
    m_chisq1_etaTS->SetYTitle("chisq1");

  m_chisq0_dipTS = QAH2F("QaGtrkChi0TanlTS","globtrk: Chisq0 vs tanl(dip), tpc+svt",20,-3.2,3.2,20,0.,5.);
    m_chisq0_dipTS->SetXTitle("dip angle");
    m_chisq0_dipTS->SetYTitle("chisq0");

  m_chisq1_dipTS = QAH2F("QaGtrkChi1TanlTS","globtrk: Chisq1 vs tanl(dip), tpc+svt",20,-5.,5.,20,0.,5.);
    m_chisq1_dipTS->SetXTitle("dip angle");
    m_chisq1_dipTS->SetYTitle("chisq1");

  m_chisq0_zfTS = QAH2F("QaGtrkChi0zfTS","globtrk: Chisq0 vs zfirst, tpc+svt",20,-250.,250.,20,0.,5.);
    m_chisq0_zfTS->SetXTitle("zfirst");
    m_chisq0_zfTS->SetYTitle("chisq0");

  m_chisq1_zfTS = QAH2F("QaGtrkChi1zfTS","globtrk: Chisq1 vs zfirst, tpc+svt",20,-250.,250.,20,0.,5.);
    m_chisq1_zfTS->SetXTitle("zfirst");
    m_chisq1_zfTS->SetYTitle("chisq1");

  m_chisq0_phiTS = QAH2F("QaGtrkChi0PhiTS","globtrk: Chisq0 vs phi, tpc+svt",20,0.,360,20,0.,5.);
    m_chisq0_etaTS->SetXTitle("phi");
    m_chisq0_etaTS->SetYTitle("chisq0");

  m_nfptonpt_momTS = QAH2F("QaGtrkRPntMomTS","globtrk: ratio Nfitpnt,Npnt vs log mom., tpc+svt",40,1.,4.,50,0.,1.2005); 
     m_nfptonpt_momTS->SetXTitle("log P (MeV)");
     m_nfptonpt_momTS->SetYTitle("Ratio Nfitpnt/Npnt");

  m_nfptonpt_etaTS = QAH2F("QaGtrkRPntEtaTS","globtrk: ratio Nfitpnt,Npnt vs Eta, tpc+svt",40,-2.,2.,50,0.,1.2005); 
     m_nfptonpt_etaTS->SetXTitle("eta");
     m_nfptonpt_etaTS->SetYTitle("Ratio Nfitpnt/Npnt");

  m_psi_phiTS = QAH2F("QaGtrkPsiPhiTS","globtrk: psi vs phi, tpc+svt",36, 0.,360.,36,0.,360.);
     m_psi_phiTS->SetXTitle("phi");
     m_psi_phiTS->SetYTitle("psi");


// 1D ftpc

  m_pointFE      = QAH1F("QaGtrkNPntFE",    "globtrk: N points on trk,ftpc east", 15, 0.,15.);
  m_pointFW      = QAH1F("QaGtrkNPntFW",    "globtrk: N points on trk,ftpc west", 15, 0.,15.);
  m_max_pointFE  = QAH1F("QaGtrkNPntMaxFE", "globtrk: N max pnts on trk, ftpc east", 15, 0.,15.);
  m_max_pointFW  = QAH1F("QaGtrkNPntMaxFW", "globtrk: N max pnts on trk, ftpc west", 15, 0.,15.);
  m_fit_pointFE  = QAH1F("QaGtrkNPntFitFE", "globtrk: N fit pnts on trk, ftpc east", 15, 0.,15.);
  m_fit_pointFW  = QAH1F("QaGtrkNPntFitFW", "globtrk: N fit pnts on trk, ftpc west", 15, 0.,15.);
  m_glb_ratioFE  = QAH1F("QaGtrkRnfFE",     "globtrk: ratio Nfit/tot pnt, ftpc east", 55, 0., 1.1);
  m_glb_ratioFW  = QAH1F("QaGtrkRnfFW",     "globtrk: ratio Nfit/tot pnt, ftpc west", 55, 0., 1.1);
  m_glb_ratiomFE = QAH1F("QaGtrkRnmFE",     "globtrk: ratio Nfit/max pnt, ftpc east", 55, 0., 1.1);
  m_glb_ratiomFW = QAH1F("QaGtrkRnmFW",     "globtrk: ratio Nfit/max pnt, ftpc west", 55, 0., 1.1);
  m_glb_chargeFE = QAH1F("QaGtrkChrgFE",    "globtrk: charge, ftpc east ", 20,-2.,2.);
  m_glb_chargeFW = QAH1F("QaGtrkChrgFW",    "globtrk: charge, ftpc west ", 20,-2.,2.);
  m_glb_rfFE     = QAH1F("QaGtrkRfFE",      "globtrk: radial position of first hit, ftpc east", 100,40,200.);
  m_glb_rfFW     = QAH1F("QaGtrkRfFW",      "globtrk: radial position of first hit, ftpc west", 100,40,200.);
  m_glb_xfFE     = QAH1F("QaGtrkXfFE",      "globtrk: x of first hit on trk, ftpc east", 50,-200.,200.);
  m_glb_xfFW     = QAH1F("QaGtrkXfFW",      "globtrk: x of first hit on trk, ftpc west", 50,-200.,200.);
  m_glb_yfFE     = QAH1F("QaGtrkYfFE",      "globtrk: y of first hit on trk, ftpc east", 50,-200.,200.);
  m_glb_yfFW     = QAH1F("QaGtrkYfFW",      "globtrk: y of first hit on trk, ftpc west", 50,-200.,200.);
  m_glb_zfFE     = QAH1F("QaGtrkZfFE",      "globtrk: z of first hit on trk, ftpc east", 50,-300.,300.);
  m_glb_zfFW     = QAH1F("QaGtrkZfFW",      "globtrk: z of first hit on trk, ftpc west", 50,-300.,300.);
  m_glb_radfFE   = QAH1F("QaGtrkRFE",       "globtrk: radial position of first hit, ftpc east", 40,0.,40.);
  m_glb_radfFW   = QAH1F("QaGtrkRFW",       "globtrk: radial position of first hit, ftpc west", 40,0.,40.);
  m_lengthFE     = QAH1F("QaGtrkLengthFE",  "globtrk: track length, ftpc east", 60,0.,120.);
  m_lengthFW     = QAH1F("QaGtrkLengthFW",  "globtrk: track length, ftpc west", 60,0.,120.);
  m_psiFE        = QAH1F("QaGtrkPsiFE",     "globtrk: psi, ftpc east", 36, 0.,360.);
  m_psiFW        = QAH1F("QaGtrkPsiFW",     "globtrk: psi, ftpc west", 36, 0.,360.);
  m_etaFE        = QAH1F("QaGtrkEtaFE",     "globtrk: eta, ftpc east",80,-8.,8.);
  m_etaFW        = QAH1F("QaGtrkEtaFW",     "globtrk: eta, ftpc west",80,-8.,8.);
  m_pTFE         = QAH1F("QaGtrkPtFE",      "globtrk: pT, ftpc east",50,0.,5.);
  m_pTFW         = QAH1F("QaGtrkPtFW",      "globtrk: pT, ftpc west",50,0.,5.);
  m_momFE        = QAH1F("QaGtrkPFE",       "globtrk: momentum, ftpc east ",50,0.,5.);
  m_momFW        = QAH1F("QaGtrkPFW",       "globtrk: momentum, ftpc west ",50,0.,5.);
  m_chisq0FE     = QAH1F("QaGtrkChisq0FE",  "globtrk: chisq0, ftpc east", 50, 0.,5000.);
  m_chisq0FW     = QAH1F("QaGtrkChisq0FW",  "globtrk: chisq0, ftpc west", 50, 0.,5000.);
  m_chisq1FE     = QAH1F("QaGtrkChisq1FE",  "globtrk: chisq1, ftpc east", 50, 0.,500.);
  m_chisq1FW     = QAH1F("QaGtrkChisq1FW",  "globtrk: chisq1, ftpc west", 50, 0.,500.);

// 2D - ftpc

   m_pT_eta_recFE = QAH2F("QaGtrkPtVsEtaFE","globtrk: log pT vs eta, ftpcE",20,-5.,0.,40,1.,4.);
   m_pT_eta_recFW = QAH2F("QaGtrkPtVsEtaFW","globtrk: log pT vs eta, ftpcW",20,0.,5.,40,1.,4.);
  m_globtrk_xf_yfFE = QAH2F("QaGtrkXfYfFE","globtrk: Y vs X of first hit on trk, ftpcE", 40,-40.,40.,40,-40.,40.);
    m_globtrk_xf_yfFE->SetXTitle("x first");
    m_globtrk_xf_yfFE->SetYTitle("y first");
  m_globtrk_xf_yfFW = QAH2F("QaGtrkXfYfFW","globtrk: Y vs X of first hit on trk, ftpcW", 40,-40.,40.,40,-40.,40.);
    m_globtrk_xf_yfFW->SetXTitle("x first");
    m_globtrk_xf_yfFW->SetYTitle("y first");
  m_eta_trklengthFE = QAH2F("QaGtrkLengthVEtaFE","globtrk: trk length vs eta, ftpcE",25,-4.5,-2.,30,0.,120.);
    m_eta_trklengthFE->SetXTitle("eta");
    m_eta_trklengthFE->SetYTitle("length");
  m_eta_trklengthFW = QAH2F("QaGtrkLengthVEtaFW","globtrk: trk length vs eta, ftpcW",25,2.,4.5,30,0.,120.);
    m_eta_trklengthFW->SetXTitle("eta");
    m_eta_trklengthFW->SetYTitle("length");
  m_npoint_lengthFE = QAH2F("QaGtrkNPntLengthFE","globtrk: N pnts vs length, ftpcE",30,0.,120.,15,0.,15.);
    m_npoint_lengthFE->SetXTitle("trk length");
    m_npoint_lengthFE->SetYTitle("Npoints on trk");
  m_npoint_lengthFW = QAH2F("QaGtrkNPntLengthFW","globtrk: N pnts vs length, ftpcW",30,0.,120.,15,0.,15.);
    m_npoint_lengthFW->SetXTitle("trk length");
    m_npoint_lengthFW->SetYTitle("Npoints on trk");
  m_fpoint_lengthFE = QAH2F("QaGtrkFitPntLengthFE","globtrk: N fit pnts vs length, ftpcE",30,0.,120.,15,0.,15.);
    m_fpoint_lengthFE->SetXTitle("trk length");
    m_fpoint_lengthFE->SetYTitle("Npoints on trk");
  m_fpoint_lengthFW = QAH2F("QaGtrkFitPntLengthFW","globtrk: N fit pnts vs length, ftpcW",30,0.,120.,15,0.,15.);
    m_fpoint_lengthFW->SetXTitle("trk length");
    m_fpoint_lengthFW->SetYTitle("Npoints on trk");
}
//____________________________________________________
void StQABookHist::BookHistPrim(){

// for method MakeHistPrim - from table primtrk

// 1D
  m_primtrk_tot     = QAH1F("QaPtrkTot",   "primtrk: tot num tracks",100,0.,5000.);
  m_primtrk_tot_sm  = QAH1F("QaPtrkTotsm", "primtrk: tot num tracks",100,0.,20.);
  m_primtrk_iflag   = QAH1F("QaPtrkFlag",  "primtrk: iflag - all",160,-799.,801.);
  m_primtrk_good    = QAH1F("QaPtrkGood",  "primtrk: tot num tracks iflag>0",50,0.,2500.);
  m_primtrk_good_sm = QAH1F("QaPtrkGoodsm","primtrk: tot num tracks iflag>0",50,0.,20.);
  m_pdet_id     = QAH1F("QaPtrkDetId",   "primtrk: Detector ID good tracks - all",25,0.,25.);

// 1D tpc
  m_ppointT      = QAH1F("QaPtrkNPntT",   "primtrk: N points on trk,tpc", 60, 0.,60.);
  m_pmax_pointT  = QAH1F("QaPtrkNPntMaxT","primtrk: N max pnts on trk, tpc", 50, 0.,100.);
  m_pfit_pointT  = QAH1F("QaPtrkNPntFitT","primtrk: N fit pnts on trk, tpc", 60, 0.,60.);
  m_prim_ratioT  = QAH1F("QaPtrkRnfT",    "primtrk: ratio Nfit/tot pnt, tpc", 55, 0., 1.1);
  m_prim_ratiomT = QAH1F("QaPtrkRnmT",    "primtrk: ratio Nfit/max pnt, tpc", 55, 0., 1.1);
  m_prim_chargeT = QAH1F("QaPtrkChrgT",   "primtrk: charge, tpc ", 20,-2.,2.);
  m_prim_r0T     = QAH1F("QaPtrkR0T",     "primtrk: radius at start (cm), tpc ", 50,0.,200.);
  m_prim_phi0T   = QAH1F("QaPtrkPhi0T",   "primtrk: azimuth (phi) at start (deg,force 0,360), tpc ", 64,0.,360.);
  m_prim_z0T     = QAH1F("QaPtrkZ0T",     "primtrk: z-coord at start (cm), tpc ", 50, -300.,300.);
  m_prim_curvT   = QAH1F("QaPtrkCurvT",   "primtrk: log10 curvature (1/cm), tpc ", 80,-3.5,0.5);
  m_prim_f0      = QAMH1F("QaPtrkf0",     "primtrk: first point: hit-helix, tpc",60,-3.,3.,3,-0.5,2.5);
  m_prim_xfT     = QAH1F("QaPtrkXfT",     "primtrk: x of first hit on trk, tpc", 50,-200.,200.);
  m_prim_xf0     = QAH1F("QaPtrkXf0",     "primtrk: first point: x_hit - x_helix, tpc",60,-3.,3.);
  m_prim_yfT     = QAH1F("QaPtrkYfT",     "primtrk: y of first hit on trk, tpc", 50,-200.,200.);
  m_prim_yf0     = QAH1F("QaPtrkYf0",     "primtrk: first point: y_hit - y_helix, tpc",60,-3.,3.);
  m_prim_zfT     = QAH1F("QaPtrkZfT",     "primtrk: z of first hit on trk, tpc", 50,-200.,200.);
  m_prim_zf0     = QAH1F("QaPtrkZf0",     "primtrk: first point: z_hit - z_helix, tpc",60,-3.,3.);
  m_prim_radfT   = QAH1F("QaPtrkRT",      "primtrk: radial position of first hit, tpc", 50,0.,200.);
  m_plengthT     = QAH1F("QaPtrkLengthT", "primtrk: track length, tpc", 50,0.,300.);
  m_ppsiT        = QAH1F("QaPtrkPsiT",    "primtrk: psi, tpc (deg)", 36, 0.,360.);
  m_ptanlT       = QAH1F("QaPtrkTanlT",   "primtrk: tanl, tpc",32,-4.,4.);
  m_prim_thetaT  = QAH1F("QaPtrkThetaT",  "primtrk: theta (degrees), tpc",36,0.,180.);
  m_petaT        = QAH1F("QaPtrkEtaT",    "primtrk: eta, tpc",40,-2.,2.);
  m_ppTT         = QAH1F("QaPtrkPtT",     "primtrk: pT, tpc",50,0.,5.);
  m_pmomT        = QAH1F("QaPtrkPT",      "primtrk: momentum, tpc",50,0.,5.);
  m_pchisq0T     = QAH1F("QaPtrkChisq0T", "primtrk: chisq0, tpc", 50, 0.,5.);
  m_pchisq1T     = QAH1F("QaPtrkChisq1T", "primtrk: chisq1, tpc", 50, 0.,5.);
  m_prim_impactT = QAH1F("QaPtrkImpactT", "primtrk: log10 impact param from prim vtx, tpc",120,-3.0,3.0);
  m_prim_impactrT = QAH1F("QaPtrkImpactrT", "primtrk: impact param from prim vtx, tpc",100,0.,10.);

// 2D - tpc
  m_ppT_eta_recT = QAH2F("QaPtrkPtVsEtaT","primtrk: log pT vs eta, tpc", 20,-2.,2.,40,1.,4.);
    m_ppT_eta_recT->SetXTitle("eta");
    m_ppT_eta_recT->SetYTitle(" log pT (MeV)");
  m_primtrk_xf_yfT = QAH2F("QaPtrkXfYfT",  "primtrk: Y vs X of first hit on trk, tpc", 40,-200.,200.,40,-200.,200.);
    m_primtrk_xf_yfT->SetXTitle("x first");
    m_primtrk_xf_yfT->SetYTitle("y first");
  m_ptanl_zfT = QAH2F("QaPtrkTanlzf","primtrk: tanl(dip) versus zfirst-zvtx, tpc",50,-250.,250.,60,-3.,3.);
    m_ptanl_zfT->SetXTitle("zfirst-zvtx");
    m_ptanl_zfT->SetYTitle("tanl");
  m_pmom_trklengthT = QAH2F("QaPtrkPVsTrkLength","primtrk: log mom vs trk length, tpc",50,70.,350.,40,1.,4.);
    m_pmom_trklengthT->SetXTitle("trk length");  
    m_pmom_trklengthT->SetYTitle("log P (MeV)");
  m_peta_trklengthT = QAH2F("QaPtrkLengthVEtaT","primtrk: trk length vs eta, tpc",20,-2.,2.,50,70.,350.);
    m_peta_trklengthT->SetXTitle("eta");
    m_peta_trklengthT->SetYTitle("length");
  m_pnpoint_lengthT = QAH2F("QaPtrkNPntLengthT","primtrk: N pnts vs length, tpc",25,70.,350.,25,0.,50.);
    m_pnpoint_lengthT->SetXTitle("trk length");
    m_pnpoint_lengthT->SetYTitle("Npoints on trk");
  m_pfpoint_lengthT = QAH2F("QaPtrkFitPntLengthT","primtrk: N fit pnts vs length, tpc",25,70.,350.,25,0.,50.);
    m_pfpoint_lengthT->SetXTitle("trk length");
    m_pfpoint_lengthT->SetYTitle("Npoints on trk");
  m_pchisq0_momT = QAH2F("QaPtrkChi0MomT","primtrk: Chisq0 vs log mom, tpc",40,1.,4.,50,0.,5.);
    m_pchisq0_momT->SetXTitle("log P (MeV)");
    m_pchisq0_momT->SetYTitle("chisq0") ;
  m_pchisq1_momT = QAH2F("QaPtrkChi1MomT","primtrk: Chisq1 vs log mom, tpc",40,1.,4.,50,0.,5.);
    m_pchisq1_momT->SetXTitle("log P (MeV)");
    m_pchisq1_momT->SetYTitle("chisq1");
  m_pchisq0_etaT = QAH2F("QaPtrkChi0EtaT","primtrk: Chisq0 vs eta, tpc",20,-2.,2.,20,0.,5.);
    m_pchisq0_etaT->SetXTitle("eta");
    m_pchisq0_etaT->SetYTitle("chisq0");
  m_pchisq1_etaT = QAH2F("QaPtrkChi1EtaT","primtrk: Chisq1 vs eta, tpc",20,-2.,2.,20,0.,5.);
    m_pchisq1_etaT->SetXTitle("eta");
    m_pchisq1_etaT->SetYTitle("chisq1");
  m_pchisq0_dipT = QAH2F("QaPtrkChi0TanlT","primtrk: Chisq0 vs tanl(dip), tpc",20,-3.2,3.2,20,0.,5.);
    m_pchisq0_dipT->SetXTitle("dip angle");
    m_pchisq0_dipT->SetYTitle("chisq0");
  m_pchisq1_dipT = QAH2F("QaPtrkChi1TanlT","primtrk: Chisq1 vs tanl(dip), tpc",20,-3.2,3.2,20,0.,5.);
    m_pchisq1_dipT->SetXTitle("dip angle");
    m_pchisq1_dipT->SetYTitle("chisq1");
  m_pchisq0_zfT = QAH2F("QaPtrkChi0zfT","primtrk: Chisq0 vs zfirst, tpc",20,-200.,200.,20,0.,5.);
    m_pchisq0_zfT->SetXTitle("zfirst");
    m_pchisq0_zfT->SetYTitle("chisq0");
  m_pchisq1_zfT = QAH2F("QaPtrkChi1zfT","primtrk: Chisq1 vs zfirst, tpc",20,-200.,200.,20,0.,5.);
    m_pchisq1_zfT->SetXTitle("zfirst");
    m_pchisq1_zfT->SetYTitle("chisq1");
  m_pnfptonpt_momT = QAH2F("QaPtrkRPntMomT","primtrk: ratio Nfitpnt,Npnt vs log mom., tpc",40,1.,4.,50,0.,1.2005); 
     m_pnfptonpt_momT->SetXTitle("log P (MeV)");
     m_pnfptonpt_momT->SetYTitle("Ratio Nfitpnt/Npnt");
  m_pnfptonpt_etaT = QAH2F("QaPtrkRPntEtaT","primtrk: ratio Nfitpnt,Npnt vs Eta, tpc",40,-2.,2.,50,0.,1.2005); 
     m_pnfptonpt_etaT->SetXTitle("eta");
     m_pnfptonpt_etaT->SetYTitle("Ratio Nfitpnt/Npnt");
  m_ppsi_phiT = QAH2F("QaPtrkPsiPhiT","primtrk: psi vs phi, tpc",36, 0.,360.,36,0.,360.);
     m_ppsi_phiT->SetXTitle("phi");
     m_ppsi_phiT->SetYTitle("psi");

// 1D tpc + silicon (svt+ssd)
  m_ppointTS      = QAH1F("QaPtrkNPntTS",   "primtrk: N points on trk,tpc+svt", 60, 0.,60.);
  m_pmax_pointTS  = QAH1F("QaPtrkNPntMaxTS","primtrk: N max pnts on trk, tpc+svt", 50, 0.,100.);
  m_pfit_pointTS  = QAH1F("QaPtrkNPntFitTS","primtrk: N fit pnts on trk, tpc+svt", 60, 0.,60.);
  m_prim_ratioTS  = QAH1F("QaPtrkRnfTS",    "primtrk: ratio Nfit/tot pnt, tpc+svt", 55, 0., 1.2005);
  m_prim_ratiomTS = QAH1F("QaPtrkRnmTS",    "primtrk: ratio Nfit/max pnt, tpc+svt", 55, 0., 1.2005);
  m_prim_chargeTS = QAH1F("QaPtrkChrgTS",   "primtrk: charge, tpc+svt ", 20,-2.,2.);
  m_prim_r0TS     = QAH1F("QaPtrkR0TS",     "primtrk: radius at start (cm), tpc+svt", 50,0.,200.);
  m_prim_phi0TS   = QAH1F("QaPtrkPhi0TS",   "primtrk: azimuth (phi) at start (deg,force 0-360),tpc+svt", 64, 0.,360.);
  m_prim_z0TS     = QAH1F("QaPtrkZ0TS",     "primtrk: z-coord at start (cm), tpc+svt", 50, -300.,300.);
  m_prim_curvTS   = QAH1F("QaPtrkCurvTS",   "primtrk: log10 curvature (1/cm), tpc+svt", 80,-3.5,0.5);
  m_prim_f0TS     = QAMH1F("QaPtrkf0TS",    "primtrk: first point: hit-helix, tpc+svt",50,-5.,5.,3,-0.5,2.5);
  m_prim_xfTS     = QAH1F("QaPtrkXfTS",     "primtrk: x of first hit on trk, tpc+svt", 50,-200.,200.);
  m_prim_xf0TS    = QAH1F("QaPtrkXf0TS",    "primtrk: first point: y_hit - y_helix, tpc+svt",50,-5.,5.);
  m_prim_yfTS     = QAH1F("QaPtrkYfTS",     "primtrk: y of first hit on trk, tpc+svt", 50,-200.,200.);
  m_prim_yf0TS    = QAH1F("QaPtrkYf0TS",    "primtrk: first point: y_hit - y_helix, tpc+svt",50,-5.,5.);
  m_prim_zfTS     = QAH1F("QaPtrkZfTS",     "primtrk: z of first hit on trk, tpc+svt", 50,-200.,200.);
  m_prim_zf0TS    = QAH1F("QaPtrkZf0TS",    "primtrk: first point: z_hit - z_helix, tpc+svt",50,-5.,5.);
  m_prim_radfTS   = QAH1F("QaPtrkRTS",      "primtrk: radial position of first hit, tpc+svt", 50,0.,200.);
  m_plengthTS     = QAH1F("QaPtrkLengthTS", "primtrk: track length, tpc+svt", 50,0.,300.);
  m_ppsiTS        = QAH1F("QaPtrkPsiTS",    "primtrk: psi, tpc+svt (deg) ", 36, 0.,360.);
  m_ptanlTS       = QAH1F("QaPtrkTanlTS",   "primtrk: tanl, tpc+svt",32,-4.,4.);
  m_prim_thetaTS  = QAH1F("QaPtrkThetaTS",  "primtrk: theta (degrees), tpc+svt",36,0.,180.);
  m_petaTS        = QAH1F("QaPtrkEtaTS",    "primtrk: eta, tpc+svt",40,-2.,2.);
  m_ppTTS         = QAH1F("QaPtrkPtTS",     "primtrk: pT, tpc+svt",50,0.,5.);
  m_pmomTS        = QAH1F("QaPtrkPTS",      "primtrk: momentum, tpc+svt",50,0.,5.);
  m_pchisq0TS     = QAH1F("QaPtrkChisq0TS", "primtrk: chisq0, tpc+svt", 50, 0.,5.);
  m_pchisq1TS     = QAH1F("QaPtrkChisq1TS", "primtrk: chisq1, tpc+svt", 50, 0.,5.);
  m_prim_impactTS = QAH1F("QaPtrkImpactTS", "primtrk: log10 impact param from prim vtx, tpc+svt",120,-3.0,3.0);
  m_prim_impactrTS = QAH1F("QaPtrkImpactrTS", "primtrk: impact param from prim vtx, tpc+svt",100,0.,10.);

// 2D - tpc + silicon (svt + ssd)
  m_ppT_eta_recTS = QAH2F("QaPtrkPtVsEtaTS","primtrk: log pT vs eta, tpc+svt", 20,-2.,2.,40,1.,4.);
    m_ppT_eta_recTS->SetXTitle("eta");
    m_ppT_eta_recTS->SetYTitle(" log pT (MeV)");
  m_primtrk_xf_yfTS = QAH2F("QaPtrkXfYfTS",  "primtrk: Y vs X of first hit on trk, tpc+svt", 40,-200.,200.,40,-200.,200.);
    m_primtrk_xf_yfTS->SetXTitle("x first");
    m_primtrk_xf_yfTS->SetYTitle("y first");
  m_ptanl_zfTS = QAH2F("QaPtrkTanlzfTS","primtrk: tanl(dip) versus zfirst-zvtx, tpc+svt",50,-250.,250.,60,-3.,3.);
    m_ptanl_zfTS->SetXTitle("zfirst-zvtx");
    m_ptanl_zfTS->SetYTitle("tanl");
  m_pmom_trklengthTS = QAH2F("QaPtrkPVsTrkLTS","primtrk: log mom vs trk length, tpc+svt",50,70.,350.,40,1.,4.);
    m_pmom_trklengthTS->SetXTitle("trk length");  
    m_pmom_trklengthTS->SetYTitle("log P (MeV)");
  m_peta_trklengthTS = QAH2F("QaPtrkLVEtaTS","primtrk: trk length vs eta, tpc+svt",20,-2.,2.,50,70.,350.);
    m_peta_trklengthTS->SetXTitle("eta");
    m_peta_trklengthTS->SetYTitle("length");
  m_pnpoint_lengthTS = QAH2F("QaPtrkNPntLTS","primtrk: N pnts vs length, tpc+svt",25,70.,350.,25,0.,50.);
    m_pnpoint_lengthTS->SetXTitle("trk length");
    m_pnpoint_lengthTS->SetYTitle("Npoints on trk");
  m_pfpoint_lengthTS = QAH2F("QaPtrkFitPntLTS","primtrk: N fit pnts vs length, tpc+svt",25,70.,350.,25,0.,50.);
    m_pfpoint_lengthTS->SetXTitle("trk length");
    m_pfpoint_lengthTS->SetYTitle("Npoints on trk");
  m_pchisq0_momTS = QAH2F("QaPtrkChi0MomTS","primtrk: Chisq0 vs log mom, tpc+svt",40,1.,4.,50,0.,5.);
    m_pchisq0_momTS->SetXTitle("log P (MeV)");
    m_pchisq0_momTS->SetYTitle("chisq0") ;
  m_pchisq1_momTS = QAH2F("QaPtrkChi1MomTS","primtrk: Chisq1 vs log mom, tpc+svt",40,1.,4.,50,0.,5.);
    m_pchisq1_momTS->SetXTitle("log P (MeV)");
    m_pchisq1_momTS->SetYTitle("chisq1");
  m_pchisq0_etaTS = QAH2F("QaPtrkChi0EtaTS","primtrk: Chisq0 vs eta, tpc+svt",20,-2.,2.,20,0.,5.);
    m_pchisq0_etaTS->SetXTitle("eta");
    m_pchisq0_etaTS->SetYTitle("chisq0");
  m_pchisq1_etaTS = QAH2F("QaPtrkChi1EtaTS","primtrk: Chisq1 vs eta, tpc+svt",20,-2.,2.,20,0.,5.);
    m_pchisq1_etaTS->SetXTitle("eta");
    m_pchisq1_etaTS->SetYTitle("chisq1");
  m_pchisq0_dipTS = QAH2F("QaPtrkChi0TanlTS","primtrk: Chisq0 vs tanl(dip), tpc+svt",20,-3.2,3.2,20,0.,5.);
    m_pchisq0_dipTS->SetXTitle("dip angle");
    m_pchisq0_dipTS->SetYTitle("chisq0");
  m_pchisq1_dipTS = QAH2F("QaPtrkChi1TanlTS","primtrk: Chisq1 vs tanl(dip), tpc+svt",20,-3.2,3.2,20,0.,5.);
    m_pchisq1_dipTS->SetXTitle("dip angle");
    m_pchisq1_dipTS->SetYTitle("chisq1");
  m_pchisq0_zfTS = QAH2F("QaPtrkChi0zfTS","primtrk: Chisq0 vs zfirst, tpc+svt",20,-200.,200.,20,0.,5.);
    m_pchisq0_zfTS->SetXTitle("zfirst");
    m_pchisq0_zfTS->SetYTitle("chisq0");
  m_pchisq1_zfTS = QAH2F("QaPtrkChi1zfTS","primtrk: Chisq1 vs zfirst, tpc+svt",20,-200.,200.,20,0.,5.);
    m_pchisq1_zfTS->SetXTitle("zfirst");
    m_pchisq1_zfTS->SetYTitle("chisq1");
  m_pnfptonpt_momTS = QAH2F("QaPtrkRPntMomTS","primtrk: ratio Nfitpnt,Npnt vs log mom., tpc+svt",40,1.,4.,50,0.,1.2005); 
    m_pnfptonpt_momTS->SetXTitle("log P (MeV)");
    m_pnfptonpt_momTS->SetYTitle("Ratio Nfitpnt/Npnt");
  m_pnfptonpt_etaTS = QAH2F("QaPtrkRPntEtaTS","primtrk: ratio Nfitpnt,Npnt vs Eta, tpc+svt",40,-2.,2.,50,0.,1.2005); 
    m_pnfptonpt_etaTS->SetXTitle("eta");
    m_pnfptonpt_etaTS->SetYTitle("Ratio Nfitpnt/Npnt");
  m_ppsi_phiTS = QAH2F("QaPtrkPsiPhiTS","primtrk: psi vs phi, tpc+svt",36, 0.,360.,36,0.,360.);
    m_ppsi_phiTS->SetXTitle("phi");
    m_ppsi_phiTS->SetYTitle("psi");

/* These are FTPC histograms. The FTPC doesn't do primary tracking yet.
// 1D ftpc
  m_ppointFE      = QAH1F("QaPtrkNPntFE",    "primtrk: N points on trk,ftpc east", 15, 0.,15.);
  m_ppointFW      = QAH1F("QaPtrkNPntFW",    "primtrk: N points on trk,ftpc west", 15, 0.,15.);
  m_pmax_pointFE  = QAH1F("QaPtrkNPntMaxFE", "primtrk: N max pnts on trk, ftpc east", 15, 0.,15.);
  m_pmax_pointFW  = QAH1F("QaPtrkNPntMaxFW", "primtrk: N max pnts on trk, ftpc west", 15, 0.,15.);
  m_pfit_pointFE  = QAH1F("QaPtrkNPntFitFE", "primtrk: N fit pnts on trk, ftpc east", 15, 0.,15.);
  m_pfit_pointFW  = QAH1F("QaPtrkNPntFitFW", "primtrk: N fit pnts on trk, ftpc west", 15, 0.,15.);
  m_prim_ratioFE  = QAH1F("QaPtrkRnfFE",     "primtrk: ratio Nfit/tot pnt, ftpc east", 55, 0., 1.1);
  m_prim_ratioFW  = QAH1F("QaPtrkRnfFW",     "primtrk: ratio Nfit/tot pnt, ftpc west", 55, 0., 1.1);
  m_prim_ratiomFE = QAH1F("QaPtrkRnmFE",     "primtrk: ratio Nfit/max pnt, ftpc east", 55, 0., 1.1);
  m_prim_ratiomFW = QAH1F("QaPtrkRnmFW",     "primtrk: ratio Nfit/max pnt, ftpc west", 55, 0., 1.1);
  m_prim_chargeFE = QAH1F("QaPtrkChrgFE",    "primtrk: charge, ftpc east ", 20,-2.,2.);
  m_prim_chargeFW = QAH1F("QaPtrkChrgFW",    "primtrk: charge, ftpc west ", 20,-2.,2.);
  m_prim_radfFE   = QAH1F("QaPtrkRFE",       "primtrk: radial position of first hit, ftpc east", 40,0.,40.);
  m_prim_radfFW   = QAH1F("QaPtrkRFW",       "primtrk: radial position of first hit, ftpc west", 40,0.,40.);
  m_prim_xfFE     = QAH1F("QaPtrkXfFE",      "primtrk: x of first hit on trk, ftpc east", 50,-200.,200.);
  m_prim_xfFW     = QAH1F("QaPtrkXfFW",      "primtrk: x of first hit on trk, ftpc west", 50,-200.,200.);
  m_prim_yfFE     = QAH1F("QaPtrkYfFE",      "primtrk: y of first hit on trk, ftpc east", 50,-200.,200.);
  m_prim_yfFW     = QAH1F("QaPtrkYfFW",      "primtrk: y of first hit on trk, ftpc west", 50,-200.,200.);
  m_prim_zfFE     = QAH1F("QaPtrkZfFE",      "primtrk: z of first hit on trk, ftpc east", 50,-200.,200.);
  m_prim_zfFW     = QAH1F("QaPtrkZfFW",      "primtrk: z of first hit on trk, ftpc west", 50,-200.,200.);
  m_prim_radfFE   = QAH1F("QaPtrkRFE",       "primtrk: radial position of first hit, ftpc east", 40,0.,40.);
  m_prim_radfFW   = QAH1F("QaPtrkRFW",       "primtrk: radial position of first hit, ftpc west", 40,0.,40.);
  m_plengthFE     = QAH1F("QaPtrkLengthFE",  "primtrk: track length, ftpc east", 60,0.,120.);
  m_plengthFW     = QAH1F("QaPtrkLengthFW",  "primtrk: track length, ftpc west", 60,0.,120.);
  m_ppsiFE        = QAH1F("QaPtrkPsiFE",     "primtrk: psi, ftpc east", 36, 0.,360.);
  m_ppsiFW        = QAH1F("QaPtrkPsiFW",     "primtrk: psi, ftpc west", 36, 0.,360.);
  m_petaFE        = QAH1F("QaPtrkEtaFE",     "primtrk: eta, ftpc east",80,-8.,8.);
  m_petaFW        = QAH1F("QaPtrkEtaFW",     "primtrk: eta, ftpc west",80,-8.,8.);
  m_ppTFE         = QAH1F("QaPtrkPtFE",      "primtrk: pT, ftpc east",50,0.,5.);
  m_ppTFW         = QAH1F("QaPtrkPtFW",      "primtrk: pT, ftpc west",50,0.,5.);
  m_pmomFE        = QAH1F("QaPtrkPFE",       "primtrk: momentum, ftpc east ",50,0.,5.);
  m_pmomFW        = QAH1F("QaPtrkPFW",       "primtrk: momentum, ftpc west ",50,0.,5.);
  m_pchisq0FE     = QAH1F("QaPtrkChisq0FE",  "primtrk: chisq0, ftpc east", 50, 0.,5000.);
  m_pchisq0FW     = QAH1F("QaPtrkChisq0FW",  "primtrk: chisq0, ftpc west", 50, 0.,5000.);
  m_pchisq1FE     = QAH1F("QaPtrkChisq1FE",  "primtrk: chisq1, ftpc east", 50, 0.,500.);
  m_pchisq1FW     = QAH1F("QaPtrkChisq1FW",  "primtrk: chisq1, ftpc west", 50, 0.,500.);

// 2D - ftpc
  m_ppT_eta_recFE = QAH2F("QaPtrkPtVsEtaFE","primtrk: log pT vs eta, ftpcE",20,-5.,0.,40,1.,4.);
  m_ppT_eta_recFW = QAH2F("QaPtrkPtVsEtaFW","primtrk: log pT vs eta, ftpcW",20,0.,5.,40,1.,4.);
  m_primtrk_xf_yfFE = QAH2F("QaPtrkXfYfFE","primtrk: Y vs X of first hit on trk, ftpcE", 40,-40.,40.,40,-40.,40.);
    m_primtrk_xf_yfFE->SetXTitle("x first");
    m_primtrk_xf_yfFE->SetYTitle("y first");
  m_primtrk_xf_yfFW = QAH2F("QaPtrkXfYfFW","primtrk: Y vs X of first hit on trk, ftpcW", 40,-40.,40.,40,-40.,40.);
    m_primtrk_xf_yfFW->SetXTitle("x first");
    m_primtrk_xf_yfFW->SetYTitle("y first");
  m_peta_trklengthFE = QAH2F("QaPtrkLengthVEtaFE","primtrk: trk length vs eta, ftpcE",25,-4.5,-2.,30,0.,120.);
    m_peta_trklengthFE->SetXTitle("eta");
    m_peta_trklengthFE->SetYTitle("length");
  m_peta_trklengthFW = QAH2F("QaPtrkLengthVEtaFW","primtrk: trk length vs eta, ftpcW",25,2.,4.5,30,0.,120.);
    m_peta_trklengthFW->SetXTitle("eta");
    m_peta_trklengthFW->SetYTitle("length");
  m_pnpoint_lengthFE = QAH2F("QaPtrkNPntLengthFE","primtrk: N pnts vs length, ftpcE",30,0.,120.,15,0.,15.);
    m_pnpoint_lengthFE->SetXTitle("trk length");
    m_pnpoint_lengthFE->SetYTitle("Npoints on trk");
  m_pnpoint_lengthFW = QAH2F("QaPtrkNPntLengthFW","primtrk: N pnts vs length, ftpcW",30,0.,120.,15,0.,15.);
    m_pnpoint_lengthFW->SetXTitle("trk length");
    m_pnpoint_lengthFW->SetYTitle("Npoints on trk");
  m_pfpoint_lengthFE = QAH2F("QaPtrkFitPntLengthFE","primtrk: N fit pnts vs length, ftpcE",30,0.,120.,15,0.,15.);
    m_pfpoint_lengthFE->SetXTitle("trk length");
    m_pfpoint_lengthFE->SetYTitle("Npoints on trk");
  m_pfpoint_lengthFW = QAH2F("QaPtrkFitPntLengthFW","primtrk: N fit pnts vs length, ftpcW",30,0.,120.,15,0.,15.);
    m_pfpoint_lengthFW->SetXTitle("trk length");
    m_pfpoint_lengthFW->SetYTitle("Npoints on trk");
*/
}
//_____________________________________________________________________________
void StQABookHist::BookHistDE(){
  
  // for method MakeDE - from table dst_dedx
  m_ndedxr  = QAH1F("QaDedxNum",     "dedx: number of tracks", 64,0., 16000.); 

  m_ndedxT   = QAH1F("QaDedxNdedxT", "dedx: number of point to define dE/dx, tpc", 50,0., 50.);  
  m_dedx0T   = QAH1F("QaDedxDedx0T", "dedx: dE/dx mean (GeV/cm), tpc", ndedx, 0., 0.0005);
  m_dedx1T   = QAH1F("QaDedxDedx1T", "dedx: dE/dx error on mean, tpc", ndedx, 0., 0.0001);
  
  m_ndedxFE   = QAH1F("QaDedxNdedxFE", "dedx: number of point to define dE/dx, ftpcE", 20,0., 20.);  
  m_dedx0FE   = QAH1F("QaDedxDedx0FE", "dedx: dE/dx mean, ftpcE", ndedx,  0., 750.);
  m_dedx1FE   = QAH1F("QaDedxDedx1FE", "dedx: dE/dx sigma, ftpcE", ndedx,  0., 0.05);
  
  m_ndedxFW   = QAH1F("QaDedxNdedxFW", "dedx: number of point to define dE/dx, ftpcW", 20,0., 20.);  
  m_dedx0FW   = QAH1F("QaDedxDedx0FW", "dedx: dE/dx mean, ftpcW", ndedx,  0., 750.);
  m_dedx1FW   = QAH1F("QaDedxDedx1FW", "dedx: dE/dx sigma, ftpcW", ndedx, 0., 0.05);
  
}
//_____________________________________________________________________________
void StQABookHist::BookHistGen(){

  // for MakeHistGen - from table particle
  m_H_npart     = QAH1F("QaEvgenNPart",      "particle:total num particles (generated)",100,0.,30000.);
  m_H_npart_sm  = QAH1F("QaEvgenNPartsm",    "particle:total num particles (generated)",20,0.,20.);
  m_H_ncpart    = QAH1F("QaEvgenNChgPart",   "particle:num chg (e,mu,pi,K,p) part (generated)",
                       100,0.,20000.);
  m_H_ncpart_sm = QAH1F("QaEvgenNChgPartsm", "particle:num chg (e,mu,pi,K,p) part (generated)",
                       20,0.,20.);
  m_H_pT_gen  = QAH1F("QaEvgenPt",         "particle: charged pt (generated)",nxpT,xminpT,xmaxpT);
  m_H_pT_genT = QAH1F("QaEvgenPtT",        "particle: charged pt (generated), tpc",nxpT,xminpT,xmaxpT);
  m_H_eta_gen = QAH1F("QaEvgenEta",        "particle: charged eta (generated)",60,-6.,6.);
  m_H_pT_eta_gen = QAH2F("QaEvgenPtVsEta", "particle: charged pT versus eta (generated)",
			    20,-2.,2.,40,1.,4.);
  m_H_pT_eta_gen->SetXTitle("eta");
  m_H_pT_eta_gen->SetYTitle("pT (GeV)");
  m_H_vtxx    = QAH1F("QaEvgenVtxX","particle: Generator prod vertex x (mm)",50,-1.,1.);
  m_H_vtxy    = QAH1F("QaEvgenVtxY","particle: Generator prod vertex y (mm)",50,-1.,1.);
  m_H_vtxz    = QAH1F("QaEvgenVtxZ","particle: Generator prod vertex z (mm)",50,-5.,5.);

}
//_____________________________________________________________________________
void StQABookHist::BookHistPID(){
  
  // for MakeHistPID - from tables globtrk & dst_dedx 
  // Spectra/pid histograms. C.Ogilvie
  
  m_p_dedx_rec = QAH2F("QaPidGlobtrkDstdedxPVsDedx","PID: globtrk-dst_dedx,  p vs dedx (reconstructed)",
			  cnp,cminp,cmaxp,cndedx,cmindedx,cmaxdedx);
  m_p_dedx_rec->SetYTitle("dedx");
  m_p_dedx_rec->SetXTitle("p (GeV)");
  
}
//_____________________________________________________________________________
void StQABookHist::BookHistVertex(){
  // for MakeHistVertex - from table dst_vertex
  
  
  m_v_num   = QAH1F("QaVtxNum",  " vertex: num vertices ",50,0.,2000.);
  m_v_num_sm= QAH1F("QaVtxNumsm",  " vertex: num vertices ",50,0.,20.);

  m_v_vtxid = QAH1F("QaVtxVtxId"," vertex,2ndary: Vertex ID ",10,0.,10.);
  m_v_x     = QAH1F("QaVtxX",    " vertex,2ndary: x ",50,-250.,250.);
  m_v_y     = QAH1F("QaVtxY",    " vertex,2ndary: y ",50,-250.,250.);
  m_v_z     = QAH1F("QaVtxZ",    " vertex,2ndary: z ",50,-250.,250.);
  m_v_pchi2 = QAH1F("QaVtxChisq"," vertex,2ndary: chisq/dof ",50,0.,50.);
  m_v_r     = QAH1F("QaVtxR",    " vertex,2ndary: r ",100,0,50);
  
  m_pv_vtxid = QAH1F("QaVtxPrVtxId"," vertex,prim: Vertex ID ",10,0.,10.);
  m_pv_x     = QAH1F("QaVtxPrX",    " vertex,prim: x ",50,-5.,5.);
  m_pv_y     = QAH1F("QaVtxPrY",    " vertex,prim: y ",50,-5.,5.);
  m_pv_z     = QAH1F("QaVtxPrZ",    " vertex,prim: z ",50,-50.,50.);
  m_pv_pchi2 = QAH1F("QaVtxPrChisq"," vertex,prim: chisq/dof ",40,0.,20.);
  m_pv_r     = QAH1F("QaVtxPrR",    " vertex,prim: r ",100,0,0.1);

  m_vtx_z    = QAH1F("QaVtxZres"," vertex: z(tpc)-z(svt), resolution check",100,-.1,.1);
  
  m_v0             = QAH1F("QaV0Vtx","dst_v0_vertex: Number V0 found ",50,0.,2000.);
  m_ev0_lama_hist  = QAH1F("QaV0LambdaMass","dst_v0_vertex: Lambda mass",50,1.05,1.15);
  m_ev0_k0ma_hist  = QAH1F("QaV0K0Mass","dst_v0_vertex: k0 mass",50,.4,.6);

  m_xi_tot     = QAH1F("QaXiVtxTot", "dst_xi_vertex: tot # vertices ",80,0.,800.);
  m_xi_ma_hist = QAH1F("QaXiaMass",  "dst_xi_vertex: Xi mass",50,1.2,1.4);

  m_kink_tot   = QAH1F("QaKinkTot",  "kinkVertex: # kinks ",25,0.,25.);

}
//_____________________________________________________________________________
void StQABookHist::BookHistPoint(){

  m_pnt_tot     = QAH1F("QaPointTot", "point: # hits total ",100, 0.,250000.);
  m_pnt_tot_med = QAH1F("QaPointTotmed","point: # hits total ",100, 0.,2500.);
  m_pnt_tot_sm  = QAH1F("QaPointTotsm", "point: # hits total ",100, 0.,250.);
  m_pnt_id      = QAH1F("QaPointId","point: detector ID of hit",30,0.,30.);

  m_pnt_tpc     = QAH1F("QaPointTpc",  "point: # hits tpc ",100, 0.,250000.);
  m_pnt_svt     = QAH1F("QaPointSvt",  "point: # hits svt ",100, 0.,10000.);
  m_pnt_ssd     = QAH1F("QaPointSsd",  "point: # hits ssd ",100, 0.,5000.);
  m_pnt_ftpcE   = QAH1F("QaPointFtpcE","point: # hits ftpcE ",100, 0.,10000.);
  m_pnt_ftpcW   = QAH1F("QaPointFtpcW","point: # hits ftpcW ",100, 0.,10000.);

}
//_____________________________________________________________________________
void StQABookHist::BookHistRich(){

  m_rich_tot   = QAH1F("QaRichTot",  "g2t_rch_hit: multiplicity ",50,0.,1000.);

}
//_____________________________________________________________________________

void StQABookHist::BookHistEval(){

// these only get filled if the geant dataset is available!

   m_geant_reco_pvtx_x  = QAH1F("QaGRpvtxDx"," diff geant - reco prim vtx X",
                              100, -0.25,0.25);
   m_geant_reco_pvtx_y  = QAH1F("QaGRpvtxDy"," diff geant - reco prim vtx Y",
                              100, -0.25,0.25);
   m_geant_reco_pvtx_z  = QAH1F("QaGRpvtxDz"," diff geant - reco prim vtx Z",
                              100, -0.25,0.25);
   m_geant_reco_vtx_z_z = QAH2F("QaGRpvtxDzZ",
        " reco pvtx Z vs diff geant - reco Z", 100, -0.5,0.5,100,-50.,50.);
     m_geant_reco_vtx_z_z->SetXTitle("z vtx resolution (cm)");
     m_geant_reco_vtx_z_z->SetYTitle("z position of vtx (cm)");

}

//_____________________________________________________________________________


