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

#include "exception.h"
#include "fs/sc/simconnecttypes.h"
#include "geo/calculations.h"

#include "tests/airspacetest.h"
#include "tests/atoolstest.h"
#include "tests/calctest.h"
#include "tests/dbtest.h"
#include "tests/dtmtest.h"
#include "tests/flightplantest.h"
#include "tests/geotest.h"
#include "tests/gribtest.h"
#include "tests/magdectest.h"
#include "tests/metartest.h"
#include "tests/onlinetest.h"
#include "tests/perftest.h"
#include "tests/routetest.h"
#include "tests/scenerycfgtest.h"
#include "tests/spatialtest.h"
#include "tests/stringtest.h"
#include "tests/tracktest.h"
#include "tests/updatetest.h"
#include "tests/utiltest.h"
#include "tests/versiontest.h"

#include <QString>
#include <QTimer>
#include <QtTest/QtTest>

/*
 * Macros that allow to create and add test classes for command line selection.
 */
#define RUNTESTEXT_COND(name, condition) \
  { \
    if(condition) \
    { \
      if(parser->isSet( # name) || parser->isSet("RunAll")) \
      { \
        name tst; \
        runtest(tst, messages, otherOptions); \
      } \
    } \
    else \
    qStdOut() << "Skipping test" << # name << "since condition" << # condition << "is false" << endl; \
  }

#define RUNTESTEXT(name) \
  { \
    if(parser->isSet( # name) || parser->isSet("RunAll")) \
    { \
      name tst; \
      runtest(tst, messages, otherOptions); \
      qStdOut() << "Execution time for" << # name << timer.restart() << "ms" << endl; \
    } \
  }

#define RUNTEST(name) \
  { \
    name tst; \
    QTest::qExec(&tst, argCount, argVector); \
  }

#define DEFINETEST(name) \
  { \
    addOption(parser, # name); \
  }

// Forward declarations
void test();
void runtest(QObject& testObject, QVector<std::pair<int, QString> >& messages, const QStringList& options);
void addOption(QCommandLineParser *parser, const QString& shortOpt);

static int argCount = 0;
static char **argVector = nullptr;
static QCommandLineParser *parser = nullptr;
static QStringList otherOptions;

// Use own streams since Qt directs all output to stderr per default
QDebug& qStdOut();
QDebug& qStdErr();
void initIo();
void deinitIo();

// =====================================================================================
// main
// =====================================================================================
int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(atools);

  // Register all types to allow conversion from/to QVariant and thus reading/writing into settings
  atools::geo::registerMetaTypes();
  atools::fs::sc::registerMetaTypes();

  qRegisterMetaTypeStreamOperators<TestEnums>();

  argCount = argc;
  argVector = argv;

  initIo();

  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("atoolstest");
  QCoreApplication::setOrganizationName("ABarthel");
  QCoreApplication::setOrganizationDomain("littlenavmap.org");
  QCoreApplication::setApplicationVersion(VERSION_NUMBER_ATOOLSTEST); // VERSION_NUMBER

  qStdOut() << "SSL supported" << QSslSocket::supportsSsl()
            << "build library" << QSslSocket::sslLibraryBuildVersionString()
            << "library" << QSslSocket::sslLibraryVersionString() << endl;

  qStdOut() << "Starting tests ..." << endl;
  qStdOut() << "LANG=" << QProcessEnvironment::systemEnvironment().value("LANG") << endl;
  qStdOut() << "LANGUAGE=" << QProcessEnvironment::systemEnvironment().value("LANGUAGE") << endl;

  // Command line reading
  parser = new QCommandLineParser();
  parser->setApplicationDescription("atools unit tests.");
  parser->addHelpOption();
  parser->addVersionOption();

  QCommandLineOption allOpt({"a", "RunAll"}, "Run all test classes.");
  parser->addOption(allOpt);

  QCommandLineOption maxWarningsOpt({"w", "MaxWarnings"},
                                    "Maximum number of <warnings> or debug output lines.",
                                    "warnings");
  parser->addOption(maxWarningsOpt);

  QCommandLineOption testFunctions({"f", "TestFunctions"},
                                   "A comma separated list of test <functions>.",
                                   "functions");
  parser->addOption(testFunctions);

  DEFINETEST(AirspaceTest)
  DEFINETEST(AtoolsTest)
  DEFINETEST(CalcTest)
  DEFINETEST(DtmTest)
  DEFINETEST(FlightplanTest)
  DEFINETEST(GeoTest)
  DEFINETEST(GribTest)
  DEFINETEST(MagdecTest)
  DEFINETEST(MetarTest)
  DEFINETEST(OnlineTest)
  DEFINETEST(DbTest)
  DEFINETEST(PerfTest)
  DEFINETEST(RouteTest)
  DEFINETEST(SceneryCfgTest)
  DEFINETEST(SpatialTest)
  DEFINETEST(StringTest)
  DEFINETEST(TrackTest)
  DEFINETEST(UpdateTest)
  DEFINETEST(UtilTest)
  DEFINETEST(VersionTest)

  parser->parse(QCoreApplication::arguments());
  otherOptions.append(QCoreApplication::arguments().first());

  if(parser->isSet(maxWarningsOpt))
  {
    otherOptions.append("-maxwarnings");
    otherOptions.append(parser->value(maxWarningsOpt));
  }

  QStringList testFuncs;
  testFuncs.append(parser->value(testFunctions).split(","));
  testFuncs.removeAll(QString());

  otherOptions.append(testFuncs);

  if(parser->optionNames().isEmpty() || parser->isSet("h"))
    // Display help and exit
    parser->showHelp();
  else if(parser->isSet("v"))
    // Display version and exit
    parser->showVersion();
  else
  {
    // Start test execution in event queue
    QTimer::singleShot(0, test);
    return app.exec();
  }
}

void test()
{
  // status |= QTest::qExec(&tc, argc, argv);
  QVector<std::pair<int, QString> > messages;
  try
  {
    QElapsedTimer timerTotal;
    timerTotal.start();

    QElapsedTimer timer;
    timer.start();

    RUNTESTEXT(AirspaceTest)
    RUNTESTEXT(AtoolsTest)
    RUNTESTEXT(CalcTest)
    RUNTESTEXT(DbTest)
    RUNTESTEXT(DtmTest)
    RUNTESTEXT(FlightplanTest)
    RUNTESTEXT(GeoTest)
    RUNTESTEXT_COND(GribTest, QSslSocket::supportsSsl())
    RUNTESTEXT(MagdecTest)
    RUNTESTEXT(MetarTest)
    RUNTESTEXT(OnlineTest)
    RUNTESTEXT(PerfTest)
    RUNTESTEXT(RouteTest)
    RUNTESTEXT(SceneryCfgTest)
    RUNTESTEXT(SpatialTest)
    RUNTESTEXT(StringTest)
    RUNTESTEXT(TrackTest)
    RUNTESTEXT_COND(UpdateTest, QSslSocket::supportsSsl())
    RUNTESTEXT(UtilTest)
    RUNTESTEXT(VersionTest)

    qStdOut() << "Total execution time" << timerTotal.restart() << "ms" << endl;
  }
  catch(std::exception& e)
  {
    qStdErr() << "*** Caught exception" << e.what() << endl;
  }

  bool failed = false;
  for(const std::pair<int, QString>& msg : messages)
  {
    if(msg.first > 0)
    {
      qStdErr() << "*** " << msg.second << "FAILED" << msg.first << endl;
      failed = true;
    }
    else
      qStdOut() << msg.second << "Success" << endl;
  }

  qStdOut() << "exit" << static_cast<int>(failed) << endl;

  deinitIo();
  QApplication::exit(failed);
}

void runtest(QObject& testObject, QVector<std::pair<int, QString> >& messages, const QStringList& options)
{
  int result = QTest::qExec(&testObject, options);
  messages.append(std::make_pair(result, QString(testObject.metaObject()->className())));
}

void addOption(QCommandLineParser *cmdParser, const QString& shortOpt)
{
  cmdParser->addOption(QCommandLineOption(shortOpt, QString("Run the %1 test class.").arg(shortOpt)));
}

// =============================================================
// Own IO to avoid all redirected to stderr and have functions of QDebug
static QDebug *qDbgStdOut;
static QDebug *qDbgStdErr;
static QFile stdOutFile;
static QFile stdErrFile;

void initIo()
{
  stdOutFile.open(stdout, QIODevice::WriteOnly | QIODevice::Text);
  stdErrFile.open(stderr, QIODevice::WriteOnly | QIODevice::Text);

  qDbgStdOut = new QDebug(&stdOutFile);
  qDbgStdErr = new QDebug(&stdErrFile);
}

void deinitIo()
{
  delete qDbgStdOut;
  qDbgStdOut = nullptr;
  delete qDbgStdErr;
  qDbgStdErr = nullptr;
}

QDebug& qStdOut()
{
  return *qDbgStdOut;

}

QDebug& qStdErr()
{
  return *qDbgStdErr;
}
