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

#include <math.h>

#include "scan.h"

///////////////////////////////////////
/////  Class Scan                 /////
///////////////////////////////////////


///////////////////
/// constructor ///
///////////////////
Scan::Scan()
{
  // set variables to default values
  beginFreq = DEFAULT_FREQ;
  endFreq = DEFAULT_FREQ;
  ts = DEFAULT_TS;
  freq = DEFAULT_FREQ;
  algorithm = FixStep;
  sig = 0;
  msec = 500;
  tictoc.stop();
  sync.stop();
  doneSync = false;
  pause = false;
  connect( &tictoc, SIGNAL(timeout()), this, SLOT(timeToStepFreq()));
  connect( &sync, SIGNAL(timeout()), this, SLOT(Start()));
}



////////////////////////////////////////////////
////  Set scanning frequency                ////
////  must call this before start scaning   ////
////////////////////////////////////////////////
void Scan::SetFrequency( pcrfreq_t beginFreq, pcrfreq_t endFreq, pcrfreq_t ts)
{
  this->beginFreq = beginFreq;
  this->endFreq = endFreq;
  this->freq = beginFreq;
  this->ts = ts;
}



/////////////////////////////////////////
///  time deday for each frequecy step //
///  default should be 100 millisecond //
///  To fast causes flooding           //
/////////////////////////////////////////
void Scan::SetTimeDelay( int msec )
{
  this->msec = msec;
}



/////////////////////////////////////////
////  set frequency changing algorithm //
/////////////////////////////////////////
void Scan::SetAlgorithm( Algorithm algo )
{
  this->algorithm = algo;
}



//////////////////////////////////////
///  recieve signal strength update //
///  from radio                     //
//////////////////////////////////////
void Scan::sigStrengthUpdate( unsigned char sig)
{
  // fix step algorithm -
  //    wait until fixed passed, then step up frequency
  //
  // change driven algorithm -
  //    as soom as we recieve signal strength update
  //    from radio, we change frequency to the next step

  if(algorithm == FixStep){
     this->sig = sig;
     
  }else if(algorithm == ChangeDriven){

    if(tictoc.isActive() && this->sig != sig){
      this->sig = sig;
      timeToStepFreq();
      tictoc.start( msec );  
      
    }
  }
}



///////////////////////////////////////
///  Also check if squelch on/off    //
///////////////////////////////////////
void Scan::squelchOnUpdate( bool flag )
{
  if(!flag)
    this->sig = 0;
}



//////////////////////////////
///  start scanning        ///
//////////////////////////////
void Scan::Start()
{
  pause = false;

  if(doneSync){
    tictoc.start( msec );
    emit scanBegun();
    return;
  }

  emit setFreq( beginFreq );

  freq = beginFreq;
  sig = 0;
  prevSig = sig;
  outSig = 0;
  doneSync = true;

  // We need to wait about 500 millisecond before
  // start scanning because the radio needs to
  // settle down at the starting frequency.
  // Failing to do this causes the frequency to shift
  // to the left.
  sync.start( 500, true );
}


///////////////////////////////
///// pause scanning       ////
///////////////////////////////
void Scan::Pause(bool flag)
{
   pause = flag;
}



///////////////////////////////
/// stop scanning          ////
///////////////////////////////
void Scan::Stop()
{
  tictoc.stop();
  sync.stop();
  doneSync = false;
  pause = false;
}


void Scan::timeToStepFreq()
{
  if(pause) return;
  //
  // Sometime radio fails to update signal strength value
  // even though the real value has drops to zero.
  // To get around this, we check if this has occurs, then
  // exponentially decay the signal by 0.7
  if(prevSig == sig)
    outSig = (unsigned char)(outSig*0.7);
  else
    outSig = sig;

  emit scanUpdate( freq , outSig );

  prevSig = sig;
  freq += ts;

  // check if scan should end
  if(freq >= endFreq){
    tictoc.stop();
    doneSync = false;
    emit scanEnded();
    return;
  }

  emit setFreq( freq );
}
