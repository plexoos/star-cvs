/*  find the vertex position that leads to
 *  the maximum number of straight tracks
 *  all distance units in cm            
 */

#include <sys/types.h>
#include <time.h>

#include <math.h>   
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include "StChain.h"
#include "StMessMgr.h"
#include "SvtVertFind.h"
#include "StDetectorId.h"
#include "StVertexId.h"
#include "tables/St_dst_vertex_Table.h"
#include "tables/St_scs_spt_Table.h"
#include "TH1.h"



SPACEPOINT 	 spt[30000];
int              PixelOcc[PIX_NMAX];
int              iNumOccupiedPixels;
int             *OccupiedPixels[SPT_PTR_NMAX];
long             spt_n;

double           r_ref;
double           x_scale, z_scale;
long     	 x_size, z_size, iptr;
double           x_min, x_max, z_min, z_max;
int              icall = 1;
//TH1F             *mtemp;

void sft_init(void);
float sft_process_event ( long NSpt, scs_spt_st *scs_spt, dst_vertex_st *sft_vertex, long Nvtx);
float sft_vf_ntrack(double z0);
float sft_find_vertex();
int bracket2 ( float (*)(double), float    fStart, float    fStep,
	       float   *ax, float   *bx, float   *cx, float   *fa,
	       float   *fb, float   *fc);
float x0tpc=0;
float y0tpc=0;

float sft_main(  St_scs_spt  *scs_spt, St_dst_vertex *svt_vertex, float x0,float y0, TH1F *hist)
{
   sft_init();

   x0tpc=x0;
   y0tpc=y0;
   
   gMessMgr->Info() << "TPC X at = " << x0tpc << endm;
   gMessMgr->Info() << "TPC Y at = " << y0tpc << endm;
   //mtemp=hist;
   scs_spt_st *s_spt = scs_spt->GetTable();  
   long NSpt = scs_spt->GetNRows();
   dst_vertex_st *sft_vertex = svt_vertex->GetTable();
   long Nvtx = svt_vertex->GetNRows();
   float zsvt = sft_process_event ( NSpt, s_spt, sft_vertex, Nvtx);
   
   if( zsvt != -999){
     svt_vertex->SetNRows(++Nvtx);
     return zsvt;
   }
   else
     return -999;
}



/*  svt vertex finding
 *  analyze all available events in file
 *  calculate average difference and rms
 */

float sft_process_event ( long Nspt,  scs_spt_st    *staf_spt,
                   dst_vertex_st *vertex, long n_vtx)
{    
 
   float my_ntracks=0;
   float zsvt=-999;
   long     ispt,
            jspt;
 

   if (Nspt <= 0)
      return -1;
 
   /*  Start by grabbing the contents of the scs_spt
    *  array and copying them into our local array.     */

   ispt = 0;
   r_ref = 11.; 
 
   for (jspt = 0; jspt < Nspt; jspt++)
   {
     if (staf_spt[jspt].id_wafer < 7000 && staf_spt[jspt].flag<2
	 && staf_spt[jspt].de[1] > 15) 
       {
	 spt[ispt].x[0]  = (double)staf_spt[jspt].x[0];
	 spt[ispt].x[1]  = (double)staf_spt[jspt].x[1];
	 spt[ispt].x[2]  = (double)staf_spt[jspt].x[2];
	 ispt++;
       }
   }
   
   spt_n = ispt;
   cout << spt_n << endl;
   if( spt_n < 4) return -1;

   zsvt= sft_find_vertex ();
   vertex[n_vtx].z = zsvt;
   vertex[n_vtx].x = -sft_vf_ntrack(zsvt); 
   vertex[n_vtx].y = 0.;
   vertex[n_vtx].n_daughters =0;
   vertex[n_vtx].id_aux_ent = 0;
   vertex[n_vtx].chisq[0] = 0.;
   vertex[n_vtx].chisq[1] = 0.;
   vertex[n_vtx].vtx_id = kEventVtxId;
   vertex[n_vtx].det_id = kSvtId; 
   vertex[n_vtx].iflag = 201;
   vertex[n_vtx].id = n_vtx+1;
   vertex[n_vtx].covar[0] = 0.;
   vertex[n_vtx].covar[1] = 0.;
   vertex[n_vtx].covar[2] = 0.;
   vertex[n_vtx].covar[3] = 0.;
   vertex[n_vtx].covar[4] = 0.;
   vertex[n_vtx].covar[5] = 0.;

   gMessMgr->Info() << "SVT vertex found at z= " <<  vertex[n_vtx].z << endm;
   

  return zsvt;

}



/* calculate the number of straight lines  */
/* compatible with the vertex position z0   */
float sft_vf_ntrack(double z0)
{
   double     xc, zc, zc0, x, y, z, r, rx, rz;
   int        ix, iz, ii, ntrack, ispt;

   if (icall == 1)
   {
      icall = 0; 
 
      /* build or update the list in feature space */
      /* loop on all space points                  */
      for (ispt = 0; ispt < spt_n; ispt++)
      {
         x   = spt[ispt].x[0]-x0tpc;
         y   = spt[ispt].x[1]-y0tpc;
         z   = spt[ispt].x[2];
         r   = sqrt (x * x + y * y);          /*xc = fatan2(y,x)   */
         rz  = r_ref / r;                     /* r_ref = 11. */
         /*rx  = x_scale / r;
	   xc  = x * rx;
	   Changed by Helen */
	 xc  = atan2(y,x); 

         zc0 = z * rz;                               /* the constant part */
         zc  = (zc0 + z0 - z0 * rz);                 /* the variable part */
         iz  = (int)((zc - z_min)*z_scale);          /* Split into mm bins Z-scale=140, zmin=-70*/
         ix  = (int)((xc - x_min)*x_scale);          /* Split into 3.6 degree bins x-scale=6.6*/
         spt[ispt].off = ix*z_size; 
         ii = spt[ispt].off + iz;



         if (ix < 0 || ix > x_size - 1 || iz < 0 || iz > z_size - 1)
         {
	     printf("\n out of domain %d  %d ",ix,iz); 
	      continue;
	 }

	 spt[ispt].rx  = r;
         spt[ispt].rz  = rz;
         spt[ispt].xc  = xc;
         spt[ispt].zc0 = zc0;
         spt[ispt].zc  = zc;
         spt[ispt].ix  = ix;
         spt[ispt].iz  = iz;
         spt[ispt].ii  = ii;

	 /* ii     is a pixel index                               */
	 /* ilist  is the position of the pixel index in the list */

	 if (ii >= 0 && ii < PIX_NMAX)
	 {
            PixelOcc[ii]++;

            if (PixelOcc[ii] == 1)
            {
               OccupiedPixels[iNumOccupiedPixels] = PixelOcc + ii;
               iNumOccupiedPixels++;
	       if( iNumOccupiedPixels >= SPT_PTR_NMAX) {
		printf("\n error: iNumOccupiedPixels =%d",iNumOccupiedPixels);
		 ispt=spt_n;
	       }
            }
         }
         else
         {
             printf("\n error: out of bounds ii=%d",ii); 
         }
      } /* for loop */
   }    /* if icall ==1 loop */
   else
   {
      for (ispt = 0; ispt < spt_n; ispt++)
      {
         /* the cte part - the variable part */
         /* zc = spt[ispt].zc0 - z0/spt[ispt].rz;  */
         zc = spt[ispt].zc0 +z0 - z0 * spt[ispt].rz;

         iz  = (int)((zc-z_min)*z_scale);
         if (iz < 0 || iz > z_size-1)
         {
	   printf("\n out of domain2 %d  %d ",(int)spt[ispt].off,iz); 
	   continue;
         }

         spt[ispt].zc  = zc;
         spt[ispt].iz  = iz;
         spt[ispt].ii  = ii  = spt[ispt].off + iz;

         /* ii     is a pixel index                               */
         /* ilist  is the position of the pixel index in the list */

         if (ii >= 0 && ii < PIX_NMAX){
	   PixelOcc[ii]++;
	   if (PixelOcc[ii] == 1){
	     OccupiedPixels[iNumOccupiedPixels] = PixelOcc + ii;
	     iNumOccupiedPixels++;
	     if( iNumOccupiedPixels >= SPT_PTR_NMAX) {
	       printf("\n error: iNumOccupiedPixels =%d",iNumOccupiedPixels);
	       ispt=spt_n;
	     }
	   }
         }
         else
         {
            printf("\n error: out of bounds ii=%d",ii);
         }
      }
   } /* if icall != 1 loop */



   /* count the number of groups */
   /* group = tracks             */ 
   /*                            */
   ntrack = 0;
   for (iptr = 0; iptr < iNumOccupiedPixels; iptr++){
     /* definition of track */
     if (*(OccupiedPixels[iptr]) > 1){
       ntrack++;
     }
     if (*(OccupiedPixels[iptr]) > 2){
       ntrack++;
     }
     *OccupiedPixels[iptr] = 0;
     OccupiedPixels[iptr] = NULL;
   }
   iNumOccupiedPixels = 0;
   return -ntrack;
}



void sft_init (void)
{

  x_size  = SPT_X;   /* 60 */
  z_size  = SPT_Z;  /* 1400 */
   x_min   = -3.3;
   z_min   = -70.;
   x_max   =  3.3;
   z_max   =  70.;
   x_scale = x_size / (x_max - x_min);
   z_scale = z_size / (z_max - z_min);

   for (iptr = 0; iptr < PIX_NMAX; iptr++) 
   {
     
     PixelOcc[iptr] = 0;
   
   }

   iNumOccupiedPixels = 0;
}



float sft_find_vertex ()
{

   int     iNumPoints;
   float   ax, bx, cx, fa, fb, fc;
   float   step;
   double  d, p, q, r;

   icall = 1;

   /* Home in on primary vertex */
   d = 0.0;
   //   step=1.;
   //iNumPoints = bracket2 (sft_vf_ntrack, (float)d, step, &ax, &bx, &cx, &fa, &fb, &fc);

   //r = (double)((bx - cx) * (fb - fa));
   //q = (double)((bx - ax) * (fb - fc));
   //p = (double)((bx - ax)*q - (bx - cx)*r);
   //q = 2.0 * (q - r);

   step=.1;
   iNumPoints = bracket2 (sft_vf_ntrack, (float)d, step, &ax, &bx, &cx, &fa, &fb, &fc);
   /* printf ("point a: z= % 12.5f with % 12.5f tracks \n", ax, fa);
      printf ("point b: z= % 12.5f with % 12.5f tracks \n", bx, fb);
      printf ("point c:   % 12.5f    % 12.5f\n", cx, fc);  */

   /* Not sure how this works!*/
   r = (double)((bx - cx) * (fb - fa));
   q = (double)((bx - ax) * (fb - fc));
   p = (double)((bx - ax)*q - (bx - cx)*r);
   q = 2.0 * (q - r);

   if (q > 0.0)
      p = -p;
   else
      q = -q;

   if( q !=0) {
     d = (double)bx + p / q; 
   }
   else{
     d = -999;
   }

   if (fb > -4) {d=-888;}
     
   return (float)d;
}



int bracket2 ( float (*func)(double),
          float    fStart,
          float    fStep,
          float   *ax,
          float   *bx,
          float   *cx,
          float   *fa,
          float   *fb,
          float   *fc)
{
  int     iNumPoints = 0, iDone, iStep, iMaxPos, iLeftPos, iRightPos;
  float   fX, fVal, fMaxVal = 0.0, fMaxPos;
  POINT   pPoints[1000];
  int     iMaxPoints = 1000;
#ifdef DEBUG
  FILE    *fout;
#endif
  /* start in the middle, and work our way outwards */

  iStep = 1;

  fX = fStart; /* 0 in the first loop  and bx in the second loop */

  iMaxPos =0;
  fVal = (*func)(fX);         /* is the number of tracks found for z=fx */
  pPoints[iNumPoints].x = fX;
  pPoints[iNumPoints].y = fVal;
  iNumPoints++;

  iDone = 0;
  while (!iDone && iNumPoints < iMaxPoints)
    {
      if (fVal < fMaxVal){           /* fVal is the -ntrack */
	fMaxVal = fVal;              /* that is why Max is found by "<" */ 
	iMaxPos = iNumPoints - 1;
	fMaxPos = fX;
      }
      
      /*  If the max wasn't one of the last two points we
       *  looked at, it we must have bracketed it.
       */
      
      /*  Didn't find a max?  Keep going! */
      
      fX += iStep * fStep;
      fVal = (*func)(fX);
      // printf("%f %f \n",fX,-fVal);
      //if (fStep==.1){
      //if (mtemp)
      //	mtemp->Fill(fX,-fVal);
	//}

      pPoints[iNumPoints].x = fX;
      pPoints[iNumPoints].y = fVal;
      iNumPoints++;
      iStep = (iStep > 0 ? -1 * (iStep + 1) : -1 * (iStep - 1));
    } /* end of while */
  
  /*  OK; we have the minimum bracketed.  Now let's pick out
   *  the three bracketing points.
   */
  
  iLeftPos  = (iMaxPos-2 > 0 ? iMaxPos - 2 : 0);
  iRightPos = iMaxPos + 2;
  if( iMaxPos == 0){
    iLeftPos=1;
  }
  
  *ax = pPoints[iLeftPos].x;
  *fa = pPoints[iLeftPos].y;
  *bx = pPoints[iMaxPos].x;
  *fb = pPoints[iMaxPos].y;
  *cx = pPoints[iRightPos].x;
  *fc = pPoints[iRightPos].y;
  
#ifdef DEBUG
  
  fout = fopen ("groups.dat", "w");
  
  for (iLeftPos=-500; iLeftPos<500; iLeftPos++){
    
    fX = (float) iLeftPos/10.;
    fprintf(fout,"%f %f \n",fX,(*func)(fX));
  }
  fclose(fout);
#endif     
  
  return iNumPoints;

}

