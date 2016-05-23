// $Id: TQtCanvasPainter.h,v 1.1.4.1 2016/05/23 18:32:45 jeromel Exp $
// Author: Valeri Fine   21/05/2010

#ifndef ROOT_TQTCANVASPAINTER
#define ROOT_TQTCANVASPAINTER

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// TQtCanvasPainter                                                        //
//                                                                         //
// TQtCanvasPainter is abstract visitor interface                          //
// to customize TQtWidget painting                                         //
// It allows the arbitrary low level Qt painting onto the TQtWidget face   //
// on the top of  TCanvas image                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include <QObject>

class TQtCanvasPainter : public QObject 
{
    protected:
       TQtCanvasPainter(){}
    public:
       TQtCanvasPainter(QObject *mother) : QObject(mother) {}
       virtual ~TQtCanvasPainter() {}
       virtual void paintEvent(QPainter &painter, QPaintEvent *e=0) = 0;
//     ClassDef(TQtCanvasPainter,0); // Interface to use the Qt graphics to custom paint over ROOT TCanvas 
};

#endif
