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

#ifndef ATOOLSTEST_MAGDEC_TEST_H
#define ATOOLSTEST_MAGDEC_TEST_H

#include "wmm/magdectool.h"

#include <QString>
#include <QtTest>

namespace atools {
namespace geo {
class Pos;
}
}

class MagdecTest :
  public QObject
{
  Q_OBJECT

public:
  MagdecTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testTool();

  void testOpen();
  void testMagdecBgl_data();
  void testMagdecBgl();

  void testMagdecWmm_data();
  void testMagdecWmm();

  void testMagdecDate();

  void testWmmTool_data();
  void testWmmTool();
  void testWmm();

private:
  atools::wmm::MagDecTool tool;

};

#endif // ATOOLSTEST_MAGDEC_TEST_H
