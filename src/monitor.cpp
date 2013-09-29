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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h> 
#include <errno.h>

#include <qapplication.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include "monitor.h"



////////////////////////////////////////////////////////////
////               CLASS MonitorItem                      //
////////////////////////////////////////////////////////////
MonitorItem::MonitorItem( QListView * parent)
 : QListViewItem ( parent )
{
  bzero( &bm, sizeof(bookmark_t));
  CmdStart = QString::null;
  CmdOn = QString::null;
  CmdOff = QString::null;
  CmdStop = QString::null;
  CmdOnPID = 0;
  CmdOffPID = 0;
  CmdStartPID = 0;
  CmdStopPID = 0;
  elapseTime = 30000; 
}


void MonitorItem::getBookmark(bookmark_t *dst)
{
  memcpy( dst, &bm, sizeof(bookmark_t));
}


void MonitorItem::setBookmark(bookmark_t *src)
{
  memcpy( &bm, src, sizeof(bookmark_t));

  // display freq
  setText(0,QString::number(bm.freq));

  // mode 
  switch(bm.mode){
  case MODE_NFM: setText(1,"NFM"); break;
  case MODE_WFM: setText(1,"WFM"); break;
  case MODE_USB: setText(1,"USB"); break;
  case MODE_LSB: setText(1,"LSB"); break;
  case MODE_AM:  setText(1,"AM") ; break;
  default: setText(1, "ERROR");
  }

}

QString MonitorItem::getCmd( CmdType type )
{
 switch(type){
 case On:    return CmdOn; 
 case Off:   return CmdOff; 
 case Start: return CmdStart; 
 case Stop:  return CmdStop;
 } 
 return QString::null;
}

void MonitorItem::setCmd( CmdType type, const QString & str )
{
 switch(type){
 case On:    CmdOn    = str; break;
 case Off:   CmdOff   = str; break;
 case Start: CmdStart = str; break;
 case Stop:  CmdStop  = str; break;
 }
}

void MonitorItem::setCmdPID( CmdType type, int pid )
{
  switch(type){
  case On: CmdOnPID = pid; break;
  case Off: CmdOffPID = pid; break;
  case Start: CmdStartPID = pid; break;
  case Stop: CmdStopPID = pid; break;
  }
}

int MonitorItem::getCmdPID( CmdType type )
{
  switch( type ){
  case On: return CmdOnPID; 
  case Off: return CmdOffPID;
  case Start: return CmdStartPID;
  case Stop: return CmdStopPID;
  }

  // error
  return -1;
}

void MonitorItem::setElapseTime( int e )
{
   elapseTime = e;
}


int MonitorItem::getElapseTime()
{
  return elapseTime; 
}


////////////////////////////////////////
///// Overwrite paint focus. No paint //
////////////////////////////////////////
void MonitorItem::paintFocus ( QPainter *, const QColorGroup & cg, const QRect & r )
{
}




////////////////////////////////////////
///// Overwrite paint subroutine     ///
///// to us more flexibily to adjust ///
///// color and style                ///
////////////////////////////////////////
void MonitorItem::paintCell ( QPainter * p, const QColorGroup & cg,
                   int column, int width, int align )
{
  QColorGroup _cg(cg);

  if(isSelected()){

   _cg.setColor (QColorGroup::HighlightedText, Qt::red );
   _cg.setColor (QColorGroup::Text, Qt::red );
   _cg.setColor (QColorGroup::Highlight, Qt::green);
   _cg.setColor (QColorGroup::Base, Qt::green);
  }

  QListViewItem::paintCell (p, _cg, column, width, align);
}


/////////////////////////////////////////////////////////////
///////////         CLASS  MonitorDialog               //////
/////////////////////////////////////////////////////////////

MonitorDialog::MonitorDialog( QWidget * parent, const char * name)
  : QFrame( parent, name )
{
  QGridLayout *UpGrid;
  QFrame *UpFrame;
  QGridLayout *MainGrid;
  QVBox *VBoxPtr;
  QHBox *HBoxPtr;
  QGroupBox *GBoxPtr;
  QLabel *label;
 
  setFrameStyle( NoFrame );
 
  // Crate main box
  MainGrid = new QGridLayout( this, 2 , 1 ); 

  // Create upper frame and grid
  UpFrame = new QFrame( this );
  UpFrame->setFrameStyle( NoFrame );
  UpGrid = new QGridLayout( UpFrame, 1, 2);

  // Create right holder
  VBoxPtr = new QVBox( UpFrame );
  UpGrid->addWidget( VBoxPtr, 0, 1 );
  VBoxPtr->setFrameStyle( NoFrame );
  VBoxPtr->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );

  // add console
  console = new QMultiLineEdit( VBoxPtr );
  console->setReadOnly( true );

  // add monitor command box
  GBoxPtr = new QGroupBox(2, Qt::Horizontal, "Monitor",  VBoxPtr );
  startButton = new QPushButton("Start", GBoxPtr );
  stopButton  = new QPushButton("Stop",  GBoxPtr );
  addButton   = new QPushButton("Add",   GBoxPtr );
  updateButton = new QPushButton("Update", GBoxPtr );
  optionButton = new QPushButton("Option", GBoxPtr );
  deleteButton = new QPushButton("Delete", GBoxPtr );

  optionButton->setToggleButton( true );
 
  connect( startButton, SIGNAL(clicked()), 
           this, SLOT(StartSlot()));

  connect( stopButton, SIGNAL(clicked()),
           this, SLOT(StopSlot()));

  connect( addButton, SIGNAL(clicked()),
           this, SLOT(AddSlot()));

  connect( deleteButton, SIGNAL(clicked()),
           this, SLOT(DeleteSlot()));

  connect( updateButton, SIGNAL(clicked()),
           this, SLOT(UpdateSlot()));

  connect( optionButton, SIGNAL(toggled( bool )),
           this, SLOT(OptionSlot( bool )));

  // Create file buttons
  GBoxPtr = new QGroupBox(2, Qt::Horizontal, "File", VBoxPtr );
  loadButton = new QPushButton("Load", GBoxPtr );
  saveButton = new QPushButton("Save", GBoxPtr );

  connect( loadButton, SIGNAL(clicked()),
           this, SLOT(LoadSlot()));

  connect( saveButton, SIGNAL(clicked()),
           this, SLOT(SaveSlot()));


  // create list view
  mlistView = new QListView( UpFrame );
  mlistView->addColumn("Frequency");
  mlistView->setColumnAlignment( 0, Qt::AlignRight );
  mlistView->addColumn("Mode");
  mlistView->addColumn("Preset");

  mlistView->setFrameStyle( NoFrame );
  UpGrid->addWidget( mlistView, 0, 0);
  UpGrid->setColStretch( 0, 100 );

  // add upgrid to main grid
  MainGrid->addWidget( UpFrame, 0, 0);

  // add options
  GBoxPtr = new QGroupBox(5, Qt::Vertical, "Options", this );

  HBoxPtr = new QHBox( GBoxPtr );
  HBoxPtr->setSpacing( 10 );
  new QLabel("Elapse (msec)", HBoxPtr);
  elapseEdit = new QLineEdit( HBoxPtr );
  new QLabel("Step (msec)", HBoxPtr);
  timeStepEdit = new QLineEdit( HBoxPtr  );
  timeStepEdit->setText("500");
  label = new QLabel("Preset", HBoxPtr);
  label->setAlignment( Qt::AlignRight );
  label->setFrameStyle(NoFrame);
  label->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred) );
  presetCombo = new QComboBox( HBoxPtr );
  addPresetButton = new QPushButton("Add" , HBoxPtr );
  delPresetButton = new QPushButton("Del" , HBoxPtr );

  presetCombo->setFocusPolicy( NoFocus );

  connect( presetCombo, SIGNAL(activated(const QString &)),
           this, SLOT(PresetSlot(const QString &)));

  connect( addPresetButton, SIGNAL(clicked()),
           this, SLOT(AddPreset()));

  connect( delPresetButton, SIGNAL(clicked()),
           this, SLOT(DelPreset()));

  elapseEdit->setMaxLength( 6 );
  timeStepEdit->setMaxLength( 4 );
  timeStepEdit->setText("500");

  HBoxPtr = new QHBox( GBoxPtr );
  label = new QLabel("Start Command ", HBoxPtr );
  label->setFrameStyle( Panel | Sunken );
  cmdStartEdit = new QLineEdit( HBoxPtr );
  cmdStartEdit->setFrame( false );

  HBoxPtr = new QHBox( GBoxPtr );  
  label = new QLabel("On   Command ", HBoxPtr );
  label->setFrameStyle( Panel | Sunken );
  cmdOnEdit = new QLineEdit( HBoxPtr );
  cmdOnEdit->setFrame( false );

  HBoxPtr = new QHBox( GBoxPtr );
  label = new QLabel("Off   Command ", HBoxPtr);
  label->setFrameStyle( Panel | Sunken );
  cmdOffEdit = new QLineEdit( HBoxPtr );
  cmdOffEdit->setFrame( false );
 
  HBoxPtr = new QHBox( GBoxPtr );
  label = new QLabel("Stop Command ", HBoxPtr );
  label->setFrameStyle( Panel | Sunken );
  cmdStopEdit = new QLineEdit( HBoxPtr );
  cmdStopEdit->setFrame( false );
 
  // add options to lowwer main grid
  MainGrid->addWidget( GBoxPtr, 1, 0); 

  // connect time control
  connect( &timeStep, SIGNAL(timeout()),
           this, SLOT(TimeToChangeChannel()));

  connect( &elapseTimer, SIGNAL(timeout()),
           this, SLOT(RecordTimeout()));

  timeStepMsec = 500;
  squelchOn    = true;
  monitoring   = false;

  // selection
  connect(mlistView, SIGNAL(selectionChanged(QListViewItem *)),
           this,SLOT(SelectionChangedSlot(QListViewItem *)));

  connect(mlistView, SIGNAL(doubleClicked(QListViewItem *)),
           this, SLOT(DoubleClickedSlot(QListViewItem *)));

  // set buttons enable
  stopButton->setEnabled(  false  );
  cmdOnEdit->setEnabled( false  );
  cmdOffEdit->setEnabled( false );
  cmdStartEdit->setEnabled( false );
  cmdStopEdit->setEnabled( false );
  elapseEdit->setEnabled(  false  );
  setFocusPolicy( NoFocus );

  // presetList init
  presetList.setAutoDelete( true );
}



MonitorDialog::~MonitorDialog()
{
  // clean up messs
  if(!startButton->isEnabled()){
    StopSlot();
  }
}


///////////////////////////////////////////////////
////      Buttons Handlers                    /////
///////////////////////////////////////////////////
void MonitorDialog::StartSlot()
{

  bool ok;

  timeStepMsec = timeStepEdit->text().toUInt(&ok);
  if( !ok ){
    QMessageBox::warning( this, "QtPCR", "Cannot determine time step");
    timeStepMsec = 500;
    return;
  }

  if( timeStepMsec < 50 ){
    QMessageBox::warning( this, "QtPCR", "Time step is too small");
    timeStepMsec = 500;
    return;
  } 


  currentItem = (MonitorItem *)mlistView->firstChild();
  if(currentItem){
   
   currentItem->getBookmark( &bm );
   emit gotoBookmark( &bm );

   // execute start 
   QListViewItemIterator it ( mlistView );
   for( ; it.current() ; ++it ){
     currentItem = (MonitorItem *)it.current();
     if(currentItem->getCmd(MonitorItem::Start))
        ExecuteCmd( MonitorItem::Start, currentItem->getCmd(MonitorItem::Start) );
   }

   // begin with first child 
   currentItem = (MonitorItem *)mlistView->firstChild();
   currentItem->getBookmark( &bm );
   emit gotoBookmark( &bm );

   monitoring = true;

   timeStep.start( timeStepMsec, true );

   // set buttons enable
   mlistView->setEnabled(   true  );
   cmdOnEdit->setEnabled(   false );
   cmdOffEdit->setEnabled(  false );
   cmdStartEdit->setEnabled(false );
   cmdStopEdit->setEnabled( false );
   elapseEdit->setEnabled(  false );
   timeStepEdit->setEnabled(false );
   presetCombo->setEnabled( false );
   startButton->setEnabled( false );
   stopButton->setEnabled(  true  );
   optionButton->setEnabled(false );
   updateButton->setEnabled(false );
   addButton->setEnabled(   false );
   deleteButton->setEnabled(false );
   loadButton->setEnabled(  false );
   saveButton->setEnabled(  true  );
 
   console->clear();

  }

}


void MonitorDialog::ExecuteStop()
{
   // execute stop command 
   QListViewItemIterator it ( mlistView );
   for( ; it.current() ; ++it ){
     currentItem = (MonitorItem *)it.current();
     if(currentItem->getCmd(MonitorItem::Stop))
        ExecuteCmd( MonitorItem::Stop, currentItem->getCmd(MonitorItem::Stop) );
   }

   // set buttons enable
   mlistView->setEnabled(   true  );
   cmdOnEdit->setEnabled(   false );
   cmdOffEdit->setEnabled(   false );
   cmdStartEdit->setEnabled( false );
   cmdStopEdit->setEnabled(  false );
   elapseEdit->setEnabled(  false );
   timeStepEdit->setEnabled(true );
   presetCombo->setEnabled( true  );
   startButton->setEnabled( true );
   stopButton->setEnabled(  false  );
   updateButton->setEnabled(true );
   optionButton->setEnabled(true );
   addButton->setEnabled(   true );
   deleteButton->setEnabled(true );
   loadButton->setEnabled(  true );
   saveButton->setEnabled(  true  );

}


void MonitorDialog::StopSlot()
{
  monitoring = false;
  timeStep.stop();
  if(elapseTimer.isActive()){
     elapseTimer.stop();
     RecordTimeout();
  }

   // wait before execute stop, just to be save 
   QTimer::singleShot(2000, this, SLOT(ExecuteStop()));
}

void MonitorDialog::AddSlot()
{
  MonitorItem *mItem;

  // load radio data 
  emit getBookmark( &bm );

  // create item
  mItem = new MonitorItem( mlistView );
  mItem->setBookmark( &bm );

  mlistView->setSelected( mItem, true );
}


void MonitorDialog::toMonitorSlot( struct bookmark_t *src)
{
  MonitorItem *mItem;

  memcpy( &bm, src, sizeof(struct bookmark_t));

  // create item
  mItem = new MonitorItem( mlistView );
  mItem->setBookmark( &bm );
}


void MonitorDialog::UpdateSlot()
{
  MonitorItem *mItem;
  // load radio data
  emit getBookmark( &bm );

  // overwrite
  mItem = (MonitorItem *)(mlistView->selectedItem());
  if(mItem)
    mItem->setBookmark( &bm ); 
}


void MonitorDialog::OptionSlot(bool flag)
{
  bool ok;
  int i;

  if( !flag ){

    i = elapseEdit->text().toUInt( &ok );
    if(!ok){
      disconnect( optionButton, SIGNAL(toggled( bool )),
           this, SLOT(OptionSlot( bool )));

      optionButton->setOn( true );

      connect( optionButton, SIGNAL(toggled( bool )),
           this, SLOT(OptionSlot( bool )));

      return;
    }

    MonitorItem *mItem = (MonitorItem *)(mlistView->selectedItem());
    if(mItem){
      mItem->setElapseTime(i);
      mItem->setCmd(MonitorItem::On, cmdOnEdit->text());
      mItem->setCmd(MonitorItem::Off, cmdOffEdit->text());
      mItem->setCmd(MonitorItem::Start, cmdStartEdit->text());
      mItem->setCmd(MonitorItem::Stop, cmdStopEdit->text());

      mItem->setText( 2, QString::null );
    }
  }else{

    MonitorItem *mItem = (MonitorItem *)(mlistView->selectedItem());
    if(!mItem){

      disconnect( optionButton, SIGNAL(toggled( bool )),
           this, SLOT(OptionSlot( bool )));

      optionButton->setOn( false );

      connect( optionButton, SIGNAL(toggled( bool )),
           this, SLOT(OptionSlot( bool )));

      return;
    }
  }

  mlistView->setEnabled(   ! flag );
  cmdOnEdit->setEnabled(     flag );
  cmdOffEdit->setEnabled (   flag );
  cmdStartEdit->setEnabled(  flag );
  cmdStopEdit->setEnabled(   flag );
  elapseEdit->setEnabled(    flag );
  startButton->setEnabled( ! flag );
  updateButton->setEnabled(! flag );
  addButton->setEnabled(   ! flag );
  deleteButton->setEnabled(! flag );
  loadButton->setEnabled(  ! flag );
  saveButton->setEnabled(  ! flag );
}


void MonitorDialog::DeleteSlot()
{
  QListViewItem *selectedItem;

  selectedItem = mlistView->selectedItem();

  if(selectedItem){
    // clear option text
    elapseEdit->setText(QString::null);
    cmdOnEdit->setText(QString::null);
    cmdOffEdit->setText(QString::null);
    cmdStartEdit->setText(QString::null);
    cmdStopEdit->setText(QString::null);
    delete selectedItem;
  }
}

void MonitorDialog::LoadSlot()
{
  QFile f;
  QString str;

  str = QFileDialog::getOpenFileName();
  if(str.isNull())
    return;

  f.setName( str );
  if(!f.open( IO_ReadOnly )){
    QMessageBox::warning(0, "QtPCR", "Cannot open file for reading");
    return;
  }

  char buf[1024];
  int i;
  QString alias, on, off, start, stop, e;
  while(!f.atEnd()){
#define READ_PRESET(entry)    bzero(buf, 1024); \
                              i = f.readLine(buf, 1024); \
                              if(i < 0){ \
                                 QMessageBox::warning(0, "QtPCR", "Read file error"); \
                                 return; \
                              } \
                              for( i--; i >= 0; i--) \
                                if( buf[i]=='\r' || buf[i]=='\n') \
                                   buf[i] = 0; \
                                 else \
                                   break; \
                              entry = buf 

      READ_PRESET(alias);
      READ_PRESET(start);
      READ_PRESET(on);
      READ_PRESET(off);
      READ_PRESET(stop);
      READ_PRESET(e);
     
      bool ok;
      e.toUInt(&ok);
      if(!ok){
        QMessageBox::warning(0, "QtPCR", "Read file error");
        return;
      }
 
      presetList.append( new Preset(alias, start, on, off, stop, e.toUInt() ));
      presetCombo->insertItem( alias );
  }

}

void MonitorDialog::SaveSlot()
{
  QFile f;
  QString str;

  str = QFileDialog::getSaveFileName();
  if(str.isNull())
    return;

  f.setName( str );
  if(!f.open( IO_WriteOnly | IO_Truncate )){
    QMessageBox::warning(0, "QtPCR", "Cannot open file for writing");
    return;
  }

  Preset *preset;

#define WRITE_PRESET(entry) str = preset->entry(); \
                            str.append("\r\n");    \
                            f.writeBlock( (const char *)str, str.length())

  // write preset to text file 
  for( preset = presetList.first(); preset != 0; preset = presetList.next()){
     WRITE_PRESET(Alias);
     WRITE_PRESET(CmdStart);
     WRITE_PRESET(CmdOn);
     WRITE_PRESET(CmdOff);
     WRITE_PRESET(CmdStop);

     str = QString::number(preset->ElapseTime());
     str.append("\r\n");
     f.writeBlock( (const char *)str, str.length());
  }

  f.flush();
}


void MonitorDialog::AddPreset()
{
  QString str;

  str = QInputDialog::getText("QtPCR","Enter Preset Alias");
  if(str.isNull()) 
    return;

  presetList.append( new Preset(
                      str,
                      cmdStartEdit->text(),
                      cmdOnEdit->text(),
                      cmdOffEdit->text(),
                      cmdStopEdit->text(),
                      elapseEdit->text().toUInt() 
                      )); 
 
  presetCombo->insertItem( str, 0 );
  presetCombo->setCurrentItem( 0 );
}


void MonitorDialog::DelPreset()
{
  Preset *preset;

  for( preset = presetList.first(); preset != 0; preset = presetList.next() ){
    if(preset->Alias() == presetCombo->currentText()){
       presetList.remove(preset);
       break;
    }
  }

  presetCombo->removeItem( presetCombo->currentItem() );
}

/////////////////////////////////////////////////
/////     next channel control                 //
/////////////////////////////////////////////////
void MonitorDialog::TimeToChangeChannel()
{
  if(squelchOn){

   // execute command
   if( currentItem->getCmd( MonitorItem::On ) )
      ExecuteCmd( MonitorItem::On, currentItem->getCmd( MonitorItem::On ) );

   // report to console
   currentItem->getBookmark( &bm );
   console->insertLine(QTime::currentTime().toString() + " - " + QString::number(bm.freq));

   // begin elapse clock
   elapseTimer.start( currentItem->getElapseTime(), true ); 

  }else{

    // step to next frequency
    currentItem = (MonitorItem *)currentItem->nextSibling();
    if(!currentItem)
      currentItem = (MonitorItem *)mlistView->firstChild();

    currentItem->getBookmark( &bm );

    emit gotoBookmark( &bm );

    // reset timer
    if(monitoring)
      timeStep.start( timeStepMsec, true );
  }
}


void MonitorDialog::RecordTimeout()
{
     // execute command
     if( currentItem->getCmd( MonitorItem::Off ) )
          ExecuteCmd( MonitorItem::Off , currentItem->getCmd( MonitorItem::Off ) );

     if(monitoring){

       // step to next frequency
       currentItem = (MonitorItem *)currentItem->nextSibling();
       if(!currentItem)
         currentItem = (MonitorItem *)mlistView->firstChild();

       currentItem->getBookmark( &bm );
       emit gotoBookmark( &bm );

       // reset timer
       timeStep.start( timeStepMsec, true );
     }
}



void MonitorDialog::ExecuteCmd( MonitorItem::CmdType type, QString Cmd )
{
   int CmdOnPID, CmdOffPID, CmdStartPID, CmdStopPID;

   currentItem->getBookmark( &bm );
   CmdOnPID = currentItem->getCmdPID( MonitorItem::On );
   CmdOffPID = currentItem->getCmdPID( MonitorItem::Off );
   CmdStartPID = currentItem->getCmdPID( MonitorItem::Start );
   CmdStopPID = currentItem->getCmdPID( MonitorItem::Stop );
 
   // parse variables
   Cmd.replace(QRegExp("\\$FREQ"),QString::number(bm.freq));
   Cmd.replace(QRegExp("\\$ONPID"),QString::number(CmdOnPID));
   Cmd.replace(QRegExp("\\$OFFPID"),QString::number(CmdOffPID));
   Cmd.replace(QRegExp("\\$STARTPID"),QString::number(CmdStartPID));
   Cmd.replace(QRegExp("\\$STOPPID"),QString::number(CmdStopPID));
   Cmd.replace(QRegExp("\\$TIME"),QTime::currentTime().toString());

   char buf[1024];
   char *ch=buf;
   char *argv[128];
   int  n=0;

   bzero(buf, 1024);
   bzero(argv, 128*sizeof(char *));
   Cmd.truncate( 1023 );
   strncpy( buf, (const char *)Cmd, 1023 );

   // argv parser
   ch = buf;
   while( *ch && n < 127){
     switch( *ch ){

     case ' ':
        *(ch++) = '\0';
        for(; *ch && *ch==' '; *(ch++)='\0');
     break;

     case '"':
        *(ch++) = '\0';
        argv[n++] = ch++;
        for(; *ch && *ch!='"'; ch++);
        if( *ch == '"' )
           *(ch++) = '\0';
     break;

     default:
        argv[n++] = ch++;
        for(; *ch && *ch!=' ' && *ch!='"'; ch++);
     }
   }

   argv[n++] = NULL;

   // fork child
   int pid = fork();
   if(pid == 0){ // child

      execvp(argv[0],argv);
      perror("Cannot execute Start command");
      exit(1);

   }else if(pid > 0){ // parent  success
      currentItem->setCmdPID( type, pid );
   }else{   // parent error
      currentItem->setCmdPID( type, 0 );
   }
  
}


//////////////////////////////////////////////
////////  update from radio               ////
//////////////////////////////////////////////
void MonitorDialog::sigStrengthUpdate( unsigned char sig)
{
}

void MonitorDialog::squelchOnUpdate( bool flag)
{
  squelchOn = flag;

  if(!flag && elapseTimer.isActive()){
    elapseTimer.stop();
    RecordTimeout();
  }
}

void MonitorDialog::DoubleClickedSlot(QListViewItem *l)
{
  MonitorItem *mItem = (MonitorItem *)l;

  mItem->getBookmark( &bm );
  emit gotoBookmark( &bm );
}

void MonitorDialog::SelectionChangedSlot(QListViewItem *l)
{
  MonitorItem *mItem = (MonitorItem *)l;

  elapseEdit->setText(QString::number(mItem->getElapseTime())); 
  cmdOnEdit->setText(mItem->getCmd(MonitorItem::On));
  cmdOffEdit->setText(mItem->getCmd(MonitorItem::Off));
  cmdStartEdit->setText(mItem->getCmd(MonitorItem::Start));
  cmdStopEdit->setText(mItem->getCmd(MonitorItem::Stop));
}

void MonitorDialog::PresetSlot( const QString & str )
{
  // not option mode and not selected 
  if( !mlistView->selectedItem() ) 
     return;

  MonitorItem *mItem = (MonitorItem *)mlistView->selectedItem();

  Preset *preset;
  for( preset = presetList.first(); preset != 0; preset = presetList.next() )
    if(preset->Alias() == presetCombo->currentText()) break;

  if(preset == 0) return;

  elapseEdit->setText(QString::number(preset->ElapseTime()));
  cmdOnEdit->setText(preset->CmdOn());
  cmdOffEdit->setText(preset->CmdOff());
  cmdStartEdit->setText(preset->CmdStart());
  cmdStopEdit->setText(preset->CmdStop());

  if( !optionButton->isOn() )
    mItem->setText(2, preset->Alias());

  if( !optionButton->isOn() ){
    mItem->setElapseTime( elapseEdit->text().toUInt() );
    mItem->setCmd( MonitorItem::On,    cmdOnEdit->text() );
    mItem->setCmd( MonitorItem::Off,   cmdOffEdit->text() );
    mItem->setCmd( MonitorItem::Start, cmdStartEdit->text() );
    mItem->setCmd( MonitorItem::Stop,  cmdStopEdit->text() );
  }
}
