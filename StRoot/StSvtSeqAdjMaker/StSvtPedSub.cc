/***************************************************************************
 *
 * $Id: StSvtPedSub.cc,v 1.4 2000/08/21 12:57:30 caines Exp $
 *
 * Author: Helen Caines
 ***************************************************************************
 *
 * Description: SVT Pedestal Subtraction Code
 *
 ***************************************************************************
 *
 * $Log: StSvtPedSub.cc,v $
 * Revision 1.4  2000/08/21 12:57:30  caines
 * Now opens and reads in ped using CalibMaker
 *
 * Revision 1.3  2000/07/16 22:32:23  caines
 * Now also saves RAW data
 *
 * Revision 1.2  2000/07/03 02:07:56  perev
 * StEvent: vector<TObject*>
 *
 * Revision 1.1  2000/06/15 20:04:54  caines
 * Initial versions of sequence adjusting codes
 *
 *
 **************************************************************************/

#include "StSvtPedSub.h"
#include "StSequence.hh"
#include "StSvtClassLibrary/StSvtHybridData.hh"
#include "StSvtClassLibrary/StSvtHybridPed.hh"
#include "StSvtClassLibrary/StSvtHybridCollection.hh"


StSvtPedSub::StSvtPedSub( StSvtHybridCollection *PedPointer)
{
  mPed = NULL;
  mSvtPed = PedPointer;
}



//_____________________________________________________________________________


int StSvtPedSub::SubtractPed( StSvtHybridData* fData, int Index, int PedOffset)
{


  int nAnodes, anodeID, nSeq, iseq, time, newAdc, status;
  StSequence* Seq;
  int* anodeList;
	  
  anodeList = NULL;

  nAnodes = fData->getAnodeList(anodeList);
  mPed = (StSvtHybridPed *) mSvtPed->at(Index);
  
  for (int ianode=0;ianode<nAnodes;ianode++) {
    
    anodeID = anodeList[ianode];
    Seq = NULL;
    nSeq = 0;
    
    status = fData->getSequences(anodeID,nSeq,Seq);
    for (iseq=0;iseq<nSeq;iseq++) {	  	  
      for (time=Seq[iseq].startTimeBin; time<Seq[iseq].startTimeBin+Seq[iseq].length; time++) {
	
	// Actually subtract the pedestal per pixel. PedOffset  
	//allows undershoot to be seen 
		newAdc= (int)Seq[iseq].firstAdc[time]-
		  (int) mPed->getPixelContent(anodeID,time)
		  +PedOffset;
	//Check adc hasn't gone -ve
	if( newAdc < 0) newAdc=0;
	else if( newAdc >= 256) newAdc=255;
	Seq[iseq].firstAdc[time]= newAdc;
	
      }
    }
  }
  
  return 0;
}
  

//_____________________________________________________________________________


