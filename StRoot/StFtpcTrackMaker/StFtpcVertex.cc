// $Id: StFtpcVertex.cc,v 1.5 2000/11/28 14:00:53 hummler Exp $
// $Log: StFtpcVertex.cc,v $
// Revision 1.5  2000/11/28 14:00:53  hummler
// protect vertex finder against nan
//
// Revision 1.4  2000/11/10 18:39:25  oldi
// Changes due to replacement of StThreeVector by TVector3.
// New constructor added to find the main vertex with given point array.
//
// Revision 1.3  2000/06/13 14:35:00  oldi
// Changed cout to gMessMgr->Message().
//
// Revision 1.2  2000/05/15 14:28:15  oldi
// problem of preVertex solved: if no main vertex is found (z = NaN) StFtpcTrackMaker stops with kStWarn,
// refitting procedure completed and included in StFtpcTrackMaker (commented),
// new constructor of StFtpcVertex due to refitting procedure,
// minor cosmetic changes
//
// Revision 1.1  2000/05/10 13:39:34  oldi
// Initial version of StFtpcTrackMaker
//

//----------Author:        Holm G. H&uuml;ummler, Markus D. Oldenburg
//----------Last Modified: 24.07.2000
//----------Copyright:     &copy MDO Production 1999

#include "StFtpcVertex.hh"
#include "StFtpcPoint.hh"

#include "St_DataSet.h"
#include "St_DataSetIter.h"
#include "tables/St_g2t_vertex_Table.h"

#include "StMessMgr.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// StFtpcVertex class - representation of the main vertex for the FTPC.   //
//                                                                        //
// This class contains the coordinates of the main vertex plus the usual  //
// getters and setter. It is just a wrapper of the Staf tables.           //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "tables/St_fcl_fppoint_Table.h"

ClassImp(StFtpcVertex)


StFtpcVertex::StFtpcVertex()
{
  // Default constructor.

  SetX(0.);
  SetY(0.);
  SetZ(0.);

  SetXerr(0.);
  SetYerr(0.);
  SetZerr(0.);
  
  return;
}


StFtpcVertex::StFtpcVertex(fcl_fppoint_st *thisFppoint, Int_t numFppoints)
{
  // constructor with ftpc points - fits vertex from points

  // constants, to be moved to parameter database
#define HISTOBINS 300
#define HISTOMIN -75.0
#define HISTOMAX 75.0

  Float_t *rmap = new Float_t[20*6*numFppoints];
  Float_t *zmap = new Float_t[20];
  Int_t *mapMax = new Int_t[20*6];
  Int_t *myhist = new Int_t[HISTOBINS];
  Float_t hratio=HISTOBINS/(HISTOMAX-HISTOMIN);
  
  for(Int_t iii=0; iii<HISTOBINS; iii++) {
    myhist[iii]=0;
  }
  
  for(Int_t ii=0; ii<120; ii++) mapMax[ii]=0;
  
  for(Int_t i=0; i<numFppoints;i++) {
    rmap[(thisFppoint[i].row-1)+20*(thisFppoint[i].sector-1)+120*mapMax[(thisFppoint[i].row-1)+20*(thisFppoint[i].sector-1)]]=sqrt(thisFppoint[i].x*thisFppoint[i].x+thisFppoint[i].y*thisFppoint[i].y);
    zmap[thisFppoint[i].row-1]=thisFppoint[i].z;
    mapMax[(thisFppoint[i].row-1)+20*(thisFppoint[i].sector-1)]++;
  }

  for(Int_t secI=0; secI<6; secI++) {
    
    for(Int_t rowOut=0; rowOut<19; rowOut++) {

      for(Int_t rowIn=rowOut+1; rowIn<20; rowIn++) {
	
	if(rowIn<10 || rowOut>=10) {
	  
	  for(Int_t iOut=0; iOut<mapMax[rowOut+20*secI]; iOut++) {
	    Float_t ri=rmap[rowOut+20*secI+120*iOut];	    

	    for(Int_t iIn=0; iIn<mapMax[(rowIn)+20*secI]; iIn++) {
	      Float_t rj=rmap[rowIn+20*secI+120*iIn];
			  
	      if(rj>ri) {
		Float_t intersect=(rj*zmap[rowOut]-ri*zmap[rowIn])/(rj-ri);
		
		if(intersect>HISTOMIN && intersect<HISTOMAX) {
		  myhist[int((intersect-HISTOMIN)*hratio)]++;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  Int_t maxBin=HISTOBINS/2, maxHeight=0;
  
  Float_t vertex=0;

  for(Int_t hindex=1; hindex<HISTOBINS-1; hindex++) {
    
    if(myhist[hindex]>maxHeight && myhist[hindex]>=myhist[hindex-1] && myhist[hindex]>=myhist[hindex+1]) {
      maxBin=hindex;
      maxHeight=myhist[hindex];
    }  
  }

  // check if Gaussfit will fail
  if((myhist[maxBin] == 0) 
     || (myhist[maxBin+1] == 0) 
     || (myhist[maxBin-1] == 0) 
     || (myhist[maxBin] <= myhist[maxBin+1]) 
     || (myhist[maxBin] <= myhist[maxBin-1])) {
    
    // use weighted mean instead 
    vertex=(myhist[maxBin]*((maxBin+0.5)/hratio+HISTOMIN)
	    + myhist[maxBin-1]*((maxBin-0.5)/hratio+HISTOMIN)
	    + myhist[maxBin+1]*((maxBin+1.5)/hratio+HISTOMIN))
      / (myhist[maxBin]+myhist[maxBin-1]+myhist[maxBin+1]);
  }

  else {
      
    // do gaussfit 
    Float_t sigma = sqrt (1 / ((2 * log(myhist[maxBin])) -
			       (log(myhist[maxBin+1]) + 
				log(myhist[maxBin-1]))));
    vertex =  ((maxBin+0.5)/hratio+HISTOMIN) + 
      sigma*sigma/(hratio*hratio) * (log(myhist[maxBin+1]) - 
				     log(myhist[maxBin-1]));
  } 
		  
  delete[] myhist;
  delete[] mapMax;
  delete[] zmap;
  delete[] rmap;
  
  SetX((Double_t) 0);
  SetY((Double_t) 0);
  if(vertex*0 != 0)
    {
      cerr << "vertex not found, setting to 0!" << endl;
      vertex = 0;
    }
  SetZ((Double_t) vertex);
}


StFtpcVertex::StFtpcVertex(TClonesArray *hits)
{
  // Constructor with TClonesArray of ftpc points - fits vertex from points

  // constants, to be moved to parameter database
#define HISTOBINS 300
#define HISTOMIN -75.0
#define HISTOMAX 75.0

  Int_t numFppoints = hits->GetEntriesFast();

  Float_t *rmap = new Float_t[20*6*numFppoints];
  Float_t *zmap = new Float_t[20];
  Int_t *mapMax = new Int_t[20*6];
  Int_t *myhist = new Int_t[HISTOBINS];
  Float_t hratio=HISTOBINS/(HISTOMAX-HISTOMIN);
  
  for(Int_t iii=0; iii<HISTOBINS; iii++) {
    myhist[iii]=0;
  }
  
  for(Int_t ii=0; ii<120; ii++) mapMax[ii]=0;
  
  for(Int_t i=0; i<numFppoints;i++) {

    StFtpcPoint *thispoint = (StFtpcPoint *)hits->At(i);

    rmap[(thispoint->GetPadRow()-1)+20*(thispoint->GetSector()-1)+120*mapMax[(thispoint->GetPadRow()-1)+20*(thispoint->GetSector()-1)]]=sqrt(thispoint->GetX()*thispoint->GetX()+thispoint->GetY()*thispoint->GetY());
    zmap[thispoint->GetPadRow()-1]=thispoint->GetZ();
    mapMax[(thispoint->GetPadRow()-1)+20*(thispoint->GetSector()-1)]++;
  }

  for(Int_t secI=0; secI<6; secI++) {
    
    for(Int_t rowOut=0; rowOut<19; rowOut++) {

      for(Int_t rowIn=rowOut+1; rowIn<20; rowIn++) {
	
	if(rowIn<10 || rowOut>=10) {
	  
	  for(Int_t iOut=0; iOut<mapMax[rowOut+20*secI]; iOut++) {
	    Float_t ri=rmap[rowOut+20*secI+120*iOut];	    

	    for(Int_t iIn=0; iIn<mapMax[(rowIn)+20*secI]; iIn++) {
	      Float_t rj=rmap[rowIn+20*secI+120*iIn];
			  
	      if(rj>ri) {
		Float_t intersect=(rj*zmap[rowOut]-ri*zmap[rowIn])/(rj-ri);
		
		if(intersect>HISTOMIN && intersect<HISTOMAX) {
		  myhist[int((intersect-HISTOMIN)*hratio)]++;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  Int_t maxBin=HISTOBINS/2, maxHeight=0;
  
  Float_t vertex=0;

  for(Int_t hindex=1; hindex<HISTOBINS-1; hindex++) {
    
    if(myhist[hindex]>maxHeight && myhist[hindex]>=myhist[hindex-1] && myhist[hindex]>=myhist[hindex+1]) {
      maxBin=hindex;
      maxHeight=myhist[hindex];
    }  
  }

  // check if Gaussfit will fail
  if((myhist[maxBin] == 0) 
     || (myhist[maxBin+1] == 0) 
     || (myhist[maxBin-1] == 0) 
     || (myhist[maxBin] <= myhist[maxBin+1]) 
     || (myhist[maxBin] <= myhist[maxBin-1])) {
    
    // use weighted mean instead 
    vertex=(myhist[maxBin]*((maxBin+0.5)/hratio+HISTOMIN)
	    + myhist[maxBin-1]*((maxBin-0.5)/hratio+HISTOMIN)
	    + myhist[maxBin+1]*((maxBin+1.5)/hratio+HISTOMIN))
      / (myhist[maxBin]+myhist[maxBin-1]+myhist[maxBin+1]);
  }

  else {
      
    // do gaussfit 
    Float_t sigma = sqrt (1 / ((2 * log(myhist[maxBin])) -
			       (log(myhist[maxBin+1]) + 
				log(myhist[maxBin-1]))));
    vertex =  ((maxBin+0.5)/hratio+HISTOMIN) + 
      sigma*sigma/(hratio*hratio) * (log(myhist[maxBin+1]) - 
				     log(myhist[maxBin-1]));
  } 
		  
  delete[] myhist;
  delete[] mapMax;
  delete[] zmap;
  delete[] rmap;
  
  SetX(0.);
  SetY(0.);
  if(vertex*0 != 0)
    {
      cerr << "vertex not found, setting to 0!" << endl;
      vertex = 0;
    }
  SetZ((Double_t) vertex);
}


StFtpcVertex::StFtpcVertex(St_DataSet *const geant)
{
  // Obsolete constructor taking vertex from geant.

  if (geant) {
    St_DataSetIter geantI(geant);
    St_g2t_vertex *g2t_vertex = (St_g2t_vertex *) geantI.Find("g2t_vertex");
    
    if (g2t_vertex) {
      g2t_vertex_st   *vertex = g2t_vertex->GetTable();
      SetX((Double_t) vertex->ge_x[0]);
      SetY((Double_t) vertex->ge_x[1]);
      SetZ((Double_t) vertex->ge_x[2]);
      gMessMgr->Message("Using primary vertex coordinates (Geant): ", "I", "OST");
    }
    
    else {
      Double_t dummy = 0.0;
      SetX(dummy);
      SetY(dummy);
      SetZ(dummy);
      gMessMgr->Message("Using primary vertex coordinates (Default): ", "I", "OST");
    }
  }
}


StFtpcVertex::StFtpcVertex(Double_t pos[3])
{
  // constructor from Doubles
  
  SetX((Double_t) pos[0]);
  SetY((Double_t) pos[1]);
  SetZ((Double_t) pos[2]);
}  


StFtpcVertex::StFtpcVertex(Double_t x, Double_t y, Double_t z)
{
  // constructor from Doubles
  
  SetX(x);
  SetY(y);
  SetZ(z);
}  


StFtpcVertex::~StFtpcVertex() 
{
  // Destructor.
  // Does nothing except destruct.
}
