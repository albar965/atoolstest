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

void GeoTest::testSunsetSunrise_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<QDate>("date");
  QTest::addColumn<float>("zenith");
  QTest::addColumn<QTime>("result");

  float sunriseOfficial = 90.f + 50.f / 60.f;
  float sunsetOfficial = -(90.f + 50.f / 60.f);

  // http://edwilliams.org/sunrise_sunset_example.htm
  QTest::newRow("Example Rising") << Pos(-74.3, 40.9) << QDate(1990, 6, 25) << sunriseOfficial << QTime(9, 26, 29);
  QTest::newRow("Example Setting") << Pos(-74.3, 40.9) << QDate(1990, 6, 25) << sunsetOfficial << QTime(0, 33, 0);

  QTest::newRow("EDDF Rise") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << 90.f << QTime(3, 56, 45);
  QTest::newRow("EDDF Set") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << -90.f << QTime(19, 5, 53);

  QTest::newRow("EDDF Rise") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << sunriseOfficial << QTime(3, 50, 43);
  QTest::newRow("EDDF Set") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << sunsetOfficial << QTime(19, 11, 54);

  QTest::newRow("YSSY Rise") << Pos(151.177, -33.9461) << QDate(2018, 7, 30) << sunriseOfficial << QTime(20, 50, 0);
  QTest::newRow("YSSY Set") << Pos(151.177, -33.9461) << QDate(2018, 7, 30) << sunsetOfficial << QTime(7, 13, 46);

  QTest::newRow("SUMU Rise") << Pos(-56.0281, -34.8339) << QDate(2018, 7, 30) << sunriseOfficial << QTime(10, 40, 07);
  QTest::newRow("SUMU Set") << Pos(-56.0281, -34.8339) << QDate(2018, 7, 30) << sunsetOfficial << QTime(21, 1, 15);
}

void GeoTest::testSunsetSunrise()
{
  QFETCH(Pos, pos);
  QFETCH(QDate, date);
  QFETCH(float, zenith);
  QFETCH(QTime, result);

  QTime time = atools::geo::calculateSunriseSunset(pos, date, zenith);
  qDebug() << pos << date << zenith;
  qDebug() << time << QDateTime(date, time).toLocalTime();
  QCOMPARE(time, result);
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

}

void GeoTest::testCoordString()
{
  QFETCH(QString, coord);
  QFETCH(Pos, pos);

  // qInfo() << QLocale().decimalPoint();
  qInfo() << coord << atools::fs::util::fromAnyFormat(coord);
  QCOMPARE(atools::fs::util::fromAnyFormat(coord), pos);
}
