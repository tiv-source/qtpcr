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

#ifndef BMTREE_H_
#define BMTREE_H_

//////////////////////////
///  Channel Browser   ///
///  (Bookmark style   ///
//////////////////////////
#include <qdialog.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

#include "pcrproto.h"
#include "bmdata.h"

class BookmarkTree : public QDialog
{
  Q_OBJECT

public:
  BookmarkTree(QWidget * parent=0, const char * name=0);

  QSize sizeHint () const;
signals:
 void gotoBookmark(struct bookmark_t *);
 void getBookmark(struct bookmark_t *);
 void toMonitor( struct bookmark_t *);

private slots:
 void DoubleClickedSlot(QListViewItem *);
 void SelectionChangedSlot(QListViewItem *);
 void LoadBookmarkSlot();
 void SaveBookmarkSlot();
 void AddChannelSlot();
 void MonitorChannelSlot();
 void ScriptChannelSlot();
 void FolderChannelSlot();
 void UpdateChannelSlot();
 void DeleteChannelSlot();

private:
 struct bookmark_t bm;

 BookmarkData *bmData;
 QListView    *bmList;
 QPushButton  *addButton;
 QPushButton  *monitorButton;
 QPushButton  *scriptButton;
 QPushButton  *folderButton;
 QPushButton  *updateButton;
 QPushButton  *deleteButton;
 QPushButton  *loadButton;
 QPushButton  *saveButton;
 QMultiLineEdit *info;
};
#endif // BMTREE_H_
