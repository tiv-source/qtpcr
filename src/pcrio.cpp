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

#include "pcrio.h"

//#define DEBUG_VER_

#ifdef DEBUG_VER_
#include <stdio.h>
#endif // DEBUG_VER_

extern int errno;

////////////////////////////////////////////////
////  Set variable to default values   /////////
////////////////////////////////////////////////
PcrIO::PcrIO(QObject *parent, const char *name)
  : QObject(parent, name)
{
  // prevent child process from messing up with any of files
  for(int i = 3; i < 10 ; i++ ){

    int ctrlbit = fcntl( i, F_GETFD, 0 );
    fcntl( i, F_SETFD, FD_CLOEXEC | ctrlbit);

  }

  fd = -1;
  bytesToWrite  = 0;
  bufferReadLen = 0;
  timerId = startTimer( 10 );
  bzero(bufferRead, PCRIO_BUFFERSIZE);
  bzero(bufferWrite, PCRIO_BUFFERSIZE);
  bzero(&oldtio, sizeof(struct termios));

#ifdef DEBUG_VER_
  t = time(NULL);
#endif // DEBUG_VER_
}


PcrIO::~PcrIO()
{
  if(fd > 0)
    Close();
}


/////////////////////////////////////////////////
///////////  Set highPriority ///////////////////
/////////////////////////////////////////////////
bool PcrIO::highPriority () const{ return true; }


/////////////////////////////////////////////////////////
// Open serial port to PCR1000 and initialize baudrate //
// If sucessful, it sends connected signals            //
// Then, start timer to check every interval if there  //
// is a data coming in from PCR1000                    //
/////////////////////////////////////////////////////////
bool PcrIO::Open(const char *device, tcflag_t baudrate)
{
  // does not check if device is already opened
  // potential bug here ...

  // open serial device
  fd = open(device, O_RDWR |  O_NOCTTY | O_NONBLOCK );
  if(fd < 0){
    perror(device);
    return false;
  }

  // get current terminal state
  tcgetattr(fd, &oldtio);
  bzero( &tio, sizeof(struct termios));

  // set baudrate (adjust so that's BSD compatible)
  cfsetispeed( &tio, baudrate);
  cfsetospeed( &tio, baudrate);

  tio.c_cflag |= CRTSCTS;      // flow control
  tio.c_cflag |= CS8;          // 8 bits character
  tio.c_cflag |= CLOCAL;       // ignore modem control 
  tio.c_cflag |= CREAD;        // enable reciever
  tio.c_iflag |= IGNPAR;       // no parity check 

  tio.c_oflag = 0;

  tio.c_lflag &= ~ECHO;        // no echo
  tio.c_lflag &= ~ICANON;      // no canonical mode 

  tio.c_cc[VTIME]  = 0;        // no timer check
  tio.c_cc[VMIN]   = 4;        // block read 4 bytes minimum

  // set terminal attibute
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&tio);

  // set DTR and RTS on
  int ctrlbit = TIOCM_DTR;
  if(ioctl(fd,TIOCMBIS, &ctrlbit) < 0)
    perror("Cannot raise DTR bit");

  ctrlbit = TIOCM_RTS;
  if(ioctl(fd, TIOCMBIS, &ctrlbit) < 0)
    perror("Cannot rasise RTS bit");

  // prevent children from messsing up with our serial port 
  ctrlbit = fcntl( fd, F_GETFD, 0 );
  if(fcntl( fd, F_SETFD, FD_CLOEXEC | ctrlbit) < 0)
    perror("Cannot set close-exec flag on serial line");

  emit connected();


#ifdef DEBUG_VER_
  fprintf(stderr, "PcrIO::Open - %s successful\n", device);
#endif // DEBUG_VER_
  
  return true;
}



//////////////////////////////////////////
// Close serial port and kill timer     //
// if sucessful, it emits closed signal //
//////////////////////////////////////////
bool PcrIO::Close()
{
  // return terminal state
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&oldtio);

  if(close(fd) < 0){
    perror("could not close fd");
    return false;
  }

  fd = -1;

  emit closed();

  return true;
}



/////////////////////////////////////////////////////////
// sendMessageSlot waits for data from external source //
// and forward the message to PCR1000                  //
/////////////////////////////////////////////////////////
void PcrIO::sendMessageSlot(const char *data, int len)
{
  // check file descriptor
  if(fd < 0) return;

  memcpy(bufferWrite+bytesToWrite, data, len);
  bytesToWrite += len;

#ifdef DEBUG_VER_
  fprintf(stderr, "PcrIO::sendMessageSlot - bytesToWrite = %d\n%s\n",
				bytesToWrite, data);
#endif // DEBUG_VER_

  ReadWrite();
}



////////////////////////////////////////////////////////////////////////
// IsTokenComplete checks if data received from PCR1000 has completed //
// at least one token                                                 //
// first --> points to first character of the token                   //
// tokenlen --> token len                                             //
////////////////////////////////////////////////////////////////////////
bool PcrIO::IsTokenComplete(const char *data, int len, int *first, int *tokenlen)
{
  int i;

  for(i = 0; i < len; i++)

    // normal inquiries
	if(data[i] == 'I' && (len-i)>=4){
	  *first = i;
	  *tokenlen = 4;
	  return true;

    // bandscope 
	}else if(data[i] == 'N' && (len-i)>=37){
	  
	  *first = i;
	  *tokenlen = 37;
	  return true;

    // on/off status
	}else if(data[i] == 'H' && (len-i)>=4){

      *first = i;
      *tokenlen = 4;
      return true;
    // command validate
    }else if(data[i] == 'G' && (len-i)>=4){

      *first = i;
      *tokenlen = 4;
      return true;
    }

  return false;
}




////////////////////////////////////////////////////////////////////////
// timerEvent checks to see if there is data waiting to be read/write //
////////////////////////////////////////////////////////////////////////
void PcrIO::timerEvent( QTimerEvent *e){

#ifdef DEBUG_VER_
  struct termios tio;
 
  // print term info 
  if( time(NULL) > t+60){

    int ctrlbit;
    if(ioctl(fd, TIOCMGET, &ctrlbit) < 0)
       perror("Cannot retrive control bits");

    fprintf(stderr, "TIOCMGET returns 0x%X\n", ctrlbit);
  }
#endif // DEBUG_VER_


  if( fd < 0) return;
  ReadWrite();
}



///////////////////////////////////
// ReadWrite does IO operations  //
///////////////////////////////////
bool PcrIO::ReadWrite()
{
  int n;
  int pos, len;
  struct timeval tv;

  tv.tv_usec = 0;
  tv.tv_sec  = 0;

  FD_ZERO(&rset);
  FD_ZERO(&wset);
  
  FD_SET(fd, &rset);
  
  if(bytesToWrite > 0)
	FD_SET(fd, &wset);
  
  // check if there is data waiting
  select(fd+1, &rset, &wset, NULL, &tv);
  
  if(FD_ISSET(fd, &rset)){

	if((n = read(fd, bufferRead+bufferReadLen, PCRIO_BUFFERSIZE-bufferReadLen))<0){
	  if(errno != EAGAIN){
		perror("read error on fd");
        emit disconnected();
      }
	}else if(n == 0){
	  
	  // get disconnected somehow
	  emit disconnected();
      fprintf(stderr, "PcrIO::ReadWrite - read turns zero byte\n");

	}else{
	 
	  // read successful
	  bufferReadLen += n;

#ifdef DEBUG_VER_
        fprintf(stderr, "PcrIO::ReadWrite - read from serial port %d bytes\n", n);
#endif // DEBUG_VER_

	  if(IsTokenComplete(bufferRead, bufferReadLen, &pos, &len)){
		
		// send message
		bzero(mesg, PCRIO_BUFFERSIZE);
		memcpy(mesg, bufferRead+pos, len);
		emit radioMessage(mesg, len);
#ifdef DEBUG_VER_
		fprintf(stderr, "PcrIO::ReadWrite - emit radioMessage(%s, %d)\n", mesg,len);
#endif // DEBUG_VER_
		
		bufferReadLen -= (pos+len);
		memmove(bufferRead, bufferRead+pos+len, bufferReadLen);
	  }
	  
	}
  }

    
  if(FD_ISSET(fd, &wset)){
	if((n = write(fd, bufferWrite, bytesToWrite)) < 0){
	  if(errno != EAGAIN){
		perror("write error to fd");
        emit disconnected();
      }
	}else{
	  bytesToWrite -= n;
	  if(n < bytesToWrite){
		memmove( bufferWrite, bufferWrite + n, bytesToWrite);
	  }else{
		emit messageSent();
	  }
	}
  }
  
  return true;
}
