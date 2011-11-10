//
//  $Id: StFgtSimpleClusterAlgo.cxx,v 1.18 2011/11/10 23:59:22 avossen Exp $
//  $Log: StFgtSimpleClusterAlgo.cxx,v $
//  Revision 1.18  2011/11/10 23:59:22  avossen
//  modified simple cluster algo so that it should find phi clusters with R<19
//
//  Revision 1.17  2011/11/09 01:53:04  avossen
//  changed weighting by adc to weighting by charge
//
//  Revision 1.16  2011/11/03 20:04:17  avossen
//  updated clustering makers and algos to reflect new containers
//
//  Revision 1.15  2011/11/03 15:54:05  avossen
//  fixed error for last cluster on disk
//
//  Revision 1.14  2011/11/03 15:00:10  sgliske
//  Error estimate set to st. dev. of the ordinate
//
//  Revision 1.13  2011/11/02 18:44:45  sgliske
//  updated for changed StFgtHit constructor:
//  changed saving central strip ptr to geoId in StFgtHit
//
//  Revision 1.12  2011/11/01 18:46:30  sgliske
//  Updated to correspond with StEvent containers, take 2.
//
//  Revision 1.11  2011/10/27 14:18:25  avossen
//  minor update
//
//  Revision 1.10  2011/10/26 20:56:50  avossen
//  use geoIds to determine if two strips are adjacent
//
//  Revision 1.9  2011/10/14 18:45:27  avossen
//  fixed some bugs in simple cluster algo
//
//  Revision 1.8  2011/10/13 20:35:22  balewski
//  cleanup, added missing return value
//
//  Revision 1.7  2011/10/10 20:35:08  avossen
//  fixed strip-cluster association in MaxCluster algo, made other files cvs compliant
//
//
// \class StFgtSimpleClusterAlgo
// \author Anselm Vossen (avossen@indiana.edu)
//

#include "StFgtSimpleClusterAlgo.h"
#include "StRoot/StFgtUtil/geometry/StFgtGeom.h"

#include "StRoot/StEvent/StFgtStripCollection.h"
#include "StRoot/StEvent/StFgtStrip.h"
#include "StRoot/StEvent/StFgtHitCollection.h"
#include "StRoot/StEvent/StFgtHit.h"
//for floor
#include <math.h>

StFgtSimpleClusterAlgo::StFgtSimpleClusterAlgo():mIsInitialized(0)
{
  //nothing else to do....
};

Int_t StFgtSimpleClusterAlgo::Init()
{
  mIsInitialized=true;
  return 0; // ?,jan
};

Int_t StFgtSimpleClusterAlgo::doClustering( StFgtStripCollection& strips, StFgtHitCollection& clusters )
{
  //  cout.precision(10);
  //we make use of the fact, that the hits are already sorted by geoId
  strips.sortByGeoId();

  Float_t defaultError = 0.001;

  Short_t disc, quadrant,prvDisc,prvQuad;
  Char_t layer,prvLayer,noLayer='z';
  Double_t ordinate, lowerSpan, upperSpan, prvOrdinate;
  Int_t prvGeoId;
  Double_t accuCharge=0; 
  Double_t meanOrdinate=0;
  Double_t meanSqOrdinate=0;
  Int_t numStrips=0;
  //bool lookForNewCluster=true;
  prvLayer=noLayer;
  bool isPhi, isR;
  StFgtHit* newCluster=0;
  //to compute energy weighted strip id
  Double_t meanGeoId=0;
  //for R < 19 cm the difference in geo id of the phi strips is 2 and only even numbers are used...
  bool stepTwo=false;
  //const 
  for( StSPtrVecFgtStripIterator it=strips.getStripVec().begin();it!=strips.getStripVec().end();++it)
    {
      StFgtGeom::getPhysicalCoordinate((*it)->getGeoId(),disc,quadrant,layer,ordinate,lowerSpan,upperSpan);

        if((layer=='R')&& ordinate < 19)
	{
	  stepTwo=true;
	  break;
	}

    }

  for( StSPtrVecFgtStripIterator it=strips.getStripVec().begin();it!=strips.getStripVec().end();++it)
    {
      StFgtGeom::getPhysicalCoordinate((*it)->getGeoId(),disc,quadrant,layer,ordinate,lowerSpan,upperSpan);
      isPhi=(layer=='P');
      isR=(!isPhi);


      if(prvLayer==noLayer)//first hit
	{

	  newCluster=new StFgtHit(clusters.getHitVec().size(),meanGeoId,accuCharge, disc, quadrant, layer, ordinate, defaultError,ordinate, defaultError,0.0,0.0);
          stripWeightMap_t &stripWeightMap = newCluster->getStripWeightMap();
          stripWeightMap[ *it ] = 1;

	  //	  cout <<" adc: " << accuCharge <<" geoId: " << (*it)->getGeoId() <<" meangeo: ";
	  accuCharge=(*it)->getCharge();  
	  meanGeoId=(*it)->getCharge()*(*it)->getGeoId();
	  //	  cout << meanGeoId<<endl;

	  meanOrdinate=ordinate;
	  meanSqOrdinate=ordinate*ordinate;
	  prvLayer=layer;
	  prvGeoId=(*it)->getGeoId();
	  numStrips=1;
	  prvOrdinate=ordinate;
	  //go to next hit
	  continue;
	}

      //      bool adjacentStrips=(((abs(prvOrdinate-ordinate)<StFgtGeom::kFgtPhiAnglePitch) &&isPhi)|| ((abs(prvOrdinate-ordinate)<StFgtGeom::kFgtRadPitch) && isR));
      //so either 
      bool adjacentStrips=((abs(prvGeoId-(*it)->getGeoId())<2)|| (( abs(prvGeoId-(*it)->getGeoId())==2) && stepTwo && isPhi && (prvGeoId%2==0)));

      //if the strip is adjacent to the last one? Then we add it to the cluster
      if((layer==prvLayer && adjacentStrips)&& prvLayer!=noLayer) 
	{
	  //should really be charge...
	  //accuCharge+=(*it)->getCharge();  
	  accuCharge+=(*it)->getCharge();
	  meanOrdinate+=ordinate;

	  meanGeoId+=(*it)->getCharge()*(*it)->getGeoId();
	  //	  cout<<"accuCharge is: " << accuCharge <<endl;
	  //	  cout <<"meango: " << (*it)->getCharge() <<" * " << (*it)->getGeoId() <<" meanGeoId now " << meanGeoId <<endl;

	  meanSqOrdinate+=ordinate*ordinate;
	  numStrips++;

          stripWeightMap_t &stripWeightMap = newCluster->getStripWeightMap();
          stripWeightMap[ *it ] = 1;
	  prvLayer=layer;
	  prvGeoId=(*it)->getGeoId();
	  prvOrdinate=ordinate;
	  continue;
	}
      else
	{//we are looking at a new cluster because we are not at the beginning and the new strip is not adjacent to the old one
	  //set charge, push back cluster, start new one
	  //set layer etc of cluster

	  //	  cout <<"setting charge of new cluster to " << accuCharge <<endl;

	  newCluster->setCharge(accuCharge);
          // compute mean and st. dev.
          meanOrdinate /= (float)numStrips;
	  //	  cout <<" geo id is : " <<  meanGeoId <<" / " << accuCharge;
	  meanGeoId /= accuCharge;

	  //	  cout <<" = " << meanGeoId <<" ends up as :  " << floor(meanGeoId+0.5)<<endl;

          meanSqOrdinate /= (float)numStrips;
          meanSqOrdinate -= meanOrdinate*meanOrdinate;
          if( meanSqOrdinate > 0 )
             meanSqOrdinate = sqrt(meanSqOrdinate);
          // meanSqOrdinate is now the st. dev. of the ordinate
          // avoid unreasonable small uncertainty, due to small cluster sizes
          Double_t pitch = ( layer == 'R' ? StFgtGeom::radStrip_pitch() : StFgtGeom::phiStrip_pitch() );
          if( meanSqOrdinate < 2*pitch )
             meanSqOrdinate = 2*pitch;

	  if(layer=='R')
	    {
	      newCluster->setPositionR(meanOrdinate );
	      newCluster->setErrorR(meanSqOrdinate);
	    }
	  else
	    {
	      newCluster->setPositionPhi(meanOrdinate );
	      newCluster->setErrorPhi(meanSqOrdinate);
	    }
	  newCluster->setCentralStripGeoId(floor(meanGeoId+0.5));
	  if(numStrips<=10)
             clusters.getHitVec().push_back(newCluster);
          else
             delete newCluster;
	  //	      cout <<"cluster has size: " << numStrips <<endl;
	  //
	  accuCharge=(*it)->getCharge();
	  meanOrdinate=ordinate;
	  meanGeoId=(*it)->getCharge()*(*it)->getGeoId();
	  //	  cout<<" geo ID: " << (*it)->getGeoId() <<" charge: " << (*it)->getCharge() <<endl;
          meanSqOrdinate=ordinate*ordinate;
	  numStrips=1;

	  //	  cout << " starting new cluster with " << meanGeoId <<" and charge: " << accuCharge <<endl;

	  newCluster=new StFgtHit(clusters.getHitVec().size(),meanGeoId,accuCharge, disc, quadrant, layer, ordinate, defaultError,ordinate, defaultError,0.0,0.0);
	  //add the current stuff
          stripWeightMap_t &stripWeightMap = newCluster->getStripWeightMap();
          stripWeightMap[ *it ] = 1;
	  prvLayer=layer;
	  prvGeoId=(*it)->getGeoId();
	  prvDisc=disc;
	  prvQuad=quadrant;
	  prvOrdinate=ordinate;
	}
    }

  //if there has been any 1+ clusters, we have to add the last cluster to the list
  if(newCluster)
    {
      //new cluster was started but not included yet..
      //      newCluster->setPosition1D(meanOrdinate/(float)numStrips, defaultError );
      meanOrdinate /= (float)numStrips;
      //      cout <<" finishing stuff up, mean geo: " << meanGeoId << " accucharge: " << accuCharge;
      meanGeoId/=accuCharge;
      //      cout <<" and corrected: " << meanGeoId<<endl;
      newCluster->setCharge(accuCharge);
      meanSqOrdinate /= (float)numStrips;
      meanSqOrdinate -= meanOrdinate*meanOrdinate;
      if( meanSqOrdinate > 0 )
	meanSqOrdinate = sqrt(meanSqOrdinate);

      // meanSqOrdinate is now the st. dev. of the ordinate

      // avoid unreasonable small uncertainty, due to small cluster sizes
      Double_t pitch = ( layer == 'R' ? StFgtGeom::radStrip_pitch() : StFgtGeom::phiStrip_pitch() );
      if( meanSqOrdinate < 2*pitch )
	meanSqOrdinate = 2*pitch;
      if(layer=='R')
	{
	  newCluster->setPositionR(meanOrdinate );
	  newCluster->setErrorR(meanSqOrdinate);
	}
      else
	{
	  newCluster->setPositionPhi(meanOrdinate );
	  newCluster->setErrorPhi(meanSqOrdinate);
	}

      //      cout <<"setting central strip to " << floor(meanGeoId+0.5)<<endl;

      newCluster->setCentralStripGeoId(floor(meanGeoId+0.5));
      if(numStrips<=10)
         clusters.getHitVec().push_back(newCluster);
      else
         delete newCluster;
    }

  return kStOk;
};

ClassImp(StFgtSimpleClusterAlgo);
