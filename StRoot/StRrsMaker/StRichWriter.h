/*********************************************************Writer.h**\
 * $Id: StRichWriter.h,v 2.0 2000/08/09 16:17:05 gans Exp $
 *
 * Description:
 *   Writer is the output module for  
 *   StRichRawData project.
 * 
 *   It is the module that stores an array of pads, and it also
 *   creates DataSets to output. See also the html reference.
 *
 ********************************************************************
 * $Log: StRichWriter.h,v $
 * Revision 2.0  2000/08/09 16:17:05  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.6  2000/04/05 16:07:46  lasiuk
 * add to coded list of info
 *
 * Revision 1.5  2000/03/12 23:56:34  lasiuk
 * new coordinate system
 * exchange MyRound with inline templated funtion
 *
 * Revision 1.4  2000/02/14 01:10:40  lasiuk
 * interface for StRichID is changed
 *
 * Revision 1.3  2000/01/27 17:10:01  lasiuk
 * modify to work stand-alone from ROOT
 *
 * Revision 1.2  2000/01/25 22:02:23  lasiuk
 * Second Revision
 *
 * Revision 1.1  2000/01/18 21:32:05  lasiuk
 * Initial Revision
 *
 *   revision history:
 *     - 8/19/1999 created the class,               Alexandre Nevski.
 *
********************************************************************/

#ifndef ST_RICH_WRITER_H
#define ST_RICH_WRITER_H
#ifdef __ROOT__
#include "St_DataSet.h"
#endif
#ifndef ST_NO_NAMESPACES
//namespace StRichRawData {
#endif
#include "StRichRrsMacros.h"
#include "StRichPadPlane.h"
#include "StRichPhysicsDb.h"
#include "StRichEnumeratedTypes.h"

class StRichWriter {
public:
    
    static StRichWriter* getInstance();
    static StRichWriter* getInstance(StRichPadPlane*);
    
    ~StRichWriter();
    
    //void initStorage(int rows, int cols) { mStorage->resize(rows,cols); }
    void putSignal(int row, int col, double s, int id, int gid, int track_p, StRichSignalType signal=eUnknown);
    
    pad_type& getSignal(int row, int col);
    
    //St_DataSet* getPadsTable();
    //St_DataSet* getIDTable();
    
    int rows() const;
    int cols() const;
    int pads() const;
    int getADC(int row, int col) const;

    void clear();
    void cleanUpMCInfo();

protected:
    StRichWriter();
    StRichWriter(StRichPadPlane*);
    
private:
    StRichPadPlane* mStorage;
    StRichPhysicsDb* mPhysicsDb;

    double       mAdcConversion;

    static StRichWriter* p2Instance;     // handle to only instance
};

inline int
StRichWriter::rows() const { return mStorage->row_size(); }

inline int
StRichWriter::cols() const { return mStorage->col_size(); }

inline int
StRichWriter::pads() const { return mStorage->col_size(); }

inline void
StRichWriter::clear()  { mStorage->clear(); }

inline int
StRichWriter::getADC(int row, int col) const { return (int)(*mStorage)[row][col].signal; }

inline pad_type&
StRichWriter::getSignal(int row, int col) { return (*mStorage)[row][col];}

#ifndef ST_NO_NAMESPACES
//}
#endif

#endif
