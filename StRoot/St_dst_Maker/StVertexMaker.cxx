/*!
 *
 * StVertexMaker class ( est + evr/lmv/pplmv )
 * Modes: 0 = lmv/evr
 *        1 = ppLMV
 *        2 = lmv/evr with VtxOffSet
 *        3 = lmv evr
 *        4 = pplmv using EST if around
 *        5 = lmv/evr with VtxOffset
 *        8 = pplmv using pplmv5 cuts
 *
 * 0, 2, 3, 5 can be treated together (lmv)
 * 1, 4, 8    are ppLMV related
 * 
 */


#include <Stiostream.h>
#include <stdlib.h>
#include <math.h>
#include "StVertexMaker.h"

#include "math_constants.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StVertexId.h"

#include "St_DataSet.h"
#include "St_DataSetIter.h"

#include "StMessMgr.h"

#include "global/St_evr_am_Module.h"
#include "St_db_Maker/St_db_Maker.h"
#include "TH2.h"

#include "CtbResponse.h" // for recon of pileup in pp
#include "MatchedTrk.h" // for recon of pileup in pp

#include "tables/St_vertexSeed_Table.h"

#define NSWITCH 30    /// lmv to evr switch value

long lmv(St_dst_track *track, St_dst_vertex *vertex, Int_t mdate);
int curEvNum=-1;


ClassImp(StVertexMaker)
//_____________________________________________________________________________
  StVertexMaker::StVertexMaker(const char *name, int key):StMaker(name),
  m_evr_evrpar(0),
  m_fixedVertex(0),
  m_fixedArrayX(0),
  m_fixedArrayY(0),
  m_fixedArrayZ(0),
  m_fixedArrayR(0),
  m_fixedArrayE(0)
{
  m_Mode = key;
  zCutppLMV=0; // turn off ppLMV  as default
  UnSetBeam4ppLMV(); // turn off adding beam line to ppLMV  as default (1)
  embedVerts = kFALSE;
  ppLMVparI[1]=0;
  this->SetCTBMode(0);
}

//_____________________________________________________________________________
StVertexMaker::~StVertexMaker(){
  UnFixVertex();
}
//_____________________________________________________________________________

/// m_Mode is used for selecting the vertex finding method. 
Int_t StVertexMaker::Init(){

  switch(m_Mode) { // lmv/evr or ppLMV

  case 0:   // initialize lmv/evr
  case 2:  // initialize lmv/evr with VtxOffSet
  case 3:  // initialize lmv/evr using EST
  case 5: { // initialize lmv/evr with VtxOffSet using EST
    // Create evr table
    m_evr_evrpar = new St_evr_evrpar("evr_evrpar",1);
    evr_evrpar_st row;
    //
    memset(&row,0,sizeof(row));
    if ((m_Mode & 2) || (m_Mode ==5) ) row.fitoption = 2; // For Y2K real data set evrpar.fitoption = 2
    else row.fitoption = 0;
    row.vcut	 =          3; // distance below where track is marked as default primary ;
    row.cut2	 =          2; // select tracks for 2nd vertex fit ;
    row.cut3	 =        0.5; // select tracks for 3rd vertex fit ;
    row.cutxy	 =          1; // select tracks for vertex fitting ;
    row.cutz	 =         10; // select tracks for vertex fitting ;
    row.ptmin	 =          0; // minimum pt of individual tracks ;
    m_evr_evrpar->AddAt(&row,0);
    AddRunCont(m_evr_evrpar);
    break; }
  case 1:  // initialize ppLMV
  case 4: 
  case 8:  // ppLMV5
    { // initialize ppLMV using EST
    int   ppLMVparIdef[10]={2, 10, 0, 1, 0, 0, 0, 0, 0, 9999};
    float ppLMVparFdef[10]={1., 3.9, 0.20, .02, 1.,180., 0, 0, 0, 8888};
    if(m_Mode==8) {
      gMessMgr->Info() << GetName() <<" Init() activate  ppLMV5 cuts "<<endm;
      ppLMVparFdef[1]=2.0;  // MaxTrkDcaRxy
      ppLMVparFdef[4]=0.;   // CtbPhiErr/deg
      ppLMVparFdef[5]=150.; // MaxTrkDcaZ
      ppLMVparIdef[1]=15;   // MinTrkPonits
      ppLMVparIdef[2]=20;   // beamEequivNtr
      ppLMVparIdef[3]=2;    // MinMatchTr
    } else {
     gMessMgr->Info() << GetName() <<" Init() activate  ppLMV4 cuts, m_Mode= "<<m_Mode<<endm;
    }
    ppLMVuse(ppLMVparIdef,ppLMVparFdef);

    // few histos for monitoring of ppLMV
    hPiFi[0] = new TH1F("Fin0","ADC spectrum for  CTB=trigBXing slats",257,-0.5,256.5);
    hPiFi[1] = new TH1F("Fin1","No. of CTB=trigBXing slats/eve",261,-0.5,260.5);
    hPiFi[2] = (TH1F *) new TH2F("Fi-m","Match Tr @ CTB  phi/deg vs. eta",15,-1.5,1.5,18,0.,360.);
    hPiFi[3] = new TH1F("Fin3","Match #Delta #eta",50,-1.,1.);
    hPiFi[4] = new TH1F("Fin4","Match #Delta #phi/deg",50,-10.,10.);
    hPiFi[5] = new TH1F("Fin5","No. of track match to CTB=trigBXing tracks/eve",261,-0.5,260.5);
    hPiFi[6] =(TH1F *) new TH2F("Fin6","Vertex Y/cm vs. X/cm found",25,-5,5,25,-5,5);

    hPiFi[7] = new TH1F("Fin7","Vertex X/cm found",100,-5,5);
    hPiFi[8] = new TH1F("Fin8","Vertex Y/cm found",100,-5,5);
    hPiFi[9] = new TH1F("Fin9","Vertex Z/cm found",100,-250,250);

    hPiFi[10] = new TH1F("Fin10","Vertex Z/cm Geant-found/cm ",100,-5,5);
    hPiFi[11] = new TH1F("Fin11","Vertex X/cm Geant-found/cm ",100,-5,5);
    hPiFi[12] = new TH1F("Fin12","Vertex Y/cm Geant-found/cm ",100,-5,5);

    hPiFi[13] = new TH1F("Fin13","Primary multiplicity",551,-0.5,550.5);
    hPiFi[14] = new TH1F("Fin14","Primary (global) pT distrib",100,0.,10.);
    hPiFi[15] = new TH1F("Fin15","Primary No. of points/track",51,-0.5,50.5);
    hPiFi[16] =(TH1F *) new TH2F("vXZ","Vertex X/cm vs. Z/cm found",50,-250,250,30,-1.5,1.5);
    hPiFi[17] =(TH1F *) new TH2F("vYZ","Vertex Y/cm vs. Z/cm found",50,-250,250,30,-1.5,1.5);

    //temp schizophrenia JB
    hPiFi[18] = new TH1F("Fin18","Vertex X/cm found, no BeamLine",100,-5,5);
    hPiFi[19] = new TH1F("Fin19","Vertex Y/cm found, no BeamLine",100,-5,5);
    hPiFi[20] = new TH1F("Fin20","Vertex Z/cm found, no BeamLine",100,-250,250);

    hPiFi[21] = new TH1F("Fin21","Vertex Z/cm found-noBeamLine",200,-5,5);
    hPiFi[22] =(TH1F *) new TH2F("Fin22","Vertex X/cm vs. Z/cm -noBeamLine",50,-250,250,30,-1.5,1.5);
    //temp schizophrenia JB

    // matching to many bXing
    hctb[0] = new TH1F("Gctb0","Geant TOF of CTB hit (ns)",200,-8000.,8000.);
    hctb[1] = new TH1F("Gctb1","Geant TOF of CTB hit (ns)",200,0.,200.);
    hctb[2] = new TH1F("Gctb2","Geant TOF of CTB hit (ns), trig only",200,0.,200.);
    hctb[3] = new TH1F("Gctb3","Geant TOF of CTB hit (ns)",200,-400.,400.);
    hctb[4] = new TH1F("Gctb4","Geant CTB hit vs. bXingID",61,-0.5,60.5);
    hctb[5] = new TH1F("Dctb1","CTB slat with ADC>Th",241,-0.5,240.5);
    hctb[6] = new TH1F("Dctb2","CTB slat with ADC>Th matched to track",241,-0.5,240.5);

    hmtr[0] = new TH1F("mtr0","counts(1,...6)",11,-0.5,10.5);
    hmtr[1] = new TH1F("mtr1","starggling (a.u.) of tracks",100,.0,10.);
    hmtr[2] = new TH1F("mtr2","Spath (cm) of tracks",100,.0,250.);
    break; }
  default:
    gMessMgr->Error() << "StVertexMaker: unknown vertex switch =" << m_Mode << endm;
    return kStErr;
  } // end of lmv/evr,ppLMV switch

  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StVertexMaker::Make(){
  PrintInfo();

  St_DataSet *match = GetDataSet("match");
  St_DataSetIter matchI(match);
  St_dst_track   *EstGlobal = 0;
  EstGlobal = (St_dst_track *) matchI("EstGlobal");

  St_dst_track   *globtrk = (St_dst_track *) matchI("globtrk");
  if (! globtrk) {globtrk = new St_dst_track("globtrk",1); AddGarb(globtrk);}

  St_dst_vertex  *vertex  = new St_dst_vertex("vertex", 4);
  StMaker* vertexDataSet;
  if (! (vertexDataSet = GetMaker("primary"))) vertexDataSet = this;
  vertexDataSet->AddData(vertex);

  St_db_Maker *db = ( St_db_Maker *)GetMaker("db");
  Int_t mdate = db->GetDateTime().GetDate();

  int iRes=0;
  St_dst_vertex  *preVertex=0;

  preVertex = (St_dst_vertex *)GetDataSet("preVertex/.data/preVertex");
  if( !preVertex)
    preVertex = (St_dst_vertex *)GetDataSet("SvtVtx/.data/preVertex");
  St_dst_vertex  *clusterVertex = (St_dst_vertex *)GetDataSet("tpc_tracks/.data/clusterVertex");
  if( preVertex ) {
    Int_t numRowPreVertex = preVertex->GetNRows();
    vertex->ReAllocate( numRowPreVertex+4 );
    Int_t sizeToCopy = sizeof(dst_vertex_st) * numRowPreVertex;
    memcpy(vertex->GetTable(), preVertex->GetTable(), sizeToCopy);
    vertex->SetNRows( numRowPreVertex );
  } else if ( clusterVertex ) {
    Int_t numRowClusterVertex = clusterVertex->GetNRows();
    vertex->ReAllocate( numRowClusterVertex+4 );
    Int_t sizeToCopy = sizeof(dst_vertex_st) *numRowClusterVertex ;
    memcpy(vertex->GetTable(), clusterVertex->GetTable(), sizeToCopy);
    vertex->SetNRows( numRowClusterVertex );
  }

  switch(m_Mode) { // lmv/evr or ppLMV

  case 0:   // lmv/evr
  case 2:  // lmv/evr with VtxOffSet
  case 3:  //lmv/evr using EST
  case 5: { //lmv/evr with VtxOffSet using EST
    long NGlbTrk = 0;
    if (globtrk) NGlbTrk = globtrk->GetNRows();

    // Using file of fixed vertices
    if (GetMatchedSize()) {                  // Match event/run IDs
      if (FixVertex(GetRunNumber(),GetEventNumber())) return kStErr;
    } else if (GetFixedSize()) {
      if (FixVertex(GetNumber()-1)) return kStErr;
    }

    if (m_fixedVertex) {  // Fixed primary vertex
      gMessMgr->Warning("StVertexMaker: --------- WARNING!!! ---------","E-");
      gMessMgr->Warning() << "StVertexMaker: Fixing the primary vertex at (" <<
        m_fixedVertex->x << "," <<
        m_fixedVertex->y << "," <<
        m_fixedVertex->z << ")" << endm;
      gMessMgr->Warning("StVertexMaker: --------- WARNING!!! ---------","E-");
      // evr with fixed vertex
      vertex->AddAt(m_fixedVertex,0);
      if (NGlbTrk < 1) {
        gMessMgr->Warning("Cannot call evr with <1 tracks");
        iRes = kStErr;
      } else {
        if(Debug()) gMessMgr->Debug() <<
            "run_evr: calling evr_am with fixed vertex" << endm;
	if( EstGlobal && (m_Mode == 3 || m_Mode == 5))
	  iRes = evr_am(m_evr_evrpar,EstGlobal,vertex);
	else
	  iRes = evr_am(m_evr_evrpar,globtrk,vertex);
      }
    } else {  // Primary vertex is not fixed, find it
      // Switch to Low Multiplicity Primary Vertex Finder for multiplicities < NSWITCH
      if( NGlbTrk < NSWITCH ){

        // lmv
        if(Debug()) gMessMgr->Debug("run_lmv: calling lmv");
	if( EstGlobal && (m_Mode == 3 || m_Mode == 5) ) iRes = lmv(EstGlobal,vertex,mdate);
	else
	  iRes = lmv(globtrk,vertex,mdate);

      } else {

        // evr
        if(Debug()) gMessMgr->Debug("run_evr: calling evr_am");
	if( EstGlobal && (m_Mode == 3 || m_Mode == 5) )
	  iRes = evr_am(m_evr_evrpar,EstGlobal,vertex);
	else
	  iRes = evr_am(m_evr_evrpar,globtrk,vertex);
      }

    }  // end section on finding a primary vertex if not fixed


    break; }


  case 1:  // ppLMV
  case 8:  // ppLMV5
  case 4:{ //pplmv using EST
    gMessMgr->Info() << GetName() <<
      "-maker will use ppLMV with zCutppLMV=" << zCutppLMV << "/cm" << endm;

    if (! (ppLMVparI[1]>0)) {
      gMessMgr->Error("StVertexMaker: ppLMV not initialized!");
      return kStErr;
    }

    gMessMgr->Info() << GetName() << "CTB Mode = " << this->GetCTBMode() << endm;
    CtbResponse ctbResponse(this, ppLMVparI, ppLMVparF,this->GetCTBMode());


    MatchedTrk maEstTrk(this, ppLMVparI, ppLMVparF, &ctbResponse, EstGlobal) ;
    MatchedTrk maTrk(this, ppLMVparI, ppLMVparF, &ctbResponse, globtrk) ;

    if (beam4ppLMV.isOn) {// take beam line params from DB
      TDataSet* dbDataSet = GetDataBase("Calibrations/rhic");
      vertexSeed_st* vSeed =
	((St_vertexSeed*) (dbDataSet->FindObject("vertexSeed")))->GetTable();
      int beamEequivNtr=ppLMVparI[2];
      if(beamEequivNtr<=0) beamEequivNtr=(int) (vSeed->weight);
      // I can't beleive I wrote this dirty patch to switch cut4-->cut5, JB.  
      // Well, it works though so we will live, JL
      SetBeam4ppLMV(beamEequivNtr, vSeed->x0, vSeed->y0,
		    vSeed->dxdz, vSeed->dydz);
    }

    iRes = ppLMV4(maTrk,globtrk,vertex,mdate);

    if(EstGlobal && m_Mode == 4) {

      dst_vertex_st* MyVert = vertex->GetTable();
      for( int nVert=0; nVert<vertex->GetNRows(); nVert++){
	if( MyVert->iflag == 1) MyVert->iflag = 302;
	MyVert++;
      }
      ppLMV4(maEstTrk,EstGlobal,vertex,mdate);
    }

    break; }
  default:
    gMessMgr->Error() << "StVertexMaker: unknown vertex switch ="
		      << m_Mode << endm;
    return kStErr;
  } // end of lmv/evr,ppLMV switch

  return iRes;
}
//_____________________________________________________________________________
void StVertexMaker::FixVertex(Float_t x, Float_t y, Float_t z){
  if (m_fixedVertex) {
    m_fixedVertex->x = x;
    m_fixedVertex->y = y;
    m_fixedVertex->z = z;
  } else {
    m_fixedVertex = new dst_vertex_st;
    m_fixedVertex->vtx_id = kEventVtxId;
    m_fixedVertex->n_daughters = 0;
    m_fixedVertex->id = 1;
    m_fixedVertex->iflag = 1;
    m_fixedVertex->det_id = 3;
    m_fixedVertex->id_aux_ent = 0;
    m_fixedVertex->x = x;
    m_fixedVertex->y = y;
    m_fixedVertex->z = z;
    m_fixedVertex->covar[0] = 0.;
    m_fixedVertex->covar[1] = 0.;
    m_fixedVertex->covar[2] = 0.;
    m_fixedVertex->covar[3] = 0.;
    m_fixedVertex->covar[4] = 0.;
    m_fixedVertex->covar[5] = 0.;
    m_fixedVertex->chisq[0] = -1.;
    m_fixedVertex->chisq[1] = -1.;
  }
}
//_____________________________________________________________________________
Int_t StVertexMaker::FixVertex(Int_t eventNumber){
  if (eventNumber >= GetFixedSize()) {
    gMessMgr->Error() << "StVertexMaker: exceeded number of events for\n"
      << "      vertices in file!!! EXITING WITH ERROR!!!" << endm;
    return kStErr;
  }
  FixVertex(m_fixedArrayX.At(eventNumber),
            m_fixedArrayY.At(eventNumber),
            m_fixedArrayZ.At(eventNumber));
  return kStOK;
}
//_____________________________________________________________________________
Int_t StVertexMaker::FixVertex(Int_t runID, Int_t eventID){
  Bool_t found=kFALSE;
  Int_t i=curEvNum+1;                                 // Search forward
  while (!(found || (i >= GetMatchedSize()))) {
    if ((m_fixedArrayE.At(i) == eventID) &&
        (embedVerts || (m_fixedArrayR.At(i) == runID)))
      found = kTRUE;
    else i++;
  }
  if (! found) i=curEvNum-1;                          // Search backward
  while (!(found || (i < 0))) {
    if ((m_fixedArrayE.At(i) == eventID) &&
        (embedVerts || (m_fixedArrayR.At(i) == runID)))
      found = kTRUE;
    else i--;
  }
  if (! found) {
    gMessMgr->Error() << "StVertexMaker: no matching run and event IDs\n"
      << "      found from vertex file!!! EXITING WITH ERROR!!!" << endm;
    return kStErr;
  }
  curEvNum = i;
  FixVertex(m_fixedArrayX.At(curEvNum),
            m_fixedArrayY.At(curEvNum),
            m_fixedArrayZ.At(curEvNum));
  return kStOK;
}
//_____________________________________________________________________________
Int_t StVertexMaker::FixVertexFileRead(char* fname, Bool_t idMatch){
  // If idMatch, then read eventID, runID (or mult for embedding), X, Y, Z.
  // Otherwise read X, Y, Z.

  FILE* fp = fopen(fname,"r");
  if (fp) {
    int asize=1024;
    int count=0;
    int fR,fE;
    float fX,fY,fZ;
    while (( (idMatch) ?
             (fscanf(fp,"%d %d %f %f %f",&fE,&fR,&fX,&fY,&fZ)) :
             (fscanf(fp,"%f %f %f",&fX,&fY,&fZ)) )
           != EOF) {
      if ((count + 1) > GetFixedSize()) {
        m_fixedArrayX.Set(asize);
        m_fixedArrayY.Set(asize);
        m_fixedArrayZ.Set(asize);
        if (idMatch) {
          m_fixedArrayR.Set(asize);
          m_fixedArrayE.Set(asize);
        }
        asize *= 2;
      }
      m_fixedArrayX.AddAt(fX,count);
      m_fixedArrayY.AddAt(fY,count);
      m_fixedArrayZ.AddAt(fZ,count);
      if (idMatch) {
        m_fixedArrayR.AddAt(fR,count);
        m_fixedArrayE.AddAt(fE,count);
      }
      count++;
    }
    m_fixedArrayX.Set(count);
    m_fixedArrayY.Set(count);
    m_fixedArrayZ.Set(count);
    if (idMatch) {
      m_fixedArrayR.Set(count);
      m_fixedArrayE.Set(count);
    }
    fclose(fp);
    gMessMgr->Info() << "StPrimaryVertex: read in " << count <<
      " event vertices from vertex file: " << fname << endm;
    return kStOK;
  }
  gMessMgr->Error() << "StPrimaryVertex: Failed to find vertex file: "
    << fname << "\n           NOT FIXING VERTICES!!!" <<  endm;
  return kStErr;
}
//_____________________________________________________________________________
void StVertexMaker::UnFixVertex(){
  if (m_fixedVertex) {
    delete m_fixedVertex;
    m_fixedVertex = 0;
  }
}


//_____________________________________________________________________________
// $Id: StVertexMaker.cxx,v 1.11 2007/04/28 17:55:54 perev Exp $
// $Log: StVertexMaker.cxx,v $
// Revision 1.11  2007/04/28 17:55:54  perev
// Redundant StChain.h removed
//
// Revision 1.10  2004/12/16 00:29:56  jeromel
// Modifications (shall we say hack ?) to implement the ppLMV-5 cuts to
// ppLMV4.
//
// Revision 1.9  2003/09/02 17:59:26  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.8  2003/08/19 15:37:29  jeromel
// if(EstGlobal and m_Mode == 4) (???). Replaced with &&
// Logic is slightly changed as follow :
// - before EST => always OK as iRes=0 always and no re-assignement of iRes
//          TPT => counts on ppLMV4 returned value
// - now
//    EST or TPT, if ppLMV fails using globtrk, it fails and that is it.
//    Logic doew not change TPT behavior but change the meaning of the EST mode.
//
// Revision 1.7  2003/08/07 00:19:52  caines
// Write out the TPC only vertex as a calibration vertex if est vertex found
//
// Revision 1.6  2003/07/31 01:37:11  caines
// Set up code to find vertex using SVT matched tracks if that mode is selected
//
// Revision 1.5  2003/01/29 23:44:44  caines
// Looks for SVT vertex finder output if prevertex doesnt exist
//
// Revision 1.4  2002/12/04 15:43:05  jeromel
// Changes by J.Gans. Approved by Gene
//
// Revision 1.3  2002/02/22 23:44:57  jeromel
// 15 to 30 tracks as a start.
//
// Revision 1.2  2002/02/19 20:28:19  genevb
// Fix mode=2 option (VtxOffSet)
//
// Revision 1.1  2002/02/18 21:52:59  genevb
// Introduction of StVertexMaker for finding primary vertices
//
//
