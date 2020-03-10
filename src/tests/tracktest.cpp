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

#include "tracktest.h"
#include "track/trackreader.h"
#include "track/trackdownloader.h"
#include "testutil/testutil.h"

using atools::track::TrackReader;
using atools::track::TrackDownloader;

TrackTest::TrackTest()
{

}

void TrackTest::runtest(int argc, char *argv[])
{
  TrackTest tst;
  QTest::qExec(&tst, argc, argv);
}

void TrackTest::initTestCase()
{
}

void TrackTest::cleanupTestCase()
{

}

void TrackTest::testTrackReader_data()
{
  // curl  "https://notams.aim.faa.gov/nat.html" > NATS.html
  // curl --data "queryType=pacificTracks&actionType=advancedNOTAMFunctions" https://www.notams.faa.gov/dinsQueryWeb/advancedNotamMapAction.do >PACOTS.html
  // curl  "https://www.airservicesaustralia.com/flextracks/text.asp?ver=1" > AUSOTS.html

  QTest::addColumn<QString>("filename");
  QTest::addColumn<int>("type");
  QTest::addColumn<int>("number");
  QTest::addColumn<int>("invalid");

  QTest::newRow("AUSOTS") << "testdata/AUSOTS.html" << int(atools::track::AUSOTS) << 21 << 0;
  QTest::newRow("AUSOTS 2") << "testdata/AUSOTS2.html" << int(atools::track::AUSOTS) << 19 << 0;
  QTest::newRow("PACOTS") << "testdata/PACOTS.html" << int(atools::track::PACOTS) << 14 << 0;
  QTest::newRow("PACOTS 2") << "testdata/PACOTS2.html" << int(atools::track::PACOTS) << 16 << 0;
  QTest::newRow("NATS (2 invalid)") << "testdata/NATS.html" << int(atools::track::NATS) << 13 << 2;
  QTest::newRow("NATS 2") << "testdata/NATS2.html" << int(atools::track::NATS) << 9 << 0;
}

void TrackTest::testTrackReader()
{
  QFETCH(QString, filename);
  QFETCH(int, type);
  QFETCH(int, number);
  QFETCH(int, invalid);

  TrackReader reader;
  reader.readTracks(filename, atools::track::TrackType(type));

  QCOMPARE(reader.removeInvalid(), invalid);
  QCOMPARE(reader.getTracks().size(), number);

  for(const atools::track::Track& track : reader.getTracks())
  {
    qDebug() << track;
    QCOMPARE(track.type, atools::track::TrackType(type));
    QVERIFY(track.isFullyValid());
  }
}

void TrackTest::testDownload_data()
{
  QTest::addColumn<int>("downloadType");
  QTest::addColumn<int>("downloadNumber");

  QTest::newRow("NATS") << int(atools::track::NATS) << 5;
  QTest::newRow("AUSOTS") << int(atools::track::AUSOTS) << 10;
  QTest::newRow("PACOTS") << int(atools::track::PACOTS) << 10;
}

void TrackTest::testDownload()
{
  QFETCH(int, downloadType);
  QFETCH(int, downloadNumber);

  atools::track::TrackType downloadTrackType = atools::track::TrackType(downloadType);

  TrackDownloader downloader(this, true);

  bool done = false, err = false;

  atools::track::TrackVectorType resultTracks;
  atools::track::TrackType resultType = atools::track::UNKNOWN;
  connect(&downloader, &TrackDownloader::downloadFinished,
          [&resultTracks, &resultType, &err, &done](const atools::track::TrackVectorType& tracks,
                                                    atools::track::TrackType type) -> void
  {
    qInfo() << "downloadFinished tracks size" << tracks.size() << "type" << type;
    resultTracks = tracks;
    resultType = type;
    done = true;
    err = false;
  });

  connect(&downloader, &TrackDownloader::downloadFailed,
          [&err, &done](const QString& error, int errorCode, QString downloadUrl, atools::track::TrackType type) -> void
  {
    qWarning() << "downloadFailed" << error << errorCode << downloadUrl << type;
    done = true;
    err = true;
  });

  downloader.startDownload(downloadTrackType);

  testutil::waitForValue(done, 30);

  QCOMPARE(TrackReader::removeInvalid(resultTracks), 0);
  QCOMPARE(err, false);
  QCOMPARE(done, true);
  QCOMPARE(resultType, downloadTrackType);
  QVERIFY(resultTracks.size() > downloadNumber);
  QVERIFY(downloader.hasTracks(downloadTrackType));
  QVERIFY(downloader.hasAnyTracks());

  verifyTracks(resultTracks, downloadTrackType);
}

void TrackTest::verifyTracks(const atools::track::TrackVectorType& resultTracks, atools::track::TrackType type)
{
  QDateTime tomorrow, old;
  tomorrow = old = QDateTime::currentDateTimeUtc();

  tomorrow.setTime(QTime(0, 0, 0));
  tomorrow = tomorrow.addDays(2);

  old.setTime(QTime(0, 0, 0));
  old = old.addDays(-2);

  for(const atools::track::Track& track : resultTracks)
  {
    qDebug() << track;
    QCOMPARE(track.type, type);
    QVERIFY(track.isFullyValid());
    QVERIFY(track.validFrom > old);
    QVERIFY(track.validFrom < tomorrow);
    QVERIFY(track.validTo > old);
    QVERIFY(track.validTo < tomorrow);
  }
}
