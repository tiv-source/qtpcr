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

#ifndef _BMDATA_H
#define _BMDATA_H

////////////////////////////////////////
///////   Process channel database  ////
////////////////////////////////////////
#include <qfile.h>
#include <qfiledialog.h>
#include <qlistview.h>
#include <qregexp.h>
#include "pcrproto.h"

////////////////////////////////////////
////  Process individual channel ///////
////////////////////////////////////////
class BookmarkItem : public QListViewItem
{
public:
  BookmarkItem( QListView * parent );
  BookmarkItem( QListView * parent, BookmarkItem *after);
  BookmarkItem( QListViewItem *parent );
  BookmarkItem( QListViewItem *parent, BookmarkItem *after);

  bool isFolderBegin();
  void setIsFolderBegin( bool );
  void setBookmark( struct bookmark_t *, bool);
  void getBookmark( struct bookmark_t *);
  void paintCell( QPainter *, const QColorGroup &, int, int, int); 
  void paintFocus ( QPainter *, const QColorGroup & , const QRect & );
  QString key ( int, bool ) const;

private:
  struct bookmark_t bm;
  bool   folderBegin;
  void   SetDefault();
  void   fillColumnText();
};




//////////////////////////////////////
////////  Process entire database ////
//////////////////////////////////////
class BookmarkData
{
public:
  BookmarkData();
  bool Load(QListView *);
  bool Save(QListView *);

private:
  QFile dataFile;
  QListView *bmList;

  bool ReadCycle(BookmarkItem *, int);
  bool WriteEntry(struct bookmark_t *);
  bool ReadEntry(struct bookmark_t *);
  bool WriteCycle();
};

#endif // _BMDATA_H

