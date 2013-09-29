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
#include "dial.h"

#ifdef DEBUG_VER_
#include <stdio.h>
#endif // DEBUG_VER_

Dial::Dial(QWidget *parent, const char *name, int min, int max, int ang)
  : QWidget(parent , name)
{
  // set angle
  this->ang = ang;
  this->min = min;
  this->max = max;
  
  maskBitmap = NULL;
  
  reportValue = false;
  setFocusPolicy(QWidget::NoFocus);
  
  state = NoButton;
  autoRepeatTimer.stop();
  connect( &autoRepeatTimer, SIGNAL(timeout()), 
	   this, SLOT( ControlHandler() ));
}



void Dial::setRange(int min, int max, int val)
{
  reportValue = true;
  range.setValue(val);
  range.setRange(min, max);
  range.setSteps( (max-min)*DIAL_STEPANGLE/(this->max-this->min), 1); 
}

void Dial::updateValue(int val)
{
  range.setValue(val);
  ang = (int)((double)(val-range.minValue())*(max-min)/(range.maxValue()-range.minValue())) 
    + min;
  repaint();
}

void Dial::mouseReleaseEvent( QMouseEvent *e)
{
  state = NoButton;
  autoRepeatTimer.stop();
}



void Dial::mousePressEvent( QMouseEvent *e)
{
  if(e->button() == LeftButton){
    state = LeftButton;
    ControlHandler();
  }else	if(e->button() == RightButton){
    state = RightButton;
    ControlHandler();
  }else
    return;
  
  autoRepeatTimer.start( DIAL_TIMESTEP*5, true );
}


void Dial::ControlHandler()
{
  if(state == LeftButton){
    ang -= DIAL_STEPANGLE;
    if(min < max  &&  ang < min){
      ang += DIAL_STEPANGLE;
      return;
    }
    emit turnLeft();
    if(reportValue){
      range.subtractLine();
      emit setValue( range.value() );
    }
  }else if(state == RightButton){
    ang += DIAL_STEPANGLE;
    if(min < max  && max < ang){
      ang -= DIAL_STEPANGLE;
      return;
    }
    emit turnRight();
    if(reportValue){
      range.addLine();
      emit setValue( range.value() );
    }
  }else
    return;
  
#ifdef DEBUG_VER_
  fprintf( stderr, "Dial::ControlHandler - angle = %d\n", ang);
#endif // DEBUG_VER_
  
  repaint();
  if(!autoRepeatTimer.isActive())
    autoRepeatTimer.start( DIAL_TIMESTEP );
  
  if(ang >  720) ang -= 720;
  if(ang < -720) ang += 720;
}


void Dial::resizeEvent( QResizeEvent *e)
{
  
  if(maskBitmap != NULL)
    delete maskBitmap;
  
  maskBitmap = new QBitmap(e->size().width(), e->size().height());
  
  QPainter p( maskBitmap );
  
  p.setBrush( Qt::black );
  p.setPen( NoPen );
  p.drawRect( 0, 0, e->size().width(), e->size().height() );
  
  p.setBrush( Qt::white );
  p.drawEllipse( 0, 0, e->size().width(), e->size().height() );	
  
  this->setMask( *maskBitmap );
}



void Dial::paintEvent( QPaintEvent *)
{
  
  QPainter p( this );
  
  p.translate(width()/2,height()/2);
  p.rotate( ang );
  
  p.setBrush( Qt::black );
  p.setPen( QPen(Qt::white,1) );
  p.drawEllipse( width()/4-2, -width()/8, width()/4, width()/4);
}
