/*****************************************************************************
* Copyright 2015-2024 Alexander Barthel alex@littlenavmap.org
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

#ifndef ATOOLSTEST_FPTEST_H
#define ATOOLSTEST_FPTEST_H

#include "fs/gpx/gpxio.h"
#include "fs/pln/flightplan.h"
#include "fs/pln/flightplanio.h"

#include <QString>
#include <QtTest>

class FlightplanTest :
  public QObject
{
  Q_OBJECT

public:
  FlightplanTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

#ifdef FPR_TEST

  void testReadFpr();

#endif

  void testEncoding();
  void testCompress();

  void testSaveFprDirect();
  void testSaveFprAirway();

  void testLoadFs9();
  void testLoadFsc();
  void testLoadFms();
  void testLoadFms2();

  void testSaveFlpDirect();
  void testSaveFlpAirway();
  void testSaveFlpMixed();

  void testSaveFlpCrjDirect();
  void testSaveFlpCrjAirway();
  void testSaveFlpCrjMixed();

  void testSaveFltplanDirect();
  void testSaveFltplanAirway();
  void testSaveFltplanMixed();

  void testSavePln();
  void testSaveRte();
  void testSaveFlp();
  void testSaveFms3();
  void testSaveFms11();
  void testSaveGpx();

  void testSaveLnm();
  void testSaveLnmGz();
  void testReadLnmBroken();
  void testReadLnmEmpty();

  void testSaveGarminFpl();
  void testLoadGarminFpl();

  void testLoadFsxPln();
  void testLoadMsfsPln();
  void testLoadMsfs24Pln();

  void testSaveFlightGearDirect();
  void testSaveFlightGearAirway();
  void testSaveFlightGearMixed();
  void testSaveFlightGearSidStarTrans();

  void testLoadFlightGearRunway();
  void testLoadFlightGearSidStarTrans();
  void testLoadFlightGear();

  void testDetectFormat();

  void testSaveLeveld();
  void testSaveLeveld2();
  void testSaveFeelthere();
  void testSaveFeelthere2();

  void testSaveEfbr();
  void testSaveEfbr2();

  void testSaveQwRte();

  void testSaveMaddogMdr();

  void testLoadGfp();

  void testSaveCiva();

private:
  atools::fs::pln::Flightplan flightplan, flightplanUser, flightplanAll;
  atools::fs::pln::FlightplanIO io;
  atools::fs::gpx::GpxIO gpxio;
};

#endif // ATOOLSTEST_FPTEST_H
