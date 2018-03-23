/*****************************************************************************
* Copyright 2015-2018 Alexander Barthel albar965@mailbox.org
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

#include "sql/sqldatabase.h"
#include "sql/sqlutil.h"
#include "fs/online/onlinedatamanager.h"

using atools::sql::SqlDatabase;
using atools::sql::SqlUtil;
using atools::fs::online::OnlineDataManager;

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
  SqlDatabase::addDatabase("QSQLITE", "TESTDBIVAO");
  dbIvao = new SqlDatabase("TESTDBIVAO");
  dbIvao->setDatabaseName("online_test_ivao.sqlite");
  dbIvao->open();

  SqlDatabase::addDatabase("QSQLITE", "TESTDBVATSIM");
  dbVatsim = new SqlDatabase("TESTDBVATSIM");
  dbVatsim->setDatabaseName("online_test_vatsim.sqlite");
  dbVatsim->open();

  SqlDatabase::addDatabase("QSQLITE", "TESTDBCUSTOM");
  dbCustom = new SqlDatabase("TESTDBCUSTOM");
  dbCustom->setDatabaseName("online_test_custom.sqlite");
  dbCustom->open();
}

void OnlineTest::cleanupTestCase()
{
  if(dbIvao != nullptr)
  {
    dbIvao->close();
    delete dbIvao;
    dbIvao = nullptr;
  }
  if(dbVatsim != nullptr)
  {
    dbVatsim->close();
    delete dbVatsim;
    dbVatsim = nullptr;
  }
  if(dbCustom != nullptr)
  {
    dbCustom->close();
    delete dbCustom;
    dbCustom = nullptr;
  }
  SqlDatabase::removeDatabase("TESTDBIVAO");
  SqlDatabase::removeDatabase("TESTDBVATSIM");
  SqlDatabase::removeDatabase("TESTDBCUSTOM");
}

void OnlineTest::testCreateSchemaVatsim()
{
  OnlineDataManager odm(dbVatsim);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testCreateSchemaIvao()
{
  OnlineDataManager odm(dbIvao);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testCreateSchemaCustom()
{
  OnlineDataManager odm(dbCustom);
  odm.createSchema();
  odm.initQueries();
  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testOpenStatusVatsim()
{
  OnlineDataManager odm(dbVatsim);
  odm.readFromStatus(strFromFile(":/test/resources/vatsim-status.txt"));

  QCOMPARE(odm.getMessageFromStatus(), QString("VATSIM TEST message"));
  QCOMPARE(odm.getWhazzupUrlFromStatus().startsWith("http://"), true);
  QCOMPARE(odm.getWhazzupVoiceUrlFromStatus().startsWith("http://"), true);
}

void OnlineTest::testOpenStatusIvao()
{
  OnlineDataManager odm(dbIvao);
  odm.readFromStatus(strFromFile(":/test/resources/ivao-status.txt"));

  QCOMPARE(odm.getMessageFromStatus(), QString("IVAO TEST message"));
  QCOMPARE(odm.getWhazzupUrlFromStatus().startsWith("http://"), true);
  QCOMPARE(odm.getWhazzupVoiceUrlFromStatus().startsWith("http://"), true);
}

void OnlineTest::testOpenWhazzupVatsim()
{
  OnlineDataManager odm(dbVatsim);
  odm.initQueries();
  odm.readFromWhazzup(strFromFile(":/test/resources/vatsim-whazzup.txt"), atools::fs::online::VATSIM);

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 2);
  QCOMPARE(odm.getAtisAllowMinutesFromWhazzup(), 5);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2018, 3, 22), QTime(17, 0, 14)));
  QCOMPARE(SqlUtil(dbVatsim).rowCount("client"), 586);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("server"), 16);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("prefile"), 22);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("voice_server"), 25);
}

void OnlineTest::testOpenWhazzupIvao()
{
  OnlineDataManager odm(dbIvao);
  odm.initQueries();
  odm.readFromWhazzup(strFromFile(":/test/resources/ivao-whazzup.txt"), atools::fs::online::IVAO);

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 1);
  QCOMPARE(odm.getAtisAllowMinutesFromWhazzup(), 0);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2018, 3, 21), QTime(15, 54, 54)));
  QCOMPARE(SqlUtil(dbIvao).rowCount("client"), 691);
  QCOMPARE(SqlUtil(dbIvao).rowCount("server"), 6);
  QCOMPARE(SqlUtil(dbIvao).rowCount("prefile"), 0);
  QCOMPARE(SqlUtil(dbIvao).rowCount("voice_server"), 0);
}

void OnlineTest::testOpenServersVatsim()
{
  OnlineDataManager odm(dbVatsim);
  odm.initQueries();
  odm.readFromWhazzup(strFromFile(":/test/resources/vatsim-servers.txt"), atools::fs::online::IVAO);

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 2);
  QCOMPARE(odm.getAtisAllowMinutesFromWhazzup(), 5);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("client"), 586);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("server"), 32);
  QCOMPARE(SqlUtil(dbVatsim).rowCount("voice_server"), 25);
}

void OnlineTest::testOpenServersIvao()
{
  OnlineDataManager odm(dbIvao);
  odm.initQueries();
  odm.readFromWhazzup(strFromFile(":/test/resources/ivao-servers.txt"), atools::fs::online::IVAO);

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 15);
  QCOMPARE(odm.getAtisAllowMinutesFromWhazzup(), 0);
  QCOMPARE(SqlUtil(dbIvao).rowCount("client"), 691);
  QCOMPARE(SqlUtil(dbIvao).rowCount("server"), 6);
  QCOMPARE(SqlUtil(dbIvao).rowCount("voice_server"), 6);
}

void OnlineTest::testOpenWhazzupCustom()
{
  OnlineDataManager odm(dbCustom);
  odm.initQueries();
  odm.readFromWhazzup(strFromFile(":/test/resources/custom-whazzup.txt"), atools::fs::online::VATSIM);

  QCOMPARE(odm.hasSchema(), true);
  QCOMPARE(odm.hasData(), true);
  QCOMPARE(odm.getReloadMinutesFromWhazzup(), 1);
  QCOMPARE(odm.getAtisAllowMinutesFromWhazzup(), 0);
  QCOMPARE(odm.getLastUpdateTimeFromWhazzup(), QDateTime(QDate(2017, 9, 26), QTime(20, 12, 32)));
  QCOMPARE(SqlUtil(dbCustom).rowCount("client"), 11);
  QCOMPARE(SqlUtil(dbCustom).rowCount("server"), 1);
  QCOMPARE(SqlUtil(dbCustom).rowCount("prefile"), 0);
  QCOMPARE(SqlUtil(dbCustom).rowCount("voice_server"), 0);
}

void OnlineTest::testDropSchemaVatsim()
{
  OnlineDataManager odm(dbVatsim);
  odm.deInitQueries();
  odm.dropSchema();
  QCOMPARE(odm.hasSchema(), false);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testDropSchemaIvao()
{
  OnlineDataManager odm(dbIvao);
  odm.deInitQueries();
  odm.dropSchema();
  QCOMPARE(odm.hasSchema(), false);
  QCOMPARE(odm.hasData(), false);
}

void OnlineTest::testDropSchemaCustom()
{
  OnlineDataManager odm(dbCustom);
  odm.deInitQueries();
  odm.dropSchema();
  QCOMPARE(odm.hasSchema(), false);
  QCOMPARE(odm.hasData(), false);
}

QString OnlineTest::strFromFile(const QString& filename)
{
  QFile file(filename);
  if(file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream stream(&file);
    return stream.readAll();
  }
  return QString();
}
