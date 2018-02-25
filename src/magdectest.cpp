/*****************************************************************************
* Copyright 2015-2018 Alexander Barthel albar965@mailbox.org
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
#include "geo/pos.h"

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
}

void MagdecTest::cleanupTestCase()
{
}

void MagdecTest::testOpen()
{
  MagDecReader reader;
  reader.readFromBgl(":/test/resources/magdec.bgl");

  QCOMPARE(reader.isValid(), true);
}

void MagdecTest::testMagdec_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("magvar");

  QTest::newRow("Waypoint GOMER") << Pos(-17.3333435058594, 28.0) << -5.5838f;

  QTest::newRow("First Offset") << Pos(0.f, -90.f) << -30.3607f;
  QTest::newRow("Last Offset") << Pos(-1.f, 90.f) << -3.59802f;
  QTest::newRow("Atlantic") << Pos(0.f, 0.f) << -5.16357f;
  QTest::newRow("Atlantic 2") << Pos(0.001f, 0.001f) << -4.81784f;
  QTest::newRow("Atlantic 3") << Pos(-0.001f, -0.001f) << -5.16466f;
  QTest::newRow("Vancouver YVR") << Pos(-123.1491, 49.0773) << 16.4714f; // 17° East
  QTest::newRow("Natash YNA") << Pos(-61.7809, 50.1836) << -19.6735f; // 21° West
  QTest::newRow("Nosy-Be NSB") << Pos(48.3233, -13.3056) << -9.47886f; // 9° West

  QTest::newRow("West Min") << Pos(-180.f, 0.f) << 9.61853f;
  QTest::newRow("East Max") << Pos(180.f, 0.f) << 9.61853f;
  QTest::newRow("North") << Pos(0.f, 90.f) << -2.59827f;
  QTest::newRow("South") << Pos(0.f, -90.f) << -30.3607f;

  QTest::newRow("West North") << Pos(-180.f, 90.f) << 177.402f;
  QTest::newRow("West South") << Pos(-180.f, -90.f) << 149.639f;
  QTest::newRow("East North") << Pos(180.f, 90.f) << 177.402f;
  QTest::newRow("East South") << Pos(180.f, -90.f) << 149.639f;
}

void MagdecTest::testMagdec()
{
  MagDecReader reader;
  reader.readFromBgl(":/test/resources/magdec.bgl");

  QFETCH(Pos, pos);
  QFETCH(float, magvar);

  QCOMPARE(reader.getMagVar(pos), magvar);
}
