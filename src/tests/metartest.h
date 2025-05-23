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

#ifndef ATOOLSTEST_METARTEST_H
#define ATOOLSTEST_METARTEST_H

#include <QString>
#include <QtTest>

namespace atools {

namespace fs {
namespace weather {

class Metar;
class WeatherDownloadBase;
}
}
}

class MetarTest :
  public QObject
{
  Q_OBJECT

public:
  MetarTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testVatsimDownload();
  void testVatsimDownloadFailed();

  void testIvaoRead();
  void testIvaoDownload();
  void testIvaoDownloadFailed();
  void testNoaaDownload();
  void testNoaaDownloadFailed();
  void testNoaaDownloadFailedNoHost();

  void testMetarAsn();
  void testMetarSim();
  void testMetarInterpolatedSimple();

  void testMetarInterpolated();

private:
  void testDownload(atools::fs::weather::WeatherDownloadBase& downloader, bool expectFail);
  void validateMetar(const atools::fs::weather::Metar& m,
                     const QString& stationFlightRules, const QString& stationMetar,
                     const QString& nearestFlightRules, const QString& nearestMetar,
                     const QString& interpolatedFlightRules, const QString& interpolatedMetar);

};

#endif // ATOOLSTEST_METARTEST_H
