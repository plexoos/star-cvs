/*************************************************
 *
 * $Id: StMcEventMaker.cxx,v 1.43 2003/08/20 18:51:01 calderon Exp $
 * $Log: StMcEventMaker.cxx,v $
 * Revision 1.43  2003/08/20 18:51:01  calderon
 * Filling of Tof and Pixel classes.
 *
 * Revision 1.42  2003/02/19 03:17:04  calderon
 * Code to fill the StMcCtbHitCollection from the g2t tables by the Gansinator.
 *
 * Revision 1.41  2003/01/23 04:03:20  jeromel
 * Include fixed
 *
 * Revision 1.40  2001/10/15 20:38:56  pavlinov
 * Added bfcTree/geantBranch for searching g2t tablesStMcEventMaker.cxx
 *
 * Revision 1.39  2001/05/13 21:14:49  calderon
 * Modifications from Aleksei : StMcEmcHitCollections changed, added
 * method for printing Emc information of the event
 *
 * Revision 1.38  2001/04/25 18:10:50  perev
 * HPcorrs
 *
 * Revision 1.37  2001/03/13 16:14:02  pavlinov
 * StEmcGeom moved to StEmcUtil
 *
 * Revision 1.36  2000/09/07 21:04:51  calderon
 * Remove requirement on having g2t_tpc_hit table to create StMcEvent.
 * Print diagnostics accordingly.
 * Add doUseBsmd(kTRUE) to StMcEventMaker constructor.
 *
 * Revision 1.35  2000/08/11 20:57:00  calderon
 * bug fix to PrintInfo(), Thanks Maria
 *
 * Revision 1.34  2000/06/22 23:53:31  calderon
 * Changes from Aleksei for filling of emc hits.
 * ttemp and ttempParticle are now data members.
 *
 * Revision 1.33  2000/06/09 19:52:42  calderon
 * use compMcHit instead of compMcTpcHit and compMcSvtHit
 *
 * Revision 1.32  2000/06/06 03:00:18  calderon
 * Introduction of Calorimeter classes.  Filled according to algorithm from
 * Aleksei, plus some additional checks.
 *
 * Revision 1.31  2000/05/19 17:46:56  calderon
 * remove requirement to find particle table
 * remove requirement to check volume id for rich hits
 *
 * Revision 1.30  2000/05/17 23:43:11  calderon
 * assign the parent of the tracks from the particle table at the end
 * of the track loop.
 *
 * Revision 1.29  2000/05/17 19:42:01  calderon
 * Fix bug in assigning parent tracks, Thanks Zhangbu
 *
 * Revision 1.28  2000/05/11 20:16:01  calderon
 * Fix typo in checking for volume id of rich hits, Thanks Jamie.
 *
 * Revision 1.27  2000/05/11 14:40:29  calderon
 * Added switches to do/do not load hit information from different detectors.
 * By default, all the detectors' hit information is loaded.
 *
 * Revision 1.26  2000/05/04 22:46:39  calderon
 * pdg Id is now read in the track constructor for all tracks
 *
 * Revision 1.25  2000/04/24 22:08:23  calderon
 * change for indexing of mother particles
 *
 * Revision 1.24  2000/04/20 15:56:16  calderon
 * If particle & g2t_rch_hit table are not found in geant branch,
 * look for them in dst branch (for backwards compatibility).
 *
 * Revision 1.23  2000/04/19 17:45:22  calderon
 * particle table and g2t_rch_hit table are now in geant branch
 *
 * Revision 1.22  2000/04/18 23:17:23  calderon
 * delete the svt hit if it is not possible to store it
 *
 * Revision 1.21  2000/04/18 00:56:39  calderon
 * Add pdgId to tracks that appear in both tables
 *
 * Revision 1.20  2000/04/17 23:01:56  calderon
 * proper casting to remove a comparison warning
 *
 * Revision 1.19  2000/04/12 21:32:36  calderon
 * check for eg_label in range of particle table
 *
 * Revision 1.18  2000/04/06 23:29:40  calderon
 * The parent track is now stored for all tracks.
 *
 * Revision 1.17  2000/04/06 20:26:55  calderon
 * All particles in the particle table are now filled.
 * Relationships to parents can be followed to event generator particles.
 *
 * Revision 1.16  2000/04/06 08:38:08  calderon
 * First version using the particle table.
 * The parent daughter relationship(if it exists) between tracks in the g2t_track table
 * and tracks that come from the
 * event generator is successfully established.  Next step is to load
 * the rest of the particle table entries that don't have any descendants
 * in the g2t table (which is the majority of the entries).
 *
 * Revision 1.15  2000/04/04 23:15:43  calderon
 * Report number of hits successfully stored and additional
 * reporting of hits with bad volume id.
 *
 * Revision 1.14  2000/03/06 18:07:36  calderon
 * 1) Check tpc hit volume id to not load hits in pseudo pad rows.
 * 2) Sort the hits in the collections, in order to save time
 * later during hit associations.
 *
 * Revision 1.13  2000/02/04 15:40:52  calderon
 * Fix dumping of vertex info when there is just one vertex.
 *
 * Revision 1.12  2000/01/18 20:53:08  calderon
 * Changes to work with CC5
 *
 * Revision 1.11  2000/01/11 23:18:56  calderon
 * Check if there are hits before writing info to screen.
 *
 * Revision 1.10  1999/12/14 07:05:32  calderon
 * Use numbering scheme
 *
 * Revision 1.9  1999/12/03 19:40:42  calderon
 * volume_id for SVT hits can be up to ~8700.
 *
 * Revision 1.8  1999/12/03 00:55:21  calderon
 * Completely revised for StMcEvent 2.0
 * Using StDbUtilities for coordinate transformations.
 * Tested g2t_event table is read properly (when available).
 * Added messages for diagnostics.
 * Tested in Linux, Solaris 4.2  and HP.
 *
 * Revision 1.7  1999/09/24 01:23:18  fisyak
 * Reduced Include Path
 *
 * Revision 1.6  1999/09/23 21:25:59  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 * Revision 1.5  1999/09/15 18:39:28  calderon
 * -Do not require g2t_ftp_hit table for filling of StMcEvent
 * -Update README for changes
 *
 * Revision 1.4  1999/09/10 19:11:54  calderon
 * Write the Ntuple in StMcAnalysisMaker into a file.
 * This way it can be accessed after the macro finishes,
 * otherwise it gets deleted.
 *
 * Revision 1.3  1999/07/28 20:27:42  calderon
 * Version with SL99f libraries
 *
 *
 *************************************************/
#include <iostream.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#ifndef ST_NO_NAMESPACES
using std::string;
using std::sort;
using std::find;
#endif

#include "TStyle.h"
#include "TCanvas.h"
#include "StMcEventMaker.h"

#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StGlobals.hh"
#include "StMessMgr.h"
#include "StMemoryInfo.hh"
#include "StTimer.hh"

#include "StThreeVectorF.hh"

#include "StChain.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"

#include "tables/St_g2t_event_Table.h"
#include "tables/St_g2t_ftp_hit_Table.h"
#include "tables/St_g2t_rch_hit_Table.h"
#include "tables/St_g2t_ctf_hit_Table.h"
#include "tables/St_g2t_svt_hit_Table.h"
#include "tables/St_g2t_tpc_hit_Table.h"
#include "tables/St_g2t_emc_hit_Table.h"
#include "tables/St_g2t_pix_hit_Table.h"
#include "tables/St_g2t_track_Table.h"
#include "tables/St_g2t_vertex_Table.h"
#include "tables/St_particle_Table.h"

#include "StMcEventTypes.hh"

#include "StEmcUtil/geometry/StEmcGeom.h" // For Barrel Emc


static double vertexCut = .0000025; // 25 nm (lifetime of the pi0)
struct vertexFlag {
	      StMcVertex* vtx;
	      int primaryFlag; };

static const char rcsid[] = "$Id: StMcEventMaker.cxx,v 1.43 2003/08/20 18:51:01 calderon Exp $";
ClassImp(StMcEventMaker)


//_____________________________________________________________________________

    
StMcEventMaker::StMcEventMaker(const char*name, const char * title) :
    StMaker(name,title),
    doPrintEventInfo (kFALSE),  
    doPrintMemoryInfo(kFALSE),  
    doPrintCpuInfo   (kFALSE), 
    doUseTpc         (kTRUE),
    doUseSvt	     (kTRUE),
    doUseFtpc	     (kTRUE),
    doUseRich        (kTRUE),
    doUseBemc        (kTRUE),
    doUseBsmd        (kTRUE),
    doUseCtb         (kTRUE),
    doUseTofp        (kTRUE),
    doUseTof         (kTRUE),
    doUsePixel       (kTRUE),
    ttemp(),
    ttempParticle(),
    mCurrentMcEvent(0)
    
{
    // StMcEventMaker - constructor
    // - set all pointers defined in the header file to zero

}
//_____________________________________________________________________________


StMcEventMaker::~StMcEventMaker()
{
    // StMcEventMaker - destructor
    cout << "Inside ReaderMaker Destructor" << endl;
    SafeDelete(mCurrentMcEvent);  //

}



//_____________________________________________________________________________

void StMcEventMaker::Clear(const char*)
{
    if (doPrintMemoryInfo) StMemoryInfo::instance()->snapshot();    
    // StMcEventMaker - Clear,
    delete mCurrentMcEvent;
    mCurrentMcEvent = 0;
    if (doPrintMemoryInfo) {
	StMemoryInfo::instance()->snapshot();
	StMemoryInfo::instance()->print();
    }
    StMaker::Clear();
}


//_____________________________________________________________________________


Int_t StMcEventMaker::Finish()
{
    // StMcEventMaker - Finish, Draw histograms if SetDraw true

    // Right now I'm not doing any histograms, later on, I would need to uncomment
    // the next line, and add a DrawHists() method.  Look in St_QA_Maker.cxx
    //if (drawinit)  DrawHists();
  return StMaker::Finish();
}


//_____________________________________________________________________________

Int_t StMcEventMaker::Init()
{
    return StMaker::Init();
}

//_____________________________________________________________________________

Int_t StMcEventMaker::Make()
{
    // StMcEventMaker - Make; fill StMcEvent objects
    StTimer timer;
    if (doPrintCpuInfo) timer.start();
    if (doPrintMemoryInfo) StMemoryInfo::instance()->snapshot();
    
    cout << "Inside StMcEventMaker::Make()" << endl;
    // We're supposed to get the dataset from the chain. I don't know how yet. I think it is:
    
    Char_t* geaTmp[3]={"geant","event/geant/Event","bfcTree/geantBranch"};
    St_DataSet* dsGeant = 0;
    for(UInt_t i=0; i<3; i++){ 
      dsGeant = GetDataSet(geaTmp[i]);
      if(!dsGeant || !dsGeant->GetList()) {
	gMessMgr->Warning() << "Could not find dataset " << geaTmp[i] << endm;
        dsGeant = GetDataSet("event/geant/Event");
      } else {
	gMessMgr->Info() << "Get Geant info from  dataset " << geaTmp[i] << endm;
        break;
      }
    }
    if(!dsGeant) return kStWarn;;
    /* 
    if(!dsGeant || !dsGeant->GetList()) {
	gMessMgr->Warning() << "Could not find dataset geant" << endm;
        dsGeant = GetDataSet("event/geant/Event");
        if(!dsGeant || !dsGeant->GetList()) {  // Try direct output from Geant
	  gMessMgr->Warning() << "Could not find dataset event/geant/Event" << endm;
          dsGeant = GetDataSet("bfcTree/geantBranch");
          if(!dsGeant || !dsGeant->GetList()) {  // Try output from bfc forGeant
	    gMessMgr->Warning() << "Could not find dataset bfcTree/geantBranch" << endm;
	    return kStWarn;
          } else cout<< "Find dataset bfcTree/geantBranch "<<endl;
        }
    }
    */
    // This is done only for one file, though.  I haven't put functionality for
    // multiple file handling.  If needed, it should start in the StMcEventReadMacro
    // and try to follow the doEvents.C macro to read in multiple files.
    // Then, we would need to mimic the nextRootFile() methods and so on.
    
  
    // Now we have the DataSet, but for some reason, we need the Iterator to navigate
    
    St_DataSetIter geantDstI(dsGeant);
      
    // Now the Iterator is set up, and this allows us to access the tables
    // This is done like so:
    // TableClass *instanceOfTableClassPointer = cast to TableClassPointer instanceOfDataSetIter("actual name of table in data set");
    
    St_g2t_event   *g2t_eventTablePointer   =  (St_g2t_event   *) geantDstI("g2t_event");
    St_g2t_vertex  *g2t_vertexTablePointer  =  (St_g2t_vertex  *) geantDstI("g2t_vertex");
    St_g2t_track   *g2t_trackTablePointer   =  (St_g2t_track   *) geantDstI("g2t_track");
    St_g2t_tpc_hit *g2t_tpc_hitTablePointer =  (St_g2t_tpc_hit *) geantDstI("g2t_tpc_hit");
    St_g2t_svt_hit *g2t_svt_hitTablePointer =  (St_g2t_svt_hit *) geantDstI("g2t_svt_hit");
    St_g2t_ftp_hit *g2t_ftp_hitTablePointer =  (St_g2t_ftp_hit *) geantDstI("g2t_ftp_hit");
    St_g2t_rch_hit *g2t_rch_hitTablePointer =  (St_g2t_rch_hit *) geantDstI("g2t_rch_hit");
    St_g2t_emc_hit *g2t_emc_hitTablePointer =  (St_g2t_emc_hit *) geantDstI("g2t_emc_hit");
    St_g2t_emc_hit *g2t_smd_hitTablePointer =  (St_g2t_emc_hit *) geantDstI("g2t_smd_hit");
    St_g2t_ctf_hit *g2t_ctb_hitTablePointer =  (St_g2t_ctf_hit *) geantDstI("g2t_ctb_hit"); // Added CTB Hits
    St_g2t_ctf_hit *g2t_tof_hitTablePointer =  (St_g2t_ctf_hit *) geantDstI("g2t_tof_hit");
    St_g2t_ctf_hit *g2t_tfr_hitTablePointer =  (St_g2t_ctf_hit *) geantDstI("g2t_tfr_hit");
    St_g2t_pix_hit *g2t_pix_hitTablePointer =  (St_g2t_pix_hit *) geantDstI("g2t_pix_hit");
    St_particle    *particleTablePointer    =  (St_particle    *) geantDstI("particle");

    // For backwards compatibility, look for the rch and particle tables also in the dstBranch
    St_DataSetIter dstDstI(GetDataSet("dst"));
    if (!particleTablePointer)
	particleTablePointer    = (St_particle    *) dstDstI("particle");
    if (!g2t_rch_hitTablePointer)
	g2t_rch_hitTablePointer = (St_g2t_rch_hit *) dstDstI("g2t_rch_hit");

    // Now we check if we have the pointer, if we do, then we can access the tables!
  
    if (g2t_vertexTablePointer && g2t_trackTablePointer){
	
	//
	// g2t_event Table
	//
	g2t_event_st   *eventTable = 0;
	
	// Check Pointer
	if (g2t_eventTablePointer)
	    eventTable  = g2t_eventTablePointer->GetTable();
	else
	    cerr << "Table g2t_event Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;

	// Check Table
	if(!g2t_eventTablePointer->GetNRows()) {
	    cout << "Event Table is EMPTY!! " << endl;
	    PR(g2t_eventTablePointer->GetNRows());
	    PR(eventTable)
	}
	//
	// Vertex, Track and table don't have problems normally.
	//
	g2t_vertex_st  *vertexTable = g2t_vertexTablePointer->GetTable();
	g2t_track_st   *trackTable  = g2t_trackTablePointer->GetTable();

	//
	// Tpc Hit Table (might not be there for photon events
	//
	g2t_tpc_hit_st *tpcHitTable = 0;
	if (g2t_tpc_hitTablePointer)
	    tpcHitTable = g2t_tpc_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_tpc_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;
	//
	// Ftpc Hit Table
	//
	g2t_ftp_hit_st *ftpHitTable = 0;
	if (g2t_ftp_hitTablePointer)
	    ftpHitTable = g2t_ftp_hitTablePointer->GetTable();
	else 
	    cerr << "Table g2t_ftp_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;

	//
	// Svt Hit Table
	//
	g2t_svt_hit_st *svtHitTable = 0;
	if (g2t_svt_hitTablePointer)
	    svtHitTable = g2t_svt_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_svt_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;
	
	//
	// Rich Hit Table
	//
	g2t_rch_hit_st *rchHitTable = 0;
	if (g2t_rch_hitTablePointer)
	    rchHitTable = g2t_rch_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_rch_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;
	//
	// Ctb Hit Table
	//
	g2t_ctf_hit_st *ctbHitTable = 0;
	if (g2t_ctb_hitTablePointer)
	    ctbHitTable = g2t_ctb_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_rch_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;

	// TOF Hit Tables
	//
	g2t_ctf_hit_st *tofHitTable = 0;
	if (g2t_tof_hitTablePointer)
	    tofHitTable = g2t_tof_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_tof_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;
	
	g2t_ctf_hit_st *tfrHitTable = 0;
	if (g2t_tfr_hitTablePointer)
	    tfrHitTable = g2t_tfr_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_tfr_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;

	//
	// BEMC and BPRS Hit Table
	//
	g2t_emc_hit_st *emcHitTable = 0;
	if (g2t_emc_hitTablePointer)
	    emcHitTable = g2t_emc_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_emc_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;

	//
	// BSMDE and BSMDP Hit Table
	//
	g2t_emc_hit_st *smdHitTable = 0;
	if (g2t_smd_hitTablePointer)
	    smdHitTable = g2t_smd_hitTablePointer->GetTable();
	else
	    cerr << "Table g2t_smd_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;
	//	
	// Pixel Hit Table
	//
	g2t_pix_hit_st *pixHitTable=0;
	if (g2t_pix_hitTablePointer)
	    pixHitTable = g2t_pix_hitTablePointer->GetTable();
	else 
	    cerr << "Table g2t_pix_hit Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;

	//
	// particle Table
	//
	particle_st *particleTable = 0;
	if (particleTablePointer)
	    particleTable = particleTablePointer->GetTable();
	else
	    cerr << "Table particle Not found in Dataset " << geantDstI.Pwd()->GetName() << endl;
       
       // Before filling StMcEvent, we can check whether we can actually
       // access the tables.
       
// 	  cout << "Event Table Examples: " << endl;
// 	  cout << "eg_label: " << eventTable->eg_label << endl; 
// 	  cout << "n_event : " << eventTable->n_event << endl;
// 	  cout << "n_run   : " << eventTable->n_run << endl;
// 	  cout << "n_part_prot_west: " <<  eventTable->n_part_prot_west << endl;
// 	  cout << "n_part_neut_west: " <<  eventTable->n_part_neut_west << endl;
// 	  cout << "n_part_prot_east: " <<  eventTable->n_part_prot_east << endl;
// 	  cout << "n_part_neut_east: " <<  eventTable->n_part_neut_east << endl;

// 	  cout << "Vertex Table Examples:" << endl;
// 	  cout << "ge_x[0] :" << vertexTable[0].ge_x[0] << endl;
// 	  cout << "ge_x[1] :" << vertexTable[0].ge_x[1] << endl;
// 	  cout << "ge_x[2] :" << vertexTable[0].ge_x[2] << endl;

// 	  cout << "Track Table Examples:" << endl;
// 	  cout << "p[0] :" << trackTable[0].p[0] << endl;
// 	  cout << "p[1] :" << trackTable[0].p[1] << endl;
// 	  cout << "p[2] :" << trackTable[0].p[2] << endl;
	  
// 	  cout << "Tpc Hit Table Examples:" << endl;
// 	  cout << "p[0] :" << tpcHitTable[0].p[0] << endl;
// 	  cout << "p[1] :" << tpcHitTable[0].p[1] << endl;
// 	  cout << "p[2] :" << tpcHitTable[0].p[2] << endl;

// 	  cout << "Svt Hit Table Examples:" << endl;
// 	  cout << "p[0] :" << svtHitTable[0].p[0] << endl;
// 	  cout << "p[1] :" << svtHitTable[0].p[1] << endl;
// 	  cout << "p[2] :" << svtHitTable[0].p[2] << endl;

// 	  cout << "Ftpc Hit Table Examples:" << endl;
// 	  cout << "p[0] :" << ftpHitTable[0].p[0] << endl;
// 	  cout << "p[1] :" << ftpHitTable[0].p[1] << endl;
// 	  cout << "p[2] :" << ftpHitTable[0].p[2] << endl;
	  
// 	  cout << "Rich Hit Table Examples:" << endl;
// 	  cout << "p[0] :" << rchHitTable[0].p[0] << endl;
// 	  cout << "p[1] :" << rchHitTable[0].p[1] << endl;
// 	  cout << "p[2] :" << rchHitTable[0].p[2] << endl;
	  
	// Ok, now we have the g2t tables for this event, now we can create the
	// StMcEvent with them.
	
	// Now Here goes the scheme Mike Lisa and I cooked up
	
	//______________________________________________________________________
	// Step 1 - fill the StMcEvent, already created in the header file
	
	if (eventTable)
	    mCurrentMcEvent = new StMcEvent(eventTable);
	else {
	    gMessMgr->Warning() << "StMcEventMaker::Make(): g2t_event Table not found.  Using default constructor." << endm;
	    mCurrentMcEvent = new StMcEvent;  
	}
	
	if (mCurrentMcEvent) cout << "****  Created new StMcEvent" << endl;
	else {
	    gMessMgr->Warning() << "Could not create StMcEvent! Exit from StMcEventMaker." << endm;
	    return kStWarn;
	}
	//______________________________________________________________________
	// Step 2 - Fill Vertices - we do not fill parent/daughters until Step 3
	
	long NVertices = g2t_vertexTablePointer->GetNRows();
	  
#ifndef ST_NO_TEMPLATE_DEF_ARGS
	vector<vertexFlag> vtemp(NVertices); // Temporary array for Step 3
#else
	vector<vertexFlag, allocator<vertexFlag> > vtemp(NVertices);
#endif
       
	cout << "Preparing to process and fill VERTEX information ....." << endl;
	StMcVertex* v = 0;
	
	
	// First vertex is PRIMARY
	
	v = new StMcVertex(&(vertexTable[0]));
	mCurrentMcEvent->vertices().push_back(v);
	vtemp[vertexTable[0].id - 1].vtx = v;
	vtemp[vertexTable[0].id - 1].primaryFlag = 1;
	mCurrentMcEvent->setPrimaryVertex(v);
	
	StThreeVectorF primVertexPos = v->position();
	
	StThreeVectorF testVertexPos;
	int nThrownVertices = 0;
	for (long ivtx=1; ivtx<NVertices; ivtx++)
	    {    
		v = new StMcVertex(&(vertexTable[ivtx]));
		
		// get the position of the current vertex
		testVertexPos = v->position();
		
		if (vertexTable[ivtx].eg_label == 0 ||
		    abs(primVertexPos - testVertexPos) > vertexCut ) {
		    // GEANT vertex or (generator vertex that satisfies cut) ...
		    mCurrentMcEvent->vertices().push_back(v);  // adds vertex v to master collection
		    vtemp[vertexTable[ivtx].id - 1].primaryFlag = 0;
		    vtemp[vertexTable[ivtx].id - 1].vtx = v;
		}
		else { // These "vertices" are the same as primary, so flag them
		    nThrownVertices++;
		    vtemp[vertexTable[ivtx].id - 1].primaryFlag = 1;
		    
		    delete v;
		    vtemp[vertexTable[ivtx].id - 1].vtx = mCurrentMcEvent->primaryVertex();
		}
	    }
	if (nThrownVertices) 
	    gMessMgr->Warning() << "StMcEventMaker::Make(): Throwing " << nThrownVertices
				<< " that are the same as the primary vertex." << endm;
       
	//______________________________________________________________________
	// Step 3 - Fill Tracks - we do not fill associated hits until Step 4
	
	long NTracks = g2t_trackTablePointer->GetNRows();
	size_t usedTracksG2t = 0;
	long NGeneratorTracks = (particleTablePointer) ? particleTablePointer->GetNRows() : 0;
	size_t usedTracksEvGen = 0;
	ttemp.resize(NTracks);
	ttempParticle.resize(NGeneratorTracks);
	cout << "Preparing to process and fill TRACK information ....." << endl;
	StMcTrack* egTrk = 0;
	size_t nParticlesInBothTables = 0;
	cout << "Event Generator Tracks..." << endl;

	// The filling of the event generator track assumes that if we encounter a particle that
	// has a mother, the mother should ALREADY HAVE BEEN CREATED, i.e. that the mother indices
	// of the particles in the particle table is never more than the current index.

	long motherIndex = -1;  // Set it to some unused number. 
	{for (long gtrk=0; gtrk<NGeneratorTracks; gtrk++) {
	    egTrk = new StMcTrack(&(particleTable[gtrk]));
	    egTrk->setEventGenLabel(gtrk+1);
	    ttempParticle[gtrk] = egTrk;
	    mCurrentMcEvent->tracks().push_back(egTrk); // adds track egTrk to master collection 
	    usedTracksEvGen++;
	    
	}} // Generator Tracks Loop
	
	StMcTrack* t = 0;
	long iStartVtxId = 0;
	long iStopVtxId = 0;
	long iItrmdVtxId = 0;
	
	int nThrownTracks = 0;
	cout << "g2t Tracks..." << endl;
	
	{for (long itrk=0; itrk<NTracks; itrk++) {
	    iStopVtxId = (trackTable[itrk].stop_vertex_p) - 1;
		    		
	    if (iStopVtxId >= 0) {
		if (vtemp[iStopVtxId].primaryFlag == 1) {
		    
		    nThrownTracks++;
		    continue; // This skips until the next itrk
		    
		}
	    }    
	    t = new StMcTrack(&(trackTable[itrk]));
	    usedTracksG2t++;
	    ttemp[ trackTable[itrk].id - 1 ] = t; // This we do for all accepted tracks
	    
	    
	    mCurrentMcEvent->tracks().push_back(t); // adds track t to master collection
	    
	    // point track to its stop vertex,
	    // and tell stop vertex that this is its parent
	    if (iStopVtxId >= 0) {
		t->setStopVertex(vtemp[iStopVtxId].vtx);
		vtemp[iStopVtxId].vtx->setParent(t);
	    }
	    //cout << "Established relation btw track & STOP vertex" << endl;
	    
	    // point track to its parent vertex,
	    // and tell parent vertex that this is its daughter
	    
	    iStartVtxId = trackTable[itrk].start_vertex_p - 1;
	    
	    v = vtemp[iStartVtxId].vtx; // This should already know the right vertex.
	    
	    t->setStartVertex(v);
	    v->addDaughter(t);
	    //cout << "Established relation btw track & START vertex" << endl;
	    
	    // now fill track's intermediate vertex collection,
	    // and tell those vertices their parents
	    
	    iItrmdVtxId = (trackTable[itrk].itrmd_vertex_p) - 1;
	    
	    if (iItrmdVtxId >= 0) {
		t->intermediateVertices().push_back(vtemp[iItrmdVtxId].vtx);
		
		vtemp[iItrmdVtxId].vtx->setParent(t);
		
		// follow the "linked list" of g2t_vertex table to get the rest
		
		while(vertexTable[iItrmdVtxId].next_itrmd_p != 0) {
		    iItrmdVtxId = (vertexTable[iItrmdVtxId].next_itrmd_p) - 1;
		    t->intermediateVertices().push_back(vtemp[iItrmdVtxId].vtx);
		    vtemp[iItrmdVtxId].vtx->setParent(t);
		}
		
		
	    } // Intermediate vertices

	    // Look in the particle table
	    // for particles from event generator
	    long iEventGeneratorLabel = (trackTable[itrk].eg_label) - 1;
	    if (iEventGeneratorLabel >=0 ) {

		// Now make sure that this track is really from the table,
		// When embedding, the particle got an eg_label = 99999 even
		// though there was only one entry in the particle table.
		if (iEventGeneratorLabel < NGeneratorTracks) {
		    // Track should already be loaded from the particle table
		    // i.e. t & ttempParticle[iEventGeneratorLabel] are the same tracks,
		    // obtained from different tables.
		    // We should rather keep the one in the g2t table, but we
		    // need to keep the information of its parentage.
		    nParticlesInBothTables++;
		    if (Debug()) {
			if (particleTable[iEventGeneratorLabel].jmohep[0] && !(ttempParticle[particleTable[iEventGeneratorLabel].jmohep[0]])) {
			    cout << "There should be a parent and there isn't one!\n";
			    PR(iEventGeneratorLabel);
			    PR(particleTable[iEventGeneratorLabel].jmohep[0]);
			    PR(ttempParticle[particleTable[iEventGeneratorLabel].jmohep[0]]);
			    
			}
		    }
		    if (particleTable[iEventGeneratorLabel].jmohep[0])
			t->setParent(ttempParticle[particleTable[iEventGeneratorLabel].jmohep[0]]);
		    StMcTrackIterator trkToErase = find (mCurrentMcEvent->tracks().begin(),
							 mCurrentMcEvent->tracks().end(),
							 ttempParticle[iEventGeneratorLabel]);
		    delete *trkToErase; // if we delete using the iterator, deleting should be done before erasing!
		    mCurrentMcEvent->tracks().erase(trkToErase);
		    
		    ttempParticle[iEventGeneratorLabel] = t;
		}
		
	    }
	    else {
		// Track from GEANT, use next_parent_p to get to the parent
		// track.  Use the same scheme as for the particle table.
		motherIndex = trackTable[itrk].next_parent_p;
		if ((motherIndex > 0) && (motherIndex < NTracks))
		    if (motherIndex > itrk) 
			gMessMgr->Warning()
			    << "Wrong ordering!  Track " << itrk+1 << "from particle table: "
			    << "Can't assign mother track " << motherIndex
			    << "because it has not been created yet!" << endm;
		    else t->setParent(ttemp[motherIndex-1]);
	    }
	    
	}} // Track loop
	{for (long gtrk=0; gtrk<NGeneratorTracks; gtrk++) {
	    // Find Mother...
	    motherIndex = particleTable[gtrk].jmohep[0];
	    if ((motherIndex > 0) && (motherIndex < NGeneratorTracks)) {
		if (motherIndex > gtrk) 
		    gMessMgr->Warning()
			<< "Wrong ordering!  Track " << gtrk+1 << " from particle table: "
			<< "Can't assign mother track " << motherIndex
			<< " to track with index " << gtrk << endm;
		else ttempParticle[gtrk]->setParent(ttempParticle[motherIndex]);
		if (Debug()) {
		    if (motherIndex && !(ttempParticle[gtrk]->parent())) {
			cout << "Error in assigning parent to particle table!\n There should be a parent and there isn't one!\n";
			PR(gtrk);
			PR(motherIndex);
			PR(particleTable[gtrk].jmohep[0]);
			PR(ttempParticle[gtrk]->parent());
		    }
		}
	    }
	}}
	if (nThrownTracks)
	    gMessMgr->Warning() << "StMcEventMaker::Make(): Throwing " << nThrownTracks
				<< " whose stop vertex is the same as primary vertex." << endm;
	if (Debug()) {
	    // Check the whole ttempParticle for entries with problems
	    for (long gtrk=0; gtrk<NGeneratorTracks; gtrk++)
		if (ttempParticle[gtrk]->parent() && ttempParticle[gtrk]->parent() != ttempParticle[particleTable[gtrk].jmohep[0]]) {
		    cout << "The indexing got screwed up!" << endl;
		    PR(ttempParticle[gtrk]->eventGenLabel());
		    PR(ttempParticle[gtrk]->key());
		    PR(ttempParticle[gtrk]->parent());
		    PR(ttempParticle[particleTable[gtrk].jmohep[0]]);
		}
	    cout << "Used   tracks from g2t_track table: " << usedTracksG2t << endl;
	    cout << "Avail. tracks from g2t_track table: " << NTracks       << endl;
	    cout << "Used   tracks from particle  table: " << usedTracksEvGen  << endl;
	    cout << "Avail. tracks from particle  table: " << NGeneratorTracks << endl;
	    cout << "Tracks appearing in both tables   : " << nParticlesInBothTables << endl;
	    cout << "Total tracks in StMcEvent         : " << mCurrentMcEvent->tracks().size() << endl;
	}
	vtemp.clear();
	ttempParticle.clear();
	
	//______________________________________________________________________
	// Step 4 - Fill Hits
		
	cout << "Preparing to process and fill HIT information ....." << endl;
	

	//
	// TPC Hits
	//
	if (doUseTpc) {
	    if (g2t_tpc_hitTablePointer) {
		StMcTpcHit* th = 0;
		long NHits = g2t_tpc_hitTablePointer->GetNRows();
		long iTrkId = 0;
		long nBadVolId = 0;
		long nPseudoPadrow = 0;
		long ihit;
		for(ihit=0; ihit<NHits; ihit++) {
		    if (tpcHitTable[ihit].volume_id < 101 || tpcHitTable[ihit].volume_id > 2445) {
			if (tpcHitTable[ihit].volume_id <= 202445 &&
			    tpcHitTable[ihit].volume_id > 2445) nPseudoPadrow++; 
			else nBadVolId++;
			continue;
		    }
		    
		    th = new StMcTpcHit(&tpcHitTable[ihit]);
		    
		    if(!mCurrentMcEvent->tpcHitCollection()->addHit(th)) {// adds hit th to collection
			nBadVolId++;
			delete th;
			th = 0;
			continue;
		    }
		    // point hit to its parent and add it to collection
		    // of the appropriate track
		    
		    iTrkId = (tpcHitTable[ihit].track_p) - 1;
		    
		    th->setParentTrack(ttemp[iTrkId]);
		    ttemp[iTrkId]->addTpcHit(th);
	    
		} // hit loop
		cout << "Filled " << mCurrentMcEvent->tpcHitCollection()->numberOfHits() << " TPC Hits" << endl;
		cout << "Found " << nPseudoPadrow << " Hits in Pseudo-Padrows." << endl;
		if (nBadVolId) {gMessMgr->Warning() << "StMcEventMaker::Make(): cannot store " << nBadVolId
						    << " TPC hits, wrong Volume Id." << endm;}
		// Sort the hits
		for (unsigned int iSector=0;
		      iSector<mCurrentMcEvent->tpcHitCollection()->numberOfSectors(); iSector++)
		    for (unsigned int iPadrow=0;
			  iPadrow<mCurrentMcEvent->tpcHitCollection()->sector(iSector)->numberOfPadrows();
			  iPadrow++) {
			StSPtrVecMcTpcHit& tpcHits = mCurrentMcEvent->tpcHitCollection()->sector(iSector)->padrow(iPadrow)->hits();
			sort (tpcHits.begin(), tpcHits.end(), compMcHit() );
			
		    }
	    } // pointer exists
	    else {
		cout << "No TPC Hits in this event" << endl;
	    }
	} // doUseTpc
	
	//
	// SVT Hits
	//
	if (doUseSvt) {
	if (g2t_svt_hitTablePointer) {
	    StMcSvtHit* sh = 0;
	    long NHits = g2t_svt_hitTablePointer->GetNRows();
	    long iTrkId = 0;
	    long nBadVolId = 0;
	    long ihit;
	    for(ihit=0; ihit<NHits; ihit++) {
		if (svtHitTable[ihit].volume_id < 1101 || svtHitTable[ihit].volume_id > 9000) {
		    nBadVolId++;
		    continue;
		}
		sh = new StMcSvtHit(&svtHitTable[ihit]);
		if (!mCurrentMcEvent->svtHitCollection()->addHit(sh)) {// adds hit sh to collection
		    nBadVolId++;
		    delete sh; // If the hit couldn't be assigned, delete it.
		    sh = 0;
		    continue;
		}
		
		// point hit to its parent and add it to collection
		// of the appropriate track
		
		iTrkId = (svtHitTable[ihit].track_p) - 1;
		sh->setParentTrack(ttemp[iTrkId]);
		ttemp[iTrkId]->addSvtHit(sh);
		
	    }
	    cout << "Filled " << mCurrentMcEvent->svtHitCollection()->numberOfHits() << " SVT Hits" << endl;
	    cout << "Nhits, " << NHits << " rows from table" << endl;
	    if (nBadVolId)
		gMessMgr->Warning() << "StMcEventMaker::Make(): cannot store " << nBadVolId
				    << " SVT hits, wrong Volume Id." << endm;
	    // Sort the hits
	    for (unsigned int iBarrel=0;
		 iBarrel<mCurrentMcEvent->svtHitCollection()->numberOfBarrels(); iBarrel++)
		for (unsigned int iLadder=0;
		     iLadder<mCurrentMcEvent->svtHitCollection()->barrel(iBarrel)->numberOfLadders();
		     iLadder++)
		    for (unsigned int iWafer=0;
			 iWafer<mCurrentMcEvent->svtHitCollection()->barrel(iBarrel)->ladder(iLadder)->numberOfWafers();
			 iWafer++) {
			StSPtrVecMcSvtHit& svtHits = mCurrentMcEvent->svtHitCollection()->barrel(iBarrel)->ladder(iLadder)->wafer(iWafer)->hits();
			sort (svtHits.begin(), svtHits.end(), compMcHit() );
	        
	    }

	}
	else {
	    cout << "No SVT Hits in this event" << endl;
	}
	} // do use svt
	
	//
	// FTPC Hits
	//
	if (doUseFtpc) {
	if (g2t_ftp_hitTablePointer) {
	    StMcFtpcHit* fh = 0;
	    long  NHits = g2t_ftp_hitTablePointer->GetNRows();
	    long  iTrkId = 0;
	    long  nBadVolId = 0;
	    long ihit;
	    for(ihit=0; ihit<NHits; ihit++) {
		if (ftpHitTable[ihit].volume_id < 101 || ftpHitTable[ihit].volume_id > 210) {
		    nBadVolId++;
		    continue;
		}

		fh = new StMcFtpcHit(&ftpHitTable[ihit]);

		if (!mCurrentMcEvent->ftpcHitCollection()->addHit(fh)){ // adds hit fh to collection
		    nBadVolId++;
		    delete fh;
		    fh = 0;
		    continue;
		}
		// point hit to its parent and add it to collection
		// of the appropriate track
		
		iTrkId = (ftpHitTable[ihit].track_p) - 1;
		fh->setParentTrack(ttemp[iTrkId]);
		ttemp[iTrkId]->addFtpcHit(fh);
		
	    }
	    cout << "Filled " << mCurrentMcEvent->ftpcHitCollection()->numberOfHits() << " FTPC Hits" << endl;
	    if (nBadVolId)
		gMessMgr->Warning() << "StMcEventMaker::Make(): cannot store " << nBadVolId
				    << " FTPC hits, wrong Volume Id." << endm;
	    // Sort the hits
	    for (unsigned int iPlane=0;
		 iPlane<mCurrentMcEvent->ftpcHitCollection()->numberOfPlanes(); iPlane++) {
		StSPtrVecMcFtpcHit& ftpcHits = mCurrentMcEvent->ftpcHitCollection()->plane(iPlane)->hits();
		sort (ftpcHits.begin(), ftpcHits.end(), compMcFtpcHit() );
	        
	    }
	}
	else {
	    cout << "No FTPC Hits in this event" << endl;
	}
	}// do use ftpc

	//
	// RICH Hits
	//
	if (doUseRich) {
	if (g2t_rch_hitTablePointer) {
	    StMcRichHit* rh = 0;
	    long  NHits = g2t_rch_hitTablePointer->GetNRows();
	    long  iTrkId = 0;
	    long ihit;
	    for(ihit=0; ihit<NHits; ihit++) {
	
		rh = new StMcRichHit(&rchHitTable[ihit]);
		mCurrentMcEvent->richHitCollection()->addHit(rh); // adds hit rh to collection
		
		// point hit to its parent and add it to collection
		// of the appropriate track
		
		iTrkId = (rchHitTable[ihit].track_p) - 1;
		rh->setParentTrack(ttemp[iTrkId]);
		ttemp[iTrkId]->addRichHit(rh);
		
	    }
	    cout << "Filled " << mCurrentMcEvent->richHitCollection()->numberOfHits() << " RICH Hits" << endl;
	    
	}
	else {
	    cout << "No RICH Hits in this event" << endl;
	}
	} // do use rich

	if (doUseCtb) {
	if (g2t_ctb_hitTablePointer) {
	    StMcCtbHit* ch = 0;
	    long  NHits = g2t_ctb_hitTablePointer->GetNRows();
	    long  iTrkId = 0;
	    long ihit;
	    for(ihit=0; ihit<NHits; ihit++) {
		
		ch = new StMcCtbHit(&ctbHitTable[ihit]);
		mCurrentMcEvent->ctbHitCollection()->addHit(ch); // adds hit ch to collection
		
		// point hit to its parent and add it to collection
		// of the appropriate track
		
		iTrkId = (ctbHitTable[ihit].track_p) - 1;
		ch->setParentTrack(ttemp[iTrkId]);
		ttemp[iTrkId]->addCtbHit(ch);
		
	    }
	    cout << "Filled " << mCurrentMcEvent->ctbHitCollection()->numberOfHits() << " Ctb Hits" << endl;
	    
	}
	else {
	    cout << "No Ctb Hits in this event" << endl;
	}
	}
	// TOFp hits
	long nTofpHits(0);
	if (doUseTofp) {
	    if (g2t_tof_hitTablePointer) {
		StMcTofHit* ch = 0;
		long  NHits = g2t_tof_hitTablePointer->GetNRows();
		long  iTrkId = 0;
		long ihit;
		
		for(ihit=0; ihit<NHits; ihit++) {
		    ch = new StMcTofHit(&tofHitTable[ihit]);
		    mCurrentMcEvent->tofHitCollection()->addHit(ch);
		    
		    // point hit to its parent and add it to collection
		    // of the appropriate track
		    
		    iTrkId = (tofHitTable[ihit].track_p) - 1;
		    ch->setParentTrack(ttemp[iTrkId]);
		    ttemp[iTrkId]->addTofHit(ch);
		}
		nTofpHits =  mCurrentMcEvent->tofHitCollection()->numberOfHits();
		cout << "Filled " <<nTofpHits << " TOFp Hits" << endl;
	    }
	    else {
		cout << "No TOFp Hits in this event" << endl;
	    }
        }
	
	// TOFr hits
        if (doUseTof) {
	    if (g2t_tfr_hitTablePointer) {
		StMcTofHit* ch = 0;
		long  NHits = g2t_tfr_hitTablePointer->GetNRows();
		long  iTrkId = 0;
		long ihit;
		
		for(ihit=0; ihit<NHits; ihit++) {
		    ch = new StMcTofHit(&tfrHitTable[ihit]);
		    mCurrentMcEvent->tofHitCollection()->addHit(ch);
		    
		    // point hit to its parent and add it to collection
		    // of the appropriate track
		    
		    iTrkId = (tfrHitTable[ihit].track_p) - 1;
		    ch->setParentTrack(ttemp[iTrkId]);
		    ttemp[iTrkId]->addTofHit(ch);
		}
		cout << "Filled " << mCurrentMcEvent->tofHitCollection()->numberOfHits() - nTofpHits
		     << " TOFr Hits" << endl;
	    }
	    else {
		cout << "No TOFr Hits in this event" << endl;
	    }
        }
	
	//
	// Pixel Hits
	//
	if (doUsePixel) {
	    if (g2t_pix_hitTablePointer) {    
		StMcPixelHit* fh = 0;
		long  NHits = g2t_pix_hitTablePointer->GetNRows();
		long  iTrkId = 0;
		long ihit;
		for(ihit=0; ihit<NHits; ihit++) {
		    //cout << ihit << " " << &pixHitTable[ihit] << endl;
		    fh = new StMcPixelHit(&pixHitTable[ihit]);
		    //cout << *fh << endl;
		    if (!mCurrentMcEvent->pixelHitCollection()->addHit(fh)){ 
			delete fh;
			fh = 0;
			continue;
		    }
		    // point hit to its parent and add it to collection
		    // of the appropriate track
		    iTrkId = (pixHitTable[ihit].track_p) - 1;
		    fh->setParentTrack(ttemp[iTrkId]);
		    //ttemp[iTrkId]->addPixelHit(fh);
		}
		cout << "Filled " << mCurrentMcEvent->pixelHitCollection()->numberOfHits() << " Pixel Hits" << endl;
	    }
	    else {
		cout << "No Pixel Hits in this event" << endl;
	    }
	}// do use pixel
	
	
	// BEMC and BPRS Hits
	if (doUseBemc) fillBemc(g2t_emc_hitTablePointer);

	// BSMDE and BSMDP Hits
	if (doUseBsmd) fillBsmd(g2t_smd_hitTablePointer);
	
	ttemp.clear();
	
	//_______________________________________________________________
	// At this point StMcEvent should be loaded.
		
    }
    
    
    if (!g2t_vertexTablePointer)
	gMessMgr->Warning() << "StMcEventMaker:   g2t_vertex  Table Not found " << endm;
    if (!g2t_trackTablePointer)
	gMessMgr->Warning() << "StMcEventMaker:   g2t_track   Table Not found " << endm;
    if (!g2t_tpc_hitTablePointer)
	gMessMgr->Info()    << "StMcEventMaker:   g2t_tpc_hit Table Not found " << endm;
    if (!particleTablePointer)
	gMessMgr->Info()    << "StMcEventMaker:   particle    Table Not found " << endm;
    
    if (doPrintEventInfo) printEventInfo();
    if (doPrintMemoryInfo) {
	StMemoryInfo::instance()->snapshot();
	StMemoryInfo::instance()->print();
    }
    if (doPrintCpuInfo) {
	timer.stop();
	cout << "CPU time for StMcEventMaker::Make(): "
	     << timer.elapsedTime() << " sec\n" << endl;
    }
  
  return kStOK;

}

void
StMcEventMaker::fillBemc(St_g2t_emc_hit* g2t_emc_hitTablePointer)
{
    if (g2t_emc_hitTablePointer == 0) {
        cout << "No BEMC and BPRS Hits in this event" << endl;
        return;
    }

    g2t_emc_hit_st* emcHitTable = g2t_emc_hitTablePointer->GetTable();
    StEmcGeom *geomBemc = StEmcGeom::getEmcGeom(1);
    int module, eta, sub, detector; 
    float de;
    StMcTrack *tr; 
    StMcCalorimeterHit *emchBemc, *emchBprs;
	
    StMcEmcHitCollection *bemcColl=mCurrentMcEvent->bemcHitCollection();
    m_DataSet->Add(bemcColl);
    bemcColl->SetName("BemcHits");
    StMcEmcHitCollection  *bprsColl=mCurrentMcEvent->bprsHitCollection();
    m_DataSet->Add(bprsColl);
    bprsColl->SetName("BprsHits");
		
    long NHits = g2t_emc_hitTablePointer->GetNRows();
		
    for(long ihit=0; ihit<NHits; ihit++,emcHitTable++) { 

	geomBemc->getVolIdBemc(emcHitTable->volume_id, module,eta,sub,detector); // Must check ??
// 	cout << "hit       " << ihit << endl;
// 	cout << "volume id " << emcHitTable->volume_id << endl;
// 	cout << "module    " << module   << endl;
// 	cout << "eta       " << eta      << endl;
// 	cout << "sub       " << sub      << endl;
// 	cout << "detector  " << detector << endl;
	tr   = ttemp[emcHitTable->track_p - 1];
	de   = emcHitTable->de;
		    
	if (detector == 1 || detector == 2) {
	    emchBemc = new StMcCalorimeterHit(module,eta,sub,de, tr);
            emchBprs = 0;                                                      // For safety
	    StMcEmcHitCollection::EAddHit bemcNew = bemcColl->addHit(emchBemc);

	    if (bemcNew == StMcEmcHitCollection::kNew){ 
		tr->addBemcHit(emchBemc);
		if(detector == 2) emchBprs = new StMcCalorimeterHit(module,eta,sub,de, tr);
	    }
	    else if(bemcNew == StMcEmcHitCollection::kAdd){ 
		emchBprs = emchBemc;
            }
	    else if(bemcNew == StMcEmcHitCollection::kErr){ 
		delete emchBemc;
                emchBemc = 0;
		gMessMgr->Warning()<<"<E> Bad hit in Bemc collection " << endm;
	    }
	    else { // Unused branch
		delete emchBemc;
                emchBemc = 0;
		emchBprs = 0;
		gMessMgr->Warning()<<"<E> Funny return value! EAddHit = " << static_cast<int>(bemcNew) <<endm;
	    }

	    if (detector == 2 && emchBprs) {
		StMcEmcHitCollection::EAddHit bprsNew = bprsColl->addHit(emchBprs);
		if (bprsNew == StMcEmcHitCollection::kNew) {
                    tr->addBprsHit(emchBprs);
                }
		else { 
		    delete emchBprs;
		    emchBprs = 0;
		}
	    }
	    else if(emchBprs) {
		 delete emchBprs;
		 emchBprs = 0;
	    }
        }
        else {
	    gMessMgr->Warning() << "<E> Bad EMC detector number " << detector << " volume_id "
                                << emcHitTable->volume_id << " detector "<< detector << endm;
        }
    }
    cout << "Filled " << mCurrentMcEvent->bemcHitCollection()->numberOfHits() << " BEMC Hits" << endl;
    cout << "Filled " << mCurrentMcEvent->bprsHitCollection()->numberOfHits() << " BPRS Hits" << endl;
		
}

void
StMcEventMaker::fillBsmd(St_g2t_emc_hit* g2t_smd_hitTablePointer)
{
    if (g2t_smd_hitTablePointer == 0) {
        cout << "No BSMDE and BSMDP Hits in this event" << endl;
        return;
    }

    g2t_emc_hit_st* smdHitTable = g2t_smd_hitTablePointer->GetTable();
    StEmcGeom *geomBsmd = StEmcGeom::getEmcGeom(3);
    int module, eta, sub, detector; 
    float de;
    StMcTrack *tr; 
    StMcCalorimeterHit *emchBsmde, *emchBsmdp;

    StMcEmcHitCollection *bsmdeColl=mCurrentMcEvent->bsmdeHitCollection();
    m_DataSet->Add(bsmdeColl);
    bsmdeColl->SetName("BsmdeHits");
    StMcEmcHitCollection *bsmdpColl=mCurrentMcEvent->bsmdpHitCollection();
    m_DataSet->Add(bsmdpColl);
    bsmdpColl->SetName("BsmdpHits");

    long NHits = g2t_smd_hitTablePointer->GetNRows();
    for(long ihit=0; ihit<NHits; ihit++,smdHitTable++) { 
	geomBsmd->getVolIdBsmd(smdHitTable->volume_id, module,eta,sub,detector); // Must check ??
	tr   = ttemp[smdHitTable->track_p - 1];
	de   = smdHitTable->de;

        if (detector == 3) {
            emchBsmde = new StMcCalorimeterHit(module,eta,sub,de, tr);
	    StMcEmcHitCollection::EAddHit bsmdeNew = bsmdeColl->addHit(emchBsmde);
            if (bsmdeNew == StMcEmcHitCollection::kNew) {
                tr->addBsmdeHit(emchBsmde);
            }
	    else {
                delete emchBsmde;
                emchBsmde = 0;
            } 
        }
        else if (detector == 4) {
            emchBsmdp = new StMcCalorimeterHit(module,eta,sub,de, tr);
	    StMcEmcHitCollection::EAddHit bsmdpNew = bsmdpColl->addHit(emchBsmdp);
            if (bsmdpNew == StMcEmcHitCollection::kNew) {
                tr->addBsmdpHit(emchBsmdp);
            }
	    else {
                delete emchBsmdp;
                emchBsmdp = 0;
            } 
        }
        else {
	    gMessMgr->Warning() << "<E> Bad SMD detector number " << detector << " volume_id "
				<< smdHitTable->volume_id << " detector "<< detector << endm;
        }
    }
    cout << "Filled " << mCurrentMcEvent->bsmdeHitCollection()->numberOfHits() << " BSMDE Hits" << endl;
    cout << "Filled " << mCurrentMcEvent->bsmdpHitCollection()->numberOfHits() << " BSMDP Hits" << endl;

}

void
StMcEventMaker::printEventInfo()
{
    cout << "*********************************************************" << endl;
    cout << "*                  StMcEvent Information                *" << endl;
    cout << "*********************************************************" << endl;

    cout << "---------------------------------------------------------" << endl;
    cout << "StMcEvent at " << (void*) mCurrentMcEvent                  << endl;
    cout << "---------------------------------------------------------" << endl;
    if (mCurrentMcEvent)
	cout << *mCurrentMcEvent << endl;
    else
	return;

    cout << "---------------------------------------------------------" << endl;
    cout << "StSPtrVecMcTrack"                                          << endl;
    cout << "Dumping first element in collection only (if available). " << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "collection size = " << mCurrentMcEvent->tracks().size()    << endl;
    if (mCurrentMcEvent->tracks().size()) {
	cout << "---------------------------------------------------------" << endl;
	cout << "StMcTrack at "
	     << (void*) mCurrentMcEvent->tracks()[0]                        << endl;
	cout << "---------------------------------------------------------" << endl;
	cout << *(mCurrentMcEvent->tracks()[0])                             << endl;
    }

    cout << "---------------------------------------------------------" << endl;
    cout << "StMcVertex"                                                << endl;
    cout << "Dumping vertex info and first daughter track.            " << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "Primary Vertex at "
	 << (void*) mCurrentMcEvent->primaryVertex()                    << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << *(mCurrentMcEvent->primaryVertex())                         << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "Daughters of Primary Vertex : "
	 << mCurrentMcEvent->primaryVertex()->numberOfDaughters() << endl;
    if (mCurrentMcEvent->primaryVertex()->numberOfDaughters()) {
	cout << "First Daughter of Primary Vertex" << endl;
	cout << *(mCurrentMcEvent->primaryVertex()->daughter(0));
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "StSPtrVecMcVertex"                                         << endl;
    cout << "# of Vertices    : " << mCurrentMcEvent->vertices().size() << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "Dumping second element in collection (First is Primary). " << endl;
    if (mCurrentMcEvent->vertices().size()>1) {
	cout << "---------------------------------------------------------" << endl;
	cout << "Second StMcVertex at "
	     << (void*) mCurrentMcEvent->vertices()[1]                      << endl;
	cout << "---------------------------------------------------------" << endl;
	cout << *(mCurrentMcEvent->vertices()[1])                           << endl;
	cout << "---------------------------------------------------------" << endl;
	cout << "Daughters of second Vertex : "
	     << mCurrentMcEvent->vertices()[1]->numberOfDaughters() << endl;
	if (mCurrentMcEvent->vertices()[1]->numberOfDaughters()) {
	    cout << "First Daughter of this Vertex" << endl;
	    cout << *(mCurrentMcEvent->vertices()[1]->daughter(0));
	}
	
    }

    
    unsigned int       i, j, k, nhits;
    Bool_t             gotOneHit;
    StMcTpcHitCollection *tpcColl = mCurrentMcEvent->tpcHitCollection();
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcTpcHitCollection at " << (void*) tpcColl               << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    nhits = tpcColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;
    if (tpcColl && nhits) {
	
	
	gotOneHit = kFALSE;
	for (k=0; !gotOneHit && k<tpcColl->numberOfSectors(); k++)
	    for (j=0; !gotOneHit && j<tpcColl->sector(k)->numberOfPadrows(); j++)
		if (tpcColl->sector(k)->padrow(j)->hits().size()) {
		    cout << "Tpc Hit" << endl;
		    cout << *(tpcColl->sector(k)->padrow(j)->hits()[0]);
		    cout << "Parent track of this Hit" << endl;
		    cout << *(tpcColl->sector(k)->padrow(j)->hits()[0]->parentTrack()) << endl;
		    gotOneHit = kTRUE;
		    cout << "Dumping all the z coordinates in this padrow" << endl;
		    cout << "Should be sorted according to z: " << endl;
		    cout << "---------------------------------------------------------" << endl;
    		    for (StMcTpcHitIterator thi = tpcColl->sector(k)->padrow(j)->hits().begin();
			 thi!=tpcColl->sector(k)->padrow(j)->hits().end(); thi++)
			cout << (*thi)->position().z() << " ";
		    cout << endl;
		}
    }
    
    StMcFtpcHitCollection *ftpcColl = mCurrentMcEvent->ftpcHitCollection();
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcFtpcHitCollection at " << (void*) ftpcColl             << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    nhits = ftpcColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;
    if (ftpcColl &&  nhits) {
	
	
	gotOneHit = kFALSE;
	for (k=0; !gotOneHit && k<ftpcColl->numberOfPlanes(); k++)
	    if (ftpcColl->plane(k)->hits().size()) {
		cout << "Ftpc Hit" << endl;
		cout << *(ftpcColl->plane(k)->hits()[0]);
		cout << "Parent track of this Hit" << endl;
		cout << *(ftpcColl->plane(k)->hits()[0]->parentTrack()) << endl;
		gotOneHit = kTRUE;
	    }
    }
    
    StMcRichHitCollection *richColl = mCurrentMcEvent->richHitCollection();
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcRichHitCollection at " << (void*) richColl             << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    nhits = richColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;
    if (richColl &&  nhits) {
	
	if (richColl->hits().size()) {
	    cout << "Rich Hit" << endl;
	    cout << *(richColl->hits()[0]);
	    cout << "Parent track of this Hit" << endl;
	    cout << *(richColl->hits()[0]->parentTrack()) << endl;
		
	}
    }
    
    StMcSvtHitCollection *svtColl = mCurrentMcEvent->svtHitCollection();
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcSvtHitCollection at " << (void*) svtColl               << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    nhits = svtColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;

    if (svtColl && nhits) {
	
	gotOneHit = kFALSE;
	for (k=0; !gotOneHit && k<svtColl->numberOfBarrels(); k++)
	    for (j=0; !gotOneHit && j<svtColl->barrel(k)->numberOfLadders(); j++)
		for (i=0; !gotOneHit && i<svtColl->barrel(k)->ladder(j)->numberOfWafers(); i++)
		    if (svtColl->barrel(k)->ladder(j)->wafer(i)->hits().size()) {
			cout << "Svt Hit" << endl;
			cout << *(svtColl->barrel(k)->ladder(j)->wafer(i)->hits()[0]);
			cout << "Parent track of this Hit" << endl;
			cout << *(svtColl->barrel(k)->ladder(j)->wafer(i)->hits()[0]->parentTrack()) << endl;
			gotOneHit = kTRUE;
		    }
    }
    StMcTofHitCollection *tofColl = mCurrentMcEvent->tofHitCollection();
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcTofHitCollection at " << (void*) tofColl             << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    nhits = tofColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;
    if (tofColl &&  nhits) {
	
	if (tofColl->hits().size()) {
	    cout << "Tof Hit" << endl;
	    cout << *(tofColl->hits()[0]);
	    cout << "Parent track of this Hit" << endl;
	    cout << *(tofColl->hits()[0]->parentTrack()) << endl;
	    
	}
    }
    StMcPixelHitCollection *pixelColl = mCurrentMcEvent->pixelHitCollection();
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcPixelHitCollection at " << (void*) pixelColl             << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    nhits = pixelColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;
    if (pixelColl &&  nhits) {
	gotOneHit = kFALSE;
	for (k=0; !gotOneHit && k<pixelColl->numberOfLayers(); k++)
	    if (pixelColl->layer(k)->hits().size()) {
		cout << "Pixel Hit" << endl;
		cout << *(pixelColl->layer(k)->hits()[0]);
		cout << "Parent track of this Hit" << endl;
		cout << *(pixelColl->layer(k)->hits()[0]->parentTrack()) << endl;
		gotOneHit = kTRUE;
	    }
	
    }

    cout << endl;

    printEventInfoForEmc(mCurrentMcEvent->bemcHitCollection());
    printEventInfoForEmc(mCurrentMcEvent->bprsHitCollection());
    printEventInfoForEmc(mCurrentMcEvent->bsmdeHitCollection());
    printEventInfoForEmc(mCurrentMcEvent->bsmdpHitCollection());
    
}  

void
StMcEventMaker::printEventInfoForEmc(StMcEmcHitCollection *emcColl)
{
    bool gotOneHit;
    unsigned int i, m;
    
    if(emcColl==0){
	cout<<"<W> In StMcEventMaker::printEventInfoForEmc emcColl undefined" << endl;
	return;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "StMcEmcHitCollection at " << (void*) emcColl;
    cout << "  name "<<emcColl->GetName() << endl;
    cout << "Dumping collection size and one hit only."                 << endl;
    cout << "---------------------------------------------------------" << endl;
    int nhits = emcColl->numberOfHits();
    cout << "# of hits in collection = " << nhits << endl;
    
    if (emcColl && nhits) {
	gotOneHit = kFALSE;
	for (i=0; !gotOneHit && i<emcColl->numberOfModules(); i++){
	  m = i + 1; // module number
	    if (emcColl->module(m)->hits().size()){
		cout << *(emcColl->module(m)->hits()[0]);
		cout << "Parent track of this Hit" << endl;
		cout << *(emcColl->module(m)->hits()[0]->parentTrack()) << endl;
		gotOneHit = kTRUE;
	    }
	}
	cout << endl;
    }
}

void
StMcEventMaker::printEventInfoForEmcDet(unsigned int det)
{
  StMcEmcHitCollection* emcCol =  mCurrentMcEvent->bemcHitCollection();

  switch (det){
  case 1: emcCol = mCurrentMcEvent->bemcHitCollection();  break;
  case 2: emcCol = mCurrentMcEvent->bprsHitCollection();  break;
  case 3: emcCol = mCurrentMcEvent->bsmdeHitCollection(); break;
  case 4: emcCol = mCurrentMcEvent->bsmdpHitCollection(); break;
  }

  printEventInfoForEmc(emcCol);
}   
    
