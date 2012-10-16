#ifndef FILES_H
#define FILES_H

#include <QtCore>
#include <andyk/core/core_fwd.h>
#include "history_structs.h"

// -----------------------------------------------------------------------------------------------------------
template<class Item>
void globalAddItem(QDomElement& elem, QList<Item>* lst)
{
    Item item(elem.text());

    if (!lst->contains(item))
        *lst << item;
}

// -----------------------------------------------------------------------------------------------------------
// Ticker
// -----------------------------------------------------------------------------------------------------------
class Ticker
{
public:
    static QSharedPointer<Ticker> find(QString name);

public:
    QString m_name;
    QString m_section;
    QString m_code;

public:
    Ticker(){}
    void parse(XmlParser& parser, QDomElement& element, bool optional = false);
};

// -----------------------------------------------------------------------------------------------------------
// TickersGroup
// -----------------------------------------------------------------------------------------------------------
class TickersGroup
{
public:
    static QSharedPointer<TickersGroup> find(QString name);

private:
    QString m_name;
    QList< QSharedPointer<Ticker> > m_tikers;

public:
    TickersGroup(){}
    void addString(QDomElement& elem, int);
    void parse(XmlParser& parser, QDomElement& element, bool optional = false);

    QString name() const { return m_name; }
    QList< QSharedPointer<Ticker> > tikers() const { return m_tikers; }
};

// -----------------------------------------------------------------------------------------------------------
// FilesGroup
// -----------------------------------------------------------------------------------------------------------
class FilesGroup
{
public:
    static QSharedPointer<FilesGroup> find(QString name);

private:
    QString m_name;
    QSharedPointer<TickersGroup> m_tikersGroup;
    QList<TimeFrame> m_timeFrames;

public:
    FilesGroup(){}
    void addTimeFrame(QDomElement& elem, int) { globalAddItem(elem, &m_timeFrames); }
    void parse(XmlParser& parser, QDomElement& element, bool optional = false);

    QString name() const { return m_name; }
    QSharedPointer<TickersGroup> tikersGroup() const { return m_tikersGroup; }
    QList<TimeFrame> timeFrames() const { return m_timeFrames; }
};

// -----------------------------------------------------------------------------------------------------------
class SymbolExtractor : public ParamsExtractor
{
public:
    enum eParams { eTicker, eTimeFrame, eStart, eEnd, eCount };
    SymbolExtractor()
    {
        m_paramsStr   << "{Ticker}" << "{TimeFrame}" << "{Start}"  << "{End}";
        m_paramsRegEx << "(\\w*)"   << "(\\w{1,3})"  << "(\\d{6})" << "(\\d{6})";
        m_paramPattern = "[{].*[}]";
    }
    void setDateRegEx(QString dateRegEx)
    {
        m_paramsRegEx[2] = m_paramsRegEx[3] = dateRegEx;
    }
};

// -----------------------------------------------------------------------------------------------------------
// StockFileManager
// -----------------------------------------------------------------------------------------------------------
class StockFileManager : public FileWatcher
{
public:
    static QSharedPointer<StockFileManager> find(QString name);

private:
    QString m_name;
    QString m_filePath;
    QString m_dateFormat;
    QString m_dateRegEx;
    QString m_fileName;
    QList< QSharedPointer<FilesGroup> > m_filesGroups;
    int m_size;

    QSharedPointer<SymbolExtractor> m_extractor;

public:
    StockFileManager(){}
    void addFilesGroup(QDomElement& elem, int);
    void parse(XmlParser& parser, QDomElement& element, bool optional = false);

    QString name() const {}
    QList< QSharedPointer<FilesGroup> > filesGroups() const { return m_filesGroups; }

    virtual bool processFile(QString fileName, QVector<QString> paramsName);
    virtual bool parseFile(QString fileName, QSharedPointer<Ticker> ticker, TimeFrame timeFrame,
        QSharedPointer<FilesGroup> filesGroup, QDate start, QDate end);
};

// Файлы появляются из файловой системы, а не из программы. Это дает большую гибкость.
// 0. FileWatcher -> andyk
// 1. Индексация!!!
// 2. Создание. Если HistoryFiles - меняется не только он, но и CacheFiles
// 3. Чтение.
// 4. Оптимизация прохода. Страничное кэширование.

// -----------------------------------------------------------------------------------------------------------
#endif // FILES_H
