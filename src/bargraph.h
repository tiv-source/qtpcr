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

#ifndef BARGRAPH_H_
#define BARGRAPH_H_

//
// BarGraph displays bandscope data
//

#include <qwidget.h>

class BarGraph : public QWidget
{
	Q_OBJECT

public:
	BarGraph( QWidget *parent=0, const char *name=0 );

public slots:
        void setData( const unsigned char *, unsigned char );
	void setZero();
	void setPause( bool );
	void scopeOnUpdate( bool );

signals:
	// user request freqency shift off centers
	void stepFreq( int );

protected:
	void paintEvent( QPaintEvent * );
	void mousePressEvent( QMouseEvent * ); 

private:
	bool active;
	unsigned char data[256];
	unsigned char nsamples;	
};

#endif // BARGRAPH_H_
