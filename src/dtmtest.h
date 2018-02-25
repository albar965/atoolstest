/*****************************************************************************
* Copyright 2015-2018 Alexander Barthel albar965@mailbox.org
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

#ifndef ATOOLSTEST_DTMTEST_H
#define ATOOLSTEST_DTMTEST_H

#include <QString>
#include <QtTest>

namespace atools {
namespace geo {
class Pos;
}
}

class DtmTest :
  public QObject
{
  Q_OBJECT

public:
  DtmTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testOpen();
  void testFilePos();
  void testElevation_data();
  void testElevation();
  void testElevationLine_data();
  void testElevationLine();

  void testFilePos_data();

private:
  void addData(const atools::geo::Pos& pos, qint64 fileOffset, int fileIndex);

};

#endif // ATOOLSTEST_DTMTEST_H
