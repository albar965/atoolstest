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

#include "metartest.h"

#include "fs/weather/metar.h"
#include "fs/weather/metarparser.h"
#include "fs/weather/noaaweatherdownloader.h"
#include "fs/weather/weathernetdownload.h"
#include "geo/pos.h"
#include "fs/weather/weatherdownloadbase.h"
#include "testutil/testutil.h"

using atools::fs::weather::NoaaWeatherDownloader;
using atools::fs::weather::WeatherNetDownload;
using atools::fs::weather::WeatherDownloadBase;

// select '{"'||ident||'"'|| ', atools::geo::Pos('||lonx ||',' || laty || ')}' from airport where ident like 'EDD%';
const static QHash<QString, atools::geo::Pos> AIRPORT_COORDS = {
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
  {"EDDI", atools::geo::Pos(13.4031963348389, 52.4747467041016)} // Closed
};

const static QHash<QString, QString> AIRPORT_SUBST = {
  {"EDDI", "EDDT"}
};

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

void MetarTest::testVatsimDownload()
{
  WeatherNetDownload downloader(this, atools::fs::weather::FLAT, true);
  downloader.setRequestUrl("https://metar.vatsim.net/metar.php?id=ALL");
  testDownload(downloader, false);
}

void MetarTest::testVatsimDownloadFailed()
{
  WeatherNetDownload downloader(this, atools::fs::weather::FLAT, true);
  downloader.setRequestUrl("https://www.vatsim.net/XXX");
  testDownload(downloader, true);
}

void MetarTest::testIvaoDownload()
{
  WeatherNetDownload downloader(this, atools::fs::weather::FLAT, true);
  downloader.setRequestUrl("http://wx.ivao.aero/metar.php");
  testDownload(downloader, false);
}

void MetarTest::testIvaoDownloadFailed()
{
  WeatherNetDownload downloader(this, atools::fs::weather::FLAT, true);
  downloader.setRequestUrl("http://wx.ivao.aero/XXX");
  testDownload(downloader, true);
}

void MetarTest::testNoaaDownload()
{
  NoaaWeatherDownloader downloader(this, true);
  downloader.setRequestUrl("https://tgftp.nws.noaa.gov/data/observations/metar/cycles/%1Z.TXT");
  testDownload(downloader, false);
}

void MetarTest::testNoaaDownloadFailed()
{
  NoaaWeatherDownloader downloader(this, true);
  downloader.setRequestUrl("https://tgftp.nws.noaa.gov/data/observations/metar/cycles/XXX%1XXX");
  testDownload(downloader, true);
}

void MetarTest::testNoaaDownloadFailedNoHost()
{
  NoaaWeatherDownloader downloader(this, true);
  downloader.setRequestUrl("https://NONONONO.nws.noaa.gov/data/observations/metar/cycles/%1Z.TXT");
  testDownload(downloader, true);
}

void MetarTest::testDownload(atools::fs::weather::WeatherDownloadBase& downloader, bool expectFail)
{
  bool updateFlag = false, errorFlag = false, finished = false;

  connect(&downloader, &WeatherDownloadBase::weatherUpdated, [&updateFlag, &finished]() -> void
  {
    updateFlag = true;
    finished = true;
  });

  connect(&downloader, &WeatherDownloadBase::weatherDownloadFailed,
          [&errorFlag, &finished](const QString& error, int errorCode, QString url) -> void
  {
    qDebug() << Q_FUNC_INFO << error << errorCode << url;
    errorFlag = true;
    finished = true;
  });

  const QHash<QString, atools::geo::Pos> *coords = &AIRPORT_COORDS;
  downloader.setFetchAirportCoords([&coords](const QString& airportIdent) -> atools::geo::Pos
  {
    return coords->value(airportIdent);
  });

  atools::fs::weather::MetarResult metar = downloader.getMetar("EDDF", AIRPORT_COORDS.value("EDDF"));
  QVERIFY(metar.isEmpty());

  testutil::waitForValue(finished, 180);

  qDebug() << Q_FUNC_INFO << downloader.size() << "updateFlag" << updateFlag << "errorFlag" << errorFlag;

  if(expectFail)
  {
    QVERIFY(!updateFlag);
    QVERIFY(errorFlag);
    QVERIFY(downloader.size() == 0);
  }
  else
  {
    QVERIFY(updateFlag);
    QVERIFY(!errorFlag);
    QVERIFY(downloader.size() > 500);

    for(auto it = AIRPORT_COORDS.begin(); it != AIRPORT_COORDS.end(); ++it)
    {
      const QString ident = it.key();
      const atools::geo::Pos pos = it.value();
      qDebug() << Q_FUNC_INFO << "Checking for" << ident << "at" << pos;

      metar = downloader.getMetar(ident, pos);

      qDebug() << Q_FUNC_INFO << "Result" << metar;
      QVERIFY(!metar.isEmpty());

      QVERIFY(metar.requestIdent == ident);
      if(AIRPORT_SUBST.contains(ident))
      {
        QVERIFY(metar.metarForStation.isEmpty());
        QVERIFY(metar.metarForNearest.startsWith(AIRPORT_SUBST.value(ident)));
      }
      else
      {
        QVERIFY(metar.metarForStation.startsWith(ident));
        QVERIFY(metar.metarForNearest.isEmpty());
      }
    }

    for(auto it = AIRPORT_COORDS.begin(); it != AIRPORT_COORDS.end(); ++it)
    {
      const QString ident = it.key();
      const atools::geo::Pos pos = it.value();
      qDebug() << Q_FUNC_INFO << "Checking for" << ident << "at" << pos;

      metar = downloader.getMetar(QString(), pos);

      qDebug() << Q_FUNC_INFO << "Result" << metar;
      QVERIFY(!metar.isEmpty());
      QVERIFY(metar.metarForStation.isEmpty());

      if(AIRPORT_SUBST.contains(ident))
      {
        QVERIFY(metar.requestIdent == AIRPORT_SUBST.value(ident));
        QVERIFY(metar.metarForNearest.startsWith(AIRPORT_SUBST.value(ident)));
      }
      else
      {
        QVERIFY(metar.requestIdent == ident);
        QVERIFY(metar.metarForNearest.startsWith(ident));
      }
    }

    metar = downloader.getMetar("KORD", atools::geo::EMPTY_POS);
    qDebug() << Q_FUNC_INFO << "Result" << metar;
    QVERIFY(!metar.isEmpty());
    QVERIFY(metar.metarForStation.startsWith("KORD"));
    QVERIFY(metar.requestIdent == "KORD");
    QVERIFY(metar.metarForNearest.isEmpty());
  }
}

void MetarTest::testMetarAsn()
{
  QFile metarFile(":/test/resources/current_wx_snapshot.txt");
  QVERIFY(metarFile.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFile);

  int numFailed = 0, numFailedPressure = 0, numFailedTemp = 0, numFailedDewpoint = 0, numFailedWind = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    QStringList list = line.split("::");
    QVERIFY(list.size() >= 2);

    if(!line.isEmpty())
    {
      atools::fs::weather::Metar metar(list.at(1), "XXXX", QDateTime(), true);
      numFailed += !metar.isValid();
      numFailedPressure += !(metar.getParsedMetar().getPressureMbar() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedTemp += !(metar.getParsedMetar().getTemperatureC() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedDewpoint += !(metar.getParsedMetar().getDewpointDegC() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedWind += !(metar.getParsedMetar().getWindSpeedKts() < atools::fs::weather::INVALID_METAR_VALUE);
    }
  }
  metarFile.close();

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 21);
  QCOMPARE(numFailedPressure, 366);
  QCOMPARE(numFailedTemp, 67);
  QCOMPARE(numFailedDewpoint, 107);
  QCOMPARE(numFailedWind, 72);
}

void MetarTest::testMetarSim()
{
  QFile metarFiles(":/test/resources/METAR.txt");
  QVERIFY(metarFiles.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFiles);

  int numFailed = 0, numFailedPressure = 0, numFailedTemp = 0, numFailedDewpoint = 0, numFailedWind = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    if(!line.isEmpty() && !line.startsWith('#'))
    {
      atools::fs::weather::Metar metar(line, "XXXX", QDateTime(), true);
      numFailed += !metar.isValid();
      numFailedPressure += !(metar.getParsedMetar().getPressureMbar() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedTemp += !(metar.getParsedMetar().getTemperatureC() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedDewpoint += !(metar.getParsedMetar().getDewpointDegC() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedWind += !(metar.getParsedMetar().getWindSpeedKts() < atools::fs::weather::INVALID_METAR_VALUE);
    }
  }
  metarFiles.close();

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 15);
  QCOMPARE(numFailedPressure, 398);
  QCOMPARE(numFailedTemp, 105);
  QCOMPARE(numFailedDewpoint, 160);
  QCOMPARE(numFailedWind, 137);
}
