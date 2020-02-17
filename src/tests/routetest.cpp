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
  networks.insert(NET_RADIO, net);

  net = new atools::routing::RouteNetwork(db, atools::routing::SOURCE_AIRWAY);
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

  // Airway - new
  QTest::newRow("EDDF to LIRF Airway New")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 527.819f << 21 << int(NET_AIRWAY) << true;
  QTest::newRow("EGAA to LGAV Airway New")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1655.41f << 69 << int(NET_AIRWAY) << true;
  QTest::newRow("ESNQ to FEFF Airway New")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 3980.29f << 89 << int(NET_AIRWAY) << true;

  // Radio navaids - new
  QTest::newRow("EDDF to LIRF Radio New")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 520.505f << 4 << int(NET_RADIO) << true;
  QTest::newRow("EGAA to LGAV Radio New")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1584.73f << 9 << int(NET_RADIO) << true;
  QTest::newRow("ESNQ to FEFF Radio New")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 4192.02f << 21 << int(NET_RADIO) << true;
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

  bool res = router.calculateRoute(from, to, 0, atools::routing::MODE_ALL);

  QCOMPARE(res, result);

  float distance;
  QVector<atools::routing::RouteLeg> routeLegs;
  router.extractLegs(routeLegs, distance);

  QCOMPARE(atools::geo::meterToNm(distance), dist);
  QCOMPARE(routeLegs.size(), num);
}

void RouteTest::testRouteNetwork()
{
  atools::routing::RouteNetwork *radioNet = networks.value(NET_RADIO),
                                *airwayNet = networks.value(NET_AIRWAY);

  airwayNet->clearParameters();
  radioNet->clearParameters();

  QCOMPARE(airwayNet->getNodes().size(), 202670);
  QCOMPARE(radioNet->getNodes().size(), 9073);

  for(const atools::routing::Node& n : radioNet->getNodes())
    QCOMPARE(n.edges.size(), 0);

  for(const atools::routing::Node& n : airwayNet->getNodes())
  {
    // if(n.type == atools::routing::WAYPOINT_JET || n.type == atools::routing::WAYPOINT_VICTOR ||
    // n.type == atools::routing::WAYPOINT_BOTH)
    // QCOMPARE(n.edges.size() > 0, true);

    for(const atools::routing::Edge& e : n.edges)
    {
      // if(e.lengthMeter == 0)
      // qDebug() << n;
      // QCOMPARE(e.lengthMeter > 0, true);
      QCOMPARE(e.airwayId > 0, true);
      QCOMPARE(e.toIndex >= 0, true);
      QCOMPARE(e.toIndex < airwayNet->getNodes().size(), true);
    }
  }

  atools::routing::Node node = airwayNet->getNodeById(103010);
  QCOMPARE(node.edges.size(), 5);

  QVector<atools::routing::Node> nodes;
  QVector<atools::routing::Edge> edges;
  airwayNet->getNeighbours(nodes, edges, node);
  QCOMPARE(nodes.size(), 5);
  QCOMPARE(edges.size(), 5);

  node = airwayNet->getNodeById(203004);
  QCOMPARE(node.edges.size(), 2);
  QCOMPARE(node.edges.at(0).lengthMeter > 0, true);
  QCOMPARE(node.edges.at(1).lengthMeter > 0, true);

  nodes.clear();
  edges.clear();
  airwayNet->getNeighbours(nodes, edges, node);
  QCOMPARE(nodes.size(), 2);
  QCOMPARE(edges.size(), 2);

  // wpNet->getNavIdAndTypeForNode(0, navId, type);
  // QCOMPARE(navId, 28);
  // QCOMPARE(type, atools::routing::WAYPOINT);

  // =====================================================
  // rdNet->addDepartureAndDestinationNodes(Pos(8.57046, 50.0333), Pos(12.2389, 41.8003));
  // atools::routing::Node departureNode = rdNet->getDepartureNode();
  // atools::routing::Node destinationNode = rdNet->getDestinationNode();

  // QVector<atools::routing::Node> neighbours;
  // QVector<atools::routing::Edge> edges;
  // qDebug() << "Departure" << departureNode << "Destination" << destinationNode;
  // rdNet->getNeighbours(departureNode, neighbours, edges, departureNode.pos, destinationNode.pos);
  // printNearestResult(neighbours, edges);

  // rdNet->getNeighbours(neighbours.at(0), neighbours, edges, departureNode.pos, destinationNode.pos);
  // printNearestResult(neighbours, edges);

  // =====================================================
  // airwayNet->addDepartureAndDestinationNodes(Pos(8.57046, 50.0333), Pos(12.2389, 41.8003));
  // departureNode = airwayNet->getDepartureNode();
  // destinationNode = airwayNet->getDestinationNode();
  // qDebug() << "Departure" << departureNode << "Destination" << destinationNode;
  // airwayNet->getNeighbours(departureNode, neighbours, edges, departureNode.pos, destinationNode.pos);
  // printNearestResult(neighbours, edges);
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
