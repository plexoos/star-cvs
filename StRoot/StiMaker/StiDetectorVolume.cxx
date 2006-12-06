// $Id: StiDetectorVolume.cxx,v 2.1 2006/12/06 00:47:52 fine Exp $
// Author: Valeri Fine, Dec 2006

#include "StiDetectorVolume.h"

#include "TVolumePosition.h"
#include "TRotMatrix.h"
#include "TTUBS.h"
#include "TBRIK.h"
#include "TMath.h"
#include "TGeometry.h"
#include "Sti/StiPlanarShape.h"
#include "Sti/StiCylindricalShape.h"
#include "Sti/StiGenericDetectorGroup.h"
#include "Sti/StiDetector.h"
#include "Sti/StiDetectorGroups.h"
#include "Sti/StiDetectorBuilder.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiMaterial.h"
#include "Sti/StiToolkit.h"
//
#if 0
//_____________________________________________________________________________
static Bool_t CompareMatrix(TRotMatrix &a,TRotMatrix &b)
{
  // the code was borrowed from St_geant_Maker 
  double *pa=a.GetMatrix(); double *pb=b.GetMatrix();
  for (int i=0; i<9; i++)  if (pa[i]!=pb[i]) return kFALSE;
  return kTRUE;
}
#endif

//_____________________________________________________________________________
inline static TRotMatrix *GetMatrix(float angle)
{
   // Fill the matrix object with the rotation matrix 
   Double_t m[9] = {
       TMath::Cos(angle), -TMath::Sin(angle), 0
      ,TMath::Sin(angle),  TMath::Cos(angle), 0
      ,      0          ,         0         , 1        
   };            
   //fRotMatrix.SetMatrix(m);
   TRotMatrix *mat = new TRotMatrix();
   mat->SetMatrix(m);
   return mat;
}

//_____________________________________________________________________________
// class TVolume decorator for StiDetector's
// 
ClassImp(StiDetectorVolume)
// 
//_____________________________________________________________________________

// class TVolume decorator for StiDetector's
//_____________________________________________________________________________
StiDetectorVolume::StiDetectorVolume(StiToolkit &tool ) :
 TVolume("RnD","Sti",(TShape*)0),fDetector(0) 
{ 
   MakeDetector(tool);
}
 
//_____________________________________________________________________________
StiDetectorVolume::StiDetectorVolume(const StiDetectorBuilder &builder) :
 TVolume(builder.getName().c_str(),"StiDetectorBuilder",(TShape *)0),fDetector(0) 
{ 
   MakeVolume(builder);
}
 
//_____________________________________________________________________________
StiDetectorVolume::StiDetectorVolume(StiDetector *detector) :
 TVolume(),fDetector(detector) { }

//_____________________________________________________________________________
StiDetectorVolume::StiDetectorVolume(StiDetector *detector,const Text_t* name, const Text_t* title, const Text_t* shapename, Option_t* option):
TVolume( name, title, shapename, option),fDetector(detector) { }

//_____________________________________________________________________________
StiDetectorVolume::StiDetectorVolume(StiDetector *detector,const Text_t* name, const Text_t* title, TShape* shape, Option_t* option):
 TVolume(name,title,shape,option ),fDetector(detector) { }
      
//_____________________________________________________________________________
void   StiDetectorVolume::Browse(TBrowser *b) 
{
   TVolume::Browse(b);
}
//_____________________________________________________________________________
char *StiDetectorVolume::GetObjectInfo(Int_t px, Int_t py) const
{
   return TVolume::GetObjectInfo(px, py);
}
//_____________________________________________________________________________
void StiDetectorVolume::MakeDetector(StiToolkit &tool)
{
   // Conststruct the TVolume from the StToolKit
   StiDetectorGroups *groups=tool.getDetectorGroups();
   vector<StiGenericDetectorGroup *>::iterator it = groups->begin();
   for (; it != groups->end(); ++it) {
      StiGenericDetectorGroup *group = *it;
      Add(new  StiDetectorVolume(*group->getDetectorBuilder()));
   }
}
//_____________________________________________________________________________
void StiDetectorVolume::MakeVolume(const StiDetectorBuilder &builder)
{
  // Conststruct the TVolume from the StDetectorBuilder
  unsigned int nRows = builder.getNRows();
  for (unsigned int i=0; i < nRows; i++) {
     unsigned int nSectors = builder.getNSectors(i);
     Int_t iColor  = 3 + i%6;
     for (unsigned int j=0;j<nSectors;j++) 
     {
        StiDetector *next = builder.getDetector(i,j) ;
        const StiShape *stiShape = next->getShape();
        TShape     *shape = MakeShape(stiShape
                        ,(const char*)next->getMaterial()->getName().c_str() ); 
        StiPlacement *place = next->getPlacement();
        TString canonicName = (const char*)next->getName().c_str();
        canonicName.ReplaceAll("/","_");
        StiDetectorVolume *nextVolume = new StiDetectorVolume(next,(const char*)canonicName,"StiDetector",shape);
        nextVolume->SetFillColor(iColor); nextVolume->SetLineColor(iColor);
        bool planar = (stiShape->getShapeCode() == kPlanar);
        TVolumePosition  *position = 0;
        if (planar) {
           if (place->getNormalRefAngle() != 0) {
              position = new TVolumePosition(0, 0, 0, 0, GetMatrix(place->getNormalRefAngle()));
              position->SetMatrixOwner();
              TVolumePosition  *translate= new TVolumePosition (0
                                       ,place->getNormalYoffset()
                                       ,place->getNormalRadius()
                                       ,place->getZcenter()
                                      );
              position->Mult(*translate );
              delete translate;
           } else {
              position = new TVolumePosition(0
                                       ,place->getNormalYoffset()
                                       ,place->getNormalRadius()
                                       ,place->getZcenter()
                                      );
           }
        }  else  {
           position = new TVolumePosition(0, 0, 0, place->getZcenter());
        }
        position->SetNode(nextVolume);
        Add(nextVolume,position);
     }
  }
}      

//_____________________________________________________________________________
TShape *StiDetectorVolume::MakeShape(const StiShape *shape, const char*material)      
{
   TShape *rootShape=0;
   if (shape) {
     switch (shape->getShapeCode()) {
        case kPlanar:
           rootShape = MakeShape(*(const StiPlanarShape *)shape,material);
           break;
        case kCylindrical:
           rootShape = MakeShape(*(const StiCylindricalShape *)shape,material);
           break;
        default: assert(0);
     }
   }
   return rootShape;
}

//_____________________________________________________________________________
TShape *StiDetectorVolume::MakeShape(const StiPlanarShape &shape,const char*material)
{
   return 
      new TBRIK((const char*)shape.getName().c_str()
             , "StiPlanarShape"
             , material
             , shape.getHalfWidth()
             , shape.getThickness()
             , shape.getHalfDepth() );
}

//_____________________________________________________________________________
TShape *StiDetectorVolume::MakeShape(const StiCylindricalShape &shape,const char*material) 
{
   return (shape.getOpeningAngle() < (TMath::TwoPi()-0.001) ) ? 
         new  TTUBS((const char*)shape.getName().c_str()
                  , "StiCylindricalShape"
                  , material
                  ,  shape.getOuterRadius() - shape.getThickness() // rmin
                  ,  shape.getOuterRadius()                        // rmax
                  ,  shape.getHalfDepth()                          // Dz
                  ,  -shape.getOpeningAngle()/2 
                  ,  +shape.getOpeningAngle()/2 )
         :
         new  TTUBE((const char*)shape.getName().c_str()
                  , "StiCylindricalShape"
                  ,  material
                  ,  shape.getOuterRadius() - shape.getThickness() // rmin
                  ,  shape.getOuterRadius()                        // rmax
                  ,  shape.getHalfDepth()                          // Dz
                  );             
}
