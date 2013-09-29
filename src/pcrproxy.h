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

#ifndef PCRPROXY_H_
#define PCRPROXY_H_

///////////////////////////////////////////////////////////
// PcrProxy acts as an agent to communicate with PCR1000 //
// It contains many functions to control the radio       //
///////////////////////////////////////////////////////////


#include <qobject.h>
#include <qtimer.h>
#include "pcrproto.h"
#include "pcrio.h"

class PcrProxy : public QObject
{
  Q_OBJECT
public:
  PcrProxy( QObject *parent=0, const char *name=0 );
  ~PcrProxy();

public slots:

  // Radio commands
  bool InitRadio(const char *device);
  bool TurnOffRadio();
  bool setFreq( pcrfreq_t );
  bool setIFShift( ifshift_t );
  bool stepUpTS();
  bool stepDownTS();
  bool stepUpFreq();
  bool stepDownFreq();
  bool stepFreq( int );
  bool setMode( modulate_t );
  bool stepUpFilter();
  bool stepDownFilter();
  bool setVol( vol_t );
  bool setSquelch( squelch_t );
  bool setATT( bool );
  bool setAGC( bool );
  bool setNB(  bool );
  bool setVSC( bool );
  bool setAFC( bool );
  bool scopeOn( bool );
  bool scopeWide();
  bool scopeNarrow();
  bool getBookmark( struct bookmark_t *);
  bool gotoBookmark( struct bookmark_t *);
  bool setAlias( const char * );
  bool setInfo( const char * );
  bool setCTCSS( unsigned char );
  bool setFilter ( filter_t );
  bool setTS( pcrfreq_t );

  // PcrIO messages handler
  void radioMessageSlot( const char *, int );
  void ResetSerialPort();
  
signals:
  // Radio states update notification
  void onUpdate( bool on );
  void bandScopeUpdate( const unsigned char *data, unsigned char nsamples );
  void sigStrengthUpdate( unsigned char sig);
  void sigCenterUpdate( unsigned char center);
  void freqUpdate( pcrfreq_t freq );
  void filterUpdate( filter_t filter);
  void tsUpdate( pcrfreq_t ts );
  void modeUpdate( modulate_t mode);
  void squelchOnUpdate( bool );
  void ifShiftUpdate( ifshift_t );
  void volUpdate( vol_t );
  void squelUpdate( squelch_t );
  void attUpdate( bool );
  void agcUpdate( bool );
  void nbUpdate( bool );
  void vscUpdate( bool );
  void afcUpdate( bool );
  void scopeOnUpdate( bool );
  void infoUpdate( const char * );
  void aliasUpdate( const char * );
  void ctcssUpdate( unsigned char );

  // PcrIO messages senders
  void sendMessage( const char *, int );
  
protected:
  bool          freqParse( pcrfreq_t, modulate_t, filter_t, char *, int * );	
  unsigned char charHexToDec( char );

private:
  
  // Radio Information
  struct pcrinfo_t{
    pcrfreq_t      freq;
    pcrfreq_t      ts;
    modulate_t     mode;
    filter_t       filter;
    vol_t          vol;
    squelch_t      squelch;
    ifshift_t      ifShift;
    bool           ATT;
    bool           AGC;
    bool           NB;
    bool           VSC;
    bool           AFC;
    unsigned char  CTCSS;
    bool           isBandScopeOn;
    char           alias[ALIASSIZE];
    char           info[INFOSIZE];

    char           device[64];
    unsigned char  sigStrength;
    unsigned char  sigCenter;
    unsigned char  bandScope[256];
    unsigned char  bandScopeNsamples;
    bool           isPowerOn;
    unsigned char  previousCTCSS;
  };
  
  PcrIO  pcrIO;
  struct pcrinfo_t pcrinfo;
  char   buffer[PCRIO_BUFFERSIZE];
};

#endif // PCRPROXY_H_


