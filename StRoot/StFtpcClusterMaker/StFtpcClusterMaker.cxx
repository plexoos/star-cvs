// $Id: StFtpcClusterMaker.cxx,v 1.13 2000/11/14 13:08:16 hummler Exp $
// $Log: StFtpcClusterMaker.cxx,v $
// Revision 1.13  2000/11/14 13:08:16  hummler
// add charge step calculation, minor cleanup
//
// Revision 1.12  2000/09/18 14:26:46  hummler
// expand StFtpcParamReader to supply data for slow simulator as well
// introduce StFtpcGeantReader to separate g2t tables from simulator code
// implement StFtpcGeantReader in StFtpcFastSimu
//
// Revision 1.11  2000/08/03 14:39:00  hummler
// Create param reader to keep parameter tables away from cluster finder and
// fast simulator. StFtpcClusterFinder now knows nothing about tables anymore!
//
// Revision 1.8  2000/04/13 18:08:21  fine
// Adjusted for ROOT 2.24
//
// Revision 1.7  2000/02/24 15:18:42  jcs
// inactivate histograms for MDC3
//
// Revision 1.6  2000/02/04 13:49:36  hummler
// upgrade ffs:
// -remove unused fspar table
// -make hit smearing gaussian with decent parameters and static rand engine
// -separate hit smearing from cluster width calculation
//
// Revision 1.5  2000/02/02 15:20:33  hummler
// correct acceptance at sector boundaries,
// take values from fcl_det
//
// Revision 1.4  2000/01/27 09:47:18  hummler
// implement raw data reader, remove type ambiguities that bothered kcc
//
//
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StFtpcClusterMaker class for Makers                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDAQMaker/StFTPCReader.h"

#include "StFtpcClusterMaker.h"
#include "StFtpcParamReader.hh"
#include "StFtpcGeantReader.hh"
#include "StFtpcChargeStep.hh"
#include "StFtpcClusterFinder.hh"
#include "StFtpcTrackMaker/StFtpcPoint.hh"
#include "StFtpcFastSimu.hh"
#include "StChain.h"
#include "St_DataSetIter.h"
#include "TH1.h"
#include "TH2.h"
#include "TClonesArray.h"

#include "tables/St_fcl_fppoint_Table.h"
#include "tables/St_fcl_ftpcsqndx_Table.h"
#include "tables/St_fcl_ftpcadc_Table.h"

#include "tables/St_fcl_padtrans_Table.h"
#include "tables/St_fcl_ampslope_Table.h"
#include "tables/St_fcl_ampoff_Table.h"
#include "tables/St_fcl_timeoff_Table.h"
#include "tables/St_fcl_det_Table.h"
#include "tables/St_fcl_zrow_Table.h"
#include "tables/St_g2t_vertex_Table.h"
#include "tables/St_g2t_track_Table.h"
#include "tables/St_g2t_ftp_hit_Table.h"
#include "tables/St_ffs_gaspar_Table.h"
#include "tables/St_ffs_gepoint_Table.h"

ClassImp(StFtpcClusterMaker)

  //_____________________________________________________________________________
StFtpcClusterMaker::StFtpcClusterMaker(const char *name):
StMaker(name),
    m_ampoff(0),
    m_ampslope(0),
    m_timeoff(0),
    m_padtrans(0),
    m_det(0),
    m_zrow(0),
    m_fspar(0),
    m_gaspar(0)
{
  drawinit=kFALSE;
}
//_____________________________________________________________________________
StFtpcClusterMaker::~StFtpcClusterMaker(){
}
//_____________________________________________________________________________
Int_t StFtpcClusterMaker::Init(){

  St_DataSet *ftpc = GetDataBase("ftpc");
  assert(ftpc);
  St_DataSetIter       local(ftpc);

  m_ampoff     = (St_fcl_ampoff   *)local("fclpars/ampoff"  );
  m_ampslope   = (St_fcl_ampslope *)local("fclpars/ampslope");
  m_timeoff    = (St_fcl_timeoff  *)local("fclpars/timeoff" );
  m_padtrans   = (St_fcl_padtrans *)local("fclpars/padtrans");
  m_det        = (St_fcl_det      *)local("fclpars/det"     );
  m_zrow       = (St_fcl_zrow     *)local("fclpars/zrow"    );
  m_gaspar     = (St_ffs_gaspar   *)local("ffspars/gaspar"  );

// 		Create Histograms
m_csteps      = new TH2F("fcl_csteps"	,"FTPC charge steps by sector"	,60,-0.5,59.5, 260, -0.5, 259.5);
//m_flags      = new TH1F("fcl_flags"	,"FTPC cluster finder flags"	,7,0.,8.);
//m_row        = new TH1F("fcl_row"	,"FTPC rows"			,20,1.,21.);
//m_sector     = new TH1F("fcl_sector"	,"FTPC sectors"			,6,1.,7.);
//m_pads       = new TH1F("fcl_pads"	,"FTPC pads"			,80,1.,161.);
//m_timebins   = new TH1F("fcl_timebins","FTPC timebins"		,100,1.,257.);
//m_row_sector = new TH2F("fcl_row_sector","FTPC(fcl) row vs. sector"	,20,1.,21.,6,1.,7.);
//m_npad_nbin  = new TH2F("fcl_pad_bin"	,"FTPC(fcl) pad vs. timebin"	,80,1.,161.,100,1.,257.);

  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StFtpcClusterMaker::Make()
{
  int iMake=kStOK;

  St_DataSet *daqDataset;
  StDAQReader *daqReader;
  StFTPCReader *ftpcReader;
  daqDataset=GetDataSet("StDaqReader");
  if(daqDataset)
    {
      cout << "Using StDAQReader to get StFTPCReader" << endl;
      assert(daqDataset);
      daqReader=(StDAQReader *)(daqDataset->GetObject());
      assert(daqReader);
      ftpcReader=daqReader->getFTPCReader();
    }

  // create parameter reader
  StFtpcParamReader *paramReader = new StFtpcParamReader(m_ampoff,
							 m_ampslope,
							 m_timeoff,
							 m_padtrans,
							 m_det,
							 m_zrow,
							 m_gaspar);
  
  St_DataSet *raw = GetDataSet("ftpc_raw");
  if (raw) {
    //			FCL
    St_DataSetIter get(raw);
    
    St_fcl_ftpcsqndx *fcl_ftpcsqndx = (St_fcl_ftpcsqndx*)get("fcl_ftpcsqndx");
    St_fcl_ftpcadc   *fcl_ftpcadc   = (St_fcl_ftpcadc*  )get("fcl_ftpcadc");

    if (fcl_ftpcsqndx&&fcl_ftpcadc) { 

      ftpcReader=new StFTPCReader((short unsigned int *) fcl_ftpcsqndx->GetTable(),
				  fcl_ftpcsqndx->GetNRows(),
				  (char *) fcl_ftpcadc->GetTable(),
				  fcl_ftpcadc->GetNRows());

      cout << "created StFTPCReader from tables" << endl;

      TClonesArray *hitarray = new TClonesArray("StFtpcPoint", 0);

      StFtpcChargeStep *step = new StFtpcChargeStep(m_csteps,
						    ftpcReader, 
						    paramReader);
      // uncomment to recalculate normalized pressure from charge step:
      step->histogram(1);
      // uncomment to fill charge step histogram only:
      //      step->histogram(0);

      if(Debug()) cout<<"start running StFtpcClusterFinder"<<endl;
            
      StFtpcClusterFinder *fcl = new StFtpcClusterFinder(ftpcReader, 
							 paramReader, 
							 hitarray);

      int searchresult=fcl->search();
      
      if (searchresult == 0)
	{
	  iMake=kStWarn;
	}
      else
	{
	  Int_t num_points = hitarray->GetEntriesFast();
	  St_fcl_fppoint *fcl_fppoint = new St_fcl_fppoint("fcl_fppoint",num_points);
	  m_DataSet->Add(fcl_fppoint);

	  fcl_fppoint_st *pointTable= fcl_fppoint->GetTable();

	  StFtpcPoint *point;
    
	  for (Int_t i=0; i<num_points; i++) 
	    {
	      point = (StFtpcPoint *)hitarray->At(i);
	      point->ToTable(&(pointTable[i]));    
	    }
   
	  fcl_fppoint->SetNRows(num_points);
	  
	}
      delete fcl;
      delete step;
      delete ftpcReader;
    }
    else {
      
      cout <<"StFtpcClusterMaker: Tables are not found:" 
	   << " fcl_ftpcsqndx = " << fcl_ftpcsqndx 
	   << " fcl_ftpcadc   = " << fcl_ftpcadc << endl;
    }
  }
  else { 
    
    //                      FFS
    St_DataSet *gea = GetDataSet("geant");
    St_DataSetIter geant(gea);
    St_g2t_vertex  *g2t_vertex  = (St_g2t_vertex *) geant("g2t_vertex");
    St_g2t_track   *g2t_track   = (St_g2t_track *)   geant("g2t_track");
    St_g2t_ftp_hit *g2t_ftp_hit = (St_g2t_ftp_hit *) geant("g2t_ftp_hit");
    if (g2t_vertex && g2t_track && g2t_ftp_hit){
      StFtpcGeantReader *geantReader = new StFtpcGeantReader(g2t_vertex,
							     g2t_track,
							     g2t_ftp_hit);

      St_ffs_gepoint *ffs_gepoint = new St_ffs_gepoint("ffs_gepoint",150000);
      m_DataSet->Add(ffs_gepoint);
      St_fcl_fppoint *fcl_fppoint = new St_fcl_fppoint("fcl_fppoint",150000);
      m_DataSet->Add(fcl_fppoint);
      
      if(Debug()) cout<<"NO RAW DATA AVAILABLE - start running StFtpcFastSimu"<<endl;
      
      Int_t numGepoint=ffs_gepoint->GetNRows();
      Int_t maxGepoint=ffs_gepoint->GetTableSize();
      Int_t numFppoint=fcl_fppoint->GetNRows();
      Int_t maxFppoint=fcl_fppoint->GetTableSize();
      StFtpcFastSimu *ffs = new StFtpcFastSimu(ffs_gepoint->GetTable(),
					       &numGepoint, maxGepoint,
					       fcl_fppoint->GetTable(),
					       &numFppoint, maxFppoint,
					       geantReader,
					       paramReader);
      ffs_gepoint->SetNRows(numGepoint);				      
      fcl_fppoint->SetNRows(numFppoint);				      
      if(Debug())cout<<"finished running StFtpcFastSimu"<<endl;
      delete ffs;
      delete geantReader;
    }
  }
  
  delete paramReader;
// Deactivate histograms for MDC3
//MakeHistograms(); // FTPC cluster finder histograms
  return iMake;
}
//_____________________________________________________________________________
void StFtpcClusterMaker::MakeHistograms() 
{

 // cout<<"*** NOW MAKING HISTOGRAMS FOR fcl ***"<<endl;

  // Create an iterator
  St_DataSetIter fcl_points(m_DataSet);

  //Get the table
  St_fcl_fppoint *ppointh;
  ppointh = (St_fcl_fppoint *) fcl_points.Find("fcl_fppoint");
  if (! ppointh) 	return;
  fcl_fppoint_st *r = ppointh->GetTable();
  for (Int_t i=0; i<ppointh->GetNRows();i++,r++) {
    Int_t flag = r->flags;
    if (flag > 0) {
      Int_t bin = 6;
      for (Int_t twofac=32; twofac>0; twofac=twofac/2,bin--) {
        Int_t nbit = flag/twofac;
        if (nbit != 1) 	continue;
        m_flags->Fill((float)bin);
        flag = flag - nbit*twofac;        
      }//end loop twofac
    }//endif flag

    Float_t nrow = r->row;
    m_row->Fill(nrow);
    Float_t nsec = r->sector;
    m_sector->Fill(nsec);
    m_row_sector->Fill(nrow,nsec);
    Float_t npad = r->n_pads;
    m_pads->Fill(npad);
    Float_t nbin = r->n_bins;
    m_timebins->Fill(nbin);
    m_npad_nbin->Fill(npad,nbin);
  }//end rows loop 
}
                                   

