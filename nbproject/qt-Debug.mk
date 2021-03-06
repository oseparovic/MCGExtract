#############################################################################
# Makefile for building: MCGExtract
# Generated by qmake (2.01a) (Qt 4.8.4) on: Wed Sep 11 20:25:22 2013
# Project:  nbproject/qt-Debug.pro
# Template: app
# Command: c:/Qt/4.8.4/bin/qmake.exe -spec c:/Qt/4.8.4/mkspecs/win32-g++ VPATH=. -o qttmp-Debug.mk nbproject/qt-Debug.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB -DQT_HAVE_MMX -DQT_HAVE_3DNOW -DQT_HAVE_SSE -DQT_HAVE_MMXEXT -DQT_HAVE_SSE2 -DQT_THREAD_SUPPORT -DQT_NEEDS_QMAIN
CFLAGS        = -pipe -g -Wall -Wextra $(DEFINES)
CXXFLAGS      = -pipe -g -frtti -fexceptions -mthreads -Wall -Wextra $(DEFINES)
INCPATH       = -I'c:/Qt/4.8.4/include/QtCore' -I'c:/Qt/4.8.4/include/QtGui' -I'c:/Qt/4.8.4/include' -I'c:/Qt/4.8.4/include/ActiveQt' -I'.' -I'.' -I'nbproject' -I'.' -I'c:/Qt/4.8.4/mkspecs/win32-g++'
LINK        =        g++
LFLAGS        =        -mthreads -Wl,-subsystem,windows
LIBS        =        -L'c:/Qt/4.8.4/lib' -lmingw32 -lqtmaind build/Debug/MinGW-Windows/MCGExtract_resource_res.o -lQtGuid4 -lQtCored4 
QMAKE         = c:/Qt/4.8.4/bin/qmake.exe
IDC           = c:/Qt/4.8.4/bin/idc.exe
IDL           = midl
ZIP           = zip -r -9
DEF_FILE      = 
RES_FILE      = build/Debug/MinGW-Windows/MCGExtract_resource_res.o
COPY          = cp
SED           = 
COPY_FILE     = $(COPY)
COPY_DIR      = cp -r
DEL_FILE      = rm
DEL_DIR       = rmdir
MOVE          = mv
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
INSTALL_FILE    = $(COPY_FILE)
INSTALL_PROGRAM = $(COPY_FILE)
INSTALL_DIR     = $(COPY_DIR)

####### Output directory

OBJECTS_DIR   = build/Debug/MinGW-Windows/

####### Files

SOURCES       = MainScreen.cpp \
		main.cpp moc_MainScreen.cpp
OBJECTS       = build/Debug/MinGW-Windows/MainScreen.o \
		build/Debug/MinGW-Windows/main.o \
		build/Debug/MinGW-Windows/moc_MainScreen.o
DIST          = 
QMAKE_TARGET  = MCGExtract
DESTDIR        = dist/Debug/MinGW-Windows/ #avoid trailing-slash linebreak
TARGET         = MCGExtract.exe
DESTDIR_TARGET = dist/Debug/MinGW-Windows/MCGExtract.exe

####### Implicit rules

.SUFFIXES: .cpp .cc .cxx .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

first: all
all: qttmp-Debug.mk  $(DESTDIR_TARGET)

$(DESTDIR_TARGET): ui_MainScreen.h $(OBJECTS) build/Debug/MinGW-Windows/MCGExtract_resource_res.o
	$(LINK) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS)  $(LIBS)

build/Debug/MinGW-Windows/MCGExtract_resource_res.o: MCGExtract_resource.rc
	windres -i MCGExtract_resource.rc -o build/Debug/MinGW-Windows/MCGExtract_resource_res.o --include-dir=. $(DEFINES)

qttmp-Debug.mk: nbproject/qt-Debug.pro  c:/Qt/4.8.4/mkspecs/win32-g++/qmake.conf c:/Qt/4.8.4/mkspecs/features/device_config.prf \
		c:/Qt/4.8.4/mkspecs/qconfig.pri \
		c:/Qt/4.8.4/mkspecs/modules/qt_webkit_version.pri \
		c:/Qt/4.8.4/mkspecs/features/qt_functions.prf \
		c:/Qt/4.8.4/mkspecs/features/qt_config.prf \
		c:/Qt/4.8.4/mkspecs/features/exclusive_builds.prf \
		c:/Qt/4.8.4/mkspecs/features/default_pre.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/default_pre.prf \
		c:/Qt/4.8.4/mkspecs/features/debug.prf \
		c:/Qt/4.8.4/mkspecs/features/default_post.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/default_post.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/rtti.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/exceptions.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/stl.prf \
		c:/Qt/4.8.4/mkspecs/features/shared.prf \
		c:/Qt/4.8.4/mkspecs/features/warn_on.prf \
		c:/Qt/4.8.4/mkspecs/features/qt.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/thread.prf \
		c:/Qt/4.8.4/mkspecs/features/moc.prf \
		c:/Qt/4.8.4/mkspecs/features/win32/windows.prf \
		c:/Qt/4.8.4/mkspecs/features/resources.prf \
		c:/Qt/4.8.4/mkspecs/features/uic.prf \
		c:/Qt/4.8.4/mkspecs/features/yacc.prf \
		c:/Qt/4.8.4/mkspecs/features/lex.prf \
		c:/Qt/4.8.4/mkspecs/features/include_source_dir.prf \
		c:/Qt/4.8.4/lib/qtmaind.prl
	$(QMAKE) -spec c:/Qt/4.8.4/mkspecs/win32-g++ VPATH=. -o qttmp-Debug.mk nbproject/qt-Debug.pro
c:/Qt/4.8.4/mkspecs/features/device_config.prf:
c:/Qt/4.8.4/mkspecs/qconfig.pri:
c:/Qt/4.8.4/mkspecs/modules/qt_webkit_version.pri:
c:/Qt/4.8.4/mkspecs/features/qt_functions.prf:
c:/Qt/4.8.4/mkspecs/features/qt_config.prf:
c:/Qt/4.8.4/mkspecs/features/exclusive_builds.prf:
c:/Qt/4.8.4/mkspecs/features/default_pre.prf:
c:/Qt/4.8.4/mkspecs/features/win32/default_pre.prf:
c:/Qt/4.8.4/mkspecs/features/debug.prf:
c:/Qt/4.8.4/mkspecs/features/default_post.prf:
c:/Qt/4.8.4/mkspecs/features/win32/default_post.prf:
c:/Qt/4.8.4/mkspecs/features/win32/rtti.prf:
c:/Qt/4.8.4/mkspecs/features/win32/exceptions.prf:
c:/Qt/4.8.4/mkspecs/features/win32/stl.prf:
c:/Qt/4.8.4/mkspecs/features/shared.prf:
c:/Qt/4.8.4/mkspecs/features/warn_on.prf:
c:/Qt/4.8.4/mkspecs/features/qt.prf:
c:/Qt/4.8.4/mkspecs/features/win32/thread.prf:
c:/Qt/4.8.4/mkspecs/features/moc.prf:
c:/Qt/4.8.4/mkspecs/features/win32/windows.prf:
c:/Qt/4.8.4/mkspecs/features/resources.prf:
c:/Qt/4.8.4/mkspecs/features/uic.prf:
c:/Qt/4.8.4/mkspecs/features/yacc.prf:
c:/Qt/4.8.4/mkspecs/features/lex.prf:
c:/Qt/4.8.4/mkspecs/features/include_source_dir.prf:
c:\Qt\4.8.4\lib\qtmaind.prl:
qmake:  FORCE
	@$(QMAKE) -spec c:/Qt/4.8.4/mkspecs/win32-g++ VPATH=. -o qttmp-Debug.mk nbproject/qt-Debug.pro

dist:
	$(ZIP) MCGExtract.zip $(SOURCES) $(DIST) nbproject/nbproject/qt-Debug.pro c:/Qt/4.8.4/mkspecs/features/device_config.prf c:/Qt/4.8.4/mkspecs/qconfig.pri c:/Qt/4.8.4/mkspecs/modules/qt_webkit_version.pri c:/Qt/4.8.4/mkspecs/features/qt_functions.prf c:/Qt/4.8.4/mkspecs/features/qt_config.prf c:/Qt/4.8.4/mkspecs/features/exclusive_builds.prf c:/Qt/4.8.4/mkspecs/features/default_pre.prf c:/Qt/4.8.4/mkspecs/features/win32/default_pre.prf c:/Qt/4.8.4/mkspecs/features/debug.prf c:/Qt/4.8.4/mkspecs/features/default_post.prf c:/Qt/4.8.4/mkspecs/features/win32/default_post.prf c:/Qt/4.8.4/mkspecs/features/win32/rtti.prf c:/Qt/4.8.4/mkspecs/features/win32/exceptions.prf c:/Qt/4.8.4/mkspecs/features/win32/stl.prf c:/Qt/4.8.4/mkspecs/features/shared.prf c:/Qt/4.8.4/mkspecs/features/warn_on.prf c:/Qt/4.8.4/mkspecs/features/qt.prf c:/Qt/4.8.4/mkspecs/features/win32/thread.prf c:/Qt/4.8.4/mkspecs/features/moc.prf c:/Qt/4.8.4/mkspecs/features/win32/windows.prf c:/Qt/4.8.4/mkspecs/features/resources.prf c:/Qt/4.8.4/mkspecs/features/uic.prf c:/Qt/4.8.4/mkspecs/features/yacc.prf c:/Qt/4.8.4/mkspecs/features/lex.prf c:/Qt/4.8.4/mkspecs/features/include_source_dir.prf c:\Qt\4.8.4\lib\qtmaind.prl  HEADERS RESOURCES IMAGES SOURCES OBJECTIVE_SOURCES FORMS YACCSOURCES YACCSOURCES LEXSOURCES 

clean: compiler_clean 
	-$(DEL_FILE) build/Debug/MinGW-Windows/MainScreen.o build/Debug/MinGW-Windows/main.o build/Debug/MinGW-Windows/moc_MainScreen.o
	-$(DEL_FILE) build/Debug/MinGW-Windows/MCGExtract_resource_res.o

distclean: clean
	-$(DEL_FILE) $(DESTDIR_TARGET)
	-$(DEL_FILE) qttmp-Debug.mk

check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_MainScreen.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_MainScreen.cpp
moc_MainScreen.cpp: ui_MainScreen.h \
		MainScreen.h
	C:/Qt/4.8.4/bin/moc.exe $(DEFINES) $(INCPATH) -D__GNUC__ -DWIN32 MainScreen.h -o moc_MainScreen.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_MainScreen.h
compiler_uic_clean:
	-$(DEL_FILE) ui_MainScreen.h
ui_MainScreen.h: MainScreen.ui
	c:/Qt/4.8.4/bin/uic.exe MainScreen.ui -o ui_MainScreen.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 



####### Compile

build/Debug/MinGW-Windows/MainScreen.o: MainScreen.cpp MainScreen.h \
		ui_MainScreen.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/Debug/MinGW-Windows/MainScreen.o MainScreen.cpp

build/Debug/MinGW-Windows/main.o: main.cpp MainScreen.h \
		ui_MainScreen.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/Debug/MinGW-Windows/main.o main.cpp

build/Debug/MinGW-Windows/moc_MainScreen.o: moc_MainScreen.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/Debug/MinGW-Windows/moc_MainScreen.o moc_MainScreen.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

