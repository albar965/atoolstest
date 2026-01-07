/*****************************************************************************
* Copyright 2015-2026 Alexander Barthel alex@littlenavmap.org
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

namespace ageo = atools::geo;
using ageo::Pos;

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

void CalcTest::testNextSixHour_data()
{
  QTest::addColumn<QDateTime>("datetime");
  QTest::addColumn<QDateTime>("result");

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(0, 0), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), Qt::UTC);

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(1, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(6, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(7, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(11, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(12, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(17, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(19, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(23, 59), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), Qt::UTC);

}

void CalcTest::testNextSixHour()
{
  QFETCH(QDateTime, datetime);
  QFETCH(QDateTime, result);

  QDateTime local = atools::timeToNextHourInterval(datetime, 6);

  qDebug() << datetime << local;
  QCOMPARE(local, result);
}

void CalcTest::testLastSixHour_data()
{
  QTest::addColumn<QDateTime>("datetime");
  QTest::addColumn<QDateTime>("result");

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(0, 0), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(0, 0), Qt::UTC);

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(1, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(0, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(6, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(7, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(11, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(12, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(17, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(19, 30), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(23, 59), Qt::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), Qt::UTC);

}

void CalcTest::testLastSixHour()
{
  QFETCH(QDateTime, datetime);
  QFETCH(QDateTime, result);

  QDateTime local = atools::timeToLastHourInterval(datetime, 6);

  qDebug() << datetime << local;
  QCOMPARE(local, result);
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

  QCOMPARE(ageo::pressureMbarForAltMeter(altMeter), pressureMbar);
  QCOMPARE(ageo::altMeterForPressureMbar(pressureMbar), altMeter);
}

void CalcTest::testDensityAltitude_data()
{
  QTest::addColumn<float>("temperatureC");
  QTest::addColumn<float>("seaLevelPressureMbar");
  QTest::addColumn<float>("altitudeFt");
  QTest::addColumn<float>("densityAltResult");
  QTest::addColumn<float>("pressureAltResult");

  QTest::newRow("15 << 1013.25 << 0") << 15.f << 1013.25f << 0.f << 0.f << 0.f;
  QTest::newRow("30 << 1013.25 << 0") << 30.f << 1013.25f << 0.f << 1723.93f << 0.f;
  QTest::newRow("0  << 1013.25 << 0") << 0.f << 1013.25f << 0.f << -1838.49f << 0.f;

  QTest::newRow("15 << 1013.25 << 1000") << 15.f << 1013.25f << 1000.f << 1233.97f << 1000.f;
  QTest::newRow("30 << 1013.25 << 1000") << 30.f << 1013.25f << 1000.f << 2943.28f << 1000.f;
  QTest::newRow("0  << 1013.25 << 1000") << 0.f << 1013.25f << 1000.f << -588.919f << 1000.f;

  QTest::newRow("15 << 980 << 1000") << 15.f << 980.f << 1000.f << 2367.45f << 1920.f;
  QTest::newRow("30 << 980 << 1000") << 30.f << 980.f << 1000.f << 4063.32f << 1920.f;
  QTest::newRow("0  << 980 << 1000") << 0.f << 980.f << 1000.f << 558.89f << 1920.f;

  QTest::newRow("15 << 1030 << 1000") << 15.f << 1030.f << 1000.f << 673.71f << 545.769f;
  QTest::newRow("30 << 1030 << 1000") << 30.f << 1030.f << 1000.f << 2389.66f << 545.769f;
  QTest::newRow("0  << 1030 << 1000") << 0.f << 1030.f << 1000.f << -1156.26f << 545.769f;

}

void CalcTest::testDensityAltitude()
{
  QFETCH(float, temperatureC);
  QFETCH(float, seaLevelPressureMbar);
  QFETCH(float, altitudeFt);
  QFETCH(float, densityAltResult);
  QFETCH(float, pressureAltResult);

  float pressureAltitude = ageo::pressureAltitudeFt(altitudeFt, seaLevelPressureMbar);
  QCOMPARE(pressureAltitude, pressureAltResult);

  float densityAltitude = ageo::densityAltitudeFt(temperatureC, pressureAltitude);
  QCOMPARE(densityAltitude, densityAltResult);

  densityAltitude = ageo::densityAltitudeFt(temperatureC, altitudeFt, seaLevelPressureMbar);
  QCOMPARE(densityAltitude, densityAltResult);
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
  QTime time = ageo::calculateSunriseSunset(neverRises, neverSets, pos, date, zenith);
  // qDebug() << pos << date << zenith;
  // qDebug() << time << QDateTime(date, time).toLocalTime();
  QCOMPARE(time, result);
  QCOMPARE(neverrise, neverRises);
  QCOMPARE(neverset, neverSets);
}

void CalcTest::testCorrectDateLocal()
{
  for(int i = 0; i < 24; i++)
    correctDateLocalRun(6, i, i, 0.f, 0);

  correctDateLocalRun(6, 3, 0, 170.f, 3);
  correctDateLocalRun(6, 6, 0, 170.f, 6);
  correctDateLocalRun(6, 9, 0, 170.f, 9);
  correctDateLocalRun(6, 12, 0, 170.f, 12);
  correctDateLocalRun(6, 15, 0, 170.f, 9);
  correctDateLocalRun(6, 18, 0, 170.f, 6);

  correctDateLocalRun(6, 3, 0, -170.f, -3);
  correctDateLocalRun(6, 6, 0, -170.f, -6);
  correctDateLocalRun(6, 9, 0, -170.f, -9);
  correctDateLocalRun(6, 12, 0, -170.f, -12);
  correctDateLocalRun(6, 6, 3, -170.f, -3);
  correctDateLocalRun(6, 3, 6, 170.f, 3);
}

void CalcTest::correctDateLocalRun(int localDateDays, int localTimeHour, int utcTimeHour, float longitudeX, int expectedOffsetHours)
{
  int step = 2;

  qDebug() << "GMT" << expectedOffsetHours << "==========================================================";
  for(int i = 0; i < 24 / step; i++)
  {
    QDateTime localDateTime = atools::correctDateLocal(localDateDays, localTimeHour * 3600, utcTimeHour * 3600, longitudeX);
    QDateTime zuluDateTime = localDateTime.toUTC();

    qDebug() << "local" << localDateTime.toString(Qt::ISODateWithMs) << "zulu" << zuluDateTime.toString(Qt::ISODateWithMs)
             << "localDateDays" << localDateDays << "localTimeHour" << localTimeHour << "utcTimeHour" << utcTimeHour;

    QCOMPARE(localDateTime.offsetFromUtc() / 3600, expectedOffsetHours);
    QCOMPARE(localDateTime.time().msecsSinceStartOfDay() / 1000 / 3600, localTimeHour);
    QCOMPARE(localDateTime.date().dayOfYear(), localDateDays);

    localTimeHour += step;
    utcTimeHour += step;

    if(localTimeHour >= 24)
    {
      localTimeHour -= 24;
      localDateDays++;
    }
    if(utcTimeHour >= 24)
      utcTimeHour -= 24;
  }
}
