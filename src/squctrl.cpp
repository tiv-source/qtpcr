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

#include "squctrl.h"

SquCtrl::SquCtrl(QWidget *parent, const char *name) 
  : Dial(parent, name, 150, 390, 150)
{
  setRange(0, 255, 0);
  connect( this, SIGNAL(setValue(int)), this, SLOT(setValue_Squ(int)));
}


void SquCtrl::squelUpdate( squelch_t squ)
{
  updateValue( (int) squ );
}

// convert from int --> unsigned char type
void SquCtrl::setValue_Squ(int squ)
{
  emit setSquelch( (squelch_t) squ);
}
