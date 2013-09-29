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

#include <qtooltip.h>

#include "bmtree.h"

///////////////////////////////////////////////////////////////
/////////////       Create Dialog Windows            //////////
///////////////////////////////////////////////////////////////
BookmarkTree::BookmarkTree( QWidget * parent, const char * name)
  : QDialog(parent, name)
{
  // dummy pointer to setup dialog
  QGroupBox   *GBoxPtr;
  QGridLayout *MainGrid, *RightGrid;
  QFrame      *FramePtr;

  // set variables to default values
  bzero( &bm, sizeof(bm));
  bm.freq     = DEFAULT_FREQ;
  bm.mode     = DEFAULT_MODE;
  bm.filter   = DEFAULT_FILTER;
  bm.ifshift  = DEFAULT_IFSHIFT;
  bm.vol      = 0;
  bm.squelch  = 0;
  bm.ATT      = 0;
  bm.AGC      = 0;
  bm.NB       = 0;
  bm.VSC      = 0;
  bm.AFC      = 0;
  bm.alias[0] = '\0';
  bm.info[0]  = '\0';

  // create frame and grid for right side
  FramePtr  = new QFrame( this );
  RightGrid = new QGridLayout( FramePtr, 3 , 1);

  // create group boxs to hold channel buttons
  GBoxPtr = new QGroupBox( FramePtr );
  GBoxPtr->setColumnLayout(3, QGroupBox::Vertical);
  GBoxPtr->setTitle( "Channel" );

  // create channel buttons
  addButton     = new QPushButton("Add" ,    GBoxPtr);
  monitorButton = new QPushButton("Monitor", GBoxPtr);
  scriptButton  = new QPushButton("Script",  GBoxPtr);
  updateButton  = new QPushButton("Update",  GBoxPtr);
  folderButton  = new QPushButton("Folder",  GBoxPtr);
  deleteButton  = new QPushButton("Delete",  GBoxPtr);

  // the buttons has no keyboard focus, preventing 
  // accidental clicked
  addButton->setFocusPolicy( NoFocus );
  folderButton->setFocusPolicy( NoFocus );
  updateButton->setFocusPolicy( NoFocus );
  deleteButton->setFocusPolicy( NoFocus );

  // connect buttons to handlers
  connect(addButton, SIGNAL(clicked()),
	  this, SLOT(AddChannelSlot()));

  connect(monitorButton, SIGNAL(clicked()),
  	  this, SLOT(MonitorChannelSlot()));

  connect(scriptButton, SIGNAL(clicked()),
      this, SLOT(ScriptChannelSlot()));

  connect(folderButton, SIGNAL(clicked()),
	  this, SLOT(FolderChannelSlot()));

  connect(updateButton, SIGNAL(clicked()),
	  this, SLOT(UpdateChannelSlot()));

  connect(deleteButton, SIGNAL(clicked()),
	  this, SLOT(DeleteChannelSlot()));

  // add this channel group box to right frame
  RightGrid->addWidget( GBoxPtr, 1, 0 );

  // create group box to hold file buttons
  GBoxPtr = new QGroupBox( FramePtr );
  GBoxPtr->setColumnLayout(1, QGroupBox::Vertical);
  GBoxPtr->setTitle("File");

  // create files buttons
  loadButton = new QPushButton("Load", GBoxPtr);
  saveButton = new QPushButton("Save", GBoxPtr);

  // again they have no keyboard focus
  loadButton->setFocusPolicy( NoFocus );
  saveButton->setFocusPolicy( NoFocus );

  // connect buttons to handlers
  connect(loadButton, SIGNAL(clicked()),
	  this, SLOT(LoadBookmarkSlot()) );

  connect(saveButton, SIGNAL(clicked()),
	  this, SLOT(SaveBookmarkSlot()) );

  // add this file group box to right frame
  RightGrid->addWidget( GBoxPtr, 2, 0); 

  // create label box
  info = new QMultiLineEdit( FramePtr );

  // set property for the beauty
  info->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  info->setReadOnly( true );
  info->setWordWrap( QMultiLineEdit::WidgetWidth );

  // add label box to grid and make it recieves 
  // largest area
  RightGrid->setRowStretch( 0, 10 );
  RightGrid->addWidget( info, 0, 0 );

  // create objects to handles channels database
  bmData = new BookmarkData();

  // create listview to display above database
  bmList = new QListView( this );

  // set styles
  bmList->setFrameStyle( QFrame::NoFrame );
  bmList->setRootIsDecorated( true );
  bmList->addColumn("Frequency");
  bmList->addColumn("Mode");
  bmList->addColumn("Alias");
  bmList->setColumnAlignment(0,Qt::AlignRight);
  
  // connect to handlers
  connect(bmList,SIGNAL(doubleClicked( QListViewItem *)),
	  this,SLOT(DoubleClickedSlot( QListViewItem *)));

  connect(bmList, SIGNAL(returnPressed(QListViewItem *)),
	  this,SLOT(DoubleClickedSlot( QListViewItem *)));

  connect(bmList, SIGNAL(selectionChanged(QListViewItem *)),
      this,SLOT(SelectionChangedSlot(QListViewItem *)));

  // create 2x1 grid for whole dialog
  MainGrid  = new QGridLayout( this, 1, 2); 

  // both left and right
  MainGrid->addWidget( bmList, 0, 0);
  MainGrid->setColStretch( 0, 10 );
  MainGrid->addWidget( FramePtr, 0, 1);
  
  // add tool tip
  QToolTip::add(addButton,"Add channel to current folder");
  QToolTip::add(folderButton,"Create new folder");
  QToolTip::add(updateButton,"Overwrite radio status\nto current channel");
  QToolTip::add(deleteButton,"Delete current channel");

  updateGeometry();
}



////////////////////////////////////////
/////        Load channel database  ////
////////////////////////////////////////
void BookmarkTree::LoadBookmarkSlot(){
  bmData->Load( bmList );
}



////////////////////////////////////////
////           Save Channel database ///
////////////////////////////////////////
void BookmarkTree::SaveBookmarkSlot(){
  bmData->Save( bmList );
}



//////////////////////////////////////////
////  Send channel to monitor Dialog /////
//////////////////////////////////////////
void BookmarkTree::MonitorChannelSlot(){
  BookmarkItem *selectedItem;

  selectedItem = (BookmarkItem *)bmList->selectedItem();

  if(selectedItem){
    selectedItem->getBookmark( &bm );
    emit toMonitor( &bm );
  }

}


///////////////////////////////////////////
///////  Send channel to script Dialog ////
///////////////////////////////////////////
void BookmarkTree::ScriptChannelSlot(){
  QMessageBox::information(this,
       "Script - Wish List",
       "Script - (Not yet implemented).\n"
       "Build-in script language makes it easy to implement\n"
       "sophisticate search (e.g. truck-tracking )");
}



///////////////////////////////////////////
/////  Take channel from radio control  ///
/////  panel and add to database        ///
///////////////////////////////////////////
void BookmarkTree::AddChannelSlot(){
  BookmarkItem *lastItem;
  BookmarkItem *selectedItem;

  emit getBookmark( &bm );
  
  selectedItem = (BookmarkItem *) bmList->selectedItem();
  if(selectedItem){
    // check if folder is selected
    if(selectedItem->isFolderBegin()){
      lastItem = new BookmarkItem( selectedItem );
    }else{ // otherwise look for parent
      if(selectedItem->parent())
	lastItem = new BookmarkItem( selectedItem->parent());
      else
	lastItem = new BookmarkItem( bmList );
    }
  }else{
    // add to bmList by default
    lastItem = new BookmarkItem( bmList );
  }

  lastItem->setBookmark( &bm, true );
  bmList->setSelected( lastItem, true );
}



////////////////////////////////////////
////  Add folder to database          //
////////////////////////////////////////
void BookmarkTree::FolderChannelSlot(){
  BookmarkItem *selectedItem;
  BookmarkItem *lastItem;
  bookmark_t   bm;
  QString      str;

  
  // get folder name
  str = QInputDialog::getText(QString("Folder"), 
			      QString("Please enter folder name"));
  if(str.isEmpty()) return;

  // add folder
  selectedItem = (BookmarkItem *)bmList->selectedItem();
  if(selectedItem){
    
    // otherwise look for parent
    if(selectedItem->parent())
      lastItem = new BookmarkItem( selectedItem->parent());
    else
      lastItem = new BookmarkItem( bmList );
    
  }else
    lastItem = new BookmarkItem( bmList );

  // fill in data structure and set folder flag
  bzero( &bm, sizeof(struct bookmark_t));
  strcpy( bm.alias, str.prepend("FOLDER BEGIN:"));
  lastItem->setBookmark( &bm, false );
  lastItem->setText(0, str.remove(0,13));
  lastItem->setIsFolderBegin( true );
}



/////////////////////////////////////////
////  Take channel from radio control ///
////  panel and overwrite it to       ///
////  database                        ///
/////////////////////////////////////////
void BookmarkTree::UpdateChannelSlot(){
  BookmarkItem *selectedItem;
  
  emit getBookmark( &bm );
  selectedItem = (BookmarkItem *)bmList->selectedItem();

  if(selectedItem)
    selectedItem->setBookmark( &bm, true );

  info->setText( bm.info );
}



////////////////////////////////////////
//// Delete channel from database    ///
////////////////////////////////////////
void BookmarkTree::DeleteChannelSlot(){
  QListViewItem *selectedItem;

  selectedItem = bmList->selectedItem();
  
  if(selectedItem)
    delete selectedItem;
}



//////////////////////////////////////////
////  Active radio control according   ///
////  to selected database             ///
//////////////////////////////////////////
void BookmarkTree::DoubleClickedSlot( QListViewItem *currentItem)
{  
  BookmarkItem *bmItem = (BookmarkItem *)currentItem;

  // if is a folder, then rename it
  if(bmItem->isFolderBegin()){

    // get folder name
    QString str = QInputDialog::getText(QString("Rename Folder"),
                      QString("Please enter folder name"));
    if(str.isEmpty()) return;

    bmItem->setText(0,str);
    bmItem->getBookmark( &bm );
    strncpy(bm.alias , str.prepend("FOLDER BEGIN:"), ALIASSIZE);
    bmItem->setBookmark( &bm, false);

    return;
  } 

  bmItem->getBookmark( &bm );
  emit gotoBookmark( &bm );
}



/////////////////////////////////////////
//// Update Info                     ////
/////////////////////////////////////////
void BookmarkTree::SelectionChangedSlot(QListViewItem *currentItem)
{
  BookmarkItem *bmItem = (BookmarkItem *)currentItem;
  bookmark_t bm;

  bmItem->getBookmark( &bm );
  info->setText( bm.info );  
}


/////////////////////////////////////
///// size cosmetic /////////////////
/////////////////////////////////////
QSize BookmarkTree::sizeHint() const
{ return QSize( 500, 400); }
