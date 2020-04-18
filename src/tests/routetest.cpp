/*****************************************************************************
* Copyright 2015-2020 Alexander Barthel alex@littlenavmap.org
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
#include "routing/routenetworkloader.h"
#include "sql/sqldatabase.h"
#include "geo/calculations.h"
#include "testutil/testutil.h"

#include "geo/rect.h"
#include "geo/linestring.h"

using atools::sql::SqlDatabase;
using namespace atools::geo;
using namespace atools::routing;

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
  NET_TRACK,
  NET_AIRWAY_TRACK,
  NET_RADIO
};

void RouteTest::initTestCase()
{
  dbNav = testutil::createDb("TESTDBROUTE", "testdata/test_route.sqlite");
  dbTrack = testutil::createDb("TESTDBTRACK", "testdata/test_track.sqlite");

  RouteNetwork *net = new RouteNetwork(SOURCE_RADIO);
  RouteNetworkLoader(dbNav, nullptr).load(net);
  networks.insert(NET_RADIO, net);

  net = new RouteNetwork(SOURCE_AIRWAY);
  RouteNetworkLoader(dbNav, nullptr).load(net);
  networks.insert(NET_AIRWAY, net);

  net = new RouteNetwork(SOURCE_AIRWAY);
  RouteNetworkLoader(nullptr, dbTrack).load(net);
  networks.insert(NET_TRACK, net);

  net = new RouteNetwork(SOURCE_AIRWAY);
  RouteNetworkLoader(dbNav, dbTrack).load(net);
  networks.insert(NET_AIRWAY_TRACK, net);
}

void RouteTest::cleanupTestCase()
{
  qDeleteAll(networks);
  networks.clear();

  testutil::removeDb(dbNav, "TESTDBROUTE");
  testutil::removeDb(dbTrack, "TESTDBTRACK");
}

void RouteTest::testRouteNetworkRadio()
{
  RouteNetwork *net = networks.value(NET_RADIO);

  net->clearParameters();

  QCOMPARE(net->getNodes().size(), 9073);

  for(const Node& n : net->getNodes())
  {
    QCOMPARE(net->getNode(n.index).id, n.id);
    if(!n.pos.isPole())
      QVERIFY(n.pos.almostEqual(net->getNearestNode(n.pos).pos, Pos::POS_EPSILON_1M));

    QCOMPARE(n.edges.size(), 0);
    QVERIFY(n.id > 0);
    QVERIFY(n.pos.isValid());
    QVERIFY(n.index >= 0);
    QVERIFY(n.con == CONNECTION_NONE);
    QVERIFY(n.type == NODE_VOR || n.type == NODE_NDB || n.type == NODE_VORDME);
    QVERIFY(!n.hasAnyAirway());
  }

}

void RouteTest::testRouteNetworkAirway()
{
  RouteNetwork *net = networks.value(NET_AIRWAY);

  net->clearParameters();

  QCOMPARE(net->getNodes().size(), 156801);

  for(const Node& n : net->getNodes())
  {
    QCOMPARE(net->getNode(n.index).id, n.id);
    if(!n.pos.isPole())
      QVERIFY(n.pos.almostEqual(net->getNearestNode(n.pos).pos, Pos::POS_EPSILON_1M));

    QVERIFY(n.edges.size() >= 0);
    QVERIFY(n.id > 0);
    QVERIFY(n.pos.isValid());
    QVERIFY(n.index >= 0);
    QVERIFY(n.con != CONNECTION_TRACK);
    QVERIFY(n.type == NODE_WAYPOINT);

    if(n.edges.isEmpty())
      QVERIFY(n.con == CONNECTION_NONE);

    for(const Edge& e : n.edges)
    {
      QVERIFY(e.isAnyAirway());
      QVERIFY(e.airwayHash > 0);
      QVERIFY(e.id > 0);
      QVERIFY(e.toIndex >= 0);
      QVERIFY(e.lengthMeter >= 0);
      QVERIFY(e.toIndex < net->getNodes().size());
    }
  }
}

void RouteTest::testRouteNetworkTrack()
{
  RouteNetwork *net = networks.value(NET_TRACK);

  net->clearParameters();

  QCOMPARE(net->getNodes().size(), 269);

  int numWestLevels = 0, numEastLevels = 0;

  for(const Node& n : net->getNodes())
  {
    QCOMPARE(net->getNode(n.index).id, n.id);
    if(!n.pos.isPole())
      QVERIFY(n.pos.almostEqual(net->getNearestNode(n.pos).pos, Pos::POS_EPSILON_1M));

    QVERIFY(n.edges.size() >= 0 && n.edges.size() <= 4);
    QVERIFY(n.id > 0);
    QVERIFY(n.index >= 0);
    QVERIFY(n.type == NODE_WAYPOINT);

    if(n.edges.isEmpty())
    {
      QVERIFY(n.con == CONNECTION_NONE || n.con == CONNECTION_TRACK_START_END);
      QVERIFY(!n.hasTracks());
    }
    else
    {
      QVERIFY(n.con & CONNECTION_TRACK);
      QVERIFY(n.hasTracks());
    }

    for(const Edge& e : n.edges)
    {
      if(e.hasAltLevels)
      {
        QVERIFY(net->getAltitudeLevelsEast(e.id).size() + net->getAltitudeLevelsWest(e.id).size() > 0);
        numEastLevels += net->getAltitudeLevelsEast(e.id).size();
        numWestLevels += net->getAltitudeLevelsWest(e.id).size();
      }
      else
      {
        QCOMPARE(net->getAltitudeLevelsEast(e.id).size(), 0);
        QCOMPARE(net->getAltitudeLevelsWest(e.id).size(), 0);
      }

      QVERIFY(e.isTrack());
      QVERIFY(e.airwayHash > 0);
      QVERIFY(e.id >= 0);
      QVERIFY(e.toIndex >= 0);
      QVERIFY(e.lengthMeter > 1000);
      QVERIFY(e.toIndex < net->getNodes().size());
    }
  }

  // Z -> 350, 370, 380, 390, 400 * 6
  QCOMPARE(numEastLevels, 30);
  // A <- 340, 350, 360, 370, 380, 390 * 5
  QCOMPARE(numWestLevels, 30);
}

void RouteTest::testRouteNetworkAirwayTrack()
{
  RouteNetwork *net = networks.value(NET_AIRWAY_TRACK);

  net->clearParameters();

  QCOMPARE(net->getNodes().size(), 156882);

  for(const Node& n : net->getNodes())
  {
    QCOMPARE(net->getNode(n.index).id, n.id);
    if(!n.pos.isPole())
      QVERIFY(n.pos.almostEqual(net->getNearestNode(n.pos).pos, Pos::POS_EPSILON_1M));

    QVERIFY(n.edges.size() >= 0);
    QVERIFY(n.id > 0);
    QVERIFY(n.index >= 0);
    QVERIFY(n.type == NODE_WAYPOINT);

    if(n.edges.isEmpty())
    {
      QVERIFY(n.con == CONNECTION_NONE || n.con == CONNECTION_TRACK_START_END);
      QVERIFY(!n.hasTracks());
    }
    else
    {
      QVERIFY(n.con & CONNECTION_AIRWAY_TRACK);
      QVERIFY(n.hasTracks() || n.hasAnyAirway());
    }

    for(const Edge& e : n.edges)
    {
      QVERIFY(e.isTrack() || e.isAnyAirway());
      QVERIFY(e.airwayHash > 0);
      QVERIFY(e.id >= 0);
      QVERIFY(e.toIndex >= 0);
      QVERIFY(e.lengthMeter >= 0);
      QVERIFY(e.toIndex < net->getNodes().size());
    }
  }

  // << Pos(-55.8333, 52.4)    /* NEEKO */
  Node node = net->getNearestNode(Pos(-55.8333, 52.4));
  QCOMPARE(node.getConnections() & CONNECTION_TRACK_START_END, CONNECTION_TRACK_START_END);
  QCOMPARE(node.getConnections() & CONNECTION_TRACK, CONNECTION_TRACK);
  QCOMPARE(node.getConnections() & CONNECTION_JET, CONNECTION_JET);

  // << Pos(-14, 55)    /* NETKI */
  node = net->getNearestNode(Pos(-14., 55.));
  QCOMPARE(node.getConnections() & CONNECTION_TRACK_START_END, CONNECTION_TRACK_START_END);
  QCOMPARE(node.getConnections() & CONNECTION_TRACK, CONNECTION_NONE);
  QCOMPARE(node.getConnections() & CONNECTION_AIRWAY, CONNECTION_NONE);

  // << Pos(-30, 56) /* 5630N */
  node = net->getNearestNode(Pos(-30., 56.));
  QCOMPARE(node.getConnections() & CONNECTION_TRACK_START_END, CONNECTION_NONE);
  QCOMPARE(node.getConnections() & CONNECTION_TRACK, CONNECTION_TRACK);
  QCOMPARE(node.getConnections() & CONNECTION_AIRWAY, CONNECTION_NONE);
}

void RouteTest::testRouteFinder_data()
{
  QTest::addColumn<Pos>("from");
  QTest::addColumn<Pos>("to");
  QTest::addColumn<float>("dist");
  QTest::addColumn<int>("num");
  QTest::addColumn<int>("network");
  QTest::addColumn<int>("mode");
  QTest::addColumn<bool>("result");

  QTest::newRow("RJAA to Pos(176.392, 26.116) MODE_AIRWAY_WAYPOINT_TRACK PACOTS avoid")
    << Pos(140.386, 35.7653) << Pos(176.392, 26.116) << 1959.f << 11
    << int(NET_AIRWAY_TRACK) << int(MODE_AIRWAY_WAYPOINT_TRACK) << true;

  QTest::newRow("Pos(176.392, 26.116) to RJAA MODE_AIRWAY_WAYPOINT_TRACK PACOTS avoid")
    << Pos(176.392, 26.116) << Pos(140.386, 35.7653) << 1959.f << 11
    << int(NET_AIRWAY_TRACK) << int(MODE_AIRWAY_WAYPOINT_TRACK) << true;

  QTest::newRow("RJAA to Pos(176.392, 26.116) MODE_AIRWAY_WAYPOINT_TRACK PACOTS use")
    << Pos(140.386, 35.7653) << Pos(-161.033, 22.0198) << 3175.f << 11
    << int(NET_AIRWAY_TRACK) << int(MODE_AIRWAY_WAYPOINT_TRACK) << true;

  QTest::newRow("Pos(176.392, 26.116) to RJAA MODE_AIRWAY_WAYPOINT_TRACK PACOTS use")
    << Pos(-161.033, 22.0198) << Pos(140.386, 35.7653) << 3314.f << 17
    << int(NET_AIRWAY_TRACK) << int(MODE_AIRWAY_WAYPOINT_TRACK) << true;

  QTest::newRow("RJTO to KSFO MODE_AIRWAY_WAYPOINT_TRACK")
    << Pos(139.36, 34.7819) << Pos(-122.375, 37.6188) << 4592.f << 24
    << int(NET_AIRWAY_TRACK) << int(MODE_AIRWAY_WAYPOINT_TRACK) << true;
  QTest::newRow("CYML to EIDW MODE_AIRWAY_WAYPOINT_TRACK")
    << Pos(-70.2239, 47.5976) << Pos(-6.27, 53.4214) << 2398.f << 12
    << int(NET_AIRWAY_TRACK) << int(MODE_AIRWAY_WAYPOINT_TRACK) << true;

#ifndef QT_DEBUG
  // Too slow for debug mode
  QTest::newRow("KLAS to LPAZ MODE_AIRWAY_WAYPOINT SLOW")
    << Pos(-115.152, 36.0801) << Pos(-25.1711, 36.9739) << 4210.f << 57
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;
#endif

  QTest::newRow("MMSD to NTTO MODE_WAYPOINT")
    << Pos(-109.721, 23.1521) << Pos(-140.957, -18.0642) << 3143.f << 10
    << int(NET_AIRWAY) << int(MODE_WAYPOINT) << true;
  QTest::newRow("MMSD to NTTO MODE_AIRWAY_AND_WAYPOINT")
    << Pos(-109.721, 23.1521) << Pos(-140.957, -18.0642) << 3276.f << 16
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;
  QTest::newRow("LPAZ to TXKF MODE_AIRWAY_AND_WAYPOINT")
    << Pos(-25.1711, 36.9739) << Pos(-64.6787, 32.364) << 1991.f << 14
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;
  QTest::newRow("LEVX to LPFL MODE_AIRWAY_AND_WAYPOINT")
    << Pos(-8.6275, 42.2292) << Pos(-31.1322, 39.4581) << 1132.f << 11
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;
  QTest::newRow("EDDF to LIRF MODE_AIRWAY_AND_WAYPOINT")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 528.f << 22
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;
  QTest::newRow("EGAA to LGAV MODE_AIRWAY_AND_WAYPOINT")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1625.28f << 52
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;
  QTest::newRow("ESNQ to FEFF MODE_AIRWAY_AND_WAYPOINT")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 3974.f << 102
    << int(NET_AIRWAY) << int(MODE_AIRWAY_WAYPOINT) << true;

  // Radio navaids
  QTest::newRow("LEVX to LPMA MODE_RADIONAV")
    << Pos(-8.6275, 42.2292) << Pos(-16.7781, 32.6942) << 723.f << 3
    << int(NET_RADIO) << int(MODE_RADIONAV) << true;
  QTest::newRow("LEVX to LPFL MODE_RADIONAV")
    << Pos(-8.6275, 42.2292) << Pos(-31.1322, 39.4581) << 1252.73f << 4
    << int(NET_RADIO) << int(MODE_RADIONAV) << true;
  QTest::newRow("EDDF to LIRF MODE_RADIONAV")
    << Pos(8.57046, 50.0333) << Pos(12.2389, 41.8003) << 520.505f << 4
    << int(NET_RADIO) << int(MODE_RADIONAV) << true;
  QTest::newRow("EGAA to LGAV MODE_RADIONAV")
    << Pos(-6.21583, 54.6575) << Pos(23.9445, 37.9367) << 1584.73f << 9
    << int(NET_RADIO) << int(MODE_RADIONAV) << true;
  QTest::newRow("ESNQ to FEFF MODE_RADIONAV")
    << Pos(20.3356, 67.8213) << Pos(18.5197, 4.39778) << 3948.21f << 21
    << int(NET_RADIO) << int(MODE_RADIONAV) << true;
}

void RouteTest::testRouteFinder()
{
  QFETCH(Pos, from);
  QFETCH(Pos, to);
  QFETCH(float, dist);
  QFETCH(int, num);
  QFETCH(int, network);
  QFETCH(int, mode);
  QFETCH(bool, result);

  int numCalled = 0;
  RouteFinder router(networks.value(network));
  router.setProgressCallback([&numCalled](int distMeterToDest, int currentDistMeterToDest) -> bool {
    Q_UNUSED(distMeterToDest)
    Q_UNUSED(currentDistMeterToDest)
    // qDebug() << distMeterToDest << currentDistMeterToDest
    // << (100.f / distMeterToDest * (distMeterToDest - currentDistMeterToDest)) << "%";
    numCalled++;
    return true;
  });

  bool actualResult = router.calculateRoute(from, to, 0, Mode(mode));

  float actualDistance;
  QVector<RouteLeg> routeLegs;
  router.extractLegs(routeLegs, actualDistance);

  qDebug() << "res" << actualResult
           << "distance" << meterToNm(actualDistance) << "expected" << dist
           << "size" << routeLegs.size() << "expected" << num << "numCalled" << numCalled;

  // qDebug() << routeLegs;

  QCOMPARE(actualResult, result);

  QVERIFY(atools::almostEqual(meterToNm(actualDistance), dist, 10.f));
  QVERIFY(atools::almostEqual(routeLegs.size(), num, 5));
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

  // << Pos(-55.8333, 52.4)    /* NEEKO */
  // << Pos(-14, 55)    /* NETKI */
  // << Pos(-30, 56) /* 5630N */
  // << Pos(-56.2841, 52.2143)    /* Near NEEKO */
  // << Pos(-13.1078, 54.9997)    /* Near NETKI */

  // N555A
  // EBONY/K6 Pos(-67.1566, 44.9024) -> NEEKO/CY Pos(-55.8333, 52.4)
  // airway_id = 26718

  QTest::newRow("NAT MODE_AIRWAY_WAYPOINT_TRACK NET_AIRWAY_TRACK EBONY West")
    << Pos(-67.1566, 44.9024)      /* EBONY */
    << Pos(-67.1726, 44.8967) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_WAYPOINT_TRACK)
    << int(NET_AIRWAY_TRACK)
    << 18
    << Rect();    /* Do not test */

  QTest::newRow("NAT MODE_AIRWAY_TRACK NET_AIRWAY_TRACK NETKI East")
    << Pos(-13.1078, 54.9997)      /* NETKI */
    << Pos(-56.2841, 52.2143) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_WAYPOINT_TRACK)
    << int(NET_AIRWAY_TRACK)
    << 1
    << Rect(-13.1078, 54.9997);

  QTest::newRow("NAT MODE_AIRWAY_TRACK NET_AIRWAY_TRACK 5630N Center")
    << Pos(-30., 56.)    /* 5630N */
    << Pos(-56.2841, 52.2143) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_TRACK)
    << int(NET_AIRWAY_TRACK)
    << 1
    << Rect(-20., 56.); /* 5620N */

  QTest::newRow("NAT MODE_AIRWAY_TRACK NET_AIRWAY_TRACK NEEKO West")
    << Pos(-55.8333, 52.4)    /* NEEKO */
    << Pos(-56.2841, 52.2143) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_TRACK)
    << int(NET_AIRWAY_TRACK)
    << 1
    << Rect(-50., 54.);  /* 5450N */

  // *************************************************
  QTest::newRow("NAT MODE_AIRWAY_TRACK NEEKO West")
    << Pos(-55.8333, 52.4)   /* NEEKO */
    << Pos(-56.2841, 52.2143) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_TRACK)
    << int(NET_TRACK)
    << 1
    << Rect(-50., 54.); /* 5450N */

  QTest::newRow("NAT MODE_AIRWAY_TRACK NETKI East")
    << Pos(-13.1078, 54.9997)      /* NETKI */
    << Pos(-56.2841, 52.2143) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_TRACK)
    << int(NET_TRACK)
    << 1
    << Rect(-13.1078, 54.9997);

  QTest::newRow("NAT MODE_AIRWAY_TRACK 5630N Center")
    << Pos(-30., 56.)    /* 5630N */
    << Pos(-56.2841, 52.2143) << Pos(-13.1078, 54.9997)
    << int(MODE_AIRWAY_TRACK)
    << int(NET_TRACK)
    << 1
    << Rect(-20., 56.); /* 5620N */

  // *************************************************
  QTest::newRow("LPAZ to TXKF MODE_AIRWAY_WAYPOINT center of route")
    << Pos(-45., 36.)    /* Waypoint: 3645N */
    << Pos(-25.1711, 36.9739) << Pos(-64.6787, 32.364)
    << int(MODE_AIRWAY_WAYPOINT)
    << int(NET_AIRWAY)
    << 10
    << Rect(Pos(-45., 36.), nmToMeter(600.f));

  QTest::newRow("EDDH - LIQW MODE_AIRWAY far from destination")
    << Pos(9.947, 49.7175)     /* WUR */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(MODE_AIRWAY)
    << int(NET_AIRWAY)
    << 3
    << Rect(Pos(9.947, 49.7175), nmToMeter(340.f));

  QTest::newRow("EDDH - LIQW MODE_WAYPOINT far from destination")
    << Pos(9.947, 49.7175)       /* WUR */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(MODE_WAYPOINT)
    << int(NET_AIRWAY)
    << 308
    << Rect(Pos(9.947, 49.7175), nmToMeter(340.f));

  QTest::newRow("EDDH - LIQW MODE_AIRWAY_WAYPOINT near destination")
    << Pos(10, 45.4781) /* VAKON */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(MODE_AIRWAY_WAYPOINT)
    << int(NET_AIRWAY)
    << 21
    << Rect(LineString({Pos(6, 45.4781), Pos(16, 44.0889)}));

  QTest::newRow("EDDH - LIQW MODE_AIRWAY_WAYPOINT far from destination")
    << Pos(9.947, 49.7175)     /* WUR */
    << Pos(9.98823, 53.6304) << Pos(9.98889, 44.0889)
    << int(MODE_AIRWAY_WAYPOINT)
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

  RouteNetwork *net = networks.value(network);
  net->setParameters(departure, destination, 0, Mode(mode));

  Result result;
  net->getNeighbours(result, origin);

  // printNearestResult(net, origin, destination, result);

  QVERIFY(result.nodes.size() == result.edges.size());
  QCOMPARE(result.nodes.size(), num);

  if(rect.isValid())
  {
    for(int idx : result.nodes)
    {
      const Node& n = net->getNode(idx);
      QVERIFY(rect.contains(n.getPosition()));
    }
  }
}

void RouteTest::printNearestResult(RouteNetwork *net,
                                   const Pos& origin, const Pos& dest,
                                   const Result& result)
{
  qDebug() << "==========================================================";
  qDebug() << "neighbours.size()" << result.size()
           << "origin to dest" << meterToNm(dest.distanceMeterTo(origin)) << "nm";

  for(int i = 0; i < result.size(); i++)
  {
    const Node& node = net->getNode(result.nodes.at(i));
    qDebug() << "to origin" << meterToNm(node.getPosition().distanceMeterTo(origin)) << "nm"
             << "to dest" << meterToNm(node.getPosition().distanceMeterTo(dest)) << "nm" << endl
             << "#" << i << "index" << result.nodes.at(i) << node;
    qDebug() << result.edges.at(i);
  }
}
