/*****************************************************************************
* Copyright 2015-2019 Alexander Barthel alex@littlenavmap.org
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

#ifndef ATOOLSTEST_SPATIAL_TEST_H
#define ATOOLSTEST_SPATIAL_TEST_H

#include <QString>
#include <QtTest>

#include "geo/pos.h"
#include "geo/spatialindex.h"

class SpatialTest :
  public QObject
{
  Q_OBJECT

public:
  SpatialTest();
  ~SpatialTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testSpatialRadius_data();
  void testSpatialRadius();

  void testSpatialNearest_data();
  void testSpatialNearest();

  void testSpatialRadiusDest_data();
  void testSpatialRadiusDest();

  void testCartesianDist_data();
  void testCartesianDist();

private:
  struct Data
  {
    int id;
    QString ident;
    atools::geo::Pos pos;

    const atools::geo::Pos& getPosition() const
    {
      return pos;
    }

  };
  void printObjects(const QVector<Data>& objects, const atools::geo::Pos& origin);
  void printObject(int idx, const Data& data, const atools::geo::Pos& origin);

  atools::geo::SpatialIndex<Data> *index = nullptr;
};

#endif // ATOOLSTEST_SPATIAL_TEST_H
