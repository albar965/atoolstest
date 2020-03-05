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

#include "calctest.h"
#include "geo/calculations.h"

#include "geo/pos.h"

using atools::geo::Pos;

CalcTest::CalcTest()
{

}

void CalcTest::runtest(int argc, char *argv[])
{
  CalcTest tst;
  QTest::qExec(&tst, argc, argv);
}

void CalcTest::initTestCase()
{
}

void CalcTest::cleanupTestCase()
{

}

void CalcTest::testAltitudePressure_data()
{
  QTest::addColumn<float>("altMeter");
  QTest::addColumn<float>("pressureMbar");

  // https://de.wikipedia.org/wiki/Normatmosph%C3%A4re#/media/File:Internationale_Standard_Atmosph%C3%A4re.png
  QTest::newRow("SL") << 0.f << 1013.25f;
  QTest::newRow("5000 ft") << 1524.f << 843.099f;
  QTest::newRow("10000 ft") << 3048.f << 696.86f;
  QTest::newRow("20000 ft") << 6096.f << 465.693f;
  QTest::newRow("30000 ft") << 9144.f << 300.957f;
  QTest::newRow("40000 ft") << 12192.f << 186.947f;
}

void CalcTest::testAltitudePressure()
{
  QFETCH(float, altMeter);
  QFETCH(float, pressureMbar);

  QCOMPARE(atools::geo::pressureMbarForAltMeter(altMeter), pressureMbar);
  QCOMPARE(atools::geo::altMeterForPressureMbar(pressureMbar), altMeter);

}

void CalcTest::testSunsetSunrise_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<QDate>("date");
  QTest::addColumn<float>("zenith");
  QTest::addColumn<QTime>("result");
  QTest::addColumn<bool>("neverrise");
  QTest::addColumn<bool>("neverset");

  float sunriseOfficial = 90.f + 50.f / 60.f;
  float sunsetOfficial = -(90.f + 50.f / 60.f);

  // http://edwilliams.org/sunrise_sunset_example.htm
  QTest::newRow("Example Rising") << Pos(-74.3, 40.9) << QDate(1990, 6, 25) << sunriseOfficial << QTime(9, 26, 29)
                                  << false << false;
  QTest::newRow("Example Setting") << Pos(-74.3, 40.9) << QDate(1990, 6, 25) << sunsetOfficial << QTime(0, 33, 0)
                                   << false << false;

  QTest::newRow("EDDF Rise") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << 90.f << QTime(3, 56, 45)
                             << false << false;
  QTest::newRow("EDDF Set") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << -90.f << QTime(19, 5, 53)
                            << false << false;

  QTest::newRow("EDDF Rise") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << sunriseOfficial << QTime(3, 50, 43)
                             << false << false;
  QTest::newRow("EDDF Set") << Pos(8.67972, 50.11361) << QDate(2018, 7, 30) << sunsetOfficial << QTime(19, 11, 54)
                            << false << false;

  QTest::newRow("YSSY Rise") << Pos(151.177, -33.9461) << QDate(2018, 7, 30) << sunriseOfficial << QTime(20, 50, 0)
                             << false << false;
  QTest::newRow("YSSY Set") << Pos(151.177, -33.9461) << QDate(2018, 7, 30) << sunsetOfficial << QTime(7, 13, 46)
                            << false << false;

  QTest::newRow("SUMU Rise") << Pos(-56.0281, -34.8339) << QDate(2018, 7, 30) << sunriseOfficial << QTime(10, 40, 07)
                             << false << false;
  QTest::newRow("SUMU Set") << Pos(-56.0281, -34.8339) << QDate(2018, 7, 30) << sunsetOfficial << QTime(21, 1, 15)
                            << false << false;
}

void CalcTest::testSunsetSunrise()
{
  QFETCH(Pos, pos);
  QFETCH(QDate, date);
  QFETCH(float, zenith);
  QFETCH(QTime, result);
  QFETCH(bool, neverrise);
  QFETCH(bool, neverset);

  bool neverRises, neverSets;
  QTime time = atools::geo::calculateSunriseSunset(neverRises, neverSets, pos, date, zenith);
  // qDebug() << pos << date << zenith;
  // qDebug() << time << QDateTime(date, time).toLocalTime();
  QCOMPARE(time, result);
  QCOMPARE(neverrise, neverRises);
  QCOMPARE(neverset, neverSets);
}
