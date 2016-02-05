TEMPLATE = app

QT += qml quick
CONFIG += c++11

RESOURCES += main.qrc

SOURCES += src/main.cpp \
           src/AWSIoTClient.cpp
           
HEADERS += src/AWSIoTClient.h

qml.files = src/main.qml

AWSIOT = ../../../mqtt/org.eclipse.paho.mqtt.c

INCLUDEPATH += $$AWSIOT/src

macx:LIBS += -L$$AWSIOT/build/macosx -L/usr/local/opt/openssl/lib

LIBS += -lpaho-mqtt3cs -lssl -lcrypto

DEFINES += OPENSSL

CONFIG += warn_off
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
