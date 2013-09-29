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

#include <qapplication.h>
#include <qpixmap.h>
#include <qpalette.h>
#include <qmessagebox.h>

#include "qtpcr.h"
#include "qtpcr.xpm"

QtPcr::QtPcr(QWidget *parent, const char *name)
	: QWidget( parent, name )
{
  ///////////////////////////////////
  // Initialize control 
  ///////////////////////////////////
  pcr 		      = new PcrProxy();
  bmtree          = new BookmarkTree( 0 );
  scanDialog      = new ScanDialog( 0 );
  barGraph        = new BarGraph( this );
  freqDisplay     = new FreqDisplay( this );
  keyPad          = new KeyPad( this );
  modeCtrl        = new ModeCtrl( this );
  modeDisplay     = new ModeDisplay( this );
  fltDisplay      = new FilterDisplay( this );
  fltCtrl         = new FilterCtrl( this );
  dialFreq        = new Dial( this );
  vol             = new VolCtrl( this, "volume");
  squ             = new SquCtrl( this, "squelch");
  meter           = new Meter( this );
  channelButton   = new QPushButton( "Channel", this );
  scanButton      = new QPushButton( "Scan", this );
  aliasDisplay    = new AliasDisplay( this );
  infoDisplay     = new InfoDisplay( this );
  infoEdit        = new QMultiLineEdit( this );
  power 	      = new QPushButton( "POWER", this );
  channelDialog   = new QTabDialog( 0 );
  monitorDialog   = new MonitorDialog( 0 );

  //////////////////////////////////////
  // Initialize radio and serial port //
  //////////////////////////////////////
  power->setToggleButton( true );
  power->setFocusPolicy( NoFocus );

  EnableControl( false );
  EnableDisplay( false );
  
  ///////////////////////////////
  // connect signals and slots //
  ///////////////////////////////

  //// power on/off button ////
  connect(power, SIGNAL(toggled( bool )), this, SLOT(powerOn( bool )) );
 
  connect( pcr, SIGNAL(onUpdate( bool )), this, SLOT(RadioOn( bool)));

 
  ///// bargraph //////

  connect(pcr, SIGNAL(bandScopeUpdate(const unsigned char *, unsigned char)),
	  barGraph, SLOT(setData(const unsigned char *, unsigned char)) );
  
  connect(barGraph, SIGNAL(stepFreq(int)),
	  pcr, SLOT(stepFreq(int)) );

  connect(pcr, SIGNAL(scopeOnUpdate(bool)), 
	  barGraph, SLOT(scopeOnUpdate(bool)));


  ///// frequency display and keypad /////

  connect(pcr, SIGNAL(freqUpdate( pcrfreq_t )),
	  freqDisplay, SLOT(displayFreq( pcrfreq_t)) );
  
  connect(keyPad, SIGNAL(displayTemp( const QString *)),
	  freqDisplay, SLOT(displayTemp( const QString *)) );
  
  connect(keyPad, SIGNAL(displayReset()),
	  freqDisplay, SLOT(displayReset()) );
  
  connect(keyPad, SIGNAL(setFreq( pcrfreq_t)),
	  pcr, SLOT(setFreq( pcrfreq_t)) );

  
  ///// mode controls ///////

  connect(modeCtrl, SIGNAL(setCTCSS(unsigned char)),
	  pcr, SLOT(setCTCSS(unsigned char)));

  connect(pcr, SIGNAL(ctcssUpdate(unsigned char)),
	  modeCtrl, SLOT(ctcssUpdate(unsigned char)));

  connect(modeCtrl, SIGNAL(setMode( modulate_t )),
	  pcr, SLOT(setMode( modulate_t )) );

  connect(pcr, SIGNAL(modeUpdate( modulate_t )),
	  modeCtrl, SLOT( modeUpdate( modulate_t )) );

  connect(modeCtrl, SIGNAL(setAFC( bool )),
	  pcr, SLOT(setAFC( bool )) );

  connect(pcr, SIGNAL(afcUpdate( bool )),
	  modeCtrl, SLOT(afcUpdate( bool )) );
  
  connect(modeCtrl, SIGNAL(setATT( bool )),
	  pcr, SLOT(setATT( bool )) );

  connect(pcr, SIGNAL(attUpdate( bool )),
	  modeCtrl, SLOT(attUpdate( bool )));

  connect(modeCtrl, SIGNAL(setNB( bool )),
	  pcr, SLOT(setNB( bool )));

  connect(pcr, SIGNAL(nbUpdate( bool )), 
	  modeCtrl, SLOT(nbUpdate( bool )) );

  connect(modeCtrl, SIGNAL(setAGC( bool )), 
	  pcr, SLOT(setAGC( bool )) );

  connect(pcr, SIGNAL(agcUpdate( bool )), 
	  modeCtrl, SLOT(agcUpdate( bool )));

  connect(modeCtrl, SIGNAL(setVSC(bool)), 
	  pcr, SLOT(setVSC(bool)));

  connect(pcr, SIGNAL(vscUpdate(bool)), 
	  modeCtrl, SLOT(vscUpdate(bool)));

  connect(modeCtrl, SIGNAL(scopeWide()), 
	  pcr, SLOT(scopeWide()));

  connect(modeCtrl, SIGNAL(scopeNarrow()), 
	  pcr, SLOT(scopeNarrow()));

  connect(modeCtrl, SIGNAL(scopeOn( bool )),
	  pcr, SLOT(scopeOn( bool )) );

  connect(pcr, SIGNAL(scopeOnUpdate(bool)), 
	  modeCtrl, SLOT(scopeOnUpdate(bool)));



  ////// mode display ///////

  connect(pcr, SIGNAL(modeUpdate(modulate_t)), 
	  modeDisplay, SLOT(modeUpdate(modulate_t)));

  connect(pcr, SIGNAL(agcUpdate(bool)), 
	  modeDisplay, SLOT(agcUpdate(bool)));

  connect(pcr, SIGNAL(vscUpdate(bool)), 
	  modeDisplay, SLOT(vscUpdate(bool)));

  connect(pcr, SIGNAL(nbUpdate(bool)), 
	  modeDisplay, SLOT(nbUpdate(bool)));

  connect(pcr, SIGNAL(attUpdate(bool)), 
	  modeDisplay, SLOT(attUpdate(bool)));

  connect(pcr, SIGNAL(afcUpdate(bool)), 
	  modeDisplay, SLOT(afcUpdate(bool)));
  

  /////// filter control and display //////

  connect(pcr, SIGNAL( ctcssUpdate( unsigned char)),
	  fltDisplay, SLOT(setCTCSS( unsigned char)));

  connect(pcr, SIGNAL( filterUpdate( filter_t )),
	  fltDisplay, SLOT(setFilter( filter_t)) );

  connect(pcr, SIGNAL( tsUpdate( pcrfreq_t )),
	  fltDisplay, SLOT(setTS( pcrfreq_t)) );

  connect(pcr, SIGNAL( ifShiftUpdate(ifshift_t)),
	  fltDisplay, SLOT(setIFShift(ifshift_t)) );

  connect(fltCtrl, SIGNAL( stepUpFilter()),
	  pcr, SLOT( stepUpFilter()) );
  
  connect(fltCtrl, SIGNAL( stepDownFilter()),
	  pcr, SLOT( stepDownFilter()) );
  
  connect(fltCtrl, SIGNAL( stepUpTS()),
	  pcr, SLOT( stepUpTS()) );
  
  connect(fltCtrl, SIGNAL( stepDownTS()),
	  pcr, SLOT( stepDownTS()) );
  
  connect(fltCtrl, SIGNAL( setIFShift(ifshift_t)),
	  pcr, SLOT( setIFShift(ifshift_t)) );
  
  connect(pcr, SIGNAL(ifShiftUpdate(ifshift_t)),
	  fltCtrl, SLOT( ifShiftUpdate(ifshift_t)) );


  
  ////////// sig stregnth meter  ///////////
  connect(pcr, SIGNAL(sigStrengthUpdate(unsigned char)),
	  meter, SLOT(setValue(unsigned char)) );
  
  connect(pcr, SIGNAL(squelchOnUpdate( bool )),
	  meter, SLOT(setSquelchOn( bool )) );


  
  //////////// dial frequency   /////////
  connect(dialFreq, SIGNAL( turnRight() ),
	  pcr, SLOT( stepUpFreq()) );
  
  connect(dialFreq, SIGNAL( turnLeft() ),
	  pcr, SLOT( stepDownFreq()) );
  

  ////////// volume and squelch control  ///////

  connect(vol, SIGNAL( setVol(vol_t)),
	  pcr, SLOT( setVol(vol_t)) );

  connect(pcr, SIGNAL( volUpdate(vol_t)),
	  vol, SLOT( volUpdate(vol_t)) );

  connect(squ, SIGNAL( setSquelch(squelch_t)),
	  pcr, SLOT( setSquelch(squelch_t)) );

  connect(pcr, SIGNAL( squelUpdate(squelch_t)),
	  squ, SLOT( squelUpdate(squelch_t)) );


  ///////// aliasdisplay  /////////

  connect( aliasDisplay, SIGNAL(setAlias(const char *)),
	   pcr, SLOT(setAlias(const char *)) );

  connect( pcr, SIGNAL(aliasUpdate(const char *)),
	   aliasDisplay, SLOT(aliasUpdate(const char *)) );


  ////////// infoedit  //////////

  connect( infoDisplay, SIGNAL(editingMode(bool)),
	   this, SLOT(editingModeSlot(bool)));

  connect( pcr, SIGNAL(infoUpdate(const char *)),
	   infoDisplay, SLOT(infoUpdate(const char *)));



  //////////// bookmark and channel button ////////

  connect(channelButton, SIGNAL(toggled(bool)),
	  this, SLOT(ChannelSlot(bool)) );

  connect(bmtree, SIGNAL(gotoBookmark(struct bookmark_t *)),
	  pcr, SLOT(gotoBookmark(struct bookmark_t *)) );

  connect(bmtree, SIGNAL(getBookmark(struct bookmark_t *)),
	  pcr,SLOT(getBookmark(struct bookmark_t *)));


  //////////// scan button and dialog  ////////////

  connect( scanButton, SIGNAL(toggled(bool)),
           this, SLOT(ScanSlot( bool )));

  connect( scanDialog, SIGNAL(setFreq( pcrfreq_t)),
           pcr, SLOT(setFreq( pcrfreq_t )));

  connect( pcr, SIGNAL(sigStrengthUpdate(unsigned char)),
           scanDialog, SLOT(sigStrengthUpdate(unsigned char)));

  connect( pcr, SIGNAL(squelchOnUpdate( bool )),
           scanDialog, SLOT(squelchOnUpdate( bool )));

  connect( scanDialog, SIGNAL(EnableControl(bool)),
           this, SLOT(EnableControl(bool)));


  //////////////  monitor  ///////////////

  connect( monitorDialog, SIGNAL(getBookmark(struct bookmark_t *)),
           pcr, SLOT(getBookmark(struct bookmark_t *)));

  connect( monitorDialog, SIGNAL(gotoBookmark(struct bookmark_t *)),
           pcr, SLOT(gotoBookmark(struct bookmark_t *)));

  connect( pcr, SIGNAL(sigStrengthUpdate(unsigned char)),
           monitorDialog, SLOT(sigStrengthUpdate(unsigned char)));

  connect( pcr, SIGNAL(squelchOnUpdate(bool)),
           monitorDialog, SLOT(squelchOnUpdate(bool)));

  connect( bmtree, SIGNAL(toMonitor(struct bookmark_t *)),
           monitorDialog, SLOT(toMonitorSlot(struct bookmark_t *)));


  ////////// set tabdialog  //////////////

  channelDialog->addTab( bmtree, "Channel");
  channelDialog->addTab( monitorDialog, "Monitor");
  channelDialog->setDefaultButton( QString::null );
  channelDialog->setOkButton( QString::null );

  // set autopile button styles
  channelButton->setToggleButton( true );
  channelButton->setFocusPolicy( NoFocus );


  // set scan buttons
  scanButton->setToggleButton( true );
  scanButton->setFocusPolicy( NoFocus );

  //////////////////
  // Set skin //////
  //////////////////

  // background
  setBackgroundPixmap( QPixmap((const char **)qtpcr_xpm ));
  resize( 600, 240 );
  channelDialog->resize( 600, 480 );

  // set black display background
  fltDisplay->setPalette( QPalette( Qt::green, Qt::black ) );
  bmtree->setPalette( QPalette( QColor(0x00,0x2c,0x59), Qt::black ));  
  scanDialog->setPalette( QPalette( QColor(0x00,0x2c,0x59), Qt::black));
  monitorDialog->setPalette( QPalette( QColor(0x00,0x2c,0x59), Qt::black));
  channelDialog->setPalette( QPalette( QColor(0x00,0x2c,0x59), Qt::black));

  QColorGroup cg(colorGroup()); 
  cg.setColor(QColorGroup::Text, Qt::yellow);
  cg.setColor(QColorGroup::Base, Qt::darkGreen);
  cg.setColor(QColorGroup::Background, Qt::darkGreen);
  infoEdit->setPalette( QPalette(cg, cg, cg) );


  // set layout
  power->setGeometry( 5, 5, 60, 30 );
  channelButton->setGeometry(5, 125, 55, 20);
  scanButton->setGeometry(60, 125, 55, 20);
  freqDisplay->setGeometry( 200, 15, 260, 60 );
  fltDisplay->setGeometry( 270, 80, 185, 30 );
  barGraph->setGeometry( 130, 119, 328, 64 );
  keyPad->setGeometry( 470, 10, 105, 80 );
  modeCtrl->setGeometry( 130, 195, 320, 40);
  modeDisplay->setGeometry( 130, 15, 90, 80 ); 
  fltCtrl->setGeometry( 470, 90, 105, 40 );
  dialFreq->setGeometry(475, 140, 90, 90);
  vol->setGeometry(15, 160, 40, 40);
  squ->setGeometry(70, 160, 40, 40);
  meter->setGeometry( 5, 50, 110, 70 );
  aliasDisplay->setGeometry( 205, 10, 250, 18);
  aliasDisplay->setAlignment(Qt::AlignRight);
  infoDisplay->setGeometry(130, 98, 130, 15);
  infoEdit->setGeometry(130, 119, 328, 64);

  // keypad has keyboard focus
  keyPad->setFocus();

  // fix size
  setFixedSize(width(),height());

  // initialize infoDisplay and Edit
  // infoEdit->setMaxLength( INFOSIZE );  Qt 3.0 doesn't have this
  infoDisplay->setRunning( true );  
  infoEdit->hide();

}




/////////////////////////////////////
///  Power Button handlers        ///
/////////////////////////////////////
void QtPcr::powerOn( bool flag)
{
  if(flag){
    if(!(pcr->InitRadio( "/dev/pcr1000" ))){
       QMessageBox::critical(this, "QtPCR",
                             "QtPCR cannot initialize PCR-1000 radio.\n"
                             "Please refer to FAQ.html. Exiting ...");
       close();
    }

  }else{
    switch( QMessageBox::warning(0, "QtPCR", "Main power operation",
                                 "Cancel", "Turn off Radio" , "Exit")){
    case 0:
       disconnect(power, SIGNAL(toggled( bool )), this, SLOT(powerOn( bool )) );
       power->setOn( true );
       connect(power, SIGNAL(toggled( bool )), this, SLOT(powerOn( bool )) );
    break;
    case 1:
       pcr->TurnOffRadio(); 
    break;
    case 2:
       close();
    break;
    }
  }
}



///////////////////////////////////////
////     Enter info editing mode  /////
///////////////////////////////////////
void QtPcr::editingModeSlot( bool flag)
{
  QString str;

  if( flag ){
    str = infoDisplay->text();
    infoEdit->setText(str);
    infoEdit->show();
  }else{
    infoEdit->hide();

    // set focus to keyPad by default
    keyPad->setFocus();
    str = infoEdit->text();
    pcr->setInfo( str );
  }
}



///////////////////////////////////////
//// Show channel dialog         //////
///////////////////////////////////////
void QtPcr::ChannelSlot( bool flag )
{
  if(flag){
    channelDialog->show();
  }else{
    channelDialog->hide();
  }
}



/////////////////////////////////
/////// Show Scan Dialog   //////
/////////////////////////////////
void QtPcr::ScanSlot( bool flag )
{
  if(flag){
    scanDialog->show();
  }else{
    scanDialog->hide();
  }
}


///////////////////////////////////
/////// Enable all control     ////
///////////////////////////////////
void QtPcr::EnableControl( bool flag )
{
   keyPad->setEnabled( flag );
   modeCtrl->setEnabled( flag );
   fltCtrl->setEnabled( flag );
   dialFreq->setEnabled( flag );
   vol->setEnabled( flag );
   squ->setEnabled( flag );
}



//////////////////////////////////
////// Enable all display  ///////
//////////////////////////////////
void QtPcr::EnableDisplay( bool flag )
{
   if( flag ){
      barGraph->show();
      freqDisplay->show();
      modeDisplay->show();
      fltDisplay->show();
      aliasDisplay->show();
      infoDisplay->show();
   }else{
      barGraph->hide();
      freqDisplay->hide();
      modeDisplay->hide();
      fltDisplay->hide();
      aliasDisplay->hide();
      infoDisplay->hide();
   }
}



/////////////////////
/// Clean up     ////
/////////////////////
QtPcr::~QtPcr()
{
  delete pcr;
  delete power;
  delete barGraph;
  delete freqDisplay;
  delete keyPad;
  delete bmtree;
  delete scanDialog;
}


// set focus to keyPad by default
void QtPcr::enterEvent ( QEvent *e )
{
  if(!infoEdit->isActiveWindow())
     keyPad->setFocus();
}


void QtPcr::RadioOn( bool on)
{
  if( on ){
    EnableControl( true );
    EnableDisplay( true );
  }else{
    EnableControl( false );
    EnableDisplay( false );
  }
} 
