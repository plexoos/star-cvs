#ifndef StiIstDetectorBuilder_h
#define StiIstDetectorBuilder_h

#include "Sti/StiDetectorBuilder.h"

class StIstDb;
class StiMaterial;
class StiPlacement;


class StiIstDetectorBuilder : public StiDetectorBuilder
{
public:

   StiIstDetectorBuilder(bool active, bool buildIdealGeom = true);
   virtual void buildDetectors(StMaker &source);
   virtual void useVMCGeometry();

protected:

   bool          mBuildIdealGeom;
   StIstDb      *mIstDb;

private:

   void buildInactiveVolumes();
   void buildPlanerVolume(StiDetector& detector, std::string detName, float halfDepth, float thickness, float halfWidth,
         float yShift, float rShift, float zShift, StiPlacement *placement, StiMaterial *mat);
   void buildTubeVolume(StiDetector& detector, std::string detName, float halfDepth, float thickness,
         float outerRadius, float openingAngle, float zCenter, StiMaterial *mat);
};

#endif
