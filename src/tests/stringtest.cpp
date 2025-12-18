/*****************************************************************************
* Copyright 2015-2025 Alexander Barthel alex@littlenavmap.org
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

#include "stringtest.h"

#include "fs/util/fsutil.h"
#include "util/csvreader.h"
#include "atools.h"

StringTest::StringTest()
{

}

void StringTest::runtest(int argc, char *argv[])
{
  StringTest tst;
  QTest::qExec(&tst, argc, argv);
}

void StringTest::initTestCase()
{
}

void StringTest::cleanupTestCase()
{

}

void StringTest::testNormalize()
{
  QStringList TEST({
    " !\" § $ % & / ( ) = ? ` ´ { [ ] } \\ @ ; , : . _ - * + ' # ",
    "Ä ä Ö ö Ü ü ¼ ½ ¾ À Á Â Ã Ä Å Æ Ç È É Ê Ë Ì Í Î Ï Ð Ñ Ò Ó Ô Õ Ö × Ø ",
    "Ù Ú Û Ü Ý Þ ß à á â ã ä å æ ç è é ê ë ì í î ï ñ ò ó ô õ ö ø ù ú û ý ÿ ",
    "Ā ā Ă ă Ą ą Ć ć Ĉ ĉ Ċ ċ Č Č č Ď ď Đ đ Ē ē Ĕ ĕ Ė ė Ę ę Ě ě Ĝ ĝ Ğ ğ Ġ ",
    "ġ Ģ ģ Ĥ Ĥ Ħ ħ Ĩ ĩ Ī ī Ĭ ĭ Į į İ ı Ĳ ĳ Ĵ ĵ Ķ ķ ĸ Ĺ ĺ Ļ ļ Ľ ľ Ŀ ŀ Ł ł ",
    "Ń ń Ņ ņ Ň ň ŉ Ŋ ŋ Ō ō Ŏ ŏ Ő ő Œ œ Ŕ ŕ Ŗ ŗ Ř ř Ś ś Ŝ ŝ Ş ş Š š Ţ ţ Ť ť ",
    "Ŧ ŧ Ũ ũ Ū ū Ŭ ŭ Ů ů Ű ű Ų ų Ŵ ŵ Ŷ ŷ Ÿ Ź ź Ż ż Ž ž ſ"
  });

  for(QString s : TEST)
  {
    qInfo() << s;
    qInfo() << atools::normalizeStr(s);
  }
}

void StringTest::testElide()
{
  using namespace atools;
  QCOMPARE(elideTextShort("0123456789", 5), "0123…");
  QCOMPARE(elideTextShort("012345", 5), "0123…");
  QCOMPARE(elideTextShort("01234", 5), "01234");
  QCOMPARE(elideTextShort("0123", 5), "0123");
  QCOMPARE(elideTextShort("0", 5), "0");
  QCOMPARE(elideTextShort("", 5), "");
  QCOMPARE(elideTextShortLeft("0123456789", 5), "…6789");
  QCOMPARE(elideTextShortLeft("012345", 5), "…2345");
  QCOMPARE(elideTextShortLeft("01234", 5), "01234");
  QCOMPARE(elideTextShortLeft("0123", 5), "0123");
  QCOMPARE(elideTextShortLeft("0", 5), "0");
  QCOMPARE(elideTextShortLeft("", 5), "");
  QCOMPARE(elideTextShortMiddle("0123456789", 5), "01…89");
  QCOMPARE(elideTextShortMiddle("012345", 5), "01…45");
  QCOMPARE(elideTextShortMiddle("01234", 5), "01234");
  QCOMPARE(elideTextShortMiddle("0123", 5), "0123");
  QCOMPARE(elideTextShortMiddle("0", 5), "0");
  QCOMPARE(elideTextShortMiddle("", 5), "");

  QCOMPARE(elideTextLinesShort("0123456789", 5, 5), "0123…");
  QCOMPARE(elideTextLinesShort("012345", 5, 5), "0123…");
  QCOMPARE(elideTextLinesShort("01234", 5, 5), "01234");
  QCOMPARE(elideTextLinesShort("0123", 5, 5), "0123");
  QCOMPARE(elideTextLinesShort("0", 5, 5), "0");
  QCOMPARE(elideTextLinesShort("", 5, 5), "");
  QCOMPARE(elideTextLinesShort("0123456789\n\nABCDEFGHIJ\n0123456789\n0123456789\n0123456789", 2, 5), "0123…\n\n…");
  QCOMPARE(elideTextLinesShort("0123456789\nABCDEFGHIJ\n0123456789\n0123456789\n0123456789", 2, 5), "0123…\nABCD…\n…");
  QCOMPARE(elideTextLinesShort("0123456789\nABCDEFGHIJ\n0123456789", 2, 10), "0123456789\nABCDEFGHIJ\n…");
  QCOMPARE(elideTextLinesShort("0123456789\nABCDEFGHIJ\n0123456789", 2, 10, false, false), "0123456789\nABCDEFGHIJ…");
  QCOMPARE(elideTextLinesShort("0123456789\nABCDEFGHIJ", 2, 10), "0123456789\nABCDEFGHIJ");
  QCOMPARE(elideTextLinesShort("0123456789\nABCDEFGHIJ", 2, 5), "0123…\nABCD…");
  QCOMPARE(elideTextLinesShort("0123456789", 2, 10), "0123456789");
  QCOMPARE(elideTextLinesShort("0123456789", 2, 9), "01234567…");

  QCOMPARE(elideTextLinesShort("0123456789\n\nABCDEFGHIJ\n0123456789\n\n0123456789\n0123456789", 2, 5, true), "0123…\nABCD…\n…");
  QCOMPARE(elideTextLinesShort("0123456789\n\nABCDEFGHIJ\n\n0123456789", 2, 10, true), "0123456789\nABCDEFGHIJ\n…");
  QCOMPARE(elideTextLinesShort("0123456789\n\nABCDEFGHIJ", 2, 10, true), "0123456789\nABCDEFGHIJ");
  QCOMPARE(elideTextLinesShort("\n0123456789\n", 2, 10, true), "0123456789");
}

void StringTest::testCapAirport_data()
{

  QTest::addColumn<QString>("name");
  QTest::addColumn<QString>("capname");

  QTest::newRow("Tustin Mcas") << "Tustin Mcas" << "Tustin MCAS";
  QTest::newRow("TUSTIN MCAS") << "TUSTIN MCAS" << "Tustin MCAS";
  QTest::newRow("tustin mcas") << "tustin mcas" << "Tustin MCAS";
  QTest::newRow("San Clemente Island Nalf") << "San Clemente Island Nalf" << "San Clemente Island NALF";
  QTest::newRow("St Mary's") << "St Mary's" << "St Mary's";
  QTest::newRow("ST MARY'S") << "ST MARY'S" << "St Mary's";
  QTest::newRow("st mary's") << "st mary's" << "St Mary's";

}

void StringTest::testCapAirport()
{
  QFETCH(QString, name);
  QFETCH(QString, capname);

  QCOMPARE(atools::fs::util::capAirportName(name), capname);
}

void StringTest::testPath_data()
{
  QTest::addColumn<QStringList>("path");
  QTest::addColumn<QString>("result");

#ifdef Q_OS_WIN32
  QTest::newRow("Existing correct case") << QStringList({"C:/Windows", "System32"}) << "C:/Windows\\System32";
  QTest::newRow("Existing correct case") << QStringList({"C:", "Windows", "System32"}) << "C:\\Windows\\System32";
  QTest::newRow("Existing correct case") << QStringList({"C:", "windows", "SYSTEM32"}) << "C:\\windows\\SYSTEM32";
  QTest::newRow("Existing correct case") << QStringList({"c:", "windows", "SYSTEM32"}) << "c:\\windows\\SYSTEM32";
#else

  QTest::newRow("Existing correct case") << QStringList({"/usr", "bin", "aclocal"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case") << QStringList({"/usr", "BIN", "aclocal"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case 2") << QStringList({"/usr", "BIN", "ACLOCAL"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case 3") << QStringList({"/usr", "bin", "ACLOCAL"}) << "/usr/bin/aclocal";
  QTest::newRow("Existing wrong case 4") << QStringList({"/", "USR", "BIN", "ACLOCAL"}) << "/usr/bin/aclocal";

  QTest::newRow("Not Existing correct case") << QStringList({"/usr", "bin", "aclocalx"}) << "/usr/bin/aclocalx";
  QTest::newRow("Not Existing wrong case") << QStringList({"/usr", "BIN", "aclocalx"}) << "/usr/bin/aclocalx";
  QTest::newRow("Not Existing wrong case 2") << QStringList({"/usr", "BIN", "ACLOCALX"}) << "/usr/bin/ACLOCALX";
  QTest::newRow("Not Existing wrong case 3") << QStringList({"/usr", "bin", "ACLOCALX"}) << "/usr/bin/ACLOCALX";
  QTest::newRow("Not Existing wrong case 4") << QStringList({"/", "USR", "BIN", "ACLOCALX"}) << "/usr/bin/ACLOCALX";

  QTest::newRow("Not Existing Path correct case") << QStringList({"/usr", "binx", "aclocalx"}) << "/usr/binx/aclocalx";
  QTest::newRow("Not Existing Path wrong case") << QStringList({"/usr", "BINX", "aclocalx"}) << "/usr/BINX/aclocalx";
  QTest::newRow("Not Existing Path wrong case 2") << QStringList({"/usr", "BINX", "ACLOCALX"}) << "/usr/BINX/ACLOCALX";
  QTest::newRow("Not Existing Path wrong case 3") <<
    QStringList({"/", "USR", "BINX", "ACLOCALX"}) << "/usr/BINX/ACLOCALX";
#endif
}

void StringTest::testPath()
{
  QFETCH(QStringList, path);
  QFETCH(QString, result);

  QCOMPARE(atools::buildPathNoCase(path), result);
}

void StringTest::testCsv_data()
{
  QTest::addColumn<QString>("csv");
  QTest::addColumn<QStringList>("result");

  // value1,"value2 with , separator",value3,"value4 with "" escaped",value4
  QTest::newRow("CSV Line Unescaped space") << "a, ,c,d" << QStringList({"a", "", "c", "d"});
  QTest::newRow("CSV Line Escaped space") << "a,\" \",c,d" << QStringList({"a", " ", "c", "d"});
  QTest::newRow("CSV Line Simple") << "a,b,c,d" << QStringList({"a", "b", "c", "d"});
  QTest::newRow("CSV Line Simple 2") << "aaa,bbb,ccc,ddd" << QStringList({"aaa", "bbb", "ccc", "ddd"});
  QTest::newRow("CSV Line Empty at end") << "aaa,bbb,ccc,ddd," << QStringList({"aaa", "bbb", "ccc", "ddd", ""});
  QTest::newRow("CSV Line Empty at beginning and middle") << ",bbb,,ddd" << QStringList({"", "bbb", "", "ddd"});
  QTest::newRow("CSV Line Escaped") << "aaa,\"bbb\",ccc,ddd" << QStringList({"aaa", "bbb", "ccc", "ddd"});
  QTest::newRow("CSV Line Start and end escaped") << "\"aaa\",bbb,ccc,\"ddd\"" << QStringList({"aaa", "bbb", "ccc",
                                                                                               "ddd"});
  QTest::newRow("CSV Line") << "\"a,a,a\",bbb,ccc,\"ddd\"" << QStringList({"a,a,a", "bbb", "ccc", "ddd"});
  QTest::newRow("CSV Line Escaped \" and ,") << "\"a,a,a\",bbb,ccc,\"dd\"\"dd\"" << QStringList({"a,a,a", "bbb", "ccc",
                                                                                                 "dd\"dd"});
  QTest::newRow("CSV Line Escaped \"\"") << "\"a,a,a\",bbb,ccc,\"dd\"\"dd\"" << QStringList({"a,a,a", "bbb", "ccc",
                                                                                             "dd\"dd"});
  QTest::newRow("CSV Line Escaped \"\" at start or value") << "\"a,a,a\",bbb,ccc,\"\"\"dd\"" <<
    QStringList({"a,a,a", "bbb", "ccc",
                 "\"dd"});
  QTest::newRow("CSV Line Escaped \"\" at end or value") << "\"a,a,a\",bbb,ccc,\"dd\"\"\"" <<
    QStringList({"a,a,a", "bbb", "ccc",
                 "dd\""});
  QTest::newRow("CSV Line Escaped \\n") << "\"a,a,a\",bbb,ccc,\"dd\ndd\"" << QStringList({"a,a,a", "bbb", "ccc",
                                                                                          "dd\ndd"});
}

void StringTest::testCsv()
{
  QFETCH(QString, csv);
  QFETCH(QStringList, result);

  atools::util::CsvReader reader(',', '"', true);
  reader.readCsvLine(csv);
  qInfo().noquote() << "VALUE" << csv;
  qInfo().noquote() << "RESULT" << reader.getValues().join("|");

  QCOMPARE(reader.getValues(), result);

}
