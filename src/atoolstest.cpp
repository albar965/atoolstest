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

#include "metartest.h"
#include "geotest.h"
#include "updatetest.h"
#include "dtmtest.h"
#include "versiontest.h"
#include "magdectest.h"
#include "flightplantest.h"
#include "scenerycfgtest.h"
#include "stringtest.h"
#include "onlinetest.h"
#include "maintest.h"

#include <QString>
#include <QTimer>
#include <QtTest/QtTest>

#if !defined(TEST_OWN_MAIN)

QTEST_MAIN(MainTest)

#else

#define RUNTESTEXT(name) \
  if(parser->isSet( # name) || parser->isSet("RunAll")) \
  { \
    name tst; \
    runtest(tst, messages, otherOptions); \
  }

#define RUNTEST(name) \
  { \
    name tst; \
    QTest::qExec(&tst, argCount, argVector); \
  }

#define DEFINETEST(name) \
  { \
    addOption(parser, # name); \
  };

void test();
void runtest(QObject& testObject, QVector<std::pair<int, QString> >& messages, const QStringList& options);
void addOption(QCommandLineParser *parser, const QString& shortOpt);

static int argCount = 0;
static char **argVector = nullptr;
static QCommandLineParser *parser = nullptr;
static QStringList otherOptions;

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(atools);

  argCount = argc;
  argVector = argv;

  QCoreApplication app(argc, argv);
  parser = new QCommandLineParser();
  parser->setApplicationDescription("atools unit tests.");
  parser->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  parser->addHelpOption();
  parser->addVersionOption();

  QCommandLineOption allOpt("RunAll", QObject::tr("Run all."));
  parser->addOption(allOpt);

  DEFINETEST(OnlineTest);
  DEFINETEST(SceneryCfgTest);
  DEFINETEST(MagdecTest);
  // DEFINETEST(UpdateTest);
  DEFINETEST(StringTest);
  DEFINETEST(VersionTest);
  DEFINETEST(GeoTest);
  DEFINETEST(FlightplanTest);
  DEFINETEST(MetarTest);
  DEFINETEST(DtmTest);

  parser->parse(QCoreApplication::arguments());
  otherOptions.append(QCoreApplication::arguments().first());
  otherOptions.append(parser->unknownOptionNames());

  if(parser->optionNames().isEmpty())
    parser->showHelp();

  QTimer::singleShot(0, test);

  return app.exec();
}

void test()
{
  // status |= QTest::qExec(&tc, argc, argv);
  QVector<std::pair<int, QString> > messages;

  RUNTESTEXT(OnlineTest);
  RUNTESTEXT(SceneryCfgTest);
  RUNTESTEXT(MagdecTest);
  // RUNTESTEXT(UpdateTest);
  RUNTESTEXT(StringTest);
  RUNTESTEXT(VersionTest);
  RUNTESTEXT(GeoTest);
  RUNTESTEXT(FlightplanTest);
  RUNTESTEXT(MetarTest);
  RUNTESTEXT(DtmTest);

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
  cmdParser->addOption(QCommandLineOption(shortOpt));
}

#endif
