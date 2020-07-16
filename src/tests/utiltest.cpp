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

#include "utiltest.h"

UtilTest::UtilTest()
{

}

void UtilTest::runtest(int argc, char *argv[])
{
  UtilTest tst;
  QTest::qExec(&tst, argc, argv);
}

void UtilTest::initTestCase()
{
}

void UtilTest::cleanupTestCase()
{

}

void UtilTest::testFlags()
{
  TestEnums test(TestEnum::NONE);
  QCOMPARE(test, TestEnum::NONE);

  test |= TestEnum::ONE64;
  QCOMPARE(static_cast<TestEnum>(test), TestEnum::ONE64);
  QVERIFY(test.testFlag(TestEnum::ONE64));

  test &= TestEnum::FOUR64;
  QVERIFY(static_cast<TestEnum>(test) != TestEnum::ONE64);
  QVERIFY(!test.testFlag(TestEnum::ONE64));

  TestEnums testList2({TestEnum::NONE, TestEnum::FOUR32, TestEnum::ONE64});
  QVERIFY(testList2.testFlag(TestEnum::FOUR32));
  QVERIFY(testList2.testFlag(TestEnum::ONE64));

  testList2 = ~testList2;
  QVERIFY(testList2.testFlag(TestEnum::ONE32));
  QVERIFY(!testList2.testFlag(TestEnum::FOUR32));
  QVERIFY(!testList2.testFlag(TestEnum::ONE64));
  QVERIFY(testList2.testFlag(TestEnum::FOUR64));

  TestEnums testList(TestEnum::NONE | TestEnum::FOUR32 | TestEnum::ONE64);
  QVERIFY(testList.testFlag(TestEnum::FOUR32));
  QVERIFY(testList.testFlag(TestEnum::ONE64));

  QByteArray bytes;
  QDataStream out(&bytes, QIODevice::WriteOnly);
  out << testList;

  QDataStream in(&bytes, QIODevice::ReadOnly);
  TestEnums read;
  in >> read;
  QCOMPARE(testList, read);
}
