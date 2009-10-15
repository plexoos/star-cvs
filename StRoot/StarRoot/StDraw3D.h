#ifndef STAR_StDraw3D
#define STAR_StDraw3D
// $Id: StDraw3D.h,v 1.31 2009/10/15 22:30:50 fine Exp $
// *-- Author :    Valery Fine(fine@bnl.gov)   27/04/2008

#include "TObject.h"
#include "Gtypes.h"
#include "TString.h"
#include <map>

class TVirtualPad;
class TVirtualViewer3D;

//! EDraw3DStyle defines the set of the pre-defined "STAR event component" styles
enum EDraw3DStyle {kVtx           //!< "Vertex" style
                  ,kPrimaryTrack  //!< "Primary" track style
                  ,kGlobalTrack   //!< "Global" track style
                  ,kUsedHit       //!< "Used" hit style
                  ,kUnusedHit     //!< "Unsed" hit style
                  ,kTrackBegin    //!< The track "begin" point
                  ,kTrackEnd      //!< The track "end" point
                  ,kUser          //!< "Custom" style
};

//! StDraw3DStyle maps "STAR event" EDraw3DStyle onto ROOT  \b (color,style,size) attributes
class StDraw3DStyle {
   private:
       EDraw3DStyle  fType; 
       Color_t fColor;
       Style_t fSty;
       Size_t  fSiz;
   public:
     StDraw3DStyle(EDraw3DStyle typ=kUsedHit
          ,  Color_t col = Color_t(-1)
          ,  Style_t sty = Style_t(-1)
          ,  Size_t  siz = Size_t (-1))
      : fType(typ),fColor(col),fSty(sty),fSiz(siz) {}
     Color_t      Col()  const { return fColor;}
     Style_t      Sty()  const { return fSty;  }
     Size_t       Siz()  const { return fSiz;  }
     EDraw3DStyle Type() const { return fType; }
     Color_t      &Col()  { return fColor;}
     Style_t      &Sty()  { return fSty;  }
     Size_t       &Siz()  { return fSiz;  }
     EDraw3DStyle &Type() { return fType; }
     void         SetCol(Color_t col) { Col() = col;}
     void         SetSty(Style_t sty) { Sty() = sty;}
     void         SetSiz(Size_t  siz) { Siz() = siz;}
     void         SetType(Color_t col, Style_t sty, Size_t  siz)
                  { SetCol(col); SetSiz(siz); SetSty(sty);  }
     static Color_t Pt2Color(double pt);
};


class view_3D;

//! \author Valery Fine(fine@bnl.gov)
//! \date 27/04/2008

/*! \brief  Class StDraw3D - to draw the 3D primitives like 3D points and 3D lines
 *          decorated with the STAR detector geometry
 */
/// \author Valery Fine (fine@bnl.gov)
/// \date 27/04/2008
/// \sa Draw3D.C
///
///  Class provides the simple way to visualize the event 
///  primitives in 3D against of the STAR detector 
///  geometry quickly.
///
/// \n Try:
  /// \code
  ///  > starnew	
  ///  > ln -s  $STAR/QtRoot/qtExamples/QtGBrowser/.rootrc
  ///  > root.exe Draw3D.C
  /// \endcode
  ///  to get the test picture below:\n
  ///  <img src="http://www.star.bnl.gov/public/comp/vis/StDraw3D/examples/Draw3DClass.png">
  ///  \image html Draw3DClass.png
  ///  The base StDraw3D class is a controller connecting the arbitrary "model" object 
  ///  with the arbitrary ROOT 3D class "view" object to implement the so-called 
  ///  <a href="http://en.wikipedia.org/wiki/Model-view-controller"> "Model / View" paradigm</a>. \n
  ///  In our case, the "Model" is an arbitrary object and the "View" is an instance of some ROOT 3D class. 
  ///  To render views the StDraw3D instantiates the TCanvas and TVirtualViewer3D to allow 
  ///  the user select interactively the "view" instance and invoke the model methods like:
  ///
  ///  \li \c TObject::Inspect() to inspect the model data-member values; 
  ///  \li \c TObject::GetObjectInfo to label the view with the current model information; 
  ///  \li \c TVirtualPadEditor to set the model attributes.
class StDraw3D : public TObject
{
   private:
       static int fgDraw_3d_init;
       std::map<EDraw3DStyle,StDraw3DStyle> fStyles;
       TVirtualPad *fPad;
       Color_t  fBkColor; // background color
       TVirtualViewer3D *fViewer; 
       view_3D *fView;
       TString fDetectorName;
       StDraw3D *fMaster;

       static Color_t fgColorDefault;
       static Style_t fgStyDefault;
       static Size_t  fgSizDefault;
       static Color_t fgBkColor;
       static Int_t   fDrawCanvasCounter;
       TVirtualPad *InitPad();
       void SetMaster(StDraw3D *master);
       void InitViewer();
       void Redraw();

public:
   StDraw3D(const char *detectorName="TPC",TVirtualPad *pad = 0);
   virtual ~StDraw3D();
   virtual const StDraw3DStyle &AddStyle(EDraw3DStyle type,Color_t col,Style_t sty,Size_t siz);
   TVirtualPad *Pad() const;
   TVirtualViewer3D *Viewer() const;
   virtual void  Clear(Option_t *opt="");
   virtual TObject *Draw(TObject *o);
   virtual const TString &DetectorNames() const;
   virtual void  SetDetectors(const char*nameDetectors);
   virtual void  AddDetectors(const char*nameDetectors);
   virtual void  Draw(Option_t *option="") {TObject::Draw(option);}
   virtual const StDraw3DStyle &Style(EDraw3DStyle type);
   virtual void  SetBkColor(Color_t newBkColor);

   virtual TObject *Draw3D(int n,  const float *xyz);

   virtual TObject *Points(int n, const float *xyz
         ,  EDraw3DStyle sty);

   virtual TObject *Points(int n, const float *xyz
         ,  Color_t col= Color_t(-1)
         ,  Style_t sty= Style_t(-1)
         ,  Size_t siz = Size_t (-1));

   virtual TObject *Point(float x, float y, float z
         ,  Color_t col= Color_t(-1)
         ,  Style_t sty= Style_t(-1)
         ,  Size_t siz = Size_t (-1));

   virtual TObject *Point(float x, float y, float z
         ,  EDraw3DStyle sty);

   virtual TObject *Line(int n,  const float *xyz
         ,  Color_t col= Color_t(-1)
         ,  Style_t sty= Style_t(-1)
         ,  Size_t siz = Size_t (-1));

   virtual TObject *Line(int n,  const float *xyz
         ,  EDraw3DStyle sty);

   virtual TObject *Trap(float radius, float lamda, float phi, float w, float h, Color_t col,Style_t sty, Size_t siz);

   virtual void Joint(StDraw3D *dsp);
   virtual void SetModel(TObject *model);
   virtual void SetComment(const char *cmnt);
   virtual void AddComment(const char *cmnt);
   virtual void Print(const char *filename) const ;
   virtual void Print(const char *filename, const char*type) const ;
   virtual void Save(const char *filename, const char*type="png") const ;
   virtual void Update();
   virtual void Modified();

    void Draw3DTest();
    static void ShowTest();
    static void ShowDetectorTest(const char *detectorName="StarDetectorUnfolding");
    ClassDef(StDraw3D,0);
};
#endif
