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

#include "timetest.h"
#include "geo/calculations.h"

#include "geo/pos.h"
#include "timezone/timezonemanager.h"

namespace ageo = atools::geo;
using ageo::Pos;

TimeTest::TimeTest()
{

}

void TimeTest::runtest(int argc, char *argv[])
{
  TimeTest tst;
  QTest::qExec(&tst, argc, argv);
}

void TimeTest::initTestCase()
{
  timezone = new atools::timezone::TimeZoneManager(true);
  timezone->readFile("testdata/timezone/timezone21");
}

void TimeTest::cleanupTestCase()
{
  ATOOLS_DELETE_LOG(timezone);
}

void TimeTest::testTimeZonePath()
{
  atools::timezone::TimeZoneManager manager(true);
  timezone->readFile("testdata/timezone äöüß/timezone21");
}

void TimeTest::testTimeZone_data()
{
  QTest::addColumn<float>("lonX");
  QTest::addColumn<float>("latY");
  QTest::addColumn<QTimeZone>("result");

  QTest::newRow("GMT-5") << 74.05035f << -33.87041f << QTimeZone("Etc/GMT-5");
  QTest::newRow("GMT") << 4.61675f << -37.30027f << QTimeZone("Etc/GMT");
  QTest::newRow("GMT+3") << -45.4809f << 33.87041f << QTimeZone("Etc/GMT+3");
  QTest::newRow("GMT+7") << -110.87153f << 5.61599f << QTimeZone("Etc/GMT+7");

  // EDDF
  // 8.54692 50.02561
  QTest::newRow("EDDF") << 8.54692f << 50.02561f << QTimeZone("Europe/Berlin");
  // LPPD
  // -25.69855 37.74221
  QTest::newRow("LPPD") << -25.69855f << 37.74221f << QTimeZone("Atlantic/Azores");

  // NZAA
  // 174.75835 -36.985
  QTest::newRow("NZAA") << 174.75835f << -36.985f << QTimeZone("Pacific/Auckland");

  // NFFN
  // 177.44341 -17.75637
  QTest::newRow("NFFN") << 177.44341f << -17.75637f << QTimeZone("Pacific/Fiji");

  // PLCH
  // Kiritimati (+14 h)
  QTest::newRow("PLCH") << -157.37115f << 1.96617f << QTimeZone("Pacific/Kiritimati");

  // PHNL
  // -157.92244 21.31944
  // 12:00 local
  // 22:00 UTC
  QTest::newRow("PHNL") << -157.92244f << 21.31944f << QTimeZone("Pacific/Honolulu");

  // LEZL
  // -5.9276 37.39962
  QTest::newRow("LEZL") << -5.9276f << 37.39962f << QTimeZone("Europe/Madrid");

  // PMDY
  // -177.38063 28.20519
  // Honolulu (-10 h)
  QTest::newRow("PMDY") << -177.38063f << 28.20519f << QTimeZone("Pacific/Midway");

  // EGLC
  // 0.05317 51.50404
  QTest::newRow("EGLC") << 0.05317f << 51.50404f << QTimeZone("Europe/London");

  // EGFF
  // -3.34244 51.3961
  QTest::newRow("EGFF") << -3.34244f << 51.3961f << QTimeZone("Europe/London");

}

void TimeTest::testTimeZone()
{
  QFETCH(float, lonX);
  QFETCH(float, latY);
  QFETCH(QTimeZone, result);

  QTimeZone zone = timezone->getTimezone(lonX, latY);

  qDebug() << "Expected" << result << "zone" << zone << zone.standardTimeOffset(QDateTime()) / 3600.f;
  QCOMPARE(result, zone);
}

void TimeTest::testNextSixHour_data()
{
  QTest::addColumn<QDateTime>("datetime");
  QTest::addColumn<QDateTime>("result");

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(0, 0), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), QTimeZone::UTC);

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(1, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(6, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(7, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(11, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(12, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(17, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(19, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(23, 59), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 24), QTime(0, 0), QTimeZone::UTC);

}

void TimeTest::testNextSixHour()
{
  QFETCH(QDateTime, datetime);
  QFETCH(QDateTime, result);

  QDateTime local = atools::timeToNextHourInterval(datetime, 6);

  qDebug() << datetime << local;
  QCOMPARE(local, result);
}

void TimeTest::testLastSixHour_data()
{
  QTest::addColumn<QDateTime>("datetime");
  QTest::addColumn<QDateTime>("result");

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(0, 0), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(0, 0), QTimeZone::UTC);

  QTest::newRow("0:0") << QDateTime(QDate(2020, 12, 23), QTime(1, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(0, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(6, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(7, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(11, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(6, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(12, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(17, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(12, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(19, 30), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC);

  QTest::newRow("1:0") << QDateTime(QDate(2020, 12, 23), QTime(23, 59), QTimeZone::UTC)
                       << QDateTime(QDate(2020, 12, 23), QTime(18, 0), QTimeZone::UTC);

}

void TimeTest::testLastSixHour()
{
  QFETCH(QDateTime, datetime);
  QFETCH(QDateTime, result);

  QDateTime local = atools::timeToLastHourInterval(datetime, 6);

  qDebug() << datetime << local;
  QCOMPARE(local, result);
}

void TimeTest::testCorrectDate_data()
{
  QTest::addColumn<int>("day");
  QTest::addColumn<int>("hour");
  QTest::addColumn<int>("minute");
  QTest::addColumn<QDateTime>("reference");
  QTest::addColumn<QDateTime>("result");

  QTest::newRow("151000") << 15 << 10 << 00 << QDateTime(QDate(2026, 2, 15), QTime(12, 0)) << QDateTime(QDate(2026, 2, 15), QTime(10, 0));
  QTest::newRow("151200") << 15 << 12 << 00 << QDateTime(QDate(2026, 2, 15), QTime(12, 0)) << QDateTime(QDate(2026, 2, 15), QTime(12, 0));
  QTest::newRow("151400") << 15 << 14 << 00 << QDateTime(QDate(2026, 2, 15), QTime(12, 0)) << QDateTime(QDate(2026, 1, 15), QTime(14, 0));

  QTest::newRow("311200") << 31 << 12 << 00 << QDateTime(QDate(2026, 12, 30), QTime(12, 0)) << QDateTime(QDate(2026, 10, 31), QTime(12, 0));
  QTest::newRow("301200") << 30 << 12 << 00 << QDateTime(QDate(2026, 12, 30), QTime(12, 0)) << QDateTime(QDate(2026, 12, 30), QTime(12, 0));
  QTest::newRow("291200") << 29 << 12 << 00 << QDateTime(QDate(2026, 12, 30), QTime(12, 0)) << QDateTime(QDate(2026, 12, 29), QTime(12, 0));

  QTest::newRow("301100") << 30 << 11 << 00 << QDateTime(QDate(2026, 1, 1), QTime(11, 0)) << QDateTime(QDate(2025, 12, 30), QTime(11, 0));
  QTest::newRow("152000") << 15 << 20 << 00 << QDateTime(QDate(2026, 1, 1), QTime(20, 0)) << QDateTime(QDate(2025, 12, 15), QTime(20, 0));

  QTest::newRow("291230") << 29 << 12 << 30 << QDateTime(QDate(2026, 2, 2), QTime(22, 0)) << QDateTime(QDate(2026, 1, 29), QTime(12, 30));
  QTest::newRow("282000") << 28 << 20 << 00 << QDateTime(QDate(2026, 2, 2), QTime(20, 0)) << QDateTime(QDate(2026, 1, 28), QTime(20, 0));
  QTest::newRow("292000") << 29 << 20 << 00 << QDateTime(QDate(2026, 2, 2), QTime(20, 0)) << QDateTime(QDate(2026, 1, 29), QTime(20, 0));
  QTest::newRow("312100") << 31 << 21 << 00 << QDateTime(QDate(2026, 2, 28), QTime(21, 0)) << QDateTime(QDate(2026, 1, 31), QTime(21, 0));
  QTest::newRow("312000") << 31 << 20 << 00 << QDateTime(QDate(2026, 4, 2), QTime(20, 0)) << QDateTime(QDate(2026, 3, 31), QTime(20, 0));
  QTest::newRow("010000") << 1 << 0 << 00 << QDateTime(QDate(2026, 1, 1), QTime(0, 0)) << QDateTime(QDate(2026, 1, 1), QTime(0, 0));
}

void TimeTest::testCorrectDate()
{
  QFETCH(int, day);
  QFETCH(int, hour);
  QFETCH(int, minute);
  QFETCH(QDateTime, reference);
  QFETCH(QDateTime, result);

  QCOMPARE(atools::correctDate(day, hour, minute, reference), result);
}

void TimeTest::testSunsetSunrise_data()
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

void TimeTest::testSunsetSunrise()
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

void TimeTest::testCorrectDateLocal_data()
{
  // EDDF
  // 8.54692 50.02561
  // 12:00 local
  // 11:00 UTC

  // LPPD
  // -25.69855 37.74221
  // 12:00 local
  // 13:00 UTC

  // NZAA
  // 174.75835 -36.985
  // 11:00 local
  // 01:00 UTC

  // NFFN
  // 177.44341 -17.75637
  // 10:00 local
  // 22:00 UTC

  // PLCH
  // Kiritimati (+14 h)
  // -157.37115 1.96617
  // 15:00 local
  // 01:00 UTC
  // 01:00 local
  // 11:00 UTC
  // 14:00 local
  // 00:00 UTC

  // PHNL
  // -157.92244 21.31944
  // 12:00 local
  // 22:00 UTC

  // LEZL
  // -5.9276 37.39962
  // sim/time/local_date_days = 154 // 04.06.
  // sim/time/local_time_sec = 43450
  // sim/time/zulu_time_sec = 39890
  // 12:00 local
  // 11:00 UTC
  // Simulator Date and Time: 04.06.26 13:00 UTC
  // Simulator Local Time: 12:00 UTC-01:00[04.06.26 12:00]

  // PMDY
  // -177.38063 28.20519
  // Honolulu (-10 h)
  // 12:00 local
  // 23:00 UTC
  // 20:00 local
  // 07:00 UTC

  // EGLC
  // 0.05317 51.50404
  // 12:00 local
  // 12:00 UTC

  // EGFF
  // -3.34244 51.3961
  // 12:00 local
  // 12:00 UTC

  // YPPH
  // 115.91814 -31.95216
  // 18:00 local
  // 10:00 UTC
  // 05:00 local
  // 21:00 UTC

  // KORD
  // -87.90797 41.97723
  // 05:00 local
  // 11:00 UTC
  // 19:00 local
  // 01:00 UTC

  QTest::addColumn<int>("dayOfYearLocal"); // Date in days since January 1st at the user's location
  QTest::addColumn<float>("hoursOfDayLocal"); // Local time (seconds since midnight) at the user's location
  QTest::addColumn<float>("hoursOfDayUtc"); // Zulu time (seconds since midnight)
  QTest::addColumn<float>("lonX");
  QTest::addColumn<float>("latY");
  QTest::addColumn<QString>("resultLocal");
  QTest::addColumn<QString>("resultUtc");

  QTest::newRow("KORD") << 154 << 05.f << 11.f << -87.90797f << 41.97723f
                        << "2026-06-04T05:00:00-06:00" << "2026-06-04T11:00:00Z";
  QTest::newRow("KORD 2") << 154 << 19.f << 1.f << -87.90797f << 41.97723f
                          << "2026-06-04T19:00:00-06:00" << "2026-06-05T01:00:00Z";

  QTest::newRow("YPPH") << 154 << 18.f << 10.f << 115.9f << -31.9f
                        << "2026-06-04T18:00:00+08:00" << "2026-06-04T10:00:00Z";
  QTest::newRow("YPPH 2") << 154 << 05.f << 21.f << 115.9f << -31.9f
                          << "2026-06-04T05:00:00+08:00" << "2026-06-03T21:00:00Z";

  QTest::newRow("EGLC") << 154 << 12.f << 12.f << 0.05317f << 51.50404f
                        << "2026-06-04T12:00:00+00:00" << "2026-06-04T12:00:00Z";
  QTest::newRow("EGLC Midnight") << 154 << 0.f << 0.f << 0.05317f << 51.50404f
                                 << "2026-06-04T00:00:00+00:00" << "2026-06-04T00:00:00Z";
  QTest::newRow("EGFF") << 154 << 12.f << 12.f << -3.34244f << 51.3961f
                        << "2026-06-04T12:00:00+00:00" << "2026-06-04T12:00:00Z";
  QTest::newRow("EGFF Midnight") << 154 << 0.f << 0.f << -3.34244f << 51.3961f
                                 << "2026-06-04T00:00:00+00:00" << "2026-06-04T00:00:00Z";
  QTest::newRow("PMDY") << 154 << 12.f << 23.f << -177.38063f << 28.20519f
                        << "2026-06-04T12:00:00-11:00" << "2026-06-04T23:00:00Z";
  QTest::newRow("PMDY day") << 154 << 20.f << 07.f << -177.38063f << 28.20519f
                            << "2026-06-04T20:00:00-11:00" << "2026-06-05T07:00:00Z";
  QTest::newRow("PHNL") << 154 << 12.f << 22.f << -157.f << 21.3f
                        << "2026-06-04T12:00:00-10:00" << "2026-06-04T22:00:00Z";
  QTest::newRow("PHNL day") << 154 << 22.f << 8.f << -157.f << 21.3f
                            << "2026-06-04T22:00:00-10:00" << "2026-06-05T08:00:00Z";
  QTest::newRow("LPPD") << 154 << 12.f << 13.f << -25.6f << 37.7f
                        << "2026-06-04T12:00:00-01:00" << "2026-06-04T13:00:00Z";
  QTest::newRow("EDDF") << 154 << 13.f << 12.f << 8.5f << 50.f
                        << "2026-06-04T13:00:00+01:00" << "2026-06-04T12:00:00Z";
  QTest::newRow("LEZL") << 154 << 12.f << 11.f << -5.9f << 37.39f
                        << "2026-06-04T12:00:00+01:00" << "2026-06-04T11:00:00Z";
  QTest::newRow("NZAA day") << 154 << 11.f << 1.f << 174.7f << -36.9f
                            << "2026-06-04T11:00:00+12:00" << "2026-06-03T23:00:00Z";  // ***
  QTest::newRow("NFFN") << 154 << 10.f << 22.f << 177.4f << -17.7f
                        << "2026-06-04T10:00:00+12:00" << "2026-06-03T22:00:00Z";

  // QTest::newRow("PLCH 15:00") << 154 << 15.f << 1.f << -157.f << 1.96617f  << "2026-06-04T15:00:00-10:00" << "2026-06-05T01:00:00Z";
  // QTest::newRow("PLCH 01:00") << 154 << 1.f << 11.f << -157.f << 1.96617f  << "2026-06-04T01:00:00-10:00" << "2026-06-04T11:00:00Z";
  // QTest::newRow("PLCH 14:00") << 154 << 14.f << 0.f << -157.f << 1.96617f  << "2026-06-04T14:00:00-10:00" << "2026-06-05T00:00:00Z";
}

void TimeTest::testCorrectDateLocal()
{
  QFETCH(int, dayOfYearLocal);
  QFETCH(float, hoursOfDayLocal);
  QFETCH(float, hoursOfDayUtc);
  QFETCH(float, lonX);
  QFETCH(float, latY);
  QFETCH(QString, resultLocal);
  QFETCH(QString, resultUtc);

  QDateTime local, utc;
  timezone->correctDateLocal(local, utc, dayOfYearLocal + 1, hoursOfDayLocal * 3600.f, hoursOfDayUtc * 3600.f, lonX, latY);

  qDebug() << "dayOfYearLocal" << dayOfYearLocal << "hoursOfDayLocal" << hoursOfDayLocal << "hoursOfDayUtc" << hoursOfDayUtc;
  qDebug() << "local" << local.toString(Qt::ISODate);
  qDebug() << "utc" << utc.toString(Qt::ISODate);
  qDebug() << "resultLocal" << resultLocal;
  qDebug() << "resultUtc" << resultUtc;

  QCOMPARE(local.toString(Qt::ISODate), resultLocal);
  QCOMPARE(utc.toString(Qt::ISODate), resultUtc);
}
