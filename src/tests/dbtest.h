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

#ifndef ATOOLSTEST_DB_TEST_H
#define ATOOLSTEST_DB_TEST_H

#include <QString>
#include <QtTest>

namespace atools {
namespace sql {
class SqlDatabase;
}
}

class DbTest :
  public QObject
{
  Q_OBJECT

public:
  DbTest();

  static void runtest(int argc, char *argv[]);

private slots:
  void initTestCase();
  void cleanupTestCase();

  void testException_data();
  void testException();

  void testBound();
  void testInsert();
  void testInsertId();
  void testUpdate();
  void testDelete();

  void testInsertUndo();
  void testInsertUndoId();
  void testUpdateUndo();
  void testDeleteUndo();
  void testUndoDeleteAll();
  void testUndoBulkInsert();

  void testExecUndoRedo();
  void testExecUndoRedoBulk();

private:
  atools::sql::SqlDatabase *db = nullptr, *dbUndo = nullptr, *dbUndoBulk = nullptr;

};

#endif // ATOOLSTEST_DB_TEST_H
