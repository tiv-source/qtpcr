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

#ifndef PCRPROTO_H_
#define PCRPROTO_H_

// features
#define SMART_RESETINFO 1

// raw pcr1000 protocol
#define QTPCR_VERSION 0x01

#define RAW_CMD "\x0d\x0a"

#define AFC_LOW    0x00
#define AFC_CENTER 0x80
#define AFC_HIGH   0xff

// frequency 
#define FREQ_MIN 500
#define FREQ_MAX 1300000000
typedef ulong pcrfreq_t;

// filter
#define FILTER_3kHz   0x00
#define FILTER_6kHz   0x01
#define FILTER_15kHz  0x02
#define FILTER_50kHz  0x03
#define FILTER_230kHz 0x04
typedef unsigned char filter_t;

#define FILTER_TO_HERTZ(f) (f==0?3000:(f==1?6000:(f==2?15000:(f==3?50000:230000))))

// mode
#define MODE_LSB 0x00
#define MODE_USB 0x01
#define MODE_AM  0x02
#define MODE_CW  0x03
#define MODE_NFM 0x05
#define MODE_WFM 0x06
typedef unsigned char modulate_t;
typedef unsigned char ifshift_t;
typedef unsigned char vol_t;
typedef unsigned char squelch_t;

// freqency step size
#define TS_10Hz       10
#define TS_25Hz       25
#define TS_50Hz       50
#define TS_100Hz     100
#define TS_250Hz     250
#define TS_500Hz     500
#define TS_1000Hz   1000
#define TS_2500Hz   2500
#define TS_3000Hz   3000
#define TS_5000Hz   5000
#define TS_9000Hz   9000
#define TS_10000Hz 10000
#define TS_12500Hz 12500
#define TS_25000Hz 25000

// CTCSS tone chart
#define CTCSS_SIZE 52
extern pcrfreq_t CTCSS_TONE[52]; 

// default setup
#define DEFAULT_FREQ    145000000
#define DEFAULT_MODE    MODE_NFM
#define DEFAULT_TS      TS_10000Hz
#define DEFAULT_FILTER  FILTER_15kHz
#define DEFAULT_IFSHIFT 127


// set min <= val <= max
#define clipped(min, val, max) ((min)>((val)>(max)?(max):(val))?(min):((val)>(max)?(max):(val)))


// pcr bookmark
#define ALIASSIZE 32
#define INFOSIZE 256

struct bookmark_t{
  pcrfreq_t     freq;
  pcrfreq_t     ts;
  modulate_t    mode;
  filter_t      filter;
  vol_t         vol;
  squelch_t     squelch;
  ifshift_t     ifshift;
  bool          ATT;
  bool          AGC;
  bool          NB;
  bool          VSC;
  bool          AFC;
  unsigned char CTCSS;
  bool          scope;
  char          alias[ALIASSIZE];
  char          info[INFOSIZE];
};

struct bmfilehdr_t{
  char          signature[8];
  unsigned char version;
};

#define BMFILE_SIGNATURE "HI BABY!"
#define BMFILE_VERSION QTPCR_VERSION
#endif // PCRPROTO_H_
