// $Id: StFtpcClusterFinder.cc,v 1.9 2000/11/14 13:08:04 hummler Exp $
//
// $Log: StFtpcClusterFinder.cc,v $
// Revision 1.9  2000/11/14 13:08:04  hummler
// add charge step calculation, minor cleanup
//
// Revision 1.8  2000/08/03 14:39:00  hummler
// Create param reader to keep parameter tables away from cluster finder and
// fast simulator. StFtpcClusterFinder now knows nothing about tables anymore!
//
// Revision 1.6  2000/04/13 18:08:21  fine
// Adjusted for ROOT 2.24
//
// Revision 1.5  2000/01/27 09:47:16  hummler
// implement raw data reader, remove type ambiguities that bothered kcc
//
// Revision 1.4  2000/01/03 12:48:44  jcs
// Add CVS Id strings
//

#include <iostream.h>
#include <stdlib.h>
#include "StFtpcClusterFinder.hh"
#include "StFtpcTrackMaker/StFtpcPoint.hh"
#include "math_constants.h"
#include <math.h>

StFtpcClusterFinder::StFtpcClusterFinder(StFTPCReader *reader,  
					 StFtpcParamReader *paramReader,
					 TClonesArray *pointarray)
{
//   cout << "StFtpcClusterFinder constructed" << endl;  
mReader = reader;
mParam = paramReader; 
mPoint = pointarray;
}

StFtpcClusterFinder::~StFtpcClusterFinder()
{
//   cout << "StFtpcClusterFinder destructed" << endl;
}

int StFtpcClusterFinder::search()
{
  double *pradius, *pdeflection;
  int iRow, iSec, iPad, iPadBuf, iHardSec, iHardRow;
  int iRowBuf, iSecBuf;
  int bNewSec;
  int clusters;
  int iNowSeqIndex, iNewSeqIndex, iOldSeqNumber, iOldSeqIndex;
  int iCUCSequence, iMoveSequence, iOldSeqBuf;
  int bOldSequenceUsed, bLastSequence;
  TClusterUC *FirstCUC, *CurrentCUC, *LastCUC, *DeleteCUC;
  TClusterUC *SequenceInCUC;
  TPCSequence *OldSequences, *NewSequences, *(SequencePointer[3]);
  int iNewSeqNumber;

  int iIndex;
  float fastlog[256];

  /* variables for dynamic CUC memory handling */
  TClusterUC CUCMemory[MAXNUMCUC];
  int CUCMemoryArray[MAXNUMCUC];
  int CUCMemoryPtr;

#ifdef DEBUGFILE
  FILE *fin, *fpoints;
  char finname[100], fpointsname[100];
  TPeak testpeak;
  int iBin, iHeight;
  int xLower, xUpper, yLower, yUpper, xNow, yNow;
  int *maparray;
  char getstring[100];
  float xwrite, ywrite;

  maparray= (int *)malloc(4000000*sizeof(int));
#endif

//   cout << "maxpads: " << mReader->getMaxPad(1) << endl;

  /* is magboltz database loaded, if not exit */
  if(mParam->numberOfPadtransBins()<1)
    {
      printf("Couldn't find magboltz data table, exiting!\n");
      return 0;
    }

  /* is calibration amplitude slope database loaded, if not load */
  if(mParam->numberOfCalibrationValues()<1)
    {
      printf("Couldn't find calibration table, exiting!\n");
      return 0;
    }

  /* allocate memory for padtrans table */
  pradius = (double *)malloc(mParam->numberOfDriftSteps()
			     *mParam->numberOfPadrowsPerSide()*sizeof(double));
  pdeflection = (double *)malloc(mParam->numberOfDriftSteps()
				 *mParam->numberOfPadrowsPerSide()*sizeof(double));

  if(pradius == NULL || pdeflection == 0)
    {
      printf("Padtrans memory allocation failed, exiting!\n");
      return 0;
    }

  /* integrate padtrans table from magboltz database */
  if(!calcpadtrans(pradius, pdeflection))
    {
      printf("Couldn't calculate padtrans table, exiting!\n");
      return 0;
    }

  /* initialize CUC memory handling */
  if(!cucInit(CUCMemory, CUCMemoryArray, &CUCMemoryPtr))
    {
      printf("Couldn't initialize CUC memory, exiting!\n");
      return 0;
    }

  /* calculate fastlog lookup */
  for(iIndex=1; iIndex<256; iIndex++)
    {
      fastlog[iIndex] = log((double) iIndex);
    }

  /* reset counter for found clusters */
  clusters = 0;

  /* initialize sequence and cluster lists */ 
  NewSequences=NULL;
  FirstCUC = NULL;
  iOldSeqNumber = 0;
  iNewSeqIndex=0;

#ifdef DEBUGFILE
  fin=fopen("test00", "w");
  fpoints=fopen("points00", "w");
#endif

  /* loop over raw data sequences */
  iNowSeqIndex = 0;
  iPad=0;
  iSec=0;
  iRow=0;
  bLastSequence=0;

  for(iRow=mParam->firstPadrowToSearch()-1,
	iRowBuf=mParam->firstPadrowToSearch()-1; 
      iRow<mParam->lastPadrowToSearch(); iRow++)
    {
      for(iSec=mParam->firstSectorToSearch()-1,
	    iSecBuf=mParam->firstSectorToSearch()-1; 
	  iSec<mParam->lastSectorToSearch(); iSec++)
	{
	  // new sector, set pad buffer so there can be no matches
	  iPadBuf=-2;
	  bNewSec=TRUE;
#ifdef DEBUG
	  printf("Now on Sector %d, Row %d\n", iSec, iRow);
#endif

	  // calculate hardware (daq) sectors from software position
	  iHardSec = mParam->numberOfSectors()*(int)(iRow/2) + iSec + 1;
	  iHardRow = iRow%2 + 1;

	  // get list of occupied pads in sector
	  unsigned char *(padlist[2]);
	  int iOccPads=mReader->getPadList(iHardSec, iHardRow, 
					   padlist[iHardRow-1]);

	  // loop over occupied pads
	  int iThPad;
	  for(iThPad=0; iThPad<iOccPads; iThPad++)
	    {
	      iPad=padlist[iHardRow-1][iThPad];

	      // search, fit and remove finished CUCs  
	      for(CurrentCUC = FirstCUC; CurrentCUC!=NULL; 
		  CurrentCUC = CurrentCUC->NextClusterUC)
		{
		  // check if CurrentCUC is adjacent to this pad
		  if(iPad > CurrentCUC->EndPad + 1 || bNewSec)
		    {
		      // CurrentCUC is finished 
		      // if CUC has not been lost by merging, process cluster
		      if(CurrentCUC->EndPad > CurrentCUC->StartPad)
			{
			  // cluster processing: increment cluster counter 
			  clusters ++;

			  // cluster processing: call hitfinder 
			  if(!findHits(CurrentCUC, iRowBuf, iSecBuf, 
				       pradius, pdeflection, 
				       fastlog)
			     )
			    {
#ifdef DEBUG
			      printf("Hitfinder failed! Cluster is lost.\n");
#endif
			    }
			}
		      DeleteCUC=CurrentCUC;
		      // bypass CurrentCUC in CUC list 
		      if(CurrentCUC==FirstCUC)
			{
			  FirstCUC=CurrentCUC->NextClusterUC;
			}
		      else
			{
			  LastCUC->NextClusterUC=CurrentCUC->NextClusterUC;
			  CurrentCUC=LastCUC;
			}
		      // free CurrentCUC memory 
		      if(!cucFree(CUCMemory, CUCMemoryArray, 
				  &CUCMemoryPtr, DeleteCUC))
			{
			  printf("Fatal memory management error.\n");
			  return 0;
			}
		    }
		  LastCUC=CurrentCUC;
		}
	      iRowBuf=iRow;
	      iSecBuf=iSec;

#ifdef DEBUGFILE
	      if(bNewSec)
		{
		  fclose(fin);
		  fclose(fpoints);
		  sprintf(finname,"test%d%d", iRow,iSec); 
		  sprintf(fpointsname,"points%d%d", iRow,iSec); 
		  fin=fopen(finname, "w");
		  fpoints=fopen(fpointsname, "w");
		}
#endif

	      // initialize sequence lists: 
	      // new-array is moved to old 
	      // and first element initialized 
	      OldSequences=NewSequences;
	      iOldSeqNumber=iNewSeqIndex;
	      iNewSeqIndex=0;
      
	      if(iPad!=iPadBuf+1)
		{
		  iOldSeqNumber=0;
		}
	      iPadBuf=iPad;

	      // reset beginning of sequence comparison
	      iOldSeqBuf=0;


	      // get sequences on this pad
	      mReader->getSequences(iHardSec, iHardRow, iPad, &iNewSeqNumber,
				   SequencePointer[iHardRow]);
	      NewSequences=SequencePointer[iHardRow];
	      
	      // loop over sequences
	      for(iNewSeqIndex=0; iNewSeqIndex < iNewSeqNumber; 
		  iNewSeqIndex++)
		{

		  // mark sequence as unused 
		  SequenceInCUC=NULL;

		  // compare this sequence to old sequences
		  for(iOldSeqIndex=iOldSeqBuf; iOldSeqIndex < iOldSeqNumber; 
		      iOldSeqIndex++)
		    {
		      // are beginning or end of new sequence between
		      // beginning and end of old sequence?
		      if(((NewSequences[iNewSeqIndex].startTimeBin >= 
			   OldSequences[iOldSeqIndex].startTimeBin) && 
			  (NewSequences[iNewSeqIndex].startTimeBin <= 
			   OldSequences[iOldSeqIndex].startTimeBin + 
			   OldSequences[iOldSeqIndex].Length-1)) || 
			 ((NewSequences[iNewSeqIndex].startTimeBin + 
			   NewSequences[iNewSeqIndex].Length-1 >= 
			   OldSequences[iOldSeqIndex].startTimeBin) && 
			  (NewSequences[iNewSeqIndex].startTimeBin + 
			   NewSequences[iNewSeqIndex].Length-1 <= 
			   OldSequences[iOldSeqIndex].startTimeBin + 
			   OldSequences[iOldSeqIndex].Length-1)) ||
			 ((OldSequences[iOldSeqIndex].startTimeBin >= 
			   NewSequences[iNewSeqIndex].startTimeBin) && 
			  (OldSequences[iOldSeqIndex].startTimeBin <= 
			   NewSequences[iNewSeqIndex].startTimeBin + 
			   NewSequences[iNewSeqIndex].Length-1)))
			{
			  // yes, matching sequence found 
			  // set beginning of search for next sequence 
			  iOldSeqBuf=iOldSeqIndex;
			  bOldSequenceUsed=0;
 
			  // compare matching sequences to old CUCs  
			  // loop over all active CUCs 
			  for(CurrentCUC = FirstCUC; CurrentCUC!=NULL; 
			      CurrentCUC = CurrentCUC->NextClusterUC)
			    {
			      LastCUC=CurrentCUC;
			      // loop over all sequences in CUC
			      for(iCUCSequence=1; 
				  iCUCSequence<CurrentCUC->NumSequences; 
				  iCUCSequence++)
				{
				  // is cuc sequence identical to 
				  // matching old sequence?
				  if((OldSequences[iOldSeqIndex].startTimeBin == 
				      CurrentCUC->Sequence[iCUCSequence].startTimeBin)
				     && (CurrentCUC->SequencePad[iCUCSequence] == 
					 iPad-1))
				    {
				      bOldSequenceUsed=1;
				      // matching old sequence is in CUC 
				      // check if new sequence is already used 
				      if(SequenceInCUC!=CurrentCUC)
					{
					  if(SequenceInCUC!=NULL && SequenceInCUC!=CurrentCUC)
					    {
					      // yes, already used, merge CUCs 
					      // mark old CUC for removal
					      SequenceInCUC->EndPad = 
						SequenceInCUC->StartPad;
					      // set StartPad to the smaller StartPad
					      if(SequenceInCUC->StartPad < 
						 CurrentCUC->StartPad)
						{
						  CurrentCUC->StartPad = 
						    SequenceInCUC->StartPad;
						}
					      CurrentCUC->EndPad=iPad;
					      // append SequenceInCUC to CurrentCUC
					      // copy all sequences to CurrentCUC 
					      for(iMoveSequence=0; 
						  (iMoveSequence
						   <SequenceInCUC->NumSequences) &&
						    (CurrentCUC->NumSequences+iMoveSequence+1
						     <MAXNUMSEQUENCES); 
						  iMoveSequence++)
						{
						  CurrentCUC->
						    Sequence[iMoveSequence + 
							    CurrentCUC->NumSequences] = 
						    SequenceInCUC->
						    Sequence[iMoveSequence];
						  CurrentCUC->
						    SequencePad[iMoveSequence + 
							       CurrentCUC->NumSequences] = 
						    SequenceInCUC->
						    SequencePad[iMoveSequence]; 
						}
					      // add up number of sequences
					      CurrentCUC->NumSequences += 
						SequenceInCUC->NumSequences;
					      if(CurrentCUC->NumSequences > MAXNUMSEQUENCES)
						{
						  CurrentCUC->NumSequences = MAXNUMSEQUENCES;
						}
					      
					      // sequence is now in CurrentCUC
					      SequenceInCUC=CurrentCUC;
					    }
					  else // to: if(SequenceInCUC!=NULL)
					    {
					      // sequence did not belong to any CUC before
					      // add sequence to CurrentCUC
					      if(CurrentCUC->NumSequences<MAXNUMSEQUENCES)
						{
						  CurrentCUC->Sequence[CurrentCUC->NumSequences]
						    = NewSequences[iNewSeqIndex];
						  CurrentCUC->SequencePad[CurrentCUC->NumSequences]
						    =iPad;
						  CurrentCUC->NumSequences++;
						}
					      CurrentCUC->EndPad=iPad;
					      SequenceInCUC=CurrentCUC;
				      
					      // check if new sequence touches sector limit
					      if(NewSequences[iNewSeqIndex].startTimeBin==0 ||
						 NewSequences[iNewSeqIndex].startTimeBin
						 +NewSequences[iNewSeqIndex].Length
						 ==mParam->numberOfTimebins()-1 || 
						 iPad==mParam->numberOfPads()-1)
						{
						  CurrentCUC->CutOff=1;
						}
					    } // end of: if(SequenceInCUC!=NULL) ... else 
					} // end of: if(SequenceInCUC...) 
				    } // end of: if((OldSequences...)) 
				} // end of: for(ICUCSequence...) 
			    } // end of: for(CurrentCUC...)
			  if(SequenceInCUC==NULL && NewSequences[iNewSeqIndex].Length>1)
			    {
			      // no matching CUC was found: create new CUC
			      // allocate memory
			      CurrentCUC=cucAlloc(CUCMemory, CUCMemoryArray, 
						  &CUCMemoryPtr);
			      if(CurrentCUC == NULL)
				{
				  // no free memory, overwrite last CUC
#ifdef DEBUG
				  printf("Previous cluster is now lost.\n");
#endif
				  CurrentCUC=LastCUC;
				  return 0;
				}
			      else
				{
				  // set pointers to this CUC
				  if(FirstCUC == NULL)
				    {
				      FirstCUC = CurrentCUC;
				    }
				  else
				    {
				      LastCUC->NextClusterUC=CurrentCUC;
				    }
				  
				  // this is the newest CUC 
				  CurrentCUC->NextClusterUC=NULL;
				}
		      
			      // fill new CUC structure
			      CurrentCUC->StartPad=iPad-1;
			      CurrentCUC->EndPad=iPad;
			      CurrentCUC->NumSequences=2;
		      
			      // copy sequences to CUC 
			      CurrentCUC->Sequence[0] =
				OldSequences[iOldSeqIndex];
			      // struct assignment operator used
			      // created copy of sequence in CUC
			      // pointer operation avoided because data in CUC
			      // will be calibrated
			      CurrentCUC->SequencePad[0] = iPad-1;
		      
			      CurrentCUC->Sequence[1] =
				NewSequences[iNewSeqIndex];
			      CurrentCUC->SequencePad[1]=iPad;
			      SequenceInCUC=CurrentCUC;
			      
			      // check if new CUC touches sector limits
			      if(iPad==1 || iPad==mParam->numberOfPads()-1 || 
				 CurrentCUC->Sequence[0].startTimeBin==0 || 
				 CurrentCUC->Sequence[0].startTimeBin
				 +CurrentCUC->Sequence[0].Length
				 ==mParam->numberOfTimebins()-1 || 
				 CurrentCUC->Sequence[1].startTimeBin==0 || 
				 CurrentCUC->Sequence[1].startTimeBin
				 +CurrentCUC->Sequence[1].Length
				 ==mParam->numberOfTimebins()-1)
				{
				  CurrentCUC->CutOff=1;
				}
			      else
				{
				  CurrentCUC->CutOff=0;
				}
			    }   // end of: if(SequenceInCUC==NULL)
			  else
			    {
			      if(bOldSequenceUsed==0 && SequenceInCUC!=0)
				{  
				  // new sequence has been used but old one hasn't 
				  // append that to cluster, too
				  if(SequenceInCUC->NumSequences<MAXNUMSEQUENCES)
				    {
				      SequenceInCUC->Sequence[SequenceInCUC->NumSequences]
					= OldSequences[iOldSeqIndex];
				      SequenceInCUC->SequencePad[SequenceInCUC->NumSequences]
					=iPad-1;
				      SequenceInCUC->NumSequences++;
				    }
			  
				  // check if Old sequence touches sector limit 
				  if(OldSequences[iOldSeqIndex].startTimeBin==0 ||
				     OldSequences[iOldSeqIndex].startTimeBin
				     +OldSequences[iOldSeqIndex].Length
				     ==mParam->numberOfTimebins()-1 || 
				     iPad==mParam->numberOfPads()-1)
				    {
				      SequenceInCUC->CutOff=1;
				    }
				}
			    }
			} // end of: if(sequence matching) 
		    } // end of: for(iOldSeqIndex...) 
		} // end of: for(iNewSeqIndex...)
	      bNewSec=FALSE;
	    } // end of: for(iThPad...)

	  // sector done, fit and remove all remaining CUCs  
	  for(CurrentCUC = FirstCUC; CurrentCUC!=NULL; 
	      CurrentCUC = CurrentCUC->NextClusterUC)
	    {
	      // CurrentCUC is finished 
	      // if CUC has not been lost by merging, process cluster
	      if(CurrentCUC->EndPad > CurrentCUC->StartPad)
		{
		  // cluster processing: increment cluster counter 
		  clusters ++;
		  
		  // cluster processing: call hitfinder 
		  if(!findHits(CurrentCUC, iRowBuf, iSecBuf, 
			       pradius, pdeflection, 
			       fastlog)
		     )
		    {
#ifdef DEBUG
		      printf("Hitfinder failed! Cluster is lost.\n");
#endif
		    }
		}
	      DeleteCUC=CurrentCUC;
	      // bypass CurrentCUC in CUC list 
	      if(CurrentCUC==FirstCUC)
		{
		  FirstCUC=CurrentCUC->NextClusterUC;
		}
	      else
		{
		  LastCUC->NextClusterUC=CurrentCUC->NextClusterUC;
		  CurrentCUC=LastCUC;
		}
	      // free CurrentCUC memory 
	      if(!cucFree(CUCMemory, CUCMemoryArray, 
			  &CUCMemoryPtr, DeleteCUC))
		{
		  printf("Fatal memory management error.\n");
		  return 0;
		}
	      LastCUC=CurrentCUC;
	    }
	  
	} // end of: for(iSec...)
    } // end of: for(iRow...)

  printf("StFtpcClusterFinder Found %d clusters and processed to %d hits.\n", clusters,(int) mPoint->GetEntriesFast() );
  
#ifdef DEBUGFILE
  fclose(fin);
  fclose(fpoints);
  
  for(iRow=0; iRow<20; iRow++)
    {
      for(iSec=0; iSec<6; iSec++)
	{
	  sprintf(finname, "test%d%d", iRow, iSec);
	  sprintf(fpointsname, "charge%d%d", iRow, iSec);
	  fin=fopen(finname, "r");
	  fpoints=fopen(fpointsname, "w");
	  for(xNow=0; xNow<2000; xNow++)
	    {
	      for(yNow=0; yNow<2000; yNow++)
		{
		  maparray[xNow+2000*yNow]=0;
		}
	    }

	  sprintf(getstring, "\0");
	  fgets(getstring, sizeof(getstring), fin);
	  while(strlen(getstring)>3)
	    {
	      sscanf(getstring, "%d %d %d", &iPad, &iBin, &iHeight);
	      
	      testpeak.PadPosition=iPad-0.5;
	      testpeak.TimePosition=iBin-0.5;
	      padtrans(&testpeak, iRow, iSec, pradius, pdeflection);
	      xLower=(int) ((1000 / 31) * testpeak.x + 1000);
	      yLower=(int) ((1000 / 31) * testpeak.y + 1000);

	      testpeak.PadPosition=iPad+0.5;
	      testpeak.TimePosition=iBin+0.5;
	      padtrans(&testpeak, iRow, iSec, pradius, pdeflection);
	      xUpper=(int) ((1000 / 31) * testpeak.x + 1000);
	      yUpper=(int) ((1000 / 31) * testpeak.y + 1000);

	      for(xNow=0; xNow<abs(xUpper-xLower); xNow++)
		{
		  for(yNow=0; yNow<abs(yUpper-yLower); yNow++)
		    {
		      if(xUpper<xLower)
			maparray[(xUpper+xNow)+2000*(yUpper+yNow)]=iHeight;
		      else
			maparray[(xLower+xNow)+2000*(yLower+yNow)]=iHeight;
		    }
		}

	      sprintf(getstring, "\0");
	      fgets(getstring, sizeof(getstring), fin);

	    }
	  
	  for(xNow=0; xNow<2000; xNow++)
	    {
	      for(yNow=0; yNow<2000; yNow++)
		{
		  if(maparray[xNow+2000*yNow]!=0)
		    {
		      xwrite= ((float) xNow-1000)*31/1000;
		      ywrite= ((float) yNow-1000)*31/1000;

		      fprintf(fpoints, "%f %f %d\n", xwrite, ywrite, maparray[xNow+2000*yNow]);
		    }
		}
	    }
	  fclose (fin);
	  fclose (fpoints);
	}
    }
  
  free(maparray);
  
#endif
  
  free(pradius);
  free(pdeflection);
  
#ifdef DEBUG 
  cout<<"finished running cluster search"<<endl;
#endif
  int dummy=1;
  return dummy;
}

int StFtpcClusterFinder::findHits(TClusterUC *Cluster, 
				 int iRow, 
				 int iSec, 
				 double *pRadius, 
				 double *pDeflection, 
				 float fastlog[256])
{
  int iPad, iSequence; 
  int iPadPeaks, iThisPadPeaks;
  int iNewPeakLoop, iOldPeakLoop;
  int iNumPeaks, iOldPeakstore, iNewPeakstore;
  int iPeakCompare;
  TPadPeak PadPeaks[2][MAXSEQPEAKS];
  TPeak Peaks[MAXPEAKS];

  iNumPeaks=0;
  iPadPeaks=0;
  iOldPeakstore=0;
/*   printf("starting hitfinder\n"); */

  for(iPad=Cluster->StartPad; iPad<=Cluster->EndPad; iPad++)
    {
      /* set index for alternating use of sequencepeak array */
      iNewPeakstore=(iOldPeakstore+1)&1;
      
      iThisPadPeaks=0;

      /* loop over sequences as they need not be in ascending order, */
      /* if cluster is deformed. */
      for(iSequence = 0; 
          iSequence < Cluster->NumSequences; iSequence++)
	{
	  if(Cluster->SequencePad[iSequence] == iPad)
	    {
	      /* now looping over sequences in pad order */
	      /* get peaks for this sequence */
	      getSeqPeaksAndCalibAmp(&(Cluster->Sequence[iSequence]), 
				     iRow*mParam->numberOfSectors()
				     *mParam->numberOfPads(),
				     iSec*mParam->numberOfPads(), iPad,
				     PadPeaks[iNewPeakstore], &iThisPadPeaks
#ifdef DEBUGFILE
				     , fin
#endif
				     );
	      /* peaks are appended to peak list for this pad */
	    }
	}
      

      /* loop over peaks on this pad */ 
      for(iNewPeakLoop=0; iNewPeakLoop < iThisPadPeaks; iNewPeakLoop++)
	{
	  /* if peak is not matched => slope=0 => reprocessing */
	  PadPeaks[iNewPeakstore][iNewPeakLoop].slope=0;
	  
	  for(iOldPeakLoop=0; iOldPeakLoop < iPadPeaks; iOldPeakLoop++)
	    {
	      iPeakCompare=PadPeaks[iNewPeakstore][iNewPeakLoop].Timebin - 
		PadPeaks[iOldPeakstore][iOldPeakLoop].Timebin;
	      if(   ((iPeakCompare <= 1+PadPeaks[iNewPeakstore][iNewPeakLoop].width) 
		     && (iPeakCompare >= 0)) 
		 || ((iPeakCompare >=-1-PadPeaks[iOldPeakstore][iOldPeakLoop].width) 
		     && (iPeakCompare <= 0)))
		{
		  /* now we found a matching sequence peak  */
		  /* is the local cluster slope rising */
		  if(PadPeaks[iOldPeakstore][iOldPeakLoop].height <= 
		     PadPeaks[iNewPeakstore][iNewPeakLoop].height)
		    {
		      /* if slope is 0 keep the old slope */
		      /* otherwise flat tops will be split in 2 hits */ 
		      if(PadPeaks[iOldPeakstore][iOldPeakLoop].height == 
			 PadPeaks[iNewPeakstore][iNewPeakLoop].height)
			{
			  PadPeaks[iNewPeakstore][iNewPeakLoop].slope = 
			    PadPeaks[iOldPeakstore][iOldPeakLoop].slope;
			}
		      else
			{
			  /* yes, set slope to rising */
			  PadPeaks[iNewPeakstore][iNewPeakLoop].slope=1;
			}
		    }
		  else
		    {
		      /* slope is falling */
		      PadPeaks[iNewPeakstore][iNewPeakLoop].slope=-1;

		      /* was the slope rising before? */
		      if(PadPeaks[iOldPeakstore][iOldPeakLoop].slope > 0)
			{
			  /* yes, peak is found */
			  Peaks[iNumPeaks].pad=iPad-1;
			  Peaks[iNumPeaks].Timebin
			    =PadPeaks[iOldPeakstore][iOldPeakLoop].
			    Timebin;
			  Peaks[iNumPeaks].Sequence = 
			    PadPeaks[iOldPeakstore][iOldPeakLoop].Sequence;
			  iNumPeaks++;
			  /* check if peak memory full */ 
			  if(iNumPeaks>MAXPEAKS)
			    {
#ifdef DEBUG
			      printf("Sick cluster. %d are too many peaks.\n",
				     iNumPeaks);
#endif
			      return FALSE;
			    }

			}
		    }
		  /* set slope to 0 to mark old peak as used */
		  PadPeaks[iOldPeakstore][iOldPeakLoop].slope=0;
		}
	    } /* end of: for(iOldPeakLoop...) */
	  /* is this peak unmatched, then reprocess */

	  if(PadPeaks[iNewPeakstore][iNewPeakLoop].slope == 0)
	    {
	      /* default for an unmatched peak is rising */
	      PadPeaks[iNewPeakstore][iNewPeakLoop].slope=1;
	      
	      for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
		{
		  if((Cluster->SequencePad[iSequence] == iPad - 1) && 
		     (Cluster->Sequence[iSequence].startTimeBin <= 
		      PadPeaks[iNewPeakstore][iNewPeakLoop].Timebin) && 
		     (Cluster->Sequence[iSequence].startTimeBin + 
		      Cluster->Sequence[iSequence].Length > 
		      PadPeaks[iNewPeakstore][iNewPeakLoop].Timebin) &&
		     (Cluster->Sequence[iSequence]
		      .FirstAdc[PadPeaks[iNewPeakstore][iNewPeakLoop].Timebin
			       - Cluster->Sequence[iSequence].startTimeBin] >= 
		      (unsigned char) PadPeaks[iNewPeakstore][iNewPeakLoop].height))
		    {
		      PadPeaks[iNewPeakstore][iNewPeakLoop].slope=-1;
		    }
		}
	    }
	} /* end of: for(iNewPeakLoop...) */
      /* all new peaks have been matched */
      /* check for unmatched old peaks with rising slope */
      /* make them hits, too, if their right neighbor pixel is lower */
      for(iOldPeakLoop=0; iOldPeakLoop < iPadPeaks; iOldPeakLoop++)
	{
	  if(PadPeaks[iOldPeakstore][iOldPeakLoop].slope > 0)
	    {
	      /* find neighbor pixel */ 
	      for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
		{
		  if((Cluster->SequencePad[iSequence] == iPad) && 
		     (Cluster->Sequence[iSequence].startTimeBin <= 
		      PadPeaks[iOldPeakstore][iOldPeakLoop].Timebin) && 
		     (Cluster->Sequence[iSequence].startTimeBin + 
		      Cluster->Sequence[iSequence].Length > 
		      PadPeaks[iOldPeakstore][iOldPeakLoop].Timebin) &&
		     (Cluster->Sequence[iSequence]
		      .FirstAdc[PadPeaks[iOldPeakstore][iOldPeakLoop].Timebin
			       - Cluster->Sequence[iSequence].startTimeBin] >= 
		      (unsigned char) PadPeaks[iOldPeakstore][iOldPeakLoop].height))
		    {
		      PadPeaks[iOldPeakstore][iOldPeakLoop].slope = -1;
		    }
		}
	    }
	  if(PadPeaks[iOldPeakstore][iOldPeakLoop].slope > 0)
	    {	  
	      Peaks[iNumPeaks].pad = iPad-1;
	      Peaks[iNumPeaks].Timebin = 
		PadPeaks[iOldPeakstore][iOldPeakLoop].Timebin;
	      Peaks[iNumPeaks].Sequence = 
		PadPeaks[iOldPeakstore][iOldPeakLoop].Sequence;
	      iNumPeaks++;
	      
	      
	      /* check if peak memory full */ 
	      if(iNumPeaks>MAXPEAKS)
		{
#ifdef DEBUG
		  printf("Sick cluster. %d are too many peaks.\n", iNumPeaks);
#endif
		  return FALSE;
		}
	    }
	}
      iPadPeaks=iThisPadPeaks;
      iOldPeakstore=iNewPeakstore;
    }/* end of: for(iPad...) */
  /* check if there are peaks left over with rising slope */ 
  /* make them hits, too */
  for(iOldPeakLoop=0; iOldPeakLoop < iPadPeaks; iOldPeakLoop++)
    {
      if(PadPeaks[iOldPeakstore][iOldPeakLoop].slope > 0)
	{
	  Peaks[iNumPeaks].pad = iPad-1;
	  Peaks[iNumPeaks].Timebin = 
	    PadPeaks[iOldPeakstore][iOldPeakLoop].Timebin;
	  Peaks[iNumPeaks].Sequence = 
	    PadPeaks[iOldPeakstore][iOldPeakLoop].Sequence;
	  iNumPeaks++;
	  
	  /* check if peak memory full */ 
	  if(iNumPeaks>MAXPEAKS)
	    {
#ifdef DEBUG
	      printf("Sick cluster. %d are too many peaks.\n", iNumPeaks);
#endif
	      return FALSE;
	    }
	}
    }
  if(!fitPoints(Cluster, iRow, iSec, pRadius, pDeflection, Peaks, 
		iNumPeaks, fastlog))
    {
#ifdef DEBUG
      printf("Point fitting failed! Cluster is lost.\n");
#endif
      return FALSE;
    }

#ifdef DEBUGFILE
  for(iOldPeakLoop=0; iOldPeakLoop<iNumPeaks; iOldPeakLoop++)
    {
      if(Peaks[iOldPeakLoop].PadPosition>0 && Peaks[iOldPeakLoop].TimePosition>0)
	{
	  fprintf(fpoints, "%f %f %f %f\n", 
		  Peaks[iOldPeakLoop].PadPosition, Peaks[iOldPeakLoop].TimePosition, Peaks[iOldPeakLoop].x, Peaks[iOldPeakLoop].y);
	}
    }
#endif

  return TRUE;
}

int StFtpcClusterFinder::getSeqPeaksAndCalibAmp(TPCSequence *Sequence,
					       int iRowTimesSeqsTimesPads,
					       int iSeqTimesPads,
					       int iPad,
					       TPadPeak *Peak, 
					       int *pNumPeaks)
{
  int bSlope;
  int iIndex, iPadIndex;
  int iWidth;
  unsigned char cLastADC, cTemp;
  
  cLastADC=0;
  iWidth=0;
  iPadIndex=iRowTimesSeqsTimesPads+iSeqTimesPads+iPad;
  
  for(iIndex=0; iIndex< Sequence->Length; iIndex++)
    {
      cTemp=(unsigned char)((float)(unsigned int)(Sequence->FirstAdc[iIndex])
			    * mParam->amplitudeSlope(iPadIndex) 
			    + mParam->amplitudeOffset(iPadIndex));
      Sequence->FirstAdc[iIndex]=cTemp;

      if(cTemp < cLastADC)
	{
	  if(bSlope > 0)
	    {
	      /* peak is found */

	      Peak[*pNumPeaks].Sequence = *Sequence;
	      Peak[*pNumPeaks].Timebin = Sequence->startTimeBin+iIndex-1;
	      Peak[*pNumPeaks].height = cLastADC;
	      Peak[*pNumPeaks].width = iWidth;
	      if(*pNumPeaks < MAXSEQPEAKS - 1)
		{
		  (*pNumPeaks)++;
		}
	    }
	  bSlope = -1;
	  iWidth=0;
	}
      else
	{
	  if(cTemp != cLastADC)
	    {	     
	      bSlope = 1;
	      iWidth=0;
	    }
	  else
	    {
	      iWidth++;
	    }
	}

#ifdef DEBUGFILE
      fprintf(fin, "%d %d %d\n", iPad, Sequence->startTimeBin+iIndex, (int)cTemp);
#endif

      cLastADC=cTemp;
    }    
  if(bSlope > 0)
    {
	      Peak[*pNumPeaks].Sequence = *Sequence;
	      Peak[*pNumPeaks].Timebin =
		Sequence->startTimeBin+Sequence->Length-1;
	      Peak[*pNumPeaks].height = 
		Sequence->FirstAdc[Sequence->Length-1];
	      Peak[*pNumPeaks].width = iWidth;
	      if(*pNumPeaks < MAXSEQPEAKS - 1)
		{
		  (*pNumPeaks)++;
		}
    }
  return TRUE;
}

int StFtpcClusterFinder::fitPoints(TClusterUC* Cluster, 
				  int iRow, 
				  int iSec, 
				  double *pRadius, 
				  double *pDeflection, 
				  TPeak *Peak, 
				  int iNumPeaks, 
				  float fastlog[256])
{
  int iADCValue, iADCPlus, iADCMinus;
  int iADCTimePlus, iADCTimeMinus;
  int iSequence, iBin;
  int ChargeSum, PadSum;
  float TimeSum;
  int iUseGauss;
  int iPeakIndex, iInnerIndex;
  int iNumUnfoldLoops;
  int BadFit, PeakShifted, FailedFit;
  int PadtransPerTimebin, PadtransBin;
  float SumDeltaPos, SumDeltaAmp;
  float NewTimePosition, NewPadPosition;
  float NewPeakHeight, PeakHeightSum;
  float fDeltaADC, fDeltaADCPlus, fDeltaADCMinus;
  float fDeltaADCTimePlus, fDeltaADCTimeMinus;
  float fDriftLength, fRadError, fPhiError;

  PadtransPerTimebin=(int) mParam->numberOfDriftSteps()
    / mParam->numberOfTimebins();

  if(iNumPeaks == 0)
    {
#ifdef DEBUG
      printf("Cluster starting %d, %d has no peak!\n", Cluster->StartPad, Cluster->Sequence->startTimeBin);
#endif
      return FALSE;
    }

  /* sum up cluster charge */
  ChargeSum=0;
  PadSum=0;
  TimeSum=0;
  for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
    {
      for(iBin = 0; iBin < Cluster->Sequence[iSequence].Length; iBin++)
	{
	  iADCValue = Cluster->Sequence[iSequence].FirstAdc[iBin];
	  ChargeSum += iADCValue;
	  PadSum += Cluster->SequencePad[iSequence] * iADCValue;
	  TimeSum += (Cluster->Sequence[iSequence].startTimeBin + iBin + 
		      mParam->timeOffset(iRow*mParam->numberOfSectors()
					 *mParam->numberOfPads()
					 +iSec*mParam->numberOfPads()
					 +Cluster->SequencePad[iSequence])) 
	    * iADCValue;
	}
    }

  if(iNumPeaks == 1)
    {  
      Peak->PeakHeight = 
	Peak->Sequence.FirstAdc[Peak->Timebin - Peak->Sequence.startTimeBin];  

      /* one-peak cluster, fit according to preference from det */
      iUseGauss=mParam->gaussFittingFlags();


      /* calculate pad position first */
      if((iUseGauss & 1) == 1)
	{
	  /* get values for gaussfit */
	  iADCValue = (int) Peak->PeakHeight;
	  iADCPlus=0;
	  iADCMinus=0;
	  /* find sequences with neighbor pixels */ 
	  for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
	    {
	      if((Cluster->SequencePad[iSequence] == Peak->pad - 1) && 
		 (Cluster->Sequence[iSequence].startTimeBin <= Peak->Timebin) && 
		 (Cluster->Sequence[iSequence].startTimeBin + 
		  Cluster->Sequence[iSequence].Length > Peak->Timebin))
		{
		  iADCMinus = Cluster->Sequence[iSequence]
		    .FirstAdc[Peak->Timebin 
			     - Cluster->Sequence[iSequence].startTimeBin];
		}
	      if((Cluster->SequencePad[iSequence] == Peak->pad + 1) && 
		 (Cluster->Sequence[iSequence].startTimeBin < Peak->Timebin) && 
		 (Cluster->Sequence[iSequence].startTimeBin + 
		  Cluster->Sequence[iSequence].Length > Peak->Timebin))
		{
		  iADCPlus = Cluster->Sequence[iSequence]
		    .FirstAdc[Peak->Timebin 
			     - Cluster->Sequence[iSequence].startTimeBin];
		}
	    }
	  
	  /* check if Gaussfit is possible */
	  if((iADCValue == 0) || (iADCPlus == 0) || (iADCMinus == 0) || 
	     ((iADCValue <= iADCPlus) && (iADCValue <= iADCMinus)))
	    {
	      /* use weighted mean instead */
	      iUseGauss = iUseGauss & 2;
	    }
	  else
	    {
	      /* do gaussfit */
	      Peak->PadSigma = sqrt (1 / 
			       ((2 * fastlog[iADCValue]) -
				    (fastlog[iADCPlus] + fastlog[iADCMinus])));
	      Peak->PadPosition = (float) Peak->pad + 
		sqr(Peak->PadSigma) * (fastlog[iADCPlus] - fastlog[iADCMinus]);
	    }
	} 
      
      if((iUseGauss & 1) == 0)
	{
	  /* calculate pad position by weighted mean */ 
	  Peak->PadPosition = (float) PadSum / (float) ChargeSum;
	  
	  /* calculate pad direction cluster width by weighted mean */
	  Peak->PadSigma=0;
	  for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
	    {
	      for(iBin = 0; iBin < Cluster->Sequence[iSequence].Length; iBin++)
		{
		  Peak->PadSigma+=(sqr((float) Cluster->SequencePad[iSequence] 
				       - Peak->PadPosition)
				   *Cluster->Sequence[iSequence].FirstAdc[iBin]);
		}
	    }
	  Peak->PadSigma /= (float) ChargeSum;
	}

      /* now calculate the time position */
      if((iUseGauss & 2) > 0)
	{
	  /* get values for gaussfit */
	  iADCValue = (int) Peak->PeakHeight;
	  iADCPlus=0;
	  iADCMinus=0;

	  if(Peak->Timebin > Peak->Sequence.startTimeBin)
	    {
	      iADCMinus = Peak->Sequence.FirstAdc[Peak->Timebin - 
						 Peak->Sequence.startTimeBin - 1];
	    }

	  if(Peak->Timebin + 1 < 
	     Peak->Sequence.startTimeBin + Peak->Sequence.Length)
	    {
	      iADCPlus = Peak->Sequence.FirstAdc[Peak->Timebin - 
						 Peak->Sequence.startTimeBin + 1];
	    }
	  
	  /* check if Gaussfit will fail */
	  if((iADCValue == 0) || (iADCPlus == 0) || (iADCMinus == 0) || 
	     ((iADCValue <= iADCPlus) && (iADCValue <= iADCMinus)))
	    {
	      /* use weighted mean instead */
	      iUseGauss = 0;
	    }
	  else
	    {
	      /* do gaussfit */
	      Peak->TimeSigma = sqrt (1 / 
				((2 * fastlog[iADCValue]) -
				 (fastlog[iADCPlus] + fastlog[iADCMinus])));
	      Peak->TimePosition = (float) Peak->Timebin + 
		mParam->timeOffset(iRow*mParam->numberOfSectors()
				   *mParam->numberOfPads()+
				   iSec*mParam->numberOfPads()+Peak->pad) +
		sqr(Peak->TimeSigma) * (fastlog[iADCPlus] - fastlog[iADCMinus]);
	    }
	}

      if((iUseGauss & 2) == 0)
	{
	  /* calculate time position by weighted mean */ 
	  Peak->TimePosition = (float) TimeSum / (float) ChargeSum;
	  
	  /* calculate pad direction cluster width by weighted mean */
	  Peak->TimeSigma=0;
	  for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
	    {
	      for(iBin = 0; iBin < Cluster->Sequence[iSequence].Length; iBin++)
		{
		  Peak->TimeSigma+=(sqr((float) 
					Cluster->Sequence[iSequence].startTimeBin 
					+ (float) iBin - Peak->TimePosition)
				    *Cluster->Sequence[iSequence].FirstAdc[iBin]);
		}
	    }
	  Peak->TimeSigma /= (float) ChargeSum;
	}

      /* transform from pad/time to x/y/z */
      if(!padtrans(Peak, iRow, iSec, 
		   pRadius, pDeflection))
	{
#ifdef DEBUG
	  printf("Peak position can't be transformed!\n");
#endif
	}


      if(!isnan(Peak->x) && !isnan(Peak->y) && !isnan(Peak->PadSigma) &&
!isnan(Peak->TimeSigma))// && Peak->PeakHeight>=mParam->minimumClusterMaxADC())
	{
	  // create new point
	  Int_t numPoint = mPoint->GetEntriesFast();
	  TClonesArray &pointInit = *mPoint;
	  new(pointInit[numPoint]) StFtpcPoint();
	  StFtpcPoint *thispoint = (StFtpcPoint *) mPoint->At(numPoint);

	  thispoint->SetPadRow(iRow+1);           
	  thispoint->SetSector(iSec+1);
	  thispoint->SetNumberPads(Cluster->EndPad +1 - Cluster->StartPad);
	  thispoint->SetNumberBins(Peak->Sequence.Length);
	  thispoint->SetMaxADC(Peak->PeakHeight);
	  thispoint->SetCharge(ChargeSum);
	  thispoint->SetX(Peak->x);
	  thispoint->SetY(Peak->y);
	  thispoint->SetZ(Peak->z);
	  thispoint->SetSigmaPhi(Peak->PadSigma*mParam->radiansPerPad());
	  thispoint->SetSigmaR(Peak->TimeSigma*Peak->Rad/Peak->TimePosition);
	  fDriftLength = mParam->sensitiveVolumeOuterRadius() - Peak->Rad;
          fPhiError = mParam->padDiffusionErrors(0) 
	    + fDriftLength*mParam->padDiffusionErrors(1) 
	    + fDriftLength*fDriftLength*mParam->padDiffusionErrors(2);
          fRadError = mParam->timeDiffusionErrors(0)
	      + fDriftLength*mParam->timeDiffusionErrors(1) 
	      + fDriftLength*fDriftLength*mParam->timeDiffusionErrors(2);
	  if(thispoint->GetNumberPads()==2)
	    {
	      fPhiError = sqrt(fPhiError * fPhiError
			       + sqr(mParam->twoPadWeightedError()));
	    }
	  if(thispoint->GetNumberPads()==3 && iUseGauss & 1 == 1)
	    {
	      fPhiError = sqrt(fPhiError * fPhiError
			       + sqr(mParam->threePadGaussError()));
	    }
	  if(thispoint->GetNumberPads()==3 && iUseGauss & 1 == 0)
	    {
	      fPhiError = sqrt(fPhiError * fPhiError
			       + sqr(mParam->threePadWeightedError()));
	    }

	  thispoint->SetFlags(0);

	  if(iADCValue>254)
	    {
	      thispoint->SetFlags(4);
	      fPhiError = sqrt(fPhiError * fPhiError
			       + sqr(mParam->padSaturatedClusterError()));
	      fRadError = sqrt(fRadError * fRadError
			       + sqr(mParam->timeSaturatedClusterError()));
	    }	  
	  if(Cluster->CutOff==1)
	    {
	      thispoint->SetFlags(thispoint->GetFlags() | 16);
	      fPhiError = sqrt(fPhiError * fPhiError
			       + sqr(mParam->padCutoffClusterError()));
	      fRadError = sqrt(fRadError * fRadError
			       + sqr(mParam->timeCutoffClusterError()));
	    }

	  /* transform errors to actual hit position */ 
	  PadtransBin=(int) ((Peak->TimePosition+0.5)*PadtransPerTimebin);
	  fPhiError *= Peak->Rad / mParam->sensitiveVolumeOuterRadius();
	  fRadError *= (pRadius[10*PadtransBin]-pRadius[10*PadtransBin+10])
	    / (pRadius[10]-pRadius[20]);

	  thispoint->SetXerr(sqrt(fRadError*cos(Peak->Phi)
				  *fRadError*cos(Peak->Phi) 
				  + fPhiError*sin(Peak->Phi)
				  *fPhiError*sin(Peak->Phi)));
	  thispoint->SetYerr(sqrt(fRadError*sin(Peak->Phi)
				  *fRadError*sin(Peak->Phi) 
				  + fPhiError*cos(Peak->Phi)
				  *fPhiError*cos(Peak->Phi)));
	  thispoint->SetZerr(mParam->zDirectionError());
	}
      else
	{
#ifdef DEBUG
	  printf("Cluster fitting error. Point not stored.\n");
#endif
	}
    } /* end of: if(iNumPeaks == 1) */
  else
    {
      /* multi-peak cluster, unfold */
      /* initialize all peaks for unfolding */
      BadFit=0;
      FailedFit=0;
      for(iPeakIndex=0; iPeakIndex < iNumPeaks; iPeakIndex++)
	{
	  Peak[iPeakIndex].TimePosition = (float) Peak[iPeakIndex].Timebin;
	  Peak[iPeakIndex].Timebin_saved = Peak[iPeakIndex].Timebin;
	  Peak[iPeakIndex].PadPosition = (float) Peak[iPeakIndex].pad;
	  Peak[iPeakIndex].pad_saved = Peak[iPeakIndex].pad;
	  Peak[iPeakIndex].OldTimePosition = 0;
	  Peak[iPeakIndex].OldPadPosition = 0;
	  Peak[iPeakIndex].OldPeakHeight = 0;
	  Peak[iPeakIndex].TimeSigma = sigmat((float) Peak[iPeakIndex].Timebin);
	  Peak[iPeakIndex].PadSigma = sigmax((float) Peak[iPeakIndex].Timebin);
	  Peak[iPeakIndex].PeakHeight = 
	    (float)Peak[iPeakIndex].Sequence
	    .FirstAdc[Peak[iPeakIndex].Timebin 
		     - Peak[iPeakIndex].Sequence.startTimeBin];  
	}

      /* approximation loop */
      iNumUnfoldLoops=0;
      do 
      { /* beginning of: while((SumDeltaPos > 0.01... */
	SumDeltaPos=0;
	SumDeltaAmp=0;
	PeakHeightSum=0;

	/* loop over all peaks in cluster */ 
	for(iPeakIndex=0; iPeakIndex < iNumPeaks; iPeakIndex++)
	  {

	    do
	      {

		PeakShifted=0;
		/* get values for pad direction gaussfit */
		iADCValue=0;
		iADCPlus=0;
		iADCMinus=0;
		iADCTimePlus=0;
		iADCTimeMinus=0;
		/* find sequences on the right pads */ 
		for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
		  {
		    if((Cluster->SequencePad[iSequence] 
			== Peak[iPeakIndex].pad) && 
		       (Cluster->Sequence[iSequence].startTimeBin 
			<= Peak[iPeakIndex].Timebin) && 
		       (Cluster->Sequence[iSequence].startTimeBin + 
			Cluster->Sequence[iSequence].Length 
			> Peak[iPeakIndex].Timebin))
		      {
			iADCValue = Cluster->Sequence[iSequence]
			  .FirstAdc[Peak[iPeakIndex].Timebin
				   - Cluster->Sequence[iSequence].startTimeBin];
			/* get values for time direction gaussfit */ 
			if(Peak[iPeakIndex].Timebin > 
			   Cluster->Sequence[iSequence].startTimeBin)
			  {
			    iADCTimeMinus = Cluster->Sequence[iSequence]
			      .FirstAdc[Peak[iPeakIndex].Timebin 
				       - Cluster->Sequence[iSequence].startTimeBin - 1];
			  }
			
			if(Peak[iPeakIndex].Timebin + 1 < 
			   Cluster->Sequence[iSequence].startTimeBin 
			   + Cluster->Sequence[iSequence].Length)
			  {
			    iADCTimePlus = Cluster->Sequence[iSequence]
			      .FirstAdc[Peak[iPeakIndex].Timebin 
				       - Cluster->Sequence[iSequence].startTimeBin + 1];
			  }
			
		      }
		    if((Cluster->SequencePad[iSequence] 
			== Peak[iPeakIndex].pad -1) && 
		       (Cluster->Sequence[iSequence].startTimeBin 
			<= Peak[iPeakIndex].Timebin) && 
		       (Cluster->Sequence[iSequence].startTimeBin + 
			Cluster->Sequence[iSequence].Length 
			> Peak[iPeakIndex].Timebin))
		      {
			iADCMinus = Cluster->Sequence[iSequence]
			  .FirstAdc[Peak[iPeakIndex].Timebin
				   - Cluster->Sequence[iSequence].startTimeBin];
		      }
		    if((Cluster->SequencePad[iSequence] 
			== Peak[iPeakIndex].pad + 1) && 
		       (Cluster->Sequence[iSequence].startTimeBin 
			<= Peak[iPeakIndex].Timebin) && 
		       (Cluster->Sequence[iSequence].startTimeBin + 
			Cluster->Sequence[iSequence].Length 
			> Peak[iPeakIndex].Timebin))
		      {
			iADCPlus = Cluster->Sequence[iSequence]
			  .FirstAdc[Peak[iPeakIndex].Timebin
				   - Cluster->Sequence[iSequence].startTimeBin];
		      }
		  }
		
		/* calculate influence of other peaks */ 
		fDeltaADC=0;

		fDeltaADCPlus=0;
		fDeltaADCMinus=0;
		fDeltaADCTimePlus=0; 
		fDeltaADCTimeMinus=0;
		for(iInnerIndex=0; iInnerIndex < iNumPeaks; iInnerIndex++)
		  {
		    if(iInnerIndex != iPeakIndex)
		      {
			/* check if peaks are overlaid */
			if((Peak[iPeakIndex].pad == Peak[iInnerIndex].pad)
			   && (Peak[iPeakIndex].Timebin == Peak[iInnerIndex].Timebin))
			  {
			    /* one peak has been shifted on top of the other */
			    /* that kills the unfold procedure */ 
			    FailedFit=1;
			    break;
			  }
			
			fDeltaADC+=gauss_2d(Peak[iPeakIndex].pad, 
					    Peak[iPeakIndex].Timebin, 
					    Peak[iInnerIndex].PadPosition, 
					    Peak[iInnerIndex].PadSigma, 
					    Peak[iInnerIndex].TimePosition, 
					    Peak[iInnerIndex].TimeSigma, 
					    Peak[iInnerIndex].PeakHeight);
			fDeltaADCPlus+=gauss_2d(Peak[iPeakIndex].pad+1, 
						Peak[iPeakIndex].Timebin, 
						Peak[iInnerIndex].PadPosition, 
						Peak[iInnerIndex].PadSigma, 
						Peak[iInnerIndex].TimePosition, 
						Peak[iInnerIndex].TimeSigma, 
						Peak[iInnerIndex].PeakHeight);
			fDeltaADCMinus+=gauss_2d(Peak[iPeakIndex].pad-1, 
						 Peak[iPeakIndex].Timebin, 
						 Peak[iInnerIndex].PadPosition, 
						 Peak[iInnerIndex].PadSigma, 
						 Peak[iInnerIndex].TimePosition, 
						 Peak[iInnerIndex].TimeSigma, 
						 Peak[iInnerIndex].PeakHeight);
			fDeltaADCTimePlus+=gauss_2d(Peak[iPeakIndex].pad, 
						    Peak[iPeakIndex].Timebin+1, 
						    Peak[iInnerIndex].PadPosition, 
						    Peak[iInnerIndex].PadSigma, 
						    Peak[iInnerIndex].TimePosition, 
						    Peak[iInnerIndex].TimeSigma, 
						    Peak[iInnerIndex].PeakHeight);
			fDeltaADCTimeMinus+=gauss_2d(Peak[iPeakIndex].pad, 
						     Peak[iPeakIndex].Timebin-1, 
						     Peak[iInnerIndex].PadPosition, 
						     Peak[iInnerIndex].PadSigma, 
						     Peak[iInnerIndex].TimePosition, 
						     Peak[iInnerIndex].TimeSigma, 
						     Peak[iInnerIndex].PeakHeight);
		      }
		  }
		
		/* subtract influence of other peaks */
		iADCValue-=(int) (fDeltaADC+0.5);
		iADCPlus-=(int) (fDeltaADCPlus+0.5);
		iADCMinus-=(int) (fDeltaADCMinus+0.5);
		iADCTimePlus-=(int) (fDeltaADCTimePlus+0.5);
		iADCTimeMinus-=(int) (fDeltaADCTimeMinus+0.5);
		
		/* check if Gaussfit is possible */
		
		/* introduce minimal gradient if none is there */ 
		if(iADCValue == iADCPlus)
		  {
		    iADCPlus--;
		  }
		if(iADCValue == iADCMinus)
		  {
		    iADCMinus--;
		  }
		if(iADCValue == iADCTimePlus)
		  {
		    iADCTimePlus--;
		  }
		if(iADCValue == iADCTimeMinus)
		  {
		    iADCTimeMinus--;
		  }
		
		
		/* set all 0s to 1 to keep gauss from crashing */
                /* and iADCValue=2 if iADCValue <= 1 (09/28/98) */
		if(iADCValue <= 1)
		  {
		    iADCValue=2;
		    BadFit=1;
		  }
		if(iADCPlus <= 0)
		  {
		    iADCPlus=1;
		    BadFit=1;
		  }
		if(iADCMinus <= 0)
		  {
		    iADCMinus=1;
		    BadFit=1;
		  }
		if(iADCTimePlus <= 0)
		  {
		    iADCTimePlus=1;
		    BadFit=1;
		  }
		if(iADCTimeMinus <= 0)
		  {
		    iADCTimeMinus=1;
		    BadFit=1;
		  }
		
/*    move here to protect against iADCValue=iADCPlus=iADCMinus which
      causes Peak[..].PadSigma=Inf            09.30.98    HH,JCS   */
		if(FailedFit == 1)
		  {
		    break;
		  }
		
		/* shift if peak has moved after correction */ 
		if(iADCValue < iADCPlus)
		  {
		    Peak[iPeakIndex].pad++;
		    PeakShifted=1;
		  }
		if(iADCValue < iADCMinus && PeakShifted==0)
		  {
		    Peak[iPeakIndex].pad--;
		    PeakShifted=1;
		  }
		if(iADCValue < iADCTimePlus && PeakShifted==0)
		  {
		    Peak[iPeakIndex].Timebin++;
		    PeakShifted=1;
		  }
		if(iADCValue < iADCTimeMinus && PeakShifted==0)
		  {
		    Peak[iPeakIndex].Timebin--;
		    PeakShifted=1;
		  }
	      }
	    while(PeakShifted>0 && PeakShifted<5);
	    
	    /* recalculate peak position */
	    NewPeakHeight=iADCValue;
	    Peak[iPeakIndex].PadSigma = sqrt (1 / 
					      ((2 * fastlog[iADCValue]) -
					       (fastlog[iADCPlus] + 
						fastlog[iADCMinus])));
	    NewPadPosition = (float) Peak[iPeakIndex].pad + 
	      sqr(Peak[iPeakIndex].PadSigma) * 
	      (fastlog[iADCPlus] - fastlog[iADCMinus]);
	    Peak[iPeakIndex].TimeSigma = sqrt (1 / 
					       ((2 * fastlog[iADCValue]) -
						(fastlog[iADCTimePlus] + 
						 fastlog[iADCTimeMinus])));
	    NewTimePosition = (float) Peak[iPeakIndex].Timebin + 
	      mParam->timeOffset(iRow*mParam->numberOfSectors()
				 *mParam->numberOfPads()
				 +iSec*mParam->numberOfPads()
				 +Peak[iPeakIndex].pad) +
	      sqr(Peak[iPeakIndex].TimeSigma) * 
	      (fastlog[iADCTimePlus] - fastlog[iADCTimeMinus]);
	    
	    /* introduce inertia to break infinite loops */ 
 	    if(iNumUnfoldLoops > MAXFASTLOOPS)
 	      { 
		NewPeakHeight=(NewPeakHeight+Peak[iPeakIndex].PeakHeight)/2;
		NewPadPosition=(NewPadPosition+Peak[iPeakIndex].PadPosition)/2;
		NewTimePosition=(NewTimePosition+Peak[iPeakIndex].TimePosition)/2;
 	      } 

	    SumDeltaAmp+=fabs(NewPeakHeight - Peak[iPeakIndex].PeakHeight);
	    Peak[iPeakIndex].OldPeakHeight=Peak[iPeakIndex].PeakHeight;
	    Peak[iPeakIndex].PeakHeight=NewPeakHeight;
	    SumDeltaPos+=fabs(NewPadPosition - Peak[iPeakIndex].PadPosition);
	    Peak[iPeakIndex].OldPadPosition=Peak[iPeakIndex].PadPosition;
	    Peak[iPeakIndex].PadPosition=NewPadPosition;
	    SumDeltaPos+=fabs(NewTimePosition - Peak[iPeakIndex].TimePosition);
	    Peak[iPeakIndex].OldTimePosition=Peak[iPeakIndex].TimePosition;
	    Peak[iPeakIndex].TimePosition=NewTimePosition;
	    
	    PeakHeightSum+=NewPeakHeight;

	  } /* end of: for(iPeakIndex=0;... */ 
	iNumUnfoldLoops++;
      }
      while((SumDeltaPos > UNFOLDLIMIT || SumDeltaAmp > 1) 
	    && (iNumUnfoldLoops < MAXLOOPS || SumDeltaPos > UNFOLDFAILEDLIMIT)
	    && iNumUnfoldLoops < MAXLOOPS+1 && FailedFit == 0);
      /* try unfold loop once more if error > failedlimit */
      
      if(SumDeltaPos > UNFOLDFAILEDLIMIT || FailedFit == 1)
	{
	  /* unfolding has failed, use original peak positions */ 

	  for(iPeakIndex=0; iPeakIndex < iNumPeaks; iPeakIndex++)
	    {
	      Peak[iPeakIndex].TimePosition = (float) Peak[iPeakIndex].Timebin_saved;
	      Peak[iPeakIndex].Timebin = Peak[iPeakIndex].Timebin_saved;
	      Peak[iPeakIndex].PadPosition = (float) Peak[iPeakIndex].pad_saved;
	      Peak[iPeakIndex].pad = Peak[iPeakIndex].pad_saved;
		for(iSequence = 0; iSequence < Cluster->NumSequences; iSequence++)
		  {
		    if((Cluster->SequencePad[iSequence] 
			== Peak[iPeakIndex].pad) && 
		       (Cluster->Sequence[iSequence].startTimeBin 
			<= Peak[iPeakIndex].Timebin) && 
		       (Cluster->Sequence[iSequence].startTimeBin + 
			Cluster->Sequence[iSequence].Length 
			> Peak[iPeakIndex].Timebin))
		      {
			Peak[iPeakIndex].PeakHeight = Cluster->Sequence[iSequence]
			  .FirstAdc[Peak[iPeakIndex].Timebin
				   - Cluster->Sequence[iSequence].startTimeBin];
		      }
		  }
	    }
#ifdef DEBUG
	  printf("unfold failed!\n");
#endif
	  
	} /* end of: if(SumDeltaPos > UNFOLDFAILEDLIMIT ... */

      /* write unfolded peaks to point table */
      for(iPeakIndex=0; iPeakIndex < iNumPeaks; iPeakIndex++)
	{
	  /* transform from pad/time to x/y/z */
	  if(!padtrans(&(Peak[iPeakIndex]), iRow, iSec, 
		       pRadius, pDeflection))
	    {
#ifdef DEBUG
	      printf("Peak position can't be transformed!\n");
#endif
	    }
	  
	  /* in very complicated clusters some hits may have been unfolded
	     with errors while the rest of the cluster is okay, don't fill 
	     these hits into array: */
	  if(!isnan(Peak[iPeakIndex].x) && !isnan(Peak[iPeakIndex].y) &&
!isnan(Peak[iPeakIndex].PadSigma) && !isnan(Peak[iPeakIndex].TimeSigma))// && Peak[iPeakIndex].PeakHeight>=mParam->minimumClusterMaxADC())
	    {
	      // create new point
	      Int_t numPoint = mPoint->GetEntriesFast();
	      TClonesArray &pointInit = *mPoint;
	      new(pointInit[numPoint]) StFtpcPoint();
	      StFtpcPoint *thispoint = (StFtpcPoint *) mPoint->At(numPoint);
	      
	      thispoint->SetPadRow(iRow+1);           
	      thispoint->SetSector(iSec+1);
	      thispoint->SetNumberPads(Cluster->EndPad +1 - Cluster->StartPad);
	      thispoint->SetNumberBins(Peak[iPeakIndex].Sequence.Length);
	      thispoint->SetMaxADC(Peak[iPeakIndex].PeakHeight);
	      thispoint->SetCharge(ChargeSum*Peak[iPeakIndex].PeakHeight
				   /PeakHeightSum);
	      thispoint->SetX(Peak[iPeakIndex].x);
	      thispoint->SetY(Peak[iPeakIndex].y);
	      thispoint->SetZ(Peak[iPeakIndex].z);
	      thispoint->SetSigmaPhi(Peak[iPeakIndex].PadSigma
				     *mParam->radiansPerPad());
	      thispoint->SetSigmaR(Peak[iPeakIndex].TimeSigma
				   * Peak[iPeakIndex].Rad
				   /Peak[iPeakIndex].TimePosition);

	      fDriftLength = mParam->sensitiveVolumeOuterRadius() 
		- Peak[iPeakIndex].Rad;
	      fPhiError = mParam->padDiffusionErrors(0) 
		  + fDriftLength*mParam->padDiffusionErrors(1) 
		  + fDriftLength*fDriftLength*mParam->padDiffusionErrors(2);
	      fRadError = mParam->timeDiffusionErrors(0)
		  + fDriftLength*mParam->timeDiffusionErrors(1)
		  + fDriftLength*fDriftLength*mParam->timeDiffusionErrors(2);
	      
	      /* clusters are unfolded */ 
	      thispoint->SetFlags(1);
	      fPhiError = sqrt(fPhiError * fPhiError
			       + sqr(mParam->padUnfoldError()));
	      fRadError = sqrt(fRadError * fRadError
			       + sqr(mParam->timeUnfoldError()));
	      
	      if(iADCValue>254)
		{
		  thispoint->SetFlags(5);
		  fPhiError = sqrt(fPhiError * fPhiError
				   + sqr(mParam->padSaturatedClusterError()));
		  fRadError = sqrt(fRadError * fRadError
				   + sqr(mParam->timeSaturatedClusterError()));
		}	  
	      if(BadFit==1)
		{
		  thispoint->SetFlags(thispoint->GetFlags() | 8);
		  fPhiError = sqrt(fPhiError * fPhiError
				   + sqr(mParam->padBadFitError()));
		  fRadError = sqrt(fRadError * fRadError
				   + sqr(mParam->timeBadFitError()));
		}	  
	      if(iNumUnfoldLoops == MAXLOOPS)
		{
		  thispoint->SetFlags(thispoint->GetFlags() | 10);
		  fPhiError = sqrt(fPhiError * fPhiError
				   + sqr(mParam->padFailedFitError()));
		  fRadError = sqrt(fRadError * fRadError
				   + sqr(mParam->timeFailedFitError()));
		}	  
	      if(Cluster->CutOff==1)
		{
		  thispoint->SetFlags(thispoint->GetFlags() | 16);
		  fPhiError = sqrt(fPhiError * fPhiError
				   + sqr(mParam->padCutoffClusterError()));
		  fRadError = sqrt(fRadError * fRadError
				   + sqr(mParam->timeCutoffClusterError()));
		}
	      
	      /* transform errors to actual hit position */ 
	      PadtransBin=(int) ((Peak->TimePosition+0.5)*PadtransPerTimebin);
	      fPhiError *= Peak->Rad / mParam->sensitiveVolumeOuterRadius();
	      fRadError *= (pRadius[10*PadtransBin]-pRadius[10*PadtransBin+10])
		/ (pRadius[10]-pRadius[20]);

	      thispoint->SetXerr(sqrt(fRadError*cos(Peak[iPeakIndex].Phi)
				      *fRadError*cos(Peak[iPeakIndex].Phi) 
				      + fPhiError*sin(Peak[iPeakIndex].Phi)
				      *fPhiError*sin(Peak[iPeakIndex].Phi)));
	      thispoint->SetYerr(sqrt(fRadError*sin(Peak[iPeakIndex].Phi)
				      *fRadError*sin(Peak[iPeakIndex].Phi) 
				      + fPhiError*cos(Peak[iPeakIndex].Phi)
				      *fPhiError*cos(Peak[iPeakIndex].Phi)));
	      thispoint->SetZerr(mParam->zDirectionError());
	    }
	} /* end of: for(iPeakIndex=0;... */
    } /*end of: if(iNumPeaks==1) ... else { */
  return TRUE;
}

int StFtpcClusterFinder::padtrans(TPeak *Peak, 
				  int iRow, 
				  int iSec, 
				  double *pRadius, 
				  double *pDeflection)
{  
  int PadtransPerTimebin;
  int PadtransLower;
  float PhiDeflect, TimeCoordinate;

  /* preparatory calculations */
  TimeCoordinate = Peak->TimePosition + 0.5; /*time start at beginning of bin 0*/
  PadtransPerTimebin = (int) mParam->numberOfDriftSteps() 
    / mParam->numberOfTimebins();
  PadtransLower= (int) (TimeCoordinate*PadtransPerTimebin);

  /* linear interpolation in radius table */
  Peak->Rad = ((pRadius[iRow + mParam->numberOfPadrowsPerSide() * PadtransLower] * 
		((float) (PadtransLower+1) / (float) PadtransPerTimebin - 
		 TimeCoordinate) + pRadius[iRow + mParam->numberOfPadrowsPerSide()
					   * (PadtransLower+1)] * 
		(TimeCoordinate - (float) PadtransLower 
		 / (float) PadtransPerTimebin)) / 
	       (((float) (PadtransLower + 1) / (float) PadtransPerTimebin - 
		 (float) PadtransLower / (float) PadtransPerTimebin)));
  
  /* linear interpolation in deflection table */
  PhiDeflect = mParam->directionOfMagnetField() 
    * ((pDeflection[iRow + mParam->numberOfPadrowsPerSide() * PadtransLower]
	* ((float) (PadtransLower + 1.0) 
	 / (float) PadtransPerTimebin - TimeCoordinate) 
	+ pDeflection[iRow + mParam->numberOfPadrowsPerSide() * (PadtransLower + 1)] 
	* (TimeCoordinate - (float) PadtransLower / (float) PadtransPerTimebin))
       / (((float) (PadtransLower + 1.0) / (float) PadtransPerTimebin 
	   - (float) PadtransLower / (float) PadtransPerTimebin)));
			      
  /* calculate phi angle from pad position */
  Peak->Phi = mParam->radiansPerBoundary() / 2 
    + (Peak->PadPosition + 0.5) * mParam->radiansPerPad()
    - PhiDeflect + iSec * (mParam->numberOfPads() * mParam->radiansPerPad()
			   + mParam->radiansPerBoundary())+C_PI_2;


  /* transform to cartesian */
  Peak->x = Peak->Rad*cos(Peak->Phi);
  Peak->y = Peak->Rad*sin(Peak->Phi);
  Peak->z = mParam->padrowZPosition(iRow);
  return TRUE;
}

float StFtpcClusterFinder::gauss_2d(int x, int y, float x1, float sigma_x1, float y1, float sigma_y1, float amp1)
{
  float result;
  float g1, g2;
  
  if(sigma_x1==0 || sigma_y1==0)
    {
      return 0.0;
    }

  g1=exp(-sqr((float)x-x1)/(2*sqr(sigma_x1)));
  g2=exp(-sqr((float)y-y1)/(2*sqr(sigma_y1)));
  
  result=amp1*g1*g2;
  
  return result;
}

float StFtpcClusterFinder::sigmax(float timebin)
{
  return 0.9;
}

float StFtpcClusterFinder::sigmat(float timebin)
{
  return 0.6;
}

int StFtpcClusterFinder::calcpadtrans(double *pradius, 
				      double *pdeflection)
{
  int i, j, v_buf, padrow;
  double t_last, t_next, r_last, r_next, e_now, v_now, psi_now;
  double step_size, deltap;
  
  step_size=((float) mParam->numberOfTimebins()
	     / (float) mParam->numberOfDriftSteps());
  deltap=mParam->normalizedNowPressure()-mParam->standardPressure();
  
#ifdef DEBUG
  printf("integrating padtrans table...\n");
#endif

  for (padrow=0; padrow<mParam->numberOfPadrowsPerSide(); padrow++)
    {
      /* determine starting values */
      t_last=0;
      v_buf=0;
      r_last=mParam->sensitiveVolumeOuterRadius();
      pradius[padrow]=mParam->sensitiveVolumeOuterRadius();
      pdeflection[padrow]=0;
      e_now = mParam->radiusTimesField() / (0.5*r_last);
      for(j=v_buf; mParam->padtransEField(j) < e_now
	    && j<mParam->numberOfPadtransBins(); j++);
      if(j<1 || j>=mParam->numberOfPadtransBins())
	{
	  printf("Error 1: j=%d, v_buf=%d e_drift=%f, e_now=%f\n", 
		 j, v_buf, mParam->padtransEField(j), e_now);
	  return FALSE;
	}
      v_buf=j-1;
      v_now=((mParam->padtransVDrift(v_buf, padrow)
	      +deltap*mParam->padtransdVDriftdP(v_buf, padrow))
	     *(mParam->padtransEField(j)-e_now)
	     +(mParam->padtransVDrift(j, padrow)
	       +deltap*mParam->padtransdVDriftdP(j, padrow))
	     *(e_now-mParam->padtransEField(v_buf)))
	/(mParam->padtransEField(j)-mParam->padtransEField(v_buf));
      psi_now=((mParam->padtransDeflection(v_buf,padrow)
		+deltap*mParam->padtransdDeflectiondP(v_buf,padrow))
	       *(mParam->padtransEField(j)-e_now)
	       +(mParam->padtransDeflection(j,padrow)
		 +deltap*mParam->padtransdDeflectiondP(j,padrow))
	       *(e_now-mParam->padtransEField(v_buf)))
	/(mParam->padtransEField(j)-mParam->padtransEField(v_buf));
      for (i=0; i<mParam->numberOfDriftSteps() 
	     && e_now < mParam->padtransEField(mParam->numberOfPadtransBins()-2)
	     ; i++) 
	{
	  t_next = t_last + step_size;
	  /* first guess for r_next: */
	  r_next = r_last - v_now * step_size * mParam->microsecondsPerTimebin();
	  e_now = mParam->radiusTimesField() / (0.5*(r_last+r_next));
	  
	  for(j=v_buf; mParam->padtransEField(j) < e_now 
		       && j<mParam->numberOfPadtransBins(); j++);
	  
	  if(j<1 || j>=mParam->numberOfPadtransBins())
	    {
	      printf("Error 2: j=%d, v_buf=%d e_drift=%f, e_now=%f\n", 
		     j, v_buf, mParam->padtransEField(j), e_now);
	      return FALSE;
	    }
	  
	  v_buf=j-1;
	  v_now=((mParam->padtransVDrift(v_buf, padrow)
		  +deltap*mParam->padtransdVDriftdP(v_buf, padrow))
		 *(mParam->padtransEField(j)-e_now)
		 +(mParam->padtransVDrift(j, padrow)
		   +deltap*mParam->padtransdVDriftdP(j, padrow))
		 *(e_now-mParam->padtransEField(v_buf)))
	  /(mParam->padtransEField(j)-mParam->padtransEField(v_buf));
	  psi_now=((mParam->padtransDeflection(v_buf,padrow)
		    +deltap*mParam->padtransdDeflectiondP(v_buf,padrow))
		   *(mParam->padtransEField(j)-e_now)
		   +(mParam->padtransDeflection(j,padrow)
		     +deltap*mParam->padtransdDeflectiondP(j,padrow))
		   *(e_now-mParam->padtransEField(v_buf)))
	  /(mParam->padtransEField(j)-mParam->padtransEField(v_buf));
	  
	  /* correct r_next: */
	  r_next = r_last - v_now * step_size *mParam->microsecondsPerTimebin();
	  pradius[padrow+mParam->numberOfPadrowsPerSide()*(i+1)]=r_next;
	  pdeflection[padrow+mParam->numberOfPadrowsPerSide()*(i+1)]
	    =pdeflection[padrow+mParam->numberOfPadrowsPerSide()*i]
	    +((r_last-r_next)*tan(mParam->radiansPerDegree() * psi_now)/r_last);
	  t_last=t_next;
	  r_last=r_next;
	}
#ifdef DEBUG
      printf("%d steps calculated, padrow %d\n", i, padrow);
#endif
      
    }
  return TRUE;
}

int StFtpcClusterFinder::cucInit(TClusterUC memory[MAXNUMCUC], int RealMemory[MAXNUMCUC], int *pointer)
{
  int i;
  
  for(i=0; i<MAXNUMCUC; i++)
    {
      RealMemory[i]=i;
    }
  *pointer = -1;
  return TRUE;
}

TClusterUC *StFtpcClusterFinder::cucAlloc(TClusterUC memory[MAXNUMCUC], int RealMemory[MAXNUMCUC], int *pointer)
{

  if((*pointer)+1 < MAXNUMCUC)
    {
      (*pointer)++;
      memory[RealMemory[*pointer]].MemoryPtr = RealMemory[*pointer];
      return &memory[RealMemory[*pointer]];
    }
  else
    {
#ifdef DEBUG
      printf("CUC memory exhausted! requested %d CUCs\n", *pointer);
#endif
      return NULL;
    }
}

int StFtpcClusterFinder::cucFree(TClusterUC memory[MAXNUMCUC], int RealMemory[MAXNUMCUC], int *pointer, TClusterUC *OldCUC)
{
  if(*pointer >= 0)
    {
      RealMemory[*pointer] = OldCUC->MemoryPtr;
      (*pointer)--;
      return TRUE;
    }
  else
    {
#ifdef DEBUG
      printf("CUC memory management confused!\n");
#endif
      return FALSE;
    }
}
