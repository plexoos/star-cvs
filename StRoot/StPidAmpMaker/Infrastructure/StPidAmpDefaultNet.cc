/***************************************************************************
 *
 * $Id: StPidAmpDefaultNet.cc,v 1.2 2000/04/14 16:07:30 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             StPidAmpDefaultNet is the basic units for building
 *             a default instance of StPidAmpChannelCollection 
 ***************************************************************************
 *
 * $Log: StPidAmpDefaultNet.cc,v $
 * Revision 1.2  2000/04/14 16:07:30  aihong
 * change BetheBlock to BetheBloch :-)
 *
 * Revision 1.1.1.1  2000/03/09 17:48:34  aihong
 * Installation of package
 *
 **************************************************************************/


#include "TF1.h"

#include "StPidAmpMaker/Infrastructure/StPidAmpDefaultNet.hh"

#include "StPidAmpMaker/Include/MaxllBoltz.hh"
#include "StPidAmpMaker/Include/Linear.hh"



//-------------------------------
StPidAmpDefaultNet::StPidAmpDefaultNet():StPidAmpNet(){

  StPidAmpDefaultNet::funcAmpPt=&MaxllBoltz;
  StPidAmpDefaultNet::funcResoPt=&Linear;
}
//-------------------------------
StPidAmpDefaultNet::StPidAmpDefaultNet(StPidAmpParticle def, StPidAmpChannelInfo channelInfo):StPidAmpNet(def, channelInfo){

  StPidAmpDefaultNet::funcAmpPt=&MaxllBoltz;
  StPidAmpDefaultNet::funcResoPt=&Linear;
}

//------------------------------
void StPidAmpDefaultNet::fitBand(TH3D* histo){

  double varyRange=0.1;

  if (histo) histo->SetDirectory(0);

   TF1 *mBetheBlochFcn = new TF1 ("mBetheBlochFcn",funcBandPt, BandsBegin,BandsEnd,NBandParam);
  

   mBetheBlochFcn->SetParLimits(0,(mBandParams[0]-varyRange*fabs(mBandParams[0])),(mBandParams[0]+varyRange*fabs(mBandParams[0])));
   mBetheBlochFcn->SetParLimits(1,(mBandParams[1]-varyRange*fabs(mBandParams[1])),(mBandParams[1]+varyRange*fabs(mBandParams[1])));
   mBetheBlochFcn->SetParLimits(2,(mBandParams[2]-varyRange*fabs(mBandParams[2])),(mBandParams[2]+varyRange*fabs(mBandParams[2])));
          
   mBetheBlochFcn->SetParameter(3,double(mParticleType.charge()));
   mBetheBlochFcn->SetParameter(4,double(mParticleType.mass()));
   mBetheBlochFcn->SetParameter(5, double(CalibFactor));
   mBetheBlochFcn->SetParameter(6, double(Saturation));
   mBetheBlochFcn->SetParLimits(3, 1,1); //fixed.
   mBetheBlochFcn->SetParLimits(4, 1,1);
   mBetheBlochFcn->SetParLimits(5, 1,1);
   mBetheBlochFcn->SetParLimits(6, 1,1);

  if ((bandGraph()->GetN())>0){
   bandGraph()->Fit("mBetheBlochFcn","R"); //R means use the range in TF1.
   mBandParams.clear();
   for (int i=0; i<NBandParam; i++) 
   mBandParams.push_back(mBetheBlochFcn->GetParameter(i));
  }

   delete mBetheBlochFcn;


}



//------------------------------
void StPidAmpDefaultNet::fitAPath(StPidAmpPath& path, StPidAmpTrkVector* trks,TH3D* histo){

     double varyRange4Height=0.35;  
     double varyRange4Center=0.2;
     double varyRange4Width=0.3;

    if (histo) histo->SetDirectory(0);

    double totalTrks=double(trks->size());
    int  idex=getSliceIndex(fabs(mParticleType.maxllPeakPos()));
    double gausCenter=((*sliceVector())[idex])->midBound();

    //use a gauss at peakPos to get the fitting limits for the right path.
    
    TF1 *mGaussFcn = new TF1("mGaussFcn", "gaus",((*sliceVector())[idex])->lowBound(),((*sliceVector())[idex])->highBound());

        mGaussFcn->SetParameter(0,totalTrks*(mParticleType.maxllRatio()));
        mGaussFcn->SetParameter(1,gausCenter);
        mGaussFcn->SetParameter(2,0.15*gausCenter);

   double pathDedx=((gausCenter-(NPaths/2.0)*PathHeight+double(path.index())*PathHeight)>0.0)? (gausCenter-(NPaths/2.0)*PathHeight+double(path.index())*PathHeight) : 0.0;

   double expectedHeight=mGaussFcn->Eval(pathDedx, 0,0);

//now fit the path.
   TF1 *mMaxllBoltzFcn = new TF1 ("mMaxllBoltzFcn",funcAmpPt, BandsBegin,BandsEnd,NAmpParam);
    
   mMaxllBoltzFcn->SetParLimits(0,expectedHeight*(1.0-varyRange4Height),expectedHeight*(1.0+varyRange4Height));
   mMaxllBoltzFcn->SetParLimits(1,fabs(mParticleType.maxllPeakPos())*(1.0-varyRange4Center),fabs(mParticleType.maxllPeakPos())*(1.0+varyRange4Center));
   mMaxllBoltzFcn->SetParLimits(2,(mParticleType.maxllWidth())*(1.0-varyRange4Width),(mParticleType.maxllWidth())*(1.0+varyRange4Width));

   mMaxllBoltzFcn->SetParameter(3,double(fabs(mParticleType.start())));
   mMaxllBoltzFcn->SetParLimits(3, 1,1); //fixed par.
    
    if (((path.pathGraph())->GetN())>0){
     (path.pathGraph())->Fit("mMaxllBoltzFcn","R");
     (path.pathParams())->clear();
   for (int i=0; i<NAmpParam; i++) (path.pathParams())->push_back(mMaxllBoltzFcn->GetParameter(i));
    }

  delete mGaussFcn, mMaxllBoltzFcn;

}

//------------------------------
void StPidAmpDefaultNet::fitAmp(StPidAmpTrkVector* trks,TH3D* histo){

     double totalTrks=double(trks->size());

     double varyRange4Height=0.35;  
     double varyRange4Center=0.3;
     double varyRange4Width=0.3;

     

     /*   if (histo) histo->SetDirectory(0);
//the mParticleType.maxllRatio is not used here, but not sure need it or not 
//when the real data come. so I's rather like to keep this block here.
//cause we do not know how the window would be like in the real case,
//the simple using maxPoint() to decide the heightExpected might be a question

   TF1 *mMaxllBoltzFcn = new TF1 ("mMaxllBoltzFcn",funcAmpPt, BandsBegin,BandsEnd,NAmpParam);
    
   mMaxllBoltzFcn->SetParLimits(0,totalTrks*(mParticleType.maxllRatio())*(1.0-varyRange4Height),totalTrks*(mParticleType.maxllRatio())*(1.0+varyRange4Height));
   mMaxllBoltzFcn->SetParLimits(1,fabs(mParticleType.maxllPeakPos())*(1.0-varyRange4Center),fabs(mParticleType.maxllPeakPos())*(1.0+varyRange4Center));
   mMaxllBoltzFcn->SetParLimits(2,(mParticleType.maxllWidth())*(1.0-varyRange4Width),(mParticleType.maxllWidth())*(1.0+varyRange4Width));
     */


     double heightExpected;

     if (mParticleType.id()==2||mParticleType.id()==3) {
   if (maxPoint(ampGraph(),false)<0.1) heightExpected=(maxPoint(ampGraph(),true))*0.25;
   else heightExpected=maxPoint(ampGraph(),true);
     }


     if (mParticleType.id()==8||mParticleType.id()==9) heightExpected=(maxPoint(ampGraph(),true))*1.4;

     if (mParticleType.id()==14||mParticleType.id()==15) heightExpected=(maxPoint(ampGraph(),true))*1.0;//1.2606;

     if (mParticleType.id()==11||mParticleType.id()==12) heightExpected=(maxPoint(ampGraph(),true))*1.0;//2.0//2.475;


     if (mParticleType.id()==45) heightExpected=totalTrks*(mParticleType.maxllRatio());

     
     double centerExpected=fabs(mParticleType.maxllPeakPos());
     double widthExpected=mParticleType.maxllWidth();


     if (mParticleType.id()==2||mParticleType.id()==3) centerExpected=maxPoint(ampGraph(),false);//electron's amp center has strong dependence of NHits.





   if (histo) histo->SetDirectory(0);

   TF1 *mMaxllBoltzFcn = new TF1 ("mMaxllBoltzFcn",funcAmpPt, BandsBegin,BandsEnd,NAmpParam);
    
   mMaxllBoltzFcn->SetParLimits(0,heightExpected*(1.0-varyRange4Height),heightExpected*(1.0+varyRange4Height));
   mMaxllBoltzFcn->SetParLimits(1,centerExpected*(1.0-varyRange4Center),centerExpected*(1.0+varyRange4Center));
   mMaxllBoltzFcn->SetParLimits(2,widthExpected*(1.0-varyRange4Width),widthExpected*(1.0+varyRange4Width));


   mMaxllBoltzFcn->SetParameter(3,double(fabs(mParticleType.start())));
   mMaxllBoltzFcn->SetParLimits(3, 1,1); //fixed par.
    
  if ((ampGraph()->GetN())>0){
     ampGraph()->Fit("mMaxllBoltzFcn","R");
     mAmpParams.clear();
     for (int i=0; i<NAmpParam; i++) mAmpParams.push_back(mMaxllBoltzFcn->GetParameter(i));
  }

  delete mMaxllBoltzFcn;

}
//----------------------------------
void StPidAmpDefaultNet::fitReso(){
    double adj=0.2;

  TF1 *mResoFcn = new TF1 ("mResoFcn",funcResoPt, BandsBegin,BandsEnd,NResoParam);

  /*   switch(mParticleType.id()){

        case 2  :  mResoFcn->SetParLimits(0,0.16*(1.0-adj),0.16*(1.0+adj));
	              break;// ePlus
        case 3  :  mResoFcn->SetParLimits(0,0.16*(1.0-adj),0.16*(1.0+adj));
	              break;//eMinus
        case 8  :  mResoFcn->SetParLimits(0,0.12*(1.0-adj),0.12*(1.0+adj));
	              break;//piPlus
        case 9  :  mResoFcn->SetParLimits(0,0.12*(1.0-adj),0.12*(1.0+adj));
	              break;//piMinus
        case 11 :  mResoFcn->SetParLimits(0,0.08*(1.0-adj),0.135*(1.0+adj));
	              break;//kaonPlus
        case 12 :  mResoFcn->SetParLimits(0,0.08*(1.0-adj),0.135*(1.0+adj));
	              break;//kaonMinus
        case 14 :  mResoFcn->SetParLimits(0,0.07*(1.0-adj),0.12*(1.0+adj));
	              break;//proton
        case 15 :  mResoFcn->SetParLimits(0,0.07*(1.0-adj),0.12*(1.0+adj));
	              break;//proton
        case 45 :  mResoFcn->SetParLimits(0,0.14*(1.0-adj),0.14*(1.0+adj));
	              break;//deuteron
        default :  mResoFcn->SetParLimits(0,0.13*(1.0-adj),0.13*(1.0+adj));

	}
  */
        mResoFcn->SetParLimits(1,-0.05517,0.055172414); //almost flat

     if (mParticleType.id()==2 || mParticleType.id()==3) {
          mResoFcn->SetParLimits(0,0.14*(1.0-0.05),0.14*(1.0+0.05));
          mResoFcn->SetParLimits(1,-0.02517,0.025172414);
     }

       if ((linrGraph()->GetN())>0){
        linrGraph()->Fit("mResoFcn","R");
        mResoParams.clear();
   for (int i=0; i<NResoParam; i++) mResoParams.push_back(mResoFcn->GetParameter(i));
       }

        delete mResoFcn;

}
//------------------------------
ostream& StPidAmpDefaultNet::put(ostream& s) const{// for calling the right put() by operator.

  s<<endl;
  s<<name().c_str()<<endl;
 
  for (int i=0; i<mBandParams.size(); i++) s<<mBandParams[i]<<" ";
  s<<endl;
  for (int j=0; j<mAmpParams.size(); j++) s<<mAmpParams[j]<<" ";
  s<<endl;
  for (int k=0; k<mResoParams.size(); k++) s<<mResoParams[k]<<" ";
  s<<endl;
    
  return s;
}
//----------------------------------
void StPidAmpDefaultNet::fillPathFittedSlices(){

   StPidAmpPathIter pathIter;
   StPidAmpPath*    thePath;

   for (pathIter=pathVector()->begin(); pathIter!=pathVector()->end(); pathIter++){//path loop

   thePath=*pathIter;
   TF1 *mMaxllBoltzFcn = new TF1 ("mMaxllBoltzFcn",funcAmpPt, BandsBegin,BandsEnd,NAmpParam);
   
   for(int i=0; i<NAmpParam; i++) mMaxllBoltzFcn->SetParameter(i,((*(thePath->pathParams())))[i]);
   
   // mMaxllBoltzFcn->SetParameter(NAmpParam,double(fabs(mParticleType.start())));

       
   StPidAmpSliceIter sliceIter;
   StPidAmpSlice* theSlice;

   for (sliceIter=sliceVector()->begin(); sliceIter!=sliceVector()->end(); sliceIter++){//slice loop

   theSlice=*sliceIter;
   (theSlice->pathFittedSlice())->SetBinContent((thePath->index()+1), mMaxllBoltzFcn->Eval(fabs(theSlice->meanRig()),0,0));
   //path index begin with 0, bin # begin with 1.

   }
   
   delete mMaxllBoltzFcn, theSlice;

   }

   //now turn on StPidAmpSlice::mUsePathFitResult       
   StPidAmpSliceIter sliceIter;
   StPidAmpSlice* theSlice;
   for (sliceIter=sliceVector()->begin(); sliceIter!=sliceVector()->end(); sliceIter++){//slice loop
     theSlice=*sliceIter;
     theSlice->usePathFitResult();
   }


}




