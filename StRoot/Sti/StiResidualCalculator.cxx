//StiResidualCalculator.cxx
/***************************************************************************
 *
 * $Id: StiResidualCalculator.cxx,v 2.17 2006/03/09 22:45:14 didenko Exp $
 *
 * \class  StiResidualCalculator provides a utility for determining the
 *         track residuals.
 * \author Andrew Rose, Wayne State University 
 * \date   October 2002
 ***************************************************************************
 * $Log: StiResidualCalculator.cxx,v $
 * Revision 2.17  2006/03/09 22:45:14  didenko
 * get back previuos version
 *
 * Revision 2.15  2005/03/24 18:06:42  perev
 * Do not allow to modify node anymore
 *
 * Revision 2.14  2005/01/17 03:59:13  pruneau
 * change track container to vector
 *
 * Revision 2.13  2005/01/17 01:31:26  perev
 * New parameter model
 *
 * Revision 2.12  2004/05/03 23:36:46  perev
 * Possible non init WarnOff
 *
 * Revision 2.11  2004/04/05 21:51:45  andrewar
 * Added hist for dz vs. dy plot
 *
 * Revision 2.10  2004/02/21 18:27:41  pruneau
 * Updates to comply with changes made in abstract interfaces.
 *
 * Revision 2.9  2004/01/27 02:55:11  perev
 * WarnOff
 *
 * Revision 2.8  2003/10/28 15:59:22  andrewar
 * Fixed int() to (int) for var.
 *
 * Revision 2.7  2003/09/07 03:49:08  perev
 * gcc 3.2 + WarnOff
 *
 * Revision 2.6  2003/09/02 17:59:41  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.5  2003/06/10 16:23:29  andrewar
 * Added functions to residual calculator. Added parallel hist set for
 * different detector layers.
 *
 * Revision 2.4  2003/04/30 16:38:16  pruneau
 * active detector hit filtering
 *
 * Revision 2.3  2003/04/30 15:38:56  pruneau
 * Integrating StiResidualCalculator into the main stream.
 *
 * Revision 2.2  2003/04/29 18:48:33  pruneau
 * *** empty log message ***
 *
 * Revision 2.1  2003/04/29 14:59:01  andrewar
 * Modified to conform to naming convention. Added
 * initDetectors(StiDetectorBuilder) to switch desired detectors 'off' during
 * tracking, so residual will be unbiased.
 *
 */

#include <math.h>
#include <float.h>

//STL
#include <algorithm>
using std::transform;
#include <numeric>
using std::accumulate;
#include <functional>
using std::less;


//ROOT
#include "TH3.h"
#include "TH2.h"
#include "TFile.h"

//StEvent includes
#include "StDetectorId.h"
#include "StHit.h"

//Sti Includes
#include "Sti/StiNeverActiveFunctor.h"
#include "Sti/StiHit.h"
#include "Sti/StiTrack.h"
#include "Sti/StiKalmanTrack.h"
#include "Sti/StiKalmanTrackNode.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiDetectorBuilder.h"
#include "Sti/StiHitErrorCalculator.h"

#include "Sti/StiResiduals.h"
#include "Sti/StiResidualCalculator.h"

class TH3D;
class TFile;

//Constructor
StiResidualCalculator::StiResidualCalculator(StiHitContainer *hitC)
  :candidateHits(hitC)
{
  candidates.clear();
  cout <<"StiResidualCalculator::StiResidualCalculator created "<<endl;
  int check=Init();
  if(check<1) cout <<"StiResidualCalculator::StiResidualCalculator "
		   <<"Error while initializing histograms."<<endl;
}

void StiResidualCalculator::initialize(StiDetectorBuilder*detBuilder)
{
  initDetector(detBuilder);
}

void StiResidualCalculator::initDetector(StiDetectorBuilder *detBuilder)
{
  string baseName;
  string detName;

  cout <<"StiResidualCalculator::initDetector"<<endl;
  if (detBuilder)
    {
      cout <<"Group ID: "<<detBuilder->getGroupId()<<endl;
      cout <<"Det Name: "<<detBuilder->getName()<<endl;
      int rows = detBuilder->getNRows();
      cout <<"Rows: "<<rows;
      int sectors = detBuilder->getNSectors(0);
      cout <<" Sectors "<<sectors<<endl;

      isNotActiveFunc = new StiNeverActiveFunctor();
      


      for(int i=49, j=0; j<(int)detBuilder->getNSectors(i);j++)
	{
	  StiDetector* det = detBuilder->getDetector(i,j);
	  //mark detector as unused
	  cout <<"Name: "<<det->getName()<<endl;
	  det->setIsActive(isNotActiveFunc);
	  //store detector pointer in vector
	  candidates.push_back(det);
	  
	  //Slashes in name make root unhappy
	  detName=(string)(det->getName());
	  int x=detName.find("/");

	  while( x < (int)(detName.size()) && x>0 )
	    {
	      detName.replace(x,1,"_");
	      x=detName.find("/");
	    }

	  cout <<"Residuals for "<<detName
	       <<" set"<<endl;


	  //init hists for detectors
	  baseName = "y_cross_z_";
	  baseName = baseName + detName;
	  mYResidualCrossZ.push_back(book(baseName,
					   "Residual in Y vs. Cross, Z", 
					   20, -1, 1, 40, -200, 200, 
					   75, -1.5,1.5));
	  baseName= "y_dip_z_";
	  baseName=baseName+ detName;
	  mYResidualDipZ.push_back(book(baseName,"Residual in Y vs. Dip, Z",
				      20,-1,1,40,-200,200.,
				      75,-1.5,1.5));
	  baseName= "yd_cross_z_";
	  baseName=baseName+ detName;
	  mYDResidualCrossZ.push_back(book(baseName,
					   "Residual in Y vs. Cross, Z", 
					   20, -1, 1, 40, -200, 200, 
					   75, -1.5,1.5));
	  baseName= "yd_dip_z_";
	  baseName=baseName+ detName;
	  mYDResidualDipZ.push_back(book(baseName,"Residual in Y vs. Dip, Z",
				      20,-1,1,40,-200,200.,
				      75,-1.5,1.5));

	  baseName="z_cross_z";
	  baseName=baseName+ detName;
	  mZResidualCrossZ.push_back(book(baseName,
					   "Residual in Z vs. Cross, Z", 
					   20, -1, 1, 40, -200,200, 
					   75, -1.5,1.5));
	  baseName= "z_dip_z_";
	  baseName=baseName+ detName;
	  mZResidualDipZ.push_back(book(baseName,"Residual in Z vs. Dip, Z",
				      20,-1,1,40,-200,200.,
				      75,-1.5,1.5));

	  baseName="zd_cross_z";
	  baseName=baseName+ detName;
	  mZDResidualCrossZ.push_back(book(baseName,
					   "Residual in Z vs. Cross, Z", 
					   20, -1, 1, 40, -200,200, 
					   75, -1.5,1.5));
	  baseName= "zd_dip_z_";
	  baseName=baseName+ detName;
	  mZDResidualDipZ.push_back(book(baseName,"Residual in Z vs. Dip, Z",
				      20,-1,1,40,-200,200.,
				      75,-1.5,1.5));

	  baseName = "t_cross_dip_";
	  baseName=baseName+detName;
	  mResidualCrossDip.push_back(book(  baseName,
					   "Residual in Y vs. Cross, Dip", 
					   60, -1, 1, 60, -1, 1, 
					   75, -1.5,1.5));
	  baseName= "y_z_";
	  baseName=baseName+detName;
	  mResidualZY.push_back(book( baseName,"",20,-.5,.5,
				    20,-.5,.5));


	  mDetectorHist->Fill(i,j);
	}
    }
  else
    cout << "StiResidualCalculator::No valid Detector Builder found."<<endl;


  return;
}

//Methods
int StiResidualCalculator::Init()
{

  cout <<"StiResidualCalculator::Init"<<endl;

  //!
  //! Initialize histograms (with detector info).
  //! Returns 0 if fails, 1 if success.
  //!




  mCross = book("mCross","",30,-1.7,1.7);
  mDip = book("mDip","",30,-1.7,1.7);
  mPt = book("mPt","",30,0.,6.);
  mDrift = book("mDrift","",100,-200.,200.);

  mDetectorHist = book("mDetectorHist","Detectors in Study", 
		       50,0.,50.,12,0.,12.);



      _BackgroundZ = book("_BackgroundZ","Background dZ",
				  100,-2.,2.);
      _BackgroundY = book("_BackgroundY","Background dY",
				  100,-2.,2.);
      _BackgroundClosestZ = book("_BackgroundClosestZ",
				 "Background Closest dZ",
				 100,-2.,2.);
      _BackgroundClosestY = book("_BackgroundClosestY",
				 "Background Closest dY",
				 100,-2.,2.);


  return 1;
}

void StiResidualCalculator::calcResiduals(StiTrackContainer *tracks)
{
  //Do some checking to make sure the next call isn't pointless
  if(candidates.size()<=1)
    {
      //only background hists
      cout<<"StiResidualCalculator::trackResiduals() -I-"
	  <<" no detectors initialized for residuals."<<endl;
      return;
    }  

  //get iterator from track container
  vector<StiTrack*>::const_iterator trackIt = tracks->begin();
  //loop over tracks
  int check=0;
  while(trackIt!=tracks->end())
    {
      //this is where track cuts could be used
      check= trackResidue(*trackIt);
      trackIt++;
    }
}

///Take a general StiTrack. Since this maker is written
///specifically for StiKalmanTracks (needs handles in
///Kalman tracks), check for castness to Kalman track.
///If fails, exit. If success, call trackResidue(StiKalmanTrack).
int StiResidualCalculator::trackResidue(const StiTrack *track)
{
  int check;
  //cast to Kalman Track; if fail, end
  const StiKalmanTrack* kTrack = 
    static_cast<const StiKalmanTrack*>(track);  
  if(!kTrack)
    {
      cout <<"Error: Could not cast to Kalman Track!"<<endl;
      return -10;
    }  
  
  //if(kTrack->getCurvature()<0) 
  check = trackResidue(kTrack);
  return check;
}

/// Retrieves the node list for the input KalmanTrack,
/// calls a subroutine to fill the hists with the
/// pertinant values (fillHist).
int StiResidualCalculator::trackResidue(const StiKalmanTrack *track)
{
  double nHits =0;	
  vector<StiKalmanTrackNode*> trackNodes=track->getNodes(kSvtId);
  nHits=trackNodes.size();
  if (nHits<2) return 1;
  StiKalmanTrackNode* leaf = track->getLastNode();
  StiKTNForwardIterator iT(leaf);
  StiKTNForwardIterator end = iT.end();
  double pt=track->getPt();
  while(iT != end)
    {
      StiKalmanTrackNode iNode = (*iT);
      //if the node has no valid detector pointer, go on to next node
      if(!iNode.getDetector()) {iT++;continue;}
      //Residuals are calculated uner following conditions:
      //1.) The node's detector is *not* active
      //2.) The detector has been entered into the list of
      //    detectors to examine (should be an exclusive subset of 1) 
      if(!(iNode.getDetector()->isActive())
	 && (find(candidates.begin(), candidates.end(),iNode.getDetector())
  	     !=candidates.end()))
	{

	  if(iNode.getHit())
	    { 
	      cout <<"Warning! Node has a hit!"<<endl
	           <<"Detector: "<<iNode.getDetector()->getName()
		   <<endl
		   <<"Is Active?: "<<iNode.getDetector()->isActive()<<endl
		   <<"Hit det?: "<<iNode.getHit()->detector()->getName()
		   <<endl;
	      ++iT;
	      continue;
	    }

	  //get node values
	  double cross = iNode.crossAngle();
	  double dip   = iNode.pitchAngle();
	  double nodeZ = iNode.getZ();
//VP	  double dy,dz;

	  fillTrackHist(cross,dip,pt, nodeZ);


	  //The offset of the detector in the candidate vec should
	  //be the same as the offset for the histograms...
	  int histVecOffset = find(candidates.begin(), 
				   candidates.end(),
				   iNode.getDetector())-candidates.begin();
	  vector<StiHit*> hitVec = candidateHits->getHits(iNode.getDetector());
	  NodeResidue(iNode, hitVec, histVecOffset);

	  
	  //if(histVecOffset == candidates.size()) histVecOffset=1;
	  //vector<StiDetector*>::iterator iD = candidates.begin()+histVecOffset;
	  //hitVec=candidateHits->hits((*iD));	        
	  //NodeResidue(iNode, hitVec, 0);
	  
	  //(iD==candidates.end()) ? iD++ : iD=candidates.begin();
	  //hitVec=candidateHits->hits((*iD));
	  //fill background hist
	  //ResidualBackground(iNode, hitVec);

	}
      iT++;
    }//end while over nodes


  return 1;
   
}

void StiResidualCalculator::NodeResidue(StiKalmanTrackNode iNode,
					vector<StiHit*> hitVec,
					int histVecOffset)
{
  //get node values
  double cross = iNode.crossAngle();
  double dip   = iNode.pitchAngle();
  double nodeZ = iNode.getZ();
  double nodeY = iNode.getY();
  double dy,dz;

  double nodeZE = iNode.getEzz();
  double nodeYE = iNode.getEyy();
  iNode.getDetector()->getHitErrorCalculator()->calculateError(&iNode,nodeYE,nodeZE);
  //cout <<" D: "<<nodeZE<<endl;

  nodeYE =(nodeYE>0)? ::sqrt(nodeYE):1.;
  nodeZE =(nodeZE>0)? ::sqrt(nodeZE):1.;

  vector<StiHit*>::iterator iH = hitVec.begin();
  StiHit* hit;

  while(iH!=hitVec.end())
  {
    hit=*iH;
     //Fill local Y residuals
    if(fabs(hit->y() - nodeY)<10 && fabs(hit->z()-nodeZ)<10)
      {
	dy = hit->y() - nodeY;

    //Fill local Z residuals
	dz = hit->z() - nodeZ;


	FillHist(histVecOffset,nodeZ,nodeY,cross,dip,dz,dy, 
		 dz/nodeZE, dy/nodeYE);
      }//end check on hit pos
     ++iH;
   }//end loop over hits in detector

  return;
}

void StiResidualCalculator::ResidualBackground(StiKalmanTrackNode iNode,
					       vector<StiHit*> hitVec)
{

  StiHit* nearest=0;
  StiHit* hit;
  vector<StiHit*>::iterator iH = hitVec.begin();

  double z = iNode.getZ();
  double y = iNode.getY();

  double dz=0;
  double dy=0;
  double r=0;
  double closestR=0;

  //loop over hits
  while(iH!=hitVec.end())
    {
      hit = *iH;

      dz=hit->z()-z;
      dy=hit->y()-y;

      _BackgroundZ->Fill(dz);
      _BackgroundY->Fill(dy);

      //find closest hit
      r=dz*dz+dy*dy;
      if(r<=closestR) 
	{
	  closestR=r;
	  nearest = hit;
	}
      ++iH;
    }

  _BackgroundClosestZ->Fill(nearest->z()-z);
  _BackgroundClosestY->Fill(nearest->y()-y);
  

}

void StiResidualCalculator::FillHist(int offset, double z, double y,
				     double cross, double dip,
				     double dz, double dy,
				     double dze, double dye)
{
  TH3D* hist;
  int check=0;

  vector<TH3D*>::iterator iH = mYResidualCrossZ.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(cross, z, dy);
  else check=-1;


  iH =mYResidualDipZ.begin();
  hist=*(iH+offset);
  if(hist) hist->Fill(dip,z, dy);
  else check=-1;

  iH = mYDResidualCrossZ.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(cross, z, dye);
  else check=-1;


  iH =mYDResidualDipZ.begin();
  hist=*(iH+offset);
  if(hist) hist->Fill(dip,z, dye);
  else check=-1;

  iH= mZResidualCrossZ.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(cross, z, dz);
  else check=-1;

  iH = mZResidualDipZ.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(dip,z, dz);
  else check=-1;

  iH= mZDResidualCrossZ.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(cross, z, dze);
  else check=-1;

  iH = mZDResidualDipZ.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(dip,z, dze);
  else check=-1;

  double diff = ::sqrt(dy*dy+dz*dz);
  iH = mResidualCrossDip.begin();
  hist = *(iH+offset);
  if(hist) hist->Fill(cross, dip, diff);
  else check=-1;  

  
  vector<TH2D*>::iterator iH2 = mResidualZY.begin();
  TH2D* hist2 = *(iH2+offset);
  if(hist2) hist2->Fill(dz, dy); 
  else check=-1;

  if (check==-1) cout <<"Error in hist finding..."<<endl;
}

void StiResidualCalculator::fillTrackHist(double cross, double dip,
					  double pt, double z)
{
  mCross->Fill(cross);
  mDip->Fill(dip);
  mPt->Fill(pt);
  mDrift->Fill(z);
}
