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

#include "dbtest.h"

#include "testutil/testutil.h"
#include "fs/userdata/userdatamanager.h"
#include "sql/datamanagerbase.h"
#include "sql/sqldatabase.h"
#include "sql/sqlutil.h"
#include "sql/sqltransaction.h"
#include "sql/sqlrecord.h"
#include "atools.h"

using atools::sql::SqlDatabase;
using atools::sql::SqlUtil;
using atools::sql::SqlTransaction;
using atools::sql::DataManagerBase;

DbTest::DbTest()
{

}

void DbTest::runtest(int argc, char *argv[])
{
  DbTest tst;
  QTest::qExec(&tst, argc, argv);
}

void DbTest::initTestCase()
{
  db = testutil::createDb("TESTDBMANUSER", "test_user.sqlite");
  atools::fs::userdata::UserdataManager userdata(db);
  userdata.dropSchema();
  userdata.createSchema();
  userdata.setUndoActive(false);
  userdata.importCsv("testdata/dbmanagertest.csv");

  QCOMPARE(userdata.hasSchema(), true);
  QCOMPARE(userdata.hasUndoSchema(), true);
  QCOMPARE(userdata.hasData(), true);
  QCOMPARE(userdata.rowCount(), 54);

  dbUndo = testutil::createDb("TESTDBMANUSERUNDO", "test_user_undo.sqlite");
  atools::fs::userdata::UserdataManager userdataUndo(dbUndo);
  userdataUndo.dropSchema();
  userdataUndo.createSchema();

  userdataUndo.setUndoActive(false);
  userdataUndo.importCsv("testdata/dbmanagertest.csv"); // Import without undo
  userdataUndo.setUndoActive(true);

  QCOMPARE(userdataUndo.hasSchema(), true);
  QCOMPARE(userdataUndo.hasUndoSchema(), true);
  QCOMPARE(userdataUndo.hasData(), true);
  QCOMPARE(userdataUndo.rowCount(), 54);

  dbUndoBulk = testutil::createDb("TESTDBMANUSERUNDOBULK", "test_user_undo_bulk.sqlite");
  atools::fs::userdata::UserdataManager userdataUndoBulk(dbUndoBulk);
  userdataUndoBulk.dropSchema();
  userdataUndoBulk.createSchema();
  QCOMPARE(userdataUndoBulk.hasSchema(), true);
  QCOMPARE(userdataUndoBulk.hasUndoSchema(), true);
  QCOMPARE(userdataUndoBulk.rowCount(), 0);
}

void DbTest::cleanupTestCase()
{
#ifndef DEBUG_TEST_KEEP_DATABASE
  atools::fs::userdata::UserdataManager userdata(db);
  userdata.dropSchema();
  QCOMPARE(userdata.hasSchema(), false);
  QCOMPARE(userdata.hasData(), false);
  QCOMPARE(userdata.rowCount(), 0);

  atools::fs::userdata::UserdataManager userdataUndo(dbUndo);
  userdataUndo.dropUndoSchema();
  userdataUndo.dropSchema();
  QCOMPARE(userdataUndo.hasSchema(), false);
  QCOMPARE(userdataUndo.hasData(), false);
  QCOMPARE(userdataUndo.rowCount(), 0);
#endif

  testutil::removeDb(db, "TESTDBMANUSER");
  testutil::removeDb(dbUndo, "TESTDBMANUSERUNDO");
}

// CREATE TABLE userdata (
// userdata_id         INTEGER        PRIMARY KEY,
// type                VARCHAR (10)   COLLATE nocase,
// name                VARCHAR (200)  COLLATE nocase,
// ident               VARCHAR (10)   COLLATE nocase,
// region              VARCHAR (10)   COLLATE nocase,
// description         VARCHAR (1024) COLLATE nocase,
// tags                VARCHAR (1024) COLLATE nocase,
// last_edit_timestamp VARCHAR (100)  NOT NULL,
// import_file_path    VARCHAR (512),
// visible_from        INTEGER,
// altitude            INTEGER,
// [temp]              INTEGER,
// lonx                DOUBLE         NOT NULL,
// laty                DOUBLE         NOT NULL
// );

void DbTest::testInsert()
{
  atools::fs::userdata::UserdataManager userdata(db);
  {
    atools::sql::SqlRecord rec = userdata.getEmptyRecord();
    rec.setValue("name", "TESTINSERT");
    rec.setValue("lonx", 4.9f);
    rec.setValue("laty", 49.f);
    rec.setValue("last_edit_timestamp", QDateTime::currentDateTime());

    SqlTransaction transaction(db);
    userdata.insertOneRecord(rec);
    transaction.commit();
  }

  QCOMPARE(userdata.getRecord(userdata.getCurrentId()).value("name").toString(), "TESTINSERT");
  QCOMPARE(userdata.rowCount(), 55);
}

void DbTest::testInsertId()
{
  atools::fs::userdata::UserdataManager userdata(db);

  {
    atools::sql::SqlRecord rec = userdata.getEmptyRecord();

    rec.setValue("name", "TESTINSERTID");
    rec.setValue("lonx", 4.9f);
    rec.setValue("laty", 49.f);
    rec.setValue("last_edit_timestamp", QDateTime::currentDateTime());

    SqlTransaction transaction(db);
    userdata.insertOneRecord(rec, 999);
    transaction.commit();
  }

  QCOMPARE(userdata.getRecord(999).value("name").toString(), "TESTINSERTID");
  QCOMPARE(userdata.rowCount(), 56);
}

void DbTest::testUpdate()
{
  atools::fs::userdata::UserdataManager userdata(db);

  {
    atools::sql::SqlRecord rec = userdata.getRecord(54);
    QCOMPARE(rec.valueStr("name"), "Washington State Fair");

    rec.setValue("name", "TESTUPDATE");

    SqlTransaction transaction(db);
    userdata.updateOneRecord(rec);
    transaction.commit();
  }

  QCOMPARE(userdata.getRecord(54).value("name").toString(), "TESTUPDATE");
  QCOMPARE(userdata.rowCount(), 56);
}

void DbTest::testDelete()
{
  atools::fs::userdata::UserdataManager userdata(db);

  {
    SqlTransaction transaction(db);
    userdata.deleteOneRow(53);
    transaction.commit();
  }

  QCOMPARE(userdata.hasRecord(53), false);
  QCOMPARE(userdata.rowCount(), 55);
}

void DbTest::testInsertUndo()
{
  atools::fs::userdata::UserdataManager userdata(dbUndo);

  int id = userdata.getCurrentId();
  {
    atools::sql::SqlRecord rec1 = userdata.getEmptyRecord();
    rec1.setValue("name", "TESTINSERTUNDO1");
    rec1.setValue("lonx", 4.9f);
    rec1.setValue("laty", 49.f);
    rec1.setValue("last_edit_timestamp", QDateTime::currentDateTime());

    atools::sql::SqlRecord rec2 = userdata.getEmptyRecord();
    rec2.setValue("name", "TESTINSERTUNDO2");
    rec2.setValue("lonx", 4.9f);
    rec2.setValue("laty", 49.f);
    rec2.setValue("last_edit_timestamp", QDateTime::currentDateTime());

    SqlTransaction transaction(dbUndo);
    userdata.insertRecords({rec1, rec2});
    transaction.commit();
  }

  QCOMPARE(userdata.getRecord(id + 1).value("name").toString(), "TESTINSERTUNDO1");
  QCOMPARE(userdata.getRecord(id + 2).value("name").toString(), "TESTINSERTUNDO2");
  QCOMPARE(userdata.rowCount(), 56);
}

void DbTest::testInsertUndoId()
{
  atools::fs::userdata::UserdataManager userdata(dbUndo);

  {
    atools::sql::SqlRecord rec = userdata.getEmptyRecord();

    rec.setValue("name", "TESTINSERTUNDOID");
    rec.setValue("lonx", 4.9f);
    rec.setValue("laty", 49.f);
    rec.setValue("last_edit_timestamp", QDateTime::currentDateTime());

    SqlTransaction transaction(dbUndo);
    userdata.insertOneRecord(rec, 9999);
    transaction.commit();
  }

  QCOMPARE(userdata.getRecord(9999).value("name").toString(), "TESTINSERTUNDOID");
  QCOMPARE(userdata.rowCount(), 57);

  userdata.undo();
  QCOMPARE(userdata.hasRecord(9999), false);
  userdata.redo();
  QCOMPARE(userdata.getRecord(9999).value("name").toString(), "TESTINSERTUNDOID");
}

void DbTest::testUpdateUndo()
{
  atools::fs::userdata::UserdataManager userdata(dbUndo);

  {
    atools::sql::SqlRecord rec = userdata.getRecord(52);
    QCOMPARE(rec.valueStr("name"), "Virginia Mason Athletic Center");

    rec.setValue("name", "TESTUPDATEUNDO");

    SqlTransaction transaction(dbUndo);
    userdata.updateOneRecord(rec);
    transaction.commit();
  }

  QCOMPARE(userdata.getRecord(52).value("name").toString(), "TESTUPDATEUNDO");
  QCOMPARE(userdata.rowCount(), 57);

  userdata.undo();
  QCOMPARE(userdata.getRecord(52).valueStr("name"), "Virginia Mason Athletic Center");
  userdata.redo();
  QCOMPARE(userdata.getRecord(52).value("name").toString(), "TESTUPDATEUNDO");
}

void DbTest::testDeleteUndo()
{
  atools::fs::userdata::UserdataManager userdata(dbUndo);

  {
    SqlTransaction transaction(dbUndo);
    userdata.deleteRows({45, 46, 47});
    transaction.commit();
  }

  QCOMPARE(userdata.hasRecord(45), false);
  QCOMPARE(userdata.hasRecord(46), false);
  QCOMPARE(userdata.hasRecord(47), false);
  QCOMPARE(userdata.rowCount(), 54);

  userdata.undo();
  QCOMPARE(userdata.hasRecord(45), true);
  QCOMPARE(userdata.hasRecord(46), true);
  QCOMPARE(userdata.hasRecord(47), true);

  userdata.redo();
  QCOMPARE(userdata.hasRecord(45), false);
  QCOMPARE(userdata.hasRecord(46), false);
  QCOMPARE(userdata.hasRecord(47), false);
}

void DbTest::testUndoDeleteAll()
{
  atools::fs::userdata::UserdataManager userdata(dbUndo);

  {
    SqlTransaction transaction(dbUndo);
    userdata.deleteAllRows();
    transaction.commit();
  }

  QCOMPARE(userdata.rowCount(), 0);
}

void DbTest::testUndoBulkInsert()
{
  atools::fs::userdata::UserdataManager userdata(dbUndo);

  userdata.importCsv("testdata/dbmanagertest2.csv");

  QCOMPARE(userdata.rowCount(), 50);
}

void DbTest::testExecUndoRedo()
{
  qDebug() << Q_FUNC_INFO << "UNDO ======================================================================";
  atools::fs::userdata::UserdataManager data(dbUndo);
  data.setTextSuffix("TEST", "TESTS");
  {
    SqlTransaction transaction(dbUndo);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Bulk insert 6 "History"
    QCOMPARE(data.rowCount(), 50);
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);
    data.undo();
    QCOMPARE(data.rowCount(), 0);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Delete all 5
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.undo();
    QCOMPARE(data.rowCount(), 54);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Three deletes 4
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.undo();
    QCOMPARE(data.rowCount(), 57);
    QCOMPARE(data.getRecord(45).value("name").toString(), "Snoqualmie Falls");
    QCOMPARE(data.getRecord(46).value("name").toString(), "Mount Si");
    QCOMPARE(data.getRecord(47).value("name").toString(), "Rattlesnake Ridge");

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // One update 3
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.undo();
    QCOMPARE(data.rowCount(), 57);
    QCOMPARE(data.getRecord(52).value("name").toString(), "Virginia Mason Athletic Center");

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // One insert with given id 2
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.undo();
    QCOMPARE(data.rowCount(), 56);
    QCOMPARE(data.hasRecord(9999), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Two inserts with auto id 1
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.undo();
    QCOMPARE(data.rowCount(), 54);
    QCOMPARE(data.hasRecord(55), false);
    QCOMPARE(data.hasRecord(56), false);

    QCOMPARE(data.canUndo(), false);
    QCOMPARE(data.canRedo(), true);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    transaction.commit();
  }

  qDebug() << Q_FUNC_INFO << "REDO ======================================================================";
  {
    SqlTransaction transaction(dbUndo);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Two inserts 1
    QCOMPARE(data.canUndo(), false);
    QCOMPARE(data.canRedo(), true);
    data.redo();
    QCOMPARE(data.rowCount(), 56);
    QCOMPARE(data.getRecord(55).value("name").toString(), "TESTINSERTUNDO1");
    QCOMPARE(data.getRecord(56).value("name").toString(), "TESTINSERTUNDO2");

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // One insert 2
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.redo();
    QCOMPARE(data.rowCount(), 57);
    QCOMPARE(data.getRecord(9999).value("name").toString(), "TESTINSERTUNDOID");

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // One update 3
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.redo();
    QCOMPARE(data.rowCount(), 57);
    QCOMPARE(data.getRecord(52).value("name").toString(), "TESTUPDATEUNDO");

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Three deletes 4
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.redo();
    QCOMPARE(data.rowCount(), 54);
    QCOMPARE(data.hasRecord(45), false);
    QCOMPARE(data.hasRecord(46), false);
    QCOMPARE(data.hasRecord(47), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Delete all 5
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.redo();
    QCOMPARE(data.rowCount(), 0);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    // Bulk insert 6
    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);
    data.redo();
    QCOMPARE(data.rowCount(), 50);

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    transaction.commit();
  }

  qDebug() << Q_FUNC_INFO << "Truncate ======================================================================";
  {
    SqlTransaction transaction(dbUndo);

    data.undo(); // Bulk insert 6
    data.undo(); // Delete all 5
    data.undo(); // Three deletes 4

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), true);

    atools::sql::SqlRecord rec = data.getRecord(52);
    rec.setValue("name", "TESTUPDATEUNDOTRUNCATE");
    data.updateOneRecord(rec);

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    rec = data.getRecord(53);
    rec.setValue("name", "TESTUPDATEUNDOTRUNCATE2");
    data.updateOneRecord(rec);

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    rec = data.getRecord(53);
    rec.setValue("name", "TESTUPDATEUNDOTRUNCATE3");
    data.updateOneRecord(rec);

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    rec = data.getRecord(53);
    rec.setValue("name", "TESTUPDATEUNDOTRUNCATE4");
    data.updateOneRecord(rec);

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    while(data.canUndo())
      data.undo();

    QCOMPARE(data.canUndo(), false);
    QCOMPARE(data.canRedo(), true);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();

    while(data.canRedo())
      data.redo();

    QCOMPARE(data.canUndo(), true);
    QCOMPARE(data.canRedo(), false);

    qDebug() << Q_FUNC_INFO << "UNDO" << data.undoStepName();
    qDebug() << Q_FUNC_INFO << "REDO" << data.redoStepName();
    transaction.commit();
  }
}

void DbTest::testExecUndoRedoBulk()
{
  qDebug() << Q_FUNC_INFO << "UNDO BULK ======================================================================";
  atools::fs::userdata::UserdataManager data(dbUndoBulk);
  data.setTextSuffix("BULK TEST", "BULK TESTS");

  qDebug() << Q_FUNC_INFO << "Truncate ======================================================================";
  {
    SqlTransaction transaction(dbUndoBulk);

    data.setMaximumUndoSteps(50);

    for(int i = 0; i < 100; i++)
    {
      atools::sql::SqlRecordList records;
      for(int j = 0; j < 10; j++)
      {
        atools::sql::SqlRecord rec = data.getEmptyRecord();
        rec.setValue("name", "TESTINSERTUNDOBULK_" + QString::number(i + 1) + "_" + QString::number(j + 1));
        rec.setValue("lonx", 4.9f + i);
        rec.setValue("laty", 49.f + j);
        rec.setValue("last_edit_timestamp", QDateTime::currentDateTime());
        records.append(rec);
      }

      data.insertRecords(records);
    }

    int steps = 0;
    while(data.canUndo())
    {
      steps++;
      data.undo();
    }
    QCOMPARE(steps, 60);

    transaction.commit();
  }
}
