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

#ifndef ATOOLSTEST_FSUTILTEST_H
#define ATOOLSTEST_FSUTILTEST_H

#include <QtTest>

class FsUtilTest :
  public QObject
{
  Q_OBJECT

public:
  FsUtilTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testRunwaySplit();
  void testRunways_data();
  void testRunways();
  void testRunwaysEqual_data();
  void testRunwaysEqual();

  void testXpWaypointFlags();

};

#endif // ATOOLSTEST_FSUTILTEST_H
