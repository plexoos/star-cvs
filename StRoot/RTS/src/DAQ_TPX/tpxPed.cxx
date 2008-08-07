#include <stdio.h>
#include <sys/types.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include <rtsLog.h>
#include <TPC/rowlen.h>

#include <DAQ_TPX/tpxCore.h>
#include <DAQ_TPX/tpxPed.h>


tpxPed::tpxPed()
{
	smoothed = 0 ;
	valid = 0 ;
	memset(evts,0,sizeof(evts)) ;
	memset(valid_evts,0,sizeof(valid_evts)) ;
	
	sizeof_ped = sizeof(struct peds) * 46 * 183 ;

	ped_store = 0 ;	// unassigned!


	return ;
}


tpxPed::~tpxPed()
{
	if(ped_store) free(ped_store) ;

	return ;
}

tpxPed::peds *tpxPed::get(int row, int pad)
{
	return (ped_store + row*183 + pad) ;
}

void tpxPed::init()
{
	smoothed = 0 ;
	valid = 0 ;

	memset(evts,0,sizeof(evts)) ;
	memset(valid_evts,0,sizeof(valid_evts)) ;

	if(ped_store == 0) {
		ped_store = (struct peds *) malloc(sizeof_ped) ;
	}

	memset(ped_store,0,sizeof_ped) ;

	LOG(TERR,"Pedestals zapped.") ;
}

/*
	Called per event, per RDO. evbbuff is the raw RDO contribuition
*/
void tpxPed::accum(char *evbuff, int bytes)
{
	int t ;
	u_int *data_end ;
	tpx_rdo_event rdo ;
	tpx_altro_struct a ;


	t = tpx_get_start(evbuff, bytes/4, &rdo, 0) ;

	if(t <= 0) return ;	// non data event...



	a.what = TPX_ALTRO_DO_ADC ;
	a.rdo = rdo.rdo - 1 ;	// a.rdo counts from 0
	a.t = t ;

	evts[a.rdo]++ ;


	// skip first few events!
	if(evts[a.rdo] <= 3) {
		LOG(WARN,"RDO %d: skipping event %d < 3",rdo.rdo,evts[a.rdo]) ;
		return ;
	}
	if(tpx_rdo_dbg[a.rdo].delta < 200000) {
		LOG(WARN,"RDO %d: skipping event %d: delta %u too small",rdo.rdo,evts[a.rdo],tpx_rdo_dbg[a.rdo].delta) ;
		usleep(10000) ;
		return ;
	}

	valid_evts[a.rdo]++ ;

        LOG(NOTE,"RDO %d: event %d: delta %u OK",rdo.rdo,evts[a.rdo],tpx_rdo_dbg[a.rdo].delta) ;

	data_end = rdo.data_end ;

	do {
		data_end = tpx_scan_to_next(data_end, rdo.data_start, &a) ;
		accum(&a) ;
	} while(data_end && (data_end > rdo.data_start)) ;


	return ;

}

void tpxPed::accum(tpx_altro_struct *a)
{
	int i ;
	int row, pad ;
	struct peds *p ;



	row = a->row ;
	pad = a->pad ;



	p = get(row, pad) ;
	if(p==0) {
		LOG(ERR,"ped::accum for row %d, pad %d, A %d:%d bad?",row,pad,a->id,a->ch) ;
		return ;
	}

	// do not allow more than 1000 events; use tb[20]'s counter...
	if(p->cou[20] > 1000) return ;
	
	
	LOG(DBG,"count %d",a->count) ;
	for(i=0;i<a->count;i++) {
		int tb, adc ;

		adc = a->adc[i] ;
		tb = a->tb[i] ;

		p->ped[tb] += (double) adc ;
		p->rms[tb] += (double) (adc*adc) ;
		p->cou[tb]++ ;
	}


	return ;
}

void tpxPed::calc()
{
	int r,p,t ;

	LOG(TERR,"Calculating pedestals...") ;
	for(r=0;r<=45;r++) {
	for(p=0;p<=182;p++) {
		struct peds *ped = get(r,p) ;

		for(t=0;t<512;t++) {
			if(ped->cou[t] == 0) {
				ped->ped[t] = 1023.0 ;
				ped->rms[t] = 9.999 ;
			}
			else {
				double pp, rr ;

				pp = ped->ped[t] / (double) ped->cou[t] ;
				rr = ped->rms[t] / (double) ped->cou[t] ;

				// due to roundoff I can have super small negative numbers
				if(rr < (pp*pp)) rr = 0.0 ;
				else rr = sqrt(rr - pp*pp) ;

				ped->ped[t] = pp ;
				ped->rms[t] = rr ;
			}
		}
	}
	}

	for(r=0;r<6;r++) {
		if(evts[r]) {	// RDO got some events
			if(valid_evts[r]<10) {
				LOG(ERR,"RDO %d: not enough pedestal events with required characteristics: %d/%d",r+1,valid_evts[r],evts[r]) ;
			}
		}
	}

	LOG(TERR,"Pedestals calculated. RDO counts: %u %u %u %u %u %u",valid_evts[0],valid_evts[1],valid_evts[2],valid_evts[3],valid_evts[4],valid_evts[5]) ;
	valid = 1 ;

	return ;
}

// returns bytes!
int tpxPed::to_altro(char *buff, int rb, int timebins)
{
	int row, pad, t ;
	int a, ch ;

	FILE *fff ;
	char fname[128] ;

	sprintf(fname,"/RTScache/altro_ped_%d_%03d.txt",rb+1,timebins) ;

	fff = fopen(fname,"w") ;
	if(fff==0) {
		LOG(WARN,"Can't open \"%s\"",fname) ;
	}

	char *rbuff = buff ;

	if(!valid || !smoothed) {
		LOG(ERR,"ped::to_evb peds are bad: valid %d, smoothed %d",valid,smoothed) ;
	}

	LOG(TERR,"Preparing pedestals for RDO %d...",rb) ;

	for(a=0;a<256;a++) {
	for(ch=0;ch<16;ch++) {

		tpx_from_altro(rb,a,ch,row,pad) ;

		if(row > 45) continue ;	// not here...



		u_int *addr = (u_int *) rbuff ;	// remember where to store the address
	
		rbuff += 4 ;	// skip 4 bytes

		u_short *ptr = (u_short *) rbuff ;	// start

		int tcou = 0 ;	// zero counter...

		// get the corresponding row & pad

		struct peds *ped = get(row,pad) ;


		for(t=0;t<timebins+15;t++) {
			if(fff) fprintf(fff,"%3d %2d %3d %3d\n",a,ch,t,(u_short)ped->ped[t]) ;
		}
#if 0
		// copy as shorts BUT:
		//	needs to go from 15 _AND_ needs to be even!
		for(t=15;t<509;t++) {
			*ptr++ = (u_short) ped->ped[t] ;
			tcou++ ;
		}

		if(tcou & 1) {
			*ptr++ = (u_short) ped->ped[t] ;
			tcou++ ;
		}
#endif


		for(t=15;t<timebins+15;t++) {
			*ptr++ = (u_short) ped->ped[t] ;
			tcou++ ;
		}
		for(t=0;t<15;t++) {
			*ptr++ = (u_short) ped->ped[t] ;
			tcou++ ;
		}
		// need to be even
		if(tcou & 1) {
			*ptr++ = (u_short) ped->ped[t] ;
			tcou++ ;
		}

		*addr = (a << 24) | (ch << 16) | tcou ;

		rbuff += 2 * tcou ;	// skip stored...
	}
	}

	if(fff) fclose(fff) ;

	LOG(TERR,"Pedestals prepared for RDO %d, bytes %d",rb+1,rbuff-buff) ;
	return rbuff - buff ;	// bytes!
}

int tpxPed::to_evb(char *buff)
{
	int r, p, t ;

	u_int *addr ;
	u_short *ptr ;
	char *rbuff = buff ;

	if(!valid || !smoothed) {
		LOG(ERR,"ped::to_evb peds are bad: valid %d, smoothed %d",valid,smoothed) ;
	}

	LOG(TERR,"Preparing pedestals for later EVB...") ;
	for(r=0;r<=45;r++) {
		for(p=1;p<tpc_rowlen[r];p++) {
			struct peds *ped = get(r, p) ;

			addr = (u_int *) rbuff ;	// remember address
			ptr = (u_short *) (addr + 1) ;	// advance 4 bytes

			for(t=0;t<512;t++) {
				double rms = (ped->rms[t] * 16.0) ;
				u_short val ;

				if((u_short)rms > 0x3F) val = 0x3F ;
				else val = (u_short) rms ;

				*ptr++ = (val << 10) | (u_short)ped->ped[t] ;
			}

			*addr = (r << 24) | (p << 16) | t ;
			rbuff = (char *) ptr ;
		}
	}

	LOG(TERR,"Pedestals prepared for later EVB.") ;
	return (rbuff-buff) ;
}

int tpxPed::from_cache(char *fname) 
{
	FILE *f ;
	char *fn ;

	init() ;	// to clear

	// trivial load from disk...
	if(fname) {
		fn = fname ;
		f = fopen(fname,"r") ;
	}
	else {
		fn = "/RTScache/pedestals.txt" ;
		f = fopen(fn,"r") ;
	}

	if(f==0) {
		LOG(ERR,"ped::from_cache can't open output file \"%s\" [%s]",fn,strerror(errno)) ;
		return -1 ;
	}


	LOG(TERR,"Loading pedestals from cache \"%s\"...",fn) ;
	while(!feof(f)) {
		int r, p , t ;
		float pp, rr ;

		int ret = fscanf(f,"%d %d %d %f %f",&r,&p,&t,&pp,&rr) ;
		if(ret != 5) continue ;

		struct peds *peds = get(r,p) ;

		//if((r==12) && (p==158) && (t==0)) LOG(TERR,"peds row %d, pad %d: %f %f",r,p,pp,rr) ;

		peds->ped[t] = pp ;
		peds->rms[t] = rr ;
	}

	fclose(f) ;
	LOG(TERR,"Pedestals loaded.") ;
	smoothed = 0 ;
	valid = 1 ;

	return 1 ;
}

int tpxPed::to_cache(char *fname)
{
	FILE *f, *f_sum ;
	int r, p, t ;
	char *fn ;
	char f_sum_name[128] ;

	if(!valid || smoothed) {
		LOG(ERR,"ped::to_cache peds are bad: valid %d, smoothed %d",valid,smoothed) ;
	}

	if(fname) {
		fn = fname ;
		f = fopen(fname,"w") ;
	}
	else {
		fn = "/RTScache/pedestals.txt" ;
		f = fopen(fn,"w") ;
	}

	if(f==0) {
		LOG(ERR,"ped::to_cache can't open output file \"%s\" [%s]",fn,strerror(errno)) ;
		return -1 ;
	}

	sprintf(f_sum_name,"/RTScache/ped_sum_%u.txt",(u_int)time(NULL)) ;

	f_sum = fopen(f_sum_name,"w") ;
	if(f_sum==0) {
		LOG(ERR,"ped::to_cache can't open trace file \"%s\" [%s]",f_sum_name,strerror(errno)) ;
		return -1 ;
	}


	LOG(TERR,"Writing pedestals to cache \"%s\"...",fn) ;
	for(r=0;r<=45;r++) {

		// ONLY from 1 to rowlen!
		for(p=1;p<=tpc_rowlen[r];p++) {
			struct peds *peds = get(r, p) ;

			double sum = 0.0 ;
			int cou = 0 ;

			for(t=0;t<22;t++) {
				sum += peds->ped[t] ;
				cou++ ;
			}

			fprintf(f_sum,"%2d %3d %9.4f\n",r,p,sum/(double)cou) ;

			for(t=0;t<512;t++) {	
			
				//if((r==12) && (p==158) && (t==0)) LOG(TERR,"peds row %d, pad %d: %f %f",r,p,peds->ped[t],peds->rms[t]) ;
				fprintf(f,"%2d %3d %3d %8.3f %.3f\n",r,p,t,peds->ped[t],peds->rms[t]) ;
			}
		}
	}

	fclose(f) ;	
	fclose(f_sum) ;
	LOG(TERR,"Pedestals written.") ;
	return 1 ;
}



void tpxPed::smooth()
{
	int r, p, t ;

	double mean ;
	int cou ;

	if(smoothed) {
		LOG(ERR,"ped::smooth already done!") ;
		return ;
	}
#define TPX_GG_START	22
#define TPX_GG_STOP	51	// was 32 before Feb 20, 2008!
	LOG(TERR,"Smoothing pedestals...") ;
	for(r=0;r<=45;r++) {
	for(p=0;p<183;p++) {
		struct peds *ped = get(r,p) ;


#ifdef TPX_PED_CONSTANT
		mean = 0.0 ;
		cou = 0 ;

		for(t=0;t<30;t++) {
			mean += ped->ped[t] ;
			cou++ ;
		}

		mean /= (double)cou ;

		// we need to round off correctly
		for(t=0;t<512;t++) ped->ped[t] = (float) ((u_short) (mean+0.5)) ;	

#else		// for GG


		/******  time before GG (tb<22) -- flat! */

		mean = 0.0 ;
		cou = 0 ;

		for(t=0;t<TPX_GG_START;t++) {	// before GG
			mean += ped->ped[t] ;
			cou++ ;
		}

		mean /= (double)cou ;

		// we need to round off correctly
		for(t=0;t<TPX_GG_START;t++) ped->ped[t] = (double) ((u_short) (mean+0.5)) ;	


		/****** during GG [22-32] ****************/
		for(t=TPX_GG_START;t<TPX_GG_STOP;t++) {
			ped->ped[t] = (double) ((u_short) (ped->ped[t]+0.5)) ;

		}

		/****** after GG [33...] needs smoothing */
		for(t=TPX_GG_STOP;t<512;t++) {
			mean = 0.0 ;
			cou = 0 ;
			for(int j=0;j<8;j++) {
				if(((t+j)<512) && (ped->ped[t+j]<1000.0)) {
					mean += ped->ped[t+j] ;
					cou++ ;
				}
			}

			if(cou) {
				mean /= (double) cou ;
			}
			else {
				mean = ped->ped[t] ;
			}

			ped->ped[t] = (double) ((u_short) (mean+0.5)) ;
		}

#endif

	}
	}

	LOG(TERR,"Pedestals smoothed.") ;
	smoothed = 1 ;

	return ;
}


void tpxPed::kill_bad(int row, int pad)
{
	struct peds *p ;
	int t ;

	p = get(row, pad) ;

	for(t=0;t<512;t++) {
		p->ped[t] = 1023.0 ;
		p->rms[t] = 9.999 ;
	}

	return ;
}

int tpxPed::from_evb(char *buff, int bytes) 
{
	init() ;	// zap struct
	valid = 0 ;
	smoothed = 0 ;



	return 0 ;
}

int tpxPed::summarize(FILE *log)
{
	int notes = 0 ;


	return notes ;
}
