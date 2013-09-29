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

#include "aliasdisplay.h"
#include <stdio.h>

InfoLabel::InfoLabel(QWidget * parent, const char * name)
  : QLabel( parent, name ){}

void InfoLabel::mouseReleaseEvent ( QMouseEvent * e)
{
  emit mouseReleased(); 
}

InfoDisplay::InfoDisplay(QWidget * parent, const char * name)
  : QScrollView( parent, name)
{
  QColorGroup cg(colorGroup());
  
  cg.setColor(QColorGroup::Foreground, Qt::cyan);
  cg.setColor(QColorGroup::Dark, Qt::black);
  cg.setColor(QColorGroup::Background, Qt::black);

  setPalette( QPalette(cg, cg, cg) );

  info = QString::null;
  
  infoLine = new InfoLabel( viewport() );
  tictoc = new QTimer;
  speed = 50;
  editing = false;
  
  setFrameStyle( NoFrame );
  infoLine->setFrameStyle( NoFrame );
  infoLine->setPalette( QPalette(cg, cg, cg) );
  setHScrollBarMode( AlwaysOff );
  setVScrollBarMode( AlwaysOff );
  addChild(infoLine);
  connect( tictoc, SIGNAL(timeout()),
	   this, SLOT(timeToScroll()) );

  connect( infoLine, SIGNAL(mouseReleased()),
	   this, SLOT(infoMouseReleasedSlot()) );

  infoUpdate("");
}

QString InfoDisplay::text()
{ return info; }

void InfoDisplay::infoUpdate(const char *str){
  QString temp;
  QColorGroup cg(colorGroup());

  info = str;
  if(info.isEmpty()){
    tictoc->stop();
  }

  tictoc->start( speed );
  temp = info.simplifyWhiteSpace();
  temp.prepend("                                       ");
  temp.append ("                                       ");

  infoLine->setText(temp);
}

void InfoDisplay::setRunning(bool flag)
{
  if(flag){
    tictoc->start( speed  );
  }else
    tictoc->stop();
}

void InfoDisplay::setSpeed( int speed )
{
  this->speed = speed;
  if(tictoc->isActive())
    tictoc->start( speed );
}

void InfoDisplay::timeToScroll()
{
  int x;
  
  x = contentsX();
  scrollBy( 1 , 0 );

  if(x == contentsX())
    scrollBy( -1024, 0);

}


void InfoDisplay::infoMouseReleasedSlot() 
{
  editing = !editing;
  emit editingMode( editing );
  if(editing)
    infoUpdate("Click HERE again after you done editing");
}


AliasDisplay::AliasDisplay(QWidget * parent, const char * name)
  : QLineEdit(parent, name)
{
  QColorGroup cg(colorGroup());
  
  cg.setColor(QColorGroup::Text, Qt::cyan);
  cg.setColor(QColorGroup::Base, Qt::black);
  cg.setColor(QColorGroup::Background, Qt::black);

  setPalette( QPalette(cg, cg, cg) );

  bzero(alias, ALIASSIZE);
  setMaxLength( ALIASSIZE );
  setReadOnly( true );
  setFrame( false );
  setFocusPolicy( ClickFocus );
  connect( this, SIGNAL(returnPressed()), 
	   this, SLOT(returnPressedSlot()));
}


void AliasDisplay::aliasUpdate(const char *alias )
{
  strncpy( this->alias, alias, ALIASSIZE);
  setText(alias);
}

void AliasDisplay::mouseReleaseEvent ( QMouseEvent * e)
{
  QColorGroup cg(colorGroup());

  if(isReadOnly()){

    cg.setColor(QColorGroup::Text, Qt::yellow);
    cg.setColor(QColorGroup::Base, Qt::darkGreen);
    cg.setColor(QColorGroup::Background, Qt::darkGreen);
    setPalette( QPalette(cg, cg, cg) );
    repaint();

    setReadOnly( false );
  }else{
    returnPressedSlot();
  }
}


void AliasDisplay::returnPressedSlot()
{
  QString str;
  QColorGroup cg(colorGroup());

  setReadOnly( true );
  if(edited()){
    strncpy(alias, (const char *)text(), ALIASSIZE);
    emit setAlias(alias);
  }

  cg.setColor(QColorGroup::Text, Qt::cyan);
  cg.setColor(QColorGroup::Base, Qt::black);
  cg.setColor(QColorGroup::Background, Qt::black);

  setPalette( QPalette(cg, cg, cg) );
  repaint();

  focusNextPrevChild( false );
}
