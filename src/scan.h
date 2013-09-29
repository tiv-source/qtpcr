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

#ifndef _SCAN_H_
#define _SCAN_H_

/////////////////////////////////////
///////  Send command to control  ///
///////  how frequency changes    ///
///////  in time                  ///
/////////////////////////////////////
#include <qobject.h>
#include <qtimer.h>
#include "pcrproto.h"

class Scan : public QObject
{

Q_OBJECT

public:
 Scan();
 void SetFrequency(pcrfreq_t beginFreq, pcrfreq_t endFreq, pcrfreq_t ts);
 void SetTimeDelay(int msec);

 enum Algorithm {FixStep, ChangeDriven};
 void SetAlgorithm( Algorithm algo = FixStep );

signals:
 // monitor these signals to get scan
 void scanBegun();
 void scanUpdate( pcrfreq_t, unsigned char );
 void scanEnded();

 // Must connect these signals to PcrProxy
 void setFreq( pcrfreq_t );
 
public slots:
 // Must connect these slots to PcrProxy
 void sigStrengthUpdate( unsigned char );
 void squelchOnUpdate( bool  );

 // auxilary. Might connect these to buttons
 void Start();
 void Pause(bool);
 void Stop();
 
private slots:
 void timeToStepFreq();

private:
 pcrfreq_t beginFreq, endFreq, ts, freq;
 bool doneSync, pause;
 unsigned char sig, prevSig, outSig;
 int msec;
 QTimer tictoc;
 QTimer sync;
 Algorithm algorithm;
};

#endif // _SCAN_H_
