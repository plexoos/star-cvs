// $Id: StDraw3D.cxx,v 1.60 2009/10/17 00:03:29 fine Exp $
//*-- Author :    Valery Fine(fine@bnl.gov)   27/04/2008
#include "StDraw3D.h"
#include "TCanvas.h"
#include "TTRAP.h"
#include "TVolume.h"
#include "TVolumePosition.h"
#include "TRotMatrix.h"
#include "TMath.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TColor.h"
#include "TEnv.h"
#include "StCheckQtEnv.h"
#include "TStyle.h"
#include "TVirtualViewer3D.h"
#include <cassert>
#include <cmath>

static Color_t colorDefault = Color_t(-1);
static Style_t styDefault   = Style_t(-1);
static Size_t  sizDefault   = Size_t (-1);

Color_t StDraw3D::fgColorDefault = Color_t(-1);
Style_t StDraw3D::fgStyDefault   = Style_t(-1);
Size_t  StDraw3D::fgSizDefault   = Size_t (-1);
Color_t StDraw3D::fgBkColor      = kBlack;

// Canvas counter to create the Unique Canvas names
Int_t   StDraw3D::fDrawCanvasCounter = -1; 



//___________________________________________________
static inline TVirtualViewer3D *InitCoin(TVirtualPad *pad,const char *detectorName) 
{
   TVirtualViewer3D *viewer = 0;
   // check Coin env and load if present
   bool CheckCoin = true;
   if (!StCheckQtEnv::SetQtEnv(false)) {   CheckCoin = true; }

//   if (CheckCoin && pad ) {
   if (CheckCoin) {
      // define the background image
      TString backShape = detectorName;
      backShape.ReplaceAll(",",".iv:");
      backShape+= ".iv";
      printf(" Setting the background shape to be 	%s\n", backShape.Data());
      gEnv->SetValue("Gui.InventorShapeDir",":.:StRoot/macros/graphics:$STAR/StRoot/macros/graphics:/afs/rhic.bnl.gov/star/doc/www/comp/2/vis/iv");
      if  ( (viewer = TVirtualViewer3D::Viewer3D(pad,"oiv") )) {
          viewer->SetDrawOption(backShape.Data());
         // Create Open GL viewer
//        TGQt::SetCoinFlag(1);
         viewer->BeginScene();
         viewer->EndScene();
      }
    }
    return viewer;
}
//! Maps the track \a pt to the STAR StTrack track color code
/*!
  \param pt - pt value from some StEvent /StMuDst \c track object
  \return the ROOT color index
 */
//___________________________________________________
Color_t StDraw3DStyle::Pt2Color(double pt)
{ 
   const Int_t lightness    = 50;
   const Int_t saturation   = 100;
   Int_t hue  = (pt > 1.5 ) ? 0 : Int_t(256.*(1.-pt/1.5)); //color code from StuPostscript
   Int_t r,g,b;
   TColor::HLS2RGB(hue, lightness, saturation, r, g, b);
   // Normalize
   float factor = 1./sqrt(1.*r*r+1.*g*g+1.*b*b);
   return  TColor::GetColor(r*factor,g*factor,b*factor);
}

//___________________________________________________
//
//   view_3D
//___________________________________________________
class view_3D  {
   private:
      TObject *fModel;
      TString  fComment;
      TString  fObjectInfo;
      void makeInfo()
      {
         fObjectInfo="";
         if (fModel) {
            fObjectInfo = Form("( %s *)%p ",fModel->ClassName(),fModel);
         }
         if (!fComment.IsNull()) fObjectInfo += fComment;
      }
   public:
     view_3D(TObject *model = 0, const char *comment="") : fModel(model),fComment(comment)
     { makeInfo(); }
     ~view_3D(){;}
     TObject *model() const               { return fModel;                   }
     void setModel(TObject *model)        { fModel     = model ; makeInfo(); }
     void setComment(const char *comment) { fComment  = comment; makeInfo(); }
     void addComment(const char *comment) { fComment += comment; makeInfo(); }
     const TString &info() const { return fObjectInfo;              }
};

//___________________________________________________
//
//   poly_line_3D
//___________________________________________________
class poly_line_3D : public TPolyLine3D, public view_3D {
   public:
     poly_line_3D(Int_t n, Float_t *p, Option_t *option="") : TPolyLine3D(n,p),view_3D(){;}
     virtual ~poly_line_3D(){;}
     virtual char  *GetObjectInfo(Int_t x, Int_t y) const
     {
        const TString &customInfo = info();
        const char *info = 0;
        if (customInfo.IsNull()) 
           info = TPolyLine3D::GetObjectInfo(x,y);
        else 
           info = customInfo.Data();
        return (char *)info;
     }
     void Inspect() const {
        if ( model() ) model()->Inspect();
        else TPolyLine3D::Inspect();
     }
};

//___________________________________________________
//
//   poly_marker_3D
//___________________________________________________
class poly_marker_3D : public TPolyMarker3D, public view_3D {
   public:
     poly_marker_3D(Int_t n, Float_t *p, Option_t *option="") : TPolyMarker3D(n,p,1,option),view_3D(){;}
     virtual ~poly_marker_3D(){;}
     virtual char  *GetObjectInfo(Int_t x, Int_t y) const
     {
        const TString &customInfo = info();
        const char *info = 0;
        if (customInfo.IsNull()) 
           info = TPolyMarker3D::GetObjectInfo(x,y);
        else 
           info = customInfo.Data();
        return (char *)info;
     }
     void Inspect() const {
        if ( model() ) model()->Inspect();
        else TPolyMarker3D::Inspect();
     }
};
      
//! StDraw3D( const char *detectorName,TVirtualPad *pad) ctor
/*!  
     \param detectorName (default = "TPC") - the names of the STAR detectors 
                                                 to be used as the "event primitives" background.
                               The detectorName is a comma separated list of the OpenInventor files 
                               with no extension\n
                               For all names on the list one should provide the iv file with 
                               the "iv" extension:\n
                                     \code   <name>.iv \endcode   
         \param detectorName = 0  - no detector geometry is to be rendered
         \param pad (default = 0) - The ROOT TPad to be used to render the event wired view
 \htmlonly
 <table>
 <tr>
 <th>Event over detector geometry
 <th>Event with no detector geometry
 </tr>
 <tr>
 <td><img src="http://www.star.bnl.gov/public/comp/vis/StDraw3D/examples/EventDisplayWGeom.png">
 <td><img src="http://www.star.bnl.gov/public/comp/vis/StDraw3D/examples/EventDisplayWOGeom.png">
 </tr></table>
 \endhtmlonly
             
 \note This is the base class for the advanced EventDisplay subclasses. \n
        Normaly you do not need to instantiate StDraw3D directly.

 \note StDraw class ctor defines the set of the pre-defines styles:
 
\htmlonly 
  <table>
  <thead><th> Style       </th><th> Color   </th><th> Style </th><th> Size </th><th> comment </th></thead>
  <tr> <td> kVtx          </td><td> kYellow </td><td>  5    </td><td> 3.5 </td><td>  <a href="http://root.cern.ch/root/html/TAttMarker.html">3D marker</a> - vertex  </tr>
  <tr> <td> kPrimaryTrack </td><td> kRed    </td><td>  1    </td><td> 2.00 </td><td> <a href="http://root.cern.ch/root/html/TAttLine.html">3D polyline</a> - track </tr>
  <tr> <td> kGlobalTrack  </td><td> kRed    </td><td>  1    </td><td> 2.00 </td><td> <a href="http://root.cern.ch/root/html/TAttLine.html">3D polyline</a> - track </tr>
  <tr> <td> kTrackBegin   </td><td> kYellow </td><td>  5    </td><td> 3.5 </td><td>  <a href="http://root.cern.ch/root/html/TAttMarker.html">3D marker</a> - vertex  </tr>
  <tr> <td> kTrackEnd     </td><td> kYellow </td><td>  5    </td><td> 3.5 </td><td>  <a href="http://root.cern.ch/root/html/TAttMarker.html">3D marker</a> - vertex  </tr>
  <tr> <td> kUsedHit      </td><td> kBlue   </td><td>  4    </td><td> 0.35 </td><td> <a href="http://root.cern.ch/root/html/TAttMarker.html">3D marker</a> - track start point  </td></tr>
  <tr> <td> kUnusedHit    </td><td> kGreen  </td><td>  1    </td><td> 1.00 </td><td> <a href="http://root.cern.ch/root/html/TAttMarker.html">3D marker</a> - track end point   </td></tr>
  </table>
 \endhtmlonly
 \sa http://root.cern.ch/root/html/TColor.html
 \sa http://root.cern.ch/root/html/TAttLine.html
 \sa http://root.cern.ch/root/html/TAttMarker.html

*/
//___________________________________________________
StDraw3D::StDraw3D(const char *detectorName,TVirtualPad *pad): fPad(pad),fBkColor(fgBkColor),fViewer(0),fView(0)
      , fDetectorName(detectorName),fMaster(0),fTopVolume(0)
{

   // The detectorName is a comma separated list of the OpenInventor files with no extension
   // For all names on the list one should provide the iv file with the "iv extension:
   //                         <name>.iv

   static const Style_t UHitSty = 4; static const Size_t UHitSiz = 0.35; static const Color_t UHitCol=kBlue;
   static const Style_t NHitSty = 1; static const Size_t NHitSiz = 1.00; static const Color_t NHitCol=kGreen;
   static const Style_t TrakSty = 1; static const Size_t TrakSiz = 2.00; static const Color_t TrakCol=kRed;
   static const Style_t VertSty = 5; static const Size_t VertSiz = 3.50; static const Color_t VertCol=kYellow;
   AddStyle(kVtx,         VertCol,VertSty,VertSiz);
   AddStyle(kPrimaryTrack,TrakCol,TrakSty,TrakSiz);
   AddStyle(kGlobalTrack, TrakCol,TrakSty,TrakSiz);
   AddStyle(kTrackBegin,  VertCol,VertSty,VertSiz);
   AddStyle(kTrackEnd,    VertCol,VertSty,VertSiz);
   AddStyle(kUsedHit,     UHitCol,UHitSty,UHitSiz);
   AddStyle(kUnusedHit,   NHitCol,NHitSty,NHitSiz);
}

//__________________________________________________________________________________
TVirtualPad *StDraw3D::InitPad() 
{
   if (fMaster) fMaster->InitPad();
   else if (!fPad ) {
      fDrawCanvasCounter++;
      TString canvasName = "STAR";
      TString canvasTitle;
      if (fDrawCanvasCounter) {
           canvasName+="_";
           canvasName  += fDrawCanvasCounter;
           canvasTitle += fDrawCanvasCounter;
           canvasTitle += " : ";
     }
      canvasTitle += "STAR Event Viewer";
      fPad = new TCanvas(canvasName.Data(),canvasTitle.Data(), 400,400);
      fPad->SetFillColor(fBkColor);
      fPad->Modified();
      fPad->Update();
   }
   return Pad();
}
//__________________________________________________________________________________
void StDraw3D::InitViewer()
{
   //Create 3D viewer if  no master provided

   if (fMaster) fMaster->InitViewer();
   else if ( !fViewer ) fViewer = InitCoin(fPad,fDetectorName);
   assert(Viewer());
}
//___________________________________________________
StDraw3D::~StDraw3D()
{
    if (fPad) {
       if (!fMaster) fPad->Clear();
       delete fPad;
       fPad       = 0;
       fMaster    = 0;
       fTopVolume = 0;
    }
}

//! \return The TString of the comma separated names 
//__________________________________________________________________________________
const TString &StDraw3D::DetectorNames() const 
{
   // return the list of the names 
   return fDetectorName; 
} 
/*! \return The TPad pointer used to paint onto.
 */
//__________________________________________________________________________________
TVirtualPad *StDraw3D::Pad() const 
{ 
   return fMaster ? fMaster->Pad() : fPad;
}

//! \return The TVirtualViewer3D viewer pointer used to render into.
//__________________________________________________________________________________
TVirtualViewer3D *StDraw3D::Viewer() const
{
   return fMaster ? fMaster->Viewer() : fViewer;
}

//! Set the list of the detector names to be used as the event "background"
/*! \param  nameDetectors - a comma separated list of the OpenInventor files with no extension\n
                               For all names on the list one should provide the iv file with 
                               the "iv" extension:\n
                              \code   <name>.iv \endcode   
 */   
//__________________________________________________________________________________
void StDraw3D::SetDetectors(const char*nameDetectors)
{
   // The detectorName is a comma separated list of the OpenInventor files with no extension
   // For all names on the list one should provide the iv file with the "iv extension:
   //                         <name>.iv
   if (fViewer) {
      Warning("StDraw3D::SetDetectors","Can not change the detector names. It is too late. The viewer had been created");
   } else {
      fDetectorName = nameDetectors;
   }
}
//! Append the detector names to the list of  the event "background" shapes.
/*! \param  nameDetectors - a comma separated list of the OpenInventor files with no extension\n
                               For all names on the list one should provide the iv file with 
                               the "iv" extension:\n
                              \code   <name>.iv \endcode   
*/
//__________________________________________________________________________________
void StDraw3D::AddDetectors(const char*nameDetectors)
{ 
   // The detectorName is a comma separated list of the OpenInventor files with no extension
   // For all names on the list one should provide the iv file with the "iv extension:
   //                         <name>.iv

   if (fViewer) {
      Warning("StDraw3D::AddDetectors","Can not the change detector names. It is too late. The viewer had been created");
   } else if (nameDetectors && nameDetectors[0]){
      fDetectorName += ",";
      fDetectorName += nameDetectors;
   }
}

//! Remove all objects from the screen
//___________________________________________________
void  StDraw3D::Clear(Option_t *opt)
{
   // Clear the view
   TVirtualPad *pad = Pad();
   if (pad) {
      pad->Clear(opt);
      fTopVolume = 0;
      Update();
   }
}

//___________________________________________________
TObject *StDraw3D::Draw(TObject *o,const char *option)
{
   // Draw the 3d object 
   // and set the new background color if needed
   if (o) {
      TVirtualPad *sav = gPad;
      if (!Pad())        InitPad();
      if (Pad() != sav)  Pad()->cd();
      assert (fPad==gPad);
      o->Draw(option);
      if (sav && (Pad() != sav)) sav->cd();
      if (!Viewer()) InitViewer();
   }
   return o;
}
//! Set the ROOT color as the widget background
/*! 
    \param newBkColor - ROOT index of the color to paint the widget backgorund ( \sa http://root.cern.ch/root/html/TColor.html )
*/
//___________________________________________________
void StDraw3D::SetBkColor(Color_t newBkColor)
{
   // Set the canvas background color;
   fBkColor = newBkColor;
   TVirtualPad *pad = Pad();
   if (pad && pad->GetFillColor() != fBkColor)
       pad->SetFillColor(fBkColor);
}

//! Map the predefined style \a type to the ROOT graphical attributes \a col color \a sty style \a siz size
/*! 
    Normally one does not need to call this  method. All pre-defined styles are to be filled by  StDraw3D class ctor
    \param   type - The pre-define type we want to define
    \param col - ROOT color attribute  (See:  http://root.cern.ch/root/html/TColor.html) 
    \param sty - ROOT style attribute. It can be either line http://root.cern.ch/root/html/TAttLine.html or marker 
                                       http://root.cern.ch/root/html/TAttMarker.html
    \param siz - ROOT graphical attribute size . It can be either line http://root.cern.ch/root/html/TAttLine.html or marker 
                                       http://root.cern.ch/root/html/TAttMarker.html                                   
 */
//___________________________________________________
const StDraw3DStyle &StDraw3D::AddStyle(EDraw3DStyle type,Color_t col,Style_t sty,Size_t siz)
{
   fStyles.insert(std::pair<EDraw3DStyle,StDraw3DStyle>(type,StDraw3DStyle(type,col,sty,siz)));
   return Style(type);
}


//! Return the reference to the predefined StDraw3DStyle object
/*! 
    \param type - The pre-defined \a type we want to get the reference to
 */
//___________________________________________________
const StDraw3DStyle &StDraw3D::Style(EDraw3DStyle type)
{
    return fStyles[type];
}

//! Add \a n 3D coordinates from the \xyz array to the display list with the \a col color, \a sty style, and \a siz size if provided
/*! 
   \param     n - the number of the 3D coordinates 
   \param   xyz - the pointer to the array of the floating ount values ( the array should be 3*n long at least )
   \param   col - ROOT line color ( see: http://root.cern.ch/root/html/TAttLine.html ) 
   \param   sty - ROOT line style ( see: http://root.cern.ch/root/html/TAttLine.html ) 
   \param   siz - ROOT line width ( see: http://root.cern.ch/root/html/TAttLine.html ) 
   \return - a pointer to the ROOT "view" TPolyMarker3D created to render the input \a xyz array 
*/
//___________________________________________________
TObject *StDraw3D::Points(int n, const float *xyz, Color_t col,Style_t sty,Size_t siz)
{ 
   //
   // Draw "n" points of the "xyz" array of the float coordinates 
   // with ROOT TPolyMarker3D class
   // with the ROOT color, style, size attributes
   //
   
   poly_marker_3D *plMk  = new poly_marker_3D(n,(Float_t*)xyz);
   if (col != colorDefault) plMk->SetMarkerColor(col);
   if (sty != styDefault)   plMk->SetMarkerStyle(sty);
   if (siz != sizDefault)   plMk->SetMarkerSize(siz);
   fView = plMk;
   return Draw(plMk);
}
//! This is an overloaded member function, provided for convenience.
/*! Add \a n 3D coordinates from the \xyz array to the display list with the \a sty pre-defined style if provided 
   \param     n - the number of the 3D coordinates 
   \param   xyz - the pointer to the array of the floating ount values ( the array should be 3*n long at least )
   \param   sty  - EDraw3DStyle value selecting some predefined style 
   \return - a pointer to the ROOT "view" TPolyMarker3D created to render the input \a xyz array 
*/
//___________________________________________________
TObject *StDraw3D::Points(int n, const float *xyz, EDraw3DStyle sty)
{
   //
   // Draw "n" points of the "xyz" array of the float coordinates 
   // with ROOT TPolyMarker3D class and the predefined attrbutes
   //
   // This is an overloaded member function, provided for convenience.
   // It behaves essentially like the above function.
   //
   
  const StDraw3DStyle &style =  Style(sty);
  return Points(n, xyz, style.Col(),style.Sty(),style.Siz());
}

//___________________________________________________
TObject *StDraw3D::Draw3D(int n,  const float *xyz)
{
   //
   // Draw "n" points of the "xyz" array of the float coordinates 
   // and the kVtx attrbute
   //
   // This is an overloaded member function, provided for convenience.
   // It behaves essentially like the above function.
   //
   return Points(n,xyz,kVtx);
}
//___________________________________________________
TObject *StDraw3D::Point(float x, float y, float z, Color_t col,Style_t sty,Size_t siz)
{
   //
   // Draw ONE 3D marker with ROOT TPolyMarker3D class at x,y,z position
   // with the ROOT color, style, size attributes
   //
   
   float xyz[]={x,y,z};
   return Points(1,xyz,col,sty,siz);
}

//___________________________________________________
TObject *StDraw3D::Point(float x, float y, float z, EDraw3DStyle sty)
{
   //
   // Draw ONE 3D marker with ROOT TPolyMarker3D class at x,y,z position
   // and the predefined attrbutes
   //
   // This is an overloaded member function, provided for convenience.
   // It behaves essentially like the above function.
   //
   const StDraw3DStyle &style =  Style(sty);
   return Point(x,y, z, style.Col(),style.Sty(),style.Siz());
}

//___________________________________________________
TObject *StDraw3D::Line(int n,  const float *xyz, Color_t col,Style_t sty,Size_t siz)
{
   //
   // Draw "n" points of the "xyz" array of the float coordinates 
   // with ROOT TPolyline3D class
   // with the ROOT color, style, size attributes
   //
   poly_line_3D *plLine  = new poly_line_3D(n,(Float_t*)xyz);
   if (col != colorDefault) plLine->SetLineColor(col);
   if (sty != styDefault)   plLine->SetLineStyle(sty);
   if (siz != sizDefault)   plLine->SetLineWidth(Width_t(siz));
   fView = plLine;
   return Draw(plLine);
}

//___________________________________________________
TObject *StDraw3D::Line(int n,  const float *xyz,EDraw3DStyle sty)
{
   //
   // Draw "n" points of the "xyz" array of the float coordinates 
   // with ROOT TPolyLine3D class and the predefined attrbutes
   //
   // This is an overloaded member function, provided for convenience.
   // It behaves essentially like the above function.
   //
   const StDraw3DStyle &style =  Style(sty);
   return Line(n,xyz,  style.Col(),style.Sty(),style.Siz() );
}
//___________________________________________________
void StDraw3D::Joint(StDraw3D *dsp)
{
   // The method to force two different instances
   // of the StDraw3D class  paint onto one and the same
   // TPad.

   // Force "dsp" to share the fPad of this object
   // As result of the "Joint" method both objects 
   // "this" as well as "dsp" will share the TPad of "this" object.
   // The original TPad of "dsp" is to be abandoned if exists

   if (dsp) dsp->SetMaster(this);
}
//___________________________________________________
void StDraw3D::Redraw()
{
   // Move all existent view to the master window if any
   if (fMaster && fPad) {
      TList *p = fPad->GetListOfPrimitives();
      if (p) {
         TObject *o = 0;
         TIter next(p);
         while ( (o=next()) )  Draw(o);
         p->Clear();
      }
   }
}
//___________________________________________________
void StDraw3D::SetMaster(StDraw3D *master) 
{
   //Make this object slave of the "master" object
   if (fMaster != master) {
      if (fMaster) 
         Error("StDraw3D::SetMaster"
               ,"The object (StDraw3D*)%p already has another master %p", this, fMaster);
     fMaster = master;
     Redraw();
   }
}

//___________________________________________________
void StDraw3D::SetModel(TObject *model)
{
   // add the "model" reference to the current view
   if (fView) fView->setModel(model);
}

//___________________________________________________
void StDraw3D::SetComment(const char *cmnt)
{
   // set the "model" comment for the current view
   if (fView) fView->setComment(cmnt);
}

//___________________________________________________
void StDraw3D::AddComment(const char *cmnt)
{
   // add the "model" comment for the current view
   if (fView) fView->addComment(cmnt);
}

//! Save the current 3D scene using "wrl" file format
/*! \param  filename - the file name to save the 3d scene
   \Note: The "wrl" format can be converted to the standatd 3D PDF format 
   \htmlonly 
   It can be done <a href="http://blogs.adobe.com/mfg/2009/04/more_3d_reviewer_features">
   "Adobe 3D reviewer"</a>  
   <P>See examples:  
  <blockquote>
  You need to install the <a href="http://get.adobe.com/reader/?promoid=BUIGO">Adobe Reader version 9 or higher
  <img src="http://www.adobe.com/images/shared/download_buttons/get_adobe_reader.png"></a>
  to be able to "click and see" the interactive ( zoom, pan, select / highlight the pieces, etc )  3D image also
  </blockquote>
  <center> 
   <a href="http://www.star.bnl.gov/public/comp/vis/StDraw3D/examples/production_dAu2008.pdf">
  <img src="http://www.star.bnl.gov/public/comp/vis/StDraw3D/examples/production_dAu2008.png"></a>
  <br>
  <a href="http://www.star.bnl.gov/public/comp/vis/GeomBrowser/y2009.pdf">
  <img src="http://www.star.bnl.gov/public/comp/vis/GeomBrowser/StarGeometryY2007.png"></a>
  </center><p>
  \endhtmlonly
*/   
//___________________________________________________
void StDraw3D::Print(const char *filename) const
{
   Save(filename,"wrl");
}

//! This is an overloaded member function, provided for convenience.
/*! Save the current 3D scene using the \a type  file format 
   \param  filename - the file name to save the 3d scene
   \param  type - the pre-defined file format.
   It can be 3D scene format like "wrl"/"iv" 
   or the well-known pixmap formats like "pnd", "jpg" etc 
*/
//___________________________________________________
void StDraw3D::Print(const char *filename, const char*type) const
{
   Save(filename,type);
}

//! This is an overloaded member function, provided for convenience.
/*! Save the current 3D scene using the 3D "wrl" format if possible otherwise use the \a type file format 
   \param  filename - the file name to save the 3d scene
   \param  type - the pre-defined file format.
   It can be 3D scene format like "wrl"/"iv" 
   or the well-known pixmap formats like "pnd", "jpg" etc 
*/
//___________________________________________________
void StDraw3D::Save(const char *filename, const char*type) const
{
    if ( TVirtualViewer3D *viewer = Viewer() ) {
       viewer->Print(filename);
//       viewer->PrintObjects();  
    }
    else if (Pad()) Pad()->Print(filename,type);
}

//___________________________________________________
void StDraw3D::Update()
{
   TVirtualPad *pad = Pad();
   if (pad) {
      TVirtualPad *sav = gPad;
      if (pad != sav)  pad->cd();
      assert (pad==gPad);
      pad->Update();
      if (sav && (pad != sav)) sav->cd();
   }
}

//___________________________________________________
void StDraw3D::Modified()
{
   // One doesn't need to call this method
   // because one can not change any object yet
   TVirtualPad *pad = Pad();
   if (pad) {
      TVirtualPad *sav = gPad;
      if (pad != sav)  pad->cd();
      assert (pad==gPad);
      pad->Modified();
      if (sav && (pad != sav)) sav->cd();
   }
}

//_______________________________________________________________
void StDraw3D::UpdateModified()
{
   // One doesn't need to call this method
   // because one can not change any object yet
   TVirtualPad *pad = Pad();
   if (pad) {
      TVirtualPad *sav = gPad;
      if (pad != sav)  pad->cd();
      assert (pad==gPad);
      pad->Modified();
      pad->Update();
      if (sav && (pad != sav)) sav->cd();
   }
}

//___________________________________________________
void StDraw3D::Draw3DTest(){
   //  ------------------------------------------------
   //  The  method to test the class
   //  It should produce the 3D Coin widget:
   //  <begin_html> <img src="http://www.star.bnl.gov/public/comp/vis/StDraw3D/examples/Draw3DClass.png">end_html
   //  ------------------------------------------------
   //                 x             y              z  
   //  ------------------------------------------------
   float xyz[] = { 189.195,       27.951,       123.966
                  ,187.195,       28.6187,      122.89
                  ,181.195       ,30.6788      ,119.556
                  ,179.195       ,31.3387      ,118.454
                  ,177.195       ,32.0065      ,117.328
                  ,175.195       ,32.6132      ,116.26
                  ,173.195       ,33.2385      ,115.146
                  ,171.195       ,33.8552      ,114.016
                  ,169.195       ,34.3924      ,112.964
         };

   int sizeXYZ = sizeof(xyz)/sizeof(float)/3;
   
   Draw3D(sizeXYZ,xyz);
   SetComment("The hits from the TPC sector");
   
   Line(sizeXYZ,xyz,kGlobalTrack);
   SetComment("The recontstructed track");
}

//______________________________________________________________________________
void StDraw3D::ShowDetectorTest(const char *detectorName)
{
   // Test to show the detector geometry only
   StDraw3D *viewer = new StDraw3D(detectorName); 
   if (!viewer->Viewer()) viewer->InitViewer();
}
//______________________________________________________________________________
void StDraw3D::ShowTest()
{
    // More complex test.
    //
    // It creates TWO different widgets
    // One is decorated with the detector geometry, 
    // another one "plain"
    //
    // Method does not recreate the widgets when it is called 
    // for several times
    //
    // It creates the widget at once and reuses it with each call.

   static StDraw3D *fine[2]={0};
   if (!fine[0]) {
      fine[0] = new StDraw3D;
      fine[1] = new StDraw3D(0);// View with no detector geometry decoration
   } else {
      fine[0]->Clear();
      fine[1]->Clear();
   }
//        P  G         P  G
  float NodX[100][3]= {{189.195,       27.951,       123.966}
                      ,{187.195,       28.6187,      122.89 }
                      ,{181.195       ,30.6788      ,119.556}
                      ,{179.195       ,31.3387      ,118.454}
                      ,{177.195       ,32.0065      ,117.328}
                      ,{175.195       ,32.6132      ,116.26 }
                      ,{173.195       ,33.2385      ,115.146}
                      ,{171.195       ,33.8552      ,114.016}
                      ,{169.195       ,34.3924      ,112.964}};

  float HitX[100][3]= {{189.195,       27.951,       123.966}
                      ,{187.195,       28.6187,      122.89 }
                      ,{181.195       ,30.6788      ,119.556}
                      ,{179.195       ,31.3387      ,118.454}
                      ,{177.195       ,32.0065      ,117.328}
                      ,{175.195       ,32.6132      ,116.26 }
                      ,{173.195       ,33.2385      ,115.146}
                      ,{171.195       ,33.8552      ,114.016}
                      ,{169.195       ,34.3924      ,112.964}};

  float NodL[100][3]= {{189.195+5,       27.951+10,       123.966-50}
                      ,{187.195+5,       28.6187+10,      122.89-50 }
                      ,{181.195+5       ,30.6788+10      ,119.556-50}
                      ,{179.195+5       ,31.3387+10      ,118.454-50}
                      ,{177.195+5       ,32.0065+10      ,117.328-50}
                      ,{175.195+5       ,32.6132+10      ,116.26-50 }
                      ,{173.195+5       ,33.2385+10      ,115.146-50}
                      ,{171.195+5       ,33.8552+10      ,114.016-50}};
  float HitL[100][3]= {{189.195+5,       27.951+10,       123.966-50}
                      ,{187.195+5,       28.6187+10,      122.89-50 }
                      ,{181.195+5       ,30.6788+10      ,119.556-50}
                      ,{179.195+5       ,31.3387+10      ,118.454-50}
                      ,{177.195+5       ,32.0065+10      ,117.328-50}
                      ,{175.195+5       ,32.6132+10      ,116.26-50 }
                      ,{173.195+5       ,33.2385+10      ,115.146-50}
                      ,{171.195+5       ,33.8552+10      ,114.016-50}};
  int nN=9,nH=9;
  // Draw the test points
  fine[0]->Points(nH, HitX[0], kVtx);
  fine[0]->SetComment("Hits and Geometry");
  
  fine[0]->Line  (nN, NodX[0], kGlobalTrack);  
  fine[0]->SetComment("Track and Geometry");
  
  fine[1]->Points(nH, HitL[0], kVtx);
  fine[1]->SetComment("Hits no Geometry");
  
  fine[1]->Line  (nN, NodL[0], kGlobalTrack);
  fine[1]->SetComment("Track no Geometry");
  for (int i=0;i<2;i++) { fine[i]->UpdateModified(); }
//  while(!gSystem->ProcessEvents()){}; 
}

//! Draw the TTRAP object suitable to represent the calorimeter data
/*! The method creates one "tower" object
   \param  radius - the distance between the the the base ofd the ower and the "origin"
   it can be either the distance to the Z-axis for the kBarrelStyle 
   tower or the distance to the xy plane (for End Cap towers, for example)n
   \param  lamda - the tower direction (in rads). It is the angle in respect of the Y-axis for kBarrelStyle tower or Z-axis.
   \param  dlambda - the angle "width" of the tower.
   \param dphi   - the angle "length" of the tower
   \param   col - ROOT line color ( see: http://root.cern.ch/root/html/TAttLine.html ) 
   \param   sty - ROOT line style ( see: http://root.cern.ch/root/html/TAttLine.html ) 
             one can ad kBarrelStyle constant to ROOT style to get the "barrel" style tower
   \param   siz - the height of the tower. It can used to visualize the energy deposit or for any other reason.
   \return - a pointer to the ROOT "view" TVolume created to render the input parameters.
 */

//______________________________________________________________________________
TObject *StDraw3D::Tower(float radius, float lambda, float phi, float dlambda, float dPhi, Color_t col,Style_t sty, Size_t siz)
{
   float zNear, xNear, x1Near,x2Near, yNear, y1Near,y2Near, zFar, xFar, yFar, x1Far,x2Far, y1Far, y2Far;
   zNear  = radius;
   
   yNear  = zNear*TMath::Tan(lambda );
   y1Near = zNear*TMath::Tan(lambda - dlambda/2);
   y2Near = zNear*TMath::Tan(lambda + dlambda/2);
   
   xNear  = 0;
   x1Near = TMath::Sqrt(y1Near*y1Near + zNear*zNear) * TMath::Tan(dPhi/2);
   x2Near = TMath::Sqrt(y2Near*y2Near + zNear*zNear) * TMath::Tan(dPhi/2); 

   zFar  = radius+siz;
   
   yFar  = zFar*TMath::Tan(lambda);
   y1Far = zFar*TMath::Tan(lambda - dlambda/2);
   y2Far = zFar*TMath::Tan(lambda + dlambda/2);
   
   xFar  = 0;
   x1Far = TMath::Sqrt(y1Far*y1Far + zFar*zFar) * TMath::Tan(dPhi/2);
   x2Far = TMath::Sqrt(y2Far*y2Far + zFar*zFar) * TMath::Tan(dPhi/2); 
   
   float dy = TMath::Tan(lambda )*siz/2;
   
   TTRAP *trap = new TTRAP(  "CALO", Form("Angle%d",lambda)
         , "Barrel"                // Material
         , siz/2                   // dz
         , lambda*TMath::RadToDeg()// Float_t theta (ROOT needs degree)
         , 90                      // Float_t phi   (ROOT needs degree)
         , (y2Near-y1Near )/2      // Float_t h1
         , x1Near                  // Float_t bl1
         , x2Near                  // Float_t tl1
         , 0                       // Float_t alpha1 (ROOT needs degree)
         , (y2Far-y1Far )/2        // Float_t h2
         , x1Far                   // Float_t bl2
         , x2Far                   // Float_t tl2
         , 0                       // Float_t alpha2 (ROOT needs degree)
         );
   bool draw = false;
   if (!fTopVolume) {
       draw = true;
       fTopVolume = new TVolume();
   }
   TVolume *thisShape = new TVolume("a","b",trap);
 //  TRotMatrix *rotaion = new TRotMatrix("a","b",90,90,0,90- phi*TMath::RadToDeg(),0);
 //  TRotMatrix *rotaion = new TRotMatrix("a","b",90,90,0,90,90,0);
//   fTopVolume->Add(thisShape,0,yNear+dy,zNear + siz/2,rotaion);
  bool barrel = (sty >= kBarrelStyle);

   double rotmatrixZ[] = {  1,        0,        0
                          , 0,        0,       -1
                          , 0,        1,        0
                         };
   float a = -phi+TMath::PiOver2();
   double rotmatrixX[] = {  cos(a), -sin(a), 0
                          , sin(a),  cos(a), 0
                          , 0,          0,       1
                         };
   TRotMatrix *rootMatrixX   = new TRotMatrix("rotx","rotx",rotmatrixX);
   TVolumePosition *position = fTopVolume->Add(thisShape,0,0,0,rootMatrixX);
   if (barrel) {
      TRotMatrix *rootMatrixZ   = new TRotMatrix("rotz","rotZ",rotmatrixZ);
      TVolumePosition zpos(thisShape,0,0,0,rootMatrixZ);
      position->Mult(zpos);   
   }
   TVolumePosition rpos(thisShape,0,yNear+dy,zNear + siz/2);
   position->Mult(rpos);   
   thisShape->SetFillColor(col);
   thisShape->SetLineColor(col);
   thisShape->SetFillStyle(barrel ? sty-kBarrelStyle : sty );
   if (draw) {
      Draw(fTopVolume,"same");
   } else {
      UpdateModified();
   }
   return thisShape;
}


ClassImp(StDraw3D)
