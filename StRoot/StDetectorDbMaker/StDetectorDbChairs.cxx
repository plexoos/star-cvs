#include <assert.h>
#include "StarChairDefs.h"
#include "St_db_Maker/St_db_Maker.h"
//___________________Calibrations/ftpc_____________________________________________________________
#include "StDetectorDbFTPCGas.h"
StDetectorDbFTPCGas* StDetectorDbFTPCGas::fgInstance = 0; 
#include "St_ftpcGasSystemC.h"
MakeChairInstance(ftpcGasSystem,Calibrations/ftpc/ftpcGasSystem);
#include "St_ftpcGasOutC.h"
MakeChairInstance(ftpcGasOut,Calibrations/ftpc/ftpcGasOut);
#include "St_ftpcVoltageC.h"
MakeChairInstance(ftpcVoltage,Calibrations/ftpc/ftpcVoltage);
#include "St_ftpcVoltageStatusC.h"
MakeChairInstance(ftpcVoltageStatus,Calibrations/ftpc/ftpcVoltageStatus);
//__________________Calibrations/tpc______________________________________________________________
#include "St_tpcGasC.h"
MakeChairInstance(tpcGas,Calibrations/tpc/tpcGas);
#include "St_tpcGridLeakC.h"
MakeChairInstance(tpcGridLeak,Calibrations/tpc/tpcGridLeak);
#include "St_tpcOmegaTauC.h"
MakeChairInstance(tpcOmegaTau,Calibrations/tpc/tpcOmegaTau);
#include "St_tpcDriftVelocityC.h"
MakeChairInstance(tpcDriftVelocity,Calibrations/tpc/tpcDriftVelocity);
#include "St_TpcSecRowCorC.h"
MakeChairInstance(TpcSecRowCor,Calibrations/tpc/TpcSecRowCor);
#include "St_tpcEffectiveGeomC.h"
MakeChairInstance(tpcEffectiveGeom,Calibrations/tpc/tpcEffectiveGeom);
#include "St_tpcElectronicsC.h"
MakeChairInstance(tpcElectronics,Calibrations/tpc/tpcElectronics);
#include "St_tpcPedestalC.h"
MakeChairInstance(tpcPedestal,Calibrations/tpc/tpcPedestal);
#include "St_tpcPadResponseC.h"
MakeChairInstance(tpcPadResponse,Calibrations/tpc/tpcPadResponse);
#include "St_tpcSlowControlSimC.h"
MakeChairInstance(tpcSlowControlSim,Calibrations/tpc/tpcSlowControlSim);
#include "St_tpcHitErrorsC.h"
MakeChairInstance(tpcHitErrors,Calibrations/tpc/tpcHitErrors);
#include "St_tpcGainMonitorC.h"
MakeChairInstance(tpcGainMonitor,Calibrations/tpc/tpcGainMonitor);
#include "St_tpcHighVoltagesC.h"
MakeChairInstance(tpcHighVoltages,Calibrations/tpc/tpcHighVoltages);
#include "St_tpcPadrowT0C.h"
MakeChairInstance(tpcPadrowT0,Calibrations/tpc/tpcPadrowT0);
#include "St_tpcSectorT0offsetC.h"
MakeChairInstance(tpcSectorT0offset,Calibrations/tpc/tpcSectorT0offset);
#include "St_TpcAltroParametersC.h"
MakeChairInstance(TpcAltroParameters,Calibrations/tpc/TpcAltroParameters);
#include "St_asic_thresholdsC.h"
MakeChairInstance(asic_thresholds,Calibrations/tpc/asic_thresholds);
#include "St_asic_thresholds_tpxC.h"
MakeChairInstance(asic_thresholds_tpx,Calibrations/tpc/asic_thresholds_tpx);
#include "St_tpcAnodeHVC.h"
MakeChairInstance(tpcAnodeHV,Calibrations/tpc/tpcAnodeHV);
//________________________________________________________________________________
void  St_tpcAnodeHVC::sockets(Int_t sector, Int_t padrow, Int_t &e1, Int_t &e2, Float_t &f2) {
  e1 = (sector-1)*19;
  e2 = e1;
  f2 = 0;
  // sector=1..24 , padrow=1..45
  // f2 represents signal couplings from neighboring HV sections
  // see: http://www.star.bnl.gov/public/tpc/hard/signals/signal_division.html
  switch (padrow) {
    case  1: e1+= 1; e2+= 2; f2 = 0.00197; break;
    case  2: e1+= 2; break;
    case  3: e1+= 3; e2+= 2; f2 = 0.04547; break;
    case  4: e1+= 3; break;
    case  5: e1+= 4; break;
    case  6: e1+= 4; e2+= 5; f2 = 0.00007; break;
    case  7: e1+= 5; break;
    case  8: e1+= 6; e2+= 5; f2 = 0.04547; break;
    case  9: e1+= 6; break;
    case 10: e1+= 7; break;
    case 11: e1+= 8; e2+= 7; f2 = 0.33523; break;
    case 12: e1+= 8; break;
    case 13: e1+=17; break;
    case 14: e1+= 9; e2+=18; f2 = 0.00312; break;
    case 15:
    case 16: e1+= 9; break;
    case 17: e1+= 9; e2+=10; f2 = 0.40250; break;
    case 18:
    case 19:
    case 20: e1+=10; break;
    case 21: e1+=10; e2+=11; f2 = 0.40250; break;
    case 22:
    case 23:
    case 24: e1+=11; break;
    case 25: e1+=11; e2+=12; f2 = 0.40250; break;
    case 26:
    case 27:
    case 28: e1+=12; break;
    case 29: e1+=12; e2+=13; f2 = 0.40250; break;
    case 30:
    case 31:
    case 32: e1+=13; break;
    case 33: e1+=13; e2+=14; f2 = 0.40250; break;
    case 34:
    case 35:
    case 36: e1+=14; break;
    case 37: e1+=14; e2+=15; f2 = 0.40250; break;
    case 38:
    case 39:
    case 40: e1+=15; break;
    case 41: e1+=15; e2+=16; f2 = 0.40250; break;
    case 42:
    case 43:
    case 44: e1+=16; break;
    case 45: e1+=16; e2+=19; f2 = 0.40250; break;
    default: e1 = 0; e2 = 0; f2 = 0;
  }
}
//________________________________________________________________________________
Float_t St_tpcAnodeHVC::voltagePadrow(Int_t sector, Int_t padrow) const {
  Int_t e1 = 0, e2 = 0;
  Float_t f2 = 0;
  St_tpcAnodeHVC::sockets(sector, padrow, e1, e2, f2);
  if (e1==0) return -99;
  Float_t v1=voltage(e1-1);
  if (f2==0) return v1;
  Float_t v2=voltage(e2-1);
  if (v2==v1) return v1;
  // different voltages on influencing HVs
  // effective voltage is a sum of exponential gains
  Float_t B = (padrow <= 13 ? 13.05e-3 : 10.26e-3);
  Float_t v_eff = TMath::Log((1.0-f2)*TMath::Exp(B*v1) + f2*TMath::Exp(B*v2)) / B;
  return v_eff;
}
#include "St_tpcAnodeHVavgC.h"
MakeChairInstance(tpcAnodeHVavg,Calibrations/tpc/tpcAnodeHVavg);
//________________________________________________________________________________
Float_t St_tpcAnodeHVavgC::voltagePadrow(Int_t sector, Int_t padrow) const {
  Int_t e1 = 0, e2 = 0;
  Float_t f2 = 0;
  St_tpcAnodeHVC::sockets(sector, padrow, e1, e2, f2);
  if (e1==0) return -99;
  Float_t v1=voltage(e1-1);
  if (f2==0) return v1;
  Float_t v2=voltage(e2-1);
  if (v2==v1) return v1;
  // different voltages on influencing HVs
  // effective voltage is a sum of exponential gains
  Float_t B = (padrow <= 13 ? 13.05e-3 : 10.26e-3);
  Float_t v_eff = TMath::Log((1.0-f2)*TMath::Exp(B*v1) + f2*TMath::Exp(B*v2)) / B;
  return v_eff;
}
//________________________________________________________________________________
#include "St_tpcPadGainT0C.h"
MakeChairInstance(tpcPadGainT0,Calibrations/tpc/tpcPadGainT0);
#include "St_tpcSlewingC.h"
MakeChairInstance(tpcSlewing,Calibrations/tpc/tpcSlewing);
//__________________Calibrations/trg______________________________________________________________
#include "St_defaultTrgLvlC.h"
MakeChairInstance(defaultTrgLvl,Calibrations/trg/defaultTrgLvl);
#include "St_trigDetSumsC.h"
St_trigDetSumsC *St_trigDetSumsC::fgInstance = 0;
ClassImp(St_trigDetSumsC);
//___________________tpc_____________________________________________________________
#include "St_tss_tssparC.h"
MakeChairInstance(tss_tsspar,tpc/tsspars/tsspar);
Float_t St_tss_tssparC::gain_in(Int_t i) {
  return Struct(i)->gain_in;
}
//________________________________________________________________________________
Float_t St_tss_tssparC::gain_in(Int_t sec, Int_t row) {
  Float_t V = St_tpcAnodeHVavgC::instance()->voltagePadrow(sec,row);
  Float_t gain = gain_in();
  gain *= (V > 0) ? TMath::Exp(13.087e-3*(V-1170)) : 0; //Run IX: -4.580535e-01/35V; 13.05e-3 => -13.087e-3
  return gain;
}
//________________________________________________________________________________
Float_t St_tss_tssparC::gain_out(Int_t i) 	{
  return Struct(i)->gain_out;
}
//________________________________________________________________________________
Float_t St_tss_tssparC::gain_out(Int_t sec, Int_t row) {
  Float_t V = St_tpcAnodeHVavgC::instance()->voltagePadrow(sec,row);
  Float_t gain = gain_out();
  gain *= (V > 0) ? TMath::Exp(10.211e-3*(V-1390)) : 0; //Run IX: -4.59477e-01/45V; 10.26e-3 => -10.211e-3
  return gain;
}
//________________________________________________________________________________
Float_t St_tss_tssparC::gain(Int_t sec, Int_t row) {
  return row <= 13 ? gain_in(sec,row) : gain_out(sec,row);
}
//__________________Calibrations/rich______________________________________________________________
#include "StDetectorDbRichScalers.h"
StDetectorDbRichScalers *StDetectorDbRichScalers::fgInstance = 0;
ClassImp(StDetectorDbRichScalers);
#include "St_richvoltagesC.h"
MakeChairInstance(richvoltages,Calibrations/rich/richvoltages);
#include "St_y1MultC.h"
MakeChairInstance(y1Mult,Calibrations/rich/y1Mult);
#include "St_spaceChargeCorC.h"
ClassImp(St_spaceChargeCorC);
MakeChairInstance2(spaceChargeCor,St_spaceChargeCorR1C,Calibrations/rich/spaceChargeCor);
MakeChairInstance2(spaceChargeCor,St_spaceChargeCorR2C,Calibrations/rich/spaceChargeCorR2);
//_________________RunLog_____________________________________________________________
#include "St_MagFactorC.h"
MakeChairInstance(MagFactor,RunLog/MagFactor);
//_________________RunLog/onl_______________________________________________________________
#include "St_starClockOnlC.h"
MakeChairInstance(starClockOnl,RunLog/onl/starClockOnl);
//________________________________________________________________________________
starClockOnl_st *St_starClockOnlC::Struct(Int_t i) {
  starClockOnl_st *s = ((St_starClockOnl* ) instance()->Table())->GetTable(); 
  Int_t N =  getNumRows(); // with i < 0 look for positive frequency
  if (i >= 0 && i < N) return s + i;
  for (Int_t j = 0; j < N; j++, s++) if (s->frequency > 0) break;
  assert(s->frequency > 0 && s->frequency < 1e7);
  return s;
}
#include "St_starMagOnlC.h"
MakeChairInstance(starMagOnl,RunLog/onl/starMagOnl);
#include "St_beamInfoC.h"
MakeChairInstance(beamInfo,RunLog/onl/beamInfo);
#include "St_tpcRDOMasksC.h"
MakeChairInstance(tpcRDOMasks,RunLog/onl/tpcRDOMasks);
//________________________________________________________________________________
UInt_t       St_tpcRDOMasksC::getSectorMask(UInt_t sector) {
  UInt_t MASK = 0x0000; // default is to mask it out
  //UInt_t MASK = 0xFFFF; // change to  ON by default ** THIS WAS A HACK
  if(sector < 1 || sector > 24 || getNumRows() == 0){
    LOG_WARN << "St_tpcRDOMasksC:: getSectorMask : return default mask for " 
	     << "sector= " << sector << " getNumRows()=" << getNumRows() << endm;
    return MASK;
  }
  MASK = mask(((sector + 1) / 2) - 1); // does the mapping from sector 1-24 to packed sectors
  if( sector % 2 == 0){ // if its even relevent bits are 6-11
    MASK = MASK >> 6;
  }
  // Otherwise want lower 6 bits    
  MASK &= 0x000003F; // Mask out higher order bits
  if (sector == 16 && MASK == 0 && runNumber() > 8181000 && runNumber() < 9181000) MASK = 4095;
  return MASK;
}
//________________________________________________________________________________
#include "St_triggerInfoC.h"
MakeChairInstance(triggerInfo,RunLog/onl/triggerInfo);
#include "St_triggerIDC.h"
MakeChairInstance(triggerID,RunLog/onl/triggerID);
//________________________________________________________________________________
#include "St_trigPrescalesC.h"
MakeChairOptionalInstance(trigPrescales,RunLog/onl/trigPrescales);
//________________________________________________________________________________
#include "St_L0TriggerInfoC.h"
MakeChairInstance(L0TriggerInfo,RunLog/onl/L0TriggerInfo);
#include "St_trigL3ExpandedC.h"
MakeChairOptionalInstance(trigL3Expanded,RunLog/onl/trigL3Expanded);
#include "St_dsmPrescalesC.h"
MakeChairOptionalInstance(dsmPrescales,RunLog/onl/dsmPrescales);
#include "St_additionalTriggerIDC.h"
MakeChairOptionalInstance(additionalTriggerID,RunLog/onl/additionalTriggerID);
#include "StDetectorDbTriggerID.h"
StDetectorDbTriggerID *StDetectorDbTriggerID::fgInstance = 0;
//________________________________________________________________________________
map<Int_t,Float_t> StDetectorDbTriggerID::getTotalPrescales() {
  map<Int_t,Float_t> value;
  // First walk forward through the multiple levels of prescales
  for (UInt_t irow=0;irow<getDsmPrescalesNumRows(); ++irow) {
    Int_t trgId = getDsmPrescalesTrgId(irow);
    value[trgId] = Float_t(getDsmPrescalesDsmPrescale(irow));
  }
  
  for (UInt_t irow=0; irow<getL0NumRows(); ++irow) {
    Int_t trgId = getL0OfflineTrgId(irow);
    map<Int_t,Float_t>::iterator p=value.find(trgId);
    if (p != value.end()) {
      (*p).second *= Float_t(getPsL0(irow));
    }
    else {
      value[trgId] = Float_t(getPsL0(irow));
    }
  }
  // For completeness: this one is always unity as far as I can tell
  for (UInt_t irow=0; irow<getSNumRows(); ++irow) {
    UInt_t idxTrigger = getIdxTrigger(irow);
    Int_t trgId = 0;
    for (UInt_t jrow=0; jrow<getIDNumRows(); ++jrow) {
      if (idxTrigger == getIdxTrg(jrow)) {
	trgId = getOfflineTrgId(jrow);
	break;
      }
    }
    map<Int_t,Float_t>::iterator p=value.find(trgId);
    
    if (p != value.end()) {
      (*p).second *= Float_t(getPs(irow));
    }
    else {
      value[trgId] = Float_t(getPs(irow));
    }
  }
  
  // Now deal with L3Expanded
  for (UInt_t irow=0; irow<getTrigL3ExpandedNumRows(); ++irow) {
    Int_t oldtid = getTrigL3ExpandedL3TrgId(irow);
    Int_t newtid = getTrigL3ExpandedL3ExpandedTrgId(irow);
    Float_t l2ps = getTrigL3ExpandedL2Ps(irow);
    
    map<Int_t,Float_t>::iterator p = value.find(oldtid);
    if (p!= value.end()) {
      value[newtid] = ((*p).second)*l2ps;
    }
    else {
      value[newtid] = l2ps;
    }
    
  }
  return value;
}
//________________________________________________________________________________
Float_t StDetectorDbTriggerID::getTotalPrescaleByTrgId(Int_t trgId) {
  map<Int_t,Float_t> theMap = getTotalPrescales();
  map<Int_t,Float_t>::const_iterator p = theMap.find(trgId);
  if (p != theMap.end()) {
    return (*p).second;
  }
  else {
    return 0;
  }
}
//________________________________________________________________________________
#include "StDetectorDbIntegratedTriggerID.h"
StDetectorDbIntegratedTriggerID *StDetectorDbIntegratedTriggerID::fgInstance = 0;
//___________________Conditions/trg_____________________________________________________________
#include "St_trgTimeOffsetC.h"
MakeChairInstance(trgTimeOffset,Conditions/trg/trgTimeOffset);
//___________________Geometry/tpc_____________________________________________________________
#include "St_tpcDimensionsC.h"
MakeChairInstance(tpcDimensions,Geometry/tpc/tpcDimensions);
#include "St_tpcWirePlanesC.h"
MakeChairInstance(tpcWirePlanes,Geometry/tpc/tpcWirePlanes);
#include "St_tpcSectorPositionC.h"
ClassImp(St_tpcSectorPositionC); 
St_tpcSectorPositionC *St_tpcSectorPositionC::fgInstance = 0; 
St_tpcSectorPosition  *St_tpcSectorPositionC::fgTables[24] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
St_tpcSectorPositionC *St_tpcSectorPositionC::instance() { 
  if (fgInstance) return fgInstance;					
  St_db_Maker *dbMk = (St_db_Maker *) StMaker::GetChain()->Maker("db");	
  for (Int_t sec = 1; sec <= 24; sec++) {
    TString path = Form("Geometry/tpc/Sector_%02i/tpcSectorPosition",sec);
    fgTables[sec-1] = (St_tpcSectorPosition  *) StMaker::GetChain()->GetDataBase(path.Data()); 
    if (! fgTables[sec-1]) {							
      LOG_WARN << "St_tpcSectorPositionC::instance " << path.Data() << "\twas not found" << endm; 
      assert(fgTables[sec-1]);							
    }						
    if (dbMk && dbMk->Debug() ) {						
      TDatime t[2];							
      dbMk->GetValidity(fgTables[sec-1],t);					        
      Int_t Nrows = fgTables[sec-1]->GetNRows();					
      LOG_WARN << "St_tpcSectorPositionC::instance found table " << fgTables[sec-1]->GetName() 
	       << " with NRows = " << Nrows << " in db" << endm;		
      LOG_WARN << "Validity:" << t[0].GetDate() << "/" << t[0].GetTime()	
	       << " -----   " << t[1].GetDate() << "/" << t[1].GetTime() << endm; 
      fgTables[sec-1]->Print(0,1);		
    }
  }			
  fgInstance = new St_tpcSectorPositionC();				
  return fgInstance;							
}
#include "St_tpcFieldCageC.h"
MakeChairInstance(tpcFieldCage,Geometry/tpc/tpcFieldCage);
#include "St_tpcPadPlanesC.h"
MakeChairInstance(tpcPadPlanes,Geometry/tpc/tpcPadPlanes);
#include "St_tpcGlobalPositionC.h"
MakeChairInstance(tpcGlobalPosition,Geometry/tpc/tpcGlobalPosition);
#include "St_tpcFieldCageShortC.h"
MakeChairInstance(tpcFieldCageShort,Geometry/tpc/tpcFieldCageShort);
#include "St_SurveyC.h"
#include "St_SurveyC.h"
ClassImp(St_SurveyC);
#include "StSvtSurveyC.h"
MakeChairInstance2(Survey,StSvtOnGlobal,Geometry/svt/SvtOnGlobal);
MakeChairInstance2(Survey,StSvtShellOnGlobal,Geometry/svt/ShellOnGlobal);
MakeChairInstance2(Survey,StSvtLadderOnSurvey,Geometry/svt/LadderOnSurvey);
MakeChairInstance2(Survey,StSvtLadderOnShell,Geometry/svt/LadderOnShell);
MakeChairInstance2(Survey,StSvtWaferOnLadder,Geometry/svt/WaferOnLadder);
#include "StSsdSurveyC.h"
MakeChairInstance2(Survey,StSsdOnGlobal,Geometry/ssd/SsdOnGlobal);
MakeChairInstance2(Survey,StSsdSectorsOnGlobal,Geometry/ssd/SsdSectorsOnGlobal);
MakeChairInstance2(Survey,StSsdLaddersOnSectors,Geometry/ssd/SsdLaddersOnSectors);
MakeChairInstance2(Survey,StSsdWafersOnLadders,Geometry/ssd/SsdWafersOnLadders);
//________________________________________________________________________________
void St_SurveyC::GetAngles(Double_t &phi, Double_t &the, Double_t &psi, Int_t i) {
  phi = the = psi = 0;  // Korn 14.10-5
  Double_t cosDelta = (r00(i) + r11(i) + r22(i) - 1)/2; // (Tr(R) - 1)/2
  Double_t Delta = TMath::ACos(cosDelta);      
  if (Delta < 0) Delta += 2*TMath::Pi();  
  Double_t sinDelta2 = TMath::Sin(Delta/2);
  if (TMath::Abs(sinDelta2) < 1.e-7) return;
  Double_t c[3] = {
    (r21(i) - r12(i))/(2*sinDelta2), // a32-a23
    (r02(i) - r20(i))/(2*sinDelta2), // a13-a31
    (r10(i) - r01(i))/(2*sinDelta2)  // a21-a12
  };
  Double_t u = TMath::ATan2(c[0],c[1]);
  Double_t v = TMath::ATan(c[2]*TMath::Tan(Delta/2));
  phi = (v - u)/2 - TMath::Pi()/2;
  psi = (v + u)/2 - TMath::Pi()/2;
  the = 2*TMath::ATan2(c[0]*TMath::Sin(v),c[2]*TMath::Sin(u));
  Double_t raddeg = 180./TMath::Pi();
  phi   *= raddeg;
  the   *= raddeg;
  psi   *= raddeg;
}
//________________________________________________________________________________
St_SurveyC   *St_SurveyC::instance(const Char_t *name) {
  TString Name(name);
  if (Name == "SvtOnGlobal")          return (St_SurveyC   *) StSvtOnGlobal::instance();	    
  if (Name == "ShellOnGlobal")        return (St_SurveyC   *) StSvtShellOnGlobal::instance();  
  if (Name == "LadderOnSurvey")       return (St_SurveyC   *) StSvtLadderOnSurvey::instance(); 
  if (Name == "LadderOnShell")        return (St_SurveyC   *) StSvtLadderOnShell::instance();  
  if (Name == "WaferOnLadder")        return (St_SurveyC   *) StSvtWaferOnLadder::instance();  
  if (Name == "SsdOnGlobal")          return (St_SurveyC   *) StSsdOnGlobal::instance();
  if (Name == "SsdSectorsOnGlobal")   return (St_SurveyC   *) StSsdSectorsOnGlobal::instance();
  if (Name == "SsdLaddersOnSectors")  return (St_SurveyC   *) StSsdLaddersOnSectors::instance();
  if (Name == "SsdWafersOnLadders")   return (St_SurveyC   *) StSsdWafersOnLadders::instance();
  return 0;
}
