 /***************************************************************************
 *
 * $Id: mysqlAccessor.hh,v 1.11 2000/08/15 22:51:53 porter Exp $
 *
 * Author: R. Jeff Porter
 ***************************************************************************
 *
 * Description: Storage specific SQL queries to database
 *
 ***************************************************************************
 *
 * $Log: mysqlAccessor.hh,v $
 * Revision 1.11  2000/08/15 22:51:53  porter
 * Added Root2DB class from Masashi Kaneta
 * + made code more robust against requesting data from non-existent databases
 *
 * Revision 1.10  2000/03/01 20:56:17  porter
 * 3 items:
 *    1. activated reConnect for server timeouts
 *    2. activated connection sharing; better resource utilization but poorer
 *       logging
 *    3. made rollback method in mysqlAccessor more robust (affects writes only)
 *
 * Revision 1.9  2000/02/15 20:27:45  porter
 * Some updates to writing to the database(s) via an ensemble (should
 * not affect read methods & haven't in my tests.
 *  - closeAllConnections(node) & closeConnection(table) method to mgr.
 *  - 'NullEntry' version to write, with setStoreMode in table;
 *  -  updated both StDbTable's & StDbTableDescriptor's copy-constructor
 *
 * Revision 1.8  2000/01/27 05:54:36  porter
 * Updated for compiling on CC5 + HPUX-aCC + KCC (when flags are reset)
 * Fixed reConnect()+transaction model mismatch
 * added some in-code comments
 *
 * Revision 1.7  2000/01/19 20:20:08  porter
 * - finished transaction model needed by online
 * - fixed CC5 compile problem in StDbNodeInfo.cc
 * - replace TableIter class by StDbTableIter to prevent name problems
 *
 * Revision 1.6  2000/01/10 20:37:55  porter
 * expanded functionality based on planned additions or feedback from Online work.
 * update includes:
 * 	1. basis for real transaction model with roll-back
 * 	2. limited SQL access via the manager for run-log & tagDb
 * 	3. balance obtained between enumerated & string access to databases
 * 	4. 3-levels of diagnostic output: Quiet, Normal, Verbose
 * 	5. restructured Node model for better XML support
 *
 * Revision 1.5  1999/12/03 22:24:01  porter
 * expanded functionality used by online, fixed bug in
 * mysqlAccessor::getElementID(char*), & update StDbDataSet to
 * conform to changes in Xml reader & writer
 *
 * Revision 1.4  1999/10/19 14:30:41  porter
 * modifications relevant to use with StDbBroker and future merging with
 * "params" database structure + some docs + suppressing diagnostics messages
 *
 * Revision 1.3  1999/09/30 02:06:14  porter
 * add StDbTime to better handle timestamps, modify SQL content (mysqlAccessor)
 * allow multiple rows (StDbTable), & Added the comment sections at top of
 * each header and src file
 *
 **************************************************************************/
#ifndef mysqlAccessor_HH
#define mysqlAccessor_HH

#include "StDbDefs.hh"
#include "tableQuery.hh" // Interface for StarDb Queries

#include "StDbTable.h"
#include "StDbConfigNode.hh"
#include "MysqlDb.h"
#include "StDbBuffer.h"


class mysqlAccessor : public tableQuery {

  // MySQL Specific functions 

unsigned int theEndTime;

char* mserverName;
int mportNumber;
char* mdbName;
StDbType mdbType;
StDbDomain mdbDomain;

public:

MysqlDb Db;
StDbBuffer buff;

  mysqlAccessor(const char* serverName, int portNumber); 
  mysqlAccessor(StDbType type, StDbDomain domain):theEndTime(2145934799), mserverName(0), mportNumber(0), mdbName(0), mdbType(type), mdbDomain(domain) { };
   ~mysqlAccessor();
    
  virtual int initDbQuery(const char* dbname, 
                          const char* serverName, 
                          const char* host, 
                          const int portNumber);

  virtual void close();
  // tableQuery Interface;

  virtual int QueryDb(StDbTable* table, unsigned int reqTime);
  virtual int QueryDb(StDbTable* table, const char* whereClause);
  virtual int WriteDb(StDbTable* table, unsigned int storeTime);
  virtual int WriteDb(StDbConfigNode* node, int currentID);
  virtual int QueryDb(StDbConfigNode* node);
  virtual int QueryDb(StDbNode* node);
  virtual bool rollBack(StDbNode* node);
  virtual bool rollBack(StDbTable* table);
  
  virtual int QueryDescriptor(StDbTable* table);
  virtual bool selectDb(const char* dbName, StDbType type, StDbDomain domain);


  virtual StDbBuffer* getBuffer(){return (StDbBuffer*) &buff;}; 
  virtual bool  IsConnected();


  // DB & timestamp helper methods

  virtual unsigned int getUnixTime(const char* time);
  virtual char* getDateTime(unsigned int time);
  virtual char* getDbName() const;

  // If write with no data, then assume index points to default
  virtual int findDefaultID(StDbTable* table);

protected:

  // low-level delete for roll-back & duplicate entries
  virtual void  deleteRows(const char* tableName, int* rowID, int nrows);
  // check node attributes
  virtual bool  queryNodeInfo(StDbNodeInfo* node);
  // read node attributes
  virtual bool  readNodeInfo(StDbNodeInfo* node);
  // store node attributes
  virtual bool  storeNodeInfo(StDbNodeInfo* node);
  // combine query & read of node attributes
  virtual bool  prepareNode(StDbNode* dbNode, StDbNodeInfo* node);
  // check if a data-instance exists in database
  virtual bool  hasInstance(StDbNodeInfo* node);

  virtual char* getEndDateTime() {return getDateTime(theEndTime);};
  virtual unsigned int getEndTime(){return theEndTime; };

};


inline
void
mysqlAccessor::close(){ Db.Close();}

#endif









