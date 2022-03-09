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
#include "fs/userdata/airspacereaderivao.h"
#include "fs/userdata/airspacereadervatsim.h"
#include "testutil/testutil.h"

#include "sql/sqldatabase.h"
#include "sql/sqlutil.h"
#include "sql/sqlscript.h"

using atools::sql::SqlDatabase;
using atools::sql::SqlUtil;
using atools::sql::SqlScript;
using atools::fs::userdata::AirspaceReaderBase;

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

  QFile file("testdata/airports.csv");
  if(file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream stream(&file);

    while(!stream.atEnd())
    {
      QStringList line = stream.readLine().simplified().split(',');
      airports.insert(line.value(0), atools::geo::Pos(line.value(1).toFloat(), line.value(2).toFloat()));
    }
    file.close();
  }
}

void AirspaceTest::cleanupTestCase()
{
  // testutil::removeDb(db, "TESTDBAIRSPACE");
}

void AirspaceTest::testLoadGeoJsonTracon()
{
  QCOMPARE(AirspaceReaderBase::detectFileFormat("testdata/vatsim_traconboundaries_fmt.json"), AirspaceReaderBase::VATSIM_GEO_JSON);

  atools::fs::userdata::AirspaceReaderVatsim reader(db);
  reader.setFetchAirportCoords(std::bind(&AirspaceTest::fetchAirportCoordinates, this, std::placeholders::_1));
  reader.setFileId(1);
  reader.setAirspaceId(10000);
  reader.readFile("testdata/vatsim_traconboundaries_fmt.json");
  db->commit();

  SqlUtil util(db);
  QCOMPARE(util.hasTableAndRows("boundary"), true);
  QCOMPARE(util.rowCount("boundary", "file_id = 1"), 346);
  QCOMPARE(reader.getErrors().size(), 0);
  QCOMPARE(reader.getNumAirspacesRead(), 346);
}

void AirspaceTest::testLoadGeoJsonFir()
{
  QCOMPARE(AirspaceReaderBase::detectFileFormat("testdata/vatsim_firboundaries_fmt.json"), AirspaceReaderBase::VATSIM_GEO_JSON);

  atools::fs::userdata::AirspaceReaderVatsim reader(db);
  reader.setFetchAirportCoords(std::bind(&AirspaceTest::fetchAirportCoordinates, this, std::placeholders::_1));
  reader.setFileId(2);
  reader.setAirspaceId(20000);
  reader.readFile("testdata/vatsim_firboundaries_fmt.json");
  db->commit();

  SqlUtil util(db);
  QCOMPARE(util.hasTableAndRows("boundary"), true);
  QCOMPARE(util.rowCount("boundary", "file_id = 2"), 538);
  QCOMPARE(reader.getErrors().size(), 0);
  QCOMPARE(reader.getNumAirspacesRead(), 538);
}

void AirspaceTest::testLoadIvaoJson()
{
  QCOMPARE(AirspaceReaderBase::detectFileFormat("testdata/ivao_atc_positions_fmt.json"), AirspaceReaderBase::IVAO_JSON);

  atools::fs::userdata::AirspaceReaderIvao reader(db);
  reader.setFetchAirportCoords(std::bind(&AirspaceTest::fetchAirportCoordinates, this, std::placeholders::_1));
  reader.setFileId(3);
  reader.setAirspaceId(30000);
  reader.readFile("testdata/ivao_atc_positions_fmt.json");
  db->commit();

  SqlUtil util(db);
  QCOMPARE(util.hasTableAndRows("boundary"), true);
  QCOMPARE(util.rowCount("boundary", "file_id = 3"), 695);
  QCOMPARE(reader.getErrors().size(), 0);
  QCOMPARE(reader.getNumAirspacesRead(), 695);
}

void AirspaceTest::testLoadOpenAir()
{
  QCOMPARE(AirspaceReaderBase::detectFileFormat("testdata/airspace.txt"), AirspaceReaderBase::OPEN_AIR);

  atools::fs::userdata::AirspaceReaderOpenAir reader(db);
  reader.setFileId(4);
  reader.setAirspaceId(40000);
  reader.readFile("testdata/airspace.txt");
  db->commit();

  SqlUtil util(db);
  QCOMPARE(util.hasTableAndRows("boundary"), true);
  QCOMPARE(util.rowCount("boundary", "file_id = 4"), 3151);
  QCOMPARE(reader.getErrors().size(), 5);
  QCOMPARE(reader.getNumAirspacesRead(), 3151);
}

atools::geo::Pos AirspaceTest::fetchAirportCoordinates(const QString& airportIdent)
{
  return airports.value(airportIdent, atools::geo::EMPTY_POS);
}
