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

#ifndef ATOOLSTEST_GEOTEST_H
#define ATOOLSTEST_GEOTEST_H

#include <QString>
#include <QtTest>

class GeoTest :
  public QObject
{
  Q_OBJECT

public:
  GeoTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testAngleDiff_data();
  void testAngleDiff();

  void testWindDrift_data();
  void testWindDrift();

  void testSunsetSunrise_data();
  void testSunsetSunrise();

  void testNormalize();

  void testInterpolate();
  void testInterpolateForX();
  void testInterpolateForY();

  void testRectOverlap_data();
  void testRectOverlap();

  void testCoordString_data();
  void testCoordString();

  void testRectExpandDateBoundary();
  void testRectExpandDateBoundary2();
  void testDistanceToLineBeforeLong();
  void testDistanceToLineAfterLong();

  void testDistanceToLineBug();
  void testDistanceToLineBug2();

  void testDistanceToLineAlongHoriz();
  void testDistanceToLineAlongVert();
  void testDistanceToLineAlongVertNorth();
  void testDistanceToLineBefore();
  void testDistanceToLineAfter();

  void testAngleQuad1();
  void testAngleQuad2();
  void testAngleQuad3();
  void testAngleQuad4();
};

#endif // ATOOLSTEST_GEOTEST_H
