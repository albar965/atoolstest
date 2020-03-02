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
#include "routing/routenetwork.h"
#include "sql/sqldatabase.h"
#include "geo/calculations.h"

#include "geo/rect.h"
#include "geo/linestring.h"

using atools::sql::SqlDatabase;
using namespace atools::geo;

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

enum RouteNetworkType
{
  NET_AIRWAY,
  NET_RADIO
};

void RouteTest::initTestCase()
{
  SqlDatabase::addDatabase("QSQLITE", "TESTDBROUTE");
  db = new SqlDatabase("TESTDBROUTE");
  db->setDatabaseName("testdata/test_route.sqlite");
  db->open();

  atools::routing::RouteNetwork *net =
    new atools::routing::RouteNetwork(db, atools::routing::SOURCE_RADIO);
  net->ensureLoaded();
  networks.insert(NET_RADIO, net);

  net = new atools::routing::RouteNetwork(db, atools::routing::SOURCE_AIRWAY);
  net->ensureLoaded();
  networks.insert(NET_AIRWAY, net);
}

void RouteTest::cleanupTestCase()
{
  qDeleteAll(networks);
  networks.clear();

  if(db != nullptr)
  {
    db->close();
    delete db;
    db = nullptr;
  }

  SqlDatabase::removeDatabase("TESTDBROUTE");
}

void RouteTest::testRouteFinder_data()
{
  QTest::addColumn<Pos>("from");
  QTest::addColumn<Pos>("to");
  QTest::addColumn<float>("dist");
  QTest::addColumn<int>("num");
  QTest::addColumn<int>("network");
  QTest::addColumn<bool>("result");

  // Radio navaids - new
  QTest::newRow("LEVX to LPMA Radio New")
    << Pos(-8.6275, 42.2292) << Pos(-16.7781, 32.6942) << 723.f << 3 << int(NET_RADIO) << true;
  QTest::newRow("LEVX to LPFL Radio New")
    << Pos(-8.6275, 42.2292) << Pos(-31.1322, 39.4581) << 1252.73f << 4 << int(NET_RADIO) << true;
  QTest::newRow("EDDF to LIRF Radio New")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 520.505f << 4 << int(NET_RADIO) << true;
  QTest::newRow("EGAA to LGAV Radio New")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1584.73f << 9 << int(NET_RADIO) << true;
  QTest::newRow("ESNQ to FEFF Radio New")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 3948.21f << 21 << int(NET_RADIO) << true;

  // Airway - new
  QTest::newRow("LEVX to LPFL Airway New")
    << Pos(-8.6275, 42.2292) << Pos(-31.1322, 39.4581) << 1158.76f << 11 << int(NET_AIRWAY) << true;
  QTest::newRow("EDDF to LIRF Airway New")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 527.008f << 22 << int(NET_AIRWAY) << true;
  QTest::newRow("EGAA to LGAV Airway New")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1626.28f << 53 << int(NET_AIRWAY) << true;
  QTest::newRow("ESNQ to FEFF Airway New")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 4003.f << 95 << int(NET_AIRWAY) << true;
}

void RouteTest::testRouteFinder()
{
  QFETCH(Pos, from);
  QFETCH(Pos, to);
  QFETCH(float, dist);
  QFETCH(int, num);
  QFETCH(int, network);
  QFETCH(bool, result);

  atools::routing::RouteFinder router(networks.value(network));
  router.setProgressCallback([](int distMeterToDest, int currentDistMeterToDest) -> bool {
    qDebug() << distMeterToDest << currentDistMeterToDest
             << (100.f / distMeterToDest * (distMeterToDest - currentDistMeterToDest)) << "%";
    return true;
  });

  bool actualResult = router.calculateRoute(from, to, 0,
                                            network ==
                                            NET_RADIO ? atools::routing::MODE_RADIONAV : atools::routing::MODE_JET_AND_WAYPOINT);

  float actualDistance;
  QVector<atools::routing::RouteLeg> routeLegs;
  router.extractLegs(routeLegs, actualDistance);

  qDebug() << "res" << actualResult
           << "distance" << meterToNm(actualDistance) << "expected" << dist
           << "size" << routeLegs.size() << "expected" << num;

  QCOMPARE(actualResult, result);

  QVERIFY(atools::almostEqual(meterToNm(actualDistance), dist, 10.f));
  QVERIFY(atools::almostEqual(routeLegs.size(), num, 5));
}

void RouteTest::testRouteNetwork()
{
  atools::routing::RouteNetwork *radioNet = networks.value(NET_RADIO),
                                *airwayNet = networks.value(NET_AIRWAY);

  airwayNet->clearParameters();
  radioNet->clearParameters();

  QCOMPARE(airwayNet->getNodes().size(), 154511);
  QCOMPARE(radioNet->getNodes().size(), 9073);

  for(const atools::routing::Node& n : radioNet->getNodes())
  {
    QCOMPARE(n.edges.size(), 0);
    QVERIFY(n.id > 0);
    QVERIFY(n.index >= 0);
    QVERIFY(!n.isAnyAirway());
  }

  for(const atools::routing::Node& n : airwayNet->getNodes())
  {
    for(const atools::routing::Edge& e : n.edges)
    {
      QVERIFY(e.isAnyAirway());
      QVERIFY(e.airwayHash > 0);
      QVERIFY(e.airwayId > 0);
      QVERIFY(e.toIndex >= 0);
      QVERIFY(e.toIndex < airwayNet->getNodes().size());
    }
  }
}

void RouteTest::testRouteNeighbors_data()
{
  QTest::addColumn<Pos>("origin");
  QTest::addColumn<Pos>("departure");
  QTest::addColumn<Pos>("destination");
  QTest::addColumn<int>("mode");
  QTest::addColumn<int>("network");
  QTest::addColumn<int>("num");
  QTest::addColumn<Rect>("rect");

  QTest::newRow("EDDH - LIQW MODE_AIRWAY far from destination")
    << Pos(9.947, 49.7175)     /* WUR */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(atools::routing::MODE_AIRWAY)
    << int(NET_AIRWAY)
    << 3
    << Rect(Pos(9.947, 49.7175), nmToMeter(340.f));

  QTest::newRow("EDDH - LIQW MODE_WAYPOINT far from destination")
    << Pos(9.947, 49.7175)       /* WUR */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(atools::routing::MODE_WAYPOINT)
    << int(NET_AIRWAY)
    << 149
    << Rect(Pos(9.947, 49.7175), nmToMeter(340.f));

  QTest::newRow("EDDH - LIQW MODE_AIRWAY_AND_WAYPOINT near destination")
    << Pos(10, 45.4781) /* VAKON */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(atools::routing::MODE_AIRWAY_AND_WAYPOINT)
    << int(NET_AIRWAY)
    << 9
    << Rect(LineString({Pos(6, 45.4781), Pos(16, 44.0889)}));

  QTest::newRow("EDDH - LIQW MODE_AIRWAY_AND_WAYPOINT far from destination")
    << Pos(9.947, 49.7175)     /* WUR */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(atools::routing::MODE_AIRWAY_AND_WAYPOINT)
    << int(NET_AIRWAY)
    << 3
    << Rect(Pos(9.947, 49.7175), nmToMeter(340.f));
}

void RouteTest::testRouteNeighbors()
{
  QFETCH(Pos, origin);
  QFETCH(Pos, departure);
  QFETCH(Pos, destination);
  QFETCH(int, mode);
  QFETCH(int, network);
  QFETCH(int, num);
  QFETCH(Rect, rect);

  atools::routing::RouteNetwork *net = networks.value(network);
  net->setParameters(departure, destination, 0, atools::routing::Mode(mode));

  atools::routing::Result result;
  net->getNeighbours(result, origin);
  QVERIFY(result.nodes.size() == result.edges.size());
  QCOMPARE(result.nodes.size(), num);

  // printNearestResult(net, origin, destination, result);

  for(int idx : result.nodes)
  {
    const atools::routing::Node& n = net->getNode(idx);
    QVERIFY(rect.contains(n.getPosition()));
  }
}

void RouteTest::printNearestResult(atools::routing::RouteNetwork *net,
                                   const Pos& origin, const Pos& dest,
                                   const atools::routing::Result& result)
{
  qDebug() << "==========================================================";
  qDebug() << "neighbours.size()" << result.size()
           << "origin to dest" << meterToNm(dest.distanceMeterTo(origin)) << "nm";

  for(int i = 0; i < result.size(); i++)
  {
    const atools::routing::Node& node = net->getNode(result.nodes.at(i));
    qDebug() << "to origin" << meterToNm(node.getPosition().distanceMeterTo(origin)) << "nm"
             << "to dest" << meterToNm(node.getPosition().distanceMeterTo(dest)) << "nm"
             << "index" << result.nodes.at(i) << node;
    qDebug() << result.edges.at(i);
  }
}
