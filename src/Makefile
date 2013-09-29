### Slackware 7.0 
#INCPATH = -I/usr/lib/qt/include
#LIBS = -lqt -L/usr/lib
#MOC = /usr/lib/qt/bin/moc 

### FreeBSD 4.3  
#INCPATH = -I/usr/X11R6/include/qt2
#LIBS = -lqt2 -L/usr/X11R6/lib
#MOC = moc2

### Mandrake 8.0
#INCPATH = -I/usr/lib/qt2/include
#LIBS = -lqt -L/usr/lib
#MOC = /usr/lib/qt2/bin/moc 


##############################################
#### DONT  DONT  DONT  DONT   EDIT BELOW #####
##############################################

CXX = g++
CXXFLAGS = -O2 -Wall 

OBJ_GEN = 	pcrio.o \
			monitor.o \
			scan.o \
			scandialog.o \
			pcrproxy.o \
			bargraph.o \
			freqdisplay.o \
			keypad.o \
			modectrl.o \
			filterdisplay.o \
			aliasdisplay.o \
			filterctrl.o \
			dial.o \
			volctrl.o \
			squctrl.o \
			bmtree.o \
			bmdata.o \
			modedisplay.o \
			meter.o \
			qtpcr.o \
			main.o 

OBJ_MOC =	moc_pcrio.o \
			moc_monitor.o \
			moc_scan.o \
			moc_scandialog.o \
			moc_pcrproxy.o \
			moc_bargraph.o \
			moc_freqdisplay.o \
			moc_keypad.o \
			moc_modectrl.o \
			moc_filterdisplay.o \
			moc_aliasdisplay.o \
			moc_filterctrl.o \
			moc_dial.o \
			moc_bmtree.o \
			moc_volctrl.o \
			moc_squctrl.o \
			moc_meter.o \
			moc_modedisplay.o \
			moc_qtpcr.o 

OBJ = $(OBJ_GEN) $(OBJ_MOC)

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

qtpcr: $(OBJ)
	g++ $(LIBS) -lm $(OBJ)  -o qtpcr 

clean:
	rm -f *.o
	rm -f moc_*.cpp

pcrio.o: pcrio.h pcrio.cpp

monitor.o: monitor.h pcrproto.h monitor.cpp

scan.o: scan.h pcrproto.h scan.cpp

scandialog.o: scandialog.h scan.h pcrproto.h scandialog.cpp

pcrproxy.o: pcrproto.h tips.h pcrproxy.h pcrio.h pcrproxy.cpp

bargraph.o: bargraph.h bargraph.cpp 

freqdisplay.o: pcrproto.h freqdisplay.h freqdisplay.cpp

keypad.o: pcrproto.h keypad.h keypad.cpp

modectrl.o: pcrproto.h modectrl.h modectrl.cpp

filterdisplay.o: pcrproto.h filterdisplay.h filterdisplay.cpp

aliasdisplay.o: pcrproto.h aliasdisplay.h aliasdisplay.cpp

filterctrl.o: pcrproto.h filterctrl.h filterctrl.cpp

dial.o: dial.h dial.cpp

bmtree.o: pcrproto.h bmdata.h bmtree.h bmtree.cpp

bmdata.o: bmdata.h pcrproto.h bmdata.cpp

volctrl.o: volctrl.h dial.h pcrproto.h volctrl.cpp

squctrl.o: pcrproto.h squctrl.h squctrl.cpp

modedisplay.o: modedisplay.h pcrproto.h modedisplay.cpp

qtpcr.o: pcrio.h pcrproto.h qtpcr.h pcrproxy.h bargraph.h freqdisplay.h keypad.h \
	   	 modectrl.h modedisplay.h filterdisplay.h filterctrl.h dial.h \
	     bmtree.h volctrl.h squctrl.h aliasdisplay.h scandialog.h monitor.h

meter.o: meter.h meter.cpp

main.o: main.cpp qtpcr.h

moc_pcrio.cpp: pcrio.h
	$(MOC) pcrio.h -o moc_pcrio.cpp

moc_monitor.cpp: monitor.h
	$(MOC) monitor.h -o moc_monitor.cpp

moc_scan.cpp: scan.h
	$(MOC) scan.h -o moc_scan.cpp

moc_scandialog.cpp: scandialog.h
	$(MOC) scandialog.h -o moc_scandialog.cpp

moc_pcrproxy.cpp: pcrproxy.h pcrio.h
	$(MOC) pcrproxy.h -o moc_pcrproxy.cpp

moc_bargraph.cpp: bargraph.h
	$(MOC) bargraph.h -o moc_bargraph.cpp

moc_freqdisplay.cpp: freqdisplay.h
	$(MOC) freqdisplay.h -o moc_freqdisplay.cpp

moc_keypad.cpp: keypad.h
	$(MOC) keypad.h -o moc_keypad.cpp

moc_modectrl.cpp: modectrl.h
	$(MOC) modectrl.h -o moc_modectrl.cpp

moc_aliasdisplay.cpp: aliasdisplay.h
	$(MOC) aliasdisplay.h -o moc_aliasdisplay.cpp

moc_filterdisplay.cpp: filterdisplay.h
	$(MOC) filterdisplay.h -o moc_filterdisplay.cpp

moc_filterctrl.cpp: filterctrl.h
	$(MOC) filterctrl.h -o moc_filterctrl.cpp

moc_dial.cpp: dial.h
	$(MOC) dial.h -o moc_dial.cpp

moc_bmtree.cpp: bmdata.h bmtree.h
	$(MOC) bmtree.h -o moc_bmtree.cpp

moc_volctrl.cpp: volctrl.h
	$(MOC) volctrl.h -o moc_volctrl.cpp

moc_squctrl.cpp: squctrl.h
	$(MOC) squctrl.h -o moc_squctrl.cpp

moc_modedisplay.cpp: modedisplay.h
	$(MOC) modedisplay.h -o moc_modedisplay.cpp

moc_qtpcr.cpp:  pcrio.h qtpcr.h pcrproxy.h bargraph.h freqdisplay.h keypad.h \
				modectrl.h modedisplay.h filterdisplay.h filterctrl.h dial.h \
				meter.h bmtree.h aliasdisplay.h monitor.h volctrl.h squctrl.h \
				pcrproto.h  scandialog.h
	$(MOC) qtpcr.h -o moc_qtpcr.cpp

moc_meter.cpp: meter.h
	$(MOC) meter.h -o moc_meter.cpp
