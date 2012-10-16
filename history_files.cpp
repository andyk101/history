#include "history_files.h"
#include <andyk/core/error_base.h>

TimeFrame HistoryTextFileReader::strToTimeFrame(QString str)
{
    if (str == "0")
        return TimeFrame(TimeFrames::eT);
    if (str == "1")
        return TimeFrame(TimeFrames::eM1);
    if (str == "D")
        return TimeFrame(TimeFrames::eD);

    throw ErrorBase::create("Invalid str");
}

double HistoryTextFileReader::strToDouble(QString str)
{
    bool ok = true;
    double val = QLocale(QLocale::C).toDouble(str, &ok);
    if (!ok)
        throw ErrorBase::create("Invalid double");
    return val;
}

int HistoryTextFileReader::strToInt(QString str)
{
    bool ok = true;
    int val = str.toInt(&ok);
    if (!ok)
        throw ErrorBase::create("Invalid int");
    return val;
}

QString HistoryTextFileReader::readLine()
{
    QByteArray bytes = m_file.readLine();
    QString line = QString::fromAscii(bytes.data(), bytes.size());
    if (line.right(1) == "\n")
        line.resize(line.size()-1);
    return line;
}

void HistoryTextFileReader::open(QString fileName)
{
    m_file.setFileName(fileName);
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw ErrorBase::create("Cannot open");

    m_header = readLine();
    m_ticker = QString();
    m_timeFrame = TimeFrames::eT;
    m_start = QDateTime();
    m_end = QDateTime();
}

void HistoryTextFileReader::read(HistoryTick& tick)
{
    if (m_header != "<TICKER>,<PER>,<DATE>,<TIME>,<LAST>,<VOL>")
        throw ErrorBase::create("Invalid header");

    QStringList vals = readLine().split(",");
    m_ticker = vals[0];
    m_timeFrame = strToTimeFrame(vals[1]);
    if (m_timeFrame != TimeFrames::eT)
        throw ErrorBase::create("Invalid TimeFrame");
    tick.m_time = QDateTime(QDate::fromString(vals[2],"yyyyMMdd"),QTime::fromString(vals[3],"hhmmss"));
    tick.m_price = strToDouble(vals[4]);
    tick.m_sign = 0;
    tick.m_volume = strToInt(vals[5]);
    if (m_start.isNull())
        m_start = tick.m_time;
    m_end = tick.m_time;
}

void HistoryTextFileReader::read(HistoryCandle& candle)
{
    if (m_header != "<TICKER>,<PER>,<DATE>,<TIME>,<OPEN>,<HIGH>,<LOW>,<CLOSE>,<VOL>")
        throw ErrorBase::create("Invalid header");

    QStringList vals = readLine().split(",");
    m_ticker = vals[0];
    m_timeFrame = strToTimeFrame(vals[1]);
    if (m_timeFrame == TimeFrames::eT)
        throw ErrorBase::create("Invalid TimeFrame");
    candle.m_time = QDateTime(QDate::fromString(vals[2],"yyyyMMdd"),QTime::fromString(vals[3],"hhmmss"));
    candle.m_open = strToDouble(vals[4]);
    candle.m_high = strToDouble(vals[5]);
    candle.m_low = strToDouble(vals[6]);
    candle.m_close = strToDouble(vals[7]);
    candle.m_volume = strToInt(vals[8]);
    candle.m_imbalanceVolume = 0;
    if (m_start.isNull())
        m_start = candle.m_time;
    m_end = candle.m_time;
}
