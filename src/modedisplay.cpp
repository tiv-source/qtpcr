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

#include <qpainter.h>

#include "modedisplay.h"

ModeDisplay::ModeDisplay( QWidget *parent, const char *name)
	: QWidget( parent, name )
{
	att = false;
	agc = false;
	nb  = false;
	vsc = false;
	afc = false;
	mode = DEFAULT_MODE;

	setBackgroundColor( Qt::black );
}

void ModeDisplay::paintEvent( QPaintEvent *e)
{
	int w, h;
	QPainter p( this );

#define setModeDisplayColor( val ) if( val ) \
                                     p.setPen( Qt::green); \
                                    else \
                                     p.setPen( Qt::gray );
	
	w = width()/3;
	h = height()/4;

	setModeDisplayColor( att );
	p.drawText( w, h*3, "ATT" );

	setModeDisplayColor( vsc );	
	p.drawText( 0, h*3, "VSC" );

	setModeDisplayColor( nb );
	p.drawText( 2*w, h*4, "NB");

	setModeDisplayColor( agc );
	p.drawText( w, h*4, "AGC" );

	setModeDisplayColor( afc );
	p.drawText( 0,h*4, "AFC"); 
		
	p.setPen( Qt::green );
	p.setFont( QFont("Time", 25, QFont::Bold, true) );
	switch( mode ){
	case MODE_NFM: p.drawText(0, h*2, "FM" ); break;
	case MODE_WFM: p.drawText(0, h*2, "WFM" ); break;
	case MODE_AM: p.drawText( 0, h*2, "AM" ); break;
	case MODE_CW: p.drawText( 0, h*2, "CW" ); break;
	case MODE_USB: p.drawText(0, h*2, "USB"); break;
	case MODE_LSB: p.drawText(0, h*2, "LSB"); break;
	} 
}

void ModeDisplay::attUpdate( bool val) { this->att = val; repaint(); }
void ModeDisplay::vscUpdate( bool val) { this->vsc = val; repaint(); }
void ModeDisplay::afcUpdate( bool val) { this->afc = val; repaint(); }
void ModeDisplay::agcUpdate( bool val) { this->agc = val; repaint(); }
void ModeDisplay::nbUpdate(  bool val) { this->nb  = val; repaint(); }
void ModeDisplay::modeUpdate( modulate_t mode) { this->mode = mode; repaint(); }
