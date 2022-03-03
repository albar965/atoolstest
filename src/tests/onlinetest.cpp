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

#include "onlinetest.h"

#include "testutil/testutil.h"
#include "sql/sqldatabase.h"
#include "sql/sqlutil.h"
#include "zip/gzip.h"
#include "atools.h"
#include "util/httpdownloader.h"
#include "fs/online/onlinedatamanager.h"

using atools::sql::SqlDatabase;
using atools::sql::SqlUtil;
using atools::fs::online::OnlinedataManager;

OnlineTest::OnlineTest()
{

}

void OnlineTest::runtest(int argc, char *argv[])
{
  OnlineTest tst;
  QTest::qExec(&tst, argc, argv);
}

void OnlineTest::initTestCase()
{
  dbVatsim = testutil::createDb("TESTDBVATSIM", "online_test_vatsim.sqlite");
  dbVatsim3 = testutil::createDb("TESTDBVATSIM3", "online_test_vatsim3.sqlite");

  dbIvao = testutil::createDb("TESTDBIVAO", "online_test_ivao.sqlite");
  dbIvao2 = testutil::createDb("TESTDBIVAO2", "online_test_ivao2.sqlite");

  dbCustom = testutil::createDb("TESTDBCUSTOM", "online_test_custom.sqlite");
}

void OnlineTest::cleanupTestCase()
{
  testutil::removeDb(dbVatsim, "TESTDBVATSIM");
  testutil::removeDb(dbVatsim3, "TESTDBVATSIM3");

  testutil::removeDb(dbIvao, "TESTDBIVAO");
  testutil::removeDb(dbIvao2, "TESTDBIVAO2");

  testutil::removeDb(dbCustom, "TESTDBCUSTOM");
}

void OnlineTest::testCreateSchemaVatsim()
{
  OnlinedataManager odm(dbVatsim, true);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testCreateSchemaVatsim3()
{
  OnlinedataManager odm(dbVatsim3, true);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testCreateSchemaIvao()
{
  OnlinedataManager odm(dbIvao, true);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testCreateSchemaIvao2()
{
  OnlinedataManager odm(dbIvao2, true);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testCreateSchemaCustom()
{
  OnlinedataManager odm(dbCustom, true);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testOpenStatusVatsim()
{
  OnlinedataManager odm(dbVatsim, true);
  odm.readFromStatus(atools::strFromFile("testdata/vatsim-status.txt"));

  QCOMPARE(odm.getMessageFromStatus(), QString("VATSIM TEST message"));

  for(int i = 0; i < 10; i++)
  {
    bool gzipped, json;
    QString url0 = odm.getWhazzupUrlFromStatus(gzipped, json);
    qInfo() << "url0" << url0;
    QCOMPARE(url0.startsWith("http://"), true);
    QCOMPARE(gzipped, false);
    QCOMPARE(json, false);

    QString url1 = odm.getWhazzupVoiceUrlFromStatus();
    qInfo() << "url1" << url1;
    QCOMPARE(url1.startsWith("http://"), true);
  }
}

void OnlineTest::testOpenStatusVatsim3()
{
  OnlinedataManager odm(dbVatsim, true);
  odm.readFromStatus(atools::strFromFile("testdata/vatsim-status-json.txt"));

  QCOMPARE(odm.getMessageFromStatus(), QString("VATSIM TEST message"));

  for(int i = 0; i < 10; i++)
  {
    bool gzipped, json;
    QString url0 = odm.getWhazzupUrlFromStatus(gzipped, json);
    qInfo() << "url0" << url0;
    QCOMPARE(url0.startsWith("https://"), true);
    QCOMPARE(gzipped, false);
    QCOMPARE(json, true);

    QString url1 = odm.getWhazzupVoiceUrlFromStatus();
    qInfo() << "url1" << url1;
    QCOMPARE(url1.startsWith("http://"), true);
  }
}

void OnlineTest::testOpenStatusIvao()
{
  OnlinedataManager odm(dbIvao, true);
  odm.readFromStatus(atools::strFromFile("testdata/ivao-status.txt"));

  QCOMPARE(odm.getMessageFromStatus(), QString("IVAO TEST message"));

  for(int i = 0; i < 10; i++)
  {
    bool gzipped, json;
    QString url0 = odm.getWhazzupUrlFromStatus(gzipped, json);
    qInfo() << "url0" << url0;
    QCOMPARE(url0.startsWith("http://"), true);

    QString url1 = odm.getWhazzupVoiceUrlFromStatus();
    qInfo() << "url1" << url1;
    QCOMPARE(url1.startsWith("http://"), true);
  }
}

void OnlineTest::testOpenWhazzupVatsim()
{
  OnlinedataManager odm(dbVatsim, true);
  odm.initQueries();
  odm.readFromWhazzup(atools::strFromFile("testdata/vatsim-whazzup.txt"), atools::fs::online::VATSIM,
                      QDateTime(QDate(2018, 3, 22), QTime(16, 0, 00)));
  dbVatsim->commit();

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 2);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2018, 3, 22), QTime(17, 0, 14)));

  QDebug dbg(qDebug());
  SqlUtil(dbVatsim3).printTableStats(dbg, {"client", "atc", "server"}, true);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("client"), 543); // 586
  QCOMPARE(SqlUtil(dbVatsim).rowCount("atc"), 65);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("server"), 41);
}

void OnlineTest::testOpenWhazzupVatsim3()
{
  OnlinedataManager odm(dbVatsim3, true);
  odm.initQueries();
  odm.readFromTransceivers(atools::strFromFile("testdata/transceivers-data-fmt.json"));
  odm.readFromWhazzup(atools::strFromFile("testdata/vatsim-data-fmt.json"),
                      atools::fs::online::VATSIM_JSON3,
                      QDateTime(QDate(2018, 3, 22), QTime(16, 0, 00)));
  dbVatsim3->commit();

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 1);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2021, 3, 20), QTime(13, 0, 4, 940), Qt::UTC)); // 20180322170014

  QDebug dbg(qDebug());
  SqlUtil(dbVatsim3).printTableStats(dbg, {"client", "atc", "server"}, true);
  QCOMPARE(SqlUtil(dbVatsim3).rowCount("client"), 1470);
  QCOMPARE(SqlUtil(dbVatsim3).rowCount("atc"), 155);
  QCOMPARE(SqlUtil(dbVatsim3).rowCount("server"), 7);
}

void OnlineTest::testOpenWhazzupIvao()
{
  OnlinedataManager odm(dbIvao, true);
  odm.initQueries();
  odm.readFromWhazzup(atools::strFromFile("testdata/ivao-whazzup.txt"), atools::fs::online::IVAO,
                      QDateTime(QDate(2018, 3, 21), QTime(14, 0, 0)));

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 1);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2018, 3, 21), QTime(15, 54, 54))); // 20180321155454

  QDebug dbg(qDebug());
  SqlUtil(dbVatsim3).printTableStats(dbg, {"client", "atc", "server"}, true);
  QCOMPARE(SqlUtil(dbIvao).rowCount("client"), 599); // 691
  QCOMPARE(SqlUtil(dbIvao).rowCount("atc"), 92);
  QCOMPARE(SqlUtil(dbIvao).rowCount("server"), 6);
}

void OnlineTest::testOpenWhazzupIvao2()
{
  OnlinedataManager odm(dbIvao2, true);
  odm.initQueries();
  odm.readFromWhazzup(atools::strFromFile("testdata/ivao-data-fmt.json"),
                      atools::fs::online::IVAO_JSON2,
                      QDateTime(QDate(2018, 3, 22), QTime(16, 0, 00)));
  dbIvao2->commit();

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 0);
  // "updatedAt": "2021-06-20T21:09:19.642Z",
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2021, 6, 20), QTime(21, 9, 19, 642), Qt::UTC));  // 20180322170014

  QDebug dbg(qDebug());
  SqlUtil(dbIvao2).printTableStats(dbg, {"client", "atc", "server"}, true);
  QCOMPARE(SqlUtil(dbIvao2).rowCount("client"), 540);
  QCOMPARE(SqlUtil(dbIvao2).rowCount("atc"), 80);
  QCOMPARE(SqlUtil(dbIvao2).rowCount("server"), 8);
}

void OnlineTest::testOpenServersVatsim()
{
  OnlinedataManager odm(dbVatsim, true);
  odm.initQueries();
  odm.readFromWhazzup(atools::strFromFile(
                        "testdata/vatsim-servers.txt"), atools::fs::online::IVAO,
                      QDateTime(QDate(2018, 3, 22), QTime(16, 0, 00)));

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 2);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("server"), 41);
}

void OnlineTest::testOpenServersIvao()
{
  OnlinedataManager odm(dbIvao, true);
  odm.initQueries();
  odm.readFromWhazzup(atools::strFromFile(
                        "testdata/ivao-servers.txt"), atools::fs::online::IVAO,
                      QDateTime(QDate(2018, 3, 21), QTime(14, 0, 0)));

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 15);
  QCOMPARE(SqlUtil(dbIvao).rowCount("server"), 12);
}

void OnlineTest::testOpenWhazzupCustom()
{
  OnlinedataManager odm(dbCustom, true);
  odm.initQueries();
  odm.readFromWhazzup(atools::strFromFile(
                        "testdata/custom-whazzup.txt"), atools::fs::online::VATSIM,
                      QDateTime(QDate(2017, 9, 26), QTime(20, 0, 0)));

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 1);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2017, 9, 26), QTime(20, 12, 32))); // 20170926201232

  QDebug dbg(qDebug());
  SqlUtil(dbVatsim3).printTableStats(dbg, {"client", "atc", "server"}, true);
  QCOMPARE(SqlUtil(dbCustom).rowCount("client"), 10);
  QCOMPARE(SqlUtil(dbCustom).rowCount("atc"), 1);
  QCOMPARE(SqlUtil(dbCustom).rowCount("server"), 1);
}

void OnlineTest::testDropSchemaVatsim()
{
#ifdef TEST_DROP_ONLINE_SCHEMA
  OnlineDataManager odm(dbVatsim);
  odm.deInitQueries();
  odm.dropSchema();
  QCOMPARE(odm.hasSchema(), false);
  QCOMPARE(odm.hasData(), false);
#endif
}

void OnlineTest::testDropSchemaIvao()
{
#ifdef TEST_DROP_ONLINE_SCHEMA
  OnlineDataManager odm(dbIvao);
  odm.deInitQueries();
  odm.dropSchema();
  QCOMPARE(odm.hasSchema(), false);
  QCOMPARE(odm.hasData(), false);
#endif
}

void OnlineTest::testDropSchemaCustom()
{
#ifdef TEST_DROP_ONLINE_SCHEMA
  OnlineDataManager odm(dbCustom);
  odm.deInitQueries();
  odm.dropSchema();
  QCOMPARE(odm.hasSchema(), false);
  QCOMPARE(odm.hasData(), false);
#endif
}

void OnlineTest::testVatsimJson3Download()
{
  atools::util::HttpDownloader downloader(this, true);
  downloader.setUrl("https://data.vatsim.net/v3/vatsim-data.json");

  bool errorFlag = false, finished = false;

  downloader.startDownload();

  connect(&downloader, &atools::util::HttpDownloader::downloadFinished,
          [&finished, &errorFlag](const QByteArray& data, QString downloadUrl) -> void
  {
    qDebug() << Q_FUNC_INFO << data.size() << downloadUrl;
    errorFlag = false;
    finished = true;
  });

  connect(&downloader, &atools::util::HttpDownloader::downloadFailed,
          [&errorFlag, &finished](const QString& error, int errorCode, QString downloadUrl) -> void
  {
    qDebug() << Q_FUNC_INFO << error << errorCode << downloadUrl;
    errorFlag = true;
    finished = true;
  });

  testutil::waitForValue(finished, 180);

  qDebug() << Q_FUNC_INFO << "errorFlag" << errorFlag << "size" << downloader.getData().size();

  QVERIFY(!atools::zip::isGzipCompressed(downloader.getData()));
  QVERIFY(QString(downloader.getData()).startsWith("{"));
  QVERIFY(downloader.getData().size() > 100);
  QVERIFY(!errorFlag);
  QVERIFY(finished);

  errorFlag = finished = false;
  downloader.setAcceptEncoding("gzip");

  downloader.startDownload();

  testutil::waitForValue(finished, 180);

  qDebug() << Q_FUNC_INFO << "errorFlag" << errorFlag << "size" << downloader.getData().size();

  QVERIFY(atools::zip::isGzipCompressed(downloader.getData()));
  QVERIFY(!QString(downloader.getData()).startsWith("{"));
  QVERIFY(downloader.getData().size() > 100);
  QVERIFY(!errorFlag);
  QVERIFY(finished);
}
