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

#include "airspacetest.h"

#include "fs/userdata/airspacereaderopenair.h"
#include "testutil/testutil.h"

#include "sql/sqldatabase.h"
#include "sql/sqlutil.h"
#include "sql/sqlscript.h"

using atools::sql::SqlDatabase;
using atools::sql::SqlUtil;
using atools::sql::SqlScript;

AirspaceTest::AirspaceTest()
{

}

void AirspaceTest::runtest(int argc, char *argv[])
{
  AirspaceTest tst;
  QTest::qExec(&tst, argc, argv);
}

void AirspaceTest::initTestCase()
{
  db = testutil::createDb("TESTDBAIRSPACE", "online_test_airspaces.sqlite");

  SqlScript script(db, true /* options->isVerbose()*/);
  script.executeScript(":/atools/resources/sql/fs/db/drop_meta.sql");
  script.executeScript(":/atools/resources/sql/fs/db/drop_nav.sql");
  script.executeScript(":/atools/resources/sql/fs/db/create_boundary_schema.sql");
  script.executeScript(":/atools/resources/sql/fs/db/create_meta_schema.sql");

  db->commit();
}

void AirspaceTest::cleanupTestCase()
{
  testutil::removeDb(db, "TESTDBAIRSPACE");
}

void AirspaceTest::testLoadOpenAir()
{
  atools::fs::userdata::AirspaceReaderOpenAir writer(db);
  writer.readFile(1, ":/test/resources/airspace.txt");

  SqlUtil util(db);
  QCOMPARE(util.hasTableAndRows("boundary"), true);
  QCOMPARE(util.rowCount("boundary"), 3151);
  QCOMPARE(writer.getErrors().size(), 5);
  QCOMPARE(writer.getNumAirspacesRead(), 3151);
}
