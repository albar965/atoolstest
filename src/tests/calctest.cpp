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
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "calctest.h"
#include "geo/calculations.h"

#include "geo/pos.h"

namespace ageo = atools::geo;
using ageo::Pos;

CalcTest::CalcTest()
{

}

void CalcTest::runtest(int argc, char *argv[])
{
  CalcTest tst;
  QTest::qExec(&tst, argc, argv);
}

void CalcTest::initTestCase()
{
}

void CalcTest::cleanupTestCase()
{

}

void CalcTest::testAltitudePressure_data()
{
  QTest::addColumn<float>("altMeter");
  QTest::addColumn<float>("pressureMbar");

  // https://de.wikipedia.org/wiki/Normatmosph%C3%A4re#/media/File:Internationale_Standard_Atmosph%C3%A4re.png
  QTest::newRow("SL") << 0.f << 1013.25f;
  QTest::newRow("5000 ft") << 1524.f << 843.099f;
  QTest::newRow("10000 ft") << 3048.f << 696.86f;
  QTest::newRow("20000 ft") << 6096.f << 465.693f;
  QTest::newRow("30000 ft") << 9144.f << 300.957f;
  QTest::newRow("40000 ft") << 12192.f << 186.947f;
}

void CalcTest::testAltitudePressure()
{
  QFETCH(float, altMeter);
  QFETCH(float, pressureMbar);

  QCOMPARE(ageo::pressureMbarForAltMeter(altMeter), pressureMbar);
  QCOMPARE(ageo::altMeterForPressureMbar(pressureMbar), altMeter);
}

void CalcTest::testDensityAltitude_data()
{
  QTest::addColumn<float>("temperatureC");
  QTest::addColumn<float>("seaLevelPressureMbar");
  QTest::addColumn<float>("altitudeFt");
  QTest::addColumn<float>("densityAltResult");
  QTest::addColumn<float>("pressureAltResult");

  QTest::newRow("15 << 1013.25 << 0") << 15.f << 1013.25f << 0.f << 0.f << 0.f;
  QTest::newRow("30 << 1013.25 << 0") << 30.f << 1013.25f << 0.f << 1723.93f << 0.f;
  QTest::newRow("0  << 1013.25 << 0") << 0.f << 1013.25f << 0.f << -1838.49f << 0.f;

  QTest::newRow("15 << 1013.25 << 1000") << 15.f << 1013.25f << 1000.f << 1233.97f << 1000.f;
  QTest::newRow("30 << 1013.25 << 1000") << 30.f << 1013.25f << 1000.f << 2943.28f << 1000.f;
  QTest::newRow("0  << 1013.25 << 1000") << 0.f << 1013.25f << 1000.f << -588.919f << 1000.f;

  QTest::newRow("15 << 980 << 1000") << 15.f << 980.f << 1000.f << 2367.45f << 1920.f;
  QTest::newRow("30 << 980 << 1000") << 30.f << 980.f << 1000.f << 4063.32f << 1920.f;
  QTest::newRow("0  << 980 << 1000") << 0.f << 980.f << 1000.f << 558.89f << 1920.f;

  QTest::newRow("15 << 1030 << 1000") << 15.f << 1030.f << 1000.f << 673.71f << 545.769f;
  QTest::newRow("30 << 1030 << 1000") << 30.f << 1030.f << 1000.f << 2389.66f << 545.769f;
  QTest::newRow("0  << 1030 << 1000") << 0.f << 1030.f << 1000.f << -1156.26f << 545.769f;

}

void CalcTest::testDensityAltitude()
{
  QFETCH(float, temperatureC);
  QFETCH(float, seaLevelPressureMbar);
  QFETCH(float, altitudeFt);
  QFETCH(float, densityAltResult);
  QFETCH(float, pressureAltResult);

  float pressureAltitude = ageo::pressureAltitudeFt(altitudeFt, seaLevelPressureMbar);
  QCOMPARE(pressureAltitude, pressureAltResult);

  float densityAltitude = ageo::densityAltitudeFt(temperatureC, pressureAltitude);
  QCOMPARE(densityAltitude, densityAltResult);

  densityAltitude = ageo::densityAltitudeFt(temperatureC, altitudeFt, seaLevelPressureMbar);
  QCOMPARE(densityAltitude, densityAltResult);
}
