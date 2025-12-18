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

#include "gribtest.h"

#include "grib/gribreader.h"
#include "grib/gribdownloader.h"
#include "grib/windquery.h"
#include "geo/rect.h"
#include "geo/linestring.h"
#include "testutil/testutil.h"
#include "exception.h"

using atools::grib::GribDownloader;
using atools::grib::GribReader;
using atools::grib::WindQuery;
using atools::geo::LineString;
using atools::geo::Pos;
using atools::geo::Rect;
using atools::grib::WindPosList;
using atools::grib::WindPos;
using atools::grib::Wind;

// Use grib_ls to verify results:
// lonx=0; laty=2 ; level=700 ;grib_ls -l$laty,$(($lonx+180)),1  -p level,shortName -w level=$level -F%4.8f testdata/lnm_winds.grib|grep $level

GribTest::GribTest()
{

}

void GribTest::runtest(int argc, char *argv[])
{
  GribTest tst;
  QTest::qExec(&tst, argc, argv);
}

void GribTest::initTestCase()
{
  queryLnm = new WindQuery(this, verbose);
  queryLnm->initFromPath("testdata/lnm_winds.grib", atools::fs::weather::WEATHER_XP11);
  queryLnm->setSamplesPerDegree(100);

  queryGlobal2 = new WindQuery(this, verbose);
  queryGlobal2->initFromPath("testdata/global_winds2.grib", atools::fs::weather::WEATHER_XP11);
  queryGlobal2->setSamplesPerDegree(100);
}

void GribTest::cleanupTestCase()
{
  delete queryLnm;
  delete queryGlobal2;
}

void GribTest::testLoadSpecialCharPath()
{
  WindQuery wq(this, verbose);
  wq.initFromPath("testdata/lnm_winds äöüß.grib", atools::fs::weather::WEATHER_XP11);
  wq.setSamplesPerDegree(100);
  QCOMPARE(wq.hasWindData(), true);
}

void GribTest::testGribReadTest_data()
{
  QTest::addColumn<atools::grib::WindQuery *>("query");
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("speed");
  QTest::addColumn<float>("dir");

  QTest::newRow("Pos(0.f,0.f) 0") << queryGlobal2 << Pos(0.f, 0.f, 0.f) << 0.f << 0.f;
  QTest::newRow("Pos(1.f,1.f)") << queryGlobal2 << Pos(1.f, 1.f, 10000) << 22.2137f << 92.5149f;
  QTest::newRow("Pos(-0.5f,-0.5f)") << queryGlobal2 << Pos(-0.5f, -0.5f, 10000) << 17.9995f << 89.3592f;
  QTest::newRow("Pos(-1.f,0.f)") << queryGlobal2 << Pos(-1.f, 0.f, 10000) << 18.5741f << 84.845f;
  QTest::newRow("Pos(0.f,0.f)") << queryGlobal2 << Pos(0.f, 0.f, 10000) << 22.296f << 93.6562f;
  QTest::newRow("Pos(0.f,-1.f)") << queryGlobal2 << Pos(0.f, -1.f, 10000) << 17.4816f << 94.4095f;
  QTest::newRow("Pos(-1.f,-1.f)") << queryGlobal2 << Pos(-1.f, -1.f, 10000) << 13.9446f << 82.16f;
  QTest::newRow("Pos(-0.5f,0.f)") << queryGlobal2 << Pos(-0.5f, 0.f, 10000) << 20.3751f << 89.6526f;
  QTest::newRow("Pos(0.f,-0.5f)") << queryGlobal2 << Pos(0.f, -0.5f, 10000) << 19.8884f << 93.9873f;
  QTest::newRow("Pos(0.5f,0.f)") << queryGlobal2 << Pos(0.5f, 0.f, 10000) << 20.8644f << 96.3999f;
  QTest::newRow("Pos(0.f,0.5f)") << queryGlobal2 << Pos(0.f, 0.5f, 10000) << 22.8479f << 91.1283f;
  QTest::newRow("Pos(0.5f,0.5f)") << queryGlobal2 << Pos(0.5f, 0.5f, 10000) << 21.8117f << 93.3538f;
  QTest::newRow("Pos(0.f,0.f)") << queryGlobal2 << Pos(0.f, 0.f, 10000) << 22.296f << 93.6562f;
  QTest::newRow("Pos(43.f,33.f)") << queryGlobal2 << Pos(43.f, 33.f, 10000) << 35.3638f << 221.662f;
  QTest::newRow("Pos(-15.f,46.f)") << queryGlobal2 << Pos(-15.f, 46.f, 10000) << 54.8431f << 287.531f;
  QTest::newRow("Pos(-87.f,33.f)") << queryGlobal2 << Pos(-87.f, 33.f, 10000) << 8.83288f << 142.014f;
  QTest::newRow("Pos(-100.f,32.f)") << queryGlobal2 << Pos(-100.f, 32.f, 10000) << 30.4046f << 232.225f;
  QTest::newRow("Pos(-25.f,37.f)") << queryGlobal2 << Pos(-25.f, 37.f, 10000) << 33.194f << 271.102f;
  QTest::newRow("Pos(9.f,50.f)") << queryGlobal2 << Pos(9.f, 50.f, 10000) << 29.2664f << 233.567f;
  QTest::newRow("Pos(107.f,-17.f)") << queryGlobal2 << Pos(107.f, -17.f, 10000) << 19.7367f << 81.6838f;
  QTest::newRow("Pos(30.f,-31.f)") << queryGlobal2 << Pos(30.f, -31.f, 10000) << 5.43035f << 352.321f;
  QTest::newRow("Pos(0.9f,0.9f)") << queryGlobal2 << Pos(0.9f, 0.9f, 10000) << 22.062f << 92.7212f;
  QTest::newRow("Pos(0.1f,0.1f)") << queryGlobal2 << Pos(0.1f, 0.1f, 10000) << 22.1285f << 93.6406f;
  QTest::newRow("Pos(-100.f,66.f)") << queryGlobal2 << Pos(-100.f, 66.f, 10000) << 14.4823f << 29.7474f;
  QTest::newRow("Pos(137.f,66.0f)") << queryGlobal2 << Pos(137.f, 66.0f, 10000) << 23.5245f << 180.584f;

  QTest::newRow("Pos(-12.f,52.0f)") << queryLnm << Pos(-12.f, 52.0f, 10000) << 48.8687f << 296.868f;
}

void GribTest::testGribReadTest()
{
  QFETCH(atools::grib::WindQuery *, query);
  QFETCH(Pos, pos);
  QFETCH(float, speed);
  QFETCH(float, dir);

  atools::grib::Wind wind = query->getWindForPos(pos);
  atools::grib::Wind wind2 = {dir, speed};

  qInfo() << "result" << wind << "expected" << wind2;
  // QCOMPARE(wind.dir, wind2.dir);
  // QCOMPARE(wind.speed, wind2.speed);
}

void GribTest::testGribReadInterpolate_data()
{
  QTest::addColumn<Pos>("pos");
  QTest::addColumn<float>("speed");
  QTest::addColumn<float>("dir");

  QTest::newRow("Pos(0.f,0.f), 30000") << Pos(0.f, 0.f, 30000.f) << 17.7578f << 120.973f;
  QTest::newRow("Pos(0.f,0.f), 38000") << Pos(0.f, 0.f, 38000.f) << 14.493f << 109.68f;
  QTest::newRow("Pos(0.f,0.f), 34000") << Pos(0.f, 0.f, 34000.f) << 16.048f << 115.9f;

  QTest::newRow("Pos(-2.f,2.f), 30000") << Pos(-2.f, 2.f, 30000) << 16.9319f << 122.666f;
  QTest::newRow("Pos(-2.f,2.f), 38000") << Pos(-2.f, 2.f, 38000) << 15.7804f << 117.201f;
  QTest::newRow("Pos(-2.f,2.f), 34000") << Pos(-2.f, 2.f, 34000)
                                        << (16.9319f + 15.7804f) / 2.f << (122.666f + 117.201f) / 2.f;

  QTest::newRow("Pos(2.f,2.f), 30000") << Pos(2.f, 2.f, 30000) << 11.4569f << 134.097f;
  QTest::newRow("Pos(2.f,2.f), 38000") << Pos(2.f, 2.f, 38000) << 14.3007f << 101.056f;
  QTest::newRow("Pos(2.f,2.f), 34000") << Pos(2.f, 2.f, 34000) << 12.3537f << 115.701f;

  QTest::newRow("Pos(2.f,-2.f), 30000") << Pos(2.f, -2.f, 30000) << 16.1795f << 130.462f;
  QTest::newRow("Pos(2.f,-2.f), 38000") << Pos(2.f, -2.f, 38000) << 15.264f << 118.201f;
  QTest::newRow("Pos(2.f,-2.f), 34000") << Pos(2.f, -2.f, 34000) << 15.6319f << 124.511f;

  QTest::newRow("Pos(-2.f,-2.f), 30000") << Pos(-2.f, -2.f, 30000) << 10.5281f << 111.687f;
  QTest::newRow("Pos(-2.f,-2.f), 38000") << Pos(-2.f, -2.f, 38000) << 13.5132f << 110.292f;
  QTest::newRow("Pos(-2.f,-2.f), 34000") << Pos(-2.f, -2.f, 34000) << 12.0198f << 110.903f;

}

void GribTest::testGribReadInterpolate()
{
  QFETCH(Pos, pos);
  QFETCH(float, speed);
  QFETCH(float, dir);

  atools::grib::Wind wind = queryLnm->getWindForPos(pos);
  atools::grib::Wind wind2 = {dir, speed};

  qInfo() << "result" << wind << "expected" << wind2;
  // QCOMPARE(atools::almostEqual(wind.dir, wind2.dir, 0.5f), true);
  // QCOMPARE(atools::almostEqual(wind.speed, wind2.speed, 0.5f), true);
}

void GribTest::testGribReadNoGrib()
{
  GribReader reader;
  QVERIFY_EXCEPTION_THROWN(reader.readFile("testdata/add-on.xml"), atools::Exception);
}

void GribTest::testGribReadNoFile()
{
  GribReader reader;
  QVERIFY_EXCEPTION_THROWN(reader.readFile(QString()), atools::Exception);
}

void GribTest::testGribReadNoData()
{
  GribReader reader;
  QVERIFY_EXCEPTION_THROWN(reader.readData(QByteArray()), atools::Exception);
}

void GribTest::testGribReadInvalidFile()
{
  GribReader reader;
  QVERIFY_EXCEPTION_THROWN(reader.readFile("testdata/global_turbulence.grib"), atools::Exception);
}

void GribTest::testGribWindLineStringQuery_data()
{
  QTest::addColumn<LineString>("line");
  QTest::addColumn<float>("speed"); // knots
  QTest::addColumn<float>("dir"); // deg

  /* *INDENT-OFF* */
  QTest::newRow("Point 2, 2 10000 ft") << LineString(Pos(2.f, 2.f, 10000.f), Pos(2.f, 2.f, 10000.f)) << 19.681f << 69.8261f;
  QTest::newRow("North south 0, 2/0, -2 10000 ft") << LineString(Pos(0.f, 2.f, 10000.f), Pos(0.f, -2.f, 10000.f))<< 23.0927f << 74.7271f;
  QTest::newRow("North south 0, 2/0, -2 20000 ft") << LineString(Pos(0.f, 2.f, 20000.f), Pos(0.f, -2.f, 20000.f)) << 20.0855f << 88.9084f;
  QTest::newRow("North south 0, -2/0, 2 10000 ft") << LineString(Pos(0.f, -2.f, 10000.f), Pos(0.f, 2.f, 10000.f)) << 23.0927f << 74.7271f;

  QTest::newRow("North south 0, 2/0, 0/0, -2 10000 ft") << LineString({Pos(0.f, 2.f, 10000.f), Pos(0.f, 0.f, 10000.f), Pos(0.f, -2.f, 10000.f)}) << 23.0907f << 74.7288f;
  QTest::newRow("North south 0, -2/0, 0/0, 2 10000 ft") << LineString({Pos(0.f, -2.f, 10000.f), Pos(0.f, 0.f, 10000.f), Pos(0.f, 2.f, 10000.f)}) << 23.0907f << 74.7288f;

  QTest::newRow("North south 0, 2/0, -2 10000-20000 ft") << LineString(Pos(0.f, 2.f, 10000.f), Pos(0.f, -2.f, 20000.f)) <<  21.6218f << 80.687f;
  QTest::newRow("North south 0, -2/0, 2 10000-20000 ft") << LineString(Pos(0.f, -2.f, 10000.f), Pos(0.f, 2.f, 20000.f)) << 21.2304f << 81.9681f;

  //w1: Wind(speed 21.1542, dir 72.0618) w2: Wind(speed 7.62563, dir 112.652)
  QTest::newRow("North south 0, -2/0, 2 10000-40000 ft") << LineString(Pos(0.f, -2.f, 10000.f), Pos(0.f, 2.f, 40000.f)) << 16.2103f << 96.9579f;

  QTest::newRow("East west anti meridian 4 deg 10000 ft") << LineString({Pos(178.f, 0.f, 10000.f), Pos(-178.f, 0.f, 10000.f)}) << 18.233f << 97.828f;
  /* *INDENT-ON* */

}

void GribTest::testGribWindLineStringQuery()
{
  QFETCH(LineString, line);
  QFETCH(float, speed);
  QFETCH(float, dir);

  atools::grib::Wind wind = queryLnm->getWindAverageForLineString(line);

  qInfo() << "w1:" << queryLnm->getWindForPos(line.constFirst()) << "w2:" << queryLnm->getWindForPos(line.constLast());

  qInfo() << line << "speed" << speed << "dir" << dir
          << "wind.speed" << wind.speed << "wind.dir" << wind.dir;

  // if(speed > 0.f)
  // QCOMPARE(wind.speed, speed);
  // if(dir > 0.f)
  // QCOMPARE(wind.dir, dir);
}

void GribTest::testGribWindRectQuery_data()
{
  QTest::addColumn<Rect>("rect");
  QTest::addColumn<float>("alt");
  QTest::addColumn<atools::grib::WindPosList>("result");

  /* *INDENT-OFF* */

//  QTest::newRow("Meridian  10000 ft 2 degrees") << Rect(-2.f, 2.f, 2.f, -2.f) << 10000.f
//    << atools::grib::WindPosList({
//        {Pos(-2.f, 2.f), Wind({101.763f, 16.2887f})},
//        {Pos(-1.f, 2.f), Wind({97.1598f, 16.8164f})},
//        {Pos(0.f, 2.f), Wind({95.6853f, 18.018f})},
//        {Pos(1.f, 2.f), Wind({95.3244f, 18.3975f})},
//        {Pos(2.f, 2.f), Wind({95.8283f, 18.7259f})},
//        {Pos(-2.f, 1.f), Wind({98.9273f, 15.8864f})},
//        {Pos(-1.f, 1.f), Wind({98.6531f, 16.3858f})},
//        {Pos(0.f, 1.f), Wind({95.628f, 17.606f})},
//        {Pos(1.f, 1.f), Wind({96.1931f, 18.3474f})},
//        {Pos(2.f, 1.f), Wind({97.2635f, 19.1913f})},
//        {Pos(-2.f, 0.f), Wind({98.0599f, 17.0284f})},
//        {Pos(-1.f, 0.f), Wind({96.829f, 17.6267f})},
//        {Pos(0.f, 0.f), Wind({97.2017f, 18.4246f})},
//        {Pos(1.f, 0.f), Wind({98.0114f, 19.3624f})},
//        {Pos(2.f, 0.f), Wind({97.4485f, 19.3169f})},
//        {Pos(-2.f, -1.f), Wind({98.8412f, 17.8105f})},
//        {Pos(-1.f, -1.f), Wind({99.6415f, 17.9693f})},
//        {Pos(0.f, -1.f), Wind({98.384f, 18.2409f})},
//        {Pos(1.f, -1.f), Wind({98.1166f, 18.5624f})},
//        {Pos(2.f, -1.f), Wind({98.9129f, 19.8013f})},
//        {Pos(-2.f, -2.f), Wind({100.544f, 18.0396f})},
//        {Pos(-1.f, -2.f), Wind({99.6937f, 17.8735f})},
//        {Pos(0.f, -2.f), Wind({98.6438f, 18.214f})},
//        {Pos(1.f, -2.f), Wind({98.941f, 19.1141f})},
//        {Pos(2.f, -2.f), Wind({96.8349f, 19.4085f})}
//    });

  QTest::newRow("West of Spain 0 ft 2 degrees") << Rect(-12.f, 52.f, -11.f, 51.f) << 0.f
                                                    << atools::grib::WindPosList({ });
  QTest::newRow("West of Spain 260 ft 2 degrees") << Rect(-12.f, 52.f, -11.f, 51.f) << 260.f
                                                << atools::grib::WindPosList({ });

  QTest::newRow("West of Spain 10000 ft 2 degrees") << Rect(-12.f, 52.f, -11.f, 51.f) << 10000.f
    << atools::grib::WindPosList({ });

  /* *INDENT-ON* */
}

void GribTest::testGribWindRectQuery()
{
  QFETCH(Rect, rect);
  QFETCH(float, alt);
  QFETCH(atools::grib::WindPosList, result);

  atools::grib::WindPosList winds = queryLnm->getWindForRect(rect, alt);

  for(int i = 0; i < winds.size(); i++)
  {
    atools::grib::WindPos wp = winds.at(i);

    qDebug() << wp;

    QCOMPARE(wp.pos.isValid(), true);
  }

  if(!result.isEmpty())
  {
    QCOMPARE(winds.size(), result.size());
    for(int i = 0; i < winds.size(); i++)
    {
      QCOMPARE(winds.at(i).pos.getLonX(), result.at(i).pos.getLonX());
      QCOMPARE(winds.at(i).pos.getLatY(), result.at(i).pos.getLatY());
      QCOMPARE(winds.at(i).wind.dir, result.at(i).wind.dir);
      QCOMPARE(winds.at(i).wind.speed, result.at(i).wind.speed);
    }
  }
}

void GribTest::testGribDownload()
{
  GribDownloader downloader(this, verbose);

  bool done = false, err = false;

  atools::grib::GribDatasetList testdatasets;
  connect(&downloader, &GribDownloader::gribDownloadFinished,
          [&testdatasets, &err, &done](const atools::grib::GribDatasetList& datasets, QString) -> void
  {
    testdatasets = datasets;
    done = true;
    err = false;
  });
  connect(&downloader, &GribDownloader::gribDownloadFailed,
          [&err, &done](const QString&, int, QString) -> void
  {
    done = true;
    err = true;
  });

  downloader.setParameters({"UGRD", "VGRD"});
  downloader.setSurfaces({-80, 200, 300, 450, 700});
  downloader.startDownload();

  testutil::waitForValue(done, 30);

  QCOMPARE(done, true);
  QCOMPARE(err, false);
  QCOMPARE(testdatasets.size(), 10);
  lnmDatasetTest(testdatasets, downloader.getDatetime());
}

void GribTest::testGribDownloadFail()
{
  GribDownloader downloader(this, verbose);

  bool done = false, err = false;

  atools::grib::GribDatasetList testdatasets;
  connect(&downloader, &GribDownloader::gribDownloadFinished,
          [&testdatasets, &err, &done](const atools::grib::GribDatasetList& datasets, QString) -> void
  {
    testdatasets = datasets;
    done = true;
    err = false;
  });
  connect(&downloader, &GribDownloader::gribDownloadFailed,
          [&err, &done](const QString&, int, QString) -> void
  {
    done = true;
    err = true;
  });

  downloader.setParameters({"UGRD", "VGRD"});
  downloader.setSurfaces({-80, 200, 300, 450, 700});
  downloader.startDownload(QDateTime(QDate(2000, 01, 01), QTime(12, 0, 0), Qt::UTC));

  testutil::waitForValue(done, 30);

  QCOMPARE(err, true);
  QCOMPARE(testdatasets.size(), 0);
}

void GribTest::testGribLoadWinds()
{
  GribReader reader(verbose);
  reader.readFile("testdata/global_winds.grib");

  const atools::grib::GribDatasetList& datasets = reader.getDatasets();

  QCOMPARE(datasets.size(), 4);
  QCOMPARE(datasets.at(0).getData().size(), 65160);
  QCOMPARE(datasets.at(0).getSurface(), 700.f);
  QCOMPARE(datasets.at(0).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(0).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(0).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(0).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));

  QCOMPARE(datasets.at(1).getData().size(), 65160);
  QCOMPARE(datasets.at(1).getSurface(), 700.f);
  QCOMPARE(datasets.at(1).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(1).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(1).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(1).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));

  QCOMPARE(datasets.at(2).getData().size(), 65160);
  QCOMPARE(datasets.at(2).getSurface(), 250.f);
  QCOMPARE(datasets.at(2).getAltFeetCalculated(), 34004.1f);
  QCOMPARE(datasets.at(2).getAltFeetRounded(), 34000.f);
  QCOMPARE(datasets.at(2).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(2).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));

  QCOMPARE(datasets.at(3).getData().size(), 65160);
  QCOMPARE(datasets.at(3).getSurface(), 250.f);
  QCOMPARE(datasets.at(3).getAltFeetCalculated(), 34004.1f);
  QCOMPARE(datasets.at(3).getAltFeetRounded(), 34000.f);
  QCOMPARE(datasets.at(3).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(3).getDatetime(), QDateTime(QDate(2019, 04, 21), QTime(6, 0, 0), Qt::UTC));
}

void GribTest::testGribLoadLnm()
{
  GribReader reader(verbose);
  // https://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_1p00.pl?file=gfs.t06z.pgrb2.1p00.anl&lev_200_mb=on&lev_300_mb=on&lev_450_mb=on&lev_700_mb=on&var_UGRD=on&var_VGRD=on&dir=%2Fgfs.2019042606
  reader.readFile("testdata/lnm_winds.grib");

  lnmDatasetTest(reader.getDatasets(), QDateTime(QDate(2019, 04, 27), QTime(6, 0, 0), Qt::UTC));
}

void GribTest::lnmDatasetTest(const atools::grib::GribDatasetList& datasets, const QDateTime& datetime)
{
  QCOMPARE(datasets.size(), 10);

  int i = 0;
  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 80.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 262.467f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 260.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getSurfaceType(), atools::grib::METER_AGL);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 80.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 262.467f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 260.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getSurfaceType(), atools::grib::METER_AGL);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 700.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 700.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 9884.08f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 10000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 450.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 20815.6f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 20000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 450.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 20815.6f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 20000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 300.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 30069.9f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 30000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 300.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 30069.9f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 30000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 200.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 38637.1f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 38000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::U_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
  i++;

  QCOMPARE(datasets.at(i).getData().size(), 65160);
  QCOMPARE(datasets.at(i).getSurface(), 200.f);
  QCOMPARE(datasets.at(i).getAltFeetCalculated(), 38637.1f);
  QCOMPARE(datasets.at(i).getAltFeetRounded(), 38000.f);
  QCOMPARE(datasets.at(i).getParameterType(), atools::grib::V_WIND);
  QCOMPARE(datasets.at(i).getDatetime(), datetime);
}
