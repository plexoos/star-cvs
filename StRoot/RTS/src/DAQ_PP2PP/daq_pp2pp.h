#ifndef _DAQ_PP2PP_H_
#define _DAQ_PP2PP_H_


#include <DAQ_READER/daq_det.h>

#define PP2PP_SVX_CH	128

struct pp2pp_t {
	u_char seq_id ;		// 1..4 ;
	u_char chain_id ;	// 1..4 ;
	u_char svx_id ;		// 1..8 ;
	u_char reserved ;	// for padding...

	u_char adc[PP2PP_SVX_CH] ;	
} ;

class daq_pp2pp : public daq_det {
private:
	class daq_dta *handle_raw(int sec, int rdo) ;
	class daq_dta *handle_adc(int sec, int rdo) ;

	class daq_dta *raw ;
	class daq_dta *adc ;

	static const int MAX_SEC = 2 ;
	static const int MAX_RDO = 4 ;	// can be 0 for all RDOs; sequencers, typically 4

	static const char *help_string ;
protected:


public:
	daq_pp2pp(daqReader *rts_caller=0) ;
	~daq_pp2pp() ;


	daq_dta  *get(const char *bank="*",int c1=-1, int c2=-1, int c3=-1, void *p1=0, void *p2=0) ;

	int get_l2(char *buff, int buff_bytes, struct daq_trg_word *trg, int prompt) ;
	int get_token(char *buff, int buff_bytes) ;

	virtual const char *GetCVS() const {	// Offline
                static const char cvs[]="Tag $Name:  $Id: built "__DATE__" "__TIME__ ; return cvs;
	}

} ;


#endif	// _DAQ_PP2PP_H_
