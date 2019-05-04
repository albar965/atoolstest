/*****************************************************************************
* Copyright 2015-2019 Alexander Barthel alex@littlenavmap.org
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

#include "versiontest.h"

#include "util/version.h"

VersionTest::VersionTest()
{

}

void VersionTest::runtest(int argc, char *argv[])
{
  VersionTest tst;
  QTest::qExec(&tst, argc, argv);
}

void VersionTest::testVersion_data()
{
  QTest::addColumn<QString>("version1");
  QTest::addColumn<QString>("version2");
  QTest::addColumn<bool>("resultsmaller");
  QTest::addColumn<bool>("resultequal");

  QTest::newRow("Equal") << "1.1.1" << "1.1.1" << false << true;
  QTest::newRow("Major smaller") << "1.1.1" << "2.1.1" << true << false;
  QTest::newRow("Minor smaller") << "1.1.1" << "1.2.1" << true << false;
  QTest::newRow("Patchlevel smaller") << "1.1.1" << "1.1.2" << true << false;

  QTest::newRow("Beta equal") << "1.1.1.beta" << "1.1.1.beta" << false << true;
  QTest::newRow("Beta major smaller") << "1.1.1.beta" << "2.1.1.beta" << true << false;
  QTest::newRow("Beta minor smaller") << "1.1.1.beta" << "1.2.1.beta" << true << false;
  QTest::newRow("Beta patchlevel smaller") << "1.1.1.beta" << "1.1.2.beta" << true << false;
  QTest::newRow("Beta subversion smaller") << "1.1.1.beta1" << "1.1.1.beta2" << true << false;

  QTest::newRow("RC equal") << "1.1.1.rc" << "1.1.1.rc" << false << true;
  QTest::newRow("RC major smaller") << "1.1.1.rc" << "2.1.1.rc" << true << false;
  QTest::newRow("RC minor smaller") << "1.1.1.rc" << "1.2.1.rc" << true << false;
  QTest::newRow("RC patchlevel smaller") << "1.1.1.rc" << "1.1.2.rc" << true << false;
  QTest::newRow("RC subversion smaller") << "1.1.1.rc1" << "1.1.1.rc2" << true << false;

  QTest::newRow("RC smaller than stable") << "1.1.1.rc1" << "1.1.1" << true << false;
  QTest::newRow("RC smaller than stable") << "1.1.1.rc" << "1.1.1" << true << false;

  QTest::newRow("Beta smaller than rc") << "1.1.1.beta" << "1.1.1.rc" << true << false;

  QTest::newRow("RC smaller than new beta") << "1.1.1.rc" << "2.1.1.beta" << true << false;
  QTest::newRow("Beta smaller than new develop") << "1.1.1.beta" << "2.1.1.develop" << true << false;
  QTest::newRow("Stable smaller than new develop") << "1.1.1" << "2.1.1.develop" << true << false;
}

void VersionTest::testVersion()
{

  QFETCH(QString, version1);
  QFETCH(QString, version2);
  QFETCH(bool, resultsmaller);
  QFETCH(bool, resultequal);

  QCOMPARE(atools::util::Version(version1) < atools::util::Version(version2), resultsmaller);
  QCOMPARE(atools::util::Version(version1) == atools::util::Version(version2), resultequal);
}
