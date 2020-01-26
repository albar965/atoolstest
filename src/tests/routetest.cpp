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

#include "routetest.h"

#include "routing/routefinder.h"
#include "routing/routenetworkairway.h"
#include "routing/routenetworkradio.h"
#include "sql/sqldatabase.h"

using atools::sql::SqlDatabase;
using atools::geo::Pos;

RouteTest::RouteTest()
{
}

RouteTest::~RouteTest()
{
}

void RouteTest::runtest(int argc, char *argv[])
{
  RouteTest tst;
  QTest::qExec(&tst, argc, argv);
}

void RouteTest::initTestCase()
{
  SqlDatabase::addDatabase("QSQLITE", "TESTDBROUTE");
  db = new SqlDatabase("TESTDBROUTE");
  db->setDatabaseName("testdata/test_route.sqlite");
  db->open();

  radioNet = new atools::routing::RouteNetworkRadio(db);
  radioNet->setMode(atools::routing::ROUTE_RADIONAV);

  airwayNet = new atools::routing::RouteNetworkAirway(db);
  airwayNet->setMode(atools::routing::ROUTE_JET);
}

void RouteTest::cleanupTestCase()
{
  if(radioNet != nullptr)
  {
    delete radioNet;
    radioNet = nullptr;
  }

  if(airwayNet != nullptr)
  {
    delete airwayNet;
    airwayNet = nullptr;
  }

  if(db != nullptr)
  {
    db->close();
    delete db;
    db = nullptr;
  }

  SqlDatabase::removeDatabase("TESTDBROUTE");
}

void RouteTest::testRouteRadio_data()
{
  QTest::addColumn<Pos>("from");
  QTest::addColumn<Pos>("to");
  QTest::addColumn<float>("dist");
  QTest::addColumn<int>("num");
  QTest::addColumn<bool>("radio");

  // Jet airways
  QTest::newRow("EDDF to LIRF") << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 525.451f << 22 << false;
  QTest::newRow("EGAA to LGAV") << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1689.5f << 73 << false;

  // Radio navaids
  QTest::newRow("EDDF to LIRF") << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 526.269f << 4 << true;
  QTest::newRow("EGAA to LGAV") << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1587.18f << 9 << true;
}

void RouteTest::testRouteRadio()
{
  QFETCH(Pos, from);
  QFETCH(Pos, to);
  QFETCH(float, dist);
  QFETCH(int, num);
  QFETCH(bool, radio);

  QElapsedTimer timer;

  timer.start();
  atools::routing::RouteFinder router(radio ? radioNet : airwayNet);
  bool result = router.calculateRoute(from, to, 0);
  qDebug() << timer.restart() << "ms";

  QCOMPARE(result, true);

  float distance;
  QVector<atools::routing::RouteLeg> routeLegs;
  router.extractLegs(routeLegs, distance);

  QCOMPARE(atools::geo::meterToNm(distance), dist);
  QCOMPARE(routeLegs.size(), num);
}
