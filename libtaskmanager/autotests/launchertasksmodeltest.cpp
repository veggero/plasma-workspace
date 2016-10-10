/********************************************************************
Copyright 2016  Eike Hein <hein@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "launchertasksmodel.h"

using namespace TaskManager;

class LauncherTasksModelTest : public QObject
{
    Q_OBJECT

    private Q_SLOTS:
        void initTestCase();

        void shouldRoundTripLauncherUrlList();
        void shouldIgnoreInvalidUrls();
        void shouldRejectDuplicates();
        void shouldAddRemoveLauncher();
        void shouldReturnValidLauncherPositions();
        void shouldReturnValidData();

    private:
        QStringList m_urlStrings;
};

void LauncherTasksModelTest::initTestCase()
{
    m_urlStrings << QLatin1String("file:///usr/share/applications/org.kde.dolphin.desktop");
    m_urlStrings << QLatin1String("file:///usr/share/applications/org.kde.konsole.desktop");
}

void LauncherTasksModelTest::shouldRoundTripLauncherUrlList()
{
    LauncherTasksModel m;

    QSignalSpy launcherListChangedSpy(&m, &LauncherTasksModel::serializedLauncherListChanged);
    QVERIFY(launcherListChangedSpy.isValid());

    m.setSerializedLauncherList(m_urlStrings);

    QCOMPARE(launcherListChangedSpy.count(), 1);

    QCOMPARE(m.serializedLauncherList(), m_urlStrings);

    QCOMPARE(m.data(m.index(0, 0), AbstractTasksModel::LauncherUrl).toString(), m_urlStrings.at(0));
    QCOMPARE(m.data(m.index(1, 0), AbstractTasksModel::LauncherUrl).toString(), m_urlStrings.at(1));
}

void LauncherTasksModelTest::shouldIgnoreInvalidUrls()
{
    LauncherTasksModel m;

    QStringList urlStrings;
    urlStrings << QLatin1String("GARBAGE URL");

    QSignalSpy launcherListChangedSpy(&m, &LauncherTasksModel::serializedLauncherListChanged);
    QVERIFY(launcherListChangedSpy.isValid());

    m.setSerializedLauncherList(urlStrings);

    QCOMPARE(launcherListChangedSpy.count(), 0);

    bool added = m.requestAddLauncher(QUrl(urlStrings.at(0)));

    QVERIFY(!added);
    QCOMPARE(launcherListChangedSpy.count(), 0);

    QCOMPARE(m.serializedLauncherList(), QStringList());
}

void LauncherTasksModelTest::shouldRejectDuplicates()
{
    LauncherTasksModel m;

    QStringList urlStrings;
    urlStrings << QLatin1String("file:///usr/share/applications/org.kde.dolphin.desktop");
    urlStrings << QLatin1String("file:///usr/share/applications/org.kde.dolphin.desktop");

    QSignalSpy launcherListChangedSpy(&m, &LauncherTasksModel::serializedLauncherListChanged);
    QVERIFY(launcherListChangedSpy.isValid());

    m.setSerializedLauncherList(urlStrings);

    QCOMPARE(launcherListChangedSpy.count(), 1);

    bool added = m.requestAddLauncher(QUrl(urlStrings.at(0)));

    QVERIFY(!added);
    QCOMPARE(launcherListChangedSpy.count(), 1);

    QCOMPARE(m.serializedLauncherList(), QStringList() << urlStrings.at(0));
}

void LauncherTasksModelTest::shouldAddRemoveLauncher()
{
    LauncherTasksModel m;

    QSignalSpy launcherListChangedSpy(&m, &LauncherTasksModel::serializedLauncherListChanged);
    QVERIFY(launcherListChangedSpy.isValid());

    bool added = m.requestAddLauncher(QUrl(m_urlStrings.at(0)));

    QVERIFY(added);
    QCOMPARE(launcherListChangedSpy.count(), 1);

    QCOMPARE(m.serializedLauncherList().at(0), m_urlStrings.at(0));

    bool removed = m.requestRemoveLauncher(QUrl(m_urlStrings.at(0)));

    QVERIFY(removed);
    QCOMPARE(launcherListChangedSpy.count(), 2);

    removed = m.requestRemoveLauncher(QUrl(m_urlStrings.at(0)));

    QVERIFY(!removed);

    QCOMPARE(m.serializedLauncherList(), QStringList());
}

void LauncherTasksModelTest::shouldReturnValidLauncherPositions()
{
    LauncherTasksModel m;

    QSignalSpy launcherListChangedSpy(&m, &LauncherTasksModel::serializedLauncherListChanged);
    QVERIFY(launcherListChangedSpy.isValid());

    m.setSerializedLauncherList(m_urlStrings);

    QCOMPARE(launcherListChangedSpy.count(), 1);

    QCOMPARE(m.launcherPosition(QUrl(m_urlStrings.at(0))), 0);
    QCOMPARE(m.launcherPosition(QUrl(m_urlStrings.at(1))), 1);
}

void LauncherTasksModelTest::shouldReturnValidData()
{
    // FIXME Reuse TaskToolsTest app link setup, then run URLs through model.
}

QTEST_MAIN(LauncherTasksModelTest)

#include "launchertasksmodeltest.moc"
