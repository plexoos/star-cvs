/***************************************************************************
 *
 * $Id: tableQuery.hh,v 1.10 2000/08/15 22:51:53 porter Exp $
 *
 * Author: R. Jeff Porter
 ***************************************************************************
 *
 * Description: Abstract class for Storage specific SQL queries
 *
 ***************************************************************************
 *
 * $Log: tableQuery.hh,v $
 * Revision 1.10  2000/08/15 22:51:53  porter
 * Added Root2DB class from Masashi Kaneta
 * + made code more robust against requesting data from non-existent databases
 *
 * Revision 1.9  2000/03/01 20:56:17  porter
 * 3 items:
 *    1. activated reConnect for server timeouts
 *    2. activated connection sharing; better resource utilization but poorer
 *       logging
 *    3. made rollback method in mysqlAccessor more robust (affects writes only)
 *
 * Revision 1.8  2000/02/15 20:27:46  porter
 * Some updates to writing to the database(s) via an ensemble (should
 * not affect read methods & haven't in my tests.
 *  - closeAllConnections(node) & closeConnection(table) method to mgr.
 *  - 'NullEntry' version to write, with setStoreMode in table;
 *  -  updated both StDbTable's & StDbTableDescriptor's copy-constructor
 *
 * Revision 1.7  2000/01/27 05:54:36  porter
 * Updated for compiling on CC5 + HPUX-aCC + KCC (when flags are reset)
 * Fixed reConnect()+transaction model mismatch
 * added some in-code comments
 *
 * Revision 1.6  2000/01/19 20:20:08  porter
 * - finished transaction model needed by online
 * - fixed CC5 compile problem in StDbNodeInfo.cc
 * - replace TableIter class by StDbTableIter to prevent name problems
 *
 * Revision 1.5  2000/01/10 20:37:55  porter
 * expanded functionality based on planned additions or feedback from Online work.
 * update includes:
 * 	1. basis for real transaction model with roll-back
 * 	2. limited SQL access via the manager for run-log & tagDb
 * 	3. balance obtained between enumerated & string access to databases
 * 	4. 3-levels of diagnostic output: Quiet, Normal, Verbose
 * 	5. restructured Node model for better XML support
 *
 * Revision 1.4  1999/11/19 21:58:07  porter
 * added method to return "malloc'd" version of table instead of new
 * so that delete of St_Table class i done correctly
 *
 * Revision 1.3  1999/09/30 02:06:15  porter
 * add StDbTime to better handle timestamps, modify SQL content (mysqlAccessor)
 * allow multiple rows (StDbTable), & Added the comment sections at top of
 * each header and src file
 *
 **************************************************************************/
#ifndef TABLEQUERY_HH
#define TABLEQUERY_HH

class StDbNode;
class StDbTable;
class StDbConfigNode;
class StDbBuffer;

#include "StDbDefs.hh"

class tableQuery {

public:

  tableQuery() {};
  virtual ~tableQuery() {};

  virtual int initDbQuery(const char* dbname, const char* serverName, const char* hostName, const int portNumber) = 0;
  virtual void close() = 0;

  virtual int QueryDb(StDbTable* table, unsigned int reqTime) = 0;
  virtual int QueryDb(StDbTable* table, const char* whereClause) = 0;
  virtual int WriteDb(StDbTable* table, unsigned int storeTime) = 0;

  virtual int QueryDb(StDbConfigNode* node) = 0;
  virtual int WriteDb(StDbConfigNode* node, int currentID)=0;

  virtual int QueryDb(StDbNode* node) = 0;

  virtual bool rollBack(StDbNode* node) = 0;
  virtual bool rollBack(StDbTable* table) = 0;

  virtual int QueryDescriptor(StDbTable* table) = 0;
  virtual bool selectDb(const char* dbName, StDbType type, StDbDomain domain) =0;

  virtual unsigned int getUnixTime(const char* time) = 0;
  virtual char* getDateTime(unsigned int time) = 0;

  virtual StDbBuffer* getBuffer() = 0; 
  virtual bool IsConnected() = 0;


};

#endif





