/*****************************************************************************
* Copyright 2015-2024 Alexander Barthel alex@littlenavmap.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "utiltest.h"
#include "atools.h"
#include "util/fileoperations.h"
#include "util/props.h"
#include "util/filesystemwatcher.h"
#include "testutil/testutil.h"

#include <QFileInfo>

using atools::util::FileSystemWatcher;

UtilTest::UtilTest()
{

}

void UtilTest::runtest(int argc, char *argv[])
{
  UtilTest tst;
  QTest::qExec(&tst, argc, argv);
}

void UtilTest::initTestCase()
{
}

void UtilTest::cleanupTestCase()
{

}

void UtilTest::testFileOperations()
{
  atools::util::FileOperations operations(true);

  QVERIFY(!operations.canRemoveDir(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).constFirst()));
  QVERIFY(!operations.canRemoveDir("."));
  QVERIFY(!operations.canRemoveDir(".."));
  QVERIFY(!operations.canRemoveDir(QDir::drives().constFirst().absoluteFilePath()));
  QVERIFY(operations.canRemoveDir("testdata/MSFS"));

  operations.removeDirectory("output/MSFS-copy", false);

  QVERIFY(!QFileInfo::exists("output/MSFS-copy/Community/microsoft-aircraft-bell407/SimObjects/Airplanes/"
                             "microsoft-aircraft-bell407/aircraft.cfg"));

  operations.removeDirectory("testdata/../output/MSFS-copy2", false);
  operations.copyDirectory("testdata/MSFS", "testdata/../output/MSFS-copy2", false);
  QCOMPARE(operations.getErrors().size(), 0);
  QCOMPARE(operations.getFilesProcessed(), 27);

  operations.copyDirectory("testdata/MSFS", "testdata/../NOEXIST/MSFS-copy", false);
  QCOMPARE(operations.getErrors().size(), 1);
  QCOMPARE(operations.getFilesProcessed(), 0);

  operations.copyDirectory("testdata/MSFS", "output/MSFS-copy", false);
  QCOMPARE(operations.getErrors().size(), 0);
  QCOMPARE(operations.getFilesProcessed(), 27);

  QVERIFY(QFileInfo::exists("output/MSFS-copy/Community/microsoft-aircraft-bell407/SimObjects/Airplanes/"
                            "microsoft-aircraft-bell407/aircraft.cfg"));

  operations.copyDirectory("testdata/MSFS", "output/MSFS-copy", false);
  QCOMPARE(operations.getErrors().size(), 1);
  QCOMPARE(operations.getFilesProcessed(), 0);

  operations.copyDirectory("testdata/MSFS", "output/MSFS-copy", true);
  QCOMPARE(operations.getErrors().size(), 0);
  QCOMPARE(operations.getFilesProcessed(), 27);

  operations.removeDirectory("output/MSFS-copy", false);
  QCOMPARE(operations.getErrors().size(), 0);
  QCOMPARE(operations.getFilesProcessed(), 27);

  QVERIFY(!QFileInfo::exists("output/MSFS-copy/Community/microsoft-aircraft-bell407/SimObjects/Airplanes/"
                             "microsoft-aircraft-bell407/aircraft.cfg"));

  operations.copyDirectory("testdata/MSFS", "output/MSFS-copy", false);
  QCOMPARE(operations.getErrors().size(), 0);
  QCOMPARE(operations.getFilesProcessed(), 27);

  operations.removeDirectory("output/MSFS-copy", true);
  QCOMPARE(operations.getErrors().size(), 0);
  QCOMPARE(operations.getFilesProcessed(), 27);

  QVERIFY(!QFileInfo::exists("output/MSFS-copy/Community/microsoft-aircraft-bell407/SimObjects/Airplanes/"
                             "microsoft-aircraft-bell407/aircraft.cfg"));
  QVERIFY(QFileInfo::exists("output/MSFS-copy/Community/microsoft-aircraft-bell407/SimObjects/Airplanes/"
                            "microsoft-aircraft-bell407"));
}

void UtilTest::testFlags()
{
  TestEnums test(TestEnum::NONE);
  QCOMPARE(test, TestEnum::NONE);

  test |= TestEnum::ONE64;
  QCOMPARE(static_cast<TestEnum>(test), TestEnum::ONE64);
  QVERIFY(test.testFlag(TestEnum::ONE64));

  test &= TestEnum::FOUR64;
  QVERIFY(static_cast<TestEnum>(test) != TestEnum::ONE64);
  QVERIFY(!test.testFlag(TestEnum::ONE64));

  TestEnums testList2({TestEnum::NONE, TestEnum::FOUR32, TestEnum::ONE64});
  QVERIFY(testList2.testFlag(TestEnum::FOUR32));
  QVERIFY(testList2.testFlag(TestEnum::ONE64));

  testList2 = ~testList2;
  QVERIFY(testList2.testFlag(TestEnum::ONE32));
  QVERIFY(!testList2.testFlag(TestEnum::FOUR32));
  QVERIFY(!testList2.testFlag(TestEnum::ONE64));
  QVERIFY(testList2.testFlag(TestEnum::FOUR64));

  TestEnums testList(TestEnum::NONE | TestEnum::FOUR32 | TestEnum::ONE64);
  QVERIFY(testList.testFlag(TestEnum::FOUR32));
  QVERIFY(testList.testFlag(TestEnum::ONE64));

  QByteArray bytes;
  QDataStream out(&bytes, QIODevice::WriteOnly);
  out << testList;

  QDataStream in(&bytes, QIODevice::ReadOnly);
  TestEnums read;
  in >> read;
  QCOMPARE(testList, read);

  QVariant var = QVariant::fromValue<TestEnums>(testList);
  TestEnums fromVar = var.value<TestEnums>();
  QCOMPARE(testList, fromVar);

  var = QVariant::fromValue<TestEnums>(TestEnum::EIGHT64);
  fromVar = var.value<TestEnums>();
  QCOMPARE(TestEnum::EIGHT64, fromVar);
}

void UtilTest::testProps()
{
  using namespace atools::util;
  enum Key
  {
    KEY_BOOL = 10,
    KEY_BYTE,
    KEY_SHORT,
    KEY_INT,
    KEY_LONG,
    KEY_FLOAT,
    KEY_DOUBLE,
    KEY_STRING,
    KEY_BYTES,
    KEY_VARIANT,
    KEY_NONE
  };

  const unsigned char byteTest[] = {0x78, 0x56, 0x34, 0x12};
  QByteArray bytes1((const char *)(byteTest), 4);

  const unsigned char byteTest2[] = {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef};
  QByteArray bytes2((const char *)(byteTest2), 12);

  Props props;
  props.addProps(QVector<Prop>({Prop(KEY_BOOL, true), Prop(KEY_BOOL, false),
                                Prop(KEY_BYTE, -1),
                                Prop(KEY_BYTE, 127),
                                Prop(KEY_BYTE, 128),
                                Prop(KEY_BYTE, 255),
                                Prop(KEY_INT, 42), Prop(KEY_INT, 965),
                                Prop(KEY_INT, std::numeric_limits<int>::max()),
                                Prop(KEY_INT, std::numeric_limits<int>::min()),
                                Prop(KEY_INT, std::numeric_limits<unsigned int>::max()),
                                Prop(KEY_INT, std::numeric_limits<unsigned int>::min()),
                                Prop(KEY_SHORT, static_cast<short>(1024)),
                                Prop(KEY_LONG, 4200000000000000000LL),
                                Prop(KEY_LONG, 42LL),
                                Prop(KEY_FLOAT, 3.14159f),
                                Prop(KEY_DOUBLE, 2.71828182846),
                                Prop(KEY_STRING, QString("Hello Test")),
                                Prop(KEY_BYTES, bytes1),
                                Prop(KEY_BYTES, bytes2),
                                Prop(KEY_VARIANT, QVariant("Variant")),
                                Prop(KEY_VARIANT, QVariant(1968)),
                                Prop(KEY_VARIANT, QVariant(12.3456789f)),
                                Prop(KEY_VARIANT, QVariant(23.5678901)),
                                Prop(KEY_NONE),
                                Prop()}));

  QByteArray bytes;
  QDataStream out(&bytes, QIODevice::WriteOnly);
  out << props;

  Props propsRead;
  QDataStream in(bytes);
  in >> propsRead;

  for(const Prop& p : propsRead)
    qDebug() << p;

  QCOMPARE(props.size() - 1, propsRead.size());

  QCOMPARE(propsRead.getProp(KEY_NONE).isValid(), true);
  QCOMPARE(propsRead.getProps(KEY_BOOL).at(0).getValueBool(), true);
  QCOMPARE(propsRead.getProps(KEY_BOOL).at(1).getValueBool(), false);

  QCOMPARE(propsRead.getProps(KEY_BYTE).at(0).getValueInt(), -1);
  QCOMPARE(propsRead.getProps(KEY_BYTE).at(1).getValueInt(), 127);
  QCOMPARE(propsRead.getProps(KEY_BYTE).at(2).getValueInt(), 128);
  QCOMPARE(propsRead.getProps(KEY_BYTE).at(3).getValueInt(), 255);

  QCOMPARE(propsRead.getProp(KEY_SHORT).getValueShort(), static_cast<short>(1024));

  QCOMPARE(propsRead.getProps(KEY_INT).at(0).getValueInt(), 42);
  QCOMPARE(propsRead.getProps(KEY_INT).at(1).getValueInt(), 965);
  QCOMPARE(propsRead.getProps(KEY_INT).at(2).getValueInt(), std::numeric_limits<int>::max());
  QCOMPARE(propsRead.getProps(KEY_INT).at(3).getValueInt(), std::numeric_limits<int>::min());
  QCOMPARE(propsRead.getProps(KEY_INT).at(4).getValueUInt(), std::numeric_limits<unsigned int>::max());
  QCOMPARE(propsRead.getProps(KEY_INT).at(5).getValueUInt(), std::numeric_limits<unsigned int>::min());

  QCOMPARE(propsRead.getProps(KEY_LONG).at(0).getValueLongLong(), 4200000000000000000LL);
  QCOMPARE(propsRead.getProps(KEY_LONG).at(1).getValueLongLong(), 42LL);

  QCOMPARE(propsRead.getProp(KEY_FLOAT).getValueFloat(), 3.14159f);
  QCOMPARE(propsRead.getProp(KEY_DOUBLE).getValueDouble(), 2.71828182846);
  QCOMPARE(propsRead.getProp(KEY_STRING).getValueString(), QString("Hello Test"));
  QCOMPARE(propsRead.getProps(KEY_BYTES).at(0).getValueBytes(), bytes1);
  QCOMPARE(propsRead.getProps(KEY_BYTES).at(1).getValueBytes(), bytes2);

  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(0).getValueVariant(), QVariant("Variant"));
  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(1).getValueVariant(), QVariant(1968));
  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(2).getValueVariant(), QVariant(12.3456789f));
  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(3).getValueVariant(), QVariant(23.5678901));

  for(auto it = propsRead.constBegin(); it != propsRead.constEnd(); ++it)
    QVERIFY(it.value().isValid());
}

void UtilTest::testFilesystemWatcher()
{
  QFile::remove("testdata/watcher/DUMMYFILE");
  const int delay = 4;

  FileSystemWatcher watcher(this, true);
  QStringList filenamesResult;
  QString dirResult;
  bool foundFiles = false, foundDir = false;
  connect(&watcher, &FileSystemWatcher::filesUpdated, [&filenamesResult, &foundFiles](const QStringList& filenames)->void
  {
    foundFiles = true;
    filenamesResult = filenames;
    std::sort(filenamesResult.begin(), filenamesResult.end());
  });
  connect(&watcher, &FileSystemWatcher::dirUpdated, [&dirResult, &foundDir](const QString& dir)->void
  {
    foundDir = true;
    dirResult = dir;
  });

  QStringList files({"testdata/watcher/METAR-2022-9-6-19.00-ZULU.txt", "testdata/watcher/METAR-2022-9-6-20.00-ZULU.txt"});
  watcher.setFilenamesAndStart(files);

  QFile metar1(files.at(0)), metar2(files.at(1));
  metar1.open(QIODevice::Append);
  metar2.open(QIODevice::Append);

  qDebug() << Q_FUNC_INFO << "Test first ===========================================";
  testutil::wait(delay);
  metar1.setFileTime(QDateTime::currentDateTime().addSecs(-25), QFileDevice::FileModificationTime);
  testutil::waitForValue(foundFiles, 60);
  QVERIFY(!foundDir);
  QVERIFY(foundFiles);
  QCOMPARE(filenamesResult, {files.at(0)});

  qDebug() << Q_FUNC_INFO << "Test second ===========================================";
  foundFiles = foundDir = false;
  filenamesResult.clear();
  dirResult.clear();
  testutil::wait(delay);
  metar2.setFileTime(QDateTime::currentDateTime().addSecs(-20), QFileDevice::FileModificationTime);
  testutil::waitForValue(foundFiles, 60);
  QVERIFY(!foundDir);
  QVERIFY(foundFiles);
  QCOMPARE(filenamesResult, {files.at(1)});

  qDebug() << Q_FUNC_INFO << "Test two files ===========================================";
  foundFiles = foundDir = false;
  filenamesResult.clear();
  dirResult.clear();
  testutil::wait(delay);
  metar1.setFileTime(QDateTime::currentDateTime().addSecs(-15), QFileDevice::FileModificationTime);
  metar2.setFileTime(QDateTime::currentDateTime().addSecs(-10), QFileDevice::FileModificationTime);
  testutil::waitForValue(foundFiles, 60);
  QVERIFY(!foundDir);
  QVERIFY(foundFiles);
  QCOMPARE(filenamesResult, files);

  qDebug() << Q_FUNC_INFO << "Test dir ===========================================";
  foundFiles = foundDir = false;
  filenamesResult.clear();
  dirResult.clear();
  testutil::wait(delay);
  QFile dummyFile("testdata/watcher/DUMMYFILE");
  dummyFile.open(QIODevice::WriteOnly);
  testutil::waitForValue(foundDir, 60);
  QVERIFY(foundDir);
  QVERIFY(!foundFiles);
  QCOMPARE(dirResult, "testdata/watcher");

  qDebug() << Q_FUNC_INFO << "Test last ===========================================";
  foundFiles = foundDir = false;
  filenamesResult.clear();
  dirResult.clear();
  testutil::wait(delay);
  metar1.setFileTime(QDateTime::currentDateTime().addSecs(-5), QFileDevice::FileModificationTime);
  testutil::waitForValue(foundFiles, 60);
  QVERIFY(!foundDir);
  QVERIFY(foundFiles);
  QCOMPARE(filenamesResult, {files.at(0)});
}

void UtilTest::testLinkTarget()
{
#ifdef Q_OS_WIN

  if(QFileInfo::exists(QDir::homePath() + SEP + "AppData\\Local\\Packages\\"
                                                "Microsoft.FlightSimulator_8wekyb3d8bbwe\\"
                                                "LocalCache\\Packages\\Community"))
  {
    QString community = QDir::homePath() + SEP + "AppData\\Local\\Packages\\"
                                                 "Microsoft.FlightSimulator_8wekyb3d8bbwe\\"
                                                 "LocalCache\\Packages\\Community";

    // Mapped drive
    QString target = atools::linkTarget(QFileInfo("C:\\Mounted\\Disk"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, QString());

    // Mapped drive
    target = atools::linkTarget(QFileInfo("C:\\Mounted\\Disk\\Packages"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, QString());

    // Not linked
    target = atools::linkTarget(QFileInfo(community + SEP + "airport-lemg"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, QString());

    // Directory
    target = atools::linkTarget(QFileInfo(community + SEP + "airport-eddk"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target,
             "C:/Users/alex/AppData/Local/Packages/Microsoft.FlightSimulator_8wekyb3d8bbwe/LocalCache/Packages/Community Test/airport-eddk");

    // Symbolic Link
    target = atools::linkTarget(QFileInfo(community + SEP + "_guadeloupe TFFR-symlink"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, "D:/MSFS Addons/guadeloupe TFFR");

    // Junction
    target = atools::linkTarget(QFileInfo(community + SEP + "_airport-licc-catania-junction"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, "D:/MSFS Addons/airport-licc-catania");

    // Shortcut
    target = atools::linkTarget(QFileInfo(community + SEP + "_cyqx-gander.lnk"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, "D:/MSFS Addons/cyqx-gander");

    target = atools::linkTarget(QFileInfo(community + SEP + "_cyqx-gander"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, QString());
  }
#endif
}

void UtilTest::testCanonicalPath()
{
#ifdef Q_OS_WIN
  if(QFileInfo::exists(QDir::homePath() + SEP + "AppData\\Local\\Packages\\"
                                                "Microsoft.FlightSimulator_8wekyb3d8bbwe\\"
                                                "LocalCache\\Packages\\Community"))
  {
    QString community = QDir::homePath() + SEP + "AppData\\Local\\Packages\\"
                                                 "Microsoft.FlightSimulator_8wekyb3d8bbwe\\"
                                                 "LocalCache\\Packages\\Community";
    // Directory
    QString target, path;

    // Mapped drive
    target = atools::canonicalFilePath(QFileInfo("C:\\Mounted\\Disk"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, QDir::cleanPath("C:\\Mounted\\Disk"));

    // Mapped drive
    target = atools::canonicalFilePath(QFileInfo("C:\\Mounted\\Disk\\Packages"));
    qDebug() << Q_FUNC_INFO << target;
    QCOMPARE(target, QDir::cleanPath("C:\\Mounted\\Disk\\Packages"));

    path = community + SEP + "airport-eddk\\scenery\\world\\scenery\\EDDK.bgl";
    target = atools::canonicalFilePath(QFileInfo(path));
    qDebug() << Q_FUNC_INFO << "Directory" << target;
    QCOMPARE(target, QDir::cleanPath("C:/Users/alex/AppData/Local/Packages/Microsoft.FlightSimulator_8wekyb3d8bbwe/"
                                     "LocalCache/Packages/Community Test/airport-eddk/scenery/world/scenery/EDDK.bgl"));

    // Symbolic Link
    target = atools::canonicalFilePath(QFileInfo(community + SEP + "_guadeloupe TFFR-symlink\\scenery\\tffr.bgl"));
    qDebug() << Q_FUNC_INFO << "Symbolic link" << target;
    QCOMPARE(target, QDir::cleanPath("D:\\MSFS Addons\\guadeloupe TFFR\\scenery\\tffr.bgl"));

    // Junction
    target = atools::canonicalFilePath(QFileInfo(community + SEP + "_airport-licc-catania-junction\\scenery\\Catania scenery.bgl"));
    qDebug() << Q_FUNC_INFO << "Junction" << target;
    QCOMPARE(target, QDir::cleanPath("D:\\MSFS Addons\\airport-licc-catania\\scenery\\Catania scenery.bgl"));

    // Shortcut
    target = atools::canonicalFilePath(QFileInfo(community + SEP + "_cyqx-gander.lnk\\scenery\\world\\scenery\\cyqx\\Gander.bgl"));
    qDebug() << Q_FUNC_INFO << "Shortcut" << target;
    QCOMPARE(target, QDir::cleanPath("D:\\MSFS Addons\\cyqx-gander\\scenery\\world\\scenery\\cyqx\\Gander.bgl"));

    path = community + SEP + "_cyqx-gander\\scenery\\world\\scenery\\cyqx\\Gander.bgl";
    target = atools::canonicalFilePath(QFileInfo(path));
    qDebug() << Q_FUNC_INFO << "Shortcut no extension" << target;
    QCOMPARE(target, QDir::cleanPath(path));

    path = community + SEP + "C:\\ATOOLS DOES NEVER EXIST\\BLAH\\BLAH";
    target = atools::canonicalFilePath(QFileInfo(path));
    qDebug() << Q_FUNC_INFO << "Invalid path" << target;
    QCOMPARE(target, QDir::cleanPath(path));
  }
#endif
}
