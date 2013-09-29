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
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <qtooltip.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <stdio.h>
#include <math.h>
#include "scandialog.h"

#define CANVAS_HEIGHT 310
#define CANVAS_ZEROAXIS 305


//////////////////////////////////////////////////////////
//// CLASS  SignalLine                                ////
//////////////////////////////////////////////////////////

////////////////////////
/// constructor      ///
////////////////////////
SignalLine::SignalLine( QCanvas * canvas )
  : QCanvasLine( canvas )
{
  freq = DEFAULT_FREQ;
  sig  = 0;
}


//////////////////////////////
//// return identification ///
//////////////////////////////
int SignalLine::rtti() const{ return SignalLineID; }


/////////////////////////////
/// return frequency      ///
/////////////////////////////
pcrfreq_t SignalLine::getFrequency(){ return freq; }


/////////////////////////////
///  return signal stregnth//
/////////////////////////////
unsigned char SignalLine::getSig(){ return sig; }

void SignalLine::setFrequency(pcrfreq_t freq){ this->freq = freq; }
void SignalLine::setSig( unsigned char sig){ this->sig = sig; }





////////////////////////////////////////////////////////////
///////           CLASS SignalMarker                 ///////
////////////////////////////////////////////////////////////


////////////////////////////
/// constructor           //
////////////////////////////
SignalMarker::SignalMarker( QCanvas * canvas ) : SignalLine( canvas )
{
   freqText = new QCanvasText( canvas );                   
}


////////////////////////////
///  return id            //
////////////////////////////
int SignalMarker::rtti() const
{
    return SignalMarkerID;
}


/////////////////////////////////
// set frequency text display ///
/////////////////////////////////
void SignalMarker::setFreqText(int x)
{
  QString str;
  QRect rect;

  // adjust units
  if( freq >= 1000000000)
      str.sprintf("%6.3fGHz",freq/1000000000.0);
  else if( freq >= 1000000)
     str.sprintf("%6.3fMHz",freq/1000000.0);
  else
     str.sprintf("%6.3fkHz",freq/1000.0);

  freqText->setText(str);
  rect = freqText->boundingRect();
  freqText->setX( x - rect.width()/2 );
  freqText->setY( CANVAS_ZEROAXIS-260-rect.height() );
  freqText->setZ(10);
}



//////////////////////////////////////////////
////  set frequency for marker            ////
//////////////////////////////////////////////
void SignalMarker::setFrequency( pcrfreq_t freq)
{
  SignalLine::setFrequency( freq );
  setFreqText( startPoint().x() );
}


///////////////////////////////////////////////
/// overwrite setPoints to set text also    ///
///////////////////////////////////////////////
void SignalMarker::setPoints( int xa, int ya, int xb, int yb )
{
  SignalLine::setPoints( xa, ya, xb, yb );
  setFreqText( xa );
}


//////////////////////////////////////////
///  overwrite show() to show text too. //
//////////////////////////////////////////
void SignalMarker::show()
{
  freqText->show();
  SignalLine::show();
}


/////////////////////////////////////////
///  set text font and color        /////
/////////////////////////////////////////
void SignalMarker::setFont ( const QFont & f)
{ freqText->setFont( f ); }

void SignalMarker::setColor ( const QColor & c)
{ freqText->setColor( c );}






/////////////////////////////////////////////////////////////////
/////////////          CLASS  SignalRuler                   /////
/////////////////////////////////////////////////////////////////

///////////////////////////////
///////  initialize ///////////
///////////////////////////////
pcrfreq_t SignalRuler::f1 = 0;
pcrfreq_t SignalRuler::f2 = 0;
int SignalRuler::x1 = 0;
int SignalRuler::x2 = 0;

//////////////////////////////
/////// constructor //////////
//////////////////////////////
SignalRuler::SignalRuler() {}

////////////////////////////////////////////
//////  update ruler                ////////
//////  must be called every time we ///////
/////   change display scale         ///////
////////////////////////////////////////////
void SignalRuler::update(pcrfreq_t freq, int x)
{
  if(f1 == 0){
    f1 = freq;
    x1 = x;
  }

  if(f2 == 0){
    f2  = freq;
    x2  = x;
  }

  if( x < x1 ){
     f1 = freq;
     x1 = x;
  }

  if( x > x2 ){
     f2 = freq;
     x2 = x;
  }
}



/////////////////////////////////////////
////     convert signal to frequency  ///
/////////////////////////////////////////
pcrfreq_t SignalRuler::toFreq( int x )
{
  double freq;

  if(f1 == 0 || f2==0) return 0;
  if(x2 == x1) return 0;

  freq  = ((double)f2-(double)f1)*x/(x2-x1) +
             ((double)f1*x2 - (double)f2*x1)/(x2-x1);

  return (pcrfreq_t) freq;
}



/////////////////////////////
///    reset ruler        ///
/////////////////////////////
void SignalRuler::reset()
{
  f1 = 0;
  f2 = 0;
  x1 = 0;
  x2 = 0;
}





//////////////////////////////////////////////////////////////////////
//////////////// CLASS ScanScreen                             ////////
//////////////////////////////////////////////////////////////////////
ScanScreen::ScanScreen( QCanvas * viewing, QWidget * parent, const char * name, WFlags f)
  : QCanvasView( viewing, parent, name, f)
{
    markmode = false;
    setFrameStyle( NoFrame );
    clear();
}


///////////////////////////////////
//// geometry control           ///
///////////////////////////////////
QSize ScanScreen::minimumSizeHint () const { return QSize(550,315); }
QSize ScanScreen::sizeHint () const { return QSize(550, 315); }


/////////////////////////////
/// clear memory and image //
/// on screen              //
/////////////////////////////
void ScanScreen::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
    if ( *it )
        delete *it;
    }
}


//////////////////////////////////
//  set screen in marking mode ///
//////////////////////////////////
void ScanScreen::setMarking( bool flag)
{
  markmode = flag;
}


////////////////////////////////////
///  click to set frequency       //
////////////////////////////////////
void ScanScreen::contentsMousePressEvent ( QMouseEvent * e )
{
   pcrfreq_t freq;
   QString str;
   bool ok;

   if(markmode){

     // check if marker already exists
     if( SignalRuler::toFreq( e->x() )){
       switch( QMessageBox::warning(topLevelWidget(), "QtPCR", 
				    "Frequency markers already exist.\n"
				    "Do you really want to change them?",
				    "No", "Yes") ){
       case 0: // cancel
         return;
	 break;
       } 
     }
     
     str = QInputDialog::getText( 0, "Please enter marker frequency (Hz)");
     if(str.isEmpty()) return; 
     
     freq = str.toULong(&ok);
     if(!ok){
       QMessageBox::warning(this, "QtPCR","Cannot determine frequency");
       return;
     }
     
     if(freq < FREQ_MIN || freq > FREQ_MAX){
       QMessageBox::warning(this, "QtPCR","Frequency out of range");
       return;
     }
     
     SignalRuler::update( freq, e->x());
     
     return;
   }
   
   freq = SignalRuler::toFreq( e->x() );
   if(!freq){
     QMessageBox::warning(0, "QtPCR", "No ruler frequency found.\n\n"
			  "QtPCR needs at least two marking frequency.\n"
			  "To mark frequency, toggle Mark button and click\n"
			  "the red marker.");
     return;
   }
   emit setFreq( freq );
   
}



//////////////////////////////////////////////////////////////
/////                CLASS ScanDialog                     ////
//////////////////////////////////////////////////////////////


////////////////////////////
//// constructor         ///
////////////////////////////
ScanDialog::ScanDialog( QWidget * parent, const char * name )
  : QDialog( parent, name )
{
  QGroupBox *GBoxPtr;
  QGridLayout *MainGrid;
  
  // create main grid 
  MainGrid = new QGridLayout( this, 2, 2 );
  GBoxPtr  = new QGroupBox( 9, Qt::Vertical, "Scan", this );

  // create buttons and edit box
  start = new QPushButton("Start", GBoxPtr );
  pause = new QPushButton("Pause", GBoxPtr );
  stop  = new QPushButton("Stop",  GBoxPtr );
  alarm = new QPushButton("Alarm", GBoxPtr );
  loop  = new QPushButton("Loop",  GBoxPtr );
  lock  = new QPushButton("Lock",  GBoxPtr );
  mark  = new QPushButton("Mark",  GBoxPtr ); 
  load  = new QPushButton("Load",  GBoxPtr );
  save  = new QPushButton("Save",  GBoxPtr );

  alarmCmd = "echo -e \\a";

  loop->setToggleButton(true);
  mark->setToggleButton(true);
  lock->setToggleButton(true);
  pause->setToggleButton(true);

  alarm->setFocusPolicy( NoFocus );
  loop->setFocusPolicy( NoFocus );
  mark->setFocusPolicy( NoFocus );
  lock->setFocusPolicy( NoFocus );
  load->setFocusPolicy( NoFocus );
  save->setFocusPolicy( NoFocus );

  connect( alarm, SIGNAL(clicked()),
           this, SLOT(AlarmSlot()));

  connect( lock, SIGNAL(toggled(bool)),
           this, SLOT(LockSlot(bool)));

  connect( start, SIGNAL(clicked()),
           this, SLOT(StartSlot()));

  connect( stop, SIGNAL(clicked()),
           this, SLOT(StopSlot()));

  connect( pause, SIGNAL(toggled(bool)),
           this, SLOT(PauseSlot(bool)));

  connect( load, SIGNAL(clicked()),
           this, SLOT(LoadSlot()));

  connect( save, SIGNAL(clicked()),
           this, SLOT(SaveSlot()));
  

  stop->setEnabled( false );
  pause->setEnabled( false );

  stop->setFocusPolicy( NoFocus );
  start->setFocusPolicy( NoFocus );
  pause->setFocusPolicy( NoFocus );

  // create Canvas
  canvas = new QCanvas(600, CANVAS_HEIGHT);
  canvas->setBackgroundColor( Qt::black );
  screen = new ScanScreen( canvas, this );

  MainGrid->addWidget( screen, 0, 0);
  MainGrid->setColStretch( 0, 10 );
  MainGrid->setRowStretch( 1, 10 );
  MainGrid->addWidget( GBoxPtr, 0, 1);  

  // add options operation
  GBoxPtr = new QGroupBox(2, Qt::Vertical, "Options", this );
  new QLabel( "Start Freq", GBoxPtr );
  startFreqEdit  = new QLineEdit( GBoxPtr );
  new QLabel( "Stop Freq", GBoxPtr );
  stopFreqEdit   = new QLineEdit( GBoxPtr );

  startFreqEdit->setMaxLength(10);
  startFreqEdit->setAlignment( Qt::AlignRight);

  stopFreqEdit->setMaxLength(10);
  stopFreqEdit->setAlignment( Qt::AlignRight);

  new QLabel("Freq Step", GBoxPtr);
  freqStepEdit = new QLineEdit( GBoxPtr );
  new QLabel("Time Step (msec)", GBoxPtr );
  timeStepEdit = new QLineEdit( GBoxPtr );

  freqStepEdit->setMaxLength(10);
  freqStepEdit->setAlignment( Qt::AlignRight);
  timeStepEdit->setMaxLength(3);
  timeStepEdit->setAlignment( Qt::AlignRight);

  new QLabel("Algorithm", GBoxPtr);
  algorithm = new QComboBox( GBoxPtr);

  algorithm->setFocusPolicy( NoFocus );
  algorithm->insertItem("Fix Step");
  algorithm->insertItem("Driven");

  new QLabel("Scale", GBoxPtr );
  scale = new QComboBox( GBoxPtr);

  scale->setFocusPolicy( NoFocus ); 
  scale->insertItem( "Linear" );
  scale->insertItem( "Log");

  MainGrid->addWidget( GBoxPtr, 1, 0);

  scan = new Scan();

  connect( screen, SIGNAL(setFreq(pcrfreq_t)),
           this,   SIGNAL(setFreq(pcrfreq_t)));

  connect( mark, SIGNAL(toggled(bool)),
           screen, SLOT(setMarking(bool)));

  connect( scan, SIGNAL(scanUpdate(pcrfreq_t, unsigned char)),
           this, SLOT(scanUpdate(pcrfreq_t, unsigned char)));

  connect( scan, SIGNAL(scanBegun()),
           this, SLOT(scanBegun()));

  connect( scan, SIGNAL(scanEnded()),
           this, SLOT(scanEnded()));

  connect( scan, SIGNAL(setFreq(pcrfreq_t)),
           this, SIGNAL(setFreq(pcrfreq_t)));

  logScale = false;

  // add tool tips
  QToolTip::add(start,"Start scanning");
  QToolTip::add(stop,"Stop scanning");
  QToolTip::add(pause,"Pause scanning");
  QToolTip::add(loop,"Scan in loop");
  QToolTip::add(lock,"Lock radio control");
  QToolTip::add(mark,"Set frequency marker");
  QToolTip::add(load,"Load bitmap file");
  QToolTip::add(save,"Save screen in PNG format");

  // set enable
  pause->setEnabled( false );
  stop->setEnabled( false );
  mark->setEnabled( false );
  save->setEnabled( false );
}


///////////////////////////
//  clean up mess       ///
///////////////////////////
ScanDialog::~ScanDialog(){
  screen->clear();
  delete screen;
  delete canvas;
  delete scan;
  delete pause;
  delete start;
  delete stop;
}


//////////////////////////////////
/// radio status update handler //
//////////////////////////////////
void ScanDialog::sigStrengthUpdate( unsigned char sig)
{
  scan->sigStrengthUpdate( sig);
}

void ScanDialog::squelchOnUpdate( bool flag)
{
  scan->squelchOnUpdate( flag );
}


/////////////////////////////////////
///     Button handlers           ///
/////////////////////////////////////
void ScanDialog::LockSlot(bool flag)
{
     emit EnableControl( ! flag  );
}


void ScanDialog::executeCmd( QString cmd )
{
   char buf[1024];
   char *ch=buf;
   char *argv[128];
   int  n=0;

   bzero(buf, 1024);
   bzero(argv, 128*sizeof(char *));
   cmd.truncate( 1023 );
   strncpy( buf, (const char *)cmd, 1023 );

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
      // 
   }else{   // parent error
      perror("Can not fork child to execute alarm command");
   }

}

void ScanDialog::AlarmSlot()
{
   alarmCmd = QInputDialog::getText("QtPCR", 
                                    "Please enter alarm command (e.g. play sound.wav)"
                                    ); 

   // beep by default
   if(alarmCmd.isNull()){
      alarmCmd = "echo -e \\a";
   }
}

void ScanDialog::StartSlot()
{
   bool ok;

   if(scale->currentText().contains("Log")) 
     logScale = true;
   else
     logScale = false;
   
   if(algorithm->currentText().contains("Fix"))
     scan->SetAlgorithm( Scan::FixStep );
   else
     scan->SetAlgorithm( Scan::ChangeDriven );
   

   freqStep  = freqStepEdit->text().toULong(&ok);
   if(!ok){
     QMessageBox::warning(this,"QtPCR","Cannot determine frequency step");
     return;
   }
   
   if(freqStep < FREQ_MIN || freqStep > FREQ_MAX){
     QMessageBox::warning(this, "QtPCR","Frequency step out of range");
     return;
   }
   
   
   timeStep  = timeStepEdit->text().toUInt(&ok);
   if(!ok){
     QMessageBox::warning(this,"QtPCR","Cannot determine time step");
     return;
   }
   
   if(timeStep < 20 || timeStep > 1000){
     QMessageBox::warning(this, "QtPCR","Time step out of range");
     return;
   }
   
   
   endFreq  = stopFreqEdit->text().toULong(&ok);
   if(!ok){
     QMessageBox::warning(this,"QtPCR","Cannot determine ending frequency");
     return;
   }
   
   beginFreq = startFreqEdit->text().toULong(&ok);
   if(!ok){
     QMessageBox::warning(this, "QtPCR","Cannot determine starting frequency");
     return;
   }
   
   if(beginFreq < FREQ_MIN || beginFreq > FREQ_MAX){
     QMessageBox::warning(this, "QtPCR","Starting frequency out of range");
     return;
   }
   
   if(endFreq < FREQ_MIN || endFreq > FREQ_MAX){
     QMessageBox::warning(this, "QtPCR","Ending frequency out of range");
     return;
   }
   
   if(beginFreq > endFreq ){
     QMessageBox::warning(this, "QtPCR","Begin frequency cannot be larger than ending frequency");
     return;
   }
   
   freq = beginFreq;
   
   scan->SetFrequency(beginFreq, endFreq, freqStep);
   scan->SetTimeDelay(timeStep);
   
   screen->clear();
   canvas->resize(600, CANVAS_HEIGHT);
   canvas->setBackgroundPixmap( NULL );
   x = 50;
   ruler.reset();
   scan->Start();
   
   // draw horizontal line
   QCanvasLine *l = new QCanvasLine(canvas);
   l->setPoints( 0, CANVAS_ZEROAXIS, canvas->width(), CANVAS_ZEROAXIS);
   l->setZ(+10);
   l->setPen( QPen( Qt::red, 1) );
   l->show();
  

   // set enable
   start->setEnabled( false );
   mark->setEnabled( false );
   load->setEnabled( false );
   save->setEnabled( false );
   startFreqEdit->setEnabled( false );
   stopFreqEdit->setEnabled( false );
   freqStepEdit->setEnabled( false );
   timeStepEdit->setEnabled( false );
   algorithm->setEnabled( false );
   scale->setEnabled( false );

   stop->setEnabled( true );
   pause->setEnabled( true ); 
}

void ScanDialog::StopScan()
{
  stopFreqEdit->setEnabled( true );
  startFreqEdit->setEnabled( true );
  freqStepEdit->setEnabled( true );
  timeStepEdit->setEnabled( true );
  algorithm->setEnabled( true );
  scale->setEnabled( true );
  start->setEnabled( true );
  mark->setEnabled( true );
  load->setEnabled( true );
  save->setEnabled( true );

  stop->setEnabled( false );
  pause->setEnabled( false );
  
}

void ScanDialog::StopSlot()
{
   scan->Stop();
   StopScan();
}

void ScanDialog::PauseSlot(bool flag)
{
  start->setEnabled( !flag );
  stop->setEnabled( !flag );
  load->setEnabled( !flag );
  save->setEnabled( !flag );

  scan->Pause( flag );
}

void ScanDialog::LoadSlot()
{
  QString fname;
  
  fname =  QFileDialog::getOpenFileName();
  if(fname.isEmpty()) return;
  
  QPixmap pix;
  if(!pix.load(fname)){
    QMessageBox::warning(0, "QtPCR","QtPCR could not load image");
    return;
  }
 
  mark->setEnabled( true ); 
  ruler.reset();
  screen->clear(); 
  canvas->resize(pix.width(), pix.height()); 
  canvas->setBackgroundPixmap( pix );
  canvas->update();
}


void ScanDialog::SaveSlot()
{
  QString fname;

  fname = QFileDialog::getSaveFileName();
  if(fname.isEmpty()) return;
  
  QPixmap pix(canvas->width(), canvas->height());
  QPainter p( &pix );
  canvas->drawArea( QRect(0,0,canvas->width(),canvas->height()), &p );
  pix.save(fname,"PNG",-1);
}


/////////////////////////////////////
/// Slot handlers from Object Scan //
/////////////////////////////////////
void ScanDialog::scanBegun(){}
void ScanDialog::scanEnded(){

  // check if looping scan
   if(loop->isOn()){
     
    // save canvas
     QPixmap pix( canvas->width(), canvas->height() );
     QPainter p( &pix );
     canvas->update();
     canvas->drawArea( QRect(0,0,canvas->width(),canvas->height()), &p );
     canvas->setBackgroundPixmap( pix );
     canvas->update();
     
    // clear memory
     screen->clear();
     x=50;
     
     // draw horizontal line
     QCanvasLine *l = new QCanvasLine(canvas);
     l->setPoints( 0, CANVAS_ZEROAXIS, canvas->width(), CANVAS_ZEROAXIS);
     l->setZ(+10);
     l->setPen( QPen( Qt::red, 1) );
     l->show();
     
     scan->Start(); 
     
   }else{

     // execute alarm command
     executeCmd( alarmCmd );
     StopScan();
   }
}


void ScanDialog::scanUpdate( pcrfreq_t freq, unsigned char sig)
{
  SignalLine *i; 

  // scale
  if(logScale){
    sig = (unsigned char)(46.0*log(sig+1.0));
  }

  // mark some frequency

  ruler.update( freq, x );
  if(x%100 == 50){
    SignalMarker *m = new SignalMarker( canvas );
    m->setPoints( x, CANVAS_ZEROAXIS, x, CANVAS_ZEROAXIS-260 );
    m->setFrequency(freq);
    m->setPen( QPen(Qt::red,1,Qt::DotLine) );
    m->setColor( Qt::red );
    m->show();
  }
  

  // low signal is a waste of memory
  if(sig > 2){
    i = new SignalLine(canvas);
    i->setPoints(x, CANVAS_ZEROAXIS, x, CANVAS_ZEROAXIS-sig);
    i->setPen( QPen( QColor(0,sig,0), 1) );
    i->show();
    i->setFrequency( freq );
    i->setSig( sig );
  }
  x++;
  this->freq = freq;
  
  // expand canvas as nesscessary
  if(x > canvas->width()){
    
    // draw horizontal line
    QCanvasLine *l = new QCanvasLine(canvas);
    l->setPoints( canvas->width(), CANVAS_ZEROAXIS, canvas->width()+600, CANVAS_ZEROAXIS);
    l->setZ(+10);
    l->setPen( QPen( Qt::red, 1) );
    
    canvas->resize(canvas->width()+600, canvas->height());
    l->show();
  }
  
  canvas->update();
}
