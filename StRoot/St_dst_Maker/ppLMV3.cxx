// $Id: ppLMV3.cxx,v 1.1 2001/06/12 23:16:22 balewski Exp $
// $Log: ppLMV3.cxx,v $
// Revision 1.1  2001/06/12 23:16:22  balewski
// reject pileup in ppLMV
//
// Revision 1.1  2001/04/12 15:46:27  balewski
// *** empty log message ***
///////////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include "StChain.h"
#include "tables/St_g2t_ctf_hit_Table.h"
#include "tables/St_g2t_vertex_Table.h" // only for histo

#include "TH2.h"
void cts_get_ctb_indexes ( long volume, long &i_phi, long &i_eta ) ;

#include "StPrimaryMaker.h"

#include "SystemOfUnits.h"
#if !defined(ST_NO_NAMESPACES)
using namespace std::vector;
using namespace units;
#endif
#include "StThreeVectorD.hh"
#include "StHelixD.hh"
#include "StPhysicalHelixD.hh"
#include "TMath.h"
#include "tables/St_dst_track_Table.h"
#include "tables/St_dst_vertex_Table.h"
#include "StDetectorDefinitions.h"

#include "Stypes.h"
#include "StEventTypes.h" // for StEvent only
#include "StVertexId.h"
#include "math_constants.h"

#include "StarCallf77.h"
extern "C" {void type_of_call F77_NAME(gufld,GUFLD)(float *x, float *b);}
#define gufld F77_NAME(gufld,GUFLD)

//static const char rcsid[] = "$Id: ppLMV3.cxx,v 1.1 2001/06/12 23:16:22 balewski Exp $";

struct Jcyl {float eta,phi;};

//_____________________________________________________________________________
long StPrimaryMaker::ppLMV3(St_dst_track *track, St_dst_vertex *vertex, Int_t mdate)
{
  printf(" THIS IS ppLMV3 -START, use only tracks matched to CTB\n");
  static int eveId=0;
  eveId++;
  
  // Get BField from gufld(,) 
  //  cout<<"Trying to Get the BField the old way..."<<endl;
  float x[3] = {0,0,0};
  float b[3];
  gufld(x,b);
  double bfield = 0.1*b[2]; //This is now Tesla.

  // Parameters
  double Rmincut      = 3.9;
  long   MinTrkPoints = 10;
  double Rctb=213.6; // (cm) radius of the CTB 
  double PtMin=0.20;// ~ 0.16==R=2 m in 2001
  double DVtxMax      = 4.0;

  // Determine if SVT is in or not, depending on geometry and date in db
  long Is_SVT = 0;
  if( mdate > 20000700 )Is_SVT=1;

#ifdef ST_NO_TEMPLATE_DEF_ARGS
  vector<long,allocator<long> > index;
  vector<StPhysicalHelixD,allocator<StPhysicalHelixD> > helices;
  vector<double,allocator<double> > sigma;
#else
  vector<long > index;
  vector<StPhysicalHelixD > helices;
  vector<double > sigma;
#endif
  index.clear();
  double spath,h;
  double x0,y0,z0;
  double ptinv,psi,tanl;
  double px,py,pz;
  StThreeVectorD XVertex(0.,0.,0.);

  int i;

  long Ntrk = track->GetNRows();
  if( Ntrk <= 1 ){
    cout<<"ppLMV3: Event contains "<<Ntrk<<" global tracks. ";
    cout<<"No vertex can be reconstructed. continue.."<<endl;
    //return kStWarn; 
  }
  // --------------  A C C E S S    G E A N T   V E R T E X  (for histo)
  g2t_vertex_st *GVER=NULL;
  St_DataSet *gds=GetDataSet("geant"); // try to get M-C value
  if(gds) {
    St_g2t_vertex  *gver=(St_g2t_vertex  *) gds->Find("g2t_vertex");
    if(gver==NULL)
      {printf(" NULL pointer to St_g2t_vertext table\n");assert(gver);}
    if(gver->GetNRows()<=0)
      {printf(" empty St_g2t_vertext table\n"); assert(0);}
    GVER=gver->GetTable(); assert(GVER);
  }

  // -------------- E X T R A C T    C T B   H I T S   --------------------
  float CtbTofMax_ns=70.;
  float CtbDeThres_mev=1.;
  const int MxCtbH=240; // WARN one slat may have >1 geant hits !
  struct Jcyl CtbH[MxCtbH];
  int nCtbH=0;
  const float CtbEtaSeg=0.5, CtbPhiSeg=C_PI/30;
  struct Jcyl MatchCtbMax={CtbEtaSeg/2.+.01,CtbPhiSeg/2.+.01};

  //access the CTB data  from GEANT
  St_DataSetIter geant(GetDataSet("geant"));
  St_g2t_ctf_hit *g2t_ctb_hit = (St_g2t_ctf_hit *) geant("g2t_ctb_hit");
  g2t_ctf_hit_st *t = NULL;
  
  if (g2t_ctb_hit == NULL) { printf("No geant/ctb data set \n"); goto end_ctb ; }
  t = g2t_ctb_hit->GetTable();  assert(t);
  if (g2t_ctb_hit->GetNRows() == 0)
    { printf("Empty geant/ctb data set \n");  goto end_ctb  ; }
  
  // accumulate energy per slat , within time gate
  for (i = 0; i < g2t_ctb_hit->GetNRows(); i++,t++){
    float tof_ns=t->tof*1.e9;
    float de_mev=t->de*1000.;
    //hctb[0]->Fill(tof_ns);
    //((TH2*) hctb[10])->Fill(tof_ns,de_mev);
    if(tof_ns > CtbTofMax_ns) continue;
    if(de_mev <CtbDeThres_mev) continue;
    
    long iPhi,iEta;
    cts_get_ctb_indexes(t->volume_id,iPhi,iEta);
    iPhi--; iEta--; // change range to [0,N-1]
    assert(iPhi >= 0 && iPhi<60 && iEta>=0 && iEta<4);
    //printf("ctb_indexes , hit=%d, vol_id=%d, iPhi=%d, iEta=%d, de/MeV=%f, TOF/ns=%.1f\n",i,(int)t->volume_id,(int)iPhi,(int)iEta,t->de*1000,tof_ns );
    
    assert(nCtbH<MxCtbH);
    CtbH[nCtbH].eta=iEta*CtbEtaSeg -0.75;
    CtbH[nCtbH].phi=iPhi*CtbPhiSeg;
    //    printf("x=%d, eta=%f, phi/deg=%f\n",nCtbH,CtbH[nCtbH].eta,CtbH[nCtbH].phi/3.1416*180);
    nCtbH++;
  }
  printf("Total %d of %d CTB hits found significant\n", nCtbH,(int)g2t_ctb_hit->GetNRows()); 
  for(i=0;i<nCtbH;i++)
    printf("%d, eta=%.2f, phi/deg=%.1f\n",i,CtbH[i].eta,CtbH[i].phi/3.1416*180);
 end_ctb:

  //----------------------------  T R A C K S --------------
  cout<<"ppLMV3: Pileup Filter. Global tracks: in Table="<<Ntrk;
  //  long i_non_tpc=0;
  dst_track_st *glb_track_pointer = track->GetTable();
  dst_track_st *sec_pointer = glb_track_pointer;
  int n1=0,n2=0,n3=0, n4=0,n5=0,n6=0;
  for (long l=0; l<Ntrk; l++,glb_track_pointer++) {
    if(glb_track_pointer->iflag<=0) continue;
    n1++;
    hPiFi[0]->Fill(1);

    long NPoints = glb_track_pointer->n_point;
    if(NPoints <= MinTrkPoints) continue;
    n2++;
    hPiFi[0]->Fill(2);
    
    
    // First point on Helix
    x0 = glb_track_pointer->r0*cos(C_RAD_PER_DEG*glb_track_pointer->phi0);
    y0 = glb_track_pointer->r0*sin(C_RAD_PER_DEG*glb_track_pointer->phi0);
    z0 = glb_track_pointer->z0;
    StThreeVectorD origin(x0*centimeter, y0*centimeter, z0*centimeter);
    
    // Helicity / Sense of Curvatutre
    h  = 1.0;  if( bfield*glb_track_pointer->icharge > 0.0 )h=-1.0;
    double qtrk = 1.0; if( h*bfield > 0.0)qtrk=-1.0;
    
    // Track direction at first point
    ptinv  = glb_track_pointer->invpt;
    tanl   = glb_track_pointer->tanl;
    psi    = (C_PI/180.0)*glb_track_pointer->psi; 
    if(psi<0.0){psi=psi+2.*C_PI;}
    
    px   = (1./ptinv)*cos(psi);
    py   = (1./ptinv)*sin(psi);
    pz   = (1./ptinv)*tanl;
    StThreeVectorD MomFstPt(px*GeV, py*GeV, pz*GeV);
    
    StPhysicalHelixD TrkHlx(MomFstPt, origin, bfield*tesla, qtrk);
    printf("\ntrack i=%d px,y,z=%f %f %f psi/deg=%f tanl=%f, pT=%.3f\n",(int)l,px,py,pz,psi/C_PI*180,tanl,1./ptinv);
    printf(" track x0,y0,z0=%.1f, %.1f, %.1f, phi0/deg=%.1f, r0/cm=%.1f\n",x0,y0,z0,glb_track_pointer->phi0,glb_track_pointer->r0);
    // check Rmin condition      
    double xorigin = 0.0; double yorigin = 0.0;
    spath = TrkHlx.pathLength(xorigin, yorigin);
    StThreeVectorD XMinVec = TrkHlx.at(spath);
    cout<<" DCA Position: "<<XMinVec<<endl;
    double x_m = XMinVec.x(), y_m = XMinVec.y();
    double dmin = sqrt(x_m*x_m + y_m*y_m);
    if( dmin > Rmincut ) continue;
    n3++;
    hPiFi[0]->Fill(3);
    

    // check Zdca condition      
    float Zdca=XMinVec.z();
    if(fabs(Zdca) >zCutppLMV) continue;
    n4++;
    hPiFi[0]->Fill(4);

    //Find momentum at this point
    StThreeVectorD pmom;
    pmom = TrkHlx.momentumAt(spath, bfield*tesla);
    double beta = pmom.mag()/sqrt(pmom.mag()*pmom.mag()+0.139*0.139); //Assume pion
    float strag=0.0136/beta/pmom.mag()*fabs(spath);
    hPiFi[14]->Fill(strag); 
    hPiFi[15]->Fill(-spath); 
    //    printf("starg=%f %f %f %f \n",strag,beta,pmom.mag(),spath);

    //    goto jump; // over CTB matching
    if(1./ptinv<PtMin){printf("ignore this low PT=%f track\n",1./ptinv); continue;}
    n5++;
    hPiFi[0]->Fill(5);

    // match to CTB slats
    {
      pairD  SIfc; 
      SIfc = TrkHlx.pathLength(Rctb);
      printf(" path 1=%f, 2=%f, period=%f, R=%f\n",SIfc.first ,SIfc.second,TrkHlx.period(),1./TrkHlx.curvature());
      assert(SIfc.first<0); // propagate backwards
      assert(SIfc.second>0); // propagate forwards
      
      StThreeVectorD xpos;
      xpos = TrkHlx.at(SIfc.second);
      double xmagn = sqrt( xpos.x()*xpos.x() + xpos.y()*xpos.y() );
      printf(" punch2 x,y,z=%.1f, %.1f, %.1f, Rxy=%.1f\n",xpos.x(),xpos.y(),xpos.z(),xmagn);

      float eta=asinh(xpos.z()/xmagn);
      float phi=atan2(xpos.y(),xpos.x());
      if(phi<0) phi+=2*C_PI;
      printf(" track @CTB eta=%.2f, phi/deg=%.1f\n",eta, phi/C_PI*180);
      ((TH2F *)hPiFi[1])->Fill(eta,phi/C_PI*180);

      struct Jcyl best= MatchCtbMax;
      int ibest=-1;
      int nbest=0;
      for(i=0;i<nCtbH;i++) {
	//printf("  try CTB=%d, eta=%.2f, phi/deg=%.1f\n",i,CtbH[i].eta,CtbH[i].phi/C_PI*180);
	float del_eta=eta-CtbH[i].eta;
	float del_phi=phi-CtbH[i].phi;

	//printf("DEL1 eta=%.2f, phi/deg=%.1f\n",del_eta, del_phi/C_PI*180);
	if(fabs(del_eta) >fabs(best.eta)) continue;
	if(del_phi>C_PI) del_phi-=2*C_PI;
	if(del_phi<-C_PI) del_phi+=2*C_PI;
	//printf("  DEL@CTB eta=%.2f, phi/deg=%.1f\n",del_eta, del_phi/C_PI*180);
	if(fabs(del_phi) >fabs(best.phi)) continue;
	ibest=i;
	nbest++;
	best.eta=del_eta;
	best.phi=del_phi;
      }

      if(nbest==0) 
	printf(" CTB match result NONE ibest=%d\n",ibest);
      else
	printf(" CTB match result ibest=%d, del_eta=%.2f, del_phi/deg=%.1f, nbest=%d\n",ibest, best.eta,best.phi/C_PI*180,nbest);
      
      if(nbest==0) continue; // No match to CTB

     ((TH2F *)hPiFi[2])->Fill(eta,phi/C_PI*180);
     hPiFi[3]->Fill(best.eta);
     hPiFi[4]->Fill(best.phi/C_PI*180);
     
    }// od of CTB-Match check
    
    //  jump:
    n6++;
    // Now it is a good track
    hPiFi[0]->Fill(6);
    
    helices.push_back(TrkHlx);
    sigma.push_back(strag);
    long trk_id = glb_track_pointer->id;
    index.push_back(trk_id);
    
    // some histogramming
    hPiFi[7]->Fill(Zdca);
    if(GVER)hPiFi[8]->Fill(GVER->ge_x[2]-Zdca);
    
    hPiFi[5]->Fill(n5);
    hPiFi[6]->Fill(n6);
    
  } // end of loop over tracks

  printf(", used n1=%d n2=%d n3=%d n4=%d n5=%d, n6=%d\n",n1,n2,n3,n4,n5,n6);

  //  ----------  D O   F I N D    V E R T E X
  
  printf("%s - search for vertex using %d matched tracks: start ...\n",GetName(),helices.size());
  assert( helices.size() == index.size() );
  assert( helices.size() == sigma.size() );

  //Do the actual vertex fitting, continue until good
  double A11=0.0,A12=0.0,A13=0.0,A21=0.0,A22=0.0,A23=0.0;
  double A31=0.0,A32=0.0,A33=0.0; // Matrix Elements
  double C11=0.0,C12=0.0,C13=0.0,C21=0.0,C22=0.0,C23=0.0;
  double C31=0.0,C32=0.0,C33=0.0; // C = A^-1
  int done = 0;
  double chi2=0;
  while( done != 1 ){

    // Check that there at least are 2 tracks
    if( helices.size() <= 1 ){
      cout<<"ppLMV3: Fewer than 2 track remains. No vertex found."<<endl;
      return kStWarn;
    }
  
    // Begin by doing a fit
    A11=0.0,A12=0.0,A13=0.0,A21=0.0,A22=0.0,A23=0.0;
    A31=0.0,A32=0.0,A33=0.0; // Matrix Elements
    C11=0.0,C12=0.0,C13=0.0,C21=0.0,C22=0.0,C23=0.0;
    C31=0.0,C32=0.0,C33=0.0; // C = A^-1
    double b1=0.0,b2=0.0,b3=0.0;
    // Compute matrix A and vector b
    for(unsigned int itr=0; itr < helices.size(); itr++){ 
      //      sigma[itr]=1.0;
      double xo=0.0,yo=0.0;
      spath = helices[itr].pathLength(xo,yo);
      StThreeVectorD XClosest = helices[itr].at(spath);
      StThreeVectorD XMomAtClosest = helices[itr].momentumAt(spath,bfield*tesla);
      double xp   = XClosest.x(); double yp= XClosest.y(); double zp= XClosest.z();  
      double xhat = XMomAtClosest.x()/XMomAtClosest.mag();
      double yhat = XMomAtClosest.y()/XMomAtClosest.mag();
      double zhat = XMomAtClosest.z()/XMomAtClosest.mag();
      A11=A11+(yhat*yhat+zhat*zhat)/sigma[itr];
      A12=A12-(xhat*yhat)/sigma[itr];
      A13=A13-(xhat*zhat)/sigma[itr];
      A22=A22+(xhat*xhat+zhat*zhat)/sigma[itr];
      A23=A23-(yhat*zhat)/sigma[itr];
      A33=A33+(xhat*xhat+yhat*yhat)/sigma[itr];
      b1=b1 + ( (yhat*yhat+zhat*zhat)*xp - xhat*yhat*yp - xhat*zhat*zp )/sigma[itr];
      b2=b2 + ( (xhat*xhat+zhat*zhat)*yp - xhat*yhat*xp - yhat*zhat*zp )/sigma[itr];
      b3=b3 + ( (xhat*xhat+yhat*yhat)*zp - xhat*zhat*xp - yhat*zhat*yp )/sigma[itr];
    }
    A21 = A12; A31=A13; A32=A23;

    // Invert A
    double detA =   A11*A22*A33 + A12*A23*A31 + A13*A21*A32;
    detA = detA   - A31*A22*A13 - A32*A23*A11 - A33*A21*A12;
    //    cout<<"Determinant= "<<detA<<endl;
    //    cout<<"A11,A12,A13: "<<A11<<" "<<A12<<" "<<A13<<endl;
    //    cout<<"A21,A22,A23: "<<A21<<" "<<A22<<" "<<A23<<endl;
    //    cout<<"A31,A32,A33: "<<A31<<" "<<A32<<" "<<A33<<endl;
    //    cout<<"b1,b2,b3 "<<b1<<" "<<b2<<" "<<b3<<endl;
    C11=(A22*A33-A23*A32)/detA; C12=(A13*A32-A12*A33)/detA; C13=(A12*A23-A13*A22)/detA;
    C21=C12;                    C22=(A11*A33-A13*A31)/detA; C23=(A13*A21-A11*A23)/detA;
    C31=C13;                    C32=C23;                    C33=(A11*A22-A12*A21)/detA;

    // Find Vertex Position
    double Xv = C11*b1 + C12*b2 + C13*b3;
    double Yv = C21*b1 + C22*b2 + C23*b3;
    double Zv = C31*b1 + C32*b2 + C33*b3;
    XVertex.setX(Xv); XVertex.setY(Yv); XVertex.setZ(Zv);
    //    cout<<"Vertex Position   : "<<XVertex.x()<<" "<<XVertex.y()<<" "<<XVertex.z()<<endl;
    //    cout<<"Error in Position : "<<sqrt(C11)<<" "<<sqrt(C22)<<" "<<sqrt(C33)<<endl;
    

    // Check if the fit is any good
    // Loop over tracks again to get Chi2 and check each track's deviation
    double dmax=0.0;
#ifdef ST_NO_TEMPLATE_DEF_ARGS
    vector<StPhysicalHelixD,allocator<StPhysicalHelixD> >::iterator itehlx=helices.begin(), i1keep;
    vector<double,allocator<double> >::iterator itesig=sigma.begin(),i2keep;
    vector<long,allocator<long> >::iterator iteind=index.begin(),i3keep;
#else
    vector<StPhysicalHelixD >::iterator itehlx=helices.begin(), i1keep;
    vector<double >::iterator itesig=sigma.begin(),i2keep;
    vector<long >::iterator iteind=index.begin(),i3keep;
#endif
    while( itehlx != helices.end()){
      //      sigma[itr]=1.0;
      StPhysicalHelixD hlx = *itehlx;
      double sig = *itesig;
      spath = hlx.pathLength(XVertex); 
      StThreeVectorD XHel = hlx.at(spath);
      double d=(XHel.x()-XVertex.x())*(XHel.x()-XVertex.x());
         d = d+(XHel.y()-XVertex.y())*(XHel.y()-XVertex.y());
         d = d+(XHel.z()-XVertex.z())*(XHel.z()-XVertex.z());
         d = sqrt(d);
      chi2 = chi2 + (d*d)/(sig*sig);
      double drel = d/sig;
      if( drel > dmax ){
	// Save the track that deviates the most from vertex
        dmax = drel;
        i1keep = itehlx;
        i2keep = itesig;
        i3keep = iteind;
      }
      //
      itehlx++; itesig++; iteind++;
    }

    if( dmax > DVtxMax ){
      //      cout<<"Removing a track! dmax= "<<dmax<<endl;
      helices.erase(i1keep);
      sigma.erase(i2keep);
      index.erase(i3keep);
      done=0;
    }
    else{
      done=1;
    }
  } // End While Loop

  double  chi2pdof = chi2/(helices.size()-1);


  cout<<"ppLMV3: Primary Vertex found! Position: "<<XVertex<<endl;

  {// fill some histos
    float rZver=XVertex.z();
    float rXver=XVertex.x();
    float rYver=XVertex.y();
    hPiFi[9]->Fill(rZver);
  
    if(GVER) {
      hPiFi[10]->Fill(GVER->ge_x[2]-rZver);
      hPiFi[11]->Fill(GVER->ge_x[0]-rXver);
      hPiFi[12]->Fill(GVER->ge_x[1]-rYver);
      printf("Z Geant-found=%.2f, dx=%.2f, dy=%.2f\n",GVER->ge_x[2]-rZver,GVER->ge_x[0]-rXver,GVER->ge_x[1]-rYver);
    }
  }  // end of histos


  Int_t nrows = vertex->GetNRows();
  long IVertex = nrows+1; //By definition

  // Mark the vertex tracks in the global_trk table
  for (long ll=0; ll<Ntrk; ll++){
    long idt = sec_pointer->id;
    long icheck;
    icheck = 0;
    for(unsigned int ine=0; ine < index.size(); ine++){
      if( idt == index[ine] )icheck=1;
    }  
    Int_t istart_old;
    if( icheck == 1 ){
      // This track was included in the Vertex
      istart_old = sec_pointer->id_start_vertex;
      sec_pointer->id_start_vertex = 10*IVertex + istart_old;
    }
    sec_pointer++;
  }

  // Fill the dst_vertex table
  dst_vertex_st primvtx;
  primvtx.vtx_id      = kEventVtxId;
  primvtx.n_daughters = helices.size();
  primvtx.id          = IVertex;
  primvtx.iflag       = 1;
  if( (Is_SVT == 1) ){
    primvtx.det_id    = kTpcSsdSvtIdentifier; 
  }
  else{
    primvtx.det_id    = kTpcIdentifier; //TPC track by definition
  }
  primvtx.id_aux_ent  = 0;
  primvtx.x           = XVertex.x();
  primvtx.y           = XVertex.y();
  primvtx.z           = XVertex.z();
  primvtx.covar[0]    = nCtbH* nCtbH;// temp JB, was  =C11;
  primvtx.covar[1]    = C12;
  primvtx.covar[2]    = C22;
  primvtx.covar[3]    = C13;
  primvtx.covar[4]    = C23;
  primvtx.covar[5]    = C33;
  primvtx.chisq[0]    = chi2pdof;
  primvtx.chisq[1]    = 1.0; // Need to find the prob func in Root
  vertex->AddAt(&primvtx,nrows);

  return kStOk;
}
    
#if 0
##############################################################


  StEvent *Steve= (StEvent *) GetInputDS("StEvent");
  assert(Steve);
  // Tracks from StEvent
  StSPtrVecTrackNode& trackNodes = Steve->trackNodes();
  cout<<"** StPreEclRMaker:: Node size **"<<trackNodes.size()<<endl;
  int allGlobals =0;
  StTrack* track1;
  for (size_t nodeIndex=0; nodeIndex<trackNodes.size(); nodeIndex++) {
    
    size_t numberOfTracksInNode =  trackNodes[nodeIndex]->entries(global);
    for (size_t trackIndex=0; trackIndex<numberOfTracksInNode; trackIndex++) {
      track1 =trackNodes[nodeIndex]->track(global,trackIndex);
      if (track1){
	
	//emcFinalCoord(fieldB*tesla, track, &finalPosition, &finalMomentum);
      }
    }
  }
#endif
