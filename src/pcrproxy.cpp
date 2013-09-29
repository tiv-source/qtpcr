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

#include <time.h>
#include <stdlib.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include "pcrproxy.h"
#include "pcrproto.h"
#include "tips.h"

pcrfreq_t CTCSS_TONE[52] = 
{0,670,693,710,719,744,770,797,825,854,885,915,948,974,1000,1035,
 1072,1109,1148,1188,1230,1273,1318,1365,1413,1462,1514,1567,1598,
 1622,1655,1679,1713,1738,1773,1799,1835,1862,1899,1928,1966,1995,
 2035,2065,2107,2181,2257,2291,2336,2418,2503,2541};


#ifdef DEBUG_VER_
#include <stdio.h>
#endif // DEBUG_VER_


PcrProxy::PcrProxy(QObject *parent, const char *name)
  : QObject( parent, name)
{

  // connect to PcrIO to send/receive messages
  connect( this, SIGNAL( sendMessage(const char *, int)),
	   &pcrIO, SLOT( sendMessageSlot(const char *, int)));
  
  connect( &pcrIO, SIGNAL( radioMessage(const char *, int)),
	   this,	SLOT( radioMessageSlot(const char *, int)) );
  
  connect( &pcrIO, SIGNAL( disconnected() ),
	   this,	SLOT( ResetSerialPort() ) );

  // initialize radio conditions
  pcrinfo.freq    = DEFAULT_FREQ;
  pcrinfo.ts      = DEFAULT_TS;
  pcrinfo.mode    = DEFAULT_MODE;
  pcrinfo.vol     = 0;
  pcrinfo.squelch = 0;
  pcrinfo.filter  = DEFAULT_FILTER;
  pcrinfo.ifShift = DEFAULT_IFSHIFT;

  bzero( pcrinfo.alias, ALIASSIZE);
  bzero( pcrinfo.info, INFOSIZE);
  bzero( pcrinfo.bandScope, 256 );
  pcrinfo.bandScopeNsamples = 0x20; 
  pcrinfo.isBandScopeOn     = true;
  pcrinfo.isPowerOn         = false;

  pcrinfo.ATT   = false;
  pcrinfo.AGC   = false;
  pcrinfo.NB    = false;
  pcrinfo.VSC   = false;
  pcrinfo.AFC   = false;
  pcrinfo.CTCSS = 0;
  pcrinfo.previousCTCSS = 0;
  srandom(time(NULL));
  strcpy( pcrinfo.info, QtPcrTips[rand()%TIPS_SIZE] );
  strcpy( pcrinfo.device, "/dev/pcr1000" );
}




PcrProxy::~PcrProxy()
{
}


bool PcrProxy::InitRadio(const char *device)
{
  time_t t;

  // open serial device
  if(!pcrIO.Open(device, B9600))
  if(!pcrIO.Open(device, B38400))
    return false; 

  // wait one second for radio sync in
  for(t = time(NULL); t+1 > time(NULL) ; );

  // turn radio on
  emit sendMessage("H101\x0d\x0aG300\x0d\x0a",12);

  // wait 3 second for radio to initialize itself
  for(t = time(NULL); t+3 > time(NULL) ; );

  // set baudrate
  emit sendMessage("G105\x0d\x0a",6);

  // wait one seconds (FreeBSD-4.3 needs this)
  for(t = time(NULL); t+1 > time(NULL) ; );

  pcrIO.Close();

  // reopen serial port with this baud rate 
  if(!pcrIO.Open(device, B38400)) return false;

  // wait one seconds
  for(t = time(NULL); t+1 > time(NULL) ; );

  // set interactive mode
  emit sendMessage("H101\x0d\x0aG300\x0d\x0a",12);

  // wait one second
  for(t = time(NULL); t+1 > time(NULL) ; ); 
 
  // ask power stat
  emit sendMessage("H1?\x0d\x0a",5);

  // wait one seconds
  for(t = time(NULL); t+1 > time(NULL) ; );
 
  // autoupdate mode  
  emit sendMessage("H101\x0d\x0aG301\x0d\x0a",12);

  // init radio state
  setFreq(pcrinfo.freq);
  setVol(pcrinfo.vol);
  setSquelch(pcrinfo.squelch);
  setIFShift(pcrinfo.ifShift);
  setATT(pcrinfo.ATT);
  setAGC(pcrinfo.AGC);
  setNB(pcrinfo.NB);
  setVSC(pcrinfo.VSC);
  setAFC(pcrinfo.AFC);
  setCTCSS(pcrinfo.CTCSS);
  setAlias(pcrinfo.alias);
  setInfo(pcrinfo.info);
  setTS( pcrinfo.ts );

  strncpy(pcrinfo.device, device, 64);
  
  return true;
}




void PcrProxy::ResetSerialPort(){
  // kill all pending signal
  blockSignals( true );

  disconnect( &pcrIO, SIGNAL( disconnected() ),
             this,    SLOT( ResetSerialPort() ) );

  if(pcrIO.Close())
    pcrIO.Open(pcrinfo.device, B38400);

  connect( &pcrIO, SIGNAL( disconnected() ),
             this,    SLOT( ResetSerialPort() ) );

  blockSignals( false );

  emit sendMessage("G301\x0d\x0a",6);
}



//
// TurnOffRadio sends command to turn radio off
// and close serial port
//
bool PcrProxy::TurnOffRadio()
{
  // send radio off command
  emit sendMessage("H100\x0d\x0a",6);

  // wait one second
  for(time_t t = time(NULL); t+2 > time(NULL) ; );

  // ask if radio is off
  emit sendMessage("H1?\x0d\x0a",5);
  
  // wait one second
  for(time_t t = time(NULL); t+2 > time(NULL) ; );

  pcrIO.Close();

  return true;
}


//
// freqParse parses freqency, mode, and filter into valid radio
// command string
// Sometimes the filter value does not valid for a given mode,
// it automatically reset filter to a valid value and send 
// filterUpdate signal
//
bool PcrProxy::freqParse(pcrfreq_t freq, modulate_t mode, filter_t filter, char *str, int *len)
{
  // check filter
  switch( mode ){
  case MODE_NFM: filter = clipped(FILTER_6kHz, filter, FILTER_50kHz); break;
  case MODE_WFM: filter = clipped(FILTER_50kHz, filter, FILTER_230kHz); break;
  case MODE_AM : filter = clipped(FILTER_3kHz, filter, FILTER_50kHz); break;
  case MODE_USB: case MODE_LSB: case MODE_CW:
    filter = clipped(FILTER_3kHz, filter, FILTER_6kHz);
    break;			
  }
  
  pcrinfo.filter = filter;
  
  sprintf(str, "K0%.10lu%.2X%.2X00\x0d\x0a", freq, mode, filter);
  *len = strlen(str);
  
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::freqParse : %s\n", str);
#endif // DEBUG_VER_
  
  return true;
}



bool PcrProxy::setFreq(pcrfreq_t freq)
{
  int len;

  
  // check valid freqency range
  if((FREQ_MIN < freq) && (freq < FREQ_MAX)){

#ifdef SMART_RESETINFO
    // absolute vale
    if((freq>pcrinfo.freq?freq-pcrinfo.freq:pcrinfo.freq-freq) 
        > (pcrfreq_t)4*FILTER_TO_HERTZ((pcrinfo.filter))){
      bzero(pcrinfo.alias, ALIASSIZE);
      bzero(pcrinfo.info, INFOSIZE);
      emit aliasUpdate( pcrinfo.alias );
      emit infoUpdate( pcrinfo.info );
    }
#endif    

    pcrinfo.freq = freq;
    freqParse(pcrinfo.freq, pcrinfo.mode, pcrinfo.filter, buffer, &len);
    emit sendMessage(buffer, len); 	
    emit freqUpdate( freq );

    return true;

  }else
    return false;
	
}




bool PcrProxy::setMode(modulate_t mode)
{
  int len;
	
  pcrinfo.mode = mode;
  freqParse(pcrinfo.freq, pcrinfo.mode, pcrinfo.filter, buffer, &len);
  emit sendMessage(buffer, len);
  emit filterUpdate( pcrinfo.filter );
  emit modeUpdate( pcrinfo.mode );
  return true;
}




bool PcrProxy::stepUpFilter()
{
  switch( pcrinfo.filter ){
  case FILTER_3kHz : return setFilter( FILTER_6kHz   ); break;
  case FILTER_6kHz : return setFilter( FILTER_15kHz  ); break;
  case FILTER_15kHz: return setFilter( FILTER_50kHz  ); break;
  case FILTER_50kHz: return setFilter( FILTER_230kHz ); break;
  }
  return true;
}



bool PcrProxy::stepDownFilter()
{
  switch( pcrinfo.filter ){
  case FILTER_230kHz: return setFilter( FILTER_50kHz  ); break;
  case FILTER_50kHz : return setFilter( FILTER_15kHz  ); break;
  case FILTER_15kHz : return setFilter( FILTER_6kHz   ); break;
  case FILTER_6kHz  : return setFilter( FILTER_3kHz   ); break;
  }
  return true;
}




bool PcrProxy::setFilter(filter_t filter)
{
  int len;

#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setFilter %d\n", (int)filter);
#endif // DEBUG_VER_

  pcrinfo.filter = filter;
  freqParse(pcrinfo.freq, pcrinfo.mode, pcrinfo.filter, buffer, &len);
  emit sendMessage(buffer, len);
  emit filterUpdate( pcrinfo.filter );
  return true;
}


bool PcrProxy::setIFShift(ifshift_t shift)
{
  pcrinfo.ifShift = shift;
  sprintf(buffer, "J43%.2X\x0d\x0a", shift);
  emit sendMessage(buffer, 7);
  emit ifShiftUpdate( shift );
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setIFShift : %s\n", buffer);
#endif // DEBUG_VER_
  return true;
}


bool PcrProxy::setVol(vol_t vol)
{
  sprintf(buffer, "J40%.2X\x0d\x0a", vol);

  pcrinfo.vol = vol;

  emit sendMessage(buffer, 7);
  emit volUpdate( vol );
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setVol : %s\n", buffer);
#endif // DEBUG_VER_
  return true;
}



bool PcrProxy::setATT( bool val )
{
  if( val )
    sprintf(buffer, "J4701\x0d\x0a");
  else
    sprintf(buffer, "J4700\x0d\x0a");
  emit sendMessage(buffer, 7);
  emit attUpdate( val );

  pcrinfo.ATT = val;
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setATT : %s\n", buffer);
#endif // DEBUG_VER_

  return true;
}


bool PcrProxy::setVSC( bool val )
{
  if( val )
    sprintf(buffer, "J5001\x0d\x0a");
  else
    sprintf(buffer, "J5000\x0d\x0a");
  emit sendMessage(buffer, 7);
  emit vscUpdate( val );

  pcrinfo.VSC = val;
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setVSC : %s\n", buffer);
#endif // DEBUG_VER_
  return true;
}


bool PcrProxy::setAGC( bool val )
{
  if( val )
    sprintf(buffer, "J4501\x0d\x0a");
  else
    sprintf(buffer, "J4500\x0d\x0a");
  emit sendMessage(buffer, 7);
  emit agcUpdate( val );

  pcrinfo.AGC = val;
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setAGC : %s\n", buffer);
#endif // DEBUG_VER_

  return true;
}


bool PcrProxy::setAFC( bool val )
{
  pcrinfo.AFC = val;
  emit afcUpdate( val );

  return true;
}


bool PcrProxy::setNB( bool val )
{
  if( val )
    sprintf(buffer, "J4601\x0d\x0a");
  else
    sprintf(buffer, "J4600\x0d\x0a");
  emit sendMessage(buffer, 7);
  emit nbUpdate( val );

  pcrinfo.NB = val;
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setNB : %s\n", buffer);
#endif // DEBUG_VER_

  return true;
}



bool PcrProxy::setSquelch( squelch_t squ )
{
  sprintf(buffer, "J41%.2X\x0d\x0a", squ);
  emit sendMessage(buffer, 7);
  emit squelUpdate( squ );

  pcrinfo.squelch = squ;
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setSquelch : %s\n", buffer);
#endif // DEBUG_VER_

  return true;
}


// sending tone = 0xFF reset to previous CTCSS
bool PcrProxy::setCTCSS( unsigned char tone )
{
  // check range
  if( tone > 0x33 ){

    if(pcrinfo.previousCTCSS)
      tone = pcrinfo.previousCTCSS;
    else
      tone = 0x01;
  }
  pcrinfo.previousCTCSS = pcrinfo.CTCSS;
  pcrinfo.CTCSS = tone;

  sprintf(buffer, "J51%.2X\x0d\x0a", tone);
  emit sendMessage(buffer, 7);
  emit ctcssUpdate( tone );

  pcrinfo.CTCSS = tone;
#ifdef DEBUG_VER_
  fprintf(stderr, "PcrProxy::setCTCSS : %s\n", buffer);
#endif // DEBUG_VER_

  return true;
}



bool PcrProxy::setTS( pcrfreq_t ts )
{
  pcrinfo.ts = ts;

  if(pcrinfo.isBandScopeOn){
    scopeOn( true );
  }

  emit tsUpdate( ts );

  return true;
}


bool PcrProxy::stepUpTS()
{
  // KISS (Keep It Simple and Stupid) algorithm
  switch( pcrinfo.ts ){
    // Dave, VK5DG, added low freqency steps 
  case TS_10Hz   : setTS( TS_25Hz ); break;
  case TS_25Hz   : setTS( TS_50Hz ); break;
  case TS_50Hz   : setTS( TS_100Hz ); break;
  case TS_100Hz  : setTS( TS_250Hz ); break;
  case TS_250Hz  : setTS( TS_500Hz ); break;
  case TS_500Hz  : setTS( TS_1000Hz ); break;
  case TS_1000Hz : setTS( TS_2500Hz ); break;
  case TS_2500Hz : setTS( TS_3000Hz ); break;
  case TS_3000Hz : setTS( TS_5000Hz ); break;
  case TS_5000Hz : setTS( TS_9000Hz ); break;
  case TS_9000Hz : setTS( TS_10000Hz ); break;
  case TS_10000Hz: setTS( TS_12500Hz ); break;
  case TS_12500Hz: setTS( TS_25000Hz ); break;
  }

  return true;
}


bool PcrProxy::stepDownTS()
{
  // KISS (Keep It Simple and Stupid) algorithm
    switch( pcrinfo.ts ){
      // Dave, VK5DG, added low freqency steps
    case TS_25000Hz: setTS( TS_12500Hz ); break;
    case TS_12500Hz: setTS( TS_10000Hz ); break;
    case TS_10000Hz: setTS( TS_9000Hz ); break;
    case TS_9000Hz : setTS( TS_5000Hz ); break;
    case TS_5000Hz : setTS( TS_3000Hz ); break;
    case TS_3000Hz : setTS( TS_2500Hz ); break;
    case TS_2500Hz : setTS( TS_1000Hz ); break;
    case TS_1000Hz : setTS( TS_500Hz ); break;
    case TS_500Hz  : setTS( TS_250Hz ); break;
    case TS_250Hz  : setTS( TS_100Hz ); break;
    case TS_100Hz  : setTS( TS_50Hz ); break;
    case TS_50Hz   : setTS( TS_25Hz ); break;
    case TS_25Hz   : setTS( TS_10Hz ); break;
    }

  return true;
}

bool PcrProxy::stepUpFreq()
{
  pcrfreq_t newFreq;

  newFreq = pcrinfo.freq + pcrinfo.ts;
  if(setFreq( newFreq )){
    pcrinfo.freq = newFreq;
    return true;
  }else
    return false;
}




bool PcrProxy::stepDownFreq()
{
  pcrfreq_t newFreq;

  newFreq = pcrinfo.freq - pcrinfo.ts;
  if(setFreq( newFreq )){
    pcrinfo.freq = newFreq;
    return true;
  }else
    return false;
}



bool PcrProxy::stepFreq(int step)
{
  pcrfreq_t newFreq;

  newFreq = pcrinfo.freq + step*pcrinfo.ts;
  if(setFreq( newFreq )){
    pcrinfo.freq = newFreq;
    return true;
  }else
    return false;
}	



//
// charHexToDex convert a single HEX character '0'..'F'
// into a decival value
//
unsigned char PcrProxy::charHexToDec(char digit)
{
  if(('A' <= digit) && (digit <= 'F'))
    return digit-'A'+10;
  else if(('1' <= digit) && (digit <= '9'))
    return digit-'1'+1;
  else
    return 0;
}



//
// radioMessageSlot waits for messages from PcrIO
//
void PcrProxy::radioMessageSlot(const char *mesg, int len)
{
  int index;
  int i;
  char digit;
  char buffer[PCRIO_BUFFERSIZE];
  unsigned char val;

  bzero( buffer, PCRIO_BUFFERSIZE );
  memcpy( buffer, mesg, len );

  // on off status
  if(buffer[0] == 'H'){
    switch(buffer[3]){
    case '1':
       pcrinfo.isPowerOn = true;
       emit onUpdate( true );
    break;

    case '0': 
       pcrinfo.isPowerOn = false;
       emit onUpdate( false);
    break;
    } 
  }

  // bandscope
  if(buffer[0] == 'N'){

    digit = buffer[3];
    index = (int)charHexToDec(digit)*16;

    for(i = 0; i < 16; i++){

      digit = buffer[i*2 + 5];
      val = charHexToDec(digit)*16;
      digit = buffer[i*2 + 6];
      val += charHexToDec(digit);
      pcrinfo.bandScope[index+i] = val;
    }

    emit bandScopeUpdate(pcrinfo.bandScope, pcrinfo.bandScopeNsamples);
  }

  // normal inquires 
  if(buffer[0] == 'I'){
    switch(buffer[1]){

      // squelch ON/ OFF
    case '0':
      digit = buffer[3];
      if(digit == '4')
	emit squelchOnUpdate( false );
      else 
	emit squelchOnUpdate( true );
      break;

      // sig strength
    case '1':
      digit = buffer[2];
      val = charHexToDec(digit)*16;	
      digit = buffer[3];
      val += charHexToDec(digit);
      pcrinfo.sigStrength = val;

      emit sigStrengthUpdate( val );
      break;

      // AFC center 
    case '2':
      digit = buffer[2];
      val = charHexToDec(digit)*16;
      digit = buffer[3];
      val += charHexToDec(digit);
      
      pcrinfo.sigCenter = val;
      emit sigCenterUpdate( val );

      // control frequency
      if(pcrinfo.AFC){
	switch( val ){
	case AFC_LOW: stepDownFreq(); break;
	case AFC_HIGH: stepUpFreq(); break;
	}
      }
      break;
    }
  }
}


bool PcrProxy::scopeOn( bool flag)
{
  unsigned char rate;
  unsigned char nsamples;

  if(flag){

    nsamples = pcrinfo.bandScopeNsamples;
    rate = 0x05;
    
    // parse band scope command
    sprintf(buffer, "ME00001%.2X%.2X0100%.6lu\x0d\x0a", 
	    nsamples, 
	    rate,  
	    pcrinfo.ts );
    
    emit sendMessage( buffer, 23);
    emit scopeOnUpdate( true );

    pcrinfo.isBandScopeOn = true;
    
#ifdef DEBUG_VER_
    fprintf( stderr, "PcrProxy::scopeOn - emit %s \n", buffer );
#endif	// DEBUG_VER_
  }else{

    sprintf( buffer, "ME0000114050000010000\x0d\x0a" );
    emit sendMessage( buffer , 23);
    emit scopeOnUpdate( false );

    pcrinfo.isBandScopeOn = false;
#ifdef DEBUG_VER_
    fprintf( stderr, "PcrProxy::scopeOn - emit %s \n", buffer );
#endif  // DEBUG_VER_
    
  }

  return true;
}

bool PcrProxy::scopeWide()
{
  if(pcrinfo.isBandScopeOn && (int)pcrinfo.bandScopeNsamples*2 < 255){
    pcrinfo.bandScopeNsamples  = pcrinfo.bandScopeNsamples*2;	
    scopeOn( true );
  }

  return true;
}


bool PcrProxy::scopeNarrow()
{
  if(pcrinfo.isBandScopeOn && (int)pcrinfo.bandScopeNsamples/2 > 8){
    pcrinfo.bandScopeNsamples  = pcrinfo.bandScopeNsamples/2;
    scopeOn( true );
  }

  return true;
}


bool PcrProxy::setAlias( const char * str )
{
  strncpy(pcrinfo.alias, str, ALIASSIZE);
  emit aliasUpdate( pcrinfo.alias );

  return true;
}

bool PcrProxy::setInfo( const char * str )
{
  strncpy(pcrinfo.info, str, INFOSIZE);
  emit infoUpdate( pcrinfo.info );

  return true;
}

bool PcrProxy::getBookmark(struct bookmark_t *bm)
{
  // copy mem directly to speed up, but need to
  // be carefull every time we change bookmark_t
  memcpy(bm, &pcrinfo, sizeof(struct bookmark_t));

  return true;
}


bool PcrProxy::gotoBookmark(struct bookmark_t *bm)
{
  int len;

  // set auxilary mode first
  if(bm->squelch != pcrinfo.squelch)  
    setSquelch(bm->squelch);

  if(bm->ATT != pcrinfo.ATT)
    setATT(bm->ATT);

  if(bm->AGC != pcrinfo.AGC)
    setAGC(bm->AGC);

  if(bm->NB != pcrinfo.NB)
    setNB(bm->NB);

  if(bm->VSC != pcrinfo.VSC)
    setVSC(bm->VSC);
 
  if(bm->AFC != pcrinfo.AFC)
    setAFC(bm->AFC);

  if(bm->CTCSS != pcrinfo.CTCSS)
    setCTCSS(bm->CTCSS);

  if(bm->ifshift != pcrinfo.ifShift)
    setIFShift(bm->ifshift);

  if(bm->ts != pcrinfo.ts)
    setTS(bm->ts);
  
  // this should be hard-coded
  pcrinfo.freq   = bm->freq;
  pcrinfo.mode   = bm->mode;
  pcrinfo.filter = bm->filter;
  freqParse(pcrinfo.freq, pcrinfo.mode, pcrinfo.filter, buffer, &len);

  emit sendMessage(buffer, len);
  emit freqUpdate( pcrinfo.freq );
  emit modeUpdate( pcrinfo.mode );
  emit filterUpdate( pcrinfo.filter );

  if(bm->vol != pcrinfo.vol)
    setVol(bm->vol);

  if(bm->scope != pcrinfo.isBandScopeOn)
    scopeOn( bm->scope );
  
  setAlias( bm->alias );
  setInfo( bm->info );

  return true;
}

