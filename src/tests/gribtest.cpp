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

#include "gribtest.h"

#include "atools.h"
#include "grib/gribreader.h"
#include "grib/gribdownloader.h"
#include "grib/gribwindquery.h"
#include "geo/calculations.h"
#include "geo/rect.h"
#include "geo/linestring.h"

using atools::grib::GribDownloader;
using atools::grib::GribReader;
using atools::grib::GribWindQuery;
using atools::geo::LineString;
using atools::geo::Pos;
using atools::geo::Rect;
using atools::grib::WindPosVector;
using atools::grib::WindPos;
using atools::grib::Wind;

// Use grib_ls to verify results:
// lonx=0; laty=2 ; level=700 ;grib_ls -l$laty,$(($lonx+180)),1  -p level,shortName -w level=$level -F%4.8f testdata/lnm_winds.grib|grep $level

GribTest::GribTest()
{

}

void GribTest::runtest(int argc, char *argv[])
{
  GribTest tst;
  QTest::qExec(&tst, argc, argv);
}

void GribTest::initTestCase()
{
  query = new GribWindQuery(this, verbose);
  query->initFromFile("testdata/lnm_winds.grib");
  // query->initFromFile("testdata/global_winds.grib");
}

void GribTest::cleanupTestCase()
{
  delete query;
}

void GribTest::testGribWindLineStringQuery_data()
{
  QTest::addColumn<LineString>("line");
  QTest::addColumn<float>("alt");
  QTest::addColumn<float>("speed"); // knots
  QTest::addColumn<float>("dir"); // deg

  /* *INDENT-OFF* */
  QTest::newRow("Point 10000 ft")             << LineString(Pos(2.f, 2.f), Pos(2.f, 2.f)) << 10000.f << 18.7237f << 275.828f;
  QTest::newRow("West east 4 deg 260 ft")     << LineString(Pos(-2.f, 0.f), Pos(2.f, 0.f)) << 260.f << 14.6721f << 250.187f;
  QTest::newRow("North south 4 deg 10000 ft") << LineString(Pos(0.f, 2.f), Pos(0.f, -2.f)) << 10000.f << 18.0959f << 277.099f;
  QTest::newRow("North south 4 deg 15000 ft") << LineString({Pos(0.f, 2.f), Pos(0.f, 1.f), Pos(0.f, 0.f), Pos(0.f, -1.f), Pos(0.f, -2.f)}) << 15000.f << 13.7128f << 300.16f;
  QTest::newRow("North south 4 deg 20000 ft") << LineString(Pos(0.f, 2.f), Pos(0.f, -2.f)) << 20000.f << 9.23416f << 323.71f;
  QTest::newRow("North south 4 deg 50000 ft") << LineString(Pos(0.f, 2.f), Pos(0.f, -2.f)) << 50000.f << 13.3324f << 65.1409f;

  QTest::newRow("East west anti meridian 4 deg 10000 ft") << LineString({Pos(178.f, 0.f), Pos(-178.f, 0.f)}) << 10000.f << 22.7628f << 255.559f;
  QTest::newRow("North-west south-east 178 deg 10000 ft") << LineString({Pos(-178.f, 80.f), Pos(0.f, 0.f), Pos(178.f, -80.f)}) << 10000.f << 20.8477f << 167.584f;
  /* *INDENT-ON* */

}

void GribTest::testGribWindLineStringQuery()
{
  QFETCH(LineString, line);
  QFETCH(float, alt);
  QFETCH(float, speed);
  QFETCH(float, dir);

  atools::grib::Wind wind = query->getWindAverageForLineString(line, alt);

  qInfo() << "w1:" << query->getWindForPos(line.first(), alt);
  qInfo() << "w2:" << query->getWindForPos(line.last(), alt);

  qInfo() << line << "alt" << alt << "speed" << speed << "dir" << dir
          << "wind.speed" << wind.speed << "wind.dir" << wind.dir;

  if(speed > 0.f)
    QCOMPARE(wind.speed, speed);
  if(dir > 0.f)
    QCOMPARE(wind.dir, dir);
}

void GribTest::testGribWindLineQuery_data()
{
  QTest::addColumn<Pos>("pos1");
  QTest::addColumn<Pos>("pos2");
  QTest::addColumn<float>("alt");
  QTest::addColumn<float>("speed"); // knots
  QTest::addColumn<float>("dir"); // deg

  /* *INDENT-OFF* */
  QTest::newRow("Point 10000 ft") << Pos(2.f, 2.f) << Pos(2.f, 2.f) << 10000.f << 18.7237f << 275.828f;
  QTest::newRow("West east 4 deg 260 ft") << Pos(-2.f, 0.f) << Pos(2.f, 0.f) << 260.f << 14.6721f << 250.187f;
  QTest::newRow("North south 4 deg 10000 ft") << Pos(0.f, 2.f) << Pos(0.f, -2.f) << 10000.f << 18.0959f << 277.099f;
  QTest::newRow("North south 4 deg 15000 ft") << Pos(0.f, 2.f) << Pos(0.f, -2.f) << 15000.f << 13.665f << 300.404f;
  QTest::newRow("North south 4 deg 20000 ft") << Pos(0.f, 2.f) << Pos(0.f, -2.f) << 20000.f << 9.23416f << 323.71f;
  QTest::newRow("North south 4 deg 50000 ft") << Pos(0.f, 2.f) << Pos(0.f, -2.f) << 50000.f << 13.3324f << 65.1409f;

  QTest::newRow("East west anti meridian 4 deg 10000 ft") << Pos(178.f, 0.f) << Pos(-178.f, 0.f) << 10000.f << 22.7628f << 255.559f;
  QTest::newRow("North-west south-east 178 deg 10000 ft") << Pos(-178.f, 80.f) << Pos(178.f, -80.f) << 10000.f << 22.0036f << 165.739f;
  /* *INDENT-ON* */
}

void GribTest::testGribWindLineQuery()
{
  QFETCH(Pos, pos1);
  QFETCH(Pos, pos2);
  QFETCH(float, alt);
  QFETCH(float, speed);
  QFETCH(float, dir);

  atools::grib::Wind wind = query->getWindAverageForLine(pos1, pos2, alt);

  // qInfo() << atools::geo::nmToMeter(atools::geo::windUComponent(wind.speed, wind.dir)) / 3600.f;
  // qInfo() << atools::geo::nmToMeter(atools::geo::windVComponent(wind.speed, wind.dir)) / 3600.f;
  // qInfo() << "w1:" << query->getWindForPos(pos1, alt);
  // qInfo() << "w2:" << query->getWindForPos(pos2, alt);
  // qInfo() << pos1 << pos2 << "alt" << alt << "speed" << speed << "dir" << dir
  // << "wind.speed" << wind.speed << "wind.dir" << wind.dir;

  if(speed > 0.f)
    QCOMPARE(wind.speed, speed);
  if(dir > 0.f)
    QCOMPARE(wind.dir, dir);
}

void GribTest::testGribWindRectQuery_data()
{
  QTest::addColumn<Rect>("rect");
  QTest::addColumn<float>("alt");
  QTest::addColumn<atools::grib::WindPosVector>("result");
  QTest::addColumn<QVector<float> >("u");
  QTest::addColumn<QVector<float> >("v");

  /* *INDENT-OFF* */
  QTest::newRow("North 0 ft AGL point") << Rect(-180.f, 90.f, -180.f, 90.f) << 0.f
    << atools::grib::WindPosVector({{Pos(-180.00000000f, 90.00000000f), Wind({314.26669312f, 8.75289440f})}})
    << QVector<float>({-3.22488022f})
    << QVector<float>({3.14337182f});
  QTest::newRow("North 260 ft AGL point") << Rect(-180.f, 90.f, -180.f, 90.f) << 260.f
    << atools::grib::WindPosVector({{Pos(-180.00000000f, 90.00000000f), Wind({314.26669312f, 8.75289440f})}})
    << QVector<float>({-3.22488022f})
    << QVector<float>({3.14337182f});

  QTest::newRow("North 260  ft AGL 1 degree") << Rect(-179.5f, 89.5f, -178.5f, 88.5f) << 260.f
    << atools::grib::WindPosVector({{Pos(-180.00000000f, 90.00000000f), Wind({314.26669312f, 8.75289440f})},
                                    {Pos(-179.00000000f, 90.00000000f), Wind({315.25592041f, 8.76600838f})},
                                    {Pos(-178.00000000f, 90.00000000f), Wind({316.24581909f, 8.75424671f})},
                                    {Pos(-180.00000000f, 89.00000000f), Wind({336.75976563f, 8.44698811f})},
                                    {Pos(-179.00000000f, 89.00000000f), Wind({337.64181519f, 8.45555210f})},
                                    {Pos(-178.00000000f, 89.00000000f), Wind({338.52185059f, 8.46611595f})},
                                    {Pos(-180.00000000f, 88.00000000f), Wind({1.85015428f, 7.53228045f})},
                                    {Pos(-179.00000000f, 88.00000000f), Wind({1.98765481f, 7.57178211f})},
                                    {Pos(-178.00000000f, 88.00000000f), Wind({2.11831951f, 7.63074923f})}})
    << QVector<float>({-3.22488022f,
                       -3.17488074f,
                       -3.11488056f,
                       -1.71488082f,
                       -1.65487945f,
                       -1.59488022f,
                       0.12511963f,
                       0.13511963f,
                       0.14511965f})
    << QVector<float>({3.14337182f,
                       3.20337152f,
                       3.25337124f,
                       3.99337101f,
                       4.02337170f,
                       4.05337191f,
                       3.87337136f,
                       3.89337158f,
                       3.92337132f});

  QTest::newRow("Meridian  10000 ft 2 degrees") << Rect(-2.f, 2.f, 2.f, -2.f) << 10000.f
    << atools::grib::WindPosVector({{Pos(-2.00000000f, 2.00000000f), Wind({281.76263428f, 16.28676224f})},
                                    {Pos(-1.00000000f, 2.00000000f), Wind({277.15979004f, 16.81443596f})},
                                    {Pos(0.00000000f, 2.00000000f), Wind({275.68527222f, 18.01586151f})},
                                    {Pos(1.00000000f, 2.00000000f), Wind({275.32437134f, 18.39533234f})},
                                    {Pos(2.00000000f, 2.00000000f), Wind({275.82824707f, 18.72373009f})},
                                    {Pos(-2.00000000f, 1.00000000f), Wind({278.92733765f, 15.88450527f})},
                                    {Pos(-1.00000000f, 1.00000000f), Wind({278.65310669f, 16.38391304f})},
                                    {Pos(0.00000000f, 1.00000000f), Wind({275.62802124f, 17.60394096f})},
                                    {Pos(1.00000000f, 1.00000000f), Wind({276.19311523f, 18.34528542f})},
                                    {Pos(2.00000000f, 1.00000000f), Wind({277.26345825f, 19.18909073f})},
                                    {Pos(-2.00000000f, 0.00000000f), Wind({278.05987549f, 17.02643585f})},
                                    {Pos(-1.00000000f, 0.00000000f), Wind({276.82904053f, 17.62468529f})},
                                    {Pos(0.00000000f, 0.00000000f), Wind({277.20175171f, 18.42242622f})},
                                    {Pos(1.00000000f, 0.00000000f), Wind({278.01141357f, 19.36010361f})},
                                    {Pos(2.00000000f, 0.00000000f), Wind({277.44848633f, 19.31469917f})},
                                    {Pos(-2.00000000f, -1.00000000f), Wind({278.84121704f, 17.80842400f})},
                                    {Pos(-1.00000000f, -1.00000000f), Wind({279.64147949f, 17.96722984f})},
                                    {Pos(0.00000000f, -1.00000000f), Wind({278.38403320f, 18.23877525f})},
                                    {Pos(1.00000000f, -1.00000000f), Wind({278.11660767f, 18.56019402f})},
                                    {Pos(2.00000000f, -1.00000000f), Wind({278.91290283f, 19.79895210f})},
                                    {Pos(-2.00000000f, -2.00000000f), Wind({280.54418945f, 18.03745651f})},
                                    {Pos(-1.00000000f, -2.00000000f), Wind({279.69366455f, 17.87142754f})},
                                    {Pos(0.00000000f, -2.00000000f), Wind({278.64385986f, 18.21184158f})},
                                    {Pos(1.00000000f, -2.00000000f), Wind({278.94104004f, 19.11184692f})},
                                    {Pos(2.00000000f, -2.00000000f), Wind({276.83483887f, 19.40625000f})}})
    << QVector<float>({-8.20364475f,
                       -8.58364487f,
                       -9.22364616f,
                       -9.42364407f,
                       -9.58364487f,
                       -8.07364464f,
                       -8.33364487f,
                       -9.01364613f,
                       -9.38364601f,
                       -9.79364491f,
                       -8.67364407f,
                       -9.00364494f,
                       -9.40364361f,
                       -9.86364460f,
                       -9.85364342f,
                       -9.05364513f,
                       -9.11364555f,
                       -9.28364468f,
                       -9.45364475f,
                       -10.06364441f,
                       -9.12364388f,
                       -9.06364536f,
                       -9.26364517f,
                       -9.71364498f,
                       -9.91364479f})
    << QVector<float>({1.70824635f,
                       1.07824647f,
                       0.91824567f,
                       0.87824935f,
                       0.97824633f,
                       1.26824498f,
                       1.26824832f,
                       0.88824660f,
                       1.01825082f,
                       1.24824882f,
                       1.22824490f,
                       1.07824874f,
                       1.18824852f,
                       1.38824666f,
                       1.28824508f,
                       1.40824807f,
                       1.54824340f,
                       1.36824560f,
                       1.34825003f,
                       1.57824719f,
                       1.69824874f,
                       1.54824352f,
                       1.40824747f,
                       1.52824652f,
                       1.18824399f});

  QTest::newRow("Anti meridian 10000 ft 4 degrees") << Rect(179.f, 2.f, -179.f, -2.f) << 10000.f
<< atools::grib::WindPosVector({
                                 {Pos(-180.00000000f, 2.00000000f), Wind({252.79507446f, 26.29526138f})},
                                 {Pos(-179.00000000f, 2.00000000f), Wind({250.88574219f, 23.93072128f})},
                                 {Pos(179.00000000f, 2.00000000f), Wind({258.54046631f, 28.48528099f})},
                                 {Pos(-180.00000000f, 1.00000000f), Wind({254.90397644f, 24.04360771f})},
                                 {Pos(-179.00000000f, 1.00000000f), Wind({255.39019775f, 21.82031059f})},
                                 {Pos(179.00000000f, 1.00000000f), Wind({253.99938965f, 26.31294823f})},
                                 {Pos(-180.00000000f, 0.00000000f), Wind({255.43833923f, 22.27740860f})},
                                 {Pos(-179.00000000f, 0.00000000f), Wind({255.30279541f, 22.61266899f})},
                                 {Pos(179.00000000f, 0.00000000f), Wind({255.36074829f, 22.54641724f})},
                                 {Pos(-180.00000000f, -1.00000000f), Wind({256.24987793f, 22.33803177f})},
                                 {Pos(-179.00000000f, -1.00000000f), Wind({255.83741760f, 22.09739494f})},
                                 {Pos(179.00000000f, -1.00000000f), Wind({257.36709595f, 23.03290176f})},
                                 {Pos(-180.00000000f, -2.00000000f), Wind({252.06182861f, 21.15172386f})},
                                 {Pos(-179.00000000f, -2.00000000f), Wind({252.21694946f, 21.52109909f})},
                                 {Pos(179.00000000f, -2.00000000f), Wind({253.17553711f, 22.70875740f})}})
  << QVector<float>({-12.92364597f,
                     -11.63364601f,
                     -14.36364460f,
                     -11.94364357f,
                     -10.86364460f,
                     -13.01364326f,
                     -11.09364510f,
                     -11.25364590f,
                     -11.22364521f,
                     -11.16364479f,
                     -11.02364445f,
                     -11.56364441f,
                     -10.35364342f,
                     -10.54364586f,
                     -11.18364429f})
  << QVector<float>({-4.00175190f,
                     -4.03175306f,
                     -2.91175151f,
                     -3.22175455f,
                     -2.83175206f,
                     -3.73175454f,
                     -2.88175297f,
                     -2.95175099f,
                     -2.93175292f,
                     -2.73175263f,
                     -2.78175330f,
                     -2.59175324f,
                     -3.35175657f,
                     -3.38175440f,
                     -3.38175511f});

  QTest::newRow("Meridian  25000 ft 2 degrees") << Rect(-2.f, 2.f, 2.f, -2.f) << 25000.f
    << atools::grib::WindPosVector({{Pos(-2.00000000f, 2.00000000f), Wind({182.72377014f, 10.82728577f})},
                                    {Pos(-1.00000000f, 2.00000000f), Wind({187.29034424f, 11.94995499f})},
                                    {Pos(0.00000000f, 2.00000000f), Wind({191.03895569f, 13.63262177f})},
                                    {Pos(1.00000000f, 2.00000000f), Wind({193.03652954f, 14.54140282f})},
                                    {Pos(2.00000000f, 2.00000000f), Wind({193.66143799f, 14.46808338f})},
                                    {Pos(-2.00000000f, 1.00000000f), Wind({165.86511230f, 12.59752464f})},
                                    {Pos(-1.00000000f, 1.00000000f), Wind({172.88606262f, 12.89711094f})},
                                    {Pos(0.00000000f, 1.00000000f), Wind({176.04037476f, 13.61309814f})},
                                    {Pos(1.00000000f, 1.00000000f), Wind({174.49487305f, 13.88189507f})},
                                    {Pos(2.00000000f, 1.00000000f), Wind({173.45925903f, 13.42145824f})},
                                    {Pos(-2.00000000f, 0.00000000f), Wind({162.17823792f, 12.92967415f})},
                                    {Pos(-1.00000000f, 0.00000000f), Wind({166.07196045f, 13.66066647f})},
                                    {Pos(0.00000000f, 0.00000000f), Wind({167.11424255f, 14.11333942f})},
                                    {Pos(1.00000000f, 0.00000000f), Wind({163.14384460f, 12.89450359f})},
                                    {Pos(2.00000000f, 0.00000000f), Wind({164.13891602f, 13.09103489f})},
                                    {Pos(-2.00000000f, -1.00000000f), Wind({162.20960999f, 12.75462818f})},
                                    {Pos(-1.00000000f, -1.00000000f), Wind({160.78140259f, 13.39972782f})},
                                    {Pos(0.00000000f, -1.00000000f), Wind({161.32879639f, 12.96193886f})},
                                    {Pos(1.00000000f, -1.00000000f), Wind({159.03741455f, 12.64882469f})},
                                    {Pos(2.00000000f, -1.00000000f), Wind({156.68128967f, 13.29855537f})},
                                    {Pos(-2.00000000f, -2.00000000f), Wind({161.94941711f, 11.38784218f})},
                                    {Pos(-1.00000000f, -2.00000000f), Wind({158.75415039f, 11.62555885f})},
                                    {Pos(0.00000000f, -2.00000000f), Wind({156.11090088f, 11.56109428f})},
                                    {Pos(1.00000000f, -2.00000000f), Wind({153.47297668f, 11.89438248f})},
                                    {Pos(2.00000000f, -2.00000000f), Wind({154.03050232f, 13.08754063f})}})
    << QVector<float>({-0.26472425f,
                       -0.78020412f,
                       -1.34302378f,
                       -1.68764460f,
                       -1.75812960f,
                       1.58281457f,
                       0.82177430f,
                       0.48365000f,
                       0.68519503f,
                       0.78659105f,
                       2.03600526f,
                       1.69177461f,
                       1.61934400f,
                       1.92374110f,
                       1.84082401f,
                       2.00501943f,
                       2.26939273f,
                       2.13498521f,
                       2.32824636f,
                       2.70844030f,
                       1.81547916f,
                       2.16748309f,
                       2.40884280f,
                       2.73318577f,
                       2.94859242f})
    << QVector<float>({-5.56439352f,
                       -6.09860134f,
                       -6.88426542f,
                       -7.28878832f,
                       -7.23329258f,
                       -6.28524542f,
                       -6.58453894f,
                       -6.98728085f,
                       -7.10935402f,
                       -6.86045361f,
                       -6.33314848f,
                       -6.82182837f,
                       -7.07851028f,
                       -6.34924316f,
                       -6.47896004f,
                       -6.24850750f,
                       -6.51000166f,
                       -6.31799603f,
                       -6.07714796f,
                       -6.28327847f,
                       -5.57073021f,
                       -5.57487011f,
                       -5.43865490f,
                       -5.47545242f,
                       -6.05368853f});

  QTest::newRow("Meridian  50000 ft 2 degrees") << Rect(-2.f, 2.f, 2.f, -2.f) << 50000.f
    << atools::grib::WindPosVector({{Pos(-2.00000000f, 2.00000000f), Wind({84.74573517f, 23.57729721f})},
                                    {Pos(-1.00000000f, 2.00000000f), Wind({78.30400848f, 21.19728470f})},
                                    {Pos(0.00000000f, 2.00000000f), Wind({70.41102600f, 16.87480164f})},
                                    {Pos(1.00000000f, 2.00000000f), Wind({70.32035828f, 15.64581871f})},
                                    {Pos(2.00000000f, 2.00000000f), Wind({70.62990570f, 17.05804062f})},
                                    {Pos(-2.00000000f, 1.00000000f), Wind({76.48110962f, 17.55056953f})},
                                    {Pos(-1.00000000f, 1.00000000f), Wind({78.83277130f, 19.17663765f})},
                                    {Pos(0.00000000f, 1.00000000f), Wind({74.54184723f, 18.30972290f})},
                                    {Pos(1.00000000f, 1.00000000f), Wind({72.93532562f, 16.63028336f})},
                                    {Pos(2.00000000f, 1.00000000f), Wind({68.33551025f, 15.85168934f})},
                                    {Pos(-2.00000000f, 0.00000000f), Wind({64.35442352f, 13.97019482f})},
                                    {Pos(-1.00000000f, 0.00000000f), Wind({59.49261856f, 11.91047096f})},
                                    {Pos(0.00000000f, 0.00000000f), Wind({61.13041306f, 11.71792221f})},
                                    {Pos(1.00000000f, 0.00000000f), Wind({65.68965149f, 12.32643700f})},
                                    {Pos(2.00000000f, 0.00000000f), Wind({64.73508453f, 13.71116352f})},
                                    {Pos(-2.00000000f, -1.00000000f), Wind({53.91272736f, 11.25507450f})},
                                    {Pos(-1.00000000f, -1.00000000f), Wind({64.16632843f, 10.75318718f})},
                                    {Pos(0.00000000f, -1.00000000f), Wind({66.29430389f, 9.72136021f})},
                                    {Pos(1.00000000f, -1.00000000f), Wind({56.37684250f, 12.32335854f})},
                                    {Pos(2.00000000f, -1.00000000f), Wind({60.61586380f, 13.11505604f})},
                                    {Pos(-2.00000000f, -2.00000000f), Wind({65.72109985f, 9.97796249f})},
                                    {Pos(-1.00000000f, -2.00000000f), Wind({58.38780212f, 6.34392595f})},
                                    {Pos(0.00000000f, -2.00000000f), Wind({49.40087891f, 10.18716908f})},
                                    {Pos(1.00000000f, -2.00000000f), Wind({39.83567047f, 11.16449547f})},
                                    {Pos(2.00000000f, -2.00000000f), Wind({44.72861862f, 9.05775833f})}})
    << QVector<float>({12.07965279f,
                       10.67965412f,
                       8.17965412f,
                       7.57965422f,
                       8.27965546f,
                       8.77965546f,
                       9.67965508f,
                       9.07965279f,
                       8.17965412f,
                       7.57965422f,
                       6.47965384f,
                       5.27965450f,
                       5.27965450f,
                       5.77965403f,
                       6.37965441f,
                       4.67965364f,
                       4.97965384f,
                       4.57965374f,
                       5.27965450f,
                       5.87965393f,
                       4.67965364f,
                       2.77965403f,
                       3.97965431f,
                       3.67965388f,
                       3.27965403f})
    << QVector<float>({1.11087072f,
                       2.21087098f,
                       2.91087031f,
                       2.71087122f,
                       2.91087031f,
                       2.11087012f,
                       1.91087008f,
                       2.51086998f,
                       2.51087022f,
                       3.01087046f,
                       3.11086965f,
                       3.11086988f,
                       2.91087055f,
                       2.61087084f,
                       3.01087046f,
                       3.41087055f,
                       2.41087031f,
                       2.01087093f,
                       3.51087046f,
                       3.31086993f,
                       2.11087084f,
                       1.71087027f,
                       3.41087055f,
                       4.41087055f,
                       3.31087041f});
  /* *INDENT-ON* */
}

void GribTest::testGribWindRectQuery()
{
  QFETCH(Rect, rect);
  QFETCH(float, alt);
  QFETCH(atools::grib::WindPosVector, result);
  QFETCH(QVector<float>, u);
  QFETCH(QVector<float>, v);

  atools::grib::WindPosVector winds = query->getWindForRect(rect, alt);

  // qInfo() << "winds" << winds.size();

  QVector<float> uCalculated, vCalculated;
  QStringList uCalculatedStr, vCalculatedStr, windPosStr;

  for(atools::grib::WindPos wp : winds)
  {
    float uCalc = atools::geo::nmToMeter(atools::geo::windUComponent(wp.wind.speed, wp.wind.dir)) / 3600.f;
    float vCalc = atools::geo::nmToMeter(atools::geo::windVComponent(wp.wind.speed, wp.wind.dir)) / 3600.f;
    uCalculated.append(uCalc);
    vCalculated.append(vCalc);
    uCalculatedStr.append(QString::number(uCalc, 'f', 8) + "f");
    vCalculatedStr.append(QString::number(vCalc, 'f', 8) + "f");

    windPosStr.append(QString("{Pos(%1f, %2f), Wind({%3f, %4f})}").
                      arg(wp.pos.getLonX(), 0, 'f', 8).arg(wp.pos.getLatY(), 0, 'f', 8).
                      arg(wp.wind.dir, 0, 'f', 8).arg(wp.wind.speed, 0, 'f', 8));
  }

  // qInfo().noquote().nospace()
  // << "=========================================================================" << endl
  // << "<< atools::grib::WindPosVector({" + windPosStr.join(",") + "}) " << endl
  // << "<< QVector<float>({" + uCalculatedStr.join(",") + "}) " << endl
  // << "<< QVector<float>({" + vCalculatedStr.join(",") + "});";
  // qInfo() << "=========================================================================";

  for(int i = 0; i < winds.size(); i++)
  {
    atools::grib::WindPos wp = winds.at(i);

    // qDebug() << wp;
    // qDebug() << "u" << uCalculated.at(i) << "v" << vCalculated.at(i);

    QCOMPARE(wp.pos.isValid(), true);
    QCOMPARE(wp.wind.dir > 0.f, true);
    QCOMPARE(wp.wind.speed >= 0.f, true);
  }

  if(!result.isEmpty())
  {
    QCOMPARE(winds.size(), result.size());
    for(int i = 0; i < winds.size(); i++)
    {
      QCOMPARE(winds.at(i).pos.getLonX(), result.at(i).pos.getLonX());
      QCOMPARE(winds.at(i).pos.getLatY(), result.at(i).pos.getLatY());
      QCOMPARE(winds.at(i).wind.dir, result.at(i).wind.dir);
      QCOMPARE(winds.at(i).wind.speed, result.at(i).wind.speed);
    }
  }

  QCOMPARE(uCalculated.size(), vCalculated.size());

  if(!v.isEmpty())
  {
    QCOMPARE(v.size(), vCalculated.size());
    for(int i = 0; i < v.size(); i++)
      QCOMPARE(v.at(i), vCalculated.at(i));
  }

  if(!u.isEmpty())
  {
    QCOMPARE(u.size(), uCalculated.size());
    for(int i = 0; i < u.size(); i++)
      QCOMPARE(u.at(i), uCalculated.at(i));
  }
}

void GribTest::testGribWindQuery_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("speed"); // knots
  QTest::addColumn<float>("dir"); // deg
  QTest::addColumn<float>("u"); // east m/s
  QTest::addColumn<float>("v"); // north m/s

  QTest::newRow("North AGL") << Pos(-180.f, 90.f, 0.f) << 0.f << 0.f << -3.22488f << 3.14337f;
  QTest::newRow("North first 80 m layer") << Pos(-180.f, 90.f, 260.f) << 0.f << 0.f << -3.22488f << 3.14337f;
  QTest::newRow("South first 80 m layer") << Pos(-180.f, -90.f, 260.f) << 0.f << 0.f << 0.35512f << -6.01663f;
  QTest::newRow("Pos 0,0 first 80 m layer") << Pos(0.f, 0.f, 260.f) << 0.f << 0.f << -6.75488f << -2.88663f;
  QTest::newRow("Pos 179,0 first 80 m layer") << Pos(179.f, 0.f, 260.f) << 0.f << 0.f << -2.24488f << 5.91337f;

  QTest::newRow("Pos 0,0 10000 ft") << Pos(0.f, 0.f, 10000.f) << 18.4224f << 277.202f << -9.40365f << 1.18825f;
  QTest::newRow("Pos 0,0 20000 ft") << Pos(0.f, 0.f, 20000.f) << 10.4797f << 316.587f << -3.70558f << 3.91674f;
  QTest::newRow("Pos 0,0 30000 ft") << Pos(0.f, 0.f, 30000.f) << 0.f << 0.f << 2.76723f << 8.70146f;
  QTest::newRow("Pos 0,0 38000 ft") << Pos(0.f, 0.f, 38000.f) << 0.f << 0.f << 5.27965f << 2.91087f;
  QTest::newRow("Pos 0,0 50000 ft") << Pos(0.f, 0.f, 50000.f) << 0.f << 0.f << 5.27965f << 2.91087f;

  QTest::newRow("Pos 0,0 15000 ft interpolated") << Pos(0.f, 0.f, 15000.f)
                                                 << (18.4224f + 10.4797f) / 2.f << (277.202f + 316.587f) / 2.f
                                                 << 0.f << 0.f;
  QTest::newRow("Pos 0,0 12000 ft interpolated") << Pos(0.f, 0.f, 12000.f)
                                                 << atools::interpolate(18.4224f, 10.4797f, 10000.f, 20000.f, 12000.f)
                                                 << atools::interpolate(277.202f, 316.587f, 10000.f, 20000.f, 12000.f)
                                                 << 0.f << 0.f;
  QTest::newRow("Pos 0,0 11000 ft interpolated") << Pos(0.f, 0.f, 11000.f)
                                                 << atools::interpolate(18.4224f, 10.4797f, 10000.f, 20000.f, 11000.f)
                                                 << atools::interpolate(277.202f, 316.587f, 10000.f, 20000.f, 11000.f)
                                                 << 0.f << 0.f;
  QTest::newRow("Pos 0,0 19000 ft interpolated") << Pos(0.f, 0.f, 19000.f)
                                                 << atools::interpolate(18.4224f, 10.4797f, 10000.f, 20000.f, 19000.f)
                                                 << atools::interpolate(277.202f, 316.587f, 10000.f, 20000.f, 19000.f)
                                                 << 0.f << 0.f;
}

void GribTest::testGribWindQuery()
{
  QFETCH(Pos, pos);
  QFETCH(float, speed);
  QFETCH(float, dir);
  QFETCH(float, u);
  QFETCH(float, v);

  atools::grib::Wind wind = query->getWindForPos(pos);
  float uCalculated = atools::geo::nmToMeter(atools::geo::windUComponent(wind.speed, wind.dir)) / 3600.f;
  float vCalculated = atools::geo::nmToMeter(atools::geo::windVComponent(wind.speed, wind.dir)) / 3600.f;

  qInfo() << pos << "speed" << speed << "dir" << dir
          << "wind.speed" << wind.speed << "wind.dir" << wind.dir
          << "u" << u << "v" << v << "uCalculated" << uCalculated << "vCalculated" << vCalculated;

  if(speed > 0.f)
    QCOMPARE(wind.speed, speed);
  if(dir > 0.f)
    QCOMPARE(wind.dir, dir);

  if(u > 0.f)
    QCOMPARE(atools::almostEqual(uCalculated, u, 0.0001f), true);
  if(v > 0.f)
    QCOMPARE(atools::almostEqual(vCalculated, v, 0.0001f), true);
}

void GribTest::testGribDownload()
{
  GribDownloader downloader(this, verbose);

  bool done = false, err = false;

  atools::grib::GribDatasetVector testdatasets;
  connect(&downloader, &GribDownloader::gribDownloadFinished,
          [&testdatasets, &err, &done](const atools::grib::GribDatasetVector & datasets, QString)->void
          {
            testdatasets = datasets;
            done = true;
            err = false;
          });
  connect(&downloader, &GribDownloader::gribDownloadFailed,
          [&err, &done](const QString &, int, QString)->void
          {
            done = true;
            err = true;
          });

  downloader.setParameters({"UGRD", "VGRD"});
  downloader.setSurfaces({-80, 200, 300, 450, 700});
  downloader.startDownload();

  waitForValue(done, 30);

  QCOMPARE(err, false);
  QCOMPARE(testdatasets.size(), 10);
  lnmDatasetTest(testdatasets, downloader.getDatetime());
}

void GribTest::testGribDownloadFail()
{
  GribDownloader downloader(this, verbose);

  bool done = false, err = false;

  atools::grib::GribDatasetVector testdatasets;
  connect(&downloader, &GribDownloader::gribDownloadFinished,
          [&testdatasets, &err, &done](const atools::grib::GribDatasetVector & datasets, QString)->void
          {
            testdatasets = datasets;
            done = true;
            err = false;
          });
  connect(&downloader, &GribDownloader::gribDownloadFailed,
          [&err, &done](const QString &, int, QString)->void
          {
            done = true;
            err = true;
          });

  downloader.setParameters({"UGRD", "VGRD"});
  downloader.setSurfaces({-80, 200, 300, 450, 700});
  downloader.startDownload(QDateTime(QDate(2020, 04, 26), QTime(12, 0, 0), Qt::UTC));

  waitForValue(done, 30);

  QCOMPARE(err, true);
  QCOMPARE(testdatasets.size(), 0);
}

void GribTest::testGribLoadWinds()
{
  GribReader reader(verbose);
  reader.readFile("testdata/global_winds.grib");

  const atools::grib::GribDatasetVector& datasets = reader.getDatasets();

  QCOMPARE(datasets.size(), 4);
  QCOMPARE(datasets.at(0).getData().size(), 65160);
  QCOMPARE(datasets.at(0).getSurface(), 700.f);
  QCOMPARE(datasets.at(0).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(0).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(0).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(0).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));

  QCOMPARE(datasets.at(1).getData().size(), 65160);
  QCOMPARE(datasets.at(1).getSurface(), 700.f);
  QCOMPARE(datasets.at(1).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(1).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(1).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(1).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));

  QCOMPARE(datasets.at(2).getData().size(), 65160);
  QCOMPARE(datasets.at(2).getSurface(), 250.f);
  QCOMPARE(datasets.at(2).getAltFeetCalculated(), 34004.1f);
  QCOMPARE(datasets.at(2).getAltFeetRounded(), 34000.f);
  QCOMPARE(datasets.at(2).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(2).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));

  QCOMPARE(datasets.at(3).getData().size(), 65160);
  QCOMPARE(datasets.at(3).getSurface(), 250.f);
  QCOMPARE(datasets.at(3).getAltFeetCalculated(), 34004.1f);
  QCOMPARE(datasets.at(3).getAltFeetRounded(), 34000.f);
  QCOMPARE(datasets.at(3).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(3).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));
}

void GribTest::testGribLoadLnm()
{
  GribReader reader(verbose);
  // https://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_1p00.pl?file=gfs.t06z.pgrb2.1p00.anl&lev_200_mb=on&lev_300_mb=on&lev_450_mb=on&lev_700_mb=on&var_UGRD=on&var_VGRD=on&dir=%2Fgfs.2019042606
  reader.readFile("testdata/lnm_winds.grib");

  lnmDatasetTest(reader.getDatasets(), QDateTime(QDate(2019, 04, 27), QTime(6, 0, 0), Qt::UTC));
}

void GribTest::lnmDatasetTest(const atools::grib::GribDatasetVector& datasets, const QDateTime& datetime)
{
  QCOMPARE(datasets.size(), 10);

  int i = 0;
  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 80.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 262.467f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 260.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getSurfaceType(), atools::grib::METER_AGL);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 80.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 262.467f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 260.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getSurfaceType(), atools::grib::METER_AGL);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 700.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 700.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 450.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 20815.6f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 20000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 450.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 20815.6f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 20000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 300.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 30069.9f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 30000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 300.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 30069.9f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 30000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 200.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 38637.1f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 38000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 200.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 38637.1f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 38000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
}

void GribTest::waitForValue(bool& done, int seconds)
{
  int ti = 200;

  int i = 0;
  while(done == false && i++ < (seconds * 1000 / ti))
  {
    QApplication::processEvents();
    QThread::msleep(static_cast<unsigned long>(ti));
  }

}
