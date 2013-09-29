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

#include "filterctrl.h"
#include <stdio.h>

FilterCtrl::FilterCtrl( QWidget *parent, const char *name)
	: QWidget( parent, name )
{
	wide = new QPushButton("WIDE", this );
	nar  = new QPushButton("NAR",  this );
	reset = new QPushButton("ZERO",this );
	tswide = new QPushButton("+", this );
	tsnar = new QPushButton( "-", this );
	tslabel = new QLabel("TS", this );

	// Bug fixed: Dave, VK5DG. IF shift ranges 0-->255
	ifshift = new QSlider( 0, 255, 1, DEFAULT_IFSHIFT, Qt::Horizontal, this );

	wide->setFocusPolicy( NoFocus );
	nar->setFocusPolicy( NoFocus );
	reset->setFocusPolicy( NoFocus );
	tswide->setFocusPolicy( NoFocus );
	tsnar->setFocusPolicy( NoFocus );
	ifshift->setFocusPolicy( NoFocus );

	setBackgroundColor( Qt::darkGray );
	wide->setFont( QFont("Time", 8));
	nar->setFont( QFont("Time", 8));
	reset->setFont( QFont("Time", 8));
	tslabel->setFont( QFont("Time", 8) );

	tslabel->setAlignment( Qt::AlignHCenter );
	tslabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );

	connect( wide, SIGNAL(clicked()), this, SLOT( wideSlot()) );
	connect( nar,  SIGNAL(clicked()), this, SLOT( narSlot())  );
	connect( tswide, SIGNAL(clicked()), this, SIGNAL(stepUpTS()) );
	connect( tsnar, SIGNAL(clicked()), this, SIGNAL(stepDownTS()) );
	connect( reset, SIGNAL(clicked()), this, SLOT( resetIF()) );
	connect( ifshift, SIGNAL(valueChanged(int)),this, SLOT(ifShiftMoved(int)));
}

FilterCtrl::~FilterCtrl()
{
	delete wide;
	delete nar;
	delete reset;
	delete ifshift;

	delete tswide;
	delete tsnar;
	delete tslabel;
}


void FilterCtrl::wideSlot()
{
#ifdef DEBUG_VER_
	fprintf(stderr, "FilterCtrl::wideSlot\n");
#endif //DEBUG_VER_
	emit stepUpFilter();
}

void FilterCtrl::narSlot()
{
#ifdef DEBUG_VER_
    fprintf(stderr, "FilterCtrl::narSlot\n");
#endif //DEBUG_VER_

	emit stepDownFilter();
}

void FilterCtrl::resizeEvent( QResizeEvent *e)
{
	int w,h;
	
	w = e->size().width()/3;
	h = e->size().height()/8;

	ifshift->setGeometry( 0, 0, e->size().width(), h*2 );
	wide->setGeometry( w*2, h*2, w, h*3);
	nar->setGeometry(  0, h*2, w, h*3);
	reset->setGeometry(w,h*2, w, h*3);
	
	tswide->setGeometry( w*2, h*5, w, h*3);
	tslabel->setGeometry(w  , h*5, w, h*3);
	tsnar->setGeometry(  0,   h*5, w, h*3);
}


void FilterCtrl::resetIF()
{
  // Bug fixed: Dave, VK5DG.
  ifshift->setValue(127);
} 

void FilterCtrl::ifShiftMoved(int shift)
{
  emit setIFShift((ifshift_t)shift);
}

void FilterCtrl::ifShiftUpdate(ifshift_t shift)
{
  ifshift->setValue(shift);
}
