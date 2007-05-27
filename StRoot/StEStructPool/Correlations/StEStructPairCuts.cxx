/**********************************************************************
 *
 * $Id: StEStructPairCuts.cxx,v 1.7 2007/05/27 22:45:03 msd Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  Cut class for track-pair level quantities
 *
 *
 ***********************************************************************/
#include "StEStructPairCuts.h"
#include <math.h>
#include <stdlib.h>

ClassImp(StEStructPairCuts)

StEStructPairCuts::StEStructPairCuts(): StEStructCuts(){ init(); };
StEStructPairCuts::StEStructPairCuts(const char* cutfileName): StEStructCuts(cutfileName) { init(); };

StEStructPairCuts::~StEStructPairCuts() {};

void StEStructPairCuts::init(){ 

  strcpy(mcutTypeName,"Pair");
  initCuts();
  initNames();
  if(isLoaded())loadCuts();

}

void StEStructPairCuts::initCuts(){

    mdphi[0]=mdphi[1]=0;
    mdeta[0]=mdeta[1]=0;
    mdmt[0]=mdmt[1]=0;
    mqInv[0]=mqInv[1]=0;
    mEntSep[0]=mEntSep[1]=0;
    mExitSep[0]=mExitSep[1]=0;
    mQuality[0]=mQuality[1]=0;
    mMidTpcSepLS[0]=mMidTpcSepLS[1]=0;    
    mMidTpcSepUS[0]=mMidTpcSepUS[1]=0;
    mHBT[0]=mHBT[1]=mHBT[2]=mHBT[3]=0;
    mCoulomb[0]=mCoulomb[1]=mCoulomb[2]=mCoulomb[3]=0;
    mMerging[0]=mMerging[1]=0;
    mCrossing[0]=mCrossing[1]=0;
    
    mdeltaPhiCut=mdeltaEtaCut=mdeltaMtCut=mqInvCut=mEntSepCut=mExitSepCut=mQualityCut=mMidTpcSepLSCut=mMidTpcSepUSCut=false;
    mHBTCut=mCoulombCut=mMergingCut=mCrossingCut = false;

    for(int i=0;i<4;i++)
    mdphiCounter[i]=mdetaCounter[i]=mdmtCounter[i]=mqInvCounter[i]=mEntSepCounter[i]=mExitSepCounter[i]=mQualityCounter[i]=msplitLSCounter[i]=msplitUSCounter[i]=mHBTCounter[i]=mCoulombCounter[i]=mMergingCounter[i]=mCrossingCounter[i]=0;
    
    mdeltaPhi=mdeltaEta=mdeltaMt=mqInvarient= mEntranceSeparation=mExitSeparation=mQualityVal=mMidTpcSeparationLS=mMidTpcSeparationUS=0;

    mapMask0 = 0xFFFFFF00;
    mapMask1 = 0x1FFFFF;
    for(int i=0;i<32;i++)bitI[i]=1UL<<i;

    mZoffset = 0;

};

void StEStructPairCuts::initNames(){

  strcpy(mdphiName.name,"DeltaPhi");
  strcpy(mdetaName.name,"DeltaEta");
  strcpy(mdmtName.name,"DeltaMt");
  strcpy(mqInvName.name,"qInv");
  strcpy(mEntSepName.name,"EntranceSep");
  strcpy(mExitSepName.name,"ExitSep");
  strcpy(mQualityName.name,"Quality");
  strcpy(mMidTpcSepLSName.name,"MidTpcSepLikeSign");
  strcpy(mMidTpcSepUSName.name,"MidTpcSepUnlikeSign");
  strcpy(mHBTName.name,"HBT");
  strcpy(mCoulombName.name,"Coulomb");
  strcpy(mMergingName.name,"Merging");
  strcpy(mCrossingName.name,"Crossing");
  
};


bool StEStructPairCuts::loadBaseCuts(const char* name, const char** vals, int nvals){

  //  cout<<" Cut Name="<<name<<endl;
  if(!strcmp(name,mdphiName.name)){
    mdphi[0]=(float)(M_PI*atof(vals[0])); mdphi[1]=(float)(M_PI*atof(vals[1]));
    mdphiName.idx=createCutHists(name,mdphi);
    mdeltaPhiCut=true;
    return true;
  }

  if(!strcmp(name,mdetaName.name)){
    mdeta[0]=atof(vals[0]); mdeta[1]=atof(vals[1]);
    mdetaName.idx=createCutHists(name,mdeta);
    mdeltaEtaCut=true;
    return true;
  }

  if(!strcmp(name,mdmtName.name)){
    mdmt[0]=atof(vals[0]); mdmt[1]=atof(vals[1]);
    mdmtName.idx=createCutHists(name,mdmt);
    mdeltaMtCut=true;
    return true;
  }

  if(!strcmp(name,mqInvName.name)){
    mqInv[0]=atof(vals[0]); mqInv[1]=atof(vals[1]);
    mqInvName.idx=createCutHists(name,mqInv);
    mqInvCut=true;
    return true;
  }

  if(!strcmp(name,mEntSepName.name)){
    mEntSep[0]=atof(vals[0]); mEntSep[1]=atof(vals[1]);
    mEntSepName.idx=createCutHists(name,mEntSep);
    mEntSepCut=true;
    return true;
  }

  if(!strcmp(name,mExitSepName.name)){
    mExitSep[0]=atof(vals[0]); mExitSep[1]=atof(vals[1]);
    mExitSepName.idx=createCutHists(name,mExitSep);
    mExitSepCut=true;
    return true;
  }

  if(!strcmp(name,mQualityName.name)){
    mQuality[0]=atof(vals[0]); mQuality[1]=atof(vals[1]);
    mQualityName.idx=createCutHists(name,mQuality);
    mQualityCut=true;
    return true;
  }

  if(!strcmp(name,mMidTpcSepLSName.name)){
    mMidTpcSepLS[0]=atof(vals[0]); mMidTpcSepLS[1]=atof(vals[1]);
    mMidTpcSepLSName.idx=createCutHists(name,mMidTpcSepLS);
    mMidTpcSepLSCut=true;
    return true;
  }

  if(!strcmp(name,mMidTpcSepUSName.name)){
    mMidTpcSepUS[0]=atof(vals[0]); mMidTpcSepUS[1]=atof(vals[1]);
    mMidTpcSepUSName.idx=createCutHists(name,mMidTpcSepUS);
    mMidTpcSepUSCut=true;
    return true;
  }

  if(!strcmp(name,mHBTName.name)){
    mHBT[0]=atof(vals[0]); mHBT[1]=atof(vals[1]);
    mHBT[2]=atof(vals[2]); mHBT[3]=atof(vals[3]);
    //mHBTName.idx=createCutHists(name,mHBT,4);  // not making cut histograms
    mHBTCut=true;
    cout << " Loading HBT cut with range of cuts = "<<mHBT[0]<<","<<mHBT[1]<<","<<mHBT[2]<<","<<mHBT[3]<<endl;
    return true;
  }

  if(!strcmp(name,mCoulombName.name)){
    mCoulomb[0]=atof(vals[0]); mCoulomb[1]=atof(vals[1]);
    mCoulomb[2]=atof(vals[2]); mCoulomb[3]=atof(vals[3]);
    //mCoulombName.idx=createCutHists(name,mCoulomb,4);  // not making cut histograms
    mCoulombCut=true;
    cout << " Loading Coulomb cut with range of cuts = "<<mCoulomb[0]<<","<<mCoulomb[1]<<","<<mCoulomb[2]<<","<<mCoulomb[3]<<endl;
    return true;
  }

  if(!strcmp(name,mMergingName.name)){
    mMerging[0]=atof(vals[0]); mMerging[1]=atof(vals[1]);
    //mMergingName.idx=createCutHists(name,mMerging,2);  // not making cut histograms
    mMergingCut=true;
    cout << " Loading Merging cut with range of cuts = "<<mMerging[0]<<","<<mMerging[1]<<endl;
    return true;
  }

  if(!strcmp(name,mCrossingName.name)){
    mCrossing[0]=atof(vals[0]); mCrossing[1]=atof(vals[1]);
    //mCrossingName.idx=createCutHists(name,mCrossing,2);  // not making cut histograms
    mCrossingCut=true;
    cout << " Loading Crossing cut with range of cuts = "<<mCrossing[0]<<","<<mCrossing[1]<<endl;
    return true;
  }

  //  cout<<" didn't find any cut with this name "<<endl;
  return false;
};

void StEStructPairCuts::printCutCounts(ostream& ofs, char* cutName,int c1,int c2){
  ofs<<cutName<<c1<<" + "<<c2<<"  =  "<<c1+c2<<endl;
}
  

void StEStructPairCuts::printCutStats(ostream& ofs){

  //  ofs<<"# ******************************************** "<<endl;
  //  ofs<<"# *************** Pair Cuts ****************** "<<endl;
  //  ofs<<"# *** format = Cut, minvalue, maxvalue     *** "<<endl;
  // ofs<<"# ***      Sib LS + US = Total             *** "<<endl;
  // ofs<<"# ***      Mix LS + US = Total             *** "<<endl;
  //  ofs<<"# ******************************************** "<<endl;
  ofs<<endl;
  char* cutTypes[]={"#---  Sibling Pairs : LS + US = ",
                    "#---  Mixed   Pairs : LS + US = "};
  if(mdeltaPhiCut){
    ofs<<mdphiName.name<<","<<mdphi[0]/M_PI<<","<<mdphi[1]/M_PI<<"\t\t\t"<<" # pair dphi cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mdphiCounter[0],mdphiCounter[1]);
    printCutCounts(ofs,cutTypes[1],mdphiCounter[2],mdphiCounter[3]);
  }
  if(mdeltaEtaCut){
     ofs<<mdetaName.name<<","<<mdeta[0]<<","<<mdeta[1]<<"\t\t\t"<<" # pair deta cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mdetaCounter[0],mdetaCounter[1]);
    printCutCounts(ofs,cutTypes[1],mdetaCounter[2],mdetaCounter[3]);
  }

  if(mdeltaMtCut){
     ofs<<mdmtName.name<<","<<mdmt[0]<<","<<mdmt[1]<<"\t\t\t"<<" # pair dmt cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mdmtCounter[0],mdmtCounter[1]);
    printCutCounts(ofs,cutTypes[1],mdmtCounter[2],mdmtCounter[3]);
  }

  if(mqInvCut){
     ofs<<mqInvName.name<<","<<mqInv[0]<<","<<mqInv[1]<<"\t\t\t"<<" # pair qInv cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mqInvCounter[0],mqInvCounter[1]);
    printCutCounts(ofs,cutTypes[1],mqInvCounter[2],mqInvCounter[3]);
  }

  if(mEntSepCut){
    ofs<<mEntSepName.name<<","<<mEntSep[0]<<","<<mEntSep[1]<<"\t\t\t"<<" # pair EntSep cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mEntSepCounter[0],mEntSepCounter[1]);
    printCutCounts(ofs,cutTypes[1],mEntSepCounter[2],mEntSepCounter[3]);
  }

  if(mExitSepCut){
    ofs<<mExitSepName.name<<","<<mExitSep[0]<<","<<mExitSep[1]<<"\t\t\t"<<" # pair ExitSep cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mExitSepCounter[0],mExitSepCounter[1]);
    printCutCounts(ofs,cutTypes[1],mExitSepCounter[2],mExitSepCounter[3]);
  }

  if(mQualityCut){
     ofs<<mQualityName.name<<","<<mQuality[0]<<","<<mQuality[1]<<"\t\t\t"<<" # pair Quality cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mQualityCounter[0],mQualityCounter[1]);
    printCutCounts(ofs,cutTypes[1],mQualityCounter[2],mQualityCounter[3]);
  }

  if(mMidTpcSepLSCut){
     ofs<<mMidTpcSepLSName.name<<","<<mMidTpcSepLS[0]<<","<<mMidTpcSepLS[1]<<"\t\t\t"<<" # pair MidTpcSepLS cut"<<endl;
    printCutCounts(ofs,cutTypes[0],msplitLSCounter[0],msplitLSCounter[1]);
    printCutCounts(ofs,cutTypes[1],msplitLSCounter[2],msplitLSCounter[3]);
  }

  if(mMidTpcSepUSCut){
     ofs<<mMidTpcSepUSName.name<<","<<mMidTpcSepUS[0]<<","<<mMidTpcSepUS[1]<<"\t\t\t"<<" # pair MidTpcSepUS cut"<<endl;
    printCutCounts(ofs,cutTypes[0],msplitUSCounter[0],msplitUSCounter[1]);
    printCutCounts(ofs,cutTypes[1],msplitUSCounter[2],msplitUSCounter[3]);
  }

  if(mHBTCut){
    ofs<<mHBTName.name<<","<<mHBT[0]<<","<<mHBT[1]<<","<<mHBT[2]<<","<<mHBT[3]<<"\t\t"<<" # pair HBT cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mHBTCounter[0],mHBTCounter[1]);
    printCutCounts(ofs,cutTypes[1],mHBTCounter[2],mHBTCounter[3]);
  }
  if(mCoulombCut){
    ofs<<mCoulombName.name<<","<<mCoulomb[0]<<","<<mCoulomb[1]<<","<<mCoulomb[2]<<","<<mCoulomb[3]<<"\t"<<" # pair Coulomb cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mCoulombCounter[0],mCoulombCounter[1]);
    printCutCounts(ofs,cutTypes[1],mCoulombCounter[2],mCoulombCounter[3]);
  }
  if(mMergingCut){
    ofs<<mMergingName.name<<","<<mMerging[0]<<","<<mMerging[1]<<"\t\t\t"<<" # pair Merging cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mMergingCounter[0],mMergingCounter[1]);
    printCutCounts(ofs,cutTypes[1],mMergingCounter[2],mMergingCounter[3]);
  }
  if(mCrossingCut){
    ofs<<mCrossingName.name<<","<<mCrossing[0]<<","<<mCrossing[1]<<"\t\t"<<" # pair Crossing cut"<<endl;
    printCutCounts(ofs,cutTypes[0],mCrossingCounter[0],mCrossingCounter[1]);
    printCutCounts(ofs,cutTypes[1],mCrossingCounter[2],mCrossingCounter[3]);
  }



  //  ofs<<"# ******************************************** "<<endl<<endl;

}

//------------------------------------------------------------
int StEStructPairCuts::cutPair(){
  // return 0 to use pair, return 1 to cut pair 

  //  if(cutDeltaPhi() || cutDeltaEta() || cutDeltaMt()) return 1;
  //if(goodDeltaPhi() && goodDeltaEta() && goodDeltaMt()) return 0;
  
  // *** new test ***
  /*  cout.precision(3);     
  cout <<MidTpcXYSeparation()<<"\t"<<MidTpcZSeparation()<<"\t"<<MidTpcSeparation();    
  cout << "::\t"<<NominalTpcXYExitSeparation()<<"\t"<<NominalTpcZExitSeparation()<<"\t"<<NominalTpcExitSeparation()<<endl;
  //cout << endl;
  if (cutCoulomb())  cout << "COUL\t"<<DeltaEta()<<"\t"<<DeltaPhi()<<"\t"<<DeltaPt()<<"\t"<<mTrack1->Pt()<<"\t"<<mTrack2->Pt()<<endl;
  if (cutHBT())      cout << "HBT\t"<<mType<<endl;
  if (cutMerging())  cout << "MERG\t"<<NominalTpcAvgXYSeparation()<<"\t"<<NominalTpcAvgZSeparation()<<endl;
  if (cutCrossing())  {
    cout << "CROS\t";
    if (mTrack1->Charge()>0) cout << "+ ";
    else cout << "- ";
    if (mTrack2->Charge()>0) cout << "+\t";
    else cout << "-\t";
    cout <<MidTpcXYSeparation()<<"\t"<<MidTpcZSeparation();
    float dphi = mTrack1->Phi()-mTrack2->Phi(); // signed DeltaPhi
    float dpt =  mTrack1->Pt()- mTrack2->Pt();  // signed DeltaPt
    cout << dphi << "\t" << dpt << endl;
    }*/
  // *** 

  if( cutMerging() || cutCrossing() || cutCoulomb() || cutHBT() ) return 1;

  if(!mdeltaEta) mdeltaEta=DeltaEta();  // may have been set above
  
  if(mdeltaEta<0.03){

    //--> qInv and EntSep are combined for speed & small delta eta 
    if(cutqInvORNominalEntranceSep()) return 1;
    
    if(mType==1 || mType==3) {
       if(cutMidTpcSepUS()) return 1;
    } else {
      if(cutMidTpcSepLS()) return 1;
    }
    
  }
  
  if(cutQuality()) return 1;
  
  //  if(cutExitSep() || cutQuality()) return 1;
  return 0;
}

//------------------------------------------------------------
int StEStructPairCuts::cutPairHistograms(){

  // much much slower cut code - should be run on a small sub-sample to view 
  // results of histograms.  Tricky piece here is the connection between pair
  // pair types and cut and event inter-cut relations. 
  // where I need to set the value used for histogramming to be
  // at a specific overflow: xhi+2.0*(xhi-xlo) when this particular cut is
  // not in play for this specific Pair.

  int cutCount=0;

  cutCount+=cutDeltaPhiH(); 
  cutCount+=cutDeltaEtaH();
  cutCount+=cutDeltaMtH(); 

  cutCount+=cutHBT();
  cutCount+=cutCoulomb();
  cutCount+=cutMerging();
  cutCount+=cutCrossing();

  if(mdeltaEta<0.03){
    cutCount+=cutqInvH();
    cutCount+=cutEntranceSepH();
    if(mType==1 || mType==3){
      cutCount+=cutMidTpcSepUSH();
      if(mMidTpcSepLSCut)mvalues[mMidTpcSepLSName.idx]=mMidTpcSepLS[1]+2.0*(mMidTpcSepLS[1]-mMidTpcSepLS[0]);
    } else {
      cutCount+=cutMidTpcSepLSH();
      if(mMidTpcSepUSCut)mvalues[mMidTpcSepUSName.idx]=mMidTpcSepUS[1]+2.0*(mMidTpcSepUS[1]-mMidTpcSepUS[0]);
    }    
  } else {
      if(mqInvCut)mvalues[mqInvName.idx]=mqInv[1]+2.0*(mqInv[1]-mqInv[0]);
      if(mEntSepCut)mvalues[mEntSepName.idx]=mEntSep[1]+2.0*(mEntSep[1]-mEntSep[0]);
      if(mMidTpcSepUSCut)mvalues[mMidTpcSepUSName.idx]=mMidTpcSepUS[1]+2.0*(mMidTpcSepUS[1]-mMidTpcSepUS[0]);
      if(mMidTpcSepLSCut)mvalues[mMidTpcSepLSName.idx]=mMidTpcSepLS[1]+2.0*(mMidTpcSepLS[1]-mMidTpcSepLS[0]);
  }
  
  cutCount+=cutExitSepH();
  cutCount+=cutQualityH();

  fillHistograms((cutCount==0));

  return cutCount;
}

StLorentzVectorF 
StEStructPairCuts::fourMomentumSum() const
{
  StLorentzVectorF temp = mTrack1->FourMomentum()+mTrack2->FourMomentum();
  return temp;
}
//-----------------------------------------------------------------
StLorentzVectorF 
StEStructPairCuts::fourMomentumDiff() const
{
  StLorentzVectorF temp = mTrack1->FourMomentum()-mTrack2->FourMomentum();
  return temp;
}

double 
StEStructPairCuts::quality() const {

 
  unsigned long padRow1To24Track1 = mTrack1->TopologyMapData(0) & mapMask0;
  unsigned long padRow25To45Track1 = mTrack1->TopologyMapData(1) & mapMask1;
  unsigned long padRow1To24Track2 = mTrack2->TopologyMapData(0) & mapMask0;
  unsigned long padRow25To45Track2 = mTrack2->TopologyMapData(1) & mapMask1;
  
  // AND logic
  unsigned long bothPads1To24 = padRow1To24Track1 & padRow1To24Track2;
  unsigned long bothPads25To45 = padRow25To45Track1 & padRow25To45Track2;
  // XOR logic
  unsigned long onePad1To24 = padRow1To24Track1 ^ padRow1To24Track2;
  unsigned long onePad25To45 = padRow25To45Track1 ^ padRow25To45Track2;
  int ibits;
  int Quality = 0;

  double normQual = 0.0;
  

  int MaxQuality = mTrack1->TopologyMapTPCNHits() + mTrack2->TopologyMapTPCNHits();

  for (ibits=8;ibits<=31;ibits++) {
    //    bitI = 0;
    // bitI |= 1UL<<(ibits);
    if ( onePad1To24 & bitI[ibits] ) {
      Quality++;
      //      continue;
    } else if ( bothPads1To24 & bitI[ibits] ) {
      Quality--;
    }
  }

  for (ibits=0;ibits<=20;ibits++) {
    //  bitI = 0;
    //bitI = 1UL<<(ibits);
    if ( onePad25To45 & bitI[ibits] ) {
      Quality++;
      //continue;
    } else if ( bothPads25To45 & bitI[ibits] ) {
      Quality--;
    }
  }

  normQual = (double)Quality/((double)MaxQuality );
  return ( normQual );

}

//--------------------------------------------------------
double 
StEStructPairCuts::OpeningAngle() const {
  StThreeVectorF p1 = mTrack1->FourMomentum().vect();
  StThreeVectorF p2 = mTrack2->FourMomentum().vect();
  double dAngInv = 57.296*acos((p1.dot(p2))/(p1.mag()*p2.mag()));
  return (dAngInv);
}

//--------------------------------------------------------
// Zoffset correction is needed for mixed events.  
double 
StEStructPairCuts::NominalTpcExitSeparation() const {
  StThreeVectorF off(0,0,mZoffset);
  StThreeVectorF diff = mTrack1->NominalTpcExitPoint() - mTrack2->NominalTpcExitPoint() + off;
  return (double)(diff.mag());
}

double 
StEStructPairCuts::NominalTpcXYExitSeparation() const {
  StThreeVectorF diff = mTrack1->NominalTpcExitPoint() - mTrack2->NominalTpcExitPoint();
  return (double)(diff.perp());
}

double 
StEStructPairCuts::NominalTpcZExitSeparation() const {
  // There is a problem when both tracks exit the TPC through the same endcap, then both exit
  //   points have z = (+/-) 200, and the z separation is exactly zero.
  // With normal eta and primaryVertex cuts, most tracks should exit the side of the TPC, 
  //   so we will just skip the problem pairs.
  if ( abs(mTrack1->NominalTpcExitPoint().z())==200 || abs(mTrack2->NominalTpcExitPoint().z())==200 ) return -1;
  StThreeVectorF diff = mTrack1->NominalTpcExitPoint() - mTrack2->NominalTpcExitPoint();
  return (double)(abs(diff.z()+mZoffset));

}

//--------------------------------------------------------
double 
StEStructPairCuts::NominalTpcEntranceSeparation() const {
  StThreeVectorF off(0,0,mZoffset);
  StThreeVectorF diff = mTrack1->NominalTpcEntrancePoint() - mTrack2->NominalTpcEntrancePoint() + off;
  return (double)(diff.mag());
}

double 
StEStructPairCuts::NominalTpcXYEntranceSeparation() const {
  StThreeVectorF diff = mTrack1->NominalTpcEntrancePoint() - mTrack2->NominalTpcEntrancePoint();
  return (double)(diff.perp());
}
double 
StEStructPairCuts::NominalTpcZEntranceSeparation() const {
  StThreeVectorF diff = mTrack1->NominalTpcEntrancePoint() - mTrack2->NominalTpcEntrancePoint();
  return (double)(fabs(diff.z() + mZoffset));
}

//--------------------------------------------------------
double 
StEStructPairCuts::NominalTpcAvgXYSeparation() const {
  double x1=NominalTpcXYEntranceSeparation();
  double x2=MidTpcXYSeparation();
  double x3=NominalTpcXYExitSeparation();
  return (x1+x2+x3)/3.;
}
double
StEStructPairCuts::NominalTpcAvgZSeparation() const {
  double x1=NominalTpcZEntranceSeparation();
  double x2=MidTpcZSeparation();
  double x3=NominalTpcZExitSeparation();
  if (x3==-1) return (x1+x2)/2.;  // if particle exited the endcap, exlude from average
  else return (x1+x2+x3)/3.;
}

//--------------------------------------------------------
double 
StEStructPairCuts::MidTpcXYSeparation() const {
  StThreeVectorF diff = mTrack1->MidTpcPoint() - mTrack2->MidTpcPoint();
  //cout << "Mid XY Sep" << diff.perp() << endl;
  return (double)(diff.perp());
}

double 
StEStructPairCuts::MidTpcSeparation() const {
  StThreeVectorF off(0,0,mZoffset);
  StThreeVectorF diff = mTrack1->MidTpcPoint() - mTrack2->MidTpcPoint() + off;
  //cout << "Mid XY Sep" << diff.perp() << endl;
  return (double)(diff.mag());
}

double 
StEStructPairCuts::MidTpcZSeparation() const {
  StThreeVectorF diff = mTrack1->MidTpcPoint() - mTrack2->MidTpcPoint();
  //cout << "Mid Z Sep" << fabs(diff.z()) << endl;
  return (double)(fabs(diff.z()+mZoffset));
}


/***********************************************************************
 *
 * $Log: StEStructPairCuts.cxx,v $
 * Revision 1.7  2007/05/27 22:45:03  msd
 * Added new cut bin modes 2 (soft/hard SS/AS), 6 (z-vertex binning), and 7 (modes 2*6).
 * Fixed bug in merging cut.
 * Added a few histograms to 2pt corr.
 *
 * Revision 1.6  2007/01/26 17:17:10  msd
 * Implemented new binning scheme: dEta stored in array with bin centered at zero, dPhi array has bins centered at zero and pi.  Final DEtaDPhi has 25x25 bins with dPhi bin width of pi/12 so all major angles are centered in bins.
 *
 * Revision 1.5  2006/10/02 22:21:02  prindle
 * Store only quadrant of eta_Delta - phi_Delta array/histogram.
 * Store half of eta_Sigma - phi_Delta array/histogram.
 * This required modifications in Binning.
 * I had a bug in the pair loop (which left +- not fully symmetrized)
 * and had to make changes in cut bins for mode 5 (and 3 I think)
 * when I fixed this.
 * Also change crossing cut to use only two parameters, the sign of
 * the magnetic field being taken from the MuDst.
 *
 * Revision 1.4  2006/04/04 22:10:13  porter
 * a handful of changes (specific to correlations)
 *  - added StEStructQAHists so that if NOT input frm Maker, each analysis has its own
 *  - used ability to get any max,min val from the cut class - or z-vertex binning
 *  - put z-vertex binning into 1 place
 *  - switched back 1st line of pair cut method to keep pair if good, not to reject if bad.
 *  - Pair cut object is now pointer in correlations
 *  - some diagnostic printouts available from macro
 *  - Duncan's delta-phi binning change
 *
 * Revision 1.3  2005/09/14 17:14:25  msd
 * Large update, added new pair-cut system, added pair density plots for new analysis mode (4), added event mixing cuts (rewrote buffer for this)
 *
 * Revision 1.2  2004/06/25 03:11:49  porter
 * New cut-binning implementation and modified pair-cuts for chunhui to review
 *
 * Revision 1.1  2003/10/15 18:20:46  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/




