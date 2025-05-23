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

#ifndef ATOOLSTEST_TESTUTIL_H
#define ATOOLSTEST_TESTUTIL_H

class QString;

namespace atools {
namespace sql {
class SqlDatabase;
}
}

namespace testutil {

void waitForValue(bool& done, int seconds);
void wait(int seconds);

atools::sql::SqlDatabase *createDb(const QString& name, const QString& file, bool readonly);

void removeDb(atools::sql::SqlDatabase *& db, const QString& name);

} // namespace testutil

#endif // ATOOLSTEST_TESTUTIL_H
