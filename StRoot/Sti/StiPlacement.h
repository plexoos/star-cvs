// Represents the position and orientation of a detector in STAR.
// 
// Ben Norman, Kent State
// 26 July 01
//
// The "center" of a planar detector is its center of gravity (the mindpoint
// in local x, y, and z).  For curved
// (cylindrical or conical sections) detectors, the "center" is the midpoint
// in z, opening angle, and radial thickness.
//
// The "normal" coordinates give the magnitude and azimuthal angle of a
// normal vector from the global origin to the plane of the detector.  The
// plane of a planar detector is simply the one in which it lies.  For a
// curved detector, the plane is the one which contains the tangent to the 
// curved section at its center and is normal to the transverse projection of
// the radial vector from the
// origin to its center.  Note that these definitions all assume that the
// plane of the detector is parallel to global z.  The third "normal" 
// cooridnate gives the location of the detector center along the detector
// plane in the transverse direction (i.e., local x).  This representation is
// best for the Kalman local track model.
//
// The "center" coordinates are a little more natural and are best used for
// rendering and radial ordering.  Here, the magnitude and azimuthal angle
// of a vector from the global origin to the center of the detector are
 // given, as well as an orientation angle.  The orientation angle is the
// angle from the vector above to the detector plane's outward normal.
// It is 0 for detectors which have xOffset==0.

// when setting the values, one must set all 3 for a representation at once.
// the other representation is then recalculated and both are available
// for quick access

#ifndef STI_PLACEMENT_H
#define STI_PLACEMENT_H

class StiPlacement{

public:
    // constructors
    StiPlacement();

    // accessors
    float getNormalRefAngle() const { return normalRefAngle; }
    float getNormalRadius() const { return normalRadius; }
    float getNormalXoffset() const { return normalXoffset; }
    float getCenterRefAngle() const { return centerRefAngle; }
    float getCenterRadius() const { return centerRadius; }
    float getCenterOrientation() const { return centerOrientation; }
    float getZcenter() const { return zCenter; }

    // mutators
    void setNormalRep(float refAngle_, float radius_, float xOffset_);
    void setCenterRep(float refAngle_, float radius_, float orientation_);
    void setZcenter(float val){ zCenter = val; }

protected:

    // store both representations
    float normalRefAngle; // in [0,2pi)
    float normalRadius;   // >= 0
    float normalXoffset;
    float centerRefAngle; // in [0,2pi)
    float centerRadius;   // >= 0
    float centerOrientation;  // in [-pi/2, pi/2)

    float zCenter; 

};

#endif
