/*****************************************************************************
* Copyright 2015-2019 Alexander Barthel albar965@mailbox.org
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
#include "geo/line.h"

using atools::geo::Pos;
using atools::geo::Rect;
using atools::geo::LineString;
using atools::geo::LineDistance;
using atools::geo::angleAbsDiff;

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
  QCOMPARE(Rect(-100.f, 10.f, 100.f, -10.f).getCenter(), Pos(-0.f, 0.f));
}

void GeoTest::testAngleDiff_data()
{
  QTest::addColumn<float>("angle1");
  QTest::addColumn<float>("angle2");
  QTest::addColumn<float>("result");

  QTest::newRow("(0.f, 0.f), 0.f)") << 0.f << 0.f << 0.f;
  QTest::newRow("(180.f, 180.f), 0.f)") << 180.f << 180.f << 0.f;
  QTest::newRow("(360.f, 360.f), 0.f)") << 360.f << 360.f << 0.f;

  QTest::newRow("(0.f, 360.f), 0.f)") << 0.f << 360.f << 0.f;
  QTest::newRow("(360.f, 0.f), 0.f)") << 360.f << 0.f << 0.f;

  QTest::newRow("(0.f, 180.f), 180.f)") << 0.f << 180.f << 180.f;
  QTest::newRow("(180.f, 0.f), 180.f)") << 180.f << 0.f << 180.f;

  QTest::newRow("(359.f, 1.f), 2.f)") << 359.f << 1.f << 2.f;
  QTest::newRow("(1.f, 359.f), 2.f)") << 1.f << 359.f << 2.f;

  QTest::newRow("(350.f, 10.f), 20.f)") << 350.f << 10.f << 20.f;
  QTest::newRow("(10.f, 350.f), 20.f)") << 10.f << 350.f << 20.f;

  QTest::newRow("(179.f, 181.f), 2.f)") << 179.f << 181.f << 2.f;
  QTest::newRow("(181.f, 179.f), 2.f)") << 181.f << 179.f << 2.f;

  QTest::newRow("(360.f, 180.f), 180.f)") << 360.f << 180.f << 180.f;
  QTest::newRow("(360.f, 0.f), 0.f)") << 360.f << 0.f << 0.f;

  QTest::newRow("(89.f, 271.f), 178.f)") << 89.f << 271.f << 178.f;
  QTest::newRow("(91.f, 269.f), 178.f)") << 91.f << 269.f << 178.f;

  QTest::newRow("(271.f, 89.f), 178.f)") << 271.f << 89.f << 178.f;
  QTest::newRow("(269.f, 91.f), 178.f)") << 269.f << 91.f << 178.f;

  QTest::newRow("(90.f, 270.f), 180.f)") << 90.f << 270.f << 180.f;
  QTest::newRow("(89.f, 269.f), 180.f)") << 89.f << 269.f << 180.f;
  QTest::newRow("(91.f, 271.f), 180.f)") << 91.f << 271.f << 180.f;

  QTest::newRow("(270.f, 90.f), 180.f)") << 270.f << 90.f << 180.f;
  QTest::newRow("(269.f, 89.f), 180.f)") << 269.f << 89.f << 180.f;
  QTest::newRow("(271.f, 91.f), 180.f)") << 271.f << 91.f << 180.f;

}

void GeoTest::testAngleDiff()
{
  QFETCH(float, angle1);
  QFETCH(float, angle2);
  QFETCH(float, result);
  QCOMPARE(angleAbsDiff(angle1, angle2), result);
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

  qDebug() << "hd" << hd
           << "windSpeed" << windSpeed
           << "windDirectionDeg" << windDirectionDeg
           << "courseDeg" << courseDeg
           << "trueAirspeed" << trueAirspeed;

  QCOMPARE(hd, heading);
}

void GeoTest::testSunsetSunrise_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<QDate>("date");
  QTest::addColumn<float>("zenith");
  QTest::addColumn<QTime>("result");
  QTest::addColumn<bool>("neverrise");
  QTest::addColumn<bool>("neverset");

  float sunriseOfficial = 90.f + 50.f / 60.f;
  float sunsetOfficial = -(90.f + 50.f / 60.f);

  // http://edwilliams.org/sunrise_sunset_example.htm
  QTest::newRow("Example Rising") << Pos(-74.3, 40.9) << QDate(1990, 6, 25) << sunriseOfficial << QTime(9, 26, 29)
                                  << false << false;
  QTest::newRow("Example Setting") << Pos(-74.3, 40.9) << QDate(1990, 6, 25) << sunsetOfficial << QTime(0, 33, 0)
                                   << false << false;

  QTest::newRow("EDDF Rise") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << 90.f << QTime(3, 56, 45)
                             << false << false;
  QTest::newRow("EDDF Set") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << -90.f << QTime(19, 5, 53)
                            << false << false;

  QTest::newRow("EDDF Rise") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << sunriseOfficial << QTime(3, 50, 43)
                             << false << false;
  QTest::newRow("EDDF Set") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << sunsetOfficial << QTime(19, 11, 54)
                            << false << false;

  QTest::newRow("YSSY Rise") << Pos(151.177, -33.9461) << QDate(2018, 7, 30) << sunriseOfficial << QTime(20, 50, 0)
                             << false << false;
  QTest::newRow("YSSY Set") << Pos(151.177, -33.9461) << QDate(2018, 7, 30) << sunsetOfficial << QTime(7, 13, 46)
                            << false << false;

  QTest::newRow("SUMU Rise") << Pos(-56.0281, -34.8339) << QDate(2018, 7, 30) << sunriseOfficial << QTime(10, 40, 07)
                             << false << false;
  QTest::newRow("SUMU Set") << Pos(-56.0281, -34.8339) << QDate(2018, 7, 30) << sunsetOfficial << QTime(21, 1, 15)
                            << false << false;
}

void GeoTest::testSunsetSunrise()
{
  QFETCH(Pos, pos);
  QFETCH(QDate, date);
  QFETCH(float, zenith);
  QFETCH(QTime, result);
  QFETCH(bool, neverrise);
  QFETCH(bool, neverset);

  bool neverRises, neverSets;
  QTime time = atools::geo::calculateSunriseSunset(neverRises, neverSets, pos, date, zenith);
  qDebug() << pos << date << zenith;
  qDebug() << time << QDateTime(date, time).toLocalTime();
  QCOMPARE(time, result);
  QCOMPARE(neverrise, neverRises);
  QCOMPARE(neverset, neverSets);
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

void GeoTest::testDistanceToLineAlongHoriz()
{
  LineDistance dist;

  Pos pos(0.f, 0.f);
  // Left to right
  pos.distanceMeterToLine(Pos(-1.f, 0), Pos(1.f, 0), dist);
  qDebug() << dist;

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
  qDebug() << dist;

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
  qDebug() << dist;

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
  qDebug() << dist;

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
  qDebug() << dist;

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
  qDebug() << dist;

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
  qDebug() << dist;

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
  qDebug() << dist;

  QCOMPARE(dist.status, atools::geo::BEFORE_START);
}

void GeoTest::testDistanceToLineBug2()
{
  LineDistance dist;

  Pos pos(-122.49698638916016f + 180.f, 48.41421127319336f);
  // Left to right
  pos.distanceMeterToLine(Pos(-123.48444366455078f + 180.f, 48.72694396972656f),
                          Pos(-124.12972259521484f + 180.f, 49.073055267333984f), dist);
  qDebug() << dist;

  QCOMPARE(dist.status, atools::geo::BEFORE_START);
}

// pos1	@0x555556ce9c38	atools::geo::Pos &
// altitude	2000.0	float
// latY	48.72694396972656	float
// lonX		float
// pos2	@0x555556a870d8	atools::geo::Pos &
// altitude	0.0	float
// latY	49.073055267333984	float
// lonX	-124.12972259521484	float
// this	@0x7fffffffc5a8	atools::geo::Pos
// altitude	5000.0	float
// latY	48.41421127319336	float
// lonX	-122.49698638916016	float

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
  qDebug() << line.boundingRect();
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

  qDebug() << line.boundingRect();

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

#define ROW(a, b) QTest::newRow(a) << a << b;

  QTest::addColumn<QString>("coord");
  QTest::addColumn<Pos>("pos");

  ROW("", atools::geo::EMPTY_POS);
  ROW("4", atools::geo::EMPTY_POS);
  ROW("N3E", atools::geo::EMPTY_POS);
  ROW("NE3", atools::geo::EMPTY_POS);
  ROW("49 34", atools::geo::EMPTY_POS);
  ROW("49N 34", atools::geo::EMPTY_POS);
  ROW("N49 34", atools::geo::EMPTY_POS);
  ROW("49 34E", atools::geo::EMPTY_POS);
  ROW("49 E34", atools::geo::EMPTY_POS);
  ROW("4934", atools::geo::EMPTY_POS);

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

  ROW("5020N", atools::geo::Pos(-20.f, 50.f));

#undef ROW
}

void GeoTest::testCoordString()
{
  QFETCH(QString, coord);
  QFETCH(Pos, pos);

  // qInfo() << QLocale().decimalPoint();
  qInfo() << coord << atools::fs::util::fromAnyFormat(coord);
  QCOMPARE(atools::fs::util::fromAnyFormat(coord), pos);
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
