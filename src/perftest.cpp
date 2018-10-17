/*****************************************************************************
* Copyright 2015-2018 Alexander Barthel albar965@mailbox.org
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

#include "perftest.h"
#include "fs/pln/flightplan.h"

#include "atools.h"

#include <geo/linestring.h>

using atools::fs::perf::AircraftPerf;

PerfTest::PerfTest()
{

}

void PerfTest::runtest(int argc, char *argv[])
{
  PerfTest tst;
  QTest::qExec(&tst, argc, argv);
}

void PerfTest::initTestCase()
{
  perf.setDescription("Slow climber");
  perf.setName("Boeing 737-200");
  perf.setAircraftType("B732");

  perf.setFuelUnit(atools::fs::perf::WEIGHT);
  perf.setTaxiFuel(1000);
  perf.setReserveFuel(6000);
  perf.setExtraFuel(2000);

  perf.setClimbVertSpeed(2000);
  perf.setClimbSpeed(250);
  perf.setClimbFuelFlow(800);

  perf.setCruiseSpeed(450);
  perf.setCruiseFuelFlow(400);
  perf.setContingencyFuel(10);

  perf.setDescentSpeed(300);
  perf.setDescentVertSpeed(3000);
  perf.setDescentFuelFlow(100);
}

void PerfTest::cleanupTestCase()
{

}

void PerfTest::testPerfSaveLoad()
{
  perf.save("aircraft_performance.lnmperf");

  perf = AircraftPerf();
  perf.load("aircraft_performance.lnmperf");

  QCOMPARE(perf.getDescription(), QString("Slow climber"));
  QCOMPARE(perf.getName(), QString("Boeing 737-200"));
  QCOMPARE(perf.getAircraftType(), QString("B732"));

  QCOMPARE(perf.getFuelUnit(), atools::fs::perf::WEIGHT);
  QCOMPARE(perf.getTaxiFuel(), 1000);
  QCOMPARE(perf.getReserveFuel(), 6000);
  QCOMPARE(perf.getExtraFuel(), 2000);

  QCOMPARE(perf.getClimbVertSpeed(), 2000);
  QCOMPARE(perf.getClimbSpeed(), 250);
  QCOMPARE(perf.getClimbFuelFlow(), 800);

  QCOMPARE(perf.getCruiseSpeed(), 450);
  QCOMPARE(perf.getCruiseFuelFlow(), 400);
  QCOMPARE(perf.getContingencyFuel(), 10);

  QCOMPARE(perf.getDescentSpeed(), 300);
  QCOMPARE(perf.getDescentVertSpeed(), 3000);
  QCOMPARE(perf.getDescentFuelFlow(), 100);
}
