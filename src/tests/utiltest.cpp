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
#include "util/props.h"

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

  QVariant var = QVariant::fromValue<TestEnums>(testList);
  TestEnums fromVar = var.value<TestEnums>();
  QCOMPARE(testList, fromVar);

  var = QVariant::fromValue<TestEnums>(TestEnum::EIGHT64);
  fromVar = var.value<TestEnums>();
  QCOMPARE(TestEnum::EIGHT64, fromVar);
}

void UtilTest::testProps()
{
  using namespace atools::util;
  enum Key
  {
    KEY_BOOL = 10,
    KEY_BYTE,
    KEY_SHORT,
    KEY_INT,
    KEY_LONG,
    KEY_FLOAT,
    KEY_DOUBLE,
    KEY_STRING,
    KEY_BYTES,
    KEY_VARIANT,
    KEY_NONE
  };

  const unsigned char byteTest[] = {0x78, 0x56, 0x34, 0x12};
  QByteArray bytes1((const char *)(byteTest), 4);

  const unsigned char byteTest2[] = {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef};
  QByteArray bytes2((const char *)(byteTest2), 12);

  Props props;
  props.addProps(QVector<Prop>({
    Prop(KEY_BOOL, true), Prop(KEY_BOOL, false),
    Prop(KEY_BYTE, -1),
    Prop(KEY_BYTE, 127),
    Prop(KEY_BYTE, 128),
    Prop(KEY_BYTE, 255),
    Prop(KEY_INT, 42), Prop(KEY_INT, 965),
    Prop(KEY_INT, std::numeric_limits<int>::max()),
    Prop(KEY_INT, std::numeric_limits<int>::min()),
    Prop(KEY_INT, std::numeric_limits<unsigned int>::max()),
    Prop(KEY_INT, std::numeric_limits<unsigned int>::min()),
    Prop(KEY_SHORT, static_cast<short>(1024)),
    Prop(KEY_LONG, 4200000000000000000LL),
    Prop(KEY_LONG, 42LL),
    Prop(KEY_FLOAT, 3.14159f),
    Prop(KEY_DOUBLE, 2.71828182846),
    Prop(KEY_STRING, QString("Hello Test")),
    Prop(KEY_BYTES, bytes1),
    Prop(KEY_BYTES, bytes2),
    Prop(KEY_VARIANT, QVariant("Variant")),
    Prop(KEY_VARIANT, QVariant(1968)),
    Prop(KEY_VARIANT, QVariant(12.3456789f)),
    Prop(KEY_VARIANT, QVariant(23.5678901)),
    Prop(KEY_NONE),
    Prop()
  }));

  QByteArray bytes;
  QDataStream out(&bytes, QIODevice::WriteOnly);
  out << props;

  Props propsRead;
  QDataStream in(bytes);
  in >> propsRead;

  for(const Prop& p : propsRead)
    qDebug() << p;

  QCOMPARE(props.size() - 1, propsRead.size());

  QCOMPARE(propsRead.getProp(KEY_NONE).isValid(), true);
  QCOMPARE(propsRead.getProps(KEY_BOOL).at(0).getValueBool(), true);
  QCOMPARE(propsRead.getProps(KEY_BOOL).at(1).getValueBool(), false);

  QCOMPARE(propsRead.getProps(KEY_BYTE).at(0).getValueInt(), -1);
  QCOMPARE(propsRead.getProps(KEY_BYTE).at(1).getValueInt(), 127);
  QCOMPARE(propsRead.getProps(KEY_BYTE).at(2).getValueInt(), 128);
  QCOMPARE(propsRead.getProps(KEY_BYTE).at(3).getValueInt(), 255);

  QCOMPARE(propsRead.getProp(KEY_SHORT).getValueShort(), static_cast<short>(1024));

  QCOMPARE(propsRead.getProps(KEY_INT).at(0).getValueInt(), 42);
  QCOMPARE(propsRead.getProps(KEY_INT).at(1).getValueInt(), 965);
  QCOMPARE(propsRead.getProps(KEY_INT).at(2).getValueInt(), std::numeric_limits<int>::max());
  QCOMPARE(propsRead.getProps(KEY_INT).at(3).getValueInt(), std::numeric_limits<int>::min());
  QCOMPARE(propsRead.getProps(KEY_INT).at(4).getValueUInt(), std::numeric_limits<unsigned int>::max());
  QCOMPARE(propsRead.getProps(KEY_INT).at(5).getValueUInt(), std::numeric_limits<unsigned int>::min());

  QCOMPARE(propsRead.getProps(KEY_LONG).at(0).getValueLongLong(), 4200000000000000000LL);
  QCOMPARE(propsRead.getProps(KEY_LONG).at(1).getValueLongLong(), 42LL);

  QCOMPARE(propsRead.getProp(KEY_FLOAT).getValueFloat(), 3.14159f);
  QCOMPARE(propsRead.getProp(KEY_DOUBLE).getValueDouble(), 2.71828182846);
  QCOMPARE(propsRead.getProp(KEY_STRING).getValueString(), QString("Hello Test"));
  QCOMPARE(propsRead.getProps(KEY_BYTES).at(0).getValueBytes(), bytes1);
  QCOMPARE(propsRead.getProps(KEY_BYTES).at(1).getValueBytes(), bytes2);

  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(0).getValueVariant(), QVariant("Variant"));
  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(1).getValueVariant(), QVariant(1968));
  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(2).getValueVariant(), QVariant(12.3456789f));
  QCOMPARE(propsRead.getProps(KEY_VARIANT).at(3).getValueVariant(), QVariant(23.5678901));

  for(auto it = propsRead.constBegin(); it != propsRead.constEnd(); ++it)
    QVERIFY(it.value().isValid());
}
