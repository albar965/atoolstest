/*****************************************************************************
* Copyright 2015-2020 Alexander Barthel alex@littlenavmap.org
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

#include "geotest.h"

#include "geo/pos.h"
#include "geo/linestring.h"
#include "geo/rect.h"
#include "geo/calculations.h"
#include "fs/util/coordinates.h"
#include "fs/common/binarygeometry.h"
#include "fs/common/binarymsageometry.h"
#include "geo/line.h"

using atools::geo::Pos;
using atools::geo::Rect;
using atools::geo::Line;
using atools::geo::LineString;
using atools::geo::LineDistance;
using atools::geo::angleAbsDiff;
using atools::geo::angleAbsDiff2;
using atools::geo::angleAbsDiffSign;
using atools::geo::nmToMeter;
using atools::geo::meterToNm;
using atools::geo::feetToNm;

namespace QTest {
template<>
char *toString(const Pos& pos)
{
  return qstrdup(pos.toString().toLocal8Bit());
}

template<>
char *toString(const Rect& rect)
{
  return qstrdup(rect.toString().toLocal8Bit());
}

}

GeoTest::GeoTest()
{

}

void GeoTest::runtest(int argc, char *argv[])
{
  GeoTest tst;
  QTest::qExec(&tst, argc, argv);
}

void GeoTest::initTestCase()
{
}

void GeoTest::cleanupTestCase()
{

}

void GeoTest::testScale()
{
  QCOMPARE(Rect(0.f, 0.f).scale(1.0f, 1.0f), Rect(0.f, 0.f));
  QCOMPARE(Rect(-10.f, 10.f, 10.f, -10.f).scale(1.0f, 1.0f), Rect(-10.f, 10.f, 10.f, -10.f));
  QCOMPARE(Rect(-10.f, 10.f, 10.f, -10.f).scale(2.0f, 2.0f), Rect(-20.f, 20.f, 20.f, -20.f));
  QCOMPARE(Rect(-10.f, 10.f, 10.f, -10.f).scale(0.1f, 0.1f), Rect(-1.f, 1.f, 1.f, -1.f));

  QCOMPARE(Rect(170.f, 10.f, -170.f, -10.f).scale(1.0f, 1.0f), Rect(170.f, 10.f, -170.f, -10.f));
  QCOMPARE(Rect(170.f, 10.f, -170.f, -10.f).scale(2.0f, 2.0f), Rect(160.f, 20.f, -160.f, -20.f));
  QCOMPARE(Rect(170.f, 10.f, -170.f, -10.f).scale(0.1f, 0.1f), Rect(179.f, 1.f, -179.f, -1.f));
}

void GeoTest::testCenter()
{
  QCOMPARE(Rect(0.f, 0.f).getCenter(), Pos(0.f, 0.f));
  QCOMPARE(Rect(-10.f, 10.f).getCenter(), Pos(-10.f, 10.f));
  QCOMPARE(Rect(-10.f, 10.f, 10.f, -10.f).getCenter(), Pos(0.f, 0.f));
  QCOMPARE(Rect(-10.f, 10.f, 0.f, 0.f).getCenter(), Pos(-5.f, 5.f));
  QCOMPARE(Rect(0.f, 10.f, 10.f, 0.f).getCenter(), Pos(5.f, 5.f));
  QCOMPARE(Rect(160.f, 10.f, -160.f, -10.f).getCenter(), Pos(-180.f, 0.f));
  QCOMPARE(Rect(150.f, 10.f, -170.f, -10.f).getCenter(), Pos(170.f, 0.f));
  QCOMPARE(Rect(170.f, 10.f, -150.f, -10.f).getCenter(), Pos(-170.f, 0.f));

  QCOMPARE(Rect(-100.f, 10.f, 100.f, -10.f).getCenter(), Pos(0.f, 0.f));
  QCOMPARE(Rect(-80.f, 10.f, 80.f, -10.f).getCenter(), Pos(0.f, 0.f));

  QCOMPARE(Rect(100.f, 10.f, -100.f, -10.f).getCenter(), Pos(-180.f, 0.f));
  QCOMPARE(Rect(80.f, 10.f, -80.f, -10.f).getCenter(), Pos(-180.f, 0.f));
}

void GeoTest::testCartesian_data()
{
  /* *INDENT-OFF* */
  //   | Z
  //   |
  //   |___ Y
  //  /
  // / X
  /* *INDENT-ON* */

  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("x"); // in/out equator plain
  QTest::addColumn<float>("y"); // left/right equator plain
  QTest::addColumn<float>("z"); // up/down

  QTest::newRow("Pos(0.f, 0.f)") << Pos(0.f, 0.f) << 6371000.f << 0.f << 0.f;
  QTest::newRow("Pos(90.f, 0.f)") << Pos(90.f, 0.f) << 0.f << 6371000.f << 0.f;
  QTest::newRow("Pos(-90.f, 0.f)") << Pos(-90.f, 0.f) << 0.f << -6371000.f << 0.f;

  QTest::newRow("Pos(180.f, 0.f)") << Pos(180.f, 0.f) << -6371000.f << 0.f << 0.f;
  QTest::newRow("Pos(-180.f, 0.f)") << Pos(-180.f, 0.f) << -6371000.f << 0.f << 0.f;

  QTest::newRow("Pos(0.f, 90.f)") << Pos(0.f, 90.f) << 0.f << 0.f << 6371000.f;
  QTest::newRow("Pos(0.f, -90.f)") << Pos(0.f, -90.f) << 0.f << 0.f << -6371000.f;

  QTest::newRow("Pos(0.f, 45.f)") << Pos(0.f, 45.f) << 4504980.f << 0.f << 4504980.f;
  QTest::newRow("Pos(0.f, -45.f)") << Pos(0.f, -45.f) << 4504980.f << 0.f << -4504980.f;

  QTest::newRow("Pos(45.f, 0.f)") << Pos(45.f, 0.f) << 4504980.f << 4504980.f << 0.f;
  QTest::newRow("Pos(-45.f, 0.f)") << Pos(-45.f, 0.f) << 4504980.f << -4504980.f << 0.f;

  QTest::newRow("Pos(135.f, 0.f)") << Pos(135.f, 0.f) << -4504980.f << 4504980.f << 0.f;
  QTest::newRow("Pos(-135.f, 0.f)") << Pos(-135.f, 0.f) << -4504980.f << -4504980.f << 0.f;
}

void GeoTest::testCartesian()
{
  QFETCH(Pos, pos);
  QFETCH(float, x);
  QFETCH(float, y);
  QFETCH(float, z);

  float xResult, yResult, zResult;
  pos.toCartesian(xResult, yResult, zResult);
  qDebug() << pos << xResult << yResult << zResult;

  // Add one for qFuzzyCompare where the two numbers are compared in a relative way,
  // where the exactness is stronger the smaller the numbers are.
  QCOMPARE(xResult + 1.f, x + 1.f);
  QCOMPARE(yResult + 1.f, y + 1.f);
  QCOMPARE(zResult + 1.f, z + 1.f);
}

void GeoTest::testAntiMeridian_data()
{
  QTest::addColumn<float>("lonx1");
  QTest::addColumn<float>("lonx2");
  QTest::addColumn<bool>("result");

  QTest::newRow("-180 to -170") << -180.f << -170.f << false;
  QTest::newRow("170 to 180") << 170.f << 180.f << false;

  QTest::newRow("180 to -170") << 180.f << -170.f << false;
  QTest::newRow("170 to -180") << 170.f << -180.f << false;
  QTest::newRow("-180 to 180") << -180.f << 180.f << false;

  QTest::newRow("-177 to 177") << -177.f << 177.f << true;
  QTest::newRow("-91 to 91") << -91.f << 91.f << true
  ;
  QTest::newRow("-90 to 90") << -90.f << 90.f << false;
  QTest::newRow("-89 to 89") << -89.f << 89.f << false;
  QTest::newRow("-45 to 45") << -45.f << 45.f << false;
  QTest::newRow("-1 to 1") << -1.f << 1.f << false;
  QTest::newRow("0 to 0") << 0.f << 0.f << false;
}

void GeoTest::testAntiMeridian()
{
  QFETCH(float, lonx1);
  QFETCH(float, lonx2);
  QFETCH(bool, result);

  QCOMPARE(atools::geo::crossesAntiMeridian(lonx1, lonx2), result);
  QCOMPARE(atools::geo::crossesAntiMeridian(lonx2, lonx1), result);
}

void GeoTest::testDistance_data()
{
  QTest::addColumn<Pos>("pos1");
  QTest::addColumn<Pos>("pos2");
  QTest::addColumn<float>("result");
  QTest::addColumn<float>("result3d");

  QTest::newRow("") << Pos(8.f, 49.f, 10000.f) << Pos(8.f, 49.f, 40000.f) << 0.f << feetToNm(30000.f);
  QTest::newRow("") << Pos(8.f, 49.f, 10000.f) << Pos(8.f, 50.f, 10000.f) << 60.0335f << 60.0335f;

  QTest::newRow("") << Pos(8.f, 49.f, Pos::INVALID_VALUE) << Pos(8.f, 50.f, 10000.f) << 60.0335f << 60.0335f;
  QTest::newRow("") << Pos(8.f, 49.f, 10000.f) << Pos(8.f, 50.f, Pos::INVALID_VALUE) << 60.0335f << 60.0335f;

  QTest::newRow("") << Pos(8.f, 49.f, 10000.f) << Pos(8.f, 50.f, 40000.f) << 60.0335f
                    << std::sqrt(feetToNm(30000.f) * feetToNm(30000.f) + 60.0335f * 60.0335f);
}

void GeoTest::testDistance()
{
  QFETCH(Pos, pos1);
  QFETCH(Pos, pos2);
  QFETCH(float, result);
  QFETCH(float, result3d);

  QCOMPARE(meterToNm(pos1.distanceMeterTo(pos2)), result);
  QCOMPARE(meterToNm(pos1.distanceMeterTo3d(pos2)), result3d);
}

void GeoTest::testCourse_data()
{
  QTest::addColumn<float>("lonx1");
  QTest::addColumn<float>("lonx2");
  QTest::addColumn<bool>("west");
  QTest::addColumn<bool>("east");
  const bool WEST = true, EAST = true;

  QTest::newRow("177 to -177") << 177.f << -177.f << false << EAST;

  QTest::newRow("170 to -180") << 170.f << -180.f << false << EAST;
  QTest::newRow("170 to 180") << 170.f << 180.f << false << EAST;
  QTest::newRow("-170 to 180") << -170.f << 180.f << WEST << false;
  QTest::newRow("-170 to -180") << -170.f << -180.f << WEST << false;

  QTest::newRow("180 to -170") << 180.f << -170.f << false << EAST;
  QTest::newRow("-180 to -170") << -180.f << -170.f << false << EAST;
  QTest::newRow("180 to 170") << 180.f << 170.f << WEST << false;
  QTest::newRow("-180 to 170") << -180.f << 170.f << WEST << false;

  QTest::newRow("-90 to 90") << -90.f << 90.f << false << false;
  QTest::newRow("90 to -90") << 90.f << -90.f << false << false;

  QTest::newRow("-180 to 180") << -180.f << 180.f << false << false;
  QTest::newRow("180 to -180") << 180.f << -180.f << false << false;
  QTest::newRow("0 to 0") << 0.f << 0.f << false << false;

  QTest::newRow("-177 to 177") << -177.f << 177.f << WEST << false;

  QTest::newRow("-91 to 91") << -91.f << 91.f << WEST << false;
  QTest::newRow("91 to -91") << 91.f << -91.f << false << EAST;

  QTest::newRow("89 to -89") << 89.f << -89.f << WEST << false;
  QTest::newRow("-89 to 89") << -89.f << 89.f << false << EAST;

  QTest::newRow("-45 to 45") << -45.f << 45.f << false << EAST;
  QTest::newRow("45 to -45") << 45.f << -45.f << WEST << false;

  QTest::newRow("-1 to 1") << -1.f << 1.f << false << EAST;
  QTest::newRow("1 to -1") << 1.f << -1.f << WEST << false;
}

void GeoTest::testCourse()
{
  QFETCH(float, lonx1);
  QFETCH(float, lonx2);
  QFETCH(bool, west);
  QFETCH(bool, east);

  QCOMPARE(atools::geo::isWestCourse(lonx1, lonx2), west);
  QCOMPARE(atools::geo::isEastCourse(lonx1, lonx2), east);
}

void GeoTest::testAngleDiff_data()
{
  QTest::addColumn<float>("angle1");
  QTest::addColumn<float>("angle2");
  QTest::addColumn<float>("result");
  QTest::addColumn<float>("result2");
  QTest::addColumn<float>("resultsign");

  QTest::newRow("(0.f, 0.f), 0.f)") << 0.f << 0.f << 0.f << 0.f << 0.f;
  QTest::newRow("(180.f, 180.f), 0.f)") << 180.f << 180.f << 0.f << 0.f << 0.f;
  QTest::newRow("(360.f, 360.f), 0.f)") << 360.f << 360.f << 0.f << 0.f << 0.f;

  QTest::newRow("(0.f, 360.f), 0.f)") << 0.f << 360.f << 0.f << 360.f << 0.f;
  QTest::newRow("(360.f, 0.f), 0.f)") << 360.f << 0.f << 0.f << 0.f << 0.f;

  QTest::newRow("(0.f, 180.f), 180.f)") << 0.f << 180.f << 180.f << 180.f << 180.f;
  QTest::newRow("(180.f, 0.f), 180.f)") << 180.f << 0.f << 180.f << 180.f << -180.f;

  QTest::newRow("(359.f, 1.f), 2.f)") << 359.f << 1.f << 2.f << 2.f << 2.f;
  QTest::newRow("(1.f, 359.f), 2.f)") << 1.f << 359.f << 2.f << 358.f << -2.f;

  QTest::newRow("(350.f, 10.f), 20.f)") << 350.f << 10.f << 20.f << 20.f << 20.f;
  QTest::newRow("(10.f, 350.f), 20.f)") << 10.f << 350.f << 20.f << 340.f << -20.f;

  QTest::newRow("(179.f, 181.f), 2.f)") << 179.f << 181.f << 2.f << 2.f << 2.f;
  QTest::newRow("(181.f, 179.f), 2.f)") << 181.f << 179.f << 2.f << 358.f << -2.f;

  QTest::newRow("(360.f, 180.f), 180.f)") << 360.f << 180.f << 180.f << 180.f << -180.f;
  QTest::newRow("(360.f, 0.f), 0.f)") << 360.f << 0.f << 0.f << 0.f << 0.f;

  QTest::newRow("(89.f, 271.f), 178.f)") << 89.f << 271.f << 178.f << 182.f << -178.f;
  QTest::newRow("(91.f, 269.f), 178.f)") << 91.f << 269.f << 178.f << 178.f << 178.f;

  QTest::newRow("(271.f, 89.f), 178.f)") << 271.f << 89.f << 178.f << 178.f << 178.f;
  QTest::newRow("(269.f, 91.f), 178.f)") << 269.f << 91.f << 178.f << 182.f << -178.f;

  QTest::newRow("(90.f, 270.f), 180.f)") << 90.f << 270.f << 180.f << 180.f << 180.f;
  QTest::newRow("(89.f, 269.f), 180.f)") << 89.f << 269.f << 180.f << 180.f << 180.f;
  QTest::newRow("(91.f, 271.f), 180.f)") << 91.f << 271.f << 180.f << 180.f << 180.f;

  QTest::newRow("(270.f, 90.f), 180.f)") << 270.f << 90.f << 180.f << 180.f << -180.f;
  QTest::newRow("(269.f, 89.f), 180.f)") << 269.f << 89.f << 180.f << 180.f << -180.f;
  QTest::newRow("(271.f, 91.f), 180.f)") << 271.f << 91.f << 180.f << 180.f << -180.f;
}

void GeoTest::testAngleDiff()
{
  QFETCH(float, angle1);
  QFETCH(float, angle2);
  QFETCH(float, result);
  QFETCH(float, result2);
  QFETCH(float, resultsign);

  QCOMPARE(angleAbsDiff(angle1, angle2), result);
  QCOMPARE(angleAbsDiff2(angle1, angle2), result2);
  QCOMPARE(angleAbsDiffSign(angle1, angle2), resultsign);
}

void GeoTest::testLineParallel_data()
{
  QTest::addColumn<Line>("line");
  QTest::addColumn<float>("dist");
  QTest::addColumn<Line>("expected");

  // float longitudeX1, float latitudeY1, float longitudeX2, float latitudeY2
  QTest::newRow("") << Line(0.f, 1.f, 0.f, -1.f) << 0.f << Line(0.f, 1.f, 0.f, -1.f);
  QTest::newRow("South/north to right") << Line(0.f, -1.f, 0.f, 1.f) << nmToMeter(60.f)
                                        << Line(1.000152f, -0.999848f, 1.000152f, 0.999848f);
  QTest::newRow("South/north to left") << Line(0.f, -1.f, 0.f, 1.f) << nmToMeter(-60.f)
                                       << Line(-1.000152f, -0.999848f, -1.000152f, 0.999848f);

  QTest::newRow("North/south to left") << Line(0.f, 1.f, 0.f, -1.f) << nmToMeter(60.f)
                                       << Line(-1.000152f, 0.999848f, -1.000152f, -0.999848f);
  QTest::newRow("North/south to right") << Line(0.f, 1.f, 0.f, -1.f) << nmToMeter(-60.f)
                                        << Line(1.000152f, 0.999848f, 1.000152f, -0.999848f);

  QTest::newRow("NW SE to right") << Line(-1.f, 1.f, 1.f, -1.f) << nmToMeter(60.f)
                                  << Line(-1.707044f, 0.292781f, 0.292664f, -1.707067f);
  QTest::newRow("NW SE to left") << Line(-1.f, 1.f, 1.f, -1.f) << nmToMeter(-60.f)
                                 << Line(-0.292651f, 1.707067f, 1.707031f, -0.292781f);

  QTest::newRow("West/east down") << Line(9.f, 50.f, 11.f, 50.f) << nmToMeter(60.f)
                                  << Line(9.020386f, 49.000088f, 11.020386f, 49.000088f);
  QTest::newRow("West/east up") << Line(9.f, 50.f, 11.f, 50.f) << nmToMeter(-60.f)
                                << Line(8.978760f, 50.999908f, 10.978760f, 50.999908f);
}

void GeoTest::testLineParallel()
{
  QFETCH(Line, line);
  QFETCH(float, dist);
  QFETCH(Line, expected);

  Line result = line.parallel(dist);
  // qDebug() << line << dist << "result" << result << "expected" << expected;
  // qDebug() << line .angleDeg()<<result.angleDeg();
  qDebug() << line.lengthMeter() << result.lengthMeter();
  QCOMPARE(result.getPos1().getLonX(), expected.getPos1().getLonX());
  QCOMPARE(result.getPos1().getLatY(), expected.getPos1().getLatY());
  QCOMPARE(result.getPos2().getLonX(), expected.getPos2().getLonX());
  QCOMPARE(result.getPos2().getLatY(), expected.getPos2().getLatY());
  QCOMPARE(atools::almostEqual(line.angleDeg(), result.angleDeg(), 0.1f), true);
}

void GeoTest::testLineExtend_data()
{
  QTest::addColumn<Line>("line");
  QTest::addColumn<float>("dist1");
  QTest::addColumn<float>("dist2");
  QTest::addColumn<Line>("expected");

  // float longitudeX1, float latitudeY1, float longitudeX2, float latitudeY2
  QTest::newRow("") << Line(0.f, 1.f, 0.f, -1.f) << 0.f << 0.f << Line(0.f, 1.f, 0.f, -1.f);

  QTest::newRow("South/north extend") << Line(0.f, -1.f, 0.f, 1.f) << nmToMeter(60.f) << nmToMeter(60.f)
                                      << Line(0.f, -2.f, 0.f, 2.f);
  QTest::newRow("South/north shrink") << Line(0.f, -1.f, 0.f, 1.f) << nmToMeter(-30.f) << nmToMeter(-30.f)
                                      << Line(0.f, -0.5f, 0.f, 0.5f);

  QTest::newRow("North/south to extend") << Line(0.f, 1.f, 0.f, -1.f) << nmToMeter(60.f) << nmToMeter(60.f)
                                         << Line(0.f, 2.f, 0.f, -2.f);
  QTest::newRow("North/south to shrink") << Line(0.f, 1.f, 0.f, -1.f) << nmToMeter(-30.f) << nmToMeter(-30.f)
                                         << Line(0.f, 0.5f, 0.f, -0.5f);

  QTest::newRow("NW SE to right") << Line(-1.f, 1.f, 1.f, -1.f) << nmToMeter(60.f) << nmToMeter(60.f)
                                  << Line(-1.707457f, 1.706959f, 1.707458f, -1.706959f);
}

void GeoTest::testLineExtend()
{
  QFETCH(Line, line);
  QFETCH(float, dist1);
  QFETCH(float, dist2);
  QFETCH(Line, expected);

  Line result = line.extended(dist1, dist2);
  qDebug() << line << dist1 << dist2 << "result" << result << "expected" << expected;
  // qDebug() << line .angleDeg()<<result.angleDeg();
  qDebug() << line.lengthMeter() << result.lengthMeter();
  QCOMPARE(result.getPos1().getLonX(), expected.getPos1().getLonX());
  QCOMPARE(result.getPos1().getLatY(), expected.getPos1().getLatY());
  QCOMPARE(result.getPos2().getLonX(), expected.getPos2().getLonX());
  QCOMPARE(result.getPos2().getLatY(), expected.getPos2().getLatY());
  QCOMPARE(atools::almostEqual(line.angleDeg(), result.angleDeg(), 0.1f), true);

}

void GeoTest::testWindDrift_data()
{
  // float windCorrectedHeading(float windSpeed, float windDirectionDeg, float courseDeg, float trueAirspeed)
  QTest::addColumn<float>("heading");
  QTest::addColumn<float>("windSpeed");
  QTest::addColumn<float>("windDirectionDeg");
  QTest::addColumn<float>("courseDeg");
  QTest::addColumn<float>("trueAirspeed");

  QTest::newRow("Flying 0° wind from 0° impossible") << std::numeric_limits<float>::max()
                                                     << 100.f << 0.f << 0.f << 50.f;
  QTest::newRow("Flying 180° wind from 180°") << std::numeric_limits<float>::max()
                                              << 100.f << 180.f << 180.f << 99.9f;

  QTest::newRow("Flying 0° wind from 0° at 0") << std::numeric_limits<float>::max()
                                               << 0.f << 0.f << 0.f << 0.f;

  QTest::newRow("Flying 0° wind from 0° TAS 0") << std::numeric_limits<float>::max()
                                                << 10.f << 0.f << 0.f << 0.f;

  QTest::newRow("Flying 0° wind from 0°") << 0.f << 100.f << 0.f << 0.f << 200.f;
  QTest::newRow("Flying 180° wind from 0°") << 180.f << 100.f << 0.f << 180.f << 200.f;

  QTest::newRow("Flying 0° wind from 90°") << 30.f << 100.f << 90.f << 0.f << 200.f;
  QTest::newRow("Flying 180° wind from 90°") << 150.f << 100.f << 90.f << 180.f << 200.f;

  QTest::newRow("Flying 0° wind from 270°") << 330.f << 100.f << 270.f << 0.f << 200.f;
  QTest::newRow("Flying 180° wind from 270°") << 210.f << 100.f << 270.f << 180.f << 200.f;

  QTest::newRow("Flying 90° wind from 0°") << 60.f << 100.f << 0.f << 90.f << 200.f;
  QTest::newRow("Flying 270° wind from 0°") << 300.f << 100.f << 0.f << 270.f << 200.f;

  QTest::newRow("Flying 90° wind from 180°") << 120.f << 100.f << 180.f << 90.f << 200.f;
  QTest::newRow("Flying 270° wind from 180°") << 240.f << 100.f << 180.f << 270.f << 200.f;
}

void GeoTest::testWindDrift()
{
  // float windCorrectedHeading(float windSpeed, float windDirectionDeg, float courseDeg, float trueAirspeed)
  QFETCH(float, heading);
  QFETCH(float, windSpeed);
  QFETCH(float, windDirectionDeg);
  QFETCH(float, courseDeg);
  QFETCH(float, trueAirspeed);

  float hd = atools::geo::windCorrectedHeading(windSpeed, windDirectionDeg, courseDeg, trueAirspeed);

  QCOMPARE(hd, heading);
}

void GeoTest::testWindUV_data()
{
  QTest::addColumn<float>("v"); // north
  QTest::addColumn<float>("u"); // east
  QTest::addColumn<float>("speed");
  QTest::addColumn<float>("dir"); // Blowing from

  // U component of wind; eastward_wind
  // V component of wind; northward_wind

  // QTest::newRow("No wind") << 0.f << 0.f << 0.f << 0.f;
  QTest::newRow("Wind to North") << 10.f << 0.f << 10.f << 180.f;
  QTest::newRow("Wind to South") << -10.f << 0.f << 10.f << 0.f;
  QTest::newRow("Wind to East") << 0.f << 10.f << 10.f << 270.f;
  QTest::newRow("Wind to West") << 0.f << -10.f << 10.f << 90.f;
  QTest::newRow("Wind to North-East") << 10.f << 10.f << 14.1421f << 225.f;
  QTest::newRow("Wind to North-West") << 10.f << -10.f << 14.1421f << 135.f;
  QTest::newRow("Wind to South-West") << -10.f << -10.f << 14.1421f << 45.f;
  QTest::newRow("Wind to South-East") << -10.f << 10.f << 14.1421f << 315.f;
}

void GeoTest::testWindUV()
{
  // float windCorrectedHeading(float windSpeed, float windDirectionDeg, float courseDeg, float trueAirspeed)
  QFETCH(float, v);
  QFETCH(float, u);
  QFETCH(float, speed);
  QFETCH(float, dir); // Blowing from

  float speedCalculated = atools::geo::windSpeedFromUV(u, v);
  float dirCalculated = atools::geo::windDirectionFromUV(u, v);

  QCOMPARE(speedCalculated, speed);
  QCOMPARE(dirCalculated, dir);

  float vCalculated = atools::geo::windVComponent(speed, dir);
  float uCalculated = atools::geo::windUComponent(speed, dir);

  qDebug() << "dir" << dir << "speed" << speed
           << "vCalculated" << vCalculated << "v" << v << "uCalculated" << uCalculated << "u" << u;

  QCOMPARE(atools::almostEqual(vCalculated, v, 0.0001f), true);
  QCOMPARE(atools::almostEqual(uCalculated, u, 0.0001f), true);
}

void GeoTest::testNormalize()
{
  /* Normalize course to 0 < course < 360 */

  QCOMPARE(atools::geo::normalizeCourse(0.f), 0.f);
  QCOMPARE(atools::geo::normalizeCourse(360.f), 0.f);
  QCOMPARE(atools::geo::normalizeCourse(370.f), 10.f);
  QCOMPARE(atools::geo::normalizeCourse(720.f), 0.f);
  QCOMPARE(atools::geo::normalizeCourse(730.f), 10.f);
  QCOMPARE(atools::geo::normalizeCourse(710.f), 350.f);
  QCOMPARE(atools::geo::normalizeCourse(-10.f), 350.f);
  QCOMPARE(atools::geo::normalizeCourse(-350.f), 10.f);

  QCOMPARE(atools::geo::normalizeCourse(3610.f), 10.f);
  QCOMPARE(atools::geo::normalizeCourse(-3610.f), 350.f);
  QCOMPARE(atools::geo::normalizeCourse(std::numeric_limits<float>::max()), 0.f);
  QCOMPARE(atools::geo::normalizeCourse(std::numeric_limits<float>::lowest()), 0.f);

  QCOMPARE(atools::geo::normalizeCourse(0), 0);
  QCOMPARE(atools::geo::normalizeCourse(360), 0);
  QCOMPARE(atools::geo::normalizeCourse(370), 10);
  QCOMPARE(atools::geo::normalizeCourse(720), 0);
  QCOMPARE(atools::geo::normalizeCourse(730), 10);
  QCOMPARE(atools::geo::normalizeCourse(710), 350);
  QCOMPARE(atools::geo::normalizeCourse(-10), 350);
  QCOMPARE(atools::geo::normalizeCourse(-350), 10);
  QCOMPARE(atools::geo::normalizeCourse(3610), 10);
  QCOMPARE(atools::geo::normalizeCourse(-3610), 350);
  QCOMPARE(atools::geo::normalizeCourse(std::numeric_limits<int>::max()), 127);
  QCOMPARE(atools::geo::normalizeCourse(std::numeric_limits<int>::lowest()), 232);

  /* Normalize laty to -90 < laty < 90 */
  QCOMPARE(atools::geo::normalizeLatYDeg(0.f), 0.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(10.f), 10.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(90.f), -90.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(180.f), 0.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(-180.f), 0.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(370.f), 10.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(-370.f), -10.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(100.f), -80.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(-90.f), -90.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(-100.f), 80.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(910.f), 10.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(-910.f), -10.f);

  QCOMPARE(atools::geo::normalizeLatYDeg(0), 0);
  QCOMPARE(atools::geo::normalizeLatYDeg(10), 10);
  QCOMPARE(atools::geo::normalizeLatYDeg(90), -90);
  QCOMPARE(atools::geo::normalizeLatYDeg(180), 0);
  QCOMPARE(atools::geo::normalizeLatYDeg(-180), 0);
  QCOMPARE(atools::geo::normalizeLatYDeg(370), 10);
  QCOMPARE(atools::geo::normalizeLatYDeg(-370), -10);
  QCOMPARE(atools::geo::normalizeLatYDeg(100), -80);
  QCOMPARE(atools::geo::normalizeLatYDeg(-90), -90);
  QCOMPARE(atools::geo::normalizeLatYDeg(-100), 80);
  QCOMPARE(atools::geo::normalizeLatYDeg(910), 10);
  QCOMPARE(atools::geo::normalizeLatYDeg(-910), -10);

  /* Normalize lonx to -180 < lonx < 180 */
  QCOMPARE(atools::geo::normalizeLonXDeg(0.f), 0.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(10.f), 10.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(180.f), -180.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(360.f), 0.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(-360.f), 0.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(730.f), 10.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(-730.f), -10.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(190.f), -170.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(-190.f), 170.f);
  QCOMPARE(atools::geo::normalizeLonXDeg(-100.f), -100.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(1810.f), 10.f);
  QCOMPARE(atools::geo::normalizeLatYDeg(-1810.f), -10.f);

  QCOMPARE(atools::geo::normalizeLonXDeg(0), 0);
  QCOMPARE(atools::geo::normalizeLonXDeg(10), 10);
  QCOMPARE(atools::geo::normalizeLonXDeg(180), -180);
  QCOMPARE(atools::geo::normalizeLonXDeg(360), 0);
  QCOMPARE(atools::geo::normalizeLonXDeg(-360), 0);
  QCOMPARE(atools::geo::normalizeLonXDeg(730), 10);
  QCOMPARE(atools::geo::normalizeLonXDeg(-730), -10);
  QCOMPARE(atools::geo::normalizeLonXDeg(190), -170);
  QCOMPARE(atools::geo::normalizeLonXDeg(-190), 170);
  QCOMPARE(atools::geo::normalizeLonXDeg(-100), -100);
  QCOMPARE(atools::geo::normalizeLatYDeg(1810), 10);
  QCOMPARE(atools::geo::normalizeLatYDeg(-1810), -10);
}

void GeoTest::testInterpolateForX()
{
  QCOMPARE(atools::interpolateForX(QLineF(5, 100, 15, 200), 10), QPointF(10, 150));
  QCOMPARE(atools::interpolateForX(QLineF(5, 100, 15, 200), 5), QPointF(5, 100));
  QCOMPARE(atools::interpolateForX(QLineF(5, 100, 15, 200), 15), QPointF(15, 200));
  QCOMPARE(atools::interpolateForX(QLineF(5, 100, 15, 200), 7.5), QPointF(7.5, 125));
  QCOMPARE(atools::interpolateForX(QLineF(5, 100, 15, 200), 12.5), QPointF(12.5, 175));
}

void GeoTest::testInterpolateForY()
{
  QCOMPARE(atools::interpolateForY(QLineF(5, 100, 15, 200), 150), QPointF(10, 150));
  QCOMPARE(atools::interpolateForY(QLineF(5, 100, 15, 200), 100), QPointF(5, 100));
  QCOMPARE(atools::interpolateForY(QLineF(5, 100, 15, 200), 200), QPointF(15, 200));
  QCOMPARE(atools::interpolateForY(QLineF(5, 100, 15, 200), 125), QPointF(7.5, 125));
  QCOMPARE(atools::interpolateForY(QLineF(5, 100, 15, 200), 175), QPointF(12.5, 175));
}

void GeoTest::testInterpolate()
{
  QCOMPARE(atools::interpolate(100, 200, 5, 15, 10), 150);
  QCOMPARE(atools::interpolate(100., 200., 5., 15., 7.5), 125.);
  QCOMPARE(atools::interpolate(100., 200., 5., 15., 12.5), 175.);
  QCOMPARE(atools::interpolate(100, 200, 5, 15, 5), 100);
  QCOMPARE(atools::interpolate(100, 200, 5, 15, 15), 200);
}

void GeoTest::testAngleQuad1()
{
  float x = 0.f, y = 0.f;
  Pos pos(x, y);

  QCOMPARE(pos.angleDegTo(Pos(x, y)), std::numeric_limits<float>::max());
  QCOMPARE(pos.angleDegTo(Pos(x, y + 1.f)), 0.f);
  QCOMPARE(pos.angleDegTo(Pos(x, x - 1.f)), 180.f);
  QCOMPARE(pos.angleDegTo(Pos(x + 1.f, y)), 90.f);
  QCOMPARE(pos.angleDegTo(Pos(x - 1.f, y)), 270.f);
}

void GeoTest::testAngleQuad2()
{
  float x = 45.f, y = -45.f;
  Pos pos(x, y);

  QCOMPARE(pos.angleDegTo(Pos(x, y)), std::numeric_limits<float>::max());
  QCOMPARE(pos.angleDegTo(Pos(x, y + 1.f)), 0.f);
  QCOMPARE(pos.angleDegTo(Pos(x, y - 1.f)), 180.f);
  QCOMPARE(pos.angleDegTo(Pos(x + 1.f, y)), 90.3536f);
  QCOMPARE(pos.angleDegTo(Pos(x - 1.f, y)), 269.646f);
}

void GeoTest::testAngleQuad3()
{
  float x = -45.f, y = -45.f;
  Pos pos(x, y);

  QCOMPARE(pos.angleDegTo(Pos(x, y)), std::numeric_limits<float>::max());
  QCOMPARE(pos.angleDegTo(Pos(x, y + 1.f)), 0.f);
  QCOMPARE(pos.angleDegTo(Pos(x, x - 1.f)), 180.f);
  QCOMPARE(pos.angleDegTo(Pos(x + 1.f, y)), 90.3536f);
  QCOMPARE(pos.angleDegTo(Pos(x - 1.f, y)), 269.646f);
}

void GeoTest::testAngleQuad4()
{
  float x = -45.f, y = 45.f;
  Pos pos(x, y);

  QCOMPARE(pos.angleDegTo(Pos(x, y)), std::numeric_limits<float>::max());
  QCOMPARE(pos.angleDegTo(Pos(x, y + 1.f)), 0.f);
  QCOMPARE(pos.angleDegTo(Pos(x, x - 1.f)), 180.f);
  QCOMPARE(pos.angleDegTo(Pos(x + 1.f, y)), 89.6464f);
  QCOMPARE(pos.angleDegTo(Pos(x - 1.f, y)), 270.354f);
}

void GeoTest::testLineAntiMeridian_data()
{
  QTest::addColumn<Line>("line");
  QTest::addColumn<float>("resultLat");
  QTest::addColumn<float>("resultLon");

  // QTest::newRow("180.f, 49.f, -170.f,  49.f") << Line(180.f, 49.f, -170.f, 49.f) << 9.50215f << -180.f;

  QTest::newRow("-179.f, 10.f, 179.f,  9.f") << Line(-179.f, 10.f, 179.f, 9.f) << 9.50215f << -180.f;
  QTest::newRow("179.f, 10.f, -179.f,  9.f") << Line(179.f, 10.f, -179.f, 9.f) << 9.50215f << 180.f;
  // QTest::newRow("179.f,  1.f, -179.f, -1.f") << Line(179.f, 1.f, -179.f, -1.f) << 1.34101e-05f << 180.f;
  QTest::newRow("179.f,  2.f, -179.f,  1.f") << Line(179.f, 2.f, -179.f, 1.f) << 1.50034f << 180.f;
  QTest::newRow("170.f,  1.f, -170.f,  0.f") << Line(170.f, 1.f, -170.f, 0.f) << 0.507752f << 180.f;

}

void GeoTest::testLineAntiMeridian()
{
  QFETCH(Line, line);
  QFETCH(float, resultLat);
  QFETCH(float, resultLon);

  QCOMPARE(line.crossesAntiMeridian(), true);

  QList<Line> corrected = line.splitAtAntiMeridian();

  QCOMPARE(corrected.size(), 2);
  QCOMPARE(corrected.at(0).getPos2().getLatY(), resultLat);
  QCOMPARE(corrected.at(0).getPos2().getLonX(), resultLon);
  QCOMPARE(corrected.at(1).getPos1().getLatY(), resultLat);
  QCOMPARE(corrected.at(1).getPos1().getLonX(), -resultLon);
}

const static atools::geo::LineString track({atools::geo::Pos(-8.912841278937394, 52.60663708318238),
                                            atools::geo::Pos(-8.862844205905699, 52.61221376715999),
                                            atools::geo::Pos(-8.662602813702559, 52.63407550093609),
                                            atools::geo::Pos(-8.402765170981132, 52.63008327695571),
                                            atools::geo::Pos(-8.051495013488424, 52.66678238662439),
                                            atools::geo::Pos(-7.791341410380108, 52.66146052390298),
                                            atools::geo::Pos(-7.590108505223442, 52.68145353757851),
                                            atools::geo::Pos(-7.18720374863533, 52.72068953286778),
                                            atools::geo::Pos(-6.985279544531381, 52.73958550237592),
                                            atools::geo::Pos(-6.732500222209509, 52.76263774609386),
                                            atools::geo::Pos(-6.479632750528286, 52.78541082562952),
                                            atools::geo::Pos(-6.276865722530852, 52.80297420333915),
                                            atools::geo::Pos(-6.132516801580161, 52.84700371769064),
                                            atools::geo::Pos(-5.894403110840573, 52.93085126233986),
                                            atools::geo::Pos(-5.690426214570678, 52.947194255398),
                                            atools::geo::Pos(-5.405982190787223, 53.06408723300809),
                                            atools::geo::Pos(-5.222656559035221, 53.17260544928833),
                                            atools::geo::Pos(-4.935089915514705, 53.28809650054057)});

void GeoTest::testBinaryGeo()
{
  LineString test;
  atools::fs::common::BinaryGeometry geo(track);

  QByteArray bytes = geo.writeToByteArray();
  QCOMPARE(bytes.size(), 148);

  atools::fs::common::BinaryGeometry geo2(bytes);
  QCOMPARE(geo2.getGeometry(), track);
}

void GeoTest::testBinaryMsaGeo_data()
{
  QTest::addColumn<float>("radius");
  QTest::addColumn<float>("lat");
  QTest::addColumn<float>("lon");
  QTest::addColumn<QVector<float> >("bearingAltitude");

  // area_code  icao_code  airport_identifier  msa_center  msa_center_latitude  msa_center_longitude
  // magnetic_true_indicator  multiple_code  radius_limit
  // sector_bearing_1  sector_altitude_1  sector_bearing_2  sector_altitude_2  sector_bearing_3  sector_altitude_3
  // sector_bearing_4  sector_altitude_4  sector_bearing_5  sector_altitude_5

  // USA K7 06FA PHK 26.78274167 -80.69143333 M  25 180 26
  QTest::newRow("USA K7 06FA PHK") << 25.f << 26.78274167f << -80.69143333f
                                   << QVector<float>({180.f, 26.f});

  // AFR DA DAAP ILZ 26.72005 8.63577222 M 25 180 43
  QTest::newRow("AFR DA DAAP ILZ") << 25.f << 26.72005f << 8.63577222f
                                   << QVector<float>({180.f, 43.f});

  // AFR DA DAAG MAR 36.68476389 2.78216111 M 25 270 71 90 40
  QTest::newRow("AFR DA DAAG MAR") << 25.f << 36.68476389f << 2.78216111f
                                   << QVector<float>({270.f, 71.f, 90.f, 40.f});

  // AFR DA DAAG ZEM 36.795 3.57083333 M 25 180 44 270 85 360 53 90 40
  QTest::newRow("AFR DA DAAG ZEM") << 25.f << 36.795f << 3.57083333f
                                   << QVector<float>({180.f, 44.f, 270.f, 85.f, 360.f, 53.f, 90.f, 40.f});

  // AFR DA DAAJ DJA 24.28772778 9.45334167 M 25 180 83 360 79
  QTest::newRow("AFR DA DAAJ DJA") << 25.f << 24.28772778f << 9.45334167f
                                   << QVector<float>({180.f, 83.f, 360.f, 79.f});

  // AFR DA DAAS STF 36.17666667 5.28861111 M A 25 240 69 320 84 65 78 155 87
  QTest::newRow("AFR DA DAAS STF A") << 25.f << 36.17666667f << 5.28861111f
                                     << QVector<float>({240.f, 69.f, 320.f, 84.f, 65.f, 78.f, 155.f, 87.f});

  // AFR DA DAAS STF 36.17666667 5.28861111 M B 25 180 87 270 83
  QTest::newRow("AFR DA DAAS STF B") << 25.f << 36.17666667f << 5.28861111f
                                     << QVector<float>({180.f, 87.f, 270.f, 83.f, });
}

void GeoTest::testBinaryMsaGeo()
{
  QFETCH(float, radius);
  QFETCH(float, lat);
  QFETCH(float, lon);
  QFETCH(QVector<float>, bearingAltitude);

  atools::fs::common::BinaryMsaGeometry geo;
  geo.addSectors(bearingAltitude);

  geo.calculate(atools::geo::Pos(lon, lat), radius, 0.f, false);
  QVERIFY(geo.isValid());

  atools::fs::common::BinaryMsaGeometry geo2;
  geo2.readFromByteArray(geo.writeToByteArray());

  QCOMPARE(geo.getAltitudes(), geo2.getAltitudes());
  QCOMPARE(geo.getBearings(), geo2.getBearings());
  QCOMPARE(geo.getBoundingRect(), geo2.getBoundingRect());
  QCOMPARE(geo.getGeometry(), geo2.getGeometry());
  QCOMPARE(geo.getLabelPositions(), geo2.getLabelPositions());
  QCOMPARE(geo.getBearingEndPositions(), geo2.getBearingEndPositions());
}

void GeoTest::testDistanceToLineAlongHoriz()
{
  LineDistance dist;

  Pos pos(0.f, 0.f);
  // Left to right
  pos.distanceMeterToLine(Pos(-1.f, 0), Pos(1.f, 0), dist);

  QCOMPARE(dist.status, atools::geo::ALONG_TRACK);
  QCOMPARE(dist.distance, 0.f);
  QCOMPARE(dist.distanceFrom1, 111195.f);
  QCOMPARE(dist.distanceFrom2, 111195.f);
}

void GeoTest::testDistanceToLineAlongVert()
{
  LineDistance dist;

  Pos pos(0.f, 0.f);
  // From top down
  pos.distanceMeterToLine(Pos(0.f, 1.f), Pos(0.f, -1.f), dist);

  QCOMPARE(dist.status, atools::geo::ALONG_TRACK);
  QCOMPARE(dist.distance, 0.f);
  QCOMPARE(dist.distanceFrom1, 111195.f);
  QCOMPARE(dist.distanceFrom2, 111195.f);
}

void GeoTest::testDistanceToLineAlongVertNorth()
{
  LineDistance dist;

  Pos pos(0.f, 50.f);
  // From top down
  pos.distanceMeterToLine(Pos(0.f, 51.f), Pos(0.f, 49.f), dist);

  QCOMPARE(dist.status, atools::geo::ALONG_TRACK);
  QCOMPARE(dist.distance, 0.f);
  QCOMPARE(dist.distanceFrom1, 111195.f);
  QCOMPARE(dist.distanceFrom2, 111195.f);
}

void GeoTest::testDistanceToLineBefore()
{
  LineDistance dist;

  Pos pos(-2.f, 0.f);
  // Left to right
  pos.distanceMeterToLine(Pos(-1.f, 0), Pos(1.f, 0), dist);

  QCOMPARE(dist.status, atools::geo::BEFORE_START);
  QCOMPARE(dist.distance, 111195.f);
  QCOMPARE(dist.distanceFrom1, 111195.f);
  QCOMPARE(dist.distanceFrom2, 333585.f);
}

void GeoTest::testDistanceToLineAfter()
{
  LineDistance dist;

  Pos pos(2.f, 0.f);
  // Left to right
  pos.distanceMeterToLine(Pos(-1.f, 0), Pos(1.f, 0), dist);

  QCOMPARE(dist.status, atools::geo::AFTER_END);
  QCOMPARE(dist.distance, 111195.f);
  QCOMPARE(dist.distanceFrom1, 333585.f);
  QCOMPARE(dist.distanceFrom2, 111195.f);
}

void GeoTest::testDistanceToLineBeforeLong()
{
  LineDistance dist;

  Pos pos(-10.f, 0.f);
  // Left to right
  pos.distanceMeterToLine(Pos(-1.f, 0), Pos(1.f, 0), dist);

  QCOMPARE(dist.status, atools::geo::BEFORE_START);
  // QCOMPARE(dist.distance, 111195.f);
  // QCOMPARE(dist.distanceFrom1, 111195.f);
  // QCOMPARE(dist.distanceFrom2, 333585.f);
}

void GeoTest::testDistanceToLineAfterLong()
{
  LineDistance dist;

  Pos pos(10.f, 0.f);
  // Left to right
  pos.distanceMeterToLine(Pos(-1.f, 0), Pos(1.f, 0), dist);

  QCOMPARE(dist.status, atools::geo::AFTER_END);
  // QCOMPARE(dist.distance, 111195.f);
  // QCOMPARE(dist.distanceFrom1, 333585.f);
  // QCOMPARE(dist.distanceFrom2, 111195.f);
}

void GeoTest::testDistanceToLineBug()
{
  LineDistance dist;

  Pos pos(-122.49698638916016f, 48.41421127319336f);
  // Left to right
  pos.distanceMeterToLine(Pos(-123.48444366455078f, 48.72694396972656f),
                          Pos(-124.12972259521484f, 49.073055267333984f), dist);

  QCOMPARE(dist.status, atools::geo::BEFORE_START);
}

void GeoTest::testDistanceToLineBug2()
{
  LineDistance dist;

  Pos pos(-122.49698638916016f + 180.f, 48.41421127319336f);
  // Left to right
  pos.distanceMeterToLine(Pos(-123.48444366455078f + 180.f, 48.72694396972656f),
                          Pos(-124.12972259521484f + 180.f, 49.073055267333984f), dist);

  QCOMPARE(dist.status, atools::geo::BEFORE_START);
}

// pos1  @0x555556ce9c38  atools::geo::Pos &
// altitude  2000.0  float
// latY  48.72694396972656  float
// lonX    float
// pos2  @0x555556a870d8  atools::geo::Pos &
// altitude  0.0  float
// latY  49.073055267333984  float
// lonX  -124.12972259521484  float
// this  @0x7fffffffc5a8  atools::geo::Pos
// altitude  5000.0  float
// latY  48.41421127319336  float
// lonX  -122.49698638916016  float

void GeoTest::testRectExpandDateBoundary()
{
  atools::geo::LineString line({
    Pos(-151.750000, 56.750557),
    Pos(-135.000000, 52.716667),
    Pos(-134.250000, 51.699722),
    Pos(-133.750000, 51.000000),
    Pos(-131.884964, 50.199711),
    Pos(-130.085007, 49.366390),
    Pos(-128.000000, 48.333332),
    Pos(-126.717461, 45.507114),
    Pos(-126.500000, 45.000000),
    Pos(-126.656670, 43.440018),
    Pos(-126.680672, 43.302410),
    Pos(-126.696945, 43.127224),
    Pos(-126.811668, 41.916668),
    Pos(-126.900002, 40.983334),
    Pos(-127.000023, 40.833344),
    Pos(-127.000023, 37.506397),
    Pos(-126.933357, 36.461952),
    Pos(-125.833336, 35.500000),
    Pos(-124.200020, 36.000008),
    Pos(-123.250000, 34.500000),
    Pos(-120.833336, 30.750000),
    Pos(-120.000000, 30.000000),
    Pos(-120.000000, 5.000000),
    Pos(-120.000000, 3.500000),
    Pos(-138.255005, 3.500000),
    Pos(-141.083328, 3.500000),
    Pos(-145.000000, 3.500000),
    Pos(-149.491669, -0.321667),
    Pos(-151.949997, -2.416667),
    Pos(-155.000000, -5.000000),
    Pos(-157.000000, -5.000000),
    Pos(-159.086670, -5.000000),
    Pos(-160.000000, -5.000000),
    Pos(-161.524994, -5.000000),
    Pos(-167.068604, -5.000000),
    Pos(-168.020004, -5.000000),
    Pos(179.999985, -5.000000),
    Pos(179.996002, -5.000000),
    Pos(179.996002, 3.500000),
    Pos(170.000000, 3.500000),
    Pos(160.000000, 3.500000),
    Pos(160.000000, 0.000000),
    Pos(156.833328, 0.000028),
    Pos(154.229996, 0.000000),
    Pos(148.828339, 0.000028),
    Pos(146.141663, 0.000000),
    Pos(142.856674, 0.000000),
    Pos(141.000000, 0.000000),
    Pos(141.000000, 3.500000),
    Pos(133.000000, 3.500000),
    Pos(131.156662, 5.650000),
    Pos(130.000000, 7.000000),
    Pos(130.000000, 9.245000),
    Pos(130.000000, 14.250000),
    Pos(130.000000, 19.206667),
    Pos(130.000000, 21.000000),
    Pos(132.923340, 21.000000),
    Pos(134.946671, 21.000000),
    Pos(137.000000, 21.000000),
    Pos(137.821671, 21.000000),
    Pos(140.600006, 21.000000),
    Pos(142.990005, 21.000000),
    Pos(145.199997, 21.000000),
    Pos(150.316666, 21.000000),
    Pos(154.136673, 21.000000),
    Pos(155.000000, 21.000000),
    Pos(155.000000, 27.000000),
    Pos(165.000000, 27.000000),
    Pos(165.000000, 43.000000),
    Pos(162.916672, 45.700001),
    Pos(-179.999985, 49.690418),
    Pos(-176.566696, 50.133331),
    Pos(-167.816666, 51.400002),
    Pos(-160.000000, 53.500000),
    Pos(-153.000000, 56.000000),
  });
  QCOMPARE(line.boundingRect(), Rect(130.000000f, 56.750557f, -120.000000f, -5.000000f));
}

void GeoTest::testRectExpandDateBoundary2()
{
  atools::geo::LineString line({
    Pos(78.000000, -2.000000),
    Pos(92.000000, -2.000000),
    Pos(107.000000, -12.000000),
    Pos(114.500000, -12.000000),
    Pos(115.142220, -14.136945),
    Pos(120.250000, -19.000000),
    Pos(121.672775, -20.055000),
    Pos(124.551109, -21.521944),
    Pos(126.058891, -23.396944),
    Pos(128.463608, -23.220278),
    Pos(131.839996, -21.202499),
    Pos(136.328888, -21.499722),
    Pos(136.373611, -21.858889),
    Pos(136.635284, -22.297501),
    Pos(138.389999, -26.225277),
    Pos(143.500000, -29.000000),
    Pos(146.533340, -29.000000),
    Pos(148.830826, -32.094444),
    Pos(150.529358, -33.427212),
    Pos(150.711975, -33.304508),
    Pos(150.925964, -33.225113),
    Pos(151.157013, -33.194321),
    Pos(151.182495, -33.194019),
    Pos(151.414490, -33.219353),
    Pos(151.631042, -33.293667),
    Pos(151.817703, -33.412018),
    Pos(151.961868, -33.566494),
    Pos(152.053604, -33.746738),
    Pos(152.086380, -33.940586),
    Pos(152.057587, -34.134876),
    Pos(151.968765, -34.316330),
    Pos(151.825668, -34.472458),
    Pos(151.825012, -34.473015),
    Pos(151.824753, -34.472805),
    Pos(152.931900, -35.316578),
    Pos(152.739960, -35.813446),
    Pos(152.393036, -36.252247),
    Pos(151.913284, -36.601898),
    Pos(151.333664, -36.837250),
    Pos(150.749985, -36.937763),
    Pos(150.750809, -36.945099),
    Pos(150.320557, -38.188610),
    Pos(151.000000, -43.000000),
    Pos(150.664719, -43.850834),
    Pos(150.000000, -44.565834),
    Pos(150.000000, -45.000000),
    Pos(163.000000, -45.000000),
    Pos(163.000000, -89.999985),
    Pos(75.000000, -6.000000)
  });

  // float leftLonX, float topLatY, float rightLonX, float bottomLatY
  QCOMPARE(line.boundingRect(), Rect(75.000000f, -2.000000f, 163.000000f, -89.999985f));
}

void GeoTest::testRectOverlap_data()
{
  QTest::addColumn<Rect>("r1");
  QTest::addColumn<Rect>("r2");
  QTest::addColumn<bool>("result");

  // explicit Rect(float leftLonX, float topLatY, float rightLonX, float bottomLatY);

  Rect germany(Pos(6.70917, 53.5955), Pos(13.346, 48.518));
  Rect eu(Pos(-6.33111, 58.2156), Pos(28.0862, 36.4054));
  Rect seeu(Pos(24.6197, 40.9139), Pos(38.3, 34.55));
  Rect us(Pos(-156.769, 71.2847), Pos(-74.524, 24.0633));
  Rect pacific(Pos(166.637, 19.2825), Pos(-149.611, -17.5567));
  Rect pacificE(Pos(-169.534, 16.7287), Pos(-149.611, -17.5567));
  Rect pacificW(Pos(145.73, 15.1203), Pos(177.444, -17.7564));
  Rect pacificC(Pos(173.146, 1.38116), Pos(-176, -10.f));

  QTest::newRow("Germany EU") << germany << eu << true;
  QTest::newRow("SE EU EU") << seeu << eu << true;
  QTest::newRow("Germany SE EU") << germany << seeu << false;
  QTest::newRow("US EU") << us << eu << false;
  QTest::newRow("Germany US") << germany << us << false;
  QTest::newRow("Pacific US") << pacific << us << false;

  QTest::newRow("Pacific Pacific W") << pacific << pacificW << true;
  QTest::newRow("Pacific Pacific E") << pacific << pacificE << true;
  QTest::newRow("Pacific Pacific C") << pacific << pacificC << true;

}

void GeoTest::testRectOverlap()
{
  QFETCH(Rect, r1);
  QFETCH(Rect, r2);
  QFETCH(bool, result);

  QCOMPARE(r1.overlaps(r2), result);

}

void GeoTest::testCoordString_data()
{

#define ROW(a, b) QTest::newRow(a) << a << b

  QTest::addColumn<QString>("coord");
  QTest::addColumn<Pos>("pos");

  ROW("57N30", atools::geo::Pos(-130.f, 57.f));
  ROW("5730N", atools::geo::Pos(-30.f, 57.f));
  ROW("5730E", atools::geo::Pos(30.f, 57.f));
  ROW("57E30", atools::geo::Pos(130.f, 57.f));
  ROW("57W30", atools::geo::Pos(-130.f, -57.f));
  ROW("5730W", atools::geo::Pos(-30.f, -57.f));
  ROW("5730S", atools::geo::Pos(30.f, -57.f));
  ROW("57S30", atools::geo::Pos(130.f, -57.f));

  ROW("N 52 33.58;E 13 17.26", atools::geo::Pos(13.287666, 52.559666));
  ROW("N 52 33,58:E 13 17,26", atools::geo::Pos(13.287666, 52.559666));
  ROW("N 52 33.58|E 13 17.26", atools::geo::Pos(13.287666, 52.559666));

  ROW("", atools::geo::EMPTY_POS);
  ROW("4", atools::geo::EMPTY_POS);
  ROW("N3E", atools::geo::EMPTY_POS);
  ROW("NE3", atools::geo::EMPTY_POS);
  ROW("49N 34", atools::geo::EMPTY_POS);
  ROW("N49 34", atools::geo::EMPTY_POS);
  ROW("49 34E", atools::geo::EMPTY_POS);
  ROW("49 E34", atools::geo::EMPTY_POS);
  ROW("4934", atools::geo::EMPTY_POS);

  ROW("49 34", atools::geo::Pos(34.f, 49.f));
  ROW("+49 +34", atools::geo::Pos(34.f, 49.f));
  ROW("-49 -34", atools::geo::Pos(-34.f, -49.f));
  ROW("-49 +34", atools::geo::Pos(34.f, -49.f));

  ROW("n 50 W10", atools::geo::Pos(-10.f, 50.f));
  ROW("n 50 e10", atools::geo::Pos(10.f, 50.f));
  ROW("n50 e10", atools::geo::Pos(10.f, 50.f));

  ROW("50 n 10 W", atools::geo::Pos(-10.f, 50.f));
  ROW("50n  10e", atools::geo::Pos(10.f, 50.f));

  ROW("50 n 10 W", atools::geo::Pos(-10.f, 50.f));
  ROW("50n  10e", atools::geo::Pos(10.f, 50.f));

  ROW("n 50.5 e10.5", atools::geo::Pos(10.5f, 50.5f));

  ROW("n 50,5 e10,5", atools::geo::Pos(10.5f, 50.5f));

  ROW("N49° 26' 41.57\" E9° 12' 5.49\"", atools::geo::Pos(9.201525, 49.444881));

  ROW("N54* 16.82' W008* 35.95'", atools::geo::Pos(-8.599167, 54.280334));

  ROW("N 52 33.58 E 13 17.26", atools::geo::Pos(13.287666, 52.559666));

  ROW("49° 26' 41,57\" N 9° 12' 5,49\" E", atools::geo::Pos(9.201525, 49.444881));
  ROW("49 26 41,57 N 9 12 5,49 E", atools::geo::Pos(9.201525, 49.444881));
  ROW("49 26 41,57N 9 12 5,49E", atools::geo::Pos(9.201525, 49.444881));

  ROW("49° 26,69' N 9° 12,09' E", atools::geo::Pos(9.201500, 49.444832));

  ROW("49,4449° N 9,2015° E", atools::geo::Pos(9.201500, 49.444901));

  ROW("N 49,4449° E 9,2015°", atools::geo::Pos(9.201500, 49.444901));

  ROW("E9° 12' 5.49\" N49° 26' 41.57\"", atools::geo::EMPTY_POS);

  ROW("46N078W", atools::geo::Pos(-78.f, 46.f));

  ROW("4620N07805W", atools::geo::Pos(-78.083336f, 46.333332f));

  ROW("481200N0112800E", atools::geo::Pos(11.466666f, 48.200001f));

  ROW("N6500/W08000", atools::geo::Pos(-80.f, 65.f));

#undef ROW
}

void GeoTest::testCoordString()
{
  QFETCH(QString, coord);
  QFETCH(Pos, pos);

  // qInfo() << QLocale().decimalPoint();
  // qInfo() << coord << atools::fs::util::fromAnyFormat(coord);
  bool hemisphere;
  QCOMPARE(atools::fs::util::fromAnyFormat(coord, &hemisphere), pos);
}

void GeoTest::testRectExtend_data()
{
#define FUNC QString("%1:%2").arg(__FUNCTION__).arg(__LINE__).toLocal8Bit()

  QTest::addColumn<Rect>("rect");
  QTest::addColumn<LineString>("extends");
  QTest::addColumn<Rect>("expected");

  QTest::newRow(FUNC) << Rect(0.f, 0.f) << LineString({0.f, 0.f}) << Rect(0.f, 0.f);
  QTest::newRow(FUNC) << Rect(0.f, 0.f) << LineString({1.f, 0.f}) << Rect(0.f, 0.f, 1.f, 0.f);
  QTest::newRow(FUNC) << Rect(0.f, 0.f) << LineString({0.f, 1.f}) << Rect(0.f, 1.f, 0.f, 0.f);
  QTest::newRow(FUNC) << Rect(0.f, 1.f) << LineString({1.f, 0.f}) << Rect(0.f, 1.f, 1.f, 0.f);
  QTest::newRow(FUNC) << Rect(1.f, 0.f) << LineString({0.f, 1.f}) << Rect(0.f, 1.f, 1.f, 0.f);

  QTest::newRow(FUNC) << Rect(0.f, 0.f) << LineString({-1.f, -1.f}) << Rect(-1.f, 0.f, 0.f, -1.f);

  QTest::newRow(FUNC) << Rect(0.f, 0.f)
                      << LineString({-1.f, 1.f, 1.f, 1.f, 1.f, -1.f, -1.f, -1.f})
                      << Rect(-1.f, 1.f, 1.f, -1.f);

  QTest::newRow(FUNC) << Rect(-1.f, 1.f, 1.f, 1.f)
                      << LineString({-2.f, 2.f, 2.f, 2.f, 2.f, -2.f, -2.f, -2.f})
                      << Rect(-2.f, 2.f, 2.f, -2.f);

  QTest::newRow(FUNC) << Rect(-180.f, 0.f)
                      << LineString({179.f, 0.f, -179.f, 0.f})
                      << Rect(179.f, 0.f, -179.f, 0.f);

  QTest::newRow(FUNC) << Rect(180.f, 0.f)
                      << LineString({179.f, 0.f, -179.f, 0.f})
                      << Rect(179.f, 0.f, -179.f, 0.f);

  QTest::newRow(FUNC) << Rect(-180.f, 0.f)
                      << LineString({-179.f, 0.f, 179.f, 0.f})
                      << Rect(179.f, 0.f, -179.f, 0.f);

  QTest::newRow(FUNC) << Rect(180.f, 0.f)
                      << LineString({-179.f, 0.f, 179.f, 0.f})
                      << Rect(179.f, 0.f, -179.f, 0.f);

  QTest::newRow(FUNC) << Rect(170.f, 10.f, -170.f, -10.f)
                      << LineString({-160.f, 0.f, 160.f, 0.f})
                      << Rect(160.f, 10.f, -160.f, -10.f);

  QTest::newRow(FUNC) << Rect(170.f, 10.f, -170.f, -10.f)
                      << LineString({-100.f, 40.f})
                      << Rect(170.f, 40.f, -100.f, -10.f);

  QTest::newRow(FUNC) << Rect(170.f, 10.f, -170.f, -10.f)
                      << LineString({100.f, 40.f})
                      << Rect(100.f, 40.f, -170.f, -10.f);

  QTest::newRow(FUNC) << Rect(170.f, 10.f, -170.f, -10.f)
                      << LineString({-100.f, -40.f})
                      << Rect(170.f, 10.f, -100.f, -40.f);

  QTest::newRow(FUNC) << Rect(170.f, 10.f, -170.f, -10.f)
                      << LineString({100.f, -40.f})
                      << Rect(100.f, 10.f, -170.f, -40.f);

#undef FUNC
}

void GeoTest::testRectExtend()
{
  QFETCH(Rect, rect);
  QFETCH(LineString, extends);
  QFETCH(Rect, expected);

  QCOMPARE(rect.extend(extends), expected);
}

void GeoTest::testRectInflate_data()
{
#define FUNC QString("%1:%2").arg(__FUNCTION__).arg(__LINE__).toLocal8Bit()

  QTest::addColumn<Rect>("rect");
  QTest::addColumn<float>("width");
  QTest::addColumn<float>("height");
  QTest::addColumn<Rect>("expected");

  QTest::newRow(FUNC) << Rect(-1.f, 1.f, 1.f, -1.f) << 1.f << 1.f << Rect(-2.f, 2.f, 2.f, -2.f);
  QTest::newRow(FUNC) << Rect(0.f, 0.f) << 1.f << 1.f << Rect(-1.f, 1.f, 1.f, -1.f);
  QTest::newRow(FUNC) << Rect(0.f, 1.f, 0.f, -1.f) << 1.f << 1.f << Rect(-1.f, 2.f, 1.f, -2.f);
  QTest::newRow(FUNC) << Rect(-1.f, 0.f, 1.f, 0.f) << 1.f << 1.f << Rect(-2.f, 1.f, 2.f, -1.f);

  QTest::newRow(FUNC) << Rect(170.f, 1.f, -170.f, -1.f) << 1.f << 1.f << Rect(169.f, 2.f, -169.f, -2.f);

#undef FUNC
}

void GeoTest::testRectInflate()
{
  QFETCH(Rect, rect);
  QFETCH(float, width);
  QFETCH(float, height);
  QFETCH(Rect, expected);

  QCOMPARE(rect.inflate(width, height), expected);
}

void GeoTest::testRectInflateMeter_data()
{
#define FUNC QString("%1:%2").arg(__FUNCTION__).arg(__LINE__).toLocal8Bit()

  QTest::addColumn<Rect>("rect");
  QTest::addColumn<float>("width");
  QTest::addColumn<float>("height");
  QTest::addColumn<Rect>("expected");

  QTest::newRow(FUNC) << Rect(-1.f, 1.f, 1.f, -1.f) << nmToMeter(60.f) << nmToMeter(60.f) << Rect(-2.000557f, 2.f, 2.000557f, -2.f);
  QTest::newRow(FUNC) << Rect(89.f, 1.f, 91.f, -1.f) << nmToMeter(60.f) << nmToMeter(60.f) << Rect(87.999443f, 2.f, 92.000557f, -2.f);
  QTest::newRow(FUNC) << Rect(-1.f, 71.f, 1.f, 69.f) << nmToMeter(60.f) << nmToMeter(60.f) << Rect(-1.342196f, 72.f, 1.342196f, 68.f);
  QTest::newRow(FUNC) << Rect(-1.f, -69.f, 1.f, -71.f) << nmToMeter(60.f) << nmToMeter(60.f) << Rect(-1.342196f, -68.f, 1.342196f, -72.f);

#undef FUNC
}

void GeoTest::testRectInflateMeter()
{
  QFETCH(Rect, rect);
  QFETCH(float, width);
  QFETCH(float, height);
  QFETCH(Rect, expected);

  Rect result = rect.inflateMeter(width, height);
  qDebug() << "result" << result << result.getWidthMeter() << "expected" << expected << expected.getWidthMeter();

  QVERIFY(result.almostEqual(expected, 0.00001f));
  QVERIFY(atools::almostEqual(result.getWidthMeter(), expected.getWidthMeter(), 1.f));
}

void GeoTest::testRectRadius_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("radius");
  QTest::addColumn<float>("west");
  QTest::addColumn<float>("north");
  QTest::addColumn<float>("east");
  QTest::addColumn<float>("south");

#define FUNC QString("%1:%2").arg(__FUNCTION__).arg(__LINE__).toLocal8Bit()
  QTest::newRow(FUNC) << Pos(0.f, 0.f) << 60.f << -1.071429f << 1.f << 1.071429f << -1.f;
  QTest::newRow(FUNC) << Pos(0.f, 0.f) << 120.f << -1.071429f * 2.f << 2.f << 1.071429f * 2.f << -2.f;
  QTest::newRow(FUNC) << Pos(0.f, 0.f) << 240.f << -1.071429f * 4.f << 4.f << 1.071429f * 4.f << -4.f;

  QTest::newRow(FUNC) << Pos(0.f, 20.f) << 60.f << -1.15385f << 21.f << 1.15385f << 19.f;
  QTest::newRow(FUNC) << Pos(0.f, 40.f) << 60.f << -1.53846f << 41.f << 1.53846f << 39.f;
  QTest::newRow(FUNC) << Pos(0.f, 60.f) << 60.f << -2.4f << 61.f << 2.4f << 59.f;
  QTest::newRow(FUNC) << Pos(0.f, 70.f) << 60.f << -3.87097f << 71.f << 3.87097f << 69.f;
  QTest::newRow(FUNC) << Pos(0.f, 80.f) << 60.f << -11.5385f << 81.f << 11.5385f << 79.f;
  QTest::newRow(FUNC) << Pos(0.f, 85.f) << 60.f << -60.f << 86.f << 60.f << 84.f;

  QTest::newRow(FUNC) << Pos(0.f, -60.f) << 60.f << -2.4f << -59.f << 2.4f << -61.f;
  QTest::newRow(FUNC) << Pos(0.f, -70.f) << 60.f << -3.87097f << -69.f << 3.87097f << -71.f;

  QTest::newRow(FUNC) << Pos(80.f, -60.f) << 60.f << -2.4f + 80 << -59.f << 2.4f + 80 << -61.f;
  QTest::newRow(FUNC) << Pos(80.f, -70.f) << 60.f << -3.87097f + 80 << -69.f << 3.87097f + 80 << -71.f;
#undef FUNC
}

void GeoTest::testRectRadius()
{
  QFETCH(Pos, pos);
  QFETCH(float, radius);
  QFETCH(float, west);
  QFETCH(float, north);
  QFETCH(float, east);
  QFETCH(float, south);

  Rect rect(pos, atools::geo::nmToMeter(radius), true /* fast */);

  QVERIFY(atools::geo::meterToNm(rect.getWidthMeter() / 2.f) > radius);

  if(pos.getLatY() < 82.f)
    QVERIFY(atools::geo::meterToNm(rect.getWidthMeter() / 2.f) < radius * 2.f);

  QCOMPARE(rect.getWest(), west);
  QCOMPARE(rect.getNorth(), north);
  QCOMPARE(rect.getEast(), east);
  QCOMPARE(rect.getSouth(), south);
}
