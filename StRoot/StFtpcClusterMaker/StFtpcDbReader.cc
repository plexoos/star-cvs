// $Id: StFtpcDbReader.cc,v 1.1 2001/03/06 23:34:00 jcs Exp $
//
// $Log: StFtpcDbReader.cc,v $
// Revision 1.1  2001/03/06 23:34:00  jcs
// use database instead of params
//
//

#include "StFtpcDbReader.hh"
#include "StFtpcParamReader.hh"
#include "StMessMgr.h"

StFtpcDbReader::StFtpcDbReader(StFtpcParamReader *paramReader,
                               St_ftpcPadrowZ       *zrow,
                               St_ftpcEField        *efield,
                               St_ftpcVDrift        *vdrift,
                               St_ftpcDeflection    *deflection,
                               St_ftpcdVDriftdP     *dvdriftdp,
                               St_ftpcdDeflectiondP *ddeflectiondp,
                               St_ftpcAmpSlope      *ampslope,
                               St_ftpcAmpOffset     *ampoffset,
                               St_ftpcTimeOffset    *timeoffset )
{
  mParam = paramReader;

  //  just copy zrow table start to pointer
  ftpcPadrowZ_st* padrowzTable = (ftpcPadrowZ_st*)zrow->GetTable();
  if(padrowzTable){
   mPadrowZPosition = (Float_t *)padrowzTable->z;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcPadrowZ","E");
  }

  //  just copy EField table start to pointer
  ftpcEField_st* efieldTable = (ftpcEField_st*)efield->GetTable();
  if(efieldTable){
   mMagboltzEField = (Float_t *)efieldTable->e;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcEField","E");
  }

  //  just copy VDrift table start to pointer
  ftpcVDrift_st* vdriftTable = (ftpcVDrift_st*)vdrift->GetTable();
  if(vdriftTable){
   mMagboltzVDrift = (Float_t *)vdriftTable->v;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcVDrift","E");
  }

  //  just copy Deflection table start to pointer
  ftpcDeflection_st* deflectionTable = (ftpcDeflection_st*)deflection->GetTable();
  if(deflectionTable){
   mMagboltzDeflection = (Float_t *)deflectionTable->psi;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcDeflection","E");
  }

  //  just copy dVDriftdP table start to pointer
  ftpcdVDriftdP_st* dvriftdpTable = (ftpcdVDriftdP_st*)dvdriftdp->GetTable();
  if(dvriftdpTable){
   mMagboltzdVDriftdP = (Float_t *)dvriftdpTable->dv_dp;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcdVDriftdP","E");
  }

  //  just copy dDeflectiondP table start to pointer
  ftpcdDeflectiondP_st* ddeflectiondpTable = (ftpcdDeflectiondP_st*)ddeflectiondp->GetTable();
  if(ddeflectiondpTable){
   mMagboltzdDeflectiondP = (Float_t *)ddeflectiondpTable->dpsi_dp;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcdDeflectiondP","E");
  }

  //  just copy AmpSlope table start to pointer
  ampslopeTable = (ftpcAmpSlope_st*)ampslope->GetTable();
  if(!ampslopeTable){
    gMessMgr->Message( " No data in table class St_ftpcAmpSlope","E");
  }

  //  just copy AmpOffset table start to pointer
  ampoffsetTable = (ftpcAmpOffset_st*)ampoffset->GetTable();
  if(!ampoffsetTable){
    gMessMgr->Message( " No data in table class St_ftpcAmpOffset","E");
  }

  //  just copy TimeOffset table start to pointer
  timeoffsetTable = (ftpcTimeOffset_st*)timeoffset->GetTable();
  if(!timeoffsetTable){
    gMessMgr->Message( " No data in table class St_ftpcTimeOffset","E");
  }


//   cout << "StFtpcDbReader constructed" << endl;  
}

StFtpcDbReader::StFtpcDbReader(StFtpcParamReader *paramReader,
                               St_ftpcPadrowZ       *zrow,
                               St_ftpcEField        *efield,
                               St_ftpcVDrift        *vdrift,
                               St_ftpcDeflection    *deflection,
                               St_ftpcdVDriftdP     *dvdriftdp,
                               St_ftpcdDeflectiondP *ddeflectiondp)
{
  mParam = paramReader;

  //  just copy zrow table start to pointer
  ftpcPadrowZ_st* padrowzTable = (ftpcPadrowZ_st*)zrow->GetTable();
  if(padrowzTable){
   mPadrowZPosition = (Float_t *)padrowzTable->z;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcPadrowZ","E");
  }

  //  just copy EField table start to pointer
  ftpcEField_st* efieldTable = (ftpcEField_st*)efield->GetTable();
  if(efieldTable){
   mMagboltzEField = (Float_t *)efieldTable->e;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcEField","E");
  }

  //  just copy VDrift table start to pointer
  ftpcVDrift_st* vdriftTable = (ftpcVDrift_st*)vdrift->GetTable();
  if(vdriftTable){
   mMagboltzVDrift = (Float_t *)vdriftTable->v;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcVDrift","E");
  }

  //  just copy Deflection table start to pointer
  ftpcDeflection_st* deflectionTable = (ftpcDeflection_st*)deflection->GetTable();
  if(deflectionTable){
   mMagboltzDeflection = (Float_t *)deflectionTable->psi;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcDeflection","E");
  }

  //  just copy dVDriftdP table start to pointer
  ftpcdVDriftdP_st* dvriftdpTable = (ftpcdVDriftdP_st*)dvdriftdp->GetTable();
  if(dvriftdpTable){
   mMagboltzdVDriftdP = (Float_t *)dvriftdpTable->dv_dp;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcdVDriftdP","E");
  }

  //  just copy dDeflectiondP table start to pointer
  ftpcdDeflectiondP_st* ddeflectiondpTable = (ftpcdDeflectiondP_st*)ddeflectiondp->GetTable();
  if(ddeflectiondpTable){
   mMagboltzdDeflectiondP = (Float_t *)ddeflectiondpTable->dpsi_dp;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcdDeflectiondP","E");
  }


//   cout << "StFtpcDbReader constructed" << endl;  
}

StFtpcDbReader::~StFtpcDbReader()
{

//   cout << "StFtpcDbReader destructed" << endl;
}



Float_t StFtpcDbReader::padrowZPosition(Int_t i) 
{
  if(i>=0 && i<mParam->numberOfPadrows())
    {
      return mPadrowZPosition[i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: padrowZPosition index out of range, using 0", "W", "OST");
      return mPadrowZPosition[0];
    }
}


Float_t StFtpcDbReader::magboltzEField(Int_t i)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins())
    {
      return mMagboltzEField[i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzEField index out of range, using 0", "W", "OST");
      return mMagboltzEField[0];
    }
}

Float_t StFtpcDbReader::magboltzVDrift(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      return mMagboltzVDrift[padrow+mParam->numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzVDrift index out of range, using 0", "W", "OST");
      return mMagboltzVDrift[0];
    }
}

Float_t StFtpcDbReader::magboltzDeflection(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      return mMagboltzDeflection[padrow+mParam->numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzDeflection index out of range, using 0", "W", "OST");
      return mMagboltzDeflection[0];
    }
}

Float_t StFtpcDbReader::magboltzdVDriftdP(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      return mMagboltzdVDriftdP[padrow+mParam->numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdVDriftdP index out of range, using 0", "W", "OST");
      return mMagboltzdVDriftdP[0];
    }
}

Float_t StFtpcDbReader::magboltzdDeflectiondP(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      return mMagboltzdDeflectiondP[padrow+mParam->numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdDeflectiondP index out of range, using 0", "W", "OST");
      return mMagboltzdDeflectiondP[0];
    }
}

Float_t StFtpcDbReader::amplitudeSlope(Int_t i, Int_t padrow)
{
  if(i>0 && i<=(mParam->numberOfSectors()*mParam->numberOfPads()) && padrow>=0 && padrow<mParam->numberOfPadrows())
    {
       return ampslopeTable[padrow].slope[i-1];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: amplitudeSlope index out of range, using 0", "W", "OST");
      return ampslopeTable[0].slope[0];
    }
}

Float_t StFtpcDbReader::amplitudeOffset(Int_t i, Int_t padrow)
{
  if(i>0 && i<=(mParam->numberOfSectors()*mParam->numberOfPads()) && padrow>=0 && padrow<mParam->numberOfPadrows())
    {
       return ampoffsetTable[padrow].offset[i-1];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: amplitudeOffset index out of range, using 0", "W", "OST");
      return ampoffsetTable[0].offset[0];
    }
}

Float_t StFtpcDbReader::timeOffset(Int_t i, Int_t padrow)
{
  if(i>0 && i<=(mParam->numberOfSectors()*mParam->numberOfPads()) && padrow>=0 && padrow<mParam->numberOfPadrows())
    {
       return timeoffsetTable[padrow].offset[i-1];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: timeOffset index out of range, using 0", "W", "OST");
      return timeoffsetTable[0].offset[0];
    }
}


Int_t StFtpcDbReader::setMagboltzEField(Int_t i, Float_t newvalue)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins())
    {
      mMagboltzEField[i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzEField index out of range, not changed", "W", "OST");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzVDrift(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      mMagboltzVDrift[padrow+mParam->numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzVDrift index out of range, not changed", "W", "OST");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzDeflection(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      mMagboltzDeflection[padrow+mParam->numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzDeflection index out of range, not changed", "W", "OST");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzdVDriftdP(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      mMagboltzdVDriftdP[padrow+mParam->numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdVDriftdP index out of range, not changed", "W", "OST");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzdDeflectiondP(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mParam->numberOfMagboltzBins() && padrow>=0 && padrow<mParam->numberOfPadrowsPerSide())
    {
      mMagboltzdDeflectiondP[padrow+mParam->numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdDeflectiondP index out of range, not changed", "W", "OST");
      return 0;
    }
}
