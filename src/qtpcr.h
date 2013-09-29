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

#ifndef QTPCR_H_
#define QTPCR_H_


//////////////////////////////
// QtPcr is the main window //
//////////////////////////////

#include <qwidget.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qtabdialog.h>

#include "pcrproxy.h"
#include "bargraph.h"
#include "freqdisplay.h"
#include "keypad.h"
#include "modectrl.h"
#include "filterdisplay.h"
#include "filterctrl.h"
#include "dial.h"
#include "bmtree.h"
#include "volctrl.h"
#include "squctrl.h"
#include "modedisplay.h"
#include "meter.h"
#include "aliasdisplay.h"
#include "scandialog.h"
#include "monitor.h"

class QtPcr : public QWidget
{
 Q_OBJECT

public:
	QtPcr( QWidget *parent=0, const char *name=0);
	~QtPcr();

protected slots:
 void powerOn( bool );
 void ChannelSlot( bool );
 void ScanSlot( bool ); 
 void editingModeSlot( bool );
 void EnableControl( bool );
 void EnableDisplay( bool );
 void RadioOn( bool );

protected:
 void enterEvent ( QEvent * );
 
private:
 // Radio controlers and displays 
 QPushButton     *power;
 QPushButton     *channelButton;
 QPushButton     *scanButton;
 PcrProxy        *pcr;
 BarGraph        *barGraph;
 FreqDisplay     *freqDisplay;
 KeyPad          *keyPad;
 ModeCtrl        *modeCtrl;
 ModeDisplay     *modeDisplay;
 FilterDisplay   *fltDisplay;
 FilterCtrl      *fltCtrl;
 Dial            *dialFreq;
 VolCtrl         *vol;
 SquCtrl         *squ;
 Meter	         *meter;	
 BookmarkTree    *bmtree;
 AliasDisplay    *aliasDisplay;
 InfoDisplay     *infoDisplay;
 QMultiLineEdit  *infoEdit;
 ScanDialog      *scanDialog;
 MonitorDialog   *monitorDialog;
 QTabDialog      *channelDialog;
};

#endif // QTPCR_H_
