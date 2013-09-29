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

#ifndef PCRIO_H_
#define PCRIO_H_

//////////////////////////////////////////////////////////////////
// PcrIO is responsible for sending and receiving messages with
// PCR1000. Once recieve a message, it sends signal via Qt.
//////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>

#include <qobject.h>
#include <qstring.h>

#define PCRIO_BUFFERSIZE 64

class PcrIO : public QObject
{
  Q_OBJECT

public:
  PcrIO( QObject *parent=0, const char *name=0 );
  ~PcrIO();
  
  bool highPriority () const;
  bool Open( const char *device, tcflag_t baudrate );
  bool Close();

public slots:
  void sendMessageSlot( const char *mesg, int len );

signals:
  void connected();
  void closed();
  void disconnected();
  void radioMessage( const char *mesg, int len );
  void messageSent();

protected:
  void timerEvent( QTimerEvent * );
  bool ReadWrite();
  bool IsTokenComplete( const char *data, int len, int *first, int *tokenlen );

private:

  int    fd;
  fd_set rset, wset;
  struct termios tio, oldtio;

  int  timerId;
  int  bytesToWrite;
  char bufferRead[PCRIO_BUFFERSIZE];
  char bufferWrite[PCRIO_BUFFERSIZE];
  char mesg[PCRIO_BUFFERSIZE];
  int  bufferReadLen;

#ifdef DEBUG_VER_
  time_t t;
#endif // DEBUG_VER_
};

#endif // PCRIO_H_
