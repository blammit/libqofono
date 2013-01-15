/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@gmail.com
**
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QString>
#include <QtTest>
#include "qofonomanager.h"
#include "qofonoconnectionmanager.h"
#include "qofonoconnectioncontext.h"
#include "qofonomodem.h"
#include "qofononetworkregistration.h"

// These auto tests require
// phonesim or real modem
class Tst_qofonoTest : public QObject
{
    Q_OBJECT
    
public:
    Tst_qofonoTest();
    
private Q_SLOTS:
    void testManager();
    void testModem();
    void testConnectionManager();
    void testContextConnection();
    void testNetworkRegistration();
};

Tst_qofonoTest::Tst_qofonoTest()
{

}

void Tst_qofonoTest::testManager()
{
    QOfonoManager manager;
    QStringList modems = manager.modems();
    QVERIFY(!modems.isEmpty());
//TODO modemAdded, modemRemoved signals ?

}

void Tst_qofonoTest::testModem()
{
    QOfonoManager manager;
    QOfonoModem modem;
    QVERIFY(modem.modemPath().isEmpty());
    QStringList modemList = manager.modems();
    modem.setModemPath(modemList[0]);
    QVERIFY(!modem.modemPath().isEmpty());

    if (!modem.powered()) {
        modem.setPowered(true);
        QVERIFY(modem.powered());
    } else {
        if (modemList[0] != "/phonesim") {
            // power off phonesim modem doesnt work
            modem.setPowered(false);
            QVERIFY(!modem.powered());
            modem.setPowered(true);
            QVERIFY(modem.powered());
        }
    }
    if (!modem.online()) {
        modem.setOnline(true);
        QTest::qWait(1000);
    }
    QVERIFY(modem.online());
}

void Tst_qofonoTest::testConnectionManager()
{
    QOfonoManager manager;
    QOfonoConnectionManager connman;

    QVERIFY(connman.modemPath().isEmpty());
    connman.setModemPath(manager.modems()[0]);
    QVERIFY(!connman.modemPath().isEmpty());

    if (!connman.contexts().isEmpty()) {
        Q_FOREACH(const QString &path, connman.contexts()) {
         connman.removeContext(path);
        }
    }
    QVERIFY(connman.contexts().isEmpty());
    QSignalSpy spy(&connman, SIGNAL(contextAdded(QString)));
    connman.addContext("internet");
    QTest::qWait(1000);

    QCOMPARE(spy.count(),1);
    QList<QVariant> arguments ;
    arguments = spy.takeFirst();

    QVERIFY(!connman.contexts().isEmpty());
    QSignalSpy spy2(&connman, SIGNAL(contextRemoved(QString)));
    connman.removeContext(arguments[0].toString());
    QTest::qWait(1000);

    QCOMPARE(spy2.count(),1);
    QList<QVariant> arguments2 ;
    arguments2 = spy2.takeFirst();

    QVERIFY(connman.contexts().isEmpty());

//TODO other signals
}

void Tst_qofonoTest::testContextConnection()
{
        QOfonoManager manager;
        QStringList modems = manager.modems();

        QOfonoConnectionManager connman;
        connman.setModemPath(modems[0]);

        QOfonoConnectionContext connContext;
        QStringList contextList = connman.contexts();
        QVERIFY(contextList.isEmpty());

        connman.addContext("internet");
        QTest::qWait(1000);

        contextList = connman.contexts();

        QVERIFY(!contextList.isEmpty());

        connContext.setContextPath(contextList[0]);

        QVERIFY(!connContext.contextPath().isEmpty());

        QVERIFY(connContext.accessPointName().isEmpty());
        QSignalSpy spy1(&connContext, SIGNAL(accessPointNameChanged(QString)));
        connContext.setAccessPointName("Jolla");
        QTest::qWait(1000);
        QCOMPARE(spy1.count(),1);
        QList<QVariant> arguments1;
        arguments1 = spy1.takeFirst();
        QCOMPARE(arguments1[0].toString(),QString("Jolla"));

        if (!connman.contexts().isEmpty()) {
            Q_FOREACH(const QString &path, connman.contexts()) {
             connman.removeContext(path);
            }
        }

//        QVERIFY(!connContext.active());
//        QSignalSpy spy2(&connContext, SIGNAL(activeChanged(bool)));
//        connContext.setActive(true);
//        QTest::qWait(1000);
//        QCOMPARE(spy2.count(),1);

//        QList<QVariant> arguments2 = spy2.takeFirst();
//        QCOMPARE(arguments2[0].toBool(),true);

}

void Tst_qofonoTest::testNetworkRegistration()
{

    QOfonoManager manager;
    QStringList modems = manager.modems();

    QOfonoNetworkRegistration netreg;

    QVERIFY(netreg.modemPath().isEmpty());
    netreg.setModemPath(modems[0]);

    QVERIFY(!netreg.name().isEmpty());
    QVERIFY(!netreg.mode().isEmpty());
    QVERIFY(!netreg.status().isEmpty());


    QVERIFY(!netreg.locationAreaCode() != 0);
    QVERIFY(!netreg.mcc().isEmpty());
    QVERIFY(!netreg.mnc().isEmpty());

//    QVERIFY(!netreg.cellId() != 0);
//    QVERIFY(!netreg.strength() != 0);
//    QVERIFY(!netreg.baseStation().isEmpty());
}

QTEST_MAIN(Tst_qofonoTest)

#include "tst_qofonotest.moc"
