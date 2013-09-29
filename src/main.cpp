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
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <qapplication.h>
#include "qtpcr.h"

void SIGCHLD_handler(int sig)
{
  int rc;
  time_t t;

  t = time(NULL);
  t++;
  do{
      rc = waitpid(-1, NULL, WNOHANG);
  }while( t>time(NULL)   &&  rc != -1 ); 
}


int main( int argc, char **argv )
{
    QApplication a( argc, argv );
	QtPcr qtpcr;


    // prevent defunct
    signal(SIGCHLD, SIGCHLD_handler); 

	a.setMainWidget( &qtpcr );
	qtpcr.show();
	
    return a.exec();
}
