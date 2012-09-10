#include "files.h"
#include "history_app.h"

// -----------------------------------------------------------------------------------------------------------
// Ticker
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<Ticker> Ticker::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<Ticker>(name);
}

void Ticker::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);
    parser.xmlNodeVarChange(m_section, element, "Section", XmlParser::nodeAttr, optional);
    parser.xmlNodeVarChange(m_code, element, "Code", XmlParser::nodeAttr, optional);
}

// -----------------------------------------------------------------------------------------------------------
// TickersGroup
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<TickersGroup> TickersGroup::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<TickersGroup>(name);
}

void TickersGroup::addString(QDomElement& elem, int)
{
    QSharedPointer<Ticker> item(Ticker::find(elem.text()));
    if (!m_tikers.contains(item))
        m_tikers << item;
}

void TickersGroup::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    m_tikers.clear();
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);
    parser.xmlParseCollection(this, &TickersGroup::addString, 0, element, "Ticker", optional);
}

// -----------------------------------------------------------------------------------------------------------
// FilesGroup
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<FilesGroup> FilesGroup::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<FilesGroup>(name);
}

void FilesGroup::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);

    QString groupName = parser.xmlNodeVar<QString>(element, "TickersGroup",  XmlParser::nodeChild, optional);
    if (!groupName.isEmpty())
        m_tikersGroup = TickersGroup::find(groupName);

    m_timeFrames.clear();
    parser.xmlParseCollection(this, &FilesGroup::addTimeFrame, 0, element, "TimeFrame", optional);
}

// -----------------------------------------------------------------------------------------------------------
// StockFileManager
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<StockFileManager> StockFileManager::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<StockFileManager>(name);
}

void StockFileManager::addFilesGroup(QDomElement& elem, int)
{
    QSharedPointer<FilesGroup> item = FilesGroup::find(elem.text());
    if (!m_filesGroups.contains(item))
        m_filesGroups << item;
}

void StockFileManager::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    m_filesGroups.clear();
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);
    parser.xmlNodeVarChange(m_filePath, element, "FilePath", XmlParser::nodeChild, optional);
    parser.xmlNodeVarChange(m_dateFormat, element, "DateFormat", XmlParser::nodeChild, optional);
    parser.xmlNodeVarChange(m_dateRegEx, element, "DateRegEx", XmlParser::nodeChild, optional);
    parser.xmlNodeVarChange(m_fileName, element, "FileName", XmlParser::nodeChild, optional);
    parser.xmlParseCollection(this, &StockFileManager::addFilesGroup, 0, element, "FilesGroup", optional);
    parser.xmlNodeVarChange(m_size, element, "Size", XmlParser::nodeChild, optional);

    m_extractor = QSharedPointer<SymbolExtractor>(new SymbolExtractor());
    m_extractor->setDateRegEx(m_dateRegEx);
    m_extractor->setPattern(m_fileName);
    FileWatcher::init(m_filePath, m_extractor);
}

bool StockFileManager::processFile(QString fileName, QVector<QString> paramsName)
{
    QString tickerName = paramsName[SymbolExtractor::eTicker];
    QString timeFrameName = paramsName[SymbolExtractor::eTimeFrame];
    QString startDate = paramsName[SymbolExtractor::eStart];
    QString endDate = paramsName[SymbolExtractor::eEnd];
    enum { eTicker, eTimeFrame, eCount };
    Cartesian cartesian(eCount);

    foreach(QSharedPointer<FilesGroup> fileGroup, m_filesGroups)
    {
        QList< QSharedPointer<Ticker> > tickers = fileGroup->tikersGroup()->tikers();
        QList<TimeFrame> timeFrames = fileGroup->timeFrames();
        cartesian.setLimit(eTicker, tickers.count());
        cartesian.setLimit(eTimeFrame, timeFrames.count());

        for (cartesian.first(); cartesian.isValid(); cartesian.next())
        {
            QSharedPointer<Ticker> ticker = tickers[cartesian[eTicker]];
            TimeFrame timeFrame = timeFrames[cartesian[eTimeFrame]];
            if (ticker->m_name != tickerName || timeFrame.name() != timeFrameName)
                continue;

            QDate start = QDate::fromString(startDate, m_dateFormat);
            QDate end = QDate::fromString(endDate, m_dateFormat);
            if (!start.isValid() || !end.isValid() || start >= end)
                continue;

            if (parseFile(fileName, ticker, timeFrame, fileGroup, start, end))
                return true;
        }
    }
    return false;
}

bool StockFileManager::parseFile(QString fileName, QSharedPointer<Ticker> ticker, TimeFrame timeFrame,
    QSharedPointer<FilesGroup> filesGroup, QDate start, QDate end)
{
    return true;
}


// -----------------------------------------------------------------------------------------------------------
