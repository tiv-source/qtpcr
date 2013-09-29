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

#ifndef DIAL_H_
#define DIAL_H_

#include <qwidget.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qrangecontrol.h>

#define DIAL_STEPANGLE 10
#define DIAL_TIMESTEP  100

class Dial : public QWidget
{
  Q_OBJECT 
	
public:
	Dial(QWidget *parent=0, const char *name=0, int min=0, int max=0, int ang=0);
	void setRange(int min, int max, int val);

public slots:
        void updateValue(int); 
signals:
	void turnRight();
	void turnLeft(); 
	void setValue(int);	

private slots:
	void ControlHandler();

protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void paintEvent( QPaintEvent * );
	void resizeEvent( QResizeEvent *);

private:
	int ang;
	int max, min;
	QRangeControl range;
	bool reportValue;
	QBitmap *maskBitmap;
	QTimer autoRepeatTimer;
	ButtonState state;
};

#endif // DIAL_H_

