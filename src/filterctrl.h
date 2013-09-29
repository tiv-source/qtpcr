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

#ifndef FILTERCTRL_H_
#define FILTERCTRL_H_

//
// FilterCtrl controls filter choices and IF shift
//


#include <qwidget.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "pcrproto.h"

class FilterCtrl : public QWidget
{
	Q_OBJECT
public:
	FilterCtrl(QWidget *parent=0, const char *name=0);
	~FilterCtrl();
	
signals:
	void stepUpFilter();
	void stepDownFilter();
	void stepUpTS();
	void stepDownTS();
	void setIFShift(ifshift_t);

public slots:
	void wideSlot();
	void narSlot();
	void resetIF();
	void ifShiftUpdate(ifshift_t);

protected:
	void resizeEvent( QResizeEvent *);

private slots:
	void ifShiftMoved(int);

private:
	QPushButton *wide;
	QPushButton *nar;
	QPushButton *reset;
	QPushButton *tswide;
	QPushButton *tsnar;

	QLabel *tslabel;

	QSlider *ifshift;
};

#endif // FILTERCTRL_H_
