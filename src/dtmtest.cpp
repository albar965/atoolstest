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

#include "dtmtest.h"

#include "fs/common/globereader.h"
#include "geo/pos.h"
#include "geo/linestring.h"
#include "geo/rect.h"
#include "geo/calculations.h"

using atools::fs::common::GlobeReader;
using atools::geo::Pos;
using atools::geo::LineString;

static QString TEST_DATA("/data/alex/Public/Global Land One-kilometer Base Elevation (GLOBE)/all10");

DtmTest::DtmTest()
{

}

void DtmTest::runtest(int argc, char *argv[])
{
  DtmTest tst;
  QTest::qExec(&tst, argc, argv);
}

void DtmTest::initTestCase()
{
}

void DtmTest::cleanupTestCase()
{
}

void DtmTest::testOpen()
{
  GlobeReader reader(QFINDTESTDATA(TEST_DATA));

  QCOMPARE(reader.openFiles(), true);
}

void DtmTest::testElevation_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("elevation");

  QTest::newRow("Atlantic") << Pos(0.f, 0.f) << atools::fs::common::OCEAN;
  // QTest::newRow("Frankfurt") << Pos(8.5704f, 50.0333f) << 104.f;
  // QTest::newRow("Mount Everest") << Pos(86.925278, 27.988056) << 8752.f;
  // QTest::newRow("Metzada") << Pos(35.3917, 31.3317) << -384.f;
}

void DtmTest::testElevationLine_data()
{
  QTest::addColumn<LineString>("line");
  QTest::addColumn<float>("minelevation");
  QTest::addColumn<float>("maxelevation");

  QTest::newRow("Frankfurt - Mount Everest") <<
    LineString(Pos(8.5704f, 50.0333f), Pos(86.925278, 27.988056)) << -42.f << 8752.f;

  QTest::newRow("Frankfurt - Ushuaia") <<
    LineString(Pos(8.5704f, 50.0333f), Pos(68.2842, -54.8380)) << -500.f << 2865.f;

  QTest::newRow("LGAV - KLAX") <<
    LineString(Pos(23.9445, 37.9366), Pos(-118.4070, 33.9424)) << -500.f << 3459.f;
}

void DtmTest::testElevation()
{
  GlobeReader reader(QFINDTESTDATA(TEST_DATA));
  reader.openFiles();

  QFETCH(Pos, pos);
  QFETCH(float, elevation);

  QCOMPARE(reader.getElevation(pos), elevation);
}

void DtmTest::testElevationLine()
{
  GlobeReader reader(QFINDTESTDATA(TEST_DATA));
  reader.openFiles();
  LineString elevations;

  QFETCH(LineString, line);
  QFETCH(float, minelevation);
  QFETCH(float, maxelevation);

  QElapsedTimer tmr;
  tmr.start();
  reader.getElevations(elevations, line);
  qDebug() << tmr.elapsed();

  qDebug() << "Num points" << elevations.size();
  float min = std::numeric_limits<float>::max(), max = std::numeric_limits<float>::min();
  for(const Pos& p : elevations)
  {
    if(p.getAltitude() < min)
      min = p.getAltitude();
    if(p.getAltitude() > max)
      max = p.getAltitude();
  }

  QCOMPARE(min, minelevation);
  QCOMPARE(max, maxelevation);
}

void DtmTest::addData(const atools::geo::Pos& pos, qint64 fileOffset, int fileIndex)
{
  QTest::newRow(qPrintable(pos.toString())) << pos << fileOffset << fileIndex;
}

void DtmTest::testFilePos_data()
{
  QTest::addColumn<Pos>("filepos");
  QTest::addColumn<qint64>("fileoffset");
  QTest::addColumn<int>("fileindex");

  addData(Pos(-180.f, -90.f), 0, 0);
  addData(Pos(180.f, 90.f), 0, 0);
  addData(Pos(179.99996f, 90.f), GlobeReader::TILE_COLUMNS * 2 - 2, 3);
  addData(Pos(89.999996f, -50.00001f), 10800 * 2 - 2, 14);
  addData(Pos(179.999996f, -89.999f), GlobeReader::FILE_SIZE_SMALL - GlobeReader::TILE_COLUMNS * 2, 12);
  addData(Pos(89.999996f, -0.00001f), 10800 * 2 - 2, 10);
  addData(Pos(89.999996f, 50.00001f), GlobeReader::FILE_SIZE_SMALL - 2, 2);
  addData(Pos(89.999996f, 0.00001f), GlobeReader::FILE_SIZE_LARGE - 2, 6);
  addData(Pos(89.999996f, -0.00001f), 10800 * 2 - 2, 10);
  addData(Pos(-135.f, 90.f), 10800, 0);
  addData(Pos(-180.f, 90.f), 0, 0);
  addData(Pos(0.f, 0.f), 0, 10);
  addData(Pos(45.f, 0.f), 10800, 10);

}

void DtmTest::testFilePos()
{
  GlobeReader reader(QFINDTESTDATA(TEST_DATA));
  reader.openFiles();

  int fileIndexResult;

  QFETCH(Pos, filepos);
  QFETCH(qint64, fileoffset);
  QFETCH(int, fileindex);

  QCOMPARE(reader.calcFileOffset(filepos, fileIndexResult), fileoffset);
  QCOMPARE(fileIndexResult, fileindex);
}
