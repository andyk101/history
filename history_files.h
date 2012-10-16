#ifndef HISTORY_FILES_H
#define HISTORY_FILES_H

#include <QtCore>
#include "history_structs.h"

class HistoryFileReader
{
protected:
    QFile m_file;
    QString m_ticker;
    TimeFrame m_timeFrame;
    QDateTime m_start;
    QDateTime m_end;

public:
    HistoryFileReader() {}

    virtual void open(QString fileName) = 0;
    virtual void read(HistoryTick& tick) = 0;
    virtual void read(HistoryCandle& candle) = 0;
//    virtual bool write(HistoryTick& tick) = 0;
//    virtual bool write(HistoryCandle& candle) = 0;

    QString ticker() const { return m_ticker; }
    TimeFrame timeFrame() const { return m_timeFrame; }
    QDateTime start() const { return m_start; }
    QDateTime end() const { return m_end; }
    bool atEnd() const { return m_file.atEnd(); }
};

// -----------------------------------------------------------------------------------------------------------
class HistoryTextFileReader : public HistoryFileReader
{
protected:
    QString m_header;

    static TimeFrame strToTimeFrame(QString str);
    static double strToDouble(QString str);
    static int strToInt(QString str);
    QString readLine();

public:
    HistoryTextFileReader() {}

    virtual void open(QString fileName);
    virtual void read(HistoryTick& tick);
    virtual void read(HistoryCandle& candle);
};

// -----------------------------------------------------------------------------------------------------------
//class HistoryBinaryFileReader : public HistoryFileReader
//{
//public:
//    HistoryBinaryFileReader();

//    virtual bool open(QString fileName) { return false; }
//    virtual bool read(HistoryTick& tick) { return false; }
//    virtual bool read(HistoryCandle& candle) { return false; }
//};

#endif // HISTORY_FILES_H
