// @(#)root/qt:$Id: TQtMarker.cxx,v 1.6 2010/03/03 21:17:30 fine Exp $
// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: TQtMarker.cxx,v 1.6 2010/03/03 21:17:30 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    Al;l rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#include "TQtRConfig.h"
#include "TQtMarker.h"
#include "TAttMarker.h"
#include "TGQt.h"
#include <QtGui/QPolygon>
#include <QtGui/QPainter>


ClassImp(TQtMarker)

////////////////////////////////////////////////////////////////////////
//
// TQtMarker - class-utility to convert the ROOT TMarker object shape 
//             into the Qt QPointArray.
//
////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TQtMarker::TQtMarker(int n, TPoint *xy, int type) : fNumNode(n),
               fChain(0), fCindex(0), fMarkerType(0),fLineWidth(0)
{
  SetPenAttributes(type);
  if (GetType() != kDot) {
     fChain.resize(n);
     TPoint *rootPoint = xy;
     for (int i=0;i<n;i++,rootPoint++)
        fChain.setPoint(i,rootPoint->fX,rootPoint->fY);
  }
}
//______________________________________________________________________________
TQtMarker::~TQtMarker(){}
//______________________________________________________________________________
TQtMarker &TQtMarker::operator=(const TAttMarker&markerAttributes)
{
   // Assign the TQtMarker from ROOT TAttMarker 
	SetMarkerAttributes(markerAttributes);
	return *this;
}

//______________________________________________________________________________
TQtMarker::TQtMarker(const TAttMarker &markerAttributes)
{
   // Create the TQtMarker from ROOT TAttMarker 
	SetMarkerAttributes(markerAttributes);
}

//______________________________________________________________________________
void TQtMarker::SetMarkerAttributes(const TAttMarker& markerAttributes)
{
   // Map Qt marker  attributes to ROOT TAttMaker parameters
   fCindex     = markerAttributes.GetMarkerColor();
   SetPenAttributes(markerAttributes.GetMarkerStyle());
  // to be done yet:  fNumNode    = markerAttributes.GetMarkerSize();
}

//______________________________________________________________________________
void  TQtMarker::SetPenAttributes(int type)
{
   // Pen attrbutes is 1000*width + "marker style"
   static const int packFactor = 1000;
   fMarkerType = type%packFactor;
   fLineWidth = (type - fMarkerType)/packFactor;
}
//______________________________________________________________________________
int   TQtMarker::GetNumber() const {return fNumNode;}
//______________________________________________________________________________
const QPolygon &TQtMarker::GetNodes() const {return fChain;}
//______________________________________________________________________________
int  TQtMarker::GetType()  const {return fMarkerType;}

//______________________________________________________________________________
int  TQtMarker::GetWidth() const { return fLineWidth;}

//______________________________________________________________________________
void TQtMarker::SetMarker(int n, TPoint *xy, int type)
{
//*-* Did we have a chain ?
  fNumNode = n;
  SetPenAttributes(type);
  if (GetType() != kDot) {
    fChain.resize(n);
    TPoint *rootPoint = xy;
    for (int i=0;i<n;i++,rootPoint++)
       fChain.setPoint(i,rootPoint->fX,rootPoint->fY);
  }
}

//______________________________________________________________________________
void  TQtMarker::DrawPolyMarker(QPainter &p, int n, TPoint *xy)
{
	// Draw n markers with the current attributes at positions xy.
	// p    : the external QPainter 
   // n    : number of markers to draw
	// xy   : x,y coordinates of markers
	
	/* Set marker Color */
	const QColor &mColor  = gQt->ColorIndex(fCindex);

	p.save();
	if( this->GetNumber() <= 0 )
	{
		p.setPen(mColor);
		QPolygon qtPoints(n);
		TPoint *rootPoint = xy;
		for (int i=0;i<n;i++,rootPoint++)
			qtPoints.setPoint(i,rootPoint->fX,rootPoint->fY);
		p.drawPoints(qtPoints);
	} else {
		int r = this->GetNumber()/2;
      if (this->GetWidth()>0) {
         p.setPen(QPen(mColor,this->GetWidth()));
      } else {
         p.setPen(mColor);
      }
		switch (this -> GetType())
		{
		case 1:
		case 3:
		default:
			p.setBrush(mColor);
			break;
		case 0:
		case 2:
			p.setBrush(Qt::NoBrush);
			break;
		case 4:
			break;
		}

		for( int m = 0; m < n; m++ )
		{
			int i;
			switch( this->GetType() )
			{
			case 0:        /* hollow circle */
			case 1:        /* filled circle */
				p.drawEllipse(xy[m].fX-r, xy[m].fY-r, 2*r, 2*r);
				break;
			case 2:        /* hollow polygon */
			case 3:        /* filled polygon */
				{
					QPolygon mxy = this->GetNodes();
					mxy.translate(xy[m].fX,xy[m].fY);
					p.drawPolygon(mxy);
					break;
				}
			case 4:        /* segmented line */
				{
					QPolygon mxy = this->GetNodes();
					mxy.translate(xy[m].fX,xy[m].fY);
					QVector<QLine> lines(this->GetNumber());
					for( i = 0; i < this->GetNumber(); i+=2 )
						lines.push_back(QLine(mxy.point(i),mxy.point(i+1)));
					p.drawLines(lines);
					break;
				}
			}
		}
	}
	p.restore();
}
