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

#ifndef ATOOLSTEST_PERFTEST_H
#define ATOOLSTEST_PERFTEST_H

#include "fs/perf/aircraftperf.h"
#include "fs/pln/flightplanio.h"

#include <QString>
#include <QtTest>

class PerfTest :
  public QObject
{
  Q_OBJECT

public:
  PerfTest();

  static void runtest(int argc, char *argv[]);

private:
  atools::fs::perf::AircraftPerf perf;
  atools::fs::pln::FlightplanIO io;

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testPerfSaveLoadIni();
  void testPerfSaveLoadXml();

  void testPerfSaveLoad_data();
  void testPerfSaveLoad();

  void testPerfSaveLoadXmlGz();

};

#endif // ATOOLSTEST_PERFTEST_H
