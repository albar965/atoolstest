/*****************************************************************************
* Copyright 2015-2017 Alexander Barthel albar965@mailbox.org
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
#include "geo/line.h"

using atools::geo::Pos;
using atools::geo::Rect;
using atools::geo::LineString;
using atools::geo::LineDistance;

GeoTest::GeoTest()
{

}

void GeoTest::initTestCase()
{
}

void GeoTest::cleanupTestCase()
{

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

  QCOMPARE(line.boundingRect(), Rect(0.f, 0.f, 0.f, 0.f));

}
