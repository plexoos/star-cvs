/*************************************************************************
 * $Id: StRichGeometryDbInterface.h,v 2.0 2000/08/09 16:17:01 gans Exp $
 *
 * Description:
 *
 * $Log: StRichGeometryDbInterface.h,v $
 * Revision 2.0  2000/08/09 16:17:01  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.4  2000/03/12 22:18:45  lasiuk
 * add from materials Db
 * add normal vector value
 *
 * Revision 1.3  2000/02/12 21:55:45  lasiuk
 * Wire position adjustment
 *
 * Revision 1.2  2000/02/08 16:26:04  lasiuk
 * rmove vector and StGlobals from Interface.
 * allocate space for survey parameters
 * calculate sector origins and pad corner positions
 *
 * Revision 1.1  2000/01/25 22:02:20  lasiuk
 * Second Revision
 *************************************************************************/
#ifndef ST_RICH_GEOMETRY_INTERFACE_H
#define ST_RICH_GEOMETRY_INTERFACE_H

#include <iostream.h>

class StRichGeometryDbInterface {
public:
    
    virtual ~StRichGeometryDbInterface() {}
    //StRichGeometryInterface(const StRichGeometryInterface&);
    //StRichGeometryInterface&(const StRichGeometryInterface&);

    virtual double version() const = 0;
    virtual double detectorLength() const = 0;
    virtual double detectorWidth() const = 0;

    virtual double quadrantX0(int) const = 0;
    virtual double quadrantY0(int) const = 0;
    virtual double quadrantXOrigin(int) const = 0;
    virtual double quadrantYOrigin(int) const = 0;

    virtual double quadrantGapInX() const = 0;
    virtual double quadrantGapInY() const = 0;    

    virtual const StThreeVector<double>& radiatorDimension() const = 0;
    virtual const StThreeVector<double>& quartzDimension() const = 0;
    virtual const StThreeVector<double>& padPlaneDimension() const = 0;
    virtual double proximityGap() const = 0; 
 
    virtual int    numberOfPadsInARow()            const = 0; // X
    virtual int    numberOfPadsInAQuadrantRow()    const = 0; // X
    virtual int    numberOfRowsInAColumn()         const = 0; // Z
    virtual int    numberOfRowsInAQuadrantColumn() const = 0; // Z
    virtual int    numberOfPads()                  const = 0;

    virtual double padLength() const = 0;
    virtual double padWidth() const = 0;
    virtual double padPitch() const = 0;
    virtual double rowPitch() const = 0;
    virtual double rowSpacing() const = 0;
    virtual double padSpacing() const = 0;
    
    virtual double wirePitch() const = 0;
    virtual double firstWirePositionInY(int) const = 0;
    virtual int    numberOfWires() const = 0;

    virtual double anodeToPadSpacing() const = 0;
	
    //
    // Survey Geometry
    virtual double localOriginR()     const = 0;
    virtual double localOriginAngle() const = 0;
    virtual double localOriginZ()     const = 0;
    // Inclination in "Euler-like" angles.
    // Rotation to get Local in Global is AngleZ*AngleY*AngleX*Permutation
    virtual double localAngleZ()      const = 0;
    virtual double localAngleY()      const = 0;
    virtual double localAngleX()      const = 0;

    virtual const StThreeVector<double>& normalVectorToPadPlane() const = 0;
    
    virtual void   print(ostream& os = cout) const = 0;
};
#endif
