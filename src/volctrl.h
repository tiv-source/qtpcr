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

#ifndef VOLCTRL_H_
#define VOLCTRL_H_

#include "dial.h"
#include "pcrproto.h"

class VolCtrl : public Dial
{
  Q_OBJECT

public:
    VolCtrl(QWidget *parent=0, const char *name=0);

public slots:
	void volUpdate(vol_t);

private slots:
        void setValue_Vol(int);

signals:
	void setVol(vol_t);

};

#endif // VOLCTRL_H_

