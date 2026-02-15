/*****************************************************************************
* Copyright 2015-2025 Alexander Barthel alex@littlenavmap.org
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

#ifndef ATOOLSTEST_TIMETEST_H
#define ATOOLSTEST_TIMETEST_H

#include <QString>
#include <QtTest>

namespace atools {
namespace timezone {
class TimeZoneManager;
}
}
class TimeTest :
  public QObject
{
  Q_OBJECT

public:
  TimeTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testTimeZone_data();
  void testTimeZone();

  void testLastSixHour_data();
  void testLastSixHour();
  void testNextSixHour_data();
  void testNextSixHour();

  void testCorrectDateLocal_data();
  void testCorrectDateLocal();

  void testSunsetSunrise_data();
  void testSunsetSunrise();

  void testCorrectDate_data();
  void testCorrectDate();

private:
  atools::timezone::TimeZoneManager *timezone;
};

#endif // ATOOLSTEST_TIMETEST_H
