/***************************************************************************
 *
 * $Id: StROOTMagneticField.hh,v 1.3 2003/09/02 17:59:16 perev Exp $
 *
 * Author: bl
 ***************************************************************************
 *
 * Description: Class is implemented as a Singleton
 *
 ***************************************************************************
 *
 * $Log: StROOTMagneticField.hh,v $
 * Revision 1.3  2003/09/02 17:59:16  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.2  1999/06/16 14:26:51  fisyak
 * Add flags for egcs on Solaris
 *
 * Revision 1.1  1999/03/23 03:38:47  lasiuk
 * Initial Revision
 *
 **************************************************************************/
#ifdef __ROOT__
#ifndef ST_ROOT_MAGNETIC_FIELD
#define ST_ROOT_MAGNETIC_FIELD

#include <Stiostream.h>

#if  defined(__sun) && ! defined(__GNUG__)
#include <stdcomp.h>
#endif
#include "SystemOfUnits.h"

#include "StMagneticField.hh"

class StROOTMagneticField : public StMagneticField {
public:
    ~StROOTMagneticField();
    
    static StMagneticField* instance();
    static StMagneticField* instance(const StThreeVector<double>& B);
    
    const StThreeVector<double>& at(const StGlobalCoordinate& gc) const;
    const StThreeVector<double>& at(const StThreeVector<double>& v) const;

private:
    StROOTMagneticField();
    StROOTMagneticField(const StThreeVector<double>&);
    
private:
    StThreeVector<double> mB;
    static StMagneticField* mInstance;
};

//
//   Inline member functions
//

inline const StThreeVector<double>&
StROOTMagneticField::at(const StGlobalCoordinate&) const {return mB;}

inline const StThreeVector<double>&
StROOTMagneticField::at(const StThreeVector<double>&) const {return mB;}
#endif
#endif
