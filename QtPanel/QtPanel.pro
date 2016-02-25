TEMPLATE = app

QT += qml quick
CONFIG += c++11

RESOURCES += QtPanel.qrc

qml.files = src/QtPanel.qml

SOURCES += src/QtPanel.cpp \
           src/Receiver.cpp \
           src/AWSIoTClient.cpp \
           src/Manager.cpp

HEADERS += src/Receiver.h \
           src/AWSIoTClient.h \
           src/Manager.h

PAHO = ../../../paho.mqtt.c

INCLUDEPATH += $$PAHO/src

macx:LIBS += -L$$PAHO/build/macosx -L/usr/local/opt/openssl/lib
qnx-armle-v7-qcc:LIBS += -L$$PAHO/build/qnx-armle-v7

LIBS += -lpaho-mqtt3as -lssl -lcrypto

DEFINES += OPENSSL

CONFIG += warn_off
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
