#ifndef STAR_StRTSBaseMaker_H
#define STAR_StRTSBaseMaker_H

/***************************************************************************
 *
 * $Id: StRTSBaseMaker.h,v 1.8 2009/11/10 17:41:20 fine Exp $
 * StRTSBaseMaker - class to fille the StEvewnt from DAQ reader
 *--------------------------------------------------------------------------
 *
 ***************************************************************************/

#include "StMaker.h"

class StRtsTable;


//! \author Valery Fine(fine@bnl.gov)
//! \date 27/04/2008

/*! \brief  Class StRTSBaseMaker - is an abstract StMaker to define 
 *       the  interface to access the DAQ data from the STAR production chain
 */
/// \author Valery Fine (fine@bnl.gov)
/// \date 27/04/2008
/// \sa StRtsReaderMaker.h
///
///  Class provides the interface and the default implementation to access the DAQ data 
///  defined by \sa http://docs.google.com/Doc?docid=dgv8pf9t_60dwhg3zd4&hl=en "A DAQ_READER Cookbook"/n
///  The user has to subclass it to re-implement the pure virtual function StRTSBaseMaker::Make() (\sa StMaker::Make() )
///  (\sa the http://en.wikipedia.org/wiki/Virtual_function#C.2B.2B_2 "Virtual function" )
///  StRTSBaseMaker  is a base class (\sa http://en.wikipedia.org/wiki/Base_class ""A DAQ_READER Cookbook" ) 
///  to implement the concrete StMaker class accessing 
///  and parse the concrete detector  subsystem DAQ information and make it available and suitable 
///  for the offline production.
///
class StRTSBaseMaker : public StMaker
{
   private:
     static const char *fRTSRootDataset; // The name of the Root dataset
     StRtsTable   *fDaq_Dta;
     TString        fDetectorName;     // name of the detector

   protected:
      StRtsTable *GetNextDaqElement(const char *elementPath);
      StRtsTable *DaqDta() {return fDaq_Dta;}
      StRtsTable *GetNext(const char* bank);

      virtual StRtsTable *GetNextRaw();
      virtual StRtsTable *GetNextAdc();
      virtual StRtsTable *GetNextLegacy();

   public:

     StRTSBaseMaker(const char *detectorName,const char *makerName="");
     virtual ~StRTSBaseMaker() ;
 
     Int_t Sector () const;
     Int_t Pad () const;
     Int_t Rdo () const;
     Int_t Row () const;

     // DAQ Event raw data

     static UInt_t Token();      //< current token
     static UInt_t Trgcmd();     //< current trigger command
     static UInt_t Daqcmd();     //< current DAQ command
     static UInt_t Trgword();    //< the Trigger Word
     static UInt_t Phyword();    //< the Physics Word
     static UInt_t Daqbits();    //< "offline" bits aka L3 summary...
     static UInt_t Daqbits_l1(); //< triggers satisfying l1 
     static UInt_t Daqbits_l2(); //< triggers satisfying l2
     static UInt_t Evpgroups() ; //< evp groups aka L3 summary[2]     
  
     UInt_t Detectors()  const;  //< detectors present bit mask according to DAQ!
     
     const TString &DetectorName() const { return fDetectorName; }

     virtual Int_t Make() = 0;

  // cvs
  virtual const char *GetCVS() const
    {
      static const char cvs[]="Tag $Name:  $Id: built "__DATE__" "__TIME__ ; return cvs;
    }
  
  ClassDef(StRTSBaseMaker, 1)    //StRTSBaseMaker - class to fille the StEvewnt from DAQ reader
};


#endif
