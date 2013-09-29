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

#include <qfont.h>

#include "modectrl.h"

//////////////////////////////////
/////       Initializer  
//////////////////////////////////
ModeCtrl::ModeCtrl(QWidget *parent, const char *name)
	: QWidget( parent, name )
{
  // Qt 3.0 doesn't have below API
  // setFontPropagation( QWidget::AllChildren );
  
  // set SSB to LSB by default
  ssb_which = SSB_LSB;

  // alocate modulates buttons
  ssb = new QPushButton( "SSB" , this );
  cw  = new QPushButton( "CW",   this );
  wfm = new QPushButton( "WFM" , this );
  fm  = new QPushButton( "FM"  , this );
  am  = new QPushButton( "AM"  , this );
  
  ssb->setFocusPolicy( NoFocus );
  cw->setFocusPolicy( NoFocus );
  wfm->setFocusPolicy( NoFocus );
  fm->setFocusPolicy( NoFocus );
  am->setFocusPolicy( NoFocus );

  // connect clicked event to handlers
  connect( ssb, SIGNAL(clicked()), this, SLOT(ssbClicked()) );
  connect( cw,  SIGNAL(clicked()), this, SLOT(cwClicked())  );
  connect( wfm, SIGNAL(clicked()), this, SLOT(wfmClicked()) );
  connect( fm,  SIGNAL(clicked()), this, SLOT(fmClicked())  );
  connect( am,  SIGNAL(clicked()), this, SLOT(amClicked())  );
  
  // alocate special fucntion buttons
  vsc = new QPushButton( "VSC",  this );
  nb  = new QPushButton( "NB" ,  this );
  att = new QPushButton( "ATT",  this );
  agc = new QPushButton( "AGC",  this );
  afc = new QPushButton( "AFC",  this );

  vsc->setFocusPolicy( NoFocus );
  nb->setFocusPolicy( NoFocus );
  att->setFocusPolicy( NoFocus );
  agc->setFocusPolicy( NoFocus );
  afc->setFocusPolicy( NoFocus );

  // set them to toggle-type buttons
  att->setToggleButton( true );
  nb->setToggleButton( true );
  agc->setToggleButton( true );
  vsc->setToggleButton( true );
  afc->setToggleButton( true );

  // connect toggled event to handlers
  connect( att, SIGNAL( toggled( bool )), this, SIGNAL( setATT( bool )) );
  connect( agc, SIGNAL( toggled( bool )), this, SIGNAL( setAGC( bool )) );
  connect( nb,  SIGNAL( toggled( bool )), this, SIGNAL( setNB( bool ))  );
  connect( vsc, SIGNAL( toggled( bool )), this, SIGNAL( setVSC( bool )) );
  connect( afc, SIGNAL( toggled( bool )), this, SIGNAL( setAFC( bool )) );

  // alocate scope controls
  scopeO = new QPushButton("SCOPE", this );
  scopeW = new QPushButton("+", this );
  scopeN = new QPushButton("-", this );	

  scopeO->setFocusPolicy( NoFocus );
  scopeW->setFocusPolicy( NoFocus );
  scopeN->setFocusPolicy( NoFocus );

  // set on-off scope to toggle-type button
  scopeO->setToggleButton( true );
  
  // connected toggles and clicked event to handlers
  connect(scopeO, SIGNAL(toggled(bool)), this, SIGNAL( scopeOn(bool)));
  connect(scopeW, SIGNAL(clicked()), this, SIGNAL( scopeWide()) );
  connect(scopeN, SIGNAL(clicked()), this, SIGNAL( scopeNarrow()) );

  // alocate ctcss controls
  ctcssButton = new QPushButton( "CTCSS", this );
  ctcssStepUp = new QPushButton( "+", this );
  ctcssStepDn = new QPushButton( "-", this );

  ctcssButton->setFocusPolicy( NoFocus );
  ctcssStepUp->setFocusPolicy( NoFocus );
  ctcssStepDn->setFocusPolicy( NoFocus );

  // set ctcss to toggle
  ctcssButton->setToggleButton( true );
  ctcssStepUp->setAutoRepeat( true );
  ctcssStepDn->setAutoRepeat( true );

  ctcssStepUp->setEnabled( false );
  ctcssStepDn->setEnabled( false );

  connect(ctcssButton, SIGNAL(toggled(bool)),
	  this, SLOT(ctcssToggled(bool)));

  connect(ctcssStepUp, SIGNAL(clicked()),
	  this, SLOT(ctcssToneUp()));

  connect(ctcssStepDn, SIGNAL(clicked()),
	  this, SLOT(ctcssToneDn()));
}

///////////////////////////////////
////    Set Layout 
///////////////////////////////////
void ModeCtrl::resizeEvent( QResizeEvent *e)
{
  int hw; // half button width
  int w, h;
  
  
  hw = e->size().width()/16;
  w  = 2*hw;
  h  = e->size().height()/2;
  
  ssb->setGeometry( 2*w, 0, w, h);
  cw->setGeometry(  4*w, 0, w, h);
  wfm->setGeometry( 4*w, h, w, h);
  fm->setGeometry(3*w, 0, w, h);
  am->setGeometry(3*w, h, w, h);
  
  vsc->setGeometry( 0, 0, w, h);
  nb->setGeometry(  2*w, h, w, h);
  att->setGeometry( w, 0, w, h);
  agc->setGeometry( w, h, w, h);
  afc->setGeometry( 0, h, w, h);

  scopeO->setGeometry(5*w,0, 3*hw, h);
  scopeN->setGeometry(5*w, h, (int)(hw*1.5), h);
  scopeW->setGeometry((int)(5.0*w+hw*1.5), h, (int)(hw*1.5), h);

  ctcssButton->setGeometry(13*hw,0,3*hw, h);
  ctcssStepDn->setGeometry(13*hw,h,(int)(hw*1.5),h);
  ctcssStepUp->setGeometry((int)(14.5*hw),h,(int)(hw*1.5),h);
  
}


///////////////////////
//// Clean up 
///////////////////////
ModeCtrl::~ModeCtrl()
{
  delete ssb;
  delete cw;
  delete wfm;
  delete fm;
  delete am;
  
  delete vsc;
  delete nb;
  delete att;
  delete agc;
  delete afc;

  delete scopeO;
  delete scopeN;
  delete scopeW;
}

///////////////////////////////////////////
////     Handlers
///////////////////////////////////////////
void ModeCtrl::fmClicked() {  emit setMode( MODE_NFM ); }

void ModeCtrl::ssbClicked()
{
  ssb_which = !ssb_which;
  
  if(ssb_which == SSB_USB)
    emit setMode( MODE_USB );
  else
    emit setMode( MODE_LSB );
}

void ModeCtrl::cwClicked() { emit setMode( MODE_CW ); }
void ModeCtrl::wfmClicked() { emit setMode( MODE_WFM ); }
void ModeCtrl::amClicked() { emit setMode( MODE_AM ); }

void ModeCtrl::attUpdate( bool flag ){ att->setOn( flag ); }
void ModeCtrl::agcUpdate( bool flag ){ agc->setOn( flag ); }
void ModeCtrl::nbUpdate( bool flag ){ nb->setOn( flag ); }
void ModeCtrl::vscUpdate( bool flag){ vsc->setOn( flag ); }
void ModeCtrl::afcUpdate( bool flag){ afc->setOn( flag ); }
void ModeCtrl::modeUpdate( modulate_t mode)
{
  if(mode == MODE_USB) ssb_which = SSB_USB;
  if(mode == MODE_LSB) ssb_which = SSB_LSB;
}
void ModeCtrl::scopeOnUpdate( bool flag){scopeO->setOn( flag );}
void ModeCtrl::ctcssUpdate( unsigned char tone)
{ 
  ctcssButton->setOn( tone ); 
  ctcss = tone;
}
void ModeCtrl::ctcssToggled( bool flag )
{  
  if(flag){
    emit setCTCSS(0xFF); 
    ctcssStepUp->setEnabled( true );
    ctcssStepDn->setEnabled( true );
  }
  else{
    emit setCTCSS(0x00);
    ctcssStepUp->setEnabled( false );
    ctcssStepDn->setEnabled( false );
  }
}
void ModeCtrl::ctcssToneUp()
{
  if(ctcss < 0x33) 
    ctcss++;

  emit setCTCSS( ctcss );
}

void ModeCtrl::ctcssToneDn()
{
  if(ctcss > 1)
    ctcss--;

  emit setCTCSS( ctcss );
}


