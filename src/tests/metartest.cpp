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

#include "metartest.h"

#include "fs/weather/metar.h"
#include "fs/weather/metarparser.h"
#include "fs/weather/noaaweatherdownloader.h"
#include "geo/pos.h"
#include "fs/weather/weathertypes.h"

using atools::fs::weather::NoaaWeatherDownloader;

MetarTest::MetarTest()
{

}

void MetarTest::runtest(int argc, char *argv[])
{
  MetarTest tst;
  QTest::qExec(&tst, argc, argv);
}

void MetarTest::initTestCase()
{
  atools::fs::weather::initTranslateableTexts();
}

void MetarTest::cleanupTestCase()
{

}

void MetarTest::testNoaaDownload()
{
  // select '{"'||ident||'"'|| ', atools::geo::Pos('||lonx ||',' || laty || ')}' from airport where ident like 'EDD%';
  QHash<QString, atools::geo::Pos> airports = {
    {"EDDM", atools::geo::Pos(11.7894306182861, 48.3521003723145)},
    {"EDDF", atools::geo::Pos(8.57045555114746, 50.0333061218262)},
    {"EDDK", atools::geo::Pos(7.14274454116821, 50.8659172058105)},
    {"EDDH", atools::geo::Pos(9.98822784423828, 53.6303901672363)},
    {"EDDL", atools::geo::Pos(6.75731134414673, 51.2809257507324)},
    {"EDDN", atools::geo::Pos(11.0780086517334, 49.4986991882324)},
    {"EDDB", atools::geo::Pos(13.5006723403931, 52.362247467041)},
    {"EDDC", atools::geo::Pos(13.7679996490479, 51.1343460083008)},
    {"EDDT", atools::geo::Pos(13.2877111434937, 52.559684753418)},
    {"EDDV", atools::geo::Pos(9.68352222442627, 52.4602127075195)},
    {"EDDW", atools::geo::Pos(8.78674697875977, 53.0474014282227)},
    {"EDDS", atools::geo::Pos(9.22196388244629, 48.6898765563965)},
    {"EDDR", atools::geo::Pos(7.1095085144043, 49.2145538330078)},
    {"EDDE", atools::geo::Pos(10.9581060409546, 50.9798126220703)},
    {"EDDG", atools::geo::Pos(7.68483066558838, 52.1346435546875)},
    {"EDDP", atools::geo::Pos(12.2363834381104, 51.4239921569824)},
    {"EDDI", atools::geo::Pos(13.4031963348389, 52.4747467041016)}
  };

  NoaaWeatherDownloader downloader(this, 10000, true);

  bool found = false;
  connect(&downloader, &NoaaWeatherDownloader::weatherUpdated, [&found]() -> void
  {
    found = true;
  });

  downloader.setFetchAirportCoords([&airports](const QString& airportIdent) -> atools::geo::Pos
  {
    return airports.value(airportIdent);
  });

  downloader.setRequestUrl("https://tgftp.nws.noaa.gov/data/observations/metar/cycles/%1Z.TXT");

  atools::fs::weather::MetarResult metar = downloader.getMetar("EDDF", airports.value("EDDF"));

  QCOMPARE(metar.isEmpty(), true);

  int i = 0;
  while(downloader.isDownloading() && i++ < 60)
  {
    QApplication::processEvents();
    QThread::sleep(1);
  }

  metar = downloader.getMetar("EDDF", airports.value("EDDF"));
  QCOMPARE(metar.isEmpty(), false);

  metar = downloader.getMetar("EDDK", airports.value("EDDK"));
  QCOMPARE(metar.isEmpty(), false);
}

void MetarTest::testMetarAsn()
{
  QFile metarFile(":/test/resources/current_wx_snapshot.txt");
  QVERIFY(metarFile.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFile);

  int numFailed = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    QStringList list = line.split("::");
    QVERIFY(list.size() >= 2);

    if(!line.isEmpty())
    {
      atools::fs::weather::Metar metar(list.at(1), "XXXX", QDateTime(), true);
      numFailed += !metar.isValid();
    }
  }
  metarFile.close();

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 26);
}

void MetarTest::testMetarSim()
{
  QFile metarFiles(":/test/resources/METAR.txt");
  QVERIFY(metarFiles.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFiles);

  int numFailed = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    if(!line.isEmpty())
    {
      atools::fs::weather::Metar metar(line, "XXXX", QDateTime(), true);
      numFailed += !metar.isValid();
    }
  }
  metarFiles.close();

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 19);
}
