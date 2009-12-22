//Author :: Renee Fatemi
//Emulates BBC trigger

//StEvent
#include "StEventTypes.h"

//StMuDSTMaker
#include "StMuDSTMaker/COMMON/StMuTypes.hh"

//StBBC
#include "StTriggerUtilities/Bbc/StBbcTriggerSimu.h"

ClassImp(StBbcTriggerSimu)
//==================================================
//==================================================

StBbcTriggerSimu::StBbcTriggerSimu(){
    BBCadcNum=48;
}
//==================================================
//==================================================

StBbcTriggerSimu::~StBbcTriggerSimu(){
} 
//==================================================
//==================================================
void  
StBbcTriggerSimu::Init(){

  LOG_INFO <<Form("Bbc::Init() MC_flag=%d, adcThres=%d",mMCflag,AdcTrigThresh)<<endm;
  if(mMCflag) LOG_WARN <<"Bbc:: TDC thresholds not implemented"<<endm;
}

//==================================================
//==================================================
void StBbcTriggerSimu::Clear(){

    bbcTrig=kNo;
    Wbbc=0;
    Ebbc=0;
    for (int i=0;i<BBCadcNum;i++) {
      BBCadc[i]=0;
    }

}
//==================================================
//==================================================

void StBbcTriggerSimu::Make()
{

  if (mSource == "MuDst") {
    StMuDst* mudst = (StMuDst*)StMaker::GetChain()->GetDataSet("MuDst");
    if (mudst) Make(mudst);
  }
  else if (mSource == "StEvent") {
    StEvent* event = (StEvent*)StMaker::GetChain()->GetDataSet("StEvent");
    if (event) Make(event);
  }

  if ((Ebbc==1)&&(Wbbc==1)) bbcTrig=kYes;

  LOG_DEBUG<<" Wbbc ="<<Wbbc<<" Ebbc="<<Ebbc<<" bbcTrig="<<bbcTrig<<endm;
  for (int i=0;i<BBCadcNum;i++) {
    LOG_DEBUG<<i<<" adc="<<BBCadc[i]<<endm;
  }

}

void StBbcTriggerSimu::Make(StMuDst*)
{
  StBbcTriggerDetector& bbc = StMuDst::event()->bbcTriggerDetector();
  Make(bbc);
}

void StBbcTriggerSimu::Make(StEvent* event)
{
  StTriggerDetectorCollection* trig = event->triggerDetectorCollection();
  if (trig) {
    StBbcTriggerDetector& bbc = trig->bbc();
    Make(bbc);
  }
}

void StBbcTriggerSimu::Make(StBbcTriggerDetector& bbc)
{
  for (UInt_t pmt=0; pmt<bbc.numberOfPMTs(); pmt++) {

    BBCadc[pmt]=bbc.adc(pmt);
    
    int bbcadc=bbc.adc(pmt);

    if (bbcadc>AdcTrigThresh) {
      if (pmt<16) Ebbc=1;
      if (23<pmt && pmt<40) Wbbc=1;
    }
  }
}


//
// $Log: StBbcTriggerSimu.cxx,v $
// Revision 1.5  2009/12/22 18:11:01  pibero
// Added ability to set input source (MuDst or StEvent) for BBC trigger simulator.
//
// Revision 1.4  2007/11/08 20:59:43  kocolosk
// subdet isTrigger returns a bool
// triggerDecision returns enumerator including kDoNotCare
//
// Revision 1.3  2007/07/23 02:59:56  balewski
// cleanup, bbc for M-C still not working
//










