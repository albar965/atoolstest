/*****************************************************************************
* Copyright 2015-2025 Alexander Barthel alex@littlenavmap.org
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

#include "magdectest.h"

#include "fs/common/magdecreader.h"
#include "wmm/magdectool.h"
#include "geo/pos.h"
#include "sql/sqlscript.h"
#include "sql/sqldatabase.h"
#include "testutil/testutil.h"

using atools::fs::common::MagDecReader;
using atools::geo::Pos;

MagdecTest::MagdecTest()
{

}

void MagdecTest::runtest(int argc, char *argv[])
{
  MagdecTest tst;
  QTest::qExec(&tst, argc, argv);
}

void MagdecTest::initTestCase()
{
  qInfo() << tool.getVersion();
  tool.init(2019);

  db = testutil::createDb("TESTDBMAGDEC", "online_test_magdec.sqlite", false /* readonly */);

  atools::sql::SqlScript script(db, true /* options->isVerbose()*/);
  script.executeScript(":/atools/resources/sql/fs/db/drop_nav.sql");
  script.executeScript(":/atools/resources/sql/fs/db/create_nav_schema.sql");
  db->commit();
}

void MagdecTest::cleanupTestCase()
{
  testutil::removeDb(db, "TESTDBMAGDEC");
}

void MagdecTest::testTool()
{
  atools::wmm::MagDecTool magDecTool;
  magDecTool.init(2019);
}

void MagdecTest::testOpen()
{
  MagDecReader reader;
  reader.readFromBgl("testdata/magdec.bgl");

  QCOMPARE(reader.isValid(), true);
}

void MagdecTest::testToolDatabase()
{
  {
    MagDecReader reader;
    reader.readFromWmm(2020, 9);
    reader.writeToTable(*db);
    db->commit();

    // VORDME: Santana (Sao Paulo Tma) (STN)
    QCOMPARE(reader.getMagVar(Pos(-46.9233, -23.4875)), -21.6098f);

    // VORDME: Albury (AY)
    QCOMPARE(reader.getMagVar(Pos(146.966, -36.0681)), 11.9171f);
  }

  {
    MagDecReader reader;
    reader.readFromTable(*db);

    // VORDME: Santana (Sao Paulo Tma) (STN)
    QCOMPARE(reader.getMagVar(Pos(-46.9233, -23.4875)), -21.6098f);

    // VORDME: Albury (AY)
    QCOMPARE(reader.getMagVar(Pos(146.966, -36.0681)), 11.9171f);
  }
}

void MagdecTest::testMagdecBgl_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("magvar");

  // Cannot test 90 degree since normalize will roll over

  QTest::newRow("VORDME: Santana (Sao Paulo Tma) (STN)") << Pos(-46.9233, -23.4875) << -21.2939f;
  QTest::newRow("VORDME: Albury (AY)") << Pos(146.966, -36.0681) << 11.7954f;

  QTest::newRow("Last Offset") << Pos(-1.f, 89.99f) << -3.59544f;
  QTest::newRow("First Offset") << Pos(0.f, -89.99f) << -30.3497f;
  QTest::newRow("Atlantic") << Pos(0.f, 0.f) << -5.16357f;
  QTest::newRow("Atlantic 2") << Pos(0.001f, 0.001f) << -5.16357f;
  QTest::newRow("Atlantic 3") << Pos(-0.001f, -0.001f) << -5.16357f;
  QTest::newRow("Vancouver YVR") << Pos(-123.1491, 49.0773) << 16.4714f; // 17° East
  QTest::newRow("Natash YNA") << Pos(-61.7809, 50.1836) << -19.6735f; // 21° West
  QTest::newRow("Nosy-Be NSB") << Pos(48.3233, -13.3056) << -9.47886f; // 9° West
  QTest::newRow("Waypoint GOMER") << Pos(-17.3333435058594, 28.0) << -5.5838f;

  QTest::newRow("West Min") << Pos(-180.f, 0.f) << 9.61853f;
  QTest::newRow("East Max") << Pos(180.f, 0.f) << 9.61853f;
  QTest::newRow("North") << Pos(0.f, 89.99f) << -2.59629f;
  QTest::newRow("South") << Pos(0.f, -89.99f) << -30.3497f;

  QTest::newRow("West North") << Pos(-180.f, 89.99f) << 177.396f;
  QTest::newRow("West South") << Pos(-180.f, -89.99f) << 149.627f;
  QTest::newRow("East North") << Pos(180.f, 89.99f) << 177.396f;
  QTest::newRow("East South") << Pos(180.f, -89.99f) << 149.627f;
}

void MagdecTest::testMagdecBgl()
{
  MagDecReader reader;
  reader.readFromBgl("testdata/magdec.bgl");

  QFETCH(Pos, pos);
  QFETCH(float, magvar);

  qDebug() << pos << magvar;
  QCOMPARE(reader.getMagVar(pos), magvar);
}

void MagdecTest::testMagdecWmm_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("magvar");

  QTest::newRow("VORDME: Santana (Sao Paulo Tma) (STN)") << Pos(-46.9233, -23.4875) << -21.5224f;
  QTest::newRow("VORDME: Albury (AY)") << Pos(146.966, -36.0681) << 11.8734f;

  QTest::newRow("Atlantic") << Pos(0.5f, 0.5f) << -4.41352f;
  QTest::newRow("Atlantic") << Pos(0.f, 0.f) << -4.75158f;
  QTest::newRow("Atlantic") << Pos(2.5f, 2.5f) << -3.16773f;
  QTest::newRow("Atlantic") << Pos(-2.5f, -2.5f) << -6.65639f;
  QTest::newRow("West Min") << Pos(-180.f, 0.f) << 9.83337f;
  QTest::newRow("East Max") << Pos(180.f, 0.f) << 9.83337f;

  QTest::newRow("Vancouver YVR") << Pos(-123.1491, 49.0773) << 16.4583f; // 17° East
  QTest::newRow("Natash YNA") << Pos(-61.7809, 50.1836) << -19.2137f; // 21° West
  QTest::newRow("Nosy-Be NSB") << Pos(48.3233, -13.3056) << -9.54478f; // 9° West
  QTest::newRow("Waypoint GOMER") << Pos(-17.3333435058594, 28.0) << -5.21769f;

  QTest::newRow("Atlantic 2") << Pos(0.001f, 0.001f) << -4.75158f;
  QTest::newRow("Atlantic 3") << Pos(-0.001f, -0.001f) <<  -4.75158f;
}

void MagdecTest::testMagdecWmm()
{
  MagDecReader reader;
  reader.readFromWmm(2019);

  QFETCH(Pos, pos);
  QFETCH(float, magvar);

  qDebug() << pos << magvar;
  QCOMPARE(reader.getMagVar(pos), magvar);
}

void MagdecTest::testMagdecDate()
{
  MagDecReader reader;
  reader.readFromWmm(2019);
  QCOMPARE(reader.getReferenceDate(), QDate(2019, 1, 1));

  MagDecReader reader2;
  reader2.readFromWmm(2019, 6);
  QCOMPARE(reader2.getReferenceDate(), QDate(2019, 6, 1));

  MagDecReader reader3;
  reader3.readFromWmm();
  QCOMPARE(reader3.getReferenceDate(), QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1));
}

void MagdecTest::testWmmTool_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("magvar");

  QTest::newRow("VORDME: Santana (Sao Paulo Tma) (STN)") << Pos(-46.9233, -23.4875) << -21.3441f;
  QTest::newRow("VORDME: Albury (AY)") << Pos(146.966, -36.0681) << 11.9005f;

  QTest::newRow("Atlantic") << Pos(0.5f, 0.5f) << -4.41352f;
  QTest::newRow("Atlantic") << Pos(0.f, 0.f) << -4.75158f;
  QTest::newRow("Atlantic") << Pos(2.5f, 2.5f) << -3.16773f;
  QTest::newRow("Atlantic") << Pos(-2.5f, -2.5f) << -6.65639f;
  QTest::newRow("West Min") << Pos(-180.f, 0.f) << 9.83337f;
  QTest::newRow("East Max") << Pos(180.f, 0.f) << 9.75196f;

  QTest::newRow("Vancouver YVR") << Pos(-123.1491, 49.0773) << 16.3509f; // 17° East
  QTest::newRow("Natash YNA") << Pos(-61.7809, 50.1836) << -19.1826f; // 21° West
  QTest::newRow("Nosy-Be NSB") << Pos(48.3233, -13.3056) << -9.51628f; // 9° West
  QTest::newRow("Waypoint GOMER") << Pos(-17.3333435058594, 28.0) << -5.15441f;

  QTest::newRow("Atlantic 2") << Pos(0.001f, 0.001f) << -4.75158f;
  QTest::newRow("Atlantic 3") << Pos(-0.001f, -0.001f) <<  -4.75158f;
}

void MagdecTest::testWmmTool()
{
  QFETCH(Pos, pos);
  QFETCH(float, magvar);

  qDebug() << pos << magvar;
  QCOMPARE(tool.getMagVar(pos), magvar);
}

void MagdecTest::testWmm()
{
  QFile outFile2("wmm.txt");
  if(outFile2.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&outFile2);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(2);

    // Same output as wmm_grid program
    int i = 0;
    for(float latY = -90.f; latY <= 90.f; latY += 1.f)
    {
      for(float lonX = -180.f; lonX < 180.f; lonX += 1.f)
      {
        out << i++ << " latY " << latY << " lonX " << lonX
            << " decl " << tool.getMagVar(atools::geo::Pos(lonX, latY)) << endl;
      }
    }
  }
#if defined(Q_OS_WIN32)
  QCOMPARE(outFile2.size(), 2688622);
#else
  QCOMPARE(outFile2.size(), 2623612);
#endif
}
