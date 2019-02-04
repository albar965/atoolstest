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

#ifndef ATOOLSTEST_FPTEST_H
#define ATOOLSTEST_FPTEST_H

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

private slots:
  void testSaveFprDirect();
  void testSaveFprAirway();

  void testLoadFs9();
  void testLoadFsc();
  void testLoadFms();
  void testLoadFms2();

  void testSaveFlpDirect();
  void testSaveFlpAirway();
  void testSaveFlpMixed();

  void testSaveFltplanDirect();
  void testSaveFltplanAirway();
  void testSaveFltplanMixed();

  void testSave();
  void testSaveClean();
  void testSaveRte();
  void testSaveFlp();
  void testSaveFms();
  void testSaveGpx();

  void testSaveGarminGns();

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

  void testSaveMaddogMdx();

private:
  atools::fs::pln::Flightplan flightplan, flightplanUser;
  atools::fs::pln::FlightplanIO io;
};

#endif // ATOOLSTEST_FPTEST_H
