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

#include "metartest.h"
#include "geotest.h"
#include "updatetest.h"
#include "dtmtest.h"
#include "magdectest.h"
#include "flightplantest.h"
#include "scenerycfgtest.h"
#include "pathtest.h"
#include "stringtest.h"

#include <QString>
#include <QTimer>
#include <QtTest/QtTest>

void test();
void runtest(QObject& testObject, QVector<std::pair<int, QString> >& messages, const QStringList& options);
void addOption(QCommandLineParser *parser, const QString& shortOpt);

static int argCount = 0;
static char **argVector = nullptr;
static QCommandLineParser *parser = nullptr;
static QStringList otherOptions;

int main(int argc, char *argv[])
{
  argCount = argc;
  argVector = argv;

  QCoreApplication app(argc, argv);
  parser = new QCommandLineParser();
  parser->setApplicationDescription("atools unit tests.");
  parser->addHelpOption();
  parser->addVersionOption();

  QCommandLineOption allOpt("RunAll", QObject::tr("Run all."));
  parser->addOption(allOpt);

  addOption(parser, "SceneryCfgTest");
  addOption(parser, "MagdecTest");
  addOption(parser, "StringTest");
  addOption(parser, "UpdateTest");
  addOption(parser, "PathTest");
  addOption(parser, "GeoTest");
  addOption(parser, "FlightplanTest");
  addOption(parser, "MetarTest");
  addOption(parser, "DtmTest");

  parser->process(*QCoreApplication::instance());
  otherOptions = parser->unknownOptionNames();

  if(parser->optionNames().isEmpty())
    parser->showHelp();

  QTimer::singleShot(0, test);

  return app.exec();
}

#define RUNTEST(name) \
  if(parser->isSet( # name) || parser->isSet("RunAll")) \
  { \
    name tst; \
    runtest(tst, messages, otherOptions); \
  };

void test()
{
  // status |= QTest::qExec(&tc, argc, argv);
  QVector<std::pair<int, QString> > messages;

  RUNTEST(SceneryCfgTest);
  RUNTEST(MagdecTest);
  RUNTEST(StringTest);
  RUNTEST(UpdateTest);
  RUNTEST(PathTest);
  RUNTEST(GeoTest);
  RUNTEST(FlightplanTest);
  RUNTEST(MetarTest);
  RUNTEST(DtmTest);

  bool failed = false;
  for(const std::pair<int, QString>& msg : messages)
  {
    if(msg.first > 0)
    {
      qWarning() << msg.second << "FAILED" << msg.first;
      failed = true;
    }
    else
      qDebug() << msg.second << "Success";
  }
  QApplication::exit(failed);
}

void runtest(QObject& testObject, QVector<std::pair<int, QString> >& messages, const QStringList& options)
{
  int result = QTest::qExec(&testObject, options);

  messages.append(std::make_pair(result, QString(testObject.metaObject()->className())));
}

void addOption(QCommandLineParser *cmdParser, const QString& shortOpt)
{
  QCommandLineOption opt(shortOpt);
  cmdParser->addOption(opt);
}
