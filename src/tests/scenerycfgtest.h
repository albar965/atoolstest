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

#ifndef ATOOLSTEST_SCTEST_H
#define ATOOLSTEST_SCTEST_H

#include <QString>
#include <QtTest>

class SceneryCfgTest :
  public QObject
{
  Q_OBJECT

public:
  SceneryCfgTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void testXplane();

  void testFsx();
  void testP3dv4();

  void testP3dv4AddOn();
  void testP3dv4AddOn2();
  void testP3dv4AddOnUtf16();

  void testP3dv4AddOnEncodingMismatch();
  void testP3dv4AddOnNoEncoding();

};

#endif // ATOOLSTEST_SCTEST_H
