#include "St_pp2pp_Maker.h"
#include "StRtsTable.h"
#include "TDataSetIter.h"
#include "RTS/src/DAQ_PP2PP/pp2pp.h"

#include "St_db_Maker/St_db_Maker.h"

#include "tables/St_pp2ppPedestal_Table.h"
#include "tables/St_pp2ppOffset_Table.h"

#include "StEvent/StEvent.h"
#include "StEvent/StRpsCollection.h"
#include "StEvent/StRpsCluster.h"

ClassImp(St_pp2pp_Maker)

  St_pp2pp_Maker::St_pp2pp_Maker(const char *name) : StRTSBaseMaker("pp2pp",name),   
						     pedestal_perchannel_filename("pedestal.in.perchannel"), LDoCluster(kTRUE) {
  // ctor
  //  nevt_count = 0 ;
}


St_pp2pp_Maker::~St_pp2pp_Maker() {
}

//_____________________________________________________________________________
/// Init - is a first method the top level StChain calls to initialize all its makers 
Int_t St_pp2pp_Maker::Init() {
  fLast_svx   = ErrorCode;
  fLast_chain = ErrorCode;
  fLast_seq   = ErrorCode ;
  return StMaker::Init();
}

Int_t St_pp2pp_Maker::InitRun(int runumber) {
  if ( LDoCluster ) {
    read_pedestal_perchannel() ;
    read_offset_perplane() ;
  }
  return 0;
}

Int_t St_pp2pp_Maker::read_pedestal_perchannel() {

  //  cout << "Size of each struct in DB : " << sizeof(pp2ppPedestal_st) << endl ;

  //  cout << "Size of pedave : " << sizeof(pedave) << " , Size of pedrms : " << sizeof(pedrms) << endl ;

  memset(pedave,0,sizeof(pedave));
  memset(pedrms,0,sizeof(pedrms));

  Int_t s, c, sv, ch, idb = 0 ;

  /*
  //  cout << GetTime() << " " << GetDate() << endl ;

  St_db_Maker *dbMk = (St_db_Maker*) GetMaker("db");
  if ( ! dbMk ) {
    LOG_WARN << "No St_db_Maker existed ?! " << endm ;
  }
  else {
    //    cout << "I got St_db_Maker ?? " << endl ;
    dbMk->SetDateTime(this->GetDate(), this->GetTime());
  }
  */

  // Database
  TDataSet *DB = 0;
  DB = GetInputDB("Calibrations/pp2pp");
  if (!DB) {
    LOG_WARN << "ERROR: cannot find database Calibrations_pp2pp?" << endm ;
  }
  else {
    // fetch ROOT descriptor of db table
    St_pp2ppPedestal *descr = 0;
    descr = (St_pp2ppPedestal*) DB->Find("pp2ppPedestal");
    // fetch data and place it to appropriate structure
    if (descr) {
        pp2ppPedestal_st *table = descr->GetTable();
	//	cout << "Reading pp2ppPedestal table with nrows = " << descr->GetNRows() << endl ;
	for ( idb = 0; idb < descr->GetNRows(); idb++ ) {
	  s = (Int_t) table[idb].sequencer ;
	  c = (Int_t) table[idb].chain ;
	  sv = (Int_t) table[idb].SVX ;
	  ch = (Int_t) table[idb].channel ;

	  pedave[s-1][c][sv][ch] = table[idb].mean ;
	  pedrms[s-1][c][sv][ch] = table[idb].rms ;

	  //		  cout << s << " " << c << " "  << sv << " " << ch << " " << pedave[s-1][c][sv][ch] << " " << pedrms[s-1][c][sv][ch] << endl ; 

	}
    } else {
      LOG_WARN << "WARNING: No data in pp2ppPedestal table (wrong timestamp?). Nothing to return, then." << endm ;
    }
  }

  cout << idb << " pedestal entries read from DB table Calibration/pp2pp read. " << endl ;


  return 1 ;

}

Int_t St_pp2pp_Maker::read_offset_perplane() {

  offset_table = 0;

  TDataSet *DB = 0;
  DB = GetInputDB("Geometry/pp2pp");
  if (!DB) { 
    LOG_WARN << "ERROR: cannot find database Geometry_pp2pp?" << std::endl; 
  }
  else {

    // fetch ROOT descriptor of db table
    St_pp2ppOffset *descr = 0;
    descr = (St_pp2ppOffset*) DB->Find("pp2ppOffset");
    // fetch data and place it to appropriate structure
    if (descr) {
      offset_table = descr->GetTable();
      cout << "Reading pp2ppOffset table with nrows = " << descr->GetNRows() << endl ;
      /*
      for (Int_t i = 0; i < descr->GetNRows(); i++) {
	for ( Int_t j = 0; j< 32 ; j++ )
	  std::cout << offset_table[i].rp_offset_plane[j] << " "  ; 
	cout << endl ;
      }
      */
    } else {
      LOG_WARN << "St_pp2pp_Maker : No data in pp2ppOffset table (wrong timestamp?). Nothing to return, then" << endm ;
    }

  }

  return 1 ;

}



//_____________________________________________________________________________
/// Clear - this method is called in loop for prepare the maker for the next event
void  St_pp2pp_Maker::Clear(Option_t *) {

  // Deleting previous cluster info.
  for ( Int_t i=0; i<MAXSEQ; i++)
    for ( Int_t j=0; j<MAXCHAIN; j++)
      (validhits[i][j]).clear();

  StMaker::Clear(); // perform the basic clear (mandatory)

}


//_____________________________________________________________________________
/// Make - this method is called in loop for each event
Int_t St_pp2pp_Maker::Make(){

  //  if ( nevt_count%10000 == 0 ) cout << "St_pp2pp_Maker:: Event count : " << nevt_count << endl ;

  //  nevt_count++ ;


  //
  //  PrintInfo();
  //
  int counter = -1; 
  
  TGenericTable *pp2ppRawHits = new TGenericTable("pp2ppRawHit_st","pp2ppRawHits");

  //  ls (0);

  // Each GetNextAdc would get a SVX ...
  while ( GetNextAdc() ) {
     counter++;
     TGenericTable::iterator iword = DaqDta()->begin();
     for (;iword != DaqDta()->end();++iword) {
        pp2pp_t &d = *(pp2pp_t *)*iword;
        // do something
	DoerPp2pp(d,*pp2ppRawHits);
     }
  }


  if (counter < 0) {
    LOG_INFO << "There was no pp2pp data for this event. " << endm;
  } else {
    LOG_INFO << "End of pp2pp data for this event : " << GetEventNumber() << ", Total = "  << counter+1 
	     << " records were found" << endm;
  }

  AddData(pp2ppRawHits);  // publish RawHits to make it available for other makers in the chain
  // one may not call AddData if the result should not be published.
  // to discard the result one should call  "delete pp2ppRawHits"


  if ( LDoCluster ) { 

    for ( Int_t i=0; i<MAXSEQ; i++)
      for ( Int_t j=0; j<MAXCHAIN; j++) {
	sort( (validhits[i][j]).begin(), (validhits[i][j]).end(), hitcompare);
	//	cout << "Size of vector of sequencer " << i+1 << " chain " << j << " " << dec << (validhits[i][j]).size() << endl ;
      }

    MakeClusters();

  }

  return kStOK;

}

//_____________________________________________________________________________
/// DoerPp2pp - this method is called as soon as next pp2pp record is read in
Int_t St_pp2pp_Maker::DoerPp2pp(const pp2pp_t &d, TGenericTable &hitsTable) {

  pp2ppRawHit_st       oneSihit = {0}; // This essentially gives adc the value of "0"
  oneSihit.sec       = Sector() ;
  oneSihit.sequencer = d.seq_id ;
  oneSihit.chain     = d.chain_id ;
  oneSihit.svx       = d.svx_id ;

  // For clustering purpose
  HitChannel onehit ;

  // Mar. 14, 2009 (K. Yip) : checking for wrong SVX_ID
  // One known case is for SEQ 3, CHAIN 2 and SVX is 7 but it should be 3.
  // Mostly, just some debugging codes that we've used in the past and shouldn't happen

  if ( (oneSihit.svx != fLast_svx) && (fLast_svx != ErrorCode) ) {

    if (  Int_t(oneSihit.svx-1) != fLast_svx )

      if (  ( (oneSihit.svx-fLast_svx) != -3 && ( (oneSihit.chain%2)==1 ) ) ||
	    ( (oneSihit.svx-fLast_svx) != -5 && ( (oneSihit.chain%2)==0 ) ) ) {

	if ( oneSihit.svx == 7 && oneSihit.sequencer == 3 && oneSihit.chain == 2 )
	  oneSihit.svx = 3 ;
	//		  else if ( oneSihit.svx < fLast_svx ) {
	else if ( oneSihit.svx < fLast_svx && ( GetRunNumber()<10185015 || (fLast_seq!=2 && fLast_chain!=2)) ) { // bad seq 2 and chain D

	  LOG_WARN << "Decreased ? " <<  GetEventNumber() << " : fLast_seq = " << fLast_seq << ", fLast_chain = " << fLast_chain << ", fLast_svx = " << fLast_svx << endm ;
	  LOG_WARN << "Decreased ?  " << GetEventNumber() << " : Now, seq = " << (int) oneSihit.sequencer << ", chain = " << (int) oneSihit.chain << ", svx = " << (int) oneSihit.svx << endm ;
	  
	  oneSihit.svx = fLast_svx + 1 ;
		    
	  LOG_WARN << "Decreased ? : So -> " << " svx is now = " << (int) oneSihit.svx << endm ;	      

	}
	//	else if ( fLast_seq!=2 && fLast_chain!=2 ) { // bad seq 2 and chain D
	else if ( GetRunNumber()<10185015 || ( fLast_seq!=2 && fLast_chain!=2 ) ) { // bad seq 2 and chain D

	  LOG_WARN << GetEventNumber() << " : fLast_seq = " << fLast_seq << ", fLast_chain = " << fLast_chain << ", fLast_svx = " << fLast_svx << endm ;
	  LOG_WARN << GetEventNumber() << " : Now, seq = " << (int) oneSihit.sequencer << ", chain = " << (int) oneSihit.chain << ", svx = " << (int) oneSihit.svx << endm ;

	}

      }
	      

  }
  else if ( (oneSihit.chain==fLast_chain) && (fLast_chain != ErrorCode) ) {
    LOG_WARN << "Repeated ? :" << GetEventNumber() << " : fLast_seq = " << fLast_seq << ", fLast_chain = " << fLast_chain << ", fLast_svx = " << fLast_svx << endm ;
    LOG_WARN << "Repeated ? : " << GetEventNumber() << " : Now, seq = " << (int) oneSihit.sequencer << ", chain = " << (int) oneSihit.chain << ", svx = " << (int) oneSihit.svx << endm ;

    oneSihit.svx = fLast_svx + 1 ;

    LOG_WARN << "Repeated : So -> " << " svx is now = " << (int) oneSihit.svx << endm ;	      
  }


  fLast_seq = oneSihit.sequencer; 
  fLast_chain = oneSihit.chain;
  fLast_svx = oneSihit.svx;

  //  cout << "Seq: " << fLast_seq << " , chain " << fLast_chain << ", SVX = " << fLast_svx << endl ;

  for(unsigned int c=0;c<sizeof(d.adc);c++) {
    //	      if( d.adc[c] ) printf("   %3d: %3d [0x%02X]\n",c,d.adc[c],d.adc[c]) ;
    //	      adc[nsvx][c] = d.adc[c];
    if ( d.trace[c] == 1 ) {
      oneSihit.channel = c ;
      oneSihit.adc = d.adc[c];
      hitsTable.AddAt(&oneSihit);

      //      cout << "channel " << c << " , adc " << (int) d.adc[c] << endl ;

      if ( LDoCluster && (c != 127) && (c != 0) ) { // Avoid the channels at 2 ends of SVX
	
	// Getting rid of the 1st channel (0) and the last channel (127)
	onehit.first = fLast_svx*(MAXSTRIP-2) + oneSihit.channel - 1  ; 

	onehit.second = oneSihit.adc -  pedave[fLast_seq-1][fLast_chain][fLast_svx][oneSihit.channel] ;

	if ( onehit.second > 5*pedrms[fLast_seq-1][fLast_chain][fLast_svx][oneSihit.channel] ) {
	  (validhits[fLast_seq-1][fLast_chain]).push_back(onehit);
	  //	  cout << "validhits : position " << onehit.first << " , energy " << onehit.second << endl ;
	}
      }

    }
    else if ( d.trace[c] != 0 )
      std::cout << GetEventNumber() << " : trace = " << (Int_t) d.trace[c] << ", Seq " << (Int_t) oneSihit.sequencer 
		<< ", chain " << (Int_t) oneSihit.chain << ", SVX " << (Int_t) oneSihit.svx << ", channel " << c 
		<< " is duplicated ? ==> " << (Int_t) d.adc[c] << std::endl ;
  }

  return 1;

}

Int_t St_pp2pp_Maker::MakeClusters() {

  //  const Int_t MAX_Cls_L = 5 ;
  //  const Int_t MIN_Charge = 20 ;
  //                                               EHI        EHO        EVU        EVD          WHI          WHO        WVD       WVU
  const short orientations[MAXCHAIN*MAXSEQ] = {-1,1,-1,1,  1,-1,1,-1,  1,1,1,1, -1,-1,-1,-1,  -1,-1,-1,-1,  1,1,1,1,  -1,1,-1,1, 1,-1,1,-1 };

  Bool_t is_candidate_to_store ;

  Int_t NCluster_Length ;
  Double_t ECluster, POStimesE, position, offset ;

  // For inserting into StEvent
  pp2ppColl = new StRpsCollection(); 

  vector< HitChannel >::iterator it, it_next ;

  for ( Int_t i=0; i<MAXSEQ; i++)
    for ( Int_t j=0; j<MAXCHAIN; j++) {

      NCluster_Length = 0 ;
      ECluster = 0 ;
      POStimesE = 0 ;

      // Assume 4 planes have the same z at least for now
      if ( i==0 || i==1 )
	pp2ppColl->romanPot(i)->plane(j)->setZ(-55.496) ; 
      else if ( i==2 || i==3 )
	pp2ppColl->romanPot(i)->plane(j)->setZ(-58.496) ; 
      else if ( i==4 || i==5 )
	pp2ppColl->romanPot(i)->plane(j)->setZ(55.496) ; 
      else if ( i==6 || i==7 )
	pp2ppColl->romanPot(i)->plane(j)->setZ(58.496) ; 
      
      if ( offset_table )
	offset = offset_table[0].rp_offset_plane[4*i+j]/1000. ; // all in m
      else
	offset = -9999. ;
      //      cout << "Offsets : " <<  i << " " << j << " " << offset_table[0].rp_offset_plane[4*i+j] << endl ; 

      pp2ppColl->romanPot(i)->plane(j)->setOffset( offset ) ; 

      pp2ppColl->romanPot(i)->plane(j)->setOrientation( orientations[4*i+j] ) ;

      it = (validhits[i][j]).begin() ;

      while ( it != (validhits[i][j]).end() ) {

	//	cout << "Seq: " << i+1 << " , chain " << j << ", channel : " << it->first << " , energy : " << it->second << endl ;
	NCluster_Length++ ;
	ECluster += it->second ;
	POStimesE += it->first*it->second ;
	
	it_next = it + 1 ;

	is_candidate_to_store = kFALSE ;

	// Deciding whether it's time to finish this particular clustering process
	if ( it_next != (validhits[i][j]).end() ) {

	  // if the next one is not a neighbor --> a candidate cluster
	  if ( (it_next->first - it->first)!=1  ) 
	    is_candidate_to_store = kTRUE ;

	}
	else { 	// if already at the end --> a candidate cluster
	  is_candidate_to_store = kTRUE ;
	}

	if ( is_candidate_to_store == kTRUE ) {

	  //	  if ( NCluster_Length <= MAX_Cls_L && ECluster >= MIN_Charge ) {

	    // StEvent Clusters
	    StRpsCluster * oneStCluster = new StRpsCluster() ;

	    oneStCluster->setEnergy(ECluster);
	    oneStCluster->setLength(NCluster_Length);
	    if ( (j % 2) == 0 ) // A or C : pitch_4svx = 0.00974 cm
	      position = POStimesE/ECluster*9.74E-5 ; // in m
	    else                // B or D : pitch_6svx = 0.01050 cm
	      position = POStimesE/ECluster*1.050E-4; // in m


	    oneStCluster->setPosition(position); // in m
	   
	    oneStCluster->setXY( offset + orientations[4*i+j]*position ) ; // all in m

	    pp2ppColl->romanPot(i)->plane(j)->addCluster(oneStCluster);

	  //	} 
	  /*
	  else
	    cout << "NOT Stored ! seq/chain : " << i+1 << "/" << j 
		 << " , length = " << NCluster_Length << " , energy = " << ECluster
		 << " , position = " << POStimesE/ECluster  << endl ;
	  */

	  ECluster = 0 ;
	  POStimesE = 0 ;
	  NCluster_Length = 0 ;

	}

	it++ ;

      } // while

    } // for ( Int_t j=0; j<MAXCHAIN; j++) {


  // Store into StEvent
  mEvent = (StEvent *) GetInputDS("StEvent");
  if ( mEvent ) {
    mEvent->setRpsCollection(pp2ppColl);
  }
  else
    LOG_WARN << "St_pp2pp_Maker : StEvent not found !" << endm ;

  return 1 ;

}


Int_t St_pp2pp_Maker::Finish() {


  return StMaker::Finish();

}


