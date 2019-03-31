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

void MetarTest::testMetarAsn()
{
  QFile metarFile(":/test/resources/current_wx_snapshot.txt");
  QVERIFY(metarFile.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFile);

  int numFailed = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    QStringList list = line.split("::");
    QVERIFY(list.size() >= 2);

    if(!line.isEmpty())
    {
      atools::fs::weather::Metar metar(list.at(1), "XXXX", QDateTime(), true);
      numFailed += !metar.isValid();
    }
  }
  metarFile.close();

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 29);
}


void MetarTest::testMetarSim()
{
  QFile metarFiles(":/test/resources/METAR.txt");
  QVERIFY(metarFiles.open(QIODevice::ReadOnly | QIODevice::Text));

  QTextStream weatherSnapshot(&metarFiles);

  int numFailed = 0;
  QString line;
  while(weatherSnapshot.readLineInto(&line))
  {
    if(!line.isEmpty())
    {
      atools::fs::weather::Metar metar(line, "XXXX", QDateTime(), true);
      numFailed += !metar.isValid();
    }
  }
  metarFiles.close();

  // Check the number of failed since too many are not readable
  QCOMPARE(numFailed, 21);
}
