/* STAR browser per VAT conference.  May 16 1995, Herb Ward. */
/***********************************************************  INCLUDES  **/
#include <stdio.h>
#include <math.h>

#ifndef WIN32
# include <strings.h>
#endif

#include <stdlib.h>
#include "dstype.h"
#include "dsuType.h"
#include "asuAlloc.h"
#include "emlLib.h"
#include "tbr_types.h"
#include "dsxdr.h"
#include "ds.h"
#ifdef EXTERN
#undef EXTERN
#endif
#define EXTERN
#include "brow.h"
#include "dscuts.h"
extern char dsu_gStr[DSU_SIZE_OF_GSTR];
void Sss(char *mess);
int gAlreadyErr;
extern int dsu_gDone;
char *gTheBlurb="\
NOT CRITICAL, CASUAL USERS CAN IGNORE:\n\
   At certain points this program tries to enlarge a window.  It will fail\n\
unless you do the following on your X server:\n\
   STEP 1: put this line into file \"$HOME/.Xdefaults\":\n\
starbrowser*allowShellResize: True\n\
   STEP 2: Put this into your .login:\n\
xrdb .Xdefaults\n\
You can look for xrdb in /usr/bin/X11.\n\
   If you are using an X terminal which does not login (like an NCD) consult\n\
the documentation on setting \"resources\".\n\
";
void Ose(void);
/***********************************************************  FUNCTIONS  **/
int TableHasMoreThanZeroRows(int tlm) {
  size_t nRow;
  if(!dsTableRowCount(&nRow,gDs[tlm]->dsPtr)) dsu_Err(938);
  if(nRow>0) return TRUE;
  Sss("Please do not select\ntables that have zero rows."); return FALSE;
}
int TableHasMoreThanZeroCols(int tlm) {
  size_t nCol;
  if(!dsTableColumnCount(&nCol,gDs[tlm]->dsPtr)) dsu_Err(538);
  if(nCol>0) return TRUE;
  Sss("Please do not select\ntables that have zero columns."); return FALSE;
}
void ExpandExceptCase(int wds) {
  char *x,c;
  if(gDs[wds]->triStat==TRI_LEAF) dsu_Err( 59);
  if(gDs[wds]->type==TYPE_DS_TABLE) return;
  x=gDs[wds]->triText; c=x[0];
  if(c>='a'&&c<='z') {
    x[0]+='Z'-'z'; x[1]+='Z'-'z'; gDs[wds]->triStat=TRI_EXPANDED;
  }
}
void ExpandCase(int wds) {
  char *x,c;
  if(gDs[wds]->triStat==TRI_LEAF) dsu_Err( 52);
  x=gDs[wds]->triText; c=x[0];
  if(c>='a'&&c<='z') {
    x[0]+='Z'-'z'; x[1]+='Z'-'z'; gDs[wds]->triStat=TRI_EXPANDED;
  }
}
void ContractCase(int wds) {
  char *x,c;
  if(gDs[wds]->triStat==TRI_LEAF) dsu_Err( 53);
  x=gDs[wds]->triText; c=x[0];
  if(c>='A'&&c<='Z') {
    x[0]+='z'-'Z'; x[1]+='z'-'Z'; gDs[wds]->triStat=TRI_CONTRACTED;
  }
}
int ToggleCase(int wds) {
  char *x,c;
  if(gDs[wds]->triStat==TRI_LEAF) dsu_Err( 54);
  x=gDs[wds]->triText; c=x[0];
  if(c>='a'&&c<='z') {
    x[0]+='Z'-'z'; x[1]+='Z'-'z'; gDs[wds]->triStat=TRI_EXPANDED;
  } else {
    x[0]+='z'-'Z'; x[1]+='z'-'Z'; gDs[wds]->triStat=TRI_CONTRACTED;
  }
  return TRUE;
}
  /* uu4 Gets various data for a column.  O=output, I=input.
  ** The term "subscript" refers to indexing within a column whose data
  ** type is an array.  Returns FALSE for error.
  ** input  dsPtr -- table pointer (dsl)
  ** input  colNum -- tab column index (0 for first column, etc)
  ** input  tentSubscript  -- what caller thinks subscript should be
  ** output off -- subscript (corrected if necessary)
  ** output dim -- dimensionality of column (0=scalar, 1=vector, 2=illegal)
  ** output array_size_t  -- array size (max for subscript (+1))
  ** output typeName  -- data type (long, float, etc)
  */
int ATable(int x) {
  if(gDs[x]->type==TYPE_DS_TABLE) return TRUE;
  return FALSE;
}
int Array(
  DS_DATASET_T *dsPtr, size_t colNum,int tentSubscript,
  int *off, size_t *dim, size_t *array_size_t, char **typeName) {
  /* see comment uu4 */
  if(!dsColumnTypeName((const char**)typeName,dsPtr,colNum))	dsu_Err( 17);
  if(!dsColumnDimCount(dim,dsPtr,colNum))		dsu_Err( 18);
  if(!dsColumnDimensions(array_size_t,dsPtr,colNum))	dsu_Err( 19);
  if(*dim>0) { *off=tentSubscript; if(*off<0||*off>=*array_size_t) dsu_Err( 20); }
  else *off=0;
  return TRUE;
}




















































static float dsu_Value(int *dt,DS_DATASET_T *tp,size_t colNum,int row,int ss) {
  size_t row_size_t; float fv; long iv; int dataType;
  row_size_t=row;
  if(!TableValue(&dataType,dsu_gStr,&fv,&iv,row_size_t,colNum,tp,ss))
      gTableValueError=TRUE; else gTableValueError=FALSE;
  if(gTableValueError) return 0.0;
  *dt=dataType;
  if(dataType==DSU_FLOAT)   return fv;
  else if(dataType==DSU_INTEGER) { fv=iv; return fv; }
  else if(dataType==DSU_STRING)  return 0;
  else if(dataType==DSU_HEX) { fv=iv; return fv; }
  else dsu_Err( 43);

  return STAFCV_BAD;
}
float ValueWrapper(int wh_gDs,size_t colNum,int row,int subscript) {
  int dt;  /* dt = Data Type */
  float rv;
  rv=dsu_Value(&dt,gDs[wh_gDs]->dsPtr,colNum,row,subscript);
  if(dt==DSU_STRING)                  { gVWType=VWSTRING; return 0.0; }
  else if(dt==DSU_INTEGER||dt==DSU_FLOAT) { gVWType=VWNUMBER; return rv; }
  else if(dt==DSU_HEX)                { gVWType=VWHEX; return rv; }
  else dsu_Err(552);

  return STAFCV_BAD;
}
int DoCutsWrapper(int max8,char *ba,char *cuts,int wh_gDs) {
  if(dsuDoCuts(max8,ba,cuts,gDs[wh_gDs]->dsPtr)) 
    return TRUE;
  else 
    return FALSE;
}
void SetToDatasetInfo(int wh_gDs,char *xx,int max) { /* BBB max unused */
  char buf[100]; size_t nCol;
  strcpy(xx,"June 29 1995"); return;
#if 0
  strcpy(xx,"**** MIDDLE WINDOW: CHOOSE TABLE ****\n");
  sprintf(buf,"        Dataset Name: %s\n",gDs[wh_gDs]->name);
  strcat(xx,buf);
  sprintf(buf,"         Parent Name: %s\n",gDs[wh_gDs]->parentName);
  strcat(xx,buf);
  if(!dsDatasetEntryCount(&nCol,gDs[wh_gDs]->dsPtr)) dsu_Err( 36);
  sprintf(buf,"    Number of Tables: %d\n",nCol); strcat(xx,buf);
  /* if(!dsDatasetMaxEntryCount(&nCol,gDs[wh_gDs]->dsPtr)) dsu_Err( 37); */
  sprintf(buf,"Max Number of Tables: %s\n","???"); strcat(xx,buf);
#endif
}
void TableName(int wh_gDs,char *name) {
  strcpy(name,gDs[wh_gDs]->name);
}
void SetToTableInfo(char *name,size_t *nRow,int wh_gDs,char *xx,int max) {
  /* BBB max unused */
  char buf[100]; size_t nCol;
  strcpy(xx,"Choose column(s) below.\n");
  sprintf(buf ," Table %s\n",gDs[wh_gDs]->name); strcat(xx,buf);
  strcpy(name,gDs[wh_gDs]->name);
  sprintf(buf,"Parent %s\n",gDs[wh_gDs]->parentName);
  strcat(xx,buf);
  if(!dsTableColumnCount(&nCol,gDs[wh_gDs]->dsPtr)) dsu_Err( 38);
  sprintf(buf," #Cols %d\n",nCol); strcat(xx,buf);
  sprintf(buf,"  Type %s\n","????"); strcat(xx,buf);
  if(!dsTableRowSize(&nCol,gDs[wh_gDs]->dsPtr)) dsu_Err( 29);
  sprintf(buf,"  Size %d bytes/row\n",nCol); strcat(xx,buf);
  if(! dsTableRowCount(nRow,gDs[wh_gDs]->dsPtr)) dsu_Err( 30);
  sprintf(buf," #Rows %d\n",*nRow); strcat(xx,buf);
  /*------------------------------------------------------
  sprintf(buf,COLUMN_LIST_FORMAT,"COLUMN","TYPE","MINIMUM","MAXIMUM",
  "AVERAGE","STD_DEV"); strcat(xx,buf);
  -----------------------------------------------------*/
}
void dsu_Err(int x) {
  if(gAlreadyErr) return;
  gAlreadyErr=7;
  PP"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  PP"Table browser fatal error number %d.\n",x);
  PP"Send email to ward@physics.utexas.edu\n");
  PP"If possible, point me to the tables and situation that caused\n");
  PP"this error.\n");
  dsu_gDone=7; return;
}
void TouchUpType(void) {
  int ii;
  for(ii=gNDs-1;ii>0;ii--) {
    if(gDs[ii]->type==TYPE_DS_TABLE&&gDs[ii-1]->type==TYPE_DS_DATASETS) {
      gDs[ii-1]->type=TYPE_DS_DATASET; /* has tables instead of datasets */
    }
  }
}
void FillgDs(char *parentName,DS_DATASET_T *dsPtr) {
  int isDataset,isTable; size_t scr,numEntries,entryNumber; const char *name;
  DS_DATASET_T *local;
  gIndent++;
  if(!dsIsDataset(&isDataset,dsPtr))            dsu_Err(  4);
  if(!dsIsTable(&isTable,dsPtr))                dsu_Err(  5);
  if( isDataset&& isTable)                      dsu_Err(  1);
  if(!isDataset&&!isTable)                      dsu_Err(  2);
  if(isDataset) if(!dsDatasetName(&name,dsPtr)) dsu_Err(  9);
  if(isTable)   if(!dsTableName(&name,dsPtr))   dsu_Err( 10);
  if((int)strlen(name)>DATASET_NAME_SIZE)       dsu_Err( 12);
  if(gNDs>=MAX_DATASET)                         dsu_Err(  0);
  gDs[gNDs]=malloc(sizeof(DATASET_INFO));
  if(gDs[gNDs]==NULL)				dsu_Err( 11);
  strcpy(gDs[gNDs]->name  ,name);
  strcpy(gDs[gNDs]->parentName  ,parentName);
         gDs[gNDs]->dsPtr =dsPtr;
         gDs[gNDs]->indent=gIndent;
  if(isTable) {
    gDs[gNDs]->type=TYPE_DS_TABLE;
    dsTableRowCount(&scr,dsPtr);
  } else {
    gDs[gNDs]->type=TYPE_DS_DATASETS;
  }
  gNDs++;
  if(isDataset) {
    strcpy(gDs[gNDs-1]->triText,"ds");
    gDs[gNDs-1]->triStat=TRI_CONTRACTED;
    if(!dsDatasetEntryCount(&numEntries,dsPtr)) dsu_Err(  8);
    for(entryNumber=0;entryNumber<numEntries;entryNumber++) {
      if(!dsDatasetEntry(&local,dsPtr,entryNumber)) dsu_Err(  6);
      FillgDs((char*)name,local); /* recursive */
    }
  } else {
    strcpy(gDs[gNDs-1]->triText,"tb");
    gDs[gNDs-1]->triStat=TRI_CONTRACTED;
  }
  gIndent--;
}
float MinMax(int control,DS_DATASET_T *pp,size_t whichCol,int subscript) {
  size_t nRow_size_t; int dataType,nRowInt,rowInt; float rv,cv;
  if(!dsTableRowCount(&nRow_size_t,pp)) dsu_Err( 35);
  nRowInt=nRow_size_t;
  if(nRowInt<=0) return 0.0;
  if(control==0) rv=+1e28; else rv=-1e28;
  for(rowInt=nRowInt-1;rowInt>=0;rowInt--) {
    cv=dsu_Value(&dataType,pp,whichCol,rowInt,subscript);
    gDataType=dataType;
    if(dataType==DSU_STRING||dataType==DSU_HEX) rv=0.0;
    else {
      if(gTableValueError) {
        Sss("Error 91t: This table has\nunsupported data types."); return 0;
      }
      if(control==0) { if(rv>cv) rv=cv; }
      else           { if(rv<cv) rv=cv; }
    }
  }
  return rv;
}
float dsu_Ave(float *stdDev,DS_DATASET_T *pp,size_t whichCol,int subscript) {
  size_t nRow_size_t; int dataType,nRowInt,rowInt; float ave,cv,sum2;
  if(!dsTableRowCount(&nRow_size_t,pp)) dsu_Err( 24);
  nRowInt=nRow_size_t;
  if(nRowInt<=0) { *stdDev=0.0; return 0.0; }
  ave=0.0;
  for(rowInt=nRowInt-1;rowInt>=0;rowInt--) {
    ave+=dsu_Value(&dataType,pp,whichCol,rowInt,subscript);
    gDataType=dataType;
    if(dataType==DSU_STRING||dataType==DSU_HEX) { *stdDev=0; return 0.0; }
    if(gTableValueError) {
      Sss("Error 92t: This table has\nunsupported data types."); return 0;
    }
  } ave/=nRowInt;
  sum2=0;
  for(rowInt=nRowInt-1;rowInt>=0;rowInt--) {
    cv=dsu_Value(&dataType,pp,whichCol,rowInt,subscript);
    if(dataType==DSU_STRING||dataType==DSU_HEX) dsu_Err(999);
    if(gTableValueError) {
      Sss("Error 93t: This table has\nunsupported data types."); return 0;
    }
    sum2+=(ave-cv)*(ave-cv);
  } *stdDev=sqrt(sum2/nRowInt);
  return ave;
}
void Format(int width,char *x,float y) {
  int ii,len;
  char format[22];
  if(y==0) { strcpy(x,"0"); return; }
  if(y>1e4||y<-1e4||(y<0.1&&y>-0.1)) {
    sprintf(format,"%%%d.2e",width); sprintf(x,format,y);
  } else {
    sprintf(format,"%%%d.3f",width); sprintf(x,format,y);
  }
  len=strlen(x);
  for(ii=len-1;ii>=0;ii--) {
    if(x[ii]=='.') { x[ii]='\0'; return; } if(x[ii]!='0') break;
  }
}
/* #define FAST_UP */
void MakeAbbreviations(char *out,const char *in) {
  *out=0;
  if(!strcmp(in,"unsigned short")) strcpy(out,"ushort");
  if(!strcmp(in,"unsigned long")) strcpy(out,"ulong");
  if(*out==0) strcpy(out,in);
  if(strlen(out)>7) out[7]='\0';
}
#define CONST 160
#define SLOPE 63
void ColumnList(char reportSpaceNeeded,int *nbytes,char *header,
  int wh_gDs,int *tot,int *tlm,int ml,char *xx,int max,
  int *subscript) {
  int asi,jj,rr,lineCnt=0,ii; char gg[25],ss[20],*dd,*cc,bf2[100],doBreak=0;
  size_t whichCol,rr_size_t,dimensionality,arraySize;
  float minFloat,maxFloat,aveFloat,stdFloat;
  char typeA[100],totalName[100];
  char minString[FORMAT],maxString[FORMAT],aveString[FORMAT],stdString[FORMAT];
  DS_DATASET_T *pp; int progNow=0,progTot=0;
  pp=gDs[wh_gDs]->dsPtr;
  if(gDs[wh_gDs]->type!=TYPE_DS_TABLE) dsu_Err( 25);
  if(!dsTableColumnCount(&rr_size_t,pp)) dsu_Err( 26); rr=rr_size_t;
  sprintf(header,COLUMN_LIST_FORMAT,"ColumnName","Type","Minimum",
      "Maximum","Average","StdDev");
  if(!gCalculateAverages) {
    strcpy(minString,"???"); strcpy(maxString,"???");
    strcpy(stdString,"???"); strcpy(aveString,"???");
  }
  if(reportSpaceNeeded||gCalculateAverages) {
    if(!reportSpaceNeeded) tbrProgress(-5,10,"ferences\" menu to speed this up.",
    "Doing min/max/averages.  See the \"Pre-");
    for(ii=0;ii<rr;ii++) { /* 1st pass for progTot */
      whichCol=ii;
      if(!dsColumnDimCount(&dimensionality,pp,whichCol)) dsu_Err( 56);
      if(dimensionality>1) dsu_Err( 57);
      if(!dsColumnDimensions(&arraySize,pp,whichCol)) dsu_Err( 58);
      if(dimensionality==0) arraySize=1;
      asi=arraySize;
      for(jj=1;jj<=asi;jj++) {
        progTot++;
        if(jj==1) MinMax(0,pp,whichCol,jj-1); /* to set gDataType */
        if(gDataType==DSU_STRING) break; /* See comment 66d. */
      }
      if(gTableValueError) break;
    }
  }
  if(reportSpaceNeeded) { /* nbytes will return soon as max, so we have  */
    *nbytes=CONST+SLOPE*progTot; return; /* a good check. */
  }
  *xx='\0';
  for(ii=0;ii<rr;ii++) { /* 2nd pass, loop over columns */
    if(gTableValueError) break;
    whichCol=ii;
    if(!dsColumnDimCount(&dimensionality,pp,whichCol)) dsu_Err( 39);
    if(dimensionality>1) dsu_Err( 40);
    if(!dsColumnDimensions(&arraySize,pp,whichCol)) dsu_Err( 41);
    if(dimensionality==0) arraySize=1;
    if(!dsColumnName((const char**)&cc,pp,whichCol)) dsu_Err( 27);
    if(!dsColumnTypeName((const char**)&dd,pp,whichCol)) dsu_Err( 28);
    MakeAbbreviations(typeA,dd);
    asi=arraySize;
    for(jj=1;jj<=asi;jj++) { /* loop over array members */
      if(gCalculateAverages) {
        tbrProgress(progNow++,progTot,NULL,NULL);
        minFloat=MinMax(0,pp,whichCol,jj-1); if(gTableValueError) break;
        maxFloat=MinMax(1,pp,whichCol,jj-1); if(gTableValueError) break;
        aveFloat=dsu_Ave(&stdFloat,pp,whichCol,jj-1); if(gTableValueError) break;
        if(gDataType==DSU_STRING||gDataType==DSU_HEX) {
          strcpy(minString,"-----"); strcpy(maxString,"-----");
          strcpy(stdString,"-----"); strcpy(aveString,"-----");
        } else {
          Format(7,minString,minFloat); Format(7,maxString,maxFloat);
          Format(7,stdString,stdFloat); Format(7,aveString,aveFloat);
        }
      } else {
        if(jj==1) MinMax(0,pp,whichCol,jj-1); /* to set gDataType */
      }
      strcpy(totalName,cc);
      if(gDataType!=DSU_STRING&&dimensionality==1) sprintf(ss,"(%d",jj);
      else *ss='\0'; /* asi is len of string if(gDataType==DSU_STRING) */
      /* PP"minString=%s, ss=%s.\n",minString,ss); */
      strcat(totalName,ss);
      if(strcmp(typeA,"char")) strcpy(gg,typeA); else strcpy(gg,"string");
      sprintf(bf2,COLUMN_LIST_FORMAT,totalName,gg,minString,maxString,
      aveString,stdString);
      if(strlen(xx)+strlen(bf2)>max-SLACK) {
        PP"ii%d,jj%d,rr%d,asi%d,max%d,strl(xx)%d,strlen(bf2)%d,SLACK%d,\n",
        ii,jj,rr,asi,max,strlen(xx),strlen(bf2),SLACK);
        dsu_Err( 16); doBreak=7; break;
      }
      strcat(xx,bf2);
      /* PP"(%2d,%2d) len=%4d bf2=%s\n",ii,jj,strlen(xx),bf2); Sleep(2); */
      if(lineCnt>=ml) {
        PP"Table browser error.\n");
        PP"lineCnt=%d, maxLines=%d.\n",lineCnt,ml); dsu_gDone=7; return;
      }
      tlm[lineCnt]=ii; /* see 6gg */
      subscript[lineCnt]=jj-1; /* www */
      strcat(xx,"\n"); /* using the SLACK */
      lineCnt++;
      if(gDataType==DSU_STRING) break; /* asi=len of data string in dsl 66d */
    }           /* loop over array members */
    if(gTableValueError) break;
    if(doBreak) break;
  }	/* loop over columns */
  if(gCalculateAverages) tbrProgress(-10,progTot,NULL,NULL);
  *tot=lineCnt;
}
  /* 6gg: Unlike pRIMARYlIST & dATASETlIST, this is not an index to gDs[].
  ** You can search for tlm in x.c to get a list of uses. */
void PrimaryList(char *header,  /* goes with WIN_TYPE_PRIMARY June 28 1995 */
  int wh_gDs,int *tot,int *wh_DsA,int ml,char *xx,int max) {
  int fo,freeze,isTable; char indentStr[83],dashes[60];
  int nextIndent,currIndent,freezeLevel;
  int off,lineCnt=0,ii; DS_DATASET_T *pp; size_t colIdx,nCol,val;
  char *colName,oneLine[200],xName[100],xCol[100],xRow[100],xMrw[100];
  *xx='\0';
  /* June 28 1995
  sprintf(header,TABLE_LIST_FORMAT,"TableName","nCOL","nROW","maxRow"); */
  *header='\0';
  if(wh_gDs!=0) dsu_Err(229); /* June 28 1995 */
  freeze=FALSE;
  for(ii=wh_gDs;ii<gNDs;ii++) { /* June 28 1995, removed +1 from wh_gDs */
    currIndent=gDs[ii]->indent;
    if(ii<gNDs-1) nextIndent=gDs[ii+1]->indent; else nextIndent=currIndent;
    if(currIndent<freezeLevel) freeze=FALSE;
    if(freeze) continue;
    if(gDs[ii]->triStat==TRI_CONTRACTED&&nextIndent>currIndent) {
      freeze=TRUE; freezeLevel=nextIndent;
    }
    pp=gDs[ii]->dsPtr;
    /* if(gDs[ii]->type!=TYPE_DS_TABLE) break; June 28 1995 */
    sprintf(xName,"%s",gDs[ii]->name);
    if(!dsIsTable(&isTable,pp))                dsu_Err( 42);
    if(isTable) {
      if(!dsTableColumnCount(&nCol,pp)) dsu_Err( 31);
      sprintf(xCol,"%d cols,",nCol);
      if(!dsTableRowCount(&val,pp)) dsu_Err( 32); sprintf(xRow,"%d rows.",val);
      if(!dsTableMaxRowCount(&val,pp)) dsu_Err( 33); sprintf(xMrw,"%d",val);
      strcpy(xMrw,"");
      strcpy(dashes,"-------------------------------------------------");
      off= T41-currIndent*2-strlen(xName)
      -strlen(xCol)-strlen(xRow)-strlen(xMrw)-6; dashes[off]='\0';
      if(lineCnt>=MAX_LINES_CLICK_PART) dsu_Err( 45);
    } else {
      strcpy(xCol,""); strcpy(xRow,""); strcpy(xMrw,""); dashes[0]='\0';
      if(lineCnt>=MAX_LINES_CLICK_PART) dsu_Err(707);
    }
    strcpy(indentStr,"                                                ");
    indentStr[currIndent*2]='\0';
    sprintf(oneLine,"%s%s %s %s %s %s %s",indentStr,gDs[ii]->triText,
    xName,dashes,xCol,xRow,xMrw);
    if(strlen(xx)+strlen(oneLine)>max-SLACK) dsu_Err( 15); strcat(xx,oneLine);
    if(lineCnt>=ml) dsu_Err( 55);
    wh_DsA[lineCnt]=ii; /* recursion of a sort, note arg wh_gDs */
    strcat(xx,"\n"); /* using the SLACK */
    lineCnt++;
    if(gDs[ii]->triStat==TRI_EXPANDED&&isTable) {
      fo=TRUE;
      strcpy(indentStr,"                                                ");
      indentStr[currIndent*2+2]='\0';
      for(colIdx=0;colIdx<nCol;colIdx++) {
        if(!dsColumnName((const char**)&colName,pp,colIdx)) dsu_Err( 46);
        sprintf(oneLine,"%sCO %s\n",indentStr,colName);
        if(strlen(xx)+strlen(oneLine)>max-SLACK) dsu_Err( 47); strcat(xx,oneLine);
        if(lineCnt>=ml) dsu_Err( 34);
        wh_DsA[lineCnt]=-19; lineCnt++;
      }
    }
  } /* loop over all gDs (includes tables, wh_gDs=0 6/28/95) */
  *tot=lineCnt;
}
void DatasetList(int *tot,int *wh_DsA,int maxLines,char *out,int max) {
  int lineCnt=0,ii; char bf2[100],buf[50];
  *out='\0';
  for(ii=0;ii<gNDs;ii++) {
    if(gDs[ii]->type==TYPE_DS_TABLE) continue;
    *bf2='\0'; for(ii=0;ii<gDs[ii]->indent;ii++) strcat(bf2," ");
    sprintf(buf,"%s",gDs[ii]->name);
    strcat(bf2,buf);
    if(gDs[ii]->type==TYPE_DS_DATASETS) strcat(bf2,", contains datasets");
    else if(gDs[ii]->type==TYPE_DS_DATASET) strcat(bf2,", contains tables");
    else dsu_Err( 13);
    if(strlen(out)+strlen(bf2)>max-SLACK) dsu_Err( 14); strcat(out,bf2);
    if(lineCnt>=maxLines) {
      PP"Error in the table browser error.\n");
      PP"lineCnt=%d, maxLines=%d.\n",lineCnt,maxLines); dsu_gDone=7; return;
    }
    wh_DsA[lineCnt]=ii; strcat(out,"\n"); /*using SLACK*/ lineCnt++;
  }
  *tot=lineCnt;
}
void DumpDatasetInfo(void) {
  int ii;
  for(ii=0;ii<gNDs;ii++) {
    PP"---------------------------------------------\n");
    PP"name  = %s\n",gDs[ii]->name);
    PP"parentname  = %s\n",gDs[ii]->parentName);
    PP"dsPtr = %p\n",gDs[ii]->dsPtr);
    PP"indent = %d\n",gDs[ii]->indent);
    PP"type = %d (see TYPE_DS_XXX)\n",gDs[ii]->type);
  }
}
void dsu_Blurb(void) {
#ifndef QUIET_ASP
 Sss(gTheBlurb);
#endif
}
void tbrNewDSTree(DS_DATASET_T **dsPtr,long nDsPtr) {
  PP"Function tbrNewDTree() in the table browser does not work yet.\n");
}
void tbrNewTbView(DS_DATASET_T *dsPtr) {

  /********************************************************
  ** Calling this function before gMainWindow is defined is probably
  ** work-aroundable.  However, the fact that the number of the primary
  ** window is hardcoded as 0 presents more of a hurdle.  Also there is
  ** the problem of entering the main loop in a way that would be compat-
  ** ible with future forking plans (for immediate return to TAS prompt)
  ** and arbitrary calls to tbrNewDSView() [the basic function].
  ************************************************************/

  PP"Function tbrNewTbView() in the table browser does not work yet.\n");

}
void UpdateUsageLog(void) {
  FILE *gg,*ff; char line[50]; int len;
  system("date   > UiR77xEe3T 2> /dev/null");
  system("whoami > UiR77xEe3t 2> /dev/null");
  ff=fopen("UiR77xEe3T","r"); if(ff!=NULL) {
    if(fgets(line,48,ff)) {
      len=strlen(line); line[len-1]='\0'; gg=fopen("/u2/ward/tbr.log","a");
      if(gg!=NULL) { fprintf(gg,"%s ",line); fclose(gg); }
    } fclose(ff);
  }
  ff=fopen("UiR77xEe3t","r"); if(ff!=NULL) {
    if(fgets(line,48,ff)) {
      gg=fopen("/u2/ward/tbr.log","a");
      if(gg!=NULL) { fprintf(gg,"%s ",line); fclose(gg); }
    } fclose(ff);
  }
  system("rm UiR77xEe3T 2> /dev/null"); system("rm UiR77xEe3t 2> /dev/null");
}
/* float dsu_Value(int *dt,DS_DATASET_T *tp,size_t colNum,int row,int ss); */
void tbrNewDSView(DS_DATASET_T **dsPtrs,long nDsPtr) {
  /* Must wait for return before calling this again, until port to C++. */
  int ii;
  UpdateUsageLog();
  gAlreadyErr=0; dsu_gDone=0; gNumDatasetWindows=0; /* June 28 1995 */
  gIndent=INDENT_INIT; gNDs=0;
#ifndef QUIET_ASP
  PP"Version %s %s.\n",__DATE__,__TIME__);
  Ose(); PP"%s",gTheBlurb); Ose();
#endif
  for(ii=0;ii<nDsPtr;ii++) FillgDs("No parent",dsPtrs[ii]);
  TouchUpType();
  if(gIndent!=INDENT_INIT) dsu_Err(  7);
  DoXStuff();
  GetRidOfWindows();
}
