#ifndef TEST_SAVE_LOAD_JSON_H
#define TEST_SAVE_LOAD_JSON_H

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

class TestSaveLoadJson: public QObject {
  Q_OBJECT

private slots:
  void testSaveLoadJson();
};


#endif
