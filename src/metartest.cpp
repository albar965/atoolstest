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

#include "metartest.h"

#include "fs/weather/metar.h"
#include "fs/weather/metarparser.h"

MetarTest::MetarTest()
{

}

void MetarTest::runtest(int argc, char *argv[])
{
  MetarTest tst;
  QTest::qExec(&tst, argc, argv);
}

void MetarTest::initTestCase()
{
  atools::fs::weather::initTranslateableTexts();
}

void MetarTest::cleanupTestCase()
{

}

void MetarTest::testMetarAsn_data()
{
  QTest::addColumn<QString>("data");

  QFile metars(":/test/resources/current_wx_snapshot.txt");
  QVERIFY(metars.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metars);

  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    QStringList list = line.split("::");
    QVERIFY(list.size() >= 2);

    QTest::newRow(qPrintable(list.at(0))) << list.at(1);
  }
  metars.close();
}

void MetarTest::testMetarAsn()
{
  QFETCH(QString, data);

  atools::fs::weather::Metar metar(data);

  QVERIFY(metar.getParsedMetar().isValid());
  // QCOMPARE(metar.getParsedMetar().getUnusedData(), QString());
  QVERIFY(!metar.getCleanMetar().isEmpty());
  QVERIFY(!metar.getMetar().isEmpty());
}

void MetarTest::testMetarSim_data()
{
  QTest::addColumn<QString>("data");

  QFile metars(":/test/resources/METAR.txt");
  QVERIFY(metars.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metars);

  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    if(!line.isEmpty())
      QTest::newRow(qPrintable(line)) << line;
  }
  metars.close();
}

void MetarTest::testMetarSim()
{
  QFETCH(QString, data);

  atools::fs::weather::Metar metar(data, "XXXX", QDateTime(), true);

  QVERIFY(metar.getParsedMetar().isValid());
  // QCOMPARE(metar.getParsedMetar().getUnusedData(), QString());
  QVERIFY(!metar.getCleanMetar().isEmpty());
  QVERIFY(!metar.getMetar().isEmpty());
}
