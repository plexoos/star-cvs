/*
	Version 5.00	07/11/2003	Many small fixes
	Version 4.31	11/19/2002	Still fixing on I960s...
	Version 4.30	11/13/2002	Re-implemented Gain & T0
					They are now mandated!
	Version 4.20	08/08/2002	Gain correction explicitly included
			Version loaded into /RTS/src/FCF on 08/09/02

	Version 4.10	05/31/2002	pad-to-pad T0 corrections added
	Version 4.01	05/31/2002	Small bug fixes..
	Version 4.00	05/20/2002	50 Hz - really good!
	Version 3.00	01/15/2002	Major revamp
	Version 3.10	01/16/2002	Revamped major internal strucutres
	Version 3.12	01/17/2002	Minor tweaks and redefinitions...
	Version 3.13	02/27/2002	Added maxClusters protection
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdarg.h>


#define FCF_VERSION	"5.11"	

#define FCF_EXTENTS


// need to define "ROOT" for use in offline
#ifdef __ROOT__


#define FCF_10BIT_ADC	// ADC data is already in 10 bits!
#define FCF_DONT_USE_LOG

#endif



//#define FCF_TEST



// use for systems without the RTS Logging facility!
#ifdef FCF_DONT_USE_LOG
#define LOG(x1,x2,x3,x4,x5,x6,x7)
#else
#include <rtsLog.h>
#endif

//static u_int delta[20] ;

#include <rts.h>
#include <rtsSystems.h>
#include <fcfClass.hh>



#if __GNUC__ == 2 && __GNUC_MINOR__ < 8
#pragma align 1
#pragma pack 1
#endif

struct fcfResx {	// 5 words or 7 if EXTENTS are defined...
	int t ;	// this may occasionally be negative!
	u_int charge ;
	u_int scharge ;

	u_short flags ;		// short
	short mean ;		// short

	u_int pad ;
#ifdef FCF_EXTENTS
	u_short	 t1, t2, p1, p2 ;	
#endif

#ifdef FCF_TEST
	// new - test only...
	u_int pix ;		// unused?
	u_int adc_max ;
	u_int id ;
#endif

} ;

#if __GNUC__ == 2 && __GNUC_MINOR__ < 8
#pragma pack 0
#pragma align 0
#endif

#ifdef FCF_TEST
void mergeTest(struct fcfResx *nw, struct fcfResx *old);

struct testADC {
	u_short adc ;
	u_short id ;
} testADC[256][512] ;

#endif

#ifdef __unix

#define FCF_MAX_RES_COU_FAST	8

#define preburst4(x)	// meaningless for UNIX

#define FCF_960_R8
#define FCF_960_R9
#define FCF_960_R10
#define FCF_960_R11
#define FCF_960_R13


extern __inline volatile void mstore(struct fcfResx *rr, int av, int ch, u_int mean, u_int flags)
{

	rr->t = av ;
	rr->charge = rr->scharge = ch ;

	rr->flags = flags ;
	rr->mean = mean ;

	return ;
}


#else	// I960

#include "mzGlobal.h"
#include "mzInlines.h"
#include "mzFastTimer.h"


#define FCF_MAX_RES_COU_FAST	((sizeof(fastMem->fcfStore)/sizeof(struct fcfResx))/2)

#define FCF_960_R8	asm ("r8") 
#define FCF_960_R9	asm ("r9") 
#define FCF_960_R10	asm ("r10") 
#define FCF_960_R11	asm ("r11") 
#define FCF_960_R13	asm ("r13") 

extern __inline volatile void mstore(struct fcfResx *rr, int av, int ch, u_int mean, u_int flags)
{

	rr->t = av ;
	rr->charge = rr->scharge = ch ;

	rr->flags = flags ;
	rr->mean = mean ;

	return ;
}

#ifdef TONKO_OPTIMIZATION
extern __inline volatile void mstore(struct fcfResx *r, int av, int ch, u_int mean, u_int flags)
{
	__asm__ volatile (" \
	\n \
	shlo 16, r11, r11 \n \
	lda (r9), r10 \n \
	or r11, %5, r11 \n \
	stq r8, (%0) \n \
	" \
	: "r=" (r) : "0" (r), "r" (av), "r" (ch), "r" (mean), "r" (flags) : "r10") ;

	return ;
}
#endif

#endif	// UNIX



	
	
// Returns the total length of words (4bytes) used in outres
// including the row and cluster count.
// Since the row and cluster count makes 2 words the routine returns 
// 2 in case of no clusters found!

int fcfClass::finder(u_char *adcin, u_short *cppin, u_int *outres)
{
	int i, j, pad ;
	int next_pad ;
	u_int start_flags ;

	chargeMinCorrect = chargeMin * FCF_GAIN_FACTOR ;

	int new_res_ix, new_res_cou, old_res_cou ;


	u_int *cl_found_pointer, *row_pointer ;


	struct fcfResx **new_res, **old_res ;


	row_pointer = outres++ ;	// mark the row
	cl_found_pointer = outres++ ;	// mark the count

	*cl_found_pointer = 0 ;
	*row_pointer = row ;


	new_res_ix = 0 ;
	new_res_cou = old_res_cou = 0 ;


	new_res = old_res = NULL ;
	next_pad = 0 ;

//	memset(delta,0,sizeof(delta)) ;




#ifdef FCF_TEST
	memset(testADC,0,sizeof(testADC)) ;
	int globalId = 1 ;
#endif

	for(pad=padStart;pad<=padStop;pad++) {
		u_int GC ;	// gain correction
		int  T0C ;	// T0 correction

		// gain and t0 corrections!
		GC = gainCorr[pad] ;
		T0C = t0Corr[pad] ;

		if(startFlags) {
			start_flags = startFlags[pad] ;
		}
		else {
			start_flags = 0 ;
		}

		if(GC == 0) {
			//LOG(WARN,"Gain Correction 0 on row %d, pad %d - skipping!",row,pad,0,0,0) ;
			continue ;
		}

		//LOG(WARN,"row %d, pad %d, GC %d, T0C %d",row,pad,GC,T0C,0) ;

//		u_int mark = mzFastTimerMark() ;


#ifdef __unix
		static u_int cppStore[32] ;
		u_int *ptrs = cppStore ;
#else
		u_int *ptrs = fastMem->cppStore ;
#endif
		register u_int *ptrs_r = ptrs ;
		register u_int *cpp_r = (u_int *)((u_int)cppin + cppOff[pad]) ;
		register u_int fe00  = 0xFE00FE00 ;

		u_int *ptrs_end = ptrs_r + 31 ;


		//LOG(DBG,"cppin 0x%X, cpp_off[] 0x%X, cpp_r 0x%X, cpp_off 0x%X",(u_int)cppin,cpp_off[pad],(u_int)cpp_r,(u_int)cpp_off,0) ;

		while(ptrs_r < ptrs_end) {
			//asm("#tada") ;


			register unsigned int first FCF_960_R8 ;
			register unsigned int second FCF_960_R9 ;
			register unsigned int third FCF_960_R10 ;
			register unsigned int fourth FCF_960_R11 ;

			preburst4(cpp_r) ;	// this guy puts the results in r8 and r9!!!
#ifdef __unix
			first = *cpp_r ;
			second = *(cpp_r+1) ;
			third = *(cpp_r+2) ;
			fourth = *(cpp_r+3) ;
			//LOG(DBG,"%X %X %X %X",first,second,third,fourth,0) ;
#endif
			if(first & fe00) goto go_out ;
			*ptrs_r++ = first ;

			if(second & fe00) goto go_out ;
			*ptrs_r++ = second ;

			if(third & fe00) goto go_out ;
			*ptrs_r++ = third ;

			if(fourth & fe00) goto go_out ;
			*ptrs_r++ = fourth ;

			cpp_r += 4 ;
		}

		// NOTICE! go-to label!
		go_out : ;

		u_int cou_ptrs = (ptrs_r - ptrs) ;

		//LOG(DBG,"Running pad %d with %d clusters, pad %d had %d protoclusters...",pad,cou_ptrs,next_pad-1,new_res_cou,0) ;

		if(cou_ptrs == 0) {
			continue ;	// continue with the next pad...
		}



#ifdef FCF_10BIT_ADC
		u_short *val = (u_short *)((u_int)adcin + adcOff[pad]) ;
#else
		u_char *val = (u_char *)((u_int)adcin + adcOff[pad]) ;
#endif



		if((next_pad != pad) && new_res_cou) {	// we skipped a pad and there
							// was some stuff remaining!
							// so we have to flush it out
			int wrote ;

			// check for space!
			if((*cl_found_pointer + new_res_cou) >= maxClusters) {
				LOG(TERR,"Too many clusters in pad %d - breaking!",pad,0,0,0,0) ;
				return outres - row_pointer ;
			}

			//LOG(WARN,"Pad skipped: this %d, expect %d, res count %d",pad,next_pad,new_res_cou,0,0) ;

			// could be less than requested due to cuts...
			wrote = saveRes(new_res, new_res_cou, outres) ;
			outres += wrote*2 ;	// occupies 8 bytes (2 words)
			*cl_found_pointer += wrote ;

			// since we skipped a pad and stored the old results we must zap the old results, V4.01
			new_res_cou = 0 ;
		}


		// swap the banks...
		if(new_res_ix == 1) {
			new_res_ix = 0 ;

			old_res = resx[1] ;
			new_res = resx[0] ;
			
		}
		else {
			new_res_ix = 1 ;

			old_res = resx[0] ;
			new_res = resx[1] ;

		}

		old_res_cou = new_res_cou ;	// the last guys are the old guys now...
		new_res_cou = 0 ;		// start from scratch for the expected guys


#ifdef OLD_CODE
		// assign flags
		start_flags = 0 ;
	
		if((pad == 1) || (pad == padMax)) start_flags = FCF_ROW_EDGE ;
		else if((pad == padStart) || (pad == padStop)) start_flags = FCF_BROKEN_EDGE ;
		// super special cases - 2 rows like this
		if(padStart == padStop) start_flags = FCF_ROW_EDGE | FCF_BROKEN_EDGE ;
#endif



//		delta[0] = mzFastTimerDelta(mark) ;


		u_short *ptrs_16 = (u_short *)ptrs ;
		u_int cl_counter ;

		for(cl_counter=0;cl_counter<cou_ptrs;cl_counter++) {
			register u_int start, stop ;

			start = (u_int)*ptrs_16++ ;
			stop = (u_int) *ptrs_16++ ;

			// paranoia
			// don;t need this anymore
			//stop &= 0x1FF ;

			//LOG(DBG,"Cluster %d: start %d, stop %d",cl_counter,start,stop,0,0) ;

			// ignore certain ranges
			// Tonko, 04/11/03 modified
			if(start < timebinLo) start = timebinLo ;
			if(stop > timebinHi) stop = timebinHi ;
			if(stop < start) continue ;


			//if((stop < timebinLo) || (start >= timebinHi)) {
			//	//LOG(WARN,"Ignoring cluster start %d, stop %d",start,stop,0,0,0) ;
			//	continue ;
			//}



			//asm("#first") ;
#ifdef FCF_10BIT_ADC
			register u_short *adc_p = val + start ;
			register u_short *adc_end = val + stop + 1;
#else
			register u_char *adc_p = val + start ;
			register u_char *adc_end = val + stop + 1;
#endif

			register int flags = FCF_ONEPAD | start_flags ; 
			register int adc = *adc_p++ ;

			// move these to registers....
			register int min_adc = minAdcT ;
#ifndef FCF_10BIT_ADC
			register u_short *adc8to10 FCF_960_R13 = a8to10 ;
#endif

			//LOG(WARN,"Pad %d, cl %d, len %d",pad,cl_counter,stop-start+1,0,0) ;

			//delta[1] = mzFastTimerDelta(mark) ;
			do {
				register int last_falling = 0 ;
				register int max_a = 0 ;
				register int mean FCF_960_R11  = 0 ;
				register u_int charge FCF_960_R9  = 0 ;
				register int av FCF_960_R8 = 0 ;

				register int last_a = 0 ;
#ifdef FCF_EXTENTS
				stop = start ;	// take this out when running!
#endif
				//delta[1] = mzFastTimerDelta(mark) ;
				//asm("#second") ;
				do {

					register int a ;

					if(unlikely(last_falling)) {
						if(unlikely(adc > (last_a + min_adc))) {	// mostly false
							flags |= FCF_DOUBLE_T ;
							//LOG(WARN,"T Split before adc %d, at %d...",adc,start,0,0,0) ;
							break ;
						}
					}
					else {
						if(unlikely(adc < (last_a - min_adc))) {	// mostly false
							last_falling = 1 ;	// Im falling
						}						
						else {
							if(unlikely(adc >= max_a)) {	// find the maximum and keep it...
								max_a = adc ;
								mean = start ;	// this is not the 

							}

						}
					}

#ifdef FCF_10BIT_ADC 
					// no lookup - already in 10 bits!
					a = adc ;
#else
					a = adc8to10[adc] ;  // transfer to 10 bits
#endif

					last_a = adc ;


#ifdef FCF_TEST
					testADC[pad][start].adc = adc ;
					testADC[pad][start].id = globalId ;
#endif


					av += start * a ;

					charge += a ;

					//LOG(WARN,"Pad %d, start %d, adc %d, charge %d",pad,start,adc,charge,0) ;

					adc = *adc_p++ ;	// advance here!

					//LOG(DBG,"... adc_p 0x%08X (end 0x%08X)",(u_int)adc_p,(u_int)adc_end,0,0,0) ;

					start++ ;

				} while(likely(adc_p <= adc_end)) ;
				//asm("#third") ;



				// t0Corr is (gain*t0*64)!
				// This value may go negative here due to the t0 corrections!!!
				// Time sum is now 64 times larger!!!
				av = GC * av + T0C * charge ;	



				charge *= GC ;	// charge is now 64 times larger!


#ifdef __unix
				if(charge > 0x7FFFFFFF) {
					LOG(TERR,"Whoa charge 0x%08X, %d GC",charge,GC,0,0,0) ;
				}
#endif
				//delta[2] += mzFastTimerDelta(mark) -delta[1];

				// get the next storage location
				struct fcfResx *rn = new_res[new_res_cou++];


				rn->pad = charge*pad ;	// this may be 64 times larger if gain correction is employed!

				// store the new results via some assembly inline...
				mstore(rn,av,charge,mean,flags) ;

#ifdef FCF_EXTENTS
				rn->p1 = pad ;
				rn->p2 = pad ;
				rn->t1 = stop ;		// this is actually the first...
				rn->t2 = start-1 ;	// this is actually the last...

#endif
				//LOG(WARN,"Pad %d, stored max at %d, t1 %d, t2 %d",pad,mean,rn->t1,rn->t2,0) ;
				
#ifdef FCF_TEST
				rn->pix = start-stop ;
				rn->adc_max = adc8to10[max_a] ;
				rn->id = globalId ;
				globalId++ ;
#endif				

				//if(((u_int)adc_p & 0xF)==0) preburst4(adc_p) ;

			} while(likely(adc_p <= adc_end)) ;

			//delta[2] += mzFastTimerDelta(mark) -delta[1];
			//asm("#fourth") ;

		}// end of the pad or cl_counter


		//delta[3] += mzFastTimerDelta(mark) - delta[0];

		
		if(new_res_cou) {	// some results
			next_pad = pad + 1 ;	// we expect this pad as the next one!
		}

		

		//LOG(WARN,"Pad %d: cl %d, res %d",pad,cou_ptrs,new_res_cou,0,0) ;


		//asm("#double") ;
		int new_start = 0 ;

		for(i=0;likely(i<old_res_cou);i++) {	// loop over old sequneces
			register struct fcfResx *rr ;
			register int start ;

			rr = old_res[i] ;

			register int merged = 0 ;

			register int old_mean = rr->mean ;
			register int old_mean_m = old_mean - param1 ;
			register int old_mean_p = old_mean + param1 ;

			register int min_adc = minAdcPad ;

			register u_int old_scharge = rr->scharge ;
			register u_int old_flags = rr->flags ;

			start = new_start ;


			for(j=start;likely(j<new_res_cou);j++) {	// loop over new sequences
				register struct fcfResx *nresx ;
				register int mean ;

				nresx = new_res[j] ;

				mean = nresx->mean ;
			
				if(mean < old_mean_m) {
					new_start = j + 1 ;
					continue ;
				}
				else if(mean <= old_mean_p) {	
					register u_int charge ;

					charge = nresx->charge ;


					if(old_flags & FCF_FALLING) {

						if(charge > (old_scharge + min_adc)) {
							register u_int sc_tmp, sc_p_tmp ;



							sc_tmp = old_scharge / 2 ;
							sc_p_tmp = sc_tmp * (pad-1) ;

							nresx->flags |= FCF_DOUBLE_PAD ;	// new one
								
							// the new one will get half of previous share

							nresx->charge += sc_tmp ;
							nresx->pad += sc_p_tmp ;

							nresx->t += (mean * sc_tmp) ;


							// the old guy gets half of the previous less
//#ifdef __unix
							if(unlikely(sc_tmp > rr->charge)) {
								LOG(WARN,"oops - going negative 0x%08X - 0x%08X",rr->charge,sc_tmp,0,0,0) ;
							}
//#endif

							rr->charge -= sc_tmp ;
							rr->pad -= sc_p_tmp ;

							// watch it! rr->t may go negative!!!
							rr->t -= old_mean * sc_tmp ;

							rr->flags = old_flags | FCF_DOUBLE_PAD ;	// old one
								
#ifdef FCF_EXTENTS
							nresx->p1 = pad - 1 ;
							rr->p2 = pad ;
#endif
							new_start = j+1 ;

							break ;	// and create a new one; break out of old results scan

						}
						// maintain "falling" character
						nresx->flags |= FCF_FALLING ;
					}
					else {
						// need to go signed for a moment...
						if((int)charge < ((int)old_scharge - min_adc)) {
							//llog("MARK!") ;
							nresx->flags |= FCF_FALLING ;
						}
					}


					merged = 1 ;

					nresx->flags |= old_flags ;
					nresx->flags &= (~FCF_ONEPAD) ;  // ...clear the one pad flag


					nresx->scharge = charge ;	// last pad charge
					nresx->charge += rr->charge ;
					nresx->pad += rr->pad ;
					nresx->t += rr->t ;
					// mean stays the same????
					// I'm not sure this is good!

#ifdef FCF_EXTENTS
//					nresx->p2 = pad ;	// the new guy had the larger pad
					nresx->p1 = rr->p1 ;	// the old guy has the smaller pad
					if(rr->t1 < nresx->t1) nresx->t1 = rr->t1 ;
					if(rr->t2 > nresx->t2) nresx->t2 = rr->t2 ; 
#endif

#ifdef FCF_TEST
					nresx->pix += rr->pix ;
					if(rr->adc_max > nresx->adc_max) nresx->adc_max = rr->adc_max ;
					//mergeTest(nresx, rr) ;
#endif
					new_start = j + 1 ;

					break ;
				}
				else {
					new_start = j ;

					break ;
				}
			
			}


			if(!merged) {	// didn't merge - must store!
				int wrote ;

				// check for space!
				if((*cl_found_pointer + 1) >= maxClusters) {
					LOG(TERR,"Too many clusters in pad %d - breaking!",pad,0,0,0,0) ;
					return outres - row_pointer ;
				}

				//LOG(DBG,"Mean diff old-new %d [%d], new res cou %d",old_mean-new_res[j]->mean,reason,new_res_cou-j,0,0) ;


				// could be less than requested due to cuts...
				wrote = saveRes(&rr, 1, outres) ;
				outres += wrote*2 ;	// occupies 8 bytes
				*cl_found_pointer += wrote ;
			}

		}
		//asm("#triple") ;

		//delta[4] += mzFastTimerDelta(mark) -delta[3];
	}// for minpad to maxpad
	
	// check for leftover in the last pass
	if(new_res_cou) {
		int wrote ;
		// check for space!
		if((*cl_found_pointer + new_res_cou) >= maxClusters) {
			LOG(TERR,"Too many clusters in pad %d - breaking!",pad,0,0,0,0) ;
			return outres - row_pointer ;
		}


		// could be less than requested due to cuts...
		wrote = saveRes(new_res, new_res_cou, outres) ;
		outres += wrote*2 ;	// occupies 8 bytes
		*cl_found_pointer += wrote ;
	}


	//LOG(WARN,"One_pad %d, pad decon %d, seq_total %d, low_adc %d, delta %d",one_pad,pad_decon,seq_total,adc_low,delta[2]) ;

	//LOG(WARN,"Delta2 %d, delta3 %d, delta4 %d",delta[2],delta[3],delta[4],0,0) ;

	return (outres - row_pointer) ;	// length in qwords

} 


fcfClass::fcfClass(int det, u_short *table) 
{
	detector = det ;

#ifdef __unix
	a8to10 = adc8to10_storage ;

#else	// I960
	a8to10 = fastMem->adc8to10 ;
#endif


	if(table == NULL) {
		noADCconversion = 1 ;
	}
	else {
		noADCconversion = 0 ;
	}
		

	svtPedestal = 0 ;


	deconTime = 1 ;
	deconPad = 1 ;
	doCuts = 1 ;

	param1 = FCF_PARAM1 ;
	minAdcT = FCF_MIN_ADC_T ;
	minAdcPad = FCF_MIN_ADC_PAD ;

	switch(det) {
	case TPC_ID :
		maxCPP = 31 ;
		maxTimebin = 511 ;
		timebinLo = 0 ;
		timebinHi = 400 ;	// should be 400 normally
		chargeMin = 40 ;	// reasonable estimate was 40
		break ;
	case FTP_ID :
		maxCPP = 31 ;
		maxTimebin = 511 ;
		timebinLo = 0 ;
		timebinHi = 255 ;
		chargeMin = 40 ;
		break ;
	case SVT_ID :
		maxCPP = 8 ;
		maxTimebin = 127 ;
		timebinLo = 0 ;
		timebinHi = 127 ;
		chargeMin = 20 ;
		break ;
	default :
		LOG(WARN,"Cluster Finder can't work with DET Type %d",det,0,0,0,0) ;
		return ;
	}

	// take into account the multipication due to
	// gain being used
	minAdcPad *= FCF_GAIN_FACTOR ;

	static struct fcfResx res_slow[2][512] ;
#ifdef __unix
	static struct fcfResx res_fast_ux[2][FCF_MAX_RES_COU_FAST] ;
	struct fcfResx *res_fast[2] = { res_fast_ux[0], res_fast_ux[1] } ;
#else

#define RES_COU	(sizeof(struct fcfResx)*2*FCF_MAX_RES_COU_FAST)
#define I960_MAX_RES (sizeof(fastMem->fcfStore))

	if(RES_COU > I960_MAX_RES) {
		LOG(CRIT,"Error in MAX_RES_COU size %d > %d!!!",RES_COU,I960_MAX_RES,0,0,0) ;
	}

	// hand adjusted!!! Beware of changes!!!
	struct fcfResx *res_fast[2] = {
		(struct fcfResx *) fastMem->fcfStore,
		(struct fcfResx *) &(fastMem->fcfStore[(sizeof(struct fcfResx)/4)*FCF_MAX_RES_COU_FAST]) } ;
		

	//LOG(WARN,"size %d, count %d at %u %u (max %d bytes)",sizeof(struct fcfResx),FCF_MAX_RES_COU_FAST,(UINT32)res_fast[0],(UINT32)res_fast[1],I960_MAX_RES) ;

#endif


	u_int i, j ;
	for(i=0;i<2;i++) {
		for(j=0;j<FCF_MAX_RES_COU_FAST;j++) {
			resx[i][j] = res_fast[i] + j ;
			//LOG(WARN,"B %d:%d = 0x%08X",i,j,(u_int)resx[i][j],0,0) ;
		}
	}
	for(i=0;i<2;i++) {
		for(j=FCF_MAX_RES_COU_FAST;j<512;j++) {
			resx[i][j] = &(res_slow[i][j]) ;
		}
	}

	return ;
}


// copy over the ADC conversion table...
void fcfClass::set8to10(u_short *table)
{
	int i ;

	if(table) {
		for(i=0;i<256;i++) {
			a8to10[i] = *table++ ;
		}
		noADCconversion = 0 ;
	}
	else {
		noADCconversion = 1 ;
	}
	return ;
}

// returns the number of clusters saved
inline int fcfClass::saveRes(struct fcfResx *res_p[], int cou, u_int *output)
{
	//asm("#saved") ;
	int i ;
	int saved = 0 ;
	register u_int ch_min = chargeMinCorrect ;
	register int do_cuts = doCuts ;
	u_int fla, cha ;
	u_int pad_c ;
	u_int time_c ;
	u_int t_cha ;

	for(i=0;i<cou;i++) {
		struct fcfResx *rr ;

		rr = res_p[i] ;


		fla = rr->flags ;
		cha = rr->charge ;


		// skip single pad hits
		// as well as those with too little charge
		//LOG(NOTE,"saveRes: flags 0x%X, charge %d",fla,cha,0,0,0) ;

		if(do_cuts) {
			if(fla & FCF_BROKEN_EDGE) {	// always pass!
				;
			}
			else if(fla & (FCF_ROW_EDGE | FCF_DEAD_EDGE | FCF_ONEPAD)) continue ;	// kill!
			else if(cha <= ch_min) continue ;	// kill!
			else if(rr->t1 == 0) continue ;		// kill if they touch timebin 0
			else if((rr->t2-rr->t1) <= 3) continue ;
		}

//#ifdef __unix
		if((cha > 0x7FFFFFFF)) {
			LOG(TERR,"Bad charge: pad 0x%08X, time 0x%08X, charge 0x%08X, flags 0x%04X",rr->pad,rr->t,rr->charge,fla,0) ;
			continue ;
		}
//#endif

		pad_c = rr->pad ;
		time_c = rr->t ;

		if(pad_c > 0x04000000) {
			t_cha = cha >> 6 ;

			if(t_cha) {
				pad_c = (pad_c + t_cha/2) / t_cha  ;
			}
			else {
				pad_c = ((pad_c + cha/2) / cha) << 6 ;
			}

//			LOG(WARN,"Precision lost: orig pad %u, orig cha %u, t_cha %u, pad_c %u",rr->pad,cha,t_cha,pad_c,0) ;
		}
		else {
			// increased resolution
			pad_c = ((pad_c << 6) + (cha/2)) / cha ;
		}

		if(time_c > 0x04000000) {
			t_cha = cha >> 6 ;

			if(t_cha) {
				time_c = (time_c + t_cha/2) / t_cha ;
			}
			else {
				time_c = ((time_c + cha/2) / cha) << 6 ;
			}
		}
		else {
			// increase resoultion
			time_c = ((time_c << 6) + (cha/2)) / cha ;	
		}


		// do the integerized rounded division
		cha = (cha+32)/64 ;



		if((pad_c > 0xFFFF) || (time_c > 0xFFFF)) {
			//LOG(WARN,"Overload pad 0x%08X, time 0x%08X, charge 0x%08X, flags 0x%04X - discarding ",pad_c,time_c,cha,fla,0) ;
			continue ;
		}

		if(cha > 0xFFFF) {
//#ifdef __unix
			//LOG(WARN,"Charge too big 0x%08X - setting to 0xffff (pad 0x%08X, time 0x%08X)...",cha,rr->pad,rr->t,0,0) ;
//#endif
			cha = 0xFFFF ;
			fla |= FCF_DOUBLE_T | FCF_DOUBLE_PAD ;	// mark it somehow!
		}

		// clear the FALLING flag 
		//fla &= (~FCF_FALLING) ;


		// thse bits should never be set!!! - will use them for flags!
		if((pad_c & 0xc000) || (time_c & 0x8000)) {
			LOG(TERR,"Strange pad 0x%04X, time 0x%04X...",pad_c,time_c,0,0,0) ;
			continue ;
		}

#ifdef FCF_EXTENTS
		u_int p = (pad_c >> 6)  ;
		u_int fl ;
		
		u_int p1 = p - rr->p1 ;
		u_int p2 = rr->p2 - p ;

		if(p1 > 7) p1 = 7 ;
		if(p2 > 7) p2 = 7 ;


//		if((p1 == 15) || (p2 ==15)) {
//			fprintf(stderr,"--- %d %d %d %d %d - 0x%04X - ",rr->pad,rr->charge,p,rr->p1,rr->p2,fla) ;
//			fprintf(stderr,"%f\n",(double)pad_c/64.0) ;
//		}


		fl = (p1 << 8) | (p2 << 11)  ;


		p = (time_c >> 6)  ;

		
		p1 = p - rr->t1 ;
		p2 = rr->t2 - p ;

		if(p1 > 15) p1 = 15 ;
		if(p2 > 15) p2 = 15 ;

//		if((p1 == 15) || (p2 ==15)) {
//			fprintf(stderr,"--- %d %d %d %d %d - 0x%04X - ",rr->t,rr->charge,p,rr->t1,rr->t2,fla) ;
//			fprintf(stderr,"%f\n",(double)time_c/64.0) ;
//		}

		fl |= (p2 << 4) | p1 ;


//		printf("%d %f %f %d %d %d %d %d %d %d %d %d\n",row,(double)(pad_c)/64.0-1.0,(double)time_c/64.0-0.5,
//		       cha, fla, rr->p1, rr->p2, rr->t1, rr->t2, rr->pix, rr->adc_max, rr->id) ;



		if(fla & FCF_ONEPAD) time_c |= 0x8000 ;
		if(fla & (FCF_DOUBLE_T | FCF_DOUBLE_PAD)) pad_c |= 0x8000 ;
		if(fla & FCF_DEAD_EDGE) pad_c |= 0x4000 ;

		if(fla & FCF_ROW_EDGE) fl |= 0x8000 ;
		if(fla & FCF_BROKEN_EDGE) fl |= 0x4000 ;

		fla = fl ;
#endif	


		// watchout for ordering!
		*output++ = (time_c << 16) | pad_c ;
		*output++ = (cha << 16) | fla ;

		//LOG(WARN,"time 0x%02X, pad 0x%02X, cha 0x%02X, fla 0x%02X",
		//   time_c, pad_c, cha, fla,0) ;

		saved++ ;


	}

	//LOG(DBG,"saveRes saved %d clusters...",saved,0,0,0,0) ;
	return saved ;
}

#ifdef FCF_TEST

void mergeTest(struct fcfResx *nw, struct fcfResx *old)
{
	int i, j ;

	for(i=0;i<256;i++) {
		for(j=0;j<512;j++) {
			if(testADC[i][j].id == old->id) {
				testADC[i][j].id = nw->id ;
			}
		}
	}

	return ;
}
#endif

