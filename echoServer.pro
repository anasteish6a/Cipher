QT += core network sql
QT -= gui

TARGET = echoServer
CONFIG += console c++17
CONFIG -= app_bundle

SOURCES += \
    main.cpp \
    mytcpserver.cpp \
    database.cpp \
    tasks.cpp

HEADERS += \
    mytcpserver.h \
    database.h \
    tasks.h

win32:LIBS += -lws2_32
else:unix: LIBS += -lpthread

target.path = $$OUT_PWD
INSTALLS += target
