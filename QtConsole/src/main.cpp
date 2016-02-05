#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AWSIoTClient.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AWSIoTClient iotClient;
    engine.rootContext()->setContextProperty("iotClient", &iotClient);

    engine.load(QUrl(QStringLiteral("qrc:/src/main.qml")));

    return app.exec();
}
