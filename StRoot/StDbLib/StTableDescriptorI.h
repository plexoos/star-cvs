/***************************************************************************
 *
 * $Id: StTableDescriptorI.h,v 1.6 2001/01/22 18:38:02 porter Exp $
 *
 * Author: R. Jeff Porter
 ***************************************************************************
 *
 * Description:  Abstract class defining access to a table-description
 *
 ***************************************************************************
 *
 * $Log: StTableDescriptorI.h,v $
 * Revision 1.6  2001/01/22 18:38:02  porter
 * Update of code needed in next year running. This update has little
 * effect on the interface (only 1 method has been changed in the interface).
 * Code also preserves backwards compatibility so that old versions of
 * StDbLib can read new table structures.
 *  -Important features:
 *    a. more efficient low-level table structure (see StDbSql.cc)
 *    b. more flexible indexing for new systems (see StDbElememtIndex.cc)
 *    c. environment variable override KEYS for each database
 *    d. StMessage support & clock-time logging diagnostics
 *  -Cosmetic features
 *    e. hid stl behind interfaces (see new *Impl.* files) to again allow rootcint access
 *    f. removed codes that have been obsolete for awhile (e.g. db factories)
 *       & renamed some classes for clarity (e.g. tableQuery became StDataBaseI
 *       and mysqlAccessor became StDbSql)
 *
 * Revision 1.5  2000/03/28 17:03:19  porter
 * Several upgrades:
 * 1. configuration by timestamp for Conditions
 * 2. query by whereClause made more systematic
 * 3. conflict between db-stored comments & number lists resolved
 * 4. ensure endtime is correct for certain query falures
 * 5. dbstl.h->handles ObjectSpace & RogueWave difference (Online vs Offline)
 *
 * Revision 1.4  1999/12/29 13:49:35  porter
 * fix for Solaris-CC4.2 within StRoot make (cons)...
 * replaced #include <config.h> with #include <ospace/config.h>
 *
 * Revision 1.3  1999/12/28 21:31:42  porter
 * added 'using std::vector' and 'using std::list' for Solaris CC5 compilation.
 * Also fixed some warnings arising from the CC5 compiles
 *
 * Revision 1.2  1999/09/30 02:06:12  porter
 * add StDbTime to better handle timestamps, modify SQL content (mysqlAccessor)
 * allow multiple rows (StDbTable), & Added the comment sections at top of
 * each header and src file
 *
 **************************************************************************/
#ifndef StTableDescriptorI_HH
#define StTableDescriptorI_HH

enum StTypeE {Stchar,Stuchar,Stshort,Stushort,Stint,Stuint,Stlong,Stulong,Stfloat,Stdouble};

const int StTypeSize[]={sizeof(char),sizeof(unsigned char),sizeof(short),sizeof(unsigned short), sizeof(int),sizeof(unsigned int),sizeof(long),sizeof(unsigned long), sizeof(float),sizeof(double)};


class StTableDescriptorI {

public:

  virtual ~StTableDescriptorI(){};
  virtual StTableDescriptorI* getCpy()                               = 0;
  virtual unsigned int getNumElements()                        const = 0;
  virtual unsigned int getTotalSizeInBytes()                   const = 0;
  virtual char* getElementName(int elementNum)                 const = 0;
  virtual unsigned int getElementOffset(int elementNum)        const = 0;
  virtual unsigned int getElementSize(int elementNum)          const = 0;
  virtual StTypeE getElementType(int elementNum)               const = 0;
  virtual unsigned int* getElementDimensions(int elementNum)   const = 0;
  virtual unsigned int getElementLength(int elementNum)        const = 0;
  virtual unsigned int getElementNumDimensions(int elementNum) const = 0;
  virtual unsigned int getElementIndexLength(int elementNum, int dimensionNum) const = 0;

};

#endif
