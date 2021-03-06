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

#ifndef ATOOLSTEST_ONLINE_TEST_H
#define ATOOLSTEST_ONLINE_TEST_H

#include <QString>
#include <QtTest>

namespace atools {
namespace sql {
class SqlDatabase;
}
}

class OnlineTest :
  public QObject
{
  Q_OBJECT

public:
  OnlineTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testCreateSchemaVatsim();
  void testCreateSchemaVatsim3();

  void testCreateSchemaIvao();
  void testCreateSchemaIvao2();

  void testCreateSchemaCustom();

  void testOpenStatusVatsim();
  void testOpenStatusVatsim3();
  void testOpenStatusIvao();

  void testOpenWhazzupVatsim();
  void testOpenWhazzupVatsim3();

  void testOpenWhazzupIvao();
  void testOpenWhazzupIvao2();

  void testOpenServersVatsim();
  void testOpenServersIvao();

  void testOpenWhazzupCustom();

  void testDropSchemaVatsim();
  void testDropSchemaIvao();
  void testDropSchemaCustom();

  void testVatsimJson3Download();

private:
  atools::sql::SqlDatabase *dbCustom = nullptr;
  atools::sql::SqlDatabase *dbIvao = nullptr;
  atools::sql::SqlDatabase *dbIvao2 = nullptr;
  atools::sql::SqlDatabase *dbVatsim = nullptr;
  atools::sql::SqlDatabase *dbVatsim3 = nullptr;

};

#endif // ATOOLSTEST_ONLINE_TEST_H
