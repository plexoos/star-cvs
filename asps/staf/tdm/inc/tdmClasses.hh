/*:Copyright 1995, Lawrence Berkeley National Laboratory
**:>--------------------------------------------------------------------
**:FILE:        tdmClasses.hh
**:DESCRIPTION: Table & Dataset Memory Classes
**:AUTHOR:      cet - Craig E. Tull, cetull@lbl.gov
**:BUGS:        -- STILL IN DEVELOPMENT --
**:HISTORY:	14may96-v004b-cet- Make cellAddress public.
**:HISTORY:     12mar96-v004a-cet- Use socFactory as base.
**:HISTORY:     29nov95-v003a-cet- Rework for working MOAST
**:HISTORY:     13sep95-v002b-cet- rework ctors & dtors
**:HISTORY:     06jul95-v000a-cet- creation
**:<--------------------------------------------------------------------
*/

#ifdef __cplusplus
#ifndef TDMCLASSES_HH
#define TDMCLASSES_HH

//:----------------------------------------------- INCLUDES           --
#define DS_ADVANCED
#include "dstype.h"
#include "asu_macros.h"
#include "asu_types.h"
#include "socClasses.hh"
#include "tdm_macros.h"
#include "tdm_types.h"

//:=============================================== CLASS              ==
class tdmObject: public virtual socObject {

public:
//:----------------------------------------------- CTORS & DTOR       --
   tdmObject();
   tdmObject(const DS_DATASET_T* pDS);
   virtual ~tdmObject();
//:----------------------------------------------- ATTRIBUTES         --
   virtual char * dslName ();
   virtual unsigned char isDataset ();
   virtual unsigned char isTable ();
//:----------------------------------------------- PUB FUNCTIONS      --

protected:
//:----------------------------------------------- PROT VARIABLES     --
   DS_DATASET_T *pDSthis;

//:----------------------------------------------- PROT FUNCTIONS     --
};

//:=============================================== CLASS              ==
class tdmTable: public virtual tdmObject {

public:
//:----------------------------------------------- CTORS & DTOR       --
//   tdmTable(const char *name);
   tdmTable(const char *name
		, const DS_DATASET_T* pDS);
   tdmTable(const DS_DATASET_T* pDS);
   tdmTable(const char *name, const char * spec, long rows);
   virtual ~tdmTable();

//:----------------------------------------------- ATTRIBUTES         --
   virtual long columnCount ();
   virtual char * dslName ();
   virtual void maxRowCount (long maxRowCount);
   virtual long maxRowCount ();
   virtual void rowCount (long rowCount);
   virtual long rowCount ();
   virtual long rowSize ();
   virtual char * typeName ();
   virtual char * typeSpecifier ();

//:----------------------------------------------- PUB FUNCTIONS      --

   virtual unsigned char isType (const char * aType);

   virtual STAFCV_T findColumn (TDM_COLUMN_T& column,
		const char * name );
   virtual STAFCV_T getColumn (TDM_COLUMN_T& column, long ncol);

   virtual STAFCV_T printRows (long ifirst, long nrows);
   virtual STAFCV_T show ();

//- Column attributes
   virtual long columnNumber (const char * name);
   virtual char * columnName (long ncol);
   virtual DS_TYPE_CODE_T columnTypeCode (long ncol);
   virtual char * columnTypeName (long ncol);
   virtual long columnSize (long ncol);
   virtual long columnRank (long ncol);
   virtual long columnShape (long ncol, long ndim);
   virtual long columnElcount (long ncol);

//- Data access
   virtual STAFCV_T getCell (TDM_CELLDATA_T& data
		, long nrow, long ncol);
   virtual STAFCV_T putCell (const TDM_CELLDATA_T& data
		, long nrow, long ncol);

   virtual STAFCV_T getData (TDM_DATABLOCK_T& data);
   virtual STAFCV_T putData (const TDM_DATABLOCK_T& data);

//- CO-LOCATED METHODS
   virtual void * cellAddress(long nrow, long ncol);

//- TEMPORARY LEGACY METHODS
   virtual STAFCV_T cvtDslPointer(DSL_PTR_T& pDS);
   virtual STAFCV_T cvtTasStructs(TABLE_HEAD_ST *& head, char *& data);

protected:
//:----------------------------------------------- PROT VARIABLES     --
//:**NONE**
//:----------------------------------------------- PROT FUNCTIONS     --

};

//:=============================================== CLASS              ==
class tdmDataset: public virtual tdmObject {

public:
//:----------------------------------------------- CTORS & DTOR       --
// tdmDataset(const char* name);
   tdmDataset(const char* name
		, const DS_DATASET_T* pDS);
   tdmDataset(const DS_DATASET_T* pDS);
   tdmDataset(const char* name, long setDim);
   virtual ~tdmDataset();

//:----------------------------------------------- ATTRIBUTES         --
   virtual char * dslName ();
   virtual long entryCount ();
   virtual long maxEntryCount ();

//:----------------------------------------------- PUB FUNCTIONS      --
   virtual STAFCV_T addDataset (const char * name, long setDim);
   virtual STAFCV_T addTable (const char * name, const char * spec
		, long rows);
   virtual STAFCV_T getEntryType (char *& type, long num);
   virtual STAFCV_T getDatasetEntry (tdmDataset*& dataset, long num);
   virtual STAFCV_T getTableEntry (tdmTable*& table, long num);
   virtual STAFCV_T findDatasetEntry (tdmDataset*& dataset
		, const char * name);
   virtual STAFCV_T findTableEntry (tdmTable*& table
		, const char * name);
   virtual STAFCV_T getDescriptor (char *& descriptor);

//- TEMPORARY LEGACY METHODS
   virtual STAFCV_T cvtDslPointer(DSL_PTR_T& pDS);

protected:
//:----------------------------------------------- PRIV VARIABLES     --

//:----------------------------------------------- PRIV FUNCTIONS     --
//:**NONE**

};

//:=============================================== CLASS              ==
class tdmFactory: public virtual socFactory {

public:
//:----------------------------------------------- CTORS & DTOR       --
   tdmFactory();
   tdmFactory(const char * name);
   virtual ~tdmFactory();

//:----------------------------------------------- ATTRIBUTES         --
//:**NONE**

//:----------------------------------------------- PUB FUNCTIONS      --
   virtual char * list ();
   virtual STAFCV_T deleteDataset (const char * name);
   virtual STAFCV_T deleteTable (const char * name);
   virtual STAFCV_T findDataset (const char * name
                , tdmDataset*& dataset);
   virtual STAFCV_T findTable (const char * name, tdmTable*& table);
   virtual STAFCV_T getDataset (IDREF_T id, tdmDataset*& dataset);
   virtual STAFCV_T getTable (IDREF_T id, tdmTable*& table);
   virtual STAFCV_T newDataset (const char * name, long setDim);
   virtual STAFCV_T newTable (const char * name, const char * spec
                , long rows);

   virtual STAFCV_T createDataset (const char * name, DS_DATASET_T *pDS
		, tdmDataset *& dataset);
   virtual STAFCV_T createTable (const char * name, DS_DATASET_T *pDS
		, tdmTable *& table);

   virtual STAFCV_T getTypeName (long tid, char *& name);
   virtual STAFCV_T getTypeSpecification (long tid, char *& spec);

protected:
//:----------------------------------------------- PROT VARIABLES     --
//:**NONE**
//:----------------------------------------------- PROT FUNCTIONS     --
//:**NONE**

};

//----------------------------------------------------------------------
CORBA_TIE(tdmObject)
CORBA_TIE(tdmTable)
CORBA_TIE(tdmDataset)
CORBA_TIE(tdmFactory)

#endif /*TDMCLASSES_HH*/
#endif /*__cplusplus*/

