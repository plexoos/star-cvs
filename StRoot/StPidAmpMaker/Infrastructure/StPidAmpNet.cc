/***************************************************************************
 *
 * $Id: StPidAmpNet.cc,v 1.9 2000/07/12 15:38:35 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             Abstract class for nets
 ***************************************************************************
 *
 * $Log: StPidAmpNet.cc,v $
 * Revision 1.9  2000/07/12 15:38:35  aihong
 * update for real data
 *
 * Revision 1.8  2000/07/06 01:55:15  perev
 * fix related to ROOT float -> double
 *
 * Revision 1.7  2000/05/01 16:59:25  aihong
 * clean up
 *
 * Revision 1.6  2000/04/14 16:07:30  aihong
 * change BetheBlock to BetheBloch :-)
 *
 * Revision 1.5  2000/04/14 14:30:53  aihong
 * bug fixed in pushATrk()
 *
 * Revision 1.4  2000/04/11 15:34:23  aihong
 * change to adapt dividing trks by channel for faster filling
 *
 * Revision 1.3  2000/04/09 16:17:29  aihong
 * screen out bad pion region for band fitting
 *
 * Revision 1.2  2000/03/22 14:08:25  aihong
 * reduce unnecessary bound checking for faster run
 *
 * Revision 1.1.1.1  2000/03/09 17:48:34  aihong
 * Installation of package
 *
 **************************************************************************/


#include "TCanvas.h"
#include "TF1.h"
#include <strstream.h>
#include <fstream.h>
#include "StMessMgr.h"

#include "StPidAmpMaker/Infrastructure/StPidAmpNet.hh"
//#include "StPidAmpMaker/Include/BetheBloch.hh"
#include "StPidAmpMaker/Include/BetheBlochFunction.hh"

#include "StPidAmpMaker/Infrastructure/StPidAmpChannelCollection.hh"

//----------------------------------
StPidAmpNet::StPidAmpNet(){

  /* no-op. */
}
//----------------------------------
StPidAmpNet::~StPidAmpNet(){

  /* no-op. */
}


//----------------------------------
StPidAmpNet::StPidAmpNet(StPidAmpParticle def, StPidAmpChannelInfo channelInfo){

      gMessMgr->Info()<<" constructing "<<def.name().c_str()<<" net "<<endm;

      mParticleType=def; //do not move this around.

      setBandParams(StPidAmpNet::mDefaultBandParams);

       mChannelInfo=channelInfo;
         mNetWindow=def.window();

         mBandGraph=new TGraph();
          mAmpGraph=new TGraph();
         mResoGraph=new TGraph();
      mSliceCollect=new StPidAmpSliceVector();
       mPathCollect=new StPidAmpPathVector();

      mName.append(mParticleType.name());
      mName.append(mChannelInfo.name());

      mDrawSlicesFit=false;
       mDrawPathsFit=false;
         mDrawAmpFit=false;
        mDrawBandFit=false;
        mDrawResoFit=false;

            mFitBand=false;
            mFitPath=false;
             mFitAmp=true;
            mFitReso=true;

	    // StPidAmpNet::funcBandPt=&BetheBloch;
           StPidAmpNet::funcBandPt=&BetheBlochFunction;

     if(mParticleType.id()>0) setUp(); //set up slices and paths.
     //although setUp() is declared as virtual, in the constructor a 
     //call to setUp() does not call the setUp() in the derivated class,
     //so we have to use id()>0 as a flag.
 }
//----------------------------------
StPidAmpNet::StPidAmpNet(const StPidAmpNet& net){
  //have not update this method for a while. not garranteed to work.
      mAmpParams  = net.mAmpParams;
      mBandParams = net.mBandParams;
      mResoParams = net.mResoParams;

      mSliceCollect  = new StPidAmpSliceVector();
      *mSliceCollect = *(net.mSliceCollect);
      mPathCollect   = new StPidAmpPathVector();
      *mPathCollect  = *(net.mPathCollect);

      mBandGraph     = new TGraph();
      *mBandGraph    = *(net.mBandGraph);
      mAmpGraph      = new TGraph();
      *mAmpGraph     = *(net.mAmpGraph);
      mResoGraph     = new TGraph();
      *mResoGraph    = *(net.mResoGraph);
      mParticleType  = net.mParticleType;
      mChannelInfo   = net.mChannelInfo;
      mNetWindow     = net.mNetWindow;

      mName=net.mName;

      mDrawSlicesFit = net.mDrawSlicesFit;
      mDrawPathsFit  = net.mDrawPathsFit;
      mDrawAmpFit    = net.mDrawAmpFit;
      mDrawBandFit   = net.mDrawBandFit;
      mDrawResoFit   = net.mDrawResoFit;

      mFitBand = net.mFitBand;
      mFitPath = net.mFitPath;
      mFitAmp  = net.mFitAmp;
      mFitReso = net.mFitReso;

      funcBandPt=net.funcBandPt;

      // do I need to copy the static member?

}

//----------------------------------
double StPidAmpNet::dedxAtBandCenter(double rig){
  //always calculate the dedx at band center by using 
  //parameters in mBandParams.
       
    TF1 mBandBetheBlochFcn("mBandBetheBlochFcn",funcBandPt, BandsBegin,BandsEnd, NBandParam);

       mBandBetheBlochFcn.SetParameter(0,mBandParams[0]);
       mBandBetheBlochFcn.SetParameter(1,mBandParams[1]);
       mBandBetheBlochFcn.SetParameter(2,mBandParams[2]);
        
       mBandBetheBlochFcn.SetParameter(3,double(mParticleType.charge()));
       mBandBetheBlochFcn.SetParameter(4,mParticleType.mass());
       mBandBetheBlochFcn.SetParameter(5, double(CalibFactor));
       mBandBetheBlochFcn.SetParameter(6, double(Saturation));

       return mBandBetheBlochFcn.Eval(fabs(rig),0,0);
}
//----------------------------------
void StPidAmpNet::fillNet(StPidAmpTrkVector* trks, StPidAmpChannelCollection* set){
// fill particles into net, will call fillSilces and fillPaths

  if (set)gMessMgr->Info()<<"filling "<<mParticleType.name().c_str()<<" tracks to mBGNet(s)... "<<endm;
  else  gMessMgr->Info()<<"filling "<<mParticleType.name().c_str()<<" net... "<<endm;

         fillSlices(trks,set);
         if (mFitPath)  fillPaths();
           
}

//----------------------------------
void StPidAmpNet::setUp(){//setUp based on mBandParams always.

       int i=0;

  if (mSliceCollect->size()>0) mSliceCollect->clear();
  if (mPathCollect->size()>0)   mPathCollect->clear();//clear() not only 
                                                   //removes all elements,
                                                  //but also forces the
                                                 // capacity to zero!!

   int sz=int(fabs(mParticleType.end()-mParticleType.start())/SliceWidth)+10;
   mSliceCollect->reserve(sz); //reserve capacity
   mPathCollect->reserve(5);

       //set up slices
 
 SliceLoop: if ( ((i+1.0)*SliceWidth+fabs(mParticleType.start()))<fabs(mParticleType.end() ) ) {

 double midRig=SliceWidth*(double(i)+0.5)+fabs(mParticleType.start());
 double lowBd=dedxAtBandCenter(midRig)- PathHeight*double(NPaths)/2.0;
 double highBd=dedxAtBandCenter(midRig)+ PathHeight*double(NPaths)/2.0;  

 StPidAmpSlice* ASlice= new StPidAmpSlice(i,midRig,lowBd, highBd, SliceWidth,mName, &mParticleType);
 mSliceCollect->push_back(ASlice);
 i++;
 goto SliceLoop;
 }
 


   //release unsused space back to memory.
   //construct a copy, whose capacity=size. 
   //after interchange (swapping) with the copy,the origin's capacity=size.
   //recommend to use this trick for pointer containers only.
   //for obj containers, coz the copying happens
   //the polymorphism will not be preserved.

       StPidAmpSliceVector tmpCollect=*mSliceCollect;
       mSliceCollect->swap(tmpCollect);
      


       //set up paths
       if (mFitPath){
        int j=0;

       for (j=0; j<NPaths; j++) {
   StPidAmpPath* APath=new StPidAmpPath(j, &mParticleType, mNetWindow,mName);
   mPathCollect->push_back(APath);
       }

       }
       //zeroing mParticleType in slices in merging area.

       StPidAmpSliceIter iter;
       StPidAmpSlice*    slicePtr;

       for ( iter=mSliceCollect->begin(); iter!=mSliceCollect->end(); iter++){
       slicePtr=*iter;
       if ( !( (mNetWindow.isInWindow(slicePtr->leftEdge())) &&  (mNetWindow.isInWindow(slicePtr->rightEdge())) ) ) slicePtr->setParticleType(0);
       //if a slice is not in clean windows, zero its mPartilceType
       }

       fillNetOut();//for StPidChannelCollection::writeBGBands2Disk() 'sake.
       //cause if drawOpt ="B" only, channel is not processed, 
    //that means netOUt is not filled. but in StPidChannelCollection::writeBGBands2Disk()
       // we still need to gid out netOut, so better to fill it during construction.
    


}

//----------------------------------
void StPidAmpNet::fillBand(){ //fill band graph

  StPidAmpSliceIter iter;
  StPidAmpSlice* theSlice;
  //add a clean window to mNetWindow, for pi rel rising part.
  //just for coding convenience.
  if (mParticleType.id()==8 || mParticleType.id()==9) 
  mNetWindow.addWindow(2.5,4.5);


  for( iter=mSliceCollect->begin(); iter!=mSliceCollect->end(); iter++) {
    theSlice=*iter;
    bool storedSth=((theSlice->slice()->GetMaximum())>0);



    //if a slice is in window, put its dedx and rig into graph.
    if (mNetWindow.isInWindow(fabs(theSlice->meanRig())) && storedSth && ((theSlice->sliceInfo()->meanDedx())!=0.0)) mBandGraph->SetPoint(mBandGraph->GetN(), float(fabs(theSlice->meanRig())),float(fabs(theSlice->sliceInfo()->meanDedx()))); 
  }

 

  if (mParticleType.id()==8 || mParticleType.id()==9) 
  mNetWindow.removeLastWindow();

}

//----------------------------------
void StPidAmpNet::fillAmp(){//fill amp graph

   StPidAmpSliceIter iter;
   StPidAmpSlice*    theSlice;
   bool     outputPoints4Tunning=false; //output some info. for better tunning.
  
  ofstream protonAmp;
  ofstream kaonAmp;
  ofstream pionAmp;
  ofstream deuteronAmp;
  ofstream electronAmp;

  if (outputPoints4Tunning){
    protonAmp.open("protonAmpGraph.txt",ios::app);
      kaonAmp.open("kaonAmpGraph.txt",ios::app);
      pionAmp.open("pionAmpGraph.txt",ios::app);
  deuteronAmp.open("deuteronGraph.txt",ios::app);
  electronAmp.open("electronGraph.txt",ios::app);
  }

  for (iter=mSliceCollect->begin(); iter!=mSliceCollect->end(); iter++) {
     theSlice=*iter;
     bool storedSth=((theSlice->slice()->GetMaximum())>0);
     if (mNetWindow.isInWindow(fabs(theSlice->meanRig())) && storedSth) {
mAmpGraph->SetPoint(mAmpGraph->GetN(),float(fabs(theSlice->meanRig())),float((theSlice->sliceInfo())->amp()));

  if (outputPoints4Tunning){
 if (mParticleType.id()==14)   protonAmp<<"gr->SetPoint("<<(mAmpGraph->GetN()-1)<<","<<float(fabs(theSlice->meanRig()))<<","<<float((theSlice->sliceInfo())->amp())<<");"<<endl;
 if (mParticleType.id()==11)     kaonAmp<<"gr->SetPoint("<<(mAmpGraph->GetN()-1)<<","<<float(fabs(theSlice->meanRig()))<<","<<float((theSlice->sliceInfo())->amp())<<");"<<endl;
 if (mParticleType.id()==8 )     pionAmp<<"gr->SetPoint("<<(mAmpGraph->GetN()-1)<<","<<float(fabs(theSlice->meanRig()))<<","<<float((theSlice->sliceInfo())->amp())<<");"<<endl;
 if (mParticleType.id()==45) deuteronAmp<<"gr->SetPoint("<<(mAmpGraph->GetN()-1)<<","<<float(fabs(theSlice->meanRig()))<<","<<float((theSlice->sliceInfo())->amp())<<");"<<endl;
 if (mParticleType.id()==3 ) electronAmp<<"gr->SetPoint("<<(mAmpGraph->GetN()-1)<<","<<float(fabs(theSlice->meanRig()))<<","<<float((theSlice->sliceInfo())->amp())<<");"<<endl;


  }

     }
  }

mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.0,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.0,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.01,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.02,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.03,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.04,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.05,0.0);
mAmpGraph->SetPoint(mAmpGraph->GetN(), 4.06,0.0);


  if (outputPoints4Tunning){
     protonAmp.close();
       kaonAmp.close();
       pionAmp.close();
   deuteronAmp.close();
  }


}
//----------------------------------
void StPidAmpNet::fillReso(){
   StPidAmpSliceIter iter;
   StPidAmpSlice*    theSlice;

   int NStoredBin=0;
   int i;

   
   TH1D temp("temp", "temp",mSliceCollect->size(),mSliceCollect->front()->leftEdge(), mSliceCollect->back()->rightEdge());

  for (iter=mSliceCollect->begin(); iter!=mSliceCollect->end(); iter++) {
            theSlice=*iter;
    if (mNetWindow.isInWindow(fabs(theSlice->meanRig()))&& (theSlice->sliceInfo()->meanDedx()>0.0)){
     temp.SetBinContent(getSliceIndex(fabs(theSlice->meanRig())),((theSlice->sliceInfo())->sigma()/fabs((theSlice->sliceInfo())->meanDedx())));
     NStoredBin++;
    }
  }

   if (NStoredBin>10) {
     for (i=0; i<int(NStoredBin*0.1); i++)
    temp.SetBinContent(temp.GetMaximumBin(),0.0);
  }//cut off bad data due to the entering part of the band.
   //in those areas, we do not have good statistics, the resolution we got
   //from there is not much useful.
  
  
  for (i=1; i<(temp.GetNbinsX()+1); i++){
    if (temp.GetBinContent(i)>0.0) 
    mResoGraph->SetPoint(mResoGraph->GetN(),temp.GetBinCenter(i),temp.GetBinContent(i));
  }

   


  /*  for (iter=mSliceCollect->begin(); iter!=mSliceCollect->end(); iter++) {
     theSlice=*iter;
     if (mNetWindow.isInWindow(fabs(theSlice->meanRig()))&& (theSlice->sliceInfo()->meanDedx()!=0.0))
       mResoGraph->SetPoint(mResoGraph->GetN(), float(fabs(theSlice->meanRig())),float(((theSlice->sliceInfo())->sigma()/fabs((theSlice->sliceInfo())->meanDedx()))));
  }

  */


}

//----------------------------------
void StPidAmpNet::adjudgeWindow() {
  //adjude clean window based on particle::window.
  //the reason to adjudge is that window is also 
  //depends on channel.


  /* not critical.  no-op yet */
}






//----------------------------------
void StPidAmpNet::setBandParams(StPidParamVector& pars){
  // if ChannelCollection choose to fit betaGamma,
  //get fitting result from ChannelCollection and override mBandParam.
  // otherwise this function do not need to be 
  //called, and the band Param for this Net is kept.

  if (mBandParams.size()>0) mBandParams.clear();
      mBandParams.reserve(NBandParam);

  for (int i=0; i<NFitBandParam; i++)
   mBandParams.push_back(pars[i]);

   mBandParams.push_back(double(mParticleType.charge()));
   mBandParams.push_back(mParticleType.mass());
   mBandParams.push_back(double(CalibFactor));
   mBandParams.push_back(double(Saturation));

  //Be aware that StPidAmpNet::setBandParams(pars)
  //just store the first three params of pars.
  //because the StPidAmpNet::mBandParams wont be refresh unless the fitBand() function get called.
  //so to avoid all mBandParams comes to be idential with mBGNet's mBandParams,
  //we choose just pick out the first "real" band parameters.
  //the other 4 will be filled by a net's own "mass, charge etc..

}



//----------------------------------
void StPidAmpNet::fillBetaGammaNet(StPidAmpTrkVector* trks,StPidAmpChannelCollection* set){
   
 //fill histo. of betaGamma from ChannelCollection.


  fillNet(trks,set);
    
}  
   
//----------------------------------
void StPidAmpNet::fitPaths(StPidAmpTrkVector* trks){
  
  StPidAmpPathIter pathIter;
  StPidAmpPath*    thePath;

  for(pathIter=mPathCollect->begin(); pathIter!=mPathCollect->end(); pathIter++){
  thePath=*pathIter;
  fitAPath(*thePath,trks);
  }  


}

//----------------------------------
void StPidAmpNet::drawNetFittings(){

  unsigned NPads=12;
  strstream stPath,stBand,stAmp,stReso;

  strstream stSlice[NWindows4BG];

 for (int mm=0; mm<NWindows4BG; mm++) 
   stSlice[mm]<<mName.c_str()<<"slicesFittings at window  "<<mNetWindow.windowBegin(mm+1)<<" - "<<mNetWindow.windowEnd(mm+1);

   stPath<<mName.c_str()<<"pathsFittings ";
   stBand<<mName.c_str()<<"bandFitting ";
    stAmp<<mName.c_str()<<"ampFitting ";
   stReso<<mName.c_str()<<"linrFitting ";

  if (mDrawBandFit){//draw band fitting
    TCanvas* theBandCanvas=new TCanvas(stBand.str(),stBand.str(),20,10,800,600);
   theBandCanvas->cd(); 
   mBandGraph->Draw("A*");
  }

  if (mDrawAmpFit){//draw amp fitting
    TCanvas* theAmpCanvas=new TCanvas(stAmp.str(),stAmp.str(),20,10,800,600);
    theAmpCanvas->cd();
    mAmpGraph->Draw("A*");
  }

  if (mDrawResoFit){//resolution fitting
    TCanvas* theResoCanvas=new TCanvas(stReso.str(),stReso.str(),20,10,800,600);
    theResoCanvas->cd();
    mResoGraph->Draw("A*");
  }


  if (mDrawPathsFit){//draw all 11 pathFittings.
   TCanvas* thePathCanvas=new TCanvas(stPath.str(),stPath.str(),20,10,800,600);
   thePathCanvas->Divide(3,4);//3X4=NPads.//this is an error prone.
  //should be robust.
   
   for (unsigned j=0; j<=mPathCollect->size(); j++){
    if ((j+1)>NPads) break;
    thePathCanvas->cd(j+1);
    ((*mPathCollect)[j])->pathGraph()->Draw("A*");
   }//attention that idex of pads of canvas begin with 1.
   //but our paths index begin with 0.
  thePathCanvas->cd();
  }


  if (mDrawSlicesFit){ //select ~12 slices from clean windows and draw histo.
 for (int NN=0; NN<mNetWindow.NWindows(); NN++)
 drawSlicesInASegment(stSlice[NN],mNetWindow.windowBegin(NN+1),mNetWindow.windowEnd(NN+1));
  


 
  }


 }

//----------------------------------
void StPidAmpNet::fitBand(){

  //this method should be clear up.
   double varyRange=0.3;


   TF1 *mBetheBlochFcn = new TF1 ("mBetheBlochFcn",funcBandPt, fabs(mParticleType.start()),fabs(mParticleType.end()),NBandParam);
   //in derived classes, range should be BandsBegin, BandsEnd.
  
  

   mBetheBlochFcn->SetParLimits(0,(mBandParams[0]-varyRange*fabs(mBandParams[0])),(mBandParams[0]+varyRange*fabs(mBandParams[0])));
   mBetheBlochFcn->SetParLimits(1,(mBandParams[1]-varyRange*fabs(mBandParams[1])),(mBandParams[1]+varyRange*fabs(mBandParams[1])));
   mBetheBlochFcn->SetParLimits(2,(mBandParams[2]-varyRange*fabs(mBandParams[2])),(mBandParams[2]+varyRange*fabs(mBandParams[2])));
          
   mBetheBlochFcn->SetParameter(3,double(mParticleType.charge()));//charge is 1 for mBGNet.
   mBetheBlochFcn->SetParameter(4,mParticleType.mass());//mass is 1 for mBGNet.
   mBetheBlochFcn->SetParameter(5, double(CalibFactor));
   mBetheBlochFcn->SetParameter(6, double(Saturation));
   mBetheBlochFcn->SetParLimits(3, 1,1); //fixed.
   mBetheBlochFcn->SetParLimits(4, 1,1);
   mBetheBlochFcn->SetParLimits(5, 1,1);
   mBetheBlochFcn->SetParLimits(6, 1,1);



 if ((bandGraph()->GetN())>0) {
   bandGraph()->Fit("mBetheBlochFcn","R"); //R means use the range in TF1.
   mBandParams.clear();
   for (int i=0; i<NBandParam; i++) 
   mBandParams.push_back(mBetheBlochFcn->GetParameter(i));
 }

   delete mBetheBlochFcn;

}

//----------------------------------
void StPidAmpNet::fitAPath(StPidAmpPath& path, StPidAmpTrkVector* trks){
  /* to be overriden by its derivatives.  no-op */
 
  //the second input "trks" just for "trks.size()"
  //then use this size() info. to controll the range of the height of a path 
  //in the path fitting.
 
}


//----------------------------------
void StPidAmpNet::fitAmp(StPidAmpTrkVector* trks){
  /* to be overriden. no-op */

}
//----------------------------------
void StPidAmpNet::fillPathFittedSlices(){
//do not forget turn on mUsePathFittedResult in StPidAmpSlice in this method.
  /* to be overriden. no-op */

  // in the overriding func in class inherited from StPidAmpNet,
  //StPidAmpSlice::mUsePathFitted should be turned on here.
}
//----------------------------------
void StPidAmpNet::fitReso(){
  /* to be overwritten. no-op */
}
//----------------------------------
ostream& StPidAmpNet::put(ostream& s) const {
// for calling the right put() by operator.
  //to be overriden by its derivatives.
  return s<<"StPidAmpNet Base Class "<<endl;
}
//----------------------------------
void StPidAmpNet::processNet(StPidAmpTrkVector* trks){
//call funs needed to process in order.

    fillNet(trks);

  if (mFitBand){
     fitSlices();

     fillBand();
     fitBand();    

       setUp();
       fillNet(trks);
       fitSlices();

  } else {
    fitSlices();
    fillBand(); //just for draw without fitting
  }



  if (trks){

  if (mFitPath) {
      fitPaths(trks);
      fillPathFittedSlices(); //StPidAmpSlice::mUsePathFitted is turn on here
      fitSlices();
    }

                 fillAmp();
    if (mFitAmp) fitAmp(trks);

                   fillReso();
     if (mFitReso) fitReso();
  }

  drawNetFittings();
  fillNetOut();

}

  
//---------------------------------- 
ostream& operator<<(ostream& s,StPidAmpNet& net){
 return net.put(s);
}    

  
//----------------------------------
void StPidAmpNet::pushATrk(StPidAmpTrk* theTrack,StPidAmpChannelCollection* set){
//put one trk to the proper slice.
  //if (set); push the trk to StPidAmpNet::mBGNet.

    int    idx;
    double dedx;
                   

 if ( (fabs(theTrack->rig())>fabs(mParticleType.start())) && 
      (fabs(theTrack->rig())<fabs(mParticleType.end()))   &&
      (((theTrack->charge())*(mParticleType.charge()))>0) ){
   //channel bound checking and band bound checking

    if (!set){
 double rg =fabs(theTrack->rig());
 dedx=fabs(theTrack->dedx());
 idx=getSliceIndex(theTrack);// get the idex for slice.


 //  if ( (rg>((*mSliceCollect)[idx])->leftEdge()) && (rg <((*mSliceCollect)[idx])->rightEdge()) ) { //slice bound checking needed for debugging.
 //to reduce run time, comment it out in the code released.
     ((*mSliceCollect)[idx])->fill(theTrack,(dedx-(dedxAtBandCenter(rg)-dedxAtBandCenter((*mSliceCollect)[idx]->meanRig())))); //transform to pixel 
     //following the slope of BetheBloch curve, and do filling
 //  }
  
 } else if (set) {

   if (theTrack->nhits()>set->nhits4BGNet()){

       //fill mBGNet

       push2BGNet(*(set->bgNet()), theTrack);
   
 
      if(CheckMultiBGNet){

       //fill mBGElectronNet
   if (mParticleType.id()==3)  push2BGNet(*(set->bgElectronNet()),   theTrack);
   if (mParticleType.id()==2)  push2BGNet(*(set->bgPositronNet()),   theTrack);

      //fill mBGPionNet
   if (mParticleType.id()==8)  push2BGNet(*(set->bgPionPlusNet()),   theTrack);
   if (mParticleType.id()==9)  push2BGNet(*(set->bgPionMinusNet()),  theTrack);
       
      //fill mBGKaonNet
   if (mParticleType.id()==11) push2BGNet(*(set->bgKaonPlusNet()),   theTrack);
   if (mParticleType.id()==12) push2BGNet(*(set->bgKaonMinusNet()),  theTrack);

      //fill mBGProtonNet
   if (mParticleType.id()==14) push2BGNet(*(set->bgProtonNet()),     theTrack);
   if (mParticleType.id()==15) push2BGNet(*(set->bgAntiProtonNet()), theTrack);

      } 


   }
   }

 }
}
//----------------------------------
int StPidAmpNet::getSliceIndex(StPidAmpTrk* trk){
//return the index of the slice which a trk should be located in.

 double rg = fabs(trk->rig());
 
 unsigned idx=int((rg-fabs(mParticleType.start()))/SliceWidth); // get the idex for slice
  
 if (idx>=mSliceCollect->size()) idx=mSliceCollect->size()-1;
 if (idx<0) idx=0;
  
 return idx;
}
//----------------------------------
int StPidAmpNet::getSliceIndex(double x){
//return the index of the slice that a rig be located in.

 double rg = fabs(x);
 
 unsigned idx=int((rg-fabs(mParticleType.start()))/SliceWidth); // get the idex for slice
 if (idx>=mSliceCollect->size()) idx=mSliceCollect->size()-1;
 if (idx<0) idx=0;
  
 return idx;
}

//----------------------------------
void StPidAmpNet::fillSlices(StPidAmpTrkVector* trks,StPidAmpChannelCollection* set){


 StPidAmpTrkIter iter;
 StPidAmpTrk* thisTrack;

 if( ( mParticleType.id()==9 || mParticleType.id()==8 ) && set ){
 mNetWindow.addWindow(2.5,4.5); //for pi relative rising
 mNetWindow.addScreen(0.0,0.12);//screen out muon for betheBlock fitting.
 }
 

 for (iter=trks->begin(); iter!=trks->end(); iter++){
   thisTrack=*iter;
   pushATrk(thisTrack,set);
 }


 if ( ( mParticleType.id()==9 || mParticleType.id()==8 ) && set){
   mNetWindow.removeLastWindow(); //for pi relative rising
   mNetWindow.removeAllScreens();
 }

}
//----------------------------------
void StPidAmpNet::fillPaths(){

  StPidAmpPathIter pathIter;
  StPidAmpPath* thePath;

  for(pathIter=mPathCollect->begin(); pathIter!=mPathCollect->end(); pathIter++){

  thePath=*pathIter;
  thePath->fillPath(*mSliceCollect);
  thePath->adjudgePathWindow();
  thePath->fillPathGraph();
  }
}



//----------------------------------
void StPidAmpNet::fitSlices(){

  StPidAmpSliceIter iter;

  StPidAmpSlice* theSlice;

  for (iter=mSliceCollect->begin(); iter!=mSliceCollect->end(); iter++) {
     theSlice=*iter;
     theSlice->fit();
  }
}

//---------------------------------
void StPidAmpNet::push2BGNet(StPidAmpNet& net, StPidAmpTrk* trk){
  //to get a better fitting of mBGNet band tail, treat e+/- sepecially.


   double betaGamma =fabs((trk->rig())*(double(trk->charge()))/(mParticleType.mass()));
   double dedx=fabs(trk->dedx());

   int idx=net.getSliceIndex(betaGamma);



  if (mParticleType.id()!=2 && mParticleType.id()!=3) {

   if (mNetWindow.isInWindow(fabs(trk->rig()))) {

  // if ( (betaGamma>((*(net.sliceVector()))[idx])->leftEdge()) && (betaGamma <((*(net.sliceVector()))[idx])->rightEdge()) ) { //slice bound checking needed 
       //for debugging.to reduce run time, comment it out in the code released.
    (((*(net.sliceVector()))[idx])->slice())->Fill((dedx-(net.dedxAtBandCenter(betaGamma)-net.dedxAtBandCenter((*(net.sliceVector()))[idx]->meanRig()))));
 //  }
   }//if (mNetWindow.isInWindow(fabs(trk->rig()))) {

  } else if (mParticleType.id()==2 || mParticleType.id()==3){

  if (trk->nhits()>15 ){

   if (mNetWindow.isInWindow(fabs(trk->rig()))) {

   // if ( (betaGamma>((*(net.sliceVector()))[idx])->leftEdge()) && (betaGamma <((*(net.sliceVector()))[idx])->rightEdge()) ) { //slice bound checking needed 
       //for debugging.to reduce run time, comment it out in the code released.
    (((*(net.sliceVector()))[idx])->slice())->Fill((dedx-(net.dedxAtBandCenter(betaGamma)-net.dedxAtBandCenter((*(net.sliceVector()))[idx]->meanRig()))));
  //  }

   }//if (mNetWindow.isInWindow(fabs(trk->rig()))) {

  } else if (trk->nhits()<=15 ){ 


   if (mNetWindow.isInFirstWindow(fabs(trk->rig()))  ) {
     //just fill first window here. coz for nhits<15, the second window 
     //for e+/- is suzzy.

   //if ( (betaGamma>((*(net.sliceVector()))[idx])->leftEdge()) && (betaGamma <((*(net.sliceVector()))[idx])->rightEdge()) ) { //slice bound checking needed 
       //for debugging.to reduce run time, comment it out in the code released

    (((*(net.sliceVector()))[idx])->slice())->Fill((dedx-(net.dedxAtBandCenter(betaGamma)-net.dedxAtBandCenter((*(net.sliceVector()))[idx]->meanRig()))));
  // }

   

   }//if (mNetWindow.isInWindow(fabs(trk->rig()))) {


  }

  }//else if (mParticleType.id()==2 || mParticleType.id()==3)



}
//---------------------------------
void StPidAmpNet::fillNetOut(){
  unsigned i;

  TArrayD bandArray;
  TArrayD ampArray;
  TArrayD linrArray;


  if (mBandParams.size()) {
  bandArray.Set(mBandParams.size());
  for (i=0; i<mBandParams.size(); i++) bandArray.AddAt(mBandParams[i],i);
  }

  if (mAmpParams.size()){
  ampArray.Set( mAmpParams.size());
  for (i=0; i<mAmpParams.size();  i++)  ampArray.AddAt( mAmpParams[i],i);
  }

  if (mResoParams.size()){
  linrArray.Set(mResoParams.size());
  for (i=0; i<mResoParams.size(); i++) linrArray.AddAt(mResoParams[i],i);
  }



  mNetOut.SetGeantID(mParticleType.id());
  mNetOut.SetBandParArray(bandArray);
  mNetOut.SetAmpParArray(ampArray);
  mNetOut.SetResoParArray(linrArray);
  mNetOut.SetName((mParticleType.name()).c_str());
  mNetOut.SetTitle((mParticleType.name()).c_str());


}




//---------------------------------
StPidParamVector StPidAmpNet::mDefaultBandParams=StPidParamVector();

//---------------------------------
void StPidAmpNet::setDefaultBandParams(StPidParamVector& pars){

  StPidAmpNet::mDefaultBandParams=pars;
  //mDefaultBandParams contains only 3 elements!!


}
//----------------------------------
void StPidAmpNet::drawSlicesInASegment(strstream& st, double theBegin, double theEnd){

   int NPads=12;

  TCanvas* theSlicesCanvas=new TCanvas(st.str(), st.str(),20,10,800,600);
  theSlicesCanvas->Divide(3,4);//3X4=NPads.//this is an error prone.
  //should be robust.
  
  double  step=(theEnd-theBegin)/12.0;
  double  x   =theBegin;
  int     j   =1;

 theLoopOfPad: if (j<=NPads){
   if (x>theEnd) j=NPads;//break; force it break
 

   if(mNetWindow.isInWindow(x)) {
     theSlicesCanvas->cd(j);
     ((*sliceVector())[getSliceIndex(x)])->slice()->Draw("*H");
      j++;
   }
   x=x+step;
   if (x>=theEnd) j=NPads+1;//break; force it break
 
   goto theLoopOfPad;
 }

    theSlicesCanvas->cd();
}
//------------------------------
double StPidAmpNet::maxPoint(TGraph* gr, bool value){
  //value=true, return the max value among points
  //value=false, return the position of the max value.

      int i;
      double x,y;
      double maxDedx=0;
      double maxX=0;

      for (i=0; i<gr->GetN();i++){
      gr->GetPoint(i,x,y);

       if (maxDedx<y){
        maxDedx=y;
        maxX=x;
       }

     }

    if (value)  return maxDedx;
    else return maxX;

}
 

   
//---------------------------------
ostream& operator<<(ostream& s, const StPidAmpNet& net){
 return net.put(s);
}  
