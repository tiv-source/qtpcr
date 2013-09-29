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

#include "bargraph.h"

#include <stdio.h>
#include <math.h>
#include <qpainter.h>

BarGraph::BarGraph( QWidget *parent, const char *name)
  : QWidget( parent, name )
{
  bzero(data, 256);
  nsamples = 0;
  active = true;
  setPalette( QPalette( Qt::black ) );
//  setFocusPolicy( ClickFocus );
}


void BarGraph::setData(const unsigned char *bandScope, unsigned char val)
{
  if( !active ) return;
  
  memcpy( data, bandScope, 256);
  nsamples = val;
  repaint();
}


void BarGraph::setZero()
{
  bzero(data, 256);
  repaint();
}

void BarGraph::setPause( bool flag )
{
  if( flag )
    active = false;
  else
    active = true;
}


void BarGraph::scopeOnUpdate( bool flag)
{
  if( flag ){
    setPause( false );
  }else{
    setPause( true );
    setZero();
  }
}

void BarGraph::paintEvent( QPaintEvent * )
{
	int i, penSize; 
	QPainter p( this );

	if(nsamples <= 0) 
		return;
  
	p.scale( width()/(double)nsamples, height()/256.0);
	p.translate( -128+nsamples/2, 0);

	// calculate pen size
	penSize = width()/(nsamples + nsamples/2 );
	if(penSize <= 0)
		penSize = 1; 	
	p.setPen( QPen(Qt::cyan, penSize) );

	// draw bar graph
	for(i = 128-nsamples/2; i < 128+nsamples/2; i++)
		p.drawLine( i, 256, i, 256-data[i] );

	// draw center line
	p.setPen( QPen(Qt::green, 1) );
	p.drawLine( 128, 0, 128, 256 );
}



//
// mousrPressEvent captures when user press on the graph
// to change freqency. It sends a signal stepFreq( int )
// the integer begin how many steps to shift
//
void BarGraph::mousePressEvent( QMouseEvent *e )
{
	double bar;
	
	bar = e->pos().x()/(double)width()*(double)nsamples  - nsamples/2.0;

	emit stepFreq( (int) rint(bar) );
}
