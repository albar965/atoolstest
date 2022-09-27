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

#include "fsutiltest.h"
#include "fs/util/fsutil.h"

using namespace atools::fs::util;

FsUtilTest::FsUtilTest()
{

}

void FsUtilTest::runtest(int argc, char *argv[])
{
  FsUtilTest tst;
  QTest::qExec(&tst, argc, argv);
}

void FsUtilTest::initTestCase()
{
}

void FsUtilTest::cleanupTestCase()
{

}

void FsUtilTest::testRunwaySplit()
{
  int number;
  QString designator;
  bool trueHeading;
  runwayNameSplit("RW01T", &number, &designator, &trueHeading);
  QCOMPARE(number, 1);
  QCOMPARE(designator, QString());
  QCOMPARE(trueHeading, true);

  runwayNameSplit("RW01", &number, &designator, &trueHeading);
  QCOMPARE(number, 1);
  QCOMPARE(designator, QString());
  QCOMPARE(trueHeading, false);

  runwayNameSplit("1C", &number, &designator, &trueHeading);
  QCOMPARE(number, 1);
  QCOMPARE(designator, "C");
  QCOMPARE(trueHeading, false);
}

void FsUtilTest::testRunways_data()
{
  QTest::addColumn<bool>("fromList");
  QTest::addColumn<QString>("runway");
  QTest::addColumn<QStringList>("airportRunways");
  QTest::addColumn<QString>("result");

  QTest::newRow("04") << true << "04" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << QString();
  QTest::newRow("02") << true << "02" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "RW01";
  QTest::newRow("02T") << true << "02T" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "RW01";
  QTest::newRow("RW02") << true << "RW02" << QStringList({"1", "09", "18", "27"}) << "1";
  QTest::newRow("2") << true << "2" << QStringList({"01", "09", "18", "27"}) << "01";
  QTest::newRow("RW2") << true << "RW2" << QStringList({"01", "09", "18", "27"}) << "01";
  QTest::newRow("RW36") << true << "RW36" << QStringList({"01", "09", "18", "27"}) << "01";
  QTest::newRow("36") << true << "36" << QStringList({"1", "09", "18", "27"}) << "1";
  QTest::newRow("01C") << true << "01C" << QStringList({"RW01C", "RW09", "RW18", "RW27"}) << "RW01C";
  QTest::newRow("RW01C") << true << "RW01C" << QStringList({"1C", "RW09", "RW18", "RW27"}) << "1C";
  QTest::newRow("RW02C") << true << "RW02C" << QStringList({"1C", "RW09", "RW18", "RW27"}) << "1C";
  QTest::newRow("01") << true << "01" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "RW01";
  QTest::newRow("01T") << true << "01T" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "RW01";
  QTest::newRow("01") << true << "01" << QStringList({"RW01T", "RW09T", "RW18T", "RW27T"}) << "RW01T";
  QTest::newRow("02") << false << "02" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "01";
  QTest::newRow("02T") << false << "02T" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "01T";
  QTest::newRow("RW02") << false << "RW02" << QStringList({"1", "09", "18", "27"}) << "RW01";
  QTest::newRow("2") << false << "2" << QStringList({"01", "09", "18", "27"}) << "1";
  QTest::newRow("RW2") << false << "RW2" << QStringList({"01", "09", "18", "27"}) << "RW1";
  QTest::newRow("RW36") << false << "RW36" << QStringList({"01", "09", "18", "27"}) << "RW01";
  QTest::newRow("36") << false << "36" << QStringList({"1", "09", "18", "27"}) << "01";
  QTest::newRow("01C") << false << "01C" << QStringList({"RW01C", "RW09", "RW18", "RW27"}) << "01C";
  QTest::newRow("RW01C") << false << "RW01C" << QStringList({"1C", "RW09", "RW18", "RW27"}) << "RW01C";
  QTest::newRow("RW02C") << false << "RW02C" << QStringList({"1C", "RW09", "RW18", "RW27"}) << "RW01C";
  QTest::newRow("01") << false << "01" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "01";
  QTest::newRow("01T") << false << "01T" << QStringList({"RW01", "RW09", "RW18", "RW27"}) << "01T";
  QTest::newRow("01") << false << "01" << QStringList({"RW01T", "RW09T", "RW18T", "RW27T"}) << "01";
}

void FsUtilTest::testRunways()
{
  QFETCH(bool, fromList);
  QFETCH(QString, runway);
  QFETCH(QStringList, airportRunways);
  QFETCH(QString, result);

  if(fromList)
    QCOMPARE(runwayBestFitFromList(runway, airportRunways), result);
  else
    QCOMPARE(runwayBestFit(runway, airportRunways), result);
}

void FsUtilTest::testRunwaysEqual_data()
{
  QTest::addColumn<bool>("almost");
  QTest::addColumn<QString>("runway1");
  QTest::addColumn<QString>("runway2");
  QTest::addColumn<bool>("equal");

  QTest::newRow("05") << true << "05" << "4" << true;
  QTest::newRow("05") << true << "05" << "04" << true;
  QTest::newRow("05C") << true << "05C" << "04CT" << true;
  QTest::newRow("05C") << true << "05C" << "4CT" << true;

  QTest::newRow("5") << true << "5" << "RW4" << true;
  QTest::newRow("5") << true << "5" << "RW04" << true;
  QTest::newRow("5") << true << "5" << "RW04T" << true;
  QTest::newRow("5") << true << "5" << "RW4T" << true;

  QTest::newRow("09") << true << "09" << "4" << false;
  QTest::newRow("09") << true << "09" << "04" << false;
  QTest::newRow("09") << true << "09" << "04T" << false;
  QTest::newRow("09") << true << "09" << "4T" << false;

  QTest::newRow("9") << true << "9" << "RW4" << false;
  QTest::newRow("9") << true << "9" << "RW04" << false;
  QTest::newRow("9") << true << "9" << "RW04T" << false;
  QTest::newRow("9") << true << "9" << "RW4T" << false;

  QTest::newRow("04") << false << "04" << "4" << true;
  QTest::newRow("04") << false << "04" << "04" << true;
  QTest::newRow("04R") << false << "04R" << "04RT" << true;
  QTest::newRow("04R") << false << "04R" << "4RT" << true;

  QTest::newRow("4") << false << "4" << "RW4" << true;
  QTest::newRow("4") << false << "4" << "RW04" << true;
  QTest::newRow("4") << false << "4" << "RW04T" << true;
  QTest::newRow("4") << false << "4" << "RW4T" << true;
  QTest::newRow("4C") << false << "4C" << "RW4CT" << true;
  QTest::newRow("04C") << false << "04C" << "RW04CT" << true;

  QTest::newRow("09") << false << "09" << "4" << false;
  QTest::newRow("09") << false << "09" << "04" << false;
  QTest::newRow("09") << false << "09" << "04T" << false;
  QTest::newRow("09") << false << "09" << "4T" << false;

  QTest::newRow("9") << false << "9" << "RW4" << false;
  QTest::newRow("9") << false << "9" << "RW04" << false;
  QTest::newRow("9") << false << "9" << "RW04T" << false;
  QTest::newRow("9") << false << "9" << "RW4T" << false;
  QTest::newRow("9R") << false << "9R" << "RW9C" << false;
}

void FsUtilTest::testRunwaysEqual()
{
  QFETCH(bool, almost);
  QFETCH(QString, runway1);
  QFETCH(QString, runway2);
  QFETCH(bool, equal);

  if(almost)
    QCOMPARE(runwayAlmostEqual(runway1, runway2), equal);
  else
    QCOMPARE(runwayEqual(runway1, runway2), equal);
}

void FsUtilTest::testXpWaypointFlags()
{
  QCOMPARE(atools::fs::util::waypointFlagsToXplane(""), "");
  QCOMPARE(atools::fs::util::waypointFlagsToXplane("XXXX"), "");
  QCOMPARE(atools::fs::util::waypointFlagsToXplane("V  "), "2105430");
  QCOMPARE(atools::fs::util::waypointFlagsToXplane("RUD"), "4478290");
  QCOMPARE(atools::fs::util::waypointFlagsToXplane("W D"), "4464727");
  QCOMPARE(atools::fs::util::waypointFlagsFromXplane(""), "");
  QCOMPARE(atools::fs::util::waypointFlagsFromXplane("ABCD"), "");
  QCOMPARE(atools::fs::util::waypointFlagsFromXplane("2105430"), "V  ");
  QCOMPARE(atools::fs::util::waypointFlagsFromXplane("4464727"), "W D");
}
