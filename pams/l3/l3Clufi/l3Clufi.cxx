//--------------------------------------------------------------------
//  ** FILE:       l3Clufi.cxx
//
//  ** HISTORY:  cleaned up 26.12.2000
//               written august 1999
//  ** Author: flierl@bnl.gov
//------------------------------------------------------------------

/* includings */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <Rtypes.h> /* use ROOT variables: ..._t */
#include "l3Clufi.h"
#include "trans_table.h"
#include "croat.h" 
#include "daqFormats.h"
#include "TStopwatch.h"

TStopwatch i960[18];

/* functions  */
Int_t asic_eve(UShort_t* cpp, UChar_t* adc);
Int_t croatFinder(UChar_t *adcin, UShort_t *cppin, uint* outres, int, int);

/* here we start the callable function */
long type_of_call l3Clufi_(
			   TABLE_HEAD_ST         *pixels_h,     PIXELARRAY_ST           *pixels ,
			   TABLE_HEAD_ST           *hits_h,       HITARRAY_ST             *hits )
{
  /*
    ROUTINE:    l3Clufi_
    IN:
    pixels.data = short = 10 bit value of the pixelentry
    pixels    - PLEASE FILL IN DESCRIPTION HERE
    pixels_h   - header Structure for pixels
    
    OUT:
    hits.data = long = 4byte = 1 DATAWORD in DAQ Format
    hits    - PLEASE FILL IN DESCRIPTION HERE
   hits_h   - header Structure for hits
   
  */

  //////
  // provide 2 values : 
  //                   adc         = pointer of pixelarray (in 8 bit)
  //                   receiver    = pointer to first receiver board (TPCRBCLP)
  //                   (sectornumb  = 1 or 2, the first or second sector in a supersector)
  //                   the TPCSECLP bank can be found in bank[0-34]                  
  //////
  

  //printf("\n Start clusterfinding module \n");   

  /*     Variable Definitions     */
  Int_t i   = 0;
  Int_t ret = 0;
  Int_t rb  = 0; 
  Int_t mz  = 0;
 
  Short_t* adclong = NULL ; // long means 10 bit value of pixelentry
  UInt_t* bank = NULL ;    // DATAWORDS (=4 BYTE) to be written out in DAQ-Format   
  UInt_t* receiver = NULL ;
  UInt_t* bank_limit = NULL ; // maximum address for outbank -> abort if we find too many clusters
 
  Int_t sectornumb = 0 ;
  struct TPCSECLP *seclp = NULL ;
    
  UShort_t* cpp = NULL;
  UChar_t* adc = NULL;     // 8 bit value of pixelentry
  
  Int_t bank_overflow = 0;

  //
  // get the pixeldata only for one sector
  // pixel.data = short = 10 bit value of pixelentry
  adclong = (Short_t*) &pixels->data;
   
  
  // check pixel data
  //printf("adc :%d  pixels:%d\n",adclong,&pixels->data);
  //for(i=780850;i<1000000;i++)
  //  {
  //	   if (pixels[i].data!=0)
  //       {
  //	   int row,pad,time;
  //	   row = (int) (i/(63700))+1;
  //             pad = (int) ((i - (row-1)*182*350)/(350))+1;
  //	   time =(int) (i - (row-1)*182*350 - (pad-1)*350)+1;
  // printf("adc [%d] = %d  row:%d   pad:%d  ",i,adc[i],row,pad);
  // printf("time:%d   pix:%d\n",time,pixels[i].data);
  //       }
  // }
  

    
  // 10 to 8 bit conversion (croat.c expects packed data)
  adc = (UChar_t*) calloc (MAX_T*MAX_P*ABS_ROWS,sizeof(*adc));
  for ( i = 0; i < MAX_T*MAX_P*ABS_ROWS; i++)
    {
      adc[i]= log10to8_table[adclong[i]];
    }

  // get pointer for outgoing bank
  bank = (UInt_t*) &hits->data;

  // get limit (=maximum space in buffer -10% = 90.000 datawords per supersector) for outgoing bank
  Int_t buffsize = Int_t (0.9 * (hits_h->maxlen)) ;
  bank_limit = (UInt_t*) &(hits[buffsize]) ;

  /* check outgoing data */
  /*printf("bank :%d  hits:%d\n",bank,&hits->data);*/
  /*printf("offset : %d\n",bank[34]);*/
    
  /* check if it's the first or second sector in a supersector*/
  /* pretty weak inquiry, i know :)*/
  if ( bank[2] != 34 ) 
    {
      sectornumb = 1; 
    }
  else 
    { 
      sectornumb = 2; 
    }

  /* set pointer to first receiver board */
  seclp =(TPCSECLP *) &bank[0];
  if (sectornumb == 1) 
    {
      /* fill TPCSECLP header */
      strncpy (seclp->bh.bank_type, CHAR_TPCSECLP, 8);
      seclp->bh.length     = 34; 
      /*seclp->bh.bank_id    = 77; must be set in the maker !! must be 1,3..,23 */
      seclp->bh.format_ver = DAQ_RAW_FORMAT_VERSION;
      seclp->bh.byte_order = DAQ_RAW_FORMAT_ORDER ; 
      seclp->bh.token      = 1;
      seclp->bh.w9         = DAQ_RAW_FORMAT_WORD9;
      seclp->bh.crc        = 0;
      /* set address of first receiver bank behind seclp */
      receiver = &bank[34]; 
    }
  else if (sectornumb == 2)
    {
      /* calculate offset from sector 1  */
      int offset = 0;
      for (i = 10 ; i <33;i=i+2)
	{
	  if( bank[i]!=0 ) { offset = bank[i]+bank[i+1]+1; };
	}
      if (offset==0)
	{
	  printf("problems with supersector ...\n");
	  return 0;
	}
      /*printf("offset :%d \n",offset);  */
      receiver = bank+offset;
    }
    
  /*
    //printf("receiver: %d\n",receiver);
    // check it the other way means fill it:
    //for(i=0;i<1;i++)
    //	{
    //bank[i+bankoffset] = 55;
    //    printf("bank [%d] = %d  hits[%d].data = %d\n",i,bank[i],i,hits[i].data);
    //	}
  */

    
  // prepare cpp array
  cpp = (UShort_t*) malloc(ABS_ROWS*MAX_P*MAX_C*sizeof(*cpp) );
  cpp = (UShort_t*) memset((short *)cpp, 0x0000, ABS_ROWS*MAX_P*MAX_C*sizeof(*cpp));
       
  // lets simulate the asic : 
  // means fill cpp array with start and end timebucket of sequenz
  // pretty bad 0 suppresion
  asic_eve(cpp,adc);
  
  /* loop over receiver boards  */ 
  for( rb=1; (rb<=6) && (bank_overflow==0) ; rb++ ) 
    { 
      struct TPCRBCLP *rbclp;
	    
      rbclp = (TPCRBCLP *) receiver;
      /* fill TPCRBCLP bank header for this Receiver Board */
      strncpy (rbclp->bh.bank_type, CHAR_TPCRBCLP, 8);
      rbclp->bh.length     = 32;
      rbclp->bh.bank_id    = rb+(sectornumb-1)*6; /* Receiver Board no. as seen by Sector Broker (1 .. 12) */
      rbclp->bh.format_ver = DAQ_RAW_FORMAT_VERSION;
      rbclp->bh.byte_order = DAQ_RAW_FORMAT_ORDER;
      rbclp->bh.token      = 1;
      rbclp->bh.w9         = DAQ_RAW_FORMAT_WORD9;
      rbclp->bh.crc        = 0;
      /* pointer set pointer behind TPCRBCLP bank  */
      receiver += rbclp->bh.length;

      /* loop over mezzanine cards  */
      for( mz=1; mz<=3; mz++ ) 
	{
	  struct bankHeader *mzcld;
	  mzcld = (struct bankHeader *) receiver;
	  /* fill TPCRBCLP bank header for this Receiver Board  */
	  strncpy (mzcld->bank_type, CHAR_TPCMZCLD, 8);
	  mzcld->length     = 10;
	  mzcld->bank_id    = mz;
	  mzcld->format_ver = DAQ_RAW_FORMAT_VERSION;
	  mzcld->byte_order = DAQ_RAW_FORMAT_ORDER;
	  mzcld->token      = 1;
	  mzcld->w9         = DAQ_RAW_FORMAT_WORD9;
	  mzcld->crc        = 0;
	  /* pointer set pointer behind TPCMZCLD bank header */
	  receiver += mzcld->length;

	  // timing
	  Int_t i960_id  = (rb-1) * 3 + (mz-1) ;
	    
	  // check whether we have enough space to write the clusters out
	  // 3 = nrows per mz ; 2 = number words per cluster ; 1000 = max cluster per row  
	  if( (bank_limit-receiver) < 3*2*1000 )
	      {
		  bank_overflow = 1;
//VPunused	  Int_t aa = bank_limit-receiver;
		  break ;
	      }

	  i960[i960_id].Start(kFALSE) ;
	  //for (Int_t depp =0 ; depp < 1000000 ; depp++) { Double_t t = depp*depp; };
	  /* do the clusterfinding on this mz  */
	  ret = croatFinder((UChar_t*)adc,(UShort_t*) cpp, (UInt_t*) receiver, rb ,mz);
	  i960[i960_id].Stop();

	  /* set length in TPCMZCLD bank */
	  mzcld->length += ret;
	  /* set new bank pointer */
	  receiver += ret;

	  /* fill TPCRBCLP data words */ 
	  rbclp->mz[mz-1].off = (Int_t *)mzcld - (Int_t *)rbclp;
	  rbclp->mz[mz-1].len = mzcld->length;
	}
      /* fill TPCSECLP data words */
      seclp->rb[rb+(sectornumb-1)*6-1].off = (Int_t *)rbclp - (Int_t *)seclp;
      seclp->rb[rb+(sectornumb-1)*6-1].len = (Int_t *)receiver - (Int_t *)rbclp;
    }

  // free memory
  free(cpp);
  free(adc);
 
  
  if (bank_overflow) 
    {
      printf("\n\n\nStop here ... too many clusters !\n\n\n");
      return 0 ; 
    }
  

  /* done  */
  /* printf("  Finished clusterfinding module!\n"); */
  return (receiver-bank) ;
}


Int_t asic_eve(UShort_t* cpp, UChar_t* adc)
{
  /****************************************************/
  /* this routine fills the cpp array for this sector */
  /****************************************************/

  /* variable definition */
  Int_t i, j, k ;
  Int_t found_max = 64 ; /* maximum sequenzes on pad : 32 */
  Char_t val ;
  Int_t found, start_time ; 
  Int_t t_max = 512 ;   /* why not MAX_T ? */
    
  /* loop over rows on this sector */
  for (k=0; k<ABS_ROWS; k++) 
    {
      /* loop over pads in this row  */
      for (i=0; i<MAX_P; i++) 
	{
	  found = 0 ;
	  start_time = 0 ;
	  /* loop over time buckets  */
	  for (j=0; j<MAX_T; j++) 
	    {
	      /* get adc entry  */
	      val = *(adc + k*MAX_T*MAX_P + i*MAX_T + j) ;
	      /* get start bucket  */
	      if ((val > 0) && (start_time == 0)) 
		{
		  /* make it bigger than 0 to keep the inquiry "if (start_time)" reasonable */
		  start_time = j+1 ;
		}
	      else if (val==0) 
		{
		  /* if start_time != 0  */
		  if (start_time) 
		    {
		      /* start timebucket of sequence  */
		      *(cpp + k*MAX_C*MAX_P + i*MAX_C + found++) = start_time - 1 ;
		      /* end timebucket of sequence */
		      *(cpp + k*MAX_C*MAX_P + i*MAX_C + found++) = j - 1 ;
		      /* only found_max sequenzes for each pad*/
		      if (found == (found_max-2)) break ;
		      start_time = 0 ;
		    }
		}
	    }/* loop over time buckets  */

	  /* if sequenz touches upper end of time scale  */
	  if(start_time && (found != (found_max-2))) 
	    {
	      /* start time bucket */
	      *(cpp + k*MAX_C*MAX_P + i*MAX_C + found++) = start_time-1 ;
	      /* set end time bucket to max number  */
	      *(cpp + k*MAX_C*MAX_P + i*MAX_C + found++) = t_max - 1 ;
	    }

	  /* fill the rest with zeros  (= ff00)  */
	  for(j=found;j<found_max;j+=2) 
	    {
		*(cpp + k*MAX_C*MAX_P + i*MAX_C + j) = 0xff00 ;
		*(cpp + k*MAX_C*MAX_P + i*MAX_C + j + 1) = 0xff00  ;
	    }
	}/* loop over pads in this row  */
    }/* loop over rows on this sector */
  return 0;
}
