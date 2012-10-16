#ifndef HISTORY_STRUCTS_H
#define HISTORY_STRUCTS_H

#include <QtCore>
#include <andyk/core/enum.h>

struct TimeFrames : public Enum
{
    enum { eY10, eY, eMN3, eMN, eW, eD, eH4, eH1, eM15, eM5, eM1, eS15, eS5, eS1, eT, eCount };

    TimeFrames() : Enum("TimeFrames")
    {
        *this << "Y10" << "Y" << "MN3" << "MN" << "W" << "D" << "H4" << "H1" << "M15" << "M5" << "M1"
              << "S15" << "S5" << "S1" << "T";
    }
};
typedef EnumItem<TimeFrames> TimeFrame;
Q_DECLARE_TYPEINFO(TimeFrame, Q_COMPLEX_TYPE|Q_MOVABLE_TYPE);

// // -----------------------------------------------------------------------------------------------------------
struct HistoryTick
{
    static HistoryTick create(QString time, double price, int volume, int sign = 0);

    QDateTime m_time;
    double m_price;
    int m_volume;
    int m_sign;

    HistoryTick(){}
    HistoryTick(QDateTime time, double price, int volume, int sign = 0);
    bool operator==(HistoryTick& other);
    QString toString() const;
};

inline HistoryTick HistoryTick::create(QString time, double price, int volume, int sign)
{
    return HistoryTick(QDateTime::fromString(time, DateTime::g_fullHumanFormat), price, volume, sign);
}

inline HistoryTick::HistoryTick(QDateTime time, double price, int volume, int sign)
    : m_time(time), m_price(price), m_volume(volume), m_sign(sign)
{
}

inline bool HistoryTick::operator==(HistoryTick& other)
{
    return m_time==other.m_time && m_price == other.m_price && m_volume == other.m_volume &&
        m_sign == other.m_sign;
}

inline QString HistoryTick::toString() const
{
    return QString("HistoryTick { %1 P=%2 V=%3 S=%4 }").arg(m_time.toString(DateTime::g_fullHumanFormat))
        .arg(m_price).arg(m_volume).arg(m_sign);
}

// -----------------------------------------------------------------------------------------------------------
struct HistoryCandle
{
    static HistoryCandle create(QString time, double open, double high, double low, double close, int volume,
        int imbalanceVolume = 0);

    QDateTime m_time;
    double m_open, m_high, m_low, m_close;
    int m_volume;
    int m_imbalanceVolume;

    HistoryCandle(){}
    HistoryCandle(QDateTime time, double open, double high, double low, double close, int volume, int imbalanceVolume = 0);
    bool operator==(HistoryCandle& other);
    QString toString() const;
};

inline HistoryCandle HistoryCandle::create(QString time, double open, double high, double low, double close,
    int volume, int imbalanceVolume)
{
    return HistoryCandle(QDateTime::fromString(time, DateTime::g_fullHumanFormat), open, high, low, close,
        volume, imbalanceVolume);
}

inline HistoryCandle::HistoryCandle(QDateTime time, double open, double high, double low, double close, int volume,
        int imbalanceVolume)
    : m_time(time), m_open(open), m_high(high), m_low(low), m_close(close), m_volume(volume), m_imbalanceVolume(imbalanceVolume)
{
}

inline bool HistoryCandle::operator==(HistoryCandle& other)
{
    return m_time==other.m_time && m_open==other.m_open && m_high==other.m_high && m_low==other.m_low &&
        m_close==other.m_close && m_volume==other.m_volume && m_imbalanceVolume==other.m_imbalanceVolume;
}

inline QString HistoryCandle::toString() const
{
    return QString("HistoryCandle { %1 O=%2 H=%3 L=%4 C=%5 V=%6 IV=%7 }").arg(m_time.toString(DateTime::g_fullHumanFormat))
        .arg(m_open).arg(m_high).arg(m_low).arg(m_close).arg(m_volume).arg(m_imbalanceVolume);
}

#endif // HISTORY_STRUCTS_H
