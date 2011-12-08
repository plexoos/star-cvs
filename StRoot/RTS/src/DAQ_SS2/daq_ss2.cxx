#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include <rtsLog.h>
#include <rtsSystems.h>



#include <SFS/sfs_index.h>
#include <DAQ_READER/daqReader.h>
#include <DAQ_READER/daq_dta.h>


#include "daq_ss2.h"


const char *daq_ss2::help_string = "SST\n\
raw	returns raw data\n" ;

class daq_det_ss2_factory : public daq_det_factory
{
public:
	daq_det_ss2_factory() {
		daq_det_factory::det_factories[SST_ID] = this ;
	}

	daq_det *create() {
		return new daq_ss2 ;
	}
} ;

static daq_det_ss2_factory ss2_factory ;


daq_ss2::daq_ss2(daqReader *rts_caller) 
{
	rts_id = SST_ID ;
	name = rts2name(rts_id) ;
	sfs_name = "sst" ;
	caller = rts_caller ;
	
	if(caller) caller->insert(this, rts_id) ;

	raw = new daq_dta ;

	LOG(DBG,"%s: constructor: caller %p",name,rts_caller) ;
	return ;
}

daq_ss2::~daq_ss2() 
{
	LOG(DBG,"%s: DEstructor",name) ;

	delete raw ;


	return ;
}



daq_dta *daq_ss2::get(const char *bank, int sec, int rdo, int pad, void *p1, void *p2) 
{	
	Make() ;

	if(present == 0) return 0 ;


	if(strcasecmp(bank,"raw")==0) {
		return handle_raw(sec,rdo) ;
	}


	LOG(ERR,"%s: unknown bank type \"%s\"",name,bank) ;
	return 0 ;
}



daq_dta *daq_ss2::handle_raw(int sec, int rdo)
{
	char *st ;
	int r_start, r_stop ;
	int s_start, s_stop ;
	int bytes ;
	char str[256] ;
	char *full_name ;


	assert(caller) ;	// sanity...

	if(!present) {
		return 0 ;
	}
	else {
		LOG(DBG,"%s: present %d",name,present) ;
	}



	if(sec <= 0) {
		s_start = 1 ;
		s_stop = 2 ;
	}
	else {
		s_start = s_stop = sec ;
	}

	if(rdo<=0) {
		r_start = 1 ;
		r_stop = 4 ;	
	}
	else {
		r_start = r_stop = rdo ;
	}


	raw->create(8*1024,"ss2_raw",rts_id,DAQ_DTA_STRUCT(char)) ;


	for(int s=s_start;s<=s_stop;s++) {

	for(int r=r_start;r<=r_stop;r++) {
		sprintf(str,"%s/sec%02d/rb%02d/raw",sfs_name, s, r) ;
		full_name = caller->get_sfs_name(str) ;
		
		if(!full_name) continue ;
		bytes = caller->sfs->fileSize(full_name) ;	// this is bytes


		st = (char *) raw->request(bytes) ;
		
		int ret = caller->sfs->read(str, st, bytes) ;
		if(ret != bytes) {
			LOG(ERR,"ret is %d") ;
		}

	
		raw->finalize(bytes,s,r,0) ;	;

	}	// end of loop over RDOs [1..4]

	}	// end of loop over Sectors [1..2]

	raw->rewind() ;

	return raw ;
	
}

	
int daq_ss2::get_l2(char *buff, int buff_bytes, struct daq_trg_word *trg, int rdo)
{
	return -1 ;
}
