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

#include "metartest.h"

#include "fs/weather/metar.h"
#include "fs/weather/metarindex.h"
#include "fs/weather/metarparser.h"
#include "fs/weather/noaaweatherdownloader.h"
#include "fs/weather/weathernetdownload.h"
#include "geo/pos.h"
#include "atools.h"
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
  {"EDDV", atools::geo::Pos(9.68352222442627, 52.4602127075195)},
  {"EDDW", atools::geo::Pos(8.78674697875977, 53.0474014282227)},
  {"EDDS", atools::geo::Pos(9.22196388244629, 48.6898765563965)},
  {"EDDR", atools::geo::Pos(7.1095085144043, 49.2145538330078)},
  {"EDDE", atools::geo::Pos(10.9581060409546, 50.9798126220703)},
  {"EDDG", atools::geo::Pos(7.68483066558838, 52.1346435546875)},
  {"EDDP", atools::geo::Pos(12.2363834381104, 51.4239921569824)},
  {"EDCF", atools::geo::Pos(13.80201, 52.28137)} // No METAR - nearest EDDB
};

const static QHash<QString, QString> AIRPORT_SUBST = {
  {"EDCF", "EDDB"}
};

const static QHash<QString, atools::geo::Pos> AIRPORT_COORDS_INTERPOLATE = {
  {"XX1N", atools::geo::Pos(0.f, 1.f)},
  {"XX1E", atools::geo::Pos(1.f, 0.f)},
  {"XX1S", atools::geo::Pos(0.f, -1.f)},
  {"XX1W", atools::geo::Pos(-1.f, 0.f)},
  {"XX2N", atools::geo::Pos(0.f, 2.f)},
  {"XX2E", atools::geo::Pos(2.f, 0.f)},
  {"XX2S", atools::geo::Pos(0.f, -2.f)},
  {"XX2W", atools::geo::Pos(-2.f, 0.f)}
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

void MetarTest::testIvaoRead()
{
  WeatherNetDownload downloader(this, atools::fs::weather::JSON, true);
  downloader.setRequestUrl("testdata/ivao_metar_fmt.json");
  testDownload(downloader, false);
}

void MetarTest::testIvaoDownload()
{
  QString KEY(":/atoolstest/little_navmap_keys/ivao_weather_api_key.bin");
  if(QFile::exists(KEY))
  {
    WeatherNetDownload downloader(this, atools::fs::weather::JSON, true);
    downloader.setRequestUrl("https://api.ivao.aero/v2/airports/all/metar");
    downloader.setHeaderParameters({
      {"accept", "application/json"},
      {"apiKey", atools::strFromCryptFile(KEY, 0x2B1A96468EB62460)}
    });
    testDownload(downloader, false);
  }
}

void MetarTest::testIvaoDownloadFailed()
{
  WeatherNetDownload downloader(this, atools::fs::weather::JSON, true);
  downloader.setRequestUrl("https://api.ivao.aero/v2/airports/all/metarXXX");
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
  // Does not work since error code is not given by NOAA
  // downloader.setRequestUrl("https://tgftp.nws.noaa.gov/data/observations/metar/cycles/XXXXXXXX");
  // testDownload(downloader, true);
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
    qDebug() << Q_FUNC_INFO << "weatherUpdated";
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

  downloader.setFetchAirportCoords([](const QString& airportIdent) -> atools::geo::Pos {
    return AIRPORT_COORDS.value(airportIdent);
  });

  atools::fs::weather::Metar metar = downloader.getMetar("EDDF", AIRPORT_COORDS.value("EDDF"));
  if(QFileInfo::exists(downloader.getRequestUrl()))
    QVERIFY(metar.hasAnyMetar());
  else
    QVERIFY(!metar.hasAnyMetar());

  testutil::waitForValue(finished, 180);

  qDebug() << Q_FUNC_INFO << downloader.getRequestUrl() << downloader.size() << "updateFlag" << updateFlag << "errorFlag" << errorFlag;

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
    QVERIFY(downloader.size() > 100);

#if 0
    for(auto it = AIRPORT_COORDS.constBegin(); it != AIRPORT_COORDS.constEnd(); ++it)
    {
      const QString ident = it.key();
      const atools::geo::Pos pos = it.value();
      qDebug() << Q_FUNC_INFO << "Checking for" << ident << "at" << pos;

      metar = downloader.getMetar(ident, pos);

      qDebug() << Q_FUNC_INFO << "Result" << metar;
      QVERIFY(metar.hasAnyMetar());

      QVERIFY(metar.getRequestIdent() == ident);
      if(AIRPORT_SUBST.contains(ident))
      {
        QVERIFY(metar.getStationMetar().isEmpty());
        QVERIFY(metar.getNearestMetar().startsWith(AIRPORT_SUBST.value(ident)));
      }
      else
      {
        QVERIFY(metar.getStationMetar().startsWith(ident));
        QVERIFY(metar.getNearestMetar().isEmpty());
      }
    }
#endif

    for(auto it = AIRPORT_COORDS.constBegin(); it != AIRPORT_COORDS.constEnd(); ++it)
    {
      const QString ident = it.key();
      const atools::geo::Pos pos = it.value();
      qDebug() << Q_FUNC_INFO << "Checking for" << ident << "at" << pos;

      if(AIRPORT_SUBST.contains(ident))
        qDebug() << Q_FUNC_INFO;

      metar = downloader.getMetar(QString(), pos);

      qDebug() << Q_FUNC_INFO << "Result" << metar;
      QVERIFY(metar.hasAnyMetar());
      QVERIFY(metar.getStationMetar().isEmpty());

      if(AIRPORT_SUBST.contains(ident))
      {
        QVERIFY(metar.getRequestIdent().isEmpty());
        QVERIFY(!metar.getNearestMetar().isEmpty());
        QVERIFY(metar.getNearestIdent() == AIRPORT_SUBST.value(ident));
        QVERIFY(!metar.getInterpolatedMetar().isEmpty());
      }
      else
      {
        QVERIFY(metar.getRequestIdent().isEmpty());
        QVERIFY(metar.getNearestIdent() == ident);
        QVERIFY(!metar.getNearestMetar().isEmpty());
        QVERIFY(metar.getInterpolatedMetar().isEmpty());
      }
    }

    metar = downloader.getMetar("KORD", atools::geo::EMPTY_POS);
    qDebug() << Q_FUNC_INFO << "Result" << metar;
    QVERIFY(metar.hasAnyMetar());
    QVERIFY(metar.getStationMetar().startsWith("KORD"));
    QVERIFY(metar.getRequestIdent() == "KORD");
    QVERIFY(metar.getNearestMetar().isEmpty());
  }
}

void MetarTest::testMetarAsn()
{
  QFile metarFile("testdata/current_wx_snapshot.txt");
  QVERIFY(metarFile.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFile);

  int numFailed = 0, numFailedPressure = 0, numFailedTemp = 0, numFailedDewpoint = 0, numFailedWind = 0, numInvalidTimestamp = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    QStringList list = line.split("::");
    QVERIFY(list.size() >= 2);

    if(!line.isEmpty())
    {
      atools::fs::weather::Metar metar(QString(), atools::geo::EMPTY_POS, QDateTime(), list.at(1));
      metar.parseAll(true /* useTimestamp */);

      const atools::fs::weather::MetarParser& metarParser = metar.getMetarParser(atools::fs::weather::STATION);

      numFailed += metarParser.hasErrors();
      numInvalidTimestamp += !metarParser.getTimestamp().isValid();
      numFailedPressure += !(metarParser.getPressureMbar() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedTemp += !(metarParser.getTemperatureC() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedDewpoint += !(metarParser.getDewpointDegC() < atools::fs::weather::INVALID_METAR_VALUE);
      numFailedWind += !(metarParser.getWindSpeedKts() < atools::fs::weather::INVALID_METAR_VALUE);
    }
  }
  metarFile.close();

  qDebug() << Q_FUNC_INFO << "numFailed" << numFailed << "numFailedPressure" << numFailedPressure << "numFailedTemp" << numFailedTemp
           << "numFailedDewpoint" << numFailedDewpoint << "numFailedWind" << numFailedWind;

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 24);
  QCOMPARE(numInvalidTimestamp, 1);
  QCOMPARE(numFailedPressure, 367);
  QCOMPARE(numFailedTemp, 69);
  QCOMPARE(numFailedDewpoint, 109);
  QCOMPARE(numFailedWind, 66);
}

void MetarTest::testMetarSim()
{
  QFile metarFiles("testdata/METAR.txt");
  QVERIFY(metarFiles.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFiles);

  int numFailed = 0, numNoPressure = 0, numNoTemp = 0, numNoDewpoint = 0, numNoWind = 0, numInvalidTimestamp = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    if(!line.isEmpty() && !line.startsWith('#'))
    {
      atools::fs::weather::Metar metar(QString(), atools::geo::EMPTY_POS, QDateTime(), line);
      metar.parseAll(true /* useTimestamp */);

      const atools::fs::weather::MetarParser& metarParser = metar.getMetarParser(atools::fs::weather::STATION);

      numFailed += metarParser.hasErrors();

      QCOMPARE(!metarParser.hasErrors(), metarParser.isParsed());

      if(metarParser.hasErrors())
        qWarning() << Q_FUNC_INFO << metarParser.getMetarString();

      bool invalidTimestamp = !metar.getTimestamp().isValid();
      numInvalidTimestamp += invalidTimestamp;

      bool noPressure = !(metarParser.getPressureMbar() < atools::fs::weather::INVALID_METAR_VALUE);
      numNoPressure += noPressure;

      bool noTemp = !(metarParser.getTemperatureC() < atools::fs::weather::INVALID_METAR_VALUE);
      numNoTemp += noTemp;

      bool noDewpoint = !(metarParser.getDewpointDegC() < atools::fs::weather::INVALID_METAR_VALUE);
      numNoDewpoint += noDewpoint;

      bool noWind = !(metarParser.getWindSpeedKts() < atools::fs::weather::INVALID_METAR_VALUE);
      numNoWind += noWind;

      if(noPressure || noTemp || noDewpoint || noWind)
      {
        qWarning() << Q_FUNC_INFO << "=========";
        qWarning() << Q_FUNC_INFO << metarParser.getErrors();
        qWarning() << Q_FUNC_INFO << "noPressure" << noPressure << "noTemp" << noTemp << "noDewpoint" << noDewpoint << "noWind" << noWind;
        qWarning() << Q_FUNC_INFO << metarParser.getMetarString();
      }
    }
  }
  metarFiles.close();

  qDebug() << Q_FUNC_INFO << "numFailed" << numFailed << "numNoPressure" << numNoPressure << "numNoTemp" << numNoTemp
           << "numNoDewpoint" << numNoDewpoint << "numNoWind" << numNoWind;

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 169);
  QCOMPARE(numInvalidTimestamp, 169);
  QCOMPARE(numNoPressure, 554);
  QCOMPARE(numNoTemp, 261);
  QCOMPARE(numNoDewpoint, 316);
  QCOMPARE(numNoWind, 288);
}

void MetarTest::testMetarInterpolatedSimple()
{
  atools::fs::weather::MetarIndex index(atools::fs::weather::FLAT);
  index.setFetchAirportCoords([](const QString& airportIdent) -> atools::geo::Pos {
    return AIRPORT_COORDS.value(airportIdent);
  });

  QCOMPARE(index.read("testdata/METAR.txt", false), 9084);

  qDebug() << Q_FUNC_INFO << "index.size()" << index.numStationMetars();
  QCOMPARE(index.numStationMetars(), 4977);

  atools::fs::weather::Metar metar = index.getMetar("EDDM", AIRPORT_COORDS.value("EDDM"));
  QVERIFY(metar.hasAnyMetar());
  QVERIFY(metar.hasStationMetar());
  QVERIFY(metar.getMetarParserStation().isParsed());
  QVERIFY(!metar.getMetarParserNearest().isParsed());
  QVERIFY(!metar.getMetarParserInterpolated().isParsed());
  qDebug() << Q_FUNC_INFO << "metar" << metar;

  metar = index.getMetar("EDCF", AIRPORT_COORDS.value("EDCF"));
  QVERIFY(metar.hasAnyMetar());
  QVERIFY(!metar.hasStationMetar());
  QVERIFY(!metar.getMetarParserStation().isParsed());
  QVERIFY(metar.getMetarParserNearest().isParsed());
  QVERIFY(metar.getMetarParserInterpolated().isParsed());
  qDebug() << Q_FUNC_INFO << "metar" << metar;

  metar = index.getMetar("EDDF", atools::geo::EMPTY_POS);
  QVERIFY(metar.hasAnyMetar());
  QVERIFY(metar.hasStationMetar());
  QVERIFY(metar.getMetarParserStation().isParsed());
  QVERIFY(!metar.getMetarParserNearest().isParsed());
  QVERIFY(!metar.getMetarParserInterpolated().isParsed());
  qDebug() << Q_FUNC_INFO << "metar" << metar;
}

void MetarTest::testMetarInterpolated()
{
  atools::fs::weather::MetarIndex index(atools::fs::weather::FLAT);

  index.setFetchAirportCoords([](const QString& airportIdent) -> atools::geo::Pos {
    return AIRPORT_COORDS_INTERPOLATE.value(airportIdent);
  });

  // XX1N 291230Z 00010KT 1000 -RA OVC010 10/10 Q1000
  // XX1E 291330Z 09010KT 2000 RA OVC020 20/20 Q1100
  // XX1S 291430Z 18010KT 3000 RA OVC030 30/30 Q1200
  // XX1W 291530Z 27010KT 4000 +RA OVC040 40/40 Q1300
  QCOMPARE(index.read("testdata/METAR4.txt", false), 4);
  qDebug() << Q_FUNC_INFO << "index.size()" << index.numStationMetars();
  QCOMPARE(index.numStationMetars(), 4);
  atools::fs::weather::Metar metar;

  metar = index.getMetar(QString(), AIRPORT_COORDS_INTERPOLATE.value("XX1N"));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";
  validateMetar(metar,
                QString(), QString(),
                "XX1N 291230Z 00010KT 1000 -RA OVC010 10/10 Q1000", "LIFR",
                QString(), QString());

  metar = index.getMetar("XX1N", AIRPORT_COORDS_INTERPOLATE.value("XX1N"));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";
  validateMetar(metar,
                "XX1N 291230Z 00010KT 1000 -RA OVC010 10/10 Q1000", "LIFR",
                QString(), QString(),
                QString(), QString());

  metar = index.getMetar(QString(), atools::geo::Pos(0.f, 0.f));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";
  validateMetar(metar,
                QString(), QString(),
                "XX1N 291230Z 00010KT 1000 -RA OVC010 10/10 Q1000", "LIFR",
                "XXXX 291530Z 00000KT 2500 RA 25/25 Q1150", "IFR");

  // XX1N 291230Z 00010KT 1000 RA OVC010 10/10 Q1100
  // XX1E 291330Z 00010KT 1000 RA OVC010 10/10 Q1100
  // XX1S 291430Z 00010KT 1000 RA OVC010 10/10 Q1100
  // XX1W 291530Z 00010KT 1000 RA OVC010 10/10 Q1100
  // XX2N 291230Z 18010KT 2000 +RA OVC020 20/20 Q1200
  // XX2E 291230Z 18010KT 2000 +RA OVC020 20/20 Q1200
  // XX2S 291230Z 18010KT 2000 +RA OVC020 20/20 Q1200
  // XX2W 291230Z 18010KT 2000 +RA OVC020 20/20 Q1200
  QCOMPARE(index.read("testdata/METAR8.txt", false), 8);
  qDebug() << Q_FUNC_INFO << "index.size()" << index.numStationMetars();
  QCOMPARE(index.numStationMetars(), 8);

  metar = index.getMetar(QString(), AIRPORT_COORDS_INTERPOLATE.value("XX2N"));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";
  validateMetar(metar,
                QString(), QString(),
                "XX2N 291230Z 18010KT 2000 +RA OVC020 20/20 Q1200", "IFR",
                QString(), QString());

  metar = index.getMetar("XX2N", AIRPORT_COORDS_INTERPOLATE.value("XX2N"));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";
  validateMetar(metar,
                "XX2N 291230Z 18010KT 2000 +RA OVC020 20/20 Q1200", "IFR",
                QString(), QString(),
                QString(), QString());

  metar = index.getMetar(QString(), atools::geo::Pos(0.f, 0.f));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";
  validateMetar(metar,
                QString(), QString(),
                "XX1N 291230Z 00010KT 1000 RA OVC010 10/10 Q1100", "LIFR",
                "XXXX 291530Z 00003KT 1300 RA 13/13 Q1133", "LIFR");
  // XX1N 291230Z 00020KT 6000 RA OVC100 10/10 Q1100
  // XX2N 291230Z 18020KT 8000 +RA OVC120 20/20 Q1200
  QCOMPARE(index.read("testdata/METAR2.txt", false), 2);
  qDebug() << Q_FUNC_INFO << "index.size()" << index.numStationMetars();
  QCOMPARE(index.numStationMetars(), 2);

  metar = index.getMetar(QString(), atools::geo::Pos(0.f, 0.f));
  qDebug() << Q_FUNC_INFO << Qt::endl << "===================" << Qt::endl << metar << Qt::endl << "============";

  validateMetar(metar,
                QString(), QString(),
                "XX1N 291230Z 00020KT 6000 RA OVC100 10/10 Q1100", "MVFR",
                "XXXX 291230Z 00007KT 6000 RA 13/13 Q1133", "MVFR");
}

void MetarTest::validateMetar(const atools::fs::weather::Metar& m,
                              const QString& stationMetar, const QString& stationFlightRules,
                              const QString& nearestMetar, const QString& nearestFlightRules,
                              const QString& interpolatedMetar, const QString& interpolatedFlightRules)
{
  QCOMPARE(m.getStationMetar(), stationMetar);
  QCOMPARE(m.getStation().getFlightRulesString(), stationFlightRules);

  QCOMPARE(m.getNearestMetar(), nearestMetar);
  QCOMPARE(m.getNearest().getFlightRulesString(), nearestFlightRules);

  QCOMPARE(m.getInterpolatedMetar(), interpolatedMetar);
  QCOMPARE(m.getInterpolated().getFlightRulesString(), interpolatedFlightRules);
}
