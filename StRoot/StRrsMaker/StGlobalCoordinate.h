/***********************************************************************
 *
 * $Id: StGlobalCoordinate.h,v 2.0 2000/08/09 16:16:58 gans Exp $
 *
 * Author:  brian Jan 27, 2000
 *
 ************************************************************************
 *
 * Description:  Global Coordinate
 *
 *************************************************************************
 *
 * $Log: StGlobalCoordinate.h,v $
 * Revision 2.0  2000/08/09 16:16:58  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.1  2000/02/08 16:34:07  lasiuk
 * Initial Revision:  eventually for StUtilities
 *
 ***********************************************************************/
#ifndef ST_GLOBAL_COORDINATE_HH
#define ST_GLOBAL_COORDINATE_HH

#include <iostream.h>

#include "StGlobals.hh"
#include "StThreeVector.hh"

class StGlobalCoordinate {         
public:
    StGlobalCoordinate();
    StGlobalCoordinate(const double, const double, const double);
    StGlobalCoordinate(const StThreeVector<double>&);

    virtual ~StGlobalCoordinate();
    //StGlobalCoordinate(const StGlobalCoordinate&);
    //StGlobalCoordinate& operator=(const StGlobalCoordinate&);
    
    // access functions provided by StThreeVector
    const StThreeVector<double>& position() const;

    StThreeVector<double>& position();
    
private:
    StThreeVector<double> mPos;
};

inline const StThreeVector<double>&
StGlobalCoordinate::position() const {return(mPos);}

inline StThreeVector<double>&
StGlobalCoordinate::position() {return(mPos);}

// Non-Member
ostream& operator<<(ostream&, const StGlobalCoordinate&);

#endif
