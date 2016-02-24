TEMPLATE = app

QT += qml quick
CONFIG += c++11

RESOURCES += QtPanel.qrc

qml.files = src/QtPanel.qml

SOURCES += src/QtPanel.cpp \
           src/Receiver.cpp \
           src/AWSIoTClient.cpp

HEADERS += src/Receiver.h \
           src/AWSIoTClient.h

PAHO = ../../../paho.mqtt.c

INCLUDEPATH += $$PAHO/src

macx:LIBS += -L$$PAHO/build/macosx -L/usr/local/opt/openssl/lib
LIBS += -lpaho-mqtt3cs -lssl -lcrypto

DEFINES += OPENSSL

CONFIG += warn_off
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
