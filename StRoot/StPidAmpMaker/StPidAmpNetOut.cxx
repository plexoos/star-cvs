/***************************************************************************
 *
 * $Id: StPidAmpNetOut.cxx,v 1.2 2000/03/24 15:10:44 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             This is the object to be written out.
 *             It contains: (if all filled)
 *             1.Parameters for describing the band
 *             2.Parameters for describing the amplitude
 *             3.Parameters for describing the resolution
 *             4.Parameters for calibration
 *             5.Geant ID for the net type
 ***************************************************************************
 *
 * $Log: StPidAmpNetOut.cxx,v $
 * Revision 1.2  2000/03/24 15:10:44  aihong
 * add PrintContent()
 *
 * Revision 1.1.1.1  2000/03/09 17:48:33  aihong
 * Installation of package
 *
 **************************************************************************/


#include "StPidAmpNetOut.h"

ClassImp(StPidAmpNetOut)

StPidAmpNetOut::StPidAmpNetOut(){
    mCalibConst=0.0;
}

StPidAmpNetOut::StPidAmpNetOut(const StPidAmpNetOut& netOut){
   mGeantID     =netOut.mGeantID;
   mBandParArray=netOut.mBandParArray;
   mAmpParArray =netOut.mAmpParArray;
   mResoParArray=netOut.mResoParArray;
   mCalibConst  =netOut.mCalibConst;
   SetName(netOut.GetName());
   SetTitle(netOut.GetTitle());
  }


StPidAmpNetOut::StPidAmpNetOut(Text_t* name,Text_t* title,Int_t id,TArrayD bandParAry, TArrayD ampParAry, TArrayD linrParAry) : TNamed(name,title) {

  mGeantID=id;
  mBandParArray=bandParAry;
  mAmpParArray=ampParAry;
  mResoParArray=linrParAry;
  mCalibConst=0.0;
  }
StPidAmpNetOut::StPidAmpNetOut(const char* name,const char* title,Int_t id,TArrayD bandParAry, TArrayD ampParAry, TArrayD linrParAry) : TNamed(name,title) {

  mGeantID=id;
  mBandParArray=bandParAry;
  mAmpParArray=ampParAry;
  mResoParArray=linrParAry;
  mCalibConst=0.0;
  }
  StPidAmpNetOut::~StPidAmpNetOut(){}

void StPidAmpNetOut::SetBandParArray(TArrayD bandParAry){
  mBandParArray=bandParAry;
}

void StPidAmpNetOut::SetAmpParArray(TArrayD ampParAry){
  mAmpParArray=ampParAry;
}

void StPidAmpNetOut::SetResoParArray(TArrayD linrParAry){
  mResoParArray=linrParAry;
}

void StPidAmpNetOut::SetGeantID(Int_t id){
  mGeantID=id;
}
void StPidAmpNetOut::SetCalibConst(Double_t cal){
  mCalibConst=cal;
}
void StPidAmpNetOut::PrintContent(){
  int i;
  cout<<GetName()<<" Geant ID: "<<mGeantID<<endl;
  cout<<"Calibration constant: "<<mCalibConst<<endl;
  cout<<"band Parameters: { ";


  for (i=0; i<mBandParArray.GetSize();i++){

         cout<<mBandParArray.At(i);

       if ( i!=(mBandParArray.GetSize()-1))    cout<<", ";
       else cout<<" ";
  }

  cout<<"}"<<endl;
 
   cout<<"amplitude Parameters: { ";



  for (i=0; i<mAmpParArray.GetSize();i++){

         cout<<mAmpParArray.At(i);

       if ( i!=(mAmpParArray.GetSize()-1))    cout<<", ";
       else cout<<" ";
  }

  cout<<"}"<<endl;
  
  cout<<"resolution Parameters: { ";


  for (i=0; i<mResoParArray.GetSize();i++){

         cout<<mResoParArray.At(i);

       if ( i!=(mResoParArray.GetSize()-1))    cout<<", ";
       else cout<<" ";
  }

  cout<<"}"<<endl;

 
  cout<<endl;

 
}





TArrayD   StPidAmpNetOut::GetBandParArray() const{ return mBandParArray;}
TArrayD   StPidAmpNetOut::GetAmpParArray() const{return mAmpParArray;}
TArrayD   StPidAmpNetOut::GetResoParArray() const{return mResoParArray;}
Int_t     StPidAmpNetOut::GetNBandPars() const{return mBandParArray.GetSize();}
Int_t     StPidAmpNetOut::GetNAmpPars() const{return mAmpParArray.GetSize();}
Int_t     StPidAmpNetOut::GetNResoPars() const{return mResoParArray.GetSize();}

Int_t     StPidAmpNetOut::GetGeantID() const{return mGeantID;}
Double_t  StPidAmpNetOut::GetCalibConst() const{return mCalibConst;}

ostream& operator<<(ostream& s, const StPidAmpNetOut& netOut){
  int i;
  s<<netOut.GetName()<<" Geant ID: "<<netOut.GetGeantID()<<endl;
  s<<"Calibration constant: "<<netOut.GetCalibConst()<<endl;

  s<<"band Parameters: { ";

  TArrayD theBandParArray=netOut.GetBandParArray();

  for (i=0; i<theBandParArray.GetSize();i++){

         s<<theBandParArray.At(i);

       if ( i!=(theBandParArray.GetSize()-1))    s<<", ";
       else s<<" ";
  }

  s<<"}"<<endl;
 
   s<<"amplitude Parameters: { ";

  TArrayD theAmpParArray=netOut.GetAmpParArray();


  for (i=0; i<theAmpParArray.GetSize();i++){

         s<<theAmpParArray.At(i);

       if ( i!=(theAmpParArray.GetSize()-1))    s<<", ";
       else s<<" ";
  }

  s<<"}"<<endl;
  
  s<<"resolution Parameters: { ";

  TArrayD theResoParArray=netOut.GetResoParArray();

  for (i=0; i<theResoParArray.GetSize();i++){

         s<<theResoParArray.At(i);

       if ( i!=(theResoParArray.GetSize()-1))    s<<", ";
       else s<<" ";
  }

  s<<"}"<<endl;

 
  s<<endl;

  return s;
}
