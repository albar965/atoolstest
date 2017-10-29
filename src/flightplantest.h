/*****************************************************************************
* Copyright 2015-2017 Alexander Barthel albar965@mailbox.org
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

#include <QString>
#include <QtTest>

class FlightplanTest :
  public QObject
{
  Q_OBJECT

public:
  FlightplanTest();

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testLoadFs9();
  void testLoadFms();
  void testLoadFms2();

  void testSaveFlpDirect();
  void testSaveFlpAirway();
  void testSaveFlpMixed();

  void testSave();
  void testSaveClean();
  void testSaveRte();
  void testSaveFlp();
  void testSaveFms();
  void testSaveGpx();

private:
  atools::fs::pln::Flightplan flightplan;
};

#endif // ATOOLSTEST_FPTEST_H
