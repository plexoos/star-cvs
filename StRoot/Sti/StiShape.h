#ifndef STI_SHAPE_H
#define STI_SHAPE_H
#include <math.h>
#include "Sti/Base/Named.h"

// allowed values for shapeCode
enum StiShapeCode {kPlanar = 1, kCylindrical, kConical}; 

/*! Class encapsulating the notion of detector/volume shape.
\author Ben Norman, Kent State, 25 July 01
\author Claude Pruneau, Wayne State, Oct 2002
*/
class StiShape : public Named
{
public:

    // constructor (for subclass use)
    StiShape(): Named("undefined"),_halfDepth(0.), _thickness(0.){}
    StiShape(const string &name,float halfDepth, float thickness);

    // accessors
    float getHalfDepth() const { return _halfDepth; } 
    virtual float getHalfWidth() const=0;
    float getThickness() const { return _thickness; }
    virtual StiShapeCode getShapeCode() const = 0;
    float getEdgeWidth() const { return _edgeWidth; }

    // mutators
    void setHalfDepth(float val);
    void setThickness(float val);

protected:
    
    double nice(double val);

    /// half extent along z, always >= 0
    float _halfDepth;  
    /// "thickness", always >= 0
    float _thickness;
    /// size of the edge used in tracking
    float _edgeWidth;  
};


inline double StiShape::nice(double val)
{
  while (val < 0.){ val += 2*M_PI; }
  while (val >= 2*M_PI){ val -= 2*M_PI; }
  return val;
}

#endif
