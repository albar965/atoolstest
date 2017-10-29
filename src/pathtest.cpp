/*****************************************************************************
* Copyright 2015-2017 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more detaiaclocal.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "pathtest.h"

#include "atools.h"

PathTest::PathTest()
{

}

void PathTest::initTestCase()
{
}

void PathTest::cleanupTestCase()
{
}

void PathTest::testPath_data()
{
  QTest::addColumn<QStringList>("path");
  QTest::addColumn<QString>("result");

  QTest::newRow("Existing correct case") << QStringList({"/usr", "bin", "aclocal"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case") << QStringList({"/usr", "BIN", "aclocal"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case 2") << QStringList({"/usr", "BIN", "ACLOCAL"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case 3") << QStringList({"/usr", "bin", "ACLOCAL"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case 4") << QStringList({"/", "USR", "BIN", "ACLOCAL"}) << "/usr/bin/aclocal";

  QTest::newRow("Not Existing correct case") << QStringList({"/usr", "bin", "aclocalx"}) << "/usr/bin/aclocalx";
  QTest::newRow("Not Existing wrong case") << QStringList({"/usr", "BIN", "aclocalx"}) << "/usr/bin/aclocalx";
  QTest::newRow("Not Existing wrong case 2") << QStringList({"/usr", "BIN", "ACLOCALX"}) << "/usr/bin/ACLOCALX";
  QTest::newRow("Not Existing wrong case 3") << QStringList({"/usr", "bin", "ACLOCALX"}) << "/usr/bin/ACLOCALX";
  QTest::newRow("Not Existing wrong case 4") << QStringList({"/", "USR", "BIN", "ACLOCALX"}) << "/usr/bin/ACLOCALX";

  QTest::newRow("Not Existing Path correct case") << QStringList({"/usr", "binx", "aclocalx"}) << "/usr/binx/aclocalx";
  QTest::newRow("Not Existing Path wrong case") << QStringList({"/usr", "BINX", "aclocalx"}) << "/usr/BINX/aclocalx";
  QTest::newRow("Not Existing Path wrong case 2") << QStringList({"/usr", "BINX", "ACLOCALX"}) << "/usr/BINX/ACLOCALX";
  QTest::newRow("Not Existing Path wrong case 3") <<
  QStringList({"/", "USR", "BINX", "ACLOCALX"}) << "/usr/BINX/ACLOCALX";
}

void PathTest::testPath()
{
  QFETCH(QStringList, path);
  QFETCH(QString, result);

  QCOMPARE(atools::buildPathNoCase(path), result);
}
