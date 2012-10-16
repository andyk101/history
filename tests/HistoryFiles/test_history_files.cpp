#include "test_history_files.h"

void Test_HistoryFiles::setReadWritePermissions(QString file)
{
    QFile::setPermissions(file, QFile::ReadOwner|QFile::WriteOwner | QFile::ReadUser|QFile::WriteUser |
        QFile::ReadGroup|QFile::WriteGroup | QFile::ReadOther|QFile::WriteOther);
}

QString Test_HistoryFiles::getHomeFilePath(QString fileName)
{
    return QDir::toNativeSeparators(qApp->applicationDirPath() + QDir::separator() + fileName);
}

QString Test_HistoryFiles::getResourceFile(QString resFile, QString newName)
{
    if (newName.isEmpty())
        newName = resFile;
    resFile = QString(":/test_files/%1").arg(resFile);
    newName = getHomeFilePath(newName);
    QFile::remove(newName);
    QFile::copy(resFile, newName);
    setReadWritePermissions(newName);
    return newName;
}

// ----------------------------------------------------------------------------------------------
void Test_HistoryFiles::initTestCase()
{
}

void Test_HistoryFiles::cleanupTestCase()
{
}

void Test_HistoryFiles::test_textFileReader()
{
    QMap<int,HistoryTick> etalonRecordsT;
    etalonRecordsT[174] = HistoryTick::create("08/10/2012 10:00:08", 157.99, 400);
    test_fileReader("GAZP_T_121008_121008.txt", "GAZP", TimeFrames::eT,
        "08/10/2012 09:59:59", "08/10/2012 10:00:12", 233, etalonRecordsT);


    QMap<int,HistoryCandle> etalonRecordsM1;
    etalonRecordsM1[206] = HistoryCandle::create("08/10/2012 13:24:00", 148050, 148090, 148010, 148030, 447);
    test_fileReader("RIZ2_M1_121008_121008.txt", "RIZ2", TimeFrames::eM1,
        "08/10/2012 10:00:00", "08/10/2012 14:49:00", 288, etalonRecordsM1);


    QMap<int,HistoryCandle> etalonRecordsD;
    etalonRecordsD[7] = HistoryCandle::create("08/10/2012 00:00:00", 148840, 149300, 147360, 149010, 909138);
    test_fileReader("RIZ2_D_121001_121012.txt", "RIZ2", TimeFrames::eD,
        "01/10/2012 00:00:00", "12/10/2012 00:00:00", 11, etalonRecordsD);
}
