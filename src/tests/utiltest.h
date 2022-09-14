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

#ifndef ATOOLSTEST_UTILTEST_H
#define ATOOLSTEST_UTILTEST_H

#include "util/flags.h"
#include <QtTest>

class UtilTest :
  public QObject
{
  Q_OBJECT

public:
  UtilTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testFlags();
  void testProps();
  void testFilesystemWatcher();

};

/* *INDENT-OFF* */
enum class TestEnum : atools::util::FlagType
{
  NONE =    0,
  ONE =     0x0000000000000001,
  TWO =     0x0000000000000002,
  FOUR =    0x0000000000000004,
  EIGHT =   0x0000000000000008,
  ONE16 =   0x0000000000001000,
  TWO16 =   0x0000000000002000,
  FOUR16 =  0x0000000000004000,
  EIGHT16 = 0x0000000000008000,
  ONE32 =   0x0000000010000000,
  TWO32 =   0x0000000020000000,
  FOUR32 =  0x0000000040000000,
  EIGHT32 = 0x0000000080000000,
  ONE64 =   0x1000000000000000,
  TWO64 =   0x2000000000000000,
  FOUR64 =  0x4000000000000000,
  EIGHT64 = 0x8000000000000000
};
/* *INDENT-ON* */

ATOOLS_DECLARE_FLAGS(TestEnums, TestEnum)
ATOOLS_DECLARE_OPERATORS_FOR_FLAGS(TestEnums)

Q_DECLARE_TYPEINFO(TestEnums, Q_PRIMITIVE_TYPE);
Q_DECLARE_METATYPE(TestEnums);

#endif // ATOOLSTEST_UTILTEST_H
