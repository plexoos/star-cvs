/*CMZ :          01/06/98  17.31.42  by  Pavel Nevski*/
/*-- Author :    Pavel Nevski   28/11/97*/
/*****************************************************/
/*               S T A F   i n t e r f a c e         */
/*****************************************************/
#ifndef STAF
 
extern "C" void staf_start_    () {}
extern "C" void staf_stop_     () {}
extern "C" int  tdm_map_table_ () {return 0;}
extern "C" void ami_module_register_ (char* name, int n) {}
 
#else
#include <stdio.h>
#include <stream.h>
#include <stdlib.h>
#include <string.h>
#include "asuLib.h"
#include "emlLib.h"
#include "socLib.h"
#include "spxLib.h"
#include "tdmLib.h"
#include "duiLib.h"
#include "dioLib.h"
#include "amiLib.h"
#include "tntLib.h"
#include "topLib.h"
#include "dstype.h"
 
/*---------------------------------------------------------------------------*/
 
typedef struct XdfLun_t
{ /* Analog of F77 logical unit */
  FILE 		*fFile;  	/* pointer to C file descriptor */
  XDR  		*fStream; 	/* XDR stream      */
  DS_DATASET_T 	*fDataSet; 	/* "root" dataset  */
  char 		fName[512];	/* File Name	*/
  char 		fType[8];	/* File type    */
} XdfLun_t;
 
/*---------------------------------------------------------------------------*/
/*                         p r o t o t y p e s                               */
 
/* extern "C"  int  dsTypeSpecifier (char**, unsigned*, int); */
 
extern "C" FNC_PTR_T cs_get_func_(char* name,int n);
extern "C" void  staf_banner     (FILE* stream);
extern CC_P int  ami_load        (amiBroker *broker);
int              ami_load        (amiBroker *broker) {return 1;}
 
/****************************************************************************/
 
extern "C" void staf_start_ ()
{
   asu_init(); asu_start();
   eml_init(); eml_start();
   soc_init(); soc_start();
   spx_init(); spx_start();
   tdm_init(); tdm_start();
   dui_init(); dui_start();
   dio_init(); dio_start();
   ami_init(); ami_start();
   top_init(); top_start();
   tnt_init(); tnt_start();
   staf_banner(stdout);
}
 
extern "C" void staf_stop_ ()
{
   tnt_stop();
   ami_stop();
   dio_stop();
   dui_stop();
   tdm_stop();
   spx_stop();
   soc_stop();
   eml_stop();
   asu_stop();
}
 
/*---------------------------------------------------------------------------*/
 
extern "C" int dui_cdir_ (char* path,int lp) { return dui->cd(path); }
 
/*---------------------------------------------------------------------------*/
 
extern "C" int tdm_map_table_(char* path, char* name, char* spec, long* l,
                              char* data, int lp, int ln, int ls)
{
  tdmDataset*       tDs = NULL; // pointer to tdm class member function
  tdmTable*         aDs = NULL; // pointer to table finder function
  DS_DATASET_T*     pDs = NULL; // pointer to directory table
  DS_DATASET_T*     dDs = NULL; // pointer to dataset table
 
  char cpath[132];  cpath[0]=0;  strncat(cpath,path,lp);
  char cname[32];   cname[0]=0;  strncat(cname,name,ln);
  char cspec[2048]; cspec[0]=0;  strncat(cspec,spec,ls);
  long k=*l;
  //             this should work,  but it does not
  //  ier=dsNewDataset(&ds,cpath);  tdm->createTable(name, ds);
 
  if (!(tDs=tdm->findDataset(cpath))) dui->mkdir(cpath);
 
  if ( (tDs=tdm->findDataset(cpath))
       &&  (tDs->cvtDslPointer((DSL_PTR_T &)pDs)) && pDs)
     { for (int i=0;  i < pDs->elcount;  i++)
       { // printf (" dataset %d %d %d %s tested \n",i,pDs->p.link[i],
         //          (pDs->p.link[i])->flags, (pDs->p.link[i])->name);
         if ((dDs=pDs->p.link[i]) && dDs->flags && !strcmp(dDs->name,cname))
         {  if (k<0)
            { size_t rsize;  dsTableRowSize(&rsize,dDs);
              if (rsize)     k=(-k)/(rsize/sizeof(k));
              int d=k*rsize+(*l)*sizeof(k);
              if (!d) printf (" table %s rsize=%d k=%d %d\n",cname,rsize,*l,d);
            }
            dDs->maxcount = k;
            dDs->elcount  = k;
            dDs->p.data   = data;
            return          k;
            // return (DSL_PTR_T &) dDs;
       } }
       return dsAddTable(pDs,cname,cspec,k,&data);
     }
  printf (" TDM_MAP_TABLE: directory %s not found \n",cpath);
  return 0;
}
 
extern "C" int tdm_new_table_ (char* name, char* spec, int* n, int ln, int ls)
{ int i;  char* s;
  /* skip spaces */  s=spec; while (*s==' ') { s++; }
  /*
     printf(" name = %s \n",name);
     printf(" spec = %s \n",spec);
     printf(" n    = %d \n",*n);    */
     i = tdm_newtable (name, s, *n);
     if (i) tdmtable_maxrowcount(name, *n);
     return i;
}
 
/*---------------------------------------------------------------------------*/
 
extern "C" int tdm_find_spec_(char* c, int lc)
{
  char *pSpec;  int i;  unsigned lspec=0;
  for (i=1;;i++)
  { if (!dsTypeSpecifier(&pSpec, &lspec, i)) return 0;
    if (strstr(pSpec,c)) return i;
  }
}
 
extern "C" int tdm_get_spec_(TABLE_HEAD_ST* pTab, char* c, int lc)
{
  char *pSpec;
/*
  printf(" dataset name = %20s \n",pTab->name);
  printf("         type = %20s \n",pTab->type);
  printf("         maxl = %d \n",pTab->maxlen);
  printf("         nok  = %d \n",pTab->nok);
  printf("         rbyte= %d \n",pTab->rbytes);
  printf("         ds_p = %d \n",pTab->dsl_pointer);
  printf("         dd_p = %d \n",pTab->data_pointer);
*/
  if (!dsTableTypeSpecifier(&pSpec,(ds_dataset_t*)pTab->dsl_pointer)) return 0;
     if (strlen(pSpec)<lc) strcpy(c,pSpec); else strncpy(c,pSpec,lc);
     return strlen(pSpec)+1;
}
 
/*---------------------------------------------------------------------------*/
 
extern "C" int tdm_get_ccount_(TABLE_HEAD_ST* pTab)
 
{ unsigned ccount;
  if (!dsTableColumnCount(&ccount, (ds_dataset_t*)pTab->dsl_pointer)) return 0;
  return (int) ccount;
}
 
extern "C" int tdm_get_column_(TABLE_HEAD_ST* pTab,int* k,char* c,char* d,
                                unsigned* l, unsigned* e, unsigned* m)
{ char *cc; char *dd;
  if (!dsColumnName     (&cc, (ds_dataset_t*) pTab->dsl_pointer,*k)
   || !dsColumnTypeName (&dd, (ds_dataset_t*) pTab->dsl_pointer,*k)
   || !dsColumnSize     (l  , (ds_dataset_t*) pTab->dsl_pointer,*k)
   || !dsColumnElcount  (e  , (ds_dataset_t*) pTab->dsl_pointer,*k)
   || !dsColumnDimCount (m  , (ds_dataset_t*) pTab->dsl_pointer,*k))  return 0;
 
  strcpy(d,dd);
  strcpy(c,cc);
  return strlen(cc)+1;
}
 
/*---------------------------------------------------------------------------*/
 
extern "C" int ami_call_(char* name,int* n, char* tables, int ln, int lt)
 
{ amiInvoker*    invoker;
  FNC_PTR_T      myPamFtn;
  long           myRank;
  tdmTable*      t[40];
  TABLE_HEAD_ST* h[40];
  char*          d[40];
  int            i,status;
 
  if (!(invoker=ami->findInvoker(name))) return 0;
  myRank   = invoker->rank();
  myPamFtn = invoker->pFunction();
  for (i=0;i<*n;i++)
   {  t[i]=tdm->findTable(tables+i*lt);   if (!t[i]) return 0;
      if (!t[i]->cvtTasStructs(h[i],d[i]))  return 0;
      /* printf(" table found %20s \n",h[i]->name); */
   }
   status = ami_pamSwitch(myRank, myPamFtn, h, d);
   if (status) { for (i=0;i<*n;i++)  { t[i]->rowCount(h[i]->nok); }}
   return status;
}
 
extern "C" int ami_module_call_ (char* name, int* n, char* tables,
                                                   int ln, int lt)
{
    int i; char* ctab[40];
    for (i=0;i<*n;i++)
    { ctab[i]=tables+i*lt; /* printf(" i,ctab = %d %s \n",i,ctab[i]); */ }
    return ami_call (name, *n, (char**) ctab);
}
 
 
#ifdef     DS_ADVANCED_try
int tabspeci_(int* i,char* c)
{ DS_TYPE_T *pType;
  if (!dsTypePtr(&pType, *i)) return 0;
       strcpy(c,pType->name); return strlen(pType->name)+1;
}
#endif
 
 
extern "C" void ami_module_register_ (char* name, int n)
{
   STRING_SEQ_T specs;
   char sname[80];
   strncpy(sname,name,n);
 
   sname[n]='_';
   FNC_PTR_T address = cs_get_func_ (sname,n+1);
   if (!address) printf (" ==> ami_module_register : address=0\n");
 
   sname[n]='\0';
   specs._length = specs._maximum = 0;
   specs._buffer = new char*[0];
 
   ami->deleteInvoker(sname);
   ami->newInvoker(sname,0,address,specs);
}
 
/****************************************************************************/
 
 
int xdf_open(XdfLun_t **Lun, char *FileName,char *mode) {
 
  XdfLun_t *lun;
 
  if (*Lun) { /* error, must be NULL*/
    printf("xdf_open. Error, lun is non zero %d\n",*lun);
    return 1;}
 
  lun = (XdfLun_t*)malloc(sizeof(XdfLun_t));
 
  strcpy(lun->fType,"r");
  if (mode && mode[0] && mode[0]!=' ') strcpy(lun->fType,mode);
  strcpy(lun->fName,FileName);
 
  lun->fFile = fopen(lun->fName,lun->fType);
 
  if(!lun->fFile) {/* open error */
    printf("xdf_open. Error, can not open file %s %s\n",lun->fName,lun->fType);
    free (lun); return 2;}
 
  if(!dsNewDataset(&lun->fDataSet,"NEVSKI")) {
    printf("xdf_open. Error, can not create data set for file %s %s\n",
                                                        lun->fName,lun->fType);
    free (lun); lun = NULL; return 2;}
 
   xdrstdio_create(lun->fStream, lun->fFile, XDR_DECODE);
 
   *Lun = lun; return 0;
}
/*---------------------------------------------------------------------------*/
int xdf_next_record(XdfLun_t *Lun)
 
{
   if(!Lun) {
     printf("xdf_next_record: Error, empty file handler\n"); return 13;}
 
 if (!xdr_dataset(Lun->fStream,&Lun->fDataSet))
  {printf("xdf_next_record: end of file %s",Lun->fName); return -1;}
 return 0;
}
 
/*---------------------------------------------------------------------------*/
 
int xdf_get_struct(XdfLun_t *lun, char *name,
     DS_DATASET_T **Entry,void **Data, size_t *nrows) {
 
   if(!lun) {
     printf("xdf_get_struct: Error, empty file handler\n"); return 13;}
 
if (*Entry) { free (*Entry); *Entry=NULL;}; 	/* ????? */
if (*Data)  { free (*Data);  *Data=NULL;};	/* ????? */
 
 
if (!dsFindEntry(Entry,lun->fDataSet,name)) return 1;
 
 
if (!dsTableDataAddress((char**)Data,*Entry)) return 2;
if (!dsTableRowCount(nrows,*Entry)) return 3;
return 0;
}
 
 
int xdf_close(XdfLun_t **Lun) {
  int ians;
 
   if(!*Lun) {
     printf("xdf_close: Error, empty file handler\n"); return 13;}
 
  ians = fclose((*Lun)->fFile);
  free ((*Lun)->fDataSet);
  free ((*Lun)); *Lun=NULL;
  return ians;
}
/***********************fortran interface to xdf************************/
 
void xdf_open_(unsigned long *Lun, char *FileName,char *mode,int *ians,
              int l77FileName, int l77mode) {
  char FileNameBuf[512],modeBuf[8];
  int l;
  for(l=l77FileName; l && FileName[l-1]!=' ';l--) {};
  FileNameBuf[0]=0; strncat(FileNameBuf,FileName,l);
  for(l=l77mode; l && mode[l-1]!=' ';l--) {};
  modeBuf[0]=0; strncat(modeBuf,mode,l);
 
  *ians = xdf_open((XdfLun_t **)Lun, FileNameBuf,modeBuf);
}
 
void xdf_next_record_(unsigned long *Lun, int *ians) {
  *ians = xdf_next_record((XdfLun_t*) *Lun);
}
 
void xdf_get_struct_(unsigned long *Lun, char *name,
     unsigned long *Entry,unsigned long *Data, size_t *nrows,int *ierr,
     int l77name) {
  char nameBuf[512];
  int l;
  for(l=l77name; l && name[l-1]!=' ';l--) {};
  nameBuf[0]=0; strncat(nameBuf,name,l);
 
*ierr = xdf_get_struct((XdfLun_t*) *Lun, nameBuf,
     (DS_DATASET_T **)Entry,(void **)Data, nrows);
}
 
void xdf_close_(unsigned long *Lun,int *ierr) {
  *ierr = xdf_close((XdfLun_t **)Lun);
}
 
#endif
 
 
