/*
 *
 * QtPcr is a PCR-1000 controls.
 * 
 * Copyright (C) 2001
 *
 *     Teepanis Chachiyo  <teepanis@physics.purdue.edu>
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

#include <qpainter.h>
#include <qpen.h>

#include "filterdisplay.h"


FilterDisplay::FilterDisplay(QWidget *parent, const char *name)
  : QWidget( parent, name )
{
  filter = DEFAULT_FILTER;
  ts = DEFAULT_TS;
  shift = DEFAULT_IFSHIFT;
}


void FilterDisplay::setIFShift( const ifshift_t shift)
{
  this->shift = shift;
  repaint();
}

void FilterDisplay::setFilter( const filter_t filter)
{
  this->filter = filter;
  repaint();
}


void FilterDisplay::setTS( const pcrfreq_t ts)
{
  this->ts = ts;
  repaint();
}


void FilterDisplay::setCTCSS( unsigned char CTCSS)
{
  this->CTCSS = CTCSS;
  repaint();
}


void FilterDisplay::paintEvent( QPaintEvent * )
{
  QPainter p( this );
  int w;
  int offset;

  // draw CTCSS status
  QColor color = p.backgroundColor();
  p.setPen( QPen(Qt::yellow) );
  p.setBackgroundColor( Qt::darkGreen );
  BGMode bmode = p.backgroundMode();
  p.setBackgroundMode( QPainter::OpaqueMode );
  if( CTCSS ){
    QString str;
    str.setNum(CTCSS_TONE[CTCSS]);
    str.prepend("CTCSS ");
    str.append(" Hz");
    p.drawText( 0, 12, str);
  }
  p.setBackgroundMode ( bmode );
  p.setBackgroundColor( color );

  
  // KISS (Keep It Simple and Stupid) algorithm
  p.setPen( QPen(Qt::green) );
  switch( ts ){
    // Dave, VK5DG, suggested low freqency step size
  case TS_10Hz   : p.drawText( 0, height(), QString("TS 10Hz  ")); break;
  case TS_25Hz   : p.drawText( 0, height(), QString("TS 25Hz  ")); break;
  case TS_50Hz   : p.drawText( 0, height(), QString("TS 50Hz  ")); break;
  case TS_100Hz  : p.drawText( 0, height(), QString("TS 100Hz ")); break;
  case TS_250Hz  : p.drawText( 0, height(), QString("TS 250Hz ")); break;
  case TS_500Hz  : p.drawText( 0, height(), QString("TS 500Hz ")); break;
  case TS_1000Hz : p.drawText( 0, height(), QString("TS 1kHz  ")); break;
  case TS_2500Hz : p.drawText( 0, height(), QString("TS 2.5kHz")); break;
  case TS_3000Hz : p.drawText( 0, height(), QString("TS 3kHz"));   break;
  case TS_5000Hz : p.drawText( 0, height(), QString("TS 5kHz  ")); break;
  case TS_9000Hz : p.drawText( 0, height(), QString("TS 9kHz  ")); break;
  case TS_10000Hz: p.drawText( 0, height(), QString("TS 10kHz ")); break;
  case TS_12500Hz: p.drawText( 0, height(), QString("TS 12.5kHz")); break;
  case TS_25000Hz: p.drawText( 0, height(), QString("TS 25kHz ")); break;
  }
  
  // draw filter picture
  p.translate( width()/2+40, height() );
  w = width()-80;
  
  p.setPen( QPen(Qt::green, 2, QPen::DotLine ));
  p.drawLine( 0, 0, 0, 15-height());
  
  p.setPen( QPen(Qt::green, 2) );
  p.drawLine( -w/2, 0, w/2, 0);
 
  offset = (int)((shift-127)*(filter+2)*w/3584.0);
  p.moveTo( clipped(-w/2,-(filter+2)*w/14+offset,w/2), 0 );
  p.lineTo( clipped(-w/2,-(filter+1)*w/14+offset,w/2), 15-height());
  p.lineTo( clipped(-w/2, (filter+1)*w/14+offset,w/2), 15-height());
  p.lineTo( clipped(-w/2, (filter+2)*w/14+offset,w/2), 0);

  p.setPen( QPen( Qt::green, 0 ));
  
  switch( filter ){
  case FILTER_230kHz:
    p.drawText( -10, 12-height(), QString("230k") );
    break;
    
  case FILTER_50kHz:
    p.drawText( -5, 12-height(), QString("50k") );
    break;
    
  case FILTER_15kHz:
    p.drawText( -5, 12-height(), QString("15k") );
    break;
    
  case FILTER_6kHz:
    p.drawText( -5, 12-height(), QString("6k") );
    break;
    
  case FILTER_3kHz:
    p.drawText( -5, 12-height(), QString("3k") );
    break;
  }
  
}











