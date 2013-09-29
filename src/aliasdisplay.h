/*
 *
 * QtPcr is a PCR-1000 controls.
 * 
 * Copyright (C) 2001
 *
 *     Teepanis Chachiyo  <teepanis@physics.purdue.edu>
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

#include <qscrollview.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfocusdata.h>
#include "pcrproto.h"

class InfoLabel : public QLabel
{
  Q_OBJECT
public:
  InfoLabel(QWidget * parent=0, const char * name=0);

signals:
  void mouseReleased(); 

protected:
  void mouseReleaseEvent ( QMouseEvent * ); 
};


class InfoDisplay  : public QScrollView
{
  Q_OBJECT
public:
 InfoDisplay(QWidget * parent=0, const char * name=0);

 QString text();

signals:
 void editingMode( bool );

public slots:
 void infoUpdate(const char *);
 void setRunning( bool );
 void setSpeed( int );

private slots:
 void infoMouseReleasedSlot(); 
 void timeToScroll();


private:
 bool editing; 
 int speed; 
 InfoLabel *infoLine;
 QString info;
 QTimer *tictoc;
};

class AliasDisplay : public QLineEdit
{
  Q_OBJECT
public:
  AliasDisplay(QWidget * parent=0, const char * name=0);

signals:
  void setAlias(const char *);

public slots:
  void aliasUpdate(const char *);

protected:
  void mouseReleaseEvent ( QMouseEvent * ); 
 
private slots:
  void returnPressedSlot();

private:
  char alias[ALIASSIZE];
};


