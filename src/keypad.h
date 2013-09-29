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

#ifndef KEYPAD_H_
#define KEYPAD_H_

//
// KeyPad displays and control keypad for enter freqency
//

#include <qwidget.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include "pcrproto.h"

class KeyPad : public QWidget 
{
	Q_OBJECT

public:
	KeyPad(QWidget *parent=0, const char *name=0);
	~KeyPad();

signals:
	void setFreq( pcrfreq_t );
	void displayTemp( const QString *);
	void displayReset();	

private slots:
	void clicked0();
	void clicked1();
	void clicked2();
	void clicked3();
	void clicked4();
	void clicked5();
	void clicked6();
	void clicked7();
	void clicked8();
	void clicked9();
	void clickedDot();
	void clickedEnter();

	void timeOut();

protected:
	void resizeEvent( QResizeEvent * );
	void keyPressEvent( QKeyEvent * );

private:
	void clickedNumber(const char *str);

	QPushButton *button0;
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button4;
    QPushButton *button5;
    QPushButton *button6;
    QPushButton *button7;
    QPushButton *button8;
    QPushButton *button9;
    QPushButton *buttonDot;
    QPushButton *buttonEnter;

	QTimer *tictoc;
	QString tempFreq;

};

#endif // KEYPAD_H_
