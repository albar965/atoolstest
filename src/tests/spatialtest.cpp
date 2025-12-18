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

#include "spatialtest.h"

#include "geo/pos.h"
#include "geo/calculations.h"
#include "util/csvreader.h"
#include "geo/spatialindex.h"
#include "exception.h"

using atools::geo::Pos;
using atools::geo::SpatialIndex;
using atools::geo::Point3D;

SpatialTest::SpatialTest()
{
  index = new SpatialIndex<Data>;
}

SpatialTest::~SpatialTest()
{
  delete index;
}

void SpatialTest::runtest(int argc, char *argv[])
{
  SpatialTest tst;
  QTest::qExec(&tst, argc, argv);
}

void SpatialTest::initTestCase()
{
  // Read test case file
  QString filepath = "testdata/vor.csv";
  int numImported = 0;
  QFile file(filepath);
  if(file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    atools::util::CsvReader reader;
    int idIndex = -1, identIndex = -1, regionIndex = -1, lonIndex = -1, latIndex = -1;

    QTextStream stream(&file);

    int lineNum = 0;
    while(!stream.atEnd())
    {
      QString line = stream.readLine();

      // Skip empty lines but add them if within an escaped field
      if(line.isEmpty() && !reader.isInEscape())
        continue;

      reader.readCsvLine(line);
      if(reader.isInEscape())
        // Still in an escaped line so continue to read unchanged until " shows the end of the field
        continue;

      const QStringList& values = reader.getValues();

      if(lineNum == 0)
      {
        for(int i = 0; i < values.size(); i++)
        {
          // Detect columns by names automatically
          const QString& val = values.at(i);
          if(val.endsWith("_id"))
            idIndex = i;
          else if(val == "ident")
            identIndex = i;
          else if(val == "region")
            regionIndex = i;
          else if(val == "lonx")
            lonIndex = i;
          else if(val == "laty")
            latIndex = i;
        }
      }
      else
      {
        if(lonIndex < 0 || latIndex < 0)
          throw atools::Exception("Index for lat/lon columns not found");

        Data data;
        data.pos = Pos(values.at(lonIndex).toFloat(), values.at(latIndex).toFloat());
        if(idIndex != -1)
          data.id = values.at(idIndex).toInt();
        if(identIndex != -1)
          data.ident = values.at(identIndex);
        if(regionIndex != -1)
          data.ident += "/" + values.at(regionIndex);

        index->append(data);
      }

      lineNum++;
      numImported++;
    }
    file.close();
  }
  else
    throw atools::Exception(QString("Cannot open file \"%1\". Reason: %2.").arg(filepath).arg(file.errorString()));

  index->updateIndex();
}

void SpatialTest::cleanupTestCase()
{
}

void SpatialTest::printObject(int idx, const Data& data, const atools::geo::Pos& origin)
{
  qDebug() << idx
           << "id" << data.id << data.pos << data.ident
           << atools::geo::meterToNm(origin.distanceMeterTo(data.pos)) << "nm";
}

void SpatialTest::printObjects(const QList<Data>& objects, const atools::geo::Pos& origin)
{
  // Print a limited number of objects with a ellipse between start and end block
  qDebug() << "size" << objects.size();
  if(objects.size() > 20)
  {
    for(int i = 0; i < 10; i++)
      printObject(i, objects.at(i), origin);
    qDebug() << "...";
    for(int i = objects.size() - 10; i < objects.size(); i++)
      printObject(i, objects.at(i), origin);
  }
  else
  {
    for(int i = 0; i < objects.size(); i++)
      printObject(i, objects.at(i), origin);
  }
}

void SpatialTest::testSpatialNearest_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<int>("number");
  QTest::addColumn<int>("result");

  QTest::newRow("Pos(0.f, 0.f)") << Pos(0.f, 0.f) << 10 << 10;
  QTest::newRow("Pos(90.f, 0.f)") << Pos(90.f, 0.f) << 10 << 10;
  QTest::newRow("Pos(-90.f, 0.f)") << Pos(-90.f, 0.f) << 10 << 10;
  QTest::newRow("Pos(180.f, 0.f)") << Pos(180.f, 0.f) << 10 << 10;
  QTest::newRow("Pos(-180.f, 0.f)") << Pos(-180.f, 0.f) << 10 << 10;
  QTest::newRow("Pos(10.7952, 50.1669)") << Pos(10.7952, 50.1669) << 10 << 10;
}

void SpatialTest::testSpatialNearest()
{
  QFETCH(Pos, pos);
  QFETCH(int, number);
  QFETCH(int, result);

  QElapsedTimer timer;
  timer.start();

  QList<Data> objects;
  index->getNearest(objects, pos, number);
  qDebug() << Q_FUNC_INFO << timer.restart() << "ms";

  printObjects(objects, pos);

  QCOMPARE(objects.size(), result);
}

void SpatialTest::testSpatialRadius_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("radius");
  QTest::addColumn<int>("result");

  QTest::newRow("Pos(0.f, 0.f)") << Pos(0.f, 0.f) << 8000.f << 2849;
  QTest::newRow("Pos(10.7952, 50.1669)") << Pos(10.7952f, 50.1669f) << 500.f << 302;
  QTest::newRow("Pos(8.165, 50.255) TAU/ED") << Pos(8.165f, 50.255f) << 0.5f << 1;
  QTest::newRow("Pos(-122.3096,47.4354) SEA/K1") << Pos(-122.31f, 47.435f) << 0.5f << 1;
}

void SpatialTest::testSpatialRadius()
{
  QFETCH(Pos, pos);
  QFETCH(float, radius);
  QFETCH(int, result);

  QElapsedTimer timer;
  timer.start();

  QList<Data> objects;
  index->getRadius(objects, pos, atools::geo::nmToMeter(radius));
  qDebug() << Q_FUNC_INFO << timer.restart() << "ms";

  printObjects(objects, pos);

  QCOMPARE(objects.size(), result);
}

void SpatialTest::testSpatialRadiusDest_data()
{
  QTest::addColumn<Pos>("origin");
  QTest::addColumn<float>("radiusMax");
  QTest::addColumn<int>("result");

  QTest::newRow("Pos(10.7952, 50.1669), 200 nm") << Pos(10.7952f, 50.1669f) << 200.f << 70;
  QTest::newRow("Pos(10.7952, 50.1669), 500 nm") << Pos(10.7952f, 50.1669f) << 500.f << 302;
  QTest::newRow("Pos(10.7952, 50.1669), 800 nm") << Pos(10.7952f, 50.1669f) << 800.f << 532;
  QTest::newRow("Pos(10.7952, 50.1669), 2000 nm") << Pos(10.7952f, 50.1669f) << 2000.f << 1184;
  QTest::newRow("Pos(0.f, 0.f), 8000 nm") << Pos(0.f, 0.f) << 8000.f << 2849;
  QTest::newRow("Pos(10.7952, 50.1669), 8000 nm") << Pos(10.7952, 50.1669) << 8000.f << 4198;
}

void SpatialTest::testSpatialRadiusDest()
{
  QFETCH(Pos, origin);
  QFETCH(float, radiusMax);
  QFETCH(int, result);

  QElapsedTimer timer;
  timer.start();

  QList<Data> objects;
  index->getRadius(objects, origin, atools::geo::nmToMeter(radiusMax));
  qDebug() << Q_FUNC_INFO << timer.restart() << "ms";

  printObjects(objects, origin);

  QCOMPARE(objects.size(), result);
}

void SpatialTest::testCartesianDist_data()
{
  QTest::addColumn<Pos>("from");
  QTest::addColumn<Pos>("to");

  QTest::newRow("Pos(0.f, 0.f) << Pos(0.f, 0.f)") << Pos(0.f, 0.f) << Pos(0.f, 0.f);
  QTest::newRow("Pos(0.f, 0.f) << Pos(0.f, 1.f)") << Pos(0.f, 0.f) << Pos(0.f, 1.f);
  QTest::newRow("Pos(0.f, 0.f) << Pos(1.f, 0.f)") << Pos(0.f, 0.f) << Pos(1.f, 0.f);
  QTest::newRow("Pos(0.f, 0.f) << Pos(1.f, 1.f)") << Pos(0.f, 0.f) << Pos(1.f, 1.f);

  QTest::newRow("Pos(0.f, 0.f) << Pos(0.f, 45.f)") << Pos(0.f, 0.f) << Pos(0.f, 45.f);

  QTest::newRow("Pos(0.f, 0.f) << Pos(90.f, 0.f)") << Pos(0.f, 0.f) << Pos(90.f, 0.f);
  QTest::newRow("Pos(0.f, 0.f) << Pos(-90.f, 0.f)") << Pos(0.f, 0.f) << Pos(-90.f, 0.f);

  QTest::newRow("Pos(0.f, 0.f) << Pos(180.f, 0.f)") << Pos(0.f, 0.f) << Pos(180.f, 0.f);
  QTest::newRow("Pos(0.f, 0.f) << Pos(-180.f, 0.f)") << Pos(0.f, 0.f) << Pos(-180.f, 0.f);

  QTest::newRow("Pos(0.f, 0.f) << Pos(0.f, 90.f)") << Pos(0.f, 0.f) << Pos(0.f, 90.f);
}

void SpatialTest::testCartesianDist()
{
  QFETCH(Pos, from);
  QFETCH(Pos, to);

  float gcDistance = from.distanceMeterTo(to);

  float x1, y1, z1, x2, y2, z2;
  from.toCartesian(x1, y1, z1);
  to.toCartesian(x2, y2, z2);

  Point3D p1(x1, y1, z1);
  Point3D p2(x2, y2, z2);
  float directDistance3D = p1.directDistanceMeter(p2);
  float gcDistance3D = p1.gcDistanceMeter(p2);

  qDebug() << "gcDistance3D" << atools::geo::meterToNm(gcDistance3D)
           << "directDistance3D" << atools::geo::meterToNm(directDistance3D)
           << "gcDistance" << atools::geo::meterToNm(gcDistance) << "nm";

  qDebug() << Qt::fixed << qSetRealNumberPrecision(0)
           << "gcDistance3D" << gcDistance3D
           << "directDistance3D" << directDistance3D
           << "gcDistance" << gcDistance << "meter";

  QCOMPARE(atools::almostEqual(gcDistance3D, gcDistance, 5.f), true);
  // QCOMPARE(atools::almostEqual(directDistance3D, gcDistance, std::max(directDistance3D, gcDistance) * 0.1f), true);
}
