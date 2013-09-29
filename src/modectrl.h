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

#ifndef MODECTRL_H_
#define MODECTRL_H_


/////////////////////////////////////////
// MoceCtrl control modes of radio
// AM, FM, SSB, WFM. It also toggles
// special fucntion--AFC, ATT, VSC, AGC,
// and NB
/////////////////////////////////////////


#include <qpushbutton.h>
#include <qwidget.h>
#include "pcrproto.h"

#define SSB_LSB true
#define SSB_USB false

class ModeCtrl : public QWidget
{
	Q_OBJECT
public:
	ModeCtrl( QWidget *parent=0, const char *name=0);
	~ModeCtrl();

 // send signal upon user request
signals:
 void setMode( modulate_t );
 void setAFC( bool );
 void setATT( bool );
 void setNB( bool );
 void setAGC( bool );
 void setVSC( bool );
 void scopeOn( bool );
 void scopeWide();
 void scopeNarrow();
 void setCTCSS( unsigned char );

 // receive radio status update
public slots:
 void attUpdate( bool );
 void agcUpdate( bool );
 void nbUpdate( bool );
 void vscUpdate( bool );
 void afcUpdate( bool );
 void modeUpdate( modulate_t );
 void scopeOnUpdate( bool );
 void ctcssUpdate( unsigned char );

protected:
 void resizeEvent( QResizeEvent * );

private slots:
 void fmClicked();
 void wfmClicked();
 void ssbClicked();
 void amClicked();
 void cwClicked();
 void ctcssToggled( bool );
 void ctcssToneUp();
 void ctcssToneDn();

private:
 bool ssb_which;
 unsigned char ctcss;

 QPushButton *fm;
 QPushButton *wfm;
 QPushButton *ssb;
 QPushButton *am;
 QPushButton *cw;

 QPushButton *agc;
 QPushButton *nb;
 QPushButton *att;
 QPushButton *vsc;
 QPushButton *afc;
 
 QPushButton *scopeO;
 QPushButton *scopeW;
 QPushButton *scopeN;

 QPushButton *ctcssButton;
 QPushButton *ctcssStepUp;
 QPushButton *ctcssStepDn;
};

#endif // MODECTRL_H_


