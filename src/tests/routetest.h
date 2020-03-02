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

#ifndef ATOOLSTEST_ROUTE_TEST_H
#define ATOOLSTEST_ROUTE_TEST_H

#include <QString>
#include <QtTest>

#include "routing/routenetwork.h"

namespace atools {
namespace sql {
class SqlDatabase;
}
namespace routing {
class RouteNetwork;
}
}

class RouteTest :
  public QObject
{
  Q_OBJECT

public:
  RouteTest();
  ~RouteTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testRouteFinder_data();
  void testRouteFinder();

  void testRouteNetwork();

  void testRouteNeighbors_data();
  void testRouteNeighbors();

private:
  atools::sql::SqlDatabase *db = nullptr;
  QHash<int, atools::routing::RouteNetwork *> networks;
  void printNearestResult(atools::routing::RouteNetwork *net, const atools::geo::Pos& origin,
                          const atools::geo::Pos& dest, const atools::routing::Result& result);

};

#endif // ATOOLSTEST_ROUTE_TEST_H
