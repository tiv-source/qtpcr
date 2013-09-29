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

#ifndef SCAN_DIALOG_H_
#define SCAN_DIALOG_H_

////////////////////////////////////
///  Scanner Dialog             ////
////////////////////////////////////
#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcanvas.h>
#include "scan.h"
#include "pcrproto.h"

///////////////////////////////////////
/// rtti - identify types of objects //
/// in our canvas                    //
///////////////////////////////////////
#define SignalLineID 1234
#define SignalMarkerID 1235




/////////////////////////////////////
//// Signal plot on your canvas /////
/////////////////////////////////////
class SignalLine : public QCanvasLine
{
public:
  SignalLine( QCanvas * canvas );

  pcrfreq_t getFrequency();
  unsigned char getSig();
  void setFrequency(pcrfreq_t);
  void setSig(unsigned char);
  int rtti() const;

protected:
  pcrfreq_t freq;
  unsigned char sig;
};



////////////////////////////////////////
///  Signal Marker showing frequency ///
///  on canvas                       ///
////////////////////////////////////////
class SignalMarker : public SignalLine 
{
public:
  SignalMarker( QCanvas * canvas ); 
  int rtti() const;

  void setPoints(int , int , int , int);
  void setFrequency( pcrfreq_t );
  void show();
  void setFont ( const QFont & );
  void setColor ( const QColor & );
 
private:
  QCanvasText *freqText;
  void setFreqText(int);
};




/////////////////////////////////////
/// Signal Ruler converts          //
/// pixel on canvas to frequency   //
/////////////////////////////////////
class SignalRuler
{
public:
  SignalRuler();
  static void update(pcrfreq_t f, int x);
  static void reset();
  static pcrfreq_t toFreq( int x );

private:
  static pcrfreq_t f1;
  static pcrfreq_t f2;
  static int       x1;
  static int       x2;
};




///////////////////////////////////////
////    Canvas view                ////
///////////////////////////////////////
class ScanScreen : public QCanvasView
{
  Q_OBJECT
public:
  ScanScreen( QCanvas * viewing=0, QWidget * parent=0, const char * name=0, WFlags f=0);
  void clear();
  QSize minimumSizeHint () const;
  QSize sizeHint () const;

signals:
  void setFreq( pcrfreq_t );

public slots:
  void setMarking(bool);

protected:
  void contentsMousePressEvent ( QMouseEvent * );

private:
  bool markmode;
};




/////////////////////////////////////
///       Main Dialog            ////
/////////////////////////////////////
class ScanDialog : public QDialog
{
  Q_OBJECT
public:
  ScanDialog( QWidget * parent=0, const char * name=0);
  ~ScanDialog();

// must be connected to PcrProxy
signals:
  void setFreq( pcrfreq_t );
  void EnableControl ( bool );

public slots:
  void sigStrengthUpdate( unsigned char );
  void squelchOnUpdate( bool );

private slots:
  void StartSlot();
  void StopSlot();
  void PauseSlot(bool);
  void LoadSlot();
  void SaveSlot();
  void LockSlot(bool);
  void AlarmSlot();

  void scanBegun();
  void scanUpdate( pcrfreq_t, unsigned char );
  void scanEnded();
  void executeCmd( QString cmd );
 
private:
  QString     alarmCmd;
  Scan        *scan;
  QPushButton *start;
  QPushButton *stop;
  QPushButton *pause;
  QPushButton *alarm;
  QPushButton *loop;
  QPushButton *lock;
  QPushButton *mark;
  QPushButton *load;
  QPushButton *save;
  QLineEdit   *startFreqEdit;
  QLineEdit   *stopFreqEdit;
  QLineEdit   *freqStepEdit;
  QLineEdit   *timeStepEdit;
  QComboBox   *algorithm;
  QComboBox   *scale;
  ScanScreen  *screen; 
  QCanvas     *canvas;
  int         x, timeStep;
  pcrfreq_t   freq, beginFreq, endFreq, freqStep;
  SignalRuler ruler;
  bool        logScale;

  void StopScan();
};
#endif // SCAN_DIALOG_H_
