/***************************************************************************
 *
 * $Id: StPidAmpChannelCollection.cc,v 1.8 2000/07/12 15:38:34 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             StPidAmpChannelCollection is a complete ensemble of 
 *             StPidAmpChannels. Can not find two identical channels in
 *             StPidAmpChannelCollection
 ***************************************************************************
 *
 * $Log: StPidAmpChannelCollection.cc,v $
 * Revision 1.8  2000/07/12 15:38:34  aihong
 * update for real data
 *
 * Revision 1.7  2000/07/06 01:55:14  perev
 * fix related to ROOT float -> double
 *
 * Revision 1.6  2000/05/05 19:20:44  aihong
 * let StPidAmpNetOut::Get*ParArray() returns pointer instead of obj.
 *
 * Revision 1.5  2000/05/01 16:59:25  aihong
 * clean up
 *
 * Revision 1.4  2000/04/11 15:34:23  aihong
 * change to adapt dividing trks by channel for faster filling
 *
 * Revision 1.3  2000/04/09 16:16:33  aihong
 * change for adapting NHitsDcaNet added
 *
 * Revision 1.2  2000/03/24 17:47:18  aihong
 * modify writeBGBands2Disk()
 *
 * Revision 1.1.1.1  2000/03/09 17:48:35  aihong
 * Installation of package
 *
 **************************************************************************/


#include <float.h> // for FLT_MAX
#include <fstream.h>

#include "TCanvas.h"
#include "StMessMgr.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"

#include "StPidAmpMaker/Infrastructure/StPidAmpChannelCollection.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpCutVector.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpCut.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpNet.hh"
#include "StPidAmpMaker/Infrastructure/StPidAmpBGNet.hh"
#include "StPidAmpMaker/Include/StPidAmpConst.hh"
//----------------------------
StPidAmpChannelCollection::StPidAmpChannelCollection(){

  setDefaultBandParameters();
  mWritePars2Disk=true;
  mDrawBGNet     =false;
  mNHitsCut4BGNet=0;
  
  mChannelCollect=new StPidAmpChannelVector();

  setUpBGNets();
}
//----------------------------
StPidAmpChannelCollection::StPidAmpChannelCollection(int n, int* nhitsAry,int p, double* ptAry,int d, double* dcaAry, StPidAmpNetType theNetType,StPidAmpTrkVector* trks,TString fitOpt, TString drawOpt){

  //n is # of marks along nhits axis.like(0,15,35,45)
  //p is # of marks along pt axis



     setDefaultBandParameters();

     mWritePars2Disk=true;
     mDrawBGNet     =false;
     mNetType       =theNetType;
     mNHitsCut4BGNet=0;
     mTrks=trks;

     filterOptions(theNetType,fitOpt,drawOpt);


  if (n<0||p<0||n>NMaxHits) 
  gMessMgr->Error()<<"illegal input for StPidAmpChannelCollection"<<endm; //illegal input
  
  mChannelCollect=new StPidAmpChannelVector();

  setUpChannels(n,nhitsAry,p,ptAry,d,dcaAry,theNetType);


  strstream mNameClone;
  int i;

  mNameClone<<"nhitsBin_";
  for (i=0; i<n; i++)  mNameClone<<nhitsAry[i]<<"_";

  mNameClone<<"ptBin_";
  for (i=0; i<p; i++) {
  if (ptAry[i]==FLT_MAX) mNameClone<<"Inf_";
  else  mNameClone<<ptAry[i]<<"_";
  }

  mNameClone<<"dcaBin_";
  for (i=0; i<d; i++){
  if (dcaAry[i]==FLT_MAX)  mNameClone<<"Inf"<<"_";
  else mNameClone<<dcaAry[i]<<"_";
  }


  mName=mNameClone.str();



  setUpBGNets();
}
//----------------------------
StPidAmpChannelCollection::~StPidAmpChannelCollection(){

  /* no -op */
}

//----------------------------
void StPidAmpChannelCollection::process(){

    bool BGBandOutPut4Debug=false;

    setDrawOpt();//pass by mDrawOpt to channels

    StPidAmpChannelIter iter;
    StPidAmpChannel* theChannel;

    for (iter=mChannelCollect->begin(); iter!=mChannelCollect->end(); iter++){
    theChannel=*iter;
    theChannel->filterAndFillTrks(mTrks);
    }

  if (mFitOpt.Contains("B")){

    for (iter=mChannelCollect->begin(); iter!=mChannelCollect->end(); iter++){
    theChannel=*iter;
    theChannel->fillBGNet(this);
    }

 processBGNet(true,false,false,false,false,  false, false,mDrawBGNet,false, *mBGNet);
   //         fBd  fPth  fAmp  fReso drSlic drPth  drAmp drBd  drReso

 if (mFitOpt.Contains("T")){//optimization
   //the band params in mBGNet has been refreshed when fitBand() get called.
   //so need not to refresh them here.
     mBGNet->setUp();
    for (iter=mChannelCollect->begin(); iter!=mChannelCollect->end(); iter++){
    theChannel=*iter;
    theChannel->fillBGNet(this);
    }
 processBGNet(true,false,false,false,false,  false, false,mDrawBGNet,false, *mBGNet);
   //         fBd  fPth  fAmp  fReso drSlic drPth  drAmp drBd  drReso
 }



   if (CheckMultiBGNet){
   //following just for taking advantage of StPidAmpNet::fitSlices()&fillBand()
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGElectronNet);
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGPositronNet);
  //         fBd   fPth  fAmp  fReso drSlic  drPth  drAmp  drBd  drReso
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGPionPlusNet);
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGPionMinusNet);
     //         fBd   fPth  fAmp  fReso drSlic  drPth  drAmp  drBd  drReso
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGKaonPlusNet);
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGKaonMinusNet);
    //         fBd   fPth  fAmp  fReso drSlic  drPth  drAmp  drBd  drReso
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGProtonNet);
 processBGNet(false,false,false,false,false,  false, false,false,false, *mBGAntiProtonNet);
    //         fBd   fPth  fAmp  fReso drSlic  drPth  drAmp  drBd  drReso
 drawMultiBGNets2Gether();
   }



      for (iter=mChannelCollect->begin(); iter!=mChannelCollect->end(); iter++){
       theChannel=*iter;
       theChannel->setBandParams4Nets(mBGNet->bandParams());
      }

  }



  if ( (mFitOpt.Contains("I")) || 
       (mFitOpt.Contains("A")) || 
       (mFitOpt.Contains("R")) ) {

    for (iter=mChannelCollect->begin(); iter!=mChannelCollect->end(); iter++){
    theChannel=*iter;
    theChannel->processChannel((mFitOpt.Contains("I")) ,  false, (mFitOpt.Contains("A")),(mFitOpt.Contains("R")) );
//                            trks, fitband,                  fitpath, fitamp,                  fitReso

    }
  }



   if (mWritePars2Disk)       writePars2Disk();
   if (BGBandOutPut4Debug)    outputBGBand4Debug();

}
   
//----------------------------
ostream& operator<<(ostream& s, StPidAmpChannelCollection& set){
    StPidAmpChannelIter iter;
    StPidAmpChannel*    theChannel;
  for (iter=(set.channelVector())->begin(); iter!=(set.channelVector())->end(); iter++){
      theChannel=*iter;
      s<<*theChannel<<endl;
  }
      return s;

}

//----------------------------
void StPidAmpChannelCollection::filterOptions(StPidAmpNetType theNetType,TString fitOpt, TString drawOpt){

     mFitOpt=fitOpt;
    mDrawOpt=drawOpt;

     mFitOpt.ToUpper();
    mDrawOpt.ToUpper();

    if ( (theNetType==ptDependent || theNetType==ptNhitsDependent) && mFitOpt.Contains("I"))
         mFitOpt.ReplaceAll("I","B");
    //do not dream to fit a individual band in a pt slice.

    if (mFitOpt.Contains("A")) mFitOpt.Append("R");
    //fit amp alone does not make sense. so add fit resolution.

    //   if ( ((mFitOpt.Contains("A")) || (mFitOpt.Contains("R"))) && 
    //     (!((mFitOpt.Contains("I")) || (mFitOpt.Contains("B"))) ) )
    //     mFitOpt.Append("B");
    //if fit amp or fit resolution, we have to fit band first.
    
    if (!(mFitOpt.Contains("B"))) mFitOpt.ReplaceAll("T","");
    //if do not fit betagamma, option "T" make not sence.//this will change later.

    if (!(mFitOpt.Contains("B"))) mDrawOpt.ReplaceAll("B","");
    //betagamma filling is time cosuming. 

    //if we have do not have mFitOpt "ARI", but have mDrawOpt "ARI"
    //the graph drawn will be just points without fitting curve.

    //if has fitOpt has "B" but do not has "I",
    //all the net will use mBGNet's band fitting result.


}
   



//----------------------------
void StPidAmpChannelCollection::setUpChannels(int n, int* nhitsAry,int p, double* ptAry,int d, double* dcaAry, StPidAmpNetType theNetType){



  //add asending value of nhitsAry and ptAry check here later.

  for (int i=0; i<(n-1); i++){
      StPidAmpCut nhitsCut;
      nhitsCut=StPidAmpCut("N", double(nhitsAry[i]),double(nhitsAry[i+1]));


      for (int j=0; j<(p-1); j++){
            StPidAmpCut           ptCut;
            ptCut=StPidAmpCut("P", ptAry[j],ptAry[j+1]);


	    for (int k=0; k<(d-1);k++){
	      StPidAmpCut     dcaCut;
              dcaCut=StPidAmpCut("D", dcaAry[k], dcaAry[k+1]);

            StPidAmpChannelInfo   channelInfo;
            StPidAmpCutVector cutCollect;
       
            cutCollect.push_back(nhitsCut); //nhits first always! do not change
            cutCollect.push_back(ptCut);
            cutCollect.push_back(dcaCut);

            channelInfo=StPidAmpChannelInfo(cutCollect);

            StPidAmpChannel* channel = 
            new StPidAmpChannel(channelInfo,theNetType);

	    mChannelCollect->push_back(channel);

     
	    }
     
      }
     
}



}
//----------------------------
void StPidAmpChannelCollection::setDefaultBandParameters(){
     
    StPidParamVector pars;
    /*
    pars.push_back(1.10177);
    pars.push_back(0.169534);
    pars.push_back(1.97245e-07);
    */
    pars.push_back(1.07174);
    pars.push_back(0.319880);
    pars.push_back(1.02801e-07);

    StPidAmpNet::setDefaultBandParams(pars);

}
//----------------------------
void StPidAmpChannelCollection::setDrawOpt(){

  //"IARB" 
  //"I" draw bandfittings
  //"A" draw amp fittings
  //"R" draw linear fittings
  //"B" draw BGNet band fitting

     StPidAmpChannelIter iter;
     StPidAmpChannel*    theChannel;

     mDrawBGNet=(mDrawOpt.Contains("B"));
      
  for (iter=mChannelCollect->begin(); iter!=mChannelCollect->end(); iter++){
       theChannel=*iter;
   theChannel->setBandFitsDraw(mDrawOpt.Contains("I")); 
   theChannel->setAmpFitsDraw(mDrawOpt.Contains("A")); 
   theChannel->setResoFitsDraw(mDrawOpt.Contains("R")); 
     }

}


//----------------------------
void StPidAmpChannelCollection::processBGNet(bool fitBand, bool fitPath, bool fitAmp, bool fitReso,bool drawSlicesFit, bool drawPathsFit, bool drawAmpFit, bool drawBandFit, bool drawResoFit,StPidAmpNet& net){

    net.setFitBand(fitBand);
    net.setFitPath(fitPath);
    net.setFitAmp(fitAmp);
    net.setFitReso(fitReso);
    net.setSlicesFitDraw(drawSlicesFit);
    net.setPathsFitDraw(drawPathsFit);
    net.setAmpFitDraw(drawAmpFit);
    net.setBandFitDraw(drawBandFit);
    net.setResoFitDraw(drawResoFit);

    net.processNet();
}

//----------------------------
void StPidAmpChannelCollection::drawMultiBGNets2Gether(){

   TString canvasName;
   canvasName.Append(mName.c_str());
   canvasName.Append("dedx-betaGamma vi particletypes");

 TCanvas* theMultiBGCanvas=new TCanvas(canvasName,canvasName,20,10,800,600);
   theMultiBGCanvas->cd();
    
     ( mBGPionPlusNet->bandGraph())->SetMarkerColor(4);
    ( mBGPionMinusNet->bandGraph())->SetMarkerColor(4);
     ( mBGElectronNet->bandGraph())->SetMarkerColor(6);
     ( mBGPositronNet->bandGraph())->SetMarkerColor(6);
     ( mBGKaonPlusNet->bandGraph())->SetMarkerColor(2);
    ( mBGKaonMinusNet->bandGraph())->SetMarkerColor(2);
       ( mBGProtonNet->bandGraph())->SetMarkerColor(3);
   ( mBGAntiProtonNet->bandGraph())->SetMarkerColor(3);

       ( mBGElectronNet->bandGraph())->Draw("A*");
       ( mBGPionPlusNet->bandGraph())->Draw("*");
      ( mBGPionMinusNet->bandGraph())->Draw("*");
       ( mBGPositronNet->bandGraph())->Draw("*");
       ( mBGKaonPlusNet->bandGraph())->Draw("*");
      ( mBGKaonMinusNet->bandGraph())->Draw("*");
         ( mBGProtonNet->bandGraph())->Draw("*");
     ( mBGAntiProtonNet->bandGraph())->Draw("*");
  
     theMultiBGCanvas->Update();
}
//----------------------------
void StPidAmpChannelCollection::setUpBGNets(){
  

   strstream bgStr;
   strstream electronStr;
   strstream positronStr;
   strstream pionPlusStr;
   strstream pionMinusStr;
   strstream kaonPlusStr;
   strstream kaonMinusStr;
   strstream protonStr;
   strstream antiProtonStr;
  
         bgStr<<StPidAmpParticle::mBGParticle.name().c_str()          <<mName.c_str();
   positronStr<<StPidAmpParticle::mBGPositronParticle.name().c_str()  <<mName.c_str();
   electronStr<<StPidAmpParticle::mBGElectronParticle.name().c_str()  <<mName.c_str();
   pionPlusStr<<StPidAmpParticle::mBGPionPlusParticle.name().c_str()  <<mName.c_str();
  pionMinusStr<<StPidAmpParticle::mBGPionMinusParticle.name().c_str() <<mName.c_str();
   kaonPlusStr<<StPidAmpParticle::mBGKaonPlusParticle.name().c_str()  <<mName.c_str();
  kaonMinusStr<<StPidAmpParticle::mBGKaonMinusParticle.name().c_str() <<mName.c_str();
     protonStr<<StPidAmpParticle::mBGProtonParticle.name().c_str()    <<mName.c_str();   
 antiProtonStr<<StPidAmpParticle::mBGAntiProtonParticle.name().c_str()<<mName.c_str();

 //make a local copy
   StPidAmpParticle theBG          =StPidAmpParticle::mBGParticle;
   StPidAmpParticle theBGElectron  =StPidAmpParticle::mBGElectronParticle;
   StPidAmpParticle theBGPositron  =StPidAmpParticle::mBGPositronParticle;
   StPidAmpParticle theBGPionPlus  =StPidAmpParticle::mBGPionPlusParticle;
   StPidAmpParticle theBGPionMinus =StPidAmpParticle::mBGPionMinusParticle;
   StPidAmpParticle theBGKaonPlus  =StPidAmpParticle::mBGKaonPlusParticle;
   StPidAmpParticle theBGKaonMinus =StPidAmpParticle::mBGKaonMinusParticle;
   StPidAmpParticle theBGProton    =StPidAmpParticle::mBGProtonParticle;
   StPidAmpParticle theBGAntiProton=StPidAmpParticle::mBGAntiProtonParticle;

   //modify the name on local copy.
           theBG.setName(bgStr.str());
   theBGElectron.setName(electronStr.str());
   theBGPositron.setName(positronStr.str());
   theBGPionPlus.setName(pionPlusStr.str());
  theBGPionMinus.setName(pionMinusStr.str());
   theBGKaonPlus.setName(kaonPlusStr.str());
  theBGKaonMinus.setName(kaonMinusStr.str());
     theBGProton.setName(protonStr.str());
 theBGAntiProton.setName(antiProtonStr.str());


  StPidAmpChannelInfo channelInfo;

  
 // def.setName(bgStr.str());

  //for the normal nets, a net's name differ to each other by their channelInfo
  //for the BGNets, the channelInfo is a fake, so we differ them by the set's 
  //name+mBGParticle name.

 mBGNet           =new StPidAmpBGNet(theBG,channelInfo);

 if (CheckMultiBGNet){
 mBGElectronNet   =new StPidAmpBGNet(theBGElectron,channelInfo); 
 mBGPositronNet   =new StPidAmpBGNet(theBGPositron,channelInfo); 
 mBGPionPlusNet   =new StPidAmpBGNet(theBGPionPlus,channelInfo);
 mBGPionMinusNet  =new StPidAmpBGNet(theBGPionMinus,channelInfo);
 mBGKaonPlusNet   =new StPidAmpBGNet(theBGKaonPlus,channelInfo); 
 mBGKaonMinusNet  =new StPidAmpBGNet(theBGKaonMinus,channelInfo); 
 mBGProtonNet     =new StPidAmpBGNet(theBGProton,channelInfo);
 mBGAntiProtonNet =new StPidAmpBGNet(theBGAntiProton,channelInfo);
 }






}
//--------------------------------
void StPidAmpChannelCollection::writePars2Disk(){

    if (mFitOpt.Contains("B"))                           writeBGBands2Disk();
    if (mFitOpt.Contains("A"))                           writeAmp2Disk();
    if (mFitOpt.Contains("R") && mNetType==noDependent ) writeReso2Disk();
    if (mFitOpt.Contains("I") && mNetType==noDependent ) writeBands2Disk();

}
//--------------------------------
void StPidAmpChannelCollection::writeAmp2Disk(){

    StPidAmpChannel* theChannel;
    StPidAmpNet*     theNet;
    TString          fileName;
    int              bufSize=32000;
    int              splitLevel=0;   
    unsigned         i,j;

   fileName.Append(mName.c_str());
   fileName.Append("Amp.root");

   TFile*     theFile     = new TFile(fileName,"recreate",fileName);
   TTree*     netSetTree  = new TTree("netSetTree", "netSetTree");
   TObjArray* channelLevel= 0;

   TBranch* br=netSetTree->Branch("netSetBranch","TObjArray", &channelLevel, bufSize,splitLevel);

   for (i=0; i<mChannelCollect->size(); i++) {
    channelLevel=new TObjArray();
    theChannel=(*mChannelCollect)[i];
    StPidAmpChannelInfoOut* channelInfoOut=new StPidAmpChannelInfoOut(theChannel->channelInfoOut());   
    channelLevel->AddLast(channelInfoOut);

    for (j=0; j<(((*mChannelCollect)[i])->netVector())->size();j++) {
    theNet=(*(theChannel->netVector()))[j];
    StPidAmpNetOut* theNetOut=new StPidAmpNetOut( theNet->netOut() );
   
     channelLevel->AddLast(theNetOut);
    }

    netSetTree->Fill();
    delete channelLevel;
   }


    theFile->Write();
    br=0;
}

//--------------------------------
void StPidAmpChannelCollection::writeReso2Disk(){

  //this method is only for defaultChannelCollection(noDependent)
  //cause other ChannelCollection would have multiple channels. their bandIO
  //can be merge into Amp.root.

    StPidAmpChannel* theChannel;
    StPidAmpNet*     theNet;
    TString          fileName;
    unsigned         j;

   fileName.Append(mName.c_str());
   fileName.Append("Reso.root");

   TFile *theFile=new TFile(fileName,"recreate",fileName);
   theChannel=(*mChannelCollect)[0];//defaultChannelCollection, only one channel
 
    for (j=0; j<(theChannel->netVector())->size();j++) {
    theNet=(*(theChannel->netVector()))[j];
    StPidAmpNetOut* theNetOut=new StPidAmpNetOut( theNet->netOut() );
    theNetOut->Write();
    }

    theFile->Write();
}

//--------------------------------
void StPidAmpChannelCollection::writeBands2Disk(){
  //this method is only for defaultChannelCollection(noDependent)
  //cause other ChannelCollection would have multiple channels. their bandIO
  //can be merge into Amp.root.

    StPidAmpChannel* theChannel;
    StPidAmpNet*     theNet;
    TString          fileName;
    unsigned         j;

   fileName.Append(mName.c_str());
   fileName.Append("Bands.root");

   TFile *theFile=new TFile(fileName,"recreate",fileName);
   theChannel=(*mChannelCollect)[0];//defaultChannelCollection, only one channel
 
    for (j=0; j<(theChannel->netVector())->size();j++) {
    theNet=(*(theChannel->netVector()))[j];
    StPidAmpNetOut* theNetOut=new StPidAmpNetOut( theNet->netOut() );
    theNetOut->Write();
    }

    theFile->Write();  
}
//--------------------------------
void StPidAmpChannelCollection::writeBGBands2Disk(){
  //write mBGNet::mNetOut to disk,
  //also write a set StPidAmpNetOut for each particle type, 
  //the first 3 bandparams is set to the first 3 of mBGNet's.
  //this is for draw convenience. people can just pick out 
  //a netOut params and draw without resetting additional parameters.

    StPidAmpChannel* theChannel;
    StPidAmpNet*     theNet;
    TString          fileName;
    unsigned         j;

   fileName.Append(mName.c_str());
   fileName.Append("BGBands.root");

   TFile*          theFile =new TFile(fileName,"recreate",fileName);
   StPidAmpNetOut* bgNetOut=new StPidAmpNetOut( mBGNet->netOut() );

   //the name from mBGNet->netOut is setname+mBGname. 
   //now we reset name only be mBGname.
   bgNetOut->SetName(StPidAmpParticle::mBGParticle.name().c_str());
   bgNetOut->SetTitle(StPidAmpParticle::mBGParticle.name().c_str());

   bgNetOut->Write();

   theChannel=(*mChannelCollect)[0];//defaultChannelCollection, only one channel
 
    for (j=0; j<(theChannel->netVector())->size();j++) {
    theNet=(*(theChannel->netVector()))[j];
    StPidAmpNetOut* theNetOut=new StPidAmpNetOut(theNet->netOut());

    TArrayD bandPar=*((theNet->netOut()).GetBandParArray());
         
   if (bandPar.GetSize()>=NBandParam) {
         for (int i=0; i<NFitBandParam; i++)
         bandPar.AddAt(((mBGNet->netOut()).GetBandParArray())->At(i),i);//refresh bandPar.
   }

     TArrayD dummyArray;

     theNetOut->SetBandParArray(bandPar);
     dummyArray.Set(NAmpParam); //TArrayD with size 0 won't be copied.
     theNetOut->SetAmpParArray(dummyArray);
     dummyArray.Set(NResoParam);
     theNetOut->SetResoParArray(dummyArray);

     theNetOut->Write();
    }

    theFile->Write();  
}


//--------------------------------
void StPidAmpChannelCollection::outputBGBand4Debug(){

  ofstream of;

 
  of.open("BGBand4Debug.txt",ios::app);

  int i;
  double x,y;

  for (i=0; i<mBGNet->bandGraph()->GetN(); i++){

            mBGNet->bandGraph()->GetPoint(i,x,y);
            of<<"gr->SetPoint(gr->GetN(),"<<x<<","<<y<<");"<<endl;
  }

  of.close();

}
