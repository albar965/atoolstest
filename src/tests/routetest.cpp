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
    << Pos(-8.6275, 42.2292) << Pos(-16.7781, 32.6942) << 745.f << 3 << int(NET_RADIO) << true;
  QTest::newRow("LEVX to LPFL Radio New")
    << Pos(-8.6275, 42.2292) << Pos(-31.1322, 39.4581) << 1883.73f << 5 << int(NET_RADIO) << true;
  QTest::newRow("EDDF to LIRF Radio New")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 520.505f << 4 << int(NET_RADIO) << true;
  QTest::newRow("EGAA to LGAV Radio New")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1584.73f << 9 << int(NET_RADIO) << true;
  QTest::newRow("ESNQ to FEFF Radio New")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 3948.21f << 21 << int(NET_RADIO) << true;

  // Airway - new
  QTest::newRow("LEVX to LPFL Airway New")
    << Pos(-8.6275, 42.2292) << Pos(-31.1322, 39.4581) << 1143.76f << 13 << int(NET_AIRWAY) << true;
  QTest::newRow("EDDF to LIRF Airway New")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 559.008f << 24 << int(NET_AIRWAY) << true;
  QTest::newRow("EGAA to LGAV Airway New")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1626.28f << 53 << int(NET_AIRWAY) << true;
  QTest::newRow("ESNQ to FEFF Airway New")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 4009.69f << 91 << int(NET_AIRWAY) << true;
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

  bool res = router.calculateRoute(from, to, 0,
                                   network ==
                                   NET_RADIO ? atools::routing::MODE_RADIONAV : atools::routing::MODE_JET);

  float distance;
  QVector<atools::routing::RouteLeg> routeLegs;
  router.extractLegs(routeLegs, distance);

  qDebug() << "res" << res << "distance" << atools::geo::meterToNm(distance) << "size" << routeLegs.size();

  QCOMPARE(res, result);

  QVERIFY(atools::almostEqual(atools::geo::meterToNm(distance), dist, 10.f));
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

void RouteTest::testRouteNeighbors()
{
  atools::routing::RouteNetwork *radioNet = networks.value(NET_RADIO),
                                *airwayNet = networks.value(NET_AIRWAY);

  airwayNet->clearParameters();
  radioNet->clearParameters();

  {
    atools::routing::Result result;
    airwayNet->setParameters(Pos(), Pos(), 0, atools::routing::MODE_AIRWAY_AND_WAYPOINT);
    airwayNet->getNeighbours(result, airwayNet->getNearestNode(Pos(9.947, 49.7175) /* WUR */));
    qDebug() << "EDDH - LIQW result.nodes.size() no destination" << result.nodes.size();
    QVERIFY(result.nodes.size() == result.edges.size());
    QCOMPARE(result.nodes.size(), 2955);
  }

  {
    atools::routing::Result result;
    airwayNet->setParameters(Pos(9.98823, 53.6304), Pos(9.98889, 44.0889), 0,
                             atools::routing::MODE_AIRWAY_AND_WAYPOINT);
    airwayNet->setDirectDistanceFactor(0.f);
    airwayNet->getNeighbours(result, airwayNet->getNearestNode(Pos(9.947, 49.7175) /* WUR */));
    qDebug() << "EDDH - LIQW result.nodes.size() destination and no distance filter" << result.nodes.size();
    for(int idx : result.nodes)
    {
      const atools::routing::Node& n = airwayNet->getNode(idx);
      QVERIFY(n.pos.getLatY() < 49.7175);
    }
    QVERIFY(result.nodes.size() == result.edges.size());
    QCOMPARE(result.nodes.size(), 1010);
  }

  {
    atools::routing::Result result;
    airwayNet->setParameters(Pos(9.98823, 53.6304), Pos(9.98889, 44.0889), 0,
                             atools::routing::MODE_AIRWAY_AND_WAYPOINT);
    airwayNet->setDirectDistanceFactor(1.02f);
    airwayNet->getNeighbours(result, airwayNet->getNearestNode(Pos(9.947, 49.7175) /* WUR */));
    qDebug() << "EDDH - LIQW result.nodes.size() destination and distance filter" << result.nodes.size();
    for(int idx : result.nodes)
    {
      const atools::routing::Node& n = airwayNet->getNode(idx);
      QVERIFY(n.pos.getLatY() < 49.7175);
    }
    QVERIFY(result.nodes.size() == result.edges.size());
    QCOMPARE(result.nodes.size(), 175);
  }

  {
    atools::routing::Result result;
    airwayNet->setParameters(Pos(9.98823, 53.6304), Pos(9.98889, 44.0889), 0,
                             atools::routing::MODE_AIRWAY_AND_WAYPOINT);
    airwayNet->getNeighbours(result, airwayNet->getNearestNode(Pos(10, 45.4781) /* VAKON */));
    qDebug() << "EDDH - LIQW result.nodes.size() near destination" << result.nodes.size();
    QVERIFY(result.nodes.size() == result.edges.size());
    QCOMPARE(result.nodes.size(), 314);
  }
}

void RouteTest::printNearestResult(const QVector<atools::routing::Node>& neighbours,
                                   const QVector<atools::routing::Edge>& edges)
{
  qDebug() << Q_FUNC_INFO << "neighbours.size()" << neighbours.size();
  qDebug() << "==========================================================";
  for(int i = 0; i < neighbours.size(); i++)
  {
    qDebug() << neighbours.at(i);
    qDebug() << edges.at(i);
  }
}
