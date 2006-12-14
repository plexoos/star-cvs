#include "TSystem.h"
#include "StGeomBrowser.h"
#include "GeomBrowser.h"
#include "TObjString.h"

//----------------------------
//  STAR geom browser:
//----------------------------
// ZEBRA files        - .fz   extension
// ROOT  files        - .root extension
// ROOT  macros       - .C    extension
// OpenInvetor2 scene - .iv   extension
// VRML scene         - .wrl  extension
// STAR  geometry descriptor - 
// as defined by http://www.star.bnl.gov/STAR/comp/prod/MCGeometry.html
//______________________________________________________________
StGeomBrowser::StGeomBrowser(const char *fileName)
: fFileName(fileName), fBrowser(0) {}
//______________________________________________________________
void StGeomBrowser::SetFile(const char *fileName) 
{
   fFileName = fileName;
   gSystem->ExpandPathName(fFileName);    
   if (!fBrowser) fBrowser = new GeomBrowser();
   if ( fFileName.EndsWith(".C") ) {
         if (!gSystem->AccessPathName(fFileName.Data()))
             fBrowser->fileOpenMacro(fFileName.Data()); 
   } else if (fFileName.EndsWith(".root") ) {         
            if (!gSystem->AccessPathName(fFileName.Data()))
                fBrowser->fileOpenRoot(fFileName.Data()); 
   } else if (fFileName.EndsWith(".fz") ) {         
        if (!gSystem->AccessPathName(fFileName.Data())) fBrowser->fileOpenZebra(fFileName.Data()); 
   } else if (fFileName.EndsWith(".iv") ) {         
        if (!gSystem->AccessPathName(fFileName.Data())) fBrowser->fileOpenInventor(fFileName.Data()); 
   } else if (fFileName.EndsWith(".wrl") ) {         
        if (!gSystem->AccessPathName(fFileName.Data())) fBrowser->fileOpenInventor(fFileName.Data()); 
   }  else if ( (fFileName(0) == 'y' && fFileName.Length() <= 8) ||fFileName == "complete") {
       // STAR geometry version
       fBrowser->STAR_geometry_activated(fFileName.Data());
   } else  {
        // The last STAR geometry
        //fBrowser->fileOpenZebra(fFileName.Data());
   }      
}

//______________________________________________________________
void StGeomBrowser::Show() {
   if (!fBrowser) fBrowser = new GeomBrowser();
   fBrowser->show();
}
