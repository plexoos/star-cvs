/***************************************************************************
 *
 * $Id: writeTrks.cc,v 1.4 2000/07/12 15:38:33 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             write StPidAmpTrks to the disk 
 *             for quick tune purpose
 ***************************************************************************
 *
 * $Log: writeTrks.cc,v $
 * Revision 1.4  2000/07/12 15:38:33  aihong
 * update for real data
 *
 * Revision 1.3  2000/04/12 20:14:29  aihong
 * change to adapt to ROOT 2.24 and bug fixed with help from valery
 *
 * Revision 1.2  2000/04/09 18:51:20  aihong
 * change Make() to read directly from dst tables instead of StEvent
 *
 * Revision 1.1.1.1  2000/03/09 17:48:34  aihong
 * Installation of package
 *
 **************************************************************************/


#include <fstream.h>
#include "float.h"

#include "StHelixModel.h"
#include "StPhysicalHelixD.hh" 
#include "StPrimaryVertex.h"
#include "StEnumerations.h"


#include "tables/St_dst_track_Table.h"
#include "tables/St_dst_dedx_Table.h"
#include "tables/St_dst_vertex_Table.h"



void writeTrks(St_dst_track* theTrackTable, St_dst_dedx* theDedxTable, St_dst_vertex* theVertexTable,Int_t dedxMethod){

  ofstream foutD;

  foutD.open("80evtsTrks.dat", ios::app);

  if (!foutD) {cout<<"Cannot open file.\n"; }//handle error

   double dedx, rig, pt, dca;
   int    nhits,charge;
   int    i=0;

 
   dst_track_st*  globtrk_v =  theTrackTable->GetTable();
   dst_dedx_st*   dedx_v    =   theDedxTable->GetTable();
   dst_vertex_st* vertex_v  = theVertexTable->GetTable();
 
 
 //now get primary vetex
  StPrimaryVertex *pvtx=0;
 Int_t nVertexRows = theVertexTable->GetNRows();

 for (i=0; i<nVertexRows;i++)
 if (vertex_v[i].iflag < 100 && vertex_v[i].iflag%10 == 1 &&
            vertex_v[i].vtx_id == kEventVtxId) 
 { pvtx = new StPrimaryVertex(vertex_v[i]); break; }

 if (pvtx){


 Int_t nDedxRows    = theDedxTable->GetNRows();
 Int_t nGlobTrkRows = theTrackTable->GetNRows();
   
 for (i=0; i<nDedxRows; i++){ //dedx loop

   if (dedx_v[i].det_id !=kTpcId ) continue;//not from tpc dector.
   if (dedx_v[i].method !=dedxMethod ) continue; //not from truncated Mn.
   

   dedx=dedx_v[i].dedx[0];
   if (!dedx) continue;  //bad assinment.  

   nhits=dedx_v[i].ndedx;


   if (dedx_v[i].id_track < nGlobTrkRows){//global track bound check

 
  
  StHelixModel theHelixModel(globtrk_v[dedx_v[i].id_track-1]);

  charge=theHelixModel.charge();

   const  StPhysicalHelixD& thePhysicalHelix=theHelixModel.helix();

   const  StThreeVectorF& p=theHelixModel.momentum();
   pt=double(p.perp()); 




   rig=double(p.mag()/double(charge));

   if (!rig) continue;
  
   dca=thePhysicalHelix.distance(pvtx->position());
   
   if ( rig && dedx && charge && 
	(fabs(pt)<FLT_MAX) && (float(nhits)<FLT_MAX) && (fabs(rig)<FLT_MAX)
       && (fabs(dca)<FLT_MAX) )
   
 foutD<<rig<<" "<<dedx<<" "<<charge<<" "<<pt<<" "<<nhits<<" "<<dca<<endl;
   

   }


 }

    delete pvtx;

 }
   foutD.close();

}
