/*****************************************************************************
* Copyright 2015-2026 Alexander Barthel alex@littlenavmap.org
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

#include "updatetest.h"

#include "util/updatecheck.h"
#include "testutil/testutil.h"

/* Test file
 *
 *  [stable]
 *  version=1.4.4
 *  url=https://albar965.github.io/release/2017/07/04/2017-07-04-littlenavmap-stable-144-released.html
 *  changelog=<h1>Changes in 1.4.4</h1><nl><li>First</li><li>Second</li></nl>
 *
 *  [beta]
 *  version=1.6.0.beta
 *  url=https://albar965.github.io/release/2017/07/04/2017-07-04-littlenavmap-stable-144-released.html
 *  changelog=<h1>Changes in 1.4.4</h1><nl><li>First beta</li><li>Second</li></nl>
 *
 *  [develop]
 *  version=1.5.5.develop
 *  url=https://albar965.github.io/release/2017/07/04/2017-07-04-littlenavmap-stable-144-released.html
 *  downloadwin=https://github.com/albar965/littlenavmap/releases/download/v1.4.4/LittleNavmap-win-1.4.4.zip
 *  downloadmac=https://github.com/albar965/littlenavmap/releases/download/v1.4.4/LittleNavmap-mac-1.4.4.zip
 *  downloadlinux=https://github.com/albar965/littlenavmap/releases/download/v1.4.4/LittleNavmap-linux-1.4.4.tar.gz
 *  changelog=<h1>Changes in 1.5.5.develop</h1><nl><li>First unstable</li><li>Second</li></nl>
 *
 */

using atools::util::UpdateCheck;

static const QLatin1String VERSION_URL("https://www.littlenavmap.org/test/littlenavmap-version");

UpdateTest::UpdateTest()
{

}

void UpdateTest::runtest(int argc, char *argv[])
{
  UpdateTest tst;
  QTest::qExec(&tst, argc, argv);
}

void UpdateTest::initTestCase()
{
}

void UpdateTest::cleanupTestCase()
{
}

void UpdateTest::testUpdateStable()
{
  UpdateCheck check("1.4.3", false);
  check.setUrl(VERSION_URL);

  bool found = false;
  atools::util::Updates testUpdates;
  connect(&check, &UpdateCheck::updateFound, [&testUpdates, &found](atools::util::Updates updates) -> void
  {
    testUpdates = updates;
    found = true;
  });

  check.checkForUpdates(QString(), true, atools::util::STABLE);

  testutil::waitForValue(found, 180);

  QCOMPARE(testUpdates.size(), 1);
}

void UpdateTest::testUpdateBeta()
{
  UpdateCheck check("1.4.3", false);
  check.setUrl(VERSION_URL);

  bool found = false;
  atools::util::Updates testUpdates;
  connect(&check, &UpdateCheck::updateFound, [&testUpdates, &found](atools::util::Updates updates) -> void
  {
    testUpdates = updates;
    found = true;
  });

  check.checkForUpdates(QString(), true, atools::util::BETA);

  testutil::waitForValue(found, 30);

  QCOMPARE(testUpdates.size(), 1);
}

void UpdateTest::testUpdateDevelop()
{
  UpdateCheck check("1.4.3", false);
  check.setUrl(VERSION_URL);

  bool found = false;
  atools::util::Updates testUpdates;
  connect(&check, &UpdateCheck::updateFound, [&testUpdates, &found](atools::util::Updates updates) -> void
  {
    testUpdates = updates;
    found = true;
  });

  check.checkForUpdates(QString(), true, atools::util::DEVELOP);

  testutil::waitForValue(found, 30);

  QCOMPARE(testUpdates.size(), 1);
}

void UpdateTest::testUpdateAll()
{
  UpdateCheck check("1.4.3", false);
  check.setUrl(VERSION_URL);

  bool found = false;
  atools::util::Updates testUpdates;
  connect(&check, &UpdateCheck::updateFound, [&testUpdates, &found](atools::util::Updates updates) -> void
  {
    testUpdates = updates;
    found = true;
  });

  check.checkForUpdates({"1.6.0.beta"}, true, atools::util::STABLE | atools::util::BETA | atools::util::DEVELOP);

  testutil::waitForValue(found, 30);

  QCOMPARE(testUpdates.size(), 2);
}

void UpdateTest::testUpdateForce()
{
  UpdateCheck check("4.4.0.develop", true);
  check.setUrl(VERSION_URL);

  bool found = false;
  atools::util::Updates testUpdates;
  connect(&check, &UpdateCheck::updateFound, [&testUpdates, &found](atools::util::Updates updates) -> void
  {
    testUpdates = updates;
    found = true;
  });

  check.checkForUpdates(QString(), true, atools::util::STABLE | atools::util::BETA | atools::util::DEVELOP);

  testutil::waitForValue(found, 30);

  QCOMPARE(testUpdates.size(), 3);
}
