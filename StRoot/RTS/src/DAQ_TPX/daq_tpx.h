#ifndef _DAQ_TPX_H_
#define _DAQ_TPX_H_


#include <RTS_READER/daq_det.h>


// forward decls
class tpxPed ;
class tpxGain ;
class tpxFCF ;
class tpxStat ;

class daq_tpx : public daq_det {
private:
	class daq_dta *handle_raw(int sec, int rdo) ;
	class daq_dta *handle_adc(int sec, int rdo) ;
	class daq_dta *handle_cld(int sec, int rdo) ;
	class daq_dta *handle_cld_raw(int sec, int rdo) ;
	class daq_dta *handle_cld_sim(int sec, int row) ;

	// direct maps to file content:
	class daq_dta *raw ;
	class daq_dta *cld_raw ;
	class daq_dta *ped_raw ;	// token 0

	// calculated from "raw"
	class daq_dta *adc ;

	// calculated from "cld_raw"
	class daq_dta *cld ;

	// input classes by the user
	class daq_dta *adc_sim ;
	class daq_dta *gain ;	// ACTIVE: from file or database
	class daq_dta *ped ;	// ACTIVE: from file

	// calculated from "adc_sim"
	class daq_dta *cld_sim ;

	// calculated via algorithm at end-run
	class daq_dta *ped_c ;	// in ped runs 
	class daq_dta *gain_c ;	// in pulser runs


	// algorithms
	class tpxPed *ped_algo ;
	class tpxGain *gain_algo ;
	class tpxStat *stat_algo ;

	class tpxFCF *fcf_algo[25] ;
	u_int *fcf_tmp_storage ;
	
	static const int FCF_TMP_BYTES = (128*1024) ;	// for local FCF

	static const int MAX_SEC = 24 ;
	static const int MAX_RDO = 6 ;

	static const char *help_string ;

protected:

public:
	daq_tpx(const char *dname="TPX", rts_reader *rts_caller=0) ;
	~daq_tpx() ;




	int Make() ;			// equivalent to analyze
	int InitRun(int run_num) ;	// used in send_config
	int FinishRun(int old_run) ;	// used in inject_token0


	class daq_dta  *get(const char *bank="*", int c1=-1, int c2=-1, int c3=-1, void *p1=0, void *p2=0) ;

	class daq_dta  *put(const char *bank="*", int c1=-1, int c2=-1, int c3=-1, void *p1=0, void *p2=0) ;

	// these functions perform on the raw DDL buffer data i.e. per RDO
	int get_token(char *buff, int buff_bytes) ;
	int get_l2(char *buff, int buff_bytes, struct daq_trg_word *trg, int prompt=0) ;



	// trivial stuff below...
	virtual const char *GetCVS() const {	// Offline
		static const char cvs[]="Tag $Name:  $Id: built "__DATE__" "__TIME__ ; return cvs;
	}

	void help() const ;

} ;


#endif	// _DAQ_TPX_H_
