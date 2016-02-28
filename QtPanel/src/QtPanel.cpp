/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Manager.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Manager manager;
    engine.rootContext()->setContextProperty("manager", &manager);

    engine.load(QUrl(QStringLiteral("qrc:/qml/QtPanel.qml")));

    return app.exec();
}
