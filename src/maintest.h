#ifndef MAINTEST_H
#define MAINTEST_H

#include "dtmtest.h"
#include <QObject>

#define DECLARTETEST_METHOD(name) \
  void test ## name()

#define DEFINETEST_METHOD(name) \
  void MainTest::test ## name() \
  { \
    name tst; \
    QTest::qExec(&tst, QApplication::arguments()); \
  }

class MainTest :
  public QObject
{
  Q_OBJECT

public:
  MainTest();

private slots:
  void initTestCase();
  void cleanupTestCase();

  DECLARTETEST_METHOD(DtmTest);

};

#endif // MAINTEST_H
