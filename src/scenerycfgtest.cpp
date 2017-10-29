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
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "scenerycfgtest.h"

#include "fs/scenery/scenerycfg.h"
#include "fs/scenery/addonpackage.h"

using atools::fs::scenery::SceneryCfg;
using atools::fs::scenery::SceneryArea;
using atools::fs::scenery::AddOnPackage;

SceneryCfgTest::SceneryCfgTest()
{

}

void SceneryCfgTest::testFsx()
{
  QString specialCharsPath(QString("Addon Scenery") + QDir::separator() + "TEST äöü~ÄÖÜß§^°|" + QDir::separator());
  QString specialCharsTitle("TEST äöü~ÄÖÜß§^°|\\");

  QString codec("Windows-1252");
  SceneryCfg cfg(codec);
  cfg.read(":/test/resources/scenery_fsx_cfg");

  const QList<SceneryArea>& areas = cfg.getAreas();
  QCOMPARE(areas.size(), 203);

  QCOMPARE(areas.at(areas.size() - 1).getTitle(), specialCharsTitle);
  QCOMPARE(areas.at(areas.size() - 2).getLocalPath(), specialCharsPath);
}

void SceneryCfgTest::testP3dv4()
{
  QString specialCharsPath(QString("Addon Scenery") + QDir::separator() + "TEST äöü~ÄÖÜß§^°|" + QDir::separator());
  QString specialCharsTitle("TEST äöü~ÄÖÜß§^°|\\");

  QString specialChars("TEST äöü~ÄÖÜß§^°|\\");
  QString codec("UTF-8");
  SceneryCfg cfg(codec);
  cfg.read(":/test/resources/scenery_p3dv4.cfg");

  const QList<SceneryArea>& areas = cfg.getAreas();
  QCOMPARE(areas.size(), 196);

  QCOMPARE(areas.at(areas.size() - 1).getTitle(), specialCharsTitle);
  QCOMPARE(areas.at(areas.size() - 2).getLocalPath(), specialCharsPath);
}

void SceneryCfgTest::testP3dv4AddOn()
{
  AddOnPackage package(":/test/resources/add-on.xml");

  QCOMPARE(package.getName(), QLatin1Literal("MyAddOn"));
  QCOMPARE(package.getDescription(), QString("My Add-on developed by My Company äöüß."));
  QCOMPARE(package.getBaseDirectory(), QString(":/test/resources"));
  QCOMPARE(package.getFilename(), QString(":/test/resources/add-on.xml"));

  QCOMPARE(package.getComponents().at(0).getName(), QLatin1Literal("Scenery01"));
  QCOMPARE(package.getComponents().at(0).getPath(), QLatin1Literal("Content\\Scenery\\Scenery01"));
  QCOMPARE(package.getComponents().at(0).getCategory(), QLatin1Literal("Scenery"));
  QCOMPARE(package.getComponents().at(0).getLayer(), -1);

  QCOMPARE(package.getComponents().at(1).getName(), QLatin1Literal("Scenery02"));
  QCOMPARE(package.getComponents().at(1).getPath(), QLatin1Literal("C:\\Content\\Scenery\\Scenery02"));
  QCOMPARE(package.getComponents().at(1).getCategory(), QLatin1Literal("Scenery"));
  QCOMPARE(package.getComponents().at(1).getLayer(), 99);
}

void SceneryCfgTest::testP3dv4AddOn2()
{
  AddOnPackage package(":/test/resources/add-on_copy.xml");

  QCOMPARE(package.getName(), QString());
  QCOMPARE(package.getDescription(), QString());
  QCOMPARE(package.getBaseDirectory(), QString(":/test/resources"));
  QCOMPARE(package.getFilename(), QString(":/test/resources/add-on_copy.xml"));

  QCOMPARE(package.getComponents().at(0).getName(), QLatin1Literal("Scenery01"));
  QCOMPARE(package.getComponents().at(0).getPath(), QLatin1Literal("Content\\Scenery\\Scenery01"));
  QCOMPARE(package.getComponents().at(0).getCategory(), QLatin1Literal("Scenery"));
  QCOMPARE(package.getComponents().at(0).getLayer(), -1);

  QCOMPARE(package.getComponents().at(1).getName(), QLatin1Literal("Scenery02"));
  QCOMPARE(package.getComponents().at(1).getPath(), QLatin1Literal("C:\\Content\\Scenery\\Scenery02"));
  QCOMPARE(package.getComponents().at(1).getCategory(), QLatin1Literal("Scenery"));
  QCOMPARE(package.getComponents().at(1).getLayer(), 99);
}

void SceneryCfgTest::testP3dv4AddOnUtf16()
{
  AddOnPackage package(":/test/resources/add-on-utf-16.xml");

  QCOMPARE(package.getName(), QLatin1Literal("Flightbeam - KMSP"));
  QCOMPARE(package.getDescription(), QLatin1Literal("Flightbeam - Minneapolis St. Paul Intl. Airport for Prepar3D"));
  QCOMPARE(package.getBaseDirectory(), QLatin1Literal(":/test/resources"));
  QCOMPARE(package.getFilename(), QLatin1Literal(":/test/resources/add-on-utf-16.xml"));

  QCOMPARE(package.getComponents().at(0).getName(), QLatin1Literal("Flightbeam - KMSP scenery"));
  QCOMPARE(package.getComponents().at(0).getPath(),
           QLatin1Literal("G:\\Program Files\\Lockheed Martin\\Prepar3D v4\\Flightbeam\\Flightbeam - KMSP\\scenery"));
  QCOMPARE(package.getComponents().at(0).getCategory(), QLatin1Literal("Scenery"));
  QCOMPARE(package.getComponents().at(0).getLayer(), -1);

}

void SceneryCfgTest::testP3dv4AddOnEncodingMismatch()
{
  AddOnPackage package(":/test/resources/add-on_encoding_mismatch.xml");
  QCOMPARE(package.getDescription(), QString("Sends simulator reports to Navigraph Charts öäüÖÜ"));
}

void SceneryCfgTest::testP3dv4AddOnNoEncoding()
{
  AddOnPackage package("/home/alex/Projekte/atoolstest/resources/add-on_win_encoding_no_bom.xml");
  QCOMPARE(package.getDescription(), QString("Sends simulator reports to Navigraph Charts öäüÖÄÜ"));

}
