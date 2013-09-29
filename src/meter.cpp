/*
 *
 * QtPcr is a PCR-1000 controls.
 * 
 * Copyright (C) 2001
 *
 *     Teepanis Chachiyo   <teepanis@physics.purdue.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.     
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.              
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software   
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <math.h>
#include <qpainter.h>

#include "meter.h"

#ifdef DEBUG_VER_
#include <stdio.h>
#endif // DEBUG_VER_

Meter::Meter( QWidget *parent, const char *name)
	: QWidget( parent, name )
{
	range.setRange( 0, 255 );
	range.setValue( 0 );
	range.setSteps( 1, 1 );

	ang_init = 0;
	len = 0;

	setBackgroundColor( Qt::black );
	setFocusPolicy( NoFocus );
	squelchOn = false;

   // update every 50 millisecond
   startTimer( 50 );
   needUpdate = false;
}

void Meter::timerEvent ( QTimerEvent * ) 
{
    if(needUpdate){
      needUpdate = false;
      repaint();
    }
}

void Meter::setValue( unsigned char val )
{
	range.setValue( val );
    needUpdate = true;
}

void Meter::setSquelchOn( bool val )
{
	squelchOn = val;
    needUpdate = true;
}

void Meter::paintEvent( QPaintEvent *e )
{
	QPainter p( this );
	float ratio;


	p.translate( width()/2, height() );

	if(squelchOn)
		p.setPen( Qt::green );
	else
		p.setPen( Qt::gray );

	p.drawText(-width()/4,-height()/2,width()/2, height()/4, 
				QPainter::AlignHCenter, "SIGNAL");

	p.setBrush( NoBrush );
	p.setPen( QPen( Qt::white, 2, QPainter::DashLine ) );
	p.drawEllipse( -len, -len+5, 2*len, 2*len );

	ratio = (double)(range.value() - range.minValue())/(range.maxValue()-range.minValue());
	p.rotate( ang_init + 180 + ratio*(180 - 2*ang_init) );

	p.setPen( QPen(Qt::yellow, 3) );
	p.drawLine( 0, 0, len,  0 );
}

void Meter::resizeEvent( QResizeEvent *e )
{
	len = e->size().height();
	if( 2*len > e->size().width() )
		len = (int)(e->size().width()*0.5774); 

	ang_init = (int)(acos( e->size().width()/2.0/len ) * 180.0 / M_PI);

#ifdef DEBUG_VER_
	fprintf( stderr, "Meter::resizeEvent - ang_init = %d\n", ang_init );
#endif // DEBUG_VER_ 
}
