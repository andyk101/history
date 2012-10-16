#ifndef TEST_HISTORY_FILES_H
#define TEST_HISTORY_FILES_H

#include <andyk/core/test.h>
#include "history_files.h"

// ----------------------------------------------------------------------
class Test_HistoryFiles : public QObject
{
    Q_OBJECT
private:
    static void setReadWritePermissions(QString file);
    static QString getHomeFilePath(QString fileName);
    static QString getResourceFile(QString resFile, QString newName = QString());

public:
    Test_HistoryFiles() {}

    template<class HistoryRecord>
    void test_fileReader(QString fileName, QString ticker, int timeFrame,
        QString start, QString end, int linesAmount, QMap<int,HistoryRecord> etalonRecords);

private slots:
    void initTestCase();

    void test_textFileReader();

    void cleanupTestCase();
};

template<class HistoryRecord>
void Test_HistoryFiles::test_fileReader(QString fileName, QString ticker, int timeFrame,
    QString start, QString end, int linesAmount, QMap<int,HistoryRecord> etalonRecords)
{
    fileName = getResourceFile(fileName);
    HistoryTextFileReader reader;
    VERIFY_THROW(reader.open("invalid file"), true);
    VERIFY_THROW(reader.open(fileName), false);

    int line = 1;
    HistoryRecord record;
    while(!reader.atEnd())
    {
        VERIFY_THROW(reader.read(record),false);
        line++;

        if (etalonRecords.contains(line))
        {
            QVERIFY2(record==etalonRecords[line], record.toString().toAscii().data());
        }
    }

    QVERIFY(line==linesAmount);
    QVERIFY(reader.ticker()==ticker);
    QVERIFY(reader.timeFrame()==TimeFrame(timeFrame));
    QVERIFY(reader.start().toString(DateTime::g_fullHumanFormat)==start);
    QVERIFY(reader.end().toString(DateTime::g_fullHumanFormat)==end);
    reader.close();
    QFile::remove(fileName);
}

// ----------------------------------------------------------------------
#endif // TEST_HISTORY_FILES_H
