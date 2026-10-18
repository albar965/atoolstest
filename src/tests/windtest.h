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

#ifndef ATOOLSTEST_WINDTEST_H
#define ATOOLSTEST_WINDTEST_H

#include "grib/gribcommon.h"

namespace atools {
namespace grib {
class WindQuery;
}
}

#include <QString>
#include <QtTest>

class WindTest :
  public QObject
{
  Q_OBJECT

public:
  WindTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testLoadSpecialCharPath();
  void testLoadXp12();

  void testGribReadTest_data();
  void testGribReadTest();
  void testGribReadInterpolate_data();
  void testGribReadInterpolate();

  void testGribReadNoGrib();
  void testGribReadNoFile();
  void testGribReadNoData();
  void testGribReadInvalidFile();

  void testGribWindLineStringQuery_data();
  void testGribWindLineStringQuery();

  void testGribDownload();
  void testGribDownloadFail();
  void testGribLoadLnm();
  void testGribLoadWinds();

  void testGribWindRectQuery_data();
  void testGribWindRectQuery();

private:
  void lnmDatasetTest(const atools::grib::GribDatasetList& datasets, const QDateTime& datetime);

  bool verbose = false;

  atools::grib::WindQuery *queryLnm = nullptr, *queryXp11 = nullptr, *queryXp12 = nullptr;

};

#endif // ATOOLSTEST_WINDTEST_H
