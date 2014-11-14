#ifndef StiPxlDetectorBuilder_h
#define StiPxlDetectorBuilder_h

#include "Sti/StiDetectorBuilder.h"

class StPxlDb;


class StiPxlDetectorBuilder : public StiDetectorBuilder
{
public:

   StiPxlDetectorBuilder(bool active, bool buildIdealGeom=true);
   virtual void buildDetectors(StMaker &source);
   virtual void useVMCGeometry();

protected:

   StiMaterial *mSiMaterial;
   StPxlDb     *mPxlDb;
   bool         mBuildIdealGeom;

private:

   void buildInactiveVolumes();
};

#endif
