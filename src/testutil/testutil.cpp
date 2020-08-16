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

#include "testutil/testutil.h"

#include "sql/sqldatabase.h"

#include <QApplication>
#include <QThread>

namespace testutil {

void waitForValue(bool& done, int seconds)
{
  int waitTimeMs = seconds * 1000, sleepTimeMs = 100, msecs = 0;
  while(!done && msecs < waitTimeMs)
  {
    QApplication::processEvents();
    QThread::msleep(sleepTimeMs);
    msecs += sleepTimeMs;
  }
}

atools::sql::SqlDatabase *createDb(const QString& name, const QString& file)
{
  atools::sql::SqlDatabase::addDatabase("QSQLITE", name);
  atools::sql::SqlDatabase *db = new atools::sql::SqlDatabase(name);
  db->setDatabaseName(file);
  db->open();
  return db;
}

void removeDb(atools::sql::SqlDatabase *& db, const QString& name)
{
  if(db != nullptr)
  {
    db->close();
    delete db;
    db = nullptr;
  }

  atools::sql::SqlDatabase::removeDatabase(name);
}

} // namespace testutil
