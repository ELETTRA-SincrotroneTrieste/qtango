include (/usr/local/qtango/include/qtango6/qtango.pri)

TEMPLATE = app

QT += 

CONFIG +=

DEFINES += QT_NO_DEBUG_OUTPUT

OBJECTS_DIR = obj

# RESOURCES += 


SOURCES += src/main.cpp \
		src/$CPPFILE$

HEADERS += src/$HFILE$

FORMS    = src/$FORMFILE$

TARGET   = bin/$PROJECT_NAME$

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src 

