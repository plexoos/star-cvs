//_____________________________________________________________________
// @(#)StRoot/StBFChain:$Name:  $:$Id: StBFChain.cxx,v 1.486 2006/02/03 19:51:45 fine Exp $
//_____________________________________________________________________
#include "TROOT.h"
#include "TString.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "TClassTable.h"
#include "TMemStat.h"
#include "StBFChain.h"
#include "St_db_Maker/St_db_Maker.h"
#include "StTreeMaker/StTreeMaker.h"
#include "StIOMaker/StIOMaker.h"
#include "StChallenger/StChallenger.h"
#include "StDbUtilities/StMagUtilities.h"
#include "StMessMgr.h"

#define STAR_LOGGER 1

//_____________________________________________________________________
// PLease, preserve the comment after = { . It is used for documentation formatting
//
#if 0
// Keep to be compartible with old documentaion
#define STR_OBSOLETE "WARNING *** Option is OBSOLETE ***"
//#include "BFC.h"
// ITTF Chain will be put here. Option list starting from minimalistic requirements
// and may not initially work.
// Please, preserve the comment after = { . It is used for documentation formatting
//
// ITTF Chains
//#include "BFC2.h"
#endif

// NoChainOptions -> Number of chain options auto-calculated
Int_t NoChainOptions = 0;
St_Bfc *chainOpt = 0;
ClassImp(StBFChain)
  
//_____________________________________________________________________________
// Hack constructor.
/*!
 * This method can be called with mode 1 or 2 to enable chain setup 1 or chain
 * setup 2.
 *
 * Note: This constructor now accepts a second argument for the chain name.
 * Embedding scripts were modified accordingly.
 *
 * This was primarily set to make possible the transition between the regular
 * chain and the ITTF chain options.
 */
void StBFChain::Setup(Int_t mode) {
  static Char_t *path  = "./StRoot/StBFChain:$STAR/StRoot/StBFChain";
  TString chain("BFC.C");
  if (mode == 2) chain = "BFC2.C";
  Char_t *file = gSystem->Which(path,chain,kReadPermission);
#ifdef STAR_LOGGER     
  if (! file) { LOG_FATAL  << Form("StBFChain::Setup","File %s has not been found in path %s",chain.Data(),path) << endm; }
  else        { LOG_WARN   << Form("StBFChain::Setup","File %s has been found as %s",chain.Data(),file) << endm; }
#else
  if (! file)   Fatal("StBFChain::Setup","File %s has not been found in path %s",chain.Data(),path);
  else        Warning("StBFChain::Setup","File %s has been found as %s",chain.Data(),file);
#endif  
#if 1
  TString cmd(".L ");
  cmd += file;
  gInterpreter->ProcessLine(cmd);
#else
  gROOT->LoadMacro(file);
#endif
  chainOpt  = (St_Bfc *) gInterpreter->Calc("CreateTable()");
  cmd = ".U ";
  cmd += file;
  gInterpreter->ProcessLine(cmd);
  assert(chainOpt);
  NoChainOptions = chainOpt->GetNRows();
  fBFC = chainOpt->GetTable();
  FDate  = FTime  = 0;
  FDateS = FTimeS = 0;
}
//_____________________________________________________________________________
/// Destructor. Call Finish() . See this method for detail on what is cleaned.
StBFChain::~StBFChain(){
  Finish();
}
//_____________________________________________________________________________
/// Routine handling library loading depending on chain options
Int_t StBFChain::Load()
{
  static const Char_t *prefix[3] = {"lib_","lib",""};
  Int_t status = kStOk;
  Int_t i, iok;
  TString lib(gSystem->GetLibraries(0,"D"));
  TObjArray LoadedLibs;
  ParseString(lib,LoadedLibs);
  TIter next(&LoadedLibs);
  TObjString *LoadedLib;
  while ((LoadedLib = (TObjString *) next())){
    TString Base(gSystem->BaseName(LoadedLib->GetName()));
    Base.ReplaceAll(".so","");
    Base.ReplaceAll(".sl","");
    if (Base.BeginsWith("lib_")) Base.ReplaceAll("lib_","");
    if (Base.BeginsWith("lib"))  Base.ReplaceAll("lib","");
    LoadedLib->SetString(Base);
  }
  for (i = 1; i< NoChainOptions; i++) { // Load Libraries if any
    if (fBFC[i].Flag) {
      if (strlen(fBFC[i].Libs) > 0) {
	TObjArray Libs;
	ParseString(fBFC[i].Libs,Libs);
	TIter nextL(&Libs);
	TObjString *libe = 0;
	Char_t *path = 0;
	while ((libe = (TObjString *) nextL())) {
	  TString libN(libe->GetString());
	  if (libN.BeginsWith("lib_")) libN.ReplaceAll("lib_","");
	  if (libN.BeginsWith("lib"))  libN.ReplaceAll("lib","");
	  
	  TString libL("");
	  for (Int_t j = 0; j < 3; j++) {
	    libL = prefix[j]; libL += libN;
	    if ((path = gSystem->DynamicPathName(libL,kTRUE))) break;
	  }
	  iok = -1;
	  if (path) {
	    TIter next(&LoadedLibs);
	    TObjString *LoadedLib;
	    while ((LoadedLib = (TObjString *) next())){
	      TString Base(LoadedLib->GetName());
	      if (Base == libN) {iok = 1; break;}
	    }
	    if (iok > 0) continue;
	    iok = gSystem->Load(libL);
	    if (iok < 0)  {

         LOG_QA  << "problem with loading of " << libL.Data() << endm;
         LOG_QA  <<  fBFC[i].Key << " is switched off \t!!!!" << endm;        

	      fBFC[i].Flag = kFALSE;
	      status = kStErr;
	      assert(iok >= 0);
	      break;
	    } else {
	      if (Debug() > 1) {  TString ts("load "); ts += libL; TMemStat::PrintMem(ts.Data());}
         
         LOG_QA  << "Library " << Form("%-20s\t(%s)\tis loaded",libL.Data(),path) << endm;
          
	      LoadedLibs.Add(new TObjString(libN));
	    }
	  }
	}
	Libs.Delete();
      }
    }
  }
  LoadedLibs.Delete();
  return status;
}

//_____________________________________________________________________________
/// Maker-instantiation handler.
/*!
  This routine contains it all (make instantiation that is), from calibration
  precedence to parameter setting depending on option etc ... Other thing done
  here which deserves attention
  - The maker's SetMode() mechanism is treated here.
  - Calibration options like NoMySQLDb or NoCintDb and path are set
  - SetFlavor() sim+ofl or sim is made
  
  If a maker is added along with some flag options, this is the place to
  implement the switches.
*/
Int_t StBFChain::Instantiate()
{
  Int_t status = kStOk;
  Int_t i, iFail=0;
  for (i = 1; i< NoChainOptions; i++) {// Instantiate Makers if any
    if (! fBFC[i].Flag) continue;
    if (strlen(fBFC[i].Maker) == 0) continue;
    TString maker(fBFC[i].Maker);
    TString Key(fBFC[i].Key);
    Key.ToLower();
    StMaker *myChain = 0;
    StMaker *mk = 0;
    if (strlen(fBFC[i].Chain) > 0) myChain = GetMaker(fBFC[i].Chain);
    if (maker == "St_db_Maker"){// only one instance for whole chain
      St_db_Maker *dbMk = (St_db_Maker *) GetChain()->Maker(fBFC[i].Name); 
      mk = dbMk;
      if (dbMk) continue;
      if (Key.CompareTo("db",TString::kIgnoreCase) == 0) {
	TString MySQLDb("MySQL:StarDb");
	TString MainCintDb("$STAR/StarDb");
	TString MyCintDb("$PWD/StarDb");
	if (GetOption("NoMySQLDb")) MySQLDb = "";
	if (GetOption("NoCintDb")) {MainCintDb = ""; MyCintDb = "";}
	TString Dirs[3];
	Int_t j;
	for (j = 0; j < 3; j++) Dirs[j] = "";
	j = 0;
	if (MySQLDb    != "") {Dirs[j] = MySQLDb;    j++;}
	if (MainCintDb != "") {Dirs[j] = MainCintDb; j++;}
	if (MyCintDb   != "") {Dirs[j] = MyCintDb;   j++;}
	dbMk = new St_db_Maker(fBFC[i].Name,Dirs[0],Dirs[1],Dirs[2]);
	if (!dbMk) goto Error;
	mk = dbMk;
	strcpy (fBFC[i].Name, (Char_t *) dbMk->GetName());
	if (GetOption("Simu"))    dbMk->SetFlavor("sim+ofl");
	else                      dbMk->SetFlavor("ofl");
	goto Add2Chain;
      }
    }
    if (maker == "StIOMaker" && fSetFiles) {
      StIOMaker *inpMk=0;
      if (GetOption("InTree")) {
	Char_t line[80] = "bfcTree";
	Int_t k = kOpt("InTree");
	sscanf(fBFC[k].Comment,"%s",line);
	inpMk = new StIOMaker("inputStream","r",fSetFiles,line);
      }
      else inpMk = new StIOMaker("inputStream","r",fSetFiles);
      if (inpMk) {
	strcpy (fBFC[i].Name,(Char_t *) inpMk->GetName());
	SetInput("StDAQReader",".make/inputStream/.make/inputStream_DAQ/.const/StDAQReader");
	if (GetOption("ReadAll")) inpMk->SetBranch("*",0,"r");	//activate all branches
	goto Add2Chain;
      }
      goto Error;
    }
    if (maker == "StTreeMaker" && fFileOut != "") {
      StTreeMaker    *treeMk  = 0;
      if (GetOption("OutTree")) {
	Char_t line[80] = "bfcTree";
	Int_t k = kOpt("OutTree");
	sscanf(fBFC[k].Comment,"%s",line);
	treeMk = new StTreeMaker("outputStream",fFileOut.Data(),line);
      }
      else treeMk = new StTreeMaker("outputStream",fFileOut.Data());
      if (treeMk) {
	strcpy (fBFC[i].Name,(Char_t *) treeMk->GetName());
	treeMk->SetIOMode("w");
	SetTreeOptions();
	if (myChain) myChain->AddMaker(treeMk);
	goto Add2Chain;
      }
    }
    // Special makers already created or action which
    // need to take place before 'maker' is created.
    if (maker == "StTpcDbMaker" || 
	maker == "St_geant_Maker" ||
	maker == "StVMCMaker") mk = GetChain()->GetMaker(fBFC[i].Name);
    // All Makers created here
    if (!mk) {
      if (strlen(fBFC[i].Name) > 0) mk = New(fBFC[i].Maker,fBFC[i].Name);
      else                          mk = New(fBFC[i].Maker);
      if (mk) {
	strcpy (fBFC[i].Name,(Char_t *) mk->GetName());
	if (maker == "St_geant_Maker") { // takes only first request for geant, if it is active then it should be the first one
	  Int_t NwGeant = 10; // default geant parameters
	  if (!GetOption("fzin") && !GetOption("ntin") &&
	      !GetOption("gstar"))                       NwGeant =  5;
	  if (GetOption("big"))                          NwGeant = 20;
	  if (GetOption("bigbig"))                       NwGeant = 40;
	  ProcessLine(Form("((St_geant_Maker *) %p)->SetNwGEANT(%i);",mk,NwGeant));
	  if (GetOption("Higz")) ProcessLine(Form("((St_geant_Maker *) %p)->SetIwtype(1);",mk));
	  if (GetOption("paw"))  ProcessLine(Form("((St_geant_Maker *) %p)->SetNwPAW(2);",mk));
	  if (GetOption("fzin") || GetOption("ntin") || GetOption("gstar")) mk->SetActive(kTRUE);
	  else                                                              mk->SetActive(kFALSE);
	}
      }
    }
    if (! mk) goto Error;
    
    // special maker options
    // m_Mode xyz 
    //        x = 1 phys_off
    //        y = 1 Passive mode (do not call RunMC()
    //        z = 1 Mixer Mode 
    if (maker == "StVMCMaker") {
      if (GetOption("VMCPassive")) {// don't use mk->SetActive(kFALSE) because we want to have InitRun
	mk->SetMode(mk->GetMode() + TMath::Sign(10,mk->GetMode()));
      }
      else {
	if (GetOption("phys_off")) mk->SetMode(mk->GetMode() + TMath::Sign(100,mk->GetMode()));
	if (fInFile != "")  ProcessLine(Form("((StVMCMaker *) %p)->SetInputFile(\"%s\")",mk,fInFile.Data()));
      }
    }
//		Sti(ITTF) start
    if (maker == "StiMaker") {
      if (GetOption("SvtIT")) 	mk->SetAttr("activeSvt"	,kTRUE);

      if (GetOption("SsdIT")){
				mk->SetAttr("useSsd"	,kTRUE);
				mk->SetAttr("activeSsd"	,kTRUE);
      }
      if (GetOption("FtpcIT")){
				mk->SetAttr("useFtpc"	,kTRUE);
				mk->SetAttr("activeFtpc",kTRUE);
      }
      if (GetOption("PixelIT")){
                               mk->SetAttr("usePixel"  ,kTRUE);
                               mk->SetAttr("activePixel",kTRUE);
      }
      if (GetOption("IstIT")){
                               mk->SetAttr("useIst"  ,kTRUE);
                               mk->SetAttr("activeIst",kTRUE);
      }
      mk->PrintAttr();
    }
//		Sti(ITTF) end
    if (maker=="StGenericVertexMaker") {
      int                   VtxOpt = 0;
      // VertexFinder methods
      if ( GetOption("VFMinuit") ){  VtxOpt |= (0x1 << 0);} // 1
      if ( GetOption("VFppLMV") ){   VtxOpt |= (0x1 << 1);} // 2
      if ( GetOption("VFppLMV5") ){  VtxOpt |= (0x1 << 2);} // 4
      if ( GetOption("VFPPV") ){     VtxOpt |= (0x1 << 3);} // 8
      mk->SetMode(VtxOpt);
      
      // All VertexFinders implement those (or not)
      if (GetOption("beamLine") || GetOption("CtbMatchVtx")) {
	TString  cmd(Form("StGenericVertexMaker* gvtxMk = (StGenericVertexMaker*) %p;",mk));
	if (GetOption("beamLine"))    {cmd += "gvtxMk->UseBeamLine();";}
	if (GetOption("CtbMatchVtx")) {cmd += "gvtxMk->UseCTB();";}
	ProcessLine(cmd);
      }
    }
    if (maker=="StAssociationMaker") {
    
      LOG_QA << "StBFChain::Instantiate Setting the Parameters for the Association Maker" << endm;
      
      TString cmd("");
      if (GetOption("ITTF")) cmd = Form ("((StAssociationMaker *) %p)->useInTracker();",mk);
      cmd += "StMcParameterDB* parameterDB = StMcParameterDB::instance();";
      // TPC
      cmd += "parameterDB->setXCutTpc(.5);"; // 5 mm
      cmd += "parameterDB->setYCutTpc(.5);"; // 5 mm
      cmd += "parameterDB->setZCutTpc(.5);"; // 5 mm
      cmd += "parameterDB->setReqCommonHitsTpc(3);"; // Require 3 hits in common for tracks to be associated
      // FTPC
      cmd += "parameterDB->setRCutFtpc(.3);"; // 3 mm
      cmd += "parameterDB->setPhiCutFtpc(5*(3.1415927/180.0));"; // 5 degrees
      cmd += "parameterDB->setReqCommonHitsFtpc(3);"; // Require 3 hits in common for tracks to be associated
      // SVT
      cmd += "parameterDB->setXCutSvt(.08);"; // 800 um
      cmd += "parameterDB->setYCutSvt(.08);"; // 800 um
      cmd += "parameterDB->setZCutSvt(.08);"; // 800 um
      cmd += "parameterDB->setReqCommonHitsSvt(1);"; // Require 1 hits in common for tracks to be associated
      if (GetOption("IdTruth")) cmd += Form("((StAssociationMaker *) %p)->useIdAssoc();",mk);
      ProcessLine(cmd);
    }
    // usually, we do maker first and option second but the
    // logic is more readable with option first here (as it
    // got a bit out of hand)
    if (GetOption("ppOpt") ) {                         // pp specific stuff
      if (maker == "StTrsMaker")
	mk->SetMode(1);       // Pile-up correction
      
      if (maker == "StVertexMaker"){
	cout << "QAInfo: ppOpt (pp mode) is turned ON" << endl;
	if( GetOption("SvtMatchVtx"))    mk->SetMode(4); // Switch vertex finder to ppLMV using EST
	else if ( GetOption("VFppLMV5")) mk->SetMode(8); // LMV4 with LMV5 cuts 
	else                             mk->SetMode(1); // Switch vertex finder to ppLMV
	if( GetOption("beamLine"))                      // Add beam-line constrain
	  ProcessLine(Form("((StVertexMaker *) %p)->SetBeam4ppLMV();",mk));
	
	if( GetOption("fzin") || GetOption("ntin") ||GetOption("gstar") || GetOption("VMC")){// get CTB's from MC
	  LOG_QA << "QAInfo: Simulation is used, setting CTB Mode to 1" << endm;
	  ProcessLine(Form("((StVertexMaker *) %p)->SetCTBMode(1);",mk));
	} else if ( GetOption("clearDAQCTB") ){        // remove CTB from DAQ (embedding)
	  LOG_QA << "QAInfo: clearDAQCTB used, will removed CTB hits from DAQ"  << endm;
	  ProcessLine(Form("((StVertexMaker *) %p)->SetCTBMode(2);",mk));
	} else{
	  LOG_QA << "QAInfo: Will get CTB from DAQ file" << endm;
	  ProcessLine(Form("((StVertexMaker *) %p)->SetCTBMode(0);",mk));
	}
      }
    } else {
      // See section above associated to GetOption("ppOpt") for pp specific.
      // This part was reshaped to avoid confusion and possible
      // option overwrite if one is not carefull enough ... VtxOffset
      // way used in RY1h and RY2000 only and abandonned later. If re-appear,
      // maker == StVertexmaker would have to be its own block with a ppOpt
      // embedded logic.
      if ((maker == "StVertexMaker"  || maker == "StPreVertexMaker") &&
	  GetOption("VtxOffSet")){
	LOG_QA << "QAInfo: VtxOffSet is ON" << endm;
	if ( GetOption("SvtMatchVtx") )  mk->SetMode(3);
	else                             mk->SetMode(2);
      }
    }
    
    if (maker == "StStrangeMuDstMaker" && GetOption("CMuDST")&& GetOption("StrngMuDST") ) {
      TString cmd(Form("StStrangeMuDstMaker *pSMMk = (StStrangeMuDstMaker*) %p;",mk));
      cmd += "pSMMk->DoV0();";                                 // Set StrangeMuDstMaker parameters
      cmd += "pSMMk->DoXi();";
      cmd += "pSMMk->DoKink();";
      cmd += "pSMMk->SetNoKeep();";                            // Set flag for output OFF
      ProcessLine(cmd);
    }
    
    // Alex requested an option (not turned by default) to disable all
    // hit reco cuts. This will make allm hits saved to MuDST /ezTree.
    if ( maker == "StEmcRawMaker" && GetOption("BEmcDebug"))
      mk->SetMode(1); // only one option now, bit a bitmask
    
    // MuDST and ezTree. Combinations are
    //  ezTree         -> ezTree only
    //  CMuDST         -> regular MuDST only
    //  ezTree,CMuDST  -> both
    if (maker == "StMuDstMaker" && GetOption("ezTree") ){
      TString cmd(Form("StMuDstMaker *pMuMk = (StMuDstMaker *) %p;",mk));
      if ( ! GetOption("CMuDST")) cmd += "pMuMk->SetStatus(\"*\",0);"; 
      cmd += "pMuMk->SetStatus(\"EztAll\",1);";
      ProcessLine(cmd);
    }

    if (maker == "St_dst_Maker") SetInput("dst",".make/dst/.data/dst");
    if (maker == "St_dst_Maker" && GetOption("HitsBranch")) mk->SetMode(2);
    if (maker == "StMatchMaker" && !GetOption("Kalman")) mk->SetMode(-1);
    if (maker == "StLaserEventMaker"){
      // Bill stuff - Empty place-holder
    }
    if (maker == "St_tpt_Maker" && (GetOption("MINIDAQ") || GetOption("Eval"))) {
      TString cmd(Form("St_tpt_Maker *tptMk = (St_tpt_Maker *) %p;",mk));
      if (GetOption("MINIDAQ")) cmd += "tptMk->Set_final(kTRUE);";// Turn on the final ntuple.
      if (GetOption("Eval")) {
	cmd += "tptMk->tteEvalOn();";   //Turn on the tpc evaluation
	cmd += "tptMk->tptResOn();";    // Turn on the residual table
      }
      ProcessLine(cmd);
    }
    if ( (maker == "StTpcHitMover" || maker == "St_tpt_Maker") && GetOption("ExB")){
      // bit 0 is ExB ON or OFF
      // The next 3 bits are reserved for yearly changes.
      // Backward compatibility preserved.
      int mask=1;                                    // Al Saulys request
      if( GetOption("EB1") ){
	// Do nothing (i.e. bit 1 at 0)
      } else if ( GetOption("EB2") ){
	// Force bit 1 at 1 regardless
	mask = mask | 2;
      } else {
	// depend on RY option i.e. take default for that RealYear data
	// expectations.
	if( GetOption("RY2001")  ||
	    GetOption("RY2003")  ||
	    GetOption("RY2003a") ||
	    GetOption("RY2003b") ||
	    GetOption("RY2003X")) mask = mask | 2 ;  // Jim Thomas request
      }
      // Other options introduced in October 2001 for distortion corrections
      // studies and year1 re-production. Those are OR additive to the mask.
      //(void) printf("StBFChain:: Options list : %d %d %d %d %d %d %d %d\n",
      //		  kPadrow13,kTwist,kClock,kMembrane,kEndcap,
      //            kIFCShift,kSpaceCharge,kSpaceChargeR2);
      if( GetOption("OBmap")      ){	mask |=   (kBMap          << 1); }
      if( GetOption("OBMap2d")    ){    mask |=   (kFast2DBMap    << 1); }
      if( GetOption("OPr13")      ){	mask |=   (kPadrow13      << 1); }
      if( GetOption("OTwist")     ){	mask |=   (kTwist         << 1); }
      if( GetOption("OClock")     ){	mask |=   (kClock         << 1); }
      if( GetOption("OCentm")     ){	mask |=   (kMembrane      << 1); }
      if( GetOption("OECap")      ){	mask |=   (kEndcap        << 1); }
      if( GetOption("OIFC")       ){	mask |=   (kIFCShift      << 1); }
      if( GetOption("OSpaceZ")    ){	mask |=   (kSpaceCharge   << 1); }
      if( GetOption("OSpaceZ2")   ){    mask |=   (kSpaceChargeR2 << 1); }
      if( GetOption("OShortR")    ){    mask |=   (kShortedRing   << 1); }
      if( GetOption("OGridLeak")  ){    mask |=   (kGridLeak      << 1); }
      if( GetOption("OGridLeak3D")){    mask |=   (k3DGridLeak    << 1); }
      LOG_QA << "StBFChain::Instantiate ExB The option passed will be " << Form("%d 0x%X\n",mask,mask) << endm;
      mk->SetMode(mask);
    }
    if (maker == "St_tpt_Maker" && GetOption("AlignSectors"))
      ProcessLine(Form("((St_tpt_Maker *)%p)->AlignHits(kTRUE);",mk));
    if (maker == "StTpcHitMover" && GetOption("AlignSectors"))
      ProcessLine(Form("((StTpcHitMover *)%p)->AlignHits(kTRUE);",mk));
    if (maker == "St_tcl_Maker") {
      TString cmd(Form("St_tcl_Maker *tclMk = (St_tcl_Maker *) %p;",mk));
      Int_t kopts = 0;
      if (GetOption("EastOff")) {cmd += "tclMk->EastOff();"; kopts++;}
      if (GetOption("WestOff")) {cmd += "tclMk->WestOff();"; kopts++;}
      if (GetOption("AllOn"))   {cmd += "tclMk->AllOn();"; kopts++;}
      if (GetOption("Eval")) {
	cmd += "tclMk->tclPixTransOn();";// Turn on flat adcxyz table
	cmd += "tclMk->tclEvalOn();";    //Turn on the hit finder evaluation
	kopts++;
      }
      if (kopts) ProcessLine(cmd);
    }
    if (maker == "St_tpcdaq_Maker") {
      Int_t DMode=0;
      TString cmd(Form("St_tpcdaq_Maker *tcpdaqMk = (St_tpcdaq_Maker *) %p;",mk));
      
      // Beware of those ...
      if      (GetOption("Trs"))   mk->SetMode(1); // trs
      else if (GetOption("Simu"))  mk->SetMode(2); // daq, no gain
      else                         mk->SetMode(0); // daq
      
      // DAQ100 or Raw switch options -- Please, adjust StRTSClientFCFMaker block as well
      if ( GetOption("onlcl") )   DMode = DMode | 0x2;  // use the online TPC clusters (DAQ100) info if any
      if ( GetOption("onlraw") )  DMode = DMode | 0x1;  // use the TPC raw hit information
      
      if (DMode != 0) // set flag, leave default = 1
	cmd += Form("tcpdaqMk->SetDAQFlag(%i);",DMode);
      // Correction depending on DAQ100 or not
      // bit 0  =   do GAIN_CORRECTION
      // bit 1  =   do NOISE_ELIM
      // bit 2  =   do ASIC_THRESHOLDS
      // WARNING Option FCF is checked in StDAQMaker
      Int_t Correction = 0;
      Int_t SequenceMerging = 0;
      if ( GetOption("fcf")   ){
	if ( GetOption("Trs")   )  Correction = 0x5; // ASIC + GAIN
	else                       Correction = 0x0; // fcf && ! trs => no corrections
      } else {
	SequenceMerging = 1;
	Correction = 0x7;
      }
      cmd += Form("tcpdaqMk->SetCorrection(%d);",Correction); // default Correction = 0x7
      cmd += Form("tcpdaqMk->SetSequenceMerging(%d);",SequenceMerging);
      LOG_QA << "StBFChain::Instantiate  maker==" << maker.Data() 
			 << Form(" SetDAQFlag(%d) SetMode(%d) SetCorrection(%d) SetSequenceMerging(%d)",
				 DMode,mk->GetMode(),Correction,SequenceMerging) << endm;	 
      ProcessLine(cmd);
    }
    if (maker == "StRTSClientFCFMaker"){
      Int_t DMode=0;
      // use the online TPC clusters (DAQ100) info if any
      if ( GetOption("onlcl") && ! GetOption("onlraw") )  DMode = DMode | 0x2;
      // use the TPC raw hit information
      if ( GetOption("onlraw")&& ! GetOption("onlcl")  )  DMode = DMode | 0x1;
      if (DMode) mk->SetMode(DMode);                 // set flag (matches tcpdaqMk->SetDAQFlag())
    }
    
    if (maker == "StTpcT0Maker"){
      Int_t mask = 0;
      if ( GetOption("tcl") ) mask = mask | 0x0;
      if ( GetOption("fcf") ) mask = mask | 0x1;
      LOG_QA << "StBFChain::Instantiate For " << maker.Data() 
			 << " tcl is ";
      if (GetOption("tcl")) { LOG_QA << "on";  }
      else                  { LOG_QA << "off"; }
      LOG_QA << ", fcf is ";
      if (GetOption("fcf")) { LOG_QA << "on";  }
      else                  { LOG_QA << "off"; }
      LOG_QA << " => mask = " << mask << endm;
      mk->SetMode(mask);
    }
    
    if (maker == "StRchMaker") {
      if (GetOption("Rrs")) mk->SetMode(1); // rrs
      else                  mk->SetMode(0); // daq
    }
    
    // Place-holder. Would possibly be a bitmask
    if (maker == "StTofrMatchMaker"){
      mk->SetMode(0);
    }
    
    // Turn on alternative V0 method
    if (maker == "StV0Maker") {
      if (GetOption("Ev03")) mk->SetMode(1);
      if (GetOption("Eval")) ProcessLine(Form("((StV0Maker *) %p)->ev0EvalOn();",mk));
    }
    //if (maker == "StKinkMaker"){
    // // Placeholder for KinkMaker control.
    // // - SetMode(1)   TPT,
    // // - SetMode(2)   ITTF
    // // Default = Both
    //}
    if (maker == "StSpaceChargeEbyEMaker") {
      if ( GetOption("SpcChgCal") )   mk->SetMode(2);
      if ( GetOption("SCScalerCal") ) mk->SetMode(4);
    }
    if (maker == "StEventQAMaker" && GetOption("alltrigger") )
      ProcessLine(Form("((StEventQAMaker *) %p)->AllTriggers();",mk));
    //Special options for V0s and Xis using estGlobal tracks
    if(maker=="StV0FinderMaker" && Key=="v0svt"){
      TString cmd(Form("StV0FinderMaker *V0mk=(StV0FinderMaker*) %p;",mk));
      cmd += "V0mk->SetLanguageUsage(0);";
      cmd += "V0mk->SetSVTUsage(1);";
      cmd += "V0mk->SetV0LanguageUsage(3);";
      ProcessLine(cmd);
    }
    if(maker=="StXiFinderMaker" && Key=="xisvt"){
      TString cmd(Form("StXiFinderMaker *Ximk=(StXiFinderMaker*) %p;",mk));
      cmd += "Ximk->SetLanguageUsage(0);";
      cmd += "Ximk->SetSVTUsage(1);";
      cmd += "Ximk->SetV0LanguageUsage(3);";
      cmd += "Ximk->SetXiLanguageUsage(5);";
      ProcessLine(cmd);
    }
    
    if (maker == "St_trg_Maker") {
      Int_t mode = 0;
      if (! GetOption("alltrigger")){
	if (GetOption("Physics"))   mode += 1;
	if (GetOption("LaserTest")) mode += 2;
	if (GetOption("PulserSvt")) mode += 4;
      } else {
        LOG_QA << "'alltrigger' option on. All others ignored" << endm;
      }
      if (mode) mk->SetMode(mode);
    }
    if ((maker == "StdEdxMaker" || maker == "StdEdxY2Maker" ) &&
	GetOption("Simu"))  mk->SetMode(-10);
    if (maker == "StTpcDbMaker"){
      mk->SetMode(0);
      // this change may be temporary i.e. if Simulation includes
      // rotation/translation, this won't be necessarily true.
      // Will investigate further.
      if (GetOption("Simu")) mk->SetMode(1);
      // This is commented for now but may be used. Those extensions
      // were implemented by David H. on Jan 2 2002. DEfault is ofl+laserDV
      //mk->UseOnlyLaserDriftVelocity();    // uses laserDV database
      //mk->UseOnlyCathodeDriftVelocity();  // uses offl database
      if ( GetOption("useLDV") || GetOption("useCDV") ) {
	TString cmd(Form("StTpcDbMaker *Tmk=(StTpcDbMaker*) %p;",mk));
	if ( GetOption("useLDV") ) cmd += "Tmk->UseOnlyLaserDriftVelocity();";  // uses laserDV database
	if ( GetOption("useCDV") ) cmd += "Tmk->UseOnlyCathodeDriftVelocity();";// uses ofl database
	ProcessLine(cmd);
      }
    }
    if (maker == "StSvtDbMaker"){
      mk->SetMode(0);
      // If simulation running make sure pick up simu stuff from db
      if (GetOption("Simu")) mk->SetMode(1);
    }
    
    
    // Hit filtering will be made from a single maker in
    // future with flexible filtering method
    if (maker == "StHitFilterMaker") {
      if (GetOption("SvtHitFilt")){
	// Filter out SVT bad hits, TPC hits not on tracks and all hits if fabs(ZVert)>30
	LOG_QA << "SVT hit filter is ON" << endm;
	TString cmd(Form("StHitFilterMaker *Filtmk=(StHitFilterMaker*) %p;",mk));
	cmd += "Filtmk->setPtLowerCut(-99.);";
	cmd += "Filtmk->setPtUpperCut(-99.);";
	cmd += "Filtmk->setAbsEtaCut(-99);";
	cmd += "Filtmk->setAbsZVertCut(30);";
	ProcessLine(cmd);
      } else {
	LOG_QA << "Default hit filtering is ON" << endm;
      }
    }
    if (GetOption("dst") && GetOption("NoHits") && maker == "StEventMaker") {
      ProcessLine(Form("((StEventMaker *) %p)->doLoadTpcHits  = kFALSE;", mk));
      ProcessLine(Form("((StEventMaker *) %p)->doLoadFtpcHits = kFALSE;", mk));
      ProcessLine(Form("((StEventMaker *) %p)->doLoadSvtHits  = kFALSE;", mk));
      ProcessLine(Form("((StEventMaker *) %p)->doLoadSsdHits  = kFALSE;", mk));
    }
    if (maker == "StMiniMcMaker" && fFileOut != "") {
      ProcessLine(Form("((StMiniMcMaker *) %p)->setFileName(\"%s\");", mk, fFileOut.Data()));
    }
  Add2Chain:
    if (myChain) myChain->AddMaker(mk);
    continue;
  Error:
    status = kStErr;
    if (i != iFail) {
      LOG_QA
	<< " ======================================\n"
	<< " problem with Instantiation of "         << fBFC[i].Maker << "\n"
	<< " ======================================" << endm;
      iFail = i;
    }
  }
  //  PrintQAInfo();
  PrintInfo();
  // START the chain (may the force be with you)
  // Create HTML docs of all Maker's inv
  if (GetOption("MakeDoc"))  MakeDoc();
  if (GetOption("Debug"))    SetDEBUG(1);
  if (GetOption("Debug1"))   SetDEBUG(1);
  if (GetOption("Debug2"))   SetDEBUG(2);
  if (GetOption("nohistos")) SetAttr(".histos",0,"*");
  else                       SetAttr(".histos",1,"*");
  if (GetOption("NoRepeat")) gMessMgr->IgnoreRepeats();
  return status;
}
//_____________________________________________________________________
Int_t StBFChain::Init() {
  SetDbOptions();
  SetGeantOptions();
  Int_t iok = StChain::Init();
  if (GetOption("gstar") && fInFile == "") {
    StMaker *geantMk = GetMaker("geant");
    if (geantMk && !geantMk->InheritsFrom("St_geant_Maker")) geantMk = 0;
    if (geantMk) {
      ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"subevent 0;\");",geantMk));
      // gkine #particles partid ptrange yrange phirange vertexrange
      ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"gkine 80 6 1. 1. -4. 4. 0 6.28  0. 0.;\");",geantMk));
      ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"mode g2tm prin 1;\");",geantMk));
      //  ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"next;\");",geantMk));
      //  ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"dcut cave z 1 10 10 0.03 0.03;\");",geantMk));
      if (GetOption("Debug") || GetOption("Debug2")) {
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"debug on;\");",geantMk));
	if (GetOption("Debug2")) ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"swit 2 3;\");",geantMk));
      } 
      if (GetOption("phys_off")) {
	LOG_INFO << "St_geant_Maker::Init switch off physics" << endm;
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"DCAY 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"ANNI 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"BREM 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"COMP 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"HADR 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"MUNU 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"PAIR 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"PFIS 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"PHOT 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"RAYL 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"LOSS 4\");",geantMk)); // no fluctuations 
	//  ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"LOSS 1\");",geantMk)); // with delta electron above dcute
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"DRAY 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"MULS 0\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"STRA 0\");",geantMk));
	//                                              CUTS   CUTGAM CUTELE CUTHAD CUTNEU CUTMUO BCUTE BCUTM DCUTE DCUTM PPCUTM TOFMAX GCUTS[5]
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"CUTS     1e-3   1e-3   .001   .001   .001  .001  .001  1e-3  .001   .001 50.e-6\");",
			 geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"gclose all\");",geantMk));
	ProcessLine(Form("((St_geant_Maker *) %p)->Do(\"physi\");",geantMk));
      }
    }
  }
  return iok;
}
//_____________________________________________________________________
/// Skip events by selecting according maker 
Int_t StBFChain::Skip(int nskip)
{
  StMaker *geant = 0;
  StIOMaker      *inpMk = 0;
  if (nskip<=0) return 0;
  if (GetOption("fzin") || GetOption("ntin") )  {
    geant = GetMaker("geant");
    if (geant && !geant->InheritsFrom("St_geant_Maker")) geant = 0;
    if (geant && !geant->IsActive()) 	               	 geant = 0;
    if (geant) {
      gMessMgr->QAInfo() << "StBFChain::Skip -> St_geant_Make::Skip(" << nskip << ") Events" << endm;
      ProcessLine(Form("((St_geant_Maker *) %p)->Skip(%i);",geant,nskip));
      return 0;
    }
  }
  if (GetOption("VMC"))  {
    geant = GetMaker("geant");
    if (geant && !geant->InheritsFrom("StVMCMaker")) geant = 0;
    if (geant && !geant->IsActive()) 	               	 geant = 0;
    if (geant) {
      gMessMgr->QAInfo() << "StBFChain::Skip -> StVMCMake::Skip(" << nskip << ") Events" << endm;
      ProcessLine(Form("((StVMCMaker *) %p)->Skip(%i);",geant,nskip));
      return 0;
    }
  }
  inpMk = (StIOMaker *) GetMaker("inputStream");
  if (inpMk && !inpMk->InheritsFrom("StIOMaker")) 	inpMk = 0;
  if (inpMk) {
      gMessMgr->QAInfo() << "StBFChain::Skip -> StIOMaker::Skip(" << nskip << ") Events" << endm;
    inpMk->Skip(nskip); return 0;}
  
  Error("Skip","No maker to Skip"); 
  return kStErr;  
}  

//_____________________________________________________________________
/// Really the destructor (close files, delete pointers etc ...)
Int_t StBFChain::Finish()
{
  if (fBFC) {
    SafeDelete(chainOpt);
    fBFC = 0;
    if (fTFile) {fTFile->Write(); fTFile->Flush(); fTFile->Close(); SafeDelete (fTFile);}
    return StMaker::Finish();
  }
  else return kStOK;
}


//_____________________________________________________________________
Int_t StBFChain::AddAB (const Char_t *mkname,const StMaker *maker,const Int_t Opt) {
  if (! maker || strlen(mkname) == 0) return kStErr;
  StMaker *parent = maker->GetParentMaker();
  if (parent) {
    TList   *list    = parent->GetMakeList();
    list->Remove((StMaker *)maker);
  }
  StMaker *mk      = GetMaker(mkname);      if (!mk)     return kStErr;
  parent  = mk->GetParentMaker();  if (!parent) return kStErr;
  TList   *list    = parent->GetMakeList(); if (!list)   return kStErr;
  if (Opt > 0) list->AddAfter (mk,(StMaker*)maker);
  else         list->AddBefore(mk,(StMaker*)maker);
  return kStOk;
}

//_____________________________________________________________________
Int_t StBFChain::ParseString (const TString &tChain, TObjArray &Opt, Bool_t Sort) {
  Opt.Clear();
  TObjArray *obj = tChain.Tokenize("[^ ;,]+");
  Int_t nParsed = obj->GetSize();
  Int_t k, N = 0;
  for (k = 0; k < nParsed && obj->At(k); k++) N++;
  nParsed = N;
  // sort options
  StBFChain *chain = (StBFChain *) StMaker::GetChain();
  if (chain && Sort) {// sort options
    TArrayI idT(nParsed); Int_t *idx = idT.GetArray();
    TArrayI kdT(nParsed); Int_t *kdx = kdT.GetArray();
    for (k = 0; k < nParsed; k++) {
      TString string = ((TObjString *) obj->At(k))->GetString();
      kdx[k] = TMath::Abs(chain->kOpt(string,kFALSE));
    }
    TMath::Sort(nParsed,kdx,idx,0);
    TString sChain;
    for (k = 0; k < nParsed; k++) {
      Opt.AddAtAndExpand(obj->At(idx[k]),k);
      if (k == 0) sChain = ((TObjString *)Opt[k])->GetString();
      else {sChain += ","; sChain += ((TObjString *)Opt[k])->GetString();}
    }
    if (N > 1 && chain->Debug() > 2) {
      gMessMgr->QAInfo() << "Requested chain is :\t" << tChain.Data() << endm;
      gMessMgr->QAInfo() << "Sorted    chain is :\t" << sChain.Data() << endm;
    }
  } else for (k = 0; k < nParsed; k++) Opt.AddAtAndExpand(obj->At(k),k);
  obj->SetOwner(kFALSE);
  delete obj;
  return nParsed;
}

//_____________________________________________________________________
/// Check option if defined (Char_t argument interface)
Int_t StBFChain::kOpt (const Char_t *tag, Bool_t Check) const {
  TString Tag(tag);
  Int_t kO = kOpt(&Tag, Check);
  return kO;
}
//_____________________________________________________________________
/// Check option if defined.
/*!
  This method checks if the options are valid by %comparing them
  to the list of declared options. This is called for each option
  passed as argument. The real sorting of all options is done in
  SetFlags().
*/
Int_t StBFChain::kOpt (const TString *tag, Bool_t Check) const {
  TString Tag = *tag;
  Tag.ToLower();
  TString opt, nopt;
  for (Int_t i = 1; i< NoChainOptions; i++) {
    opt = TString(fBFC[i].Key); //check nick name
    opt.ToLower();
    nopt = TString("-");
    nopt += opt;
    if       (Tag ==  opt) {return  i;}
    else {if (Tag == nopt) {return -i;}}
    opt = TString(fBFC[i].Maker); //check full maker name2
    nopt = TString("-");
    nopt += opt;
    if       (Tag ==  opt) {return  i;}
    else {if (Tag == nopt) {return -i;}}
  }
  if ( (strncmp( Tag.Data() ,"dbv",3) || 
	strncmp( Tag.Data() ,"sdt",3)   ) &&
       strlen(Tag.Data()) == 11 ) return 0;
  if (Check) {
    gMessMgr->Error() << "Option " << Tag.Data() << " has not been recognized" << endm;
    abort(); //assert(1);
  }
  return 0;
}

//_____________________________________________________________________
/// Enable/disable valid command line options
void StBFChain::SetOption(const Int_t k, const Char_t *chain) {
  // set all off
  if (k > 0 && !fBFC[k].Flag) {
    //    printf ("SetOption: %s %i",fBFC[k].Key,k);
    if (strlen(fBFC[k].Opts) > 0) {
      TObjArray Opts;
      ParseString(fBFC[k].Opts,Opts, kTRUE);
      TIter next(&Opts);
      TObjString *Opt;
      while ((Opt = (TObjString *) next())) SetOption(Opt->GetName(),fBFC[k].Key);
      Opts.Delete();
    }
    fBFC[k].Flag = kTRUE;
    gMessMgr->QAInfo() << Form(" Switch On  %20s by %s", fBFC[k].Key, chain) << endm;
  } else {
    if (k < 0 && fBFC[-k].Flag) {
      //      printf ("SetOption: %s %i",fBFC[-k].Key,k);
      fBFC[-k].Flag = kFALSE;
      gMessMgr->QAInfo() << Form(" Switch Off %20s by %s", fBFC[-k].Key, chain) << endm;
    }
  }
}

//_____________________________________________________________________
/// Returns chain-option state (on/off)
Bool_t StBFChain::GetOption(const Int_t k) const
{
  return (k>0 && k <NoChainOptions) ? fBFC[k].Flag : kFALSE;
}

/// Returns the comment string associated to an option
/*!
 * Any option passed a bla=XX is reshaped as follow ...
 * - The SetFlags() function strip out the =XX part and replaces
 *   the comment by the value XX
 * - This GetOptionString() returns the comment part so makers
 *   can globally access the option string.
 *
 * <i>Note</i> : If the requested option is not part of the global BFC[]
 * array, the kOpt() method is going to scream at you but it will still
 * work. You can ask for that option to be added to the chain official
 * options later whenever your code debugging is done. In other words,
 * this method allows you to pass ANY options not officially declared
 * and use it as test/work-around to pass any parameters to your maker.
 *
 * However, if the parameters are to be used in production, we DO
 * request/require that they are declared as a valid option.
 *
 *
 */
Char_t *StBFChain::GetOptionString(const Char_t *Opt)
{
  int o = kOpt(Opt);
  if(!o) return NULL;
  else if(!GetOption(o)) return NULL;
  else return(fBFC[o].Comment);
}


//_____________________________________________________________________________
/// Scan all flags, check if they are correct, manipulate the comment if necessary
/*!
  This method checks if the options passed are correct and/or sorts out
  the extraneous information passed through the comment if applies. Two
  special flags exists, that is, dbv and sdt for database interraction ...
  - The <tt>dbv</tt> tag is used to setup the end-time for entries (time after
  which any database insertions will be ignored). It used to provide a
  mechanism by which we can run production with a stable set of calibration
  constant and still allow for development value insertion to get in.
  - The <tt>sdt</tt> tag is used to setup the database look-up time stamp
  that is, on which date to look at the database. This is usefull if we
  want to decouple Geant geometry and database calibration constants.
  This may also be used to get database values when you do not run over
  a raw data file.
*/
void StBFChain::SetFlags(const Char_t *Chain)
{
  TString tChain(Chain);
  Int_t mode = 1;
  // chain choise
  if (tChain.Contains("ittf",TString::kIgnoreCase)) mode = 2;
  Setup(mode);
  Int_t k=0;
  if (tChain == "") {
    gMessMgr->QAInfo() << "\tPossible Chain Options are:" << endm;
    for (k=0;k<NoChainOptions;k++)
    gMessMgr->QAInfo() 
      << Form(" %3d:[-]%-13s:%-12s:%-12s:%-12s :%s :%s :%s"
	      ,k,fBFC[k].Key,fBFC[k].Name,fBFC[k].Chain,fBFC[k].Opts,fBFC[k].Maker,fBFC[k].Libs,fBFC[k].Comment) 
      << endm;

    return;
  }
  TString STAR_VERSION("$STAR_VERSION");
  gSystem->ExpandPathName(STAR_VERSION);
  gMessMgr->QAInfo() << "=============================================="  << endm;
  gMessMgr->QAInfo() << "============= You are in " << STAR_VERSION.Data() << " ===============" << endm;
  gMessMgr->QAInfo() << "Requested chain " << GetName() << " is :\t" << tChain.Data() << endm;
  TObjArray Opts;
  Int_t nParsed = ParseString(tChain,Opts,kTRUE);
  for (Int_t l = 0; l < nParsed; l++) {
    TString Tag = ((TObjString *)Opts[l])->GetString(); 
    Int_t kgo;
    Int_t in = Tag.Index("=");
    if (in > 0) {// string with  "="
      TString subTag(Tag.Data(),in);
      subTag.ToLower(); //printf ("Chain %s\n",tChain.Data());
      kgo = kOpt(subTag.Data());
      if (kgo > 0) {
	memset(fBFC[kgo].Comment,0,200); // be careful size of Comment
	SetOption(kgo,fBFC[k].Key);
	TString Comment(Tag.Data()+in+1,Tag.Capacity()-in-1);
	strcpy (fBFC[kgo].Comment, Comment.Data());
	gMessMgr->QAInfo() << Form(" Set        %s = %s", fBFC[kgo].Key,fBFC[kgo].Comment) << endm;
      }
    } else {
      Tag.ToLower();
      // printf ("Chain %s\n",tChain.Data());
      kgo = kOpt(Tag.Data(),kFALSE);
      if (kgo != 0){
	SetOption(kgo);
      } else {
	// it is 0 i.e. was not recognized. Check if it is a dbvXXXXXXXX
	// with a 8 digit long time stamp. We can do all of that in the
	// SetDbOptions() only (removing the fBFC[i].Flag check) but the
	// goal here is to avoid user's histeria by displaying extra
	// messages NOW !!! Debug: dbv20040917
	if( ! strncmp( Tag.Data() ,"dbv",3) && strlen(Tag.Data()) == 11){
	  (void) sscanf(Tag.Data(),"dbv%d",&FDate);
	  cout << " ... but still will be considered as a dynamic timestamp (MaxEntryTime) "
	       << FDate  << endl;
	} else if( ! strncmp( Tag.Data() ,"sdt",3) && strlen(Tag.Data()) == 11){
	  (void) sscanf(Tag.Data(),"sdt%d",&FDateS);
	  cout << " ... but still will be considered as a dynamic timestamp (DateTime)     "
	       << FDateS << endl;
	} else { // Check for predefined db time stamps ?
	  const DbAlias_t *DbAlias = GetDbAliases();
	  Int_t found = 0;
	  for (Int_t i = 0; DbAlias[i].tag; i++) {
	    Bfc_st row = {"","","","db,detDb","","","",kTRUE};
	    if (! Tag.CompareTo(DbAlias[i].tag,TString::kIgnoreCase)) {
	      found = i;
	      memcpy (&row.Key, Tag.Data(), strlen(Tag.Data()));
	      chainOpt->AddAt(&row);
	      NoChainOptions = chainOpt->GetNRows();
	      fBFC = chainOpt->GetTable();
	      break;
	    }
	    TString dbTag("r");
	    dbTag += DbAlias[i].tag;
	    if (! Tag.CompareTo(dbTag,TString::kIgnoreCase)) {
	      found = i;
	      memcpy (&row.Key, Tag.Data(), strlen(Tag.Data()));
	      chainOpt->AddAt(&row);
	      NoChainOptions = chainOpt->GetNRows();
	      fBFC = chainOpt->GetTable();
	      break;
	    }
	  } 
	  kgo = kOpt(Tag.Data(),kFALSE);
	  if (kgo != 0){
	    SetOption(kgo);
	  } else {
	    // Check that option can be library name or / and Maker
	    static Char_t *path = ".:.$STAR_HOST_SYS/lib::.$STAR_HOST_SYS/LIB:$STAR/.$STAR_HOST_SYS/lib:$STAR/.$STAR_HOST_SYS/LIB";
	    TString File = Tag; File += ".so";
	    Char_t *file = gSystem->Which(path,File.Data(),kReadPermission);
	    if (file) {
	      TString Maker("");
	      Bfc_st row = {"","","","","","","",kTRUE};
	      memcpy (&row.Key, Tag.Data(), strlen(Tag.Data()));
	      if (Tag.Contains("Maker")) memcpy (&row.Maker, Tag.Data(), strlen(Tag.Data()));
	      memcpy (&row.Libs, Tag.Data(), strlen(Tag.Data()));
	      chainOpt->AddAt(&row);
	      NoChainOptions = chainOpt->GetNRows();
	      fBFC = chainOpt->GetTable();
	    }
	    kgo = kOpt(Tag.Data(),kFALSE);
	    if (kgo != 0) {
	      SetOption(kgo);
	    } else {
	      cout << " Invalid Option " << Tag.Data() << ". !! ABORT !! " << endl;
	      abort(); //assert(1);
	      return;
	    }
	  }
	}
      }
    }
  }
  Opts.Delete();
  if (!GetOption("NoDefault")) {
    // Check flags consistency
    if (gClassTable->GetID("TGiant3") >= 0) { // root4star
      SetOption("-VMC","Default,TGiant3");
      SetOption("-VMCPassive","Default,TGiant3");
      SetOption("-VMCAppl","Default,TGiant3");
      SetOption("-RootVMC","Default,TGiant3");
      if (!( GetOption("fzin") || GetOption("ntin") || GetOption("gstar"))) {// Not Active geant
	SetOption("geant","Default,-fzin,-ntin,-gstar,TGiant3");
	SetOption("MagF","Default,-fzin,-ntin,-gstar,TGiant3");
      }
    } else {                                  // root
      if (GetOption("fzin")) {
	gMessMgr->Error() << "Option fzin cannot be used in root.exe. Use root4star" << endm;
	abort();
      }
      if (GetOption("ntin")) {
	gMessMgr->Error() << "Option ntin cannot be used in root.exe. Use root4star" << endm;
	abort();
      }
      if (GetOption("gstar")) {
	SetOption("VMC","Default,-TGiant3,gstar");
	SetOption("-gstar","Default,-TGiant3");
      }
      SetOption("-geant","Default,-TGiant3"); 
      SetOption("-geantL","Default,-TGiant3"); 
      SetOption("-geometry","Default,-TGiant3");
      SetOption("-geomNoField","Default,-TGiant3");
      if (! (GetOption("VMC") || GetOption("VMCPassive"))) {
	SetOption("VMCPassive","Default,-TGiant3");
      }
    }
  }
  if (!GetOption("Eval") && GetOption("AllEvent"))  SetOption("Eval","-Eval,AllEvent");
  // Print set values
  St_Bfc *Bfc = new St_Bfc("BFChain",NoChainOptions);
  AddRunco(Bfc);
  for (k = 1; k<NoChainOptions;k++) {
    if (GetOption(k)) {
      gMessMgr->QAInfo() << Form("================== %4d %15s\tis ON \t: %s",
				 k, (char *) fBFC[k].Key, (char *) fBFC[k].Comment) << endm;
      Bfc->AddAt(&fBFC[k]);
    }
  }
  //  gSystem->Exit(1);
}
//_____________________________________________________________________
void StBFChain::Set_IO_Files (const Char_t *infile, const Char_t *outfile){
  TString gc("");
  if (infile) {
    if (strlen(infile) > 2) {
      gc = TString(infile,3);
      gc.ToLower();
    }
    if (gc == "gc:") {SetGC(infile+3); goto SetOut;}
  }
  SetInputFile(infile);
 SetOut:
  SetOutputFile(outfile);
}
//_____________________________________________________________________
void StBFChain::SetGC (const Char_t *queue){
  TString Queue(queue);
  gMessMgr->QAInfo() << "Requested GC queue is :\t" << Queue.Data() << endm;
  TObjArray Opts;
  ParseString(Queue,Opts);
  TIter next(&Opts);
  TObjString *Opt;
  static TString ARGV[40];
  Int_t Argc = -1;
  while ((Opt = (TObjString *) next())) {
    TString string = Opt->GetString();
    const Char_t *argv = string.Data();
    if (argv[0] == '-') {
      switch (argv[1]) {
      case 'o':
      case 'i':
      case 'c':
      case 'q':
      case 's':
      case 'n':
      case 'm':
      case 't':
      case '-': // now do --options, they get added to Config
        ARGV[++Argc] = string.Data();
        Argc++;
	break;
      default :
	gMessMgr->QAInfo() << "Unrecognized option :\t" << string << endm;
	break;
      }
    }
    else if (Argc > 0) {ARGV[Argc] += " "; ARGV[Argc] += string;}
  }
  Opts.Delete();
  fSetFiles = (StFileI *)StChallenger::Challenge();
  fSetFiles->SetDebug();
  Argc++;
  Char_t **Argv = new Char_t* [Argc];
  for (int i=0;i<Argc;i++)  {Argv[i] = (Char_t *) ARGV[i].Data();}
  fSetFiles->Init(Argc,(const Char_t **) Argv);
}
//_____________________________________________________________________
void StBFChain::SetInputFile (const Char_t *infile){
  // define input file
  if (infile) fInFile = infile;
  if (fInFile != "") {
    if (!GetOption("fzin") && !GetOption("ntin")) {
      fSetFiles= new StFile();
      TObjArray Files;
      ParseString(fInFile,Files);
      TIter next(&Files);
      TObjString *File;
      while ((File = (TObjString *) next())) {
	TString string = File->GetString();
	if (!strstr(string.Data(),"*") &&
	    gSystem->AccessPathName(string.Data())) {// file does not exist
	  gMessMgr->Error() << "StBFChain::SetInputFile  *** NO FILE: " << string.Data() << ", exit!" << endm;
	  gSystem->Exit(1);
	}
	else fSetFiles->AddFile(File->String().Data());
      }
      Files.Delete();
    }
  }
  if (fInFile != "") gMessMgr->QAInfo() << "Input file name = " << fInFile.Data() << endm;
}


//_____________________________________________________________________
/// Takes care of output file name (extension)
void StBFChain::SetOutputFile (const Char_t *outfile){
  if (outfile)               fFileOut = outfile;
  else {
    if (GetOption("gstar"))  fFileOut = "gtrack";
    if (GetOption("VMC"))    fFileOut = "VMC";
    if (fInFile != "") {
      if (GetOption("fzin") || GetOption("ntin")) {
	TObjArray words;
	ParseString(fInFile,words);
	TIter nextL(&words);
	TObjString *word = 0;
	while ((word = (TObjString *) nextL())) {
	  if (word->GetString().Contains(".fz") ||
	      word->GetString().Contains(".nt")) {
	    fFileOut = gSystem->BaseName(word->GetName());
	    break;
	  }
	}
      }
      else fFileOut = gSystem->BaseName(fInFile.Data());
      if (  fFileOut != "") {
	fFileOut.ReplaceAll("*","");
	fFileOut.ReplaceAll("..",".");
	fFileOut.ReplaceAll(".daq","");
	fFileOut.ReplaceAll(".fzd","");
	fFileOut.ReplaceAll(".fz","");
	fFileOut.ReplaceAll(".nt","");
	fFileOut.ReplaceAll(".root","");
	fFileOut.Strip();
	fFileOut.Append(".root");
      }
    }
  }
  if (fFileOut != "")  gMessMgr->QAInfo() << "Output root file name " <<  fFileOut.Data() << endm;
  if (!fTFile) {
    if (GetOption("tags")  && fFileOut != "" ||
	GetOption("lana") ||  GetOption("Laser")) {
      TString TagsName = fFileOut;
      if(GetOption("LaserCal")){
	TagsName.ReplaceAll(".root",".laser.root");
      } else {
	TagsName.ReplaceAll(".root",".tags.root");
      }
      fTFile = new TFile(TagsName.Data(),"RECREATE");
    }
  }
  //    gSystem->Exit(1);
}



//_____________________________________________________________________
/// Handles all geant options
/*!
  This method sets the Geant options that is the Geometry loading
  part. Depends on St_geant_Maker instantiated in the Instantiate()
  method.
  Please, change SetDbOptions()
*/
void StBFChain::SetGeantOptions(){
  StMaker *geantMk = GetMaker("geant");
  if (geantMk && !geantMk->InheritsFrom("St_geant_Maker")) geantMk = 0;
  if (geantMk) {
    SetInput("geant",".make/geant/.data");
    TString GeomVersion("");
    if (!GetOption("fzin")) {
      GeomVersion = "y2004x";
      const DbAlias_t *DbAlias = GetDbAliases();
      Int_t found = 0;
      for (Int_t i = 0; DbAlias[i].tag; i++) {
	TString r("r");
	r +=  DbAlias[i].tag;
	if ( !GetOption(DbAlias[i].tag,kFALSE) && !GetOption(r,kFALSE)) continue;
	GeomVersion = DbAlias[i].geometry;
	found = i;
	break;
      }
      if (! found) gMessMgr->QAInfo() << "StBFChain::SetGeantOptions() Chain has not found geometry tag. Use " << GeomVersion << endm;
      TString GeometryOpt("detp geometry ");
      GeometryOpt += GeomVersion;
      ProcessLine(Form("((St_geant_Maker *) %p)->LoadGeometry(\"%s\");",geantMk,GeometryOpt.Data()));
    }
    if ((GetOption("fzin") || GetOption("ntin")) && fInFile != "") 
      ProcessLine(Form("((St_geant_Maker *) %p)->SetInputFile(\"%s\")",geantMk,fInFile.Data()));
  } // geantMk
  if (GetOption("Simu")) {
    StEvtHddr *fEvtHddr = (StEvtHddr*)GetDataSet("EvtHddr");
    if (!fEvtHddr) {
      fEvtHddr = new StEvtHddr(m_ConstSet);
      SetOutput(fEvtHddr);	              //Declare this "EvtHddr" for output
    }
    if (fEvtHddr->GetRunNumber() < 0 || fEvtHddr->GetRunNumber() >= 1000000) {
      fEvtHddr->SetRunNumber(1); // to have run positive and < 1000000 (to avoid mess with RunLog)
    }
  }
}


//_____________________________________________________________________
/// Treats the DbV options used for database timestamp.
/*!
  Re-scan all options and search for dbv options. This method also sorts
  out the string-based database timestamp for reconstruction. Those have
  to be in phase with the geant geometry (see SetGeantOptions()) if
  simulation is being reconstructed.
  
  The order matters since a later option would overwrite an earlier one.
  The mechanism introduced for a dynamic (i.e. not pre-defined) timestamp is that
  it will be used ONLY if there are no other timestamp options.
  <b>Be aware of this precedence ...</b>
  
*/
void StBFChain::SetDbOptions(){
  Int_t i;
  Int_t Idate=0,Itime=0;
  for (i = 1; i < NoChainOptions; i++) {
    if (fBFC[i].Flag && !strncmp(fBFC[i].Key ,"DbV",3)){
      gMessMgr->QAInfo() << "StBFChain::SetDbOptions  Found time-stamp " << fBFC[i].Key << " [" << fBFC[i].Comment << "]" << endm;
      (void) sscanf(fBFC[i].Comment,"%d/%d",&Idate,&Itime);
    }
  }
  
  if( ! Idate && FDate){
      gMessMgr->QAInfo() << "StBFChain::SetDbOptions  witching to user chosen dynamic time-stamp (MaxEntry) " 
			 << FDate << " " << FTime << endm;
      gMessMgr->QAInfo() << "Chain may crash if time-stamp is not validated by db interface" << endm;

    Idate = FDate;
    Itime = FTime;
  }
  
  StMakerIter nextMaker(this);
  StMaker *maker;
  while ((maker = nextMaker.NextMaker())) {
    if (!strcmp(maker->ClassName(),"St_db_Maker")) {
      St_db_Maker *db = (St_db_Maker *) maker;
      
      // Startup date over-write
      if (FDateS){
	gMessMgr->QAInfo() << "StBFChain::SetDbOptions Switching to user chosen dynamic time-stamp (Start)"
			   << FDateS << " " << FTimeS << endm;
	gMessMgr->QAInfo() << "Chain may crash if time-stamp is not validated by db interface" << endm;
	
	db->SetDateTime(FDateS,FTimeS);
      } else {
	const DbAlias_t *DbAlias = GetDbAliases();
	Int_t found = 0;
	for (Int_t i = 0; DbAlias[i].tag; i++) {
	  if (GetOption(DbAlias[i].tag,kFALSE)) {
	    db->SetDateTime(DbAlias[i].tag);
	    found = i;
	    break;
	  }
	}
	if (! found) gMessMgr->QAInfo() << "StBFChain::SetDbOptions() Chain has not set a time-stamp" << endm;
      }
      // Show date settings
      gMessMgr->QAInfo() << db->GetName()
			 << " Maker set time = "
			 << db->GetDateTime().GetDate() << "."
			 << db->GetDateTime().GetTime() << endm;
      if (GetOption("VMC") && m_EvtHddr) {
	gMessMgr->QAInfo() << GetName() << " Chain set time from  " << db->GetName() << endm;
	m_EvtHddr->SetDateTime(db->GetDateTime());
      }
      // MaxEntry over-write
      if (Idate) {
	db->SetMaxEntryTime(Idate,Itime);
	gMessMgr->Info() << "\tSet DataBase max entry time " << Idate << "/" << Itime
			 << " for St_db_Maker(\"" << db->GetName() <<"\")" << endm;
      }
    } // check if maker is St_db_Maker
  }   // loop over makers
  if (!GetOption("fzin")) {
    struct Field_t {
      Char_t *name;
      Float_t scale;
    };
    Field_t FieldOptions[5] = {
      {"FullMagFNegative", -1.0},
      {"FullMagFNegative",  1.0},
      {"HalfMagFNegative", -0.5},
      {"HalfMagFPositive",  0.5},
      {"ZeroMagF",          0.0}
    };
    Int_t k = -1;
    if         (GetOption("FieldON")) {
      if       (GetOption("ReverseField"))  k = 0;
      else                                  k = 1;
    } else if (GetOption("HalfField")) {
      if      (GetOption("ReverseField"))   k = 2;
      else                                  k = 3;
    } else if (GetOption("FieldOff"))       k = 4;
    if (k >= 0) {
      SetFlavor(FieldOptions[k].name,        "MagFactor");
      gMessMgr->QAInfo() << "StBFChain::SetDbOptions SetFlavor(\"" << FieldOptions[k].name 
			 << "\",\"MagFactor\")" << endm;
      if ( gClassTable->GetID("StarMagField") >= 0) {
	TString cmd = 
	  Form("if (!StarMagField::Instance()) new StarMagField( StarMagField::kMapped, %f, kTRUE);",
	       FieldOptions[k].scale);
	ProcessLine(cmd);
      }
    }
  }
}
//_____________________________________________________________________
/// Creates output-tree branches
void StBFChain::SetTreeOptions()
{
  StTreeMaker *treeMk = (StTreeMaker *) GetMaker("outputStream");
  if (!treeMk) return;
  treeMk->SetBranch("histBranch");
  if (GetOption("dstOut"))      {
    treeMk->IntoBranch("dstBranch","dst");
    if (GetOption("HitsBranch")) {
      treeMk->SetBranch("dstHitsBranch");
      treeMk->IntoBranch("dstHitsBranch","dst/.data/Hits");
    }
    else treeMk->IntoBranch("dstBranch","dst/.data/Hits");
    treeMk->IntoBranch("dstBranch","dst/.data/dst");
    treeMk->SetBranch("runcoBranch");
  }
  if (GetOption("Event") && GetOption("EvOut")){
    cout << "Will Write StEvent out, treeMk->GetFile() = "  << treeMk->GetFile() << endl;
    treeMk->IntoBranch("eventBranch","StEvent");
  }
  if (GetOption("McEvent") && GetOption("McEvOut")){
    cout << "Will Write StMcEvent out, treeMk->GetFile() = "  << treeMk->GetFile() << endl;
    treeMk->IntoBranch("McEventBranch","StMcEvent");
  }
  if (GetOption("GeantOut")) treeMk->IntoBranch("geantBranch","geant");
  if (GetOption("AllEvent")) {
    if (GetOption("fzin") || GetOption("ntin") || GetOption("gstar") || GetOption("VMC")  ) {
      treeMk->IntoBranch("geantBranch","geant");
      treeMk->IntoBranch("geantBranch","geant/.data/particle");
      treeMk->IntoBranch("geantBranch","geant/.data/g2t_rch_hit");
    }
    if (GetOption("fss"))    treeMk->IntoBranch("ftpc_rawBranch","ftpc_raw/.data");
    if (GetOption("tpc_daq") || GetOption("TrsMini"))
      treeMk->IntoBranch("tpc_rawBranch","tpc_raw/.data");
    if (GetOption("ems"))    treeMk->IntoBranch("emc_rawBranch","emc_raw/.data");
    if (GetOption("tcl"))    treeMk->IntoBranch("tpc_hitsBranch","tpc_hits/.data");
    if (GetOption("fcf"))    treeMk->IntoBranch("tpc_hitsBranch","tpc_hits/.data");
    if (GetOption("tpt"))    treeMk->IntoBranch("tpc_tracksBranch","tpc_tracks/.data");
    if (GetOption("srs"))    treeMk->IntoBranch("svt_hitsBranch","svt_hits/.data");
    if (GetOption("stk"))    treeMk->IntoBranch("svt_tracksBranch","svt_tracks/.data");
    if (GetOption("trg"))    treeMk->IntoBranch("trgBranch","ctf mwc trg");
    if (GetOption("global")) treeMk->IntoBranch("globalBranch","global/.data");
  }
}
//________________________________________________________________________________
Long_t  StBFChain::ProcessLine(const char *line) {
  if (! line ||  !strlen(line)) return -1;
  if (Debug()) cout << "StBFChain::ProcessLine " << line << endl;
  TInterpreter::EErrorCode error = TInterpreter::kNoError;
  Long_t  res = gInterpreter->ProcessLine(line, &error);
  if (error != TInterpreter::kNoError) {
    gMessMgr->Error() << "StBFChain::ProcessLine command:" << line << " has failed. Quit job." << endm;
    gSystem->Exit(1);
  }
  return res;
}
