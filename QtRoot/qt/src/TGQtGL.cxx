// @(#)root/win32gdk:$Name:  $:$Id: TGQtGL.cxx,v 1.3 2010/05/10 22:51:26 fine Exp $
// Author: Valeriy Onuchin(TGQtGL)/ Timur Pocheptsov (TGQtGLManager)

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGQtGL                                                               //
//                                                                      //
// The TGQtGL is win32gdk implementation of TVirtualGLImp class.        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <deque>

#include "TGQtGL.h"
#include "TVirtualViewer3D.h"
#include "TVirtualX.h"
#include "TError.h"
#include "TROOT.h"
#include "TMath.h"

#if 0

//______________________________________________________________________________
TGQtGL::TGQtGL()
{
   // Ctor.

   gPtr2VirtualGL = &TGQtVirtualGLProxy::ProxyObject;
   TGQtVirtualGLProxy::fMaxResponseTime = INFINITE;
}

//______________________________________________________________________________
TGQtGL::~TGQtGL()
{
   //
   gPtr2VirtualGL = 0;
}

//______________________________________________________________________________
Window_t TGQtGL::CreateGLWindow(Window_t wind)
{
   // Qt specific code to initialize GL window.

   GdkColormap *cmap;
   GdkWindow *GLWin;
   GdkWindowAttr xattr;
   int xval, yval;
   int wval, hval;
   ULong_t mask;
   int pixelformat;
   int depth;
   HDC hdc;
   static PIXELFORMATDESCRIPTOR pfd =
   {
      sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
      1,                              // version number
      PFD_DRAW_TO_WINDOW |            // support window
      PFD_SUPPORT_OPENGL |            // support OpenGL
      PFD_DOUBLEBUFFER,               // double buffered
      PFD_TYPE_RGBA,                  // RGBA type
      24,                             // 24-bit color depth
      0, 0, 0, 0, 0, 0,               // color bits ignored
      0,                              // no alpha buffer
      0,                              // shift bit ignored
      0,                              // no accumulation buffer
      0, 0, 0, 0,                     // accum bits ignored
      32,                             // 32-bit z-buffer
      0,                              // no stencil buffer
      0,                              // no auxiliary buffer
      PFD_MAIN_PLANE,                 // main layer
      0,                              // reserved
      0, 0, 0                         // layer masks ignored
   };

   gdk_window_get_geometry((GdkDrawable *) wind, &xval, &yval, &wval, &hval, &depth);
   cmap = gdk_colormap_get_system();

   // window attributes
   xattr.width = wval;
   xattr.height = hval;
   xattr.x = xval;
   xattr.y = yval;
   xattr.wclass = GDK_INPUT_OUTPUT;
   xattr.event_mask = 0L; //GDK_ALL_EVENTS_MASK;
   xattr.event_mask |= GDK_EXPOSURE_MASK | GDK_STRUCTURE_MASK |
                       GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK;
   xattr.colormap = cmap;
   mask = GDK_WA_X | GDK_WA_Y | GDK_WA_COLORMAP | GDK_WA_WMCLASS | GDK_WA_NOREDIR;
   xattr.window_type = GDK_WINDOW_CHILD;

   GLWin = gdk_window_new((GdkWindow *) wind, &xattr, mask);
   gdk_window_set_events(GLWin, (GdkEventMask)0);
   gdk_window_show(GLWin);
   hdc = GetWindowDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)GLWin));

   if ( (pixelformat = ChoosePixelFormat(hdc,&pfd)) == 0 ) {
       Error("InitGLWindow", "Barf! ChoosePixelFormat Failed");
   }
   if ( (SetPixelFormat(hdc, pixelformat,&pfd)) == 0 ) {
      Error("InitGLWindow", "Barf! SetPixelFormat Failed");
   }

   return (Window_t)GLWin;
}

//______________________________________________________________________________
ULong_t TGQtGL::CreateContext(Window_t wind)
{
   //

   HDC hdc = GetWindowDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)wind));
   ULong_t retVal = (ULong_t)wglCreateContext(hdc);
   ReleaseDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)wind), hdc);
   return retVal;
}

//______________________________________________________________________________
void TGQtGL::DeleteContext(ULong_t ctx)
{
   //

   ::wglDeleteContext((HGLRC)ctx);
}

//______________________________________________________________________________
void TGQtGL::MakeCurrent(Window_t wind, ULong_t ctx)
{
   //

   HDC hdc = GetWindowDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)wind));
   ::wglMakeCurrent(hdc,(HGLRC) ctx);
   ReleaseDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)wind), hdc);
}

//______________________________________________________________________________
void TGQtGL::SwapBuffers(Window_t wind)
{
   //

   HDC hdc = GetWindowDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)wind));
   ::wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
   ::glFinish();
   ReleaseDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)wind), hdc);
}

//Win32 GL Manager's stuff
struct TGQtGLManager::TGLContext {
   Int_t        fWindowIndex;
   Int_t        fPixmapIndex;
   //
   HDC          fDC;
   HBITMAP      fHBitmap;
   HGLRC        fGLContext;
   //
   UInt_t       fW;
   UInt_t       fH;
   //
   Int_t        fX;
   Int_t        fY;
   //
   Bool_t       fDirect;
   //
   UChar_t     *fDIBData;
   //
   TGLContext  *fNextFreeContext;
};

namespace {

   //RAII class for HDC, returned by CreateCompatibleDC
   class CDCGuard {
   private:
      HDC fHDC;

      CDCGuard(const CDCGuard &);
      CDCGuard &operator = (const CDCGuard &);

   public:
      explicit CDCGuard(HDC hDC) : fHDC(hDC)
      {}
      ~CDCGuard()
      {
         if (fHDC)
            DeleteDC(fHDC);
      }
      void Stop()
      {
         fHDC = 0;
      }   
   };

   //RAII class for HDC, returned by GetWindowDC
   class WDCGuard {
   private:
      HDC fHDC;
      Window_t fWinID;

      WDCGuard(const WDCGuard &);
      WDCGuard &operator = (const WDCGuard &);

   public:
      WDCGuard(HDC hDC, Window_t winID) : fHDC(hDC), fWinID(winID)
      {}
      ~WDCGuard()
      {
         if (fHDC)
            ReleaseDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)fWinID), fHDC);
      }
      void Stop()
      {
         fHDC = 0;
      }
   };

   //RAII class for HBITMAP
   class BMPGuard {
   private:
      HBITMAP fBMP;

      BMPGuard(const BMPGuard &);
      BMPGuard &operator = (const BMPGuard &);

   public:
      explicit BMPGuard(HBITMAP bmp) : fBMP(bmp)
      {}
      ~BMPGuard()
      {
         if (fBMP)
            DeleteObject(fBMP);
      }
      void Stop()
      {
         fBMP = 0;
      }
   };

   //RAII class for HGLRC
   class WGLGuard {
   private:
      HGLRC fCtx;

      WGLGuard(const WGLGuard &);
      WGLGuard &operator = (const WGLGuard &);

   public:
      explicit WGLGuard(HGLRC glrc) : fCtx(glrc)
      {}
      ~WGLGuard()
      {
         if (fCtx) 
            wglDeleteContext(fCtx);
      }
      void Stop()
      {
         fCtx = 0;
      }
   };
}

const PIXELFORMATDESCRIPTOR
doubleBufferDesc = {
   sizeof doubleBufferDesc,	   // size of this pfd
   1,                              // version number
   PFD_DRAW_TO_WINDOW |            // support window
   PFD_SUPPORT_OPENGL |            // support OpenGL
   PFD_DOUBLEBUFFER,               // double buffered
   PFD_TYPE_RGBA,                  // RGBA type
   24,                             // 24-bit color depth
   0, 0, 0, 0, 0, 0,               // color bits ignored
   0,                              // no alpha buffer
   0,                              // shift bit ignored
   0,                              // no accumulation buffer
   0, 0, 0, 0,                     // accum bits ignored
   32,                             // 32-bit z-buffer
   0,                              // no stencil buffer
   0,                              // no auxiliary buffer
   PFD_MAIN_PLANE                  // main layer
};

const PIXELFORMATDESCRIPTOR
singleScreenDesc = {
   sizeof singleScreenDesc,	     // size of this pfd
   1,                              // version number
   PFD_DRAW_TO_BITMAP |	           // draw into bitmap
   PFD_SUPPORT_OPENGL,             // support OpenGL
   PFD_TYPE_RGBA,                  // RGBA type
   24,                             // 24-bit color depth
   0, 0, 0, 0, 0, 0,               // color bits ignored
   0,                              // no alpha buffer
   0,                              // shift bit ignored
   0,                              // no accumulation buffer
   0, 0, 0, 0,                     // accum bits ignored
   32,                             // 32-bit z-buffer
   0,                              // no stencil buffer
   0,                              // no auxiliary buffer
   PFD_MAIN_PLANE                  // main layer
};

class TGQtGLManager::TGQtGLImpl {
public:
   TGQtGLImpl() : fNextFreeContext(0)
   {}
   ~TGQtGLImpl();
   std::deque<TGLContext> fGLContexts;
   TGLContext *fNextFreeContext;
};

TGQtGLManager::TGQtGLImpl::~TGQtGLImpl()
{
   //all devices should be destroyed at this moment
   std::deque<TGLContext>::size_type i = 0;

   for (; i < fGLContexts.size(); ++i) {
      TGLContext &ctx = fGLContexts[i];      

      if (ctx.fGLContext) {
         //gl context (+DIB, if exists) must be destroyed from outside, by pad.
         ::Warning("TGQtGLManager::~TGLWin32GLManager", "You forget to destroy gl-context %d\n", i);
         //destroy hdc and glrc, pixmap will be destroyed by TVirtualX
         if (ctx.fPixmapIndex != -1) {
            gVirtualX->SelectWindow(ctx.fPixmapIndex);
            gVirtualX->ClosePixmap();
         }

         wglDeleteContext(ctx.fGLContext);
         ReleaseDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)gVirtualX->GetWindowID(ctx.fWindowIndex)),
                   ctx.fDC);
      }
   }
}

ClassImp(TGQtGLManager)

//______________________________________________________________________________
TGQtGLManager::TGQtGLManager() : fPimpl(new TGQtGLImpl)
{
   gPtr2GLManager = &TGQtGLManagerProxy::ProxyObject;
   gROOT->GetListOfSpecials()->AddLast(this);
   gGLManager = this;
}

//______________________________________________________________________________
TGQtGLManager::~TGQtGLManager()
{
   delete fPimpl;
}

//______________________________________________________________________________
Int_t TGQtGLManager::InitGLWindow(Window_t winID)
{
   return gVirtualX->InitWindow(winID);
}

//______________________________________________________________________________
Int_t TGQtGLManager::CreateGLContext(Int_t winInd)
{
   //winInd is TGQt index, returned by previous call gGLManager->InitGLWindow
   //returns descripto (index) of gl context or -1 if failed
   Window_t winID = gVirtualX->GetWindowID(winInd);
   HDC hDC = GetWindowDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)winID));
   
   if (!hDC) {
      Error("CreateGLContext", "GetWindowDC failed\n");
      return -1; 
   }

   WDCGuard dcGuard(hDC, winID);

   if (Int_t pixFormat = ChoosePixelFormat(hDC, &doubleBufferDesc)) {
      if (SetPixelFormat(hDC, pixFormat, &doubleBufferDesc)) {
         HGLRC glCtx = wglCreateContext(hDC);

         if (!glCtx) {
            Error("CreateGLContext", "wglCreateContext failed\n");
            return -1;
         }

         TGLContext newDevice = {winInd, -1, hDC, 0, glCtx};

         if (TGLContext *ctx = fPimpl->fNextFreeContext) {
            Int_t ind = ctx->fWindowIndex;
            fPimpl->fNextFreeContext = fPimpl->fNextFreeContext->fNextFreeContext;
            *ctx = newDevice;
            dcGuard.Stop();
            return ind;
         } else {
            WGLGuard wglGuard(glCtx);
            fPimpl->fGLContexts.push_back(newDevice);
            wglGuard.Stop();
            dcGuard.Stop();
            return fPimpl->fGLContexts.size() - 1;
         }
      } else
         Error("CreateGLContext", "SetPixelFormat failed\n");
   } else
      Error("CreateGLContext", "ChoosePixelFormat failed\n");

   return -1;
}

//______________________________________________________________________________
Bool_t TGQtGLManager::CreateDIB(TGLContext &ctx)const
{
   //Create DIB section to read GL buffer into
   HDC dibDC = CreateCompatibleDC(0);

   if (!dibDC) {
      Error("CreateDIB", "CreateCompatibleDC failed\n");
      return kFALSE;
   }

   CDCGuard dcGuard(dibDC);
	
   BITMAPINFOHEADER bmpHeader = {sizeof bmpHeader, ctx.fW, ctx.fH, 1, 24, BI_RGB};
   void *bmpCnt = 0;
   HBITMAP hDIB = CreateDIBSection(dibDC, (BITMAPINFO*)&bmpHeader, DIB_RGB_COLORS, &bmpCnt, 0, 0);
   
   if (!hDIB) {
      Error("CreateDIB", "CreateDIBSection failed\n");
      return kFALSE;
   }

   BMPGuard bmpGuard(hDIB);

   ctx.fPixmapIndex = gVirtualX->AddPixmap((ULong_t)hDIB, ctx.fW, ctx.fH);
   ctx.fHBitmap = hDIB;
   ctx.fDIBData = static_cast<UChar_t *>(bmpCnt);

   bmpGuard.Stop();

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGQtGLManager::AttachOffScreenDevice(Int_t ctxInd, Int_t x, Int_t y, UInt_t w, UInt_t h)
{
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];
   TGLContext newCtx = {ctx.fWindowIndex, -1, ctx.fDC, 0, ctx.fGLContext, w, h, x, y};

   if (CreateDIB(newCtx)) {
      ctx = newCtx;
      return kTRUE;
   }

   return kFALSE;
}

//______________________________________________________________________________
Bool_t TGQtGLManager::ResizeOffScreenDevice(Int_t ctxInd, Int_t x, Int_t y, UInt_t w, UInt_t h)
{
   //Create new DIB if needed
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];

   if (ctx.fPixmapIndex != -1)
      if (TMath::Abs(Int_t(w) - Int_t(ctx.fW)) > 1 || TMath::Abs(Int_t(h) - Int_t(ctx.fH)) > 1) {
         TGLContext newCtx = {ctx.fWindowIndex, -1, ctx.fDC, 0, ctx.fGLContext, w, h, x, y};
         if (CreateDIB(newCtx)) {
            //new DIB created
            gVirtualX->SelectWindow(ctx.fPixmapIndex);
            gVirtualX->ClosePixmap();
            ctx = newCtx;
         } else {
            Error("ResizeOffScreenDevice", "Error trying to create new DIB\n");
            return kFALSE;
         }
      } else {
         ctx.fX = x;
         ctx.fY = y;
      }

   return kTRUE;
}

//______________________________________________________________________________
void TGQtGLManager::SelectOffScreenDevice(Int_t ctxInd)
{
   gVirtualX->SelectWindow(fPimpl->fGLContexts[ctxInd].fPixmapIndex);
}

//______________________________________________________________________________
void TGQtGLManager::MarkForDirectCopy(Int_t pixInd, Bool_t isDirect)
{
   if (fPimpl->fGLContexts[pixInd].fPixmapIndex != -1)
      fPimpl->fGLContexts[pixInd].fDirect = isDirect;
}

//______________________________________________________________________________
void TGQtGLManager::ReadGLBuffer(Int_t ctxInd)
{
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];

   if (ctx.fPixmapIndex != -1) {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glReadBuffer(GL_BACK);
      glReadPixels(0, 0, ctx.fW, ctx.fH, GL_BGR_EXT, GL_UNSIGNED_BYTE, ctx.fDIBData);
   }
}

//______________________________________________________________________________
Int_t TGQtGLManager::GetVirtualXInd(Int_t ctxInd)
{
   return fPimpl->fGLContexts[ctxInd].fPixmapIndex;
}

//______________________________________________________________________________
Bool_t TGQtGLManager::MakeCurrent(Int_t ctxInd)
{
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];
   return (Bool_t)wglMakeCurrent(ctx.fDC, ctx.fGLContext);
}

//______________________________________________________________________________
void TGQtGLManager::Flush(Int_t ctxInd)
{
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];

   if (ctx.fPixmapIndex == -1) {
      //doube-buffered OpenGL
      wglSwapLayerBuffers(ctx.fDC, WGL_SWAP_MAIN_PLANE);
   } else if (ctx.fDirect) {
      //DIB is flushed by viewer directly
      HDC hDC = CreateCompatibleDC(0);

      if (!hDC) {
         Error("Flush", "CreateCompatibleDC failed\n");
         return;
      }

      HBITMAP oldDIB = (HBITMAP)SelectObject(hDC, ctx.fHBitmap);

      if (!BitBlt(ctx.fDC, ctx.fX, ctx.fY, ctx.fW, ctx.fH, hDC, 0, 0, SRCCOPY))
         ctx.fDirect = kFALSE;

      SelectObject(hDC, oldDIB);
      DeleteDC(hDC);
   }
   //do nothing for non-direct off-screen device
}

//______________________________________________________________________________
void TGQtGLManager::DeleteGLContext(Int_t ctxInd)
{
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];

   if (ctx.fPixmapIndex != -1) {
      gVirtualX->SelectWindow(ctx.fPixmapIndex);
      gVirtualX->ClosePixmap();
      ctx.fPixmapIndex = -1;
   }

   wglDeleteContext(ctx.fGLContext);
   ctx.fGLContext = 0;
   ReleaseDC((HWND)GDK_DRAWABLE_XID((GdkWindow *)gVirtualX->GetWindowID(ctx.fWindowIndex)), 
             ctx.fDC);
   //now, save its own index before putting into list of free devices
   ctx.fWindowIndex = ctxInd;
   ctx.fNextFreeContext = fPimpl->fNextFreeContext;
   fPimpl->fNextFreeContext = &ctx;
}

//______________________________________________________________________________
void TGQtGLManager::ExtractViewport(Int_t ctxInd, Int_t *viewport)
{
   TGLContext &ctx = fPimpl->fGLContexts[ctxInd];

   if (ctx.fPixmapIndex != -1) {
      viewport[0] = 0;
      viewport[1] = 0;
      viewport[2] = ctx.fW;
      viewport[3] = ctx.fH;
   }
}

//______________________________________________________________________________
void TGQtGLManager::DrawViewer(TVirtualViewer3D *vv)
{
   vv->DrawViewer();
}

//______________________________________________________________________________
TObject *TGQtGLManager::Select(TVirtualViewer3D *vv, Int_t x, Int_t y)
{
   return vv->SelectObject(x, y);
}

//______________________________________________________________________________
void TGQtGLManager::PaintSingleObject(TVirtualGLPainter *p)
{
   p->Paint();
}

//______________________________________________________________________________
void TGQtGLManager::PrintViewer(TVirtualViewer3D *vv)
{
   vv->PrintObjects();
}
#endif