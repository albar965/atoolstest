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

#include "perftest.h"
#include "fs/pln/flightplan.h"
#include "zip/gzip.h"

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
  perf.setDescription("Slow climber <XML>\"\nLineFEED[]äöüß\\");
  perf.setName("Boeing 737-200");
  perf.setAircraftType("B732");

  perf.setFuelAsVolume(false);
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

void PerfTest::testPerfSaveLoadIni()
{
  perf.saveIni("aircraft_performance_ini.lnmperf");

  AircraftPerf loadedPerf;
  loadedPerf.load("aircraft_performance_ini.lnmperf");

  QCOMPARE(loadedPerf, perf);
}

void PerfTest::testPerfSaveLoadXml()
{
  perf.saveXml("aircraft_performance_xml.lnmperf");

  AircraftPerf loadedPerf;
  loadedPerf.load("aircraft_performance_xml.lnmperf");

  QCOMPARE(loadedPerf, perf);
}

void PerfTest::testPerfSaveLoadXmlGz()
{
  QByteArray bytes = perf.saveXmlGz();

  QVERIFY(atools::zip::isGzipCompressed(bytes));

  AircraftPerf loadedPerf;
  loadedPerf.loadXmlGz(bytes);

  QCOMPARE(loadedPerf, perf);
}

void PerfTest::testPerfSaveLoad_data()
{
  QTest::addColumn<QString>("filename");

  QTest::newRow("Laminar Research King Air C90.lnmperf") << ":/test/resources/Laminar Research King Air C90.lnmperf";
  QTest::newRow("ToLiss A319.lnmperf") << ":/test/resources/ToLiss A319.lnmperf";
  QTest::newRow("Just Flight PA28 Arrow.lnmperf") << ":/test/resources/Just Flight PA28 Arrow.lnmperf";
}

void PerfTest::testPerfSaveLoad()
{
  QFETCH(QString, filename);

  AircraftPerf loadedPerf;
  loadedPerf.load(filename);
  loadedPerf.saveXml(QFileInfo(filename).fileName() + "_xml");

  AircraftPerf loadedPerfXml;
  loadedPerfXml.load(QFileInfo(filename).fileName() + "_xml");

  QCOMPARE(loadedPerf, loadedPerfXml);
}
