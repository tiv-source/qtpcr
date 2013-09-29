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

#ifndef MODEDISPLAY_H_
#define MODEDISPLAY_H_

#include <qwidget.h>

#include "pcrproto.h"

class ModeDisplay : public QWidget
{
  Q_OBJECT 
    
public:
  ModeDisplay( QWidget *parent=0, const char *name=0);
  
public slots:
  void modeUpdate( modulate_t );
  void agcUpdate( bool );
  void vscUpdate( bool );
  void nbUpdate( bool );
  void attUpdate( bool );
  void afcUpdate( bool );

protected:
  void paintEvent( QPaintEvent *);
  
private:
  
  bool agc;
  bool vsc;
  bool nb;
  bool att;
  bool afc;
  modulate_t mode;
  
};

#endif // MODEDISPLAY_H_

