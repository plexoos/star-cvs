#ifndef StiIstDetectorBuilder_h
#define StiIstDetectorBuilder_h

#include "Sti/StiDetectorBuilder.h"

class StIstDb;
class StiPlanarShape;
class StiCylindricalShape;
class StiMaterial;
class StiPlacement;

class StiIstDetectorBuilder : public StiDetectorBuilder
{
public:

   StiIstDetectorBuilder(bool active, bool buildIdealGeom = true);
   virtual void buildDetectors(StMaker &source);
   virtual void useVMCGeometry();

protected:

   StiMaterial  *mSiMaterial;
   bool          mBuildIdealGeom;
   StIstDb      *mIstDb;

private:

   void buildInactiveVolumes();
   void buildPlanerVolume(StiDetector& detector, string detName, float halfDepth, float thickness, float halfWidth, float yShift, float rShift, float zShift, StiPlacement *placement, StiMaterial *mat);
   void buildTubeVolume(StiDetector& detector, string detName, float halfDepth, float thickness, float outerRadius, float openingAngle, float zCenter, StiMaterial *mat);
};

#endif
